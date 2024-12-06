///
///
///     fffb_scs
///     plugin.cpp
///

/// STD

#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <cstring>

/// SDK

#include <scssdk_telemetry.h>
#include <eurotrucks2/scssdk_eut2.h>
#include <eurotrucks2/scssdk_telemetry_eut2.h>
#include <amtrucks/scssdk_ats.h>
#include <amtrucks/scssdk_telemetry_ats.h>

/// UTI

#include <uti/container/tuple.hpp>

/// FLT

#include <flt/device.hpp>
#include <flt/wheel.hpp>


bool g_telemetry_paused { true } ;

scs_timestamp_t g_last_timestamp { static_cast< scs_timestamp_t >( -1 ) } ;


struct telemetry_state_t
{
        scs_timestamp_t                       timestamp ;
        scs_timestamp_t         raw_rendering_timestamp ;
        scs_timestamp_t        raw_simulation_timestamp ;
        scs_timestamp_t raw_paused_simulation_timestamp ;

        bool orientation_available ;

        float heading ;
        float   pitch ;
        float    roll ;

        float steering ;
        float throttle ;
        float    brake ;
        float   clutch ;

        float speed ;
        float   rpm ;
        int    gear ;
} ;

telemetry_state_t g_telemetry_state ;

scs_log_t g_game_log { nullptr } ;

flt::vector< flt::wheel > g_wheels {} ;


bool init_wheels ()
{
        flt::device_manager manager ;

        flt::vector< flt::hid_device > wheels = manager.find_known_wheels() ;

        g_wheels.clear() ;

        for( auto const & device : wheels )
        {
                flt::wheel wheel( device ) ;

                if( !wheel.calibrate() )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : failed initializing device" ) ;
                }
                else
                {
                        g_game_log( SCS_LOG_TYPE_message, "fffb::info : wheel initialized" ) ;
                        g_wheels.emplace_back( device ) ;
                }
        }
        return !g_wheels.empty() ;
}

bool update_leds ( flt::wheel & wheel, float rpm )
{
        static constexpr uti::u8_t led_0 { 0x00 } ;
        static constexpr uti::u8_t led_1 { 0x01 } ;
        static constexpr uti::u8_t led_2 { 0x03 } ;
        static constexpr uti::u8_t led_3 { 0x07 } ;
        static constexpr uti::u8_t led_4 { 0x0F } ;
        static constexpr uti::u8_t led_5 { 0x1F } ;

        if(      rpm ==   0 ) { return wheel.set_led_pattern( led_0 ) ; }
        else if( rpm < 1000 ) { return wheel.set_led_pattern( led_1 ) ; }
        else if( rpm < 1300 ) { return wheel.set_led_pattern( led_2 ) ; }
        else if( rpm < 1600 ) { return wheel.set_led_pattern( led_3 ) ; }
        else if( rpm < 1800 ) { return wheel.set_led_pattern( led_4 ) ; }
        else if( rpm < 1900 ) { return wheel.set_led_pattern( led_5 ) ; }
        else                  { return wheel.set_led_pattern( led_5 ) ; }
}

uti::u8_t map_rpm_to_freq ( float rpm )
{
        return ( 255 - ( rpm / 3000.0f * 255.0f ) ) / 4 ;
}

uti::tuple< uti::u8_t, uti::u8_t > calculate_resonance_params ( float speed, float rpm, float throttle )
{
        uti::u8_t amplitude { 0 } ;
        uti::u8_t frequency { 0 } ;

        if( rpm > 1 )
        {
                frequency = map_rpm_to_freq( rpm ) ;

                if(      speed <  5 ) { amplitude = 3 ;}
                else if( speed < 45 ) { amplitude = 2 ;}
                else if( speed < 75 ) { amplitude = 1 ;}
                else                  { amplitude = 0 ;}

                if(      throttle < 0.10f ) { amplitude -= 1 ; }
                else if( throttle < 0.25f ) {                  }
                else if( throttle < 0.50f ) { amplitude += 1 ; }
                else if( throttle < 0.75f ) { amplitude += 2 ; }
                else                        { amplitude += 3 ; }
        }
        return uti::tuple{ amplitude, frequency } ;
}

uti::u8_t calculate_damper_force ( float speed, float rpm )
{
        if( rpm != 0 ) return 0 ;

        if(      speed <  1 ) { return 3 ; }
        else if( speed <  5 ) { return 2 ; }
        else if( speed < 45 ) { return 1 ; }
        else if( speed < 75 ) { return 1 ; }
        else                  { return 0 ; }
}

uti::tuple< uti::u8_t, uti::u8_t > calculate_autocentering_force ( float speed )
{
        uti::u8_t force ;
        uti::u8_t slope ;

        if( speed < 1 )
        {
                force = 0 ;
                slope = 0 ;
        }
        else
        {
                if(      speed <  5 ) { slope = 2 ; }
                else if( speed < 45 ) { slope = 2 ; }
                else if( speed < 75 ) { slope = 3 ; }
                else                  { slope = 4 ; }

                force = speed * speed / 2000.0f * 255 ;

                if( force < 24 ) force = 24 ;
                if( force > 64 ) force = 64 ;
        }
        return uti::tuple{ slope, force } ;
}

bool update_forces ( flt::vector< flt::wheel > & wheels, telemetry_state_t const & telemetry )
{
        static uti::tuple< uti::u8_t, uti::u8_t > prev_resonance_params { 0, 0 } ;

        auto  resonance_params = calculate_resonance_params   ( telemetry.speed, telemetry.rpm, telemetry.throttle ) ;
        auto      damper_force = calculate_damper_force       ( telemetry.speed, telemetry.rpm ) ;
        auto autocenter_params = calculate_autocentering_force( telemetry.speed ) ;

        bool succ { true } ;

        for( auto & wheel : wheels )
        {
                if( !update_leds( wheel, telemetry.rpm ) )
                {
                        g_game_log( SCS_LOG_TYPE_error, "fffb : failed updating leds" ) ;
                        succ = false ;
                }
                if( uti::get< 0 >( resonance_params ) > 0 )
                {
                        if( uti::get< 0 >( resonance_params ) != uti::get< 0 >( prev_resonance_params ) ||
                            uti::get< 1 >( resonance_params ) != uti::get< 1 >( prev_resonance_params )  )
                        {
                                if( !wheel.set_trapezoid( 128 - uti::get< 0 >( resonance_params ) ,
                                                          128 + uti::get< 0 >( resonance_params ) ,
                                                                uti::get< 1 >( resonance_params ) ,
                                                                uti::get< 1 >( resonance_params ) ,
                                                          6, 6 ) )
                                {
                                        g_game_log( SCS_LOG_TYPE_error, "fffb : failed setting resonance force" ) ;
                                        succ = false ;
                                }
                        }

                }
                else if( damper_force > 0 )
                {
                        uti::get< 0 >( resonance_params ) = 0 ;
                        uti::get< 1 >( resonance_params ) = 0 ;

                        if( !wheel.set_damper( damper_force, damper_force, 0, 0 ) )
                        {
                                g_game_log( SCS_LOG_TYPE_error, "fffb : failed setting damper force" ) ;
                                succ = false ;
                        }
                }
                else
                {
                        uti::get< 0 >( resonance_params ) = 0 ;
                        uti::get< 1 >( resonance_params ) = 0 ;

                        if( !wheel.stop_forces() )
                        {
                                g_game_log( SCS_LOG_TYPE_error, "fffb : failed stopping forces" ) ;
                                succ = false ;
                        }
                }
                if( uti::get< 1 >( autocenter_params ) > 0 )
                {
                        if( !wheel.enable_autocenter() || !wheel.set_autocenter_spring( uti::get< 0 >( autocenter_params ), uti::get< 0 >( autocenter_params ), uti::get< 1 >( autocenter_params ) ) )
                        {
                                g_game_log( SCS_LOG_TYPE_error, "fffb : failed setting autocenter spring force" ) ;
                                succ = false ;
                        }
                }
                else
                {
                        if( !wheel.disable_autocenter() )
                        {
                                g_game_log( SCS_LOG_TYPE_error, "fffb : failed disabling autocenter spring" ) ;
                                succ = false ;
                        }
                }
        }
        uti::get< 0 >( prev_resonance_params ) = uti::get< 0 >( resonance_params ) ;
        uti::get< 1 >( prev_resonance_params ) = uti::get< 1 >( resonance_params ) ;

        return succ ;
}

bool update_wheels ( telemetry_state_t const & telemetry )
{
        static uti::i32_t ffb_rate       { 16 } ;
        static uti::i32_t ffb_rate_count { 16 } ;

        --ffb_rate_count ;

        if( ffb_rate_count == 0 )
        {
                if( !update_forces( g_wheels, telemetry ) ) { g_game_log( SCS_LOG_TYPE_error, "update_forces" ) ; return false ; }

                ffb_rate_count = ffb_rate ;
        }
        return true ;
}

bool reset_wheels ()
{
        bool succ { true } ;

        for( auto & wheel : g_wheels )
        {
                if( !wheel.       stop_forces() ) succ = false ;
                if( !wheel.disable_autocenter() ) succ = false ;
                if( !update_leds( wheel, 0 ) )    succ = false ;
        }
        return succ ;
}

void deinit_wheel ()
{
        return ;
}


SCSAPI_VOID telemetry_frame_start ( [[ maybe_unused ]] scs_event_t const event, void const * const event_info, [[ maybe_unused ]] scs_context_t const context )
{
        scs_telemetry_frame_start_t const * const info = static_cast< scs_telemetry_frame_start_t const * >( event_info ) ;

        if( g_last_timestamp == static_cast< scs_timestamp_t >( -1 ) )
        {
                g_last_timestamp = info->paused_simulation_time ;
        }
        if( info->flags & SCS_TELEMETRY_FRAME_START_FLAG_timer_restart )
        {
                g_last_timestamp = 0 ;
        }
        g_telemetry_state.timestamp += ( info->paused_simulation_time - g_last_timestamp ) ;
        g_last_timestamp = info->paused_simulation_time ;

        g_telemetry_state.        raw_rendering_timestamp = info->           render_time ;
        g_telemetry_state.       raw_simulation_timestamp = info->       simulation_time ;
        g_telemetry_state.raw_paused_simulation_timestamp = info->paused_simulation_time ;
}

SCSAPI_VOID telemetry_frame_end ( [[ maybe_unused ]] scs_event_t const event, [[ maybe_unused ]] void const * const event_info, [[ maybe_unused ]] scs_context_t const context )
{
        if( g_telemetry_paused )
        {
                reset_wheels() ;
                return ;
        }
        if( !update_wheels( g_telemetry_state ) )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : failed updating forces!" ) ;
        }
}

SCSAPI_VOID telemetry_pause ( scs_event_t const event, [[ maybe_unused ]] void const * const event_info, [[ maybe_unused ]] scs_context_t const context )
{
        g_telemetry_paused = ( event == SCS_TELEMETRY_EVENT_paused ) ;

        if( g_telemetry_paused )
        {
                reset_wheels() ;
                g_game_log( SCS_LOG_TYPE_message, "fffb::info : telemetry paused, stopped forces" ) ;
        }
        else {}
}

SCSAPI_VOID telemetry_store_orientation ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context )
{
        assert( context ) ;

        telemetry_state_t * const state = static_cast< telemetry_state_t * >( context ) ;

        if( !value )
        {
                state->orientation_available = false ;
                return ;
        }
        assert( value ) ;
        assert( value->type == SCS_VALUE_TYPE_euler ) ;
        state->orientation_available = true ;
        state->heading = value->value_euler.heading * 360.0f ;
        state->  pitch = value->value_euler.  pitch * 360.0f ;
        state->   roll = value->value_euler.   roll * 360.0f ;
}

SCSAPI_VOID telemetry_store_float ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context )
{
        assert( value ) ;
        assert( value->type == SCS_VALUE_TYPE_float ) ;
        assert( context ) ;
        *static_cast< float * >( context ) = value->value_float.value ;
}

SCSAPI_VOID telemetry_store_s32 ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context )
{
        assert( value ) ;
        assert( value->type == SCS_VALUE_TYPE_s32 ) ;
        assert( context ) ;
        *static_cast< int * >( context ) = value->value_s32.value ;
}

SCSAPI_RESULT scs_telemetry_init ( scs_u32_t const version, scs_telemetry_init_params_t const * const params )
{
        if( version != SCS_TELEMETRY_VERSION_1_01 )
        {
                return SCS_RESULT_unsupported ;
        }
        scs_telemetry_init_params_v101_t const * const version_params = static_cast< scs_telemetry_init_params_v101_t const * >( params ) ;

        g_game_log = version_params->common.log ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : version " FLT_VERSION " starting initialization..." ) ;

        if( strcmp( version_params->common.game_id, SCS_GAME_ID_EUT2 ) == 0 )
        {
                scs_u32_t const MIN_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_1_00 ;
                if( version_params->common.game_version < MIN_VERSION )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "ftl_ffb::warning : game version too old, some stuff might be broken" ) ;
                }

                scs_u32_t const IMPLD_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_CURRENT ;
                if( SCS_GET_MAJOR_VERSION( version_params->common.game_version ) > SCS_GET_MAJOR_VERSION( IMPLD_VERSION ) )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "ftl_ffb::warning : game major version too new, some stuff might be broken" ) ;
                }
        }
        else if( strcmp( version_params->common.game_id, SCS_GAME_ID_ATS ) == 0 )
        {
                scs_u32_t const MIN_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_1_00 ;
                if( version_params->common.game_version < MIN_VERSION )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "ftl_ffb::warning : game version too old, some stuff might be broken" ) ;
                }

                scs_u32_t const IMPLD_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_CURRENT ;
                if( SCS_GET_MAJOR_VERSION( version_params->common.game_version ) > SCS_GET_MAJOR_VERSION( IMPLD_VERSION ) )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "ftl_ffb::warning : game major version too new, some stuff might be broken" ) ;
                }
        }
        else
        {
                g_game_log( SCS_LOG_TYPE_warning, "ftl_ffb::warning : unknown game, some stuff might be broken" ) ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : version checks passed" ) ;
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : registering to events..." ) ;

        bool const events_registered =
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start, nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_frame_end  , telemetry_frame_end  , nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_paused     , telemetry_pause      , nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_started    , telemetry_pause      , nullptr ) == SCS_RESULT_ok )  ;

        if( !events_registered )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : failed to register event callbacks" ) ;
                return SCS_RESULT_generic_error ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : event registration successful" ) ;
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : registering to channels..." ) ;

        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_world_placement, SCS_U32_NIL, SCS_VALUE_TYPE_euler, SCS_TELEMETRY_CHANNEL_FLAG_no_value, telemetry_store_orientation, &g_telemetry_state       ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_speed          , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_float      , &g_telemetry_state.speed ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_engine_rpm     , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_float      , &g_telemetry_state.rpm   ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_engine_gear    , SCS_U32_NIL, SCS_VALUE_TYPE_s32  , SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_s32        , &g_telemetry_state.gear  ) ;

        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_steering, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.steering  ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_throttle, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.throttle  ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_brake   , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.brake     ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_clutch  , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.clutch    ) ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : channel registration completed" ) ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : initializing wheel..." ) ;
        if( !init_wheels() )
        {
                g_game_log( SCS_LOG_TYPE_error, "ftl_ffb::error : failed to initialize wheel" ) ;
                return SCS_RESULT_generic_error ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : wheel initialization successful" ) ;

        memset( &g_telemetry_state, 0, sizeof( g_telemetry_state ) ) ;
        g_last_timestamp = static_cast< scs_timestamp_t >( -1 ) ;

        g_telemetry_paused = true ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : successfully initialized" ) ;
        return SCS_RESULT_ok ;
}

SCSAPI_VOID scs_telemetry_shutdown ()
{
        g_game_log = nullptr ;
        deinit_wheel() ;
}

void __attribute__(( destructor )) unload ()
{
        deinit_wheel() ;
}

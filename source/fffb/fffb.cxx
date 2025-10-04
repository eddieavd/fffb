//
//
//      fffb
//      source/fffb.cxx
//

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

/// FFFB

#include <fffb/util/version.hxx>
#include <fffb/util/types.hxx>
#include <fffb/hid/device.hxx>
#include <fffb/joy/wheel.hxx>
#include <fffb/force/simulator.hxx>


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool g_telemetry_paused { true } ;

fffb::timestamp_t     g_last_timestamp  { static_cast< fffb::timestamp_t >( -1 ) } ;
fffb::telemetry_state g_telemetry_state {} ;
fffb::simulator       g_simulator       {} ;

scs_log_t g_game_log { nullptr } ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool   init_wheel () noexcept ;
bool  reset_wheel () noexcept ;
void deinit_wheel () noexcept ;

bool update_leds ( float rpm ) noexcept ;
bool update_ffb  ( fffb::telemetry_state const & telemetry ) noexcept ;

SCSAPI_VOID telemetry_frame_start ( [[ maybe_unused ]] scs_event_t const event,                    void const * const event_info, [[ maybe_unused ]] scs_context_t const context ) ;
SCSAPI_VOID telemetry_frame_end   ( [[ maybe_unused ]] scs_event_t const event, [[ maybe_unused ]] void const * const event_info, [[ maybe_unused ]] scs_context_t const context ) ;
SCSAPI_VOID telemetry_pause       (                    scs_event_t const event, [[ maybe_unused ]] void const * const event_info, [[ maybe_unused ]] scs_context_t const context ) ;

SCSAPI_VOID telemetry_store_orientation ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context ) ;
SCSAPI_VOID telemetry_store_float       ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context ) ;
SCSAPI_VOID telemetry_store_s32         ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context ) ;
SCSAPI_VOID telemetry_store_u32         ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context ) ;

SCSAPI_RESULT scs_telemetry_init     ( scs_u32_t const version, scs_telemetry_init_params_t const * const params ) ;
SCSAPI_VOID   scs_telemetry_shutdown (                                                                           ) ;

void __attribute__(( destructor )) unload () ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool init_wheel () noexcept
{
        if( !g_simulator.wheel_ref().calibrate() )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : wheel calibration failed!" ) ;
                FFFB_F_ERR_S( "scs::init_wheel", "wheel calibration failed!" ) ;
                return false ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : wheel calibration successful" ) ;
        FFFB_F_INFO_S( "scs::init_wheel", "wheel calibration successful" ) ;
        return true ;
}

bool update_leds ( float rpm ) noexcept
{
        static constexpr uti::u8_t led_0 { 0x00 } ;
        static constexpr uti::u8_t led_1 { 0x01 } ;
        static constexpr uti::u8_t led_2 { 0x03 } ;
        static constexpr uti::u8_t led_3 { 0x07 } ;
        static constexpr uti::u8_t led_4 { 0x0F } ;
        static constexpr uti::u8_t led_5 { 0x1F } ;

        if(      rpm ==   0 ) { return g_simulator.wheel_ref().set_led_pattern( led_0 ) ; }
        else if( rpm < 1000 ) { return g_simulator.wheel_ref().set_led_pattern( led_1 ) ; }
        else if( rpm < 1300 ) { return g_simulator.wheel_ref().set_led_pattern( led_2 ) ; }
        else if( rpm < 1600 ) { return g_simulator.wheel_ref().set_led_pattern( led_3 ) ; }
        else if( rpm < 1800 ) { return g_simulator.wheel_ref().set_led_pattern( led_4 ) ; }
        else if( rpm < 1900 ) { return g_simulator.wheel_ref().set_led_pattern( led_5 ) ; }
        else                  { return g_simulator.wheel_ref().set_led_pattern( led_5 ) ; }
}

bool reset_wheel () noexcept
{
        FFFB_F_INFO_S( "scs::reset_wheel", "resetting wheel" ) ;

        return g_simulator.wheel_ref() ? g_simulator.wheel_ref().q_disable_autocenter()
                                       , g_simulator.wheel_ref().q_stop_forces()
                                       , g_simulator.wheel_ref().q_set_led_pattern( 0 )
                                       , g_simulator.wheel_ref().flush_reports()
                                       : true ;
}

bool update_ffb ( fffb::telemetry_state const & telemetry ) noexcept
{
        if( !g_simulator.wheel_ref() ) return false ;

        static uti::i32_t ffb_rate       { 32 } ;
        static uti::i32_t ffb_rate_count { 32 } ;

        --ffb_rate_count ;

        if( ffb_rate_count == 0 )
        {
                g_simulator.update_forces( telemetry ) ;
                update_leds( telemetry.rpm ) ;

                ffb_rate_count = ffb_rate ;
        }
        return true ;
}

void deinit_wheel () noexcept {}


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
                return ;
        }
        if( !update_ffb( g_telemetry_state ) )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : failed updating force feedback!" ) ;
                FFFB_F_ERR_S( "scs::telemetry_frame_end", "failed updating force feedback!" ) ;
        }
}

SCSAPI_VOID telemetry_pause ( scs_event_t const event, [[ maybe_unused ]] void const * const event_info, [[ maybe_unused ]] scs_context_t const context )
{
        g_telemetry_paused = ( event == SCS_TELEMETRY_EVENT_paused ) ;

        if( g_telemetry_paused )
        {
                reset_wheel() ;
                g_game_log( SCS_LOG_TYPE_message, "fffb::info : telemetry paused, force feedback stopped" ) ;
                FFFB_F_INFO_S( "scs::telemetry_pause", "telemetry paused, force feedback stopped" ) ;
        }
        else
        {
                FFFB_F_INFO_S( "scs::telemetry_pause", "telemetry unpaused, resuming force feedback" ) ;
        }
}

SCSAPI_VOID telemetry_store_orientation ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context )
{
        assert( context ) ;

        fffb::telemetry_state * const state = static_cast< fffb::telemetry_state * >( context ) ;

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

SCSAPI_VOID telemetry_store_u32 ( [[ maybe_unused ]] scs_string_t const name, [[ maybe_unused ]] scs_u32_t const index, scs_value_t const * const value, scs_context_t const context )
{
        assert( value ) ;
        assert( value->type == SCS_VALUE_TYPE_u32 ) ;
        assert( context ) ;
        *static_cast< int * >( context ) = value->value_u32.value ;
}

SCSAPI_RESULT scs_telemetry_init ( scs_u32_t const version, scs_telemetry_init_params_t const * const params )
{
        if( version != SCS_TELEMETRY_VERSION_1_01 )
        {
                return SCS_RESULT_unsupported ;
        }
        scs_telemetry_init_params_v101_t const * const version_params = static_cast< scs_telemetry_init_params_v101_t const * >( params ) ;

        g_game_log = version_params->common.log ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : version " FFFB_VERSION " starting initialization..." ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "version " FFFB_VERSION " starting initialization..." ) ;

        if( strcmp( version_params->common.game_id, SCS_GAME_ID_EUT2 ) == 0 )
        {
                scs_u32_t const MIN_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_1_00 ;
                if( version_params->common.game_version < MIN_VERSION )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : game version too old, some stuff might be broken" ) ;
                        FFFB_F_WARN_S( "scs::scs_telemetry_init", "game version too old, some stuff might be broken" ) ;
                }

                scs_u32_t const IMPLD_VERSION = SCS_TELEMETRY_EUT2_GAME_VERSION_CURRENT ;
                if( SCS_GET_MAJOR_VERSION( version_params->common.game_version ) > SCS_GET_MAJOR_VERSION( IMPLD_VERSION ) )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : game major version too new, some stuff might be broken" ) ;
                        FFFB_F_WARN_S( "scs::scs_telemetry_init", "game major version too new, some stuff might be broken" ) ;
                }
        }
        else if( strcmp( version_params->common.game_id, SCS_GAME_ID_ATS ) == 0 )
        {
                scs_u32_t const MIN_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_1_00 ;
                if( version_params->common.game_version < MIN_VERSION )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : game version too old, some stuff might be broken" ) ;
                        FFFB_F_WARN_S( "scs::scs_telemetry_init", "game version too old, some stuff might be broken" ) ;
                }

                scs_u32_t const IMPLD_VERSION = SCS_TELEMETRY_ATS_GAME_VERSION_CURRENT ;
                if( SCS_GET_MAJOR_VERSION( version_params->common.game_version ) > SCS_GET_MAJOR_VERSION( IMPLD_VERSION ) )
                {
                        g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : game major version too new, some stuff might be broken" ) ;
                        FFFB_F_WARN_S( "scs::scs_telemetry_init", "game major version too new, some stuff might be broken" ) ;
                }
        }
        else
        {
                g_game_log( SCS_LOG_TYPE_warning, "fffb::warning : unknown game, some stuff might be broken" ) ;
                FFFB_F_WARN_S( "scs::scs_telemetry_init", "unknown game, some stuff might be broken" ) ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : version checks passed" ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "version checks passed" ) ;
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : registering to events..." ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "registering to events..." ) ;

        bool const events_registered =
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_frame_start, telemetry_frame_start, nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_frame_end  , telemetry_frame_end  , nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_paused     , telemetry_pause      , nullptr ) == SCS_RESULT_ok ) &&
                ( version_params->register_for_event( SCS_TELEMETRY_EVENT_started    , telemetry_pause      , nullptr ) == SCS_RESULT_ok )  ;

        if( !events_registered )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : failed to register event callbacks!" ) ;
                FFFB_F_ERR_S( "scs::scs_telemetry_init", "failed to register event callbacks!" ) ;
                return SCS_RESULT_generic_error ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : event registration successful" ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "event registration successful" ) ;
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : registering to channels..." ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "registering to channels..." ) ;

        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_world_placement, SCS_U32_NIL, SCS_VALUE_TYPE_euler, SCS_TELEMETRY_CHANNEL_FLAG_no_value, telemetry_store_orientation, &g_telemetry_state       ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_speed          , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_float      , &g_telemetry_state.speed ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_engine_rpm     , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_float      , &g_telemetry_state.rpm   ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_engine_gear    , SCS_U32_NIL, SCS_VALUE_TYPE_s32  , SCS_TELEMETRY_CHANNEL_FLAG_none    , telemetry_store_s32        , &g_telemetry_state.gear  ) ;

        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_steering, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.steering  ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_throttle, SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.throttle  ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_brake   , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.brake     ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_effective_clutch  , SCS_U32_NIL, SCS_VALUE_TYPE_float, SCS_TELEMETRY_CHANNEL_FLAG_none, telemetry_store_float, &g_telemetry_state.clutch    ) ;

        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_wheel_substance, 0, SCS_VALUE_TYPE_u32, SCS_TELEMETRY_CHANNEL_FLAG_no_value, telemetry_store_u32, &g_telemetry_state.substance_l ) ;
        version_params->register_for_channel( SCS_TELEMETRY_TRUCK_CHANNEL_wheel_substance, 1, SCS_VALUE_TYPE_u32, SCS_TELEMETRY_CHANNEL_FLAG_no_value, telemetry_store_u32, &g_telemetry_state.substance_r ) ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : channel registration completed" ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "channel registration completed" ) ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : initializing wheel..." ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "initializing wheel..." ) ;
        if( !init_wheel() )
        {
                g_game_log( SCS_LOG_TYPE_error, "fffb::error : failed to initialize wheel!" ) ;
                FFFB_F_ERR_S( "scs::scs_telemetry_init", "failed to initialize wheel!" ) ;
                return SCS_RESULT_generic_error ;
        }
        g_game_log( SCS_LOG_TYPE_message, "fffb::info : wheel initialization successful" ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "wheel initialization successful" ) ;

        memset( &g_telemetry_state, 0, sizeof( g_telemetry_state ) ) ;
        g_last_timestamp = static_cast< scs_timestamp_t >( -1 ) ;

        g_telemetry_paused = true ;

        g_game_log( SCS_LOG_TYPE_message, "fffb::info : successfully initialized" ) ;
        FFFB_F_INFO_S( "scs::scs_telemetry_init", "successfully initialized" ) ;
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

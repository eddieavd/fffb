//
//
//      fffb
//      force/simulator.hxx
//

#pragma once

#include <fffb/util/types.hxx>
#include <fffb/joy/wheel.hxx>


namespace fffb
{


////////////////////////////////////////////////////////////////////////////////

struct telemetry_state
{
        timestamp_t                       timestamp { static_cast< timestamp_t >( -1 ) } ;
        timestamp_t         raw_rendering_timestamp { static_cast< timestamp_t >( -1 ) } ;
        timestamp_t        raw_simulation_timestamp { static_cast< timestamp_t >( -1 ) } ;
        timestamp_t raw_paused_simulation_timestamp { static_cast< timestamp_t >( -1 ) } ;

        bool orientation_available { false } ;

        float heading { -1.0 } ;
        float   pitch { -1.0 } ;
        float    roll { -1.0 } ;

        float steering { -1.0 } ;
        float throttle { -1.0 } ;
        float    brake { -1.0 } ;
        float   clutch { -1.0 } ;

        float    speed { -1.0 } ;
        float      rpm { -1.0 } ;
        int       gear { -1   } ;

        int substance_l { -1 } ;
        int substance_r { -1 } ;
} ;

////////////////////////////////////////////////////////////////////////////////

class simulator
{
public:
        constexpr  simulator () noexcept ;
        constexpr ~simulator () noexcept = default ;

        constexpr bool initialize_wheel () noexcept ;

        constexpr void update_forces ( telemetry_state const & _new_state_ ) noexcept ;

        constexpr wheel       & wheel_ref ()       noexcept { return wheel_ ; }
        constexpr wheel const & wheel_ref () const noexcept { return wheel_ ; }
private:
        wheel wheel_ ;

        constexpr void _update_autocenter ( telemetry_state const & _new_state_ ) noexcept ;
        constexpr void _update_constant   ( telemetry_state const & _new_state_ ) noexcept ;
        constexpr void _update_spring     ( telemetry_state const & _new_state_ ) noexcept ;
        constexpr void _update_damper     ( telemetry_state const & _new_state_ ) noexcept ;
        constexpr void _update_trapezoid  ( telemetry_state const & _new_state_ ) noexcept ;

        constexpr uti::u8_t _map_rmp_to_freq ( float _rpm_ ) const noexcept
        { return ( 255 - ( _rpm_ / 3000.0f * 255.0f ) ) / 4 ; }

        constexpr uti::u8_t _map_speed_to_freq ( float _speed_ ) const noexcept
        { return ( 255 - ( _speed_ / 160.0f * 255.0f ) ) / 4 ; }
} ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

constexpr simulator::simulator () noexcept
{
        if( !initialize_wheel() ) FFFB_F_ERR_S( "simulator", "failed initializing wheel!" ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool simulator::initialize_wheel () noexcept
{
        if( !wheel_ ) return false ;

        wheel_.disable_autocenter() ;
        wheel_.stop_forces() ;

        return true ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::update_forces ( telemetry_state const & _new_state_ ) noexcept
{
        _update_autocenter( _new_state_ ) ;
        _update_constant  ( _new_state_ ) ;
        _update_spring    ( _new_state_ ) ;
        _update_damper    ( _new_state_ ) ;
        _update_trapezoid ( _new_state_ ) ;

        wheel_.refresh_forces() ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::_update_autocenter ( [[ maybe_unused ]] telemetry_state const & _new_state_ ) noexcept
{}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::_update_constant ( [[ maybe_unused ]] telemetry_state const & _new_state_ ) noexcept
{}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::_update_spring ( telemetry_state const & _new_state_ ) noexcept
{
        double speed = _new_state_.speed < 0.0 ? -_new_state_.speed : _new_state_.speed ;

        wheel_.spring_force() = wheel::default_spring_f ;

        if( speed < 0.10 )
        {
                wheel_.spring_force().enabled = false ;
        }
        else
        {
                wheel_.spring_force().enabled = true ;

                if     ( speed <=  2.0 ) { wheel_.spring_force().amplitude = static_cast< uti::u8_t >(      ( speed * 16 ) ) ; }
                else if( speed <= 70.0 ) { wheel_.spring_force().amplitude = static_cast< uti::u8_t >( 32 + ( speed /  2 ) ) ; }
                else                     { wheel_.spring_force().amplitude = static_cast< uti::u8_t >( 67 + ( speed - 70 ) ) ; }
        }
}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::_update_damper ( telemetry_state const & _new_state_ ) noexcept
{
        double rpm = _new_state_.rpm ;

        wheel_.damper_force() = wheel::default_damper_f ;
        wheel_.damper_force().enabled = true ;

        if( rpm == 0 )
        {
                wheel_.damper_force().slope_left  = 6 ;
                wheel_.damper_force().slope_right = 6 ;
        }
        else
        {
                wheel_.damper_force().slope_left  = 3 ;
                wheel_.damper_force().slope_right = 3 ;
        }
}

////////////////////////////////////////////////////////////////////////////////

constexpr void simulator::_update_trapezoid ( [[ maybe_unused ]] telemetry_state const & _new_state_ ) noexcept
{}

////////////////////////////////////////////////////////////////////////////////


} // namespace fffb

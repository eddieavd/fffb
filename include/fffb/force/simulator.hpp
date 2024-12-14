///
///
///     fffb
///     force/simulator.hpp
///

#pragma once

#include <fffb/wheel/protocol.hpp>


namespace fffb
{


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
} ;


class simulator
{
public:
        constexpr simulator (                               ) noexcept = default ;
        constexpr simulator ( telemetry_state const & state ) noexcept : state_( state ) {}

        constexpr vector< force_data > simulate_forces ( telemetry_state const & telemetry ) noexcept ;
private:
        telemetry_state state_ {} ;

        constexpr vector< force_data > _sim_stopped () const noexcept ;
        constexpr vector< force_data > _sim_moving  () const noexcept ;

        constexpr bool _state_changed ( telemetry_state const & telemetry ) const noexcept ;

        constexpr uti::u8_t _map_rpm_to_freq ( float rpm ) const noexcept
        { return ( 255 - ( rpm / 3000.0f * 255.0f ) ) / 4 ; }

        static constexpr force_data default_spring {
                .type = force_type::SPRING,
                .spring = {
                        .dead_start   = 127 ,
                        .dead_end     = 128 ,
                        .slope_left   =   3 ,
                        .slope_right  =   3 ,
                        .invert_left  =   0 ,
                        .invert_right =   0 ,
                        .amplitude    =  64 ,
                }
        } ;
        static constexpr force_data default_damper {
                .type = force_type::DAMPER,
                .damper = {
                        .slope_left = 4,
                        .slope_right = 4,
                        .invert_left = 0,
                        .invert_right = 0,
                        .padding = {}
                },
        } ;
        static constexpr force_data default_resonance {
                .type = force_type::TRAPEZOID,
                .trapezoid = {
                        .amplitude_max = 127 ,
                        .amplitude_min = 128 ,
                        .t_at_max      =  64 ,
                        .t_at_min      =  64 ,
                        .slope_step_x  =   6 ,
                        .slope_step_y  =   6 ,
                        .padding = {}
                }
        } ;
        static constexpr force_data default_offset_resonance {
                .type = force_type::TRAPEZOID,
                .trapezoid = {
                        .amplitude_max = 127 ,
                        .amplitude_min = 128 ,
                        .t_at_max      =  96 ,
                        .t_at_min      =  96 ,
                        .slope_step_x  =   6 ,
                        .slope_step_y  =   6 ,
                        .padding = {}
                }
        } ;
} ;

constexpr bool simulator::_state_changed ( telemetry_state const & telemetry ) const noexcept
{
        float diff_throttle = ( telemetry.throttle * 100 ) - ( state_.throttle * 100 ) ;
        float diff_speed    = ( telemetry.speed          ) - ( state_.speed          ) ;
        float diff_rpm      = ( telemetry.rpm            ) - ( state_.rpm            ) ;

        return ( 10 < diff_throttle || diff_throttle < -10 )
            || (  1 < diff_speed    || diff_speed    < - 1 )
            || ( 50 < diff_rpm      || diff_rpm      < -50 ) ;
}

constexpr vector< force_data > simulator::_sim_stopped () const noexcept
{
        FFFB_F_DBG_S( "simulator::_sim_stopped", "start" ) ;

        if( state_.rpm == 0 )
        {
                FFFB_F_DBG_S( "simulator::_sim_stopped", "rpm zero" ) ;
                vector< force_data > forces( 1 ) ;

                forces.push_back( default_damper ) ;

                return forces ;
        }
        else
        {
                FFFB_F_DBG_S( "simulator::_sim_stopped", "rpm non-zero" ) ;
                vector< force_data > forces( 2 ) ;

                forces.push_back( default_damper ) ;
                forces.back().damper.slope_left  = 3 ;
                forces.back().damper.slope_right = 3 ;

                forces.push_back( default_resonance ) ;

                auto freq = _map_rpm_to_freq( state_.rpm ) ;

                uti::u8_t amplification { 3 } ;

                amplification += state_.throttle * 100 / 25 ;

                if(      state_.throttle < 0.10f ) {                      }
                else if( state_.throttle < 0.25f ) { amplification += 1 ; }
                else if( state_.throttle < 0.50f ) { amplification += 2 ; }
                else if( state_.throttle < 0.75f ) { amplification += 3 ; }
                else                               { amplification += 4 ; }

                forces.back().trapezoid.t_at_max = freq ;
                forces.back().trapezoid.t_at_min = freq ;

                forces.back().trapezoid.amplitude_max -= amplification * 5 ;
                forces.back().trapezoid.amplitude_min += amplification * 5 ;

                forces.push_back( default_offset_resonance ) ;

                forces.back().trapezoid.t_at_max = freq + 32 ;
                forces.back().trapezoid.t_at_min = freq + 32 ;

                forces.back().trapezoid.amplitude_max -= amplification * 5 ;
                forces.back().trapezoid.amplitude_min += amplification * 5 ;

                return forces ;
        }
}

constexpr vector< force_data > simulator::_sim_moving () const noexcept
{
        FFFB_F_DBG_S( "simulator::_sim_moving", "start" ) ;

        vector< force_data > forces( 3 ) ;

        forces.push_back( default_damper ) ;
        forces.back().damper.slope_left  = 3 ;
        forces.back().damper.slope_right = 3 ;

        forces.push_back( default_spring ) ;

        uti::u8_t amplification { 8 } ;

        amplification *= ( state_.speed / 20 ) ;

        forces.back().spring.amplitude += amplification ;

        if( state_.rpm > 1 )
        {
                FFFB_F_DBG_S( "simulator::_sim_moving", "rpm non-zero" ) ;

                forces.push_back( default_resonance ) ;

                auto freq = _map_rpm_to_freq( state_.rpm ) ;

                forces.back().trapezoid.t_at_max = freq ;
                forces.back().trapezoid.t_at_min = freq ;

                uti::u8_t amplification { 0 } ;

                if(      state_.speed <  5 ) { amplification = 3 ; }
                else if( state_.speed < 40 ) { amplification = 2 ; }
                else if( state_.speed < 70 ) { amplification = 1 ; }
                else                         { amplification = 0 ; }

                if(      state_.throttle < 0.10f ) {                      }
                else if( state_.throttle < 0.25f ) { amplification += 1 ; }
                else if( state_.throttle < 0.50f ) { amplification += 2 ; }
                else if( state_.throttle < 0.75f ) { amplification += 3 ; }
                else                               { amplification += 4 ; }

                forces.back().trapezoid.amplitude_max -= amplification * 5 ;
                forces.back().trapezoid.amplitude_min += amplification * 5 ;
        }
        else
        {
                FFFB_F_DBG_S( "simulator::_sim_moving", "rpm zero" ) ;
        }
        return forces ;
}

constexpr vector< force_data > simulator::simulate_forces ( telemetry_state const & telemetry ) noexcept
{
//      if( !_state_changed( telemetry ) )
//      {
//              FFFB_F_DBG_S( "simulator::simulate_forces", "state unchanged, skipping calculations" ) ;
//              return {} ;
//      }
        FFFB_F_DBG_S( "simulator::simulate_forces", "state changed, running simulation..." ) ;

        state_ = telemetry ;

        if( telemetry.speed < 1.0f )
        {
                return _sim_stopped() ;
        }
        else
        {
                return _sim_moving() ;
        }
}


} // namespace fffb

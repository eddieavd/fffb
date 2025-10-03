//
//
//      fffb
//      joy/wheel.hxx
//

#pragma once

#include <fffb/hid/device.hxx>
#include <fffb/joy/protocol.hxx>

#define FFFB_WHEEL_USAGE_PAGE 0x01
#define FFFB_WHEEL_USAGE      0x04


namespace fffb
{


////////////////////////////////////////////////////////////////////////////////

class wheel
{
public:
        static constexpr  constant_force_params default_const_f  { FFFB_FORCE_SLOT_CONSTANT , false, 128, {} } ;
        static constexpr    spring_force_params default_spring_f { FFFB_FORCE_SLOT_SPRING   , false, 127, 128, 3, 3, 0, 0, 0 } ;
        static constexpr    damper_force_params default_damper_f { FFFB_FORCE_SLOT_DAMPER   , false,   0,   0, 0, 0, {} } ;
        static constexpr trapezoid_force_params default_trap_f   { FFFB_FORCE_SLOT_TRAPEZOID, false, 127, 128, 0, 0, 0, 0, {} } ;

        constexpr wheel () noexcept ;

        constexpr ~wheel () noexcept { if( device_ ){ stop_forces() ; enable_autocenter() ; } }

        [[ nodiscard ]] constexpr operator bool () const noexcept { return static_cast< bool >( device_ ) ; }

        constexpr bool calibrate () noexcept ;

        constexpr bool disable_autocenter () const noexcept ;
        constexpr bool  enable_autocenter () const noexcept ;

        constexpr bool download_forces () const noexcept ;
        constexpr bool  refresh_forces ()       noexcept ;

        constexpr bool play_forces () noexcept ;
        constexpr bool stop_forces () noexcept ;

        constexpr bool set_led_pattern ( uti::u8_t _pattern_ ) const noexcept ;

        constexpr void q_disable_autocenter () noexcept ;
        constexpr void  q_enable_autocenter () noexcept ;

        constexpr void q_download_forces () noexcept ;
        constexpr void  q_refresh_forces () noexcept ;

        constexpr void q_play_forces () noexcept ;
        constexpr void q_stop_forces () noexcept ;

        constexpr void q_set_led_pattern ( uti::u8_t _pattern_ ) noexcept ;

        constexpr bool flush_reports () noexcept ;

        [[ nodiscard ]] constexpr hid_device const & device () const noexcept { return device_ ; }

        [[ nodiscard ]] constexpr spring_force_params       & autocenter_force ()       noexcept { return autocenter_ ; }
        [[ nodiscard ]] constexpr spring_force_params const & autocenter_force () const noexcept { return autocenter_ ; }

        [[ nodiscard ]] constexpr constant_force_params       & constant_force ()       noexcept { return constant_ ; }
        [[ nodiscard ]] constexpr constant_force_params const & constant_force () const noexcept { return constant_ ; }

        [[ nodiscard ]] constexpr spring_force_params       & spring_force ()       noexcept { return spring_ ; }
        [[ nodiscard ]] constexpr spring_force_params const & spring_force () const noexcept { return spring_ ; }

        [[ nodiscard ]] constexpr damper_force_params       & damper_force ()       noexcept { return damper_ ; }
        [[ nodiscard ]] constexpr damper_force_params const & damper_force () const noexcept { return damper_ ; }

        [[ nodiscard ]] constexpr trapezoid_force_params       & trapezoid_force ()       noexcept { return trapezoid_ ; }
        [[ nodiscard ]] constexpr trapezoid_force_params const & trapezoid_force () const noexcept { return trapezoid_ ; }
private:
        hid_device     device_ ;
        ffb_protocol protocol_ ;

        constant_force_params   constant_ { default_const_f  } ;
        spring_force_params       spring_ { default_spring_f } ;
        spring_force_params   autocenter_ { default_spring_f } ;
        damper_force_params       damper_ { default_damper_f } ;
        trapezoid_force_params trapezoid_ { default_trap_f   } ;

        bool playing_ { false } ;

        vector< report > reports_ {} ;

        constexpr bool _write_report (          report   const & report , char const * scope ) const noexcept ;
        constexpr bool _write_reports ( vector< report > const & reports, char const * scope ) const noexcept ;
} ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

constexpr wheel::wheel () noexcept
{
        vector< hid_device > devices = list_hid_devices() ;

        for( auto & device : devices )
        {
                for( uti::ssize_t i = 0; i < known_wheel_device_ids.size(); ++i )
                {
                        if( device.device_id () == known_wheel_device_ids[ i ]
                         && device.usage_page() == FFFB_WHEEL_USAGE_PAGE
                         && device.usage     () == FFFB_WHEEL_USAGE )
                        {
                                FFFB_F_DBG_S( "wheel", "found wheel with device id 0x%.8x", device.device_id() ) ;
                                device_ = device ;
                                protocol_ = get_supported_protocol( device_ ) ;
                        }
                }
        }
        FFFB_F_ERR_S( "wheel", "no known wheels found!" ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::calibrate () noexcept
{
        if( !device_ )
        {
                return false ;
        }
        if( !disable_autocenter() )
        {
                return false ;
        }
        if( !stop_forces() )
        {
                return false ;
        }

        // turn right
        {
                constant_.  enabled = true ;
                constant_.amplitude =   96 ;

                download_forces() ;
                play_forces() ;
                FFFB_DBG_S( "wheel::calibrate", "turning right..." ) ;
                usleep( 750 * 1000 ) ;
        }
        // turn left
        {
                constant_.amplitude = 160 ;

                refresh_forces() ;
                FFFB_DBG_S( "wheel::calibrate", "turning left..." ) ;
                usleep( 1000 * 1000 ) ;
        }
        // shake it
        {
                stop_forces() ;
                constant_.enabled = false ;

                trapezoid_.      enabled = true ;
                trapezoid_.amplitude_max =   96 ;
                trapezoid_.amplitude_min =  160 ;
                trapezoid_.     t_at_max =   32 ;
                trapezoid_.     t_at_min =   32 ;
                trapezoid_. slope_step_x =    6 ;
                trapezoid_. slope_step_y =    6 ;

                download_forces() ;
                play_forces() ;
                FFFB_DBG_S( "wheel::calibrate", "shaking it..." ) ;
                usleep( 1000 * 1000 ) ;
                stop_forces() ;
                trapezoid_.enabled = false ;
        }
        // return to center
        {
                stop_forces() ;
                enable_autocenter() ;
                FFFB_DBG_S( "wheel::calibrate", "recentering..." ) ;
                usleep( 750 * 1000 ) ;
                disable_autocenter() ;
        }
        if( !stop_forces() )
        {
                return false ;
        }
        return true ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::disable_autocenter () const noexcept
{
        return _write_report( protocol::disable_autocenter( protocol_, 0x0F ), "wheel::disable_autocenter" ) ;
}

constexpr bool wheel::enable_autocenter () const noexcept
{
        return _write_report( protocol::enable_autocenter( protocol_, 0x0F ), "wheel::enable_autocenter" ) ;
}

constexpr void wheel::q_disable_autocenter () noexcept
{
        reports_.emplace_back( protocol::disable_autocenter( protocol_, 0x0F ) ) ;
}

constexpr void wheel::q_enable_autocenter () noexcept
{
        reports_.emplace_back( protocol::enable_autocenter( protocol_, 0x0F ) ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::download_forces () const noexcept
{
        force f_auto   { force_type::SPRING   , {} } ;
        force f_const  { force_type::CONSTANT , {} } ;
        force f_spring { force_type::SPRING   , {} } ;
        force f_damper { force_type::DAMPER   , {} } ;
        force f_trap   { force_type::TRAPEZOID, {} } ;

        f_auto  .spring    = autocenter_ ;
        f_const .constant  = constant_   ;
        f_spring.spring    = spring_     ;
        f_damper.damper    = damper_     ;
        f_trap  .trapezoid = trapezoid_  ;

        vector< report > reports( 5 ) ;

        if( f_auto.params.enabled )
        {
                reports.emplace_back( protocol::set_autocenter( protocol_, f_auto ) ) ;
        }
        if( f_const.params.enabled )
        {
                reports.emplace_back( protocol::download_force( protocol_, f_const ) ) ;
        }
        if( f_spring.params.enabled )
        {
                reports.emplace_back( protocol::download_force( protocol_, f_spring ) ) ;
        }
        if( f_damper.params.enabled )
        {
                reports.emplace_back( protocol::download_force( protocol_, f_damper ) ) ;
        }
        if( f_trap.params.enabled )
        {
                reports.emplace_back( protocol::download_force( protocol_, f_trap ) ) ;
        }
        return _write_reports( reports, "wheel::download_forces" ) ;
}

constexpr void wheel::q_download_forces () noexcept
{
        force f_auto   { force_type::SPRING   , {} } ;
        force f_const  { force_type::CONSTANT , {} } ;
        force f_spring { force_type::SPRING   , {} } ;
        force f_damper { force_type::DAMPER   , {} } ;
        force f_trap   { force_type::TRAPEZOID, {} } ;

        f_auto  .spring    = autocenter_ ;
        f_const .constant  = constant_   ;
        f_spring.spring    = spring_     ;
        f_damper.damper    = damper_     ;
        f_trap  .trapezoid = trapezoid_  ;

        vector< report > reports( 5 ) ;

        if( f_auto.params.enabled )
        {
                reports_.emplace_back( protocol::set_autocenter( protocol_, f_auto ) ) ;
        }
        if( f_const.params.enabled )
        {
                reports_.emplace_back( protocol::download_force( protocol_, f_const ) ) ;
        }
        if( f_spring.params.enabled )
        {
                reports_.emplace_back( protocol::download_force( protocol_, f_spring ) ) ;
        }
        if( f_damper.params.enabled )
        {
                reports_.emplace_back( protocol::download_force( protocol_, f_damper ) ) ;
        }
        if( f_trap.params.enabled )
        {
                reports_.emplace_back( protocol::download_force( protocol_, f_trap ) ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::play_forces () noexcept
{
        playing_ = true ;

        uti::u8_t slots { 0 } ;

        if( constant_ .enabled ) slots |= constant_ .slot ;
        if( spring_   .enabled ) slots |= spring_   .slot ;
        if( damper_   .enabled ) slots |= damper_   .slot ;
        if( trapezoid_.enabled ) slots |= trapezoid_.slot ;

        return _write_report( protocol::play_force( protocol_, slots ), "wheel::play_forces" ) ;
}

constexpr void wheel::q_play_forces () noexcept
{
        playing_ = true ;

        uti::u8_t slots { 0 } ;

        if( constant_ .enabled ) slots |= constant_ .slot ;
        if( spring_   .enabled ) slots |= spring_   .slot ;
        if( damper_   .enabled ) slots |= damper_   .slot ;
        if( trapezoid_.enabled ) slots |= trapezoid_.slot ;

        reports_.emplace_back( protocol::play_force( protocol_, slots ) ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::stop_forces () noexcept
{
        playing_ = false ;

        return _write_report( protocol::stop_force( protocol_, 0x0F ), "wheel::stop_forces" ) ;
}

constexpr void wheel::q_stop_forces () noexcept
{
        playing_ = false ;

        reports_.emplace_back( protocol::stop_force( protocol_, 0x0F ) ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::refresh_forces () noexcept
{
        if( !playing_ ) return play_forces() ;

        force f_const  { force_type::CONSTANT , {} } ;
        force f_spring { force_type::SPRING   , {} } ;
        force f_damper { force_type::DAMPER   , {} } ;
        force f_trap   { force_type::TRAPEZOID, {} } ;

        f_const.constant =  constant_ ;
        f_spring. spring =    spring_ ;
        f_damper. damper =    damper_ ;
        f_trap.trapezoid = trapezoid_ ;

        vector< report > reports( 4 ) ;

        if( f_const.params.enabled )
        {
                reports.emplace_back( protocol::refresh_force( protocol_, f_const ) ) ;
        }
        if( f_spring.params.enabled )
        {
                reports.emplace_back( protocol::refresh_force( protocol_, f_spring ) ) ;
        }
        if( f_damper.params.enabled )
        {
                reports.emplace_back( protocol::refresh_force( protocol_, f_damper ) ) ;
        }
        if( f_trap.params.enabled )
        {
                reports.emplace_back( protocol::refresh_force( protocol_, f_trap ) ) ;
        }
        return _write_reports( reports, "wheel::refresh_forces" ) ;
}

constexpr void wheel::q_refresh_forces () noexcept
{
        if( !playing_ ) q_play_forces() ;

        force f_const  { force_type::CONSTANT , {} } ;
        force f_spring { force_type::SPRING   , {} } ;
        force f_damper { force_type::DAMPER   , {} } ;
        force f_trap   { force_type::TRAPEZOID, {} } ;

        f_const.constant =  constant_ ;
        f_spring. spring =    spring_ ;
        f_damper. damper =    damper_ ;
        f_trap.trapezoid = trapezoid_ ;

        vector< report > reports( 4 ) ;

        if( f_const.params.enabled )
        {
                reports_.emplace_back( protocol::refresh_force( protocol_, f_const ) ) ;
        }
        if( f_spring.params.enabled )
        {
                reports_.emplace_back( protocol::refresh_force( protocol_, f_spring ) ) ;
        }
        if( f_damper.params.enabled )
        {
                reports_.emplace_back( protocol::refresh_force( protocol_, f_damper ) ) ;
        }
        if( f_trap.params.enabled )
        {
                reports_.emplace_back( protocol::refresh_force( protocol_, f_trap ) ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::set_led_pattern ( uti::u8_t pattern ) const noexcept
{
        return _write_report( protocol::set_led_pattern( protocol_, pattern ), "wheel::set_led_pattern" ) ;
}

constexpr void wheel::q_set_led_pattern ( uti::u8_t pattern ) noexcept
{
        reports_.emplace_back( protocol::set_led_pattern( protocol_, pattern ) ) ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::flush_reports () noexcept
{
        auto res = _write_reports( reports_, "wheel::flush" ) ;
        reports_.clear() ;

        return res ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::_write_report ( report const & report, [[ maybe_unused ]] char const * scope ) const noexcept
{
        if( !device_.open() )
        {
                FFFB_F_ERR_S( scope, "failed opening device %x", device_.device_id() ) ;
                return false ;
        }
        if( !device_.write( report ) )
        {
                FFFB_F_ERR_S( scope, "failed sending report to device %x", device_.device_id() ) ;
                return false ;
        }
        if( !device_.close() )
        {
                FFFB_F_ERR_S( scope, "failed closing device %x", device_.device_id() ) ;
                return false ;
        }
        return true ;
}

////////////////////////////////////////////////////////////////////////////////

constexpr bool wheel::_write_reports ( vector< report > const & reports, [[ maybe_unused ]] char const * scope ) const noexcept
{
        if( !device_.open() )
        {
                FFFB_F_ERR_S( scope, "failed opening device %x", device_.device_id() ) ;
                return false ;
        }
        for( auto const & rep : reports )
        {
                if( !device_.write( rep ) )
                {
                        FFFB_F_ERR_S( scope, "failed sending report to device %x", device_.device_id() ) ;
                        return false ;
                }
        }
        if( !device_.close() )
        {
                FFFB_F_ERR_S( scope, "failed closing device %x", device_.device_id() ) ;
                return false ;
        }
        return true ;
}

////////////////////////////////////////////////////////////////////////////////


} // namespace fffb

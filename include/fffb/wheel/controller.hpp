///
///
///     fffb
///     wheel/controller.hpp
///

#pragma once

#include <fffb/util/types.hpp>
#include <fffb/util/log.hpp>
#include <fffb/device/report.hpp>
#include <fffb/device/manager.hpp>
#include <fffb/wheel/protocol.hpp>

#include <uti/container/array.hpp>


namespace fffb
{


class wheel
{
        using  force_slots = uti::array< force_data, 4 > ;
        using spring_slots = uti::array< force_data, 2 > ;
public:
        constexpr wheel () noexcept : hid_device_(), protocol_( ffb_protocol::count ) {}

        constexpr wheel ( hid_device const & device ) noexcept
                : hid_device_( device )
                , protocol_  ( get_supported_protocol( device ) )
        {}
        constexpr wheel ( hid_device const & device, ffb_protocol const protocol ) noexcept
                : hid_device_(   device )
                , protocol_  ( protocol )
        {}

        constexpr operator bool () const noexcept { return static_cast< bool >( hid_device_ ) ; }

        UTI_NODISCARD constexpr hid_device       & device ()       noexcept { return hid_device_ ; }
        UTI_NODISCARD constexpr hid_device const & device () const noexcept { return hid_device_ ; }

        constexpr ffb_protocol protocol () const noexcept { return protocol_ ; }

        constexpr force_slots       & forces ()       noexcept { return forces_ ; }
        constexpr force_slots const & forces () const noexcept { return forces_ ; }

        constexpr spring_slots       & springs ()       noexcept { return springs_ ; }
        constexpr spring_slots const & springs () const noexcept { return springs_ ; }
private:
        hid_device hid_device_ ;
        ffb_protocol protocol_ ;

         force_slots  forces_ ;
        spring_slots springs_ ;
} ;


class controller
{
        static constexpr uti::u8_t default_autocenter_slot { uti::to_underlying( force_slot::SLOT_1 ) } ;

        static constexpr uti::u32_t wheel_usage_page { 0x01 } ;
        static constexpr uti::u32_t wheel_usage      { 0x04 } ;
public:
        constexpr controller (                 ) noexcept = default ;
        constexpr controller ( wheel const & w ) noexcept : wheel_( w ) {}

        constexpr controller ( vector< hid_device > const & devices ) noexcept : wheel_( _find_known_wheel( devices ) ) {}

        constexpr ~controller () noexcept = default ;

        constexpr operator bool () const noexcept { return static_cast< bool >( wheel_ ) ; }

        constexpr bool calibrate () noexcept ;

        constexpr bool disable_autocenter (                          ) const noexcept ;
        constexpr bool  enable_autocenter (                          ) const noexcept ;
        constexpr bool     set_autocenter ( force_data const & force ) const noexcept ;

        constexpr bool   add_force  ( force_data const & force ) noexcept ;
        constexpr bool clear_force  ( force_type const    type ) noexcept ;
        constexpr bool clear_forces (                          ) noexcept ;

        constexpr bool download_forces () const noexcept ;
        constexpr bool     play_forces () const noexcept ;
        constexpr bool     stop_forces () const noexcept ;

        constexpr bool set_led_pattern ( uti::u8_t pattern ) const noexcept ;

        constexpr hid_device       & device ()       noexcept { return wheel_.device() ; }
        constexpr hid_device const & device () const noexcept { return wheel_.device() ; }
private:
        wheel wheel_ ;

        constexpr bool _write_report ( report const & report, char const * scope ) const noexcept ;

        constexpr wheel _find_known_wheel ( vector< hid_device > const & devices ) const noexcept ;
} ;


constexpr wheel controller::_find_known_wheel ( vector< hid_device > const & devices ) const noexcept
{
        FFFB_DBG_S( "controller", "looking for wheels..." ) ;

        for( auto & device : devices )
        {
                for( uti::ssize_t i = 0; i < known_wheel_device_ids.size(); ++i )
                {
                        if( device.device_id () == known_wheel_device_ids[ i ]
                         && device.usage_page() == wheel_usage_page
                         && device.usage     () == wheel_usage )
                        {
                                return wheel( device ) ;
                        }
                }
        }
        FFFB_ERR_S( "controller", "found no known wheels in device list" ) ;
        return wheel() ;
}

constexpr bool controller::calibrate () noexcept
{
        disable_autocenter() ;
        stop_forces() ;

        force_data constant = {
                .type = force_type::CONSTANT ,
                .params = { 64 }
        } ;
        force_data damper = {
                .type = force_type::DAMPER ,
                .params = { 2, 2, 0, 0 }
        } ;
        force_data trap = {
                .type = force_type::TRAPEZOID ,
                .params = { 108, 148, 32, 32, 6, 6 }
        } ;

        clear_forces() ;
        add_force( constant ) ;
        add_force( damper ) ;
        download_forces() ;
        play_forces() ;
        usleep( 750 * 1000 ) ;
        stop_forces() ;

        enable_autocenter() ;
        usleep( 750 * 1000 ) ;
        disable_autocenter() ;

        clear_forces() ;
        add_force( trap ) ;
        download_forces() ;
        play_forces() ;
        usleep( 750 * 1000 ) ;
        stop_forces() ;

        constant.params[ 0 ] = 160 ;

        clear_forces() ;
        add_force( constant ) ;
        add_force( damper ) ;
        download_forces() ;
        play_forces() ;
        usleep( 1500 * 1000 ) ;
        stop_forces() ;
        clear_forces() ;

        set_autocenter( { .type = force_type::AUTO, .params = { 2, 64 } } ) ;
        if( !enable_autocenter() ) return false ;

        return true ;
}

constexpr bool controller::disable_autocenter () const noexcept
{
        return _write_report( protocol_provider::disable_autocenter( wheel_.protocol(), 0x0F ), "controller::disable_autocenter" ) ;
}

constexpr bool controller::enable_autocenter () const noexcept
{
        return _write_report( protocol_provider::enable_autocenter( wheel_.protocol(), default_autocenter_slot ), "controller::enable_autocenter" ) ;
}

constexpr bool controller::set_autocenter ( force_data const & force ) const noexcept
{
        return _write_report( protocol_provider::set_autocenter( wheel_.protocol(), default_autocenter_slot, force ), "controller::set_autocenter" ) ;
}

constexpr bool controller::add_force ( force_data const & force ) noexcept
{
        for( auto & f : wheel_.forces() )
        {
                if( f.type == force_type::NONE )
                {
                        f = force ;
                        return true ;
                }
        }
        return false ;
}

constexpr bool controller::clear_force ( force_type const type ) noexcept
{
        for( auto & force_data : wheel_.forces() )
        {
                if( force_data.type == type )
                {
                        force_data = {} ;
                        return true ;
                }
        }
        return false ;
}

constexpr bool controller::clear_forces () noexcept
{
        for( auto & force_data : wheel_.forces() )
        {
                force_data = {} ;
        }
        return true ;
}

constexpr bool controller::download_forces () const noexcept
{
        bool status { true } ;

        for( uti::ssize_t i = 0; i < wheel_.forces().size(); ++i )
        {
                if( wheel_.forces().at( i ).type != force_type::NONE )
                {
                        status &= _write_report( protocol_provider::download_force( wheel_.protocol(), ( 1 << i ), wheel_.forces().at( i ) ), "controller::download_forces" ) ;
                }
        }
        return status ;
}

constexpr bool controller::play_forces () const noexcept
{
        uti::u8_t slots { 0 } ;

        for( uti::ssize_t i = 0; i < wheel_.forces().size(); ++i )
        {
                if( wheel_.forces().at( i ).type != force_type::NONE )
                {
                        slots |= ( 1 << i ) ;
                }
        }
        return _write_report( protocol_provider::play_force( wheel_.protocol(), slots ), "controller::play_forces" ) ;
}

constexpr bool controller::stop_forces () const noexcept
{
        return _write_report( protocol_provider::stop_force( wheel_.protocol(), 0x0F ), "controller::stop_forces" ) ;
}

constexpr bool controller::_write_report ( report const & report, [[ maybe_unused ]] char const * scope ) const noexcept
{
        if( !wheel_.device().open() )
        {
                FFFB_ERR_S( scope, "open_device failed on device %x", wheel_.device().device_id() ) ;
                return false ;
        }
        if( !wheel_.device().write( report ) )
        {
                FFFB_ERR_S( scope, "send_report failed on device %x", wheel_.device().device_id() ) ;
                wheel_.device().close() ;
                return false ;
        }
        if( !wheel_.device().close() )
        {
                FFFB_ERR_S( scope, "close_device failed on device %x", wheel_.device().device_id() ) ;
                return false ;
        }
        return true ;
}


} // namespace fffb

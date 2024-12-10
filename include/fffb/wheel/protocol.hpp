///
///
///     fffb
///     wheel/protocol.hpp
///

#pragma once

#include <fffb/util/types.hpp>
#include <fffb/util/log.hpp>
#include <fffb/device/report.hpp>
#include <fffb/device/manager.hpp>

#define FFFB_FORCE_MAX_PARAMS 7

#define FFFB_LOGITECH_VENDOR_ID 0x046d

#define FFFB_LOGITECH_G923_PS_DID 0xc266046d


namespace fffb
{


enum class command_type
{
        AUTO_ON    ,
        AUTO_OFF   ,
        AUTO_SET   ,
        DL_FORCE   ,
        PLAY_FORCE ,
        STOP_FORCE ,
} ;

enum class force_type
{
        NONE      = 0 ,
        CONSTANT  = 1 ,
        AUTO      = 2 ,
        SPRING    = 7 ,
        DAMPER    = 4 ,
        TRAPEZOID = 6 ,
} ;

enum class force_slot : uti::u8_t
{
        SLOT_1 = 0b0001 ,
        SLOT_2 = 0b0010 ,
        SLOT_3 = 0b0100 ,
        SLOT_4 = 0b1000 ,
        COUNT  =      4 ,
} ;

struct force_data
{
        force_type type ;
        uti::u8_t params [ FFFB_FORCE_MAX_PARAMS ] ;
} ;

enum class ffb_protocol
{
        logitech_classic ,
        logitech_hidpp   ,
        count            ,
} ;

constexpr uti::array<   uti::u32_t, 1 > known_wheel_device_ids {      FFFB_LOGITECH_G923_PS_DID } ;
constexpr uti::array< ffb_protocol, 1 > known_wheel_protocols  { ffb_protocol::logitech_classic } ;


struct protocol_provider
{
        static constexpr report build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots                           ) noexcept ;
        static constexpr report build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots, force_data const & force ) noexcept ;

        static constexpr report disable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
        static constexpr report  enable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;

        static constexpr report set_autocenter ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;
        static constexpr report download_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;

        static constexpr report play_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
        static constexpr report stop_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
private:
        static constexpr report  _constant_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;
        static constexpr report    _spring_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;
        static constexpr report    _damper_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;
        static constexpr report _trapezoid_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept ;
} ;


constexpr ffb_protocol get_supported_protocol ( hid_device const & device ) noexcept ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

constexpr report protocol_provider::build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots ) noexcept
{
        switch( cmd_type )
        {
                case command_type::AUTO_OFF   : return disable_autocenter( protocol, slots ) ;
                case command_type::AUTO_ON    : return  enable_autocenter( protocol, slots ) ;
                case command_type::PLAY_FORCE : return         play_force( protocol, slots ) ;
                case command_type::STOP_FORCE : return         stop_force( protocol, slots ) ;
                case command_type::AUTO_SET   : [[ fallthrough ]] ;
                case command_type::DL_FORCE   :
                        FFFB_ERR_S( "protocol_provider::build_report", "selected command requires parameters" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::build_report", "unknown command" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots, force_data const & force ) noexcept
{
        switch( cmd_type )
        {
                case command_type::AUTO_SET   : return     set_autocenter( protocol, slots, force ) ;
                case command_type::DL_FORCE   : return     download_force( protocol, slots, force ) ;
                case command_type::AUTO_ON    : [[ fallthrough ]] ;
                case command_type::AUTO_OFF   : [[ fallthrough ]] ;
                case command_type::PLAY_FORCE : [[ fallthrough ]] ;
                case command_type::STOP_FORCE :
                        FFFB_ERR_S( "protocol_provider::build_report", "excess parameters for selected command" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::build_report", "unknown command" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::disable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x05 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_ERR_S( "protocol_provider::disable_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::disable_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::enable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x04 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_ERR_S( "protocol_provider::enable_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::enable_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::set_autocenter ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x0e ;

        uti::u8_t     slope = force.params[ 0 ] | 0b0111 ;
        uti::u8_t amplitude = force.params[ 1 ]          ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x00, slope, slope, amplitude, 0x00 } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_ERR_S( "protocol_provider::set_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::set_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::download_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        switch( force.type )
        {
                case force_type::     NONE : return {} ;
                case force_type:: CONSTANT : return  _constant_force( protocol, slots, force ) ;
                case force_type::   SPRING : return    _spring_force( protocol, slots, force ) ;
                case force_type::   DAMPER : return    _damper_force( protocol, slots, force ) ;
                case force_type::TRAPEZOID : return _trapezoid_force( protocol, slots, force ) ;
                default:
                        FFFB_ERR_S( "protocol_provider::download_force", "force type not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::play_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x02 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic: return { command, 0x00 } ;
                        break ;
                case ffb_protocol::logitech_hidpp:
                        FFFB_ERR_S( "protocol_provider::play_force", "protocol not implemented" ) ;
                        return {} ;
                default:
                        FFFB_ERR_S( "protocol_provider::play_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::stop_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x03 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic: return { command, 0x00 } ;
                        break ;
                case ffb_protocol::logitech_hidpp:
                        FFFB_ERR_S( "protocol_provider::play_force", "protocol not implemented" ) ;
                        return {} ;
                default:
                        FFFB_ERR_S( "protocol_provider::play_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::_constant_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        uti::u8_t command = slots << 4 ;
        uti::u8_t amplitude = force.params[ 0 ] ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x00, amplitude, amplitude, amplitude, amplitude, 0x00 } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_ERR_S( "protocol_provider::_constant_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::_constant_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol_provider::_spring_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        uti::u8_t command = slots << 4 ;

        uti::u8_t dead_start   = force.params[ 0 ] ;
        uti::u8_t dead_end     = force.params[ 1 ] ;
        uti::u8_t slope_left   = force.params[ 2 ] & 0b0111 ;
        uti::u8_t slope_right  = force.params[ 3 ] & 0b0111 ;
        uti::u8_t invert_left  = force.params[ 4 ] & 0b0001 ;
        uti::u8_t invert_right = force.params[ 5 ] & 0b0001 ;
        uti::u8_t amplitude    = force.params[ 6 ] ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x01,
                                                               dead_start, dead_end,
                                                               uti::u8_t( (  slope_right << 4 ) |  slope_left ),
                                                               uti::u8_t( ( invert_right << 4 ) | invert_left ),
                                                               amplitude } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_ERR_S( "protocol_provider::_spring_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::_spring_force", "protocol not supported" ) ;
                        return {} ;
        }

}

constexpr report protocol_provider::_damper_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        uti::u8_t command = slots << 4 ;

        uti::u8_t slope_left   = force.params[ 0 ] & 0b0111 ;
        uti::u8_t slope_right  = force.params[ 1 ] & 0b0111 ;
        uti::u8_t invert_left  = force.params[ 2 ] & 0b0001 ;
        uti::u8_t invert_right = force.params[ 3 ] & 0b0001 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x02, slope_left, invert_left, slope_right, invert_right, 0x00 } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_ERR_S( "protocol_provider::_damper_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::_damper_force", "protocol not supported" ) ;
                        return {} ;
        }

}

constexpr report protocol_provider::_trapezoid_force ( ffb_protocol const protocol, uti::u8_t slots, force_data const & force ) noexcept
{
        uti::u8_t command = slots << 4 ;

        uti::u8_t max_amp =   force.params[ 0 ] ;
        uti::u8_t min_amp =   force.params[ 1 ] ;
        uti::u8_t   t_max =   force.params[ 2 ] ;
        uti::u8_t   t_min =   force.params[ 3 ] ;
        uti::u8_t    dxdy = ( force.params[ 4 ] << 4 )
                            | force.params[ 5 ] ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x06, max_amp, min_amp, t_max, t_min, dxdy } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_ERR_S( "protocol_provider::_trapezoid_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_ERR_S( "protocol_provider::_trapezoid_force", "protocol not supported" ) ;
                        return {} ;
        }
}



constexpr ffb_protocol get_supported_protocol ( hid_device const & device ) noexcept
{
        switch( device.vendor_id() )
        {
                case FFFB_LOGITECH_VENDOR_ID:
                        return ffb_protocol::logitech_classic ;
                default:
                        return ffb_protocol::count ;
        }
}


} // namespace fffb

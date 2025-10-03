//
//
//      fffb
//      joy/protocol.hxx
//

#pragma once


#include <fffb/util/types.hxx>
#include <fffb/hid/report.hxx>
#include <fffb/hid/device.hxx>

#define FFFB_FORCE_MAX_PARAMS 7

#define FFFB_FORCE_SLOT_CONSTANT   0b0001
#define FFFB_FORCE_SLOT_SPRING     0b0011
#define FFFB_FORCE_SLOT_DAMPER     0b0100
#define FFFB_FORCE_SLOT_TRAPEZOID  0b1000
#define FFFB_FORCE_SLOT_AUTOCENTER 0b1111


namespace fffb
{


////////////////////////////////////////////////////////////////////////////////

constexpr uti::u32_t Logitech_VendorID         { 0x0000046d } ;
constexpr uti::u32_t Logitech_G923_PS_DeviceID { 0xc266046d } ;

constexpr uti::array< uti::u32_t, 1 > known_wheel_device_ids
{
        Logitech_G923_PS_DeviceID,
} ;

////////////////////////////////////////////////////////////////////////////////

enum class force_type
{
        CONSTANT  ,
        SPRING    ,
        DAMPER    ,
        TRAPEZOID ,
        COUNT     ,
} ;

struct force_params
{
        uti::u8_t   slot ;
        bool     enabled ;
        uti::u8_t params [ FFFB_FORCE_MAX_PARAMS ] ;
} ;

struct constant_force_params
{
        uti::u8_t      slot { FFFB_FORCE_SLOT_CONSTANT } ;
        bool        enabled { false } ;
        uti::u8_t amplitude ;
        uti::u8_t   padding [ FFFB_FORCE_MAX_PARAMS - 1 ] ;
} ;

struct spring_force_params
{
        uti::u8_t         slot { FFFB_FORCE_SLOT_SPRING } ;
        bool           enabled { false } ;
        uti::u8_t   dead_start ;
        uti::u8_t   dead_end   ;
        uti::u8_t  slope_left  ;
        uti::u8_t  slope_right ;
        uti::u8_t invert_left  ;
        uti::u8_t invert_right ;
        uti::u8_t    amplitude ;
} ;

struct damper_force_params
{
        uti::u8_t         slot { FFFB_FORCE_SLOT_DAMPER } ;
        bool           enabled { false } ;
        uti::u8_t   slope_left ;
        uti::u8_t  slope_right ;
        uti::u8_t  invert_left ;
        uti::u8_t invert_right ;
        uti::u8_t      padding [ FFFB_FORCE_MAX_PARAMS - 4 ] ;
} ;

struct trapezoid_force_params
{

        uti::u8_t          slot { FFFB_FORCE_SLOT_TRAPEZOID } ;
        bool            enabled ;
        uti::u8_t amplitude_max ;
        uti::u8_t amplitude_min ;
        uti::u8_t      t_at_max ;
        uti::u8_t      t_at_min ;
        uti::u8_t  slope_step_x ;
        uti::u8_t  slope_step_y ;
        uti::u8_t       padding [ FFFB_FORCE_MAX_PARAMS - 6 ] ;
} ;

struct force
{
        force_type type ;
        union
        {
                          force_params    params ;
                 constant_force_params  constant ;
                   spring_force_params    spring ;
                   damper_force_params    damper ;
                trapezoid_force_params trapezoid ;
        } ;
} ;

////////////////////////////////////////////////////////////////////////////////

enum class command_type
{
        AUTO_ON       ,
        AUTO_OFF      ,
        AUTO_SET      ,
        LED_SET       ,
        DL_FORCE      ,
        PLAY_FORCE    ,
        REFRESH_FORCE ,
        STOP_FORCE    ,
        COUNT         ,
} ;

enum class ffb_protocol
{
        logitech_classic ,
        logitech_hidpp   ,
        count
} ;

////////////////////////////////////////////////////////////////////////////////

class protocol
{
public:
        static constexpr report build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots ) noexcept ;
        static constexpr report build_report ( ffb_protocol const protocol, command_type const cmd_type, force const & f ) noexcept ;

        static constexpr report set_led_pattern ( ffb_protocol const protocol, uti::u8_t pattern ) noexcept ;

        static constexpr report disable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
        static constexpr report  enable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;

        static constexpr report set_autocenter ( ffb_protocol const protocol, force const & force ) noexcept ;
        static constexpr report download_force ( ffb_protocol const protocol, force const & force ) noexcept ;

        static constexpr report    play_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
        static constexpr report refresh_force ( ffb_protocol const protocol, force const & f ) noexcept ;
        static constexpr report    stop_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept ;
private:
        static constexpr report  _constant_force ( ffb_protocol const protocol, force const & force ) noexcept ;
        static constexpr report    _spring_force ( ffb_protocol const protocol, force const & force ) noexcept ;
        static constexpr report    _damper_force ( ffb_protocol const protocol, force const & force ) noexcept ;
        static constexpr report _trapezoid_force ( ffb_protocol const protocol, force const & force ) noexcept ;
} ;

constexpr ffb_protocol get_supported_protocol ( hid_device const & device ) noexcept ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

constexpr report protocol::build_report ( ffb_protocol const protocol, command_type const cmd_type, uti::u8_t slots ) noexcept
{
        switch( cmd_type )
        {
                case command_type::AUTO_OFF      : return disable_autocenter( protocol, slots ) ;
                case command_type::AUTO_ON       : return  enable_autocenter( protocol, slots ) ;
                case command_type::PLAY_FORCE    : return         play_force( protocol, slots ) ;
                case command_type::STOP_FORCE    : return         stop_force( protocol, slots ) ;
                case command_type::AUTO_SET      : [[ fallthrough ]] ;
                case command_type::DL_FORCE      : [[ fallthrough ]] ;
                case command_type::REFRESH_FORCE :
                        FFFB_F_ERR_S( "protocol::build_report", "selected command requires parameters" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::build_report", "unknown command" ) ;
                        return {} ;
        }
}

constexpr report protocol::build_report ( ffb_protocol const protocol, command_type const cmd_type, force const & f ) noexcept
{
        switch( cmd_type )
        {
                case command_type::AUTO_SET      : return     set_autocenter( protocol, f ) ;
                case command_type::DL_FORCE      : return     download_force( protocol, f ) ;
                case command_type::REFRESH_FORCE : return      refresh_force( protocol, f ) ;
                case command_type::AUTO_ON       : [[ fallthrough ]] ;
                case command_type::AUTO_OFF      : [[ fallthrough ]] ;
                case command_type::PLAY_FORCE    : [[ fallthrough ]] ;
                case command_type::STOP_FORCE    :
                        FFFB_F_ERR_S( "protocol::build_report", "excess parameters for selected command" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::build_report", "unknown command" ) ;
                        return {} ;
        }
}

constexpr report protocol::set_led_pattern ( ffb_protocol const protocol, uti::u8_t pattern ) noexcept
{
        pattern = pattern & 0b00011111 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { 0xF8, 0x12, pattern, 0x00 } ;
                case ffb_protocol::logitech_hidpp   : return {} ;
                        FFFB_F_ERR_S( "protocol::set_led_pattern", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::set_led_pattern", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::disable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x05 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_F_ERR_S( "protocol::disable_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::disable_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::enable_autocenter ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x04 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_F_ERR_S( "protocol::enable_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::enable_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::set_autocenter ( ffb_protocol const protocol, force const & f ) noexcept
{
        uti::u8_t command = ( f.params.slot << 4 ) | 0x0e ;

        uti::u8_t   slope_l = f.spring.slope_left  | 0b0111 ;
        uti::u8_t   slope_r = f.spring.slope_right | 0b0111 ;
        uti::u8_t amplitude = f.spring.amplitude            ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x00, slope_l, slope_r, amplitude, 0x00 } ;
                case ffb_protocol::logitech_hidpp   :
                        FFFB_F_ERR_S( "protocol::set_autocenter", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::set_autocenter", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::download_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        switch( f.type )
        {
                case force_type:: CONSTANT : return  _constant_force( protocol, f ) ;
                case force_type::   SPRING : return    _spring_force( protocol, f ) ;
                case force_type::   DAMPER : return    _damper_force( protocol, f ) ;
                case force_type::TRAPEZOID : return _trapezoid_force( protocol, f ) ;
                default:
                        FFFB_F_ERR_S( "protocol::download_force", "force type not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::play_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x02 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic: return { command, 0x00 } ;
                        break ;
                case ffb_protocol::logitech_hidpp:
                        FFFB_F_ERR_S( "protocol::play_force", "protocol not implemented" ) ;
                        return {} ;
                default:
                        FFFB_F_ERR_S( "protocol::play_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::refresh_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        report rep = download_force( protocol, f ) ;

        rep.data[ 0 ] &= 0xF0 ;
        rep.data[ 0 ] |= 0x0C ;

        return rep ;
}

constexpr report protocol::stop_force ( ffb_protocol const protocol, uti::u8_t slots ) noexcept
{
        uti::u8_t command = ( slots << 4 ) | 0x03 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic: return { command, 0x00 } ;
                        break ;
                case ffb_protocol::logitech_hidpp:
                        FFFB_F_ERR_S( "protocol::play_force", "protocol not implemented" ) ;
                        return {} ;
                default:
                        FFFB_F_ERR_S( "protocol::play_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::_constant_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        uti::u8_t command   = f.params.slot << 4 ;
        uti::u8_t amplitude = f.constant.amplitude ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x00, amplitude, amplitude, amplitude, amplitude, 0x00 } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_F_ERR_S( "protocol::_constant_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::_constant_force", "protocol not supported" ) ;
                        return {} ;
        }
}

constexpr report protocol::_spring_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        uti::u8_t command = f.params.slot << 4 ;

        uti::u8_t dead_start   = f.spring.dead_start   ;
        uti::u8_t dead_end     = f.spring.dead_end     ;
        uti::u8_t slope_left   = f.spring.slope_left   & 0b0111 ;
        uti::u8_t slope_right  = f.spring.slope_right  & 0b0111 ;
        uti::u8_t invert_left  = f.spring.invert_left  & 0b0001 ;
        uti::u8_t invert_right = f.spring.invert_right & 0b0001 ;
        uti::u8_t amplitude    = f.spring.amplitude    ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x01,
                                                               dead_start, dead_end,
                                                               uti::u8_t( (  slope_right << 4 ) |  slope_left ),
                                                               uti::u8_t( ( invert_right << 4 ) | invert_left ),
                                                               amplitude } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_F_ERR_S( "protocol::_spring_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::_spring_force", "protocol not supported" ) ;
                        return {} ;
        }

}

constexpr report protocol::_damper_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        uti::u8_t command = f.params.slot << 4 ;

        uti::u8_t slope_left   = f.damper.slope_left   & 0b0111 ;
        uti::u8_t slope_right  = f.damper.slope_right  & 0b0111 ;
        uti::u8_t invert_left  = f.damper.invert_left  & 0b0001 ;
        uti::u8_t invert_right = f.damper.invert_right & 0b0001 ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x02, slope_left, invert_left, slope_right, invert_right, 0x00 } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_F_ERR_S( "protocol::_damper_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::_damper_force", "protocol not supported" ) ;
                        return {} ;
        }

}

constexpr report protocol::_trapezoid_force ( ffb_protocol const protocol, force const & f ) noexcept
{
        uti::u8_t command = f.params.slot << 4 ;

        uti::u8_t max_amp =   f.trapezoid.amplitude_max ;
        uti::u8_t min_amp =   f.trapezoid.amplitude_min ;
        uti::u8_t   t_max =   f.trapezoid.t_at_max ;
        uti::u8_t   t_min =   f.trapezoid.t_at_min ;
        uti::u8_t    dxdy = ( f.trapezoid.slope_step_x << 4 )
                            | f.trapezoid.slope_step_y ;

        switch( protocol )
        {
                case ffb_protocol::logitech_classic : return { command, 0x06, max_amp, min_amp, t_max, t_min, dxdy } ;
                case ffb_protocol::logitech_hidpp :
                        FFFB_F_ERR_S( "protocol::_trapezoid_force", "protocol not implemented" ) ;
                        return {} ;
                default :
                        FFFB_F_ERR_S( "protocol::_trapezoid_force", "protocol not supported" ) ;
                        return {} ;
        }
}


constexpr ffb_protocol get_supported_protocol ( hid_device const & device ) noexcept
{
        switch( device.vendor_id() )
        {
                case Logitech_VendorID:
                        return ffb_protocol::logitech_classic ;
                default:
                        return ffb_protocol::count ;
        }
}

////////////////////////////////////////////////////////////////////////////////


} // namespace fffb

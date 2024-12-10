///
///
///     fffb
///     flt.cpp
///

#define FFFB_LOG_LVL 0

#include <fffb/util/version.hpp>
#include <fffb/device/report.hpp>
#include <fffb/device/manager.hpp>
#include <fffb/wheel/protocol.hpp>
#include <fffb/wheel/controller.hpp>

#include <uti/string/string_view.hpp>

#include <cstdio>

#define FLT_CMD_QUIT        'q'
#define FLT_CMD_HELP        'h'
#define FLT_CMD_PLAY        'p'
#define FLT_CMD_CLEAR       'c'
#define FLT_CMD_CALIBRATE   'b'

#define FLT_CMD_AUTO        'a'
#define FLT_CMD_AUTO_ON     'y'
#define FLT_CMD_AUTO_OFF    'n'
#define FLT_CMD_AUTO_CONF   'c'

#define FLT_CMD_FORCE_OFF   'x'
#define FLT_CMD_SPRING_CONF 's'
#define FLT_CMD_DAMPER_CONF 'd'
#define FLT_CMD_CONST_CONF  'f'
#define FLT_CMD_TRAP_CONF   't'
#define FLT_CMD_LED_CONF    'l'


void print_help           () ;
void print_faint_prefix   () ;
void print_warning_prefix () ;
void print_error_prefix   () ;

void do_cmd_play      ( fffb::controller const & wheel ) ;
void do_cmd_clear     ( fffb::controller       & wheel ) ;
void do_cmd_calibrate ( fffb::controller       & wheel ) ;
void do_cmd_auto      ( fffb::controller const & wheel ) ;
void do_cmd_stop      ( fffb::controller const & wheel ) ;
void do_cmd_spring    ( fffb::controller       & wheel ) ;
void do_cmd_damper    ( fffb::controller       & wheel ) ;
void do_cmd_const     ( fffb::controller       & wheel ) ;
void do_cmd_trap      ( fffb::controller       & wheel ) ;
void do_cmd_led       ( fffb::controller       & wheel ) ;

int main ()
{
        printf( "%s", fffb::terminal_bold() ) ;
        printf( "   //////////////////////\n" ) ;
        printf( "  ///   flt v" FFFB_VERSION "   ///\n"  ) ;
        printf( " //////////////////////\n"   ) ;
        printf( "%s", fffb::terminal_reset() ) ;

        printf( "%s", fffb::terminal_faint() ) ;
        printf( "/// flt : looking for steering wheels...\n" ) ;
        printf( "%s", fffb::terminal_reset() ) ;

        fffb::controller wheel( fffb::device_manager().list_devices() ) ;

        if( !wheel )
        {
                print_error_prefix() ;
                printf( "no known wheels found\n" ) ;
                return 1 ;
        }
        else
        {
                uti::string device_name = wheel.device().get_property< uti::string >( kIOHIDProductKey ) ;

                printf( "%s/// flt : connected to wheel with device id 0x%.8x\n", fffb::terminal_faint(), wheel.device().device_id() ) ;
                printf( "/// flt %s: calibrating " SV_FMT "...\n", fffb::terminal_reset(), SV_ARG( device_name ) ) ;
        }

        if( !wheel.calibrate() )
        {
                print_error_prefix() ;
                printf( "wheel calibration failed\n" ) ;
                return 1 ;
        }
        print_faint_prefix() ;
        printf( "wheel calibration successful\n" ) ;

        char cmd { ' ' } ;

        print_faint_prefix() ;
        printf( "'h' for help\n" ) ;
        print_faint_prefix() ;

        while( cmd != FLT_CMD_QUIT )
        {
                scanf( "%c", &cmd ) ;

                switch( cmd )
                {
                        case '\n':
                                break ;
                        case FLT_CMD_QUIT:
                                break ;
                        case FLT_CMD_HELP:
                                print_help() ;
                                break ;
                        case FLT_CMD_PLAY:
                                do_cmd_play( wheel ) ;
                                break ;
                        case FLT_CMD_CLEAR:
                                do_cmd_clear( wheel ) ;
                                break ;
                        case FLT_CMD_CALIBRATE:
                                do_cmd_calibrate( wheel ) ;
                                break ;
                        case FLT_CMD_AUTO:
                                do_cmd_auto( wheel ) ;
                                break ;
                        case FLT_CMD_FORCE_OFF:
                                do_cmd_stop( wheel ) ;
                                break ;
                        case FLT_CMD_SPRING_CONF:
                                do_cmd_spring( wheel ) ;
                                break ;
                        case FLT_CMD_DAMPER_CONF:
                                do_cmd_damper( wheel ) ;
                                break ;
                        case FLT_CMD_CONST_CONF:
                                do_cmd_const( wheel ) ;
                                break ;
                        case FLT_CMD_TRAP_CONF:
                                do_cmd_trap( wheel ) ;
                                break ;
                        case FLT_CMD_LED_CONF:
                                do_cmd_led( wheel ) ;
                                break ;
                        default:
                                break ;
                }
                if( cmd != '\n' && cmd != FLT_CMD_QUIT )
                {
                        print_faint_prefix() ;
                        printf( "" ) ;
                }
        }
        printf( "%s///%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        print_faint_prefix() ;
        printf( "quitting...\n" ) ;
        printf( "%s//%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s/%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;


        return 0 ;
}

void do_cmd_play ( fffb::controller const & wheel )
{
        if( wheel.download_forces() && wheel.play_forces() )
        {
                print_faint_prefix() ;
                printf( "downloaded and played forces\n" ) ;
        }
        else
        {
                print_error_prefix() ;
                printf( "failed downloading and playing forces\n" ) ;
        }
}

void do_cmd_clear ( fffb::controller & wheel )
{
        if( wheel.clear_forces() )
        {
                print_faint_prefix() ;
                printf( "cleared forces\n" ) ;
        }
        else
        {
                print_error_prefix() ;
                printf( "failed clearing forces\n" ) ;
        }
}

void do_cmd_calibrate ( fffb::controller & wheel )
{
        print_faint_prefix() ;
        printf( "calibrating wheel...\n" ) ;

        if( wheel.calibrate() )
        {
                print_faint_prefix() ;
                printf( "calibration complete\n" ) ;
        }
        else
        {
                print_error_prefix() ;
                printf( "calibration failed\n" ) ;
        }
}

void do_cmd_auto ( fffb::controller const & wheel )
{
        print_faint_prefix() ;
        printf( "y to enable, n to disable, c to configure : " ) ;

        char cmd { 'n' } ;

        scanf( "%c", &cmd ) ;
        scanf( "%c", &cmd ) ;

        if( cmd == FLT_CMD_AUTO_ON )
        {
                if( !wheel.enable_autocenter() )
                {
                        print_error_prefix() ;
                        printf( "failed enabling autocenter spring\n" ) ;
                        return ;
                }
                print_faint_prefix() ;
                printf( "autocentering spring enabled\n" ) ;
        }
        else if( cmd == FLT_CMD_AUTO_OFF )
        {
                if( !wheel.disable_autocenter() )
                {
                        print_error_prefix() ;
                        printf( "failed disabling autocenter spring\n" ) ;
                        return ;
                }
                print_faint_prefix() ;
                printf( "autocentering spring disabled\n" ) ;
        }
        else if( cmd == FLT_CMD_AUTO_CONF )
        {
                uti::i32_t k, clip ;

                print_faint_prefix() ;
                printf( "slope (0..7) : " ) ;
                scanf( "%d", &k ) ;

                print_faint_prefix() ;
                printf( "max force (0..255) : " ) ;
                scanf( "%d", &clip ) ;

                if( !wheel.set_autocenter( fffb::force_data{ .type = fffb::force_type::AUTO, .params = { static_cast< uti::u8_t >( k ), static_cast< uti::u8_t >( clip ) } } ) )
                {
                        print_error_prefix() ;
                        printf( "failed setting autocenter spring\n" ) ;
                        return ;
                }
                print_faint_prefix() ;
                printf( "autocentering spring set\n" ) ;
        }
}

void do_cmd_stop ( fffb::controller const & wheel )
{
        if( !wheel.stop_forces() )
        {
                print_error_prefix() ;
                printf( "failed stopping forces\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "stopped forces\n" ) ;
}

void do_cmd_spring ( fffb::controller & wheel )
{
        uti::i32_t k1, k2, d1, d2, s1, s2, clip ;

        print_faint_prefix() ;
        printf( "dead band left (0..255) : " ) ;
        scanf( "%d", &d1 ) ;

        print_faint_prefix() ;
        printf( "dead band right (0..255) : " ) ;
        scanf( "%d", &d2 ) ;

        print_faint_prefix() ;
        printf( "slope left (0..7) : " ) ;
        scanf( "%d", &k1 ) ;

        print_faint_prefix() ;
        printf( "slope right (0..7) : " ) ;
        scanf( "%d", &k2 ) ;

        print_faint_prefix() ;
        printf( "slope invert left (0..1) : " ) ;
        scanf( "%d", &s1 ) ;

        print_faint_prefix() ;
        printf( "slope invert right (0..1) : " ) ;
        scanf( "%d", &s2 ) ;

        print_faint_prefix() ;
        printf( "max force (0..255) : " ) ;
        scanf( "%d", &clip ) ;

        fffb::force_data spring = {
                .type = fffb::force_type::SPRING ,
                .params = {
                        static_cast< uti::u8_t >(   d1 ),
                        static_cast< uti::u8_t >(   d2 ),
                        static_cast< uti::u8_t >(   k1 ),
                        static_cast< uti::u8_t >(   k2 ),
                        static_cast< uti::u8_t >(   s1 ),
                        static_cast< uti::u8_t >(   s2 ),
                        static_cast< uti::u8_t >( clip )
                }
        } ;

        if( !wheel.add_force( spring ) )
        {
                print_error_prefix() ;
                printf( "failed setting custom spring\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "custom spring set\n" ) ;
}

void do_cmd_damper ( fffb::controller & wheel )
{
        uti::i32_t k1, k2, s1, s2 ;

        print_faint_prefix() ;
        printf( "push left (0..7) : " ) ;
        scanf( "%d", &k1 ) ;

        print_faint_prefix() ;
        printf( "invert left (0..1) : " ) ;
        scanf( "%d", &s1 ) ;

        print_faint_prefix() ;
        printf( "push right (0..7) : " ) ;
        scanf( "%d", &k2 ) ;

        print_faint_prefix() ;
        printf( "invert right (0..1) : " ) ;
        scanf( "%d", &s2 ) ;

        fffb::force_data damper = {
                .type = fffb::force_type::DAMPER ,
                .params = {
                        static_cast< uti::u8_t >( k1 ),
                        static_cast< uti::u8_t >( k2 ),
                        static_cast< uti::u8_t >( s1 ),
                        static_cast< uti::u8_t >( s2 ),
                }
        } ;

        if( !wheel.add_force( damper ) )
        {
                print_error_prefix() ;
                printf( "failed setting damper\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "damper set\n" ) ;
}

void do_cmd_const ( fffb::controller & wheel )
{
        uti::i32_t force ;

        print_faint_prefix() ;
        printf( "force (0..255) : " ) ;
        scanf( "%d", &force ) ;

        fffb::force_data constant = {
                .type = fffb::force_type::CONSTANT ,
                .params = { static_cast< uti::u8_t >( force ) }
        } ;

        if( !wheel.add_force( constant ) )
        {
                print_error_prefix() ;
                printf( "failed setting constant force\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "constant force set\n" ) ;
}

void do_cmd_trap ( fffb::controller & wheel )
{
        uti::i32_t force_max, force_min, t_max, t_min, diff_x, diff_y ;

        print_faint_prefix() ;
        printf( "max force (0..255) : " ) ;
        scanf( "%d", &force_max ) ;

        print_faint_prefix() ;
        printf( "min force (0..255) : " ) ;
        scanf( "%d", &force_min ) ;

        print_faint_prefix() ;
        printf( "time at max force (0..255) : " ) ;
        scanf( "%d", &t_max ) ;

        print_faint_prefix() ;
        printf( "time at min force (0..255) : " ) ;
        scanf( "%d", &t_min ) ;

        print_faint_prefix() ;
        printf( "slope step x (0..15) : " ) ;
        scanf( "%d", &diff_x ) ;

        print_faint_prefix() ;
        printf( "slope step y (0..15) : " ) ;
        scanf( "%d", &diff_y ) ;

        fffb::force_data trap = {
                .type = fffb::force_type::TRAPEZOID ,
                .params = { 
                        static_cast< uti::u8_t >( force_max ),
                        static_cast< uti::u8_t >( force_min ),
                        static_cast< uti::u8_t >( t_max ),
                        static_cast< uti::u8_t >( t_min ),
                        static_cast< uti::u8_t >( diff_x ),
                        static_cast< uti::u8_t >( diff_y )
                }
        } ;

        if( !wheel.add_force( trap ) )
        {
                print_error_prefix() ;
                printf( "failed setting trapezoid force\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "trapezoid force set\n" ) ;
}

void do_cmd_led ( fffb::controller & wheel )
{
        uti::i32_t led_pattern ;

        print_faint_prefix() ;
        printf( "led pattern (0..31) : " ) ;
        scanf( "%d", &led_pattern ) ;

        ( void ) wheel ;
//      if( !wheel.set_led_pattern( led_pattern ) )
        {
                print_error_prefix() ;
                printf( "failed setting led pattern\n" ) ;
                return ;
        }
        print_faint_prefix() ;
        printf( "led pattern set\n" ) ;
}

void print_help ()
{
        printf( "%s///%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s flt - fuck logitech - v" FFFB_VERSION "\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s commands :\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    q - quit\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    h - help\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    p - play forces\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    c - clear forces\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    b - recalibrate wheel\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    a - configure autocentering\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    x - stop forces\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    s - configure spring force\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    d - configure damper force\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    f - configure constant force\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    t - configure trapezoid force\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s    l - configure leds\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
        printf( "%s///%s\n", fffb::terminal_faint(), fffb::terminal_reset() ) ;
}

void print_faint_prefix ()
{
        printf( "%s/// flt %s: ", fffb::terminal_faint(), fffb::terminal_reset() ) ;
}

void print_warning_prefix ()
{
        printf( "%s/// flt::warning %s: ", fffb::terminal_yellow(), fffb::terminal_reset() ) ;
}

void print_error_prefix ()
{
        printf( "%s/// flt::error %s: ", fffb::terminal_red(), fffb::terminal_reset() ) ;
}

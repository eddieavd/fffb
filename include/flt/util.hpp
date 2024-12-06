//
//
//      flt
//      util.hpp
//

#pragma once

#include <cstdio>

#include <IOKit/IOReturn.h>

#include <mach/mach_error.h>

#define FLT_VERSION "0.0.3"


namespace flt
{


constexpr void terminal_cmd ( int cmd ) { printf( "\033[%dm", cmd ) ; }

constexpr void terminal_reset  () { terminal_cmd(  0 ) ; }
constexpr void terminal_bold   () { terminal_cmd(  1 ) ; }
constexpr void terminal_faint  () { terminal_cmd(  2 ) ; }
constexpr void terminal_red    () { terminal_cmd( 31 ) ; }
constexpr void terminal_green  () { terminal_cmd( 32 ) ; }
constexpr void terminal_yellow () { terminal_cmd( 33 ) ; }

constexpr char const * terminal_reset_cstr  () { return "\033[0m"  ; }
constexpr char const * terminal_bold_cstr   () { return "\033[1m"  ; }
constexpr char const * terminal_faint_cstr  () { return "\033[2m"  ; }
constexpr char const * terminal_red_cstr    () { return "\033[31m" ; }
constexpr char const * terminal_green_cstr  () { return "\033[32m" ; }
constexpr char const * terminal_yellow_cstr () { return "\033[33m" ; }

constexpr bool _try ( char const * loc, IOReturn result ) noexcept
{
        if( result != kIOReturnSuccess )
        {
                terminal_bold() ;
                terminal_red() ;
                printf( "=== flt::error " ) ;
                terminal_reset() ;
                printf( ": %s failed with error code %x (%s)\n", loc, result, mach_error_string( result ) ) ;
                return false ;
        }
        return true ;
}

constexpr void print_info ( char const * message ) noexcept
{
        ( void ) message ;
//      printf( "%s=== flt::info : %s%s\n", terminal_faint_cstr(), message, terminal_reset_cstr() ) ;
}

constexpr void print_error ( char const * message ) noexcept
{
        printf( "%s%s=== flt::error %s: %s\n", flt::terminal_red_cstr(), flt::terminal_bold_cstr(), flt::terminal_reset_cstr(), message ) ;
}


} // namespace flt

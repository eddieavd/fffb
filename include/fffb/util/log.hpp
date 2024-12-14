///
///
///     fffb
///     util/log.hpp
///

#pragma once

#include <uti/string/string.hpp>
#include <uti/string/string_view.hpp>

#include <ctime>
#include <cstdarg>
#include <cstdio>

#include <pthread.h>

#define FFFB_LOG_FILE_PATH "/tmp/fffb.log"

#define FFFB_VTSEQ(ID) ("\x1b[" #ID "m")

#ifndef   FFFB_LOG_LVL
#define   FFFB_LOG_LVL 99
#endif // FFFB_LOG_LVL

#ifndef FFFB_LOGS

#define FFFB_LOG(...)
#define FFFB_SUCC(...)
#define FFFB_FAIL(...)
#define FFFB_DBG(...)
#define FFFB_INFO(...)
#define FFFB_WARN(...)
#define FFFB_ERR(...)

#define FFFB_LOG_S(...)
#define FFFB_SUCC_S(...)
#define FFFB_FAIL_S(...)
#define FFFB_DBG_S(...)
#define FFFB_INFO_S(...)
#define FFFB_WARN_S(...)
#define FFFB_ERR_S(...)

#define FFFB_F_LOG(...)
#define FFFB_F_SUCC(...)
#define FFFB_F_FAIL(...)
#define FFFB_F_DBG(...)
#define FFFB_F_INFO(...)
#define FFFB_F_WARN(...)
#define FFFB_F_ERR(...)

#define FFFB_F_LOG_S(...)
#define FFFB_F_SUCC_S(...)
#define FFFB_F_FAIL_S(...)
#define FFFB_F_DBG_S(...)
#define FFFB_F_INFO_S(...)
#define FFFB_F_WARN_S(...)
#define FFFB_F_ERR_S(...)

#else

#define FFFB_LOG(...)  fffb::log_2( stderr,                        __VA_ARGS__ )
#define FFFB_SUCC(...) fffb::log_2( stderr, fffb::log_level::succ, __VA_ARGS__ )
#define FFFB_FAIL(...) fffb::log_2( stderr, fffb::log_level::fail, __VA_ARGS__ )

#define FFFB_LOG_S(...)  fffb::log_3( stderr,                        __VA_ARGS__ )
#define FFFB_SUCC_S(...) fffb::log_3( stderr, fffb::log_level::succ, __VA_ARGS__ )
#define FFFB_FAIL_S(...) fffb::log_3( stderr, fffb::log_level::fail, __VA_ARGS__ )

#define FFFB_DBG(...)
#define FFFB_INFO(...)
#define FFFB_WARN(...)
#define FFFB_ERR(...)

#define FFFB_DBG_S(...)
#define FFFB_INFO_S(...)
#define FFFB_WARN_S(...)
#define FFFB_ERR_S(...)

#if FFFB_LOG_LVL > 0
#       undef  FFFB_ERR
#       undef  FFFB_ERR_S
#       define FFFB_ERR(...)   fffb::log_2( stderr, fffb::log_level::err , __VA_ARGS__ )
#       define FFFB_ERR_S(...) fffb::log_3( stderr, fffb::log_level::err , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 1
#       undef  FFFB_WARN
#       undef  FFFB_WARN_S
#       define FFFB_WARN(...)   fffb::log_2( stderr, fffb::log_level::warn , __VA_ARGS__ )
#       define FFFB_WARN_S(...) fffb::log_3( stderr, fffb::log_level::warn , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 2
#       undef  FFFB_INFO
#       undef  FFFB_INFO_S
#       define FFFB_INFO(...)   fffb::log_2( stderr, fffb::log_level::info , __VA_ARGS__ )
#       define FFFB_INFO_S(...) fffb::log_3( stderr, fffb::log_level::info , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 3
#       undef  FFFB_DBG
#       undef  FFFB_DBG_S
#       define FFFB_DBG(...)   fffb::log_2( stderr, fffb::log_level::dbg , __VA_ARGS__ )
#       define FFFB_DBG_S(...) fffb::log_3( stderr, fffb::log_level::dbg , __VA_ARGS__ )
#endif

#define FFFB_F_LOG(...)  fffb::log_2( fffb::g_log_file.fptr,                        __VA_ARGS__ )
#define FFFB_F_SUCC(...) fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::succ, __VA_ARGS__ )
#define FFFB_F_FAIL(...) fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::fail, __VA_ARGS__ )

#define FFFB_F_LOG_S(...)  fffb::log_3( fffb::g_log_file.fptr,                        __VA_ARGS__ )
#define FFFB_F_SUCC_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::succ, __VA_ARGS__ )
#define FFFB_F_FAIL_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::fail, __VA_ARGS__ )

#define FFFB_F_DBG(...)
#define FFFB_F_INFO(...)
#define FFFB_F_WARN(...)
#define FFFB_F_ERR(...)

#define FFFB_F_DBG_S(...)
#define FFFB_F_INFO_S(...)
#define FFFB_F_WARN_S(...)
#define FFFB_F_ERR_S(...)

#if FFFB_LOG_LVL > 0
#       undef  FFFB_F_ERR
#       undef  FFFB_F_ERR_S
#       define FFFB_F_ERR(...)   fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::err , __VA_ARGS__ )
#       define FFFB_F_ERR_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::err , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 1
#       undef  FFFB_F_WARN
#       undef  FFFB_F_WARN_S
#       define FFFB_F_WARN(...)   fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::warn , __VA_ARGS__ )
#       define FFFB_F_WARN_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::warn , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 2
#       undef  FFFB_F_INFO
#       undef  FFFB_F_INFO_S
#       define FFFB_F_INFO(...)   fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::info , __VA_ARGS__ )
#       define FFFB_F_INFO_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::info , __VA_ARGS__ )
#endif

#if FFFB_LOG_LVL > 3
#       undef  FFFB_F_DBG
#       undef  FFFB_F_DBG_S
#       define FFFB_F_DBG(...)   fffb::log_2( fffb::g_log_file.fptr, fffb::log_level::dbg , __VA_ARGS__ )
#       define FFFB_F_DBG_S(...) fffb::log_3( fffb::g_log_file.fptr, fffb::log_level::dbg , __VA_ARGS__ )
#endif

#endif // FFFB_LOGS


namespace fffb
{


constexpr char const * terminal_reset     () { return FFFB_VTSEQ( 0 ) ; }
constexpr char const * terminal_bold      () { return FFFB_VTSEQ( 1 ) ; }
constexpr char const * terminal_faint     () { return FFFB_VTSEQ( 2 ) ; }
constexpr char const * terminal_italic    () { return FFFB_VTSEQ( 3 ) ; }
constexpr char const * terminal_underline () { return FFFB_VTSEQ( 4 ) ; }

constexpr char const * terminal_black     () { return FFFB_VTSEQ( 30 ) ; }
constexpr char const * terminal_red       () { return FFFB_VTSEQ( 31 ) ; }
constexpr char const * terminal_green     () { return FFFB_VTSEQ( 32 ) ; }
constexpr char const * terminal_yellow    () { return FFFB_VTSEQ( 33 ) ; }
constexpr char const * terminal_blue      () { return FFFB_VTSEQ( 34 ) ; }
constexpr char const * terminal_purple    () { return FFFB_VTSEQ( 35 ) ; }
constexpr char const * terminal_cyan      () { return FFFB_VTSEQ( 36 ) ; }
constexpr char const * terminal_lgray     () { return FFFB_VTSEQ( 37 ) ; }
constexpr char const * terminal_lred      () { return FFFB_VTSEQ( 91 ) ; }


struct log_file
{
        constexpr  log_file ( char const * filepath ) noexcept :     fptr { fopen( filepath, "w" ) } {}
        constexpr ~log_file (                       ) noexcept { if( fptr ) fclose( fptr ) ; }

        constexpr operator bool () const noexcept { return fptr != nullptr ; }

        FILE * fptr ;
} ;

static log_file g_log_file( FFFB_LOG_FILE_PATH ) ;


enum class log_level
{
        succ ,
        fail ,
        err  ,
        warn ,
        info ,
        dbg  ,
} ;


namespace _detail
{


constexpr timespec time_empty () noexcept
{
        return { 0, 0 } ;
}

constexpr timespec time_current () noexcept
{
        timespec time ;
        clock_gettime( CLOCK_REALTIME, &time ) ;

        return time ;
}

constexpr timespec time_current_mono () noexcept
{
        timespec time ;
        clock_gettime( CLOCK_MONOTONIC_RAW, &time ) ;

        return time ;
}

constexpr uti::string time_string ()
{
        uti::string time_str( 2 + 1 + 2 + 1 + 2 + 1 + 3 + 1 ) ; // HH:MM:SS:mmm\0

        timespec time = time_current() ;

        struct tm const * tms = localtime( &time.tv_sec ) ;

        uti::i32_t msec = time.tv_nsec / 1000000 ;

        time_str.push_back( '0' + ( tms->tm_hour / 10 ) ) ;
        time_str.push_back( '0' + ( tms->tm_hour % 10 ) ) ;
        time_str.push_back( ':' ) ;
        time_str.push_back( '0' + ( tms->tm_min / 10 ) ) ;
        time_str.push_back( '0' + ( tms->tm_min % 10 ) ) ;
        time_str.push_back( ':' ) ;
        time_str.push_back( '0' + ( tms->tm_sec / 10 ) ) ;
        time_str.push_back( '0' + ( tms->tm_sec % 10 ) ) ;
        time_str.push_back( ':' ) ;
        time_str.push_back( '0' + ( msec / 100 )      ) ;
        time_str.push_back( '0' + ( msec /  10 ) % 10 ) ;
        time_str.push_back( '0' + ( msec %  10 )      ) ;

        return time_str ;
}

constexpr void log_2_v ( FILE * dest, log_level level, char const * fmt, va_list args )
{
        uti::string time = time_string() ;

        switch( level )
        {
                case log_level::dbg:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::dbg  : ", terminal_purple(), terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                case log_level::info:
                        fprintf( dest, "%s:: " SV_FMT " : fffb::info : ", terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                case log_level::warn:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::warn : ", terminal_yellow(), terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                case log_level::err:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::err  : ", terminal_red(), terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                case log_level::succ:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::succ : ", terminal_green(), terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                case log_level::fail:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::fail : ", terminal_red(), terminal_bold(), SV_ARG( time ) ) ;
                        break ;
                default:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::%4d : ", terminal_red(), terminal_faint(), SV_ARG( time ), uti::to_underlying( level ) ) ;
                        break ;
        }
        fprintf( dest, "%s", terminal_reset() ) ;

        vfprintf( dest, fmt, args ) ;
        fprintf( dest, "\n" ) ;
}

constexpr void log_3_v ( FILE * dest, log_level level, char const * scope, char const * fmt, va_list args )
{
        uti::string time = time_string() ;

        switch( level )
        {
                case log_level::dbg:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::dbg  : %s : ", terminal_purple(), terminal_bold(), SV_ARG( time ), scope ) ;
                        break ;
                case log_level::info:
                        fprintf( dest, "%s:: " SV_FMT " : fffb::info : %s : ", terminal_bold(), SV_ARG( time ), scope ) ;
                        break ;
                case log_level::warn:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::warn : %s : ", terminal_yellow(), terminal_bold(), SV_ARG( time ), scope ) ;
                        break ;
                case log_level::err:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::err  : %s : ", terminal_red(), terminal_bold(), SV_ARG( time ), scope) ;
                        break ;
                case log_level::succ:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::succ : %s : ", terminal_green(), terminal_bold(), SV_ARG( time ), scope ) ;
                        break ;
                case log_level::fail:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::fail : %s : ", terminal_red(), terminal_bold(), SV_ARG( time ), scope ) ;
                        break ;
                default:
                        fprintf( dest, "%s%s:: " SV_FMT " : fffb::%4d : %s : ", terminal_red(), terminal_faint(), SV_ARG( time ), uti::to_underlying( level ), scope ) ;
                        break ;
        }
        fprintf( dest, "%s", terminal_reset() ) ;

        vfprintf( dest, fmt, args ) ;
        fprintf( dest, "\n" ) ;
}


} // namespace _detail

constexpr void log_2 ( FILE * dest, log_level level, char const * fmt, ... )
{
        if( dest == nullptr ) dest = stderr ;

        va_list args ;
        va_start( args, fmt ) ;
        _detail::log_2_v( dest, level, fmt, args ) ;
        va_end( args ) ;
}

constexpr void log_3 ( FILE * dest, log_level level, char const * scope, char const * fmt, ... )
{
        if( dest == nullptr ) dest = stderr ;

        va_list args ;
        va_start( args, fmt ) ;
        _detail::log_3_v( dest, level, scope, fmt, args ) ;
        va_end( args ) ;
}


} // namespace fffb

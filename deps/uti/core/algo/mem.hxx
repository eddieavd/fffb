//
//
//      uti
//      core/algo/mem.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/iterator/meta.hxx>

#include <memory>


namespace uti
{


template< meta::iterator Iter, typename... Args >
constexpr void construct ( Iter iter, Args&&... args )
        noexcept( is_nothrow_constructible_v< iter_value_t< Iter >, Args... > )
{
        ::std::construct_at( static_cast< iter_value_t< Iter > * >( iter ), UTI_FWD( args )... ) ;
//      ::new ( ( void * ) iter ) iter_value_t< Iter >( UTI_FWD( args )... ) ;
}

template< meta::iterator Iter, typename... Args >
constexpr void destroy ( Iter iter )
        noexcept( is_nothrow_destructible_v< iter_value_t< Iter > > )
{
        iter->~iter_value_t< Iter >() ;
}

template< meta::forward_iterator Iter >
constexpr void memclr ( Iter begin, Iter const & end )
{
        u8_t * mem = static_cast< u8_t * >( static_cast< void * >( begin ) ) ;
        u8_t * fin = static_cast< u8_t * >( static_cast< void * >(   end ) ) ;

        while( mem != fin )
        {
                *mem = 0 ;
                ++mem ;
        }
}

template< meta::forward_iterator Iter >
constexpr void memset ( Iter begin, Iter const & end, iter_value_t< Iter > const & val )
{
        while( begin != end )
        {
                *begin = val ;
                ++begin ;
        }
}

template< typename CharType >
UTI_NODISCARD constexpr ssize_t strlen ( CharType const * str ) noexcept
{
        ssize_t len { -1 };
        while( str[ ++len ] != '\0' && len < i64_t_max ) {}
        return len;
}

template< typename CharType >
UTI_NODISCARD constexpr CharType to_lower ( CharType chr ) noexcept
{
        if( 'A' <= chr && chr <= 'Z' ) return chr + 0x20;
        return chr;
}


} // namespace uti

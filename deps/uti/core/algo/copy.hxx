//
//
//      uti
//      core/algo/copy.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/algo/distance.hxx>
#include <uti/core/iterator/meta.hxx>

#if !__has_builtin( __builtin_memmove ) && defined( UTI_HAS_STL )
#include <cstring>
#endif


namespace uti
{


template< meta::forward_iterator Iter, meta::forward_iterator DestIter >
constexpr void _copy_impl ( Iter begin, Iter end, DestIter dest )
{
        while( begin != end )
        {
                *dest = *begin ;
                ++dest  ;
                ++begin ;
        }
}

template< meta::forward_iterator Iter, meta::forward_iterator DestIter >
constexpr void copy ( Iter begin, Iter const & end, DestIter dest )
{
        using value_type = iterator_traits< Iter >::value_type ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                [[ maybe_unused ]]
                ssize_t const n = ::uti::distance( begin, end ) ;
#if UTI_HAS_BUILTIN( __builtin_memcpy )
                __builtin_memcpy( dest, begin, n * sizeof( value_type ) ) ;
#elif defined( UTI_HAS_STL )
                std::memcpy( dest, begin, n * sizeof( value_type ) ) ;
#else
                _copy_impl( begin, end, dest ) ;
#endif
        }
        else
        {
                _copy_impl( begin, end, dest ) ;
        }
}


template< meta::bidirectional_iterator Iter, meta::bidirectional_iterator DestIter >
constexpr void _copy_back_impl ( Iter begin, Iter end, DestIter dest )
{
        while( begin != end )
        {
                *dest = *begin ;
                --dest  ;
                --begin ;
        }
}

template< meta::bidirectional_iterator Iter, meta::bidirectional_iterator DestIter >
constexpr void copy_backward ( Iter begin, Iter const & end, DestIter dest )
{
        _copy_back_impl( begin, end, dest ) ;
}


} // namespace uti

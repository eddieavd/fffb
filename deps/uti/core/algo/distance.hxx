//
//
//      uti
//      core/algo/distance.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/iterator/meta.hxx>


namespace uti
{


template< meta::input_iterator Iter >
constexpr typename iterator_traits< Iter >::difference_type distance ( Iter _begin_, Iter const _end_ ) noexcept
{
        typename iterator_traits< Iter >::difference_type dist { 0 } ;

        for( ; _begin_ != _end_; ++_begin_, ++dist ) {}

        return dist ;
}

template< meta::random_access_iterator Iter >
constexpr typename iterator_traits< Iter >::difference_type distance ( Iter _begin_, Iter const _end_ ) noexcept
{
        return _end_ - _begin_ ;
}


} // namespace uti

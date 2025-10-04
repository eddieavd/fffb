//
//
//      uti
//      core/algo/advance.hxx
//

#pragma once

#include <uti/core/iterator/meta.hxx>


namespace uti
{


namespace _detail
{


template< typename Iter, typename DistType >
constexpr void _advance_impl ( Iter & _iter_, DistType _dist_, input_iterator_tag ) noexcept
{
        while( _dist_ > 0 )
        {
                ++_iter_ ;
                --_dist_ ;
        }
}

template< typename Iter, typename DistType >
constexpr void _advance_impl ( Iter & _iter_, DistType _dist_, bidirectional_iterator_tag ) noexcept
{
        while( _dist_ > 0 )
        {
                ++_iter_ ;
                --_dist_ ;
        }
        while( _dist_ < 0 )
        {
                --_iter_ ;
                ++_dist_ ;
        }
}

template< typename Iter, typename DistType >
constexpr void _advance_impl ( Iter & _iter_, DistType _dist_, random_access_iterator_tag ) noexcept
{
        _iter_ += _dist_ ;
}


} // namespace _detail


template< meta::input_iterator Iter, typename DistType >
constexpr void advance ( Iter & _iter_, DistType _dist_ ) noexcept
{
        _detail::_advance_impl( _iter_, _dist_, typename iterator_traits< Iter >::iterator_category{} ) ;
}


} // namespace uti

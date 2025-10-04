//
//
//      uti
//      core/algo/swap.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>


namespace uti
{


template< typename T >
        requires is_not_v< _is_tuple_like< T > > &&
                 is_move_constructible_v< T > &&
                 is_move_assignable_v< T >
constexpr void swap ( T & _lhs_, T & _rhs_ )
        noexcept( is_nothrow_move_constructible_v< T > &&
                  is_nothrow_move_assignable_v   < T >  )
{
        T _tmp_( UTI_MOVE( _lhs_ ) ) ;
        _lhs_ =  UTI_MOVE( _rhs_ )   ;
        _rhs_ =  UTI_MOVE( _tmp_ )   ;
}

template< typename T, ssize_t N >
constexpr
enable_if_t< _is_swappable< T >::value >
swap ( T ( & a )[ N ], T ( & b )[ N ] ) noexcept( _is_nothrow_swappable< T >::value ) ;



} // namespace uti

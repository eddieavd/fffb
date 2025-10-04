//
//
//      uti
//      core/type/sequence.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP

#include <uti/core/type/traits.hxx>


namespace uti
{


template< typename T, T... Ts >
struct integer_sequence
{
        using value_type =                T ;
        using       type = integer_sequence ;

        static_assert( is_integral_v< value_type >, "uti::integer_sequence can only be instantiated with integral types" );

        static constexpr ssize_t size () noexcept { return sizeof...( Ts ); }
};

template< ssize_t... Ts >
using index_sequence = integer_sequence< ssize_t, Ts... > ;


template< typename T, typename Seq1, typename Seq2 >
struct _merge_and_renumber ;

template< typename T, T... I1, T... I2 >
struct _merge_and_renumber< T, integer_sequence< T, I1... >, integer_sequence< T, I2... > >
        : integer_sequence< T, I1..., ( sizeof...( I1 ) + I2 )... > {} ;

template< typename T, ssize_t N >
struct _make_integer_sequence
        : _merge_and_renumber< T,
                               typename _make_integer_sequence< T,     N / 2 >::type,
                               typename _make_integer_sequence< T, N - N / 2 >::type > {} ;

template< typename T > struct _make_integer_sequence< T, 0 > : integer_sequence< T    > {} ;
template< typename T > struct _make_integer_sequence< T, 1 > : integer_sequence< T, 0 > {} ;

template< typename T, ssize_t N >
using make_integer_sequence = typename _make_integer_sequence< T, N >::type ;

template< ssize_t N >
using make_index_sequence = make_integer_sequence< ssize_t, N > ;

template< typename... Ts >
using index_sequence_for = make_index_sequence< sizeof...( Ts ) > ;


} // namespace uti

#endif // UTI_DOXYGEN_SKIP

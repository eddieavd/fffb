//
//
//      uti
//      core/container/meta.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>

#define UTI_SHADOW_IMPL( name, temp_args, specialization ) \
        template< temp_args > \
                requires true \
        name< specialization >


namespace uti
{


template< typename T >
constexpr
conditional_t
<
        !is_nothrow_move_constructible_v< T > &&
         is_copy_constructible_v< T >,
        T const &,
        T &&
>
_move_if_noexcept ( T & _val_ ) noexcept
{
        return UTI_MOVE( _val_ );
}


namespace meta
{


template< typename T > using has_value_type      = T::     value_type ;
template< typename T > using has_size_type       = T::      size_type ;
template< typename T > using has_ssize_type      = T::     ssize_type ;
template< typename T > using has_difference_type = T::difference_type ;
template< typename T > using has_pointer         = T::        pointer ;
template< typename T > using has_const_pointer   = T::  const_pointer ;
template< typename T > using has_reference       = T::      reference ;
template< typename T > using has_const_reference = T::const_reference ;
template< typename T > using has_iterator        = T::       iterator ;
template< typename T > using has_const_iterator  = T:: const_iterator ;

template< typename T > using has_begin = decltype( uti::declval< T >().begin() ) ;
template< typename T > using has_end   = decltype( uti::declval< T >().  end() ) ;
template< typename T > using has_range = decltype( uti::declval< T >().range( ssize_t(), ssize_t() ) ) ;


template< typename T > using is_container = integral_constant< is_detected_v< has_value_type    , T > &&
                                                               is_detected_v< has_size_type     , T > &&
                                                               is_detected_v< has_ssize_type    , T > &&
                                                               is_detected_v< has_iterator      , T > &&
                                                               is_detected_v< has_const_iterator, T > &&
                                                               is_detected_v< has_begin         , T > &&
                                                               is_detected_v< has_end           , T > > ;

template< typename T > inline constexpr bool is_container_v = is_container< T >::value ;

template< typename T > using is_range_container = conjunction< is_container< T >, integral_constant< is_detected_v< has_range, T > > > ;

template< typename T > inline constexpr bool is_range_container_v = is_range_container< T >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T > using is_simple_container = integral_constant< is_detected_v< has_value_type, T > &&
                                                                      is_detected_v< has_begin     , T > &&
                                                                      is_detected_v< has_end       , T > > ;

template< typename T > inline constexpr bool is_simple_container_v = is_simple_container< T >::value ;

template< typename T >
concept simple_container = is_simple_container_v< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T > using is_2d_container = integral_constant< is_container_v< T > && is_container_v< typename T::value_type > > ;
template< typename T > inline constexpr bool is_2d_container_v = is_2d_container< T >::value ;

template< typename T > using is_3d_container = integral_constant< is_2d_container_v< T > && is_2d_container_v< typename T::value_type > > ;
template< typename T > inline constexpr bool is_3d_container_v = is_3d_container< T >::value ;

template< typename T > using is_2d_range_container = integral_constant< is_range_container_v< T > && is_range_container_v< typename T::value_type > > ;
template< typename T > inline constexpr bool is_2d_range_container_v = is_2d_range_container< T >::value ;

template< typename T > using is_3d_range_container = integral_constant< is_2d_range_container_v< T > && is_2d_range_container_v< typename T::value_type > > ;
template< typename T > inline constexpr bool is_3d_range_container_v = is_3d_range_container< T >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T > using _2d_container_base_t = typename T::value_type::value_type             ;
template< typename T > using _3d_container_base_t = typename T::value_type::value_type::value_type ;

template< typename Container, typename T = void > using enable_if_2d_container   = enable_if< is_2d_container_v< Container >, T > ;
template< typename Container, typename T = void > using enable_if_2d_container_t = typename enable_if_2d_container< Container, T >::type ;

template< typename Container, typename T = void > using enable_if_3d_container   = enable_if< is_3d_container_v< Container >, T > ;
template< typename Container, typename T = void > using enable_if_3d_container_t = typename enable_if_3d_container< Container, T >::type ;

template< typename Container, typename T = void > using enable_if_2d_range_container   = enable_if< is_2d_range_container_v< Container >, T > ;
template< typename Container, typename T = void > using enable_if_2d_range_container_t = typename enable_if_2d_range_container< Container, T >::type ;

template< typename Container, typename T = void > using enable_if_3d_range_container   = enable_if< is_3d_range_container_v< Container >, T > ;
template< typename Container, typename T = void > using enable_if_3d_range_container_t = typename enable_if_3d_range_container< Container, T >::type ;

////////////////////////////////////////////////////////////////////////////////

template< typename Container > using enable_2d_container_base   = enable_if_2d_container< Container, _2d_container_base_t< Container > > ;
template< typename Container > using enable_2d_container_base_t = typename enable_2d_container_base< Container >::type ;

template< typename Container > using enable_3d_container_base   = enable_if_3d_container< Container, _3d_container_base_t< Container > > ;
template< typename Container > using enable_3d_container_base_t = typename enable_3d_container_base< Container >::type ;

template< typename Container > using enable_2d_range_container_base   = enable_if_2d_range_container< Container, _2d_container_base_t< Container > > ;
template< typename Container > using enable_2d_range_container_base_t = typename enable_2d_range_container_base< Container >::type ;

template< typename Container > using enable_3d_range_container_base   = enable_if_3d_range_container< Container, _3d_container_base_t< Container > > ;
template< typename Container > using enable_3d_range_container_base_t = typename enable_3d_range_container_base< Container >::type ;

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t N, bool = is_container_v< T > > struct is_n_dim_container               : false_type                                          {} ;
template< typename T, ssize_t N                             > struct is_n_dim_container< T, N, true > : is_n_dim_container< typename T::value_type, N - 1 > {} ;

template< typename T > struct is_n_dim_container< T, 0,  true > : true_type {} ;
template< typename T > struct is_n_dim_container< T, 0, false > : true_type {} ;

template< typename T, ssize_t N > inline constexpr bool is_n_dim_container_v = is_n_dim_container< T, N >::value ;


template< typename T, ssize_t N, bool = is_container_v< T > > struct is_exactly_n_dim_container               : false_type                                                  {} ;
template< typename T, ssize_t N                             > struct is_exactly_n_dim_container< T, N, true > : is_exactly_n_dim_container< typename T::value_type, N - 1 > {} ;

template< typename T > struct is_exactly_n_dim_container< T, 0,  true > : false_type {} ;
template< typename T > struct is_exactly_n_dim_container< T, 0, false > :  true_type {} ;

template< typename T, ssize_t N > inline constexpr bool is_exactly_n_dim_container_v = is_exactly_n_dim_container< T, N >::value ;


template< typename T, bool = is_container_v< T > > struct inner_value_type            : type_identity< T >                         {} ;
template< typename T                             > struct inner_value_type< T, true > : inner_value_type< typename T::value_type > {} ;

template< typename T > using inner_value_type_t = typename inner_value_type< T >::type ;


} // namespace meta


} // namespace uti

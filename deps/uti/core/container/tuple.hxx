//
//
//      uti
//      core/container/tuple.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/type/ref_wrap.hxx>
#include <uti/core/type/sequence.hxx>
#include <uti/core/type/invoke.hxx>
#include <uti/core/meta/list.hxx>


namespace uti
{


template< typename... Elems >
struct tuple
{
        constexpr tuple () = default ;
} ;

template< typename Elem0, typename... Elems >
struct tuple< Elem0, Elems... > : tuple< Elems... >
{
        constexpr tuple () = default ;

        template< typename T, typename... Ts >
        explicit constexpr tuple ( T&& elem0, Ts&&... rest )
                : tuple< Elems... >( UTI_FWD( rest )... )
                , data( UTI_FWD( elem0 ) ) {}

        Elem0 data ;
} ;

template< typename T, typename... Ts >
tuple ( T elem0, Ts... rest ) -> tuple< unwrap_ref_decay_t< T >, unwrap_ref_decay_t< Ts >... > ;

template< typename... Elems >
constexpr auto make_tuple ( Elems&&... elems )
{
        return tuple< unwrap_ref_decay_t< Elems >... >{ UTI_FWD( elems )... } ;
}

////////////////////////////////////////////////////////////////////////////////
/// get

namespace _detail
{


template< ssize_t Idx, typename Tup >
struct get_impl : get_impl< Idx - 1, meta::list::pop_front_t< Tup > > {} ;

template< typename Tup >
struct get_impl< 0, Tup >
{
        template< typename T >
        static constexpr decltype( auto ) get ( T&& t )
        {
                constexpr bool is_lvalue = is_lvalue_reference_v< T > ;
                constexpr bool is_const  = is_const_v< remove_reference_t< T > > ;

                using data_t = meta::list::front_t< Tup > ;

                if constexpr( is_lvalue && is_const )
                {
                        return static_cast< data_t const &  >( static_cast< Tup const &  >( t ).data ) ;
                }
                if constexpr( is_lvalue && !is_const )
                {
                        return static_cast< data_t       &  >( static_cast< Tup       &  >( t ).data ) ;
                }
                if constexpr( !is_lvalue && is_const )
                {
                        return static_cast< data_t const && >( static_cast< Tup const && >( t ).data ) ;
                }
                if constexpr( !is_lvalue && !is_const )
                {
                        return static_cast< data_t       && >( static_cast< Tup       && >( t ).data ) ;
                }
        }
} ;


} // namespace _detail


template< ssize_t Idx, typename Tup >
constexpr decltype( auto ) get ( Tup&& tup )
{
        return _detail::get_impl< Idx, remove_cvref_t< Tup > >::get( UTI_FWD( tup ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// tuple_size

template< typename Tup >
struct tuple_size ;

template< typename... Elems >
struct tuple_size< tuple< Elems... > > : integral_constant< sizeof...( Elems ) > {} ;

template< typename Tup >
static constexpr ssize_t tuple_size_v = tuple_size< Tup >::value ;


template< typename Tup >
using tuple_index_sequence = make_index_sequence< tuple_size_v< remove_cvref_t< Tup > > > ;

////////////////////////////////////////////////////////////////////////////////
/// forward_as_tuple

template< typename... Ts >
constexpr auto forward_as_tuple ( Ts&&... ts ) noexcept
{
        return tuple< Ts&&... >{ UTI_FWD( ts )... } ;
}

////////////////////////////////////////////////////////////////////////////////
/// tuple_element_t

template< typename Tup, ssize_t Idx >
using tuple_element_t = meta::list::at_t< Idx, Tup > ;

////////////////////////////////////////////////////////////////////////////////
/// tuple_cat

namespace _detail
{


template< typename... Tups >
struct tuple_cat_result ;

template< typename Tup >
struct tuple_cat_result< Tup > : type_identity< Tup > {} ;

template< template< typename... > typename Tup, typename... Elems1, typename... Elems2, typename... Tups >
struct tuple_cat_result< Tup< Elems1... >, Tup< Elems2... >, Tups... >
        : tuple_cat_result< Tup< Elems1..., Elems2... >, Tups... > {} ;

template< typename... Tups >
using tuple_cat_result_t = typename tuple_cat_result< Tups... >::type ;


template< typename ResultTup, typename IdxSeq >
struct make_tup_from_fwd_tup ;

template< typename ResultTup, ssize_t... Idxs >
struct make_tup_from_fwd_tup< ResultTup, index_sequence< Idxs... > >
{
        template< typename FwdTup >
        static constexpr auto fn ( FwdTup&& fwd )
        {
                return ResultTup{ get< Idxs >( UTI_FWD( fwd ) )... } ;
        }
} ;


template< typename FwdIdxSeq, typename TupIdxSeq >
struct concat_with_fwd_tup ;

template< ssize_t... FwdIdxs, ssize_t... Idxs >
struct concat_with_fwd_tup< index_sequence< FwdIdxs... >, index_sequence< Idxs... > >
{
        template< typename FwdTup, typename Tup >
        static constexpr auto fn ( FwdTup&& fwd, Tup&& tup )
        {
                return forward_as_tuple( get< FwdIdxs >( UTI_FWD( fwd ) )..., get< Idxs >( UTI_FWD( tup ) )... ) ;
        }
} ;


template< typename ResultTup >
struct tuple_cat_impl
{
        template< typename FwdTup, typename Tup, typename... Tups >
        static constexpr auto fn ( FwdTup&& fwd, Tup&& tup, Tups&&... tups )
        {
                return fn( concat_with_fwd_tup<
                                tuple_index_sequence< FwdTup > ,
                                tuple_index_sequence<    Tup > >::fn(
                                        UTI_FWD( fwd ),
                                        UTI_FWD( tup ) ),
                                UTI_FWD( tups )...
                ) ;
        }
        template< typename FwdTup >
        static constexpr auto fn ( FwdTup&& ret )
        {
                return make_tup_from_fwd_tup< ResultTup, tuple_index_sequence< FwdTup > >::fn(
                                UTI_FWD( ret )
                ) ;
        }
} ;


} // namespace _detail


template< typename... Tups >
constexpr auto tuple_cat ( Tups&&... tups )
{
        return _detail::tuple_cat_impl< _detail::tuple_cat_result_t< remove_cvref_t< Tups >... > >::fn(
                        UTI_FWD( tups )...
        ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// apply

template< typename Tup, typename Fn >
constexpr auto apply ( Tup&& tup, Fn&& fn )
{
        return [ & ]< ssize_t... Idxs >( index_sequence< Idxs... > )
        {
                return invoke( UTI_FWD( fn ), UTI_FWD( get< Idxs >( UTI_FWD( tup ) ) )... ) ;
        }( tuple_index_sequence< Tup >{} ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// for_each

template< typename Tup, typename Fn, typename... Args >
constexpr void for_each ( Tup&& tup, Fn&& fn, Args&&... args )
{
        apply( tup, [ & ]( auto&... elems ){ ( ..., UTI_FWD( fn )( elems, args... ) ) ; } ) ;
}

template< typename Tup1, typename Tup2, typename Fn >
constexpr void for_each_pair ( Tup1&& tup1, Tup2&& tup2, Fn&& fn )
{
        return [ & ]< ssize_t... Idxs >( index_sequence< Idxs... > )
        {
                ( ..., invoke( UTI_FWD( fn ) ,
                               UTI_FWD( get< Idxs >( UTI_FWD( tup1 ) ) ) ,
                               UTI_FWD( get< Idxs >( UTI_FWD( tup2 ) ) ) )
                ) ;
        }( tuple_index_sequence< Tup1 >{} ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// transform

template< typename Tup, typename Fn >
constexpr auto transform ( Tup&& tup, Fn&& fn )
{
        return [ & ]< ssize_t... Idxs >( index_sequence< Idxs... > )
        {
                return tuple{ UTI_FWD( fn )( UTI_FWD( get< Idxs >( UTI_FWD( tup ) ) ) )... } ;
        }( tuple_index_sequence< Tup >{} ) ;
}

template< typename Tup1, typename Tup2, typename Fn >
constexpr auto transform_pair ( Tup1&& tup1, Tup2&& tup2, Fn&& fn )
{
        return [ & ]< ssize_t... Idxs >( index_sequence< Idxs... > )
        {
                return tuple{ UTI_FWD( fn )( UTI_FWD( get< Idxs >( UTI_FWD( tup1 ) ) ),
                                             UTI_FWD( get< Idxs >( UTI_FWD( tup2 ) ) ) )... } ;
        }( tuple_index_sequence< Tup1 >{} ) ;
}

template< typename Tup, typename Fn >
constexpr auto reverse_transform ( Tup&& tup, Fn&& fn )
{
        static constexpr auto N { tuple_size_v< remove_cvref_t< Tup > > } ;

        return [ & ]< ssize_t... Idxs >( index_sequence< Idxs... > )
        {
                return tuple{ UTI_FWD( fn )( UTI_FWD( get< N - Idxs >( UTI_FWD( tup ) ) ) )... } ;
        }( tuple_index_sequence< Tup >{} ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// filter

namespace _detail
{


template< typename ResultTup, typename Tup, ssize_t... Idxs >
constexpr auto cat_tup_content_as ( Tup&& tup, index_sequence< Idxs... > )
{
        return tuple_cat_impl< ResultTup >::fn( get< Idxs >( UTI_FWD( tup ) )... ) ;
}

template< template< typename... > typename Predicate, typename Tup, ssize_t... Idxs >
constexpr auto wrap_filtered ( Tup&& tup, index_sequence< Idxs... > )
{
        auto wrap_if_match = [ & ]( auto idx )
        {
                if constexpr( Predicate< tuple_element_t< remove_cvref_t< Tup >, idx.value > >::value )
                {
                        return forward_as_tuple( get< idx.value >( UTI_FWD( tup ) ) ) ;
                }
                else
                {
                        return tuple<>{} ;
                }
        } ;
        return tuple{ wrap_if_match( integral_constant< Idxs >{} )... } ;
}

template< typename Tup, template< typename... > typename Predicate >
struct filter_result ;

template< template< typename... > typename Tup, typename... Elems, template< typename... > typename Predicate >
struct filter_result< Tup< Elems... >, Predicate >
{
        template< typename Elem >
        struct wrap_if_match
        {
                using type = conditional_t< Predicate< Elem >::value, Tup< Elem >, Tup<> > ;
        } ;
        using type = tuple_cat_result_t< typename wrap_if_match< Elems >::type... > ;
} ;


template< typename Tup, template< typename... > typename Predicate >
using filter_result_t = typename filter_result< Tup, Predicate >::type ;


} // namespace _detail


template< template< typename... > typename Predicate, typename Tup >
constexpr auto filter ( Tup&& tup )
{
        auto wrapped_tup = _detail::wrap_filtered< Predicate >( UTI_FWD( tup ), tuple_index_sequence< Tup >{} ) ;

        return _detail::cat_tup_content_as< _detail::filter_result_t< remove_cvref_t< Tup >, Predicate > >(
                        UTI_MOVE( wrapped_tup ),
                        tuple_index_sequence< Tup >{} ) ;
}


} // namespace uti

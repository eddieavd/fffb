//
//
//      uti
//      core/meta/algo.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/meta/join.hxx>


namespace uti::meta
{


namespace _detail
{


template< typename Predicate, typename Continuation = identity >
struct wrap_if_not
{
        template< typename T >
        using fn = typename Continuation::template fn<
                        conditional_t< !Predicate::template value< T >, type_list< T >, type_list<> > > ;
} ;


template< typename Continuation, typename List >
struct unpack_impl ;

template< typename Continuation, template< typename... > typename List, typename... Elems >
struct unpack_impl< Continuation, List< Elems... > >
{
        using type = typename Continuation::template fn< Elems... > ;
} ;


} // namespace _detail


template< typename Continuation >
struct unpack
{
        template< typename List >
        using fn = typename _detail::unpack_impl< Continuation, List >::type ;
} ;

template< typename Continuation, typename List >
using unpack_t = typename unpack< Continuation >::template fn< List > ;


template< template< typename... > typename Predicate >
struct to_lazy_predicate
{
        template< typename... Ts >
        static constexpr bool value = Predicate< Ts... >::value ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// transform

template< typename Fn, typename Continuation = listify >
struct transform
{
        template< typename... Ts >
        using fn = typename Continuation::template fn< typename Fn::template fn< Ts >... > ;
} ;

template< typename Fn, typename Continuation, typename... Ts >
using transform_t = typename transform< Fn, Continuation >::template fn< Ts... > ;


struct to_void
{
        template< typename T >
        using fn = void ;
} ;

struct to_const
{
        template< typename T >
        using fn = T const ;
} ;

struct to_volatile
{
        template< typename T >
        using fn = T volatile ;
} ;


static_assert( is_same_v< type_list< void, void, void >, transform_t< to_void, listify, int, bool, float > > ) ;

////////////////////////////////////////////////////////////////////////////////
/// remove_if

template< typename Predicate, template< typename... > typename TypeList = type_list, typename Continuation = listify >
struct remove_if
{
        template< typename... Ts >
        using fn = typename transform< _detail::wrap_if_not< Predicate >, join< TypeList, Continuation > >::template fn< Ts... > ;
} ;

template< typename Predicate, template< typename... > typename TypeList = type_list, typename Continuation = listify, typename... Ts >
using remove_if_t = typename remove_if< Predicate, TypeList, Continuation >::template fn< Ts... > ;


namespace test
{


using lazy_is_float = to_lazy_predicate< is_floating_point > ;

using input = type_list< int, float, char > ;

using new_list = unpack_t< remove_if< lazy_is_float >, input > ;

static_assert( is_same_v< new_list, type_list< int, char > > ) ;


} // namespace test


} // namespace uti::meta

//
//
//      uti
//      core/type/invoke.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>


namespace uti
{


template< typename T > class reference_wrapper ;


namespace _detail
{


template< typename T > struct is_reference_wrapper                           : false_type {} ;
template< typename T > struct is_reference_wrapper< reference_wrapper< T > > :  true_type {} ;

template< typename T >
static constexpr auto is_reference_wrapper_v = is_reference_wrapper< T >::value ;

template< typename T >
struct invoke_impl
{
        template< typename Fn, typename... Args >
        static auto call ( Fn&& fn, Args&&... args )
                -> decltype( UTI_FWD( fn )( UTI_FWD( args )... ) ) ;
} ;

template< typename B, typename MT >
struct invoke_impl< MT B::* >
{
        template< typename T, typename Td = decay_t< T > >
                requires( is_base_of_v< B, Td > )
        static auto get ( T&& t ) -> T&& ;

        template< typename T, typename Td = decay_t< T > >
                requires( is_reference_wrapper_v< Td > )
        static auto get ( T& t ) -> decltype( t.get() ) ;

        template< typename T, typename Td = decay_t< T > >
                requires( !is_base_of_v< B, Td > && !is_reference_wrapper_v< Td > )
        static auto get ( T&& t ) -> decltype( *UTI_FWD( t ) ) ;

        template< typename T, typename... Args, typename MT1 >
                requires( is_function_v< MT1 > )
        static auto call ( MT1 B::*pmf, T&& t, Args&&... args )
                -> decltype( ( invoke_impl::get( UTI_FWD( t ) ).*pmf )( UTI_FWD( args )... ) ) ;

        template< typename T >
        static auto call ( MT B::*pmd, T&& t )
                -> decltype( invoke_impl::get( UTI_FWD( t ) ).*pmd ) ;
} ;

template< typename Fn, typename... Args, typename Fd = decay_t< Fn > >
auto _invoke ( Fn&& fn, Args&&... args )
        -> decltype( invoke_impl< Fd >::call( UTI_FWD( fn ), UTI_FWD( args )... ) ) ;

template< typename AlwaysVoid, typename, typename... >
struct invoke_result {} ;

template< typename Fn, typename... Args >
struct invoke_result< decltype( void ( _invoke( declval< Fn >(), declval< Args >()... ) ) ), Fn, Args... >
{
        using type = decltype( _invoke( declval< Fn >(), declval< Args >()... ) ) ;
} ;


} // namespace _detail


template< typename > struct result_of ;

template< typename Fn, typename... Args >
struct result_of< Fn( Args... ) > : _detail::invoke_result< void, Fn, Args... > {} ;

template< typename Fn, typename... Args >
struct invoke_result : _detail::invoke_result< void, Fn, Args... > {} ;

template< typename Fn, typename... Args >
using invoke_result_t = typename invoke_result< Fn, Args... >::type ;


namespace _detail
{


template< typename C, typename Pointed, typename Object, typename... Args >
constexpr decltype( auto ) invoke_memptr ( Pointed C::* member, Object&& object, Args&&... args )
{
        using object_t = remove_cvref_t< Object > ;

        constexpr bool is_member_fn = is_function_v< Pointed > ;
        constexpr bool is_wrapped = is_reference_wrapper_v< object_t > ;
        constexpr bool is_derived_obj = is_same_v< C, object_t > || is_base_of_v< C, object_t > ;

        if constexpr( is_member_fn )
        {
                if constexpr( is_derived_obj )
                {
                        return ( UTI_FWD( object ) .* member )( UTI_FWD( args )... ) ;
                }
                else if constexpr( is_wrapped )
                {
                        return ( object.get() .* member )( UTI_FWD( args )... ) ;
                }
                else
                {
                        return ( ( *UTI_FWD( object ) ) .* member )( UTI_FWD( args )... ) ;
                }
        }
        else
        {
                static_assert( is_object_v< Pointed > && sizeof...( args ) == 0 ) ;

                if constexpr( is_derived_obj )
                {
                        return UTI_FWD( object ) .* member ;
                }
                else if constexpr( is_wrapped )
                {
                        return object.get() .* member ;
                }
                else
                {
                        return ( *UTI_FWD( object ) ) .* member ;
                }
        }
}


} // namespace _detail


template< typename Fn, typename... Args >
constexpr invoke_result_t< Fn, Args... > invoke ( Fn&& fn, Args&&... args )
//      noexcept( is_nothrow_invocable_v< Fn, Args... > )
{
        if constexpr( is_member_pointer_v< remove_cvref_t< Fn > > )
        {
                return _detail::invoke_memptr( fn, UTI_FWD( args )... ) ;
        }
        else
        {
                return UTI_FWD( fn )( UTI_FWD( args )... ) ;
        }
}

template< typename R, typename Fn, typename... Args >
//      requires is_invocable_r_v< R, Fn, Args... >
constexpr R invoke_r ( Fn&& fn, Args&&... args )
//      noexcept( is_nothrow_invocable_r_v< R, Fn, Args... > )
{
        if constexpr( is_void_v< R > )
        {
                invoke( UTI_FWD( fn ), UTI_FWD( args )... ) ;
        }
        else
        {
                return invoke( UTI_FWD( fn ), UTI_FWD( args )... ) ;
        }
}


} // namespace uti

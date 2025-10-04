//
//
//      uti
//      core/type/common_type.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP

#include <uti/core/type/traits.hxx>


namespace uti
{


template< typename T, typename U >
using _cond_type = decltype( false ? uti::declval< T >() : uti::declval< U >() ) ;

template< typename T, typename U, typename = void >
struct _common_type_3 {} ;

template< typename T, typename U >
struct _common_type_3< T, U, void_t< _cond_type< T const &, U const & > > >
        : type_identity< remove_cvref_t< _cond_type< T const &, U const & > > > {} ;

template< typename T, typename U, typename = void >
struct _common_type_2_impl : _common_type_3< T, U > {} ;

template< typename T, typename U >
struct _common_type_2_impl< T, U, void_t< decltype( true ? uti::declval< T >() : uti::declval< U >() ) > >
        : type_identity< decay_t< decltype( true ? uti::declval< T >() : uti::declval< U >() ) > > {} ;

template< typename, typename = void >
struct _common_type_impl {} ;

template< typename... Ts >
struct _common_types ;
template< typename... Ts >
struct common_type ;

template< typename T, typename U >
struct _common_type_impl< _common_types< T, U >, void_t< typename common_type< T, U >::type > >
        : type_identity< typename common_type< T, U >::type > {} ;

template< typename T, typename U, typename V, typename... Rest >
struct _common_type_impl< _common_types< T, U, V, Rest... >, void_t< typename common_type< T, U >::type > >
        : _common_type_impl< _common_types< typename common_type< T, U >::type, V, Rest... > > {} ;

template<>
struct common_type<> {} ;

template< typename T >
struct common_type< T > : public common_type< T, T > {} ;

template< typename T, typename U >
struct common_type< T, U >
        : conditional_t
        <
                is_same_v< T, decay_t< T > > && is_same_v< U, decay_t< U > >,
                _common_type_2_impl< T, U >,
                common_type< decay_t< T >, decay_t< U > >
        > {} ;

template< typename T, typename U, typename V, typename... Rest >
struct common_type< T, U, V, Rest... >
        : _common_type_impl< _common_types< T, U, V, Rest... > > {} ;

template< typename... Ts >
using common_type_t = typename common_type< Ts... >::type ;


} // namespace uti

#endif

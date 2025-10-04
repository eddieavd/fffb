//
//
//      uti
//      core/type/common_ref.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP

#include <uti/core/type/traits.hxx>
#include <uti/core/type/common_type.hxx>


namespace uti
{


/*
 * let `COND_RES( A, B )` be:
 *
 */
template< typename A, typename B >
using _cond_res = decltype( false ? uti::declval< A( & )() >()() : uti::declval< B( & )() >()() ) ;

/*
 * let `XREF( T )` denote a unary alias template `TMP` such that `TMP< U >` denotes the same type as `U`
 * with the addition of `T`'s cv and reference qualifiers, for a non-reference cv-unqualified type `U`
 *
 */
template< typename T >
struct _xref
{
        template< typename U >
        using _apply = copy_cvref_t< T, U > ;
};

/*
 * given types `A` and `B`, let `X` be `remove_reference_t< A >`, let `Y` be `remove_reference_t< B >`,
 * and let COMMON_REF( A, B ) be:
 *
 */
template< typename A, typename B, typename X = remove_reference_t< A >, typename Y = remove_reference_t< B > >
struct _common_ref ;

template< typename X, typename Y >
using _common_ref_t = typename _common_ref< X, Y >::type ;

template< typename X, typename Y >
using _cv_cond_res = _cond_res< copy_cv_t< X, Y > &, copy_cv_t< Y, X > & > ;

/*
 * if T and U are both lvalue reference types, common_ref< T, U > is
 * cond_res< copy_cv< X, Y >&, copy_cv< Y, X >& > if that type exists and is a reference type
 *
 */
template< typename A, typename B, typename X, typename Y >
        requires requires { typename _cv_cond_res< X, Y > ; } &&
                                is_reference_v< _cv_cond_res< X, Y > >
struct _common_ref< A&, B&, X, Y > : type_identity< _cv_cond_res< X, Y > > {} ;

/*
 * otherwise, let C be remove_reference_t< COMMON_REF( X&, Y& )> &&
 *
 */
template< typename X, typename Y >
using _common_ref_C = remove_reference_t< _common_ref_t< X&, Y& > > && ;

/*
 * if A and B are both rvalue reference types, C is well-formed, and
 * is_convertible_v< A, C > && is_convertible_v< B, C > is true, then COMMON_REF( A, B ) is C
 *
 */
template< typename A, typename B, typename X, typename Y >
        requires requires { typename _common_ref_C< X, Y > ; } &&
                                is_convertible_v< A&&, _common_ref_C< X, Y > > &&
                                is_convertible_v< B&&, _common_ref_C< X, Y > >
struct _common_ref< A&&, B&&, X, Y >
        : type_identity< _common_ref_C< X, Y > > {} ;

/*
 * otherwise let D be COMMON_REF( X const &, Y & )
 *
 */
template< typename T, typename U >
using _common_ref_D = _common_ref_t< T const &, U & > ;

/*
 * if A is an rvalue reference and B is an lvalue reference and D is well-formed and
 * is_convertible_v< A, D > is true, then COMMON_REF( A, B ) is D
 *
 */
template< typename A, typename B, typename X, typename Y >
        requires requires { typename _common_ref_D< X, Y > ; } &&
                                is_convertible_v< A&&, _common_ref_D< X, Y > >
struct _common_ref< A&&, B&, X, Y > : type_identity< _common_ref_D< X, Y > > {} ;

/*
 * otherwise, if A is an lvalue reference and B is an rvalue reference, then
 * COMMON_REF( A, B ) is COMMON_REF( B, A )
 *
 */
template< typename A, typename B, typename X, typename Y >
struct _common_ref< A&, B&&, X, Y > : _common_ref< B&&, A& > {} ;

/*
 * otherwise, COMMON_REF( A, B ) is ill-formed
 *
 */
template< typename A, typename B, typename X, typename Y >
struct _common_ref {} ;

/// common_reference applied to a parameter pack

template< typename... >
struct common_reference ;

template< typename... Ts >
using common_reference_t = typename common_reference< Ts... >::type ;

/// check 1: sizeof...( Ts ) == 0

template<>
struct common_reference<> {} ;

/// check 2: sizeof...( Ts ) == 1

template< typename T >
struct common_reference< T > : type_identity< T > {} ;

/// check 3: sizeof...( Ts ) == 2

template< typename T, typename U > struct _common_reference_subcheck_3 ;
template< typename T, typename U > struct _common_reference_subcheck_2 : _common_reference_subcheck_3< T, U > {} ;
template< typename T, typename U > struct _common_reference_subcheck_1 : _common_reference_subcheck_2< T, U > {} ;

/// subcheck 1: T and U are reference types and COMMON_REF( T, U ) is well-formed,
///             then member typedef `type` denotes that type
template< typename T, typename U >
struct common_reference< T, U > : _common_reference_subcheck_1< T, U > {} ;

template< typename T, typename U >
        requires is_reference_v< T > && is_reference_v< U > && requires { typename _common_ref_t< T, U > ; }
struct _common_reference_subcheck_1< T, U > : type_identity< _common_ref_t< T, U > > {} ;

/// subcheck 2: otherwise, if basic_common_reference< remove_cvref_t< T >, remove_cvref_t< U >, XREF( T ), XREF( U ) >::type
///             is well-formed, then the member typedef `type` denotes that type
template< typename, typename, template< typename > typename, template< typename > typename >
struct basic_common_reference {} ;

template< typename T, typename U >
using _basic_common_reference_t =
        typename basic_common_reference< remove_cvref_t< T >,
                                         remove_cvref_t< U >,
                                         _xref< T >::template _apply,
                                         _xref< U >::template _apply >::type ;

template< typename T, typename U >
        requires requires { typename _basic_common_reference_t< T, U > ; }
struct _common_reference_subcheck_2< T, U > : type_identity< _basic_common_reference_t< T, U > > {} ;

/// subcheck 3: otherwise, if COND_REF( T, U ) is well-formed,
///             then the member typedef `type` denotes that type
template< typename T, typename U >
        requires requires { typename _cond_res< T, U > ; }
struct _common_reference_subcheck_3< T, U > : type_identity< _cond_res< T, U > > {} ;

/// subcheck 4&5: otherwise, if common_type_t< T, U > is well-formed,
//                then the member typedef `type` denotes that type
//                otherwise, there shall be no member `type`
template< typename T, typename U >
struct _common_reference_subcheck_3 : common_type< T, U > {} ;

/// check 4: if there is such a type `C`, the member typedef `type` shall denote the same type,
///          if any, as `common_reference_t< C, Rest... >`
template< typename T, typename U, typename V, typename... Rest >
        requires requires { typename common_reference_t< T, U > ; }
struct common_reference< T, U, V, Rest... > : common_reference< common_reference_t< T, U >, V, Rest... > {} ;

/// check 5: otherwise, there shall be no member `type`
template< typename... >
struct common_reference {} ;


} // namespace uti

#endif // UTI_DOXYGEN_SKIP

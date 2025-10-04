//
//
//      uti
//      core/meta/concepts.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/type/common_type.hxx>
#include <uti/core/type/common_ref.hxx>
#include <uti/core/type/invoke.hxx>
#include <uti/core/algo/swap.hxx>


namespace uti::meta
{


template< typename, ssize_t > concept any = true ;


template< ssize_t > struct index
{
        ssize_t idx ;

        template< ssize_t Idx_ >
        constexpr index ( index< Idx_ > _other_ ) : idx( _other_.idx  ) {}
        constexpr index ( ssize_t         _idx_ ) : idx(        _idx_ ) {}

        constexpr operator ssize_t () const noexcept { return idx ; }
};


template< typename T, typename U >
concept same_as = is_same_v< T, U > && is_same_v< U, T > ;

template< typename T, typename... Args >
concept one_of = is_one_of_v< T, Args... > ;

template< typename Derived, typename Base >
concept derived_from =
        is_base_of_v< Base, Derived > &&
        is_convertible_v< Derived const volatile *, Base const volatile * > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T, template< typename... > typename U >
concept instantiated_from = is_instance_of_v< T, U > ;

////////////////////////////////////////////////////////////////////////////////

template< typename From, typename To >
concept convertible_to =
        is_convertible_v< From, To > &&
        requires
        {
                static_cast< To >( uti::declval< From >() ) ;
        };

template< typename From, typename To >
concept nothrow_convertible_to =
        is_nothrow_convertible_v< From, To > &&
        requires
        {
                static_cast< To >( uti::declval< From >() ) ;
        };

////////////////////////////////////////////////////////////////////////////////

template< typename T, typename U >
concept common_reference_with =
        same_as< common_reference_t< T, U >,
                 common_reference_t< U, T >
        > &&
        convertible_to< T, common_reference_t< T, U > > &&
        convertible_to< U, common_reference_t< T, U > > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T, typename U >
concept common_with =
        same_as< common_type_t< T, U >,
                 common_type_t< U, T >
        > &&
        requires
        {
                static_cast< common_type_t< T, U > >( uti::declval< T >() ) ;
                static_cast< common_type_t< T, U > >( uti::declval< U >() ) ;
        } &&
        common_reference_with
        <
                add_lvalue_reference_t< T const >,
                add_lvalue_reference_t< U const >
        > &&
        common_reference_with
        <
                add_lvalue_reference_t< common_type_t< T, U > >,
                common_reference_t
                <
                        add_lvalue_reference_t< T const >,
                        add_lvalue_reference_t< U const >
                >
        > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept integral = is_integral_v< T > ;

template< typename T >
concept signed_integral = integral< T > && is_signed_v< T > ;

template< typename T >
concept unsigned_integral = integral< T > && !signed_integral< T > ;

template< typename T >
concept floating_point = is_floating_point_v< T > ;

template< typename T >
concept arithmetic = is_arithmetic_v< T > ;

template< typename T >
concept trivial = is_trivial_v< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename Lhs, typename Rhs >
concept assignable_from =
        is_lvalue_reference_v< Lhs > &&
        common_reference_with
        <
                remove_reference_t< Lhs > const &,
                remove_reference_t< Rhs > const &
        > &&
        requires( Lhs lhs, Rhs&& rhs )
        {
                { lhs = UTI_FWD( rhs ) } -> same_as< Lhs > ;
        } ;

template< typename Lhs, typename Rhs >
concept nothrow_assignable_from = assignable_from< Lhs, Rhs > && nothrow_assign< Lhs, Rhs >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept swappable =
        requires( T & t, T & u )
        {
                ::uti::swap( t, u ) ;
        };

template< typename T, typename U >
concept swappable_with =
        common_reference_with< T, U > &&
        requires( T && t, U && u )
        {
                ::uti::swap( UTI_FWD( t ), UTI_FWD( t ) ) ;
                ::uti::swap( UTI_FWD( t ), UTI_FWD( u ) ) ;
                ::uti::swap( UTI_FWD( u ), UTI_FWD( t ) ) ;
                ::uti::swap( UTI_FWD( u ), UTI_FWD( u ) ) ;
        };

template< typename T >
concept nothrow_swappable = swappable< T > && noexcept( ::uti::swap( uti::declval< T >(), uti::declval< T >() ) ) ;

////////////////////////////////////////////////////////////////////////////////

// Unlike the Destructible named requirement, the concept destructible requires the destructor to be noexcept(true),
// not merely non-throwing when invoked, and allows reference types and array types.
// - https://en.cppreference.com/w/cpp/concepts/destructible
template< typename T >
concept destructible = is_nothrow_destructible_v< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T, typename... Args >
concept constructible_from = destructible< T > && is_constructible_v< T, Args... > ;

template< typename T, typename... Args >
concept nothrow_constructible_from = constructible_from< T, Args... > && nothrow_construct< T, Args... >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept default_initializable = constructible_from< T > && requires { T{}; } && requires { ::new T; } ;

template< typename T >
concept nothrow_default_initializable = default_initializable< T > && nothrow_construct< T >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept move_constructible = constructible_from< T, T > && convertible_to< T, T > ;

template< typename T >
concept nothrow_move_constructible = move_constructible< T > && nothrow_construct< T, add_rvalue_reference_t< T > >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept copy_constructible =
        move_constructible< T > &&
        constructible_from< T, T       & > && convertible_to< T       &, T > &&
        constructible_from< T, T const & > && convertible_to< T const &, T > &&
        constructible_from< T, T const   > && convertible_to< T const  , T >  ;

template< typename T >
concept nothrow_copy_constructible = copy_constructible< T > && nothrow_construct< T, add_lvalue_reference_t< add_const_t< T > > >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename B >
concept _boolean_testable_impl = convertible_to< B, bool > ;

template< typename B >
concept boolean_testable =
        _boolean_testable_impl< B > &&
        requires( B && b )
        {
                { !UTI_FWD( b ) } -> _boolean_testable_impl ;
        };

////////////////////////////////////////////////////////////////////////////////

template< typename T, typename U >
concept _weakly_eq_comparable_with =
        requires( remove_reference_t< T > const & t ,
                  remove_reference_t< U > const & u )
        {
                { t == u } -> boolean_testable ;
                { t != u } -> boolean_testable ;
                { u == t } -> boolean_testable ;
                { u != t } -> boolean_testable ;
        };

template< typename T, typename U, typename C = common_reference_t< T const &, U const & > >
concept _comparison_common_type_with_impl =
        same_as< common_reference_t< T const &, U const & > ,
                 common_reference_t< U const &, T const & > > &&
        requires
        {
                requires convertible_to< T const &, C const & > ||
                         convertible_to< T        , C const & >  ;
                requires convertible_to< U const &, C const & > ||
                         convertible_to< U        , C const & >  ;
        };

template< typename T, typename U >
concept _comparison_common_type_with =
        _comparison_common_type_with_impl< remove_cvref_t< T >, remove_cvref_t< U > > ;

template< typename T >
concept equality_comparable = _weakly_eq_comparable_with< T, T > ;

template< typename T, typename U >
concept equality_comparable_with =
        equality_comparable< T > &&
        equality_comparable< U > &&
        _comparison_common_type_with< T, U > &&
        equality_comparable<
                common_reference_t<
                        remove_reference_t< T > const &,
                        remove_reference_t< U > const &
                >
        > &&
        _weakly_eq_comparable_with< T, U > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept totally_ordered =
        equality_comparable< T > &&
        requires( remove_reference_t< T > const & a ,
                  remove_reference_t< T > const & b )
        {
                { a <  b } -> boolean_testable ;
                { a >  b } -> boolean_testable ;
                { a <= b } -> boolean_testable ;
                { a >= b } -> boolean_testable ;
        };

template< typename T, typename U >
concept totally_ordered_with =
        totally_ordered< T > &&
        totally_ordered< U > &&
        common_reference_with<
                remove_reference_t< T > const &,
                remove_reference_t< U > const &
        > &&
        totally_ordered<
                common_reference_t<
                        remove_reference_t< T > const &,
                        remove_reference_t< U > const &
                >
        > &&
        equality_comparable_with< T, U > &&
        requires( remove_reference_t< T > const & t ,
                  remove_reference_t< U > const & u )
        {
                { t <  u } -> boolean_testable ;
                { t >  u } -> boolean_testable ;
                { t <= u } -> boolean_testable ;
                { t >= u } -> boolean_testable ;
                { u <  t } -> boolean_testable ;
                { u >  t } -> boolean_testable ;
                { u <= t } -> boolean_testable ;
                { u >= t } -> boolean_testable ;
        };

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept movable =
        is_object_v< T >
     && move_constructible< T >
     && assignable_from< T &, T >
     && swappable< T > ;

template< typename T >
concept nothrow_movable =
        is_object_v< T >
     && nothrow_move_constructible< T >
     && nothrow_assignable_from< T &, T >
     && nothrow_swappable< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept copyable =
        copy_constructible< T >
     && movable< T >
     && assignable_from< T &, T       & >
     && assignable_from< T &, T const & >
     && assignable_from< T &, T const   > ;

template< typename T >
concept nothrow_copyable =
        nothrow_copy_constructible< T >
     && nothrow_movable< T >
     && nothrow_assignable_from< T &, T       & >
     && nothrow_assignable_from< T &, T const & >
     && nothrow_assignable_from< T &, T const   > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept semiregular = copyable< T > && default_initializable< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept regular = semiregular< T > && equality_comparable< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename Fn, typename... Args >
concept invocable =
        requires( Fn && fn, Args&&... args )
        {
                ::uti::invoke( UTI_FWD( fn ), UTI_FWD( args )... );
        };

template< typename Fn, typename... Args >
concept regular_invocable = invocable< Fn, Args... > ;

////////////////////////////////////////////////////////////////////////////////

template< typename Fn, typename... Args >
concept predicate =
        regular_invocable< Fn, Args... > &&
        boolean_testable< ::uti::invoke_result_t< Fn, Args... > > ;

////////////////////////////////////////////////////////////////////////////////

/*
 * the concept relation< R, T, U > specifies that R defines a binary relation
 * over the set of expressions whose type and value category are those encoded by either T or U
 *
 */
template< typename R, typename T, typename U >
concept relation =
        predicate< R, T, T > &&
        predicate< R, U, U > &&
        predicate< R, T, U > &&
        predicate< R, U, T >  ;

/*
 * specifies that the relation R imposes an equivalence relation on its arguments
 * ( purely semantic )
 *
 */
template< typename R, typename T, typename U >
concept equivalence_relation = relation< R, T, U > ;

/*
 * specifies that the relation R imposes a strict weak ordering on its arguments
 * ( purely semantic )
 *
 */
template< typename R, typename T, typename U >
concept strict_weak_order = relation< R, T, U > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept container =
        requires( T container )
        {
                  typename T::     value_type ;
                  typename T::      size_type ;
                  typename T::     ssize_type ;
                  typename T::        pointer ;
                  typename T::      reference ;
                  typename T::const_reference ;
                  typename T::       iterator ;
                  typename T:: const_iterator ;
                { container. begin() } -> convertible_to< typename T::      iterator > ;
                { container.   end() } -> convertible_to< typename T::      iterator > ;
                { container. begin() } -> convertible_to< typename T::const_iterator > ;
                { container.   end() } -> convertible_to< typename T::const_iterator > ;
                { container.cbegin() } -> convertible_to< typename T::const_iterator > ;
                { container.  cend() } -> convertible_to< typename T::const_iterator > ;
                { container.  size() } -> one_of < typename T:: size_type
                                                 , typename T::ssize_type
                                                 >                              ;
        };

template< typename T >
concept range_container =
        requires( T c )
        {
                container< T > ;
                { c.range( ssize_t(), ssize_t() ) } -> convertible_to< typename T::value_type > ;
        };

template< typename T >
concept two_d_container =
        requires
        {
                container<          T             > ;
                container< typename T::value_type > ;
        };

template< typename T >
concept two_d_range_container =
        requires
        {
                two_d_container<          T             > ;
                range_container<          T             > ;
                range_container< typename T::value_type > ;
        };

template< typename T >
concept three_d_container =
        requires
        {
                two_d_container<          T             > ;
                two_d_container< typename T::value_type > ;
        };

template< typename T >
concept three_d_range_container =
        requires
        {
                two_d_range_container<          T             > ;
                two_d_range_container< typename T::value_type > ;
        };


} // namespace uti::meta

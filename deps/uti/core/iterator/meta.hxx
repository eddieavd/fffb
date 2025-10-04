//
//
//      uti
//      core/iterator/meta.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/meta/concepts.hxx>


namespace uti
{

////////////////////////////////////////////////////////////////////////////////
///     readable_traits
////////////////////////////////////////////////////////////////////////////////

template< typename > struct _cond_value_type {} ;

template< typename T >
        requires is_object_v< T >
struct _cond_value_type< T >
{
        using value_type = remove_cv_t< T > ;
} ;

template< typename T >
concept _has_member_value_type = requires { typename T::value_type ; } ;

template< typename T >
concept _has_member_element_type = requires { typename T::element_type ; } ;


template< typename > struct indirectly_readable_traits {} ;


template< typename T >
        requires is_array_v< T >
struct indirectly_readable_traits< T >
{
        using value_type = remove_cv_t< remove_extent_t< T > > ;
} ;

template< typename T >
struct indirectly_readable_traits< T const > : indirectly_readable_traits< T > {} ;

template< typename T >
struct indirectly_readable_traits< T * > : _cond_value_type< T > {} ;

template< _has_member_value_type T >
struct indirectly_readable_traits< T > : _cond_value_type< typename T::value_type > {} ;

template< _has_member_element_type T >
struct indirectly_readable_traits< T > : _cond_value_type< typename T::element_type > {} ;

template< _has_member_value_type T >
        requires _has_member_element_type< T >
struct indirectly_readable_traits< T > {} ;

template< _has_member_value_type T >
        requires _has_member_element_type< T > &&
                 meta::same_as< remove_cv_t< typename T::element_type >, remove_cv_t< typename T::value_type > >
struct indirectly_readable_traits< T > : _cond_value_type< typename T::value_type > {} ;


////////////////////////////////////////////////////////////////////////////////
///     incrementable_traits
////////////////////////////////////////////////////////////////////////////////

template< typename > struct incrementable_traits {} ;

template< typename T >
        requires is_object_v< T >
struct incrementable_traits< T * >
{
        using difference_type = ptrdiff_t ;
} ;

template< typename T >
struct incrementable_traits< T const > : incrementable_traits< T > {} ;

template< typename T >
concept _has_member_difference_type = requires { typename T::difference_type ; } ;

template< _has_member_difference_type T >
struct incrementable_traits< T >
{
        using difference_type = typename T::difference_type ;
} ;

template< typename T >
concept _has_integral_minus = requires( T const & x, T const & y )
{
        { x - y } -> meta::integral ;
} ;

template< _has_integral_minus T >
        requires( !_has_member_difference_type< T > )
struct incrementable_traits< T >
{
        using difference_type = make_signed_t< decltype( declval< T >() - declval< T >() ) > ;
} ;


////////////////////////////////////////////////////////////////////////////////
///     iterator_traits
////////////////////////////////////////////////////////////////////////////////

template< typename > struct iterator_traits ;


template< typename T >
using iter_difference_t = conditional_t< ::uti::_is_primary_template< iterator_traits< remove_cvref_t< T > > >::value,
                                         incrementable_traits< remove_cvref_t< T > >,
                                         iterator_traits< remove_cvref_t< T > > >::difference_type ;

template< typename T >
using _with_reference = T & ;

template< typename T >
concept _can_reference = requires { typename _with_reference< T > ; } ;

template< typename T >
concept _dereferenceable = requires( T & t )
{
        { *t } -> _can_reference ;
} ;


template< _dereferenceable T >
using iter_reference_t = decltype( *declval< T& >() ) ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter > struct iterator_traits ;


struct         input_iterator_tag                                     {} ;
struct        output_iterator_tag                                     {} ;
struct       forward_iterator_tag : public         input_iterator_tag {} ;
struct bidirectional_iterator_tag : public       forward_iterator_tag {} ;
struct random_access_iterator_tag : public bidirectional_iterator_tag {} ;
struct    contiguous_iterator_tag : public random_access_iterator_tag {} ;
struct  prefix_array_iterator_tag : public random_access_iterator_tag {} ;
struct      circular_iterator_tag : public random_access_iterator_tag {} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
struct _iter_traits_cache
{
        using type = conditional_t< _is_primary_template< iterator_traits< Iter > >::value, Iter, iterator_traits< Iter > > ;
} ;

template< typename Iter >
using _ITER_TRAITS = typename _iter_traits_cache< Iter >::type ;


struct _iter_concept_concept_test
{
        template< typename Iter >
        using fn = typename _ITER_TRAITS< Iter >::iterator_concept ;
} ;

struct _iter_concept_category_test
{
        template< typename Iter >
        using fn = typename _ITER_TRAITS< Iter >::iterator_category ;
} ;

struct _iter_concept_random_fallback
{
        template< typename Iter >
        using fn = enable_if_t< _is_primary_template< iterator_traits< Iter > >::value, random_access_iterator_tag > ;
} ;


template< typename Iter, typename Tester >
struct _test_iter_concept : _is_valid_expansion< Tester::template fn, Iter >, Tester {} ;


template< typename Iter >
struct _iter_concept_cache
{
        using type = disjunction< _test_iter_concept< Iter, _iter_concept_concept_test    > ,
                                  _test_iter_concept< Iter, _iter_concept_category_test   > ,
                                  _test_iter_concept< Iter, _iter_concept_random_fallback > > ;
} ;

template< typename Iter >
using _ITER_CONCEPT = typename _iter_concept_cache< Iter >::type::template fn< Iter > ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
struct _has_iterator_typedefs
{
private:
        template< typename U >
        static false_type _test ( ... ) ;
        template< typename U >
        static true_type
        _test ( void_t< typename U::iterator_category > * = nullptr ,
                void_t< typename U::  difference_type > * = nullptr ,
                void_t< typename U::       value_type > * = nullptr ,
                void_t< typename U::        reference > * = nullptr ,
                void_t< typename U::          pointer > * = nullptr ) ;
public:
        static const bool value = decltype( _test< T >( nullptr, nullptr, nullptr, nullptr, nullptr ) )::value ;
} ;

template< typename T >
struct _has_iterator_category
{
private:
        template< typename U >
        static false_type _test ( ... ) ;
        template< typename U >
        static true_type _test ( typename U::iterator_category * = nullptr ) ;
public:
        static const bool value = decltype( _test< T >( nullptr ) )::value ;
} ;

template< typename T >
struct _has_iterator_concept
{
private:
        template< typename U >
        static false_type _test ( ... ) ;
        template< typename U >
        static true_type _test ( typename U::iterator_concept * = nullptr ) ;
public:
        static const bool value = decltype( _test< T >( nullptr ) )::value ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
concept cpp17_iterator = requires( Iter iter )
{
        {  *iter   } -> _can_reference ;
        { ++iter   } -> meta::same_as< Iter & > ;
        {  *iter++ } -> _can_reference ;
} && meta::copyable< Iter > ;

template< typename Iter >
concept cpp17_input_iterator =
        cpp17_iterator< Iter > &&
        meta::equality_comparable< Iter > &&
        requires( Iter iter )
{
        typename incrementable_traits< Iter >::difference_type ;
        typename indirectly_readable_traits< Iter >::value_type ;
        typename common_reference_t< iter_reference_t< Iter >&&, typename indirectly_readable_traits< Iter >::value_type & > ;
        typename common_reference_t< decltype( *iter++ )&&, typename indirectly_readable_traits< Iter >::value_type & > ;
        requires meta::signed_integral< typename incrementable_traits< Iter >::difference_type > ;
} ;

template< typename Iter >
concept cpp17_forward_iterator =
        cpp17_input_iterator< Iter > &&
        meta::constructible_from< Iter > &&
        is_reference_v< iter_reference_t< Iter > > &&
        meta::same_as< remove_cvref_t< iter_reference_t< Iter > >, typename indirectly_readable_traits< Iter >::value_type > &&
        requires( Iter iter )
{
        {  iter++ } -> meta::convertible_to< Iter const & > ;
        { *iter++ } -> meta::same_as< iter_reference_t< Iter > > ;
} ;

template< typename Iter >
concept cpp17_bidirectional_iterator =
        cpp17_forward_iterator< Iter > &&
        requires( Iter iter )
{
        {  --iter } -> meta::same_as< Iter & > ;
        {  iter-- } -> meta::convertible_to< Iter const & > ;
        { *iter-- } -> meta::same_as< iter_reference_t< Iter > > ;
} ;

template< typename Iter >
concept cpp17_random_access_iterator =
        cpp17_bidirectional_iterator< Iter > &&
        meta::totally_ordered< Iter > &&
        requires( Iter iter, typename incrementable_traits< Iter >::difference_type n )
{
        { iter +=    n } -> meta::same_as< Iter & > ;
        { iter -=    n } -> meta::same_as< Iter & > ;
        { iter +     n } -> meta::same_as< Iter   > ;
        {    n +  iter } -> meta::same_as< Iter   > ;
        { iter -     n } -> meta::same_as< Iter   > ;
        { iter -  iter } -> meta::same_as< decltype( n ) > ;
        { iter[ n ]    } -> meta::convertible_to< iter_reference_t< Iter > > ;
} ;


namespace meta
{


template< typename Iter > concept               iterator = cpp17_iterator              < Iter > ;
template< typename Iter > concept         input_iterator = cpp17_input_iterator        < Iter > ;
template< typename Iter > concept       forward_iterator = cpp17_forward_iterator      < Iter > ;
template< typename Iter > concept bidirectional_iterator = cpp17_bidirectional_iterator< Iter > ;
template< typename Iter > concept random_access_iterator = cpp17_random_access_iterator< Iter > ;

template< typename Iter >
concept prefix_array_iterator =
        random_access_iterator< Iter > &&
        same_as< typename iterator_traits< Iter >::iterator_category, prefix_array_iterator_tag > ;

template< typename Iter >
concept circular_iterator =
        random_access_iterator< Iter > &&
        same_as< typename iterator_traits< Iter >::iterator_category, circular_iterator_tag > ;


} // namespace meta


////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
concept _has_member_reference = requires { typename Iter::reference ; } ;

template< typename Iter >
concept _has_member_pointer = requires { typename Iter::pointer ; } ;

template< typename Iter >
concept _has_member_iterator_category = requires { typename Iter::iterator_category ; } ;

template< typename Iter >
concept _specifies_members = requires
{
        typename Iter::     value_type ;
        typename Iter::difference_type ;
        requires _has_member_reference        < Iter > ;
        requires _has_member_iterator_category< Iter > ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename >
struct _iterator_traits_member_pointer_or_void
{
        using type = void ;
} ;

template< _has_member_pointer T >
struct _iterator_traits_member_pointer_or_void< T >
{
        using type = typename T::pointer ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
concept _cpp17_iterator_missing_members = !_specifies_members< T > && cpp17_iterator< T > ;

template< typename T >
concept _cpp17_input_iterator_missing_members =
        _cpp17_iterator_missing_members< T > && cpp17_input_iterator< T > ;

////////////////////////////////////////////////////////////////////////////////

template< typename >
struct _iterator_traits_member_pointer_or_arrow_or_void
{
        using type = void ;
} ;

template< _has_member_pointer Iter >
struct _iterator_traits_member_pointer_or_arrow_or_void< Iter >
{
        using type = typename Iter::pointer ;
} ;

template< typename Iter >
        requires requires( Iter & iter ) { iter.operator->() ; } && ( _has_member_pointer< Iter > )
struct _iterator_traits_member_pointer_or_arrow_or_void< Iter >
{
        using type = decltype( declval< Iter & >().operator->() ) ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
struct _iterator_traits_member_reference
{
        using type = iter_reference_t< Iter > ;
} ;

template< _has_member_reference Iter >
struct _iterator_traits_member_reference< Iter >
{
        using type = typename Iter::reference ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
struct _deduce_iterator_category
{
        using type = input_iterator_tag ;
} ;

template< cpp17_random_access_iterator Iter >
struct _deduce_iterator_category< Iter >
{
        using type = random_access_iterator_tag ;
} ;

template< cpp17_bidirectional_iterator Iter >
struct _deduce_iterator_category< Iter >
{
        using type = bidirectional_iterator_tag ;
} ;

template< cpp17_forward_iterator Iter >
struct _deduce_iterator_category< Iter >
{
        using type = forward_iterator_tag ;
} ;

template< typename Iter >
struct _iterator_traits_iterator_category : _deduce_iterator_category< Iter > {} ;

template< _has_member_iterator_category Iter >
struct _iterator_traits_iterator_category< Iter >
{
        using type = typename Iter::iterator_category ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename >
struct _iterator_traits_difference_type
{
        using type = void ;
} ;

template< typename Iter >
        requires requires { typename incrementable_traits< Iter >::difference_type ; }
struct _iterator_traits_difference_type< Iter >
{
        using type = typename incrementable_traits< Iter >::difference_type ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename >
struct _iterator_traits {} ;

template< _specifies_members Iter >
struct _iterator_traits< Iter >
{
        using iterator_category = typename Iter::iterator_category ;
        using        value_type = typename Iter::       value_type ;
        using   difference_type = typename Iter::  difference_type ;
        using           pointer = typename Iter::          pointer ;
        using         reference = typename Iter::        reference ;
} ;

template< _cpp17_input_iterator_missing_members Iter >
struct _iterator_traits< Iter >
{
        using iterator_category = typename _iterator_traits_iterator_category< Iter >::              type ;
        using        value_type = typename  indirectly_readable_traits       < Iter >::        value_type ;
        using   difference_type = typename  incrementable_traits             < Iter >::   difference_type ;
        using           pointer = typename _iterator_traits_member_pointer_or_arrow_or_void< Iter >::type ;
        using         reference = typename _iterator_traits_member_reference < Iter >::              type ;
} ;

template< _cpp17_iterator_missing_members Iter >
struct _iterator_traits< Iter >
{
        using iterator_category = output_iterator_tag ;
        using        value_type = void ;
        using   difference_type = typename _iterator_traits_difference_type< Iter >::type ;
        using           pointer = void ;
        using         reference = void ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
struct iterator_traits : _iterator_traits< Iter >
{
        using _primary_template = iterator_traits ;
} ;

template< typename T >
        requires is_object_v< T >
struct iterator_traits< T * >
{
        using   difference_type = ptrdiff_t        ;
        using        value_type = remove_cv_t< T > ;
        using           pointer = T *              ;
        using         reference = T &              ;
        using iterator_category = random_access_iterator_tag ;
} ;

////////////////////////////////////////////////////////////////////////////////

template< typename T, typename U, bool = _has_iterator_category< iterator_traits< T > >::value >
struct _has_iterator_category_convertible_to : is_convertible< typename iterator_traits< T >::iterator_category, U > {} ;

template< typename T, typename U >
struct _has_iterator_category_convertible_to< T, U, false > : false_type {} ;

template< typename T, typename U, bool = _has_iterator_concept< T >::value >
struct _has_iterator_concept_convertible_to : is_convertible< typename T::iterator_concept, U > {} ;

template< typename T, typename U >
struct _has_iterator_concept_convertible_to< T, U, false > : false_type {} ;


template< typename T >
using _has_input_iterator_category = _has_iterator_category_convertible_to< T, input_iterator_tag > ;

template< typename T >
static constexpr bool _has_input_iterator_category_v = _has_input_iterator_category< T >::value ;

template< typename T >
using _has_forward_iterator_category = _has_iterator_category_convertible_to< T, forward_iterator_tag > ;

template< typename T >
static constexpr bool _has_forward_iterator_category_v = _has_forward_iterator_category< T >::value ;

template< typename T >
using _has_bidirectional_iterator_category = _has_iterator_category_convertible_to< T, bidirectional_iterator_tag > ;

template< typename T >
static constexpr bool _has_bidirectional_iterator_category_v = _has_bidirectional_iterator_category< T >::value ;

template< typename T >
using _has_random_access_iterator_category = _has_iterator_category_convertible_to< T, random_access_iterator_tag > ;

template< typename T >
static constexpr bool _has_random_access_iterator_category_v = _has_random_access_iterator_category< T >::value ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
struct _is_contiguous_iterator
        : disjunction< _has_iterator_category_convertible_to< T, contiguous_iterator_tag > ,
                       _has_iterator_concept_convertible_to < T, contiguous_iterator_tag > > {} ;

template< typename T >
struct _is_contiguous_iterator< T * > : true_type {} ;

////////////////////////////////////////////////////////////////////////////////

template< typename Iter >
class _wrap_iter ;

////////////////////////////////////////////////////////////////////////////////

template< typename T >
using _has_exactly_input_iterator_category =
        integral_constant< _has_iterator_category_convertible_to< T,   input_iterator_tag >::value &&
                          !_has_iterator_category_convertible_to< T, forward_iterator_tag >::value > ;

template< typename T >
using _has_exactly_forward_iterator_category =
        integral_constant< _has_iterator_category_convertible_to< T,       forward_iterator_tag >::value &&
                          !_has_iterator_category_convertible_to< T, bidirectional_iterator_tag >::value > ;

template< typename T >
using _has_exactly_bidirectional_iterator_category =
        integral_constant< _has_iterator_category_convertible_to< T, bidirectional_iterator_tag >::value &&
                          !_has_iterator_category_convertible_to< T, random_access_iterator_tag >::value > ;

////////////////////////////////////////////////////////////////////////////////

template< typename InputIter >
using _iter_value_type = typename iterator_traits< InputIter >::value_type ;

template< typename InputIter >
using _iter_key_type = remove_const_t< typename iterator_traits< InputIter >::value_type::first_type > ;

template< typename InputIter >
using _iter_mapped_type = typename iterator_traits< InputIter >::value_type::second_type ;

/*
template< typename InputIter >
using _iter_to_alloc_type =
        pair< typename iterator_traits< InputIter >::value_type::first_type const,
               typename iterator_traits< InputIter >::value_type::second_type > ;
*/

template< typename Iter >
using _iterator_category_type = typename iterator_traits< Iter >::iterator_category ;

template< typename Iter >
using _iterator_pointer_type = typename iterator_traits< Iter >::pointer ;

template< typename Iter >
using _iter_diff_t = typename iterator_traits< Iter >::difference_type ;

template< typename Iter >
using _iter_reference = typename iterator_traits< Iter >::reference ;


template< typename Iter >
using iter_value_t =
        typename conditional_t< _is_primary_template< iterator_traits< remove_cvref_t< Iter > > >::value,
                                indirectly_readable_traits< remove_cvref_t< Iter > >,
                                iterator_traits< remove_cvref_t< Iter > > >::value_type ;


} // namespace uti

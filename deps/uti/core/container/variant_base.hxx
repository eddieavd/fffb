//
//
//      uti
//      core/container/variant_base.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/util/cm_control.hxx>
#include <uti/core/meta/concepts.hxx>
#include <uti/core/container/tagged_union.hxx>


namespace uti
{


template< typename VariantPolicy, typename T, typename... Ts >
class variant_base ;


namespace _detail
{


template< typename T > struct is_variant_impl : false_type {} ;

template< typename VariantPolicy, typename T, typename... Ts >
struct is_variant_impl< variant_base< VariantPolicy, T, Ts... > > : true_type {} ;

template< typename T >
using is_variant = is_variant_impl< decay_t< T > > ;

template< typename T >
static constexpr bool is_variant_v = is_variant< T >::value ;


template< typename... Ts >
struct variant_traits_impl
{
        using copy_constructible = conjunction< is_copy_constructible< Ts >... > ;
        using move_constructible = conjunction< is_move_constructible< Ts >... > ;

        using nothrow_move_constructible = conjunction< is_nothrow_move_constructible< Ts >... > ;
        using nothrow_move_assignable    = conjunction< is_nothrow_move_assignable   < Ts >... > ;

        using nothrow_swappable = conjunction< nothrow_move_constructible, is_nothrow_swappable< Ts >... > ;
} ;

template< typename... Ts >
using variant_traits = variant_traits_impl< decay_t< Ts >... > ;


template< typename VariantPolicy, typename Union >
struct copy_assign_union_value
{
        struct visitor
        {
                template< typename T >
                        requires is_copy_assignable_v< T >
                constexpr void do_assign ( Union & _dest_, T const & _value_ )
                {
                        _dest_.value( union_type< T >{} ) = _value_ ;
                }
                template< typename T >
                        requires( !is_copy_assignable_v< T > )
                constexpr void do_assign ( Union & _dest_, T const & _value_ )
                {
                        VariantPolicy::change_value( union_type< T >{}, _dest_, _value_ ) ;
                }

                template< typename T >
                constexpr void operator() ( T const & _value_, Union & _dest_ )
                {
                        constexpr auto id = typename Union::type_id( union_type< T >{} ) ;

                        if( _dest_.type() == id )
                        {
                                do_assign( _dest_, _value_ ) ;
                        }
                        else
                        {
                                VariantPolicy::change_value( union_type< T >{}, _dest_, _value_ ) ;
                        }
                }
        } ;
        static constexpr void assign ( Union & _dest_, Union const & _src_ )
        {
                apply( _src_, visitor{}, _dest_ ) ;
        }
} ;

template< typename VariantPolicy, typename Union >
struct move_assign_union_value
{
        struct visitor
        {
                template< typename T >
                        requires is_move_assignable_v< T >
                constexpr void do_assign ( Union & _dest_, T && _value_ )
                {
                        _dest_.value( union_type< T >{} ) = UTI_MOVE( _value_ ) ;
                }
                template< typename T >
                        requires( !is_move_assignable_v< T > )
                constexpr void do_assign ( Union & _dest_, T && _value_ )
                {
                        VariantPolicy::change_value( union_type< decay_t< T > >{}, _dest_, UTI_MOVE( _value_ ) ) ;
                }

                template< typename T >
                constexpr void operator() ( T && _value_, Union & _dest_ )
                {
                        constexpr auto id = typename Union::type_id( union_type< decay_t< T > >{} ) ;

                        if( _dest_.type() == id )
                        {
                                do_assign( _dest_, UTI_MOVE( _value_ ) ) ;
                        }
                        else
                        {
                                VariantPolicy::change_value( union_type< decay_t< T > >{}, _dest_, UTI_MOVE( _value_ ) ) ;
                        }
                }
        } ;
        static constexpr void assign ( Union & _dest_, Union && _src_ )
        {
                apply( UTI_MOVE( _src_ ), visitor{}, _dest_ ) ;
        }
} ;

template< typename VariantPolicy, typename Union >
struct swap_union
{
        struct visitor
        {
                template< typename T >
                constexpr void operator() ( T &, Union & _lhs_, Union & _rhs_ )
                {
                        constexpr auto id = typename Union::type_id( union_type< T >{} ) ;
                        UTI_CEXPR_ASSERT( _lhs_.type() == id, "uti::variant_base::swap_union: type mismatch" ) ;

                        if( _rhs_.type() == id )
                        {
                                using uti::swap ;
                                swap( _lhs_.value( union_type< T >{} ), _rhs_.value( union_type< T >{} ) ) ;
                        }
                        else
                        {
                                T tmp( UTI_MOVE( _lhs_ ).value( union_type< T >{} ) ) ;

                                move_assign_union_value< VariantPolicy, Union >::assign( _lhs_, UTI_MOVE( _rhs_ ) ) ;

                                VariantPolicy::change_value( union_type< T >{}, _rhs_, UTI_MOVE( tmp ) ) ;
                        }
                }
        } ;
        static constexpr void swap ( Union & _lhs_, Union & _rhs_ )
        {
                apply( _lhs_, visitor{}, _lhs_, _rhs_ ) ;
        }
} ;


namespace _detail
{


template< typename Fn, typename Val, typename... Args >
constexpr auto map_invoke ( Fn&& _fn_, Val&& _val_, Args&&... _args_ )
        -> decltype( UTI_FWD( _fn_ )( UTI_FWD( _val_ ), UTI_FWD( _args_ )... ) )
{
        return UTI_FWD( _fn_ )( UTI_FWD( _val_ ), UTI_FWD( _args_ )... ) ;
}

template< typename Fn, typename Val >
constexpr auto map_invoke ( Fn&& _fn_, Val&& _val_ ) -> decltype( UTI_FWD( _val_ ).*UTI_FWD( _fn_ ) )
{
        return UTI_FWD( _val_ ).*UTI_FWD( _fn_ ) ;
}

template< typename Fn, typename Val, typename... Args >
constexpr auto map_invoke ( Fn&& _fn_, Val&& _val_, Args&&... _args_ )
        -> decltype( ( UTI_FWD( _val_ ).*UTI_FWD( _fn_ ) )( UTI_FWD( _args_ )... ) )
{
        return ( UTI_FWD( _val_ ).*UTI_FWD( _fn_ ) )( UTI_FWD( _args_ )... ) ;
}


} // namespace _detail


template< typename Functor, typename Union >
struct map_union
{
        struct visitor
        {
                template< typename T, typename... Args >
                constexpr auto call ( int, Union & _res_, Functor&& _fn_, T&& _value_, Args&&... _args_ )
                        -> decltype( ( void ) map_invoke( UTI_FWD( _fn_ ), UFI_FWD( _value_ ), UTI_FWD( _args_ )... ) )
                {
                        using result = decltype( map_invoke( UTI_FWD( _fn_ ), UTI_FWD( _value_ ), UTI_FWD( _args_ )... ) ) ;

                        _res_.emplace( union_type< decay_t< result > >{}, map_invoke( UTI_FWD( _fn_ ), UTI_FWD( _value_ ), UTI_FWD( _args_ )... ) ) ;
                }
                template< typename T, typename... Args >
                constexpr void call ( short, Union & _res_, Functor&&, T && _value_, Args&&... )
                {
                        _res_.emplace( union_type< decay_t< T > >{}, UTI_FWD( _value_ ) ) ;
                }

                template< typename T, typename... Args >
                constexpr void operator() ( T && _value_, Union & _res_, Functor&& _fn_, Args&&... _args_ )
                {
                        call( 0, _res_, UTI_FWD( _fn_ ), UTI_FWD( _value_ ), UTI_FWD( _args_ )... ) ;
                }
        } ;

        template< typename... Args >
        static constexpr void map ( Union & _res_, Union const & _union_, Functor&& _fn_, Args&&... _args_ )
        {
                UTI_CEXPR_ASSERT( !_res_.has_value(), "uti::variant_base::map_union: result union non-empty" ) ;
                apply( _union_, visitor{}, _res_, UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
        }
        template< typename... Args >
        static constexpr void map ( Union & _res_, Union && _union_, Functor&& _fn_, Args&&... _args_ )
        {
                UTI_CEXPR_ASSERT( !_res_.has_value(), "uti::variant_base::map_union: result union non-empty" ) ;
                apply( UTI_MOVE( _union_ ), visitor{}, _res_, UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
        }
} ;

template< typename Variant >
struct compare_variant
{
        struct equal_visitor
        {
                bool result = false ;

                template< typename T >
                constexpr void operator() ( T const & _value_, Variant const & _other_ )
                {
                        result = ( _value_ == _other_ ) ;
                }
        } ;

        struct less_visitor
        {
                bool result = false ;

                template< typename T >
                constexpr void operator() ( T const & _value_, Variant const & _other_ )
                {
                        result = ( _value_ < _other_ ) ;
                }
        } ;

        static constexpr bool compare_equal ( Variant const & _lhs_, Variant const & _rhs_ )
        {
                if( !_lhs_.has_value() )
                {
                        return !_rhs_.has_value() ;
                }
                equal_visitor v ;
                apply( _lhs_, v, _rhs_ ) ;
                return v.result ;
        }
        static constexpr bool compare_less ( Variant const & _lhs_, Variant const & _rhs_ )
        {
                if( !_lhs_.has_value() )
                {
                        return _rhs_.has_value() ;
                }
                less_visitor v ;
                apply( _lhs_, v, _rhs_ ) ;
                return v.result ;
        }
} ;


template< typename VariantPolicy, typename... Ts >
class variant_storage
{
        using traits = variant_traits< Ts... > ;
public:
        constexpr variant_storage () noexcept = default ;

        constexpr variant_storage ( variant_storage const & _other_ )
        {
                copy( storage_, _other_.storage_ ) ;
        }
        constexpr variant_storage ( variant_storage && _other_ ) noexcept( traits::nothrow_move_constructible::value )
        {
                move( storage_, UTI_MOVE( _other_.storage_ ) ) ;
        }

        constexpr ~variant_storage () noexcept
        {
                destroy( storage_ ) ;
        }

        constexpr variant_storage & operator= ( variant_storage const & _other_ )
        {
                if( storage_.has_value() && _other_.storage_.has_value() )
                {
                        copy_assign_union_value< VariantPolicy, tagged_union< Ts... > >::assign( storage_, _other_.storage_ ) ;
                }
                else if( storage_.has_value() && !_other_.storage_.has_value() )
                {
                        destroy( storage_ ) ;
                }
                else if( !storage_.has_value() && _other_.storage_.has_value() )
                {
                        copy( storage_, _other_.storage_ ) ;
                }
                return *this ;
        }
        constexpr variant_storage & operator= ( variant_storage && _other_ )
        {
                if( storage_.has_value() && _other_.storage_.has_value() )
                {
                        move_assign_union_value< VariantPolicy, tagged_union< Ts... > >::assign( storage_, UTI_MOVE( _other_.storage_ ) ) ;
                }
                else if( storage_.has_value() && !_other_.storage_.has_value() )
                {
                        destroy( storage_ ) ;
                }
                else if( !storage_.has_value() && _other_.storage_.has_value() )
                {
                        move( storage_, UTI_MOVE( _other_.storage_ ) ) ;
                }
                return *this ;
        }

        constexpr tagged_union< Ts... > & get_union () noexcept
        {
                return storage_ ;
        }
        constexpr tagged_union< Ts... > const & get_union () const noexcept
        {
                return storage_ ;
        }
private:
        tagged_union< Ts... > storage_ ;
} ;


struct storage_access
{
        template< typename Variant >
        static constexpr auto get ( Variant & _var_ ) -> decltype( _var_.storage_ ) &
        {
                return _var_.storage_ ;
        }
        template< typename Variant >
        static constexpr auto get ( Variant const & _var_ ) -> decltype( _var_.storage_ ) const &
        {
                return _var_.storage_ ;
        }
} ;


template< typename... Ts >
using variant_copy = copy_control< variant_traits< Ts... >::copy_constructible::value > ;

template< typename... Ts >
using variant_move = move_control< variant_traits< Ts... >::move_constructible::value > ;


template< typename Union, typename T, typename... Args >
using enable_variant_type_impl = enable_if_t< Union::type_id::template is_valid< T >() && is_constructible_v< T, Args... > > ;

template< typename Union, typename T, typename... Args >
using enable_variant_type = enable_variant_type_impl< Union, decay_t< T >, Args... > ;


template< typename Union, typename T, typename... Args >
using valid_variant_type = integral_constant< Union::type_id::template is_valid< T >() && is_constructible_v< T, Args... > > ;

template< typename Union, typename T, typename... Args >
static constexpr bool valid_variant_type_v = valid_variant_type< Union, T, Args... >::value ;


} // namespace _detail


} // namespace uti

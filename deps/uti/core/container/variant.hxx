//
//
//      uti
//      core/container/variant.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/meta/concepts.hxx>
#include <uti/core/container/tagged_union.hxx>
#include <uti/core/container/variant_base.hxx>


namespace uti
{


template< typename    T  > using variant_type  = union_type < T     > ;
template< typename... Ts > using variant_types = union_types< Ts... > ;

struct nullvar_t
{
        constexpr nullvar_t () noexcept {}
} ;

constexpr nullvar_t nullvar ;


template< typename VariantPolicy, typename T, typename... Ts >
class basic_variant : _detail::variant_copy< T, Ts... >, _detail::variant_move< T, Ts... >
{
        using union_t = tagged_union< T, Ts... > ;
        using  traits = _detail::variant_traits< T, Ts... > ;
public:
        using value_types = typename union_t::value_types ;
        using     type_id = typename union_t::    type_id ;

        using allow_empty = typename VariantPolicy::allow_empty ;

        static constexpr type_id invalid_type = union_t::invalid_type ;

        constexpr basic_variant (           ) noexcept requires( VariantPolicy::allow_empty::value ) {}
        constexpr basic_variant ( nullvar_t ) noexcept requires( VariantPolicy::allow_empty::value ) : basic_variant() {}

        constexpr basic_variant ( basic_variant const &  ) = default ;
        constexpr basic_variant ( basic_variant       && ) noexcept( traits::nothrow_move_constructible::value ) = default ;

        template< typename T1, typename... Args,
                  typename = _detail::enable_variant_type< union_t, T1, Args&&... > >
        explicit constexpr basic_variant ( variant_type< T1 > _type_, Args&&... _args_ )
        {
                storage_.get_union().emplace( _type_, UTI_FWD( _args_ )... ) ;
        }

        template< typename T1, typename = _detail::enable_variant_type< union_t, T1, T1&& > >
        constexpr basic_variant ( T1&& _obj_ )
                : basic_variant( variant_type< decay_t< T1 > >{}, UTI_FWD( _obj_ ) ) {}

        explicit constexpr basic_variant ( tagged_union< T, Ts... > const & _union_ )
        {
                UTI_CEXPR_ASSERT( allow_empty::value || _union_.has_value(), "uti::basic_variant::copy_from_tagged_union: empty source union" ) ;
                copy( storage_.get_union(), _union_ ) ;
        }
        explicit constexpr basic_variant ( tagged_union< T, Ts... > && _union_ )
        {
                UTI_CEXPR_ASSERT( allow_empty::value || _union_.has_value(), "uti::basic_variant::move_from_tagged_union: empty source union" ) ;
                move( storage_.get_union(), UTI_MOVE( _union_ ) ) ;
        }

        constexpr ~basic_variant () noexcept = default ;

        constexpr basic_variant & operator= ( basic_variant const &  ) = default ;
        constexpr basic_variant & operator= ( basic_variant       && ) noexcept( traits::nothrow_move_assignable::value ) = default ;

        constexpr basic_variant & operator= ( nullvar_t ) noexcept
                requires( VariantPolicy::allow_empty::value )
        {
                reset() ;
                return *this ;
        }

        template< typename T1, typename = _detail::enable_variant_type< union_t, T1, T1&& > >
        constexpr basic_variant & operator= ( T&& _obj_ )
        {
                emplace( variant_type< decay_t< T1 > >{}, UTI_FWD( _obj_ ) ) ;
                return *this ;
        }

        friend constexpr void swap ( basic_variant & _lhs_, basic_variant & _rhs_ ) noexcept( traits::nothrow_swappable::value )
        {
                auto & lhs_union = _lhs_.storage_.get_union() ;
                auto & rhs_union = _rhs_.storage_.get_union() ;

                if( lhs_union.has_value() && rhs_union.has_value() )
                {
                        _detail::swap_union< VariantPolicy, union_t >::swap( lhs_union, rhs_union ) ;
                }
                else if( lhs_union.has_value() && !rhs_union.has_value() )
                {
                        _rhs_ = UTI_MOVE( _lhs_ ) ;
                        _lhs_.reset() ;
                }
                else if( !lhs_union.has_value() && rhs_union.has_value() )
                {
                        _lhs_ = UTI_MOVE( _rhs_ ) ;
                        _rhs_.reset() ;
                }
        }

        constexpr void reset () noexcept requires( VariantPolicy::allow_empty::value )
        {
                destroy( storage_.get_union() ) ;
        }

        template< typename T1, typename Arg >
                requires( is_direct_assignable_v< T, Arg&& > && _detail::valid_variant_type_v< union_t, T1, Arg&& > )
        constexpr void emplace ( variant_type< T1 > _type_, Arg&& _arg_ )
        {
                if( storage_.get_union().type() == typename union_t::type_id( _type_ ) )
                {
                        storage_.get_union().value( _type_ ) = UTI_FWD( _arg_ ) ;
                }
                else
                {
                        emplace_impl( _type_, UTI_FWD( _arg_ ) ) ;
                }
        }

        template< typename T1, typename Arg >
                requires( is_direct_assignable_v< T, Arg&& > && _detail::valid_variant_type_v< union_t, T1, Arg&& > )
        constexpr void emplace ( Arg&& _arg_ )
        {
                emplace( variant_type< T1 >{}, UTI_FWD( _arg_ ) ) ;
        }

        template< typename T1, typename... Args >
                requires( _detail::valid_variant_type_v< union_t, T1, Args&&... > )
        constexpr void emplace ( variant_type< T1 > _type_, Args&&... _args_ )
        {
                emplace_impl( _type_, UTI_FWD( _args_ )... ) ;
        }

        template< typename T1, typename... Args >
                requires( _detail::valid_variant_type_v< union_t, T1, Args&&... > )
        constexpr void emplace ( Args&&... _args_ )
        {
                emplace( variant_type< T1 >{}, UTI_FWD( _args_ )... ) ;
        }
private:
        template< typename T1, typename... Args >
        constexpr void emplace_impl ( variant_type< T1 > _type_, Args&&... _args_ )
        {
                if( storage_.get_union().has_value() )
                {
                        VariantPolicy::change_value( _type_, storage_.get_union(), UTI_FWD( _args_ )... ) ;
                }
                else
                {
                        storage_.get_union().emplace( _type_, UTI_FWD( _args_ )... ) ;
                }
        }

        template< typename T1 >
        using enable_valid = enable_if_t< ( type_id::template is_valid< T1 > )() > ;
public:
        UTI_NODISCARD constexpr type_id type () const noexcept
        {
                return storage_.get_union().type() ;
        }

        UTI_NODISCARD constexpr bool has_value () const noexcept
        {
                return storage_.get_union().has_value() ;
        }

        UTI_NODISCARD constexpr operator bool () const noexcept
        {
                return has_value() ;
        }

        UTI_NODISCARD constexpr bool has_value ( variant_type< nullvar_t > ) const noexcept
        {
                return !has_value() ;
        }

        template< typename T1 >
        UTI_NODISCARD constexpr bool has_value ( variant_type< T1 > _type_ ) const noexcept
        {
                return this->type() == type_id( _type_ ) ;
        }

        template< typename T1 >
        UTI_NODISCARD constexpr bool has_value () const noexcept
        {
                return has_value( variant_type< T1 >{} ) ;
        }

        UTI_NODISCARD constexpr nullvar_t value ( variant_type< nullvar_t > ) const noexcept
        {
                UTI_CEXPR_ASSERT( !has_value(), "uti::variant::value_nullvar_t: variant has value" ) ;
                return nullvar ;
        }

        template< typename T1, typename Self, typename = enable_valid< T1 > >
        UTI_NODISCARD constexpr decltype( auto ) value ( this Self && self, variant_type< T1 > _type_ ) noexcept
        {
                return UTI_FWD( self ).storage_.get_union().value( _type_ ) ;
        }
        template< typename T1, typename Self, typename = enable_valid< T1 > >
        UTI_NODISCARD constexpr decltype( auto ) value ( this Self && self ) noexcept
        {
                return UTI_FWD( self ).value( variant_type< T1 >{} ) ;
        }

        // TODO: replace with optional
        template< typename T1 > using optional_ref = T1 * ;

        UTI_NODISCARD constexpr optional_ref< nullvar_t const > optional_value ( variant_type< nullvar_t > ) const noexcept
        {
                return has_value() ? nullptr : &nullvar ;
        }

        template< typename T1, typename Self >
        UTI_NODISCARD constexpr decltype( auto ) optional_value ( this Self && self, variant_type< T1 > _type_ ) noexcept
        {
                return UTI_FWD( self ).has_value( _type_ ) ? optional_ref< T1 >( &UTI_FWD( self ).storage_.get_union().value( _type_ ) ) : nullptr ;
        }
        template< typename T1, typename Self >
        UTI_NODISCARD constexpr decltype( auto ) optional_value ( this Self && self ) noexcept
        {
                return UTI_FWD( self ).optional_value( variant_type< T1 >{} ) ;
        }

        template< typename T1, typename U1 >
                requires( is_copy_constructible_v< T1 > && is_convertible_v< U1&&, T1 > )
        UTI_NODISCARD constexpr T1 value_or ( variant_type< T1 > _type_, U1&& _other_ ) const &
        {
                return has_value( _type_ ) ? value( _type_ ) : static_cast< T1 >( UTI_FWD( _other_ ) ) ;
        }

        template< typename T1, typename U1 >
                requires( is_move_constructible_v< T1 > && is_convertible_v< U1&&, T1 > )
        UTI_NODISCARD constexpr T1 value_or ( variant_type< T1 > _type_, U1&& _other_ ) &&
        {
                return has_value( _type_ ) ? UTI_MOVE( value( _type_ ) ) : static_cast< T1 >( UTI_FWD( _other_ ) ) ;
        }

        template< typename Functor, typename... Args >
                requires( traits::copy_constructible::value )
        constexpr basic_variant map ( Functor&& _fn_, Args&&... _args_ ) const &
        {
                basic_variant result( force_empty{} ) ;

                if( !has_value() )
                {
                        return result ;
                }
                _detail::map_union< Functor&&, union_t >::map( result.storage_.get_union(),
                                                               storage_.get_union(),
                                                               UTI_FWD( _fn_ ),
                                                               UTI_FWD( _args_ )...
                ) ;
                UTI_CEXPR_ASSERT( result.has_value(), "uti::variant::map: result empty" ) ;
                return result ;
        }

        template< typename Functor, typename... Args >
                requires( traits::move_constructible::value )
        constexpr basic_variant map ( Functor&& _fn_, Args&&... _args_ ) &&
        {
                basic_variant result( force_empty{} ) ;

                if( !has_value() )
                {
                        return result ;
                }
                _detail::map_union< Functor&&, union_t >::map( result.storage_.get_union(),
                                                               UTI_MOVE( storage_.get_union() ),
                                                               UTI_FWD( _fn_ ),
                                                               UTI_FWD( _args_ )...
                ) ;
                UTI_CEXPR_ASSERT( result.has_value(), "uti::variant::map: result empty" ) ;
                return result ;
        }
private:
        struct force_empty {} ;

        constexpr basic_variant ( force_empty ) noexcept {}

        _detail::variant_storage< VariantPolicy, T, Ts... > storage_ ;

        friend _detail::storage_access ;
} ;


template< typename VariantPolicy, typename T, typename... Ts >
constexpr typename basic_variant< VariantPolicy, T, Ts... >::type_id
        basic_variant< VariantPolicy, T, Ts... >::invalid_type ;

#define UTI_DETAIL_MAKE_OP( Op, Expr1, Expr2 )                                                         \
        template< typename VariantPolicy, typename T, typename... Ts >                                  \
        constexpr bool operator Op ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, nullvar_t ) \
        {                                                                                                 \
                return ( void )_lhs_, Expr1 ;                                                              \
        }                                                                                                   \
        template< typename VariantPolicy, typename T, typename... Ts >                                       \
        constexpr bool operator Op ( nullvar_t, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )      \
        {                                                                                                      \
                return ( void )_rhs_, Expr2 ;                                                                   \
        }

UTI_DETAIL_MAKE_OP( ==, !_lhs_.has_value(), !_rhs_.has_value() )
UTI_DETAIL_MAKE_OP( !=,  _lhs_.has_value(),  _rhs_.has_value() )
UTI_DETAIL_MAKE_OP( < ,            false  ,  _rhs_.has_value() )
UTI_DETAIL_MAKE_OP( <=, !_lhs_.has_value(),             true   )
UTI_DETAIL_MAKE_OP( > ,  _lhs_.has_value(),            false   )
UTI_DETAIL_MAKE_OP( >=,             true  , !_rhs_.has_value() )

#undef UTI_DETAIL_MAKE_OP


template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator== ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        return _lhs_.has_value( variant_type< T1 >{} ) && _lhs_.value( variant_type< T1 >{} ) == _rhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator== ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return _rhs_ == _lhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator!= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        return !( _lhs_ == _rhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator!= ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _lhs_ == _rhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator< ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        constexpr auto id = typename basic_variant< VariantPolicy, T, Ts... >::type_id( variant_type< T1 >{} ) ;

        if( _lhs_.type() != id )
        {
                return _lhs_.type() < id ;
        }
        return _lhs_.value( variant_type< T1 >{} ) < _rhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator< ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        constexpr auto id = typename basic_variant< VariantPolicy, T, Ts... >::type_id( variant_type< T1 >{} ) ;

        if( id != _rhs_.type() )
        {
                return id < _rhs_.type() ;
        }
        return _lhs_.value( variant_type< T1 >{} ) < _rhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator<= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        return !( _rhs_ < _lhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator<= ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _rhs_ < _lhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator> ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        return _rhs_ < _lhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator> ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return _rhs_ < _lhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator>= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_, T1 const & _rhs_ )
{
        return !( _rhs_ > _lhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename T1 >
constexpr bool operator>= ( T1 const & _lhs_, basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _rhs_ > _lhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator== ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                            basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return _detail::compare_variant< basic_variant< VariantPolicy, T, Ts... > >::compare_equal( _lhs_, _rhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator!= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                            basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _lhs_ == _rhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator< ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                           basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return _detail::compare_variant< basic_variant< VariantPolicy, T, Ts... > >::compare_less( _lhs_, _rhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator<= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                            basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _rhs_ < _lhs_ ) ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator> ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                           basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return _rhs_ < _lhs_ ;
}

template< typename VariantPolicy, typename T, typename... Ts >
constexpr bool operator>= ( basic_variant< VariantPolicy, T, Ts... > const & _lhs_ ,
                            basic_variant< VariantPolicy, T, Ts... > const & _rhs_ )
{
        return !( _rhs_ > _lhs_ ) ;
}


template< typename VariantPolicy, typename T, typename... Ts, typename Fn, typename... Args >
constexpr void apply ( basic_variant< VariantPolicy, T, Ts... > & _variant_, Fn&& _fn_, Args&&... _args_ )
{
        apply( _detail::storage_access::get( _variant_ ).get_union(), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename Fn, typename... Args >
constexpr void apply ( basic_variant< VariantPolicy, T, Ts... > const & _variant_, Fn&& _fn_, Args&&... _args_ )
{
        apply( _detail::storage_access::get( _variant_ ).get_union(), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename Fn, typename... Args >
constexpr void apply ( basic_variant< VariantPolicy, T, Ts... > && _variant_, Fn&& _fn_, Args&&... _args_ )
{
        apply( UTI_MOVE( _detail::storage_access::get( _variant_ ).get_union() ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename VariantPolicy, typename T, typename... Ts, typename Fn, typename... Args >
constexpr void apply ( basic_variant< VariantPolicy, T, Ts... > const && _variant_, Fn&& _fn_, Args&&... _args_ )
{
        apply( UTI_MOVE( _detail::storage_access::get( _variant_ ).get_union() ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}


template< typename Fallback >
class fallback_variant_policy
{
        static_assert( is_nothrow_default_constructible_v< Fallback >,
                       "uti::variant: fallback policy must be nothrow default constructible" ) ;
public:
        using allow_empty = false_type ;

        template< typename T, typename... Ts, typename... Args >
        static constexpr void change_value ( union_type< T > _type_, tagged_union< Ts... > & _storage_, Args&&... _args_ )
        {
                change_value_impl( _type_, _storage_, UTI_FWD( _args_ )... ) ;
        }
private:
        template< typename T, typename... Ts, typename... Args >
        static constexpr auto change_value_impl ( union_type< T > _type_, tagged_union< Ts... > & _storage_, Args&&... _args_ )
                noexcept( is_nothrow_constructible_v< T, Args&&... > )
        {
                destroy( _storage_ ) ;

                _storage_.emplace( _type_, UTI_FWD( _args_ )... ) ;
        }
} ;

template< typename Fallback, typename... Ts >
using fallback_variant = basic_variant< fallback_variant_policy< Fallback >, Fallback, Ts... > ;


class optional_variant_policy
{
public:
        using allow_empty = true_type ;

        template< typename T, typename... Ts, typename... Args >
        static constexpr void change_value ( union_type< T > _type_, tagged_union< Ts... > & _storage_, Args&&... _args_ )
        {
                destroy( _storage_ ) ;
                _storage_.emplace( _type_, UTI_FWD( _args_ )... ) ;
        }
} ;


namespace _detail
{


template< bool ForceNonEmpty >
class non_empty_variant_policy
{
public:
        using allow_empty = false_type ;

        template< typename T, typename... Ts, typename... Args >
        static constexpr void change_value ( union_type< T > _type_, tagged_union< Ts... > & _storage_, Args&&... _args_ )
        {
                change_value_impl( _type_, _storage_, UTI_FWD( _args_ )... ) ;
        }
private:
        template< typename T, typename... Ts >
        static constexpr void move_emplace ( union_type< T > _type_, tagged_union< Ts... > & _storage_, T && _obj_ ) noexcept( ForceNonEmpty )
        {
                _storage_.emplace( _type_, UTI_MOVE( _obj_ ) ) ;
        }

        template< typename T, typename... Ts >
        static constexpr void change_value_impl ( union_type< T > _type_, tagged_union< Ts... > & _storage_, T && _obj_ )
        {
                destroy( _storage_ ) ;
                move_emplace( _type_, _storage_, UTI_MOVE( _obj_ ) ) ;
        }
        template< typename T, typename... Ts, typename... Args >
        static constexpr void change_value_impl ( union_type< T > _type_, tagged_union< Ts... > & _storage_, Args&&... _args_ )
                noexcept( is_nothrow_constructible_v< T, Args&&... > )
        {
                destroy( _storage_ ) ;
                _storage_.emplace( _type_, UTI_FWD( _args_ )... ) ;
        }
} ;


} // namespace _detail


using rarely_empty_variant_policy = _detail::non_empty_variant_policy< false > ;
using  never_empty_variant_policy = _detail::non_empty_variant_policy<  true > ;


namespace _detail
{


template< typename... Ts >
struct select_variant_policy : type_identity< basic_variant< rarely_empty_variant_policy, Ts... > > {} ;

template< typename... Ts >
struct select_variant_policy< nullvar_t, Ts... > : type_identity< basic_variant< optional_variant_policy, Ts... > > {} ;


} // namespace _detail


template< typename... Ts >
using variant = typename _detail::select_variant_policy< Ts... >::type ;


} // namespace uti

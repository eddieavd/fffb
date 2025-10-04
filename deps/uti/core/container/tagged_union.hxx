//
//
//      uti
//      core/container/tagged_union.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/type/traits.hxx>
#include <uti/core/meta/concepts.hxx>
#include <uti/core/meta/list.hxx>
#include <uti/core/algo/mem.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>


namespace uti
{


namespace _detail
{


template< typename... Ts >
class aligned_union
{
public:
        static constexpr auto  size_val { max_size_of < Ts... >() } ;
        static constexpr auto align_val { max_align_of< Ts... >() } ;

        using       byte_iterator = uti::iterator_base< u8_t      , random_access_iterator_tag > ;
        using const_byte_iterator = uti::iterator_base< u8_t const, random_access_iterator_tag > ;

        constexpr byte_iterator get () noexcept
        {
                return &storage_[ 0 ] ;
        }
        constexpr const_byte_iterator get () const noexcept
        {
                return &storage_[ 0 ] ;
        }
private:
        alignas( align_val ) u8_t storage_ [ size_val ] ;
} ;


using union_type_id = ssize_t ;


template< typename Union > struct destroy_union ;
template< typename Union > struct    copy_union ;
template< typename Union > struct    move_union ;


template< typename T >
using iterator_type_for = uti::iterator_base< T, random_access_iterator_tag > ;


} // namespace _detail


template< typename T >
struct union_type
{
        constexpr union_type () {}
} ;

template< typename... Ts > using union_types = meta::type_list< Ts... > ;


template< typename... Ts >
class tagged_union
{
        using trivial = conjunction< is_trivially_copyable< Ts >... > ;

        template< typename Union > friend struct _detail::destroy_union ;
        template< typename Union > friend struct _detail::   copy_union ;
        template< typename Union > friend struct _detail::   move_union ;
public:
        using value_types = union_types< decay_t< Ts >... > ;

        class type_id
        {
        public:
                template< typename T >
                static constexpr bool is_valid () noexcept
                {
                        return index_of_v< T, Ts... > >= 0 ;
                }

                constexpr type_id () noexcept : id_{ -1 } {}

                template< typename T >
                constexpr type_id ( union_type< T > ) noexcept : id_{ index_of_v< T, Ts... > } {}

                explicit constexpr operator bool () const noexcept { return id_ != type_id() ; }

                constexpr bool operator== ( type_id const & _other_ ) const noexcept { return id_ == _other_.id_ ; }
                constexpr bool operator!= ( type_id const & _other_ ) const noexcept { return id_ != _other_.id_ ; }
                constexpr bool operator<  ( type_id const & _other_ ) const noexcept { return id_ <  _other_.id_ ; }
                constexpr bool operator<= ( type_id const & _other_ ) const noexcept { return id_ <= _other_.id_ ; }
                constexpr bool operator>  ( type_id const & _other_ ) const noexcept { return id_ >  _other_.id_ ; }
                constexpr bool operator>= ( type_id const & _other_ ) const noexcept { return id_ >= _other_.id_ ; }
        private:
                _detail::union_type_id id_ ;

                explicit constexpr type_id ( _detail::union_type_id _id_ ) : id_( _id_ ) {}
        } ;

        static constexpr type_id invalid_type { type_id() } ;

        constexpr  tagged_union () noexcept = default ;
        constexpr ~tagged_union () noexcept = default ;

        constexpr tagged_union             ( tagged_union const & ) = delete ;
        constexpr tagged_union & operator= ( tagged_union const & ) = delete ;

        template< typename T, typename... Args >
                requires( meta::one_of< T, Ts... > && meta::constructible_from< T, Args... > )
        constexpr void emplace ( union_type< T >, Args&&... _args_ ) noexcept
        {
                constexpr auto idx = type_id( union_type< T >{} ) ;

                ::uti::construct( static_cast< _detail::iterator_type_for< T > >( storage_.get() ), UTI_FWD( _args_ )... ) ;
                current_type_ = idx ;
        }

        template< typename T >
        constexpr void destroy ( union_type< T > _type_ ) noexcept
        {
                _check( _type_ ) ;
                value( _type_ ).~T() ;
                current_type_ = invalid_type ;
        }

        type_id const & type () const noexcept { return current_type_ ; }

        bool has_value () const noexcept { return type() != invalid_type ; }

        template< typename T >
        T & value ( union_type< T > _type_ ) & noexcept
        {
                _check( _type_ ) ;
                return *static_cast< _detail::iterator_type_for< T > >( storage_.get() ) ;
        }
        template< typename T >
        T const & value ( union_type< T > _type_ ) const & noexcept
        {
                _check( _type_ ) ;
                return *static_cast< _detail::iterator_type_for< T const > >( storage_.get() ) ;
        }
        template< typename T >
        T && value ( union_type< T > _type_ ) && noexcept
        {
                _check( _type_ ) ;
                return UTI_MOVE( *static_cast< _detail::iterator_type_for< T > >( storage_.get() ) ) ;
        }
        template< typename T >
        T const && value ( union_type< T > _type_ ) const && noexcept
        {
                _check( _type_ ) ;
                return UTI_MOVE( *static_cast< _detail::iterator_type_for< T const > >( storage_.get() ) ) ;
        }
private:
        using storage_t = _detail::aligned_union< Ts... > ;

        storage_t    storage_ ;
        type_id current_type_ ;

        template< typename T >
        constexpr void _check ( union_type< T > _type_ ) const noexcept
        {
                UTI_CEXPR_ASSERT( current_type_ == _type_, "uti::tagged_union::_check: failed" ) ;
        }
} ;


template< typename... Ts >
constexpr typename tagged_union< Ts... >::type_id tagged_union< Ts... >::invalid_type ;


namespace _detail
{


template< typename Fn, typename Union, typename Ts, typename... Args > class apply_union ;

template< typename Fn, typename Union, typename... Ts, typename... Args >
class apply_union< Fn, Union, union_types< Ts... >, Args... >
{
public:
        static constexpr void apply ( Union && _union_, Fn&& _fn_, Args&&... _args_ )
        {
                apply_impl( typename decay_t< Union >::value_types{}, UTI_FWD( _union_ ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
        }
private:
        template< typename T >
        static constexpr auto call ( int, Union&& _union_, Fn&& _fn_, Args&&... _args_ )
                -> decltype( UTI_FWD( _fn_ )( UTI_FWD( _union_ ).value( union_type< T >{} ), UTI_FWD( _args_ )... ) )
        {
                return UTI_FWD( _fn_ )( UTI_FWD( _union_ ).value( union_type< T >{} ), UTI_FWD( _args_ )... ) ;
        }

        template< typename T >
        static constexpr void call ( short, Union&&, Fn&&, Args&&... ) {}

        template< typename... Ts1 >
        static constexpr void apply_impl ( union_types< Ts1... >, Union&& _union_, Fn&& _fn_, Args&&... _args_ )
        {
                ( ... || [ & ]
                {
                        if( _union_.type() == union_type< Ts1 >{} )
                        {
                                call< Ts1 >( 0, UTI_FWD( _union_ ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
                                return true ;
                        }
                        return false ;
                }() ) ;
        }
} ;


} // namespace _detail


template< typename... Ts, typename Fn, typename... Args >
constexpr void apply ( tagged_union< Ts... > & _union_, Fn&& _fn_, Args&&... _args_ )
{
        _detail::apply_union< Fn&&, decltype( _union_ ), typename tagged_union< Ts... >::value_types, Args&&... >
                ::apply( _union_, UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename... Ts, typename Fn, typename... Args >
constexpr void apply ( tagged_union< Ts... > const & _union_, Fn&& _fn_, Args&&... _args_ )
{
        _detail::apply_union< Fn&&, decltype( _union_ ), typename tagged_union< Ts... >::value_types, Args&&... >
                ::apply( _union_, UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename... Ts, typename Fn, typename... Args >
constexpr void apply ( tagged_union< Ts... > && _union_, Fn&& _fn_, Args&&... _args_ )
{
        _detail::apply_union< Fn&&, decltype( _union_ ), typename tagged_union< Ts... >::value_types, Args&&... >
                ::apply( UTI_MOVE( _union_ ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}

template< typename... Ts, typename Fn, typename... Args >
constexpr void apply ( tagged_union< Ts... > const && _union_, Fn&& _fn_, Args&&... _args_ )
{
        _detail::apply_union< Fn&&, decltype( _union_ ), typename tagged_union< Ts... >::value_types, Args&&... >
                ::apply( UTI_MOVE( _union_ ), UTI_FWD( _fn_ ), UTI_FWD( _args_ )... ) ;
}


namespace _detail
{


template< typename Union >
struct destroy_union
{
        struct visitor
        {
                template< typename T >
                constexpr void operator() ( T const &, Union & _union_ )
                {
                        _union_.destroy( union_type< T >{} ) ;
                }
        } ;

        static constexpr void destroy ( Union & _union_ ) noexcept
        {
                if constexpr( Union::trivial::value )
                {
                        _union_.current_type_ = Union::invalid_type ;
                }
                else
                {
                        apply( _union_, visitor{}, _union_ ) ;
                }
        }
} ;

template< typename Union >
struct copy_union
{
        struct visitor
        {
                template< typename T >
                constexpr void operator() ( T const & _value_, Union & _dest_ )
                {
                        _dest_.emplace( union_type< T >{}, _value_ ) ;
                }
        } ;

        static constexpr void copy ( Union & _dest_, Union const & _src_ ) noexcept
        {
                if constexpr( Union::trivial::value )
                {
                        _dest_.     storage_ = _src_.     storage_ ;
                        _dest_.current_type_ = _src_.current_type_ ;
                }
                else
                {
                        apply( _src_, visitor{}, _dest_ ) ;
                }
        }
} ;

template< typename Union >
struct move_union
{
        struct visitor
        {
                template< typename T >
                constexpr void operator() ( T && _value_, Union & _dest_ )
                {
                        _dest_.emplace( union_type< T >{}, UTI_MOVE( _value_ ) ) ;
                }
        } ;

        static constexpr void move ( Union & _dest_, Union && _src_ ) noexcept
        {
                if constexpr( Union::trivial::value )
                {
                        _dest_.     storage_ = _src_.     storage_ ;
                        _dest_.current_type_ = _src_.current_type_ ;
                }
                else
                {
                        apply( UTI_MOVE( _src_ ), visitor{}, _dest_ ) ;
                }
        }
} ;


} // namespace _detail


template< typename... Ts >
constexpr void destroy ( tagged_union< Ts... > & _union_ ) noexcept
{
        _detail::destroy_union< tagged_union< Ts... > >::destroy( _union_ ) ;
}

template< typename... Ts >
constexpr void copy ( tagged_union< Ts... > & _dest_, tagged_union< Ts... > const & _src_ )
{
        UTI_CEXPR_ASSERT( !_dest_.has_value(), "uti::tagged_union::copy: destination union not empty" ) ;
        _detail::copy_union< tagged_union< Ts... > >::copy( _dest_, _src_ ) ;
}

template< typename... Ts >
constexpr void move ( tagged_union< Ts... > & _dest_, tagged_union< Ts... > && _src_ )
{
        UTI_CEXPR_ASSERT( !_dest_.has_value(), "uti::tagged_union::move: destination union not empty" ) ;
        _detail::move_union< tagged_union< Ts... > >::move( _dest_, UTI_MOVE( _src_ ) ) ;
}


} // namespace uti

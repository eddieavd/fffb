//
//
//      uti
//      core/container/variant_vector.hxx
//

#pragma once

#include <uti/core/container/tuple.hxx>
#include <uti/core/container/vector.hxx>
#include <uti/core/meta/list.hxx>
#include <uti/core/type/sequence.hxx>


namespace uti
{


namespace _detail
{


template< typename T >
using iterator_type_for = iterator_base< T, random_access_iterator_tag > ;


} // namespace _detail


template< typename Resource, typename... Ts >
class variant_vector
{
public:
        using value_types = meta::type_list< Ts... > ;
private:
        static constexpr ssize_t max_elem_align { max_align_of< Ts... >() } ;
        static constexpr ssize_t min_elem_align { min_align_of< Ts... >() } ;

        static constexpr ssize_t max_elem_size { max_size_of< Ts... >() } ;
        static constexpr ssize_t min_elem_size { min_size_of< Ts... >() } ;
public:
        using    resource_type = Resource ;
        using _resource_traits = uti::resource_traits< resource_type > ;
        using       block_type = typename _resource_traits::block_type ;

        using        size_type =  size_t    ;
        using       ssize_type = ssize_t    ;
        using  difference_type = ssize_type ;

        using        byte_pointer = typename block_type::       pointer ;
        using  const_byte_pointer = typename block_type:: const_pointer ;
        using       byte_iterator = typename block_type::      iterator ;
        using const_byte_iterator = typename block_type::const_iterator ;

//      using  internal_resource_type = resource_type ;
        using  internal_resource_type = malloc_resource ;

        using  offsets_allocator_type = uti::allocator< ssize_t, internal_resource_type > ;
        using    types_allocator_type = uti::allocator<   i16_t, internal_resource_type > ;

        using    offsets_storage_type = uti::vector< ssize_t, offsets_allocator_type > ;
        using      types_storage_type = uti::vector<   i16_t,   types_allocator_type > ;

        constexpr variant_vector () noexcept = default ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr variant_vector ( type_identity< T >, ssize_type _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::forward_iterator Iter >
                requires meta::one_of< iter_value_t< Iter >, Ts... >
        constexpr variant_vector ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::simple_container Other >
                requires meta::one_of< typename Other::value_type, Ts... >
        constexpr variant_vector ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr variant_vector             ( variant_vector const &  ) UTI_NOEXCEPT_UNLESS_BADALLOC requires( conjunction_v< is_copy_constructible< Ts >... > ) ;
        constexpr variant_vector             ( variant_vector       && )     noexcept                 requires( conjunction_v< is_move_constructible< Ts >... > ) ;
        constexpr variant_vector & operator= ( variant_vector const &  ) UTI_NOEXCEPT_UNLESS_BADALLOC requires( conjunction_v< is_copy_constructible< Ts >... > ) ;
        constexpr variant_vector & operator= ( variant_vector       && )     noexcept                 requires( conjunction_v< is_move_constructible< Ts >... > ) ;

        constexpr ~variant_vector () noexcept { reset() ; }

////////////////////////////////////////////////////////////////////////////////

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void push_back ( T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void push_back ( T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T, typename... Args >
                requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args... >
        constexpr void emplace_back ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        template< meta::forward_iterator Iter >
                requires meta::one_of< iter_value_t< Iter >, Ts... >
        constexpr void append ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::simple_container Other >
                requires meta::one_of< typename Other::value_type, Ts... >
        constexpr void append ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void insert ( ssize_type _position_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void insert ( ssize_type _position_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T, typename... Args >
                requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args... >
        constexpr void insert ( ssize_type _position_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void replace ( ssize_type _idx_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void replace ( ssize_type _idx_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T, typename... Args >
                requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args&&... >
        constexpr void replace ( ssize_type _idx_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void replace_repack ( ssize_type _idx_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void replace_repack ( ssize_type _idx_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T, typename... Args >
                requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args&&... >
        constexpr void replace_repack ( ssize_type _idx_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        constexpr void repack      (                  ) noexcept { repack_from( 0 ) ; }
        constexpr void repack_from ( ssize_type _idx_ ) noexcept ;

////////////////////////////////////////////////////////////////////////////////

        constexpr void erase        ( ssize_type _position_ ) noexcept ;
        constexpr void erase_repack ( ssize_type _position_ ) noexcept ;

////////////////////////////////////////////////////////////////////////////////

        constexpr void reserve_bytes ( ssize_type _bytes_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void reserve ( ssize_type _count_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr void reserve_additional ( ssize_type _count_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

////////////////////////////////////////////////////////////////////////////////

        constexpr void clear () noexcept ;
        constexpr void reset () noexcept ;

////////////////////////////////////////////////////////////////////////////////

        template< typename T >
                requires meta::one_of< T, Ts... >
        UTI_NODISCARD constexpr ssize_type capacity       () const noexcept { return capacity_bytes() / ssizeof( T ) ; }
        UTI_NODISCARD constexpr ssize_type capacity_bytes () const noexcept { return storage_.size_ ; }
        UTI_NODISCARD constexpr ssize_type     size_bytes () const noexcept { return _find_end( size_ - 1 ) - storage_.begin() ; }

        UTI_NODISCARD constexpr ssize_type  size () const noexcept { return size_      ; }
        UTI_NODISCARD constexpr       bool empty () const noexcept { return size_ == 0 ; }
        UTI_NODISCARD constexpr       bool  null () const noexcept { return !storage_  ; }

////////////////////////////////////////////////////////////////////////////////

        template< typename T > UTI_NODISCARD constexpr T       & get ( ssize_type _idx_ )       noexcept { return *get_iter< T >( _idx_ ) ; }
        template< typename T > UTI_NODISCARD constexpr T const & get ( ssize_type _idx_ ) const noexcept { return *get_iter< T >( _idx_ ) ; }

        template< typename T > UTI_NODISCARD constexpr _detail::iterator_type_for< T       > get_iter ( ssize_type _idx_ )       noexcept ;
        template< typename T > UTI_NODISCARD constexpr _detail::iterator_type_for< T const > get_iter ( ssize_type _idx_ ) const noexcept ;

////////////////////////////////////////////////////////////////////////////////

        template< typename Self, typename Visitor, typename... Args >
        constexpr void visit ( this Self && self, ssize_type _idx_, Visitor&& _visitor_, Args&&... _args_ ) noexcept
        {
                [ & ]< ssize_type... Idxs >( uti::index_sequence< Idxs... > )
                {
                        ( ... ||
                        [ & ]
                        {
                                using       raw_type =             meta::list::at_t< Idxs, value_types >   ;
                                using effective_type = like< Self, meta::list::at_t< Idxs, value_types > > ;

                                if constexpr( meta::invocable< Visitor, effective_type, ssize_type, Args&&... > )
                                {
                                        if( UTI_FWD( self ).types_[ _idx_ ] == Idxs )
                                        {
                                                _visitor_( UTI_FWD( self ).template get< raw_type >( _idx_ ), _idx_, UTI_FWD( _args_ )... ) ;
                                                return true ;
                                        }
                                }
                                if constexpr( meta::invocable< Visitor, effective_type, Args&&... > )
                                {
                                        if( UTI_FWD( self ).types_[ _idx_ ] == Idxs )
                                        {
                                                _visitor_( UTI_FWD( self ).template get< raw_type >( _idx_ ), UTI_FWD( _args_ )... ) ;
                                                return true ;
                                        }
                                }
                                return false ;
                        }() ) ;
                }( uti::make_index_sequence< sizeof...( Ts ) >{} ) ;
        }

////////////////////////////////////////////////////////////////////////////////

        template< typename Self, typename Visitor, typename... Args >
        constexpr void for_each ( this Self && self, Visitor&& _visitor_, Args&&... _args_ ) noexcept
        {
                for( ssize_type i = 0; i < UTI_FWD( self ).size(); ++i )
                {
                        UTI_FWD( self ).visit( i, UTI_FWD( _visitor_ ), UTI_FWD( _args_ )... ) ;
                }
        }

////////////////////////////////////////////////////////////////////////////////

        template< typename Self, typename Visitor, typename... Args >
        constexpr void for_range ( this Self &&    self ,
                                   ssize_type const _x_ ,
                                   ssize_type const _y_ ,
                                   Visitor&&  _visitor_ ,
                                   Args&&...     _args_ ) noexcept
        {
                UTI_CEXPR_ASSERT( 0 <= _x_ && _x_ <= _y_ && _y_ <= UTI_FWD( self ).size(),
                                  "uti::variant_vector::for_range: index out of bounds" ) ;

                for( ssize_type i = _x_; i < _y_; ++i )
                {
                        UTI_FWD( self ).visit( i, UTI_FWD( _visitor_ ), UTI_FWD( _args_ )... ) ;
                }
        }

////////////////////////////////////////////////////////////////////////////////

        template< typename Self, typename Visitor, typename... Args >
        constexpr void for_range_back ( this Self &&    self ,
                                        ssize_type const _y_ ,
                                        ssize_type const _x_ ,
                                        Visitor&&  _visitor_ ,
                                        Args&&...     _args_ ) noexcept
        {
                UTI_CEXPR_ASSERT( 0 <= _x_ && _x_ <= _y_ && _y_ <= UTI_FWD( self ).size(),
                                  "uti::variant_vector::for_range_back: index out of bounds" ) ;

                for( ssize_type i = _y_; i > _x_; --i )
                {
                        UTI_FWD( self ).visit( i, UTI_FWD( _visitor_ ), UTI_FWD( _args_ )... ) ;
                }
        }

////////////////////////////////////////////////////////////////////////////////

        UTI_NODISCARD constexpr memory_footprint memory_usage () const noexcept
        {
                memory_footprint footprint
                {
                        .         static_usage_ = ssizeof( *this ) ,
                        .top_lvl_dynamic_usage_ = storage_.size_ ,
                        .low_lvl_dynamic_usage_ = 0
                } ;
                footprint.top_lvl_dynamic_usage_ += offsets_.memory_usage().dynamic_footprint() ;
                footprint.top_lvl_dynamic_usage_ +=   types_.memory_usage().dynamic_footprint() ;

                if constexpr( disjunction_v< integral_constant< meta::memory_reporter< Ts > >... > )
                {
                        for_each( [ & ]( auto const & elem )
                        {
                                using type = remove_cvref_t< decltype( elem ) > ;

                                if constexpr( meta::memory_reporter< type > )
                                {
                                        footprint.low_lvl_dynamic_usage_ += elem.memory_usage().dynamic_footprint() ;
                                }
                        } ) ;
                }
                return footprint ;
        }
private:
        ssize_type              size_ {          0 } ;
        block_type           storage_ { nullptr, 0 } ;
        offsets_storage_type offsets_ {            } ;
          types_storage_type   types_ {            } ;

        constexpr void _reserve ( ssize_type _bytes_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename T >
                requires meta::one_of< T, Ts... >
        constexpr bool _can_fit ( ssize_type _count_ ) const noexcept ;

        constexpr       byte_iterator _addr_of ( ssize_type _idx_ )       noexcept ;
        constexpr const_byte_iterator _addr_of ( ssize_type _idx_ ) const noexcept ;

        constexpr       byte_iterator _find_end ( ssize_type _idx_ )       noexcept ;
        constexpr const_byte_iterator _find_end ( ssize_type _idx_ ) const noexcept ;

        template< typename T > constexpr       byte_iterator _align_for (       byte_iterator _ptr_ )       noexcept ;
        template< typename T > constexpr const_byte_iterator _align_for ( const_byte_iterator _ptr_ ) const noexcept ;

        template< typename T > constexpr ssize_type _padding_for ( const_byte_iterator _ptr_ ) const noexcept ;

        constexpr void _tag_block ( byte_iterator _begin_, ssize_type _size_, char _tag_ ) const noexcept
        {
                block_t< typename byte_iterator::value_type > block( _begin_, _size_ ) ;

                for( auto & b : block )
                {
                        b = _tag_ ;
                }
        }
} ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// ctor reserve

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr
variant_vector< Resource, Ts... >::variant_vector ( type_identity< T >, ssize_type _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        /// small memory layout optimization
        /// since we use the same memory resource for the metadata and the main storage,
        /// we can force the metadata vectors to reallocate when we want
        /// and allocate main storage right after the metadata.
        ///
        /// by overallocating the metadata vectors,
        /// we ensure that they won't have to reallocate for the first few push backs
        /// which cause a reallocation of the main storage.
        /// this increases the chances of the main storage being able to reallocate in place
        /// depending on the state of our memory resource
        ///
        /// side note - u8_ts should be enough for the types metadata for all reasonable use-cases
        /// and the offsets can be stored in a custom data structure
        /// which dynamically picks the smalles type large enough to fit the current offset values.
        /// with this taken into account, the memory overhead of the metadata will decrease
        /// making the below optimization less costly

        offsets_.reserve( _capacity_ * 2 ) ;
        types_  .reserve( _capacity_ * 2 ) ;

        _reserve( _capacity_ * ssizeof( T ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// ctor from range

template< typename Resource, typename... Ts >
template< meta::forward_iterator Iter >
        requires meta::one_of< iter_value_t< Iter >, Ts... >
constexpr
variant_vector< Resource, Ts... >::variant_vector ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        using type_t = iter_value_t< Iter > ;
        using iter_t = _detail::iterator_type_for< type_t > ;

        ssize_type count = ::uti::distance( _begin_, _end_ ) ;

        offsets_.reserve( count * 2 ) ;
        types_  .reserve( count * 2 ) ;

        reserve< type_t >( count ) ;

        if( capacity< type_t >() < count ) UTI_UNLIKELY return ;

        iter_t iter { storage_.begin() } ;

        for( ssize_type i = 0; i < count && _begin_ != _end_; ++i, ++_begin_ )
        {
                offsets_.push_back( static_cast< const_byte_iterator>( iter ) - storage_.begin() ) ;
                types_  .push_back( index_of_v< type_t, Ts... > ) ;

                ::uti::construct( iter, *_begin_ ) ;
                ++size_ ;

                iter = _align_for< type_t >( ++iter ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// ctor from container

template< typename Resource, typename... Ts >
template< meta::simple_container Other >
        requires meta::one_of< typename Other::value_type, Ts... >
constexpr
variant_vector< Resource, Ts... >::variant_vector ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : variant_vector( _other_.begin(), _other_.end() )
{}

////////////////////////////////////////////////////////////////////////////////
/// ctor copy

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::variant_vector ( variant_vector const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        requires( conjunction_v< is_copy_constructible< Ts >... > )
{
        reserve_bytes( _other_.size_bytes() ) ;

        if( storage_.size_ < _other_.size_bytes() ) UTI_UNLIKELY return ;

        if constexpr( conjunction_v< is_trivially_relocatable< Ts >... > )
        {
                ::uti::copy( _other_.storage_.begin(),
                             _other_.storage_.begin() + _other_.size_bytes(),
                                     storage_.begin()
                ) ;
        }
        else
        {
                _other_.for_each(
                        [ & ]( auto const & val, ssize_type idx )
                        {
                                using type = remove_cvref_t< decltype( val ) > ;
                                using iter = _detail::iterator_type_for< type > ;

                                if constexpr( !is_trivially_relocatable_v< type > )
                                {
                                        ::uti::construct< iter >( storage_.begin() + _other_.offsets_[ idx ], val ) ;
                                }
                                else
                                {
                                        ::uti::copy( &val, &val + 1, storage_.begin() + _other_.offsets_[ idx ] ) ;
                                }
                        }
                ) ;
        }
        size_    = _other_.   size_ ;
        offsets_ = _other_.offsets_ ;
        types_   = _other_.  types_ ;
}

////////////////////////////////////////////////////////////////////////////////
/// ctor move

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::variant_vector ( variant_vector && _other_ ) noexcept
        requires( conjunction_v< is_move_constructible< Ts >... > )
        : size_   (           _other_.   size_   )
        , storage_(           _other_.storage_   )
        , offsets_( UTI_MOVE( _other_.offsets_ ) )
        , types_  ( UTI_MOVE( _other_.  types_ ) )
{
        _other_.   size_ =            0   ;
        _other_.storage_ = { nullptr, 0 } ;
}

////////////////////////////////////////////////////////////////////////////////
/// assign copy

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... > &
variant_vector< Resource, Ts... >::operator= ( variant_vector const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        requires( conjunction_v< is_copy_constructible< Ts >... > )
{
        clear() ;
        reserve_bytes( _other_.size_bytes() ) ;

        if( capacity_bytes() < _other_.size_bytes() ) UTI_UNLIKELY return *this ;

        _other_.for_each(
                [ & ]( auto const & val, ssize_type idx )
                {
                        using type = remove_cvref_t< decltype( val ) > ;
                        using iter = _detail::iterator_type_for< type > ;

                        if constexpr( !is_trivially_relocatable_v< type > )
                        {
                                ::uti::construct< iter >( storage_.begin() + _other_.offsets_[ idx ], val ) ;
                        }
                        else
                        {
                                ::uti::copy( &val, &val + 1, storage_.begin() + _other_.offsets_[ idx ] ) ;
                        }
                }
        ) ;
        size_    = _other_.   size_ ;
        offsets_ = _other_.offsets_ ;
        types_   = _other_.  types_ ;

        return *this ;
}

////////////////////////////////////////////////////////////////////////////////
/// assign move

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... > &
variant_vector< Resource, Ts... >::operator= ( variant_vector && _other_ ) noexcept
        requires( conjunction_v< is_move_constructible< Ts >... > )
{
        reset() ;

        size_    =           _other_.   size_   ;
        storage_ =           _other_.storage_   ;
        offsets_ = UTI_MOVE( _other_.offsets_ ) ;
        types_   = UTI_MOVE( _other_.  types_ ) ;

        _other_.   size_ =            0   ;
        _other_.storage_ = { nullptr, 0 } ;

        return *this ;
}

////////////////////////////////////////////////////////////////////////////////
/// reset

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::reset () noexcept
{
        clear() ;
        _resource_traits::deallocate( storage_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// clear

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::clear () noexcept
{
        for_each( []( auto & val )
        {
                using type = remove_cvref_t< decltype( val ) > ;

                if constexpr( !is_trivially_destructible_v< type > )
                {
                        ::uti::destroy( &val ) ;
                }
        } ) ;
        size_ = 0 ;
        offsets_.clear() ;
        types_  .clear() ;
}

////////////////////////////////////////////////////////////////////////////////
/// push_back copy

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::push_back ( T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back< T, T const & >( _val_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// push_back move

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::push_back ( T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back< T, T && >( UTI_MOVE( _val_ ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// emplace_back

template< typename Resource, typename... Ts >
template< typename T, typename... Args >
        requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args... >
constexpr void
variant_vector< Resource, Ts... >::emplace_back ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        using iter_t = _detail::iterator_type_for< T > ;

        iter_t dest { nullptr } ;

        ssize_type offset { 0 } ;
        ssize_type   type { index_of_v< T, Ts... > } ;

        if( null() || empty() )
        {
                offsets_.reserve( 4 ) ;
                types_  .reserve( 4 ) ;

                _reserve( ssizeof( T ) ) ;
                dest = storage_.begin() ;
        }
        else
        {
                if( !_can_fit< T >( 1 ) )
                {
                        reserve_bytes( capacity_bytes() * 2 ) ;
                        reserve_additional< T >( 1 ) ;
                }
                if( !_can_fit< T >( 1 ) ) UTI_UNLIKELY return ;

                dest   = _align_for< T >( _find_end( size_ - 1 ) ) ;
                offset = static_cast< const_byte_iterator >( dest ) - storage_.begin() ;
        }
        if( !dest ) UTI_UNLIKELY return ;

        ::uti::construct( dest, UTI_FWD( _args_ )... ) ;
        ++size_ ;

        offsets_.push_back( offset ) ;
        types_  .push_back(   type ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// append range

template< typename Resource, typename... Ts >
template< meta::forward_iterator Iter >
        requires meta::one_of< iter_value_t< Iter >, Ts... >
constexpr void
variant_vector< Resource, Ts... >::append ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        using type = iter_value_t< Iter > ;

        reserve_additional< type >( ::uti::distance( _begin_, _end_ ) ) ;

        /// TODO: emplace_back is too expensive for this, they're all the same type, do it manually

        for( ; _begin_ != _end_; ++_begin_ )
        {
                emplace_back< type >( *_begin_ ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////
/// append container

template< typename Resource, typename... Ts >
template< meta::simple_container Other >
        requires meta::one_of< typename Other::value_type, Ts... >
constexpr void
variant_vector< Resource, Ts... >::append ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        append( _other_.begin(), _other_.end() ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// insert copy

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::insert ( ssize_type _position_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        insert< T, T const & >( _position_, _val_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// insert move

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::insert ( ssize_type _position_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        insert< T, T && >( _position_, UTI_MOVE( _val_ ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// insert inplace

template< typename Resource, typename... Ts >
template< typename T, typename... Args >
        requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args... >
constexpr void
variant_vector< Resource, Ts...>::insert ( ssize_type _position_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        using iter_t = _detail::iterator_type_for< T > ;

        if( empty() || _position_ >= size() ) return emplace_back< T, Args&&... >( UTI_FWD( _args_ )... ) ;

        if( _position_ < 0 ) _position_ = 0 ;

              byte_iterator dest = _align_for< T >( _find_end( _position_ - 1 ) ) ;
        const_byte_iterator next = _addr_of( _position_ ) ;

        bool fits_in_padding = ( ( next - dest ) >= ssizeof( T ) ) ;

        if( fits_in_padding )
        {
                ::uti::construct< iter_t >( dest, UTI_FWD( _args_ )... ) ;
                ++size_ ;

                offsets_.insert( _position_, dest - storage_.begin() ) ;
                types_  .insert( _position_, index_of_v< T, Ts... > ) ;
        }
        else
        {
                /// lord have mercy

                byte_iterator iter { dest + ssizeof( T ) } ;
                offsets_storage_type new_offsets( size_ - _position_ ) ;
                offsets_storage_type elem_sizes ( size_ - _position_ ) ;

                for_range( _position_, size_, [ & ]( auto const & elem )
                {
                        using type = remove_cvref_t< decltype( elem ) > ;

                        iter = _align_for< type >( iter ) ;
                        new_offsets.push_back( iter - storage_.begin() ) ;
                        elem_sizes.push_back( ssizeof( type ) ) ;
                        iter += ssizeof( type ) ;
                } ) ;
                ssize_type new_cap_requirement { new_offsets.back() + elem_sizes.back() } ;

                if( new_cap_requirement > capacity_bytes() )
                {
                        variant_vector new_vec ;
                        new_vec.reserve_bytes( new_cap_requirement ) ;

                        constexpr auto _move_destroy = []( auto & elem, auto & new_vec )
                        {
                                using type_t = remove_cvref_t< decltype( elem ) > ;
                                using iter_t = _detail::iterator_type_for< type_t > ;

                                new_vec.template emplace_back< type_t, type_t && >( UTI_MOVE( elem ) ) ;

                                if constexpr( !is_trivially_destructible_v< type_t > )
                                {
                                        ::uti::destroy< iter_t >( &elem ) ;
                                }
                        } ;

                        for_range( 0, _position_, _move_destroy, new_vec ) ;
                        new_vec.emplace_back< T, Args&&... >( UTI_FWD( _args_ )... ) ;
                        for_range( _position_, size_, _move_destroy, new_vec ) ;

                        *this = UTI_MOVE( new_vec ) ;
                        return ;
                }
                else
                {
                        ssize_type i { new_offsets.size() - 1 } ;

                        for_range_back( size_ - 1, _position_ - 1, [ & ]( auto & elem )
                        {
                                using type_t = remove_cvref_t< decltype( elem ) > ;
                                using iter_t = _detail::iterator_type_for< type_t > ;

                                type_t tmp = UTI_MOVE( elem ) ;

                                if constexpr( !is_trivially_destructible_v< type_t > ) ::uti::destroy( &elem ) ;

                                ::uti::construct< iter_t >( storage_.begin() + new_offsets[ i ], UTI_MOVE( tmp ) ) ;

                                --i ;
                        } ) ;
                        for( i = _position_; i < size_; ++i )
                        {
                                offsets_[ i ] = new_offsets[ i - _position_ ] ;
                        }
                        ::uti::construct< iter_t >( dest, UTI_FWD( _args_ )... ) ;
                        ++size_ ;

                        offsets_.insert( _position_, dest - storage_.begin() ) ;
                        types_  .insert( _position_, index_of_v< T, Ts... > ) ;
                }
        }
}

////////////////////////////////////////////////////////////////////////////////
/// replace copy

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::replace ( ssize_type _idx_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        replace< T, T const & >( _idx_, _val_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// replace move

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::replace ( ssize_type _idx_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        replace< T, T && >( _idx_, UTI_MOVE( _val_ ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// replace inplace

template< typename Resource, typename... Ts >
template< typename T, typename... Args >
        requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args&&... >
constexpr void
variant_vector< Resource, Ts... >::replace ( ssize_type _idx_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::replace: index out of bounds" ) ;

        if( _idx_ == size_ - 1 )
        {
                erase( size_ - 1 ) ;
                emplace_back< T >( UTI_FWD( _args_ )... ) ;
                return ;
        }
        constexpr auto _replace_inplace = []( auto & elem, byte_iterator dest, Args&&... args )
        {
                using oldtype = remove_cvref_t< decltype( elem ) > ;
                using olditer = _detail::iterator_type_for< oldtype > ;
                using newiter = _detail::iterator_type_for<       T > ;

                if constexpr( !is_trivially_destructible_v< oldtype > )
                {
                        ::uti::destroy< olditer >( &elem ) ;
                }
                ::uti::construct< newiter >( dest, UTI_FWD( args )... ) ;
        } ;
        [[ maybe_unused ]]
        constexpr auto _replace_expand = []( auto & elem, byte_iterator dest, [[ maybe_unused ]] Args&&... args )
        {
                ( void ) elem ;
                ( void ) dest ;
        } ;
        byte_iterator dest = _align_for< T >( _addr_of( _idx_ ) ) ;

        bool fits = ( ( _addr_of( _idx_ + 1 ) - dest ) >= ssizeof( T ) ) ;
        if( fits )
        {
                visit( _idx_, _replace_inplace, dest, UTI_FWD( _args_ )... ) ;

                offsets_[ _idx_ ] = ( dest - storage_.begin() ) ;
                types_  [ _idx_ ] = index_of_v< T, Ts... > ;
        }
        else
        {
                /// lord have mercy
        }
}

////////////////////////////////////////////////////////////////////////////////
/// replace_repack copy

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::replace_repack ( ssize_type _idx_, T const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        replace_repack< T, T const & >( _idx_, _val_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// replace_repack move

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::replace_repack ( ssize_type _idx_, T && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        replace_repack< T, T && >( _idx_, UTI_MOVE( _val_ ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// replace_repack inplace

template< typename Resource, typename... Ts >
template< typename T, typename... Args >
        requires meta::one_of< T, Ts... > && meta::constructible_from< T, Args&&... >
constexpr void
variant_vector< Resource, Ts... >::replace_repack ( ssize_type _idx_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::replace: index out of bounds" ) ;

        replace< T, Args&&... >( _idx_, UTI_FWD( _args_ )... ) ;
        repack_from( _idx_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// repack_from

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::repack_from ( ssize_type _idx_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::repack_from: index out of bounds" ) ;

        byte_iterator iter { _idx_ == 0 ? storage_.begin() : _find_end( _idx_ - 1 ) } ;

        for_range( _idx_, size(), [ & ]( auto & elem, ssize_type idx )
        {
                using type_t = remove_cvref_t< decltype( elem ) > ;
                using iter_t = _detail::iterator_type_for< type_t > ;

                iter = _align_for< type_t >( iter ) ;

                type_t tmp( UTI_MOVE( elem ) ) ;

                ::uti::construct< iter_t >( iter, UTI_MOVE( tmp ) ) ;

                offsets_[ idx ] = iter - storage_.begin() ;

                iter += ssizeof( type_t ) ;
        } ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// erase
///
/// erases element at index _position_, leaving the memory it occupied empty
///

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::erase ( ssize_type _position_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::variant_vector::erase: index out of bounds" ) ;

        visit( _position_, []( auto & elem )
        {
                using type = remove_cvref_t< decltype( elem ) > ;
                using iter = _detail::iterator_type_for< type > ;

                if constexpr( !is_trivially_destructible_v< type > )
                {
                        ::uti::destroy< iter >( &elem ) ;
                }
        } ) ;
        offsets_.erase_stable( _position_ ) ;
        types_  .erase_stable( _position_ ) ;
        --size_ ;
}

////////////////////////////////////////////////////////////////////////////////
/// erase_repack
///
/// erases element at index _position_ and repacks all following elements backwards
///

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::erase_repack ( ssize_type _position_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::variant_vector::erase: index out of bounds" ) ;
        erase( _position_ ) ;
        repack_from( _position_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// reserve_bytes

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::reserve_bytes ( ssize_type _bytes_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        _reserve( _bytes_ ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// reserve count
///
/// reserves enough memory to store _count_ number of elements of type T
///

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::reserve ( ssize_type _count_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        _reserve( _count_ * ssizeof( T ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// reserve_additional
///
/// reserves enough memory to store an additional _count_ number of elements of type T
///

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr void
variant_vector< Resource, Ts... >::reserve_additional ( ssize_type _count_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        ssize_type padding = _padding_for< T >( _find_end( size_ - 1 ) ) ;

        _reserve( capacity_bytes() + padding + _count_ * ssizeof( T ) ) ;
}

////////////////////////////////////////////////////////////////////////////////
/// get_iter

template< typename Resource, typename... Ts >
template< typename T >
UTI_NODISCARD constexpr _detail::iterator_type_for< T >
variant_vector< Resource, Ts... >::get_iter ( ssize_type _idx_ ) noexcept
{
        if( index_of_v< T, Ts... > == types_[ _idx_ ] )
        {
                return _addr_of( _idx_ ) ;
        }
        return nullptr ;
}

////////////////////////////////////////////////////////////////////////////////
/// get_iter const

template< typename Resource, typename... Ts >
template< typename T >
UTI_NODISCARD constexpr _detail::iterator_type_for< T const >
variant_vector< Resource, Ts... >::get_iter ( ssize_type _idx_ ) const noexcept
{
        if( index_of_v< T, Ts... > == types_[ _idx_ ] )
        {
                return _addr_of( _idx_ ) ;
        }
        return nullptr ;
}

////////////////////////////////////////////////////////////////////////////////
/// _can_fit

template< typename Resource, typename... Ts >
template< typename T >
        requires meta::one_of< T, Ts... >
constexpr bool
variant_vector< Resource, Ts... >::_can_fit ( ssize_type _count_ ) const noexcept
{
        const_byte_iterator aligned = _align_for< T >( _find_end( size_ - 1 ) ) ;

        ssize_type free_space = storage_.end() - aligned ;

        return static_cast< ssize_type >( _count_ * ssizeof( T ) ) <= free_space ;
}

////////////////////////////////////////////////////////////////////////////////
/// _addr_of

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::byte_iterator
variant_vector< Resource, Ts... >::_addr_of ( ssize_type _idx_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::_addr_of: index out of bounds" ) ;

        return storage_.begin() + offsets_[ _idx_ ] ;
}

////////////////////////////////////////////////////////////////////////////////
/// _addr_of const

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::const_byte_iterator
variant_vector< Resource, Ts... >::_addr_of ( ssize_type _idx_ ) const noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::_addr_of: index out of bounds" ) ;

        return storage_.begin() + offsets_[ _idx_ ] ;
}

////////////////////////////////////////////////////////////////////////////////
/// _find_end
///
/// returns iterator to first byte after element at position _idx_
///

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::byte_iterator
variant_vector< Resource, Ts... >::_find_end ( ssize_type _idx_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::_find_end: index out of bounds" ) ;

        byte_iterator elem_start = _addr_of( _idx_ ) ;

        ssize_type elem_size { 0 } ;

        visit( _idx_, [ & ]( auto const & elem ){ elem_size = ssizeof( remove_cvref_t< decltype( elem ) > ) ; } ) ;

        return elem_start + elem_size ;
}

////////////////////////////////////////////////////////////////////////////////
/// _find_end const
///
/// returns const iterator to first byte after element at position _idx_
///

template< typename Resource, typename... Ts >
constexpr
variant_vector< Resource, Ts... >::const_byte_iterator
variant_vector< Resource, Ts... >::_find_end ( ssize_type _idx_ ) const noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _idx_ && _idx_ < size(), "uti::variant_vector::_find_end: index out of bounds" ) ;

        const_byte_iterator elem_start = _addr_of( _idx_ ) ;

        ssize_type elem_size { 0 } ;

        visit( _idx_, [ & ]( auto const & elem ){ elem_size = ssizeof( remove_cvref_t< decltype( elem ) > ) ; } ) ;

        return elem_start + elem_size ;
}

////////////////////////////////////////////////////////////////////////////////
/// _align_for
///
/// returns iterator pointing to byte at or after _ptr_ which satisfies alignment requirements of T
///

template< typename Resource, typename... Ts >
template< typename T >
constexpr
variant_vector< Resource, Ts... >::byte_iterator
variant_vector< Resource, Ts... >::_align_for ( byte_iterator _ptr_ ) noexcept
{
        static constexpr ssize_type mask { alignof( T ) - 1 } ;

        return byte_iterator{ ( _ptr_ + mask ) & ~mask } ;
}

////////////////////////////////////////////////////////////////////////////////
/// _align_for const
///
/// returns const iterator pointing to byte at or after _ptr_ which satisfies alignment requirements of T
///

template< typename Resource, typename... Ts >
template< typename T >
constexpr
variant_vector< Resource, Ts... >::const_byte_iterator
variant_vector< Resource, Ts... >::_align_for ( const_byte_iterator _ptr_ ) const noexcept
{
        static constexpr ssize_type mask { alignof( T ) - 1 } ;

        return const_byte_iterator{ ( _ptr_ + mask ) & ~mask } ;
}

////////////////////////////////////////////////////////////////////////////////
/// _padding_for
///
/// returns number of bytes necessary as padding after _ptr_ to properly align a T
///

template< typename Resource, typename... Ts >
template< typename T >
constexpr
variant_vector< Resource, Ts... >::ssize_type
variant_vector< Resource, Ts... >::_padding_for ( const_byte_iterator _ptr_ ) const noexcept
{
        const_byte_iterator aligned = _align_for< T >( _ptr_ ) ;

        return aligned - _ptr_ ;
}

////////////////////////////////////////////////////////////////////////////////
/// _reserve
///

template< typename Resource, typename... Ts >
constexpr void
variant_vector< Resource, Ts... >::_reserve ( ssize_type _bytes_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( !storage_ ) UTI_UNLIKELY
        {
                storage_ = _resource_traits::allocate( _bytes_, max_elem_align ) ;
                return ;
        }
        if( storage_.size_ >= _bytes_ ) UTI_UNLIKELY return ;

        if constexpr( conjunction_v< is_trivially_relocatable< Ts >... > )
        {
                _resource_traits::reallocate( storage_, _bytes_, max_elem_align ) ;
        }
        else
        {
                if( _resource_traits::realloc_inplace( storage_, _bytes_ ) ) return ;

                block_type new_block = _resource_traits::allocate( _bytes_, max_elem_align ) ;

                if( !new_block ) UTI_UNLIKELY return ;

                for_each( [ & ]( auto & elem, ssize_type idx )
                        {
                                using type = remove_reference_t< decltype( elem ) > ;
                                using iter = _detail::iterator_type_for< type > ;

                                if constexpr( is_trivially_relocatable_v< type > )
                                {
                                        ::uti::copy( storage_ .begin() + offsets_[ idx ] ,
                                                     storage_ .begin() + offsets_[ idx ] + ssizeof( type ) ,
                                                     new_block.begin() + offsets_[ idx ]
                                        ) ;
                                }
                                else
                                {
                                        ::uti::construct< iter >( new_block.begin() + offsets_[ idx ], UTI_MOVE( elem ) ) ;
                                        ::uti::destroy( &elem ) ;
                                }
                        }
                ) ;
                _resource_traits::deallocate( storage_ ) ;
                storage_ = new_block ;
        }
}


} // namespace uti

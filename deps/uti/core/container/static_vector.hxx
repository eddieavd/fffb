//
//
//      uti
//      core/container/static_vector.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/type/traits.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/container/meta.hxx>
#include <uti/core/container/array.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/algo/mem.hxx>
#include <uti/core/algo/distance.hxx>


namespace uti
{


template< typename T >
union storage_t
{
        using   raw =    T ;
        using dummy = char ;

        raw     val_ ;
        dummy dummy_ ;

                 constexpr storage_t (           ) noexcept : dummy_{     0 } {}
        explicit constexpr storage_t ( raw _val_ ) noexcept :   val_{ _val_ } {}

        constexpr operator T () const noexcept { return val_ ; }
} ;


template< typename Iter >
struct static_vector_iterator
{
        using        _self = static_vector_iterator ;
        using _iter_traits = iterator_traits< Iter > ;
public:
        using      iterator_type = Iter ;

        using storage_value_type = typename _iter_traits::       value_type ;
        using    storage_pointer = typename _iter_traits::          pointer ;
        using  storage_reference = typename _iter_traits::        reference ;
        using    difference_type = typename _iter_traits::  difference_type ;
        using  iterator_category = typename _iter_traits::iterator_category ;

        using storage_raw_type = copy_cv_t< typename _iter_traits::value_type, typename storage_value_type::raw > ;
        using       value_type = storage_raw_type ;
        using          pointer = value_type *     ;
        using        reference = value_type &     ;

        using raw_iterator_base = iterator_base< value_type, typename _iter_traits::iterator_category, typename _iter_traits::difference_type, pointer, reference > ;

        constexpr static_vector_iterator (                              ) noexcept : iter_( nullptr ) {}
        constexpr static_vector_iterator ( iterator_type const & _iter_ ) noexcept : iter_(  _iter_ ) {}

        constexpr static_vector_iterator             ( static_vector_iterator const &  ) noexcept = default ;
        constexpr static_vector_iterator & operator= ( static_vector_iterator const &  ) noexcept = default ;
        constexpr static_vector_iterator             ( static_vector_iterator       && ) noexcept = default ;
        constexpr static_vector_iterator & operator= ( static_vector_iterator       && ) noexcept = default ;

        constexpr ~static_vector_iterator () noexcept = default ;

        constexpr static_vector_iterator & operator++ (     ) noexcept {                     ++iter_ ; return *this ; }
        constexpr static_vector_iterator   operator++ ( int ) noexcept { auto prev = *this ; ++iter_ ; return  prev ; }

        constexpr static_vector_iterator & operator-- (     ) noexcept {                     --iter_ ; return *this ; }
        constexpr static_vector_iterator   operator-- ( int ) noexcept { auto prev = *this ; --iter_ ; return  prev ; }

        constexpr static_vector_iterator & operator+= ( difference_type const _n_ ) noexcept { iter_ += _n_ ; return *this ; }
        constexpr static_vector_iterator & operator-= ( difference_type const _n_ ) noexcept { iter_ -= _n_ ; return *this ; }

        constexpr static_vector_iterator operator+ ( difference_type const _n_ ) const noexcept { auto iter = *this ; iter += _n_ ; return iter ; }
        constexpr static_vector_iterator operator- ( difference_type const _n_ ) const noexcept { auto iter = *this ; iter -= _n_ ; return iter ; }

        constexpr bool operator== ( static_vector_iterator const & _other_ ) const noexcept { return iter_ == _other_.iter_ ; }
        constexpr bool operator!= ( static_vector_iterator const & _other_ ) const noexcept { return iter_ != _other_.iter_ ; }

        constexpr reference operator*  () const noexcept { return  iter_->val_ ; }
        constexpr   pointer operator-> () const noexcept { return &iter_->val_ ; }

        constexpr operator pointer () const noexcept { return &iter_->val_ ; }

        constexpr operator raw_iterator_base () const noexcept { return &iter_->val_ ; }
private:
        iterator_type iter_ ;
} ;


template< typename T, ssize_t Cap >
class static_vector
{
        using storage = array< storage_t< T >, Cap > ;
public:
        using      value_type =       T    ;
        using       size_type =  size_t    ;
        using      ssize_type = ssize_t    ;
        using difference_type = ssize_type ;

        using    storage_type = storage ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using         storage_pointer = typename storage::        pointer ;
        using   const_storage_pointer = typename storage::  const_pointer ;
        using       storage_reference = typename storage::      reference ;
        using const_storage_reference = typename storage::const_reference ;

        using               iterator = static_vector_iterator< typename storage_type::              iterator > ;
        using         const_iterator = static_vector_iterator< typename storage_type::        const_iterator > ;
        using       reverse_iterator = static_vector_iterator< typename storage_type::      reverse_iterator > ;
        using const_reverse_iterator = static_vector_iterator< typename storage_type::const_reverse_iterator > ;

        static constexpr ssize_type static_capacity { Cap } ;

                 constexpr static_vector (                                                ) noexcept                                                     ;
        explicit constexpr static_vector ( ssize_type _count_                             ) noexcept( is_nothrow_default_constructible_v< value_type > ) ;
                 constexpr static_vector ( ssize_type _count_, value_type const & _value_ ) noexcept( is_nothrow_copy_constructible_v   < value_type > ) ;

        template< meta::forward_iterator Iter >
        constexpr static_vector ( Iter _begin_, Iter const & _end_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) ;

        template< meta::prefix_array_iterator Iter >
        constexpr static_vector ( Iter _begin_, Iter const & _end_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) ;

        template< meta::simple_container Other >
                requires( is_convertible_v< typename Other::value_type, value_type > )
        constexpr static_vector ( Other const & _other_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) ;

        template< ssize_t N = Cap > constexpr static_vector ( static_vector< T, N > const &  _other_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) ;
        template< ssize_t N = Cap > constexpr static_vector ( static_vector< T, N >       && _other_ ) noexcept( is_nothrow_move_constructible_v< value_type > ) ;

        template< ssize_t N = Cap > constexpr static_vector & operator= ( static_vector< T, N > const &  _other_ )
                noexcept( is_nothrow_copy_constructible_v< value_type > && is_nothrow_copy_assignable_v< value_type > ) ;

        template< ssize_t N = Cap > constexpr static_vector & operator= ( static_vector< T, N >       && _other_ )
                noexcept( is_nothrow_move_constructible_v< value_type > && is_nothrow_move_assignable_v< value_type > ) ;

        constexpr ~static_vector () noexcept( is_nothrow_destructible_v< value_type > ) requires(  is_trivially_destructible_v< value_type > ) = default ;
        constexpr ~static_vector () noexcept( is_nothrow_destructible_v< value_type > ) requires( !is_trivially_destructible_v< value_type > ) { clear() ; }

        constexpr void push_back ( value_type const &  _value_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) { emplace_back(           _value_   ) ; }
        constexpr void push_back ( value_type       && _value_ ) noexcept( is_nothrow_move_constructible_v< value_type > ) { emplace_back( UTI_MOVE( _value_ ) ) ; }

        template< typename... Args >
                requires( is_constructible_v< value_type, Args&&... > )
        constexpr void emplace_back ( Args&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, Args&&... > ) ;

        constexpr void insert ( ssize_type _pos_, value_type const &  _val_ ) noexcept( is_nothrow_copy_constructible_v< value_type > ) { insert( _pos_,           _val_   ) ; }
        constexpr void insert ( ssize_type _pos_, value_type       && _val_ ) noexcept( is_nothrow_move_constructible_v< value_type > ) { insert( _pos_, UTI_MOVE( _val_ ) ) ; }

        template< typename... Args >
                requires( is_constructible_v< value_type, Args&&... > )
        constexpr void insert ( ssize_type _position_, Args&&... _args_ ) noexcept( is_nothrow_constructible_v  < value_type, Args&&... >
                                                                                 && is_nothrow_move_assignable_v< value_type            > ) ;

        constexpr void erase      ( ssize_type _position_ ) noexcept( is_nothrow_destructible_v< value_type > && is_nothrow_move_assignable_v< value_type > ) ;
        constexpr void erase_fast ( ssize_type _position_ ) noexcept( is_nothrow_destructible_v< value_type > && is_nothrow_swappable_v      < value_type > ) ;

        UTI_NODISCARD constexpr       reference operator[] ( ssize_type _index_ )       noexcept { return storage_.data_[ _index_ ].val_ ; }
        UTI_NODISCARD constexpr const_reference operator[] ( ssize_type _index_ ) const noexcept { return storage_.data_[ _index_ ].val_ ; }

        UTI_NODISCARD constexpr       reference at ( ssize_type _index_ )       noexcept ;
        UTI_NODISCARD constexpr const_reference at ( ssize_type _index_ ) const noexcept ;

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return       iterator{ storage_.begin() } ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return const_iterator{ storage_.begin() } ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return       iterator{ storage_.begin() + size_ } ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return const_iterator{ storage_.begin() + size_ } ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return end() ; }

        UTI_NODISCARD constexpr       iterator  rbegin ()       noexcept { return       reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_iterator  rbegin () const noexcept { return const_reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_iterator crbegin () const noexcept { return rbegin() ; }

        UTI_NODISCARD constexpr       iterator  rend ()       noexcept { return       reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_iterator  rend () const noexcept { return const_reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_iterator crend () const noexcept { return rend() ; }

        UTI_NODISCARD constexpr       reference  front ()       noexcept { return *begin() ; }
        UTI_NODISCARD constexpr const_reference  front () const noexcept { return *begin() ; }
        UTI_NODISCARD constexpr const_reference cfront () const noexcept { return  front() ; }

        UTI_NODISCARD constexpr       reference  back ()       noexcept { return *( --end() ) ; }
        UTI_NODISCARD constexpr const_reference  back () const noexcept { return *( --end() ) ; }
        UTI_NODISCARD constexpr const_reference cback () const noexcept { return     back()   ; }

        UTI_NODISCARD constexpr       pointer data ()       noexcept { return &storage_.data_[ 0 ].val_ ; }
        UTI_NODISCARD constexpr const_pointer data () const noexcept { return &storage_.data_[ 0 ].val_ ; }

        UTI_NODISCARD constexpr ssize_type     size () const noexcept { return               size_ ; }
        UTI_NODISCARD constexpr ssize_type capacity () const noexcept { return     static_capacity ; }
        UTI_NODISCARD constexpr bool          empty () const noexcept { return size_ == 0          ; }
        UTI_NODISCARD constexpr bool           full () const noexcept { return size_ == capacity() ; }

        constexpr void clear () noexcept( is_nothrow_destructible_v< value_type > )
        {
                if constexpr( !is_trivially_destructible_v< value_type > )
                {
                        for( auto & val : *this )
                        {
                                ::uti::destroy( &val ) ;
                        }
                }
                size_ = 0 ;
        }
private:
        storage_type storage_ ;
        ssize_type      size_ ;
} ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr
static_vector< T, Cap >::static_vector () noexcept
        : storage_{   }
        , size_   { 0 }
{}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr
static_vector< T, Cap >::static_vector ( ssize_type _count_ ) noexcept( is_nothrow_default_constructible_v< value_type > )
        : storage_{         }
        , size_   { _count_ <= capacity() ? _count_ : capacity() }
{
        for( auto iter = begin(); iter != end(); ++iter )
        {
                ::uti::construct( iter ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr
static_vector< T, Cap >::static_vector ( ssize_type _count_, value_type const & _value_ ) noexcept( is_nothrow_copy_constructible_v< value_type > )
        : storage_{         }
        , size_   { _count_ <= capacity() ? _count_ : capacity() }
{
        for( auto iter = begin(); iter != end(); ++iter )
        {
                ::uti::construct( iter, _value_ ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< meta::forward_iterator Iter >
constexpr
static_vector< T, Cap >::static_vector ( Iter _begin_, Iter const & _end_ ) noexcept( is_nothrow_copy_constructible_v< value_type > )
        : storage_{}
        , size_   {}
{
        auto dist = ::uti::distance( _begin_, _end_ ) ;

        size_ = dist <= capacity() ? dist : capacity() ;

        for( auto iter = begin(); iter != end() && _begin_ != _end_; ++iter, ++_begin_ )
        {
                ::uti::construct( iter, *_begin_ ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< meta::prefix_array_iterator Iter >
constexpr
static_vector< T, Cap >::static_vector ( Iter _begin_, Iter const & _end_ ) noexcept( is_nothrow_copy_constructible_v< value_type > )
        : storage_{}
        , size_   {}
{
        auto dist = ::uti::distance( _begin_, _end_ ) ;

        size_ = dist <= capacity() ? dist : capacity() ;

        value_type last {} ;

        for( auto iter = begin(); iter != end() && _begin_ != _end_; ++iter, ++_begin_ )
        {
                ::uti::construct( iter, *_begin_ - last ) ;
                last = *_begin_ ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< meta::simple_container Other >
        requires( is_convertible_v< typename Other::value_type, T > )
constexpr
static_vector< T, Cap >::static_vector ( Other const & _other_ ) noexcept( is_nothrow_copy_constructible_v< value_type > )
        : static_vector( _other_.begin(), _other_.end() )
{}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< ssize_t N >
constexpr
static_vector< T, Cap >::static_vector ( static_vector< T, N > const & _other_ ) noexcept( is_nothrow_copy_constructible_v< value_type > )
        : static_vector( _other_.begin(), _other_.end() )
{}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< ssize_t N >
constexpr
static_vector< T, Cap >::static_vector ( static_vector< T, N > && _other_ ) noexcept( is_nothrow_move_constructible_v< value_type > )
        : storage_{}
        , size_   {}
{
        size_ = _other_.size_ <= capacity() ? _other_.size_ : capacity() ;

        for( ssize_type i = 0; i < size_; ++i )
        {
                ::uti::construct( begin() + i, UTI_MOVE( _other_.at( i ) ) ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< ssize_t N >
constexpr
static_vector< T, Cap > &
static_vector< T, Cap >::operator= ( static_vector< T, N > const & _other_ ) noexcept( is_nothrow_copy_constructible_v< value_type >
                                                                                    && is_nothrow_copy_assignable_v   < value_type > )
{
        ssize_type    size = _other_.size_ <= capacity() ? _other_.size_ : capacity() ;
        ssize_type overlap = ::uti::min( size_, size ) ;

        for( ssize_type i = 0; i < overlap; ++i )
        {
                at( i ) = _other_.at( i ) ;
        }
        if( overlap < size )
        {
                for( ssize_type i = overlap; i < size; ++i )
                {
                        ::uti::construct( begin() + i, _other_.at( i ) ) ;
                }
        }
        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                if( size_ > size )
                {
                        for( ssize_type i = overlap; i < size_; ++i )
                        {
                                ::uti::destroy( begin() + i ) ;
                        }
                }
        }
        size_ = size ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< ssize_t N >
constexpr
static_vector< T, Cap > &
static_vector< T, Cap >::operator= ( static_vector< T, N > && _other_ ) noexcept( is_nothrow_move_constructible_v< value_type >
                                                                               && is_nothrow_move_assignable_v   < value_type > )
{
        ssize_type    size = _other_.size_ <= capacity() ? _other_.size_ : capacity() ;
        ssize_type overlap = ::uti::min( size_, size ) ;

        for( ssize_type i = 0; i < overlap; ++i )
        {
                at( i ) = UTI_MOVE( _other_.at( i ) ) ;
        }
        if( overlap < size )
        {
                for( ssize_type i = overlap; i < size; ++i )
                {
                        ::uti::construct( begin() + i, UTI_MOVE( _other_.at( i ) ) ) ;
                }
        }
        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                if( size_ > size )
                {
                        for( ssize_type i = overlap; i < size_; ++i )
                        {
                                ::uti::destroy( begin() + i ) ;
                        }
                }
        }
        size_ = size ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< typename... Args >
        requires( is_constructible_v< T, Args&&... > )
constexpr void
static_vector< T, Cap >::emplace_back ( Args&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, Args&&... > )
{
        if( full() ) return ;

        ::uti::construct( end(), UTI_FWD( _args_ )... ) ;
        ++size_ ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
template< typename... Args >
        requires( is_constructible_v< T, Args&&... > )
constexpr void
static_vector< T, Cap >::insert ( ssize_type _position_, Args&&... _args_ ) noexcept( is_nothrow_constructible_v  < value_type, Args&&... >
                                                                                   && is_nothrow_move_assignable_v< value_type            > )
{
        if( _position_ >= size_ ) return emplace_back( UTI_FWD( _args_ )... ) ;

        if( _position_ < 0 ) _position_ = 0 ;

        ::uti::construct( end(), UTI_MOVE( back() ) ) ;

        for( ssize_type i = size_ - 1; i > _position_; --i )
        {
                at( i ) = UTI_MOVE( at( i - 1 ) ) ;
        }
        ::uti::construct( begin() + _position_, UTI_FWD( _args_ )... ) ;
        ++size_ ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr void
static_vector< T, Cap >::erase ( ssize_type _position_ ) noexcept( is_nothrow_destructible_v< value_type > && is_nothrow_move_assignable_v< value_type > )
{
        for( auto i = _position_; i < size_ - 1; ++i )
        {
                at( i ) = UTI_MOVE( at( i + 1 ) ) ;
        }
        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                ::uti::destroy( --end() ) ;
        }
        --size_ ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr void
static_vector< T, Cap >::erase_fast ( ssize_type _position_ ) noexcept( is_nothrow_destructible_v< value_type > && is_nothrow_swappable_v< value_type > )
{
        using ::uti::swap ;

        swap( at( _position_ ), back() ) ;
        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                ::uti::destroy( --end() ) ;
        }
        --size_ ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr
static_vector< T, Cap >::reference
static_vector< T, Cap >::at ( ssize_type _position_ ) noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size_, "uti::static_vector::at: index out of bounds" ) ;

        return operator[]( _position_ ) ;
}

////////////////////////////////////////////////////////////////////////////////

template< typename T, ssize_t Cap >
constexpr
static_vector< T, Cap >::const_reference
static_vector< T, Cap >::at ( ssize_type _position_ ) const noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size_, "uti::static_vector::at: index out of bounds" ) ;

        return operator[]( _position_ ) ;
}


} // namespace uti

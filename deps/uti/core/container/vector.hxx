//
//
//      uti
//      core/container/vector.hxx
//

#pragma once

#include <uti/core/algo/swap.hxx>
#include <uti/core/algo/mem.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/new.hxx>
#include <uti/core/container/view.hxx>
#include <uti/core/container/buffer.hxx>
#include <uti/core/container/memory_footprint.hxx>
#include <uti/core/meta/concepts.hxx>

#ifdef UTI_HAS_STL
#include <initializer_list>
#endif // UTI_HAS_STL


namespace uti
{


template< typename T, typename Alloc = new_allocator< T > >
class vector : public buffer< T, Alloc >, public view< T >
{
protected:
        using      _self =  vector             ;
        using _buff_base =  buffer< T, Alloc > ;
        using _view_base =  view  < T        > ;
public:
        using      value_type = typename _view_base::     value_type ;
        using       size_type = typename _view_base::      size_type ;
        using      ssize_type = typename _view_base::     ssize_type ;
        using difference_type = typename _view_base::difference_type ;

        using  allocator_type = typename _buff_base:: allocator_type ;
        using   _alloc_traits = typename _buff_base::  _alloc_traits ;

        using         pointer = typename _view_base::        pointer ;
        using   const_pointer = typename _view_base::  const_pointer ;
        using       reference = typename _view_base::      reference ;
        using const_reference = typename _view_base::const_reference ;

        using               iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using         const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

                 constexpr vector (                             ) noexcept = default ;
        explicit constexpr vector ( ssize_type const _capacity_ )                    ;

        constexpr vector ( ssize_type const _count_, value_type const & _val_ ) ;

        constexpr vector ( _buff_base && _buffer_, ssize_type const _size_ ) noexcept ;

        template< meta::forward_iterator Iter >
        constexpr vector ( Iter _begin_, Iter const & _end_ ) ;

        template< meta::prefix_array_iterator Iter >
        constexpr vector ( Iter _begin_, Iter const & _end_ ) ;

#ifdef UTI_HAS_STL
        constexpr vector ( std::initializer_list< value_type > _list_ ) ;
#endif // UTI_HAS_STL

        constexpr vector             ( vector const &  _other_ )          ;
        constexpr vector             ( vector       && _other_ ) noexcept ;
        constexpr vector & operator= ( vector const &  _other_ )          ;
        constexpr vector & operator= ( vector       && _other_ ) noexcept ;

        constexpr bool operator== ( vector const & _other_ ) const noexcept ;

        constexpr ~vector () noexcept ;

        constexpr void push_back ( value_type const &  _val_ ) ;
        constexpr void push_back ( value_type       && _val_ ) ;

        template< typename... Args >
        constexpr void emplace_back ( Args&&... _args_ ) ;

        constexpr void push_front ( value_type const &  _val_ ) ;
        constexpr void push_front ( value_type       && _val_ ) ;

        template< typename... Args >
        constexpr void emplace_front ( Args&&... _args_ ) ;

        constexpr void pop_back  () noexcept ;
        constexpr void pop_front () noexcept ;

        UTI_NODISCARD constexpr value_type pop_back_val  () noexcept ;
        UTI_NODISCARD constexpr value_type pop_front_val () noexcept ;

        constexpr bool       reserve (                             ) ;
        constexpr ssize_type reserve ( ssize_type const _capacity_ ) ;

        constexpr void shrink ( ssize_type const _capacity_ ) ;

        constexpr void shrink_to_fit (                         )          ;
        constexpr void shrink_size   ( ssize_type const _size_ ) noexcept ;

        constexpr void insert ( ssize_type _position_, value_type const &  _val_ ) ;
        constexpr void insert ( ssize_type _position_, value_type       && _val_ ) ;

        template< typename... Args >
        constexpr void insert ( ssize_type _position_, Args&&... _args_ ) ;

        constexpr void erase        ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > ) ;
        constexpr void erase_stable ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > ) ;

        constexpr void clear () noexcept ;

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return _view_base:: begin(); }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return _view_base:: begin(); }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return _view_base::cbegin(); }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return _view_base:: end(); }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return _view_base:: end(); }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return _view_base::cend(); }

        UTI_NODISCARD constexpr       reverse_iterator  rbegin ()       noexcept { return _view_base:: rbegin() ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rbegin () const noexcept { return _view_base:: rbegin() ; }
        UTI_NODISCARD constexpr const_reverse_iterator crbegin () const noexcept { return _view_base::crbegin() ; }

        UTI_NODISCARD constexpr       reverse_iterator  rend ()       noexcept { return _view_base:: rend() ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rend () const noexcept { return _view_base:: rend() ; }
        UTI_NODISCARD constexpr const_reverse_iterator crend () const noexcept { return _view_base::crend() ; }

        UTI_NODISCARD constexpr memory_footprint memory_usage () const noexcept
        {
                memory_footprint footprint
                {
                        .         static_usage_ = ssizeof( *this ),
                        .top_lvl_dynamic_usage_ = _buff_base::capacity() * ssizeof( value_type ),
                        .low_lvl_dynamic_usage_ = 0
                } ;
                if constexpr( meta::memory_reporter< value_type > )
                {
                        for( auto const & val : *this )
                        {
                                footprint.low_lvl_dynamic_usage_ += val.memory_usage().dynamic_footprint() ;
                        }
                }
                return footprint ;
        }
protected:
        template< typename... Args >
        constexpr void _emplace ( iterator _position_, Args&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, Args... > ) ;

        constexpr void _copy_buffer ( _buff_base & _buff_ ) noexcept ( is_nothrow_copy_assignable_v< value_type > && is_nothrow_destructible_v< value_type > ) ;
        constexpr void _swap_buffer ( _buff_base & _buff_ ) noexcept ( is_nothrow_swappable_v< value_type > ) ;

        constexpr bool          _is_full (                          ) const noexcept { return _view_base::size() >= _buff_base::capacity() ; }
        constexpr ssize_type _space_left (                          ) const noexcept { return _buff_base::capacity() - _view_base::size() ; }
        constexpr bool          _can_fit ( ssize_type const _count_ ) const noexcept { return _space_left() >= _count_ ; }
};

template< typename T >
vector ( vector< T >::ssize_type const _count_, T const & _val_ ) -> vector< T > ;


template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::_copy_buffer ( _buff_base & _buff_ ) noexcept( is_nothrow_copy_assignable_v< value_type > && is_nothrow_destructible_v< value_type > )
{
        ssize_type len = _buff_.capacity() < _view_base::size() ? _buff_.capacity() : _view_base::size() ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy( _view_base::begin(), _view_base::begin() + len, _buff_.begin() );
        }
        else
        {
                for( ssize_type i = 0; i < len; ++i )
                {
                        ::uti::construct( _buff_.begin() + i, UTI_MOVE( _view_base::at( i ) ) );
                        if constexpr( !_buff_base::_alloc_traits::destroys_elements() )
                        {
                                ::uti::destroy( _view_base::begin() + i );
                        }
                }
        }
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::_swap_buffer ( _buff_base & _buff_ ) noexcept( is_nothrow_swappable_v< value_type > )
{
        using uti::swap ;
        swap( _buff_, *this ) ;
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( ssize_type const _capacity_ )
        : _buff_base( _capacity_ ), _view_base( _buff_base::begin(), _buff_base::begin() ) {}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( ssize_type const _count_, value_type const & _val_ )
        : _buff_base( _count_ ), _view_base( _buff_base::begin(), _buff_base::begin() )
{
        if( !_can_fit( _count_ ) ) return ;

        if constexpr( is_trivially_copy_constructible_v< value_type > )
        {
                ::uti::memset( _buff_base::begin(), _buff_base::end(), _val_ ) ;
                _view_base::_size() = _buff_base::capacity() ;
        }
        else
        {
                for( ssize_type i = 0; i < _count_; ++i )
                {
                        _emplace( _view_base::end(), _val_ );
                }
        }
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( _buff_base && _buffer_, ssize_type const _size_ ) noexcept
        : _buff_base( UTI_MOVE( _buffer_ ) ),
          _view_base( _buff_base::begin(), _buff_base::begin() + _size_ )
{}

template< typename T, typename Alloc >
template< meta::forward_iterator Iter >
constexpr
vector< T, Alloc >::vector ( Iter _begin_, Iter const & _end_ )
        : _buff_base( ::uti::distance( _begin_, _end_ ) ) ,
          _view_base( _buff_base::begin(), _buff_base::begin() )
{
        if( !_can_fit( ::uti::distance( _begin_, _end_ ) ) ) return ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy( _begin_, _end_, _view_base::begin() ) ;
                _view_base::_size() = _buff_base::capacity() ;
        }
        else
        {
                while( _begin_ != _end_ )
                {
                        _emplace( _view_base::end(), *_begin_ ) ;
                        ++_begin_ ;
                }
        }
}

template< typename T, typename Alloc >
template< meta::prefix_array_iterator Iter >
constexpr
vector< T, Alloc >::vector ( Iter _begin_, Iter const & _end_ )
        : _buff_base( ::uti::distance( _begin_, _end_ ) ) ,
          _view_base( _buff_base::begin(), _buff_base::begin() )
{
        if( !_can_fit( ::uti::distance( _begin_, _end_ ) ) ) return ;

        value_type last {} ;

        while( _begin_ != _end_ )
        {
                _emplace( _view_base::end(), *_begin_ - last ) ;
                last = *_begin_ ;
                ++_begin_ ;
        }
}

#ifdef UTI_HAS_STL
template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( std::initializer_list< value_type > _list_ )
        : vector( _list_.begin(), _list_.end() )
{}
#endif // UTI_HAS_STL

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( vector const & _other_ )
        : _buff_base( _other_.size() ), _view_base( _buff_base::begin(), _buff_base::begin() )
{
        if( !_can_fit( _other_.size() ) ) return ;

        if constexpr( is_trivially_copy_assignable_v< value_type > )
        {
                ::uti::copy( _other_.begin(), _other_.end(), _view_base::begin() );
                _view_base::_size() = _other_.size() ;
        }
        else
        {
                for( auto const & val : _other_ )
                {
                        _emplace( _view_base::end(), val );
                }
        }
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::vector ( vector && _other_ ) noexcept
        : _buff_base( UTI_MOVE( _other_ ) ), _view_base( UTI_MOVE( _other_ ) )
{}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc > &
vector< T, Alloc >::operator= ( vector const & _other_ )
{
        if constexpr( is_trivially_copy_assignable_v< value_type > && is_trivially_destructible_v< value_type > )
        {
                reserve( _other_.size() );

                if( _buff_base::capacity() < _other_.size() ) return *this ;

                ::uti::copy( _other_.begin(), _other_.end(), _view_base::begin() );
        }
        else
        {
                if( _view_base::size() >= _other_.size() )
                {
                        for( ssize_type i = 0; i < _other_.size(); ++i )
                        {
                                _view_base::at( i ) = _other_.at( i );
                        }
                        shrink_size( _other_.size() );
                }
                else
                {
                        reserve( _other_.size() );

                        if( _buff_base::capacity() < _other_.size() ) return *this ;

                        ssize_type pos { 0 } ;
                        for( ; pos < _view_base::size(); ++pos )
                        {
                                _view_base::at( pos ) = _other_.at( pos );
                        }
                        for( ; pos < _other_.size(); ++pos )
                        {
                                _emplace( _view_base::end(), _other_.at( pos ) );
                        }
                }
        }
        _view_base::_size() = _other_.size();

        return *this;
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc > &
vector< T, Alloc >::operator= ( vector && _other_ ) noexcept
{
        clear();
        _buff_base::deallocate();

        this->_buffer()   = _other_._buffer()   ;
        this->begin_      = _other_.begin_      ;
        this->size_       = _other_.size_       ;
        this->_capacity() = _other_._capacity() ;

        _other_._buffer() = nullptr;
        _other_.begin_ = nullptr ;
        _other_.size_ = 0 ;
        _other_._capacity() = 0 ;

        return *this;
}

template< typename T, typename Alloc >
constexpr bool
vector< T, Alloc >::operator== ( vector const & _other_ ) const noexcept
{
        if( !uti::is_constant_evaluated() && _buff_base::data() == _other_.data() && _view_base::size() == _other_.size() )
        {
                return true;
        }
        for( ssize_type i = 0; i < _view_base::size(); ++i )
        {
                if( _view_base::at( i ) != _other_.at( i ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::~vector () noexcept
{
        clear();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::push_back ( value_type const & _val_ )
{
        emplace_back( _val_ ) ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::push_back ( value_type && _val_ )
{
        emplace_back( UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Alloc >
template< typename... Args >
constexpr void
vector< T, Alloc >::emplace_back ( Args&&... _args_ )
{
        if( !reserve() ) return ;

        _emplace( _view_base::end(), UTI_FWD( _args_ )... );
}

template< typename T, typename Alloc >
template< typename... Args >
constexpr void
vector< T, Alloc >::_emplace ( iterator _position_, Args&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, Args... > )
{
        ::uti::construct( _position_, UTI_FWD( _args_ )... );
        _view_base::_size()++ ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::push_front ( value_type const & _val_ )
{
        emplace_front( _val_ ) ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::push_front ( value_type && _val_ )
{
        emplace_front( UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Alloc >
template< typename... Args >
constexpr void
vector< T, Alloc >::emplace_front ( Args&&... _args_ )
{
        insert( 0, UTI_FWD( _args_ )... ) ;

        if( _view_base::begin() > _buff_base::begin() )
        {
                --_view_base::begin_ ;
                _emplace( _view_base::begin(), UTI_FWD( _args_ )... ) ;
        }
        else
        {
                insert( 0, UTI_FWD( _args_ )... ) ;
        }
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::pop_back () noexcept
{
        UTI_ASSERT( !_view_base::empty(), "uti::vector::pop_back: called on empty vector" );

        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                ::uti::destroy( _view_base::end() - 1 );
        }
        _view_base::pop_back();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::pop_front () noexcept
{
        UTI_ASSERT( !_view_base::empty(), "uti::vector::pop_front: called on empty vector" );

        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                ::uti::destroy( _view_base::begin() );
        }
        _view_base::pop_front();
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::value_type
vector< T, Alloc >::pop_back_val () noexcept
{
        value_type val( UTI_MOVE( _view_base::back() ) );
        pop_back();
        return val;
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::value_type
vector< T, Alloc >::pop_front_val () noexcept
{
        value_type val( UTI_MOVE( _view_base::front() ) );
        pop_front();
        return val;
}

template< typename T, typename Alloc >
constexpr bool
vector< T, Alloc >::reserve ()
{
        if( _is_full() )
        {
                return reserve( _buff_base::capacity() == 0 ? 1 : _buff_base::capacity() * 2 ) > _view_base::size() ;
        }
        return true ;
}

template< typename T, typename Alloc >
constexpr
vector< T, Alloc >::ssize_type
vector< T, Alloc >::reserve ( ssize_type const _capacity_ )
{
        if( _capacity_ <= _buff_base::capacity() ) return _buff_base::capacity() ;

        if( _buff_base::capacity() == 0 )
        {
                _buff_base::reserve( _capacity_ );
                _view_base::_begin() = _buff_base::begin();
                _view_base::_size () = 0 ;

                return _buff_base::capacity() ;
        }
        if( _buff_base::realloc_inplace( _capacity_ ) ) return _buff_base::capacity() ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ssize_type begin_pos = _view_base::begin() - _buff_base::begin();
                ssize_type        sz = _view_base::size() ;

                if( _buff_base::reserve( _capacity_ ) < _capacity_ ) return _buff_base::capacity() ;

                _view_base::_begin() = _buff_base::begin() + begin_pos ;
                _view_base:: _size() = sz ;
        }
        else
        {
                ssize_type size = _view_base::size();
                _buff_base buff( _capacity_ );

                _copy_buffer( buff );
                _swap_buffer( buff );

                _view_base::_begin() = _buff_base::begin();
                _view_base:: _size() = size ;
        }
        return _buff_base::capacity() ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::shrink ( ssize_type const _capacity_ )
{
        shrink_size( _capacity_ );
        shrink_to_fit();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::shrink_to_fit ()
{
        if( _view_base::size() >= _buff_base::capacity() ) return;

        _buff_base buff( _view_base::size() );

        _copy_buffer( buff );
        _swap_buffer( buff );

        _view_base::_begin() = _buff_base::begin();
        _view_base:: _size() = _buff_base::capacity();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::shrink_size ( ssize_type const _size_ ) noexcept
{
        if( _size_ >= _view_base::size() ) return;

        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                for( ssize_type i = _size_; i < _view_base::size(); ++i )
                {
                        ::uti::destroy( _view_base::begin() + i );
                }
        }
        _view_base::_size() = _size_;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::insert ( ssize_type _position_, value_type const & _val_ )
{
        insert< T const & >( _position_, _val_ ) ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::insert ( ssize_type _position_, value_type && _val_ )
{
        insert< T && >( _position_, UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Alloc >
template< typename... Args >
constexpr void
vector< T, Alloc >::insert ( ssize_type _position_, Args&&... _args_ )
{
        if( _position_ >= _view_base::size() )
        {
                return emplace_back( UTI_FWD( _args_ )... ) ;
        }
        if( _position_ < 0 ) _position_ = 0 ;

        if( _position_ == 0 && _view_base::begin() > _buff_base::begin() )
        {
                --_view_base::begin_ ;
                return _emplace( _view_base::begin(), UTI_FWD( _args_ )... ) ;
        }
        if( !reserve() ) return ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy_backward( _view_base::end() - 1, _view_base::begin() + _position_ - 1, _view_base::end() ) ;
        }
        else
        {
                ::uti::construct( _view_base::end(), UTI_MOVE( _view_base::back() ) ) ;
                for( ssize_type i = _view_base::size() - 1; i > _position_; --i )
                {
                        _view_base::at( i ) = UTI_MOVE( _view_base::at( i - 1 ) ) ;
                }
        }
        ::uti::construct( begin() + _position_, UTI_FWD( _args_ )... ) ;
        ++_view_base::size_ ;
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::erase ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > )
{
        UTI_ASSERT( 0 <= _position_ && _position_ < _view_base::size(), "uti::vector::erase: index out of range" );

        _view_base::at( _position_ ) = UTI_MOVE( _view_base::back() );

        pop_back();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::erase_stable ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > )
{
        UTI_ASSERT( 0 <= _position_ && _position_ < _view_base::size(), "uti::vector::erase_stable: index out of range" );

        for( ssize_type i = _position_; i < _view_base::size() - 1; ++i )
        {
                _view_base::at( i ) = UTI_MOVE( _view_base::at( i + 1 ) );
        }
        pop_back();
}

template< typename T, typename Alloc >
constexpr void
vector< T, Alloc >::clear () noexcept
{
        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                for( ssize_type i = 0; i < _view_base::size(); ++i )
                {
                        ::uti::destroy( _view_base::begin() + i );
                }
        }
        _view_base::_begin() = _buff_base::begin();
        _view_base::_size() = 0 ;
}


} // namespace uti

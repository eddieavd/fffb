//
//
//      uti
//      core/container/segment_tree.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/algo/mem.hxx>
#include <uti/core/meta/concepts.hxx>
#include <uti/core/type/sequence.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/container/view.hxx>
#include <uti/core/container/buffer.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/new.hxx>



namespace uti
{


namespace compare
{

constexpr auto _default = []( auto const & lhs, auto const &     ) { return remove_cvref_t< decltype( lhs ) >{} ; } ;
constexpr auto  less    = []( auto const & lhs, auto const & rhs ) { return           ( lhs < rhs ) ? lhs : rhs ; } ;
constexpr auto  more    = []( auto const & lhs, auto const & rhs ) { return           ( lhs > rhs ) ? lhs : rhs ; } ;
constexpr auto  sum     = []( auto const & lhs, auto const & rhs ) { return                           lhs + rhs ; } ;

} // namespace compare


template< typename T, auto Compare = compare::_default, typename Alloc = new_allocator< T > >
class segment_tree : public buffer< T, Alloc >, public view< T >
{
        using      _self = segment_tree       ;
        using _buff_base = buffer< T, Alloc > ;
        using _view_base = view  < T        > ;

        using _comparator = decltype( Compare ) ;

        static constexpr _comparator comp_ { Compare } ;

        static_assert( is_convertible_v< remove_cv_t< T >, decltype( comp_( declval< T const & >(), declval< T const & >() ) ) >, "uti::segment_tree: bad comparator!" ) ;
        static_assert( is_default_constructible_v< T >, "uti::segment_tree: T has to be default constructible!" ) ;
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

        using               iterator = iterator_base< T      , random_access_iterator_tag > ;
        using         const_iterator = iterator_base< T const, random_access_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

                 constexpr segment_tree (                             )     noexcept       = default ;
        explicit constexpr segment_tree ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr segment_tree ( ssize_type const _count_, value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::forward_iterator Iter >
        constexpr segment_tree ( Iter _begin_, Iter const & _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::prefix_array_iterator Iter >
        constexpr segment_tree ( Iter _begin_, Iter const & _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr segment_tree             ( segment_tree const &  _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr segment_tree             ( segment_tree       && _other_ )     noexcept                 ;
        constexpr segment_tree & operator= ( segment_tree const &  _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr segment_tree & operator= ( segment_tree       && _other_ )     noexcept                 ;

        constexpr ~segment_tree () noexcept ;

        constexpr bool operator== ( segment_tree const & _other_ ) const noexcept ;

        constexpr segment_tree & operator+= ( segment_tree const & _other_ ) noexcept ;
        constexpr segment_tree & operator-= ( segment_tree const & _other_ ) noexcept ;

        constexpr segment_tree operator- () const noexcept ;

        friend constexpr segment_tree operator+ ( segment_tree const & _lhs_, segment_tree const & _rhs_ ) noexcept
        {
                auto res = _lhs_ ;
                res += _rhs_ ;
                return res ;
        }
        friend constexpr segment_tree operator- ( segment_tree const & _lhs_, segment_tree const & _rhs_ ) noexcept
        {
                auto res = _lhs_ ;
                res -= _rhs_ ;
                return res ;
        }

        UTI_NODISCARD constexpr
        decltype( auto ) range ( auto&&... _coords_ ) const noexcept
                requires( sizeof...( _coords_ ) % 2 == 0 &&
                          meta::is_n_dim_container_v< _self, sizeof...( _coords_ ) / 2 > )
        {
                constexpr auto Dim = sizeof...( _coords_ ) / 2 ;

                if constexpr( Dim == 1 )
                {
                        return _range( _coords_... ) ;
                }
                else
                {
                        return [ & ]< ssize_type... Idxs >( uti::index_sequence< Idxs... > )
                        {
                                return [ & ]( ssize_type _x1_, meta::index< Idxs >&&... lhs ,
                                              ssize_type _x2_, meta::index< Idxs >&&... rhs )
                                {
                                        return _range( _x1_, _x2_ ).range( UTI_FWD( lhs )..., UTI_FWD( rhs )... ) ;
                                }( _coords_... ) ;
                        }( uti::make_index_sequence< Dim - 1 >{} ) ;
                }
        }

        constexpr void push_back ( value_type const &  _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr void push_back ( value_type       && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void emplace_back ( auto&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void pop_back  () noexcept ;
        constexpr void pop_front () noexcept ;

        constexpr void insert ( ssize_type const _position_, value_type const &  _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr void insert ( ssize_type const _position_, value_type       && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void update ( ssize_type const _position_, value_type const &  _val_ ) noexcept( is_nothrow_copy_assignable_v< value_type > ) ;
        constexpr void update ( ssize_type const _position_, value_type       && _val_ ) noexcept( is_nothrow_move_assignable_v< value_type > &&
                                                                                                   is_nothrow_copy_assignable_v< value_type > ) ;

        constexpr ssize_type reserve (                             ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr ssize_type reserve ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void shrink        ( ssize_type const _capacity_ ) noexcept ;
        constexpr void shrink_to_fit (                             ) noexcept ;
        constexpr void shrink_size   ( ssize_type const     _size_ ) noexcept ;

        constexpr void erase        ( ssize_type const _position_ ) noexcept ;
        constexpr void erase_stable ( ssize_type const _position_ ) noexcept ;

        constexpr void clear () noexcept ;

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return _view_base::begin() ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return _view_base::begin() ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return             begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return _view_base::end() ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return _view_base::end() ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return             end() ; }

        UTI_NODISCARD constexpr       iterator  rbegin ()       noexcept { return _view_base::rbegin() ; }
        UTI_NODISCARD constexpr const_iterator  rbegin () const noexcept { return _view_base::rbegin() ; }
        UTI_NODISCARD constexpr const_iterator crbegin () const noexcept { return             rbegin() ; }

        UTI_NODISCARD constexpr       iterator  rend ()       noexcept { return _view_base::rend() ; }
        UTI_NODISCARD constexpr const_iterator  rend () const noexcept { return _view_base::rend() ; }
        UTI_NODISCARD constexpr const_iterator crend () const noexcept { return             rend() ; }

        UTI_NODISCARD constexpr       bool    empty () const noexcept { return _view_base::   empty()     ; }
        UTI_NODISCARD constexpr ssize_type     size () const noexcept { return _view_base::    size()     ; }
        UTI_NODISCARD constexpr ssize_type capacity () const noexcept { return _buff_base::capacity() / 2 ; }
private:
        constexpr void _emplace ( auto&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, decltype( _args_ )... > ) ;

        constexpr void _update ( ssize_type _position_ ) noexcept( is_nothrow_copy_assignable_v< value_type > ) ;

        constexpr       reference _at ( ssize_type const _index_ )       noexcept ;
        constexpr const_reference _at ( ssize_type const _index_ ) const noexcept ;

        constexpr value_type _range ( ssize_type _x1_, ssize_type _x2_ ) const noexcept ;

        constexpr ssize_type _msb       ( ssize_type _num_ ) const noexcept ;
        constexpr ssize_type _ceil_pow2 ( ssize_type _num_ ) const noexcept ;

        constexpr void _init_tree    () noexcept ;
        constexpr void _clear_tree   () noexcept ;
        constexpr void _rebuild_tree () noexcept ;

        constexpr void _rebalance_tree ( ssize_type const _size_, ssize_type const _old_cap_ ) noexcept
                requires( is_trivially_relocatable_v< value_type > ) ;

        constexpr void _swap_buffer    ( _buff_base & _buff_ ) noexcept ;
        constexpr void _rebalance_into ( _buff_base & _buff_ ) noexcept( is_nothrow_move_constructible_v< value_type > ) ;
} ;


template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : _buff_base( 2 * _ceil_pow2( _capacity_ ) ),
          _view_base( _buff_base::begin() + capacity() ,
                      _buff_base::begin() + capacity() )
{
        _init_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( ssize_type const _count_, value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : _buff_base( 2 * _ceil_pow2( _count_ ) )  ,
          _view_base( _buff_base::begin() + capacity() ,
                      _buff_base::begin() + capacity() )
{
        _init_tree() ;

        for( ssize_type i = 0; i < _count_; ++i )
        {
                emplace_back( _val_ ) ;
        }
}

template< typename T, auto Compare, typename Alloc >
template< meta::forward_iterator Iter >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( Iter _begin_, Iter const & _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : _buff_base( 2 * _ceil_pow2( ::uti::distance( _begin_, _end_ ) ) ) ,
          _view_base( _buff_base::begin() + capacity() ,
                      _buff_base::begin() + capacity() )
{
        _init_tree() ;

        auto const real_size = ::uti::distance( _begin_, _end_ ) ;

        while( _begin_ != _end_ )
        {
                _emplace( *_begin_++ ) ;
        }
        _view_base::_size() = real_size ;
}

template< typename T, auto Compare, typename Alloc >
template< meta::prefix_array_iterator Iter >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( Iter _begin_, Iter const & _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : _buff_base( 2 * _ceil_pow2( ::uti::distance( _begin_, _end_ ) ) ) ,
          _view_base( _buff_base::begin() + capacity() ,
                      _buff_base::begin() + capacity() )
{
        _init_tree() ;

        auto const real_size = ::uti::distance( _begin_, _end_ ) ;

        value_type last = 0 ;

        while( _begin_ != _end_ )
        {
                _emplace( *_begin_ - last ) ;
                last = *_begin_ ;
                ++_begin_ ;
        }
        _view_base::_size() = real_size ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( segment_tree const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : _buff_base( 2 * _other_.capacity() ),
          _view_base( _buff_base::begin() + capacity() ,
                      _buff_base::begin() + capacity() )
{
        _init_tree() ;

        if constexpr( is_trivially_copy_assignable_v< value_type > )
        {
                ::uti::copy( static_cast< _buff_base >( _other_ ).begin(),
                             static_cast< _buff_base >( _other_ ).  end(),
                                                      _buff_base::begin()
                ) ;
                _view_base::_size() = _other_.size() ;
        }
        else
        {
                for( auto const & val : _other_ )
                {
                        _emplace( val ) ;
                }
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::segment_tree ( segment_tree && _other_ ) noexcept
        : _buff_base( UTI_MOVE( _other_ ) )  ,
          _view_base( UTI_MOVE( _other_ ) ) {}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc > &
segment_tree< T, Compare, Alloc >::operator= ( segment_tree const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if constexpr( is_trivially_copy_assignable_v< value_type > && is_trivially_destructible_v< value_type > )
        {
                reserve( _other_.size() ) ;
                ::uti::copy( _other_.begin(), _other_.end(), _view_base::begin() ) ;
                _rebuild_tree() ; // get rid of this by copying tree as well as elements
        }
        else
        {
                if( _view_base::size() >= _other_.size() )
                {
                        for( ssize_type i = 0; i < _other_.size(); ++i )
                        {
                                _view_base::at( i ) = _other_.at( i ) ;
                        }
                        shrink_size( _other_.size() ) ;
                }
                else
                {
                        reserve( _other_.size() ) ;

                        ssize_type pos { 0 } ;
                        for( ; pos < _view_base::size(); ++pos )
                        {
                                _view_base::at( pos ) = _other_.at( pos ) ;
                        }
                        for( ; pos < _other_.size(); ++pos )
                        {
                                _emplace( _other_.at( pos ) ) ;
                        }
                }
        }
        _view_base::_size() = _other_.size() ;

        return *this ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc > &
segment_tree< T, Compare, Alloc >::operator= ( segment_tree && _other_ ) noexcept
{
        clear() ;
        _buff_base::deallocate() ;

        this->_buffer()   = _other_._buffer()   ;
        this->begin_      = _other_.begin_      ;
        this->size_       = _other_.size_       ;
        this->_capacity() = _other_._capacity() ;

        _other_._buffer()   = nullptr ;
        _other_.begin_      = nullptr ;
        _other_.size_       =       0 ;
        _other_._capacity() =       0 ;

        return *this ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::~segment_tree () noexcept
{
        _clear_tree() ;
        clear() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr bool
segment_tree< T, Compare, Alloc >::operator== ( segment_tree const & _other_ ) const noexcept
{
        if( this == &_other_ ) return true ;

        if( size() != _other_.size() ) return false ;

        for( ssize_type i = 0; i < size(); ++i )
        {
                if( _view_base::at( i ) != _other_.at( i ) ) return false ;
        }
        return true ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc > &
segment_tree< T, Compare, Alloc >::operator+= ( segment_tree const & _other_ ) noexcept
{
        if( _other_.size() == size() )
        {
                for( ssize_type i = 0; i < size(); ++i )
                {
                        _view_base::at( i ) += _other_.at( i ) ;
                }
        }
        return *this ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc > &
segment_tree< T, Compare, Alloc >::operator-= ( segment_tree const & _other_ ) noexcept
{
        if( _other_.size() == size() )
        {
                for( ssize_type i = 0; i < size(); ++i )
                {
                        _view_base::at( i ) -= _other_.at( i ) ;
                }
        }
        return *this ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >
segment_tree< T, Compare, Alloc >::operator- () const noexcept
{
        segment_tree negative = *this ;

        for( auto & val : negative )
        {
                val = -val ;
        }
        return negative ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::push_back ( value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back( _val_ ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::push_back ( value_type && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back( UTI_MOVE( _val_ ) ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::emplace_back ( auto&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        reserve() ;
        if( size() < capacity() )
        {
                _emplace( UTI_FWD( _args_ )... ) ;
                _update( size() - 1 ) ;
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::pop_back () noexcept
{
        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                ::uti::destroy( _view_base::end() - 1 ) ;
        }
        _view_base::pop_back() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::pop_front () noexcept
{
        erase_stable( 0 ) ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::insert ( ssize_type const _position_, value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::segment_tree::insert: index out of bounds" ) ;

        if( _position_ >= size() )
        {
                emplace_back( _val_ ) ;
                return ;
        }
        reserve() ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy_backward( _view_base::end() - 1, _view_base::begin() + _position_ - 1, _view_base::end() ) ;
                _view_base::at( _position_ ) = _val_ ;
        }
        else
        {
                ::uti::construct( _view_base::end(), UTI_MOVE( _view_base::back() ) ) ;

                for( ssize_type i = size() - 1; i > _position_; --i )
                {
                        _view_base::at( i ) = UTI_MOVE( _view_base::at( i - 1 ) ) ;
                }
                _view_base::at( _position_ ) = _val_ ;
        }
        ++_view_base::_size() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::insert ( ssize_type const _position_, value_type && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        UTI_CEXPR_ASSERT( 0 <= _position_, "uti::segment_tree::insert: index out of bounds" ) ;

        if( _position_ >= size() )
        {
                emplace_back( UTI_MOVE( _val_ ) ) ;
                return ;
        }
        reserve() ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy_backward( _view_base::end() - 1, _view_base::begin() + _position_ - 1, _view_base::end() ) ;
                _view_base::at( _position_ ) = _val_ ;
        }
        else
        {
                ::uti::construct( _view_base::end(), UTI_MOVE( _view_base::back() ) ) ;

                for( ssize_type i = size() - 1; i > _position_; --i )
                {
                        _view_base::at( i ) = UTI_MOVE( _view_base::at( i - 1 ) ) ;
                }
                _view_base::at( _position_ ) = UTI_MOVE( _val_ ) ;
        }
        ++_view_base::_size() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::update ( ssize_type const _position_, value_type const & _val_ )
        noexcept( is_nothrow_copy_assignable_v< value_type > )
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::segment_tree::update: index out of bounds" ) ;

        _view_base::at( _position_ ) = _val_ ;
        _update( _position_ ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::update ( ssize_type const _position_, value_type && _val_ )
        noexcept( is_nothrow_move_assignable_v< value_type > &&
                  is_nothrow_copy_assignable_v< value_type >  )
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::segment_tree::update: index out of bounds" ) ;

        _view_base::at( _position_ ) = UTI_MOVE( _val_ ) ;
        _update( _position_ ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::ssize_type
segment_tree< T, Compare, Alloc >::reserve () UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( size() >= capacity() ) reserve( capacity() == 0 ? 4 : capacity() * 2 ) ;

        return capacity() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
segment_tree< T, Compare, Alloc >::ssize_type
segment_tree< T, Compare, Alloc >::reserve ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        auto old_cap = capacity() ;

        if( _capacity_ <= old_cap ) return old_cap ;

        auto new_cap = _ceil_pow2( _capacity_ ) ;

        if( _buff_base::capacity() == 0 )
        {
                _buff_base::reserve( 2 * new_cap ) ;
                _view_base::_begin() = _buff_base::begin() + new_cap ;
                _view_base::_size() = 0 ;
                _init_tree() ;

                return new_cap ;
        }
        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                if( _buff_base::realloc_inplace( 2 * new_cap ) )
                {
                        _rebalance_tree( size(), old_cap ) ;
                        _rebuild_tree() ;
                        return new_cap ;
                }
                auto sz = size() ;

                _buff_base::reserve( 2 * new_cap ) ;

                _view_base::_begin() = _buff_base:: begin() + old_cap ;
                _view_base:: _size() = sz ;

                _rebalance_tree( sz, old_cap ) ;
                _rebuild_tree() ;
        }
        else
        {
                ssize_type sz = _view_base::size() ;

                _buff_base buff( 2 * new_cap ) ;

                _clear_tree() ;
                _rebalance_into( buff ) ;
                _swap_buffer( buff ) ;

                _view_base::_begin() = _buff_base:: begin() + new_cap ;
                _view_base:: _size() = sz ;
                _init_tree() ;
                _rebuild_tree() ;
        }
        return new_cap ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::shrink ( ssize_type const _capacity_ ) noexcept
{
        shrink_size( _capacity_ ) ;
        shrink_to_fit() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::shrink_to_fit () noexcept
{
        auto cap = capacity() ;

        if( size() >= cap ) return ;

        auto new_cap = _ceil_pow2( cap / 2 ) ;

        _buff_base buff( 2 * new_cap ) ;

        _rebalance_into( buff ) ;
        _swap_buffer( buff ) ;

        auto sz = size() ;

        _view_base::_begin() = _buff_base:: begin() + new_cap ;
        _view_base:: _size() = sz ;

        _init_tree() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::shrink_size ( ssize_type const _size_ ) noexcept
{
        if( _size_ >= size() ) return ;

        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                for( ssize_type i = _size_; i < size(); ++i )
                {
                        ::uti::destroy( _view_base::begin() + i ) ;
                }
        }
        _view_base::_size() = _size_ ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::erase ( ssize_type const _position_ ) noexcept
{
        _view_base::at( _position_ ) = UTI_MOVE( _view_base::back() ) ;

        pop_back() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::erase_stable ( ssize_type const _position_ ) noexcept
{
        for( ssize_type i = _position_; i < _view_base::size() - 1; ++i )
        {
                _view_base::at( i ) = UTI_MOVE( _view_base::at( i + 1 ) ) ;
        }
        pop_back() ;
        _rebuild_tree() ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::clear () noexcept
{
        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                for( auto it = _view_base::begin(); it != _view_base::end(); ++it )
                {
                        ::uti::destroy( it ) ;
                }
        }
        _view_base::_begin() = _buff_base::begin() + capacity() ;
        _view_base:: _size() = 0 ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_swap_buffer ( _buff_base & _buff_ ) noexcept
{
        using uti::swap ;
        swap( _buff_, *this ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_emplace ( auto&&... _args_ ) noexcept( is_nothrow_constructible_v< value_type, decltype( _args_ )... > )
{
        ::uti::construct( _view_base::end(), UTI_FWD( _args_ )... ) ;
        _view_base::_size()++ ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_update ( ssize_type _position_ ) noexcept( is_nothrow_copy_assignable_v< value_type > )
{
        UTI_CEXPR_ASSERT( 0 <= _position_ && _position_ < size(), "uti::segment_tree::_update: index out of bounds" ) ;

        auto cap = capacity() ;

        _position_ += cap ;

        for( _position_ /= 2; _position_ > 0; _position_ /= 2 )
        {
                ssize_type lvl = cap / _msb( _position_ ) ;

                if( lvl * _position_ - cap >= size() )
                {
                        _at( _position_ ) = value_type{} ;
                }
                else if( lvl * _position_ - cap + ( lvl / 2 ) >= size() )
                {
                        _at( _position_ ) = _at( 2 * _position_ ) ;
                }
                else
                {
                        _at( _position_ ) = comp_( _at( 2 * _position_ ), _at( 2 * _position_ + 1 ) ) ;
                }
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr
typename
segment_tree< T, Compare, Alloc >::reference
segment_tree< T, Compare, Alloc >::_at ( ssize_type const _index_ ) noexcept
{
//      UTI_CEXPR_ASSERT( 0 <= _index_ && _index_ < size(), "uti::segment_tree::_at: index out of bounds" ) ;

        return *( _buff_base::begin() + _index_ ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
typename
segment_tree< T, Compare, Alloc >::const_reference
segment_tree< T, Compare, Alloc >::_at ( ssize_type const _index_ ) const noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _index_ && _index_ < size(), "uti::segment_tree::_at: index out of bounds" ) ;

        return *( _buff_base::begin() + _index_ ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
typename
segment_tree< T, Compare, Alloc >::value_type
segment_tree< T, Compare, Alloc >::_range ( ssize_type _x1_, ssize_type _x2_ ) const noexcept
{
        UTI_CEXPR_ASSERT( 0 <= _x1_ && _x1_ <= _x2_ && _x2_ < size(), "uti::segment_tree::_range: index out of bounds" ) ;

        auto cap = capacity() ;

        _x1_ += cap ;
        _x2_ += cap ;

        bool is_set = false  ;
        value_type result {} ;

        ssize_type lvl = 1 ;

        while( 0 < _x1_ && _x1_ <= _x2_ )
        {
                if( lvl * _x1_ - cap < size() && _x1_ % 2 == 1 )
                {
                        if( is_set ) { result = comp_( result,  _at( _x1_++ ) ) ; }
                        else         { is_set = true;  result = _at( _x1_++ )   ; }
                }
                if( lvl * _x2_ - cap < size() && _x2_ % 2 == 0 )
                {
                        if( is_set ) { result = comp_( result,  _at( _x2_-- ) ) ; }
                        else         { is_set = true;  result = _at( _x2_-- )   ; }
                }
                _x1_ /= 2 ;
                _x2_ /= 2 ;
                lvl  *= 2 ;
        }
        return result ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
typename
segment_tree< T, Compare, Alloc >::ssize_type
segment_tree< T, Compare, Alloc >::_msb ( ssize_type _num_ ) const noexcept
{
        _num_ |= ( _num_ >>  1 ) ;
        _num_ |= ( _num_ >>  2 ) ;
        _num_ |= ( _num_ >>  4 ) ;
        _num_ |= ( _num_ >>  8 ) ;
        _num_ |= ( _num_ >> 16 ) ;
        _num_ |= ( _num_ >> 32 ) ;

        return ( _num_ & ~( _num_ >> 1 ) ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr
typename
segment_tree< T, Compare, Alloc >::ssize_type
segment_tree< T, Compare, Alloc >::_ceil_pow2 ( ssize_type _num_ ) const noexcept
{
        auto msb = _msb( _num_ ) ;

        return _num_ == msb ? _num_ : ( msb << 1 ) ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_init_tree () noexcept
{
//      if constexpr( !is_trivially_constructible_v< value_type > )
        {
                for( auto it = _buff_base::begin(); it != _view_base::begin(); ++it )
                {
                        ::uti::construct( it ) ;
                }
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_clear_tree () noexcept
{
        if constexpr( !is_trivially_destructible_v< value_type > && !_alloc_traits::destroys_elements() )
        {
                for( auto it = _buff_base::begin(); it != _view_base::begin(); ++it )
                {
                        ::uti::destroy( it ) ;
                }
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_rebuild_tree () noexcept
{
        auto cap = capacity() ;

        for( auto pos = cap - 1; pos > 0; --pos )
        {
                ssize_type lvl = cap / _msb( pos ) ;

                if( lvl * pos - cap >= size() )
                {
                        _at( pos ) = value_type{} ;
                }
                else if( lvl * pos - cap + ( lvl / 2 ) >= size() )
                {
                        _at( pos ) = _at( 2 * pos ) ;
                }
                else
                {
                        _at( pos ) = comp_( _at( 2 * pos ), _at( 2 * pos + 1 ) ) ;
                }
        }
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_rebalance_tree ( ssize_type const _size_, [[ maybe_unused ]] ssize_type const _old_cap_ ) noexcept
        requires( is_trivially_relocatable_v  < value_type > )
{
        auto new_cap = capacity() ;

        ::uti::copy( _view_base::begin(), _view_base::begin() + _size_, _buff_base::begin() + new_cap ) ;

        _view_base::_begin() = _buff_base:: begin() + new_cap ;
        _view_base:: _size() = _size_ ;
}

template< typename T, auto Compare, typename Alloc >
constexpr void
segment_tree< T, Compare, Alloc >::_rebalance_into ( _buff_base & _buff_ )
        noexcept( is_nothrow_move_constructible_v< value_type > )
{
        auto new_cap = _buff_.capacity() / 2 ;

        if constexpr( is_trivially_relocatable_v< value_type > )
        {
                ::uti::copy( _view_base::begin(), _view_base::end(), _buff_.begin() + new_cap ) ;
        }
        else
        {
                for( ssize_type i = 0; i < size(); ++i )
                {
                        ::uti::construct( _buff_.begin() + new_cap + i, UTI_MOVE( _view_base::at( i ) ) ) ;
                        if constexpr( !_alloc_traits::destroys_elements() )
                        {
                                ::uti::destroy( _view_base::begin() + i ) ;
                        }
                }
        }
}


} // namespace uti

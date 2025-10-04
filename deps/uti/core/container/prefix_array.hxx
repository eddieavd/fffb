//
//
//      uti
//      core/container/prefix_array.hxx
//

#pragma once

#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/new.hxx>
#include <uti/core/container/vector.hxx>
#include <uti/core/container/array.hxx>
#include <uti/core/type/sequence.hxx>
#include <uti/core/meta/concepts.hxx>

#ifdef UTI_HAS_STL
#include <initializer_list>
#endif // UTI_HAS_STL


namespace uti
{


template< typename T, typename Alloc = new_allocator< T > >
class prefix_array : public vector< T, Alloc >
{
        using _self = prefix_array       ;
        using _base = vector< T, Alloc > ;
public:
        using      value_type = typename _base::     value_type ;
        using       size_type = typename _base::      size_type ;
        using      ssize_type = typename _base::     ssize_type ;
        using difference_type = typename _base::difference_type ;

        using  allocator_type = typename _base:: allocator_type ;
        using   _alloc_traits = typename _base::  _alloc_traits ;

        using         pointer = typename _base::        pointer ;
        using   const_pointer = typename _base::  const_pointer ;
        using       reference = typename _base::      reference ;
        using const_reference = typename _base::const_reference ;

        using               iterator = iterator_base< T      , prefix_array_iterator_tag > ;
        using         const_iterator = iterator_base< T const, prefix_array_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

                 constexpr prefix_array (                             ) noexcept = default ;
        explicit constexpr prefix_array ( ssize_type const _capacity_ )                    ;

        constexpr prefix_array ( ssize_type const _count_, value_type const & _val_ ) ;

        template< meta::forward_iterator Iter >
        constexpr prefix_array ( Iter _begin_, Iter const & _end_ ) ;

        template< meta::prefix_array_iterator Iter >
        constexpr prefix_array ( Iter _begin_, Iter const & _end_ ) ;

#ifdef UTI_HAS_STL
        constexpr prefix_array ( std::initializer_list< value_type > _list_ ) ;
#endif // UTI_HAS_STL

        constexpr prefix_array             ( prefix_array const &  _other_ )          = default ;
        constexpr prefix_array             ( prefix_array       && _other_ ) noexcept = default ;
        constexpr prefix_array & operator= ( prefix_array const &  _other_ )          = default ;
        constexpr prefix_array & operator= ( prefix_array       && _other_ ) noexcept = default ;

        constexpr bool operator== ( prefix_array const & _other_ ) const noexcept ;

        constexpr prefix_array & operator+= ( prefix_array const & _other_ ) noexcept ;
        constexpr prefix_array & operator-= ( prefix_array const & _other_ ) noexcept ;

        constexpr prefix_array operator- () const noexcept ;

        friend constexpr prefix_array operator+ ( prefix_array const & _lhs_, prefix_array const & _rhs_ ) noexcept
        {
                auto res = _lhs_ ;
                res += _rhs_ ;
                return res ;
        }
        friend constexpr prefix_array operator- ( prefix_array const & _lhs_, prefix_array const & _rhs_ ) noexcept
        {
                auto res = _lhs_ ;
                res -= _rhs_ ;
                return res ;
        }

        constexpr ~prefix_array () noexcept = default ;

        UTI_NODISCARD constexpr
        decltype( auto ) element_at ( ssize_type const _x_, auto... _idxs_ ) const noexcept
                requires( meta::is_n_dim_container_v< _self, sizeof...( _idxs_ ) + 1 > )
        {
                if constexpr( sizeof...( _idxs_ ) == 0 )
                {
                        return _x_ == 0 ? _base::at( _x_     )
                                        : _base::at( _x_     )
                                        - _base::at( _x_ - 1 ) ;
                }
                else
                {
                        return _x_ == 0 ? _base::at( _x_     ).element_at( _idxs_... )
                                        : _base::at( _x_     ).element_at( _idxs_... )
                                        - _base::at( _x_ - 1 ).element_at( _idxs_... ) ;
                }
        }

        UTI_NODISCARD constexpr
        decltype( auto ) range ( auto&&... _coords_ ) const noexcept
                requires( sizeof...( _coords_ ) % 2 == 0 &&
                          meta::is_n_dim_container_v< _self, sizeof...( _coords_ ) / 2 > )
        {
                constexpr auto Dim = sizeof...( _coords_ ) / 2 ;

                if constexpr( Dim == 1 )
                {
                        return [ & ]( ssize_type const _x1_, ssize_type const _x2_ )
                        {
                                return _x1_ == 0 ? _base::at( _x2_     )
                                                 : _base::at( _x2_     )
                                                 - _base::at( _x1_ - 1 ) ;
                        }( _coords_... ) ;
                }
                else
                {
                        return [ & ]< ssize_type... Idxs >( uti::index_sequence< Idxs... > )
                        {
                                return [ & ]( ssize_type const _x1_, meta::index< Idxs >&&... lhs ,
                                              ssize_type const _x2_, meta::index< Idxs >&&... rhs )
                                {
                                        return _x1_ == 0 ? _base::at( _x2_     ).range( UTI_FWD( lhs )..., UTI_FWD( rhs )... )
                                                         : _base::at( _x2_     ).range( UTI_FWD( lhs )..., UTI_FWD( rhs )... )
                                                         - _base::at( _x1_ - 1 ).range( UTI_FWD( lhs )..., UTI_FWD( rhs )... ) ;
                                }( _coords_... ) ;
                        }( uti::make_index_sequence< Dim - 1 >{} ) ;
                }
        }

        constexpr void push_back ( value_type const &  _val_ ) ;
        constexpr void push_back ( value_type       && _val_ ) ;

        constexpr void emplace_back ( auto&&... _args_ ) ;

                      constexpr       void pop_front     () noexcept ;
        UTI_NODISCARD constexpr value_type pop_front_val () noexcept ;
        UTI_NODISCARD constexpr value_type pop_back_val  () noexcept ;

        constexpr void insert ( ssize_type const _position_, value_type const &  _val_ ) ;
        constexpr void insert ( ssize_type const _position_, value_type       && _val_ ) ;

        constexpr void erase        ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > ) ;
        constexpr void erase_stable ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > )
        {
                erase( _position_ ) ;
        }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return _base::begin() ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return _base::begin() ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return        begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return _base::end() ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return _base::end() ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return        end() ; }

        UTI_NODISCARD constexpr       reverse_iterator  rbegin ()       noexcept { return  --end() ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rbegin () const noexcept { return  --end() ; }
        UTI_NODISCARD constexpr const_reverse_iterator crbegin () const noexcept { return rbegin() ; }

        UTI_NODISCARD constexpr       reverse_iterator  rend ()       noexcept { return --begin() ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rend () const noexcept { return --begin() ; }
        UTI_NODISCARD constexpr const_reverse_iterator crend () const noexcept { return    rend() ; }

        constexpr memory_footprint memory_usage () const noexcept { return _base::memory_usage() ; }
private:
        constexpr void _update_range ( ssize_type const _x_, ssize_type const _y_, value_type const & _diff_ ) noexcept ;
};


template< typename T, typename Alloc = allocator< T, malloc_resource > >
prefix_array ( prefix_array< T, Alloc >::ssize_type const, T const & ) -> prefix_array< T, Alloc > ;


template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc >::prefix_array ( ssize_type const _capacity_ )
        : _base( _capacity_ ) {}

template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc >::prefix_array ( ssize_type const _capacity_, value_type const & _val_ )
        : _base( _capacity_ )
{
        value_type last = _val_ ;

        for( ssize_type i = 0; i < _base::capacity(); ++i )
        {
                _base::_emplace( end(), last ) ;
                last += _val_ ;
        }
}

template< typename T, typename Alloc >
template< meta::forward_iterator Iter >
constexpr
prefix_array< T, Alloc >::prefix_array ( Iter _begin_, Iter const & _end_ )
        : _base( ::uti::distance( _begin_, _end_ ) )
{
        value_type last = *_begin_ ;

        _base::_emplace( end(), last ) ;

        while( ++_begin_ != _end_ )
        {
                last += *_begin_ ;
                _base::_emplace( end(), last ) ;
        }
}

template< typename T, typename Alloc >
template< meta::prefix_array_iterator Iter >
constexpr
prefix_array< T, Alloc >::prefix_array ( Iter _begin_, Iter const & _end_ )
        : _base( iterator_base< value_type, random_access_iterator_tag >( _begin_ ),
                 iterator_base< value_type, random_access_iterator_tag >(   _end_ ) ) {}

#ifdef UTI_HAS_STL
template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc >::prefix_array ( std::initializer_list< value_type > _list_ )
        : prefix_array( _list_.begin(), _list_.end() )
{}
#endif // UTI_HAS_STL

template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc > &
prefix_array< T, Alloc >::operator+= ( prefix_array const & _other_ ) noexcept
{
        if( _base::size() < _other_.size() )
        {

        }
        else if( _base::size() > _other_.size() )
        {

        }
        else
        {
                for( ssize_type i = 0; i < _base::size(); ++i )
                {
                        _base::at( i ) += _other_.at( i );
                }
        }
        return *this;
}

template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc > &
prefix_array< T, Alloc >::operator-= ( prefix_array const & _other_ ) noexcept
{
        if( _base::size() < _other_.size() )
        {

        }
        else if( _base::size() > _other_.size() )
        {

        }
        else
        {
                for( ssize_type i = 0; i < _base::size(); ++i )
                {
                        _base::at( i ) -= _other_.at( i );
                }
        }
        return *this;
}

template< typename T, typename Alloc >
constexpr
prefix_array< T, Alloc >
prefix_array< T, Alloc >::operator- () const noexcept
{
        auto prefix = *this ;

        for( auto & val : prefix )
        {
                val = -val ;
        }
        return prefix ;
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::push_back ( value_type const & _val_ )
{
        emplace_back( _val_ ) ;
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::push_back ( value_type && _val_ )
{
        emplace_back( UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::emplace_back ( auto&&... _args_ )
{
        if( _base::empty() )
        {
                _base::emplace_back( UTI_FWD( _args_)... ) ;
        }
        else
        {
                _base::emplace_back( UTI_FWD( value_type( _args_... ) + _base::back() ) ) ;
        }
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::pop_front () noexcept
{
        UTI_ASSERT( !_base::empty(), "uti::prefix_array::pop_front: called on empty prefix_array" );

        _update_range( 1, _base::size(), -_base::front() ) ;
        _base::pop_front();
}

template< typename T, typename Alloc >
UTI_NODISCARD constexpr typename
prefix_array< T, Alloc >::value_type
prefix_array< T, Alloc >::pop_front_val () noexcept
{
        UTI_ASSERT( !_base::empty(), "uti::prefix_array::pop_front_val: called on empty prefix_array" );

        auto val = _base::front();
        pop_front();
        return val;
}

template< typename T, typename Alloc >
UTI_NODISCARD constexpr typename
prefix_array< T, Alloc >::value_type
prefix_array< T, Alloc >::pop_back_val () noexcept
{
        UTI_ASSERT( !_base::empty(), "uti::prefix_array::pop_front_val: called on empty prefix_array" );

        auto val = element_at( _base::size() - 1 );
        _base::pop_back();
        return val;
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::insert ( ssize_type const _position_, value_type const & _val_ )
{
        UTI_ASSERT( 0 <= _position_ && _position_ <= _base::size(), "uti::prefix_array::insert: index out of range" );

        if( _position_ == _base::size() )
        {
                push_back( _val_ ) ;
                return ;
        }
        _base::insert( _position_, _val_ );
        _update_range( _position_ + 1, _base::size(), _val_ );

        if( _position_ > 0 )
        {
                _base::at( _position_ ) += _base::at( _position_ - 1 ) ;
        }
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::insert ( ssize_type const _position_, value_type && _val_ )
{
        UTI_ASSERT( 0 <= _position_ && _position_ <= _base::size(), "uti::prefix_array::insert: index out of range" );

        if( _position_ == _base::size() )
        {
                push_back( _val_ ) ;
                return ;
        }
        _base::insert( _position_, _val_ ) ;
        _update_range( _position_ + 1, _base::size(), _val_ );

        if( _position_ > 0 )
        {
                _base::at( _position_ ) += _base::at( _position_ - 1 ) ;
        }
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::erase ( ssize_type const _position_ ) noexcept( is_nothrow_move_assignable_v< value_type > )
{
        UTI_ASSERT( 0 <= _position_ && _position_ < _base::size(), "uti::prefix_array::erase: index out of range" );

        value_type const & to_erase = _base::at( _position_ );

        _update_range( _position_, _base::size(), -( to_erase ) );
        _base::erase_stable( _position_ );
}

template< typename T, typename Alloc >
constexpr void
prefix_array< T, Alloc >::_update_range ( ssize_type const _x_, ssize_type const _y_, value_type const & _diff_ ) noexcept
{
        for( ssize_type i = _x_; i < _y_; ++i )
        {
                _base::at( i ) += _diff_ ;
        }
}


} // namespace uti

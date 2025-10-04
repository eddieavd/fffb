//
//
//      uti
//      core/container/buffer.hxx
//

#pragma once

#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/default.hxx>
#include <uti/core/allocator/new.hxx>
#include <uti/core/container/memory_footprint.hxx>


namespace uti
{


template< typename T, typename Alloc = new_allocator< T > >
class buffer
{
        using _self = buffer ;
public:
        using      value_type =       T    ;
        using       size_type =  size_t    ;
        using      ssize_type = ssize_t    ;
        using difference_type = ssize_type ;

        using  allocator_type = Alloc ;
        using   _alloc_traits = allocator_traits< allocator_type > ;
        using      block_type = typename _alloc_traits::block_type ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using               iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using         const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

                 constexpr buffer (                             )     noexcept       = default ;
        explicit constexpr buffer ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr buffer ( iterator const _begin_, ssize_type const _cap_ ) noexcept
                : block_ { _begin_, _cap_ } {}

        constexpr buffer             ( buffer const &  _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr buffer & operator= ( buffer const &  _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr buffer             ( buffer       && _other_ )     noexcept                 ;
        constexpr buffer & operator= ( buffer       && _other_ )     noexcept                 ;

        constexpr bool operator== ( buffer const & _other_ ) const noexcept ;

        constexpr operator bool () const noexcept { return static_cast< bool >( block_ ) ; }

        constexpr ~buffer () noexcept ;

        constexpr ssize_type reserve ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr bool realloc_inplace ( ssize_type const _capacity_ ) noexcept ;

        constexpr void deallocate () noexcept ;

        UTI_NODISCARD constexpr ssize_type capacity () const noexcept { return _capacity() ; }

        constexpr       iterator  begin ()       noexcept { return _buffer()               ; }
        constexpr const_iterator  begin () const noexcept { return _buffer()               ; }
        constexpr const_iterator cbegin () const noexcept { return _buffer()               ; }
        constexpr       iterator    end ()       noexcept { return _buffer() + _capacity() ; }
        constexpr const_iterator    end () const noexcept { return _buffer() + _capacity() ; }
        constexpr const_iterator   cend () const noexcept { return _buffer() + _capacity() ; }

        constexpr       pointer data ()       noexcept { return _buffer() ; }
        constexpr const_pointer data () const noexcept { return _buffer() ; }

        constexpr ssize_type max_size () const noexcept { return _alloc_traits::max_size() ; }

        constexpr pointer       & _begin ()       noexcept { return _buffer() ; }
        constexpr pointer const & _begin () const noexcept { return _buffer() ; }

        friend constexpr void swap ( buffer & lhs, buffer & rhs ) noexcept
        {
                buffer tmp( UTI_MOVE( lhs ) ) ;

                lhs = UTI_MOVE( rhs ) ;
                rhs = UTI_MOVE( tmp ) ;
        }

        UTI_NODISCARD constexpr memory_footprint memory_usage () const noexcept
        {
                return
                {
                        ssizeof( *this ) ,
                        block_.size_ * ssizeof( value_type ) ,
                        0
                } ;
        }
protected:
        block_type block_ { nullptr, 0 } ;

        constexpr iterator       & _buffer ()       noexcept { return block_.begin_ ; }
        constexpr iterator const & _buffer () const noexcept { return block_.begin_ ; }

        constexpr ssize_type       & _capacity ()       noexcept { return block_.size_ ; }
        constexpr ssize_type const & _capacity () const noexcept { return block_.size_ ; }
};


template< typename T, typename Alloc >
constexpr
buffer< T, Alloc >::buffer ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( 0 < _capacity_ && _capacity_ < max_size() )
        {
                block_ = _alloc_traits::allocate( _capacity_ ) ;
        }
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc >::buffer ( buffer const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        reserve( _other_._capacity() );
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc >::buffer ( buffer && _other_ ) noexcept
        : block_{ _other_._buffer(), _other_._capacity() }
{
        _other_._buffer  () = nullptr ;
        _other_._capacity() =       0 ;
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc > &
buffer< T, Alloc >::operator= ( buffer const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        reserve( _other_._capacity() );

        return *this;
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc > &
buffer< T, Alloc >::operator= ( buffer && _other_ ) noexcept
{
        _buffer  () = _other_._buffer  () ;
        _capacity() = _other_._capacity() ;

        _other_._buffer  () = nullptr ;
        _other_._capacity() =       0 ;

        return *this;
}

template< typename T, typename Alloc >
constexpr bool
buffer< T, Alloc >::operator== ( buffer const & _other_ ) const noexcept
{
        if( this == &_other_ ) return true ;

        return _buffer  () == _other_._buffer  () &&
               _capacity() == _other_._capacity()  ;
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc >::~buffer () noexcept
{
        deallocate();
}

template< typename T, typename Alloc >
constexpr
buffer< T, Alloc >::ssize_type
buffer< T, Alloc >::reserve ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( _capacity_ <= _capacity() ) return _capacity();

        if( _capacity() == 0 )
        {
                block_ = _alloc_traits::allocate( _capacity_ ) ;
        }
        else
        {
                _alloc_traits::reallocate( block_, _capacity_ ) ;
        }
        return _capacity();
}

template< typename T, typename Alloc >
constexpr bool
buffer< T, Alloc >::realloc_inplace ( ssize_type const _capacity_ ) noexcept
{
        if( _alloc_traits::realloc_inplace( block_, _capacity_ ) )
        {
                _capacity() = _capacity_;
                return true;
        }
        return false;
}

template< typename T, typename Alloc >
constexpr void
buffer< T, Alloc >::deallocate () noexcept
{
        if( _buffer() ) _alloc_traits::deallocate( block_ );
}


} // namespace uti

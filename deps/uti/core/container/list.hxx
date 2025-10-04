//
//
//      uti
//      core/container/list.hxx
//

#pragma once

#include <uti/core/meta/concepts.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/new.hxx>
#include <uti/core/algo/mem.hxx>

#ifdef UTI_HAS_STL
#include <initializer_list>
#endif // UTI_HAS_STL


namespace uti
{


namespace meta
{


template< typename NodeT >
concept list_node = is_detected_v< has_value_type, NodeT > &&
        requires( NodeT node )
{
        { node.prev_ } -> convertible_to< NodeT * > ;
        { node.next_ } -> convertible_to< NodeT * > ;
} ;


} // namespace meta


template< typename T >
struct list_node
{
        using   value_type = T           ;
        using    node_type = list_node   ;
        using node_pointer = node_type * ;

        constexpr list_node () = delete ;

        constexpr list_node             ( list_node const &  ) = delete ;
        constexpr list_node & operator= ( list_node const &  ) = delete ;
        constexpr list_node             ( list_node       && ) = delete ;
        constexpr list_node & operator= ( list_node       && ) = delete ;

        template< typename... Args >
                requires meta::constructible_from< value_type, Args... >
        constexpr list_node ( node_pointer _prev_, node_pointer _next_, Args&&... _args_ ) noexcept( meta::nothrow_constructible_from< value_type, Args... > )
                : prev_( _prev_ )
                , next_( _next_ )
                , data_( UTI_FWD( _args_ )... )
        {}

        constexpr ~list_node () noexcept = default ;

        node_pointer prev_ { nullptr } ;
        node_pointer next_ { nullptr } ;
        value_type   data_             ;
} ;


template< meta::list_node NodeT >
class list_iterator
{
public:
        using  node_type =          NodeT             ;
        using value_type = typename NodeT::value_type ;

        using difference_type = ssize_t ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using         node_pointer = node_type       * ;
        using   node_const_pointer = node_type const * ;
        using       node_reference = node_type       & ;
        using node_const_reference = node_type const & ;

        using iterator_category = bidirectional_iterator_tag ;

        constexpr list_iterator (                            ) noexcept : ptr_( nullptr ) {}
        constexpr list_iterator ( node_pointer const & _ptr_ ) noexcept : ptr_(   _ptr_ ) {}

        constexpr list_iterator & operator= ( pointer const & _ptr_ ) noexcept { ptr_ = _ptr_ ; return *this ; }

        constexpr list_iterator             ( nullptr_t ) noexcept : ptr_( nullptr ) {}
        constexpr list_iterator & operator= ( nullptr_t ) noexcept { ptr_ = nullptr ; return *this ; }

        constexpr list_iterator             ( list_iterator const &  ) noexcept = default ;
        constexpr list_iterator & operator= ( list_iterator const &  ) noexcept = default ;
        constexpr list_iterator             ( list_iterator       && ) noexcept = default ;
        constexpr list_iterator & operator= ( list_iterator       && ) noexcept = default ;

        constexpr list_iterator ( list_iterator< remove_const_t< node_type > > const & _other_ ) noexcept
                requires is_const_v< node_type > : ptr_( _other_.ptr_ ) {}

        constexpr list_iterator & operator= ( list_iterator< remove_const_t< node_type > > const & _other_ ) noexcept
                requires is_const_v< node_type > { ptr_ = _other_.ptr_ ; }

        constexpr list_iterator ( list_iterator< remove_const_t< node_type > > && _other_ ) noexcept
                requires is_const_v< node_type > : ptr_( _other_.ptr_ ) {}

        constexpr list_iterator & operator= ( list_iterator< remove_const_t< node_type > > && _other_ ) noexcept
                requires is_const_v< node_type > { ptr_ = _other_.ptr_ ; }

        constexpr ~list_iterator () noexcept = default ;

        constexpr operator pointer ()       noexcept { return ptr_ ; }
        constexpr operator pointer () const noexcept { return ptr_ ; }

        constexpr list_iterator & operator++ (     ) noexcept {                     if( ptr_ ) ptr_ = ptr_->next_ ; return *this ; }
        constexpr list_iterator   operator++ ( int ) noexcept { auto prev = *this ; if( ptr_ ) ptr_ = ptr_->next_ ; return  prev ; }

        constexpr list_iterator & operator-- (     ) noexcept {                     if( ptr_ ) ptr_ = ptr_->prev_ ; return *this ; }
        constexpr list_iterator   operator-- ( int ) noexcept { auto prev = *this ; if( ptr_ ) ptr_ = ptr_->prev_ ; return  prev ; }

        constexpr reference operator*  () noexcept { return  ptr_->data_ ; }
        constexpr pointer   operator-> () noexcept { return &ptr_->data_ ; }

        friend constexpr bool operator== ( list_iterator const & _lhs_, list_iterator const & _rhs_ ) noexcept
        { return _lhs_.ptr_ == _rhs_.ptr_ ; }

        friend constexpr bool operator!= ( list_iterator const & _lhs_, list_iterator const & _rhs_ ) noexcept
        { return _lhs_.ptr_ != _rhs_.ptr_ ; }

        friend constexpr bool operator== ( list_iterator const & _lhs_, nullptr_t ) noexcept
        { return _lhs_.ptr_ == nullptr ; }

        friend constexpr bool operator!= ( list_iterator const & _lhs_, nullptr_t ) noexcept
        { return _lhs_.ptr_ != nullptr ; }

        friend constexpr void swap ( list_iterator & _lhs_, list_iterator & _rhs_ ) noexcept
        {
                auto _tmp_ = _lhs_.ptr_ ;
                _lhs_.ptr_ = _rhs_.ptr_ ;
                _rhs_.ptr_ =      _tmp_ ;
        }

        node_pointer ptr_ ;
} ;


template< typename T, typename Allocator = new_allocator< list_node< T > > >
class list
{
public:
        using      value_type =            T   ;
        using       node_type = list_node< T > ;
        using       size_type =  size_t        ;
        using      ssize_type = ssize_t        ;
        using difference_type = ssize_type     ;

        using  allocator_type = Allocator ;
        using   _alloc_traits = allocator_traits< allocator_type > ;
        using      block_type = typename _alloc_traits::block_type ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using         node_pointer = node_type       * ;
        using   const_node_pointer = node_type const * ;
        using       node_reference = node_type       & ;
        using const_node_reference = node_type const & ;

        using               iterator = list_iterator< node_type       > ;
        using         const_iterator = list_iterator< node_type const > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

        constexpr list () noexcept = default ;

        explicit constexpr list ( value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::forward_iterator Iter >
                requires meta::convertible_to< iter_value_t< Iter >, value_type >
        constexpr list ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

#ifdef UTI_HAS_STL
        constexpr list ( std::initializer_list< value_type > _list_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
#endif // UTI_HAS_STL

        constexpr list             ( list const &  ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr list             ( list       && )     noexcept                 ;
        constexpr list & operator= ( list const &  ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr list & operator= ( list       && )     noexcept                 ;

        constexpr ~list () noexcept { clear() ; }

        constexpr void push_back ( value_type const &  _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr void push_back ( value_type       && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename... Args >
                requires meta::constructible_from< value_type, Args... >
        constexpr void emplace_back ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void push_front ( value_type const &  _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;
        constexpr void push_front ( value_type       && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename... Args >
                requires meta::constructible_from< value_type, Args... >
        constexpr void emplace_front ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< typename... Args >
                requires meta::constructible_from< value_type, Args... >
        constexpr void insert ( iterator _position_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::forward_iterator Iter >
                requires meta::convertible_to< iter_value_t< Iter >, value_type >
        constexpr void append ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        template< meta::simple_container Other >
                requires meta::convertible_to< typename Other::value_type, value_type >
        constexpr void append ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC ;

        constexpr void pop_back  () noexcept ;
        constexpr void pop_front () noexcept ;

        constexpr void erase ( iterator _position_ ) noexcept ;

        constexpr void clear () noexcept ;

        UTI_NODISCARD constexpr ssize_type  size () const noexcept { return size_      ; }
        UTI_NODISCARD constexpr       bool empty () const noexcept { return size_ == 0 ; }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return head_   ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return head_   ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return nullptr ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return nullptr ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return end()   ; }

        UTI_NODISCARD constexpr       reverse_iterator  rbegin ()       noexcept { return iterator( tail_ ) ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rbegin () const noexcept { return iterator( tail_ ) ; }
        UTI_NODISCARD constexpr const_reverse_iterator crbegin () const noexcept { return   rbegin(       ) ; }

        UTI_NODISCARD constexpr       reverse_iterator  rend ()       noexcept { return nullptr ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rend () const noexcept { return nullptr ; }
        UTI_NODISCARD constexpr const_reverse_iterator crend () const noexcept { return rend()  ; }

        UTI_NODISCARD constexpr       reference  front ()       noexcept { return *begin() ; }
        UTI_NODISCARD constexpr const_reference  front () const noexcept { return *begin() ; }
        UTI_NODISCARD constexpr const_reference cfront () const noexcept { return  front() ; }

        UTI_NODISCARD constexpr       reference  back ()       noexcept { return *rbegin() ; }
        UTI_NODISCARD constexpr const_reference  back () const noexcept { return *rbegin() ; }
        UTI_NODISCARD constexpr const_reference cback () const noexcept { return    back() ; }
private:
        node_type * head_ { nullptr } ;
        node_type * tail_ { nullptr } ;
        ssize_type  size_ {       0 } ;

        template< typename... Args >
                requires meta::constructible_from< value_type, Args... >
        constexpr node_pointer _new_node ( node_pointer _prev_, node_pointer _next_, Args&&... _args_ )
                UTI_NOEXCEPT_UNLESS_BADALLOC_AND( meta::nothrow_constructible_from< value_type, Args... > ) ;

        constexpr void _destroy_node ( node_pointer _node_ ) const noexcept ;
} ;


template< typename T, typename Allocator >
constexpr list< T, Allocator >::list ( value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        head_ = tail_ = _new_node( nullptr, nullptr, _val_ ) ;
        ++size_ ;
}

template< typename T, typename Allocator >
template< meta::forward_iterator Iter >
        requires meta::convertible_to< iter_value_t< Iter >, T >
constexpr list< T, Allocator >::list ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        for( ; _begin_ != _end_; ++_begin_ )
        {
                emplace_back( *_begin_ ) ;
        }
}

#ifdef UTI_HAS_STL
template< typename T, typename Allocator >
constexpr list< T, Allocator >::list ( std::initializer_list< T > _list_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : list( _list_.begin(), _list_.end() )
{}
#endif // UTI_HAS_STL

template< typename T, typename Allocator >
constexpr
list< T, Allocator >::list ( list const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        : list( _other_.begin(), _other_.end() )
{}

template< typename T, typename Allocator >
constexpr
list< T, Allocator >::list ( list && _other_ ) noexcept
        : head_( _other_.head_ )
        , tail_( _other_.tail_ )
        , size_( _other_.size_ )
{
        _other_.head_ = _other_.tail_ = nullptr ;
        _other_.size_ = 0 ;
}

template< typename T, typename Allocator >
constexpr
list< T, Allocator > &
list< T, Allocator >::operator= ( list const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        clear() ;

        append( _other_.begin(), _other_.end() ) ;

        return *this ;
}

template< typename T, typename Allocator >
constexpr
list< T, Allocator > &
list< T, Allocator >::operator= ( list && _other_ ) noexcept
{
        clear() ;

        head_ = _other_.head_ ;
        tail_ = _other_.tail_ ;
        size_ = _other_.size_ ;

        _other_.head_ = _other_.tail_ = nullptr ;
        _other_.size_ = 0 ;

        return *this ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::push_back ( value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back( _val_ ) ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::push_back ( value_type && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_back( UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Allocator >
template< typename... Args >
        requires meta::constructible_from< T, Args... >
constexpr void
list< T, Allocator >::emplace_back ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( !head_ )
        {
                head_ = tail_ = _new_node( nullptr, nullptr, UTI_FWD( _args_ )... ) ;
                ++size_ ;
        }
        else
        {
                node_pointer node = _new_node( tail_, nullptr, UTI_FWD( _args_ )... ) ;

                tail_->next_ = node ;
                tail_ = node ;

                ++size_ ;
        }
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::push_front ( value_type const & _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_front( _val_ ) ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::push_front ( value_type && _val_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        emplace_front( UTI_MOVE( _val_ ) ) ;
}

template< typename T, typename Allocator >
template< typename... Args >
        requires meta::constructible_from< T, Args... >
constexpr void
list< T, Allocator >::emplace_front ( Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( !head_ )
        {
                head_ = tail_ = _new_node( nullptr, nullptr, UTI_FWD( _args_ )... ) ;
                ++size_ ;
        }
        else
        {
                node_pointer node = _new_node( nullptr, head_, UTI_FWD( _args_ )... ) ;

                head_->prev_ = node ;
                head_ = node ;

                ++size_ ;
        }
}

template< typename T, typename Allocator >
template< typename... Args >
        requires meta::constructible_from< T, Args... >
constexpr void
list< T, Allocator >::insert ( iterator _position_, Args&&... _args_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        if( !head_ )
        {
                emplace_back( UTI_FWD( _args_ )... ) ;
                return ;
        }
        else if( _position_ == head_ )
        {
                emplace_front( UTI_FWD( _args_ )... ) ;
                return ;
        }
        else
        {
                node_pointer node = _new_node( _position_.ptr_->prev_, _position_.ptr_, UTI_FWD( _args_ )... ) ;

                _position_.ptr_->prev_->next_ = node ;
                _position_.ptr_->prev_ = node ;

                ++size_ ;
        }
}

template< typename T, typename Allocator >
template< meta::forward_iterator Iter >
        requires meta::convertible_to< iter_value_t< Iter >, T >
constexpr void
list< T, Allocator >::append ( Iter _begin_, Iter const _end_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        for( ; _begin_ != _end_; ++_begin_ )
        {
                emplace_back( *_begin_ ) ;
        }
}

template< typename T, typename Allocator >
template< meta::simple_container Other >
        requires meta::convertible_to< typename Other::value_type, T >
constexpr void
list< T, Allocator >::append ( Other const & _other_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
{
        append( _other_.begin(), _other_.end() ) ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::pop_back () noexcept
{
        if( !tail_ ) return ;

        auto node = tail_ ;

        if( head_ == tail_ ) head_ = nullptr ;
        tail_ = tail_->prev_ ;
        tail_->next_ = nullptr ;

        _destroy_node( node ) ;
        --size_ ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::pop_front () noexcept
{
        if( !head_ ) return ;

        auto node = head_ ;

        if( tail_ == head_ ) tail_ = nullptr ;
        head_ = head_->next_ ;
        head_->prev_ = nullptr ;

        _destroy_node( node ) ;
        --size_ ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::erase ( iterator _position_ ) noexcept
{
        node_pointer node = _position_.ptr_ ;

        if( !node ) return ;

        if( node == head_ && node == tail_ )
        {
                head_ = tail_ = nullptr ;
        }
        else if( node == head_ )
        {
                head_ = head_->next_ ;
                head_->prev_ = nullptr ;
        }
        else if( node == tail_ )
        {
                tail_ = tail_->prev_ ;
                tail_->next_ = nullptr ;
        }
        else
        {
                node->prev_->next_ = node->next_ ;
                node->next_->prev_ = node->prev_ ;
        }
        _destroy_node( node ) ;
        --size_ ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::clear () noexcept
{
        while( head_ )
        {
                auto ptr = head_ ;
                head_ = head_->next_ ;

                _destroy_node( ptr ) ;
        }
        head_ = tail_ = nullptr ;
        size_ = 0 ;
}

template< typename T, typename Allocator >
constexpr void
list< T, Allocator >::_destroy_node ( node_pointer _node_ ) const noexcept
{
        if constexpr( !is_trivially_destructible_v< value_type > )
        {
                ::uti::destroy( &_node_->data_ ) ;
        }
        block_type block{ _node_, sizeof( node_type ) } ;
        _alloc_traits::deallocate( block ) ;
}

template< typename T, typename Allocator >
template< typename... Args >
        requires meta::constructible_from< T, Args... >
constexpr
list< T, Allocator >::node_pointer
list< T, Allocator >::_new_node ( node_pointer _prev_, node_pointer _next_, Args&&... _args_ )
        UTI_NOEXCEPT_UNLESS_BADALLOC_AND( meta::nothrow_constructible_from< T, Args... > )
{
        node_pointer node = _alloc_traits::allocate( 1 ).begin() ;

        ::uti::construct( node, _prev_, _next_, UTI_FWD( _args_ )... ) ;

        return node ;
}


} // namespace uti

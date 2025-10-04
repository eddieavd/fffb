//
//
//      uti
//      core/iterator/base.hxx
//

#pragma once

#include <uti/core/iterator/meta.hxx>


namespace uti
{


template< typename T, typename IterCat, typename DiffType = ptrdiff_t, typename Pointer = T *, typename Reference = T & >
struct iterator_base
{
        using _self = iterator_base ;
public:
        using        value_type =         T ;
        using           pointer =   Pointer ;
        using         reference = Reference ;
        using   difference_type =  DiffType ;
        using iterator_category =   IterCat ;

        constexpr iterator_base (                       ) noexcept requires _has_forward_iterator_category_v< _self > : ptr_( nullptr ) {}
        constexpr iterator_base ( pointer const & _ptr_ ) noexcept                                                    : ptr_(   _ptr_ ) {}
        constexpr iterator_base ( u64_t       _ptrdata_ ) noexcept                                                    : ptr_( ( pointer ) _ptrdata_ ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< remove_const_t< T >, u8_t > && !is_const_v< T1 > )
        constexpr iterator_base ( T1 * _ptr_ ) noexcept : ptr_( static_cast< u8_t * >( static_cast< void * >( _ptr_ ) ) ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t const > )
        constexpr iterator_base ( T1 const * _ptr_ ) noexcept : ptr_( static_cast< u8_t const * >( static_cast< void const * >( _ptr_ ) ) ) {}

        constexpr iterator_base ( u8_t * _ptr_ ) noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > )
                : ptr_( static_cast< T * >( static_cast< void * >( _ptr_ ) ) ) {}

        constexpr iterator_base ( u8_t const * _ptr_ ) noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > && is_const_v< T > )
                : ptr_( static_cast< T const * >( static_cast< void const * >( _ptr_ ) ) ) {}

        constexpr iterator_base & operator= ( pointer const & _ptr_ ) noexcept { ptr_ = _ptr_ ; return *this ; }

        constexpr iterator_base             ( nullptr_t ) noexcept : ptr_ ( nullptr ) {}
        constexpr iterator_base & operator= ( nullptr_t ) noexcept { ptr_ = nullptr ; return *this ; }

        constexpr iterator_base             ( iterator_base const &  ) noexcept = default ;
        constexpr iterator_base & operator= ( iterator_base const &  ) noexcept = default ;
        constexpr iterator_base             ( iterator_base       && ) noexcept = default ;
        constexpr iterator_base & operator= ( iterator_base       && ) noexcept = default ;

        constexpr u64_t operator& ( u64_t const _mask_ ) const noexcept
        {
                return ( ( u64_t ) ptr_ ) & _mask_ ;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// non-const to const
        ////////////////////////////////////////////////////////////////////////////////

        constexpr iterator_base ( iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >, add_lvalue_reference_t< remove_const_t< T > > > const & _other_ )
                noexcept requires( is_const_v< T > ) : ptr_( _other_.ptr_ ) {} ;

        constexpr iterator_base & operator= ( iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >, add_lvalue_reference_t< remove_const_t< T > > > const & _other_ )
                noexcept requires( is_const_v< T > ) { ptr_ = _other_.ptr_ ; return *this ; }

        constexpr iterator_base ( iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >, add_lvalue_reference_t< remove_const_t< T > > > && _other_ )
                noexcept requires( is_const_v< T > ) : ptr_( _other_.ptr_ ) {} ;

        constexpr iterator_base & operator= ( iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >, add_lvalue_reference_t< remove_const_t< T > > > && _other_ )
                noexcept requires( is_const_v< T > ) { ptr_ = _other_.ptr_ ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        /// const to non-const
        ////////////////////////////////////////////////////////////////////////////////

        constexpr iterator_base ( iterator_base< T const, IterCat, DiffType, T const *, T const & > const & _other_ )
                noexcept requires( !is_const_v< T > ) : ptr_( const_cast< T * >( _other_.ptr_ ) ) {} ;

        constexpr iterator_base & operator= ( iterator_base< T const, IterCat, DiffType, T const *, T const & > const & _other_ )
                noexcept requires( !is_const_v< T > ) { ptr_ = const_cast< T * >( _other_.ptr_ ) ; return *this ; }

        constexpr iterator_base ( iterator_base< T const, IterCat, DiffType, T const *, T const & > && _other_ )
                noexcept requires( !is_const_v< T > ) : ptr_( const_cast< T * >( _other_.ptr_ ) ) {} ;

        constexpr iterator_base & operator= ( iterator_base< T const, IterCat, DiffType, T const *, T const & > && _other_ )
                noexcept requires( !is_const_v< T > ) { ptr_ = const_cast< T * >( _other_.ptr_ ) ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        /// u8_t to whatever
        ////////////////////////////////////////////////////////////////////////////////

        constexpr iterator_base ( iterator_base< u8_t, IterCat, DiffType, u8_t *, u8_t & > const & _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > )
                : ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) {}

        constexpr iterator_base & operator= ( iterator_base< u8_t, IterCat, DiffType, u8_t *, u8_t & > const & _other_ ) noexcept
                requires( !meta::same_as< remove_const_t< T >, u8_t > )
        { ptr_ = static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ; return *this ; }

        constexpr iterator_base ( iterator_base< u8_t, IterCat, DiffType, u8_t *, u8_t & > && _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > )
                : ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) {}

        constexpr iterator_base & operator= ( iterator_base< u8_t, IterCat, DiffType, u8_t *, u8_t & > && _other_ ) noexcept
                requires( !meta::same_as< remove_const_t< T >, u8_t > )
        { ptr_ = static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        /// u8_t const to whatever const
        ////////////////////////////////////////////////////////////////////////////////

        constexpr iterator_base ( iterator_base< u8_t const, IterCat, DiffType, u8_t const *, u8_t const & > const & _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > && is_const_v< T > )
                : ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) {}

        constexpr iterator_base & operator= ( iterator_base< u8_t const, IterCat, DiffType, u8_t const *, u8_t const & > const & _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > && is_const_v< T > )
        { ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) ; return *this ; }

        constexpr iterator_base ( iterator_base< u8_t const, IterCat, DiffType, u8_t const *, u8_t const & > && _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > && is_const_v< T > )
                : ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) {}

        constexpr iterator_base & operator= ( iterator_base< u8_t const, IterCat, DiffType, u8_t const *, u8_t const & > && _other_ )
                noexcept requires( !meta::same_as< remove_const_t< T >, u8_t > && is_const_v< T > )
        { ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        /// whatever to u8_t
        ////////////////////////////////////////////////////////////////////////////////

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t > && !is_const_v< T1 > )
        constexpr iterator_base ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > const & _other_ ) noexcept
                : ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t > && !is_const_v< T1 > )
        constexpr iterator_base & operator= ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > const & _other_ ) noexcept
        { ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) ; return *this ; }

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t > && !is_const_v< T1 > )
        constexpr iterator_base ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > && _other_ ) noexcept
                : ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t > && !is_const_v< T1 > )
        constexpr iterator_base & operator= ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > && _other_ ) noexcept
        { ptr_( static_cast< pointer >( static_cast< void * >( _other_.ptr_ ) ) ) ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        /// whatever to u8_t const
        ////////////////////////////////////////////////////////////////////////////////

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t const > )
        constexpr iterator_base ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > const & _other_ ) noexcept
                : ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t const > )
        constexpr iterator_base & operator= ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > const & _other_ ) noexcept
        { ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) ; return *this ; }

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t const > )
        constexpr iterator_base ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > && _other_ ) noexcept
                : ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) {}

        template< typename T1 >
                requires( !meta::same_as< remove_const_t< T >, remove_const_t< T1 > > && meta::same_as< T, u8_t const > )
        constexpr iterator_base & operator= ( iterator_base< T1, IterCat, DiffType, T1 *, T1 & > && _other_ ) noexcept
        { ptr_( static_cast< pointer >( static_cast< void const * >( _other_.ptr_ ) ) ) ; return *this ; }

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        constexpr ~iterator_base () noexcept = default ;

        template< typename IterTag >
        constexpr iterator_base ( iterator_base< T, IterTag, DiffType, Pointer, Reference > const & _other_ ) noexcept
                requires( is_base_of_v< IterTag, iterator_category > )
                : ptr_( _other_.ptr_ ) {}

        template< typename IterTag >
        constexpr iterator_base ( iterator_base< T, IterTag, DiffType, Pointer, Reference > const & _other_ ) noexcept
                requires( is_base_of_v< iterator_category, IterTag > )
                : ptr_( _other_.ptr_ ) {}

        constexpr iterator_base & operator++ (     ) noexcept {                     ++ptr_ ; return *this ; }
        constexpr iterator_base   operator++ ( int ) noexcept { auto prev = *this ; ++ptr_ ; return  prev ; }

        constexpr iterator_base & operator-- (     ) noexcept requires _has_bidirectional_iterator_category_v< _self > {                     --ptr_ ; return *this ; }
        constexpr iterator_base   operator-- ( int ) noexcept requires _has_bidirectional_iterator_category_v< _self > { auto prev = *this ; --ptr_ ; return  prev ; }

        constexpr iterator_base & operator+= ( difference_type const _n_ ) noexcept requires _has_random_access_iterator_category_v< _self > { this->ptr_ += _n_ ; return *this ; }
        constexpr iterator_base & operator-= ( difference_type const _n_ ) noexcept requires _has_random_access_iterator_category_v< _self > { this->ptr_ -= _n_ ; return *this ; }

        constexpr reference operator*  () const noexcept { return *ptr_ ; }
        constexpr pointer   operator-> () const noexcept { return  ptr_ ; }

        constexpr operator pointer ()       noexcept { return ptr_ ; }
        constexpr operator pointer () const noexcept { return ptr_ ; }

        friend constexpr bool operator== ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_input_iterator_category_v< _self >
        {
                return _lhs_.ptr_ == _rhs_.ptr_ ;
        }
        friend constexpr bool operator!= ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_input_iterator_category_v< _self >
        {
                return _lhs_.ptr_ != _rhs_.ptr_ ;
        }
        friend constexpr bool operator== ( iterator_base const & _iter_, nullptr_t ) noexcept
                requires _has_forward_iterator_category_v< _self >
        {
                return _iter_.ptr_ == nullptr ;
        }
        friend constexpr bool operator!= ( iterator_base const & _iter_, nullptr_t ) noexcept
                requires _has_forward_iterator_category_v< _self >
        {
                return _iter_.ptr_ != nullptr ;
        }
        friend constexpr void swap ( iterator_base & _lhs_, iterator_base & _rhs_ ) noexcept
                requires _has_input_iterator_category_v< _self >
        {
                auto _tmp_ = _lhs_.ptr_ ;
                _lhs_.ptr_ = _rhs_.ptr_ ;
                _rhs_.ptr_ = _tmp_ ;
        }

        friend constexpr bool operator== ( iterator_base const & _lhs_, pointer const & _rhs_ ) noexcept
        {
                return _lhs_.ptr_ == _rhs_ ;
        }
        friend constexpr bool operator== ( pointer const & _lhs_, iterator_base const & _rhs_ ) noexcept
        {
                return _lhs_ == _rhs_.ptr_ ;
        }

        friend constexpr bool operator== ( iterator_base const & _lhs_,
                                           iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >,
                                           add_lvalue_reference_t< remove_const_t< T > > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.ptr_ == _rhs_.ptr_ ;
        }

        friend constexpr bool operator< ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ < _rhs_.ptr_ ;
        }
        friend constexpr bool operator> ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ > _rhs_.ptr_ ;
        }
        friend constexpr bool operator<= ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ <= _rhs_.ptr_ ;
        }
        friend constexpr bool operator>= ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ >= _rhs_.ptr_ ;
        }

        friend constexpr bool operator< ( iterator_base const & _lhs_, pointer const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ < _rhs_ ;
        }

        friend constexpr bool operator> ( iterator_base const & _lhs_, pointer const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ > _rhs_ ;
        }

        friend constexpr bool operator<= ( iterator_base const & _lhs_, pointer const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ <= _rhs_ ;
        }

        friend constexpr bool operator>= ( iterator_base const & _lhs_, pointer const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ >= _rhs_ ;
        }

        friend constexpr bool operator< ( pointer const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_ < _rhs_.ptr_ ;
        }

        friend constexpr bool operator> ( pointer const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_ > _rhs_.ptr_ ;
        }

        friend constexpr bool operator<= ( pointer const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_ <= _rhs_.ptr_ ;
        }

        friend constexpr bool operator>= ( pointer const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_ >= _rhs_.ptr_ ;
        }

        friend constexpr bool operator< ( iterator_base const & _lhs_,
                                          iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >,
                                          add_lvalue_reference_t< remove_const_t< T > > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.ptr_ < _rhs_.ptr_ ;
        }

        friend constexpr bool operator> ( iterator_base const & _lhs_,
                                          iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >,
                                          add_lvalue_reference_t< remove_const_t< T > > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.ptr_ > _rhs_.ptr_ ;
        }

        friend constexpr bool operator< ( iterator_base const & _lhs_,
                                          iterator_base< T const, IterCat, DiffType, T const *, T const & > const & _rhs_ ) noexcept
                requires( !is_const_v< T > )
        {
                return _lhs_.ptr_ < _rhs_.ptr_ ;
        }

        friend constexpr bool operator> ( iterator_base const & _lhs_,
                                          iterator_base< T const, IterCat, DiffType, T const *, T const & > const & _rhs_ ) noexcept
                requires( !is_const_v< T > )
        {
                return _lhs_.ptr_ > _rhs_.ptr_ ;
        }

        friend constexpr iterator_base operator+ ( iterator_base const & _iter_, difference_type const _diff_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return iterator_base{ _iter_.ptr_ + _diff_ } ;
        }
        friend constexpr iterator_base operator+ ( difference_type const _diff_, iterator_base const & _iter_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return iterator_base{ _iter_.ptr_ + _diff_ } ;
        }

        friend constexpr iterator_base operator- ( iterator_base const & _iter_, difference_type const _diff_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return iterator_base{ _iter_.ptr_ - _diff_ } ;
        }
        friend constexpr difference_type operator- ( iterator_base const & _lhs_, iterator_base const & _rhs_ ) noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return _lhs_.ptr_ - _rhs_.ptr_ ;
        }

        friend constexpr difference_type operator- ( iterator_base const & _lhs_,
                                                   iterator_base< remove_const_t< T >, IterCat, DiffType, add_pointer_t< remove_const_t< T > >,
                                                   add_lvalue_reference_t< remove_const_t< T > > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.ptr_ - _rhs_.ptr_ ;
        }

        constexpr reference operator[] ( difference_type const _idx_ ) const noexcept
                requires _has_random_access_iterator_category_v< _self >
        {
                return this->ptr_[ _idx_ ] ;
        }

        pointer ptr_ ;
} ;


} // namespace uti

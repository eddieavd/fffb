//
//
//      uti
//      core/iterator/circular_iterator.hxx
//

#pragma once

#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/container/meta.hxx>


namespace uti
{


////////////////////////////////////////////////////////////////////////////////

namespace meta
{


template< typename T >
concept circular_container = requires( T & t, T const & ct )
{
        simple_container< T > ;

        { T::              buffer_iterator } -> random_access_iterator ;
        { T::        const_buffer_iterator } -> random_access_iterator ;
        { T::      reverse_buffer_iterator } -> random_access_iterator ;
        { T::const_reverse_buffer_iterator } -> random_access_iterator ;

        {  t.buffer_begin() } -> same_as< typename T::      buffer_iterator > ;
        {  t.buffer_end  () } -> same_as< typename T::      buffer_iterator > ;
        { ct.buffer_begin() } -> same_as< typename T::const_buffer_iterator > ;
        { ct.buffer_end  () } -> same_as< typename T::const_buffer_iterator > ;
} ;


} // namespace meta

////////////////////////////////////////////////////////////////////////////////

template< typename T >
class circular_iterator : public iterator_base< conditional_t< is_const_v< T >, typename T::value_type const, typename T::value_type >, circular_iterator_tag >
{
//      static_assert( meta::circular_container< T > ) ;

        using _self = circular_iterator ;
        using _base = iterator_base< conditional_t< is_const_v< T >, typename T::value_type const, typename T::value_type >, circular_iterator_tag > ;

        using buffer_type = T ;
public:
        using        value_type = typename _base::       value_type ;
        using           pointer = typename _base::          pointer ;
        using         reference = typename _base::        reference ;
        using   difference_type = typename _base::  difference_type ;
        using iterator_category = typename _base::iterator_category ;

        constexpr circular_iterator ( buffer_type & _buffer_, pointer _ptr_, difference_type const _index_ ) noexcept : _base( _ptr_ ), buffer_{ _buffer_ }, index_{ _index_ } {}

        constexpr circular_iterator & operator++ () noexcept
        {
                if( this->ptr_ >= buffer_.buffer_end() - 1 )
                {
                        this->ptr_ = buffer_.buffer_begin() ;
                }
                else
                {
                        ++this->ptr_ ;
                }
                ++index_ ;

                return *this ;
        }
        constexpr circular_iterator operator++ ( int ) noexcept { auto prev = *this ; ++*this ; return prev ; }

        constexpr circular_iterator & operator-- () noexcept
        {
                if( this->ptr_ == buffer_.buffer_begin() )
                {
                        this->ptr_ = buffer_.buffer_end() - 1 ;
                }
                else
                {
                        --this->ptr_ ;
                }
                --index_ ;

                return *this ;
        }
        constexpr circular_iterator operator-- ( int ) noexcept { auto prev = *this ; --*this ; return prev ; }

        constexpr circular_iterator & operator+= ( difference_type const _n_ ) noexcept ;
        constexpr circular_iterator & operator-= ( difference_type const _n_ ) noexcept ;

        friend constexpr bool operator<  ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() <  _rhs_.index() ; }
        friend constexpr bool operator>  ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() >  _rhs_.index() ; }
        friend constexpr bool operator<= ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() <= _rhs_.index() ; }
        friend constexpr bool operator>= ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() >= _rhs_.index() ; }

        friend constexpr bool operator== ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() == _rhs_.index() ; }
        friend constexpr bool operator!= ( circular_iterator const & _lhs_, circular_iterator const & _rhs_ ) noexcept { return _lhs_.index() != _rhs_.index() ; }

        friend constexpr bool operator< ( circular_iterator                        const & _lhs_ ,
                                          circular_iterator< remove_const_t< T > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.index() < _rhs_.index() ;
        }

        friend constexpr bool operator> ( circular_iterator                        const & _lhs_ ,
                                          circular_iterator< remove_const_t< T > > const & _rhs_ ) noexcept
                requires is_const_v< T >
        {
                return _lhs_.index() > _rhs_.index() ;
        }

        friend constexpr bool operator< ( circular_iterator            const & _lhs_ ,
                                          circular_iterator< T const > const & _rhs_ ) noexcept
                requires( !is_const_v< T > )
        {
                return _lhs_.index() < _rhs_.index() ;
        }

        friend constexpr bool operator> ( circular_iterator            const & _lhs_ ,
                                          circular_iterator< T const > const & _rhs_ ) noexcept
                requires( !is_const_v< T > )
        {
                return _lhs_.index() > _rhs_.index() ;
        }

        constexpr difference_type index () const noexcept
        {
                return index_ ;
        }
private:
        buffer_type  & buffer_ ;
        difference_type index_ ;
} ;

////////////////////////////////////////////////////////////////////////////////


} // namespace uti

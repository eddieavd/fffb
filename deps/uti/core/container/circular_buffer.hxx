//
//
//      uti
//      core/container/circular_buffer.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/type/traits.hxx>
#include <uti/core/meta/concepts.hxx>

#include <uti/core/allocator/meta.hxx>

#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/circular_iterator.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>


namespace uti
{


////////////////////////////////////////////////////////////////////////////////

template< meta::trivial T, ssize_t Capacity >
class circular_buffer
{
        static constexpr ssize_t capacity_ { Capacity } ;
public:
        using      value_type =       T ;
        using       size_type =  size_t ;
        using      ssize_type = ssize_t ;
        using difference_type = ssize_t ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using               iterator = circular_iterator< circular_buffer       > ;
        using         const_iterator = circular_iterator< circular_buffer const > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

        using               buffer_iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using         const_buffer_iterator = iterator_base< value_type const, random_access_iterator_tag > ;
        using       reverse_buffer_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_buffer_iterator = ::uti::reverse_iterator< const_iterator > ;

        constexpr  circular_buffer () noexcept = default ;
        constexpr ~circular_buffer () noexcept = default ;

        constexpr circular_buffer ( ssize_type _count_, value_type const & _value_ ) noexcept ;

        constexpr void push_back ( value_type const & _value_ ) noexcept ;

        UTI_NODISCARD constexpr ssize_type capacity () const noexcept { return      capacity_ ; }
        UTI_NODISCARD constexpr ssize_type     size () const noexcept { return        _size() ; }
        UTI_NODISCARD constexpr bool          empty () const noexcept { return begin_ == end_ ; }

        UTI_NODISCARD constexpr       pointer data ()       noexcept { return data_ ; }
        UTI_NODISCARD constexpr const_pointer data () const noexcept { return data_ ; }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return       iterator( *this, begin_, 0 ) ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return const_iterator( *this, begin_, 0 ) ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return                        begin()     ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return       iterator( *this, end_ + 1, size() ) ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return const_iterator( *this, end_ + 1, size() ) ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return                        end()              ; }

        UTI_NODISCARD constexpr       buffer_iterator buffer_begin  ()       noexcept { return       buffer_iterator( data_ ) ; }
        UTI_NODISCARD constexpr const_buffer_iterator buffer_begin  () const noexcept { return const_buffer_iterator( data_ ) ; }
        UTI_NODISCARD constexpr const_buffer_iterator buffer_cbegin () const noexcept { return const_buffer_iterator( data_ ) ; }

        UTI_NODISCARD constexpr       buffer_iterator buffer_end  ()       noexcept { return       buffer_iterator( data_ + capacity_ ) ; }
        UTI_NODISCARD constexpr const_buffer_iterator buffer_end  () const noexcept { return const_buffer_iterator( data_ + capacity_ ) ; }
        UTI_NODISCARD constexpr const_buffer_iterator buffer_cend () const noexcept { return const_buffer_iterator( data_ + capacity_ ) ; }
private:
        value_type data_ [ capacity_ ] {} ;

        pointer begin_ { data_ } ;
        pointer   end_ { data_ } ;

        bool full_ { false } ;

        constexpr ssize_type _size () const noexcept { return full_ ? capacity_ : end_ - begin_ ; }
} ;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template< meta::trivial T, ssize_t Capacity >
constexpr
circular_buffer< T, Capacity >::circular_buffer ( ssize_type _count_, value_type const & _value_ ) noexcept
{
        if( _count_ > capacity() ) _count_ = capacity() ;

        for( ssize_type i = 0; i < _count_; ++i )
        {
                push_back( _value_ ) ;
        }
}

////////////////////////////////////////////////////////////////////////////////

template< meta::trivial T, ssize_t Capacity >
constexpr void
circular_buffer< T, Capacity >::push_back ( value_type const & _value_ ) noexcept
{
        *end_ = _value_ ;

        if( full_ )
        {
                UTI_CEXPR_ASSERT( begin_ == end_, "uti::circular_buffer::push_back : internal error 116" ) ;

                if( begin_ >= buffer_end() - 1 )
                {
                        begin_ = buffer_begin() ;
                        end_   = buffer_begin() ;
                }
                else
                {
                        ++begin_ ;
                        ++end_   ;
                }
        }
        else
        {
                UTI_CEXPR_ASSERT( begin_ == buffer_begin(), "uti::circular_buffer::push_back : internal error 131" ) ;

                if( end_ >= buffer_end() - 1 )
                {
                        end_ = buffer_begin() ;
                        full_ = true ;
                }
                else
                {
                        ++end_ ;
                }
        }
}

////////////////////////////////////////////////////////////////////////////////


} // namespace uti

//
//
//      uti
//      core/container/array.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/container/meta.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>


namespace uti
{


template< typename T, ssize_t Size >
class array
{
        using _self = array ;
public:
        using      value_type =       T ;
        using       size_type =  size_t ;
        using      ssize_type = ssize_t ;
        using difference_type = ssize_type ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using               iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using         const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

        template< typename Self >
        UTI_NODISCARD constexpr decltype( auto ) operator[] ( this Self && self, ssize_type _index_ ) noexcept
        { return UTI_FWD( self ).data[ _index_ ] ; }

        template< typename Self >
        UTI_NODISCARD constexpr decltype( auto ) at ( this Self && self, ssize_type _index_ ) noexcept
        { return UTI_FWD( self ).data[ _index_ ] ; }

        template< typename Self > UTI_NODISCARD constexpr decltype( auto ) front ( this Self && self ) noexcept { return UTI_FWD( self ).data[        0 ] ; }
        template< typename Self > UTI_NODISCARD constexpr decltype( auto )  back ( this Self && self ) noexcept { return UTI_FWD( self ).data[ Size - 1 ] ; }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return       iterator{ data } ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return const_iterator{ data } ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return       iterator{ data + Size } ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return const_iterator{ data + Size } ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return end() ; }

        UTI_NODISCARD constexpr       iterator  rbegin ()       noexcept { return       reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_iterator  rbegin () const noexcept { return const_reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_iterator crbegin () const noexcept { return rbegin() ; }

        UTI_NODISCARD constexpr       iterator  rend ()       noexcept { return       reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_iterator  rend () const noexcept { return const_reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_iterator crend () const noexcept { return rend() ; }

        UTI_NODISCARD constexpr ssize_type     size () const noexcept { return Size ; }
        UTI_NODISCARD constexpr ssize_type capacity () const noexcept { return Size ; }

        value_type data [ Size ] ;
} ;


} // namespace uti

//
//
//      uti
//      core/container/view.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>
#include <uti/core/iterator/reverse_iterator.hxx>
#include <uti/core/algo/distance.hxx>
#include <uti/core/container/meta.hxx>


namespace uti
{


template< typename T >
class view
{
        using _self = view ;
public:
        using      value_type =       T    ;
        using       size_type =  size_t    ;
        using      ssize_type = ssize_t    ;
        using difference_type = ssize_type ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using               iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using         const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;
        using       reverse_iterator = ::uti::reverse_iterator<       iterator > ;
        using const_reverse_iterator = ::uti::reverse_iterator< const_iterator > ;

        constexpr view () noexcept = default ;

        constexpr view ( pointer const _begin_, pointer const _end_ ) noexcept
                : begin_( _begin_ ), size_( _end_ - _begin_ ) {}

        constexpr view ( pointer const _begin_, ssize_type const _size_ ) noexcept
                : begin_( _begin_ ), size_( _size_ ) {}

        template< meta::simple_container Other >
        constexpr view ( Other const & _other_ ) noexcept requires( !is_instance_of_v< Other, view > && !is_base_of_v< _self, Other > )
                : begin_( _other_.begin() ), size_( ::uti::distance( _other_.begin(), _other_.end() ) ) {}

        constexpr view             ( view const &  _other_ ) noexcept = default ;
        constexpr view             ( view       && _other_ ) noexcept           ;
        constexpr view & operator= ( view const &  _other_ ) noexcept = default ;
        constexpr view & operator= ( view       && _other_ ) noexcept           ;

        constexpr ~view () noexcept = default ;

        constexpr void pop_back  () noexcept {            --size_ ; }
        constexpr void pop_front () noexcept { ++begin_ ; --size_ ; }

        template< typename Self >
        UTI_NODISCARD constexpr
        auto & operator[] ( this Self && self, ssize_type const _index_ ) noexcept
        {
                return UTI_FWD( self ).begin_[ _index_ ];
        }

        template< typename Self >
        UTI_NODISCARD UTI_DEEP_INLINE constexpr
        auto & at ( this Self && self, ssize_type const _index_ ) noexcept
        {
                UTI_CEXPR_ASSERT( 0 <= _index_ && _index_ < UTI_FWD( self ).size(), "uti::view::at: index out of range" );

                return UTI_FWD( self ).begin_[ _index_ ];
        }

        template< typename Self, typename... Idxs >
        UTI_NODISCARD UTI_DEEP_INLINE constexpr
        decltype( auto ) at ( this Self && self, ssize_type const _x_, Idxs... _idxs_ ) noexcept
//              requires( is_n_dim_container_v< _self, sizeof...( _idxs_ ) + 1 > )
        {
                return UTI_FWD( self ).at( _x_ ).at( _idxs_... );
        }

        UTI_NODISCARD constexpr ssize_type  size () const noexcept { return  size_ ; }
        UTI_NODISCARD constexpr       bool empty () const noexcept { return !size_ ; }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return       iterator{ begin_ }  ; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return const_iterator{ begin_ }  ; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return begin() ; }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return       iterator{ begin_ + size_ } ; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return const_iterator{ begin_ + size_ } ; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return end(); }

        UTI_NODISCARD constexpr       reverse_iterator  rbegin ()       noexcept { return       reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rbegin () const noexcept { return const_reverse_iterator{ --end() } ; }
        UTI_NODISCARD constexpr const_reverse_iterator crbegin () const noexcept { return rbegin() ; }

        UTI_NODISCARD constexpr       reverse_iterator  rend ()       noexcept { return       reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_reverse_iterator  rend () const noexcept { return const_reverse_iterator{ --begin() } ; }
        UTI_NODISCARD constexpr const_reverse_iterator crend () const noexcept { return rend() ; }

        template< typename Self >
        UTI_NODISCARD constexpr
        auto & front ( this Self && self ) noexcept
        {
                UTI_CEXPR_ASSERT( !UTI_FWD( self ).empty(), "uti::view::front: called on empty view" );

                return *( UTI_FWD( self ).begin_ );
        }
        UTI_NODISCARD constexpr const_reference cfront () const noexcept { return front(); }

        template< typename Self >
        UTI_NODISCARD constexpr
        auto & back ( this Self && self ) noexcept
        {
                UTI_CEXPR_ASSERT( !UTI_FWD( self ).empty(), "uti::view::back: called on empty view" );

                return *( UTI_FWD( self ).end() - 1 );
        }
        UTI_NODISCARD constexpr const_reference cback () const noexcept { return back(); }

        constexpr ssize_type memory_usage () const noexcept
        {

        }
protected:
        iterator  begin_ { nullptr } ;
        ssize_type size_ {       0 } ;

        UTI_NODISCARD constexpr iterator         & _begin ()       noexcept { return begin_ ; }
        UTI_NODISCARD constexpr iterator   const & _begin () const noexcept { return begin_ ; }
        UTI_NODISCARD constexpr ssize_type       & _size  ()       noexcept { return  size_ ; }
        UTI_NODISCARD constexpr ssize_type const & _size  () const noexcept { return  size_ ; }
};


template< typename T >
constexpr
view< T >::view ( view && _other_ ) noexcept
        : begin_( _other_.begin_ ), size_( _other_.size_ )
{
        _other_.begin_ = nullptr ;
        _other_. size_ =       0 ;
}

template< typename T >
constexpr
view< T > &
view< T >::operator= ( view && _other_ ) noexcept
{
        begin_ = _other_.begin_ ;
        size_  = _other_. size_ ;

        _other_.begin_ = nullptr ;
        _other_. size_ =       0 ;

        return *this;
}


} // namespace uti

//
//
//      uti
//      core/allocator/resource.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/allocator/meta.hxx>
#include <uti/core/iterator/meta.hxx>
#include <uti/core/iterator/base.hxx>


namespace uti
{


template< typename Resource > using has_capacity = decltype( Resource::capacity() ) ;


template< typename Resource >
struct resource_traits
{
public:
        using   resource_type = Resource ;
        using      value_type = typename resource_type::value_type ;
        using       size_type = detected_or_t<  size_t, meta::      has_size_type, resource_type > ;
        using      ssize_type = detected_or_t< ssize_t, meta::     has_ssize_type, resource_type > ;
        using difference_type = detected_or_t< ssize_t, meta::has_difference_type, resource_type > ;

        using block_type = typename resource_type::block_type ;

        using         pointer = detected_or_t< value_type       *, meta::        has_pointer, resource_type > ;
        using   const_pointer = detected_or_t< value_type const *, meta::  has_const_pointer, resource_type > ;
        using       reference = detected_or_t< value_type       &, meta::      has_reference, resource_type > ;
        using const_reference = detected_or_t< value_type const &, meta::has_const_reference, resource_type > ;

        using        iterator = detected_or_t<       pointer, meta::      has_iterator, resource_type > ;
        using  const_iterator = detected_or_t< const_pointer, meta::has_const_iterator, resource_type > ;

        UTI_NODISCARD UTI_DEEP_INLINE static constexpr
        block_type allocate ( ssize_type _bytes_, ssize_type const _align_ ) noexcept
        {
                return resource_type::allocate( _bytes_, _align_ ) ;
        }

        UTI_DEEP_INLINE static constexpr
        void reallocate ( block_type & _block_, ssize_type _bytes_, ssize_type const _align_ ) noexcept
        {
                resource_type::reallocate( _block_, _bytes_, _align_ ) ;
        }

        UTI_NODISCARD UTI_DEEP_INLINE static constexpr
        bool realloc_inplace ( block_type & _block_, ssize_type _bytes_ ) noexcept
                requires( is_detected_v< has_inplace_realloc, resource_type, value_type > )
        {
                return resource_type::realloc_inplace( _block_, _bytes_ ) ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr
        bool realloc_inplace ( block_type &, ssize_type ) noexcept
                requires( !is_detected_v< has_inplace_realloc, resource_type, value_type > )
        {
                return false ;
        }

        UTI_DEEP_INLINE static constexpr
        void deallocate ( block_type & _block_ ) noexcept
        {
                resource_type::deallocate( _block_ ) ;
        }

        UTI_NODISCARD UTI_DEEP_INLINE static constexpr
        ssize_type capacity () noexcept
                requires( is_detected_v< has_capacity, resource_type > )
        {
                return resource_type::capacity() ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr
        ssize_type capacity () noexcept
                requires( !is_detected_v< has_capacity, resource_type > )
        {
                return i64_t_max ;
        }

        UTI_DEEP_INLINE static constexpr
        void reset () noexcept
        {
                resource_type::reset() ;
        }
} ;


struct malloc_resource
{
        using value_type =    u8_t ;
        using  size_type =  size_t ;
        using ssize_type = ssize_t ;
        using block_type = block_t< value_type > ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using       iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;

        static constexpr ssize_type      id { 0 } ;
        static constexpr ssize_type memsize { 0 } ;

        UTI_NODISCARD static constexpr block_type allocate ( ssize_type const _bytes_, ssize_type const ) noexcept
        {
                iterator buff = ::uti::alloc_buffer( _bytes_ ) ;

                return buff != nullptr ? block_type{    buff, _bytes_ }
                                       : block_type{ nullptr,      0  } ;
        }

        static constexpr void reallocate ( block_type & _block_, ssize_type const _bytes_, ssize_type const ) noexcept
        {
                iterator buff = ::uti::realloc_buffer( _block_.begin(), _bytes_ ) ;

                if( buff )
                {
                        _block_.size_ = _bytes_ ;
                }
                if( buff != _block_.begin() )
                {
                        _block_.begin_ = buff ;
                }
        }

        static constexpr void deallocate ( block_type & _block_ ) noexcept
        {
                ::uti::dealloc_buffer( _block_.begin() ) ;

                _block_.begin_ = nullptr ;
                _block_. size_ =       0 ;
        }
        static constexpr void reset () noexcept {}
} ;


template< ssize_t MemSize, ssize_t Id = 0 >
struct static_bump_resource
{
        using value_type =    u8_t ;
        using  size_type =  size_t ;
        using ssize_type = ssize_t ;
        using block_type = block_t< value_type > ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using       iterator = iterator_base< value_type      , random_access_iterator_tag > ;
        using const_iterator = iterator_base< value_type const, random_access_iterator_tag > ;

        static constexpr ssize_type      id {      Id } ;
        static constexpr ssize_type memsize { MemSize } ;

        UTI_NODISCARD static constexpr block_type allocate ( ssize_type const _bytes_, ssize_type const _align_ ) noexcept
        {
                block_type block {} ;

                if( _mem_available( _align_ ) >= _bytes_ )
                {
                        _align_end( _align_ ) ;

                        block.begin_ =    end_ ;
                        block. size_ = _bytes_ ;
                        end_        += _bytes_ ;
                }
                return block ;
        }

        static constexpr void reallocate ( block_type & _block_, ssize_type const _bytes_, ssize_type const _align_ ) noexcept
        {
                if( _can_realloc_inplace( _block_, _bytes_ ) )
                {
                        _realloc_inplace( _block_, _bytes_ ) ;
                }
                else
                {
                        block_type new_block = allocate( _bytes_, _align_ ) ;

                        if( !new_block ) return ;

                        ::uti::copy( _block_.begin(), _block_.end(), new_block.begin() ) ;
                        _block_ = new_block ;
                }
        }

        UTI_NODISCARD static constexpr bool realloc_inplace ( block_type & _block_, ssize_type const _bytes_ ) noexcept
        {
                if( _can_realloc_inplace( _block_, _bytes_ ) )
                {
                        _realloc_inplace( _block_, _bytes_ ) ;
                        return true ;
                }
                return false ;
        }

        static constexpr void deallocate ( block_type & _block_ ) noexcept
        {
                if( _is_last_block( _block_ ) )
                {
                        end_ = _block_.begin() ;
                }
                _block_.begin_ = nullptr ;
                _block_. size_ =       0 ;
        }

        static constexpr void reset () noexcept
        {
                end_ = mem_.mem_ ;
        }

        UTI_NODISCARD static constexpr ssize_type capacity () noexcept { return memsize ; }

        UTI_NODISCARD static constexpr       iterator  begin () noexcept { return mem_.mem_ ; }
        UTI_NODISCARD static constexpr const_iterator cbegin () noexcept { return begin()   ; }
private:
        struct alignas( 64 ) mem
        {
                value_type mem_ [ memsize ] ;
        } ;

        inline static mem      mem_ ;
        inline static iterator end_ { mem_.mem_ } ;

        UTI_NODISCARD static constexpr       iterator  _end () noexcept { return end_   ; }
        UTI_NODISCARD static constexpr const_iterator _cend () noexcept { return _end() ; }

        static constexpr void _align_end ( ssize_type const _align_ ) noexcept
        {
                end_ = _aligned_to( end_, _align_ ) ;
        }

        static constexpr iterator _aligned_to ( iterator _iter_, ssize_type const _align_ ) noexcept
        {
                size_type mask = _align_ - 1 ;

                while( ( _iter_ & mask ) != 0 )
                {
                        ++_iter_ ;
                }
                return _iter_ ;
        }

        static constexpr ssize_type _mem_available (                          ) noexcept { return memsize - ::uti::distance( cbegin(), _cend() ) ; }
        static constexpr ssize_type _mem_available ( ssize_type const _align_ ) noexcept
        {
                const_iterator aligned_end = _aligned_to( end_, _align_ ) ;

                return memsize - ::uti::distance( cbegin(), aligned_end ) ;
        }

        static constexpr bool _is_last_block ( block_type const & _block_ ) noexcept { return _block_.cbegin() + _block_.size_ == _cend() ; }

        static constexpr bool _can_realloc_inplace ( block_type const & _block_, ssize_type const _new_capacity_ ) noexcept
        {
                return _is_last_block( _block_ ) && _mem_available() >= ( _new_capacity_ - _block_.size_ ) ;
        }

        static constexpr void _realloc_inplace ( block_type & _block_, ssize_type const _new_capacity_ ) noexcept
        {
                end_ += _new_capacity_ - _block_.size_ ;
                _block_.size_ = _new_capacity_ ;
        }
} ;


} // namespace uti

//
//
//      uti
//      core/allocator/default.hxx
//

#pragma once

#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/resource.hxx>


namespace uti
{


template< typename T, typename Resource >
class allocator
{
public:
        using      value_type =       T    ;
        using       size_type =  size_t    ;
        using      ssize_type = ssize_t    ;
        using difference_type = ssize_type ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using          block_type = block_t< value_type > ;
        using       resource_type = Resource ;
        using resource_block_type = typename resource_type::block_type ;

        using _resource_traits = resource_traits< Resource > ;

        UTI_NODISCARD UTI_DEEP_INLINE static constexpr block_type allocate ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        {
                return _resource_traits::allocate( _capacity_ * sizeof( value_type ), alignof( value_type ) ) ;
        }
        UTI_DEEP_INLINE static constexpr void reallocate ( block_type & _block_, ssize_type const _new_capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        {
                resource_block_type block( _block_ ) ;

                _resource_traits::reallocate( block, _new_capacity_ * sizeof( value_type ), alignof( value_type ) ) ;

                _block_ = block ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr bool realloc_inplace ( block_type & _block_, ssize_type const _new_capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
                requires( is_detected_v< has_inplace_realloc, resource_type, typename resource_type::value_type > )
        {
                resource_block_type block( _block_ ) ;

                if( _resource_traits::realloc_inplace( block, _new_capacity_ * sizeof( value_type ) ) )
                {
                        _block_ = block ;
                        return true ;
                }
                return false ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr bool realloc_inplace ( block_type &, ssize_type const ) UTI_NOEXCEPT_UNLESS_BADALLOC
                requires( !is_detected_v< has_inplace_realloc, resource_type, typename resource_type::value_type > )
        {
                return false ;
        }
        UTI_DEEP_INLINE static constexpr void deallocate ( block_type & _block_ ) noexcept
        {
                resource_block_type block( _block_ ) ;

                _resource_traits::deallocate( block ) ;

                _block_ = block ;
        }
} ;


} // namespace uti

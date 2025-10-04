//
//
//      uti
//      core/allocator/new.hxx
//

#pragma once

#include <uti/core/allocator/meta.hxx>


namespace uti
{


template< typename T >
class new_allocator
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

        using block_type = block_t< value_type > ;

        UTI_NODISCARD UTI_DEEP_INLINE static constexpr block_type allocate ( ssize_type const _capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        {
                return ::uti::alloc_block< value_type >( _capacity_ ) ;
        }
        UTI_DEEP_INLINE static constexpr void reallocate ( block_type & _block_, ssize_type const _new_capacity_ ) UTI_NOEXCEPT_UNLESS_BADALLOC
        {
                ::uti::realloc_block( _block_, _new_capacity_ ) ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr bool realloc_inplace ( block_type &, ssize_type const ) UTI_NOEXCEPT_UNLESS_BADALLOC
        {
                return false ;
        }
        UTI_DEEP_INLINE static constexpr void deallocate ( block_type & _block_ ) noexcept
        {
                ::uti::dealloc_block( _block_ ) ;
        }
        UTI_NODISCARD UTI_DEEP_INLINE static constexpr bool destroys_elements () noexcept
        {
                return true ;
        }
} ;


} // namespace uti

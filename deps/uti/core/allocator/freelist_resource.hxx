//
//
//      uti
//      core/allocator/freelist_resource.hxx
//

#pragma once

#include <uti/core/allocator/meta.hxx>
#include <uti/core/allocator/default.hxx>
#include <uti/core/allocator/resource.hxx>
#include <uti/core/container/list.hxx>

#ifndef   UTI_FREELIST_CAP
#define   UTI_FREELIST_CAP 64
#endif // UTI_FREELIST_CAP

#ifndef   UTI_FREELIST_TAG
#define   UTI_FREELIST_TAG 8372
#endif // UTI_FREELIST_TAG


namespace uti
{


namespace _detail
{


template< ssize_t Id = UTI_FREELIST_TAG >
using _internal_freelist_resource = static_bump_resource< UTI_FREELIST_CAP * sizeof( uti::list_node< block_t< u8_t > > ), Id > ;


} // namespace _detail


template< ssize_t MemSize, ssize_t Id = 0, typename InternalResource = _detail::_internal_freelist_resource< UTI_FREELIST_TAG + Id > >
struct static_freelist_resource
{
        using value_type =    u8_t ;
        using  size_type =  size_t ;
        using ssize_type = ssize_t ;
        using block_type = block_t< value_type > ;

        using internal_resource_type  = InternalResource ;
        using freelist_type           = uti::list< block_type, uti::allocator< list_node< block_type >, internal_resource_type > > ;
        using freelist_iterator       = typename freelist_type::      iterator ;
        using freelist_const_iterator = typename freelist_type::const_iterator ;

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
                if( freelist_.empty() )
                {
                        return block_type{ nullptr, 0 } ;
                }
                freelist_iterator list_iter( freelist_.begin() ) ;
                freelist_iterator best_block( list_iter ) ;

                while( list_iter != freelist_.end() )
                {
                        block_type aligned = _align_to( *list_iter, _align_ ) ;

                        if( aligned.size_ >= _bytes_ && ( best_block->size_ < _bytes_ ||
                                                          best_block->size_ > aligned.size_) )
                        {
                                best_block = list_iter ;
                        }
                        ++list_iter ;
                }
                if( best_block->size_ < _bytes_ )
                {
                        return block_type{ nullptr, 0 } ;
                }
                block_type aligned = _align_to( *best_block, _align_ ) ;

                if( best_block->begin() != aligned.begin() )
                {
                        ssize_type chop = aligned.begin() - best_block->begin() ;

                        freelist_.insert( best_block, block_type{ best_block->begin(), chop } ) ;
                }
                if( aligned.size_ > _bytes_ )
                {
                        aligned.size_ = _bytes_ ;
                        best_block->begin_ += aligned.size_ ;
                        best_block-> size_ -= aligned.size_ ;
                }
                else
                {
                        freelist_.erase( best_block ) ;
                }
                return aligned ;
        }

        static constexpr void reallocate ( block_type & _block_, ssize_type const _bytes_, ssize_type const _align_ ) noexcept
        {
                if( !realloc_inplace( _block_, _bytes_ ) )
                {
                        block_type new_block = allocate( _bytes_, _align_ ) ;

                        if( !new_block ) return ;

                        ::uti::copy( _block_.begin(), _block_.end(), new_block.begin() ) ;

                        deallocate( _block_ ) ;

                        _block_ = new_block ;
                }
        }

        UTI_NODISCARD static constexpr bool realloc_inplace ( block_type & _block_, ssize_type const _bytes_ ) noexcept
        {
                freelist_iterator storage = _can_realloc_inplace( _block_, _bytes_ ) ;

                if( storage.ptr_ )
                {
                        _realloc_inplace( _block_, storage, _bytes_ ) ;
                        return true ;
                }
                return false ;
        }

        static constexpr void deallocate ( block_type & _block_ ) noexcept
        {
                if( freelist_.empty() )
                {
                        freelist_.push_back( _block_ ) ;
                        _block_.begin_ = nullptr ;
                        _block_. size_ =       0 ;
                        return ;
                }
                freelist_iterator list_iter = freelist_.begin() ;

                while( list_iter.ptr_ && list_iter->begin() < _block_.begin() )
                {
                        ++list_iter ;
                }
                if( list_iter == freelist_.begin() )
                {
                        if( freelist_.front().begin() == _block_.begin() + _block_.size_ )
                        {
                                freelist_.front().begin_ = _block_.begin() ;
                                freelist_.front().size_ += _block_.size_ ;
                        }
                        else
                        {
                                freelist_.push_front( _block_ ) ;
                        }
                }
                else if( list_iter == freelist_.end() )
                {
                        if( freelist_.back().begin() + freelist_.back().size_ == _block_.begin() )
                        {
                                freelist_.back().size_ += _block_.size_ ;
                        }
                        else
                        {
                                freelist_.push_back( _block_ ) ;
                        }
                }
                else
                {
                        block_type & prev = list_iter.ptr_->prev_->data_ ;
                        block_type & next = list_iter.ptr_->data_ ;

                        if( prev.begin() + prev.size_ == _block_.begin() )
                        {
                                prev.size_ += _block_.size_ ;

                                if( prev.begin() + prev.size_ == next.begin() )
                                {
                                        prev.size_ += next.size_ ;

                                        freelist_.erase( list_iter ) ;
                                }
                        }
                        else if( _block_.begin() + _block_.size_ == next.begin() )
                        {
                                next.begin_ = _block_.begin() ;
                                next.size_ += _block_.size_   ;
                        }
                        else
                        {
                                freelist_.insert( list_iter, _block_ ) ;
                        }
                }
                _block_.begin_ = nullptr ;
                _block_. size_ =       0 ;
        }

        static constexpr void reset () noexcept
        {
                freelist_.clear() ;
                freelist_.push_back( block_type{ mem_.mem_, memsize } ) ;
        }

        UTI_NODISCARD static constexpr ssize_type capacity () noexcept { return memsize ; }

        UTI_NODISCARD static constexpr       iterator  begin () noexcept { return mem_.mem_ ; }
        UTI_NODISCARD static constexpr const_iterator cbegin () noexcept { return begin()   ; }
private:
        struct alignas( 64 ) mem
        {
                value_type mem_ [ memsize ] ;
        } ;

        inline static mem                mem_ ;
        inline static freelist_type freelist_ { block_type{ mem_.mem_, memsize } } ;

        static constexpr block_type _align_to ( block_type & _block_, ssize_type const _align_ ) noexcept
        {
                size_type mask = _align_ - 1 ;

                while( ( _block_.begin_ & mask ) != 0 )
                {
                        ++_block_.begin_ ;
                }
                return _block_ ;
        }

        static constexpr freelist_iterator _can_realloc_inplace ( block_type const & _block_, ssize_type const _new_capacity_ ) noexcept
        {
                if( freelist_.empty() ) return nullptr ;

                freelist_iterator list_iter = freelist_.begin() ;

                while( list_iter->begin() < _block_.begin() )
                {
                        ++list_iter ;
                }
                if( _block_.begin() + _block_.size_ == list_iter->begin() &&
                    _block_.size_ + list_iter->size_ >= _new_capacity_ )
                {
                        return list_iter ;
                }
                return nullptr ;
        }

        static constexpr void _realloc_inplace ( block_type & _block_, freelist_iterator _storage_, ssize_type const _new_capacity_ ) noexcept
        {
                if( _block_.size_ + _storage_->size_ == _new_capacity_ )
                {
                        _block_.size_ += _storage_->size_ ;
                        freelist_.erase( _storage_ ) ;
                }
                else
                {
                        ssize_type diff = _new_capacity_ - _block_.size_ ;

                        _block_.size_ += diff ;

                        _storage_->begin_ += diff ;
                        _storage_-> size_ -= diff ;
                }
        }

        static constexpr void _tag_block ( block_type & _block_, u8_t const _tag_ ) noexcept
        {
                for( auto & val : _block_ )
                {
                        val = _tag_ ;
                }
        }
} ;


} // namespace uti

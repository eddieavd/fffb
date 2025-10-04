//
//
//      uti
//      core/util/copy_counter.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP

#include <uti/core/type/traits.hxx>

#include <ostream>


namespace uti::test
{


struct copy_stats
{
        i32_t n_default_constructs { 0 } ;
        i32_t n_copies             { 0 } ;
        i32_t n_moves              { 0 } ;
        i32_t n_destructs          { 0 } ;

        constexpr bool operator== ( copy_stats const & _other_ ) const noexcept = default ;
};

inline std::ostream & operator<< ( std::ostream & stream, copy_stats const & stats )
{
        stream << "copy_stats: {\n\tdefault constructs: " << stats.n_default_constructs
                            << "\n\tcopy    constructs: " << stats.n_copies
                            << "\n\tmove    constructs: " << stats.n_moves << "\n}\n";
        return stream;
}

struct old_copy_counter
{
        inline static copy_stats stats ;

        copy_stats priv_stats ;

        old_copy_counter () noexcept { ++stats.n_default_constructs; ++priv_stats.n_default_constructs; }

        old_copy_counter             ( old_copy_counter const & ) noexcept { ++stats.n_copies; ++priv_stats.n_copies;               }
        old_copy_counter & operator= ( old_copy_counter const & ) noexcept { ++stats.n_copies; ++priv_stats.n_copies; return *this; }

        old_copy_counter             ( old_copy_counter && ) noexcept { ++stats.n_moves; ++priv_stats.n_moves;               }
        old_copy_counter & operator= ( old_copy_counter && ) noexcept { ++stats.n_moves; ++priv_stats.n_moves; return *this; }

        ~old_copy_counter () noexcept { ++stats.n_destructs; }

        static copy_stats reset () noexcept
        {
                copy_stats old_stats = stats;

                stats.n_default_constructs = 0 ;
                stats.n_copies             = 0 ;
                stats.n_moves              = 0 ;

                return old_stats;
        }
};

template< ssize_t Idx >
struct indexed_copy_counter
{
        struct reset_after_construct_t {} ;

        static constexpr reset_after_construct_t reset_after_construct {} ;

        inline static copy_stats stats ;

        explicit indexed_copy_counter ( reset_after_construct_t ) noexcept { reset() ; }

        indexed_copy_counter () noexcept { stats.n_default_constructs++ ; }

        indexed_copy_counter             ( indexed_copy_counter const &  ) noexcept { stats.n_copies++ ;                }
        indexed_copy_counter             ( indexed_copy_counter       && ) noexcept { stats.n_moves ++ ;                }
        indexed_copy_counter & operator= ( indexed_copy_counter const &  ) noexcept { stats.n_copies++ ; return *this ; }
        indexed_copy_counter & operator= ( indexed_copy_counter       && ) noexcept { stats.n_moves ++ ; return *this ; }

        static copy_stats reset () noexcept
        {
                auto old_stats             = stats ;
                stats.n_copies             = 0 ;
                stats.n_moves              = 0 ;
                stats.n_default_constructs = 0 ;
                return old_stats ;
        }

        template< ssize_t Idx1 >
        bool operator== ( indexed_copy_counter< Idx1 > const & other ) const noexcept
        {
                return stats == other.stats ;
        }
} ;


namespace _detail
{

static constexpr ssize_t default_copy_counter_idx { 102115517 } ;

} // namespace _detail


using copy_counter = indexed_copy_counter< _detail::default_copy_counter_idx > ;

template< ssize_t Idx = _detail::default_copy_counter_idx >
indexed_copy_counter< Idx > make_copy_counter ()
{
        return indexed_copy_counter< Idx >{ indexed_copy_counter< Idx >::reset_after_construct } ;
}


} // namespace uti::test

#endif // UTI_DOXYGEN_SKIP

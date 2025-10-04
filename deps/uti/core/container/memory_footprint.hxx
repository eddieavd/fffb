//
//
//      uti
//      core/container/memory_footprint.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/meta/concepts.hxx>


namespace uti
{

////////////////////////////////////////////////////////////////////////////////

struct memory_footprint
{
        constexpr ssize_t  static_footprint () const noexcept { return          static_usage_ ; }
        constexpr ssize_t dynamic_footprint () const noexcept { return top_lvl_dynamic_usage_ + low_lvl_dynamic_usage_     ; }
        constexpr ssize_t   total_footprint () const noexcept { return          static_usage_ +         dynamic_footprint() ; }

        constexpr ssize_t     main_dynamic_footprint () const noexcept { return top_lvl_dynamic_usage_ ; }
        constexpr ssize_t internal_dynamic_footprint () const noexcept { return low_lvl_dynamic_usage_ ; }

        constexpr operator ssize_t () const noexcept { return total_footprint() ; }

        ssize_t          static_usage_ ;
        ssize_t top_lvl_dynamic_usage_ ;
        ssize_t low_lvl_dynamic_usage_ ;
} ;

////////////////////////////////////////////////////////////////////////////////

namespace meta
{


template< typename T >
concept memory_reporter = requires( T const & t )
{
        { t.memory_usage() } -> convertible_to< memory_footprint > ;
} ;


} // namespace meta

////////////////////////////////////////////////////////////////////////////////

} // namespace uti

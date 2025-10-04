//
//
//      uti
//      core/container/constexpr_twostep.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/container/static_vector.hxx>
#include <uti/core/container/array.hxx>
#include <uti/core/string/string_view.hxx>
#include <uti/core/algo/copy.hxx>


namespace uti::cexpr
{


/// ideally should be consteval
/// but clang optimizes consteval functions so aggressively
/// that it gets rid of our static
template< auto Fn, ssize_t Capacity = 1024 << 10 >
constexpr auto create_view ()
{
        constexpr auto oversized = []
        {
                const auto result = Fn() ;
                using type = typename decltype( result )::value_type ;

                array< type, Capacity > data {} ;
                ::uti::copy( result.begin(), result.end(), data.begin() ) ;
                return std::pair{ data, result.size() } ;
        }() ;
        static constexpr auto right_sized = [ & ]
        {
                using type = typename decltype( oversized.first )::value_type ;

                array< type, oversized.second > arr {} ;

                ::uti::copy( oversized.first.begin(), oversized.first.begin() + oversized.second, arr.begin() ) ;

                return arr ;
        }() ;
        using type = typename decltype( right_sized )::value_type ;

        return view< type >( right_sized ) ;
}

template< auto Fn, ssize_t Capacity = 1024 << 10 >
constexpr auto create_string_view ()
        requires( meta::convertible_to< typename decltype( create_view< Fn, Capacity >() )::value_type ,
                                        typename           string_view                    ::value_type > )
{
        constexpr auto dataview = create_view< Fn, Capacity >() ;

        return string_view( dataview.begin(), dataview.size() ) ;
}


} // namespace uti::cexpr

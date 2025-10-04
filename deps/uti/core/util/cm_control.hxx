//
//
//      uti
//      core/util/cm_control.hxx
//

#pragma once


namespace uti
{


template< bool AllowCopy > struct copy_control ;

template<>
struct copy_control< true >
{
        constexpr copy_control () noexcept = default ;

        constexpr copy_control             ( copy_control const & ) noexcept = default ;
        constexpr copy_control & operator= ( copy_control const & ) noexcept = default ;

        constexpr copy_control             ( copy_control && ) noexcept = default ;
        constexpr copy_control & operator= ( copy_control && ) noexcept = default ;
} ;

template<>
struct copy_control< false >
{
        constexpr copy_control () noexcept = default ;

        constexpr copy_control             ( copy_control const & ) noexcept = delete ;
        constexpr copy_control & operator= ( copy_control const & ) noexcept = delete ;

        constexpr copy_control             ( copy_control && ) noexcept = default ;
        constexpr copy_control & operator= ( copy_control && ) noexcept = default ;
} ;

template< bool AllowMove > struct move_control ;

template<>
struct move_control< true >
{
        constexpr move_control () noexcept = default ;

        constexpr move_control             ( move_control const & ) noexcept = default ;
        constexpr move_control & operator= ( move_control const & ) noexcept = default ;

        constexpr move_control             ( move_control && ) noexcept = default ;
        constexpr move_control & operator= ( move_control && ) noexcept = default ;
} ;

template<>
struct move_control< false >
{
        constexpr move_control () noexcept = default ;

        constexpr move_control             ( move_control const & ) noexcept = default ;
        constexpr move_control & operator= ( move_control const & ) noexcept = default ;

        constexpr move_control             ( move_control && ) noexcept = delete ;
        constexpr move_control & operator= ( move_control && ) noexcept = delete ;
} ;


} // namespace uti

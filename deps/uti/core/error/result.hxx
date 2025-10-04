//
//
//      uti
//      core/error/result.hxx
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/container/variant.hxx>
#include <uti/core/string/string.hxx>


namespace uti
{


template< typename T, typename ErrorType = string >
class UTI_NODISCARD result
{
public:
        using result_type = T           ;
        using  value_type = result_type ;
        using  error_type = ErrorType   ;

        UTI_NODISCARD constexpr result ( value_type && _value_ ) noexcept( meta::nothrow_move_constructible< value_type > )
                : data_( UTI_MOVE( _value_ ) ) {}

        UTI_NODISCARD constexpr result ( error_type && _error_ ) noexcept( meta::nothrow_move_constructible< error_type > )
                : data_( UTI_MOVE( _error_ ) ) {}

        template< typename... Args >
                requires( meta::constructible_from< value_type, Args&&... > &&
                         !meta::constructible_from< error_type, Args&&... >  )
        UTI_NODISCARD constexpr result ( Args&&... _args_ ) noexcept( meta::nothrow_constructible_from< value_type, Args&&... > )
                : data_( value_type( UTI_FWD( _args_ )... ) ) {}

        template< typename... Args >
                requires( meta::constructible_from< error_type, Args&&... > &&
                         !meta::constructible_from< value_type, Args&&... >  )
        UTI_NODISCARD constexpr result ( Args&&... _args_ ) noexcept( meta::nothrow_constructible_from< error_type, Args&&... > )
                : data_( error_type( UTI_FWD( _args_ )... ) ) {}

        UTI_NODISCARD constexpr bool is_ok  () const noexcept { return data_.has_value( uti::variant_type< value_type >{} ) ; }
        UTI_NODISCARD constexpr bool is_err () const noexcept { return data_.has_value( uti::variant_type< error_type >{} ) ; }

        UTI_NODISCARD constexpr value_type       & value ()       noexcept { return data_.value( uti::variant_type< value_type >{} ) ; }
        UTI_NODISCARD constexpr value_type const & value () const noexcept { return data_.value( uti::variant_type< value_type >{} ) ; }

        UTI_NODISCARD constexpr error_type       & error ()       noexcept { return data_.value( uti::variant_type< error_type >{} ) ; }
        UTI_NODISCARD constexpr error_type const & error () const noexcept { return data_.value( uti::variant_type< error_type >{} ) ; }

        UTI_NODISCARD constexpr value_type value_or ( value_type const & _fallback_ ) const noexcept
        { return is_ok() ? data_.value( uti::variant_type< value_type >{} ) : _fallback_ ; }
private:
        variant< value_type, error_type > data_ ;
} ;


} // namespace uti

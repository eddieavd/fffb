//
//
//      uti
//      core/string/string_switch.hxx
//

#pragma once

#include <uti/core/string/string_view.hxx>


namespace uti
{


template< typename T, typename R = T >
class string_switch
{
public:
        using value_type = uti::decay_t< T > ;

        constexpr string_switch ( string_view const & _str_, value_type const & _default_ ) noexcept
                : str( _str_ ), result( UTI_MOVE( _default_ ) ), is_set( false ) {}

        constexpr string_switch ( string_view const & _str_, value_type && _default_ ) noexcept
                : str( _str_ ), result( UTI_MOVE( _default_ ) ), is_set( false ) {}

        constexpr string_switch             ( string_switch const &  ) = delete ;
        constexpr string_switch & operator= ( string_switch const &  ) = delete ;
        constexpr string_switch & operator= ( string_switch       && ) = delete ;

        constexpr string_switch ( string_switch && _other_ ) noexcept
                : str( UTI_MOVE( _other_.str ) ), result( UTI_MOVE( _other_.result ) ), is_set( _other_.is_set ) {}

        constexpr ~string_switch () noexcept = default ;

        constexpr string_switch & CASE ( string_view const & _s_, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.equal_to( _s_ ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & CASE ( string_view const & _s_, value_type && _value_ ) noexcept
        {
                if( !is_set && str.equal_to( _s_ ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & ENDS_WITH ( string_view const & _s_, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.ends_with( _s_ ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & ENDS_WITH ( string_view const & _s_, value_type && _value_ ) noexcept
        {
                if( !is_set && str.ends_with( _s_ ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & STARTS_WITH ( string_view const & _s_, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.starts_with( _s_ ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & STARTS_WITH ( string_view const & _s_, value_type && _value_ ) noexcept
        {
                if( !is_set && str.starts_with( _s_ ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_, value_type const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASE( _s1_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, value_type  const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_, value_type const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, value_type  const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_, value_type const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, value_type  const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_, value_type const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_,
                                          string_view const & _s8_, value_type  const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_,
                                          string_view const & _s8_, string_view const & _s9_, value_type const & _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _s9_, _value_ );
        }

        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASE( _s1_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_,
                                          string_view const & _s8_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _value_ );
        }
        constexpr string_switch & CASES ( string_view const & _s0_, string_view const & _s1_,
                                          string_view const & _s2_, string_view const & _s3_,
                                          string_view const & _s4_, string_view const & _s5_,
                                          string_view const & _s6_, string_view const & _s7_,
                                          string_view const & _s8_, string_view const & _s9_, value_type && _value_ ) noexcept
        {
                return CASE( _s0_, _value_ ).CASES( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _s9_, _value_ );
        }

        constexpr string_switch & CASE_LOWER ( string_view const & s, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.equal_to_insensitive( s ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & CASE_LOWER ( string_view const & s, value_type && _value_ ) noexcept
        {
                if( !is_set && str.equal_to_insensitive( s ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & ENDS_WITH_LOWER ( string_view const & s, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.ends_with_insensitive( s ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & ENDS_WITH_LOWER ( string_view const & s, value_type && _value_ ) noexcept
        {
                if( !is_set && str.ends_with_insensitive( s ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & STARTS_WITH_LOWER ( string_view const & s, value_type const & _value_ ) noexcept
        {
                if( !is_set && str.starts_with_insensitive( s ) )
                {
                        result = _value_ ;
                        is_set = true;
                }
                return *this;
        }
        constexpr string_switch & STARTS_WITH_LOWER ( string_view const & s, value_type && _value_ ) noexcept
        {
                if( !is_set && str.starts_with_insensitive( s ) )
                {
                        result = UTI_MOVE( _value_ );
                        is_set = true;
                }
                return *this;
        }

        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_, value_type const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASE_LOWER( _s1_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, value_type  const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_, value_type const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, value_type  const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_, value_type const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, value_type  const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_, value_type const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_,
                                                string_view const & _s8_, value_type  const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_,
                                                string_view const & _s8_, string_view const & _s9_, value_type const & _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _s9_, _value_ );
        }

        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASE_LOWER( _s1_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_,
                                                string_view const & _s8_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _value_ );
        }
        constexpr string_switch & CASES_LOWER ( string_view const & _s0_, string_view const & _s1_,
                                                string_view const & _s2_, string_view const & _s3_,
                                                string_view const & _s4_, string_view const & _s5_,
                                                string_view const & _s6_, string_view const & _s7_,
                                                string_view const & _s8_, string_view const & _s9_, value_type && _value_ ) noexcept
        {
                return CASE_LOWER( _s0_, _value_ ).CASES_LOWER( _s1_, _s2_, _s3_, _s4_, _s5_, _s6_, _s7_, _s8_, _s9_, _value_ );
        }

        [[ nodiscard ]] constexpr R DEFAULT ( value_type const & _value_ ) noexcept
        {
                if( is_set )
                {
                        return result ;
                }
                return _value_;
        }
        [[ nodiscard ]] constexpr R DEFAULT ( value_type && _value_ ) noexcept
        {
                if( is_set )
                {
                        return UTI_MOVE( result );
                }
                return _value_;
        }

        [[ nodiscard ]] constexpr operator R () noexcept
        {
                return UTI_MOVE( result );
        }
private:
        string_view   str ;
        value_type result ;
        bool       is_set ;
};


template< typename T, typename R = T >
string_switch ( string_view str, T result ) -> string_switch< uti::decay_t< T >, uti::decay_t< T > > ;


} // namespace uti

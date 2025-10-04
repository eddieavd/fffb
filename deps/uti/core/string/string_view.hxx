//
//
//      uti
//      core/string/string_view.hxx
//

#pragma once

#include <uti/core/util/assert.hxx>
#include <uti/core/type/traits.hxx>
#include <uti/core/container/meta.hxx>
#include <uti/core/algo/mem.hxx>
#include <uti/core/algo/distance.hxx>

#include <cmath>

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int) (sv).size(), (sv).data()


namespace uti
{


template< typename CharType >
struct generic_string_view ;

using string_view = generic_string_view< char const > ;


template< typename CharType >
struct generic_string_view
{
        using value_type = CharType ;
        using  size_type =   size_t ;
        using ssize_type =  ssize_t ;

        using         pointer = value_type       * ;
        using   const_pointer = value_type const * ;
        using       reference = value_type       & ;
        using const_reference = value_type const & ;

        using       iterator =       pointer ;
        using const_iterator = const_pointer ;

        constexpr generic_string_view (                                     ) noexcept ;
        constexpr generic_string_view ( pointer _c_str_                     ) noexcept ;
        constexpr generic_string_view ( pointer  _data_, ssize_type _count_ ) noexcept ;

        template< meta::simple_container Other >
                requires meta::convertible_to< typename Other::value_type, value_type >
        constexpr generic_string_view ( Other const & _other_ ) noexcept ;

        constexpr generic_string_view ( generic_string_view const &  _other_ ) noexcept = default ;
        constexpr generic_string_view ( generic_string_view       && _other_ ) noexcept           ;

        constexpr generic_string_view & operator= ( generic_string_view const &  _other_ ) noexcept = default ;
        constexpr generic_string_view & operator= ( generic_string_view       && _other_ ) noexcept           ;

        constexpr       reference operator[] ( ssize_type const _index_ )       noexcept { return data_[ _index_ ]; }
        constexpr const_reference operator[] ( ssize_type const _index_ ) const noexcept { return data_[ _index_ ]; }

        constexpr bool operator== ( generic_string_view const & _other_ ) const noexcept { return data_ == _other_.data_ && size_ == _other_.size_; }
        constexpr bool operator!= ( generic_string_view const & _other_ ) const noexcept { return !operator==( _other_ ); }

        constexpr ssize_type operator++ (     ) noexcept { --data_; return ++size_; } // extends view on  left side
        constexpr ssize_type operator++ ( int ) noexcept {          return ++size_; } // extends view on right side

        constexpr ssize_type operator-- (     ) noexcept { ++data_; return --size_; } // shrinks view on  left side
        constexpr ssize_type operator-- ( int ) noexcept {          return --size_; } // shrinks view on right side

        UTI_NODISCARD constexpr bool equal_to             ( generic_string_view const & _other_ ) const noexcept ;
        UTI_NODISCARD constexpr bool equal_to_insensitive ( generic_string_view const & _other_ ) const noexcept ;

        UTI_NODISCARD constexpr bool starts_with             ( generic_string_view const & _prefix_ ) const noexcept ;
        UTI_NODISCARD constexpr bool starts_with_insensitive ( generic_string_view const & _prefix_ ) const noexcept ;
        UTI_NODISCARD constexpr bool   ends_with             ( generic_string_view const & _suffix_ ) const noexcept ;
        UTI_NODISCARD constexpr bool   ends_with_insensitive ( generic_string_view const & _suffix_ ) const noexcept ;

        UTI_NODISCARD constexpr bool is_prefix_of             ( generic_string_view const & _other_ ) const noexcept ;
        UTI_NODISCARD constexpr bool is_prefix_of_insensitive ( generic_string_view const & _other_ ) const noexcept ;
        UTI_NODISCARD constexpr bool is_suffix_of             ( generic_string_view const & _other_ ) const noexcept ;
        UTI_NODISCARD constexpr bool is_suffix_of_insensitive ( generic_string_view const & _other_ ) const noexcept ;

        constexpr ssize_type trim       () noexcept ;
        constexpr ssize_type trim_left  () noexcept ;
        constexpr ssize_type trim_right () noexcept ;

        UTI_NODISCARD constexpr generic_string_view trimmed       () const noexcept ;
        UTI_NODISCARD constexpr generic_string_view trimmed_left  () const noexcept ;
        UTI_NODISCARD constexpr generic_string_view trimmed_right () const noexcept ;

        UTI_NODISCARD constexpr generic_string_view substr ( ssize_type const _start_, ssize_type const _len_ ) const noexcept ;

        constexpr generic_string_view chop_left  ( ssize_type _count_ ) noexcept ;
        constexpr generic_string_view chop_right ( ssize_type _count_ ) noexcept ;

        constexpr void unchop_left  ( ssize_type _count_ ) noexcept ;
        constexpr void unchop_right ( ssize_type _count_ ) noexcept ;

        constexpr value_type chop_char_left  () noexcept ;
        constexpr value_type chop_char_right () noexcept ;

        UTI_NODISCARD constexpr ssize_type index_of ( value_type          const & _val_, ssize_type const start ) const noexcept ;
        UTI_NODISCARD constexpr ssize_type index_of ( generic_string_view const & _val_, ssize_type const start ) const noexcept ;

        UTI_NODISCARD constexpr bool contains ( value_type const & _val_ ) const noexcept ;

        UTI_NODISCARD constexpr ssize_type count ( value_type const & _val_ ) const noexcept ;

        constexpr generic_string_view chop_to_delimiter ( value_type          const & _delimiter_, bool _discard_delimiter_ ) noexcept ;
        constexpr generic_string_view chop_to_string    ( generic_string_view const & _delimiter_, bool _discard_delimiter_ ) noexcept ;

        constexpr generic_string_view chop_to_first_of ( generic_string_view const & _delims_, bool const _discard_delimiter_ ) noexcept ;

        template< typename Predicate >
        constexpr generic_string_view chop_left_while ( Predicate _predicate_ ) noexcept( noexcept( _predicate_( value_type() ) ) ) ;

        template< typename Predicate >
        constexpr generic_string_view chop_right_while ( Predicate _predicate_ ) noexcept( noexcept( _predicate_( value_type() ) ) ) ;

        template< typename Integer >
        UTI_NODISCARD constexpr Integer parse_int () const noexcept ;

        template< typename Float >
        UTI_NODISCARD constexpr Float parse_float () const noexcept ;

        UTI_NODISCARD constexpr       reference at ( ssize_type const _index_ )       noexcept ;
        UTI_NODISCARD constexpr const_reference at ( ssize_type const _index_ ) const noexcept ;

        UTI_NODISCARD constexpr ssize_type size () const noexcept { return size_; }
        UTI_NODISCARD constexpr ssize_type  len () const noexcept { return size_; }

        UTI_NODISCARD constexpr       pointer data ()       noexcept { return data_; }
        UTI_NODISCARD constexpr const_pointer data () const noexcept { return data_; }

        UTI_NODISCARD constexpr bool empty () const noexcept { return size_ == 0; }

        UTI_NODISCARD constexpr       iterator  begin ()       noexcept { return   data_; }
        UTI_NODISCARD constexpr const_iterator  begin () const noexcept { return   data_; }
        UTI_NODISCARD constexpr const_iterator cbegin () const noexcept { return begin(); }

        UTI_NODISCARD constexpr       iterator  last ()       noexcept { return &data_[ size_ - 1 ]; }
        UTI_NODISCARD constexpr const_iterator  last () const noexcept { return &data_[ size_ - 1 ]; }
        UTI_NODISCARD constexpr const_iterator clast () const noexcept { return              last(); }

        UTI_NODISCARD constexpr       iterator  end ()       noexcept { return &data_[ size_ ]; }
        UTI_NODISCARD constexpr const_iterator  end () const noexcept { return &data_[ size_ ]; }
        UTI_NODISCARD constexpr const_iterator cend () const noexcept { return           end(); }

        UTI_NODISCARD constexpr       reference  front ()       noexcept { return *begin(); }
        UTI_NODISCARD constexpr const_reference  front () const noexcept { return *begin(); }
        UTI_NODISCARD constexpr const_reference cfront () const noexcept { return  front(); }

        UTI_NODISCARD constexpr       reference  back ()       noexcept { return *last(); }
        UTI_NODISCARD constexpr const_reference  back () const noexcept { return *last(); }
        UTI_NODISCARD constexpr const_reference cback () const noexcept { return  back(); }
private:
        pointer    data_ ;
        ssize_type size_ ;
};


template< typename CharType >
constexpr generic_string_view< CharType >::generic_string_view () noexcept
        : data_( nullptr ),
          size_(       0 )
{}

template< typename CharType >
constexpr generic_string_view< CharType >::generic_string_view ( pointer _c_str_ ) noexcept
        : data_(         _c_str_   ),
          size_( strlen( _c_str_ ) )
{}

template< typename CharType >
constexpr generic_string_view< CharType >::generic_string_view ( pointer _data_, ssize_type _count_ ) noexcept
        : data_(  _data_ ),
          size_( _count_ )
{}

template< typename CharType >
template< meta::simple_container Other >
        requires meta::convertible_to< typename Other::value_type, CharType >
constexpr generic_string_view< CharType >::generic_string_view ( Other const & _other_ ) noexcept
        : data_( _other_.begin() )
        , size_( ::uti::distance( _other_.begin(), _other_.end() ) )
{}

template< typename CharType >
constexpr generic_string_view< CharType >::generic_string_view ( generic_string_view && _other_ ) noexcept
        : data_( UTI_MOVE( _other_.data_ ) ),
          size_( UTI_MOVE( _other_.size_ ) )
{
        _other_.data_ = nullptr ;
        _other_.size_ =       0 ;
}

template< typename CharType >
constexpr generic_string_view< CharType > &
generic_string_view< CharType >::operator= ( generic_string_view && _other_ ) noexcept
{
        data_ = UTI_MOVE( _other_.data_ ) ;
        size_ = UTI_MOVE( _other_.size_ ) ;

        _other_.data_ = nullptr ;
        _other_.size_ =       0 ;

        return *this;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::equal_to ( generic_string_view const & _other_ ) const noexcept
{
        if( size() != _other_.size() )
        {
                return false;
        }
        if( !uti::is_constant_evaluated() && data() == _other_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < size(); ++i )
        {
                if( at( i ) != _other_.at( i ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::equal_to_insensitive ( generic_string_view const & _other_ ) const noexcept
{
        if( size() != _other_.size() )
        {
                return false;
        }
        if( !uti::is_constant_evaluated() && data() == _other_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < size(); ++i )
        {
                if( to_lower( at( i ) ) != to_lower( _other_.at( i ) ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::starts_with ( generic_string_view const & _prefix_ ) const noexcept
{
        if( _prefix_.len() > len() ) return false;

        if( !uti::is_constant_evaluated() && data() == _prefix_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < _prefix_.size(); ++i )
        {
                if( at( i ) != _prefix_.at( i ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::starts_with_insensitive ( generic_string_view const & _prefix_ ) const noexcept
{
        if( _prefix_.len() > len() ) return false;

        if( !uti::is_constant_evaluated() && data() == _prefix_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < _prefix_.size(); ++i )
        {
                if( to_lower( at( i ) ) != to_lower( _prefix_.at( i ) ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::ends_with ( generic_string_view const & _suffix_ ) const noexcept
{
        if( _suffix_.len() > len() ) return false;

        ssize_type pos = size() - _suffix_.size();

        if( !uti::is_constant_evaluated() && data() + pos == _suffix_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < _suffix_.size(); ++i )
        {
                if( at( pos + i ) != _suffix_.at( i ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::ends_with_insensitive ( generic_string_view const & _suffix_ ) const noexcept
{
        if( _suffix_.len() > len() ) return false;

        ssize_type pos = size() - _suffix_.size();

        if( !uti::is_constant_evaluated() && data() + pos == _suffix_.data() )
        {
                return true;
        }
        for( ssize_type i = 0; i < _suffix_.size(); ++i )
        {
                if( to_lower( at( pos + i ) ) != to_lower( _suffix_.at( i ) ) )
                {
                        return false;
                }
        }
        return true;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::is_prefix_of ( generic_string_view const & _other_ ) const noexcept
{
        return _other_.starts_with( *this ) ;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::is_prefix_of_insensitive ( generic_string_view const & _other_ ) const noexcept
{
        return _other_.starts_with_insensitive( *this ) ;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::is_suffix_of ( generic_string_view const & _other_ ) const noexcept
{
        return _other_.ends_with( *this ) ;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::is_suffix_of_insensitive ( generic_string_view const & _other_ ) const noexcept
{
        return _other_.ends_with_insensitive( *this ) ;
}

template< typename CharType >
constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::trim () noexcept
{
        ssize_type count { 0 } ;
        count += trim_left();
        count += trim_right();
        return count;
}

template< typename CharType >
constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::trim_left () noexcept
{
        ssize_type count { 0 } ;
        while( !empty() && ( front() ==  ' ' || front() == '\t' ) )
        {
                data_++;
                size_--;
                count++;
        }
        return count;
}

template< typename CharType >
constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::trim_right () noexcept
{
        ssize_type count { 0 } ;
        while( !empty() && ( back() ==  ' ' || back() == '\t' ) )
        {
                size_--;
                count++;
        }
        return count;
}

template< typename CharType >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::trimmed () const noexcept
{
        return trimmed_right().trimmed_left();
}

template< typename CharType >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::trimmed_left () const noexcept
{
        generic_string_view new_view( *this );

        new_view.trim_left();

        return new_view;
}

template< typename CharType >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::trimmed_right () const noexcept
{
        generic_string_view new_view( *this );

        new_view.trim_right();

        return new_view;
}

template< typename CharType >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::substr ( ssize_type const _start_, ssize_type const _len_ ) const noexcept
{
        generic_string_view sub( *this );

        ( void ) sub.chop_left( _start_ );

        if( _len_ < sub.len() )
        {
                sub.size_ = _len_;
        }
        return sub;
}

template< typename CharType >
constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_left ( ssize_type _count_ ) noexcept
{
        if( _count_ > size() ) _count_ = size();

        generic_string_view chop( data_, _count_ );

        data_ += _count_ ;
        size_ -= _count_ ;

        return chop;
}

template< typename CharType >
constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_right ( ssize_type _count_ ) noexcept
{
        if( _count_ > size() ) _count_ = size();

        generic_string_view chop( &data_[ size() - _count_ ], _count_ );

        size_ -= _count_ ;

        return chop;
}

template< typename CharType >
constexpr void
generic_string_view< CharType >::unchop_left ( ssize_type _count_ ) noexcept
{
        data_ -= _count_ ;
        size_ += _count_ ;
}

template< typename CharType >
constexpr void
generic_string_view< CharType >::unchop_right ( ssize_type _count_ ) noexcept
{
        size_ += _count_ ;
}

template< typename CharType >
constexpr typename generic_string_view< CharType >::value_type
generic_string_view< CharType >::chop_char_left () noexcept
{
        UTI_CEXPR_ASSERT( !empty(), "uti::string_view::chop_char_left: called on empty string_view" );

        size_--;

        return ( data_++ )[ 0 ];
}

template< typename CharType >
constexpr typename generic_string_view< CharType >::value_type
generic_string_view< CharType >::chop_char_right () noexcept
{
        UTI_CEXPR_ASSERT( !empty(), "uti::string_view::chop_char_right: called on empty string_view" );

        return data_[ --size_ ];
}

template< typename CharType >
UTI_NODISCARD constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::index_of ( value_type const & _val_, ssize_type const start ) const noexcept
{
        for( auto i = start; i < size(); ++i )
        {
                if( at( i ) == _val_ )
                {
                        return i;
                }
        }
        return -1;
}

template< typename CharType >
UTI_NODISCARD constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::index_of ( generic_string_view const & _val_, ssize_type const start ) const noexcept
{
        generic_string_view window( data_ + start, _val_.size() );

        bool found =  false ;
        ssize_type pos{ start } ;

        while( pos < size() - _val_.size() )
        {
                if( window.equal_to( _val_ ) )
                {
                        found = true;
                        break;
                }
                pos++;
                window.data_++;
        }
        if( found )
        {
                return pos;
        }
        return -1;
}

template< typename CharType >
UTI_NODISCARD constexpr bool
generic_string_view< CharType >::contains ( value_type const & _val_ ) const noexcept
{
        return index_of( _val_, 0 ) >= 0;
}

template< typename CharType >
UTI_NODISCARD constexpr typename generic_string_view< CharType >::ssize_type
generic_string_view< CharType >::count ( value_type const & _val_ ) const noexcept
{
        ssize_type count{ 0 } ;

        for( auto const & val : *this )
        {
                if( val == _val_ )
                {
                        ++count;
                }
        }
        return count;
}

template< typename CharType >
constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_to_delimiter ( value_type const & _delimiter_, bool _discard_delimiter_ ) noexcept
{
        generic_string_view chop( data_, 0 );

        while( !empty() && data_[ 0 ] != _delimiter_ )
        {
                data_++;
                size_--;
                chop.size_++;
        }
        if( !empty() && _discard_delimiter_ )
        {
                data_++;
                size_--;
        }
        return chop;
}

template< typename CharType >
constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_to_string ( generic_string_view const & _delimiter_, bool _discard_delimiter_ ) noexcept
{
        generic_string_view window( data_, _delimiter_.size() );

        bool found = false ;
        ssize_type pos( 0 );

        while( pos < size() - _delimiter_.size() )
        {
                if( window.equal_to( _delimiter_ ) )
                {
                        found = true;
                        break;
                }
                pos++;
                window.data_++;
        }
        if( found )
        {
                generic_string_view chop( data_, pos );
                data_ += pos;
                size_ -= pos;
                if( _discard_delimiter_ )
                {
                        data_ += _delimiter_.size();
                        size_ -= _delimiter_.size();
                }
                return chop;
        }
        generic_string_view chop( *this );
        this->data_ = nullptr ;
        this->size_ =       0 ;
        return chop;
}

template< typename CharType >
constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_to_first_of ( generic_string_view const & _delims_, bool const _discard_delimiter_ ) noexcept
{
        generic_string_view chop( data_, 0 );

        while( !empty() && !_delims_.contains( front() ) )
        {
                data_++;
                size_--;
                chop.size_++;
        }
        if( !empty() && _discard_delimiter_ )
        {
                data_++;
                size_--;
        }
        return chop;
}

template< typename CharType >
template< typename Predicate >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_left_while ( Predicate _predicate_ ) noexcept( noexcept( _predicate_( value_type() ) ) )
{
        generic_string_view chop( data_, 0 );

        while( !empty() && _predicate_( front() ) )
        {
                data_++;
                size_--;
                chop++ ;
        }
        return chop;
}

template< typename CharType >
template< typename Predicate >
UTI_NODISCARD constexpr generic_string_view< CharType >
generic_string_view< CharType >::chop_right_while ( Predicate _predicate_ ) noexcept( noexcept( _predicate_( value_type() ) ) )
{
        generic_string_view chop( end(), 0 );

        while( !empty() && _predicate_( back() ) )
        {
                size_--;
                ++chop ;
        }
        return chop;
}

template< typename CharType >
template< typename Integer >
UTI_NODISCARD constexpr Integer
generic_string_view< CharType >::parse_int () const noexcept
{
        auto digits = trimmed();

        if( digits.empty() ) return 0;

        int        sign { 1 } ;
        ssize_type  pos { 0 } ;

        if( digits.front() == '+' || digits.front() == '-' )
        {
                sign = ( digits.front() == '-' ) ? -1 : 1 ;
                ++pos;
        }
        Integer val { 0 } ;
        for( ; pos < digits.len(); ++pos )
        {
                CharType chr = digits.at( pos );
                if( chr < '0' || chr > '9' ) break;

                val = val * 10 + ( chr - '0' );
        }
        return sign * val;
}

template< typename CharType >
template< typename Float >
UTI_NODISCARD constexpr Float
generic_string_view< CharType >::parse_float () const noexcept
{
        Float        result {     } ;
        Float          sign { 1.0 } ;
        Float decimal_place { 1.0 } ;

        i32_t exponent       { 0 } ;
        i32_t exponent_sign  { 1 } ;
        i32_t exponent_found { 0 } ;

        auto src = trimmed() ;

        if( src.empty() ) return result ;

        if( src.front() == '+' || src.front() == '-' )
        {
                char sgn = src.chop_char_left() ;

                if( sgn == '-' ) sign = -1.0 ;
        }
        while( !src.empty() && ( ( '0' <= src.front() && src.front() <= '9' ) || src.front() == '.' ) )
        {
                char digit = src.chop_char_left() ;

                if( digit == '.' )
                {
                        while( !src.empty() && ( '0' <= src.front() && src.front() <= '9' ) )
                        {
                                result = result * 10.0 + ( src.front() - '0' ) ;
                                decimal_place *= 10.0 ;
                                src.chop_char_left() ;
                        }
                        break ;
                }
                else
                {
                        result = result * 10.0 + ( digit - '0' ) ;
                }
        }
        result /= decimal_place ;

        if( src.empty() ) return result ;

        char exp = src.chop_char_left() ;

        if( exp == 'e' || exp == 'E' )
        {
                if( src.front() == '+' || src.front() == '-' )
                {
                        char sgn = src.chop_char_left() ;

                        if( sgn == '-' )
                        {
                                exponent_sign = -1 ;
                        }
                        exponent_found = 1 ;
                }
                while( !src.empty() && ( '0' <= src.front() && src.front() <= '9' ) )
                {
                        exponent = exponent * 10 + ( src.front() - '0' ) ;
                        src.chop_char_left() ;
                }
                exponent *= exponent_sign ;
        }
        return sign * result * ( exponent_found ? pow( 10, exponent ) : 1.0 ) ;
}

template< typename CharType >
UTI_NODISCARD constexpr typename generic_string_view< CharType >::reference
generic_string_view< CharType >::at ( ssize_type const _index_ ) noexcept
{
        UTI_CEXPR_ASSERT( !empty() && 0 <= _index_ && _index_ < size(), "uti::string_view::at: index out of bounds" );

        return data_[ _index_ ];
}

template< typename CharType >
UTI_NODISCARD constexpr typename generic_string_view< CharType >::const_reference
generic_string_view< CharType >::at ( ssize_type const _index_ ) const noexcept
{
        UTI_CEXPR_ASSERT( !empty() && 0 <= _index_ && _index_ < size(), "uti::string_view::at: index out of bounds" );

        return data_[ _index_ ];
}


} // namespace uti

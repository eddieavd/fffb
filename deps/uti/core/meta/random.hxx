//
//
//      uti
//      core/meta/random.hxx
//
//      https://www.researchgate.net/publication/259005783_Random_number_generator_for_C_template_metaprograms
//

#pragma once

#include <uti/core/type/traits.hxx>
#include <uti/core/container/array.hxx>

#include <bit>


namespace uti::meta
{


constexpr  i8_t inits[] = __TIME__ ;
constexpr i32_t rs = ( inits[ 0 ] - '0' ) * 100000
                   + ( inits[ 1 ] - '0' ) *  10000
                   + ( inits[ 3 ] - '0' ) *   1000
                   + ( inits[ 4 ] - '0' ) *    100
                   + ( inits[ 6 ] - '0' ) *     10
                   + ( inits[ 7 ] - '0' ) *      1 ;

template< typename E > struct eval : type_identity< typename E::type > {} ;
template< typename E > struct init : type_identity<          E       > {} ;

template< typename Engine >
struct random
{
        using type = typename init< Engine >::type ;

        static constexpr decltype( type::value ) value { type::value } ;
} ;

template< typename R >
struct next
{
        using type = typename eval< R >::type ;

        static constexpr decltype( type::value ) value { type::value } ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// linear congruential engine

template< typename          UIntType ,
          UIntType Seed =         rs ,
          UIntType    A =      16807 ,
          UIntType    C =          0 ,
          UIntType    M = 2147483647
>
struct linear_congruential_engine
{
        static constexpr UIntType    value =  Seed ;
        static constexpr UIntType maxvalue = M - 1 ;
} ;

template< typename UIntType, UIntType Seed, UIntType A, UIntType C, UIntType M >
struct eval< linear_congruential_engine< UIntType, Seed, A, C, M > >
{
        static constexpr UIntType value { ( A * Seed + C ) % M } ;

        using type = linear_congruential_engine< UIntType, ( A * Seed + C ) & M, A, C, M > ;
} ;

template< typename UIntType, UIntType Seed, UIntType A, UIntType C, UIntType M >
struct init< linear_congruential_engine< UIntType, Seed, A, C, M > >
{
        using type = typename eval< linear_congruential_engine< UIntType, Seed, A, C, M > >::type ;

        static constexpr UIntType value = type::value ;
} ;

////////////////////////////////////////////////////////////////////////////////
/// basic
/// https://gist.github.com/KoneLinx/d3601597248bed423daf1a7cf7bd9533

constexpr u32_t hash ( u32_t in )
{
        constexpr u32_t r[]{
                0xdf15236c, 0x16d16793, 0x3a697614, 0xe0fe08e4,
                0xa3a53275, 0xccc10ff9, 0xb92fae55, 0xecf491de,
                0x36e86773, 0x0ed24a6a, 0xd7153d80, 0x84adf386,
                0x17110e76, 0x6d411a6a, 0xcbd41fed, 0x4b1d6b30
        };

        u32_t out{ in ^ r[ in & 0xF ] };

        out ^= std::rotl( in, 020 ) ^ r[ ( in >> 010 ) & 0xF ];
        out ^= std::rotl( in, 010 ) ^ r[ ( in >> 020 ) & 0xF ];
        out ^= std::rotr( in, 010 ) ^ r[ ( in >> 030 ) & 0xF ];
        return out;
}

template< ssize_t N >
constexpr u32_t hash( char const( &str )[ N ] )
{
        u32_t h {} ;
        for( u32_t i{}; i < N; ++i )
        {
                h ^= u32_t( str[ i ] ) << ( i % 4 * 8 ) ;
        }
        return hash( h );
}

template < ssize_t N >
constexpr u32_t basic_random ( char const ( &file )[ N ], u32_t line, u32_t column = 0x8dc97987 )
{
        return hash( hash( __TIME__ ) ^ hash( file ) ^ hash( line ) ^ hash( column ) );
}

template< ssize_t N >
constexpr float basic_rand_float ( char const ( &file )[ N ], u32_t line, u32_t col = 0x8DC97987 )
{
        return static_cast< float >( hash( hash( __TIME__ ) ^ hash( file ) ^ hash( line ) ^ hash( col ) ) ) / static_cast< float >( u32_t_max ) ;
}


} // namespace uti::meta

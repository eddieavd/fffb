//
//
//      uti
//      core/util/assert.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP

#include <uti/core/util/config.hxx>


#ifndef UTI_ASSERT
#       ifdef UTI_RELEASE
#               define UTI_ASSERT( cond, ... ) ( ( void ) 0 )
#       else
#               include <iostream>
#               define UTI_ASSERT( cond, ... )                              \
                do                                                           \
                {                                                             \
                        if( !( cond ) )                                        \
                        {                                                       \
                                std::cerr << "uti: '" << #cond << "' failed in " \
                                          << __FILE__ << ":" << __LINE__          \
                                          << " - " << __VA_ARGS__ << "\n";         \
                                std::abort();                                       \
                        }                                                            \
                } while( 0 )
        #endif
#endif

#ifndef UTI_CEXPR_ASSERT
#define UTI_CEXPR_ASSERT( cond, ... )                   \
        UTI_DIAGS_PUSH()                                 \
        UTI_DIAGS_DISABLE( -Wunused-but-set-variable )    \
        do                                                 \
        {                                                   \
                if constexpr( uti::is_constant_evaluated() ) \
                {                                             \
                        int test { 1 } ;                       \
                        test /= ( cond ) ;                      \
                }                                                \
                else                                              \
                {                                                  \
                        UTI_ASSERT( cond, __VA_ARGS__ );            \
                }                                                    \
        } while( 0 );                                                 \
        UTI_DIAGS_POP()
#endif

#endif // UTI_DOXYGEN_SKIP

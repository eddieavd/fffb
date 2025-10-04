//
//
//      uti
//      core/util/config.hxx
//

#pragma once

#ifndef UTI_DOXYGEN_SKIP


#define UTI_STRINGIZE( x ) #x


#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __NT__ )
#       define UTI_TARGET_OS "windows"
#       define UTI_TARGET_WINDOWS 1
#elif defined( __ANDROID__ )
#       define UTI_TARGET_OS "android"
#       define UTI_TARGET_ANDROID 1
#elif defined( __linux__ )
#       define UTI_TARGET_OS "linux"
#       define UTI_TARGET_LINUX 1
#elif defined( __APPLE__ )
#       include "TargetConditionals.h"
#       if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
#               define UTI_TARGET_OS "ios_sim"
#               define UTI_TARGET_IOS_SIM 1
#       elif TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
#               define UTI_TARGET_OS "mac_catalyst"
#               define UTI_TARGET_MAC_CATALYST 1
#       elif TARGET_OS_IPHONE
#               define UTI_TARGET_OS "ios"
#               define UTI_TARGET_IOS 1
#       elif TARGET_OS_MAC
#               define UTI_TARGET_OS "mac"
#               define UTI_TARGET_MAC 1
#       else
#               define UTI_TARGET_OS "apple_other"
#               define UTI_TARGET_APPLE_OTHER 1
#       endif
#endif


#if UTI_TARGET_ANDROID || UTI_TARGET_MAC || UTI_TARGET_IOS
#       define UTI_NOEXCEPT_UNLESS_BADALLOC noexcept
#else
#       define UTI_NOEXCEPT_UNLESS_BADALLOC
#endif

#if UTI_TARGET_ANDROID || UTI_TARGET_MAC || UTI_TARGET_IOS
#       define UTI_NOEXCEPT_UNLESS_BADALLOC_AND(...) noexcept( __VA_ARGS__ )
#else
#       define UTI_NOEXCEPT_UNLESS_BADALLOC_AND(...)
#endif


// shamelessly stolen from
// https://github.com/psiha/build/blob/master/include/psi/build/disable_warnings.hpp
#if defined( __GNUC__ ) || defined( __clang__ )

#       define UTI_DIAGS_COMMAND( compiler, command, ... ) \
        _Pragma( UTI_STRINGIZE( compiler diagnostic command __VA_ARGS__ ) )

#       define UTI_DIAGS_PUSH() UTI_DIAGS_COMMAND( UTI_COMPILER, push )
#       define UTI_DIAGS_POP()  UTI_DIAGS_COMMAND( UTI_COMPILER,  pop )

#       ifdef __clang__
#              define UTI_COMPILER clang
#              define UTI_DIAGS_CLANG_DISABLE( x ) UTI_DIAGS_COMMAND( clang, ignored, #x )
#              define UTI_DIAGS_GCC_DISABLE(   x )
#       else
#              define UTI_COMPILER GCC
#              define UTI_DIAGS_GCC_DISABLE(   x ) UTI_DIAGS_COMMAND(   gcc, ignored, #x )
#              define UTI_DIAGS_CLANG_DISABLE( x )
#       endif

#       define UTI_DIAGS_DISABLE( x ) UTI_DIAGS_COMMAND( UTI_COMPILER, ignored, #x )
#else
#       error "uti: unsupported compiler"
#endif


#ifndef UTI_STD_VER
#       if __cplusplus <= 201103L
#              define UTI_STD_VER 11
#       elif __cplusplus <= 201402L
#               define UTI_STD_VER 14
#       elif __cplusplus <= 201703L
#               define UTI_STD_VER 17
#       elif __cplusplus <= 202002L
#               define UTI_STD_VER 20
#       else
#               define UTI_STD_VER 23
#       endif
#endif


#if defined( __has_cpp_attribute )
#       define UTI_HAS_CPP_ATTRIBUTE( x ) __has_cpp_attribute( x )
#else
#       define UTI_HAS_CPP_ATTRIBUTE( x ) 0
#endif

#if defined( __has_attribute )
#       define UTI_HAS_ATTRIBUTE( x ) __has_attribute( x )
#else
#       define UTI_HAS_ATTRIBUTE( x ) 0
#endif

#if defined( __has_builtin )
#       define UTI_HAS_BUILTIN( x ) __has_builtin( x )
#else
#       define UTI_HAS_BUILTIN( x ) 0
#endif

#if defined( __has_constexpr_builtin )
#       define UTI_HAS_CEXPR_BUILTIN( x ) __has_constexpr_builtin( x )
#else
#       define UTI_HAS_CEXPR_BUILTIN( x ) 0
#endif

#if defined( __has_feature )
#       define UTI_HAS_FEATURE( x ) __has_feature( x )
#else
#       define UTI_HAS_FEATURE( x ) 0
#endif


#define UTI_NORETURN   [[   noreturn ]]
#define UTI_NODISCARD  [[  nodiscard ]]
#define UTI_DEPRECATED [[ deprecated ]]

#if UTI_HAS_BUILTIN( __builtin_unreachable )
#       define UTI_UNREACHABLE __builtin_unreachable()
#else
#       define UTI_UNREACHABLE
#endif

#if ( defined( __GNUC__ ) || defined( __clang__ ) ) && UTI_HAS_ATTRIBUTE( flatten )
#       define UTI_FLATTEN __attribute__(( flatten ))
#else
#       define UTI_FLATTEN
#endif

#if ( defined( __GNUC__ ) || defined( __clang__ ) ) && UTI_HAS_ATTRIBUTE( noescape )
#       define UTI_NOESCAPE __attribute__(( noescape ))
#else
#       define UTI_NOESCAPE
#endif

#if UTI_STD_VER >= 20
#       define UTI_LIKELY [[ likely ]]
#elif ( defined( __GNUC__ ) || defined( __clang__ ) ) && UTI_HAS_ATTRIBUTE( likely )
#       define UTI_LIKELY __attribute__(( likely ))
#else
#       define UTI_LIKELY
#endif

#if UTI_STD_VER >= 20
#       define UTI_UNLIKELY [[ unlikely ]]
#elif ( defined( __GNUC__ ) || defined( __clang__ ) ) && UTI_HAS_ATTRIBUTE( unlikely )
#       define UTI_UNLIKELY __attribute__(( unlikely ))
#else
#       define UTI_UNLIKELY
#endif

#if defined( __clang__ ) && UTI_HAS_ATTRIBUTE( noinline )
#       define UTI_NOINLINE [[ clang::noinline ]]
#elif defined( __GNUC__ ) && UTI_HAS_ATTRIBUTE( noinline )
#       define UTI_NOINLINE __attribute__(( noinline ))
#else
#       define UTI_NOINLINE
#endif

#if defined( __clang__ ) && UTI_HAS_ATTRIBUTE( always_inline )
#       define UTI_FORCEINLINE [[ clang::always_inline ]] inline
#elif defined( __GNUC__ ) && UTI_HAS_ATTRIBUTE( always_inline )
#       define UTI_FORCEINLINE inline __attribute__(( __always_inline__ ))
#else
#       define UTI_FORCEINLINE inline
#endif

#ifndef UTI_DEEP_INLINE
#define UTI_DEEP_INLINE UTI_FORCEINLINE UTI_FLATTEN
#endif

#if defined( __clang__ ) && UTI_HAS_ATTRIBUTE( optnone )
#       define UTI_NOOPT [[ clang::optnone ]]
#elif defined( __GNUC__ ) && UTI_HAS_ATTRIBUTE( optimize )
#       define UTI_NOOPT __attribute__(( optimize( 0 ) ))
#else
#       define UTI_NOOPT
#endif


#if ( defined( __GNUC__ ) || defined( __clang__ ) ) && UTI_HAS_ATTRIBUTE( __no_stack_protector__ )
#       define UTI_NO_STACK_PROTECTOR __attribute__(( __no_stack_protector__ ))
#else
#       define UTI_NO_STACK_PROTECTOR
#endif


#if defined( __clang__ ) && UTI_HAS_ATTRIBUTE( trivial_abi )
#       define UTI_TRIVIAL_ABI [[ clang::trivial_abi ]]
#else
#       define UTI_TRIVIAL_ABI
#endif

#if defined( __clang__ ) && UTI_HAS_BUILTIN( __builtin_is_cpp_trivially_relocatable )
#       define UTI_IS_TRIVIALLY_RELOCATABLE( x ) __builtin_is_cpp_trivially_relocatable( x )
#elif defined( __clang__ ) && UTI_HAS_BUILTIN( __is_trivially_relocatable )
#       define UTI_IS_TRIVIALLY_RELOCATABLE( x ) __is_trivially_relocatable( x )
#else
#       define UTI_IS_TRIVIALLY_RELOCATABLE( x ) false
#endif


#ifdef UTI_FALSE_IF_MISSING
#        define UTI_DEFAULT_FOR_MISSING : false_type {}
#elif defined( UTI_TRUE_IF_MISSING )
#        define UTI_DEFAULT_FOR_MISSING : true_type {}
#endif

#if UTI_HAS_BUILTIN( __is_final                   ) && \
    UTI_HAS_BUILTIN( __is_empty                   ) && \
    UTI_HAS_BUILTIN( __is_enum                    ) && \
    UTI_HAS_BUILTIN( __is_union                   ) && \
    UTI_HAS_BUILTIN( __is_trivially_constructible ) && \
    UTI_HAS_BUILTIN( __is_trivially_assignable    ) && \
  ( UTI_HAS_BUILTIN( __is_trivially_destructible  ) || \
    UTI_HAS_BUILTIN( __has_trivial_destructor     )    \
  )
//      use compiler intrinsics

#elif defined( UTI_HAS_STL )
//      use standard library implementations
#       include <type_traits>

#elif defined( UTI_TRUE_IF_MISSING )
//      default missing intrinsics to true
#       warning uti: missing intrinsics detected. Some or all of 'is_final', 'is_empty', 'is_enum', 'is_union', and 'is_trivially_(constructible, assignable, destructible)' will default to true

#elif defined( UTI_FALSE_IF_MISSING )
//      default missing intrinsics to false
#       warning uti: missing intrinsics detected. Some or all of 'is_final', 'is_empty', 'is_enum', 'is_union', and 'is_trivially_(constructible, assignable, destructible)' will default to false

#else
#       warning uti: no implementations available for 'is_final', 'is_empty', 'is_enum', 'is_union', and/or 'is_trivially_(constructible, assignable, destructible)'. \
Defaults can be provided by defining 'UTI_TRUE_IF_MISSING' or 'UTI_FALSE_IF_MISSING'.
#endif

#ifndef UTI_DEFAULT_FOR_MISSING
#define UTI_DEFAULT_FOR_MISSING
#endif

#endif // UTI_DOXYGEN_SKIP

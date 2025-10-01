//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
///
/// This header does not belong to a module of \alib, but is
/// included in any \alibbuild.
/// Its purposes are:
/// - To identify given \ref GrpALibPreproSymbols  related to module selection and enable
///   corresponding symbols (that may be not given) of mandatory dependent modules.
/// - Detect and assert C++ language version.
/// - Define preprocessor utilities as needed throughout \alib.
//==================================================================================================
#ifndef INL_ALIB
#define INL_ALIB 1
#pragma once

#define ALIB_VERSION    2510
#define ALIB_REVISION   0

#ifndef DOXYGEN
#   define DOXYGEN 0
#endif

#include <concepts>
#include <type_traits>


#if !DOXYGEN

// #################################################################################################
// Single-threaded library?
// #################################################################################################
#if !defined( ALIB_SINGLE_THREADED   )
#   define  ALIB_SINGLE_THREADED   0
#endif
#if ALIB_SINGLE_THREADED
#   define  IF_ALIB_SINGLE_THREADED(...)         __VA_ARGS__
#   define  IFNOT_ALIB_SINGLE_THREADED(...)
#else
#   define  IF_ALIB_SINGLE_THREADED(...)
#   define  IFNOT_ALIB_SINGLE_THREADED(...)      __VA_ARGS__
#endif

// #################################################################################################
// No module selection symbol given but threads? -> choose ALL
// #################################################################################################
#if     !defined( ALIB_ALOX          )       \
     && !defined( ALIB_BITBUFFER     )       \
     && !defined( ALIB_BOXING        )       \
     && !defined( ALIB_CAMP          )       \
     && !defined( ALIB_CLI           )       \
     && !defined( ALIB_VARIABLES     )       \
     && !defined( ALIB_CONTAINERS    )       \
     && !defined( ALIB_ENUMRECORDS   )       \
     && !defined( ALIB_EXCEPTIONS    )       \
     && !defined( ALIB_EXPRESSIONS   )       \
     && !defined( ALIB_FILES         )       \
     && !defined( ALIB_FORMAT        )       \
     && !defined( ALIB_MONOMEM       )       \
     && !defined( ALIB_RESOURCES     )       \
     && !defined( ALIB_SINGLETONS    )       \
     && !defined( ALIB_STRINGS       )       \
     && !defined( ALIB_SYSTEM        )       \
     && !defined( ALIB_THREADMODEL   )
#   define  ALIB_ALOX                1
#   define  ALIB_BITBUFFER           1
#   define  ALIB_CLI                 1
#   define  ALIB_EXPRESSIONS         1
#   define  ALIB_FILES               1
#   if !ALIB_SINGLE_THREADED
#       define  ALIB_THREADMODEL     1
#   endif
#elif defined(ALIB_THREADMODEL ) &&  ALIB_SINGLE_THREADED
#   error "ALib Module THREADMODEL requested, while symbol ALIB_SINGLE_THREADED is set"
#endif

#if !defined(ALIB_ALOX         )
#   define ALIB_ALOX                0
#endif
#if !defined(ALIB_BITBUFFER    )
#   define ALIB_BITBUFFER           0
#endif
#if !defined(ALIB_BOXING       )
#   define ALIB_BOXING              0
#endif
#if !defined(ALIB_CLI          )
#   define ALIB_CLI                 0
#endif
#if !defined(ALIB_FORMAT       )
#   define ALIB_FORMAT              0
#endif
#if !defined(ALIB_EXCEPTIONS   )
#   define ALIB_EXCEPTIONS          0
#endif
#if !defined(ALIB_SYSTEM       )
#   define ALIB_SYSTEM              0
#endif
#if !defined(ALIB_RESOURCES    )
#   define ALIB_RESOURCES           0
#endif
#if !defined(ALIB_CONTAINERS   )
#   define ALIB_CONTAINERS          0
#endif
#if !defined(ALIB_VARIABLES)
#   define ALIB_VARIABLES           0
#endif
#if !defined(ALIB_ENUMRECORDS  )
#   define ALIB_ENUMRECORDS         0
#endif
#if !defined(ALIB_EXPRESSIONS  )
#   define ALIB_EXPRESSIONS         0
#endif
#if !defined(ALIB_FILES        )
#   define ALIB_FILES               0
#endif
#if !defined(ALIB_MONOMEM      )
#   define ALIB_MONOMEM             0
#endif
#if !defined(ALIB_SINGLETONS   )
#   define ALIB_SINGLETONS          0
#endif
#if !defined(ALIB_STRINGS      )
#   define ALIB_STRINGS             0
#endif
#if !defined(ALIB_CAMP         )
#   define ALIB_CAMP                0
#endif
#if !defined( ALIB_THREADMODEL )
#   define  ALIB_THREADMODEL        0
#endif

// #################################################################################################
// Resolve module dependencies
// #################################################################################################
#if ALIB_EXPRESSIONS
#    undef  ALIB_CAMP
#    define ALIB_CAMP               1
#endif
#if ALIB_CLI
#    undef  ALIB_CAMP
#    define ALIB_CAMP               1
#endif
#if ALIB_FILES
#    undef  ALIB_CAMP
#    define ALIB_CAMP               1
#endif
#if ALIB_ALOX
#    undef  ALIB_CAMP
#    define ALIB_CAMP               1
#endif
#if ALIB_CAMP
#    undef  ALIB_FORMAT
#    define ALIB_FORMAT             1
#    undef  ALIB_VARIABLES
#    define ALIB_VARIABLES          1
#endif
#if ALIB_VARIABLES
#    undef  ALIB_CONTAINERS
#    define ALIB_CONTAINERS         1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM            1
#    undef  ALIB_SYSTEM
#    define ALIB_SYSTEM             1
#endif
#if ALIB_FORMAT
#    undef  ALIB_EXCEPTIONS
#    define ALIB_EXCEPTIONS         1
#endif
#if ALIB_EXCEPTIONS
#    undef  ALIB_BOXING
#    define ALIB_BOXING             1
#    undef  ALIB_ENUMRECORDS
#    define ALIB_ENUMRECORDS        1
#    undef  ALIB_RESOURCES
#    define ALIB_RESOURCES          1
#endif
#if ALIB_THREADMODEL
#    undef  ALIB_BOXING
#    define ALIB_BOXING             1
#    undef  ALIB_CONTAINERS
#    define ALIB_CONTAINERS         1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM            1
#endif
#if ALIB_SYSTEM
#    undef  ALIB_BOXING
#    define ALIB_BOXING             1
#    undef  ALIB_ENUMRECORDS
#    define ALIB_ENUMRECORDS        1
#endif
#if ALIB_BITBUFFER
#    undef  ALIB_CONTAINERS
#    define ALIB_CONTAINERS         1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM            1
#endif
#if ALIB_RESOURCES
#    undef  ALIB_CONTAINERS
#    define ALIB_CONTAINERS         1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM            1
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS            1
#endif
#if ALIB_ENUMRECORDS
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS         1
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS            1
#endif
#if ALIB_BOXING
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS         1
#endif

// #################################################################################################
// if !DOXYGEN
// #################################################################################################
#else
#define ALIB_ALOX                     1
#define ALIB_BITBUFFER                1
#define ALIB_BOXING                   1
#define ALIB_CAMP                     1
#define ALIB_FORMAT                   1
#define ALIB_EXCEPTIONS               1
#define ALIB_SYSTEM                   1
#define ALIB_RESOURCES                1
#define ALIB_CLI                      1
#define ALIB_VARIABLES                1
#define ALIB_CONTAINERS               1
#define ALIB_ENUMRECORDS              1
#define ALIB_EXPRESSIONS              1
#define ALIB_FILES                    1
#define ALIB_MONOMEM                  1
#define ALIB_SINGLETONS               1
#define ALIB_STRINGS                  1
#define ALIB_THREADMODEL              1
#endif //!DOXYGEN

// #################################################################################################
// Macros for checking availability of modules
// #################################################################################################
#define  ALIB_DOCUMENTATION_URL   "https://alib.dev/"

#define ALIB_ASSERT_MODULE(modulename)                                                             \
static_assert( ALIB_ ## modulename,                                                                \
               "This module is not included in the ALib Build. "                                   \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \

// #################################################################################################
// Macros to select code (without using #if/#endif)
// #################################################################################################
#if ALIB_ALOX
#   define  IF_ALIB_ALOX(...)              __VA_ARGS__
#   define  IFNOT_ALIB_ALOX(...)
#else
#   define  IF_ALIB_ALOX(...)
#   define  IFNOT_ALIB_ALOX(...)           __VA_ARGS__
#endif

#if ALIB_BITBUFFER
#   define  IF_ALIB_BITBUFFER(...)         __VA_ARGS__
#   define  IFNOT_ALIB_BITBUFFER(...)
#else
#   define  IF_ALIB_BITBUFFER(...)
#   define  IFNOT_ALIB_BITBUFFER(...)      __VA_ARGS__
#endif

#if ALIB_BOXING
#   define  IF_ALIB_BOXING(...)            __VA_ARGS__
#   define  IFNOT_ALIB_BOXING(...)
#else
#   define  IF_ALIB_BOXING(...)
#   define  IFNOT_ALIB_BOXING(...)         __VA_ARGS__
#endif

#if ALIB_FORMAT
#   define  IF_ALIB_FORMAT(...)            __VA_ARGS__
#   define  IFNOT_ALIB_FORMAT(...)
#else
#   define  IF_ALIB_FORMAT(...)
#   define  IFNOT_ALIB_FORMAT(...)         __VA_ARGS__
#endif

#if ALIB_EXCEPTIONS
#   define  IF_ALIB_EXCEPTIONS(...)        __VA_ARGS__
#   define  IFNOT_ALIB_EXCEPTIONS(...)
#else
#   define  IF_ALIB_EXCEPTIONS(...)
#   define  IFNOT_ALIB_EXCEPTIONS(...)     __VA_ARGS__
#endif

#if ALIB_SYSTEM
#   define  IF_ALIB_SYSTEM(...)            __VA_ARGS__
#   define  IFNOT_ALIB_SYSTEM(...)
#else
#   define  IF_ALIB_SYSTEM(...)
#   define  IFNOT_ALIB_SYSTEM(...)         __VA_ARGS__
#endif

#if ALIB_RESOURCES
#   define  IF_ALIB_RESOURCES(...)         __VA_ARGS__
#   define  IFNOT_ALIB_RESOURCES(...)
#else
#   define  IF_ALIB_RESOURCES(...)
#   define  IFNOT_ALIB_RESOURCES(...)      __VA_ARGS__
#endif

#if ALIB_CLI
#   define  IF_ALIB_CLI(...)               __VA_ARGS__
#   define  IFNOT_ALIB_CLI(...)
#else
#   define  IF_ALIB_CLI(...)
#   define  IFNOT_ALIB_CLI(...)            __VA_ARGS__
#endif

#if ALIB_VARIABLES
#   define  IF_ALIB_VARIABLES(...)         __VA_ARGS__
#   define  IFNOT_ALIB_VARIABLES(...)
#else
#   define  IF_ALIB_VARIABLES(...)
#   define  IFNOT_ALIB_VARIABLES(...)      __VA_ARGS__
#endif

#if ALIB_CONTAINERS
#   define  IF_ALIB_CONTAINERS(...)        __VA_ARGS__
#   define  IFNOT_ALIB_CONTAINERS(...)
#else
#   define  IF_ALIB_CONTAINERS(...)
#   define  IFNOT_ALIB_CONTAINERS(...)     __VA_ARGS__
#endif

#if ALIB_ENUMRECORDS
#   define  IF_ALIB_ENUMRECORDS(...)       __VA_ARGS__
#   define  IFNOT_ALIB_ENUMRECORDS(...)
#else
#   define  IF_ALIB_ENUMRECORDS(...)
#   define  IFNOT_ALIB_ENUMRECORDS(...)    __VA_ARGS__
#endif

#if ALIB_EXPRESSIONS
#   define  IF_ALIB_EXPRESSIONS(...)       __VA_ARGS__
#   define  IFNOT_ALIB_EXPRESSIONS(...)
#else
#   define  IF_ALIB_EXPRESSIONS(...)
#   define  IFNOT_ALIB_EXPRESSIONS(...)    __VA_ARGS__
#endif

#if ALIB_FILES
#   define  IF_ALIB_FILES(...)             __VA_ARGS__
#   define  IFNOT_ALIB_FILES(...)
#else
#   define  IF_ALIB_FILES(...)
#   define  IFNOT_ALIB_FILES(...)          __VA_ARGS__
#endif

#if ALIB_MONOMEM
#   define  IF_ALIB_MONOMEM(...)           __VA_ARGS__
#   define  IFNOT_ALIB_MONOMEM(...)
#else
#   define  IF_ALIB_MONOMEM(...)
#   define  IFNOT_ALIB_MONOMEM(...)        __VA_ARGS__
#endif

#if ALIB_SINGLETONS
#   define  IF_ALIB_SINGLETONS(...)        __VA_ARGS__
#   define  IFNOT_ALIB_SINGLETONS(...)
#else
#   define  IF_ALIB_SINGLETONS(...)
#   define  IFNOT_ALIB_SINGLETONS(...)     __VA_ARGS__
#endif

#if ALIB_STRINGS
#   define  IF_ALIB_STRINGS(...)           __VA_ARGS__
#   define  IFNOT_ALIB_STRINGS(...)
#else
#   define  IF_ALIB_STRINGS(...)
#   define  IFNOT_ALIB_STRINGS(...)        __VA_ARGS__
#endif

#if ALIB_CAMP
#   define  IF_ALIB_CAMP(...)              __VA_ARGS__
#   define  IFNOT_ALIB_CAMP(...)
#else
#   define  IF_ALIB_CAMP(...)
#   define  IFNOT_ALIB_CAMP(...)           __VA_ARGS__
#endif

#if ALIB_THREADMODEL
#   define  IF_ALIB_THREADMODEL(...)       __VA_ARGS__
#   define  IFNOT_ALIB_THREADMODEL(...)
#else
#   define  IF_ALIB_THREADMODEL(...)
#   define  IFNOT_ALIB_THREADMODEL(...)    __VA_ARGS__
#endif

#if !ALIB_SINGLE_THREADED
#   define  IF_ALIB_THREADS(...)           __VA_ARGS__
#   define  IFNOT_ALIB_THREADS(...)
#else
#   define  IF_ALIB_THREADS(...)
#   define  IFNOT_ALIB_THREADS(...)        __VA_ARGS__
#endif

// #################################################################################################
// Compiler detection and specifics
// #################################################################################################
#if !DOXYGEN
    #define  DOX_MARKER(marker)
#endif

// GCC Compiler detection
#if   defined(__clang__)
#elif defined(__INTEL_COMPILER) //never tested
#elif defined(_MSC_VER)
#elif defined(__GNUC__)
#   define ALIB_GCC 1
#endif



// --- C++ standard: set ALIB_CPP_STANDARD ---

// deduce && disallow < 20
#if defined ( _MSC_VER )
#   if defined(_MSVC_LANG)
#      if _MSVC_LANG <= 201703L
#        error "ALib needs C++ standard 20. Wrong compilation settings given."
#      elif _MSVC_LANG == 202002L
#        define  ALIB_INTERNAL_DEDUCED_CPPVER  20
#      elif _MSVC_LANG >  202002L
#        define  ALIB_INTERNAL_DEDUCED_CPPVER  23
#      endif
#   endif


#elif defined(__cplusplus)
#   if   __cplusplus < 202002L
#      error "ALib needs C++20. Compilation aborted"
#   else
#     define  ALIB_INTERNAL_DEDUCED_CPPVER  20
#   endif
#   if __cplusplus > 202002L
#     undef  ALIB_INTERNAL_DEDUCED_CPPVER
#     define  ALIB_INTERNAL_DEDUCED_CPPVER  23
#   endif
#endif

// check whether detected, given or given falsely
#if !defined(ALIB_CPP_STANDARD)
#   if defined(ALIB_INTERNAL_DEDUCED_CPPVER)
#       define ALIB_CPP_STANDARD ALIB_INTERNAL_DEDUCED_CPPVER
#   else
#       error "Unknown compiler/toolchain. Can't deduce ALIB_CPP_STANDARD. Please provide this symbol 'manually' to the compiler."
#   endif
#else
#   if defined(ALIB_INTERNAL_DEDUCED_CPPVER)
#       if (ALIB_CPP_STANDARD != ALIB_INTERNAL_DEDUCED_CPPVER)
#           error "ALIB_CPP_STANDARD was explicitly passed to the compiler, but does not match the language standard deduced by ALib_"
#       endif
#   elif ALIB_CPP_STANDARD < 20
#       error "ALIB_CPP_STANDARD must be set to at least 20."
#   endif
#endif

// final internal check
#if ALIB_CPP_STANDARD < 20
#    error "Error in header. This must never happen."
#endif

// Inline code selection dependent on C++ Version
#if ALIB_CPP_STANDARD >= 23
#   define ALIB_CPP_23(...)        __VA_ARGS__
#   define ALIB_CPP_BEFORE_23(...)
#else
#   define ALIB_CPP_23(...)
#   define ALIB_CPP_BEFORE_23(...) __VA_ARGS__
#endif

// Check if modules are enabled and define C++-module related macros
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
#   define ALIB_EXPORT  export
#else
#   define ALIB_EXPORT
#endif

// Windows DLL Import/Export
#if defined( _MSC_VER ) && !defined( ALIB_API_NO_DLL )
    #ifdef ALIB_API_IS_DLL
        #define ALIB_DLL  __declspec(dllexport)
    #else
        #define ALIB_DLL  __declspec(dllimport)
    #endif
#else
    #define ALIB_DLL
#endif


// Warnings
#if defined(ALIB_GCC)

    #define ALIB_WARNINGS_UNINITIALIZED_OFF                                     \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")             \

    #define ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING                            \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wnonnull\"")                         \

    #define ALIB_WARNINGS_ALLOW_UNREACHABLE_CODE                                \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_ALLOW_MACRO_REDEFINITION                              \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE                     \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_MACRO_NOT_USED_OFF                                    \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_RESERVED_MACRO_NAME_OFF                               \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_OVERLOAD_VIRTUAL_OFF                                  \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH                              \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wswitch\"")                          \

    #define ALIB_WARNINGS_ALLOW_BITWISE_SWITCH                                  \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wswitch\"")                          \

    #define ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW                            \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wshift-count-overflow\"")            \

    #define ALIB_WARNINGS_IGNORE_DEPRECATED                                     \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")         \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-macros\"")                   \

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                \

    #define ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE                                \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")                 \

    #define ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION                                \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-function\"")                 \

    #define ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE                          \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE                              \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_IGNORE_NOTHING_RETURNED                               \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wreturn-type\"")                     \

    #define ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW                     \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER                            \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_IGNORE_DOCS                                           \
        _Pragma("GCC diagnostic push")                                          \

    #define ALIB_WARNINGS_IGNORE_SIGN_CONVERSION                                \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")                 \

    #define ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW                               \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wshift-count-overflow\"")            \

    #define ALIB_WARNINGS_RESTORE                                               \
        _Pragma("GCC diagnostic pop")

    #if __GNUC__ < 7
      #define ALIB_FALLTHROUGH
    #else
      #define ALIB_FALLTHROUGH        [[gnu::fallthrough]];
    #endif

#elif defined(__clang__)

    #define ALIB_WARNINGS_UNINITIALIZED_OFF                                     \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wuninitialized\"")                 \
        _Pragma("clang diagnostic ignored \"-Wconditional-uninitialized\"")     \

    #define ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING                            \
        _Pragma("clang diagnostic push")                                        \

    #define ALIB_WARNINGS_ALLOW_UNREACHABLE_CODE                                \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunreachable-code\"")              \

    #define ALIB_WARNINGS_OVERLOAD_VIRTUAL_OFF                                  \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Woverloaded-virtual\"")            \

    #define ALIB_WARNINGS_ALLOW_MACRO_REDEFINITION                              \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wmacro-redefined\"")               \

    #define ALIB_WARNINGS_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE                     \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-macros\"")                 \


    #define ALIB_WARNINGS_MACRO_NOT_USED_OFF                                    \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-macros\"")                 \

    #define ALIB_WARNINGS_RESERVED_MACRO_NAME_OFF                               \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wreserved-macro-identifier\"")     \

    #define ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH                              \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wswitch\"")                        \
        _Pragma("clang diagnostic ignored \"-Wswitch-enum\"")                   \

    #define ALIB_WARNINGS_ALLOW_BITWISE_SWITCH                                  \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wswitch\"")                        \
        _Pragma("clang diagnostic ignored \"-Wcovered-switch-default\"")        \

    #define ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW                            \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wshift-count-overflow\"")          \

    #define ALIB_WARNINGS_IGNORE_DEPRECATED                                     \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")       \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-macros\"")                 \

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")              \

    #define ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE                                \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-variable\"")               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION                                \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-function\"")               \
        _Pragma("clang diagnostic ignored \"-Wunused-member-function\"")        \
        _Pragma("clang diagnostic ignored \"-Wunused-template\"")               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE                          \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-lambda-capture\"")         \

    #define ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE                              \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-template\"")               \

    #define ALIB_WARNINGS_IGNORE_NOTHING_RETURNED                               \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wreturn-type\"")                   \

    #define ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW                     \
        _Pragma("clang diagnostic push")                                        \

    #define ALIB_WARNINGS_IGNORE_DOCS                                           \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wdocumentation\"")                 \

    #define ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER                            \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wreserved-identifier\"")           \

    #define ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW                               \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Winteger-overflow\"")              \

    #define ALIB_WARNINGS_IGNORE_SIGN_CONVERSION                                \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")               \


    #define ALIB_WARNINGS_RESTORE                                               \
        _Pragma("clang diagnostic pop")                                         \

    #define ALIB_FALLTHROUGH            [[clang::fallthrough]];


#elif defined(_MSC_VER)
    #define ALIB_WARNINGS_UNINITIALIZED_OFF                                     \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4701 ))                                     \

    #define ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING                            \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_UNREACHABLE_CODE                                \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_MACRO_NOT_USED_OFF                                    \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_RESERVED_MACRO_NAME_OFF                               \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_OVERLOAD_VIRTUAL_OFF                                  \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH                              \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH                              \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_MACRO_REDEFINITION                              \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4005 ))                                     \

    #define ALIB_WARNINGS_ALLOW_UNSAFE_FUNCTION_OR_VARIABLE                     \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4996 ))                                     \

    #define ALIB_WARNINGS_ALLOW_BITWISE_SWITCH                                  \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_SHIFT_COUNT_OVERFLOW                            \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_DEPRECATED                                     \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_VARIABLE                                \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION                                \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE                          \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_FUNCTION_TEMPLATE                              \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_NOTHING_RETURNED                               \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4715 ))                                     \

    #define ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW                     \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4305 ))                                     \
        __pragma(warning( disable : 4307 ))                                     \
        __pragma(warning( disable : 4309 ))                                     \
        __pragma(warning( disable : 4310 ))                                     \
        __pragma(warning( disable : 4293 ))                                     \

    #define ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW                               \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4293 ))                                     \

    #define ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER                            \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_DOCS                                           \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_SIGN_CONVERSION                                \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_DOCS                                           \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_RESTORE                                               \
        __pragma(warning( pop  ))                                               \

    #define ALIB_FALLTHROUGH

#else
    #define ALIB_WARNINGS_RESTORE
    #define ALIB_FALLTHROUGH
#endif

// #################################################################################################
// Availability of external libraries
// #################################################################################################
#if !defined(ALIB_EXT_LIB_THREADS_AVAILABLE)
#	define ALIB_EXT_LIB_THREADS_AVAILABLE   1
#endif
#if ALIB_EXT_LIB_THREADS_AVAILABLE
#   include <thread>
#endif

#if !defined(ALIB_FEAT_BOOST_REGEX)
#   define   ALIB_FEAT_BOOST_REGEX          0
#endif

// #################################################################################################
// Debug or release compilation
// #################################################################################################
#if !defined(ALIB_DEBUG)
#   if  !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)
#       define ALIB_DEBUG 1
#   else
#       define ALIB_DEBUG 0
#   endif
#endif

#if ALIB_DEBUG
    #define ALIB_DBG(...)    __VA_ARGS__
    #define ALIB_REL(...)
    #define ALIB_REL_DBG(releaseCode, ...)    __VA_ARGS__
#else
    #define ALIB_DBG(...)
    #define ALIB_REL(...)    __VA_ARGS__
    #define ALIB_REL_DBG(releaseCode, ...)    releaseCode
#endif



// #################################################################################################
// ALib Feature detection
// (Note: this has to be done outside the module code, because the features are used with
//        the compilation verification flags below)
// #################################################################################################

// ALIB_CHARACTERS_WIDE, ALIB_SIZEOF_WCHAR_T
#if defined(__WCHAR_MAX__)
    #if    __WCHAR_MAX__ == 0x7FFFFFFF     \
        || __WCHAR_MAX__ == 0xFFFFFFFF
        #define     ALIB_SIZEOF_WCHAR_T    4
    #else
        #define     ALIB_SIZEOF_WCHAR_T    2
    #endif
#elif defined(_MSC_VER)
    #define         ALIB_SIZEOF_WCHAR_T    2
#else
#   define         ALIB_SIZEOF_WCHAR_T     4 // guessing, will be checked below
#endif

static_assert( sizeof(wchar_t) == ALIB_SIZEOF_WCHAR_T, "Error: Platform not supported" );

#if !defined(ALIB_CHARACTERS_WIDE)
#   if defined(_MSC_VER)
#      define ALIB_CHARACTERS_WIDE 1
#   else
#      define ALIB_CHARACTERS_WIDE 0
#   endif
#endif

#if !defined(ALIB_CHARACTERS_SIZEOF_WCHAR)
#   define ALIB_CHARACTERS_SIZEOF_WCHAR   ALIB_SIZEOF_WCHAR_T
#elif  (ALIB_CHARACTERS_SIZEOF_WCHAR != 2) && (ALIB_CHARACTERS_SIZEOF_WCHAR != 4 )
#   error "Illegal value for symbol ALIB_CHARACTERS_SIZEOF_WCHAR given. Allowed is 2 or 4."
#endif

//  ALIB_DEBUG_CRITICAL_SECTIONS
#if !defined(ALIB_DEBUG_CRITICAL_SECTIONS)
#   define   ALIB_DEBUG_CRITICAL_SECTIONS     0
#elif !ALIB_DEBUG && ALIB_DEBUG_CRITICAL_SECTIONS
#   undef    ALIB_DEBUG_CRITICAL_SECTIONS
#   define   ALIB_DEBUG_CRITICAL_SECTIONS     0
#   pragma message "Symbol ALIB_DEBUG_CRITICAL_SECTIONS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#elif ALIB_SINGLE_THREADED && ALIB_DEBUG_CRITICAL_SECTIONS
#   undef    ALIB_DEBUG_CRITICAL_SECTIONS
#   define   ALIB_DEBUG_CRITICAL_SECTIONS     0
#   pragma message "Symbol ALIB_DEBUG_CRITICAL_SECTIONS set (from outside!) while symbol ALIB_SINGLE_THREADED is set in the ALib Build. The symbol got disabled."
#endif

//  ALIB_DEBUG_ALLOCATIONS
#if !defined(ALIB_DEBUG_ALLOCATIONS)
#   define   ALIB_DEBUG_ALLOCATIONS           0
#elif !ALIB_DEBUG && ALIB_DEBUG_ALLOCATIONS
#   undef    ALIB_DEBUG_ALLOCATIONS
#   define   ALIB_DEBUG_ALLOCATIONS           0
#   pragma message "Symbol ALIB_DEBUG_ALLOCATIONS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

//  ALIB_DEBUG_MEMORY
#if !defined(ALIB_DEBUG_MEMORY)
#   define   ALIB_DEBUG_MEMORY           0
#elif !ALIB_DEBUG && ALIB_DEBUG_MEMORY
#   undef    ALIB_DEBUG_MEMORY
#   define   ALIB_DEBUG_MEMORY           0
#   pragma message "Symbol ALIB_DEBUG_MEMORY set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif


// #################################################################################################
// Preprocessor tools
// #################################################################################################
#if DOXYGEN
#   define     ALIB_NSTRINGIFY(a)
#   define     ALIB_STRINGIFY(a)
#else
#   define     ALIB_STRINGIFY_X(a)      A_CHAR( #a )
#   define     ALIB_STRINGIFY(a)        ALIB_STRINGIFY_X(a)
#   define     ALIB_NSTRINGIFY_X(a)     #a
#   define     ALIB_NSTRINGIFY(a)       ALIB_NSTRINGIFY_X(a)
#endif

// Note: The double expansion ensures that if another macro is given, e.g., "__LINE__", it is
//       expanded before it is concatenated.
#if !DOXYGEN
#define        ALIB_CONCAT_IMPL(a,b)    a ## b
#endif
#define        ALIB_CONCAT(a,b)         ALIB_CONCAT_IMPL(a,b)

#if defined(__clang__)
#   define     ALIB_IDENTIFIER(prefix)  ALIB_WARNINGS_IGNORE_RESERVED_IDENTIFIER    \
                                            ALIB_CONCAT(prefix, __LINE__)           \
                                        ALIB_WARNINGS_RESTORE
#else
#   define     ALIB_IDENTIFIER(prefix)  ALIB_CONCAT(prefix, __LINE__)
#endif

#define     ALIB_EMPTY


#define     ALIB_COMMA              ,
#if ALIB_DEBUG
#  define   ALIB_COMMA_DBG          ,
#else
#  define   ALIB_COMMA_DBG
#endif

// Macros for writing "nicer" static_assert messages
#define ALIB_STATIC_ASSERT( CondVariable, Cond, Message )                                          \
{ constexpr bool CondVariable= Cond;                                                               \
  static_assert( CondVariable, Message );   }                                                      \

#define ALIB_STATIC_DENY( CondVariable, Cond, Message )                                            \
{ constexpr bool CondVariable= !(Cond);                                                            \
  static_assert( CondVariable, Message );   }                                                      \

//==================================================================================================
///  Used with macro \ref ALIB_ASSERT_GLOBAL_NAMESPACE for testing.
//==================================================================================================
struct ALibTestGlobalNamespace;
#define ALIB_ASSERT_GLOBAL_NAMESPACE                                                               \
struct ALibTestGlobalNamespace;                                                                    \
static_assert(std::is_same<ALibTestGlobalNamespace, ::ALibTestGlobalNamespace>::value,             \
              "This is not the global namespace!");


// #################################################################################################
// Other tools
// #################################################################################################
#define ALIB_STACK_ALLOCATED_TYPE(T)                                                               \
private:    void* operator new  (size_t);                                                          \
            void* operator new  (size_t, void*);                                                   \
            void* operator new[](size_t);                                                          \
            void* operator new[](size_t, void*);                                                   \
            T(const T&  );                                                                         \
            T(      T&& );                                                                         \
            void  operator=(const T&  );                                                           \
            void  operator=(      T&& );

// Template tool macros
#define ALIB_TVALUE(T) std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>

#define ALIB_HAS_METHOD(T,Method,...) \
    !std::same_as< lang::UnknownTag, decltype(std::declval<T>(). Method( __VA_ARGS__ ))>



// Macros for passing source code information
#if defined ( _MSC_VER )
#    define ALIB_CALLER_FUNC __FUNCTION__
#else
#    define ALIB_CALLER_FUNC __func__
#endif

#if ALIB_EXT_LIB_THREADS_AVAILABLE
#   define ALIB_CALLER   {__FILE__,__LINE__,ALIB_CALLER_FUNC,::std::this_thread::get_id(),&typeid(*this)}
#else
#   define ALIB_CALLER   {__FILE__,__LINE__,ALIB_CALLER_FUNC,&typeid(*this)}
#endif

#if ALIB_DEBUG
#   define ALIB_CALLER_PRUNED            ALIB_CALLER
#   define ALIB_COMMA_CALLER_PRUNED    , ALIB_CALLER
#   define ALIB_CALLER_PRUNED_COMMA      ALIB_CALLER ,
#   define ALIB_CALLER_NULLED            ALIB_CALLER
#   define ALIB_COMMA_CALLER_NULLED    , ALIB_CALLER
#   define ALIB_CALLER_NULLED_COMMA      ALIB_CALLER ,
#   define ALIB_DBG_TAKE_CI              const CallerInfo& ci
#else
#   define ALIB_CALLER_PRUNED
#   define ALIB_COMMA_CALLER_PRUNED
#   define ALIB_CALLER_PRUNED_COMMA
#   if ALIB_EXT_LIB_THREADS_AVAILABLE
#       define ALIB_CALLER_NULLED         {nullptr,0,nullptr,::std::thread::id(), nullptr}
#       define ALIB_COMMA_CALLER_NULLED , {nullptr,0,nullptr,::std::thread::id(), nullptr}
#       define ALIB_CALLER_NULLED_COMMA   {nullptr,0,nullptr,::std::thread::id(), nullptr} ,
#   else
#       define ALIB_CALLER_NULLED         {nullptr,0,nullptr,nullptr}
#       define ALIB_COMMA_CALLER_NULLED , {nullptr,0,nullptr,nullptr}
#       define ALIB_CALLER_NULLED_COMMA   {nullptr,0,nullptr,nullptr} ,
#   endif
#   define ALIB_DBG_TAKE_CI
#endif

// #################################################################################################
// Debug Messages and Assertions
// #################################################################################################
#if ALIB_DEBUG 
#   if !defined(ALIB_DEBUG_ASSERTION_PRINTABLES)
#      define ALIB_DEBUG_ASSERTION_PRINTABLES 0
#   endif
#   if !DOXYGEN
#       if ALIB_DEBUG_ASSERTION_PRINTABLES
#          define ALIB_DEBUG_ASSERTION_PRINTABLES_DO(...) alib::assert::CheckArgs(ALIB_CALLER_PRUNED, __VA_ARGS__);
#       else
#          define ALIB_DEBUG_ASSERTION_PRINTABLES_DO(...)
#       endif
#   endif

#   define ALIB_ERROR( domain, ...)                { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) alib::assert::Raise( ALIB_CALLER_PRUNED, 0, domain, __VA_ARGS__ ); }
#   define ALIB_WARNING( domain, ...)              { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) alib::assert::Raise( ALIB_CALLER_PRUNED, 1, domain, __VA_ARGS__ ); }
#   define ALIB_MESSAGE( domain, ...)              { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) alib::assert::Raise( ALIB_CALLER_PRUNED, 2, domain, __VA_ARGS__ ); }
#   define ALIB_ASSERT(cond, domain)               {                                                         if( !( cond)) ALIB_ERROR( domain, "Assertion Failed" ); }
#   define ALIB_ASSERT_ERROR( cond, domain, ...)   { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) if( !( cond)) ALIB_ERROR( domain, __VA_ARGS__ ) }
#   define ALIB_ASSERT_WARNING( cond, domain, ...) { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) if( !( cond)) ALIB_WARNING( domain, __VA_ARGS__ ) }
#   define ALIB_ASSERT_MESSAGE( cond, domain, ...) { ALIB_DEBUG_ASSERTION_PRINTABLES_DO(__VA_ARGS__) if( !( cond)) ALIB_MESSAGE( domain, __VA_ARGS__ ) }

#else  // ALIB_DEBUG
    #define ALIB_ERROR( ... )                    {}
    #define ALIB_WARNING( ... )                  {}
    #define ALIB_MESSAGE( ... )                  {}
    #define ALIB_ASSERT( ... )                   {}
    #define ALIB_ASSERT_ERROR( ...)              {}
    #define ALIB_ASSERT_WARNING(...)             {}
    #define ALIB_ASSERT_MESSAGE(...)             {}
#endif


#if ALIB_DEBUG
#   include <assert.h>
    #define ALIB_ASSERT_RESULT_EQUALS(      func, value ) { auto result= func; assert(result == value); ((void) result); }
    #define ALIB_ASSERT_RESULT_NOT_EQUALS(  func, value ) { auto result= func; assert(result != value); ((void) result); }
    #define ALIB_ASSERT_RESULT_GREATER_THAN(func, value ) { auto result= func; assert(result >  value); ((void) result); }
    #define ALIB_ASSERT_RESULT_LESS_THAN(   func, value ) { auto result= func; assert(result <  value); ((void) result); }
#else
    #define ALIB_ASSERT_RESULT_EQUALS(      func, value ) { func; }
    #define ALIB_ASSERT_RESULT_NOT_EQUALS(  func, value ) { func; }
    #define ALIB_ASSERT_RESULT_GREATER_THAN(func, value ) { func; }
    #define ALIB_ASSERT_RESULT_LESS_THAN(   func, value ) { func; }
#endif

// #################################################################################################
// Symbols introduced by lang::integer, and similar basics
// #################################################################################################
//------------- One of the 5 symbols given from outside? ---------------------
#if         defined(ALIB_SIZEOF_INTEGER   )          \
        ||  defined(ALIB_SIZEOF_INTGAP    )          \
        ||  defined(ALIB_INTGAP_TYPE      )          \
        ||  defined(ALIB_SIZEOF_LONGDOUBLE_REPORTED) \
        ||  defined(ALIB_SIZEOF_LONGDOUBLE_WRITTEN)

    // just check if all are given, that's it for now
    #if         !defined(ALIB_SIZEOF_INTEGER)             \
            ||  !defined(ALIB_SIZEOF_INTGAP)              \
            ||  !defined(ALIB_INTGAP_TYPE)                \
            ||  !defined(ALIB_SIZEOF_LONGDOUBLE_REPORTED) \
            ||  !defined(ALIB_SIZEOF_LONGDOUBLE_WRITTEN)

        #error "If one of the Compiler Symbols \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
is given (instead of letting ALib detect them), then the whole group has to be given!"
    #endif


//------------- None of the 5 symbols given from outside: Platform detection ---------------------
#else

    // 64-Bit platforms
    #if  (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8 ) || defined(_WIN64)

        #define ALIB_SIZEOF_INTEGER         8

        #if    defined(_WIN32)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4

        #elif  defined(__APPLE__)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      8

        #elif  defined(__GNUC__) || defined(__clang__)
            #define ALIB_INTGAP_TYPE        long long;
            #define ALIB_SIZEOF_INTGAP      8
        #else
        #   error "Cannot detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

        #endif

        #if defined(_MSC_VER)  || defined(__APPLE__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   8
        #else
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   16
        #endif

    // 32-Bit platforms
    #elif (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4 ) || defined(_WIN32) || defined( __arm__)

        #define ALIB_SIZEOF_INTEGER   4

        #if defined(__APPLE__)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4

        #elif defined(__GNUC__)  || defined(__clang__) || defined(_MSC_VER)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4
        #else
        #error "Cannot detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

        #endif

        #if defined(_MSC_VER) || defined( __arm__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   8
        #elif defined(__APPLE__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   16
        #else
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   12
        #endif

    // unrecognized platform
    #else
            #error "Cannot detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

    #endif   //64, 32, unrecognized

    // deduct stuff
    #if (ALIB_SIZEOF_LONGDOUBLE_REPORTED == 8)
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      8
    #elif defined(__aarch64__)
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      16
    #else
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      10
    #endif
#endif

// #################################################################################################
// Symbols introduced by alib::characters
// #################################################################################################
#if defined(ALIB_CHARACTERS_NATIVE_WCHAR)
#   error "Preprocessor symbol ALIB_CHARACTERS_NATIVE_WCHAR must not be passed to the compiler. It is deduced in ALib headers."
#endif

#if !DOXYGEN // strangely needed for doxygen
#define A_NCHAR(STR)  STR
#if ALIB_CHARACTERS_SIZEOF_WCHAR == ALIB_SIZEOF_WCHAR_T

    #   define ALIB_CHARACTERS_NATIVE_WCHAR 1
    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
            #define A_WCHAR(STR)  L ## STR
            #define A_XCHAR(STR)  U ## STR
            #define A_SCHAR(STR)  U ## STR
    #   else
            #define A_WCHAR(STR)  L ## STR
            #define A_XCHAR(STR)  u ## STR
            #define A_SCHAR(STR)  u ## STR
    #   endif

    #else

    #   define ALIB_CHARACTERS_NATIVE_WCHAR 0

    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
            #define A_WCHAR(STR)  u ## STR
            #define A_XCHAR(STR)  L ## STR
            #define A_SCHAR(STR)  L ## STR
    #   else
            #define A_WCHAR(STR)  U ## STR
            #define A_XCHAR(STR)  L ## STR
            #define A_SCHAR(STR)  L ## STR
    #   endif
#endif
#endif

#if !DOXYGEN // strangely needed for doxygen
#if !ALIB_CHARACTERS_WIDE
    #define A_CHAR(STR)     STR
    #define A_CCHAR(STR)    A_WCHAR(STR)
    #define ALIB_CHAR_TYPE_ID_CHARACTER   1
    #define ALIB_CHAR_TYPE_ID_COMPLEMENT  2
    #define ALIB_CHAR_TYPE_ID_STRANGE     3
#else
    #define A_CHAR(STR)     A_WCHAR(STR)
    #define A_CCHAR(STR)    STR

    #define ALIB_CHAR_TYPE_ID_CHARACTER   2
    #define ALIB_CHAR_TYPE_ID_COMPLEMENT  1
    #define ALIB_CHAR_TYPE_ID_STRANGE     3
#endif
#endif

#define ALIB_CHAR_TYPE_ID_N   1
#define ALIB_CHAR_TYPE_ID_W   2
#define ALIB_CHAR_TYPE_ID_X   3


//#####################  Helper macros to specialize ArrayTraits/ZTArrayTraits  ####################
#if !DOXYGEN
#define ALIB_CHARACTER_ARRAY_internal( PREFIX, T, TChar, Const, pAccess, pConstruction)            \
template<>  struct      PREFIX ## ArrayTraits<T, TChar>                                      {     \
    static constexpr    Policy       Access=        Policy:: pAccess;                          \
    static constexpr    Policy       Construction=  Policy:: pConstruction;                    \
    static inline       const TChar*   Buffer(  T Const &  src );                                  \
    static inline       integer        Length(  T Const &  src );                                  \
    static inline       T              Construct(  const TChar* array, integer length );           \
};
#endif

#define ALIB_CHARACTER_ARRAY(           T, TChar,        Access     , Construction )               \
ALIB_CHARACTER_ARRAY_internal(      ,   T, TChar, const, Access     , Construction )

#define ALIB_CHARACTER_ARRAY_MUTABLE(   T, TChar,                     Construction )               \
ALIB_CHARACTER_ARRAY_internal(      ,   T, TChar,      , MutableOnly, Construction )

#define ALIB_CHARACTER_ZT_ARRAY(        T, TChar,        Access     , Construction )               \
ALIB_CHARACTER_ARRAY_internal(    ZT,   T, TChar, const, Access     , Construction )

#define ALIB_CHARACTER_ZT_ARRAY_MUTABLE( T, TChar,                     Construction )              \
ALIB_CHARACTER_ARRAY_internal(    ZT,   T, TChar,      , MutableOnly, Construction )



#define ALIB_CHARACTER_ARRAY_IMPL_BUFFER(           T, TChar, ... )                                \
const TChar*  ArrayTraits<T,TChar>::Buffer(T const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_LENGTH(           T, TChar, ... )                                \
integer       ArrayTraits<T,TChar>::Length(T const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE(   T, TChar, ... )                                \
const TChar*  ArrayTraits<T,TChar>::Buffer(T      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE(   T, TChar, ... )                                \
integer       ArrayTraits<T,TChar>::Length(T      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT(        T, TChar, ... )                                \
    T  ArrayTraits  <T,TChar>::Construct( const TChar* array, integer length ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER(        T, TChar, ... )                                \
const TChar* ZTArrayTraits<T,TChar>::Buffer( T const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH(        T, TChar, ... )                                \
integer      ZTArrayTraits<T,TChar>::Length( T const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE(T, TChar, ... )                                \
const TChar* ZTArrayTraits<T,TChar>::Buffer( T      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE(T, TChar, ... )                                \
integer      ZTArrayTraits<T,TChar>::Length( T      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT(     T, TChar, ... )                                \
    T  ZTArrayTraits<T,TChar>::Construct( const TChar* array, integer length ) { __VA_ARGS__ }


// #################################################################################################
// Symbols introduced by lang::Owner
// #################################################################################################
#define   ALIB_OWN(          ownable) alib::lang::Owner         <decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);
#define   ALIB_OWN_RECURSIVE(ownable) alib::lang::OwnerRecursive<decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);
#define   ALIB_OWN_SHARED(   ownable) alib::lang::OwnerShared   <decltype(ownable)> ALIB_IDENTIFIER(owner) (ownable ALIB_COMMA_CALLER_PRUNED);

// Thread-related aliases (placed here to achieve code agnostic from availability of module Threads)
#if !ALIB_SINGLE_THREADED
#   define        ALIB_LOCK                              ALIB_OWN(*this)
#   define        ALIB_LOCK_RECURSIVE                    ALIB_OWN_RECURSIVE(*this)
#   define        ALIB_LOCK_SHARED                       ALIB_OWN_SHARED(*this)
#   define        ALIB_LOCK_WITH(lock)                   ALIB_OWN(lock)
#   define        ALIB_LOCK_RECURSIVE_WITH(lock)         ALIB_OWN_RECURSIVE(lock)
#   define        ALIB_LOCK_SHARED_WITH(lock)            ALIB_OWN_SHARED(lock)
#else
#   if ALIB_DEBUG
#       define   ALIB_LOCK                               { alib::assert::SingleThreaded(); }
#       define   ALIB_LOCK_RECURSIVE                     { alib::assert::SingleThreaded(); }
#       define   ALIB_LOCK_SHARED                        { alib::assert::SingleThreaded(); }
#       define   ALIB_LOCK_WITH(VOID)                    { alib::assert::SingleThreaded(); }
#       define   ALIB_LOCK_RECURSIVE_WITH(VOID)          { alib::assert::SingleThreaded(); }
#       define   ALIB_LOCK_SHARED_WITH(VOID)             { alib::assert::SingleThreaded(); }
#   else
#       define   ALIB_LOCK                               {}
#       define   ALIB_LOCK_RECURSIVE                     {}
#       define   ALIB_LOCK_SHARED                        {}
#       define   ALIB_LOCK_WITH(VOID)                    {}
#       define   ALIB_LOCK_RECURSIVE_WITH(VOID)          {}
#       define   ALIB_LOCK_SHARED_WITH(VOID)             {}
#   endif
#endif // ALIB_SINGLE_THREADED


// Recursive invocation detection
#if ALIB_DEBUG
#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL                                     \
bool   dbgRecursionDetectionFlag   = false;

#define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS                                                 \
struct RecursionDetection                                                                          \
{                                                                                                  \
    bool& TestMember;                                                                              \
    RecursionDetection( bool& testMember ) : TestMember(testMember) {}                             \
                                                                                                   \
    void  Acquire( const lang::CallerInfo& ci )                                                    \
    {                                                                                              \
       ALIB_ASSERT_ERROR(TestMember==false,"FSOWNER","Forbidden recursive use of method ", ci.Func)\
       TestMember= true;                                                                           \
    }                                                                                              \
    void  Release()    { TestMember= false;    }                                                   \
};                                                                                                 \
RecursionDetection dbgRecursionDetection( dbgRecursionDetectionFlag );                             \
ALIB_OWN(dbgRecursionDetection);
#else
#   define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
#   define    ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS
#endif

// #################################################################################################
// Symbols introduced by lang::DbgCriticalSections
// #################################################################################################

#if ALIB_DEBUG
#   if !ALIB_SINGLE_THREADED && ALIB_DEBUG_CRITICAL_SECTIONS
#      define ALIB_DCS                            ALIB_OWN(*this)
#      define ALIB_DCS_SHARED                     ALIB_OWN_SHARED(*this)
#      define ALIB_DCS_WITH(CS)                   ALIB_OWN(CS)
#      define ALIB_DCS_SHARED_WITH(CS)            ALIB_OWN_SHARED(CS)
#      define ALIB_DCS_ACQUIRE                       Acquire(ALIB_CALLER);
#      define ALIB_DCS_RELEASE                       Release(ALIB_CALLER);
#      define ALIB_DCS_ACQUIRE_WITH(CS)           CS.Acquire(ALIB_CALLER);
#      define ALIB_DCS_RELEASE_WITH(CS)           CS.Release(ALIB_CALLER);
#      define ALIB_DCS_ACQUIRE_SHARED                AcquireShared(ALIB_CALLER);
#      define ALIB_DCS_RELEASE_SHARED                ReleaseShared(ALIB_CALLER);
#      define ALIB_DCS_ACQUIRE_SHARED_WITH(CS)    CS.AcquireShared(ALIB_CALLER);
#      define ALIB_DCS_RELEASE_SHARED_WITH(CS)    CS.ReleaseShared(ALIB_CALLER);
#   else
#      define ALIB_DCS                            { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_SHARED                     { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_WITH(...)                  { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_RECURSIVE_WITH(...)        { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_SHARED_WITH(...)           { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_ACQUIRE                    { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_RELEASE                    {}
#      define ALIB_DCS_ACQUIRE_WITH(CS)           { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_RELEASE_WITH(CS)           {}
#      define ALIB_DCS_ACQUIRE_SHARED             { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_RELEASE_SHARED             {}
#      define ALIB_DCS_ACQUIRE_SHARED_WITH(CS)    { alib::assert::SingleThreaded(); }
#      define ALIB_DCS_RELEASE_SHARED_WITH(CS)    {}
#   endif
#else
#   define ALIB_DCS                            {}
#   define ALIB_DCS_SHARED                     {}
#   define ALIB_DCS_WITH(...)                  {}
#   define ALIB_DCS_RECURSIVE_WITH(...)        {}
#   define ALIB_DCS_SHARED_WITH(...)           {}
#   define ALIB_DCS_ACQUIRE                    {}
#   define ALIB_DCS_RELEASE                    {}
#   define ALIB_DCS_ACQUIRE_WITH(CS)           {}
#   define ALIB_DCS_RELEASE_WITH(CS)           {}
#   define ALIB_DCS_ACQUIRE_SHARED             {}
#   define ALIB_DCS_RELEASE_SHARED             {}
#   define ALIB_DCS_ACQUIRE_SHARED_WITH(CS)    {}
#   define ALIB_DCS_RELEASE_SHARED_WITH(CS)    {}
#endif

#define bitsof(type) int(sizeof(type) * 8)

#endif // INL_ALIB

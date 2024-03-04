/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * This header does not belong to a module of \alib, but is
 * included in any \alibdist.
 * Its purposes are:
 * - To identify given \ref GrpALibPreproSymbols  related to module selection and enable
 *   corresponding symbols (that may be not given) of dependent modules.
 * - Provide functions \alib{Bootstrap} and \alib{Shutdown} which select the right bootstrapping
 *   and shutdown methods, depending on the \alibdist_nl.
 *
 * \note
 *   This header is not shown in inclusion graphs of this documentation, as it is always
 *   included directly or indirectly.
 **************************************************************************************************/
#ifndef HPP_ALIB
#define HPP_ALIB 1

#define ALIB_VERSION              2402
#define ALIB_REVISION             1

#if !defined(ALIB_DOX)

// #################################################################################################
// No module selection symbol given but threads? -> choose ALL
// #################################################################################################
#if     !defined( ALIB_ALOX         )       \
     && !defined( ALIB_BITBUFFER    )       \
     && !defined( ALIB_BOXING       )       \
     && !defined( ALIB_CHARACTERS   )       \
     && !defined( ALIB_CLI          )       \
     && !defined( ALIB_CONFIGURATION)       \
     && !defined( ALIB_ENUMS        )       \
     && !defined( ALIB_EXPRESSIONS  )       \
     && !defined( ALIB_FILES        )       \
     && !defined( ALIB_MONOMEM      )       \
     && !defined( ALIB_SINGLETONS   )       \
     && !defined( ALIB_STRINGS      )       \
     && !defined( ALIB_CAMP         )       \
     && !defined( ALIB_TIME         )       \
     && !defined( ALIB_THREADS      )
#   define  ALIB_ALOX                1
#   define  ALIB_EXPRESSIONS         1
#   define  ALIB_CLI                 1
#   define  ALIB_FILES               1
#   define  ALIB_THREADS             1
#   define  ALIB_BITBUFFER           1
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
#if !defined(ALIB_CHARACTERS   )
#   define ALIB_CHARACTERS          0
#endif
#if !defined(ALIB_CONFIGURATION)
#   define ALIB_CONFIGURATION       0
#endif
#if !defined(ALIB_ENUMS        )
#   define ALIB_ENUMS               0
#endif
#if !defined(ALIB_EXPRESSIONS  )
#   define ALIB_EXPRESSIONS         0
#endif
#if !defined(ALIB_FILES        )
#   define ALIB_FILES               0
#endif
#if !defined(ALIB_MONOMEM      )
#   define ALIB_MONOMEM              0
#endif
#if !defined(ALIB_SINGLETONS   )
#   define ALIB_SINGLETONS          0
#endif
#if !defined(ALIB_STRINGS      )
#   define ALIB_STRINGS             0
#endif
#if !defined(ALIB_CAMP       )
#   define ALIB_CAMP              0
#endif
#if !defined(ALIB_TIME         )
#   define ALIB_TIME                0
#endif
#if !defined( ALIB_THREADS     )
#   define  ALIB_THREADS            0
#endif


// #################################################################################################
// Resolve module dependencies
// #################################################################################################
#if ALIB_ALOX
#    undef  ALIB_CONFIGURATION
#    define ALIB_CONFIGURATION       1
#endif
#if ALIB_CLI
#    undef  ALIB_CAMP
#    define ALIB_CAMP                1
#endif
#if ALIB_CONFIGURATION
#    undef  ALIB_CAMP
#    define ALIB_CAMP                1
#endif
#if ALIB_EXPRESSIONS
#    undef  ALIB_CAMP
#    define ALIB_CAMP                1
#endif
#if ALIB_FILES
#    undef  ALIB_CAMP
#    define ALIB_CAMP                1
#endif
#if ALIB_CAMP
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#    undef  ALIB_BOXING
#    define ALIB_BOXING              1
#    undef  ALIB_ENUMS
#    define ALIB_ENUMS               1
#    undef  ALIB_TIME
#    define ALIB_TIME                1
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM             1
#endif
#if ALIB_BITBUFFER
#    undef  ALIB_MONOMEM
#    define ALIB_MONOMEM             1
#    undef  ALIB_ENUMS
#    define ALIB_ENUMS               1
#endif
#if ALIB_ENUMS
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS          1
#endif
#if ALIB_THREADS
#    undef  ALIB_STRINGS
#    define ALIB_STRINGS             1
#endif
#if ALIB_STRINGS
#    undef  ALIB_CHARACTERS
#    define ALIB_CHARACTERS          1
#endif
#if ALIB_BOXING
#    undef  ALIB_SINGLETONS
#    define ALIB_SINGLETONS          1
#    undef  ALIB_CHARACTERS
#    define ALIB_CHARACTERS          1
#endif

// #################################################################################################
// if !defined(ALIB_DOX)
// #################################################################################################
#else
#define ALIB_ALOX                     1
#define ALIB_BITBUFFER                1
#define ALIB_BOXING                   1
#define ALIB_CAMP                     1
#define ALIB_CHARACTERS               1
#define ALIB_CLI                      1
#define ALIB_CONFIGURATION            1
#define ALIB_ENUMS                    1
#define ALIB_EXPRESSIONS              1
#define ALIB_FILES                    1
#define ALIB_MONOMEM                  1
#define ALIB_SINGLETONS               1
#define ALIB_STRINGS                  1
#define ALIB_THREADS                  1
#define ALIB_TIME                     1

#endif //!defined(ALIB_DOX)

// #################################################################################################
// Macros for checking availability of modules
// #################################################################################################
#define  ALIB_DOCUMENTATION_URL   "https://alib.dev/"

#define ALIB_ASSERT_MODULE(modulename)                                                             \
static_assert( ALIB_ ## modulename,                                                                \
               "This module is not included in the ALib distribution/built. "                      \
               "See " ALIB_DOCUMENTATION_URL "alib_manual.html for more information" );            \

// #################################################################################################
// Macros to sect code (without using #if/#endif)
// #################################################################################################
#if ALIB_ALOX
#   define  ALIB_IF_ALOX(...)               __VA_ARGS__
#   define  ALIB_IFN_ALOX(...)
#else
#   define  ALIB_IF_ALOX(...)
#   define  ALIB_IFN_ALOX(...)              __VA_ARGS__
#endif

#if ALIB_BITBUFFER
#   define  ALIB_IF_BITBUFFER(...)          __VA_ARGS__
#   define  ALIB_IFN_BITBUFFER(...)
#else
#   define  ALIB_IF_BITBUFFER(...)
#   define  ALIB_IFN_BITBUFFER(...)          __VA_ARGS__
#endif

#if ALIB_BOXING
#   define  ALIB_IF_BOXING(...)             __VA_ARGS__
#   define  ALIB_IFN_BOXING(...)
#else
#   define  ALIB_IF_BOXING(...)
#   define  ALIB_IFN_BOXING(...)            __VA_ARGS__
#endif

#if ALIB_CHARACTERS
#   define  ALIB_IF_CHARACTERS(...)         __VA_ARGS__
#   define  ALIB_IFN_CHARACTERS(...)
#else
#   define  ALIB_IF_CHARACTERS(...)
#   define  ALIB_IFN_CHARACTERS(...)        __VA_ARGS__
#endif

#if ALIB_CLI
#   define  ALIB_IF_CLI(...)                __VA_ARGS__
#   define  ALIB_IFN_CLI(...)
#else
#   define  ALIB_IF_CLI(...)
#   define  ALIB_IFN_CLI(...)               __VA_ARGS__
#endif

#if ALIB_CONFIGURATION
#   define  ALIB_IF_CONFIGURATION(...)      __VA_ARGS__
#   define  ALIB_IFN_CONFIGURATION(...)
#else
#   define  ALIB_IF_CONFIGURATION(...)
#   define  ALIB_IFN_CONFIGURATION(...)     __VA_ARGS__
#endif

#if ALIB_ENUMS
#   define  ALIB_IF_ENUMS(...)              __VA_ARGS__
#   define  ALIB_IFN_ENUMS(...)
#else
#   define  ALIB_IF_ENUMS(...)
#   define  ALIB_IFN_ENUMS(...)             __VA_ARGS__
#endif

#if ALIB_EXPRESSIONS
#   define  ALIB_IF_EXPRESSIONS(...)        __VA_ARGS__
#   define  ALIB_IFN_EXPRESSIONS(...)
#else
#   define  ALIB_IF_EXPRESSIONS(...)
#   define  ALIB_IFN_EXPRESSIONS(...)       __VA_ARGS__
#endif

#if ALIB_FILES
#   define  ALIB_IF_FILES(...)              __VA_ARGS__
#   define  ALIB_IFN_FILES(...)
#else
#   define  ALIB_IF_FILES(...)
#   define  ALIB_IFN_FILES(...)             __VA_ARGS__
#endif

#if ALIB_MONOMEM
#   define  ALIB_IF_MONOMEM(...)            __VA_ARGS__
#   define  ALIB_IFN_MONOMEM(...)
#else
#   define  ALIB_IF_MONOMEM(...)
#   define  ALIB_IFN_MONOMEM(...)           __VA_ARGS__
#endif

#if ALIB_SINGLETONS
#   define  ALIB_IF_SINGLETONS(...)         __VA_ARGS__
#   define  ALIB_IFN_SINGLETONS(...)
#else
#   define  ALIB_IF_SINGLETONS(...)
#   define  ALIB_IFN_SINGLETONS(...)        __VA_ARGS__
#endif

#if ALIB_STRINGS
#   define  ALIB_IF_STRINGS(...)            __VA_ARGS__
#   define  ALIB_IFN_STRINGS(...)
#else
#   define  ALIB_IF_STRINGS(...)
#   define  ALIB_IFN_STRINGS(...)           __VA_ARGS__
#endif

#if ALIB_CAMP
#   define  ALIB_IF_CAMP(...)             __VA_ARGS__
#   define  ALIB_IFN_CAMP(...)
#else
#   define  ALIB_IF_CAMP(...)
#   define  ALIB_IFN_CAMP(...)            __VA_ARGS__
#endif

#if ALIB_THREADS
#   define  ALIB_IF_THREADS(...)            __VA_ARGS__
#   define  ALIB_IFN_THREADS(...)
#else
#   define  ALIB_IF_THREADS(...)
#   define  ALIB_IFN_THREADS(...)           __VA_ARGS__
#endif

#if ALIB_TIME
#   define  ALIB_IF_TIME(...)               __VA_ARGS__
#   define  ALIB_IFN_TIME(...)
#else
#   define  ALIB_IF_TIME(...)
#   define  ALIB_IFN_TIME(...)              __VA_ARGS__
#endif

// #################################################################################################
// ALib Feature detection
// (Note: this has to be done outside of the module code, because the features are used with
//        the compilation verification flags below)
// #################################################################################################

// ALIB_FEAT_SINGLETON_MAPPED
#if !defined(ALIB_FEAT_SINGLETON_MAPPED)
#   if defined(_WIN32)
#       define ALIB_FEAT_SINGLETON_MAPPED   1
#   else
#       define ALIB_FEAT_SINGLETON_MAPPED   0
#   endif
#endif

// ALIB_CHARACTERS_WIDE, ALIB_SIZEOF_WCHAR_T
#if defined(__WCHAR_MAX__)
    #if    __WCHAR_MAX__ == 0x7FFFFFFF  \
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

//  ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
//  ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS
//  ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
//  ALIB_DEBUG_BOXING
#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS     0
#endif

#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS    0
#endif

#if !defined(ALIB_FEAT_BOXING_BIJECTIVE_FLOATS)
#   define ALIB_FEAT_BOXING_BIJECTIVE_FLOATS        0
#endif

#if !defined(ALIB_DEBUG_BOXING)
#   define ALIB_DEBUG_BOXING                        0
#elif !ALIB_DEBUG && ALIB_DEBUG_BOXING
#   undef    ALIB_DEBUG_BOXING
#   define   ALIB_DEBUG_BOXING                      0
#   pragma message "Symbol ALIB_DEBUG_BOXING set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

//  ALIB_DEBUG_STRINGS
#if !defined(ALIB_DEBUG_STRINGS)
#   define   ALIB_DEBUG_STRINGS 0
#elif !ALIB_DEBUG && ALIB_DEBUG_STRINGS
#   undef    ALIB_DEBUG_STRINGS
#   define   ALIB_DEBUG_STRINGS 0
#   pragma message "Symbol ALIB_DEBUG_STRINGS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

//  ALIB_DEBUG_MONOMEM
#if !defined(ALIB_DEBUG_MONOMEM)
#   define   ALIB_DEBUG_MONOMEM           0
#elif !ALIB_DEBUG && ALIB_DEBUG_MONOMEM
#   undef    ALIB_DEBUG_MONOMEM
#   define   ALIB_DEBUG_MONOMEM           0
#   pragma message "Symbol ALIB_DEBUG_MONOMEM set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

//  ALIB_DEBUG_RESOURCES
#if !defined(ALIB_DEBUG_RESOURCES)
#   define   ALIB_DEBUG_RESOURCES           0
#elif !ALIB_DEBUG && ALIB_DEBUG_RESOURCES
#   undef    ALIB_DEBUG_RESOURCES
#   define   ALIB_DEBUG_RESOURCES           0
#   pragma message "Symbol ALIB_DEBUG_RESOURCES set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

// ALOX_DBG_LOG, ALOX_REL_LOG, ALOX_DBG_LOG_CI, ALOX_REL_LOG_CI
#if !defined(ALOX_DBG_LOG)
#   define ALOX_DBG_LOG    ALIB_DEBUG
#endif
#if !defined(ALOX_DBG_LOG_CI)
#   define ALOX_DBG_LOG_CI ALIB_DEBUG
#endif

#if !defined(ALOX_REL_LOG)
#   define ALOX_REL_LOG    1
#endif
#if !defined(ALOX_REL_LOG_CI)
#   define ALOX_REL_LOG_CI 0
#endif

// #################################################################################################
// Compiler detection and specifics
// #################################################################################################
#if !defined(ALIB_DOX)
    #define  DOX_MARKER( marker )
#endif


// GCC Compiler detection
#if   defined(__clang__)
#elif defined(__INTEL_COMPILER) //never tested
#elif defined(_MSC_VER)
#elif defined(__GNUC__)
#   define ALIB_GCC 1
#endif



// Debug or release compilation
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


// --- C++ standard: set ALIB_CPP_STANDARD ---

// deduce && disallow < 17
#if defined ( _MSC_VER )
#   if  _MSC_VER < 1928 // VS 2019 == VC16.8 == _MSC_VER 1928 -> C++17
#       error "ALib needs C++ standard 17. Microsoft compiler < v16.8 not supported. Compilation aborted"
#   endif
#   if defined(_MSVC_LANG)
#      if    _MSVC_LANG < 201703L
#        error "ALib needs C++ standard 17. Wrong compilation settings given."
#      elif _MSVC_LANG == 201703L
#        define  ALIB_INTERNAL_DEDUCED_CPPVER  17
#      elif _MSVC_LANG == 202002L
#        define  ALIB_INTERNAL_DEDUCED_CPPVER  20
#      elif _MSVC_LANG >  202002L
#        define  ALIB_INTERNAL_DEDUCED_CPPVER  23
#      endif
#   endif


#elif defined(__cplusplus)
#   if   __cplusplus < 201703L
#      error "ALib needs C++ 17. Compilation aborted"
#   else
#     define  ALIB_INTERNAL_DEDUCED_CPPVER  17
#   endif
#   if  __cplusplus == 202002L
#     undef  ALIB_INTERNAL_DEDUCED_CPPVER
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
#           error "ALIB_CPP_STANDARD was explicitly passed to the compiler, but does not match the language standard deduced by ALib."
#       endif
#   elif ALIB_CPP_STANDARD != 17 && ALIB_CPP_STANDARD != 20 && ALIB_CPP_STANDARD != 23
#       error "ALIB_CPP_STANDARD must be set to 17, 20 or 23."
#   endif
#endif

// final internal check
#if ALIB_CPP_STANDARD != 17 && ALIB_CPP_STANDARD != 20 && ALIB_CPP_STANDARD != 23
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


// Windows DLL Import/Export
#if defined( _MSC_VER ) && !defined( ALIB_API_NO_DLL )
    #ifdef ALIB_API_IS_DLL
        #define ALIB_API  __declspec(dllexport)
    #else
        #define ALIB_API  __declspec(dllimport)
    #endif
#else
    #define ALIB_API
#endif


// annotations
#if defined( _MSC_VER )
#   define  ALIB_FORCE_INLINE           __forceinline
#else
#   define  ALIB_FORCE_INLINE           __attribute__((always_inline))   inline
#endif
#define     ALIB_NO_RETURN              [[ noreturn ]]


// Warnings
#if defined(ALIB_GCC)

    #define ALIB_WARNINGS_UNINITIALIZED_OFF                                     \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")             \

    #define ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING                            \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wnonnull\"")                         \

    #define ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE                             \
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

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")                \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
        _Pragma("GCC diagnostic push")                                          \
        _Pragma("GCC diagnostic ignored \"-Wunused-macros\"")                   \

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

    #define ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE                             \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunsafe-buffer-usage\"")           \

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

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-parameter\"")              \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
        _Pragma("clang diagnostic push")                                        \
        _Pragma("clang diagnostic ignored \"-Wunused-macros\"")                 \

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


    #define ALIB_WARNINGS_RESTORE                                               \
        _Pragma("clang diagnostic pop")                                         \

    #define ALIB_FALLTHROUGH            [[clang::fallthrough]];


#elif defined(_MSC_VER)
    #define ALIB_WARNINGS_UNINITIALIZED_OFF                                     \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4701 ))                                     \

    #define ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING                            \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE                             \
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

    #define ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER                               \
        __pragma(warning( push ))                                               \

    #define ALIB_WARNINGS_IGNORE_UNUSED_MACRO                                   \
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
// Preprocessor tools
// #################################################################################################
#if defined(ALIB_DOX)
#   define     ALIB_NSTRINGIFY(a)
#   define     ALIB_STRINGIFY(a)
#else
#   define     ALIB_STRINGIFY_X(a)      A_CHAR( #a )
#   define     ALIB_STRINGIFY(a)        ALIB_STRINGIFY_X(a)
#   define     ALIB_NSTRINGIFY_X(a)     #a
#   define     ALIB_NSTRINGIFY(a)       ALIB_NSTRINGIFY_X(a)
#endif

#define        ALIB_CONCAT(a,b)         a ## b

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


// Macros for passing source code information
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
  //#define ALIB_CALLER    __FILE__, __LINE__, __PRETTY_FUNCTION__
#elif defined ( _MSC_VER )
#   define ALIB_CALLER     __FILE__, __LINE__, __FUNCTION__
  //#define ALIB_CALLER     __FILE__, __LINE__, __FUNCSIG__
#else
#   pragma message ("Unknown Platform in file: " __FILE__ )
#endif

#if ALIB_DEBUG
    #define ALIB_CALLER_PRUNED        ALIB_CALLER
    #define ALIB_CALLER_NULLED        ALIB_CALLER
#else
    #define ALIB_CALLER_PRUNED
    #define ALIB_CALLER_NULLED        nullptr, 0, nullptr
#endif


// Macros for doing "nicer" static_assert messages
#define ALIB_STATIC_ASSERT( CondVariable, Cond, Message )                                          \
{ constexpr bool CondVariable= Cond;                                                               \
  static_assert( CondVariable, Message );   }                                                      \

#define ALIB_STATIC_DENY( CondVariable, Cond, Message )                                            \
{ constexpr bool CondVariable= !(Cond);                                                            \
  static_assert( CondVariable, Message );   }                                                      \

/** ************************************************************************************************
 *  Used with macro \ref ALIB_ASSERT_GLOBAL_NAMESPACE for testing.
 **************************************************************************************************/
struct ALibTestGlobalNamespace;
#define ALIB_ASSERT_GLOBAL_NAMESPACE                                                               \
struct ALibTestGlobalNamespace;                                                                    \
static_assert(std::is_same<ALibTestGlobalNamespace, ::ALibTestGlobalNamespace>::value,             \
              "This is not the global namespace!");

// #################################################################################################
// Copies of command line arguments of the process
// #################################################################################################
namespace alib {
    /** The number of command line arguments. Defaults to \c 0.
     *  \note An application is responsible for setting this global variable in its <c>main()</c>
     *        function, in case the value is needed somewhere.                                   */
    ALIB_API extern int                 ArgC;

    /** List of command line arguments if given as single byte character strings.
     *  \note An application is responsible for setting this global variable in its <c>main()</c>
     *        function, in case the value is needed somewhere.                                   */
    ALIB_API extern const char**        ArgVN;

    /** List of command line arguments if given as multi-byte character strings.
     *  \note An application is responsible for setting this global variable in its <c>main()</c>
     *        function, in case the value is needed somewhere.                                   */
    ALIB_API extern const wchar_t**     ArgVW;
}

// #################################################################################################
// Debug Messages and Assertions
// #################################################################################################
#if ALIB_DEBUG
#   if !defined (_ASSERT_H) && !defined(assert)
#      include <assert.h>
#   endif

namespace alib::lang {

/**
 * Some \alib modules do not (must not) rely on \alib{lang,Report} /
 * \alib{lang,ReportWriter} mechanics. Therefore, this simple method is
 * used for error handling in those portions of \alib that are exposed in such modules.<br>
 * This method first checks if static function pointer \alib{lang::DBG_SIMPLE_ALIB_MSG_PLUGIN} is set
 * and if yes, passes the parameters to this method and exits.
 * If module \alib_basecamp is included in the \alibdist, method \alib{lang::basecamp,BaseCamp::bootstrap}
 * sets this plug-in function to a custom one which passes the message(s) to a proper
 * \alib{lang,Report,ALib Report}.
 *
 * Otherwise the method just writes to the standard output stream and then,  if \p{type} equals
 * \c 0, invokes <c>assert(0)</c>.
 *
 * @param file    The source file of the message invocation.
 * @param line    The line number within \p{file}.
 * @param method  The method invoking this function.
 * @param type    The type of the message. The default implementation does not use this, other
 *                than invoking <c>assert(0)</c> in the case this parameter equals \c 0.
 * @param topic   The topic of the message.
 * @param msg1    The first message string.
 * @param msg2    Optional 2nd message string.
 * @param msg3    Optional 3rd message string.
 * @param msg4    Optional 4th message string.
 * @param msg5    Optional 5th message string.
 *
 */
ALIB_API
extern void DbgSimpleALibMsg( const char* file,  int line,  const char* method,
                              int         type,
                              const char* topic,
                              const char* msg1= nullptr,
                              const char* msg2= nullptr,
                              const char* msg3= nullptr,
                              const char* msg4= nullptr,
                              const char* msg5= nullptr );

/**
 * Overloaded version of
 * \ref DbgSimpleALibMsg(const char*,int,const char*,int,const char*,const char*,const char*,const char*,const char*,const char*) "DbgSimpleALibMsg"
 * which accepts one integral value and writes \p{msg} and \p{intValue} in sequence.
 *
 * @param file     The source file of the message invocation.
 * @param line     The line number within \p{file}.
 * @param method   The method invoking this function.
 * @param type     The type of the message. The default implementation does not use this, other
 *                 than invoking <c>assert(0)</c> in the case this parameter equals \c 0.
 * @param topic   The topic of the report.
 * @param msg      The message string.
 * @param intValue An integer parameter (optional due to overload).
 *
 */
ALIB_API
extern void DbgSimpleALibMsg( const char*   file,  int line,  const char* method,
                              int           type,
                              const char*   topic,
                              const char*   msg,
                              const int     intValue );

/**
 * This function pointer defaults to \c nullptr and may be set to replace function
 * #DbgSimpleALibMsg.
 * With the use of \alib_basecamp, the module's bootstrap code (precisely method
 * \alib{lang::basecamp,BaseCamp::bootstrap}) sets this pointer to a small method which creates an
 * \alib{lang,Report} on the default \alib{lang,ReportWriter}.
 *
 * - \p{file}:    Information about the scope of invocation.
 * - \p{line}:    Information about the scope of invocation.
 * - \p{method}:  Information about the scope of invocation.
 * - \p{type}:    The type of the message. As a convention, 0 is severe error, others are  warning levels.
 * - \p{qtyMsgs}: The number of messages passed.
 * - \p{msgs}:    The message strings.
 */
extern void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const char* file,  int line,  const char* method,
                                           int type   , const char* topic,
                                           int qtyMsgs, const char** msgs                   );


} // namespace [alib::lang]

#define ALIB_ERROR(...)                { alib::lang::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 0, __VA_ARGS__); }
#define ALIB_WARNING(...)              { alib::lang::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 1, __VA_ARGS__); }
#define ALIB_MESSAGE(...)              { alib::lang::DbgSimpleALibMsg( ALIB_CALLER_PRUNED, 2, __VA_ARGS__); }
#define ALIB_ASSERT(cond)              { if(!(cond)) ALIB_ERROR( "Assertion Failed" ); }
#define ALIB_ASSERT_ERROR(cond, ...)   { if(!(cond)) ALIB_ERROR( __VA_ARGS__ );        }
#define ALIB_ASSERT_WARNING(cond, ...) { if(!(cond)) ALIB_WARNING( __VA_ARGS__ );      }
#define ALIB_ASSERT_MESSAGE(cond, ...) { if(!(cond)) ALIB_MESSAGE( __VA_ARGS__ );      }

#else  // ALIB_DEBUG
    #define ALIB_ERROR(...)                 {}
    #define ALIB_WARNING(...)               {}
    #define ALIB_MESSAGE(...)               {}
    #define ALIB_ASSERT(cond)               {}
    #define ALIB_ASSERT_ERROR(cond, ...)    {}
    #define ALIB_ASSERT_WARNING(cond, ...)  {}
    #define ALIB_ASSERT_MESSAGE(cond, ...)  {}
#endif

#if ALIB_DEBUG
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
// Other ALib Features
// #################################################################################################
// Avoid analyzer (valgrind) warnings
#if !defined(ALIB_AVOID_ANALYZER_WARNINGS )
#   define   ALIB_AVOID_ANALYZER_WARNINGS   1
#endif

// 3rd party libraries included in the build
#if !defined(ALIB_EXT_LIB_THREADS_AVAILABLE)
#   define ALIB_EXT_LIB_THREADS_AVAILABLE   0
#endif
#if !defined(ALIB_FEAT_BOOST_REGEX)
#   define   ALIB_FEAT_BOOST_REGEX          0
#endif


// If module Threads is not available we have to define the locking macros. This allows
// to reduce the effort of testing for thread availability of those modules which are
// tolerant to work in thread-unsafe mode.
// On the same token, this allows a debug-check to assert that no two threads use the library.
#if ALIB_DEBUG && !ALIB_THREADS && ALIB_EXT_LIB_THREADS_AVAILABLE
    namespace alib {  ALIB_API  void DbgCheckSingleThreaded();   }
#else
    namespace alib {  inline    void DbgCheckSingleThreaded() {} } // optimized out
#endif // !ALIB_THREADS

#if !ALIB_THREADS
#   if ALIB_DEBUG
#       define   ALIB_LOCK              { alib::DbgCheckSingleThreaded(); }
#       define   ALIB_LOCK_WITH(VOID)   { alib::DbgCheckSingleThreaded(); }
#   else
#       define   ALIB_LOCK              { }
#       define   ALIB_LOCK_WITH(VOID)   { }
#   endif
#endif // !ALIB_THREADS


// #################################################################################################
// Bootstrapping & Shutdown
// #################################################################################################

// #################################################################################################
// Compilation flags
// Create a bitset from the ALib compilation features.
// For this, we ensure that the individual headers tested are included
// #################################################################################################

namespace alib {
    /** This is a simple copyable set of bits comprising the compilation flags.
     *  @see Used with methods \alib{Bootstrap} which calls \alib{AssertALibVersionAndFlags}. */
    struct TCompilationFlags
    {
        unsigned char bits[4]; ///< The Flags.
    };
}

#if !defined(ALIB_DOX)
#   define    ALIB_COMPILATION_FLAGS                       \
    (1<<0)  *  ALIB_DEBUG                                  \
 +  (1<<1)  *  ALIB_MONOMEM                                \
 +  (1<<2)  *  ALIB_SINGLETONS                             \
 +  (1<<3)  *  ALIB_CHARACTERS                             \
 +  (1<<4)  *  ALIB_ENUMS                                  \
 +  (1<<5)  *  ALIB_TIME                                   \
 +  (1<<6)  *  ALIB_BOXING                                 \
 +  (1<<7)  *  ALIB_STRINGS                                \
 ,                                                         \
 +  (1<<0)  *  ALIB_BITBUFFER                              \
 +  (1<<1)  *  ALIB_THREADS                                \
 +  (1<<2)  *  ALIB_CAMP                                   \
 +  (1<<3)  *  ALIB_ALOX                                   \
 +  (1<<4)  *  ALIB_CLI                                    \
 +  (1<<5)  *  ALIB_CONFIGURATION                          \
 +  (1<<6)  *  ALIB_EXPRESSIONS                            \
 +  (1<<7)  *  ALIB_FILES                                  \
,                                                          \
 +  (1<<0)  *  ALIB_FEAT_SINGLETON_MAPPED                  \
 +  (1<<1)  *  ALIB_CHARACTERS_WIDE                        \
 +  (1<<2)  *  (ALIB_CHARACTERS_SIZEOF_WCHAR == 4 ? 1 : 0) \
 +  (1<<3)  *  ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS        \
 +  (1<<4)  *  ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS       \
 +  (1<<5)  *  ALIB_FEAT_BOXING_BIJECTIVE_FLOATS           \
 +  (1<<6)  *  ALIB_DEBUG_BOXING                           \
 +  (1<<7)  *  ALIB_DEBUG_STRINGS                          \
 ,                                                         \
 +  (1<<0)  *  ALIB_DEBUG_MONOMEM                          \
 +  (1<<1)  *  ALIB_DEBUG_RESOURCES                        \
 +  (1<<2)  *  ALOX_DBG_LOG                                \
 +  (1<<3)  *  ALOX_DBG_LOG_CI                             \
 +  (1<<4)  *  ALOX_REL_LOG                                \
 +  (1<<5)  *  ALOX_REL_LOG_CI


#endif //ALIB_DOX


namespace alib {

/** State of initialization of the \alibmods that do not dispose over a \alib{lang::Camp}.
 *  instance. */
extern bool     NonCampModulesInitialized;

/**
 * Performs standard bootstrapping of \alib. All details are explained
 * in chapter \ref alib_manual_bootstrapping of the \ref alib_manual.
 *
 * \note In addition, function \ref alib::AssertALibVersionAndFlags is invoked.
 *       For this, the parameters of this function must not be given, but left to their
 *       default values, which are read from corresponding preprocessor symbols.<br>
 *       For more information on this topic, see chapter
 *       \ref alib_manual_bootstrapping_assertcomp  of the \ref alib_manual.
 *
 * @param alibVersion      The \alib version required.
 *                         Defaults to \ref ALIB_VERSION and \b must not be passed.
 * @param alibRevision     The \alib sub-version required.
 *                         Defaults to \ref ALIB_REVISION and \b must not be passed.
 * @param compilationFlags The flags as defined in invoking compilation unit.
 *                         Defaults to \ref ALIB_COMPILATION_FLAGS and \b must not be passed.
 */
ALIB_API    void     Bootstrap(int               alibVersion        = ALIB_VERSION,
                               int               alibRevision       = ALIB_REVISION,
                               TCompilationFlags compilationFlags   = TCompilationFlags{ALIB_COMPILATION_FLAGS});

/**
 * Terminates \alib. All details are explained
 * in chapter \ref alib_manual_bootstrapping of the \ref alib_manual.
 */
ALIB_API    void    Shutdown();


/**
 * This is a struct composing an entry in #CompilationFlagMeanings.
 */
struct CompilationFlagMeaningsEntry
{
    int             Flag; ///< The bit number in the bitset.
    const char*     Name; ///< The human readable meaning of the bit.
};

/**
 * This is for the creation of output on information about the bits found in
 * field #CompilationFlags.
 * Declared public to allow exposure, e.g. in verbose logging.
 * \see Method #AssertALibVersionAndFlags for more information.
 */
extern ALIB_API
CompilationFlagMeaningsEntry    CompilationFlagMeanings[30];


/**
 * The module version. Standard \alib versioning scheme is YYMM (as integer number)
 * of the module release date.
 * Besides this version number, field #Revision indicates if this is a revised version
 * of a former release.
 */
extern ALIB_API
int                             Version;

/**
 * The revision number of this release. Usually a module is initially released as
 * revision \e 0. Pure bug-fix releases that do not change the interface of \alib
 * are holding the same #Version but an increased number in this field.
 */
extern ALIB_API
unsigned char                   Revision;

/**
 * These flags are used internally to detect incompatibilities when linking \alib to binaries that
 * use different compilation flags. Declared public to allow exposure, e.g. in verbose logging.
 * \see Function #AssertALibVersionAndFlags, which is invoked by overloaded functions
 *      #Bootstrap.
 */
extern ALIB_API
TCompilationFlags               CompilationFlags;



/** ****************************************************************************************
 * Compares a given set of compilation flags with the ones set in the \alib library.
 * If a mismatch is found, the configuration mismatch is written to <c>std::cerr</c>
 * and <c>exit(255)</c> is performed.
 *
 * This function called in all variants of function \ref alib::Bootstrap.
 * If a using application wants to act different on such a mismatch, an own check of
 * fields \ref alib::Version, \ref alib::Revision and \ref alib::CompilationFlags against
 * preprocessor symbols \ref ALIB_VERSION, \ref ALIB_REVISION and \ref ALIB_COMPILATION_FLAGS
 * has to be performed \e prior to calling \b Bootstrap.
 *
 * \attention A mismatch in \alib library version and compilation flags, may lead to very
 *            dubious misbehavior which is almost impossible to debug. This is true for any
 *            C/C++ software/library mismatch.
 *
 * @param alibVersion      The \alib version required.
 *                         Defaults to \ref ALIB_VERSION and \b must not be passed when invoking
 *                         this function.
 * @param alibRevision     The \alib sub-version required.
 *                         Defaults to \ref ALIB_REVISION and \b must not be passed when invoking
 *                         this function.
 * @param compilationFlags The flags as defined in invoking compilation unit.
 *                         Defaults to \c ALIB_COMPILATION_FLAGS and \b must not be given.
 *
 ******************************************************************************************/
ALIB_API
void            AssertALibVersionAndFlags( int               alibVersion     = ALIB_VERSION,
                                           int               alibRevision    = ALIB_REVISION,
                                           TCompilationFlags compilationFlags= TCompilationFlags{ALIB_COMPILATION_FLAGS} );


} // namespace [alib]


#endif // HPP_ALIB

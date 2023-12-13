/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does neither belong to a fileset, nor to a
 * specific module of \alib, but is included in any \alibdist.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * \note
 *   This header is not shown in inclusion graphs of this documentation, as it is always
 *   included directly or indirectly.
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPILERS
#define HPP_ALIB_COMPILERS 1

#if !defined(ALIB_DOX)
    #define  DOX_MARKER( marker )
#endif

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
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

// C++ standard
#if defined(ALIB_CPP11) || defined(ALIB_CPP14) || defined(ALIB_CPP17) || defined(ALIB_CPP20)
#   error "Preprocessor symbols 'ALIB_CPP**' must not be given from outside."
#endif

#if defined ( _MSC_VER )  &&  _MSC_VER < 1600 // VS 2010 == VC10 == _MSC_VER 1600
#   error "ALib needs C++ 11. Compilation aborted"
#endif
#if defined(_MSVC_LANG)
#     if  _MSVC_LANG == 201103L
#     define  ALIB_CPPVER  11
#     define  ALIB_CPP11   1
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   0
#     define  ALIB_CPP20   0
#   elif  _MSVC_LANG == 201402L
#     define  ALIB_CPPVER  14
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   1
#     define  ALIB_CPP17   0
#     define  ALIB_CPP20   0
#   elif _MSVC_LANG == 201703L
#     define  ALIB_CPPVER  17
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   1
#     define  ALIB_CPP20   0
#   elif _MSVC_LANG >= 202002L
#     define  ALIB_CPPVER  20
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   0
#     define  ALIB_CPP20   1
#   endif
#else
#     if   __cplusplus < 201103L
#      error "ALib needs C++ 11. Compilation aborted"
#   elif   __cplusplus == 201103L
#     define  ALIB_CPPVER  11
#     define  ALIB_CPP11   1
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   0
#     define  ALIB_CPP20   0
#   elif __cplusplus == 201402L
#     define  ALIB_CPPVER  14
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   1
#     define  ALIB_CPP17   0
#   elif __cplusplus == 201703L
#     define  ALIB_CPPVER  17
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   1
#     define  ALIB_CPP20   0
#   elif __cplusplus >= 202002L
#     define  ALIB_CPPVER  20
#     define  ALIB_CPP11   0
#     define  ALIB_CPP14   0
#     define  ALIB_CPP17   0
#     define  ALIB_CPP20   1
#   endif
#endif

#if ALIB_CPPVER >= 14
#   define ALIB_CPP14_CONSTEXPR     constexpr
#   define ALIB_CPP14_REL_CONSTEXPR constexpr
#else
#   define ALIB_CPP14_CONSTEXPR
#   if ALIB_DEBUG
#      define ALIB_CPP14_REL_CONSTEXPR
#   else
#      define ALIB_CPP14_REL_CONSTEXPR constexpr
#   endif
#endif

#if ALIB_CPPVER >= 17
#   define ALIB_CPP17_CONSTEXPR     constexpr
#   define ALIB_CPP17_REL_CONSTEXPR constexpr
#   define ALIB_CONSTEXPR17         constexpr
#   define ALIB_NODISCARD           [[nodiscard]]
#else

#   define ALIB_CPP17_CONSTEXPR
#   if ALIB_DEBUG
#      define ALIB_CPP17_REL_CONSTEXPR
#   else
#      define ALIB_CPP17_REL_CONSTEXPR constexpr
#   endif
#   define ALIB_CONSTEXPR17
#   define ALIB_NODISCARD
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

    #define ALIB_WARNINGS_IGNORE_IF_CONSTEXPR                                   \
        _Pragma("GCC diagnostic push")                                          \

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

    #define ALIB_WARNINGS_IGNORE_IF_CONSTEXPR                                   \
        _Pragma("clang diagnostic push")                                        \

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

#if ALIB_CPPVER < 17
    #define ALIB_WARNINGS_IGNORE_IF_CONSTEXPR                                   \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4127 ))
#else
    #define ALIB_WARNINGS_IGNORE_IF_CONSTEXPR                                   \
        __pragma(warning( push ))                                               
#endif

    #define ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW                     \
        __pragma(warning( push ))                                               \
        __pragma(warning( disable : 4307 ))                                     \
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


#endif // HPP_ALIB_COMPILERS

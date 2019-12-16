/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does neither belong to a fileset, nor to a
 * specific module of \alib, but is included in any \alibdist.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * \note
 *   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
 **************************************************************************************************/
#ifndef HPP_ALIB_TOOLS
#define HPP_ALIB_TOOLS 1

#if !defined(HPP_ALIB_COMPILERS) && !defined(ALIB_DOX)
#   include "alib/lib/compilers.hpp"
#endif

#if !defined(HPP_ALIB_PLATFORMS) && !defined(ALIB_DOX)
#   include "alib/lib/platforms.hpp"
#endif


#if defined(ALIB_DOX)
#   define     ALIB_NSTRINGIFY(a)
#   define     ALIB_STRINGIFY(a)
#else
#   define     ALIB_STRINGIFY_X(a)     A_CHAR( #a )
#   define     ALIB_STRINGIFY(a)       ALIB_STRINGIFY_X(a)
#   define     ALIB_NSTRINGIFY_X(a)    #a
#   define     ALIB_NSTRINGIFY(a)      ALIB_NSTRINGIFY_X(a)
#endif

#define     ALIB_CONCAT(a,b)        a ## b

#define     ALIB_IDENTIFIER(prefix) ALIB_CONCAT(prefix, __LINE__)

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


#endif // HPP_ALIB_TOOLS

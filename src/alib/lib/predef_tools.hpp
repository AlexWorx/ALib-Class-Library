// #################################################################################################
//  ALib C++ Library
//
//  Define code selection symbols for ALib features from given compiler symbols.
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_PREDEF_TOOLS
#define HPP_ALIB_LIB_PREDEF_TOOLS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

ALIB_ASSERT_FILESET(PREDEF_PF)

#define     ALIB_STRINGIFY_X(a)     A_CHAR( #a )
#define     ALIB_STRINGIFY(a)       ALIB_STRINGIFY_X(a)

#define     ALIB_NSTRINGIFY_X(a)    #a
#define     ALIB_NSTRINGIFY(a)      ALIB_NSTRINGIFY_X(a)

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
    #define ALIB_CALLER    __FILE__, __LINE__, __func__
  //#define ALIB_CALLER    __FILE__, __LINE__, __PRETTY_FUNCTION__
#elif defined ( _MSC_VER )
    #define ALIB_CALLER     __FILE__, __LINE__, __FUNCTION__
  //#define ALIB_CALLER     __FILE__, __LINE__, __FUNCSIG__
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
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



#endif // HPP_ALIB_LIB_PREDEF_TOOLS

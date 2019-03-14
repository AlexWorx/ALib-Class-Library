// #################################################################################################
//  ALib C++ Library
//
//  Define code selection symbols for ALib features from given compiler symbols.
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_PREDEF_FEATURES
#define HPP_ALIB_LIB_PREDEF_FEATURES 1


#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(PREDEF_PF)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_PLATFORM)
#   include "alib/lib/predef_platform.hpp"
#endif


// Avoid analyzer (valgrind) warnings
#if defined(ALIB_AVOID_ANALYZER_WARNINGS_OFF) && defined(ALIB_AVOID_ANALYZER_WARNINGS_ON)
    #error "Code selector symbol ALIB_AVOID_ANALYZER_WARNINGS_OFF / ALIB_AVOID_ANALYZER_WARNINGS_ON are both set (contradiction)."
#endif

#if defined( ALIB_AVOID_ANALYZER_WARNINGS_ON )
    #define    ALIB_AVOID_ANALYZER_WARNINGS 1
#else
    #define    ALIB_AVOID_ANALYZER_WARNINGS 0
#endif


// 3rd party libraries included in the build
#if defined(ALIB_FEAT_BOOST_REGEX)
    #error "Code selector symbol ALIB_FEAT_BOOST_REGEX must not be set from outside. Use postfix '_ON' or '_OFF' for compiler symbols."
#endif

#if defined(ALIB_FEAT_BOOST_REGEX_ON) && defined(ALIB_FEAT_BOOST_REGEX_OFF)
    #error "Compiler symbols ALIB_FEAT_BOOST_REGEX_ON / ALIB_FEAT_BOOST_REGEX_OFF are both set (contradiction)."
#endif

#if defined(ALIB_FEAT_BOOST_REGEX_ON)
    #define ALIB_FEAT_BOOST_REGEX 1
#else
    #define ALIB_FEAT_BOOST_REGEX 0
#endif



// If module Threads is not available we have to define the locking macros. This allows
// to reduce the effort of testing for thread availability of those modules which are
// tolerant to work in thread-unsafe mode.
#if !ALIB_MODULE_THREADS
#   define   ALIB_LOCK
#   define   ALIB_LOCK_WITH(VOID)
#endif




#endif // HPP_ALIB_LIB_PREDEF_FEATURES

/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does neither belong to a fileset, nor to a
 * specific module of \alib, but is included in any \alibdist.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 *
 * \note
 *   To reduce complexity, this header is not shown in inclusion graphs of this documentation.
 **************************************************************************************************/
#ifndef HPP_ALIB_FEATURES
#define HPP_ALIB_FEATURES 1

#if !defined(HPP_ALIB_PLATFORMS) && !defined(ALIB_DOX)
#   include "alib/lib/platforms.hpp"
#endif


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
    namespace aworx { namespace lib
    {
        ALIB_API  void DbgCheckSingleThreaded();
    }}
#else
    namespace aworx { namespace lib
    {
        inline    void DbgCheckSingleThreaded() {} // optimized out
    }}
#endif // !ALIB_THREADS

#if !ALIB_THREADS
#   if ALIB_DEBUG
#       define   ALIB_LOCK              { aworx::lib::DbgCheckSingleThreaded(); }
#       define   ALIB_LOCK_WITH(VOID)   { aworx::lib::DbgCheckSingleThreaded(); }
#   else
#       define   ALIB_LOCK              { }
#       define   ALIB_LOCK_WITH(VOID)   { }
#   endif
#endif // !ALIB_THREADS


#if ALIB_DEBUG && ALIB_FILESET_MODULES
    namespace aworx { namespace lib {
       /**
        * This flag may be used to determine if \alib is \ref alib_manual_bootstrapping "bootstrapped"
        * already. While in general it is preferred to test corresponding single modules for
        * being bootstrapped using \alib{Module::IsBootstrapped}, this flag may be used to
        * avoid the inclusion of header files of corresponding modules.
        *
        * The flag is set to \c true at the end of phase \alib{Module,BootstrapPhases::Final}
        * of module \alib{ALibDistribution} and is cleared at the start of shutdown phase
        * \alib{Module,ShutdownPhases::Destruct}.
        *
        * ### Availability ###
        * This flag is available only with debug builds if fileset \ref ALIB_FILESET_MODULES is
        * included in the \alibdist.
        */
        extern ALIB_API bool DbgIsBootstrapped;
    }}
#endif




#endif // HPP_ALIB_FEATURES

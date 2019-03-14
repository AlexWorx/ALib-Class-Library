// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_SYSTEM_CONSOLE
#define HPP_ALIB_SYSTEM_CONSOLE 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(SYSTEM)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_PLATFORM)
#   include "alib/lib/predef_platform.hpp"
#endif


namespace aworx { namespace lib { namespace system {
/** ************************************************************************************************
 * This class provides system dependent features in respect the console attached to a process.
 **************************************************************************************************/
class Console
{
    protected:
        /** Console text width. */
        ALIB_API static
        int                 lastReceivedWidth;

    public:
        /** ****************************************************************************************
         * Tries to detect the current width of the console. Returns \c 0 in case of failure.
         *
         * @param forceRedetect If \c true, the console size is tried to be detected, even if it
         *                      had been done before (and even if it failed before).<br>
         *                      Defaults to \c false.
         *
         * @return The console width.
         ******************************************************************************************/
        ALIB_API static
        int                  GetWidth( bool forceRedetect = false );
};

}} // namespace lib::system

/// Type alias in namespace #aworx.
using     Console=       aworx::lib::system::Console;

}  // namespace [aworx]

#endif // HPP_ALIB_SYSTEM_CONSOLE

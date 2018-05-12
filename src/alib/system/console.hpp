// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_SYSTEM_CONSOLE)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_SYSTEM_CONSOLE
//! @cond NO_DOX
#define HPP_ALIB_SYSTEM_CONSOLE 1
//! @endcond

namespace aworx { namespace lib { namespace system {
/** ************************************************************************************************
 * This class provides system dependent features in respect the console attached to a process.
 **************************************************************************************************/
class Console
{
    protected:
        /// Console text width.
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

}  // namespace aworx

#endif // HPP_ALIB_SYSTEM_CONSOLE

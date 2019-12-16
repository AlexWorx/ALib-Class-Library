/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SYSTEM_CONSOLE
#define HPP_ALIB_SYSTEM_CONSOLE 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_MODULE(SYSTEM)

#if !defined(HPP_ALIB_PLATFORMS) && !defined(ALIB_DOX)
#   include "alib/lib/platforms.hpp"
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
using     Console=       lib::system::Console;

}  // namespace [aworx]

#endif // HPP_ALIB_SYSTEM_CONSOLE

/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #  alib::lang::system of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_CONSOLE
#define HPP_ALIB_LANG_CONSOLE 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)


namespace alib { namespace  lang::system {

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

} // namespace alib[:: lang::system]

/// Type alias in namespace \b alib.
using     Console=        lang::system::Console;

} // namespace [alib]

#endif // HPP_ALIB_LANG_CONSOLE

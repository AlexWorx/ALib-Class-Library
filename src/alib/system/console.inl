//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  system {

//==================================================================================================
/// This class provides system dependent features in respect the console attached to a process.
//==================================================================================================
class Console
{
    protected:
        /// Console text width.
        ALIB_DLL static
        int                 lastReceivedWidth;

    public:
        //==========================================================================================
        /// Tries to detect the current width of the console. Returns \c 0 in case of failure.
        ///
        /// @param forceRedetect If \c true, the console size is tried to be detected, even if it
        ///                      had been done before (and even if it failed before).<br>
        ///                      Defaults to \c false.
        ///
        /// @return The console width.
        //==========================================================================================
        ALIB_DLL static
        int                  GetWidth( bool forceRedetect = false );
};

} // namespace alib[::system]

/// Type alias in namespace \b alib.
using     Console=        system::Console;

} // namespace [alib]



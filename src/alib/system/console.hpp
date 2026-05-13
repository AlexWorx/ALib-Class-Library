//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  system {

//==================================================================================================
/// This class provides system-dependent features in respect the console attached to a process.
//==================================================================================================
class Console {
  protected:
        /// Console text width.
        ALIB_DLL static
        int                 lastReceivedWidth;

  public:
    /// Tries to detect the current width of the console. Returns \c 0 in case of failure.
    /// @param forceRedetect If \c true, the console size is tried to be detected, even if it
    ///                      had been done before (and even if it failed before).<br>
    ///                      Defaults to \c false.
    /// @param defaultWidth  The default value, in case the detection fails or the detected
    ///                      value is \c 0.<br>
    ///                      Defaults to \c 80.
    ///
    /// @return The console width.
    ALIB_DLL static
    int                  GetWidth( bool forceRedetect = false, int defaultWidth= 80 );
};

} // namespace alib[::system]

/// Type alias in namespace #"%alib".
using     Console=        system::Console;

} // namespace [alib]

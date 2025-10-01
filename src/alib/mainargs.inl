//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
/// With supporting legacy or module builds, .mpp-files are either recognized by the build-system
/// as C++20 Module interface files, or are included by the
/// \ref alib_manual_modules_impludes "import/include headers".
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {

    /// The number of command line arguments. Defaults to \c 0.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern int                 ARG_C;

    /// List of command line arguments if given as single byte character strings.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern const char**        ARG_VN;

    /// List of command line arguments if given as multi-byte character strings.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern const wchar_t**     ARG_VW;

} // namespace [alib]







//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {

    /// The number of command-line arguments. Defaults to \c 0.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern int                 ARG_C;

    /// List of command-line arguments if given as single byte character strings.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern const char**        ARG_VN;

    /// List of command-line arguments if given as multi-byte character strings.
    /// \note An application is responsible for setting this global variable in its <c>main()</c>
    ///       function, in case the value is needed somewhere.
    ALIB_DLL extern const wchar_t**     ARG_VW;

} // namespace [alib]

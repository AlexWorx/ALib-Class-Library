//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  system {

//==================================================================================================
/// Simple struct providing one static method that reads an environment variable into an
/// \alib string-type.
///
/// While under posix compatible operating systems, environment variable names are
/// case-sensitive, under Microsoft windows they are not.
//==================================================================================================
struct EnvironmentVariables
{
    //==============================================================================================
    /// Reads an environment variable into an \alib string.
    ///
    /// \attention
    ///    1. While under posix compatible operating systems, environment variable names are
    ///       case-sensitive, under Microsoft windows they are not.
    ///    <br><br> 
    ///    2. As documented with
    ///    \https{std::getenv,en.cppreference.com/w/cpp/utility/program/getenv}, the operation is
    ///    not thread-safe against changes of the environment in parallel threads.
    ///
    /// \note
    ///   When path-variables are requested, usually instead of passing an \b AString for parameter
    ///   \p{target}, a reference of type \alib{system;Path} may be given, which is derived
    ///   from class \b AString.
    ///   However, compatibility of both types depend on the platform and compilation flags
    ///   of the \alibbuild: The character-type of class \b Path equals C++ standard type
    ///   <c>std::filesystem::path::value_type</c>, while the one of \b AString depends on
    ///   compilation symbol \ref ALIB_CHARACTERS_WIDE. On GNU/Linux and MacOS, both default to
    ///   \alib{characters;nchar}, on WindowsOS both default to \alib{characters;wchar} and as such
    ///   compatibility is given with the defaults. If compilation of \alib is explicitly switched
    ///   to use a different character type, then conversion has to be performed.<br><p>
    /// \note
    ///   Class \alib{system;Path} provides built-in mechanics that uses the environment
    ///   to retrieve certain default paths, and that performs conversion when necessary
    ///   internally.
    ///
    ///  @param varName      The name of the variable to be retrieved.
    ///  @param target       An AString to write the result to.
    ///  @param targetData   If \c CurrentData::Keep, the parameter \p{target} is not cleared
    ///                      before the result is written. Defaults to \c CurrentData::Clear.
    ///  @return \c true if variable was found.
    //==============================================================================================
    ALIB_DLL static
    bool  Get(  const CString&    varName,
                AString&          target,
                lang::CurrentData targetData  =lang::CurrentData::Clear  );
};

} // namespace alib[::system]

/// Type alias in namespace \b alib.
using     EnvironmentVariables=        system::EnvironmentVariables;

}  // namespace [alib]



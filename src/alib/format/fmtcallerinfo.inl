//==================================================================================================
/// \file
/// This header-file is part of the module \alib_format of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  format {

/// This struct is used to format caller information provided with type \alib{lang::CallerInfo}.
///
/// Besides using method #Format "manually", instances of type \b CallerInfo may be passed as
/// arguments to type \alib{format;FormatterPythonStyle} using the according placeholder
/// syntax.
/// If done, internally an instance of this type is created and its #Format method called.
///
/// \note
///   This is achieved by boxed function \alib{format;FFormat_CallerInfo}, as
///   explained in chapter \ref alib_format_custom_types of the Programmer's Manual
///   of module \alib_format.
struct FMTCallerInfo
{
    const lang::CallerInfo& ci; ///< The wrapped caller information.

    /// Formats the wrapped caller information using a given pattern string.
    /// Within the pattern string, different symbols are interpreted as tokens.
    ///
    /// Strings within the format text that should not be interpreted as tokens may be given
    /// in single quotes.
    /// Two consecutive single quotes will be replaced to one single quote.<br>
    ///
    /// The following tokens are supported:
    ///
    ///  <center>Token</center>  | <center>Description</center>
    ///  - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ///   sf | Prints the \alib{lang::CallerInfo;File;source file name}.
    ///   sl | Prints the \alib{lang::CallerInfo;Line;line number} in the source file.
    ///   sm | Prints the \alib{lang::CallerInfo;Func;function or method name} in the source file.
    ///   tn | Prints the \alib{threads;Thread::GetName;ALib tread name}. If no thread is set, the string <em>"<None>"</em> is written, which is resourced with <b>"FMTCINT"</b>.
    ///   ti | Prints the \alib{threads;Thread::GetName;ALib tread ID}. If no thread is set, the string <em>"<Null>"</em> is written, which is resourced with <b>"FMTCINR"</b>.
    ///   tc | Prints the native tread ID of type <c>std::thread::id</c> in hexadecimal format. If no thread is set, this value is \c 0.
    ///   ta | Prints a combination of 'tn', 'ti', and 'tc'.
    ///   yf | Prints the full type name. If the \b CallerInfo does originate a static or global function, the string <em>"<None>"</em> is written, which is resourced with <b>"FMTCINY"</b>.<br>(With release-builds, nothing is printed.)
    ///   yn | Prints shortened type name, otherwise like 'yf'.
    ///   ya | Like 'sm' but in case the \b CallerInfo originates from a non-static member function, the shortened type name and <b>"::"</b> are prepended.<br>(With release-builds, nothing is printed.)
    ///
    /// @param format     The format pattern string.
    /// @param target     A reference to an AString that gets the result of the format processing
    ///                   appended.
    /// @param targetData If \c CurrentData::Keep (the default) the string is appended to \p{target}.
    ///                   if \c CurrentData::Clear, \p{target} is cleared.
    /// @returns \p{target} (for convenience).
    ALIB_DLL
    AString&    Format( Substring format,   AString& target,
                        lang::CurrentData targetData= lang::CurrentData::Keep)                const;
};


//==================================================================================================
/// Implementation of \alib{format;FFormat} for boxable type \alib{FMTCallerInfo}.<br>
/// Writes the content of \p{box} (which is of type \b %CallerInfo) to the given \b %AString
/// object \p{target} using a local instance of class \alib{format;FMTCallerInfo} and its
/// method \alib{format;FMTCallerInfo::Format}.
///
/// If parameter \p{formatSpec} is empty, a default format string defined by string resource
/// of key \b "FMTCI" is used. This resource by default evaluates to
/// \par
///  <c>"[@ sf:sl from ''ya'' by ''ta'']"</c>
///
/// which results in the same output that is generated if an instance of class \b CallerInfo is
/// simply appended to an \b AString, what in turn is defined with functor
/// \alib{strings::APPENDABLES;AppendableTraits<lang::CallerInfo,TChar,TAllocator>}.
///
/// @param self       The box that the function was invoked on.
/// @param formatSpec The specification of the format.
/// @param nf         A copy of the number format of the formatter (allowed to be modified).
/// @param target     The AString object receiving the formatted string.
//==================================================================================================
ALIB_DLL void
FFormat_CallerInfo( const Box & self, const String & formatSpec, NumberFormat& nf, AString& target );


} // namespace alib[::format]

/// Type alias in namespace \b alib.
using     FMTCallerInfo      =     format::FMTCallerInfo;

} // namespace [alib]



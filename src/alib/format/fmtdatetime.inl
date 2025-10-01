//==================================================================================================
/// \file
/// This header-file is part of the module \alib_format of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::format {

//==================================================================================================
/// Implementation of \alib{format;FFormat} for boxable type \alib{time;DateTime}.<br>
/// Writes the content of \p{box} (which is of type \b %DateTime) to the given \b %AString
/// object \p{target} using a local instance of class \alib{strings::util;CalendarDateTime}
/// and its method \alib{strings::util;CalendarDateTime::Format}.
///
/// If parameter \p{formatSpec} is empty, a default format string defined by string resource
/// of key \b "FMTDT" is used.
///
/// \note
///   This interface implementation is only available if modules \alib_strings and \alib_boxing
///   are included in the \alibbuild.
///
/// @param self       The box that the function was invoked on.
/// @param formatSpec The specification of the format.
/// @param nf         A copy of the number format of the formatter (allowed to be modified).
/// @param target     The AString object receiving the formatted string.
//==================================================================================================
ALIB_DLL void
FFormat_DateTime( const Box & self, const String & formatSpec, NumberFormat& nf, AString& target );

} // namespace [alib::format]




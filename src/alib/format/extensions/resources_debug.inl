//==================================================================================================
/// \file
/// This header-file contributes to module \alib_resources but for technical reasons is part of
/// the module \alib_format.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if ALIB_DEBUG_RESOURCES
ALIB_EXPORT namespace alib::resources {

//==================================================================================================
/// Writes the list of resources obtainable with \alib{resources;ResourcePool::DbgGetList} to an
/// \b %AString.
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file \implude{Format}.
///   Furthermore, the compiler-symbol \ref ALIB_DEBUG_RESOURCES has to be set.
///
/// \see
///   Methods \alib{resources;ResourcePool::DbgGetList} and
///   \alib{resources;ResourcePool::DbgGetCategories}.
///
/// @param list       The list of resources, obtained with \b DbgGetList.
/// @param catFilter  Comma-separated list of names of categories to print.
///                   Defaults to nulled string, which includes all caegories.
/// @param format     The format of a line.
///                   Defaults to <b>"({3:}) {1}={2!TAB20!ESC<!Q}\\n"</b>.
/// @return The dump of all resources.
//==================================================================================================
ALIB_DLL
AString DbgDump( std::vector<std::tuple<NString, NString, String, integer>>& list,
                 const NString& catFilter = nullptr,
                 const String&  format    = A_CHAR("({3:}) {1}={2!TAB20!ESC<!Q}\n") );

#include "ALib.Lang.CIMethods.H"
} // namespace [alib::enumrecords::bootstrap]

#endif // ALIB_DEBUG_RESOURCES

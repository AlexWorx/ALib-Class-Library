//==================================================================================================
/// \file
/// This header-file contributes to module \alib_resources but for technical reasons is part of
/// the module \alib_format.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if ALIB_DEBUG_RESOURCES
ALIB_EXPORT namespace alib::resources {

//==================================================================================================
/// Writes the list of resources obtainable with #"ResourcePool::DbgGetList;*" to an
/// #"%AString".
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_format and
///   is accessed through the header file #"F;ALib.Format.H".
///   Furthermore, the configuration macro #"ALIB_DEBUG_RESOURCES" has to be set.
///
/// \see
///   Methods #"ResourcePool::DbgGetList;*" and
///   #"ResourcePool::DbgGetCategories;*".
///
/// @param list       The list of resources, obtained with #"%DbgGetList".
/// @param catFilter  Comma-separated list of names of categories to print.
///                   Defaults to nulled string, which includes all caegories.
/// @param format     The format of a line.
///                   Defaults to <b>"({3:}) {1}={2!TAB20!ESC<!Q}\\n"</b>.
/// @return The dump of all resources.
//==================================================================================================
ALIB_DLL
AString DbgDump( std::vector<std::tuple<NString, NString, String, integer>>& list,
                 const NString& catFilter = nullptr,
                 const String&  format    = A_CHAR("({3:}) {1}={2!TAB20!ESC<!Q}\n")  );

#include "ALib.Lang.CIMethods.H"
} // namespace [alib::enumrecords::bootstrap]

#endif // ALIB_DEBUG_RESOURCES

//==================================================================================================
/// \file
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::strings::util  {

#if ALIB_ENUMRECORDS
#if DOXYGEN
//==================================================================================================
/// A utility namespace function that defines a table of token objects from external resourced
/// strings.
///
/// It is possible to provide the table lines in two ways:
/// - In one resource string: In this case, parameter \p{outerDelim} has to specify
///   the delimiter that separates the records.
/// - In an array of resource strings: If the resource string as given is not defined, this
///   method appends an integral index starting with \c 0 to the resource name, parses
///   a single record and increments the index.
///   Parsing ends when the next higher index is not found.
///
/// The second option is recommended for larger token sets. While the separation causes
/// some overhead in a resource backend, the external (!) management (translation,
/// manipulation, etc.) is most probably simplified with this approach.
///
/// \note
///   The length of the given table has to fit to the number of entries found in
///   the resource pool. To ensure this, with debug-builds, parameter \p{dbgSizeVerifier}
///   has to be provided (preferably by using macro \ref ALIB_DBG "ALIB_DBG(, N)").
///
/// \par Availability
///   This function is an extension, which is injected by the higher-level module \alib_resources
///   and is accessed through the header file \implude{Resources}.
///   Furthermore, the module \alib_enumrecords has to be included in the \alibbuild.
/// @param resourcePool     The resource pool to load the resource from.
/// @param resourceCategory The resource category.
/// @param resourceName     The resource name.
/// @param target           The table to fill.
/// @param dbgSizeVerifier  This parameter has to be specified only in debug-builds and
///                         provides the expected size of the resourced table.
///                         To be surrounded by macro #ALIB_DBG (not to be given in
///                         release-builds.)
/// @param outerSeparator   The character that separates the entries.
///                         Defaults to <c>','</c>.
/// @param innerSeparator   The character that separates the values of an entry.
///                         Defaults to <c>' '</c> (space).
///
/// \par Module Dependencies
///   This method is only available if the modules \alib_resources and \alib_enumrecords are
///   included in the \alibbuild.
//==================================================================================================
ALIB_DLL
void LoadResourcedTokens( resources::ResourcePool&  resourcePool,
                          const NString&            resourceCategory,
                          const NString&            resourceName,
                          strings::util::Token*     target,
                          int                       dbgSizeVerifier,
                          character                 outerSeparator = ',',
                          character                 innerSeparator = ' '     );
#else
ALIB_DLL
void LoadResourcedTokens( resources::ResourcePool&  resourcePool,
                          const NString&            resourceCategory,
                          const NString&            resourceName,
                          strings::util::Token*     target,
              ALIB_DBG(   int                       dbgSizeVerifier, )
                          character                 outerSeparator = ',' ,
                          character                 innerSeparator = ' '     );
#endif // DOXYGEN
#endif // ALIB_ENUMRECORDS


} // namespace [alib::strings::util]

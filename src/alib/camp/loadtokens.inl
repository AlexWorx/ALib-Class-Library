//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::strings::util {

#if DOXYGEN
//==================================================================================================
/// Shortcut to the sibling function \alib{strings::util;LoadResourcedTokens} that accepts a module
/// and uses its resource pool and resource category.
///
/// @param module          The \alibcamp to load the resource from.
/// @param resourceName    The resource name.
/// @param target          The table to fill.
/// @param dbgSizeVerifier This parameter has to be specified only in debug comilations and
///                        provides the expected size of the resourced table.
///                        To be surrounded by macro #ALIB_DBG (not to be given in
///                        release-builds.)
/// @param outerSeparator  The character that separates the entries.
///                        Defaults to <c>','</c>.
/// @param innerSeparator  The character that separates the values of an entry.
///                        Defaults to <c>' '</c> (space).
///
/// \par Module Dependencies
///   This method is only available if module \alib_camp is included in the \alibbuild.
//==================================================================================================
inline
void LoadResourcedTokens( camp::Camp&           module,
                          const NString&        resourceName,
                          strings::util::Token* target,
                          int                   dbgSizeVerifier,
                          character             outerSeparator = ',',
                          character             innerSeparator = ' '     );
#else
inline
void LoadResourcedTokens( camp::Camp&           module,
                          const NString&        resourceName,
                          strings::util::Token* target,
              ALIB_DBG(   int                   dbgSizeVerifier, )
                          character             outerSeparator = ',',
                          character             innerSeparator = ' '     )
{
    LoadResourcedTokens( *module.GetResourcePool(), module.ResourceCategory, resourceName,
                         target, ALIB_DBG(dbgSizeVerifier,) outerSeparator, innerSeparator );
}
#endif // DOXYGEN

} // namespace [alib::strings::util]

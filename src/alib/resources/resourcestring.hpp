// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESOURCES_RESOURCESTRING
#define HPP_ALIB_RESOURCES_RESOURCESTRING 1

#if !defined (HPP_ALIB_STRINGS_ASTRING)
#   include "alib/strings/astring.hpp"
#endif

ALIB_ASSERT_FILESET(MODULES)

#if !defined (HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif


namespace aworx  { namespace lib { namespace resources {


/** ************************************************************************************************
 * This is a simple class implementing a string loaded from the \alib{resources,Resources}
 * instance of a \aworx{lib,Module}.
 *
 * \note
 *   Boxing is performed as with other \alib string classes (as character array).
 *
 * ## Module Dependencies ##
 * This class is only available if \ref alib_manual_modules_filesets "file-set" "Modules"
 * is included in the \alibdist.
 **************************************************************************************************/
struct ResourceString  : public AString
{
    /** The module that we will load from by using resources \alib{Module::Resources}
     *  with \alib{Module::ResourceCategory}  */
    Module&     ResourceModule;

    /** The resource name. */
    NString     Name;

    /**
     * Constructor. Stores parameters. Data is lazily loaded with method #CheckLoad.
     * @param module       The module to load the string from.
     * @param resourceName The resource name.
     */
    ResourceString( Module& module, const NString& resourceName )
    : ResourceModule( module       )
    , Name          ( resourceName )
    {}

    /**
     * With the first invocation, string is loaded from resources.
     * @return A reference to the \b AString part of this object.
     */
    ALIB_API AString&       CheckLoad();

    /**
     * Casts us down to parent type \p{%AString}.
     * Resource hast to be loaded prior to calling this method. In case of doubt, #CheckLoad is
     * to be used instead of this.
     *
     * @return A reference to the \b AString part of this object..
     */
    inline AString&         Get()
    {
        return *dynamic_cast<AString*>(this);
    }
}; // class ResourceString



}} // namespace aworx[::lib::resources]

/// Type alias in namespace #aworx.
using ResourceString=    aworx::lib::resources::ResourceString;

} // namespace [aworx]



#endif // HPP_ALIB_RESOURCES_RESOURCESTRING

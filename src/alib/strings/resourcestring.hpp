// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB_STRINGS_RESOURCESTRING
#define HPP_ALIB_STRINGS_RESOURCESTRING 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(HPP_ALIB_STRINGS_STRING)
    #error "include 'alib/alib.hpp' before including this header. Also make sure to include ALib Module Strings"
#endif


namespace aworx  { namespace lib { namespace strings {


/** ************************************************************************************************
 * This is a simple class implementing a string loaded from the \alib{lang,Resources} object of a
 * library.
 *
 * \note
 *   Boxing is performed as with other \alib string classes (as character array).
 **************************************************************************************************/
struct ResourceString  : public AString
{
    /** The library that we will load from by using resources \alib{lang,Library::Res}
     *  with \alib{lang,Library::ResourceCategory}  */
    Library&    Lib;

    /// The resource name.
    String      Name;

    /**
     * Constructor. Stores parameters. Data is lazily loaded with method #CheckLoad.
     * @param library      The library to load the string from.
     * @param resourceName The resource name.
     */
    ResourceString( Library& library, const String& resourceName )
    : Lib   ( library      )
    , Name  ( resourceName )
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



}} // namespace aworx[::lib::strings]

/// Type alias in namespace #aworx.
using ResourceString=    aworx::lib::strings::ResourceString;

} // namespace [aworx]


//! @cond NO_DOX
#if ALIB_MODULE_BOXING
// Specialize boxing of resource strings as character arrays
namespace aworx { namespace lib { namespace boxing {
ALIB_BOXING_SPECIALIZE_CB( aworx::ResourceString, char, true,     false )
inline void   T_Boxing<ResourceString>::Boxing( Box& box,  const ResourceString& value )
{
    T_Boxing<String>::Boxing( box, static_cast<const String&>( value ) );
}
}}}
#endif // HPP_ALIB_STRINGS_RESOURCESTRING
//! @endcond

#endif // HPP_ALIB_STRINGS_RESOURCESTRING

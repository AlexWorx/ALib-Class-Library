// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_LIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_LANG_LIB
//! @cond NO_DOX
#define HPP_ALIB_LANG_LIB 1
//! @endcond

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This namespace collects very basic utilities for \ref aworx::lib "ALib", close to the C++
 * language.
 *
 * \note
 *  - Within the source folder of this namespace, also file \c prepro.hpp is found, which sets up
 *    all basic \alib preprocessor definitions. Further preprocessor macros are defined in various
 *    places (where they belong to). An overview of all preprocessor symbols of this and other
 *    namespaces of \alib is given with \ref GrpALibMacros.
 *  - A list of <b>general enumerations</b> is found in this namespace.
 *    It is arguable and a matter of taste, whether enumerations should be declared near or inside
 *    the class which is using it, or in a separated place like this namespace introduced here.<br>
 *    \alib follows the second approach (not strictly, domain specific enums might appear also inside
 *    classes) and this has the following reasons:
 *    - To shorten the code when using the enums.
 *    - To reach a maximum degree of source compatibility between the different programming language
 *      versions of \alib.
 *    - To make the enums reusable with different classes.
 **************************************************************************************************/
namespace lang {

/** ************************************************************************************************
 * Exception codes of namespace #aworx::lib::lang.
 **************************************************************************************************/
enum class Exceptions
{
    /** Error when writing a report. This typically indicates an erroneous format string in an
     *  \ref ALIB_ASSERT_ERROR or related macro. */
    ErrorWritingReport= 1,
};

/** ************************************************************************************************
 * The library class for namespace \ref aworx::lib::lang.
 **************************************************************************************************/
class Lang  : public lang::Library
{
    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        inline  Lang()
        : Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_LANG") )
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes the \alib \c lang namespace.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources of the \alib \c lang namespace.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class lang::Library

} // namespace aworx::lib[::lang]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::lang.
 **************************************************************************************************/
ALIB_API
extern lang::Lang  LANG;

}} // namespace [aworx::lib]



#endif // HPP_ALIB_STRINGS_STRING_LIB

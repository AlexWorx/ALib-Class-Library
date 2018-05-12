// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// Include guard
#ifndef HPP_ALIB_STRINGS_STRING_LIB
#define HPP_ALIB_STRINGS_STRING_LIB 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif

namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * The library class for namespace \ref aworx::lib::strings.
 **************************************************************************************************/
class Strings  : public lang::Library
{
    protected:

        /**
         * A singleton instance (if this library is a singleton) which is received with
         * #GetDefaultFormatter.
         */
        SPFormatter                  defaultFormatter;

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructor. Provides \c "ALIB_STRINGS" for parameter \p{resourceCategory} of
         * base classes' constructor.
         */
        inline  Strings()
        : lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_STRINGS") )
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:
    #if ALIB_MODULE_BOXING
        /** ****************************************************************************************
         * Returns the default formatter object. Because this is returned embedded in a shared
         * pointer, it is safe to not be deleted during usage.
         *
         * Within method #init, an object of type \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle}
         * is created and placed in the shared pointer object.
         *
         * @return The default formatter.
         ******************************************************************************************/
        SPFormatter      GetDefaultFormatter()
        {
            return defaultFormatter;
        }

        /** ****************************************************************************************
         * Replaces the formatter currently defined as the default formatter.
         *
         * @param newFormatter The replacement formatter.
         ******************************************************************************************/
        ALIB_API
        void                ReplaceDefaultFormatter( Formatter* newFormatter );
    #endif

    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes the \alib namespace \ref aworx::lib::strings.
         *
         * Initializes the <b>%ALib %Boxing</b> system in respect to boxing types found in module
         * \ref aworx::lib::strings "ALib Strings".<br>
         * With the use of full \alib library, this method is invoked by
         * \ref aworx::lib::ALib::Init "ALib::Init". If <b>%ALib Strings</b> and <b>%ALib %Boxing</b> are used
         * as separated modules, then the user has to invoke this somewhere in the bootstrap code.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources of the \alib \c string namespace.
         * In current version, all it does is deleting the global formatter object.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class strings::Library

} // namespace aworx::lib[::strings]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::strings.
 **************************************************************************************************/
ALIB_API
extern strings::Strings STRINGS;

} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using     Strings=                 aworx::lib::strings::Strings;

} // namespace [aworx]


#endif // HPP_ALIB_STRINGS_STRING_LIB

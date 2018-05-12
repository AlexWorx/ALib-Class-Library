// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_UTIL_AUTOSIZES
#define HPP_ALIB_STRINGS_UTIL_AUTOSIZES 1

// to preserve the right order, we check if string is already included
#if !defined(HPP_ALIB_STRINGS_STRING)
    #error "include 'alib/alib.hpp' before including this header. Also make sure to include ALib Module Strings"
#endif



// #################################################################################################
// includes
// #################################################################################################
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace aworx { namespace lib { namespace strings { namespace util  {

/** ************************************************************************************************
 * Auto sizes are used for tabulator positions and field sizes that expand automatically when they
 * are exceeded. This way, any next line will have that new, expanded tabs and field sizes set
 * leading to a nicely formatted, but still flexible, minimized width of output.
 *
 * To preserve the information across <em>sessions</em>, this class provides methods to transform
 * it's information from and to string representations which can be stored in configuration files.
 *
 * All values are doubly stored: once for the session and once for the last imported values.
 * Those values that were imported are used for getting the actual size. However, those that are
 * stored for the session are the ones that would have been created if there was no import done.
 * This leads to the fact that the session values may be smaller than the actual (imported) ones.
 * This mechanism, in combination with the fact that on importing it can be decided if the session
 * is newly started or not, allows the values to shrink again over different sessions.
 **************************************************************************************************/
class AutoSizes
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// The current sizes
        std::vector<integer>           values;

        /** The sizes measured in this session. These are the ones that will be received in method
         *  #Export                                         */
        std::vector<integer>           sessionValues;

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**  The actual index requested by #Next. This is reset to 0 with every invocation of
         *   #Start. */
        int                             ActualIndex;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Resets the whole object. All values get deleted.
         ******************************************************************************************/
        inline
        void        Reset ()                      {   values.clear(); sessionValues.clear();  Start();  }

        /** ****************************************************************************************
         * Initializes a new query sequence, which is a series of invocations of method #Next.
         ******************************************************************************************/
        inline
        void        Start ()                      {   ActualIndex=   0; }

        /** ****************************************************************************************
         * Returns the actual auto value stored, respectively, if the given requested size is
         * higher, stores and returns the requested size. The given extra growth is added to the
         * requested size if the currently stored value is unequal to 0. In other words, the extra
         * size is added only with the second growth and each subsequent one.
         *
         * @param requestedSize   The minimum size that is requested.
         * @param growthPadding   Added to the new size if the requested size is greater than
         *                        the stored size and if the stored size does not equal \c 0.
         *
         * @return The (new) size of the auto field.
         ******************************************************************************************/
        ALIB_API
        int         Actual( integer requestedSize, integer growthPadding );

        /** ****************************************************************************************
         * Invokes #Actual and then increases the internal position counter.
         *
         * @param requestedSize   The minimum size that is requested.
         * @param growthPadding   Added to the new size if the requested size is greater than
         *                        the stored size and if the stored size does not equal \c 0.
         *
         * @return The (new) size of the auto field.
         ******************************************************************************************/
        ALIB_API
        int         Next  ( integer requestedSize, integer growthPadding );

        /** ****************************************************************************************
         * Imports values from the given
         * \alib{strings,StringBase,String} by parsing it. The numbers in the string have
         * to be separated by ' ' characters (space).
         *
         * @param source    The \b %String that is parsed for the numbers
         * @param session   If \c CurrentData::Clear, which is the default, the current values
         *                  are taken from the last session stored and the sessions data is set to 0.
         *                  If \c CurrentData::Keep, both, current values and
         *                  session values are taken from the string.
         ******************************************************************************************/
        ALIB_API
        void        Import( const String& source, lib::lang::CurrentData session= lib::lang::CurrentData::Clear );

        /** ****************************************************************************************
         * Exports the current session values by converting the stored numbers to a string
         * representation and appending them to the given
         * \alib{strings,AStringBase,AString} object.
         * The numbers in the string will be separated by ' ' characters (space).
         *
         * @param target       The \b %AString to receive the our values
         ******************************************************************************************/
        ALIB_API
        void        Export( AString& target );
};


}}} // namespace aworx[::lib::strings::util]


/// Type alias in namespace #aworx.
using     AutoSizes=            aworx::lib::strings::util::AutoSizes;

}  // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_AUTOSIZES

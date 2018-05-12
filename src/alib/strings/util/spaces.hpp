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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_UTIL_SPACES)
    #error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_STRINGS_UTIL_SPACES
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_UTIL_SPACES 1
//! @endcond

namespace aworx { namespace lib { namespace strings { namespace util  {


/** ************************************************************************************************
 * This is a simple class that provides a string full of spaces.
 * This string may be used to avoid repeated memory allocations/initializations and/or repeated
 * method invocations when a software interfaces to libraries that either allow to write
 * a string or a single character (i.e. <c>std::ostream</c>).
 **************************************************************************************************/
class Spaces
{
   public:
        /** ****************************************************************************************
         * Receives a narrow string full of spaces. Its length depends on previous calls and on
         * parameter \p minLength, which defaults to \c 128 and usually should \b not be
         * specified much higher.
         *
         * @param minLength The minimum length. Defaults to \c 128.
         * @return A static string filled with spaces.
         ******************************************************************************************/
        ALIB_API
        static NString&  GetN(integer minLength= 128);

        /** ****************************************************************************************
         * Receives a narrow string full of spaces. Its length depends on previous calls and on
         * parameter \p minLength, which defaults to \c 128 and usually should \b not be
         * specified much higher.
         *
         * @param minLength The minimum length. Defaults to \c 128.
         * @return A static string filled with spaces.
         ******************************************************************************************/
        ALIB_API
        static WString&  GetW(integer minLength= 128);

        /** ****************************************************************************************
         * Receives a narrow string full of spaces. Its length depends on previous calls and on
         * parameter \p minLength, which defaults to \c 128 and usually should \b not be
         * specified much higher.
         *
         * @param minLength The minimum length. Defaults to \c 128.
         * @return A static string filled with spaces.
         ******************************************************************************************/
        inline
        static String&  Get(integer minLength= 128)
        {
            #if ALIB_NARROW_STRINGS
                return Spaces::GetN( minLength );
            #else
                return Spaces::GetW( minLength );
            #endif
        }

        /** ****************************************************************************************
         * Write the given number of spaces to the narrow-character ostream.
         * @param os    The output stream to write to.
         * @param qty   The quantity of spaces to write.
         ******************************************************************************************/
        ALIB_API
        static void    Write( std::basic_ostream<char >& os, integer qty );

        /** ****************************************************************************************
         * Write the given number of spaces to the wide-character ostream.
         * @param os    The output stream to write to.
         * @param qty   The quantity of spaces to write.
         ******************************************************************************************/
        ALIB_API
        static void    Write( std::basic_ostream<wchar_t>& os, integer qty );
};

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     Spaces   =   aworx::lib::strings::util::Spaces;

} // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_SPACES

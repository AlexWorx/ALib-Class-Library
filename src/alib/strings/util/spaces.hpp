// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_UTIL_SPACES
#define HPP_ALIB_STRINGS_UTIL_SPACES 1

#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif


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
            #if ALIB_CHARACTERS_ARE_NARROW
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

        #if ALIB_CHARACTERS_NATIVE_WCHAR  || ALIB_DOCUMENTATION_PARSER
            /** ************************************************************************************
             * Write the given number of spaces to the ostream of type \c wchar_t.
             * \note This method is available only in the if code selection symbol
             *       \ref ALIB_CHARACTERS_NATIVE_WCHAR evaluates to \c true (\b 1). This is due to
             *       the fact that otherwise, the internal wide string buffer of spaces
             *       is either of type \c char16_t or \c char32_t, the one of both that is
             *       incompatible with output streams of character type \c wchar_t.<br>
             *       Consequently, this method must not be used (and replaced by a custom
             *       implementation) in the case that a software should compile successfully
             *       with one of the symbol
             *       \ref ALIB_CHARACTERS_FORCE_WIDE_2_BYTES_ON or
             *       \ref ALIB_CHARACTERS_FORCE_WIDE_4_BYTES_ON set.
             *
             * @param os    The output stream to write to.
             * @param qty   The quantity of spaces to write.
             **************************************************************************************/
            ALIB_API
            static void    Write( std::basic_ostream<wchar_t>& os, integer qty );
        #endif
};

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     Spaces   =   aworx::lib::strings::util::Spaces;

} // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_SPACES

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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER)
    #error "Header already included"
#endif


// then, set include guard
#ifndef HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER 1
//! @endcond

namespace aworx { namespace lib { namespace strings { namespace util  {


/** ************************************************************************************************
 * This utility class implements so called <em>wildcard string matching</em>. Wildcard characters
 * are
 * -  <c>'*'</c>: Matches zero or more characters.
 * -  <c>'?'</c>: Matches exactly one character.
 *
 * Method #Compile accepts the pattern string and translates it to an internal (simple) list of
 * "matching commands". This way, the class is optimized for performance, because after compilation,
 * subsequent invocations of #Match do not need to parse the pattern string.
 *
 * @tparam TChar    The character type. Implementations for \c char and \c wchar_t are provided
 *                  with type definitions \ref aworx::WildcardMatcherN and
 *                  \ref aworx::WildcardMatcherW.
 **************************************************************************************************/
template<typename TChar>
class WildcardMatcherBase
{
        /// The result list of commands created with #Compile and executed with #Match.
        std::vector<std::pair<int,StringBase<TChar>>>  commands;

    public:
        /** ****************************************************************************************
         * Constructs a WildcardMatcher to work on a given string. Passes the optional parameters
         * to method #Compile.
         *
         * @param  pattern      The string pattern to match.
         *                      Defaults to \b NullString to allow parameterless construction,
         *                      with later invocation of #Compile.
         ******************************************************************************************/
        inline
        WildcardMatcherBase( const StringBase<TChar>& pattern= NullString )
        {
            Compile( pattern );
        }

    public:
        /** ****************************************************************************************
         * Resets this object to use the given pattern.
         *
         * @param  pattern      The string pattern to match.
         ******************************************************************************************/
        ALIB_API
        void Compile( const StringBase<TChar>& pattern );

        /** ****************************************************************************************
         * Tests if given \p{haystack} matches the actual pattern.
         * If #Compile was not invoked or an empty pattern string was given, \c true is returned.
         *
         * @param haystack     The string to test.
         * @param sensitivity  Denotes whether the matching is performed case sensitive.
         *                     Defaults to \c %Case::Sensitive.
         * @return \c true if given \p{haystack} matches the actual pattern, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool   Match( const StringBase<TChar>& haystack, Case sensitivity = Case::Sensitive  );

}; // class WildcardMatcher


extern template ALIB_API void WildcardMatcherBase<char   >::Compile( const StringBase<nchar>& pattern );
extern template ALIB_API bool WildcardMatcherBase<char   >::Match  ( const StringBase<nchar>& haystack, Case sensitivity );
extern template ALIB_API void WildcardMatcherBase<wchar_t>::Compile( const StringBase<wchar>& pattern );
extern template ALIB_API bool WildcardMatcherBase<wchar_t>::Match  ( const StringBase<wchar>& haystack, Case sensitivity );

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     WildcardMatcherN=    aworx::lib::strings::util::WildcardMatcherBase<char   >;

/// Type alias in namespace #aworx.
using     WildcardMatcherW=    aworx::lib::strings::util::WildcardMatcherBase<wchar_t>;

/// Type alias in namespace #aworx.
using     WildcardMatcher=     aworx::lib::strings::util::WildcardMatcherBase<character>;

}  // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER

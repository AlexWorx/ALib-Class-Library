// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER
#define HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER 1

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

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
 * @tparam TChar    The character type. Implementations for \c nchar and \c wchar are provided
 *                  with type definitions \ref aworx::WildcardMatcherN and
 *                  \ref aworx::WildcardMatcherW.
 **************************************************************************************************/
template<typename TChar>
class TWildcardMatcher
{
        /** The result list of commands created with #Compile and executed with #Match. */
        std::vector<std::pair<int,TString<TChar>>>  commands;

    public:
        /** ****************************************************************************************
         * Constructs a WildcardMatcher to work on a given string. Passes the optional parameters
         * to method #Compile.
         *
         * @param  pattern      The string pattern to match.
         *                      Defaults to \b NullString() to allow parameterless construction,
         *                      with later invocation of #Compile.
         ******************************************************************************************/
        inline
        TWildcardMatcher( const TString<TChar>& pattern= NullString() )
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
        void Compile( const TString<TChar>& pattern );

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
        bool   Match( const TString<TChar>& haystack, Case sensitivity = Case::Sensitive  );

}; // class WildcardMatcher


extern template ALIB_API void TWildcardMatcher<nchar>::Compile( const TString<nchar>& );
extern template ALIB_API bool TWildcardMatcher<nchar>::Match  ( const TString<nchar>&, Case );
extern template ALIB_API void TWildcardMatcher<wchar>::Compile( const TString<wchar>& );
extern template ALIB_API bool TWildcardMatcher<wchar>::Match  ( const TString<wchar>&, Case );

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     WildcardMatcher=     aworx::lib::strings::util::TWildcardMatcher<character>;

/// Type alias in namespace #aworx.
using     WildcardMatcherN=    aworx::lib::strings::util::TWildcardMatcher<nchar>;

/// Type alias in namespace #aworx.
using     WildcardMatcherW=    aworx::lib::strings::util::TWildcardMatcher<wchar>;

}  // namespace [aworx]

#endif // HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER

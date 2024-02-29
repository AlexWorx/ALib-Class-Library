/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER
#define HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER 1

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
    #include <vector>
#endif

namespace alib {  namespace strings { namespace util  {


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
 *                  with type definitions \ref alib::WildcardMatcherN and
 *                  \ref alib::WildcardMatcherW.
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
        bool   Match( const TString<TChar>& haystack, lang::Case sensitivity = lang::Case::Sensitive  );

}; // class WildcardMatcher


extern template ALIB_API void TWildcardMatcher<nchar>::Compile( const TString<nchar>& );
extern template ALIB_API bool TWildcardMatcher<nchar>::Match  ( const TString<nchar>&, lang::Case );
extern template ALIB_API void TWildcardMatcher<wchar>::Compile( const TString<wchar>& );
extern template ALIB_API bool TWildcardMatcher<wchar>::Match  ( const TString<wchar>&, lang::Case );

}} // namespace alib[::strings::util]

/// Type alias in namespace \b alib.
using     WildcardMatcher=     strings::util::TWildcardMatcher<character>;

/// Type alias in namespace \b alib.
using     WildcardMatcherN=    strings::util::TWildcardMatcher<nchar>;

/// Type alias in namespace \b alib.
using     WildcardMatcherW=    strings::util::TWildcardMatcher<wchar>;

} // namespace [alib]

#endif // HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER

// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_UTIL_REGEXMATCHER
#define HPP_ALIB_STRINGS_UTIL_REGEXMATCHER 1

#if !defined (HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif


#if ALIB_FEAT_BOOST_REGEX
namespace aworx { namespace lib { namespace strings { namespace util  {

/** ************************************************************************************************
 * This utility class wraps [boost::regex library](http://www.boost.org) and interfaces
 * \alibmod_strings with it.
 *
 * The availability of the class is dependent on code selection symbol \ref ALIB_FEAT_BOOST_REGEX,
 * which is controlled by using compiler symbols #ALIB_FEAT_BOOST_REGEX_ON and
 * #ALIB_FEAT_BOOST_REGEX_OFF (the default).
 *
 * Method #Compile accepts the pattern string and compiles it to \b boost::regex.
 * Subsequent invocations of #Match will then use the compiled regular expression for testing
 * a given string.
 *
 * The syntax of the regular expressions is compatible to
 * [Perl Regular Expressions](http://perldoc.perl.org/perlretut.html).
 *
 * \note
 *   This a most very basic wrapper that supports a just the <b>bare minimum of the features</b>
 *   available in the original \b boost::regex library.
 *   I.e, the expression syntax is fixed to be \e Perl-compatible and no string replacement
 *   features or even match positioning are available.
 **************************************************************************************************/
class RegexMatcher
{
    protected:
        /**
         * This is the internal regex matcher. Yes, a nasty \c reinterpret_cast is performed in the
         * compilation unit. This is for avoiding the inclusion of external headers with \alib
         * headers at the expense of an otherwise unnecessary heap allocation.
         */
        void*           boostRegex                                                        = nullptr;

    public:
        /** ****************************************************************************************
         * Constructs a RegexMatcher to work on a given string. Passes the optional parameters
         * to method #Compile.
         *
         * @param  pattern      The string pattern to match.
         *                      Defaults to \b NullString() to allow parameterless construction,
         *                      with later invocation of #Compile.
         ******************************************************************************************/
        inline
        RegexMatcher( const String& pattern= NullString()  )
        {
            Compile( pattern );
        }

        /** ****************************************************************************************
         * Destructor.
         ******************************************************************************************/
        ALIB_API
        ~RegexMatcher();

    public:
        /** ****************************************************************************************
         * Resets this object to use the given pattern.
         *
         * @param  pattern      The string pattern to match.
         ******************************************************************************************/
        ALIB_API
        void Compile( const String& pattern );

        /** ****************************************************************************************
         * Tests if given \p{haystack} matches the actual pattern.
         * If #Compile was not invoked or an empty pattern string was given, \c true is returned.
         *
         * @param haystack  The string to test.
         * @return \c true if given \p{haystack} matches the actual pattern, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool   Match( const String& haystack );

}; // class RegexMatcher

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     RegexMatcher=     aworx::lib::strings::util::RegexMatcher;

}  // namespace [aworx]

#endif // ALIB_FEAT_BOOST_REGEX
#endif // HPP_ALIB_STRINGS_UTIL_REGEXMATCHER

//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/strings/strings.prepro.hpp"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Strings.Search;
#else
#   include "ALib.Strings.Search.H"
#endif
//========================================== Implementation ========================================
#if ALIB_FEAT_BOOST_REGEX && ALIB_CHARACTERS_WIDE && !ALIB_CHARACTERS_NATIVE_WCHAR
#   pragma message ( "Warning: Class RegexMatcher will not be available, because ALIB_CHARACTERS_NATIVE_WCHAR is false." )
#endif

#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

namespace alib {  namespace strings { namespace util  {

void RegexMatcher::Compile( const String& pattern ) {
    if( pattern.IsNull() )
        return;

    boostRegex.assign( std::basic_string<character>(pattern.Buffer(), size_t( pattern.Length() ) ),
                       boost::regex_constants::optimize );
}

bool RegexMatcher::Match( const String& haystack ) {
    ALIB_ASSERT_ERROR( !boostRegex.empty(), "ALIB/STR", "No regular expression compiled")
    return boost::regex_match(haystack.begin(), haystack.end(), boostRegex);
}

RegexMatcher::SRange RegexMatcher::SearchIn( const String& haystack ) {
    ALIB_ASSERT_ERROR( !boostRegex.empty(), "ALIB/STR", "No regular expression compiled")
    boost::match_results<String::const_iterator> what;
    if( boost::regex_search(haystack.begin(), haystack.end(), what, boostRegex ) )
        return { what.position(), what.length(0) };
    return {-1,-1};
}

}}} // namespace [alib::strings::util]

#endif  // ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

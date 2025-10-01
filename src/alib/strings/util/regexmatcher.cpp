// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
#   include <boost/regex.hpp>
#   include <string>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings.Search;
#else
#   include "ALib.Strings.Search.H"
#endif
// ======================================   Implementation   =======================================
#if ALIB_FEAT_BOOST_REGEX && ALIB_CHARACTERS_WIDE && !ALIB_CHARACTERS_NATIVE_WCHAR
#   pragma message ( "Warning: Class RegexMatcher will not be available, because ALIB_CHARACTERS_NATIVE_WCHAR is false." )
#endif

#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

namespace alib {  namespace strings { namespace util  {

RegexMatcher::~RegexMatcher()
{
    if( boostRegex )
        delete reinterpret_cast<boost::regex*>( boostRegex );
}

void RegexMatcher::Compile( const String& pattern )
{
    if( pattern.IsNull() )
        return;

    boost::basic_regex<character>* regEx;
    if( !boostRegex )
        boostRegex= (regEx= new  boost::basic_regex<character>() );
    else
        regEx= reinterpret_cast<boost::basic_regex<character>*>( boostRegex );

    regEx->assign( std::basic_string<character>(pattern.Buffer(), size_t( pattern.Length() ) ) );
}

bool RegexMatcher::Match( const String& haystack )
{
    if( !boostRegex )
        return true;

    return boost::regex_match(std::basic_string<character>(haystack.Buffer(), size_t( haystack.Length() )),
                              *reinterpret_cast<boost::basic_regex<character>*>( boostRegex )      );

}

}}} // namespace [alib::strings::util]

#endif  // ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)


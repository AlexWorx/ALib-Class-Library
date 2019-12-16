// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_UTIL_REGEXMATCHER)
#   include "alib/strings/util/regexmatcher.hpp"
#endif
#endif // !defined(ALIB_DOX)

#if ALIB_FEAT_BOOST_REGEX && ALIB_CHARACTERS_WIDE && !ALIB_CHARACTERS_NATIVE_WCHAR
#   pragma message ( "Warning: Class RegexMatcher will not be available, because ALIB_CHARACTERS_NATIVE_WCHAR is false." )
#endif


#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

#if !defined(ALIB_DOX)
#include <boost/regex.hpp>

#if !defined (_GLIBCXX_STRING) && !defined(_STRING_)
#   include <string>
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace strings { namespace util  {



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

    regEx->assign( std::basic_string<character>(pattern.Buffer(), static_cast<size_t>( pattern.Length() ) ) );
}

bool RegexMatcher::Match( const String& haystack )
{
    if( !boostRegex )
        return true;

    return boost::regex_match(std::basic_string<character>(haystack.Buffer(), static_cast<size_t>( haystack.Length() )),
                              *reinterpret_cast<boost::basic_regex<character>*>( boostRegex )      );

}

}}}} // namespace [aworx::lib::strings::util]

#endif  // ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

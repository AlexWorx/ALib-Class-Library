#if ALIB_FEAT_BOOST_REGEX

#if ALIB_CHARACTERS_WIDE && !ALIB_CHARACTERS_NATIVE_WCHAR
#   pragma message ( "Warning: Class RegexMatcher will not be available, because ALIB_CHARACTERS_NATIVE_WCHAR is false." )
#else

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

#endif  // (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
#endif  // ALIB_FEAT_BOOST_REGEX

//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if (ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR) ) || DOXYGEN

ALIB_EXPORT namespace alib {  namespace strings { namespace util  {

//==================================================================================================
/// This utility class wraps \https{boost::regex library,www.boost.org} and interfaces
/// \alib_strings with it.
///
/// The availability of the class is dependent on the compiler-symbol \ref ALIB_FEAT_BOOST_REGEX
/// which has to be \c true, and in parallel either \ref ALIB_CHARACTERS_WIDE is \c false or
/// \ref ALIB_CHARACTERS_NATIVE_WCHAR equals \c true.
///
/// Method #Compile accepts the pattern string and compiles it to \b boost::regex.
/// Subsequent invocations of #Match will then use the compiled regular expression for testing
/// a given string.
///
/// The syntax of the regular expressions is compatible to
/// \https{Perl Regular Expressions,perldoc.perl.org/perlretut.html}.
///
/// \note
///   This is a very basic wrapper that supports just the <b>bare minimum of the features</b>
///   available in the original \b boost::regex library.
///   I.e, the expression syntax is fixed to be \e Perl-compatible and no string replacement
///   features or even match positioning are available.
//==================================================================================================
class RegexMatcher
{
  protected:
    /// This is the internal regex matcher.
    boost::basic_regex<character, boost::regex_traits<character> >    boostRegex;

  public:
    /// A simple struct that determines a range in a string.
    /// A range is not bound to a certain string object and its validty has to be checked
    /// when used.
    struct SRange {
        integer     Position;  ///< The starting index of this range in a string.
        integer     Length;    ///< The length of this range in a string.
    };

    /// Constructs a RegexMatcher to work on a given string.
    /// Passes the optional parameters to method #Compile.
    /// @param pattern The string pattern to match.
    ///                Defaults to \b NULL_STRING to allow parameterless construction, with
    ///                later invocation of #Compile.
    RegexMatcher( const String& pattern= NULL_STRING  )                      { Compile( pattern ); }

    /// Resets this object to use the given pattern.
    /// @param  pattern      The string pattern to match.
    ALIB_DLL
    void        Compile( const String& pattern );

    /// Tests if the given \p{haystack} matches the actual pattern.
    /// If #Compile was not invoked or an empty pattern string was given, \c true is returned.
    ///
    /// @param haystack  The string to test.
    /// @return \c true if given \p{haystack} matches the actual pattern, \c false otherwise.
    ALIB_DLL
    bool        Match( const String& haystack );

    /// Searches for the first match of the actual pattern in the given \p{haystack}
    /// If #Compile was not invoked or an empty pattern string was given, an
    /// \alib_assertion is raised.
    /// @param haystack The string to search in..
    /// @return The (first) range in \b haystack that matches the compiled pattern.
    ///         In case the pattern was not found, <c>{-1,-1}</c> is returned.
    ALIB_DLL
    SRange      SearchIn( const String& haystack );
}; // class RegexMatcher

}} // namespace alib[::strings::util]

/// Type alias in namespace \b alib.
using     RegexMatcher=     strings::util::RegexMatcher;

} // namespace [alib]

#endif // ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)

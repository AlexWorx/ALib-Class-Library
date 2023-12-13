/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_characters of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CHARACTERS_CHARARRAY
#define HPP_ALIB_CHARACTERS_CHARARRAY 1


#if !defined (HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif

#if !defined (HPP_ALIB_FS_COMMONENUMS_DEFS)
#   include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#endif

#if !defined (_GLIBCXX_NUMERIC_LIMITS) && !defined(_LIMITS_)
#   include <limits>
#endif
#if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
#   include <cstring>
#endif

#if !defined (_GLIBCXX_CCTYPE) && !defined(_CCTYPE_)
#   include <cctype>
#endif

#if !defined (_GLIBCXX_CWCTYPE) && !defined(_CWCTYPE_)
#   include <cwctype>
#endif

#if !defined (_GLIBCXX_CWCHAR) && !defined(_CWCHAR_)
#   include <cwchar>
#endif

#if !defined (_GLIBCXX_STRING) && !defined(_STRING_)
#   include <string>
#endif
namespace aworx { namespace lib { namespace characters {


/** ************************************************************************************************
 * This utility struct provides static methods working with character arrays of different
 * underlying type.<br>
 * The struct is similar to type traits struct \c std::char_traits of the C++ standard library
 * and for methods that are existing there already, just inline forwarding methods are defined here.
 **************************************************************************************************/
template<typename TChar>
struct CharArray
{
    /** ********************************************************************************************
     * Compares two characters of arbitrary types.
     *
     * @tparam sensitivity Letter case sensitivity of the comparison.
     * @tparam TRhs        The type of the right hand side letter to compare.
     * @param  lhs         The left-hand side character to compare of class template
     *                     type \p{TChar}.
     * @param  rhs         The right-hand side character to compare of method template
     *                     type \p{TCharRhs} .
     * @return \c true if the given characters are equal, \c false otherwise.
     **********************************************************************************************/
    template<Case sensitivity, typename TRhs >
    static
    bool Equal( TChar lhs, TRhs rhs )
    {
        ALIB_WARNINGS_IGNORE_IF_CONSTEXPR
        using TLhs= TChar;
        bool sensitive=  (sensitivity == Case::Sensitive);

             if ALIB_CONSTEXPR17 ( sizeof(TLhs) == sizeof(TRhs) )
            return  sensitive ?                             lhs  ==                            rhs
                              :  ToUpper(                   lhs) ==  ToUpper(                  rhs);

        else if ALIB_CONSTEXPR17 ( sizeof(TLhs) < sizeof(TRhs) )
            return  sensitive ?           static_cast<TRhs>(lhs)  ==                           rhs
                              :  ToUpper( static_cast<TRhs>(lhs)) == ToUpper(                  rhs);

        else if ALIB_CONSTEXPR17 ( sizeof(TLhs) > sizeof(TRhs) )
            return  sensitive ?                             lhs   ==         static_cast<TLhs>(rhs)
                              :  ToUpper(                   lhs ) == ToUpper(static_cast<TLhs>(rhs));
        ALIB_WARNINGS_RESTORE
    }

    /** ********************************************************************************************
     * Returns the length of a zero-terminated "c-style" character array.
     *
     * Note: This method is implemented as an inlined, direct direct call to
     * \c std::char_traits::length.
     *
     * @param cstring         Pointer to zero-terminated character array.
     *
     * @return The length of the string.
     **********************************************************************************************/
    static
    integer     Length( const TChar* cstring )
    {
        return static_cast<integer>( std::char_traits<TChar>::length(cstring) );
    }

    /** ********************************************************************************************
     * Copies the contents of a character array into another, non-overlapping (!) array.
     *
     * Note: This method is implemented as an inlined, direct direct call to
     * \c std::char_traits::copy.
     *
     * @param src     Pointer to the source array.
     * @param length  The length to copy.
     * @param dest    Pointer to the destination array.
     **********************************************************************************************/
    static
    void        Copy( const TChar* src, integer length, TChar* dest )
    {
        std::char_traits<TChar>::copy( dest, src, static_cast<size_t>(length) );
    }

    /** ********************************************************************************************
     * Copies the contents of a character array into another, possibly overlapping array.
     *
     * Note: This method is implemented as an inlined, direct direct call to
     * \c std::char_traits::move.
     *
     * @param src     Pointer to the source array.
     * @param length  The length to copy.
     * @param dest    Pointer to the destination array, optionally within source.
     **********************************************************************************************/
    static
    void        Move( const TChar* src, integer length, TChar* dest )
    {
        std::char_traits<TChar>::move( dest, src, static_cast<size_t>(length) );
    }

    /** ********************************************************************************************
     * Sets all elements of the given character array to value \p{value}.
     *
     * @param dest      Pointer to the destination array.
     * @param length    The length to fill.
     * @param value     The value to fill the array with.
     **********************************************************************************************/
    static
    void        Fill( TChar* dest, integer length, TChar value );

    /** ********************************************************************************************
     * Converts a character to upper case.
     *
     * @param c    The character to convert
     * @return The upper case version of the given character.
     **********************************************************************************************/
    static
    TChar       ToUpper( TChar c );

    /** ********************************************************************************************
     * Converts a character sequence to upper case.
     *
     * @param src     Pointer to the character array.
     * @param length  The length of the array.
     **********************************************************************************************/
    static
    void        ToUpper( TChar* src, integer length );

    /** ********************************************************************************************
     * Converts a character to lower case.
     *
     * @param c    The character to convert
     * @return The lower case version of the given character.
     **********************************************************************************************/
    static
    TChar       ToLower( TChar c );

    /** ********************************************************************************************
     * Converts a character sequence to lower case.
     *
     * @param src     Pointer to the character array.
     * @param length  The length of the array.
     **********************************************************************************************/
    static
    void        ToLower( TChar* src, integer length );

    /** ********************************************************************************************
     * Reverses the order of the characters.
     *
     * @param src     Pointer to the character array.
     * @param length  The length of the array.
     **********************************************************************************************/
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    static
    void        Reverse( TChar* src, integer length );
    ALIB_WARNINGS_RESTORE

    /** ********************************************************************************************
     * Searches the character. Returns a pointer to the location of \p{needle} in \p{haystack},
     * respectively \c nullptr if not found.
     *
     * Note: This method is implemented as a direct call to \c std::memchr, respectively
     *       other character versions of it.
     *
     * @param haystack        Pointer to the start of the string.
     * @param haystackLength  The length of the string or the maximum position to search.
     * @param needle          Character to search.
     *
     * @return The pointer to the first occurrence of \p{needle} respectively \c nullptr if
     *         not found.
     **********************************************************************************************/
    static
    const TChar* Search( const TChar* haystack, integer haystackLength, TChar needle )
    {
        return std::char_traits<TChar>::find( haystack, static_cast<size_t>(haystackLength), needle );
    }

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    /** ********************************************************************************************
     * Returns the index of the first character in \p{haystack} which is included in a given set
     * of \p{needles}.
     *
     * This method searches up to a given maximum index. For a search to the end of the
     * zero terminated string, use faster method provided with class
     * \alib{strings,TCString,CString}.

     * @param haystack        Pointer to the start of the string.
     * @param haystackLength  The length of the string or the maximum position to search.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs haystack to be zero terminated).
     * @param needles         Pointer to a set of characters to be searched for.
     * @param needlesLength   The length of the string of needles.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs needles to be zero terminated).
     *
     * @return The index of the first character found that is included in \p{needles}.
     *         If no character of haystack is included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static
    integer IndexOfAnyIncluded( const TChar*  haystack,   integer  haystackLength,
                                const TChar*  needles,    integer  needlesLength     );

    /** ********************************************************************************************
     * Same as #IndexOfAnyIncluded(const TChar*,integer,const TChar*,integer) but works on
     * zero-terminated strings.
     *
     * @param haystack Pointer to zero-terminated character array to search in.
     * @param needles  Pointer to zero-terminated set of characters to search for.
     *
     * @return The index of the first character found that is included in \p{needles}.
     *         If no character of haystack is included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static
    integer IndexOfAnyIncludedZT( const TChar* haystack, const TChar* needles );

    /** ********************************************************************************************
     * Returns the index of the first character in \p{haystack} which is not included in a given
     * set of \p{needles}.
     *
     * This method searches up to a given maximum index. For a search to the end of the
     * zero terminated string, use faster method provided with class
     * \alib{strings,TCString,CString}.

     * @param haystack        Pointer to the start of the string.
     * @param haystackLength  The length of the string or the maximum position to search.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs haystack to be zero terminated).
     * @param needles         Pointer to a set of characters to be searched for.
     * @param needlesLength   The length of the string of needles.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs needles to be zero terminated).
     *
     * @return The index of the first character that is not included in \p{needles}.
     *         If all characters of haystack are included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static
    integer IndexOfAnyExcluded( const TChar*  haystack,   integer  haystackLength,
                                const TChar*  needles,    integer  needlesLength     );


    /** ********************************************************************************************
     * Same as #IndexOfAnyExcluded(const TChar*,integer,const TChar*,integer) but works on
     * zero-terminated strings.
     *
     * @param haystack Pointer to zero-terminated character array to search in.
     * @param needles  Pointer to zero-terminated set of characters to search for.
     *
     * @return The index of the first character that is not included in \p{needles}.
     *         If all characters of haystack are included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static
    integer IndexOfAnyExcludedZT( const TChar* haystack, const TChar* needles );

    /** ********************************************************************************************
     * Returns the index of the last character in \p{haystack} which is included in a given set
     * of \p{needles}.
     *
     * This method searches backwards from the end of the string.
     *
     * @param haystack        Pointer to the start of the string.
     * @param startIdx        The position to start the search from. This must be smaller than
     *                        the length of the string and greater or equal to  zero.
     * @param needles         Pointer to a set of characters to be searched for.
     * @param needlesLength   The length of the string of needles.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs needles to be zero terminated).
     *
     * @return The index of the first character found which is included in the given set
     *         of characters. If nothing is found, -1 is returned.
     **********************************************************************************************/
    static
    integer LastIndexOfAnyInclude( const TChar*  haystack,    integer  startIdx,
                                   const TChar*  needles,     integer  needlesLength );

    /** ********************************************************************************************
     * Returns the index of the last character in \p{haystack} which is not included in a given
     * set of \p{needles}.
     *
     * This method searches backwards from the end of the string.
     *
     * @param haystack        Pointer to the start of the string.
     * @param startIdx        The position to start the search from. This must be smaller than
     *                        the length of the string and greater or equal to  zero.
     * @param needles         Pointer to a set of characters to be searched for.
     * @param needlesLength   The length of the string of needles.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs needles to be zero terminated).
     *
     * @return The index of the first character found which is included in the given set
     *         of characters. If nothing is found, -1 is returned.
     **********************************************************************************************/
    static
    integer LastIndexOfAnyExclude( const TChar*  haystack,    integer  startIdx,
                                   const TChar*  needles,     integer  needlesLength );

    /** ********************************************************************************************
     * Returns the index of the first character which is not equal within two strings.
     * If \p{haystack} starts with \p{needle}, then the length of \p{needle} is returned.
     *
     * @param haystack        Pointer to the start of the string.
     * @param haystackLength  The length of the string or the maximum position to search.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs haystack to be zero terminated).
     * @param needle          Pointer to the start of the string to compare with.
     * @param needleLength    The length of \p{needle}.
     *                        If -1 is provided, the length is determined using % C function
     *                        strlen (which needs needles to be zero terminated).
     * @param sensitivity     Denotes whether the comparison should be made case sensitive
     *                        or not.
     *
     * @return The index of the first character found which is not equal in given strings.
     **********************************************************************************************/
    static
    integer IndexOfFirstDifference( const TChar*  haystack,   integer  haystackLength,
                                    const TChar*  needle,     integer  needleLength,
                                    Case    sensitivity                         );
    ALIB_WARNINGS_RESTORE

    /** ********************************************************************************************
     * Searches for a difference in two character arrays of equal length.
     *
     * @param lhs       The first array to compare.
     * @param rhs       The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  \c true if the string arrays have identical contents, \c false otherwise.
     **********************************************************************************************/
    static
    bool Equal( const TChar* lhs,  const TChar* rhs, integer cmpLength  )
    {
        return  ::memcmp( lhs, rhs, static_cast<size_t>(cmpLength) * sizeof(TChar) ) == 0;
    }


    /** ********************************************************************************************
     * Compares up to \p{cmpLength} characters of two character arrays.
     * Comparison stops if termination character \c '\0' is found in one of the arrays.
     *
     * @param lhs       The first array to compare.
     * @param rhs       The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  Negative value if lhs appears before rhs in lexicographical order.
     *          \c 0 if lhs and rhs are equal.
     *          Positive value if lhs appears after rhs in lexicographical order.
     **********************************************************************************************/
    static
    int Compare( const TChar* lhs,  const TChar* rhs, integer cmpLength  )
    {
        return std::char_traits<TChar>::compare( lhs, rhs, static_cast<size_t>(cmpLength) );
    }

    /** ********************************************************************************************
     * Compares two character arrays of equal length ignoring letter case.
     *
     * @param lhs      The first array to compare.
     * @param rhs      The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  Negative value if lhs appears before rhs in lexicographical order.
     *          \c 0 if lhs and rhs are equal.
     *          Positive value if lhs appears after rhs in lexicographical order.
     **********************************************************************************************/
    static
    int CompareIgnoreCase( const TChar* lhs,  const TChar* rhs, integer cmpLength  );

}; //  struct CharArray

//! @cond NO_DOX

// #################################################################################################
// Narrow character specifics
// #################################################################################################
template<> inline void    CharArray<nchar>::Fill( nchar* dest, integer length, nchar c )
{
    memset( dest, c, static_cast<size_t>(length) );
}

template<> inline nchar   CharArray<nchar>::ToUpper(nchar c)
{
    return static_cast<nchar>( toupper(c) );
}

template<> inline nchar   CharArray<nchar>::ToLower(nchar c)
{
    return static_cast<nchar>( tolower(c) );
}

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
template<> inline void    CharArray<nchar>::ToUpper(nchar* src, integer length)
{
    nchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<nchar>::ToUpper( *src );
        ++src;
    }
}

template<> inline void    CharArray<nchar>::ToLower(nchar* src, integer length)
{
    nchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<nchar>::ToLower( *src );
        ++src;
    }
}
ALIB_WARNINGS_RESTORE

template<> inline int     CharArray<nchar>::CompareIgnoreCase( const nchar* lhs,  const nchar* rhs, integer cmpLength  )
{
    #if defined (__GLIBCXX__)  || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return  ::strncasecmp( lhs, rhs, static_cast<size_t>(cmpLength) );
    #elif defined ( _WIN32 )
        return  _strnicmp  ( lhs, rhs, static_cast<size_t>(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer CharArray<nchar>::IndexOfAnyIncludedZT   ( const nchar* haystack, const nchar* needles )
{
    const nchar* result= std::strpbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}

template<> inline integer CharArray<nchar>::IndexOfAnyExcludedZT( const nchar* haystack, const nchar* needles )
{
    return static_cast<integer>( std::strspn(haystack, needles) );
}



extern template ALIB_API integer CharArray<nchar>::IndexOfAnyIncluded    (const nchar*,integer,const nchar*,integer);
extern template ALIB_API integer CharArray<nchar>::IndexOfAnyExcluded    (const nchar*,integer,const nchar*,integer);
extern template ALIB_API integer CharArray<nchar>::LastIndexOfAnyInclude (const nchar*,integer,const nchar*,integer);
extern template ALIB_API integer CharArray<nchar>::LastIndexOfAnyExclude (const nchar*,integer,const nchar*,integer);
extern template ALIB_API integer CharArray<nchar>::IndexOfFirstDifference(const nchar*,integer,const nchar*,integer,Case);
extern template ALIB_API void    CharArray<nchar>::Reverse               (      nchar*,integer );


// #################################################################################################
// Wide character specifics
// #################################################################################################
template<> inline wchar   CharArray<wchar>::ToUpper(wchar c)
{
    return static_cast<wchar>(towupper(static_cast<wint_t>(c)));
}

template<> inline wchar   CharArray<wchar>::ToLower(wchar c)
{
    return static_cast<wchar>(towlower(static_cast<wint_t>(c)));
}

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
template<> inline void    CharArray<wchar>::ToUpper(wchar* src, integer length)
{
    wchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<wchar>::ToUpper( *src );
        ++src;
    }
}

template<> inline void    CharArray<wchar>::ToLower(wchar* src, integer length)
{
    wchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<wchar>::ToLower( *src );
        ++src;
    }
}
ALIB_WARNINGS_RESTORE

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> inline void    CharArray<wchar>::Fill( wchar* dest, integer length, wchar c )
{
    wmemset( dest, c, static_cast<size_t>(length) );
}

template<> inline int     CharArray<wchar>::CompareIgnoreCase( const wchar* lhs,  const wchar* rhs, integer cmpLength  )
{
    #if defined ( _WIN32 )
        return  _wcsnicmp  ( lhs, rhs, static_cast<size_t>(cmpLength)  );
    #elif defined (__GLIBCXX__)  || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return  ::wcsncasecmp( lhs, rhs, static_cast<size_t>(cmpLength) );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer CharArray<wchar>::IndexOfAnyIncludedZT   ( const wchar* haystack, const wchar* needles )
{
    const wchar* result= std::wcspbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}
template<> inline integer CharArray<wchar>::IndexOfAnyExcludedZT( const wchar* haystack, const wchar* needles )
{
    return static_cast<integer>( std::wcsspn(haystack, needles) );
}


#else
template<> ALIB_API void    CharArray<wchar>::Fill( wchar* dest, integer length, wchar c );
template<> ALIB_API int     CharArray<wchar>::CompareIgnoreCase   ( const wchar* lhs,      const wchar* rhs, integer cmpLength );
template<> ALIB_API integer CharArray<wchar>::IndexOfAnyIncludedZT( const wchar* haystack, const wchar* needles );
template<> ALIB_API integer CharArray<wchar>::IndexOfAnyExcludedZT( const wchar* haystack, const wchar* needles );
#endif


extern template ALIB_API integer CharArray<wchar>::IndexOfAnyIncluded    (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CharArray<wchar>::IndexOfAnyExcluded    (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CharArray<wchar>::LastIndexOfAnyInclude (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CharArray<wchar>::LastIndexOfAnyExclude (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CharArray<wchar>::IndexOfFirstDifference(const wchar*,integer,const wchar*,integer,Case);
extern template ALIB_API void    CharArray<wchar>::Reverse               (      wchar*,integer );



// #################################################################################################
// Strange character specifics
// #################################################################################################
template<> inline xchar     CharArray<xchar>::ToUpper(xchar c)
{
    return static_cast<xchar>(towupper(static_cast<wint_t>(c)));
}

template<> inline xchar     CharArray<xchar>::ToLower(xchar c)
{
    return static_cast<xchar>(towlower(static_cast<wint_t>(c)));
}

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
template<> inline void    CharArray<xchar>::ToUpper(xchar* src, integer length)
{
    xchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<xchar>::ToUpper( *src );
        ++src;
    }
}

template<> inline void    CharArray<xchar>::ToLower(xchar* src, integer length)
{
    xchar* end= src  + length;
    while( src != end )
    {
        *src=  CharArray<xchar>::ToLower( *src );
        ++src;
    }
}
ALIB_WARNINGS_RESTORE

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> ALIB_API void    CharArray<xchar>::Fill( xchar* dest, integer length, xchar c );
template<> ALIB_API int     CharArray<xchar>::CompareIgnoreCase   ( const xchar* lhs,  const xchar* rhs, integer cmpLength  );
template<> ALIB_API integer CharArray<xchar>::IndexOfAnyIncludedZT( const xchar* haystack, const xchar* needles );
template<> ALIB_API integer CharArray<xchar>::IndexOfAnyExcludedZT( const xchar* haystack, const xchar* needles );
#else
template<> inline void    CharArray<xchar>::Fill( xchar* dest, integer length, xchar c )
{
    wmemset( dest, c, static_cast<size_t>(length) );
}

template<> inline int     CharArray<xchar>::CompareIgnoreCase( const xchar* lhs,  const xchar* rhs, integer cmpLength  )
{
    #if defined (__GLIBCXX__)  || defined(__APPLE__)
        return  ::wcsncasecmp( lhs, rhs, static_cast<size_t>(cmpLength) );
    #elif defined ( _WIN32 )
        return  _wcsnicmp  ( lhs, rhs, static_cast<size_t>(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer CharArray<xchar>::IndexOfAnyIncludedZT   ( const xchar* haystack, const xchar* needles )
{
    const xchar* result= std::wcspbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}
template<> inline integer CharArray<xchar>::IndexOfAnyExcludedZT( const xchar* haystack, const xchar* needles )
{
    return static_cast<integer>( std::wcsspn(haystack, needles) );
}

#endif


extern template ALIB_API integer CharArray<xchar>::IndexOfAnyIncluded    (const xchar*,integer,const xchar*,integer);
extern template ALIB_API integer CharArray<xchar>::IndexOfAnyExcluded    (const xchar*,integer,const xchar*,integer);
extern template ALIB_API integer CharArray<xchar>::LastIndexOfAnyInclude (const xchar*,integer,const xchar*,integer);
extern template ALIB_API integer CharArray<xchar>::LastIndexOfAnyExclude (const xchar*,integer,const xchar*,integer);
extern template ALIB_API integer CharArray<xchar>::IndexOfFirstDifference(const xchar*,integer,const xchar*,integer,Case);
extern template ALIB_API void    CharArray<xchar>::Reverse               (      xchar*,integer );



//! @endcond


}}} // namespace [aworx::lib::character]


#endif // HPP_ALIB_CHARACTERS_CHARARRAY

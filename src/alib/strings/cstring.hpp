// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include guard
#ifndef HPP_ALIB_STRINGS_CSTRING
#define HPP_ALIB_STRINGS_CSTRING 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif

#if !defined (_GLIBCXX_NUMERIC_LIMITS) && !defined(_LIMITS_)
#   include <limits>
#endif
#if !defined (_GLIBCXX_CSTRING) && !defined(_CSTRING_)
#   include <cstring>
#endif
#if !defined (_GLIBCXX_CWCTYPE) && !defined(_CWCTYPE_)
#   include <cwctype>
#endif



namespace aworx { namespace lib { namespace strings {

/** ************************************************************************************************
 * This utility struct provides static methods working with zero-terminated character
 * arrays of different underlying type.
 **************************************************************************************************/
template<typename TChar>
struct CString
{
    /** ********************************************************************************************
     * Returns the length of a zero-terminated "c-style" character array.
     *
     * Note: This method is implemented as a direct call to \c std::strlen, respectively
     *       \c std::wcslen.
     *
     * @param cstring         Pointer to zero-terminated character array.
     *
     * @return The length of the string.
     **********************************************************************************************/
    static ALIB_API
    integer     Length( const TChar* cstring );

    /** ********************************************************************************************
     * Copies the contents of a character array into another, non-overlapping (!) array.
     *
     * Note: This method is implemented as a direct call to \c std::memcpy, respectively
     *       \c std::wmemcpy.
     *
     * @param src     Pointer to the source buffer.
     * @param length  The length to copy.
     * @param dest    Pointer to the destination buffer.
     **********************************************************************************************/
    static ALIB_API
    void        Copy( const TChar* src, integer length, TChar* dest );

    /** ********************************************************************************************
     * Copies the contents of a character array into another, possibly overlapping array.
     *
     * Note: This method is implemented as a direct call to \c std::memmove, respectively
     *       \c std::wmemmove.
     *
     * @param src     Pointer to the source buffer.
     * @param length  The length to copy.
     * @param dest    Pointer to the destination buffer, optionally within source.
     **********************************************************************************************/
    static ALIB_API
    void        Move( const TChar* src, integer length, TChar* dest );

    /** ********************************************************************************************
     * Copies the contents of a character array into another, possibly overlapping array.
     *
     * Note: This method is implemented as a direct call to \c std::memset, respectively
     *       \c std::wmemset.
     *
     * @param dest Pointer to the destination buffer.
     * @param c    The value to fill the buffer with.
     * @param qty  The quantity of characters to write.
     **********************************************************************************************/
    static ALIB_API
    void        Fill( TChar* dest, TChar c, integer qty  );

    /** ********************************************************************************************
     * Converts a character to upper case.
     *
     * @param c    The character to convert
     * @return The upper case version of the given character.
     **********************************************************************************************/
    static ALIB_API
    TChar       Toupper( TChar c );

    /** ********************************************************************************************
     * Converts a character to lower case.
     *
     * @param c    The character to convert
     * @return The lower case version of the given character.
     **********************************************************************************************/
    static ALIB_API
    TChar       Tolower( TChar c );

    /** ********************************************************************************************
     * Searches the character. Returns a pointer to the location in buffer, respectively
     * \c nullptr if not found.
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
    static ALIB_API
    const TChar* Search( const TChar* haystack, integer haystackLength, TChar needle );

    /** ********************************************************************************************
     * Returns the index of the first character which is included in a given set of characters.
     *
     * This method searches up to a given maximum index. For a search to the end of the
     * zero terminated string, use faster method provided with class
     * \alib{strings,TStringBase,TString}.

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
    static ALIB_API
    integer IndexOfAnyIncluded( const TChar*  haystack,   integer  haystackLength,
                                const TChar*  needles,    integer  needlesLength     );

    /** ********************************************************************************************
     * Same as #IndexOfAnyIncluded(const TChar*,integer,const TChar*,integer) but works on
     * zero-terminated strings.
     *
     * Note: This method is implemented as an inline call to \c std::strpbrk, respectively
     *       - if available - its wide character counterpart.
     *
     * @param haystack Pointer to zero-terminated character array to search in.
     * @param needles  Pointer to zero-terminated set of characters to search for.
     *
     * @return The index of the first character found that is included in \p{needles}.
     *         If no character of haystack is included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static ALIB_API
    integer IndexOfAnyIncludedZT( const TChar* haystack, const TChar* needles );

    /** ********************************************************************************************
     * Returns the index of the first character which is not included in a given set of
     * characters.
     *
     * This method searches up to a given maximum index. For a search to the end of the
     * zero terminated string, use faster method provided with class
     * \alib{strings,TStringBase,TString}.

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
    static ALIB_API
    integer IndexOfAnyExcluded( const TChar*  haystack,   integer  haystackLength,
                                const TChar*  needles,    integer  needlesLength     );


    /** ********************************************************************************************
     * Same as #IndexOfAnyExcluded(const TChar*,integer,const TChar*,integer) but works on
     * zero-terminated strings.
     *
     * Note: This method is implemented as an inline call to \c std::strspn, respectively
     *       - if available - its wide character counterpart.
     *
     * @param haystack Pointer to zero-terminated character array to search in.
     * @param needles  Pointer to zero-terminated set of characters to search for.
     *
     * @return The index of the first character that is not included in \p{needles}.
     *         If all characters of haystack are included in \p{needles}, \c -1 is returned.
     **********************************************************************************************/
    static ALIB_API
    integer IndexOfAnyExcludedZT( const TChar* haystack, const TChar* needles );

    /** ********************************************************************************************
     * Returns the index of the last character which is included in a given set of characters.
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
    static ALIB_API
    integer LastIndexOfAnyInclude( const TChar*  haystack,    integer  startIdx,
                                   const TChar*  needles,     integer  needlesLength );

    /** ********************************************************************************************
     * Returns the index of the last character which is not included in a given set of
     * characters.
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
    static ALIB_API
    integer LastIndexOfAnyExclude( const TChar*  haystack,    integer  startIdx,
                                   const TChar*  needles,     integer  needlesLength );

    /** ********************************************************************************************
     * Returns the index of the first character which not equal
     * within two strings.
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
    static ALIB_API
    integer IndexOfFirstDifference( const TChar*  haystack,   integer  haystackLength,
                                    const TChar*  needle,     integer  needleLength,
                                    lang::Case    sensitivity                         );


    /** ********************************************************************************************
     * Searches for a difference in two character arrays of equal length.
     *
     * This method usually is implemented using \c std::memcmp, respctively  \c std::wmemcmp.
     *
     * @param str1      The first array to compare.
     * @param str2      The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  \c true if the string arrays have identical contents, \c false otherwise.
     **********************************************************************************************/
    static ALIB_API
    bool Equal( const TChar* str1,  const TChar* str2, integer cmpLength  );


    /** ********************************************************************************************
     * Compares up to \p{cmpLength} characters of two character arrays.
     * Comparison stops if termination character \c '\0' is found in one of the arrays.
     *
     * This method usually is implemented using \c std::strncmp, respctively  \c std::wcsncmp.
     *
     * @param str1      The first array to compare.
     * @param str2      The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  Negative value if str1 appears before str2 in lexicographical order.
     *          \c 0 if str1 and str2 are equal.
     *          Positive value if str1 appears after str2 in lexicographical order.
     **********************************************************************************************/
    static ALIB_API
    int Compare( const TChar* str1,  const TChar* str2, integer cmpLength  );

    /** ********************************************************************************************
     * Compares two character arrays of equal length ignoring letter case.
     *
     * @param str1      The first array to compare.
     * @param str2      The second array to compare.
     * @param cmpLength The number of characters to compare.
     *
     * @return  Negative value if str1 appears before str2 in lexicographical order.
     *          \c 0 if str1 and str2 are equal.
     *          Positive value if str1 appears after str2 in lexicographical order.
     **********************************************************************************************/
    static ALIB_API
    int CompareIgnoreCase( const TChar* str1,  const TChar* str2, integer cmpLength  );

}; //  struct CString

//! @cond NO_DOX

// #################################################################################################
// NString specifics
// #################################################################################################

template<> inline integer CString<char>::Length( const char*  cstring )
{
    return static_cast<integer>( ::strlen( cstring ) );
}

template<> inline void    CString<char>::Copy( const char* src, integer length, char* dest )
{
    memcpy( dest, src, static_cast<size_t>(length) );
}

template<> inline void    CString<char>::Move( const char* src, integer length, char* dest )
{
    memmove( dest, src, static_cast<size_t>(length) );
}

template<> inline void    CString<char>::Fill( char* dest, char c, integer qty  )
{
    memset( dest, c, static_cast<size_t>(qty) );
}

template<> inline char    CString<char>::Toupper(char c)
{
    return static_cast<char>( toupper(c) );
}

template<> inline char    CString<char>::Tolower(char c)
{
    return static_cast<char>( tolower(c) );
}

template<> inline const char* CString<char>::Search( const char*  haystack,   integer  haystackLength,
                                             char         needle                               )
{
    return static_cast<const char*>( memchr( haystack, needle, static_cast<size_t>( haystackLength ) ) );
}

template<> inline bool    CString<char>::Equal( const char* str1,  const char* str2, integer cmpLength  )
{
    return  ::memcmp( str1, str2, static_cast<size_t>(cmpLength)  ) == 0;
}

template<> inline int     CString<char>::Compare( const char* str1,  const char* str2, integer cmpLength  )
{
    return  ::strncmp( str1, str2, static_cast<size_t>(cmpLength) );
}

template<> inline int     CString<char>::CompareIgnoreCase( const char* str1,  const char* str2, integer cmpLength  )
{
    #if defined (__GLIBCXX__)  || defined(__APPLE__)
        return  ::strncasecmp( str1, str2, static_cast<size_t>(cmpLength) );
    #elif defined ( _WIN32 )
        return  _strnicmp  ( str1, str2, static_cast<size_t>(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer CString<char>::IndexOfAnyIncludedZT   ( const char* haystack, const char* needles )
{
    const char* result= std::strpbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}

template<> inline integer CString<char>::IndexOfAnyExcludedZT( const char* haystack, const char* needles )
{
    return static_cast<integer>( std::strspn(haystack, needles) );
}



extern template ALIB_API integer CString<char>::IndexOfAnyIncluded    (const char*,integer,const char*,integer);
extern template ALIB_API integer CString<char>::IndexOfAnyExcluded    (const char*,integer,const char*,integer);
extern template ALIB_API integer CString<char>::LastIndexOfAnyInclude (const char*,integer,const char*,integer);
extern template ALIB_API integer CString<char>::LastIndexOfAnyExclude (const char*,integer,const char*,integer);
extern template ALIB_API integer CString<char>::IndexOfFirstDifference(const char*,integer,const char*,integer,lang::Case);


// #################################################################################################
// WString specifics
// #################################################################################################

template<> inline integer CString<wchar>::Length( const wchar*  cstring )
{
    return static_cast<integer>( ::wcslen( cstring ) );
}

template<> inline void    CString<wchar>::Copy( const wchar* src, integer length, wchar* dest )
{
    wmemcpy( dest, src, static_cast<size_t>(length) );
}

template<> inline void    CString<wchar>::Move( const wchar* src, integer length, wchar* dest )
{
    wmemmove( dest, src, static_cast<size_t>(length) );
}

template<> inline void    CString<wchar>::Fill( wchar* dest, wchar c, integer qty  )
{
    wmemset( dest, c, static_cast<size_t>(qty) );
}

template<> inline wchar CString<wchar>::Toupper(wchar c)
{
    return static_cast<wchar>(towupper(static_cast<wint_t>(c)));
}

template<> inline wchar CString<wchar>::Tolower(wchar c)
{
    return static_cast<wchar>(towlower(static_cast<wint_t>(c)));
}

template<> inline const wchar* CString<wchar>::Search( const wchar*  haystack, integer  haystackLength,
                                                           wchar         needle                             )
{
    return static_cast<const wchar*>(wmemchr( haystack, needle, static_cast<size_t>(haystackLength) ));
}

template<> inline bool    CString<wchar>::Equal( const wchar* str1,  const wchar* str2, integer cmpLength  )
{
    return  ::memcmp( str1, str2, static_cast<size_t>(cmpLength) * sizeof(wchar)  ) == 0;
}

template<> inline int     CString<wchar>::Compare( const wchar* str1,  const wchar* str2, integer cmpLength  )
{
    return  ::wcsncmp( str1, str2, static_cast<size_t>(cmpLength) );
}

template<> inline int     CString<wchar>::CompareIgnoreCase( const wchar* str1,  const wchar* str2, integer cmpLength  )
{
    #if defined (__GLIBCXX__)  || defined(__APPLE__)
        return  ::wcsncasecmp( str1, str2, static_cast<size_t>(cmpLength) );
    #elif defined ( _WIN32 )
        return  _wcsnicmp  ( str1, str2, static_cast<size_t>(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer  CString<wchar>::IndexOfAnyIncludedZT   ( const wchar* haystack, const wchar* needles )
{
    const wchar* result= std::wcspbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}

template<> inline integer  CString<wchar>::IndexOfAnyExcludedZT( const wchar* haystack, const wchar* needles )
{
    return static_cast<integer>( std::wcsspn(haystack, needles) );
}

extern template ALIB_API integer CString<wchar>::IndexOfAnyIncluded    (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CString<wchar>::IndexOfAnyExcluded    (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CString<wchar>::LastIndexOfAnyInclude (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CString<wchar>::LastIndexOfAnyExclude (const wchar*,integer,const wchar*,integer);
extern template ALIB_API integer CString<wchar>::IndexOfFirstDifference(const wchar*,integer,const wchar*,integer,lang::Case);


// #################################################################################################
// Strange-character string specifics
// #################################################################################################

template<>
integer CString<strangeChar>::Length( const strangeChar*  cstring );


//! @endcond


}} //namespace ::lib::strings

} // namespace aworx

#endif // HPP_ALIB_STRINGS_CSTRING

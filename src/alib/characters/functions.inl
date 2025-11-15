//==================================================================================================
/// \file
/// This header-file is part of module \alib_characters of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace characters {
//==================================================================================================
/// Converts a character to upper case.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param c    The character to convert
/// @return The upper case version of the given character.
//==================================================================================================
template<typename TChar>
TChar       ToUpper( TChar c );

//==================================================================================================
/// Converts a character sequence to upper case.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param src     Pointer to the character array.
/// @param length  The length of the array.
//==================================================================================================
template<typename TChar>
void        ToUpper( TChar* src, integer length );

//==================================================================================================
/// Converts a character to lower case.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param c    The character to convert
/// @return The lower case version of the given character.
//==================================================================================================
template<typename TChar>
TChar       ToLower( TChar c );

//==================================================================================================
/// Converts a character sequence to lower case.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param src     Pointer to the character array.
/// @param length  The length of the array.
//==================================================================================================
template<typename TChar>
void        ToLower( TChar* src, integer length );

//==================================================================================================
/// Compares two characters of arbitrary types.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @tparam sensitivity Letter case sensitivity of the comparison.
/// @tparam TRhs        The type of the right-hand side letter to compare.
/// @param  lhs         The left-hand side character to compare of class template
///                     type \p{TChar}.
/// @param  rhs         The right-hand side character to compare of method template
///                     type \p{TCharRhs} .
/// @return \c true if the given characters are equal, \c false otherwise.
//==================================================================================================
template<typename TChar, lang::Case sensitivity, typename TRhs >
bool Equal( TChar lhs, TRhs rhs ) {
    using TLhs= TChar;
    bool sensitive=  (sensitivity == lang::Case::Sensitive);

         if constexpr ( sizeof(TLhs) == sizeof(TRhs) )
        return  sensitive ?                             lhs  ==                            rhs
                          :  ToUpper(                   lhs) ==  ToUpper(                  rhs);

    else if constexpr ( sizeof(TLhs) < sizeof(TRhs) )
        return  sensitive ?           static_cast<TRhs>(lhs)  ==                           rhs
                          :  ToUpper( static_cast<TRhs>(lhs)) == ToUpper(                  rhs);

    else if constexpr ( sizeof(TLhs) > sizeof(TRhs) )
        return  sensitive ?                             lhs   ==         static_cast<TLhs>(rhs)
                          :  ToUpper(                   lhs ) == ToUpper(static_cast<TLhs>(rhs));
}

//==================================================================================================
/// Returns the length of a zero-terminated "c-style" character array.
///
/// Note: This method is implemented as an inlined, direct call to \c std::char_traits::length.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param cstring         Pointer to a zero-terminated character array.
/// @return The length of the string.
//==================================================================================================
template<typename TChar>
integer     Length( const TChar* cstring )
{ return integer( std::char_traits<TChar>::length(cstring) ); }

//==================================================================================================
/// Copies the contents of a character array into another, non-overlapping (!) array.
///
/// Note: This method is implemented as an inlined, direct call to \c std::char_traits::copy.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param src     Pointer to the source array.
/// @param length  The length to copy.
/// @param dest    Pointer to the destination array.
//==================================================================================================
template<typename TChar>
void        Copy( const TChar* src, integer length, TChar* dest )
{ std::char_traits<TChar>::copy( dest, src, size_t(length) ); }

//==================================================================================================
/// Copies the contents of a character array into another, possibly overlapping array.
///
/// Note: This method is implemented as an inlined, direct call to \c std::char_traits::move.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param src     Pointer to the source array.
/// @param length  The length to copy.
/// @param dest    Pointer to the destination array, optionally within source.
//==================================================================================================
template<typename TChar>
void        Move( const TChar* src, integer length, TChar* dest )
{ std::char_traits<TChar>::move( dest, src, size_t(length) ); }

//==================================================================================================
/// Sets all elements of the given character array to value \p{value}.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param dest      Pointer to the destination array.
/// @param length    The length to fill.
/// @param value     The value to fill the array with.
//==================================================================================================
template<typename TChar>
void        Fill( TChar* dest, integer length, TChar value );

//==================================================================================================
/// Reverses the order of the characters.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param src     Pointer to the character array.
/// @param length  The length of the array.
//==================================================================================================
template<typename TChar>
void        Reverse( TChar* src, integer length );

//==================================================================================================
/// Searches the character. Returns a pointer to the location of \p{needle} in \p{haystack},
/// respectively \c nullptr if not found.
///
/// Note: This method is implemented as a direct call to \c std::memchr, respectively
///       other character versions of it.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param haystackLength  The length of the string or the maximum position to search.
/// @param needle          Character to search.
///
/// @return The pointer to the first occurrence of \p{needle} respectively \c nullptr if
///         not found.
//==================================================================================================
template<typename TChar>
const TChar* Search( const TChar* haystack, integer haystackLength, TChar needle )
{ return std::char_traits<TChar>::find( haystack, size_t(haystackLength), needle ); }

//==================================================================================================
/// Returns the index of the first character in \p{haystack} which is included in a given set
/// of \p{needles}.
///
/// This method searches up to a given maximum index. For a search to the end of the
/// zero-terminated string, use faster method provided with class
/// \alib{strings;TCString;CString}.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param haystackLength  The length of the string or the maximum position to search.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs haystack to be zero-terminated).
/// @param needles         Pointer to a set of characters to be searched for.
/// @param needlesLength   The length of the string of needles.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs needles to be zero-terminated).
///
/// @return The index of the first character found that is included in \p{needles}.
///         If no character of haystack is included in \p{needles}, \c -1 is returned.
//==================================================================================================
template<typename TChar>
integer IndexOfAnyIncluded( const TChar*  haystack,   integer  haystackLength,
                            const TChar*  needles,    integer  needlesLength     );

//==================================================================================================
/// Same as #IndexOfAnyIncluded(const TChar*,integer,const TChar*,integer) but works on
/// zero-terminated strings.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack Pointer to a zero-terminated character array to search in.
/// @param needles  Pointer to a zero-terminated set of characters to search for.
///
/// @return The index of the first character found that is included in \p{needles}.
///         If no character of haystack is included in \p{needles}, \c -1 is returned.
//==================================================================================================
template<typename TChar>
integer IndexOfAnyIncludedZT( const TChar* haystack, const TChar* needles );

//==================================================================================================
/// Returns the index of the first character in \p{haystack} which is not included in a given
/// set of \p{needles}.
///
/// This method searches up to a given maximum index. For a search to the end of the
/// zero-terminated string, use the faster method provided with class
/// \alib{strings;TCString;CString}.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param haystackLength  The length of the string or the maximum position to search.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs haystack to be zero-terminated).
/// @param needles         Pointer to a set of characters to be searched for.
/// @param needlesLength   The length of the string of needles.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs needles to be zero-terminated).
///
/// @return The index of the first character that is not included in \p{needles}.
///         If all characters of haystack are included in \p{needles}, \c -1 is returned.
//==================================================================================================
template<typename TChar>
integer IndexOfAnyExcluded( const TChar*  haystack,   integer  haystackLength,
                            const TChar*  needles,    integer  needlesLength     );


//==================================================================================================
/// Same as #IndexOfAnyExcluded(const TChar*,integer,const TChar*,integer) but works on
/// zero-terminated strings.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack Pointer to a zero-terminated character array to search in.
/// @param needles  Pointer to a zero-terminated set of characters to search for.
///
/// @return The index of the first character that is not included in \p{needles}.
///         If all characters of haystack are included in \p{needles}, \c -1 is returned.
//==================================================================================================
template<typename TChar>
integer IndexOfAnyExcludedZT( const TChar* haystack, const TChar* needles );

//==================================================================================================
/// Returns the index of the last character in \p{haystack} which is included in a given set
/// of \p{needles}.
///
/// This method searches backwards from the end of the string.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param startIdx        The position to start the search from. This must be smaller than
///                        the length of the string and greater or equal to  zero.
/// @param needles         Pointer to a set of characters to be searched for.
/// @param needlesLength   The length of the string of needles.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs needles to be zero-terminated).
///
/// @return The index of the first character found which is included in the given set
///         of characters. If nothing is found, -1 is returned.
//==================================================================================================
template<typename TChar>
integer LastIndexOfAnyInclude( const TChar*  haystack,    integer  startIdx,
                               const TChar*  needles,     integer  needlesLength );

//==================================================================================================
/// Returns the index of the last character in \p{haystack} which is not included in a given
/// set of \p{needles}.
///
/// This method searches backwards from the end of the string.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param startIdx        The position to start the search from. This must be smaller than
///                        the length of the string and greater or equal to  zero.
/// @param needles         Pointer to a set of characters to be searched for.
/// @param needlesLength   The length of the string of needles.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs needles to be zero-terminated).
///
/// @return The index of the first character found which is included in the given set
///         of characters. If nothing is found, -1 is returned.
//==================================================================================================
template<typename TChar>
integer LastIndexOfAnyExclude( const TChar*  haystack,    integer  startIdx,
                               const TChar*  needles,     integer  needlesLength );

//==================================================================================================
/// Returns the index of the first character which is not equal within two strings.
/// If \p{haystack} starts with \p{needle}, then the length of \p{needle} is returned.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param haystack        Pointer to the start of the string.
/// @param haystackLength  The length of the string or the maximum position to search.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs haystack to be zero-terminated).
/// @param needle          Pointer to the start of the string to compare with.
/// @param needleLength    The length of \p{needle}.
///                        If -1 is provided, the length is determined using standard library
///                        function \b strlen (which needs needles to be zero-terminated).
/// @param sensitivity     Denotes whether the comparison should be made case-sensitive
///                        or not.
///
/// @return The index of the first character found which is not equal in given strings.
//==================================================================================================
template<typename TChar>
integer IndexOfFirstDifference( const TChar*  haystack,   integer  haystackLength,
                                const TChar*  needle,     integer  needleLength,
                                lang::Case    sensitivity                         );

//==================================================================================================
/// Searches for a difference in two character arrays of equal length.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param lhs       The first array to compare.
/// @param rhs       The second array to compare.
/// @param cmpLength The number of characters to compare.
///
/// @return \c true if the string arrays have identical contents, \c false otherwise.
//==================================================================================================
template<typename TChar>
bool Equal( const TChar* lhs,  const TChar* rhs, integer cmpLength  )
{ return  ::memcmp( lhs, rhs, size_t(cmpLength) * sizeof(TChar) ) == 0; }

//==================================================================================================
/// Compares up to \p{cmpLength} characters of two character arrays.
/// Comparison stops if termination character \c '\0' is found in one of the arrays.
///
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param lhs       The first array to compare.
/// @param rhs       The second array to compare.
/// @param cmpLength The number of characters to compare.
///
/// @return Negative value if lhs appears before rhs in lexicographical order.
///         \c 0 if lhs and rhs are equal.
///         Positive value if lhs appears after rhs in lexicographical order.
//==================================================================================================
template<typename TChar>
int Compare( const TChar* lhs,  const TChar* rhs, integer cmpLength  )
{ return std::char_traits<TChar>::compare( lhs, rhs, size_t(cmpLength) ); }

//==================================================================================================
/// Compares two character arrays of equal length ignoring letter case.
/// @tparam TChar One of the six (overlapping) \ref alib_characters_chars "character types".
/// @param lhs      The first array to compare.
/// @param rhs      The second array to compare.
/// @param cmpLength The number of characters to compare.
///
/// @return Negative value if lhs appears before rhs in lexicographical order.
///         \c 0 if lhs and rhs are equal.
///         Positive value if lhs appears after rhs in lexicographical order.
//==================================================================================================
template<typename TChar>
int CompareIgnoreCase( const TChar* lhs,  const TChar* rhs, integer cmpLength  );



//! @cond NO_DOX
//##################################################################################################
// Narrow character specifics
//##################################################################################################
template<> inline void    Fill<nchar>( nchar* dest, integer length, nchar c )
{ memset( dest, c, size_t(length) ); }

template<> inline nchar   ToUpper<nchar>(nchar c)       { return static_cast<nchar>( toupper(c) ); }

template<> inline nchar   ToLower<nchar>(nchar c)       { return static_cast<nchar>( tolower(c) ); }

template<> inline void    ToUpper<nchar>(nchar* src, integer length) {
    nchar* end= src  + length;
    while( src != end ) {
        *src=  ToUpper<nchar>( *src );
        ++src;
}   }

template<> inline void    ToLower<nchar>(nchar* src, integer length) {
    nchar* end= src  + length;
    while( src != end ) {
        *src=  ToLower<nchar>( *src );
        ++src;
}   }

template<> inline int     CompareIgnoreCase<nchar>( const nchar* lhs,  const nchar* rhs,
                                                    integer cmpLength  ) {
    #if defined (__GLIBCXX__)  || defined(_LIBCPP_VERSION) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return  ::strncasecmp( lhs, rhs, size_t(cmpLength) );
    #elif defined ( _WIN32 )
        return  _strnicmp  ( lhs, rhs, size_t(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer IndexOfAnyIncludedZT   <nchar>( const nchar* haystack,
                                                          const nchar* needles ) {
    const nchar* result= std::strpbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}

template<> inline integer IndexOfAnyExcludedZT<nchar>( const nchar* haystack, const nchar* needles )
{ return integer( std::strspn(haystack, needles) ); }



extern template ALIB_DLL integer IndexOfAnyIncluded    <nchar>(const nchar*,integer,const nchar*,integer);
extern template ALIB_DLL integer IndexOfAnyExcluded    <nchar>(const nchar*,integer,const nchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyInclude <nchar>(const nchar*,integer,const nchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyExclude <nchar>(const nchar*,integer,const nchar*,integer);
extern template ALIB_DLL integer IndexOfFirstDifference<nchar>(const nchar*,integer,const nchar*,integer,lang::Case);
extern template ALIB_DLL void    Reverse               <nchar>(      nchar*,integer );


//##################################################################################################
// Wide character specifics
//##################################################################################################
template<> inline wchar   ToUpper<wchar>(wchar c)
{ return static_cast<wchar>(towupper(static_cast<wint_t>(c))); }

template<> inline wchar   ToLower<wchar>(wchar c)
{ return static_cast<wchar>(towlower(static_cast<wint_t>(c))); }

template<> inline void    ToUpper<wchar>(wchar* src, integer length) {
    wchar* end= src  + length;
    while( src != end ) {
        *src=  ToUpper<wchar>( *src );
        ++src;
}   }

template<> inline void    ToLower<wchar>(wchar* src, integer length) {
    wchar* end= src  + length;
    while( src != end ) {
        *src=  ToLower<wchar>( *src );
        ++src;
}   }

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> inline void    Fill<wchar>( wchar* dest, integer length, wchar c )
{ wmemset( dest, c, size_t(length) ); }

template<> inline int     CompareIgnoreCase<wchar>( const wchar* lhs,  const wchar* rhs,
                                                    integer cmpLength  ) {
    #if defined ( _WIN32 )
        return  _wcsnicmp  ( lhs, rhs, size_t(cmpLength)  );
    #elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        return  ::wcsncasecmp( lhs, rhs, size_t(cmpLength) );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer IndexOfAnyIncludedZT<wchar>(const wchar* haystack, const wchar* needles) {
    const wchar* result= std::wcspbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}
template<> inline integer IndexOfAnyExcludedZT<wchar>(const wchar* haystack, const wchar* needles)
{ return integer( std::wcsspn(haystack, needles) ); }


#else
template<> ALIB_DLL void    Fill<wchar>( wchar* dest, integer length, wchar c );
template<> ALIB_DLL int     CompareIgnoreCase   <wchar>( const wchar* lhs,      const wchar* rhs, integer cmpLength );
template<> ALIB_DLL integer IndexOfAnyIncludedZT<wchar>( const wchar* haystack, const wchar* needles );
template<> ALIB_DLL integer IndexOfAnyExcludedZT<wchar>( const wchar* haystack, const wchar* needles );
#endif


extern template ALIB_DLL integer IndexOfAnyIncluded    <wchar>(const wchar*,integer,const wchar*,integer);
extern template ALIB_DLL integer IndexOfAnyExcluded    <wchar>(const wchar*,integer,const wchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyInclude <wchar>(const wchar*,integer,const wchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyExclude <wchar>(const wchar*,integer,const wchar*,integer);
extern template ALIB_DLL integer IndexOfFirstDifference<wchar>(const wchar*,integer,const wchar*,integer,lang::Case);
extern template ALIB_DLL void    Reverse               <wchar>(      wchar*,integer );



//##################################################################################################
// Strange character specifics
//##################################################################################################
template<> inline xchar     ToUpper<xchar>(xchar c)
{ return static_cast<xchar>(towupper(static_cast<wint_t>(c))); }

template<> inline xchar     ToLower<xchar>(xchar c)
{ return static_cast<xchar>(towlower(static_cast<wint_t>(c))); }

template<> inline void    ToUpper<xchar>(xchar* src, integer length) {
    xchar* end= src  + length;
    while( src != end ) {
        *src=  ToUpper<xchar>( *src );
        ++src;
}   }

template<> inline void    ToLower<xchar>(xchar* src, integer length) {
    xchar* end= src  + length;
    while( src != end ) {
        *src=  ToLower<xchar>( *src );
        ++src;
}   }

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> ALIB_DLL void    Fill<xchar>( xchar* dest, integer length, xchar c );
template<> ALIB_DLL int     CompareIgnoreCase<xchar>( const xchar* lhs,  const xchar* rhs, integer cmpLength  );
template<> ALIB_DLL integer IndexOfAnyIncludedZT<xchar>( const xchar* haystack, const xchar* needles );
template<> ALIB_DLL integer IndexOfAnyExcludedZT<xchar>( const xchar* haystack, const xchar* needles );
#else
template<> inline void    Fill<xchar>( xchar* dest, integer length, xchar c )
{
    wmemset( dest, c, size_t(length) );
}

template<> inline int     CompareIgnoreCase<xchar>( const xchar* lhs,  const xchar* rhs, integer cmpLength  )
{
    #if defined (__GLIBCXX__)|| defined(_LIBCPP_VERSION)   || defined(__APPLE__)
        return  ::wcsncasecmp( lhs, rhs, size_t(cmpLength) );
    #elif defined ( _WIN32 )
        return  _wcsnicmp  ( lhs, rhs, size_t(cmpLength)  );
    #else
        #pragma message ( "Unknown Platform in file: " __FILE__ )
    #endif
}

template<> inline integer IndexOfAnyIncludedZT   <xchar>( const xchar* haystack, const xchar* needles )
{
    const xchar* result= std::wcspbrk(haystack, needles);
    return result ? result - haystack
                  : -1;
}
template<> inline integer IndexOfAnyExcludedZT<xchar>( const xchar* haystack, const xchar* needles )
{
    return integer( std::wcsspn(haystack, needles) );
}
#endif

extern template ALIB_DLL integer IndexOfAnyIncluded    <xchar>(const xchar*,integer,const xchar*,integer);
extern template ALIB_DLL integer IndexOfAnyExcluded    <xchar>(const xchar*,integer,const xchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyInclude <xchar>(const xchar*,integer,const xchar*,integer);
extern template ALIB_DLL integer LastIndexOfAnyExclude <xchar>(const xchar*,integer,const xchar*,integer);
extern template ALIB_DLL integer IndexOfFirstDifference<xchar>(const xchar*,integer,const xchar*,integer,lang::Case);
extern template ALIB_DLL void    Reverse               <xchar>(      xchar*,integer );
//! @endcond

//##################################################################################################
// struct AlignedCharArray
//##################################################################################################
/// Encapsulates a fixed-size character buffer.
/// The character type and the buffer's length are templated.
/// Furthermore the buffer, and with it this type, is aligned to \c 8 bytes on 64-bit machines and
/// to \c 4 bytes on 32-bit hardware.
/// This supports fast access and compile-time optimizations.
///
/// An overloaded constructor and method #Fill allow filling the buffer with a distinct character.
/// This is provided, as a frequent use-case for this struct is to provide
/// \alib{strings;TString;String} objects of variable length.
/// For example, when generating indentation in text files, a <c>std::basic_ostream</c> might be
/// faster filled with spaces using a local variable of this type than putting character by
/// character to the stream in a loop.
///
/// The type's name uses the word "local" because usually it is used with local (stack allocated)
/// variables.
/// @tparam TChar    The character type.
///                  Alias names for C++ character types exist with
///                          - \alib{characters;character},
///                          - \alib{characters;nchar},
///                          - \alib{characters;wchar},
///                          - \alib{characters;xchar},
///                          - \alib{characters;complementChar}, and
///                          - \alib{characters;strangeChar}.
///
///                  Defaults to \alib{characters;character}.
///
/// @tparam TLength  The length of the local buffer. Defaults to 128 bytes.
template<typename TChar= character, size_t TLength= 128/sizeof(TChar)>
struct AlignedCharArray {
    /// The alignment of the field #buffer. Because this is the first (and only) field, this
    /// struct itself shares this alignment.
    static constexpr size_t Alignment =
        std::conditional_t<sizeof(void*) == 8, std::integral_constant<size_t, 64>,
                                               std::integral_constant<size_t, 64>>::value;

    /// The buffer.
    alignas(Alignment) TChar buffer[TLength];

    /// Default constructor. Leaves the characters uninitialized.
    constexpr               AlignedCharArray()                                    noexcept =default;

    /// Constructor taking a character to initialize the buffer with.
    /// @param fillChar The character to fill this local buffer with.
    constexpr               AlignedCharArray( TChar fillChar )        noexcept { Fill( fillChar ); }

    /// Returns a pointer to the internal buffer.
    /// @return A mutable pointer to the characters.
    constexpr TChar*        Buffer()                                     noexcept { return buffer; }

    /// Returns a pointer to the internal buffer.
    /// @return A const pointer to the characters.
    constexpr const TChar*  Buffer()                               const noexcept { return buffer; }

    /// Returns the number of characters in this buffer.
    /// @return The value of template parameter \p{TLength}.
    constexpr integer       Length()                              const noexcept { return TLength; }

    /// Fills the buffer with given \p{fillChar}.
    /// @param fillChar The character to fill this local buffer with.
    constexpr void Fill( TChar fillChar )                                                   noexcept
    { characters::Fill( buffer, TLength, fillChar); }
};

}

/// Type alias in namespace \b alib.
/// Note that the original struct has template parameters, which, for technical reasons can
/// not be defaulted with this alias as they are defaulted in the original.
/// Therefore, this alias uses explicit defaults (which are not changeable).
using AlignedCharArray=   characters::AlignedCharArray<>;

} // namespace [alib::character]

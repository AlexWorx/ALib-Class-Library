// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/characters/characters.hpp"
#endif // !DOXYGEN


ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
#include "alib/lang/callerinfo_functions.hpp"

namespace alib {

/// This is the reference documentation of sub-namespace \c characters of the \aliblink, which
/// holds types of library module \alib_characters.
///
/// Extensive documentation for this module is provided with
/// \ref alib_mod_characters "ALib Module Characters - Programmer's Manual".
namespace characters {

//! @cond NO_DOX
template<typename TChar>
void Reverse( TChar* array, integer length )
{
    TChar* start= &array[0];
    TChar* end  = &array[length-1];
    while( start < end )
    {
        TChar tmp= *start;
        *start   = *end;
        *end     = tmp;
        ++start;
        --end;
    }
}

template<typename TChar>
integer IndexOfAnyIncluded( const TChar* haystack,  integer  length,
                                              const TChar* needles,   integer  needlesLength )
{
    if ( length        == -1 )    length=        static_cast<integer>( Length( haystack ) );
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* end=    haystack + length;

    const TChar* s=      haystack;
    while( s != end )
    {
        for( integer i= 0; i < needlesLength ; ++i )
            if( *(needles + i) == *s )
                return s - haystack;
        ++s;
    }

    return -1;
}

template<typename TChar>
integer IndexOfAnyExcluded( const TChar* haystack,  integer  length,
                                            const TChar* needles,   integer  needlesLength )
{
    if ( length        == -1 )    length=        static_cast<integer>( Length( haystack ) );
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* end= haystack + length;
    const TChar* s  = haystack - 1;
    while( ++s != end )
    {
        integer i;
        for( i= 0; i < needlesLength ; ++i )
            if( needles[i] == *s )
                break;
        if ( i == needlesLength )
            return s - haystack;
    }

    return -1;
}

template<typename TChar>
integer LastIndexOfAnyInclude( const TChar* haystack,  integer startPos,
                                                 const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;
    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', "CHARS", "Found '\\0' in source string")
        for( integer i= 0; i < needlesLength ; ++i )
            if( *(needles + i) == *s )
                return s - haystack;

        --s;
    }
    return -1;
}

template<typename TChar>
integer LastIndexOfAnyExclude( const TChar* haystack,  integer startPos,
                                                 const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;
    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', "CHARS", "Found '\\0' in source string")
        {
            integer i;
            for( i= 0; i < needlesLength ; ++i )
                if( needles[i] == *s )
                    break;
            if ( i == needlesLength )
                return s - haystack;
        }
        --s;
    }
    return -1;
}

template<typename TChar>
integer IndexOfFirstDifference( const TChar* haystack,  integer haystackLength,
                                                  const TChar* needle,    integer needleLength,
                                                  lang::Case  sensitivity                        )
{
    if ( haystackLength == -1 )    haystackLength= static_cast<integer>( Length( haystack ) );
    if ( needleLength   == -1 )    needleLength=   static_cast<integer>( Length( needle   ) );

    integer idx= 0;

    if ( sensitivity == lang::Case::Sensitive )
    {
        while(    idx != haystackLength
               && idx != needleLength
               && haystack[idx] == needle[idx] )
            ++idx;
    }
    else
    {
        while(    idx != haystackLength
               && idx != needleLength
               &&      ToUpper( haystack[idx] )
                    == ToUpper( needle[idx] ) )
            ++idx;
    }
    return idx;
}


// #################################################################################################
// NString
// #################################################################################################

template integer IndexOfAnyIncluded    <nchar>(const nchar*,integer,const nchar*,integer);
template integer IndexOfAnyExcluded    <nchar>(const nchar*,integer,const nchar*,integer);
template integer LastIndexOfAnyInclude <nchar>(const nchar*,integer,const nchar*,integer);
template integer LastIndexOfAnyExclude <nchar>(const nchar*,integer,const nchar*,integer);
template integer IndexOfFirstDifference<nchar>(const nchar*,integer,const nchar*,integer,lang::Case);
template void    Reverse               <nchar>(      nchar*,integer );

// #################################################################################################
// WString
// #################################################################################################
#if !ALIB_CHARACTERS_NATIVE_WCHAR
template<> void  Fill<wchar>( wchar* dest, integer length, wchar c  )
{
    wchar* end= dest + length;
    while( dest != end )
        *dest++= c;
}

template<> int    CompareIgnoreCase<wchar>( const wchar* str1,  const wchar* str2, integer cmpLength  )
{
    const wchar* end= str1 + cmpLength;
    int  diff;
    while( str1 != end )
        if( ( diff=   static_cast<int>( towupper(static_cast<wint_t>(*str1++)))
                    - static_cast<int>( towupper(static_cast<wint_t>(*str2++))) ) != 0 )
            return diff;
    return 0;
}

template<> integer IndexOfAnyIncludedZT<wchar>( const wchar* haystack, const wchar* needles )
{
    const wchar* h= haystack;
    while( *h != 0)
    {
        const wchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                return h - haystack;
            ++n;
        }
        ++h;
    }
    return -1;
}

template<> integer IndexOfAnyExcludedZT<wchar>( const wchar* haystack, const wchar* needles )
{
    const wchar* h= haystack;
    while( *h != 0)
    {
        const wchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                break;
            ++n;
        }
        if(!*n)
            return h - haystack;
        ++h;
    }
    return -1;
}
#endif

template integer IndexOfAnyIncluded    <wchar>(const wchar*,integer,const wchar*,integer);
template integer IndexOfAnyExcluded    <wchar>(const wchar*,integer,const wchar*,integer);
template integer LastIndexOfAnyInclude <wchar>(const wchar*,integer,const wchar*,integer);
template integer LastIndexOfAnyExclude <wchar>(const wchar*,integer,const wchar*,integer);
template integer IndexOfFirstDifference<wchar>(const wchar*,integer,const wchar*,integer,lang::Case);
template void    Reverse               <wchar>(      wchar*,integer );


// #################################################################################################
// Strange character string
// #################################################################################################
template integer IndexOfAnyIncluded    <xchar>(const xchar*,integer,const xchar*,integer);
template integer IndexOfAnyExcluded    <xchar>(const xchar*,integer,const xchar*,integer);
template integer LastIndexOfAnyInclude <xchar>(const xchar*,integer,const xchar*,integer);
template integer LastIndexOfAnyExclude <xchar>(const xchar*,integer,const xchar*,integer);
template integer IndexOfFirstDifference<xchar>(const xchar*,integer,const xchar*,integer,lang::Case);
template void    Reverse               <xchar>(      xchar*,integer );

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> void  Fill<xchar>( xchar* dest, integer length, xchar c  )
{
    xchar* end= dest + length;
    while( dest != end )
        *dest++= c;
}

template<> int    CompareIgnoreCase<xchar>( const xchar* str1,  const xchar* str2, integer cmpLength  )
{
    const xchar* end= str1 + cmpLength;
    int  diff;
    while( str1 != end )
        if( ( diff=   static_cast<int>( towupper(static_cast<wint_t>(*str1++)))
                    - static_cast<int>( towupper(static_cast<wint_t>(*str2++))) ) != 0 )
            return diff;
    return 0;
}

template<> integer IndexOfAnyIncludedZT<xchar>( const xchar* haystack, const xchar* needles )
{
    const xchar* h= haystack;
    while( *h != 0)
    {
        const xchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                return h - haystack;
            ++n;
        }
        ++h;
    }
    return -1;
}

template<> integer IndexOfAnyExcludedZT<xchar>( const xchar* haystack, const xchar* needles )
{
    const xchar* h= haystack;
    while( *h != 0)
    {
        const xchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                break;
            ++n;
        }
        if(!*n)
            return h - haystack;
        ++h;
    }
    return -1;
}
#endif


//! @endcond


}} // namespace [alib::character]

#include "alib/lang/callerinfo_methods.hpp"

ALIB_WARNINGS_RESTORE // ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE


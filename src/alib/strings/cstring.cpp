// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


namespace aworx { namespace lib { namespace strings {

//! @cond NO_DOX
template<typename TChar>
integer CString<TChar>::IndexOfAnyIncluded( const TChar* haystack,  integer  length,
                                            const TChar* needles,   integer  needlesLength )
{
    if ( length        == -1 )    length=        static_cast<integer>( Length( haystack ) );
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* end=    haystack + length;

    const TChar* s=      haystack;
    while( s != end )
    {
        for( int i= 0; i < needlesLength ; ++i )
            if( *(needles + i) == *s )
                return s - haystack;
        s++;
    }

    return -1;
}

template<typename TChar>
integer CString<TChar>::IndexOfAnyExcluded( const TChar* haystack,  integer  length,
                                            const TChar* needles,   integer  needlesLength )
{
    if ( length        == -1 )    length=        static_cast<integer>( Length( haystack ) );
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* end=    haystack + length;

    const TChar* s=   haystack - 1;
    while( ++s != end )
    {
        int i;
        for( i= 0; i < needlesLength ; ++i )
            if( needles[i] == *s )
                break;
        if ( i == needlesLength )
            return s - haystack;
    }

    return -1;
}

template<typename TChar>
integer CString<TChar>::LastIndexOfAnyInclude( const TChar* haystack,  integer startPos,
                                               const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;

    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', ASTR("AString::LastIndexOfAny(): found '\\0' in source"));
        for( int i= 0; i < needlesLength ; ++i )
            if( *(needles + i) == *s )
                return s - haystack;

        s--;
    }
    return -1;
}

template<typename TChar>
integer CString<TChar>::LastIndexOfAnyExclude( const TChar* haystack,  integer startPos,
                                               const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;

    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', ASTR("String::LastIndexOfAny(): found '\\0' in source"));
        {
            int i;
            for( i= 0; i < needlesLength ; ++i )
                if( needles[i] == *s )
                    break;
            if ( i == needlesLength )
                return s - haystack;
        }

        s--;
    }
    return -1;
}

template<typename TChar>
integer CString<TChar>::IndexOfFirstDifference( const TChar* haystack,  integer haystackLength,
                                                const TChar* needle,    integer needleLength,
                                                lang::Case  sensitivity                        )
{
    if ( haystackLength == -1 )    haystackLength= static_cast<integer>( Length( haystack ) );
    if ( needleLength   == -1 )    needleLength=   static_cast<integer>( Length( needle   ) );

    integer idx= 0;

    if ( sensitivity == Case::Sensitive )
    {
        while(    idx < haystackLength
               && idx < needleLength
               && haystack[idx] == needle[idx] )
            idx++;
    }
    else
    {
        while(    idx < haystackLength
               && idx < needleLength
               && toupper( haystack[idx] ) == toupper( needle[idx] ) )
            idx++;
    }
    return idx;
}


// #################################################################################################
// NString
// #################################################################################################

template integer CString<char>::IndexOfAnyIncluded    (const char*,integer,const char*,integer);
template integer CString<char>::IndexOfAnyExcluded    (const char*,integer,const char*,integer);
template integer CString<char>::LastIndexOfAnyInclude (const char*,integer,const char*,integer);
template integer CString<char>::LastIndexOfAnyExclude (const char*,integer,const char*,integer);
template integer CString<char>::IndexOfFirstDifference(const char*,integer,const char*,integer,lang::Case);

// #################################################################################################
// WString
// #################################################################################################

template integer CString<wchar_t>::IndexOfAnyIncluded    (const wchar_t*,integer,const wchar_t*,integer);
template integer CString<wchar_t>::IndexOfAnyExcluded    (const wchar_t*,integer,const wchar_t*,integer);
template integer CString<wchar_t>::LastIndexOfAnyInclude (const wchar_t*,integer,const wchar_t*,integer);
template integer CString<wchar_t>::LastIndexOfAnyExclude (const wchar_t*,integer,const wchar_t*,integer);
template integer CString<wchar_t>::IndexOfFirstDifference(const wchar_t*,integer,const wchar_t*,integer,lang::Case);

// #################################################################################################
// Strange character string
// #################################################################################################
template<>
integer CString<strangeChar>::Length( const strangeChar*  cstring )
{
    if ( !cstring )
         return 0;
    const strangeChar* p= cstring;
    while( *p != 0 )
        p++;
    return static_cast<integer>( p - cstring );
}

//! @endcond


}}}// namespace [aworx::lib::strings]

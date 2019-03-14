// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/characters/chararray.hpp"

#if !defined(HPP_ALIB_LIB_ASSERT)
#   include "alib/lib/assert.hpp"
#endif


namespace aworx { namespace lib {

/**
 * This is the reference documentation of sub-namespace \b characters of the \aliblink which
 * holds types of library module \alibmod_characters.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_characters "ALib Module Characters - Programmer's Manual".
 */
namespace characters {

//! @cond NO_DOX
template<typename TChar>
integer CharArray<TChar>::IndexOfAnyIncluded( const TChar* haystack,  integer  length,
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
        s++;
    }

    return -1;
}

template<typename TChar>
integer CharArray<TChar>::IndexOfAnyExcluded( const TChar* haystack,  integer  length,
                                            const TChar* needles,   integer  needlesLength )
{
    if ( length        == -1 )    length=        static_cast<integer>( Length( haystack ) );
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* end=    haystack + length;

    const TChar* s=   haystack - 1;
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
integer CharArray<TChar>::LastIndexOfAnyInclude( const TChar* haystack,  integer startPos,
                                                 const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;

    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', "AString::LastIndexOfAny(): found '\\0' in source")
        for( integer i= 0; i < needlesLength ; ++i )
            if( *(needles + i) == *s )
                return s - haystack;

        s--;
    }
    return -1;
}

template<typename TChar>
integer CharArray<TChar>::LastIndexOfAnyExclude( const TChar* haystack,  integer startPos,
                                                 const TChar* needles,   integer needlesLength  )
{
    if ( needlesLength == -1 )    needlesLength= static_cast<integer>( Length( needles  ) );

    const TChar* s= haystack + startPos;

    while( s >= haystack )
    {
        ALIB_ASSERT_ERROR( *s != '\0', "String::LastIndexOfAny(): found '\\0' in source")
        {
            integer i;
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
integer CharArray<TChar>::IndexOfFirstDifference( const TChar* haystack,  integer haystackLength,
                                                  const TChar* needle,    integer needleLength,
                                                  Case  sensitivity                        )
{
    if ( haystackLength == -1 )    haystackLength= static_cast<integer>( Length( haystack ) );
    if ( needleLength   == -1 )    needleLength=   static_cast<integer>( Length( needle   ) );

    integer idx= 0;

    if ( sensitivity == Case::Sensitive )
    {
        while(    idx != haystackLength
               && idx != needleLength
               && haystack[idx] == needle[idx] )
            idx++;
    }
    else
    {
        while(    idx != haystackLength
               && idx != needleLength
               && toupper( haystack[idx] ) == toupper( needle[idx] ) )
            idx++;
    }
    return idx;
}


// #################################################################################################
// NString
// #################################################################################################

template integer CharArray<nchar>::IndexOfAnyIncluded    (const nchar*,integer,const nchar*,integer);
template integer CharArray<nchar>::IndexOfAnyExcluded    (const nchar*,integer,const nchar*,integer);
template integer CharArray<nchar>::LastIndexOfAnyInclude (const nchar*,integer,const nchar*,integer);
template integer CharArray<nchar>::LastIndexOfAnyExclude (const nchar*,integer,const nchar*,integer);
template integer CharArray<nchar>::IndexOfFirstDifference(const nchar*,integer,const nchar*,integer,Case);

// #################################################################################################
// WString
// #################################################################################################
#if !ALIB_CHARACTERS_NATIVE_WCHAR
template<> void  CharArray<wchar>::Fill( wchar* dest, integer length, wchar c  )
{
    wchar* end= dest + length;
    while( dest != end )
        *dest++= c;
}

template<> int    CharArray<wchar>::CompareIgnoreCase( const wchar* str1,  const wchar* str2, integer cmpLength  )
{
    const wchar* end= str1 + cmpLength;
    int  diff;
    while( str1 != end )
        if( ( diff=   static_cast<int>( towupper(static_cast<wint_t>(*str1++)))
                    - static_cast<int>( towupper(static_cast<wint_t>(*str2++))) ) != 0 )
            return diff;
    return 0;
}

template<> integer CharArray<wchar>::IndexOfAnyIncludedZT( const wchar* haystack, const wchar* needles )
{
    const wchar* h= haystack;
    while( *h != 0)
    {
        const wchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                return h - haystack;
            n++;
        }
        h++;
    }
    return -1;
}

template<> integer CharArray<wchar>::IndexOfAnyExcludedZT( const wchar* haystack, const wchar* needles )
{
    const wchar* h= haystack;
    while( *h != 0)
    {
        const wchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                break;
            n++;
        }
        if(!*n)
            return h - haystack;
        h++;
    }
    return -1;
}
#endif

template integer CharArray<wchar>::IndexOfAnyIncluded    (const wchar*,integer,const wchar*,integer);
template integer CharArray<wchar>::IndexOfAnyExcluded    (const wchar*,integer,const wchar*,integer);
template integer CharArray<wchar>::LastIndexOfAnyInclude (const wchar*,integer,const wchar*,integer);
template integer CharArray<wchar>::LastIndexOfAnyExclude (const wchar*,integer,const wchar*,integer);
template integer CharArray<wchar>::IndexOfFirstDifference(const wchar*,integer,const wchar*,integer,Case);


// #################################################################################################
// Strange character string
// #################################################################################################
template integer CharArray<xchar>::IndexOfAnyIncluded    (const xchar*,integer,const xchar*,integer);
template integer CharArray<xchar>::IndexOfAnyExcluded    (const xchar*,integer,const xchar*,integer);
template integer CharArray<xchar>::LastIndexOfAnyInclude (const xchar*,integer,const xchar*,integer);
template integer CharArray<xchar>::LastIndexOfAnyExclude (const xchar*,integer,const xchar*,integer);
template integer CharArray<xchar>::IndexOfFirstDifference(const xchar*,integer,const xchar*,integer,Case);

#if ALIB_CHARACTERS_NATIVE_WCHAR
template<> void  CharArray<xchar>::Fill( xchar* dest, integer length, xchar c  )
{
    xchar* end= dest + length;
    while( dest != end )
        *dest++= c;
}

template<> int    CharArray<xchar>::CompareIgnoreCase( const xchar* str1,  const xchar* str2, integer cmpLength  )
{
    const xchar* end= str1 + cmpLength;
    int  diff;
    while( str1 != end )
        if( ( diff=   static_cast<int>( towupper(static_cast<wint_t>(*str1++)))
                    - static_cast<int>( towupper(static_cast<wint_t>(*str2++))) ) != 0 )
            return diff;
    return 0;
}

template<> integer CharArray<xchar>::IndexOfAnyIncludedZT( const xchar* haystack, const xchar* needles )
{
    const xchar* h= haystack;
    while( *h != 0)
    {
        const xchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                return h - haystack;
            n++;
        }
        h++;
    }
    return -1;
}

template<> integer CharArray<xchar>::IndexOfAnyExcludedZT( const xchar* haystack, const xchar* needles )
{
    const xchar* h= haystack;
    while( *h != 0)
    {
        const xchar* n= needles;
        while( *n != 0 )
        {
            if (*h == *n )
                break;
            n++;
        }
        if(!*n)
            return h - haystack;
        h++;
    }
    return -1;
}
#endif

template<> int    CharArray<xchar>::Compare( const xchar* str1,  const xchar* str2, integer cmpLength  )
{
    const xchar* end= str1 + cmpLength;
    int diff;
    while( str1 != end )
        if( (diff =   static_cast<int>( *str1++ )
                    - static_cast<int>( *str2++ ) ) != 0)
            return diff;

    return 0;
}


//! @endcond


}}} // namespace [aworx::lib::character]


// #################################################################################################
// Set 'flags' for pretty printers by defining global symbols.
// See file $ALOX_LIB_PATH/ALox/tools/ideplugins/gdb/alibpp.py for more information
// #################################################################################################

#if  defined(ALIB_GDB_PP_SUPPRESS_CHILDREN)
    extern int ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN;
           int ALIB_PRETTY_PRINTERS_SUPPRESS_CHILDREN;
#endif

#if  defined(ALIB_GDB_PP_FIND_POINTER_TYPES)
    extern int ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES;
           int ALIB_PRETTY_PRINTERS_FIND_POINTER_TYPES;
#endif


//
// define further global symbols which are detected by the pretty printer python script for gdb
//
#if  !ALIB_CHARACTERS_ARE_NARROW
    extern int ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE;
           int ALIB_PRETTY_PRINTERS_DEFAULT_CHAR_IS_WIDE;
#endif

#if ALIB_SIZEOF_WCHAR_T == 4
    extern int ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4;
           int ALIB_PRETTY_PRINTERS_WCHAR_SIZE_IS_4;

#endif


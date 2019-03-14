// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"


#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION)
#   include "alib/strings/detail/numberconversion.hpp"
#endif

#if ALIB_DEBUG && defined( _WIN32 ) && !defined(HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \b strings of the \aliblink which
 * holds types of library module \alibmod_nolink_strings.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_strings "ALib Module Strings - Programmer's Manual".
 **************************************************************************************************/
namespace strings {

#if !ALIB_DOCUMENTATION_PARSER


// #################################################################################################
// dbgCheck()
// #################################################################################################
#if ALIB_STRINGS_DEBUG

#if !ALIB_DEBUG
    #pragma message "Compiler symbol ALIB_STRINGS_DEBUG_ON set, while ALIB_DEBUG is off. Is this really wanted?"
#endif

namespace {
    bool astringCheckReported= false;
}

template<typename TChar>
void TString<TChar>::dbgCheck() const
{
    // write to the console once that we are debugging AString
    if ( !astringCheckReported )
    {
        astringCheckReported= true;
        ALIB_MESSAGE( "ALIB_STRINGS_DEBUG is ON! To switch off, "
                      "unset compilation symbol ALIB_STRINGS_DEBUG_ON."  );
    }

    ALIB_ASSERT_ERROR( length == 0 ||  buffer != nullptr,
                       "Nulled string has a length of ", length );

    for (integer i= length -1 ; i >= 0 ; i--)
        if ( buffer[i] == '\0' )
        {
            ALIB_ERROR( "Found termination character '\\0' in buffer. Index=", i )
            break;
        }
}

#endif


// #################################################################################################
// indexOf...()
// #################################################################################################
template<typename TChar>
template<Case TSensitivity>
integer  TString<TChar>::indexOfString( const TString<TChar>&  needle, integer startIdx )  const
{
    integer nLen=   needle.Length();
    if( nLen == 0 )
        return startIdx;
    const TChar*  buf=    buffer + startIdx;
    const TChar*  bufEnd= buffer + length - nLen + 1;
    const TChar* nBuf=    needle.Buffer();
    const TChar* nBufEnd= nBuf + nLen;

    while ( buf <= bufEnd )
    {
        const TChar* b=  buf;
        const TChar* n= nBuf;
        while ( characters::CharArray<TChar>::template Equal<TSensitivity>(*b++, *n++ ) )
            if( n == nBufEnd )
                return buf  - buffer;
        buf++;
    }
    return -1;
}

template<typename TChar>
integer  TString<TChar>::IndexOfSegmentEnd( TChar opener, TChar closer, integer  idx ) const
{
    int openCnt= 1;
    while( idx < length )
    {
        if( buffer[idx] == opener )   openCnt++;
        if( buffer[idx] == closer )
            if( --openCnt == 0 )
                break;
        idx++;
    }

    return openCnt == 0 ? idx : -openCnt;
}

// #################################################################################################
// ParseXXX()
// #################################################################################################
template<typename TChar>
uint64_t    TString<TChar>::ParseDecDigits( integer  startIdx, integer* newIdx )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseDecDigits<TChar>( *this, *indexPointer );
}


template<typename TChar>
int64_t    TString<TChar>::ParseInt( integer  startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseInt( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                           : *numberFormat );
}

template<typename TChar>
uint64_t    TString<TChar>::ParseDec( integer  startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseDec( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                           : *numberFormat );
}

template<typename TChar>
uint64_t    TString<TChar>::ParseBin( integer startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx  )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseBin( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                           : *numberFormat );
}

template<typename TChar>
uint64_t    TString<TChar>::ParseHex( integer startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx  )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseHex( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                           : *numberFormat );
}

template<typename TChar>
uint64_t    TString<TChar>::ParseOct( integer startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx  )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseOct( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                           : *numberFormat );
}

template<typename TChar>
double    TString<TChar>::ParseFloat( integer startIdx, TNumberFormat<TChar>* numberFormat, integer* newIdx )
const
{
    // we need an index pointer reference. So if none was given in parameter newIdx
    // then we just use parameter startIdx!
    integer* indexPointer;
    if ( newIdx == nullptr )
        indexPointer= &startIdx;
    else
    {
        indexPointer=  newIdx;
        *indexPointer= startIdx;
    }

    return detail::ParseFloat( *this, *indexPointer, numberFormat == nullptr ? TNumberFormat<TChar>::Computational
                                                                             : *numberFormat );
}


// #################################################################################################
// NString
// #################################################################################################
template<>
integer  TString<nchar>::WStringLength()  const
{
    if ( length == 0 )
        return 0;

    const nchar* cs= buffer;

    //--------- __GLIBCXX__ Version ---------
    #if defined (__GLIBCXX__) || defined(__APPLE__)

        size_t conversionSize=  mbsnrtowcs( nullptr, &cs, static_cast<size_t>(length), 0, nullptr );
        if ( conversionSize ==  static_cast<size_t>(-1) )
        {
            ALIB_WARNING( "MBCS to WCS conversion failed. Illegal MBC sequence" )
            return -1;
        }

        ALIB_ASSERT_ERROR( conversionSize <= static_cast<size_t>( length ),
                           "MBCS to WCS conversion failed. Conversion length=",
                           static_cast<integer>(conversionSize) )

        return static_cast<integer>(conversionSize);

    //--------- Windows Version ----------
    #elif defined( _WIN32 )

        int conversionSize= MultiByteToWideChar( CP_UTF8, NULL, cs, static_cast<int>( length ), nullptr, 0 );

        // check for errors
        #if ALIB_DEBUG
            if ( conversionSize == 0 )
            {
                // not enough space?
                int error= GetLastError();

                NString128 msg( "AString: Conversion to wide character string failed (Error: " );
                     if ( error == ERROR_INSUFFICIENT_BUFFER      ) msg._( "ERROR_INSUFFICIENT_BUFFER."   );
                else if ( error == ERROR_INVALID_FLAGS            ) msg._( "ERROR_INVALID_FLAGS."         );
                else if ( error == ERROR_INVALID_PARAMETER        ) msg._( "ERROR_INVALID_PARAMETER"      );
                else if ( error == ERROR_NO_UNICODE_TRANSLATION   ) msg._( "ERROR_NO_UNICODE_TRANSLATION" );
                else                                                msg._( error );
                msg._(')');

                ALIB_WARNING( msg )
                return -1;
            }

            if( conversionSize > length )
            {
                ALIB_ERROR( "MBCS to WCS conversion failed. Conversion length=", conversionSize );
                return -1;
            }
        #endif

        return conversionSize;

    #else

        #pragma message ("Unknown Platform in file: " __FILE__ )
        return 0;

    #endif
}

template integer  TString<nchar>::indexOfString<Case::Sensitive>(const TString<nchar>&, integer)  const;
template integer  TString<nchar>::indexOfString<Case::Ignore   >(const TString<nchar>&, integer)  const;
template integer  TString<nchar>::IndexOfSegmentEnd( nchar opener, nchar closer, integer  idx )      const;
template uint64_t TString<nchar>::ParseDecDigits( integer, integer*                           ) const;
template  int64_t TString<nchar>::ParseInt      ( integer, TNumberFormat<nchar>*, integer* ) const;
template uint64_t TString<nchar>::ParseDec      ( integer, TNumberFormat<nchar>*, integer* ) const;
template uint64_t TString<nchar>::ParseBin      ( integer, TNumberFormat<nchar>*, integer* ) const;
template uint64_t TString<nchar>::ParseHex      ( integer, TNumberFormat<nchar>*, integer* ) const;
template uint64_t TString<nchar>::ParseOct      ( integer, TNumberFormat<nchar>*, integer* ) const;
template double   TString<nchar>::ParseFloat    ( integer, TNumberFormat<nchar>*, integer* ) const;


// #################################################################################################
// WString
// #################################################################################################
template integer  TString<wchar>::indexOfString<Case::Sensitive>(const TString<wchar>&, integer)  const;
template integer  TString<wchar>::indexOfString<Case::Ignore   >(const TString<wchar>&, integer)  const;
template integer  TString<wchar>::IndexOfSegmentEnd( wchar opener, wchar closer, integer  idx )     const;
template uint64_t TString<wchar>::ParseDecDigits( integer, integer*                           ) const;
template  int64_t TString<wchar>::ParseInt      ( integer, TNumberFormat<wchar>*, integer* ) const;
template uint64_t TString<wchar>::ParseDec      ( integer, TNumberFormat<wchar>*, integer* ) const;
template uint64_t TString<wchar>::ParseBin      ( integer, TNumberFormat<wchar>*, integer* ) const;
template uint64_t TString<wchar>::ParseHex      ( integer, TNumberFormat<wchar>*, integer* ) const;
template uint64_t TString<wchar>::ParseOct      ( integer, TNumberFormat<wchar>*, integer* ) const;
template double   TString<wchar>::ParseFloat    ( integer, TNumberFormat<wchar>*, integer* ) const;

// #################################################################################################
// XString
// #################################################################################################
template<>
integer  TString<xchar>::WStringLength()  const
{
    if ( length == 0 )
        return 0;

    const xchar* src=       buffer;
    const xchar* srcEnd=    buffer + length;
    integer result= 0;
#if ALIB_SIZEOF_WCHAR_T == 4

    // convert UTF16 to UTF32
    while (src < srcEnd)
    {
        const char32_t uc = static_cast<char32_t>( *src++ );
        if ((uc - 0xd800) >= 2048) // not surrogate
        {
            result++;
        }
        else
        {
            ALIB_ASSERT_ERROR(    src < srcEnd                                                   // has one more?
                               && ((uc                               & 0xfffffc00) == 0xd800)    // is low?
                               && ((static_cast<char32_t>( *src++ )  & 0xfffffc00) == 0xdc00),   // is high?
                               "Error decoding UTF16" )

            result++;
            src++;
        }
    }

#else

    // convert UTF32 to UTF16
    while (src < srcEnd)
    {
        uinteger uc= *src++;
        ALIB_ASSERT_ERROR(       uc <  0xd800
                            || ( uc >= 0xe000 && uc <= 0x10ffff ),
                            "Illegal unicode 32 bit codepoint"       )

        if( uc < 0x10000 )
        {
            result++;
        }
        else
        {
            uc-= 0x10000;
            result+= 2;
        }
    }

#endif

    return result;
}

template integer  TString<xchar>::indexOfString<Case::Sensitive>(const TString<xchar>&, integer)  const;
template integer  TString<xchar>::indexOfString<Case::Ignore   >(const TString<xchar>&, integer)  const;
template integer  TString<xchar>::IndexOfSegmentEnd( xchar opener, xchar closer, integer  idx )      const;
template uint64_t TString<xchar>::ParseDecDigits( integer, integer*                           ) const;
template  int64_t TString<xchar>::ParseInt      ( integer, TNumberFormat<xchar>*, integer* ) const;
template uint64_t TString<xchar>::ParseDec      ( integer, TNumberFormat<xchar>*, integer* ) const;
template uint64_t TString<xchar>::ParseBin      ( integer, TNumberFormat<xchar>*, integer* ) const;
template uint64_t TString<xchar>::ParseHex      ( integer, TNumberFormat<xchar>*, integer* ) const;
template uint64_t TString<xchar>::ParseOct      ( integer, TNumberFormat<xchar>*, integer* ) const;
template double   TString<xchar>::ParseFloat    ( integer, TNumberFormat<xchar>*, integer* ) const;


// #################################################################################################
// debug methods
// #################################################################################################
#if ALIB_STRINGS_DEBUG
template   void TString<nchar>::dbgCheck() const;
template   void TString<wchar>::dbgCheck() const;
template   void TString<xchar>::dbgCheck() const;
#endif


#endif // ALIB_DOCUMENTATION_PARSER

}}}// namespace [aworx::lib::strings]

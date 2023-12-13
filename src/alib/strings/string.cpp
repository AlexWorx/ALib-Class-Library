// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_DETAIL_NUMBERCONVERSION)
#   include "alib/strings/detail/numberconversion.hpp"
#endif

#if ALIB_DEBUG && !defined(HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif
#endif // !defined(ALIB_DOX)

using namespace aworx::lib::characters;

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \c strings of the \aliblink, which
 * holds types of library module \alib_strings_nl.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_strings "ALib Module Strings - Programmer's Manual".
 **************************************************************************************************/
namespace strings {

#if !defined(ALIB_DOX)


// #################################################################################################
// dbgCheck()
// #################################################################################################
#if ALIB_DEBUG_STRINGS

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
        ALIB_MESSAGE( "STRINGS", "ALIB_DEBUG_STRINGS is enabled")
    }

    ALIB_ASSERT_ERROR( length == 0 ||  buffer != nullptr, "STRINGS",
                       "Nulled string has a length of ", int(length) )

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    for (integer i= length -1 ; i >= 0 ; --i)
        if ( buffer[i] == '\0' )
        {
            ALIB_ERROR( "STRINGS", "Found termination character '\\0' in buffer. Index=", int(i) )
            break;
        }
    ALIB_WARNINGS_RESTORE
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
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const TChar*  buf=    buffer + startIdx;
    const TChar*  bufEnd= buffer + length - nLen + 1;
    const TChar* nBuf=    needle.Buffer();
    const TChar* nBufEnd= nBuf + nLen;

    while ( buf < bufEnd )
    {
        const TChar* b=  buf;
        const TChar* n= nBuf;
        while ( CharArray<TChar>::template Equal<TSensitivity>(*b++, *n++ ) )
            if( n == nBufEnd )
                return buf  - buffer;
        ++buf;
    }
    return -1;
    ALIB_WARNINGS_RESTORE
}

template<typename TChar>
integer  TString<TChar>::IndexOfSegmentEnd( TChar opener, TChar closer, integer  idx ) const
{
    int openCnt= 1;
    while( idx < length )
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if( buffer[idx] == opener )   ++openCnt;
        if( buffer[idx] == closer )
            if( --openCnt == 0 )
                break;
        ++idx;
        ALIB_WARNINGS_RESTORE
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

    //--------- Windows Version ----------
    #if defined( _WIN32 )

        int conversionSize= MultiByteToWideChar( CP_UTF8, 0, cs, static_cast<int>( length ), nullptr, 0 );

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
                return length;
            }

            if( conversionSize > length )
            {
                ALIB_ERROR( "STRINGS", "MBCS to WCS conversion failed. Conversion length=", conversionSize );
                return length;
            }
        #endif

        return conversionSize;

    //--------- __GLIBCXX__ Version ---------
    #elif defined (__GLIBCXX__) || defined(__APPLE__) || defined(__ANDROID_NDK__)

        size_t conversionSize=  mbsnrtowcs( nullptr, &cs, static_cast<size_t>(length), 0, nullptr );
        if ( conversionSize ==  static_cast<size_t>(-1) )
        {
            ALIB_WARNING( "STRINGS", "MBCS to WCS conversion failed. Illegal MBC sequence. Probably UTF-8 is not set in locale" )
            return length;
        }

        ALIB_ASSERT_ERROR( conversionSize <= static_cast<size_t>( length ), "STRINGS",
                           NString128( "MBCS to WCS conversion failed. Conversion length=")
                           << conversionSize )

        return static_cast<integer>(conversionSize);

    #else

        #pragma message ("Unknown Platform in file: " __FILE__ )
        return length;

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

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    const xchar* src=       buffer;
    const xchar* srcEnd=    buffer + length;
    integer result= 0;
#if ALIB_SIZEOF_WCHAR_T == 4

    // convert UTF16 to UTF32
    while (src < srcEnd)
    {
        const char32_t uc = static_cast<char32_t>( *src++ );
        if ((uc - 0xd800) >= 2048) // not surrogate
            ++result;
        else
        {
            ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
            ALIB_ASSERT_ERROR(    src < srcEnd                                                   // has one more?
                               && ((uc                               & 0xfffffc00) == 0xd800)    // is low?
                               && ((static_cast<char32_t>( *src++ )  & 0xfffffc00) == 0xdc00),   // is high?
                               "STRINGS", "Error decoding UTF16" )

            ++result;
            ++src;
            ALIB_WARNINGS_RESTORE
        }
    }

#else

    // convert UTF32 to UTF16
    while (src < srcEnd)
    {
        uinteger uc= *src++;
        ALIB_ASSERT_ERROR(       uc <  0xd800
                            || ( uc >= 0xe000 && uc <= 0x10ffff ), "STRINGS",
                            "Illegal unicode 32 bit codepoint"       )

        if( uc < 0x10000 )
            ++result;
        else
        {
            uc-= 0x10000;
            result+= 2;
        }
    }

#endif

    return result;
}

template integer  TString<xchar>::indexOfString<Case::Sensitive>(const TString<xchar>&, integer) const;
template integer  TString<xchar>::indexOfString<Case::Ignore   >(const TString<xchar>&, integer) const;
template integer  TString<xchar>::IndexOfSegmentEnd( xchar opener, xchar closer, integer  idx  ) const;
template uint64_t TString<xchar>::ParseDecDigits( integer, integer*                            ) const;
template  int64_t TString<xchar>::ParseInt      ( integer, TNumberFormat<xchar>*, integer*     ) const;
template uint64_t TString<xchar>::ParseDec      ( integer, TNumberFormat<xchar>*, integer*     ) const;
template uint64_t TString<xchar>::ParseBin      ( integer, TNumberFormat<xchar>*, integer*     ) const;
template uint64_t TString<xchar>::ParseHex      ( integer, TNumberFormat<xchar>*, integer*     ) const;
template uint64_t TString<xchar>::ParseOct      ( integer, TNumberFormat<xchar>*, integer*     ) const;
template double   TString<xchar>::ParseFloat    ( integer, TNumberFormat<xchar>*, integer*     ) const;

// #################################################################################################
// Hashcode
// #################################################################################################
template<typename TChar>
std::size_t TString<TChar>::Hashcode()                                                         const
{
    #if ALIB_CPPVER < 17
        std::size_t result=
        #if ALIB_SIZEOF_INTEGER == 4
                                      2364114217ul;
        #else
                            4611686018427387847ull;
        #endif

        for (aworx::integer i = 0; i < length; ++i)
        {
            result^= static_cast<std::size_t>( buffer[i] ) * 33;
            result^= (result << 21 );
            result^= (result >> 11);
        }
        return result;
    #else
        return std::hash<std::basic_string_view<TChar>>()(
               std::basic_string_view<TChar>( Buffer(), static_cast<size_t>( Length() ) ) );
    #endif
}


template std::size_t TString<nchar>::Hashcode          () const;
template std::size_t TString<wchar>::Hashcode          () const;
template std::size_t TString<xchar>::Hashcode          () const;

template<typename TChar>
std::size_t TString<TChar>::HashcodeIgnoreCase()                                               const
{
    std::size_t result= 68460391ul * ( static_cast<size_t>(length) + 1 );

    for (aworx::integer i = 0; i != length; ++i)
        result = 199ul*result + static_cast<std::size_t>( CharArray<TChar>::ToUpper( buffer[i] ) );

    return result;
}
template std::size_t TString<nchar>::HashcodeIgnoreCase() const;
template std::size_t TString<wchar>::HashcodeIgnoreCase() const;
template std::size_t TString<xchar>::HashcodeIgnoreCase() const;

// #################################################################################################
// debug methods
// #################################################################################################
#if ALIB_DEBUG_STRINGS
template   void TString<nchar>::dbgCheck() const;
template   void TString<wchar>::dbgCheck() const;
template   void TString<xchar>::dbgCheck() const;
#endif


#endif // defined(ALIB_DOX)

}}// namespace aworx[::lib::strings]

// #################################################################################################
// String constants
// #################################################################################################
String              NULL_STRING             = nullptr;
ComplementString    NULL_COMPLEMENT_STRING  = nullptr;
StrangeString       NULL_STRANGE_STRING     = nullptr;
NString             NULL_N_STRING           = nullptr;
WString             NULL_W_STRING           = nullptr;
XString             NULL_X_STRING           = nullptr;
CString             EMPTY_STRING            =  A_CHAR ("");
ComplementCString   EMPTY_COMPLEMENT_STRING = A_CCHAR ("");
StrangeCString      EMPTY_STRANGE_STRING    = A_SCHAR ("");
NCString            EMPTY_N_STRING          = A_NCHAR ("");
WCString            EMPTY_W_STRING          = A_WCHAR ("");
XString             EMPTY_X_STRING          = A_XCHAR ("");

}// namespace [aworx]

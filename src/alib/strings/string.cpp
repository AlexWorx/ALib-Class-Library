// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Strings;
    import   ALib.Lang;
#else
#   include "ALib.Lang.H"
#   include "ALib.Strings.H"
#endif
// ======================================   Implementation   =======================================
using namespace alib::characters;

namespace alib {

//==================================================================================================
/// This is the reference documentation of sub-namespace \c strings of the \aliblink, which
/// holds types of library module \alib_strings_nl.
///
/// Extensive documentation for this module is provided with
/// \ref alib_mod_strings "ALib Module Strings - Programmer's Manual".
//==================================================================================================
namespace strings {

#if !DOXYGEN


// #################################################################################################
// dbgCheck()
// #################################################################################################
#if ALIB_DEBUG_STRINGS

namespace {
    bool astringCheckReported= false;
}

template<typename TChar>
requires alib::characters::IsCharacter<TChar>
void TString<TChar>::dbgCheck() const
{
    // write to the console once that we are debugging AString
    if ( !astringCheckReported )
    {
        astringCheckReported= true;
        ALIB_MESSAGE( "STRINGS", "ALIB_DEBUG_STRINGS is enabled" )
    }

    ALIB_ASSERT_ERROR( length == 0 ||  buffer != nullptr, "STRINGS",
                       "Nulled string has a length of ", length )

    for (integer i= length -1 ; i >= 0 ; --i)
        if ( buffer[i] == '\0' )
        {
            ALIB_ERROR( "STRINGS", "Found termination character '\\0' in buffer. Index=", i )
            break;
        }
}

#endif


// #################################################################################################
// indexOf...()
// #################################################################################################
template<typename   TChar>
requires alib::characters::IsCharacter<TChar>
template<lang::Case TSensitivity>
integer  TString<TChar>::indexOfString( const TString<TChar>&  needle,
                                        integer startIdx, integer endIdx )  const
{
    integer nLen=   needle.Length();
    if( nLen == 0 )
        return startIdx;
    ALIB_ASSERT_ERROR( startIdx >= 0 && startIdx <= length, "STRINGS",
                       "Illegal start index given: 0 <= {} < {}.", startIdx, length )
    ALIB_ASSERT_ERROR(   endIdx <= length -nLen + 1       , "STRINGS",
                       "Illegal end index given: {} > {}.", endIdx, length -nLen + 1 )
    const TChar*  buf=    buffer + startIdx;
    const TChar*  bufEnd= buffer + endIdx;
    const TChar* nBuf=    needle.Buffer();
    const TChar* nBufEnd= nBuf + nLen;

    while ( buf < bufEnd )
    {
        const TChar* b=  buf;
        const TChar* n= nBuf;
        while ( characters::Equal<TChar,TSensitivity>(*b++, *n++ ) )
            if( n == nBufEnd )
                return buf  - buffer;
        ++buf;
    }
    return -1;
}

template<typename TChar>
requires alib::characters::IsCharacter<TChar>
integer  TString<TChar>::IndexOfSegmentEnd( TChar opener, TChar closer, integer  idx ) const
{
    int openCnt= 1;
    while( idx < length )
    {
        if( buffer[idx] == opener )   ++openCnt;
        if( buffer[idx] == closer &&  --openCnt == 0 )
            break;

        ++idx;
    }

    return openCnt == 0 ? idx : -openCnt;
}

// #################################################################################################
// ParseXXX()
// #################################################################################################
template<typename TChar>
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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
requires alib::characters::IsCharacter<TChar>
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

        int conversionSize= MultiByteToWideChar( CP_UTF8, 0, cs, int( length ), nullptr, 0 );

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

                ALIB_WARNING( "STRINGS", msg )
                return length;
            }

            if( conversionSize > length )
            {
                ALIB_ERROR( "STRINGS", "MBCS to WCS conversion failed. Conversion length=",
                                       conversionSize );
                return length;
            }
        #endif

        return conversionSize;

    //--------- __GLIBCXX__ Version ---------
    #elif defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__) || defined(__ANDROID_NDK__)

        size_t conversionSize=  mbsnrtowcs( nullptr, &cs, size_t(length), 0, nullptr );
        if ( conversionSize ==  size_t(-1) )
        {
            ALIB_WARNING( "STRINGS", "MBCS to WCS conversion failed. Illegal MBC sequence. "
                                     "Probably UTF-8 is not set in locale" )
            return length;
        }

        ALIB_ASSERT_ERROR( conversionSize <= size_t( length ), "STRINGS",
                           "MBCS to WCS conversion failed. Conversion length=", conversionSize )

        return integer(conversionSize);

    #else

        #pragma message ("Unknown Platform in file: " __FILE__ )
        return length;

    #endif
}

template integer  TString<nchar>::indexOfString<lang::Case::Sensitive>(const TString<nchar>&,integer,integer) const;
template integer  TString<nchar>::indexOfString<lang::Case::Ignore   >(const TString<nchar>&,integer,integer) const;
template integer  TString<nchar>::IndexOfSegmentEnd( nchar opener, nchar closer, integer  idx               ) const;
template uint64_t TString<nchar>::ParseDecDigits( integer, integer*                                         ) const;
template  int64_t TString<nchar>::ParseInt      ( integer, TNumberFormat<nchar>*            , integer*      ) const;
template uint64_t TString<nchar>::ParseDec      ( integer, TNumberFormat<nchar>*            , integer*      ) const;
template uint64_t TString<nchar>::ParseBin      ( integer, TNumberFormat<nchar>*            , integer*      ) const;
template uint64_t TString<nchar>::ParseHex      ( integer, TNumberFormat<nchar>*            , integer*      ) const;
template uint64_t TString<nchar>::ParseOct      ( integer, TNumberFormat<nchar>*            , integer*      ) const;
template double   TString<nchar>::ParseFloat    ( integer, TNumberFormat<nchar>*            , integer*      ) const;


// #################################################################################################
// WString
// #################################################################################################
template integer  TString<wchar>::indexOfString<lang::Case::Sensitive>(const TString<wchar>&,integer,integer) const;
template integer  TString<wchar>::indexOfString<lang::Case::Ignore   >(const TString<wchar>&,integer,integer) const;
template integer  TString<wchar>::IndexOfSegmentEnd( wchar opener, wchar closer             , integer  idx  ) const;
template uint64_t TString<wchar>::ParseDecDigits( integer, integer*                                         ) const;
template  int64_t TString<wchar>::ParseInt      ( integer, TNumberFormat<wchar>*            , integer*      ) const;
template uint64_t TString<wchar>::ParseDec      ( integer, TNumberFormat<wchar>*            , integer*      ) const;
template uint64_t TString<wchar>::ParseBin      ( integer, TNumberFormat<wchar>*            , integer*      ) const;
template uint64_t TString<wchar>::ParseHex      ( integer, TNumberFormat<wchar>*            , integer*      ) const;
template uint64_t TString<wchar>::ParseOct      ( integer, TNumberFormat<wchar>*            , integer*      ) const;
template double   TString<wchar>::ParseFloat    ( integer, TNumberFormat<wchar>*            , integer*      ) const;

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
            ++result;
        else
        {
            ALIB_ASSERT_ERROR(    src < srcEnd                                    // has one more?
                               && ((uc                & 0xfffffc00) == 0xd800)    // is low?
                               && ((char32_t(*src++)  & 0xfffffc00) == 0xdc00),   // is high?
                               "STRINGS", "Error decoding UTF16" )

            ++result;
            ++src;
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

template integer  TString<xchar>::indexOfString<lang::Case::Sensitive>(const TString<xchar>&,integer,integer) const;
template integer  TString<xchar>::indexOfString<lang::Case::Ignore   >(const TString<xchar>&,integer,integer) const;
template integer  TString<xchar>::IndexOfSegmentEnd( xchar opener, xchar closer             ,integer        ) const;
template uint64_t TString<xchar>::ParseDecDigits( integer, integer*                                         ) const;
template  int64_t TString<xchar>::ParseInt      ( integer, TNumberFormat<xchar>*            , integer*      ) const;
template uint64_t TString<xchar>::ParseDec      ( integer, TNumberFormat<xchar>*            , integer*      ) const;
template uint64_t TString<xchar>::ParseBin      ( integer, TNumberFormat<xchar>*            , integer*      ) const;
template uint64_t TString<xchar>::ParseHex      ( integer, TNumberFormat<xchar>*            , integer*      ) const;
template uint64_t TString<xchar>::ParseOct      ( integer, TNumberFormat<xchar>*            , integer*      ) const;
template double   TString<xchar>::ParseFloat    ( integer, TNumberFormat<xchar>*            , integer*      ) const;

// #################################################################################################
// Hashcode
// #################################################################################################
template<typename TChar>
requires alib::characters::IsCharacter<TChar>
std::size_t TString<TChar>::Hashcode()                                                         const
{
    return std::hash<std::basic_string_view<TChar>>()(
           std::basic_string_view<TChar>( Buffer(), size_t( Length() ) ) );
}


template std::size_t TString<nchar>::Hashcode          () const;
template std::size_t TString<wchar>::Hashcode          () const;
template std::size_t TString<xchar>::Hashcode          () const;

template<typename TChar>
requires alib::characters::IsCharacter<TChar>
std::size_t TString<TChar>::HashcodeIgnoreCase()                                               const
{
    std::size_t result= 68460391ul * ( size_t(length) + 1 );

    auto* begin= buffer;
    auto* end  = buffer + length;
    while( begin != end )
        result = 199ul * result + std::size_t( ToUpper<TChar>( *begin++ ) );

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


#endif // DOXYGEN

} // namespace alib[::strings]
} // namespace [alib]


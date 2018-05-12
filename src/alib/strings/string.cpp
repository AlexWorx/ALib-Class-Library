// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif


namespace aworx { namespace lib { namespace strings {

//! @cond NO_DOX


// #################################################################################################
// _dbgCheck()
// #################################################################################################
#if ALIB_DEBUG_STRINGS

#if !ALIB_DEBUG
    #pragma message "Compiler symbol ALIB_DEBUG_STRINGS_ON set, while ALIB_DEBUG is off. Is this really wanted?"
#endif

extern bool _astringCheckReported;
bool _astringCheckReported= false;

template<typename TChar>
void StringBase<TChar>::_dbgCheck() const
{
    // write to the console once that we are debugging AString
    if ( !_astringCheckReported && lib::ALIB.IsInitialized() )
    {
        _astringCheckReported= true;
        ALIB_WARNING( ASTR("ALIB_DEBUG_STRINGS is ON! To switch off, "
                           "unset compilation symbol ALIB_DEBUG_STRINGS_ON." ) );
    }

    ALIB_ASSERT_ERROR(      length  == 0
                        ||  buffer != nullptr   ,ASTR("Not allocated but length >0") );

    for (integer aworx_astring_dbg_i= length -1 ; aworx_astring_dbg_i >= 0 ; aworx_astring_dbg_i--)
        if ( buffer[aworx_astring_dbg_i] == '\0' )
        {
            ALIB_ERROR( ASTR("Found termination character '\\0' in buffer. Index="), aworx_astring_dbg_i );
            break;
        }
}

#endif


// #################################################################################################
// indexOfString()
// #################################################################################################
namespace {
template<typename TChar, lang::Case>
                  bool compChar( TChar lhs, TChar rhs );
template<> inline bool compChar<nchar, lang::Case::Sensitive>( nchar lhs, nchar rhs )   {  return lhs == rhs;  }
template<> inline bool compChar<wchar, lang::Case::Sensitive>( wchar lhs, wchar rhs )   {  return lhs == rhs;  }
template<> inline bool compChar<nchar, lang::Case::Ignore   >( nchar lhs, nchar rhs )   {  return CString<nchar>::Toupper(lhs) == CString<nchar>::Toupper(rhs);  }
template<> inline bool compChar<wchar, lang::Case::Ignore   >( wchar lhs, wchar rhs )   {  return CString<wchar>::Toupper(lhs) == CString<wchar>::Toupper(rhs);  }
}

template<typename TChar>
template<lang::Case TSensitivity>
integer  StringBase<TChar>::indexOfString( const StringBase<TChar>&  needle, integer startIdx )  const
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
        while ( compChar<TChar,TSensitivity>(*b++, *n++ ) )
            if( n == nBufEnd )
                return buf  - buffer;
        buf++;
    }
    return -1;
}

// #################################################################################################
// ParseXXX()
// #################################################################################################
template<typename TChar>
uint64_t    StringBase<TChar>::ParseDecDigits( integer  startIdx, integer* newIdx )
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

    return NumberFormatBase<TChar>::ParseDecDigits( *this, *indexPointer );
}


template<typename TChar>
int64_t    StringBase<TChar>::ParseInt( integer  startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseInt( *this, *indexPointer );
}

template<typename TChar>
uint64_t    StringBase<TChar>::ParseDec( integer  startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseDec( *this, *indexPointer );
}

template<typename TChar>
uint64_t    StringBase<TChar>::ParseBin( integer startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx  )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseBin( *this, *indexPointer );
}

template<typename TChar>
uint64_t    StringBase<TChar>::ParseHex( integer startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx  )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseHex( *this, *indexPointer );
}

template<typename TChar>
uint64_t    StringBase<TChar>::ParseOct( integer startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx  )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseOct( *this, *indexPointer );
}

template<typename TChar>
double    StringBase<TChar>::ParseFloat( integer startIdx, NumberFormatBase<TChar>* numberFormat, integer* newIdx )
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

    return (numberFormat == nullptr ? &NumberFormatBase<TChar>::Computational
                                    : numberFormat )
           ->ParseFloat( *this, *indexPointer );
}


// #################################################################################################
// NString
// #################################################################################################
template<>
integer  NString::WStringLength()  const
{
    if ( length == 0 )
        return 0;

    const nchar* cs= buffer;

    //--------- __GLIBCXX__ Version ---------
    #if defined (__GLIBCXX__) || defined(__APPLE__)

        size_t conversionSize=  mbsnrtowcs( nullptr, &cs, static_cast<size_t>(length), 0, nullptr );
        if ( conversionSize ==  static_cast<size_t>(-1) )
        {
            ALIB_WARNING( ASTR("MBCS to WCS conversion failed. Illegal MBC sequence") );
            return -1;
        }

        ALIB_ASSERT_ERROR( conversionSize <= static_cast<size_t>( length ),
                           ASTR("MBCS to WCS conversion failed. Conversion length="),
                           static_cast<integer>(conversionSize) );

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

                String128 msg( "AString: Conversion to wide character string failed (Error: " );
                     if ( error == ERROR_INSUFFICIENT_BUFFER      ) msg._( "ERROR_INSUFFICIENT_BUFFER."   );
                else if ( error == ERROR_INVALID_FLAGS            ) msg._( "ERROR_INVALID_FLAGS."         );
                else if ( error == ERROR_INVALID_PARAMETER        ) msg._( "ERROR_INVALID_PARAMETER"      );
                else if ( error == ERROR_NO_UNICODE_TRANSLATION   ) msg._( "ERROR_NO_UNICODE_TRANSLATION" );
                else                                                msg._( error );
                msg._(')');

                ALIB_WARNING( msg );
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

template integer  StringBase<nchar >::indexOfString<Case::Sensitive>(const StringBase<nchar>&, integer)  const;
template integer  StringBase<nchar >::indexOfString<Case::Ignore   >(const StringBase<nchar>&, integer)  const;
template uint64_t StringBase<nchar >::ParseDecDigits( integer, integer*                           ) const;
template  int64_t StringBase<nchar >::ParseInt      ( integer, NumberFormatBase<nchar>*, integer* ) const;
template uint64_t StringBase<nchar >::ParseDec      ( integer, NumberFormatBase<nchar>*, integer* ) const;
template uint64_t StringBase<nchar >::ParseBin      ( integer, NumberFormatBase<nchar>*, integer* ) const;
template uint64_t StringBase<nchar >::ParseHex      ( integer, NumberFormatBase<nchar>*, integer* ) const;
template uint64_t StringBase<nchar >::ParseOct      ( integer, NumberFormatBase<nchar>*, integer* ) const;
template double   StringBase<nchar >::ParseFloat    ( integer, NumberFormatBase<nchar>*, integer* ) const;


// #################################################################################################
// WString
// #################################################################################################

template uint64_t StringBase<wchar>::ParseDecDigits( integer, integer*                           ) const;
template  int64_t StringBase<wchar>::ParseInt      ( integer, NumberFormatBase<wchar>*, integer* ) const;
template uint64_t StringBase<wchar>::ParseDec      ( integer, NumberFormatBase<wchar>*, integer* ) const;
template uint64_t StringBase<wchar>::ParseBin      ( integer, NumberFormatBase<wchar>*, integer* ) const;
template uint64_t StringBase<wchar>::ParseHex      ( integer, NumberFormatBase<wchar>*, integer* ) const;
template uint64_t StringBase<wchar>::ParseOct      ( integer, NumberFormatBase<wchar>*, integer* ) const;
template double   StringBase<wchar>::ParseFloat    ( integer, NumberFormatBase<wchar>*, integer* ) const;
template integer  StringBase<wchar>::indexOfString<Case::Sensitive>(const StringBase<wchar>&, integer)  const;
template integer  StringBase<wchar>::indexOfString<Case::Ignore   >(const StringBase<wchar>&, integer)  const;


// #################################################################################################
// debug methods
// #################################################################################################
#if ALIB_DEBUG
template<>   ALIB_WARN_ONCE_PER_TYPE_DEFINE( StringBase<nchar>, SetLengthLonger , true);
template<>   ALIB_WARN_ONCE_PER_TYPE_DEFINE( StringBase<wchar>, SetLengthLonger , true);
#endif

#if ALIB_DEBUG_STRINGS
template   void StringBase<nchar>::_dbgCheck() const;
template   void StringBase<wchar>::_dbgCheck() const;
#endif


//! @endcond


}}}// namespace [aworx::lib::strings]

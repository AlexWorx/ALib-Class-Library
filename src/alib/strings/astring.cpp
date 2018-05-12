// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if defined(_MSC_VER)
    #include <algorithm>
#endif

#include <codecvt>

namespace aworx { namespace lib { namespace strings {
// #################################################################################################
//  globals
// #################################################################################################

//! @cond NO_DOX
ALIB_DBG( void appendErrorToAString( AStringBase<nchar>& target ) { target._( "T_Apply<Unknown Type>");} )
ALIB_DBG( void appendErrorToAString( AStringBase<wchar>& target ) { target._(L"T_Apply<Unknown Type>");} )
//! @endcond


// ####################################################################################################
// AString::_dbgCheck()
// ####################################################################################################
//! @cond NO_DOX
#if ALIB_DEBUG_STRINGS

#if !ALIB_DEBUG
    #pragma message "Compiler symbol ALIB_DEBUG_STRINGS_ON set, while ALIB_DEBUG is off. Is this really wanted?"
#endif

template<typename TChar>
void AStringBase<TChar>::_dbgCheck() const
{
    TStringBase<TChar>::_dbgCheck();
    integer cap= Capacity();

    ALIB_ASSERT_ERROR(      StringBase<TChar>::buffer != nullptr
                            ||  StringBase<TChar>::length  == 0
                            ,ASTR("No buffer but length != 0") );

    ALIB_ASSERT_ERROR(      cap      == 0
                            ||  TStringBase<TChar>::debugIsTerminated != 0
                            ||  StringBase<TChar>::buffer[StringBase<TChar>::length]   == '\1'
                            ,ASTR("Not terminated but dbg-terminator character '\\1' not present") );

    ALIB_ASSERT_ERROR(      debugLastAllocRequest == 0
                            ||  StringBase<TChar>::length <= debugLastAllocRequest
                            ,ASTR("Error: Previous allocation request was too short")         );

    ALIB_ASSERT_ERROR(      StringBase<TChar>::length <= cap
                            ,ASTR("Error: Length greater than allocation size")               );

    if( StringBase<TChar>::buffer && debugBufferWithMagicBytePadding )
    {
        for (integer aworx_astring_dbg_i= -16 ; aworx_astring_dbg_i < 0 ; aworx_astring_dbg_i++)
            if ( StringBase<TChar>::buffer[aworx_astring_dbg_i] != 2 )
            {
                ALIB_ERROR( ASTR("Magic byte not found at start of buffer.") );
                break;
            }
        for (integer aworx_astring_dbg_i= 1 ; aworx_astring_dbg_i <= 16 ; aworx_astring_dbg_i++)
            if ( StringBase<TChar>::buffer[ cap + aworx_astring_dbg_i] != 3 )
            {
                ALIB_ERROR( ASTR("Magic byte not found at end of buffer.") );
                break;
            }
    }
}

#endif
//! @endcond



// ####################################################################################################
// Allocation
// ####################################################################################################
template<typename TChar>
void AStringBase<TChar>::SetBuffer( integer newCapacity )
{
    ALIB_STRING_DBG_CHK(this)

    ALIB_ASSERT( newCapacity >= 0 );

    // do nothing if life-cycle is managed by us and same size,
    if ( capacity >= 0 && capacity == newCapacity )
        return;

    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= newCapacity;
    #endif

    // set uninitialized (and return)
    if ( newCapacity == 0 )
    {
        if ( capacity > 0 )
            #if !ALIB_DEBUG_STRINGS
                delete[] StringBase<TChar>::buffer;
            #else
                delete[] (StringBase<TChar>::buffer - (debugBufferWithMagicBytePadding ? 16 : 0) );
            #endif

        capacity=
        StringBase<TChar>::length=     0;
        StringBase<TChar>::buffer=     nullptr;
        return;
    }

    ALIB_WARN_ONCE_IF( *this, ReplaceExternalBuffer, capacity < 0,
                      ASTR("AString::SetAllocation(): replacing an external buffer. This may not be wanted.") )

    // extend or shrink an existing buffer (and return)
    if( capacity > 0 )
    {
        #if !ALIB_DEBUG_STRINGS
            StringBase<TChar>::buffer= static_cast<TChar*>( std::realloc(  StringBase<TChar>::vbuffer,
                                                                           static_cast<size_t>(newCapacity  + 1)
                                                                           * sizeof( TChar )                       ) );
            #if ALIB_AVOID_ANALYZER_WARNINGS
                if( newCapacity > capacity )
                    CString<TChar>::Fill( StringBase<TChar>::vbuffer + StringBase<TChar>::length, '\0', newCapacity  + 1 - StringBase<TChar>::length);
            #endif
        #else
            // add 16 characters of padding at start/end
            StringBase<TChar>::buffer= static_cast<TChar*>( std::realloc( StringBase<TChar>::vbuffer - 16,
                                                                          static_cast<size_t>( newCapacity  + 1 + 33 )
                                                                          * sizeof( TChar ) ) ) + 16;

            // write '\3' to end ('\0'= termination byte, '\1'= untermination byte )
            CString<TChar>::Fill( StringBase<TChar>::vbuffer + newCapacity + 1, '\3', 16 );
        #endif

        capacity=   newCapacity;
        if ( StringBase<TChar>::length > capacity )
            StringBase<TChar>::length= capacity;

        ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
        #if ALIB_DEBUG_STRINGS
            debugBufferWithMagicBytePadding= true;
        #endif
        return;
    }

    // create new Buffer
    #if !ALIB_DEBUG_STRINGS
        TChar* newBuffer= static_cast<TChar*>( std::malloc(  static_cast<size_t>(newCapacity  + 1)
                                                           * sizeof( TChar )                       ) );
        #if ALIB_AVOID_ANALYZER_WARNINGS
            CString<TChar>::Fill( newBuffer, '\0', newCapacity  + 1 );
        #endif
    #else
        // add 16 characters of padding at start/end
        TChar* newBuffer= static_cast<TChar*>(   std::malloc(static_cast<size_t>( newCapacity  + 1 + 33 )
                                               * sizeof( TChar ) ) ) + 16;

        // write '\2' to start, '\3' to end ('\0'= termination byte, '\1'= untermination byte )
        CString<TChar>::Fill( newBuffer - 16,              '\2', 16 );
        CString<TChar>::Fill( newBuffer + newCapacity + 1, '\3', 16 );
    #endif

    // if we had a buffer before
    if ( capacity != 0 )
    {
        // copy data and delete old buffer
        CString<TChar>::Copy( StringBase<TChar>::buffer, (std::min)( StringBase<TChar>::length + 1, newCapacity + 1),
                    newBuffer );
        if ( capacity > 0 )
          #if !ALIB_DEBUG_STRINGS
            delete[] StringBase<TChar>::buffer;
          #else
            delete[] (StringBase<TChar>::buffer - (debugBufferWithMagicBytePadding ? 16 : 0) );
          #endif
    }
    else
    {
        ALIB_ASSERT( StringBase<TChar>::length == 0 );
    }

    // set new Buffer and adjust length
    StringBase<TChar>::buffer=     newBuffer;
    capacity=   newCapacity;
    if ( StringBase<TChar>::length > capacity )
        StringBase<TChar>::length= capacity;

    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
    #if ALIB_DEBUG_STRINGS
        debugBufferWithMagicBytePadding= true;
    #endif
}


template<typename TChar>
void AStringBase<TChar>::SetBuffer( TChar* extBuffer, integer extBufferSize, integer extLength,
                                    Responsibility responsibility  )
{
    ALIB_ASSERT_ERROR(    !(extBufferSize == 0 && extBuffer != nullptr)
                       && !(extBufferSize != 0 && extBuffer == nullptr) ,
                       ASTR("AString::SetBuffer(): Given buffer is nullptr while given alloc size is not 0 (or vice versa)") );

    // delete any existing
    if ( capacity > 0 )
    {
        #if !ALIB_DEBUG_STRINGS
            delete[] StringBase<TChar>::buffer;
        #else
            delete[] (StringBase<TChar>::buffer - (debugBufferWithMagicBytePadding ? 16 : 0) );
        #endif
    }

    // too small? treat as if a nullptr was given.
    if ( extBufferSize < 1 )
    {
        ALIB_ERROR( ASTR("allocation size < 1") );
        extBuffer= nullptr;
    }

    // null buffer?
    if ( (StringBase<TChar>::buffer= extBuffer) == nullptr )
    {
        #if ALIB_DEBUG_STRINGS
            debugLastAllocRequest=
        #endif
        capacity=
        StringBase<TChar>::length=       0;
        return;
    }


    if ( extLength >= extBufferSize  )
    {
        ALIB_ERROR( ASTR("ext length >= ext allocation size") );
        extLength= extBufferSize -1;
    }


    // save given buffer
    extBufferSize--;     // we count one less
    capacity=   responsibility==Responsibility::Transfer ?  extBufferSize
                                                         : -extBufferSize;
    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= extBufferSize;
    #endif
    StringBase<TChar>::length=     extLength;

    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
}



// #############################################################################################
// Trim
// #############################################################################################

template<typename TChar>
integer AStringBase<TChar>::TrimAt( integer idx, const TStringBase<TChar>& trimChars )
{
    if ( idx < 0 )
         return 0;
    if ( idx >= StringBase<TChar>::length )
         return StringBase<TChar>::length;

    integer regionStart=  TStringBase<TChar>::template LastIndexOfAny<Inclusion::Exclude, false>( trimChars, idx ) + 1;
    if (regionStart < 0 )
        regionStart= 0;

    integer regionEnd=    TStringBase<TChar>::template IndexOfAny    <Inclusion::Exclude, false>( trimChars, idx );
    if (regionEnd < 0 )
        regionEnd= StringBase<TChar>::length;

    integer regionLength= regionEnd - regionStart;
    if ( regionLength > 0 )
        Delete<false>( regionStart, regionLength );

    return regionStart;
}

template<typename TChar>
AStringBase<TChar>& AStringBase<TChar>::Trim( const TStringBase<TChar>& trimChars )
{
    // check
    if (StringBase<TChar>::length == 0 || trimChars.IsEmpty() )
        return *this;

    // trim end
    integer idx= TStringBase<TChar>::template LastIndexOfAny<Inclusion::Exclude, false>( trimChars, StringBase<TChar>::length - 1 ) + 1;
    if ( (StringBase<TChar>::length= idx) > 0 )
    {
        // trim front
        idx= TStringBase<TChar>::template IndexOfAny<Inclusion::Exclude,false>( trimChars );
        if ( idx > 0 )
            Delete<false>( 0, idx );
    }

    ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0)
    return *this;
}


// #################################################################################################
//  Replace()
// #################################################################################################
template<typename TChar>
integer AStringBase<TChar>::SearchAndReplace(  TChar       needle,
                                    TChar       replacement,
                                    integer    startIdx        )
{
    ALIB_STRING_DBG_CHK(this)
         if ( startIdx < 0  )       startIdx= 0;
    else if ( startIdx >= StringBase<TChar>::length )  return 0;

    // replacement loop
    integer cntReplacements=    0;
    do
    {
        startIdx= TStringBase<TChar>::template IndexOfOrLength<false>( needle, startIdx );
        if ( startIdx == StringBase<TChar>::length  )
            break;
        StringBase<TChar>::vbuffer[ startIdx ]= replacement;
        cntReplacements++;
    }
    while(  ++startIdx < StringBase<TChar>::length ) ;
    return cntReplacements;
}


template<typename TChar>
integer AStringBase<TChar>::SearchAndReplace(  const TStringBase<TChar>&  needle,
                                               const  StringBase<TChar>&  replacement,
                                               integer                    startIdx,
                                               integer                    maxReplacements,
                                               Case                       sensitivity        )
{
    ALIB_STRING_DBG_CHK(this)

    // check null arguments
    if ( needle.IsEmpty() )
        return 0;

    integer nLen=    needle.Length();
    integer rLen=    replacement.Length();
    integer lenDiff= rLen - nLen;

    // terminate needle
    needle.Terminate();

    // replacement loop
    integer cntReplacements=    0;
    while ( cntReplacements < maxReplacements && startIdx < StringBase<TChar>::length)
    {
        // search  next occurrence
        ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
        integer    idx= ( sensitivity == Case::Sensitive ? TStringBase<TChar>::template IndexOf<false, Case::Sensitive>( needle, startIdx  )
                                                         : TStringBase<TChar>::template IndexOf<false, Case::Ignore   >( needle, startIdx  ) );
        if ( idx < 0 )
            break;

        // copy rest up or down
        if ( lenDiff != 0 )
        {
            if ( lenDiff > 0 )
                EnsureRemainingCapacity( lenDiff );
            CString<TChar>::Move( StringBase<TChar>::vbuffer + idx + nLen,
                                           StringBase<TChar>::length  - idx - nLen,
                                           StringBase<TChar>::vbuffer + idx + nLen + lenDiff );
            StringBase<TChar>::length+= lenDiff;
            ALIB_STRING_DBG_UNTERMINATE(TChar,*this, 0);
        }

        // fill replacement in
        if( rLen > 0 )
            CString<TChar>::Copy( replacement.Buffer(), rLen, StringBase<TChar>::vbuffer + idx );

        // set start index to first character behind current replacement
        startIdx= idx + rLen;

        // next
        cntReplacements++;
    }


    // that's it
    return cntReplacements;
}


//! @cond NO_DOX

#if ALIB_DEBUG_STRINGS
template   void AStringBase<nchar>::_dbgCheck() const;
template   void AStringBase<wchar>::_dbgCheck() const;
#endif

// #################################################################################################
// #################################################################################################
// NString specific implementations
// #################################################################################################
// #################################################################################################

template  void                AStringBase<nchar>::SetBuffer       (integer);
template  void                AStringBase<nchar>::SetBuffer       (nchar*,integer,integer,lang::Responsibility);
template  integer             AStringBase<nchar>::SearchAndReplace(nchar,nchar,integer );
template  integer             AStringBase<nchar>::SearchAndReplace(const TStringBase<nchar>&,const StringBase<nchar>&,integer,integer,lang::Case );
template  AStringBase<nchar>& AStringBase<nchar>::Trim            (const TStringBase<nchar>& );
template  integer             AStringBase<nchar>::TrimAt          (integer,const TStringBase<nchar>& );

template<>
template<>
AStringBase<nchar>& AStringBase<nchar>::Append<false>( const wchar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )
    ALIB_ASSERT_ERROR( src || srcLength == 0,  ASTR("NC: nullptr passed") )

    //--------- __GLIBCXX__ Version ---------
    #if defined (__GLIBCXX__) || defined(__APPLE__)

        integer maxConversionSize= static_cast<integer>(MB_CUR_MAX) * srcLength;
        mbstate_t ps;
        EnsureRemainingCapacity( maxConversionSize );
        memset( &ps, 0, sizeof(mbstate_t) );
        const wchar* srcp= src;
        size_t conversionSize= wcsnrtombs( StringBase<nchar>::vbuffer + StringBase<nchar>::length, &srcp, static_cast<size_t>(srcLength), static_cast<size_t>(maxConversionSize),  &ps);
        if ( conversionSize == static_cast<size_t>( -1 ) )
        {
            ALIB_WARNING( ASTR("Cannot convert WCS to MBCS.") );
            return *this;
        }

        if ( conversionSize < 1 )
        {
            ALIB_ERROR( ASTR("Error converting WCS to MBCS.") );
            return *this;
        }

        SetLength<false>( StringBase<nchar>::length + static_cast<integer>(conversionSize) );
        return *this;

    //--------- Windows Version ---------
    #elif defined( _WIN32 )

        // loop until reserved size is big enough
        EnsureRemainingCapacity( srcLength * 2 );
        for(;;)
        {
            int conversionSize= WideCharToMultiByte( CP_UTF8, NULL,
                                                     src, static_cast<int>( srcLength),
                                                     StringBase<nchar>::vbuffer + StringBase<nchar>::length, static_cast<int>( Capacity() - StringBase<nchar>::length ),
                                                     NULL, NULL );
            if ( conversionSize > 0 )
            {
                SetLength<false>( StringBase<nchar>::length + conversionSize );
                return *this;
            }

            // not enough space?
            int error= GetLastError();
            if (error == ERROR_INSUFFICIENT_BUFFER )
            {
                EnsureRemainingCapacity( srcLength );
                continue;
            }

            // quit on other errors
            ALIB_WARNING(
                     ASTR("AString: Cannot convert wide character string to UTF-8. Error: "),
                         ( error == ERROR_INVALID_FLAGS          ? ASTR("ERROR_INVALID_FLAGS.")
                        :  error == ERROR_INVALID_PARAMETER      ? ASTR("ERROR_INVALID_PARAMETER")
                        :  error == ERROR_NO_UNICODE_TRANSLATION ? ASTR("ERROR_NO_UNICODE_TRANSLATION")
                                                                 : (String32()._( error )).ToCString())
                    )

            return *this;
        }

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        return *this;
    #endif
}
template<>
template<>
AStringBase<nchar>& AStringBase<nchar>::Append<true>( const wchar* src, integer srcLength )
{
    if ( src == nullptr ||  srcLength <= 0 )
    {
        if ( StringBase<nchar>::IsNull() )
        {
            // special treatment if currently nothing is allocated and a blank string ("") is added:
            // we allocate, which means, we are not a nulled object anymore!
            // (...also, in this case we check the src parameter)
            SetBuffer( 15 );
            ALIB_STRING_DBG_UNTERMINATE(nchar,*this, 0);
        }

        return *this;
    }
    return Append<false>( src, srcLength );
}


template<>
template<>
AStringBase<nchar>& AStringBase<nchar>::Append<false>( const strangeChar* src, integer srcLength )
{
    // convert to wchar and invoke wchar version
    WString4K   converter;
    converter.Append( src, srcLength );
    Append( converter.Buffer(), converter.Length() );

    return *this;
}

template<>
template<>
AStringBase<nchar>& AStringBase<nchar>::Append<true> ( const strangeChar* src, integer srcLength )
{
    if ( src == nullptr ||  srcLength <= 0 )
    {
        if ( StringBase<nchar>::IsNull() )
        {
            // special treatment if currently nothing is allocated and a blank string ("") is added:
            // we allocate, which means, we are not a nulled object anymore!
            // (...also, in this case we check the src parameter)
            SetBuffer( 15 );
            ALIB_STRING_DBG_UNTERMINATE(nchar,*this, 0);
        }

        return *this;
    }

    // convert to wchar and invoke wchar version
    return Append<false>( src, srcLength );
}



// #################################################################################################
// #################################################################################################
// WString specific implementations
// #################################################################################################
// #################################################################################################
template  void                  AStringBase<wchar>::SetBuffer       (integer);
template  void                  AStringBase<wchar>::SetBuffer       (wchar*,integer,integer,lang::Responsibility);
template  integer               AStringBase<wchar>::SearchAndReplace(wchar,wchar,integer );
template  integer               AStringBase<wchar>::SearchAndReplace(const TStringBase<wchar>&,const StringBase<wchar>&,integer,integer,lang::Case );
template  AStringBase<wchar>&   AStringBase<wchar>::Trim            (const TStringBase<wchar>& );
template  integer               AStringBase<wchar>::TrimAt          (integer,const TStringBase<wchar>& );

template<>
template<>
AStringBase<wchar>& AStringBase<wchar>::Append<false>( const nchar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )

    // copy loop
    EnsureRemainingCapacity( srcLength );

    //--------- __GLIBCXX__ Version ---------
    #if defined (__GLIBCXX__) || defined(__APPLE__)

        while(srcLength > 0 )
        {
            integer actConversionLenght= srcLength;
            for( int pass= 0 ; pass < 2 ; ++pass )
            {

                mbstate_t    ps;    memset( &ps, 0, sizeof(mbstate_t) );
                const nchar* srcp= src;
                size_t       wcWritten= mbsnrtowcs( vbuffer + length,  &srcp,
                                                    static_cast<size_t>(actConversionLenght),
                                                    static_cast<size_t>(Capacity() - length), &ps );

                // single character failed?
                if( wcWritten == static_cast<size_t >(-1) )
                {
                    // already repeated?
                    // This can't (must not) happen! If it did, release code does infinite loop!
                    ALIB_ASSERT( pass == 0 )

                    // first character that failed?
                    if( srcp == src )
                    {
                        src++;
                        srcLength--;
                        *(vbuffer + length++)= '?';
                        break; // break try loop, continue with next character
                    }

                    // retry those characters that succeeded
                    actConversionLenght= srcp - src;
                    continue;
                }

                length+=    wcWritten;
                src+=       wcWritten;
                srcLength-= actConversionLenght;
                break;
            }
        }
        ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);
        return *this;

    //--------- Windows Version ----------
    #elif defined( _WIN32 )
        integer conversionSize= MultiByteToWideChar( CP_UTF8, NULL,
                                                     src, static_cast<int>( srcLength ),
                                                     vbuffer + length, static_cast<int>( Capacity() - length ) );
        // check for errors
        #if ALIB_DEBUG
            if ( conversionSize == 0 )
            {
                // not enough space?
                int error= GetLastError();

                ALIB_WARNING(
                    ( String128( "MBCS to WCS conversion failed (Error: " )._(
                          ( error == ERROR_INSUFFICIENT_BUFFER      ?  ASTR("ERROR_INSUFFICIENT_BUFFER."  )
                          : error == ERROR_INVALID_FLAGS            ?  ASTR("ERROR_INVALID_FLAGS."        )
                          : error == ERROR_INVALID_PARAMETER        ?  ASTR("ERROR_INVALID_PARAMETER"     )
                          : error == ERROR_NO_UNICODE_TRANSLATION   ?  ASTR("ERROR_NO_UNICODE_TRANSLATION")
                                                                    : ( String32()._( error ) ).ToCString() ) )
                        ._( ')' ) ) )
            }

            ALIB_ASSERT_ERROR( conversionSize <= srcLength,
                              String128( "MBCS to WCS conversion failed. Requested length=" )._( srcLength )
                                 ._( ", conversion length=" )._(conversionSize)
                            )
        #endif

        length+= conversionSize;
        ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);
        return *this;


    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        conversionSize= 0;
        return *this;
    #endif
}

template<>
template<>
AStringBase<wchar>& AStringBase<wchar>::Append<true>( const nchar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )

    if ( src == nullptr ||  srcLength <= 0 )
    {
        if ( StringBase<wchar>::IsNull() )
        {
            // special treatment if currently nothing is allocated and a blank string ("") is added:
            // we allocate, which means, we are not a nulled object anymore!
            // (...also, in this case we check the src parameter)
            SetBuffer( 15 );
            ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);
        }

        return *this;
    }

    return Append<false>(src, srcLength);
}


#if ALIB_SIZEOF_WCHAR_T == 4

    template<>
    template<>
    AStringBase<wchar>& AStringBase<wchar>::Append<false>( const strangeChar* src, integer srcLength )
    {
        EnsureRemainingCapacity( srcLength );

        // convert UTF16 to UTF32
        const strangeChar* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            const char32_t uc = *src++;
            if ((uc - 0xd800) >= 2048) // not surrogate
            {
                vbuffer[length++] = static_cast<wchar>(uc);
            }
            else
            {
                ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                   && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                   && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                   "Error decoding UTF16" )

                vbuffer[length++]=  static_cast<wchar>(     (uc << 10)
                                                         +  ((*src++) - 0x35fdc00 )    );
            }
        }
        ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);

        return *this;
    }

#else

    template<>
    template<>
    AStringBase<wchar>& AStringBase<wchar>::Append<false>( const strangeChar* src, integer srcLength )
    {
        // convert UTF32 to UTF16
        EnsureRemainingCapacity( srcLength * 2 );

        const strangeChar* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            uinteger uc= *src++;
            ALIB_ASSERT_ERROR(       uc <  0xd800
                                || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                ASTR("Illegal unicode 32 bit codepoint")       )

            if( uc < 0x10000 )
            {
                  vbuffer[length++]=  static_cast<wchar>( uc );
            }
            else
            {
                uc-= 0x10000;
                vbuffer[length++]= static_cast<wchar>(  ( uc >> 10    ) + 0xd800  );
                vbuffer[length++]= static_cast<wchar>(  ( uc &  0x3ff ) + 0xdc00  );
            }
        }
        ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);

        return *this;
    }

#endif

template<>
template<>
AStringBase<wchar>& AStringBase<wchar>::Append<true>( const strangeChar* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )
    if ( src == nullptr ||  srcLength <= 0 )
    {
        if ( StringBase<wchar>::IsNull() )
        {
            SetBuffer( 15 );
            ALIB_STRING_DBG_UNTERMINATE(wchar,*this, 0);
        }

        return *this;
    }

    return Append<false>( src, srcLength );
}


//! @cond NO_DOX


} // aworx::lib[::strings]

// #################################################################################################
// debug
// #################################################################################################
#if ALIB_DEBUG
//! @cond NO_DOX

namespace debug {

    AString&  RemoveALibNamespaces( AString& target, bool remove )
    {
        if( remove )
        {
            target.SearchAndReplace(ASTR("aworx::lib::boxing::ftypes::")        , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::boxing::")                , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::config::")                , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::debug::")                 , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::lang::")                  , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::strings::boxing::")       , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::strings::util::")         , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::strings::format::")       , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::strings::")               , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::system::")                , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::threads::")               , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::time::")                  , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::util::")                  , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::")                        , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::lox::core::textlogger::") , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::lox::core::")             , EmptyString );
            target.SearchAndReplace(ASTR("aworx::lib::lox::")                   , EmptyString );

            ALIB_ASSERT_ERROR( target.IndexOf(ASTR("aworx::lib")) < 0,
                               ASTR("Not all namespaces were fetched")    );

        }
        return target;
    }

    #if ALIB_FEAT_SINGLETON_MAPPED
        int GetSingletons( NAString& target )
        {
            auto types= GetSingletons();
            for( auto& it : types )
                target << it.first <<  " = 0x" << NFormat::Hex(reinterpret_cast<uint64_t>(it.second) ) << NNewLine;

            return static_cast<int>( types.size() );
        }
    #endif
}
#endif
//! @endcond

}}// namespace [aworx::lib]


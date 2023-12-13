// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_STRINGS_ASTRING)
#       include "alib/strings/astring.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#       include "alib/strings/localstring.hpp"
#   endif

#   if !defined (HPP_ALIB_STRINGS_CSTRING)
#      include "alib/strings/cstring.hpp"
#   endif
#endif // !defined(ALIB_DOX)

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib { namespace strings {
// ####################################################################################################
// AString::_dbgCheck()
// ####################################################################################################
//! @cond NO_DOX
#if ALIB_DEBUG_STRINGS

template<typename TChar>
void TAString<TChar>::dbgCheck() const
{
    TString<TChar>::dbgCheck();

    integer cap= Capacity();

    ALIB_ASSERT_ERROR( debugLastAllocRequest == 0
                       ||  TString<TChar>::length <= debugLastAllocRequest, "STRINGS",
                       "Error: Previous allocation request was too short"         )

    ALIB_ASSERT_ERROR( TString<TChar>::length <= cap, "STRINGS",
                       "Error: Length greater than allocation size"               )

    if( TString<TChar>::buffer && HasInternalBuffer() )
    {
        for (integer i= -16 ; i < 0 ; ++i)
            if ( TString<TChar>::buffer[i] != 2 )
            {
                ALIB_ERROR( "STRINGS", "Magic byte not found at start of buffer." )
                break;
            }
        for (integer i= 1 ; i <= 16 ; ++i)
            if ( TString<TChar>::buffer[ cap + i] != 3 )
            {
                ALIB_ERROR( "STRINGS", "Magic byte not found at end of buffer." )
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
void TAString<TChar>::GrowBufferAtLeastBy( integer minimumGrowth )
{
    integer actCapacity= Capacity();

    ALIB_ASSERT_WARNING (  TString<TChar>::length + minimumGrowth > actCapacity,
                           "STRINGS", "Unnecessary invocation of Grow()" )

    // first allocation? Go with given growth as size
    if (actCapacity == 0 )
    {
        SetBuffer( minimumGrowth > 16 ? minimumGrowth : 16 );
        #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= minimumGrowth;
        #endif

        return;
    }

    // calc new size: in general grow by 50%
    integer newCapacity= actCapacity + (actCapacity / 2);
    if ( newCapacity < TString<TChar>::length + minimumGrowth )
        newCapacity+= minimumGrowth;

    if ( newCapacity < 16 )
        newCapacity= 16;

    SetBuffer( newCapacity );
    #if ALIB_DEBUG_STRINGS
    debugLastAllocRequest= actCapacity + minimumGrowth;
    #endif
}

template<typename TChar>
void TAString<TChar>::SetBuffer( integer newCapacity )
{
    ALIB_STRING_DBG_CHK(this)

    ALIB_ASSERT( newCapacity >= 0 )

    // do nothing if life-cycle is managed by us and same size,
    if ( capacity >= 0 && capacity == newCapacity )
        return;

    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= newCapacity;
    #endif

    // set uninitialized (and return)
    if ( newCapacity == 0 )
    {
        ALIB_ASSERT_WARNING( !dbgWarnWhenExternalBufferIsReplaced || capacity >= 0, "STRINGS",
            "AString::SetBuffer(): removing an external buffer (setting string nulled). "
            "This may not be wanted."  )

        if ( capacity > 0 )
            std::free( const_cast<void*>(reinterpret_cast<const void*>( TString<TChar>::buffer
                                              #if ALIB_DEBUG_STRINGS
                                                                        - 16
                                              #endif
                                                                                               )) );

        capacity=
        TString<TChar>::length=     0;
        TString<TChar>::buffer=     nullptr;
        return;
    }

    #if ALIB_DEBUG
        if( dbgWarnWhenExternalBufferIsReplaced && capacity < 0)
            ALIB_WARNING( "STRINGS",
            "AString::SetBuffer(): replacing an external buffer by an internally managed one. "
            "This may not be wanted: ", reinterpret_cast<const char*>(Terminate())   )
    #endif

    // extend or shrink an existing buffer (and return)
    if( capacity > 0 )
    {
        #if !ALIB_DEBUG_STRINGS
            TString<TChar>::buffer= static_cast<TChar*>( std::realloc(  TString<TChar>::vbuffer,
                                                                           static_cast<size_t>(newCapacity  + 1)
                                                                           * sizeof( TChar )                       ) );
            #if ALIB_AVOID_ANALYZER_WARNINGS
                ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                if( newCapacity > capacity )
                    characters::CharArray<TChar>::Fill( TString<TChar>::vbuffer + TString<TChar>::length,
                                                        newCapacity  + 1 - TString<TChar>::length
                                                        ,'\0'                                                   );
                ALIB_WARNINGS_RESTORE
            #endif
        #else
            // add 16 characters of padding at start/end
            TString<TChar>::buffer= static_cast<TChar*>( std::realloc( TString<TChar>::vbuffer - 16,
                                                                          static_cast<size_t>( newCapacity  + 1 + 33 )
                                                                          * sizeof( TChar ) ) ) + 16;

            // write '\3' to end ('\0'= termination byte, '\1'= untermination byte )
            characters::CharArray<TChar>::Fill( TString<TChar>::vbuffer + newCapacity + 1, 16, '\3' );
        #endif

        capacity=   newCapacity;
        if ( TString<TChar>::length > capacity )
            TString<TChar>::length= capacity;

        return;
    }

    // create new Buffer
    #if !ALIB_DEBUG_STRINGS
        TChar* newBuffer= static_cast<TChar*>( std::malloc(  static_cast<size_t>(newCapacity  + 1)
                                                           * sizeof( TChar )                       ) );
        #if ALIB_AVOID_ANALYZER_WARNINGS
            characters::CharArray<TChar>::Fill( newBuffer, newCapacity  + 1, '\0' );
        #endif
    #else
        // add 16 characters of padding at start/end
        TChar* newBuffer= static_cast<TChar*>(   std::malloc(static_cast<size_t>( newCapacity  + 1 + 32 )
                                               * sizeof( TChar ) ) ) + 16;

        // write '\2' to start, '\3' to end ('\0'= termination byte, '\1'= untermination byte )
        characters::CharArray<TChar>::Fill( newBuffer - 16             , 16 , '\2');
        characters::CharArray<TChar>::Fill( newBuffer + newCapacity + 1, 16 , '\3');
    #endif

    // if we had a buffer before
    if ( capacity != 0 )
    {
        // copy data and delete old buffer
        characters::CharArray<TChar>::Copy( TString<TChar>::buffer, (std::min)( TString<TChar>::length + 1, newCapacity + 1),
                                            newBuffer );
        if ( capacity > 0 )
            std::free( const_cast<void*>(reinterpret_cast<const void*>( TString<TChar>::buffer
                                                                #if ALIB_DEBUG_STRINGS
                                                                        - 16
                                                                #endif
                                                                                               )) );

    }
    else
    {
        ALIB_ASSERT( TString<TChar>::length == 0 )
    }

    // set new Buffer and adjust length
    TString<TChar>::buffer=     newBuffer;
    capacity=   newCapacity;
    if ( TString<TChar>::length > capacity )
        TString<TChar>::length= capacity;
}


template<typename TChar>
void TAString<TChar>::SetBuffer( TChar* extBuffer, integer extBufferSize, integer extLength,
                                    Responsibility responsibility  )
{
    ALIB_ASSERT_ERROR(    !(extBufferSize == 0 && extBuffer != nullptr)
                       && !(extBufferSize != 0 && extBuffer == nullptr) , "STRINGS",
        "AString::SetBuffer(): Given buffer is nullptr while given alloc size is not 0 (or vice versa)" )

    // delete any existing
    if ( capacity > 0 )
        std::free( const_cast<void*>(reinterpret_cast<const void*>( TString<TChar>::buffer
                                                            #if ALIB_DEBUG_STRINGS
                                                                    - 16
                                                            #endif
                                                                                               )) );

    // too small? treat as if a nullptr was given.
    if ( extBufferSize < 1 )
    {
        ALIB_ERROR( "STRINGS", "allocation size < 1" )
        extBuffer= nullptr;
    }

    // null buffer?
    if ( (TString<TChar>::buffer= extBuffer) == nullptr )
    {
        #if ALIB_DEBUG_STRINGS
            debugLastAllocRequest=
        #endif
        capacity=
        TString<TChar>::length=       0;
        return;
    }


    if ( extLength >= extBufferSize  )
    {
        ALIB_ERROR( "STRINGS", "ext length >= ext allocation size" )
        extLength= extBufferSize -1;
    }


    // save given buffer
    --extBufferSize;     // we count one less
    capacity=   responsibility==Responsibility::Transfer ?  extBufferSize
                                                         : -extBufferSize;
    #if ALIB_DEBUG_STRINGS
        debugLastAllocRequest= extBufferSize;
    #endif
    TString<TChar>::length=     extLength;
}



// #############################################################################################
// Trim
// #############################################################################################

template<typename TChar>
integer TAString<TChar>::TrimAt( integer idx, const TCString<TChar>& trimChars )
{
    if ( idx < 0 )
         return 0;
    if ( idx >= TString<TChar>::length )
         return TString<TChar>::length;

    integer regionStart=  TString<TChar>::template       LastIndexOfAny<Inclusion::Exclude, false>( trimChars, idx ) + 1;
    if (regionStart < 0 )
        regionStart= 0;

    integer regionEnd=    TCString<TChar>(this).template IndexOfAny    <Inclusion::Exclude, false>( trimChars, idx );
    if (regionEnd < 0 )
        regionEnd= TString<TChar>::length;

    integer regionLength= regionEnd - regionStart;
    if ( regionLength > 0 )
        Delete<false>( regionStart, regionLength );

    return regionStart;
}

template<typename TChar>
TAString<TChar>& TAString<TChar>::Trim( const TCString<TChar>& trimChars )
{
    // check
    if (TString<TChar>::length == 0 || trimChars.IsEmpty() )
        return *this;

    // trim end
    integer idx= TString<TChar>::template LastIndexOfAny<Inclusion::Exclude, false>( trimChars, TString<TChar>::length - 1 ) + 1;
    if ( (TString<TChar>::length= idx) > 0 )
    {
        // trim front
        idx= TCString<TChar>(this).template IndexOfAny<Inclusion::Exclude,false>( trimChars );
        if ( idx > 0 )
            Delete<false>( 0, idx );
    }

    return *this;
}


// #################################################################################################
//  Replace()
// #################################################################################################
template<typename TChar>
integer TAString<TChar>::SearchAndReplace(  TChar      needle,
                                            TChar      replacement,
                                            integer    startIdx        )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    ALIB_STRING_DBG_CHK(this)
         if ( startIdx < 0  )       startIdx= 0;
    else if ( startIdx >= TString<TChar>::length )  return 0;

    // replacement loop
    TCString<TChar> thisAsCString= this;
    integer cntReplacements=    0;
    do
    {
        startIdx= thisAsCString.template IndexOfOrLength<false>( needle, startIdx );
        if ( startIdx == TString<TChar>::length  )
            break;
        TString<TChar>::vbuffer[ startIdx ]= replacement;
        ++cntReplacements;
    }
    while(  ++startIdx < TString<TChar>::length ) ;
    return cntReplacements;
    ALIB_WARNINGS_RESTORE
}


template<typename TChar>
integer TAString<TChar>::SearchAndReplace( const TString<TChar>&  needle,
                                           const TString<TChar>&  replacement,
                                           integer                startIdx,
                                           integer                maxReplacements,
                                           Case                   sensitivity       )
{
    ALIB_STRING_DBG_CHK(this)

    // check null arguments
    if ( needle.IsEmpty() )
        return 0;

    integer nLen=    needle.Length();
    integer rLen=    replacement.Length();
    integer lenDiff= rLen - nLen;

    // replacement loop
    integer cntReplacements=    0;
    while ( cntReplacements < maxReplacements && startIdx < TString<TChar>::length)
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        // search  next occurrence
        integer    idx= sensitivity == Case::Sensitive
                        ? TString<TChar>(*this).template IndexOf<false, Case::Sensitive>( needle, startIdx )
                        : TString<TChar>(*this).template IndexOf<false, Case::Ignore   >( needle, startIdx );
        if ( idx < 0 )
            break;

        // copy rest up or down
        if ( lenDiff != 0 )
        {
            if ( lenDiff > 0 )
                EnsureRemainingCapacity( lenDiff );
            characters::CharArray<TChar>::Move( TString<TChar>::vbuffer + idx + nLen,
                                                TString<TChar>::length  - idx - nLen,
                                                TString<TChar>::vbuffer + idx + nLen + lenDiff );
            TString<TChar>::length+= lenDiff;
        }

        // fill replacement in
        if( rLen > 0 )
            characters::CharArray<TChar>::Copy( replacement.Buffer(), rLen, TString<TChar>::vbuffer + idx );

        // set start index to first character behind current replacement
        startIdx= idx + rLen;

        // next
        ++cntReplacements;
        ALIB_WARNINGS_RESTORE
    }

    // that's it
    return cntReplacements;
}


#if !defined(ALIB_DOX)

#if ALIB_DEBUG_STRINGS
template   void TAString<nchar>::dbgCheck() const;
template   void TAString<wchar>::dbgCheck() const;
template   void TAString<xchar>::dbgCheck() const;
#endif

// #################################################################################################
// #################################################################################################
// Template instantiations
// #################################################################################################
// #################################################################################################

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
template  void             TAString<nchar>::GrowBufferAtLeastBy(integer);
template  void             TAString<nchar>::SetBuffer          (integer);
template  void             TAString<nchar>::SetBuffer          (nchar*,integer,integer,Responsibility);
template  integer          TAString<nchar>::SearchAndReplace   (nchar,nchar,integer );
template  integer          TAString<nchar>::SearchAndReplace   (const TString<nchar>&,const TString<nchar>&,integer,integer,Case );
template  TAString<nchar>& TAString<nchar>::Trim               (const TCString<nchar>& );
template  integer          TAString<nchar>::TrimAt             (integer,const TCString<nchar>& );

template  void             TAString<wchar>::GrowBufferAtLeastBy(integer);
template  void             TAString<wchar>::SetBuffer          (integer);
template  void             TAString<wchar>::SetBuffer          (wchar*,integer,integer,Responsibility);
template  integer          TAString<wchar>::SearchAndReplace   (wchar,wchar,integer );
template  integer          TAString<wchar>::SearchAndReplace   (const TString<wchar>&,const TString<wchar>&,integer,integer,Case );
template  TAString<wchar>& TAString<wchar>::Trim               (const TCString<wchar>& );
template  integer          TAString<wchar>::TrimAt             (integer,const TCString<wchar>& );

template  void             TAString<xchar>::GrowBufferAtLeastBy(integer);
template  void             TAString<xchar>::SetBuffer          (integer);
template  void             TAString<xchar>::SetBuffer          (xchar*,integer,integer,Responsibility);
template  integer          TAString<xchar>::SearchAndReplace   (xchar,xchar,integer );
template  integer          TAString<xchar>::SearchAndReplace   (const TString<xchar>&,const TString<xchar>&,integer,integer,Case );
template  TAString<xchar>& TAString<xchar>::Trim               (const TCString<xchar>& );
template  integer          TAString<xchar>::TrimAt             (integer,const TCString<xchar>& );
ALIB_WARNINGS_RESTORE


// #################################################################################################
// #################################################################################################
// Append methods
// #################################################################################################
// #################################################################################################

// NOTE:
// The implementations of specialized "Append" methods are using the C++ native character
// definitions. This way, the code can use external functions more efficient.
// What we need for this, is the knowledge of the third C++ type used by ALib besides char and
// wchar_t. It will be one of char16_t and char32_t and is defined below as CHARXX_T.
// Depending on the compilation settings, this is either equal to wchar or xchar.
#if ALIB_SIZEOF_WCHAR_T == 2
#   define CHARXX_T    char32_t
#else
#   define CHARXX_T    char16_t
#endif

// #################################################################################################
// TCheck==true Versions
//   Checks the src parameter. Then, if src not nullptr but empty, then this string is
//   assured to be not nulled, which means a buffer is allocated if non was before.
// #################################################################################################
#define CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(TChar,TCharParam)                                \
    template<> template<>                                                                          \
    TAString<TChar>& TAString<TChar>::Append<true> ( const TCharParam* src, integer srcLength )    \
    {                                                                                              \
        if( !src )                                                                                 \
            return *this;                                                                          \
                                                                                                   \
        if ( srcLength <= 0 )                                                                      \
        {                                                                                          \
            if ( TString<TChar>::IsNull() )                                                        \
                SetBuffer( 15 );                                                                   \
            return *this;                                                                          \
        }                                                                                          \
                                                                                                   \
        return Append<false>( src, srcLength );                                                    \
    }                                                                                              \

    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(char    , wchar_t  )
    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(char    , CHARXX_T )
    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(wchar_t , char     )
    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(wchar_t , CHARXX_T )
    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(CHARXX_T, char     )
    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(CHARXX_T, wchar_t  )

#undef CHECKING_VERSION_IMPLEMENTATION_OF_APPEND


// #################################################################################################
// <char>
// #################################################################################################
template<> template<>
TAString<char>& TAString<char>::Append<false>( const wchar_t* src, integer srcLength )
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    ALIB_STRING_DBG_CHK( this )
    ALIB_ASSERT_ERROR( src,  "STRINGS", "nullptr appended to string" )
    if( srcLength == 0 )
        return *this;

    //--------- Windows Version ---------
    #if defined( _WIN32 )

        // loop until reserved size is big enough
        EnsureRemainingCapacity( srcLength * 2 );
        for(;;)
        {
            int conversionSize= WideCharToMultiByte( CP_UTF8, 0,
                                                     src, static_cast<int>( srcLength),
                                                     TString<char>::vbuffer + TString<char>::length, static_cast<int>( Capacity() - TString<char>::length ),
                                                     NULL, NULL );
            if ( conversionSize > 0 )
            {
                TString<char>::length+= conversionSize;
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
                     "AString: Cannot convert wide character string to UTF-8. Error: ",
                      (   error == ERROR_INVALID_FLAGS          ? "ERROR_INVALID_FLAGS."
                        : error == ERROR_INVALID_PARAMETER      ? "ERROR_INVALID_PARAMETER"
                        : error == ERROR_NO_UNICODE_TRANSLATION ? "ERROR_NO_UNICODE_TRANSLATION"
                                                                : NString64(error).Terminate()      )
                    )

            return *this;
        }

    //--------- __GLIBCXX__ Version ---------
    #elif defined (__GLIBCXX__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
                                                                                                                            
        integer maxConversionSize= integer(MB_CUR_MAX) * srcLength;

        mbstate_t ps;
        EnsureRemainingCapacity( maxConversionSize );
        memset( &ps, 0, sizeof(mbstate_t) );
        const wchar_t* srcp= src;
        size_t conversionSize= wcsnrtombs( TString<char>::vbuffer + TString<char>::length, &srcp, static_cast<size_t>(srcLength), static_cast<size_t>(maxConversionSize),  &ps);
        if ( conversionSize == static_cast<size_t>( -1 ) )
        {
            ALIB_WARNING( "STRINGS", "Cannot convert WCS to MBCS. Check locale settings (should be UTF-8)" )
            return *this;
        }

        if ( conversionSize < 1 )
        {
            ALIB_ERROR( "STRINGS", "Error converting WCS to MBCS." )
            return *this;
        }

        TString<char>::length+= conversionSize;
        return *this;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        return *this;
    #endif
    ALIB_WARNINGS_RESTORE
}

template<>
template<>
TAString<char>& TAString<char>::Append<false>( const CHARXX_T* src, integer srcLength )
{
    // convert to wchar_t and invoke wchar_t version
    TLocalString<wchar_t,2048>   converter;
    converter.DbgDisableBufferReplacementWarning();
    converter.Append<false>( src, srcLength );
    Append<false>( converter.Buffer(), converter.Length() );

    return *this;
}


ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
// #################################################################################################
// <wchar_t>
// #################################################################################################
template<> template<>
TAString<wchar_t>& TAString<wchar_t>::Append<false>( const char* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )

    // copy loop
    EnsureRemainingCapacity( srcLength );

    //--------- Windows Version ----------
    #if defined( _WIN32 )
        if( srcLength == 0)
            return *this;
        integer conversionSize= MultiByteToWideChar( CP_UTF8, 9,
                                                     src, static_cast<int>( srcLength ),
                                                     vbuffer + length, static_cast<int>( Capacity() - length ) );
        // check for errors
        #if ALIB_DEBUG
            if ( conversionSize == 0 )
            {
                // not enough space?
                int error= GetLastError();

                ALIB_WARNING(
                    "MBCS to WCS conversion failed (Error: ",
                    (  error == ERROR_INSUFFICIENT_BUFFER      ?  "ERROR_INSUFFICIENT_BUFFER."
                     : error == ERROR_INVALID_FLAGS            ?  "ERROR_INVALID_FLAGS."
                     : error == ERROR_INVALID_PARAMETER        ?  "ERROR_INVALID_PARAMETER"
                     : error == ERROR_NO_UNICODE_TRANSLATION   ?  "ERROR_NO_UNICODE_TRANSLATION"
                                                               :  NString64( error ).Terminate()   ),
                     ")" )
            }

            ALIB_ASSERT_ERROR( conversionSize <= srcLength, "STRINGS",
                               NString128( "MBCS to WCS conversion failed. Requested length=" )._( srcLength )
                                        ._( ", conversion length=" )._(conversionSize)
                            )
        #endif

        length+= conversionSize;
        return *this;


    //--------- __GLIBCXX__ Version ---------
    #elif defined (__GLIBCXX__) || defined(__APPLE__) || defined(__ANDROID_NDK__)

        while(srcLength > 0 )
        {
            integer actConversionLenght= srcLength;
            for( int pass= 0 ; pass < 2 ; ++pass )
            {

                mbstate_t    ps;    memset( &ps, 0, sizeof(mbstate_t) );
                const char* srcp= src;
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
                        ++src;
                        --srcLength;
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
        return *this;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        return *this;
    #endif
}


#if ALIB_SIZEOF_WCHAR_T == 4

    template<>
    template<>
    TAString<wchar_t>& TAString<wchar_t>::Append<false>( const CHARXX_T* src, integer srcLength )
    {
        EnsureRemainingCapacity( srcLength );

        // convert UTF16 to UTF32
        const CHARXX_T* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            const char32_t uc = *src++;
            if ((uc - 0xd800) >= 2048) // not surrogate
            {
                vbuffer[length++] = static_cast<wchar_t>(uc);
            }
            else
            {
                ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                   && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                   && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                   "STRINGS", "Error decoding UTF16" )

                vbuffer[length++]=  static_cast<wchar_t>(     (uc << 10)
                                                         +  ((*src++) - 0x35fdc00 )    );
            }
        }

        return *this;
    }

#else

    template<>
    template<>
    TAString<wchar_t>& TAString<wchar_t>::Append<false>( const CHARXX_T* src, integer srcLength )
    {
        // convert UTF32 to UTF16
        EnsureRemainingCapacity( srcLength * 2 );

        const CHARXX_T* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            uinteger uc= *src++;
            ALIB_ASSERT_ERROR(       uc <  0xd800
                                || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                "STRINGS", "Illegal unicode 32 bit codepoint"       )

            if( uc < 0x10000 )
            {
                  vbuffer[length++]=  static_cast<wchar_t>( uc );
            }
            else
            {
                uc-= 0x10000;
                vbuffer[length++]= static_cast<wchar_t>(  ( uc >> 10    ) + 0xd800  );
                vbuffer[length++]= static_cast<wchar_t>(  ( uc &  0x3ff ) + 0xdc00  );
            }
        }

        return *this;
    }
#endif

// #################################################################################################
// <XCHARXX_T> (char16_t or char32_t)
// #################################################################################################

#if ALIB_SIZEOF_WCHAR_T == 2
    template<> template<>
    TAString<CHARXX_T>& TAString<CHARXX_T>::Append<false>( const wchar_t* src, integer srcLength )
    {
        EnsureRemainingCapacity( srcLength );

        // convert UTF16 to UTF32
        const wchar_t* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            const char32_t uc = *src++;
            if ((uc - 0xd800) >= 2048) // not surrogate
            {
                vbuffer[length++] = static_cast<CHARXX_T>(uc);
            }
            else
            {
                ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                   && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                   && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                   "STRINGS", "Error decoding UTF16" )

                vbuffer[length++]=  static_cast<CHARXX_T>(     (uc << 10)
                                                         +  ((*src++) - 0x35fdc00 )    );
            }
        }

        return *this;
    }

#else

    template<> template<>
    TAString<CHARXX_T>& TAString<CHARXX_T>::Append<false>( const wchar_t* src, integer srcLength )
    {
        // convert UTF32 to UTF16
        EnsureRemainingCapacity( srcLength * 2 );

        const wchar_t* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            uinteger uc= static_cast<uinteger>( *src++ );
            ALIB_ASSERT_ERROR(       uc <  0xd800
                                || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                "STRINGS", "Illegal unicode 32 bit codepoint"       )

            if( uc < 0x10000 )
            {
                  vbuffer[length++]=  static_cast<CHARXX_T>( uc );
            }
            else
            {
                uc-= 0x10000;
                vbuffer[length++]= static_cast<CHARXX_T>(  ( uc >> 10    ) + 0xd800  );
                vbuffer[length++]= static_cast<CHARXX_T>(  ( uc &  0x3ff ) + 0xdc00  );
            }
        }

        return *this;
    }
#endif
ALIB_WARNINGS_RESTORE

template<> template<>
TAString<CHARXX_T>& TAString<CHARXX_T>::Append<false>( const char* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )

    // We are using a WAString to do the job. Not efficient, but for today, this should be all we do!
    TLocalString<wchar_t,2048>   converter;
    converter.Append<false>( src, srcLength );
    return Append<false>( converter.Buffer(), converter.Length() );
}


#undef CHARXX_T

#endif // !defined(ALIB_DOX)



}}}// namespace [aworx::lib::strings]


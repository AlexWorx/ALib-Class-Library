// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#if !DOXYGEN

#if !defined(HPP_ALIB_STRINGS_TASTRING_INLINING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// #################################################################################################
// This inlined compilation unit is to be included by compilation that specialize template
// type "strings::TAString" in respect to its two template parameters TChar and TAllocator.
// For this, before inclusion two preprocessor symbols have to be specified:
// - ASTR_TCHAR_SPEC  The character type
// - ASTR_TALLOC_SPEC The allocator type
// At the end of this header, both symbols will be set undefined, to allow a next invocation
// with new symbols (and to enforce the right usage).
//
// With "tastring_ca_decl.inl", the corresponding declarations are to be made.
// #################################################################################################
// #################################################################################################

#if !defined(ASTR_TCHAR_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif
#if !defined(ASTR_TALLOC_SPEC)
#  error "Preprocessor symbol ASTR_TCHAR_SPEC has to be given as ALIB_CHAR_TYPE_ID_XYZ before invoking this .INL"
#endif


#if    ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N
#   define TCHARNAME  nchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W
#   define TCHARNAME  wchar
#elif ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X
#   define TCHARNAME  xchar
#else
#   error
#endif



namespace alib::strings {
#if ALIB_DEBUG_STRINGS
template  void                              TAString<TCHARNAME, ASTR_TALLOC_SPEC>::dbgCheck()     const;
#endif
template  void                              TAString<TCHARNAME, ASTR_TALLOC_SPEC>::GrowBufferAtLeastBy(integer);
template  void                              TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          (integer);
template  void                              TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SetBuffer          (TCHARNAME*,integer,integer,lang::Responsibility);
template  integer                           TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   (TCHARNAME,TCHARNAME,integer,integer );
template  integer                           TAString<TCHARNAME, ASTR_TALLOC_SPEC>::SearchAndReplace   (const TString<TCHARNAME>&,const TString<TCHARNAME>&,integer,integer,lang::Case,integer );
template  TAString<TCHARNAME, ASTR_TALLOC_SPEC>&  TAString<TCHARNAME, ASTR_TALLOC_SPEC>::Trim               (const TCString<TCHARNAME>& );
template  integer                           TAString<TCHARNAME, ASTR_TALLOC_SPEC>::TrimAt             (integer,const TCString<TCHARNAME>& );


#if ALIB_DEBUG
template void T_Append<std::type_info            , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::type_info&   );
#endif
#if ALIB_EXT_LIB_THREADS_AVAILABLE
template void T_Append<std::thread::id           , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const std::thread::id&  );
#endif
template void T_Append<lang::CallerInfo          , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const lang::CallerInfo& );


template void T_Append<bool                      , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, bool     );
template void T_Append<  int64_t                 , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&,  int64_t );
template void T_Append< uint64_t                 , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, uint64_t );
template void T_Append<float                     , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, float    );
template void T_Append<double                    , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, double   );
template void T_Append<TFormat<TCHARNAME>        , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>&         );
template void T_Append<TFormat<TCHARNAME>::Tab   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Tab&    );
template void T_Append<TFormat<TCHARNAME>::Field , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Field&  );
template void T_Append<TFormat<TCHARNAME>::Escape, TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Escape& );
template void T_Append<TFormat<TCHARNAME>::Bin   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Bin&    );
template void T_Append<TFormat<TCHARNAME>::Hex   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Hex&    );
template void T_Append<TFormat<TCHARNAME>::Oct   , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Oct&    );
template void T_Append<TFormat<TCHARNAME>::Fill  , TCHARNAME, ASTR_TALLOC_SPEC>::operator()( TAString<TCHARNAME, ASTR_TALLOC_SPEC>&, const TFormat<TCHARNAME>::Fill&   );




// #################################################################################################
// #################################################################################################
// Append methods of incompatible character width.
// #################################################################################################
// #################################################################################################

// #################################################################################################
// TCheck==CHK Versions
//  Checks the src parameter. Then, if src not nullptr but empty, this string is
//  ensured to be not nulled, which means a buffer is allocated if non was before.
//  Finally invokes the TCheck==NC version.
// #################################################################################################
#define CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(TChar,TAllocator,TCharParam)                     \
    template<> template<>                                                                          \
    ALIB_API                                                                                       \
    TAString<TChar,TAllocator>& TAString<TChar,TAllocator>::Append<CHK> ( const TCharParam* src, integer srcLength )    \
    {                                                                                              \
        if( !src )  return *this;                                                                  \
        if ( srcLength <= 0 )                                                                      \
        {                                                                                          \
            if ( base::IsNull() )                                                                  \
                SetBuffer( 15 );                                                                   \
            return *this;                                                                          \
        }                                                                                          \
        return Append<NC>( src, srcLength );                                                       \
    }                                                                                              \


// We have to know what the "real" types are.
#if ALIB_CHARACTERS_NATIVE_WCHAR
#   define REAL_WCHAR wchar
#   define REAL_XCHAR xchar
#else
#   define REAL_WCHAR xchar
#   define REAL_XCHAR wchar
#endif

ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
ALIB_WARNINGS_IGNORE_SIGN_CONVERSION

// #################################################################################################
// <wchar_t>
// #################################################################################################
#if ( (ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W) && ALIB_CHARACTERS_NATIVE_WCHAR ) || ( (ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X) && !ALIB_CHARACTERS_NATIVE_WCHAR )
template<> template<>
ALIB_API
TAString<wchar_t, ASTR_TALLOC_SPEC>& TAString<wchar_t, ASTR_TALLOC_SPEC>::Append<NC>( const char* src, integer srcLength )
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
                const nchar* srcp= src;
                size_t       wcWritten= mbsnrtowcs( vbuffer + length,  &srcp,
                                                    size_t(actConversionLenght),
                                                    size_t(Capacity() - length), &ps );

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
CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(wchar_t, ASTR_TALLOC_SPEC, char   )


#if ALIB_SIZEOF_WCHAR_T == 4

    template<>
    template<>
    ALIB_API
    TAString<wchar_t, ASTR_TALLOC_SPEC>& TAString<wchar_t, ASTR_TALLOC_SPEC>::Append<NC>( const char16_t* src, integer srcLength )
    {
        EnsureRemainingCapacity( srcLength );

        // convert UTF16 to UTF32
        const char16_t* srcEnd=    src + srcLength;
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

    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(wchar_t, ASTR_TALLOC_SPEC, char16_t  )
#else

    template<>
    template<>
    ALIB_API
    TAString<wchar_t, ASTR_TALLOC_SPEC>& TAString<wchar_t, ASTR_TALLOC_SPEC>::Append<NC>( const char32_t* src, integer srcLength )
    {
        // convert UTF32 to UTF16
        EnsureRemainingCapacity( srcLength * 2 );

        const char32_t* srcEnd=    src + srcLength;
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

    CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(wchar_t, ASTR_TALLOC_SPEC, char32_t  )
#endif


#endif // #if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_W


// #################################################################################################
// <XCHARXX_T> (char16_t or char32_t)
// #################################################################################################
#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X

#if ALIB_SIZEOF_WCHAR_T == 2

    template<> template<>
    ALIB_API
    TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>& TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>::Append<NC>( const wchar_t* src, integer srcLength )
    {
        EnsureRemainingCapacity( srcLength );

        // convert UTF16 to UTF32
        const wchar_t* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            const char32_t uc = *src++;
            if ((uc - 0xd800) >= 2048) // not surrogate
            {
                vbuffer[length++] = static_cast<REAL_XCHAR>(uc);
            }
            else
            {
                ALIB_ASSERT_ERROR(    src < srcEnd                        // has one more?
                                   && ((uc    & 0xfffffc00) == 0xd800)    // is low
                                   && ((*src  & 0xfffffc00) == 0xdc00),   // is high
                                   "STRINGS", "Error decoding UTF16" )

                vbuffer[length++]=  static_cast<REAL_XCHAR>(     (uc << 10)
                                                              +  ((*src++) - 0x35fdc00 )    );
            }
        }

        return *this;
    }

#else

    template<> template<>
    ALIB_API
    TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>& TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>::Append<NC>( const wchar_t* src, integer srcLength )
    {
        // convert UTF32 to UTF16
        EnsureRemainingCapacity( srcLength * 2 ); // can potentially double!

        const wchar_t* srcEnd=    src + srcLength;
        while (src < srcEnd)
        {
            uinteger uc= static_cast<uinteger>( *src++ );
            ALIB_ASSERT_ERROR(       uc <  0xd800
                                || ( uc >= 0xe000 && uc <= 0x10ffff ),
                                "STRINGS", "Illegal unicode 32 bit codepoint"       )

            if( uc < 0x10000 )
            {
                  vbuffer[length++]=  static_cast<REAL_XCHAR>( uc );
            }
            else
            {
                uc-= 0x10000;
                vbuffer[length++]= static_cast<REAL_XCHAR>(  ( uc >> 10    ) + 0xd800  );
                vbuffer[length++]= static_cast<REAL_XCHAR>(  ( uc &  0x3ff ) + 0xdc00  );
            }
        }

        return *this;
    }
#endif

template<> template<>
ALIB_API
TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>& TAString<REAL_XCHAR, ASTR_TALLOC_SPEC>::Append<NC>( const char* src, integer srcLength )
{
    ALIB_STRING_DBG_CHK( this )

    // We are using a WAString to do the job. Not efficient, but for today, this should be all we do!
    TLocalString<REAL_WCHAR,8192>   converter;
    converter.Append<NC>( src, srcLength );
    return Append<NC>( converter.Buffer(), converter.Length() );
}

CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(REAL_XCHAR , ASTR_TALLOC_SPEC, char  )
CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(REAL_XCHAR , ASTR_TALLOC_SPEC, wchar_t  )

#endif  //#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_X

// #################################################################################################
// <nchar>
// #################################################################################################
#if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N

template<> template<>
ALIB_API
TAString<char, ASTR_TALLOC_SPEC>& TAString<char, ASTR_TALLOC_SPEC>::Append<NC>( const REAL_WCHAR* src, integer srcLength )
{
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
                                                     TString<nchar>::vbuffer + TString<nchar>::length, static_cast<int>( Capacity() - TString<nchar>::length ),
                                                     NULL, NULL );
            if ( conversionSize > 0 )
            {
                TString<nchar>::length+= conversionSize;
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
        size_t conversionSize= wcsnrtombs( TString<nchar>::vbuffer + TString<nchar>::length, &srcp, size_t(srcLength), size_t(maxConversionSize),  &ps);
        if ( conversionSize == size_t( -1 ) )
        {
            ALIB_WARNING( "STRINGS", "Cannot convert WCS to MBCS. Check locale settings (should be UTF-8)" )
            return *this;
        }

        if ( conversionSize < 1 )
        {
            ALIB_ERROR( "STRINGS", "Error converting WCS to MBCS." )
            return *this;
        }

        TString<nchar>::length+= conversionSize;
        return *this;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
        return *this;
    #endif
}

template<>
template<>
ALIB_API
TAString<char, ASTR_TALLOC_SPEC>& TAString<nchar, ASTR_TALLOC_SPEC>::Append<NC>( const REAL_XCHAR* src, integer srcLength )
{
    // convert to wchar_t and invoke wchar_t version
    TLocalString<wchar_t,2048>   converter;
    converter.DbgDisableBufferReplacementWarning();
    converter.Append<NC>( src, srcLength );
    Append<NC>( converter.Buffer(), converter.Length() );

    return *this;
}

CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(char , ASTR_TALLOC_SPEC, wchar  )
CHECKING_VERSION_IMPLEMENTATION_OF_APPEND(char , ASTR_TALLOC_SPEC, xchar  )

#endif   // #if ASTR_TCHAR_SPEC == ALIB_CHAR_TYPE_ID_N


} // namespace [alib::strings]


ALIB_WARNINGS_RESTORE
ALIB_WARNINGS_RESTORE
#undef  CHECKING_VERSION_IMPLEMENTATION_OF_APPEND
#undef  REAL_WCHAR
#undef  REAL_XCHAR
#undef  TCHARNAME
#undef  ASTR_TALLOC_SPEC
#undef  ASTR_TCHAR_SPEC

#endif //if !DOXYGEN


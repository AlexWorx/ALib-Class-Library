// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_STRINGS_FWDS
#define HPP_ALIB_STRINGS_FWDS 1

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif


// #################################################################################################
// Preprocessor definitions
// #################################################################################################
#if defined(ALIB_STRINGS_DEBUG)
#   error "Code selector symbol ALIB_STRINGS_DEBUG must not be set from outside. Use  postfix '_ON' or '_OFF' for compiler symbols."
#endif
#if defined(ALIB_STRINGS_DEBUG_OFF) && defined(ALIB_STRINGS_DEBUG_ON)
    #error "Compiler symbols ALIB_STRINGS_DEBUG_OFF / ALIB_STRINGS_DEBUG_ON are both set (contradiction)."
#endif

#if defined( ALIB_STRINGS_DEBUG_ON )
    #define    ALIB_STRINGS_DEBUG 1
#else
    #define    ALIB_STRINGS_DEBUG 0
#endif


#if ALIB_STRINGS_DEBUG
#   define ALIB_STRING_DBG_CHK(instance)     \
    {                                        \
        (instance)->dbgCheck();              \
    }
#else
#   define  ALIB_STRING_DBG_CHK(instance)
#endif


// #################################################################################################
// Forward declarations
// #################################################################################################
namespace aworx { namespace lib { namespace strings {

    template<typename TChar>                     class  TString;
    template<typename TChar>                     class  TSubstring;
    template<typename TChar>                     class  TCString;
    template<typename TChar>                     class  TAString;
    template<typename TChar, integer TCapacity>  class  TLocalString;
    template<typename TChar>                     class  TStringNZT;

    template<typename TChar>                     struct TNumberFormat;
    template<typename TChar>                     class  TFormat;

    namespace util
    {
        template< typename... TAssociatedTypes > class  StringTable;
    }


    /**
     * This TMP struct is tested by implicit "auto cast" operators. Those might be disabled for
     * combinations of types.
     *
     * For example, implicit casting from class \b AString to sibling types \b CString and \b Substring
     * has to be forbidden, because both are also constructible implicitly from this class and hence an
     * ambiguity between implicit cast and implicit construction would arise.
     *
     * Custom classes might encounter similar problems. In that case, this TMP type may be specialized
     * to mitigate compilation issues.
     *
     * @tparam TFrom        The type that allows implicit auto-casts.
     * @tparam TImOrExplict Denotes if implicit or explicit casts are suppressed.
     * @tparam TTo          The destination type that auto-casts should be suppressed for.
     * @tparam TEnableIf    Helper parameter used for templated specializations of this struct.
     */
    template<typename TFrom, characters::ConstructionType TImOrExplict, typename TTo, typename TEnableIf=void>
    struct T_SuppressAutoCast : public std::false_type   {};

}} // namespace aworx[::lib::strings]

// #################################################################################################
// Alias names
// #################################################################################################

/// Type alias in namespace #aworx.
using  String           =     aworx::lib::strings::TString    <character>;

/// Type alias in namespace #aworx.
using  ComplementString =     aworx::lib::strings::TString    <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeString    =     aworx::lib::strings::TString    <strangeChar>;

/// Type alias in namespace #aworx.
using  NString          =     aworx::lib::strings::TString    <nchar>;

/// Type alias in namespace #aworx.
using  WString          =     aworx::lib::strings::TString    <wchar>;

/// Type alias in namespace #aworx.
using  XString          =     aworx::lib::strings::TString    <xchar>;

/// Type alias in namespace #aworx.
using  Substring        =     aworx::lib::strings::TSubstring <character>;

/// Type alias in namespace #aworx.
using  ComplementSubstring =  aworx::lib::strings::TSubstring <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeSubstring =     aworx::lib::strings::TSubstring <strangeChar>;

/// Type alias in namespace #aworx.
using  NSubstring       =     aworx::lib::strings::TSubstring <nchar>;

/// Type alias in namespace #aworx.
using  WSubstring       =     aworx::lib::strings::TSubstring <wchar>;

/// Type alias in namespace #aworx.
using  XSubstring       =     aworx::lib::strings::TSubstring <xchar>;

/// Type alias in namespace #aworx.
using  CString          =     aworx::lib::strings::TCString   <character>;

/// Type alias in namespace #aworx.
using  ComplementCString=     aworx::lib::strings::TCString   <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeCString   =     aworx::lib::strings::TCString   <strangeChar>;

/// Type alias in namespace #aworx.
using  NCString         =     aworx::lib::strings::TCString   <nchar>;

/// Type alias in namespace #aworx.
using  WCString         =     aworx::lib::strings::TCString   <wchar>;

/// Type alias in namespace #aworx.
using  XCString         =     aworx::lib::strings::TCString   <xchar>;

/// Type alias in namespace #aworx.
using  AString          =     aworx::lib::strings::TAString   <character>;

/// Type alias in namespace #aworx.
using  ComplementAString=     aworx::lib::strings::TAString   <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeAString   =     aworx::lib::strings::TAString   <strangeChar>;

/// Type alias in namespace #aworx.
using  NAString         =     aworx::lib::strings::TAString   <nchar>;

/// Type alias in namespace #aworx.
using  WAString         =     aworx::lib::strings::TAString   <wchar>;

/// Type alias in namespace #aworx.
using  XAString         =     aworx::lib::strings::TAString   <xchar>;

/// Type alias in namespace #aworx.
using  StringNZT        =     aworx::lib::strings::TStringNZT <character>;

/// Type alias in namespace #aworx.
using  ComplementStringNZT=   aworx::lib::strings::TStringNZT <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeStringNZT =     aworx::lib::strings::TStringNZT <strangeChar>;

/// Type alias in namespace #aworx.
using  NStringNZT       =     aworx::lib::strings::TStringNZT <nchar>;

/// Type alias in namespace #aworx.
using  WStringNZT       =     aworx::lib::strings::TStringNZT  <wchar>;

/// Type alias in namespace #aworx.
using  XStringNZT       =     aworx::lib::strings::TStringNZT  <xchar>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using LocalString       =     aworx::lib::strings::TLocalString<character, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using ComplementLocalString=  aworx::lib::strings::TLocalString<complementChar, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using StrangeLocalString=     aworx::lib::strings::TLocalString<strangeChar, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using NLocalString      =     aworx::lib::strings::TLocalString <nchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using WLocalString      =     aworx::lib::strings::TLocalString <wchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using XLocalString      =     aworx::lib::strings::TLocalString <xchar    , TCapacity>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,8>}.
using NString8 =  NLocalString<  8>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,8>}.
using String8  =  LocalString<   8>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,16>}.
using String16 =  LocalString<  16>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,32>}.
using String32 =  LocalString<  32>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,64>}.
using String64 =  LocalString<  64>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,128>}.
using String128=  LocalString< 128>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,256>}.
using String256=  LocalString< 256>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,512>}.
using String512=  LocalString< 512>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,1024>}.
using String1K =  LocalString<1024>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,2048>}.
using String2K =  LocalString<2048>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<character\,4096>}.
using String4K =  LocalString<4096>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,16>}.
using NString16 =  NLocalString<  16>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,32>}.
using NString32 =  NLocalString<  32>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,64>}.
using NString64 =  NLocalString<  64>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,128>}.
using NString128=  NLocalString< 128>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,256>}.
using NString256=  NLocalString< 256>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,512>}.
using NString512=  NLocalString< 512>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,1024>}.
using NString1K =  NLocalString<1024>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,2048>}.
using NString2K =  NLocalString<2048>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<nchar\,4096>}.
using NString4K =  NLocalString<4096>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,8>}.
using WString8  =  WLocalString<   8>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,16>}.
using WString16 =  WLocalString<  16>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,32>}.
using WString32 =  WLocalString<  32>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,64>}.
using WString64 =  WLocalString<  64>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,128>}.
using WString128=  WLocalString< 128>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,256>}.
using WString256=  WLocalString< 256>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,512>}.
using WString512=  WLocalString< 512>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,1024>}.
using WString1K =  WLocalString<1024>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,2048>}.
using WString2K =  WLocalString<2048>;

/// Type alias name for \alib{strings,TLocalString,TLocalString<wchar\,4096>}.
using WString4K =  WLocalString<4096>;


/// Type alias in namespace #aworx.
using  NumberFormat     =     aworx::lib::strings::TNumberFormat<character>;

/// Type alias in namespace #aworx.
using  ComplementNumberFormat=aworx::lib::strings::TNumberFormat<complementChar>;

/// Type alias in namespace #aworx.
using  StrangeNumberFormat=   aworx::lib::strings::TNumberFormat<strangeChar>;

/// Type alias in namespace #aworx.
using  NNumberFormat    =     aworx::lib::strings::TNumberFormat<nchar>;

/// Type alias in namespace #aworx.
using  WNumberFormat    =     aworx::lib::strings::TNumberFormat<wchar>;

/// Type alias in namespace #aworx.
using  XNumberFormat    =     aworx::lib::strings::TNumberFormat<xchar>;


/// Type alias in namespace #aworx.
using  Format           =     aworx::lib::strings::TFormat<character>;

/// Type alias in namespace #aworx.
using  ComplementFormat =     aworx::lib::strings::TFormat<complementChar >;

/// Type alias in namespace #aworx.
using  StrangeFormat    =     aworx::lib::strings::TFormat<strangeChar >;

/// Type alias in namespace #aworx.
using  NFormat          =     aworx::lib::strings::TFormat<nchar>;

/// Type alias in namespace #aworx.
using  WFormat          =     aworx::lib::strings::TFormat<wchar>;

/// Type alias in namespace #aworx.
using  XFormat          =     aworx::lib::strings::TFormat<xchar>;




} // namespace aworx


// #################################################################################################
// Macros
// #################################################################################################
#if ALIB_CHARACTERS_ARE_NARROW
#   define ALIB_STRINGS_TO_NARROW(       src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_FROM_NARROW(     src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_TO_NARROW_ARG(   src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_NARROW_ARG( src,bufSize      )  src;
#   define ALIB_STRINGS_TO_WIDE(         src,dest,bufSize )  aworx::lib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_WIDE(       src,dest,bufSize )  aworx::lib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_WIDE_ARG(     src,bufSize      )  aworx::lib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_WIDE_ARG    src,bufSize      )  aworx::lib::strings::TLocalString<nchar,bufSize>(src);
#else
#   define ALIB_STRINGS_TO_NARROW(       src,dest,bufSize )  aworx::lib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_NARROW(     src,dest,bufSize )  aworx::lib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(   src,bufSize      )  aworx::lib::strings::TLocalString<nchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG( src,bufSize      )  aworx::lib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_TO_WIDE(         src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_FROM_WIDE(       src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_TO_WIDE_ARG(     src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_WIDE_ARG(   src,bufSize      )  src;
#endif



#endif // HPP_ALIB_STRINGS_FWDS

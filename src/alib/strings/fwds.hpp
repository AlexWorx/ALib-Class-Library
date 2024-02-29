/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_STRINGS_FWDS
#define HPP_ALIB_STRINGS_FWDS 1

#if !defined(HPP_ALIB_CHARACTERS_CHARACTERS)
#   include "alib/characters/characters.hpp"
#endif


// #################################################################################################
// Preprocessor definitions
// #################################################################################################
#if ALIB_DEBUG_STRINGS
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
namespace alib {  namespace strings {

    template<typename TChar>                     class  TString;
    template<typename TChar>                     class  TSubstring;
    template<typename TChar>                     class  TCString;
    template<typename TChar>                     class  TAString;
    template<typename TChar, integer TCapacity>  class  TLocalString;
    template<typename TChar>                     class  TStringNZT;

    template<typename TChar>                     struct TNumberFormat;
    template<typename TChar>                     class  TFormat;
    template<typename TChar>                     class  TStringLengthResetter;

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
    template<typename                       TFrom,
             characters::ConstructionType   TImOrExplict,
             typename                       TTo,
             typename                       TEnableIf       =void>
    struct T_SuppressAutoCast : public std::false_type   {};

} // namespace alib[::strings]

// #################################################################################################
// Alias names
// #################################################################################################

/// Type alias in namespace \b alib.
using  String           =     strings::TString    <character>;

/// Type alias in namespace \b alib.
using  ComplementString =     strings::TString    <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeString    =     strings::TString    <strangeChar>;

/// Type alias in namespace \b alib.
using  NString          =     strings::TString    <nchar>;

/// Type alias in namespace \b alib.
using  WString          =     strings::TString    <wchar>;

/// Type alias in namespace \b alib.
using  XString          =     strings::TString    <xchar>;

/// Type alias in namespace \b alib.
using  Substring        =     strings::TSubstring <character>;

/// Type alias in namespace \b alib.
using  ComplementSubstring =  strings::TSubstring <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeSubstring =     strings::TSubstring <strangeChar>;

/// Type alias in namespace \b alib.
using  NSubstring       =     strings::TSubstring <nchar>;

/// Type alias in namespace \b alib.
using  WSubstring       =     strings::TSubstring <wchar>;

/// Type alias in namespace \b alib.
using  XSubstring       =     strings::TSubstring <xchar>;

/// Type alias in namespace \b alib.
using  CString          =     strings::TCString   <character>;

/// Type alias in namespace \b alib.
using  ComplementCString=     strings::TCString   <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeCString   =     strings::TCString   <strangeChar>;

/// Type alias in namespace \b alib.
using  NCString         =     strings::TCString   <nchar>;

/// Type alias in namespace \b alib.
using  WCString         =     strings::TCString   <wchar>;

/// Type alias in namespace \b alib.
using  XCString         =     strings::TCString   <xchar>;

/// Type alias in namespace \b alib.
using  AString          =     strings::TAString   <character>;

/// Type alias in namespace \b alib.
using  ComplementAString=     strings::TAString   <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeAString   =     strings::TAString   <strangeChar>;

/// Type alias in namespace \b alib.
using  NAString         =     strings::TAString   <nchar>;

/// Type alias in namespace \b alib.
using  WAString         =     strings::TAString   <wchar>;

/// Type alias in namespace \b alib.
using  XAString         =     strings::TAString   <xchar>;

/// Type alias in namespace \b alib.
using  StringNZT        =     strings::TStringNZT <character>;

/// Type alias in namespace \b alib.
using  ComplementStringNZT=   strings::TStringNZT <complementChar>;

/// Type alias in namespace \b alib.
using  StrangeStringNZT =     strings::TStringNZT <strangeChar>;

/// Type alias in namespace \b alib.
using  NStringNZT       =     strings::TStringNZT <nchar>;

/// Type alias in namespace \b alib.
using  WStringNZT       =     strings::TStringNZT  <wchar>;

/// Type alias in namespace \b alib.
using  XStringNZT       =     strings::TStringNZT  <xchar>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using LocalString       =     strings::TLocalString<character, TCapacity>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using ComplementLocalString=  strings::TLocalString<complementChar, TCapacity>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using StrangeLocalString=     strings::TLocalString<strangeChar, TCapacity>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using NLocalString      =     strings::TLocalString <nchar    , TCapacity>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using WLocalString      =     strings::TLocalString <wchar    , TCapacity>;

/// Type alias in namespace \b alib.
template<integer TCapacity>
using XLocalString      =     strings::TLocalString <xchar    , TCapacity>;

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


/// Type alias in namespace \b alib.
using  NumberFormat     =     strings::TNumberFormat<character>;

/// Type alias in namespace \b alib.
using  ComplementNumberFormat=strings::TNumberFormat<complementChar>;

/// Type alias in namespace \b alib.
using  StrangeNumberFormat=   strings::TNumberFormat<strangeChar>;

/// Type alias in namespace \b alib.
using  NNumberFormat    =     strings::TNumberFormat<nchar>;

/// Type alias in namespace \b alib.
using  WNumberFormat    =     strings::TNumberFormat<wchar>;

/// Type alias in namespace \b alib.
using  XNumberFormat    =     strings::TNumberFormat<xchar>;


/// Type alias in namespace \b alib.
using  Format           =     strings::TFormat<character>;

/// Type alias in namespace \b alib.
using  ComplementFormat =     strings::TFormat<complementChar >;

/// Type alias in namespace \b alib.
using  StrangeFormat    =     strings::TFormat<strangeChar >;

/// Type alias in namespace \b alib.
using  NFormat          =     strings::TFormat<nchar>;

/// Type alias in namespace \b alib.
using  WFormat          =     strings::TFormat<wchar>;

/// Type alias in namespace \b alib.
using  XFormat          =     strings::TFormat<xchar>;

/// Type alias in namespace \b alib.
using  StringLengthResetter          =     strings::TStringLengthResetter<character>;

/// Type alias in namespace \b alib.
using  ComplementStringLengthResetter=     strings::TStringLengthResetter<complementChar>;

/// Type alias in namespace \b alib.
using  StrangeStringLengthResetter   =     strings::TStringLengthResetter<strangeChar >;

/// Type alias in namespace \b alib.
using  NStringLengthResetter         =     strings::TStringLengthResetter<nchar>;

/// Type alias in namespace \b alib.
using  WStringLengthResetter         =     strings::TStringLengthResetter<wchar>;

/// Type alias in namespace \b alib.
using  XStringLengthResetter         =     strings::TStringLengthResetter<xchar>;

} // namespace [alib]


// #################################################################################################
// Macros
// #################################################################################################

#if !ALIB_CHARACTERS_WIDE
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  src;
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  alib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  alib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  alib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_WIDE_ARG     src,bufSize      )  alib::strings::TLocalString<nchar,bufSize>(src);
#else
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  alib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  alib::strings::TLocalString<nchar,bufSize>(src);
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  alib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  alib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_FROM_WIDE_ARG(    src,bufSize      )  src;
#endif



#endif // HPP_ALIB_STRINGS_FWDS

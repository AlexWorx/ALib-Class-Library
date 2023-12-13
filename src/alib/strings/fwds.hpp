/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_strings of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
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
#if !defined(ALIB_DEBUG_STRINGS)
#   define   ALIB_DEBUG_STRINGS 0
#elif !ALIB_DEBUG && ALIB_DEBUG_STRINGS
#   undef    ALIB_DEBUG_STRINGS
#   define   ALIB_DEBUG_STRINGS           0
#   pragma message "Symbol ALIB_DEBUG_STRINGS set (from outside!) while ALIB_DEBUG is not. The symbol got disabled."
#endif

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
namespace aworx { namespace lib { namespace strings {

    template<typename TChar>                     class  TString;
    template<typename TChar>                     class  TSubstring;
    template<typename TChar>                     class  TCString;
    template<typename TChar>                     class  TAString;
    template<typename TChar, integer TCapacity>  class  TLocalString;
    template<typename TChar>                     class  TStringNZT;

    template<typename TChar>                     struct TNumberFormat;
    template<typename TChar>                     class  TFormat;

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

}} // namespace aworx[::lib::strings]

// #################################################################################################
// Alias names
// #################################################################################################

/// Type alias in namespace #aworx.
using  String           =     lib::strings::TString    <character>;

/// Type alias in namespace #aworx.
using  ComplementString =     lib::strings::TString    <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeString    =     lib::strings::TString    <strangeChar>;

/// Type alias in namespace #aworx.
using  NString          =     lib::strings::TString    <nchar>;

/// Type alias in namespace #aworx.
using  WString          =     lib::strings::TString    <wchar>;

/// Type alias in namespace #aworx.
using  XString          =     lib::strings::TString    <xchar>;

/// Type alias in namespace #aworx.
using  Substring        =     lib::strings::TSubstring <character>;

/// Type alias in namespace #aworx.
using  ComplementSubstring =  lib::strings::TSubstring <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeSubstring =     lib::strings::TSubstring <strangeChar>;

/// Type alias in namespace #aworx.
using  NSubstring       =     lib::strings::TSubstring <nchar>;

/// Type alias in namespace #aworx.
using  WSubstring       =     lib::strings::TSubstring <wchar>;

/// Type alias in namespace #aworx.
using  XSubstring       =     lib::strings::TSubstring <xchar>;

/// Type alias in namespace #aworx.
using  CString          =     lib::strings::TCString   <character>;

/// Type alias in namespace #aworx.
using  ComplementCString=     lib::strings::TCString   <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeCString   =     lib::strings::TCString   <strangeChar>;

/// Type alias in namespace #aworx.
using  NCString         =     lib::strings::TCString   <nchar>;

/// Type alias in namespace #aworx.
using  WCString         =     lib::strings::TCString   <wchar>;

/// Type alias in namespace #aworx.
using  XCString         =     lib::strings::TCString   <xchar>;

/// Type alias in namespace #aworx.
using  AString          =     lib::strings::TAString   <character>;

/// Type alias in namespace #aworx.
using  ComplementAString=     lib::strings::TAString   <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeAString   =     lib::strings::TAString   <strangeChar>;

/// Type alias in namespace #aworx.
using  NAString         =     lib::strings::TAString   <nchar>;

/// Type alias in namespace #aworx.
using  WAString         =     lib::strings::TAString   <wchar>;

/// Type alias in namespace #aworx.
using  XAString         =     lib::strings::TAString   <xchar>;

/// Type alias in namespace #aworx.
using  StringNZT        =     lib::strings::TStringNZT <character>;

/// Type alias in namespace #aworx.
using  ComplementStringNZT=   lib::strings::TStringNZT <complementChar>;

/// Type alias in namespace #aworx.
using  StrangeStringNZT =     lib::strings::TStringNZT <strangeChar>;

/// Type alias in namespace #aworx.
using  NStringNZT       =     lib::strings::TStringNZT <nchar>;

/// Type alias in namespace #aworx.
using  WStringNZT       =     lib::strings::TStringNZT  <wchar>;

/// Type alias in namespace #aworx.
using  XStringNZT       =     lib::strings::TStringNZT  <xchar>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using LocalString       =     lib::strings::TLocalString<character, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using ComplementLocalString=  lib::strings::TLocalString<complementChar, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using StrangeLocalString=     lib::strings::TLocalString<strangeChar, TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using NLocalString      =     lib::strings::TLocalString <nchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using WLocalString      =     lib::strings::TLocalString <wchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using XLocalString      =     lib::strings::TLocalString <xchar    , TCapacity>;

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
using  NumberFormat     =     lib::strings::TNumberFormat<character>;

/// Type alias in namespace #aworx.
using  ComplementNumberFormat=lib::strings::TNumberFormat<complementChar>;

/// Type alias in namespace #aworx.
using  StrangeNumberFormat=   lib::strings::TNumberFormat<strangeChar>;

/// Type alias in namespace #aworx.
using  NNumberFormat    =     lib::strings::TNumberFormat<nchar>;

/// Type alias in namespace #aworx.
using  WNumberFormat    =     lib::strings::TNumberFormat<wchar>;

/// Type alias in namespace #aworx.
using  XNumberFormat    =     lib::strings::TNumberFormat<xchar>;


/// Type alias in namespace #aworx.
using  Format           =     lib::strings::TFormat<character>;

/// Type alias in namespace #aworx.
using  ComplementFormat =     lib::strings::TFormat<complementChar >;

/// Type alias in namespace #aworx.
using  StrangeFormat    =     lib::strings::TFormat<strangeChar >;

/// Type alias in namespace #aworx.
using  NFormat          =     lib::strings::TFormat<nchar>;

/// Type alias in namespace #aworx.
using  WFormat          =     lib::strings::TFormat<wchar>;

/// Type alias in namespace #aworx.
using  XFormat          =     lib::strings::TFormat<xchar>;

} // namespace [aworx]


// #################################################################################################
// Macros
// #################################################################################################

#if !ALIB_CHARACTERS_WIDE
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  src;
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  aworx::lib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  aworx::lib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  decltype(src)& dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  aworx::lib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_WIDE_ARG     src,bufSize      )  aworx::lib::strings::TLocalString<nchar,bufSize>(src);
#else
#   define ALIB_STRINGS_TO_NARROW(        src,dest,bufSize )  aworx::lib::strings::TLocalString<nchar,bufSize> dest(src);
#   define ALIB_STRINGS_TO_NARROW_ARG(    src,bufSize      )  aworx::lib::strings::TLocalString<nchar,bufSize>(src);
#   define ALIB_STRINGS_TO_WIDE(          src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_TO_WIDE_ARG(      src,bufSize      )  src;
#   define ALIB_STRINGS_FROM_NARROW(      src,dest,bufSize )  aworx::lib::strings::TLocalString<wchar,bufSize> dest(src);
#   define ALIB_STRINGS_FROM_NARROW_ARG(  src,bufSize      )  aworx::lib::strings::TLocalString<wchar,bufSize>(src);
#   define ALIB_STRINGS_FROM_WIDE(        src,dest,bufSize )  decltype(src)& dest= src;
#   define ALIB_STRINGS_FROM_WIDE_ARG(    src,bufSize      )  src;
#endif



#endif // HPP_ALIB_STRINGS_FWDS

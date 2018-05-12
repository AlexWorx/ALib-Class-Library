// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// Include guard
#ifndef HPP_ALIB_STRINGS_STRING
#define HPP_ALIB_STRINGS_STRING 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
#   error "include 'alib/alib.hpp' instead of this header"
#endif

#include <memory>



// #################################################################################################
// Forward declarations
// #################################################################################################
namespace aworx { namespace lib { namespace strings {


    // forwards
    template<typename TChar>                    class SubstringBase;
    template<typename TChar>                    class TStringBase;
    template<typename TChar, size_t  TLength>   class StringLiteralBase;
    template<typename TChar>                    class AStringBase;
    template<typename TChar, integer TCapacity> class PreallocatedStringBase;

    template<typename TChar>                    class NumberFormatBase;
    template<typename TChar>                    class FormatBase;

    namespace format {

        template<typename TChar>   class FormatterBase;
        template<typename TChar>   class FormatterPythonStyleBase;
        template<typename TChar>   class FormatterJavaStyleBase;
        template<typename TChar>   class SimpleTextBase;

    } // namespace aworx::lib::strings[::util]

    namespace util {

        template< typename... TAssociatedTypes > class StringTable;

    } // namespace aworx::lib::strings[::util]

}}} // namespace aworx::[lib::strings]


#include "alib/strings/cstring.hpp"
#include "alib/strings/string.hpp"
#include "alib/strings/tstring.hpp"
#include "alib/strings/stringliteral.hpp"


// #################################################################################################
// Alias names
// #################################################################################################
namespace aworx {

/// Type alias in namespace #aworx.
using  NString          =     aworx::lib::strings::StringBase       <nchar>;

/// Type alias in namespace #aworx.
using  WString          =     aworx::lib::strings::StringBase       <wchar  >;

/// Type alias in namespace #aworx.
using  String           =     aworx::lib::strings::StringBase       <character>;

/// Type alias in namespace #aworx.
using  NSubstring       =     aworx::lib::strings::SubstringBase    <nchar    >;

/// Type alias in namespace #aworx.
using  WSubstring       =     aworx::lib::strings::SubstringBase    <wchar    >;

/// Type alias in namespace #aworx.
using  Substring        =     aworx::lib::strings::SubstringBase    <character>;

/// Type alias in namespace #aworx.
using  NTString         =     aworx::lib::strings::TStringBase      <nchar    >;

/// Type alias in namespace #aworx.
using  WTString         =     aworx::lib::strings::TStringBase      <wchar    >;

/// Type alias in namespace #aworx.
using  TString          =     aworx::lib::strings::TStringBase      <character>;

/// Type alias in namespace #aworx.
template<size_t TLength>
using  NSLiteral        =     aworx::lib::strings::StringLiteralBase<nchar    , TLength>;

/// Type alias in namespace #aworx.
template<size_t TLength>
using  WSLiteral        =     aworx::lib::strings::StringLiteralBase<wchar    , TLength>;

/// Type alias in namespace #aworx.
template<size_t TLength>
using  SLiteral         =     aworx::lib::strings::StringLiteralBase<character, TLength>;

/// Type alias in namespace #aworx.
using  NAString         =     aworx::lib::strings::AStringBase      <nchar    >;

/// Type alias in namespace #aworx.
using  WAString         =     aworx::lib::strings::AStringBase      <wchar    >;

/// Type alias in namespace #aworx.
using  AString          =     aworx::lib::strings::AStringBase      <character>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using NPreallocatedString=    aworx::lib::strings::PreallocatedStringBase<nchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using WPreallocatedString=    aworx::lib::strings::PreallocatedStringBase<wchar    , TCapacity>;

/// Type alias in namespace #aworx.
template<integer TCapacity>
using PreallocatedString=     aworx::lib::strings::PreallocatedStringBase<character, TCapacity>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,8>}.
using NString8  =  NPreallocatedString<   8>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,16>}.
using NString16 =  NPreallocatedString<  16>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,32>}.
using NString32 =  NPreallocatedString<  32>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,64>}.
using NString64 =  NPreallocatedString<  64>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,128>}.
using NString128=  NPreallocatedString< 128>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,256>}.
using NString256=  NPreallocatedString< 256>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,512>}.
using NString512=  NPreallocatedString< 512>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,1024>}.
using NString1K =  NPreallocatedString<1024>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,2048>}.
using NString2K =  NPreallocatedString<2048>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<nchar\,4096>}.
using NString4K =  NPreallocatedString<4096>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,8>}.
using WString8  =  WPreallocatedString<   8>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,16>}.
using WString16 =  WPreallocatedString<  16>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,32>}.
using WString32 =  WPreallocatedString<  32>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,64>}.
using WString64 =  WPreallocatedString<  64>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,128>}.
using WString128=  WPreallocatedString< 128>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,256>}.
using WString256=  WPreallocatedString< 256>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,512>}.
using WString512=  WPreallocatedString< 512>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,1024>}.
using WString1K =  WPreallocatedString<1024>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,2048>}.
using WString2K =  WPreallocatedString<2048>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<wchar\,4096>}.
using WString4K =  WPreallocatedString<4096>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,8>}.
using String8  =  PreallocatedString<   8>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,16>}.
using String16 =  PreallocatedString<  16>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,32>}.
using String32 =  PreallocatedString<  32>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,64>}.
using String64 =  PreallocatedString<  64>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,128>}.
using String128=  PreallocatedString< 128>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,256>}.
using String256=  PreallocatedString< 256>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,512>}.
using String512=  PreallocatedString< 512>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,1024>}.
using String1K =  PreallocatedString<1024>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,2048>}.
using String2K =  PreallocatedString<2048>;

/// Type alias name for \alib{strings,PreallocatedStringBase,PreallocatedStringBase<character\,4096>}.
using String4K =  PreallocatedString<4096>;


/// Type alias in namespace #aworx.
using  NNumberFormat    =     aworx::lib::strings::NumberFormatBase <nchar    >;

/// Type alias in namespace #aworx.
using  WNumberFormat    =     aworx::lib::strings::NumberFormatBase <wchar    >;

/// Type alias in namespace #aworx.
using  NumberFormat     =     aworx::lib::strings::NumberFormatBase <character>;

/// Type alias in namespace #aworx.
using  NFormat          =     aworx::lib::strings::FormatBase <nchar    >;

/// Type alias in namespace #aworx.
using  WFormat          =     aworx::lib::strings::FormatBase <wchar    >;

/// Type alias in namespace #aworx.
using  Format           =     aworx::lib::strings::FormatBase <character>;


/// Type alias in namespace #aworx.
using  NFormatter       =     aworx::lib::strings::format::FormatterBase<nchar    >;

/// Type alias in namespace #aworx.
using  WFormatter       =     aworx::lib::strings::format::FormatterBase<wchar    >;

/// Type alias in namespace #aworx.
using  Formatter        =     aworx::lib::strings::format::FormatterBase<character>;

/// Type alias in namespace #aworx.
using  NFormatterPythonStyle= aworx::lib::strings::format::FormatterPythonStyleBase<nchar   >;

/// Type alias in namespace #aworx.
using  WFormatterPythonStyle= aworx::lib::strings::format::FormatterPythonStyleBase<wchar   >;

/// Type alias in namespace #aworx.
using  FormatterPythonStyle=  aworx::lib::strings::format::FormatterPythonStyleBase<character>;

/// Type alias in namespace #aworx.
using  NFormatterJavaStyle=   aworx::lib::strings::format::FormatterJavaStyleBase<nchar   >;

/// Type alias in namespace #aworx.
using  WFormatterJavaStyle=   aworx::lib::strings::format::FormatterJavaStyleBase<wchar   >;

/// Type alias in namespace #aworx.
using  FormatterJavaStyle=    aworx::lib::strings::format::FormatterJavaStyleBase<character>;

/// Type alias in namespace #aworx.
using  NSimpleText      =     aworx::lib::strings::format::SimpleTextBase<nchar    >;

/// Type alias in namespace #aworx.
using  WSimpleText      =     aworx::lib::strings::format::SimpleTextBase<wchar    >;

/// Type alias in namespace #aworx.
using  SimpleText       =     aworx::lib::strings::format::SimpleTextBase<character>;

/**
 * Shared pointer to to \alib{strings::format,FormatterBase,standard formatter}.
 *
 * \see Namespace variable \alib{strings,Strings::GetDefaultFormatter}
 */
using   SPFormatter  =    std::shared_ptr<lib::strings::format::FormatterBase<character>>;



// #################################################################################################
// Constants
// #################################################################################################
/// A constant \e nulled \alib string.
constexpr NString NNullString;

/// A constant \e nulled \alib string.
constexpr WString WNullString;

/// A constant \e nulled \alib string.
constexpr String  NullString;

/// A constant empty (but not \e nulled) \alib string
constexpr NString NEmptyString {"", 0};

/// A constant empty (but not \e nulled) \alib string
constexpr WString WEmptyString {L"", 0};

/// A constant empty (but not \e nulled) \alib string
constexpr String  EmptyString  {ASTR(""), 0};

#if defined(_WIN32) || defined(DOX_PARSER)
/// The system dependent new line character code(s).
constexpr static NSLiteral<2>  NNewLine {      "\r\n"  };

/// The system dependent new line character code(s).
constexpr static WSLiteral<2>  WNewLine {     L"\r\n"  };

/// The system dependent new line character code(s).
constexpr static  SLiteral<2>   NewLine { ASTR("\r\n") };
#else
constexpr static NSLiteral<1>  NNewLine {      "\n"    };
constexpr static WSLiteral<1>  WNewLine {     L"\n"    };
constexpr static  SLiteral<1>   NewLine { ASTR("\n")   };
#endif

/// Characters that are usually ignored or trimmed.
constexpr static NSLiteral<4>   NDefaultWhitespaces {      " \n\r\t"  };

/// Characters that are usually ignored or trimmed.
constexpr static WSLiteral<4>   WDefaultWhitespaces {     L" \n\r\t"  };

/// Characters that are usually ignored or trimmed.
constexpr static SLiteral<4>    DefaultWhitespaces { ASTR(" \n\r\t") };


/**
 * This empty template class has two specializations for template types \c nchar and \c wchar  ,
 * which each provide static fields:
 * - \b Null,
 * - \b Empty,
 * - \b NewLine and
 * - \b DefaultWhitespaces.
 *
 * The class is useful to implement generic template methods. In non-generic specializations
 * it is more efficient to use \c constexpr namespace members
 * - \ref NNullString, \ref WNullString, \ref NullString,
 * - \ref NEmptyString, \ref WEmptyString, \ref EmptyString,
 * - \ref NNewLine, \ref WNewLine, \ref NewLine and
 * - \ref NDefaultWhitespaces, \ref WDefaultWhitespaces, \ref DefaultWhitespaces.
 *
 * @tparam TChar The character type.
 */
template<typename TChar> struct StringConstants {};

//! @cond NO_DOX
template<> struct StringConstants<nchar  >
{
    ALIB_API static NString       NullString;
    ALIB_API static NString       EmptyString;
#if defined(_WIN32)
    ALIB_API static NSLiteral<2>  NewLine;
#else
    ALIB_API static NSLiteral<1>  NewLine;
#endif
    ALIB_API static NSLiteral<4>  DefaultWhitespaces;
};
template<> struct StringConstants<wchar  >
{
    ALIB_API static WString       NullString;
    ALIB_API static WString       EmptyString;
#if defined(_WIN32)
    ALIB_API static WSLiteral<2>  NewLine;
#else
    ALIB_API static WSLiteral<1>  NewLine;
#endif
    ALIB_API static WSLiteral<4>  DefaultWhitespaces;
};
//! @endcond




} // namespace aworx

#endif // HPP_ALIB_STRINGS_STRING

// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if ALIB_MODULE_BOXING
#  if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER)
#     include "alib/strings/format/formatter.hpp"
#  endif
#  if !defined (HPP_ALIB_STRINGS_FORMAT_FORMATTER_PYTHONSTYLE)
#     include "alib/strings/format/formatterpythonstyle.hpp"
#  endif
#endif



namespace aworx {

//! @cond NO_DOX

NString      StringConstants<char   >::NullString        =  nullptr ;
NString      StringConstants<char   >::EmptyString       =  "";
NSLiteral<4> StringConstants<char   >::DefaultWhitespaces=  {  " \n\r\t"  };

WString      StringConstants<wchar_t>::NullString        =  nullptr ;
WString      StringConstants<wchar_t>::EmptyString       =  L"";
WSLiteral<4> StringConstants<wchar_t>::DefaultWhitespaces=  { L" \n\r\t"  };

#if defined(_WIN32)
NSLiteral<2> StringConstants<char   >::NewLine           =  {  "\r\n"       };
WSLiteral<2> StringConstants<wchar_t>::NewLine           =  { L"\r\n"       };
#else
NSLiteral<1> StringConstants<char   >::NewLine           =  {  "\n"       };
WSLiteral<1> StringConstants<wchar_t>::NewLine           =  { L"\n"       };
#endif
//! @endcond


namespace lib {

// The singleton of the library class located in namespace aworx::lib.
strings::Strings STRINGS;

ALIB_WARNINGS_IGNORE_DOCS // clang and doxygen... well, but we keep clang warnings otherwise

/** ************************************************************************************************
 @addtogroup GrpALibMacros
 @{
 @name Macros Supporting ALib Strings
 @{
 \def ASTR
    Macro used to express C++ string literals in a platform independent way. Dependent on
    code selection symbol \ref ALIB_NARROW_STRINGS, this macro defines string literals
    as either narrow character strings or wide character strings.

    Whenever a software targets different platforms that change the underlying character width of
    the string types, the string literals have to change as well. Therefore, almost all string
    literals in \alib sources are enclosed by this macro. The exclamation are of-course such
    literals which are assigned to string types of a specific, non-altering width.

    The following sample demonstrates the use of the macro:
    \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_MACRO_ASTR

    \note To keep this frequently used macro short, it does not use prefix <b>"ALIB_"</b>.

    @param STR The string literal to optionally prefix with <c>'L'</c>, <c>'u'</c> or <c>'U'</c>.

 \def COMPLSTR
    Macro used to express C++ string literals in a platform independent way. Dependent on
    the definition of type \ref aworx::strangeChar (which is in turn dependent on the platform,
    compiler and code selection symbols) this macro defines string literals, which have an either
    two bytes or four byte character width, namely the width corresponding to the width of
    \b %strangeChar.

    \note
      The need to use this macro should be very seldom. \alib itself does not use it internally.
      It is provided for completeness, being complementary to macro \ref ASTR.

     @param STR The string literal to prefix with <c>'u'</c> or <c>'U'</c> .

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY
    Helper macro for specializing struct
    \alib{strings,T_Apply} for type \p{TYPE}.
    This macro has to be positioned outside of any namespace and the given type has to include
    its full namespace qualification.

    This macro is to be used in combination with macro #ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF.<br>
    As an alternative to the two macros, #ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE might be used, which will
    specialize \c T_Apply and define method \b %Apply inline.

    \see \ref alib_namespace_strings_astring_applyto "Applying Objects to AStrings"

    @param TYPE The type to specialize struct \b %T_Apply for.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_N
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY but for character type \ref aworx::nchar.

    @param TYPE The type to specialize struct \b %T_Apply for.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_W
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY but for character type \ref aworx::wchar.

    @param TYPE The type to specialize struct \b %T_Apply for.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF
    Macro used in combination with \ref ALIB_STRINGS_SPECIALIZE_T_APPLY which specializes struct
    \alib{strings,T_Apply} for type \p{TYPE} and standard character type, and with this declares its
    member function \b %Apply.<br>
    This macro is used for the implementation of this member function.

    @param TYPE  The type that struct \b %T_Apply was specialized for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF_N
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF but for character type \ref aworx::nchar.

    @param TYPE  The type that struct \b %T_Apply was specialized for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF_W
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF but for character type \ref aworx::wchar.

    @param TYPE  The type that struct \b %T_Apply was specialized for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE
    Helper macro for specializing struct
    \alib{strings,T_Apply} for a custom type \p{TYPE}.
    This macro has to be positioned outside of any namespace and the given type has to include
    its full namespace qualification.

    With the specialization of struct \b %T_Apply, method \b %Apply will be defined and
    implemented inline.

    Macros #ALIB_STRINGS_SPECIALIZE_T_APPLY and #ALIB_STRINGS_SPECIALIZE_T_APPLY_DEF
    provide a non-inline alternative to this macro.

    \see \ref alib_namespace_strings_astring_applyto "Applying Objects to AStrings"

    @param TYPE The type to specialize struct \b %T_Apply for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE_N
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE but for character type \ref aworx::nchar.

    @param TYPE The type to specialize struct \b %T_Apply for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE_W
    Same as \ref ALIB_STRINGS_SPECIALIZE_T_APPLY_INLINE but for character type \ref aworx::wchar.

    @param TYPE The type to specialize struct \b %T_Apply for.
    @param IMPL The implementation code for method \b %Apply.

 \def ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR
    Helper macro for specializing struct
    \alib{strings::thirdparty::std,T_SuppressStdOstreamOperator} for a custom type
    \p{TYPE}.
    This macro has to be positioned outside of any namespace and the given type has to include
    its full namespace qualification.

    @param TYPE The type to specialize struct \b %T_Apply for.
 @}
 @}

 This is namespace documentation starts with a comprehensive programmer's manual.<br>
<b>Reference documentation</b> is found below:

  - [Nested Namespaces](#namespaces)
  - [Classes](#nested-classes)
  - [Typedefs](#typedef-members) and
  - [Functions](#func-members)


 # %ALib %Strings - Programmer's Manual #

 <b>Contents:</b>

 \ref                   alib_strings_intro                 "1. Introduction"
 <br> &nbsp;&nbsp; \ref alib_strings_intro_hell             "1.1 The C++ String Hell"
 <br> &nbsp;&nbsp; \ref alib_strings_intro_goals            "1.2 Library Design Goals"
 <br> &nbsp;&nbsp; \ref alib_strings_intro_feat             "1.3 Library Features"

 \ref                   alib_strings_characters            "2. Characters"
 <br> &nbsp;&nbsp; \ref alib_strings_char_hell               "2.1 The Character Hell"
 <br> &nbsp;&nbsp; \ref alib_strings_char_by_width           "2.2 ALib Character Types By Widths"
 <br> &nbsp;&nbsp; \ref alib_strings_char_by_logic           "2.3 ALib Logical Character Types"
 <br> &nbsp;&nbsp; \ref alib_strings_char_summary            "2.4 Character Type Summary"

 \ref                   alib_strings_templated             "3. Templated String Types"

 \ref                   alib_strings_classes               "4. The String Classes"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_String          "4.1 Class String"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_String_custom      "4.1.1 Binding User Types To %ALib Strings"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_String_nullable    "4.1.2 Nullable %Strings"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_String_term        "4.1.3 No Zero-Termination"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_TString         "4.2 Class TString"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_AString         "4.3 Class AString"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_AString_constr     "4.3.1 Construction"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_AString_bufmngmnt  "4.3.2 Buffer Management"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_AString_apply      "4.3.3 Appending and Applying Objects"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_PAString        "4.4 Class PreallocatedString"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_PAString_bo        "4.4.1 Exceeding the Buffer Capacity"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_PAString_constr    "4.4.2 Implicit Construction"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_PAString_move      "4.4.3 No Move Constructor"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_PAString_aliases   "4.4.4 Aliases For Frequently Used Sizes"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_Substring       "4.5 Class Substring"
 <br> &nbsp;&nbsp; \ref alib_strings_classes_StringLiteral   "4.6 Class StringLiteral"

 \ref                   alib_strings_details               "5. Details On Strings"
 <br> &nbsp;&nbsp; \ref alib_strings_details_nonchecking     "5.1 \"Non-Checking Methods\" Of ALib String Classes"
 <br> &nbsp;&nbsp; \ref alib_strings_details_format          "5.2 Formatting Strings"
 <br> &nbsp;&nbsp; \ref alib_strings_details_compat          "5.3 Compatibility with External Libraries"
 <br> &nbsp;&nbsp; \ref alib_strings_details_debug           "5.4 Debugging ALib String Classes"


 \~Comment ####################################################################################### \~
 \anchor alib_strings_intro
 # 1. Introduction #

 \~Comment ####################################################################################### \~
 \anchor alib_strings_intro_hell
 ## 1.1 The C/C++ String Hell ##
 C++ programmer's do not need to get motivated about using a 3rd-party string library. This is true
 due to the fact that the language itself does not offer a powerful built-in type that solves their
 daily needs.

 Even worse, there are two types of character strings available:
 1. Traditional zero-terminated string buffers, also called a "c-string" (in this manual \e cstring), and
 2. C++ <c>std::basic_string<CharT></c> template class.

 Even if \e cstrings are not supposed to be used in modern software, they still are needed and
 will be in the (long) future, because they until today are used when strings are exchanged between
 software units, especially when it comes to operating system function calls.

 \note
   We, the makers of \alib, very much agree to the theses that the invention of zero-terminated
   strings during the previous millennium, was one of the
   [most expensive one-byte mistakes](http://queue.acm.org/detail.cfm?id=2010365) of IT-history!

 The second type, which is the base for \c std::string and \c std::wstring is a nice feature
 of the standard library, but - in the its typical, rightfully minimalistic fashion - there are
 no higher level string features available with them. But apart from such higher level features
 that are found in string types of almost any programing language (see Java, C+, Python, etc.)
 the single missing feature that really constitute the need of a 3rd party string is the fact
 that class std::basic_string does not allow to distinguish between strings that are constant
 and immutable and those that have a dynamically managed (growing) buffer attached. The following
 simple line of code:

        std::string  s= "string";

 claims heap memory and copies the characters into the allocated string buffer. To avoid this overhead,
 a programmer would often write

        const char* s= "string";

 Unfortunately, now we have a cstring: This means the information about the string's length
 disappeared. So, where did it go? Well, technically it was "casted away" by the
 compiler. Therefore a programmer could write:

        char s[]= "string";

 and voila, the type of variable \c s would become <c>char[7]</c>. But why \c 7, not \c 6? Because
 the array is created from a cstring, which is a zero-terminated array and the character array
 includes this termination character. So, the array somehow has the "wrong" length. Apart from this,
 the length is stuck in the type, hence known to the compiler but not at run-time. Therefore,
 a string class is needed that comprises the start of the string's buffer together with the length
 of the string - just as \c std::string does, but without the allocation of heap memory.

 As of C++ 17, new string class \c std::basic_string_view was added, solving exactly this:

        std::string_view s= "string";

 A next stumbling block using C++ strings is the width of the characters. When type
 \c wchar_t was introduced, most people thought that this will solve most of the problems when
 it comes to international character representations and uni-code. Unfortunately, the promise
 was (could!) not be fulfilled because of the complexity of languages and writing codes of different
 human cultures worldwide. Even worse, the width of \c wchar_t was not specified by the language,
 therefore different compilers and platforms implement it either as two or four byte integral type.
 Even one byte implementation is theoretically allowed by the language specification!

 To make it short: Any more complex C++ software needs to rely on some 3rd party string library.
 The string classes provided with the standard library are a basic tool for simple programs, but
 do not have and probably will never have the aspiration to provide the convenience that
 a more "high level" library will provide in this area.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_intro_goals
 ## 1.2 Library Design Goals ##

 The design goals of module \alibmod_nolink_strings are:

 <b>1. The Provision of high level string features</b><br>
 \par
    As explained in the previous chapter, the C++ standard library does not have this goal, hence
    such features are needed to be added.

 <b>2. Mitigate the "%CString-Problem"</b><br>
 \par
    Zero-terminated strings are considered "evil", their use is to be reduced to a safe minimum but
    if necessary, their use should still be transparent and convenient.


 <b>3. Hide character widths</b><br>
 \par
    Unlike with types \c std::string, \c std::wstring or for example \c QString of the
    [QT Class Library](https://www.qt.io), the character width of \alib strings should be not fixed,
    but rather only "preferred". This preference should depend on the platform and compilation settings.
    Only in special situations, a programmer needs to start caring about the underlying character width.
    Hence code should compile without changes with different underlying string width.
    Furthermore, strings of different width can be conveniently converted or even transparently used in a mixed
    fashion.

 <b>4. Compatibility with \c std::string as well as any other 3rd-party string</b><br>
 \par
    An \alib principal goal is "non-intrusiveness". Hence, \alib strings are always considered to
    co-exist with other string types used in the same software module. Therefore, a last but
    not least important goal of \alib strings library design is to provide seamless conversion from
    any other 3rd-party string type.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_intro_feat
 ## 1.3 Library Features ##
 With the definition of the general problem and the resulting goals, the feature list of
 module \alibmod_nolink_strings can be well understood. It is:

 - <b>Implicit string object construction</b> from virtually any custom/3rd party string type.
   This is implemented using some template meta programming. Some of the consequences of this feature are:
   - If a function/method that accepts a **const aworx::String&**, then any object that
     "smells like a string" may be passed to this function.
   - This means, there is no need to overload methods to accept different string types:
     One method takes them all.
   - On the other hand, there is no need to convert string types when passing them.
     This means that users of libraries which use **ALib Strings**, do not need to know about
     ALib Strings. Instead, users just pass strings "as they are" (aka the 3rd party type directly).
     We call this "non-intrusiveness".
   <br> <p>

 - Concept of <b>"application of objects to strings"</b>, for example using operator <c><<</c>:

        AString() << anyObject;

   Such "application" of arbitrarily typed objects usually means:
   "appending a human readable representation of the object to the string". This is very similar to
   <c>"Object.ToString()"</c> methods in other programming languages. Details hereof are:
   - Support for fundamental C++ types included (of-course). Furthermore support for some
     types of the standard library as well as 3rd-party library types included.
   - Special "applicable types" are included that allow formatting of data, upper/lower conversion, etc.
     For example:

           AString() << Format::Hex(65536);

   - Enabling support for custom types is usually a matter of a few lines of code.
   - In contrast to having *ToString()* methods (like Java, C# have), this TMP approach allows to add
     support for custom types without "touching" those custom types.
     Again, we call this "non-intrusiveness of ALib": Any existing 3rd-party type becomes
     "applicable" to class \b %AString.
     <br>
   <br> <p>
 - Handles zero terminated cstrings as well as non-zero terminated strings and provides a
   reasonable, <b>mostly seamless bridge</b> between the two.
   <br> <p>

 - Multi-byte character and unicode support and conversion between strings of differend widths
   (1-byte, 2-byte and 4-byte character widths).
   <br> <p>

 - Various <b>tool/helper types</b>, for example:
   - String \b formatters supporting format strings in \alib{strings::format,FormatterPythonStyleBase,Python style}
     as well as in \alib{strings::format,FormatterJavaStyleBase,Java/printf style}, including:
     - hexadecimal, binary and octal number formatting
     - date/time formatting
     - custom type formatting, optionally including custom format string specifications
   - String \alib{strings,SubstringBase,parsing} and \alib{strings::util,Tokenizer,tokenizing}
   - \alib{strings::util,WildcardMatcherBase,Wildcard matching} (using <c>'*'</c> and <c>'?'</c>) and
     \alib{strings::util,RegexMatcher,Regular expression matching}.
   - String \alib{strings,SubstringBase,parsing} and \alib{strings::util,Tokenizer,tokenizing}
   - Special substring search using \alib{strings::util,SubstringSearchBase,Knuth-Morris-Pratt} algorithm.
   - Simple \alib{strings::format,SimpleTextBase,text formatting}, e.g. for nicely indented and
     structured console or log-file output.

 \note
   Some of the features are only available when module \alibmod_nolink_strings is used in combination
   with other \alib modules or the complete \alib distribution. Especially the formatters extensively
   use features of module \alibmod_nolink_boxing.

 <br>
 \note
   <b>%ALib %Strings</b> are available for \b C# (.Net) and \b Java as well. Of-course there are
   some differences, but the goal here is to provide a certain level of source-compatibility within
   <b>cross-language projects</b>.
   C# and Java versions of the library can be downloaded bundled with
   [ALox Logging Library](https://github.com/AlexWorx/ALox-Logging-Library).

 \~Comment ####################################################################################### \~
 \anchor alib_strings_characters
 # 2. Characters #
 %Strings are arrays of characters. Unfortunately (because it makes it all a little complex) before
 talking about strings, some theory and basics about characters have to be discussed.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_char_hell
 ## 2.1 The C++ Character Hell ##
 In the previous introduction sections, the situation with C++ and character strings has already
 been named a "hell". This is mainly due to two things: The invention and continued use of zero-terminated
 strings ("cstrings") and secondly the encoding of international character sets within strings of
 <b>different character width</b>. Isolating only the latter of the problems can be named
 "a hell" in itself already - in that respect that the consistent and rightful treatment
 of single characters can become very complex already. Furthermore testing software in different
 cultures (locales) may become a very expensive process.

 This programmer's manual can not elaborate on this topic. To understand the rationals behind
 the design decisions and to make best effective use of module \alibmod_nolink_strings, it is
 important to know the basics and today's state of discussion.
 For this, some links are provided here:

 - [ASCII, Wikipedia](https://en.wikipedia.org/wiki/ASCII)
 - [Unicode, Wikipedia](https://en.wikipedia.org/wiki/Unicode)
 - [UTF-8 ](https://en.wikipedia.org/wiki/UTF-8),
   [UTF-16](https://en.wikipedia.org/wiki/UTF-16),
   [UTF-32](https://en.wikipedia.org/wiki/UTF-32) (all Wikipedia)
 - [UTF-8 Everywhere](http://utf8everywhere.org/), a "manifesto" on the use of UTF-8 written by
   people that have a strong opinion. Absolutely worth to read.
 - [ICU Library](http://site.icu-project.org/). See the more general and introduction/tutorial
   parts of their documentation.

 The fact that choosing "the right" character width to efficiently implement C++ strings is a
 non-trivial task, is already shown by the following facts:
 1. GNU C++ and Clang compilers define \c wchar_t as a <b>4-byte</b> integral type under GNU/Linux.
 2. Microsoft Visual Studio C++ compiler for Windows defines \c wchar_t as a <b>2-byte</b> integral type.
 3. Famous and wide-spread [QT Class Library](https://www.qt.io) uses a <b>2-byte</b> integral type
    on both platforms, GNU/Linux and Windows OS.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_char_by_width
 ## 2.2 %ALib Character Types By Widths ##
 In general, \alib differs between two scenarios when it comes to choosing the underlying
 character width of a string:
 1. Explicit choice, or
 2. Logical choice (or no choice, which is using whatever is the default).

 The explicit choice of the character width is the less frequently approach and rather used
 internally in the library and when it comes to implementing explicit string functionality.
 For this, type definitions
 - \ref aworx::nchar and
 - \ref aworx::wchar

 are provided.<br>
 While \b nchar is always an alias name for C++ type \c char, type \c wchar might
 resolve to either of built-in types \c wchar_t, \c char16_t or \c char32_t.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_char_by_logic
 ## 2.3 %ALib Logical Character Types ##

 While the two explicit character types \ref aworx::nchar "nchar" and \ref aworx::wchar "wchar" are
 aliasing two of the built-in types (\c char, \c wchar_t, \c char16_t or \c char32_t.), the< themselves
 are aliased by two further type definitions:

 - \ref aworx::character and
 - \ref aworx::complementChar.

 Depending on the platform and/or compilation symbols provided when building the library, one of
 these types aliases \b nchar and the other \b wchar. This logical naming means that a
 \ref aworx::character "character" is the default type for characters used with \alib and type
 \ref aworx::complementChar "complementChar" is just the counterpart.

 All together this results in a three-level type/alias scheme. For example, the default on GNU/Linux
 platform, is:

        aworx::character      <==> aworx::nchar <==> char
        aworx::complementChar <==> aworx::wchar <==> wchar_t

 In contrast to this, under Windows OS the scheme is:

        aworx::character      <==> aworx::wchar <==> wchar_t
        aworx::complementChar <==> aworx::nchar <==> char

 The attentive reader now notices that an \b aworx::character under Windows OS is not equivalent
 to an aworx::complementChar GNU/Linux, although both are aliasing C++ type\c wchar_t: As explained
 above, this type is platform dependent, hence 2-bytes wide on Windows OS and 4-bytes wide on
 GNU Linux.<br>
 Therefore, \alib defines a third logical character type, which holds the third possible width:

 - \ref aworx::strangeChar

 So why did we choose this (strange) name? It should express that this third type is neither compatible with
 \alib narrow characters nor with \alib wide characters.

 Looking back at the GNU/Linux sample (with default compilation options), we then have covered
 all three possible width:

        aworx::character      <==> aworx::nchar <==> char      // 1-byte character
        aworx::complementChar <==> aworx::wchar <==> wchar_t   // 4-byte character
        aworx::strangeChar            <==>           char16_t  // 2-byte character

 \note
   From this perspective, if mixing \alib with strings of [QT Class Library](https://www.qt.io),
   these \c QStrings are based on character type \ref aworx::strangeChar "strangeChar" under GNU/Linux,
   while they are based on \ref aworx::character "character" on Windows OS - because in any case
   \c QString objects have a character width of 2 bytes!

 While it is obvious that the default character type alias \ref aworx::character is needed
 to write platform independent code, the use of alias types \c complementChar and \c strangeChar
 is not so obvious. Mostly again, it is needed internally, but is also very useful when it
 comes to template meta programming with the aim to create functionality that supports any type of
 string. We will see samples of this later in this programmer's manual.

 \note
   When programming types or functions that are templated in respect to to the character type,
   helper struct \ref aworx::T_ComplementChar may be used to identify the complementary character
   type in respect to the template type that is currently compiled. Its two specializations
   <b>%T_ComplementChar\<nchar\></b> and <b>%T_ComplementChar\<wchar\></b> each define inner type
   \b type to being the respectively complementary type.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_char_summary
 ## 2.4 Character Type Summary ##
 Let us quickly summarize what was said in this chapter about character types:
 - Four C++ built-in types exist: \c char, \c char16_t or \c char32_t, \c wchar_t.  While the
   first three have a defined size, the fourth, \c wchar_t is not defined in respect to its byte
   width (it may even be 1-byte wide!).
 - \alib provides two alias types that define characters by their width:
    - \ref aworx::nchar for 1-byte narrow characters, and
    - \ref aworx::wchar for wide characters of either two or four bytes width.<br>
      (The width of \c wchar is not necessarily the same as the width of built-in type \c wchar_t.)

 - \alib provides three  alias types that define characters in a logical sense:
    - \ref aworx::character for default character types. This either resolves to \c nchar or \c wchar.
    - \ref aworx::complementChar providing the complementary character to \b %character.
    - \ref aworx::strangeChar providing the third possible character width which is neither
      \b nchar nor \b wchar.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_templated
 # 3. Templated %String Types #
 Likewise the standard library uses template class \c std::basic_string<CharT>, \alib uses
 template classes for its various string classes and for most of the string helper and utility
 classes.

 Before these classes are presented, it is important to understand the schematic of naming
 string types and placing shortcuts (alias types) of them in namespace \ref aworx.

 The different string classes use a template parameter called p{TChar} and carry the suffix
 <c>"Base"</c> in their name. The central types are:

 - \alib{strings,StringBase,aworx::lib::strings::StringBase<TChar>}
 - \alib{strings,SubstringBase,aworx::lib::strings::SubstringBase<TChar>}
 - \alib{strings,TStringBase,aworx::lib::strings::TStringBase<TChar>}
 - \alib{strings,AStringBase,aworx::lib::strings::AStringBase<TChar>}
 - \alib{strings,PreallocatedStringBase,aworx::lib::strings::PreallocatedStringBase<TChar>}

 Now, for each of the classes, <b>three alias types</b> are defined. For convenience, those are placed
 in namespace \b %aworx (instead of namespace \b %aworx::lib::strings). In respect to their base
 type's name, their name translates using the following scheme:
 - The suffix \b Base is removed
 - With \p{TChar} being logical character type \b character, nothing is prefixed to the name.<br>
   Note that this version of the type is referred to as <b>"default string type"</b> at various places
   in this manual and the reference documentation.
 - With \p{TChar} being character type \b nchar, <c>'N'</c> is prefixed to the name.
 - With \p{TChar} being character type \b wchar, <c>'W'</c> is prefixed to the name.

 For example, for type \b StringBase, this translates to types:
 - \ref aworx::String, being <b>aworx::lib::strings::StringBase<aworx::character></b>
 - \ref aworx::NString, being <b>aworx::lib::strings::StringBase<aworx::nchar></b>
 - \ref aworx::WString, being <b>aworx::lib::strings::StringBase<aworx::wchar></b>

 This schematic is important to understand. Mostly, only the simple form \b "String" is needed and
 this is equivalent to either \b %NString or \b %WString, depending on the whether \c character
 is equivalent to \c nchar, respectively \c wchar.<br>
 As a second sample for this naming scheme, type \c %NAString is equivalent to
 class \b %AStringBase\<nchar\>.

 \note
   In a small exclamation, the name of template class \alib{strings,StringLiteralBase} is shortened
   with the definition of the three types, to
   - \ref aworx::SLiteral,
   - \ref aworx::NSLiteral and
   - \ref aworx::WSLiteral

 To avoid redundancy, in the next sections we finally can start to talk about the string classes,
 their use and functionality. Within these sections, it is only seldom mentioned that in fact templated
 underlying base classes are meant using different character types.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes
 # 4. The String Classes #

  \attention
   In the upcoming chapters of this manual, when for example type \c %String is mentioned, this in
   general means the template base type \b %StringBase\<TChar\> and thus includes types \b %NString or
   \b %WString. The same applies to the other string types, which all follow the same
   naming scheme, as explained in the previous chapter.


  The classes discussed in this chapter are:
   - \alib{strings,StringBase,String}
   - \alib{strings,TStringBase,TString}
   - \alib{strings,AStringBase,AString}
   - \alib{strings,PreallocatedStringBase,PreallocatedString}
   - \alib{strings,StringLiteralBase,StringLiteral}
   - \alib{strings,SubstringBase,Substring}

  The documentation of class \b %String includes an \alib{strings,StringBase,inheritance diagram}
  at the top of the page, which shows how the classes relate to each other. The next sub-chapters
  discuss each class in detail.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_String
 ## 4.1 Class String ##

 The most basic class is \alib{strings,StringBase,String}. It just contains the pointer to the
 first character and the length of the string.
 Class \b %String does \e not allocate any data and with the exclamation of assigning something
 (completely) different, it does not allow to change the pointer to its buffer or its length.
 It could be said that an object of \b %String is "immutable". But instead, a better way to look
 at it is that the class is just not really a string, but it rather is pointer to a string!
 While the interface does not allow to change the contents of the string, of-course the pointer
 may be changed easily.<br>

 The good thing about considering class \b String as being rather a pointer is that this implies
 that the class is very lightweight. Look at the following code:

        String s= "Hello";
        s= "World";
        s= s.Substring( 2, 3 ); // assignment of value type -> overwrites the object
        s= nullptr;             // writes nullptr to buffer and 0 to length

 This lightweight, pointer-like nature of the class makes clear that the class is not comparable
 to class \c std::string at all. Much more, it is comparable to \c std::string_view, which was
 introduced only with C++ version 17.
 Due to the minimum footprint, move construction and assignment is compiler defined and very fast.
 Even passing objects as value has a very limited performance impact (with move semantics even no
 impact).

 Objects of type \b %String are implicitly constructed or assigned to from other string types like
 - C++ string literals
 - \b char* / \b wchar_t* pointers
 - \b std::string objects
 - custom, 3rd-party string types (Note: this is most important and explained below)

 The use cases of the class is obvious in two scenarios:

 1. Using class String for String Constants:<br>
    Often, some constant string data is needed. By having this field of type \b %String, just any
    string type can be assigned to it, especially string literals of-course.
    The advantage to type <c>const char*</c>, along with the pointer, the length of
    the string is stored. While with this, the buffer is not needed to be zero-terminated, a huge
    advantage is a good performance improvement, as for example no \c std::strlen()
    call has to be made if the length needs to be evaluated by the receiver of the string.

    Same as with a use of <c>const char*</c> (or any other pointer type), it has to be made
    sure that the underlying string buffer is kept valid during the life-time of a string that
    points to it. This is not in the field of responsibility of the string object.

 2. Using <c>const String&</c> as type of method parameters:<br>
    All \alib interface methods that need some string input use a constant reference to \c %String
    as parameter type (alternatively using derived type \c %TString, discussed later).
    The code that uses such methods often does not need to care about parameter type conversion.
    References or pointers to any sort of "string-like objects" might be just passed "as is".

    With C++ 11 move construction and assignment in place, and by not having a destructor, no virtual
    methods, etc., objects of type \c %String are perfect candidates to be  constructed implicitly
    for method parameter passing.<br>
    As a sample, see constructor of class
    \ref aworx::lib::system::Directory::Directory(const String&) "Directory". It is declared as:
    \code{.cpp}
    Directory( const aworx::String& path );
    \endcode
    With this declaration, it is possible to invoke that constructor passing using just any type of strings:
    \snippet "DOX_ALIB_TOAS.cpp"     DOX_ALIB_AS_DIRECTORY


 \anchor alib_strings_classes_String_custom
 ### 4.1.1 Binding User Types To %ALib %Strings: ###
 With a little one-time effort for specializing a simple template struct of two methods, also user
 defined string types (or types from other 3rd party libraries) can be made compatible easily. Due
 to the template meta programming, variations of these types, like pointers or references, const or
 non-const, etc. will be accepted with minimum (mostly no!) runtime impact.

 For details and a source code sample on how to make \alib support user defined types, see
 \alib{strings,T_String,documentation of partially specialized template function T_String}.

 \note
   The value of this feature should not be under-estimated. For a software that uses \alib that
   means that the software is "allowed" to continue using the type of string class that it
   ever used. No \alib \b %Strings objects start cluttering into the code. Instead, whenever
   \alib is invoked, an implicit, lightweight conversion is done.


 \anchor alib_strings_classes_String_nullable
 ### 4.1.2 Nullable %Strings ###
 Another important note is the fact that class \b %String are 'nullable'. This term means here, that
 when they got constructed with no value, with <em>nullptr</em>, with other \e nulled \alib strings,
 etc., this state is detectable and is different from representing an empty string of zero length.
 This is very important to create for lean and clean user code.

 Derived mutable class \b %AString (which is discussed below in more detail) inherits this ability
 See the sample exposed in \ref alib_namespace_strings_astring_nullable "Nullable AStrings"
 to get a better understanding of this concept.

 \anchor alib_strings_classes_String_term
 ### 4.1.3 No Zero-Termination ###
 %Strings represented by this class may be zero terminated. E.g. when constructed from a string literal
 they definitely are (as C++ string literals are zero terminated an not writable). However,
 class \b %String does not count on that and treats its content as not zero-terminated.

 For further information about class \b %String, consult
 \alib{strings,StringBase,reference documentation of class String}.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_TString
 ## 4.2 Class TString ##

 A first specialization of class
 \alib{strings,StringBase,String} is class
 \alib{strings,TStringBase,TString} which is representing strings that - other
 than the name might indicate - either
 - <b>are</b> terminated with a trailing '\0' character, or
 - <b>can be</b>  terminated by writing a trailing '\0' into the
   \ref aworx::lib::strings::StringBase::buffer "buffer" at position
   \ref aworx::lib::strings::StringBase::length "length".

 This is important to understand. For example, a C++ string literal like <c>"Hello"</c> is
 zero terminated. If it was not, it could not be terminated explicitly because write access is not
 allowed. But still it meets the requirements to get represented by a objects of \b %TString!

 The important design decision made here is that the construction of objects this class is still
 implicit. This is sometimes a little dangerous, but runtime checks are provided in debug compilations
 to detect illegal construction with data that does not meet the criteria (either is not terminated
 or can't be terminated).

 The interface of class \b %TString extends the set of methods provided by parent class \b %String,
 especially by method \alib{strings::TStringBase,ToCString}. This method terminates the buffer (if
 not terminated, yet) and returns the pointer to it. Such zero-terminated strings are often
 needed with 3rd party library functions, including operating system calls.

 For details, refer to the \alib{strings,TStringBase,reference documentation of class TString}.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_AString
 ## 4.3 Class AString ##

 Class \alib{strings,AStringBase,AString}, which specializes class \alib{strings,TStringBase,TString},
 disposes of an allocation mechanism to allow the creation and manipulation of character strings.
 Therefore, this is the central class to assemble strings at runtime.
 Whenever append operations exceed the existing buffer, the contents  is copied to a new, bigger
 buffer and the old one is disposed.

 Conceptually, it could be said that the difference to parent classes \b %String and \b %TString
 is that those <em>"represent"</em> character strings, while this class <em>"implements"</em> a string.

 \anchor alib_strings_classes_AString_constr
 ### 4.3.1 Construction ###
 Class \b %AString hides all parent constructors and offers re-implementations that rather
 <b>copy</b> the data that is passed. Consequently - as this copying is not considered a lightweight
 operation - all constructors are explicit. If, for example, a function expects a constant
 \b %AString reference, such \b %AString has to be created explicitly before or inside the function
 invocation. On the same token, the assignment operator is not applicable with initializations as well.<br>
 The following code will not compile:
 \verbatim
 AString as=  "This will not compile";
 \endverbatim
  * Instead, explicit creation has to be made, as shown here:
 \verbatim
 AString as("This will compile");

 // or alternatively
 AString as;
 as= "This will compile";
 \endverbatim


 With templated constructor
 \alib{strings,AStringBase::AStringBase(const TApplicable&),AString::AString(const TApplicable&)},
 class \b %AString accepts any type of object that a conversion function exists for. This makes
 construction very flexible.
 In the terminology of \alib, the conversion of custom types into string representations is called
 <em>"to apply an object to an AString"</em>. What this precisely means, is explained below.

 Copy constructor, move constructor and move assignment are well defined, which allows \b %AString
 objects to be used (as efficiently as possible) as value types in containers of the standard library,
 for example as in <c>std::vector\<AString\></c>.

 \anchor alib_strings_classes_AString_bufmngmnt
 ### 4.3.2 Buffer Management ###
 As mentioned before, class \b %AString provides logic to manage its own buffer.
 Furthermore, it is also possible to have objects of class \b %AString work on 'external' buffers.
 This means that the life-cycle of an \b %AString's buffer not necessarily is bound to the life-cycle
 of the \b %AString object itself. For details on using external buffers, see method
 \alib{strings,AStringBase::SetBuffer(TChar*\,integer\,integer\,lang::Responsibility),AString::SetBuffer}.
 Class \alib{strings,PreallocatedStringBase,PreallocatedString}, which is discussed below, makes use
 of this feature and provides the possibility to have local (stack based) allocations of strings.


 \anchor alib_strings_classes_AString_apply
 ### 4.3.3 Appending and Applying Objects ###
 Class \b %AString uses some template meta programming method to be able to create a string
 representation from any object of arbitrary type. In \alib terminology, this is called "to apply
 an object to an AString". Such "application" can be performed by either calling method
 - \alib{strings,AStringBase::Apply,AString::Apply},
 - alias method \alib{strings,AStringBase::_,AString::_} (yes, the method name is simply <c>"_"</c>), or using
 -  \alib{strings,AStringBase::operator<<,AString::operator<<},

 which are all exactly the same. Furthermore, an "applicable" object may also be passed to the
 constructor of an \b %AString, which applies the object right during construction.


 For many types, e.g. string types or integer and floating point types, the term '<em>applying</em>'
 is means nothing else than '<em>appending</em>' a string representation of the object to the current
 end of the string.
 However, as almost any type, including 'external', user defined types
 might be applied and such application is performed in a specialized template method, the operation
 associated with the <em>application</em> of an object of a certain type is not limited to appending data.

 As an example, the concept of \e 'applying' is used to add 'fields' to an \b AString.
 To achieve this, a special \e 'applicable' type named
 \alib{strings,FormatBase::Field,Format::Field} is defined. Appending a field
 is done as follows:
 \snippet "DOX_ALIB_APPLYTO.cpp"     DOX_ALIB_APPLYTO_FIELD
 which produces the following output:
 \snippet "DOX_ALIB_APPLYTO_FIELD.txt"     OUTPUT


 For information on how to make a custom type an "applicable" type, see documentation of method
 \alib{strings,AStringBase::Apply,AString::Apply} and of partially specialized template struct
 \alib{strings,T_Apply}.

 The concept of having "applicable types" is a little comparable to the concept of having a
 common base class that exposes a virtual function called <b>ToString</b> as found in some programming
 languages. However, the concept introduced with \alib is more flexible and convenient. This is
 especially true because:
 - Any type can be made applicable without extending the type's definition: No "ToString" or similar method
   has to be added to the type. Instead, all necessary code is defined outside of the type definition.
   Even 3rd-party library types can be defined to be "applicable" without having access to the
   library's source code.
 - Besides appending string representations, some completely different meanings can be given to
   what "apply" means for a certain custom type.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_PAString
 ## 4.4 Class PreallocatedString ##

 Template class \alib{strings,PreallocatedStringBase,PreallocatedString<int TCapacity>}, derived from class \b %AString
 uses an internal member of type <em>char[TCapacity]</em> of a length specified by a template parameter,
 to store the string data.
 The huge benefit lies in performance: The impact of frequent <em>heap allocations</em> is often
 underestimated by software developers. Therefore, for local string operations with foreseeable
 maximum string buffer sizes, class \b %PreallocatedString should be considered as a fast
 alternative of class \b %AString.


 \anchor alib_strings_classes_PAString_bo
 ### 4.4.1 Exceeding the Buffer Capacity ###
 Although the internal buffer size is fixed at compile time and hence can not be expanded, a user
 of the class must not fear an assertion from '<em>buffer overflows</em>'. If the internal buffer capacity
 is exceeded, a regular allocated buffer from the <em>free memory</em> (aka '\e heap') will be
 allocated. The slight disadvantage then is, that the memory of the internal buffer is (of-course)
 not freed and thus remains unused.

 In debug compilations of \alib, parent class
 \alib{strings,AStringBase,AString} provides a warning mechanism that enables the easy
 detection of such probably unwanted replacements of external buffers
 (in this case the buffer given by the character array member).
 Using macro \ref ALIB_WARN_ONCE_PER_INSTANCE_DECL an <em>ALib Warning</em> will be issued when a buffer
 is replaced. There are two scenarios how this mechanism might be used during development:
 - If the buffer should never be replaced, the capacity of an %PreallocatedString has to be increased
   step by step (during the software development/testing cycle) in the moment the warning is issued.
   This has to be done, until the member-buffer is huge enough and no more warning is raised..
 - If it is OK that the buffer is replaced "every once in a while" (because there special
   situations with higher capacity needs may occur, but are "relatively" seldom) then, the warning
   should be switched off for this specific object. By switching the warning off, a developer
   places the information in the code that the internal buffer size might be too small in some occasions.
   Having this explicit information, helps to understand the intentions of the software developer.

 If the latter case applies, then the one-time warning can be disabled using macro
 ALIB_WARN_ONCE_PER_INSTANCE_DISABLE(myPreallocatedString, ReplaceExternalBuffer)

 Note that the warning, the macro that disables it and all related efforts and code are effective
 only with debug compilations of module \alibmod_nolink_strings.

 \anchor alib_strings_classes_PAString_constr
 ### 4.4.2 Implicit construction ###
 While class \b %AString, as
 \ref alib_strings_classes_AString_constr "noted above", does not provide
 implicit construction, this class re-implements
 the common constructors of \b %AString and exposes them as <em>implicit</em>.
 The rationale here is that although the data is copied (which might not be a very lightweight task),
 still the performance impact is far less compared to constructing an \b %AString that uses a
 heap-allocated buffer. So, the design decision is that it is taken into account that an
 \b %PreallocatedString is created without explicit exposure of this creation.

 The following method, as a sample, takes three different \alib string types as parameters:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_AS_PARAMETERS_1
 The following code will \e not compile:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_AS_PARAMETERS_2
 Class AString has to be explicitly created, the others don't:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_AS_PARAMETERS_3

 In addition, besides having implicit construction, the default assign operator is defined as well.
 This allows to use objects of this type as class members that are initialized within the class
 declaration as shown here:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_ASPREALLOC_MEMBER
 Such members are not allowed to be initialized in the declaration if their type is \b %AString.

 \anchor alib_strings_classes_PAString_move
 ### 4.4.3 No Move Constructor ###
 Class \b %PreallocatedString provides no <em>move constructor</em> and thus is very inefficient
 in scenarios where objects of the class could rather be moved than copied. Consequently such
 situations are to be avoided. The use of \b %PreallocatedString should instead be
 very determined and it should not be subject to copy and move operations.

 \anchor alib_strings_classes_PAString_aliases
 ### 4.4.4 Aliases For Frequently Used Sizes ###
 Within namespace \ref aworx, some convenient alias type definitions are available that define
 preallocated strings of frequently uses sizes:
 - #String16, #String32 ... #String1K, #String2K, #String4K,
 - #NString16, #NString32 ... #NString1K, #NString2K, #NString4K, and
 - #WString16, #WString32 ... #WString1K, #WString2K, #WString4K.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_Substring
 ## 4.5 Class Substring ##

 Class \alib{strings,SubstringBase,Substring} is derived directly from class \b %String.
 Conceptually, the difference to its parent class is that it is not constant. It was discussed
 earlier, that objects of type \b %String, once constructed, will never change the pointer to the buffer
 or its length. Objects of class \b %Substring are entitled to do so. In general, the string they
 represent is a portion of another string and if they change, this change normally means
 shrinking the string to a substring of itself.

 \note
   Speaking of changes, this must not be confused with changes of the contents of the string
   data. What is meant here are changes of the two fields
   \alib{strings,StringBase::buffer,buffer} and \alib{strings,StringBase::length,length}

 Class \b %Substring does not extend parent class \b %String with new field members. All
 methods of the class could therefore technically be implemented in \b %String already. The
 reason why all methods that change the \b %buffer and/or the \b %length of the string have been
 aggregated in this derived type is, that these methods must not be inherited by class \b %AString,
 which manages its own string buffer and must not loose information about the start of the
 allocated memory.

 \b %Objects of type %Substring are especially useful to perform simple parsing of string data.
 For this reason, a big portion of its interface methods are named "ConsumeXYZ". These methods
 read something from the substring, cut it off the string and return the result value that was
 "consumed".


 \~Comment ####################################################################################### \~
 \anchor alib_strings_classes_StringLiteral
 ## 4.6 Class StringLiteral ##

 Template class \alib{strings,StringLiteralBase,StringLiteral}, which is derived from class \b %TString,
 represents strings whose length is fixed and known at compile time.
 Most objects of this type are constructed from C++ string literals, like in the following samples:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_LITERAL_1

 But it is also allowed to construct them from other character arrays, even writable ones, like here:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_LITERAL_2

 As the class specializes \alib{strings,TStringBase,TString}, the user of the class has to make sure
 that the string represented by objects of this type is zero terminated.

 Using class \b %StringLiteral improves the performance of certain operations. For example appending
 (applying) objects of this type to objects of class \b %AString will not result in a copy loop but
 will simply inline the correct number of copy operations into the object code (at least for the
 smaller sizes which are typically used).

 Similar to TMP struct \alib{strings,T_String} used to enable the construction of \b %String object
 from string-like types, TMP struct \alib{strings,T_StringLiteral} is used to construct objects
 of class \b %StringLiteral. Consequently, it is possible to enable 'external', user defined types
 to seamlessly act as string literals as well.

 This is a rather unlikely scenario, but if requested (solely for a potentially small performance gain),
 these types can get adopted to \alib by specializing \b %T_StringLiteral.
 This process is not further documented or demonstrated by a sample. Interested users should consult
 the source code of \alib for doing it.<br>
 The only built-in specialization of the TMP struct provided with \alib, is for C++ string literals,
 respectively for character arrays of a known, fixed length.


 \~Comment ####################################################################################### \~
 \anchor alib_strings_details
 # 5. Details On %Strings #

  \~Comment ####################################################################################### \~
 \anchor alib_strings_details_nonchecking
 ## 5.1 "Non-Checking" Methods Of %ALib String Classes ##

 Several of the methods found in the different string classes of \alib are templated with
 a boolean template parameter named \p{TCheck}. This template parameter is defaulted with
 the value \c true which is sort of 'hiding' it in normal code. Consider the following
 snippet:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_NONCHECKING_1

 The implementation of method
 \alib{strings,SubstringBase::ConsumeChars,Substring::ConsumeChars} by default checks if
 object \e line is big enough to cut the given number of characters from the front.
 Obviously this check is redundant here, because \b ConsumeChars is invoked only if \b IndexOf had
 found the string <em>"<start>"</em>. To avoid the redundant check, we invoke the non-checking version of
 method \b ConsumeChars by providing template parameter \p{TCheck} with value \c false:
 \snippet "DOX_ALIB_STRINGS.cpp"     DOX_ALIB_STRINGS_NONCHECKING_2

 \note
   In this simple sample, especially as these methods are inlined, the compiler might detect
   already that the check is redundant and would remove it from the code. However, there
   are many occasions where the compiler is lacking the information and the possibility to detect
   such redundancy.

 In the C# and Java versions of \alib, where such template methods are not available,
 still some non-checking methods are provided, but less than in the C++ implementation. In these
 languages, these methods exist twice with the same name, the non-checking one with
 suffix <em>_NC</em>.

 The following general rules apply for all non-checking methods:
 - String parameters are not checked for being nullptr.
 - Index, size, length, region start/end and other parameters are not checked to be correct
 - If parameters are incorrect, the result of the methods is undefined
   and an exception/assertion may be raised.
 - Append methods that in their original version would set a \e nulled \b %AString to non \e nulled
   state in the case that an empty string or string region is appended, are not confirming
   to this  principle in their non-checking version.<br>
   In other words, \e nulled strings keep being \e nulled if empty strings are appended.
 - In the Java and C# versions of \alib, the hash value of an \b %AString object keeps
   being cached when invoking an <em>_NC</em> method. This may lead to wrong behavior, e.g. when
   an \b %AString object is used as a key of a hash table. To avoid errors, within a context that
   makes use of an \b %AStrings' hash value, before each hash code retrieval
   (C#: <em>GetHashCode()</em>, Java: <em>hashCode()</em>),
   it has to be certified that at least one of the methods manipulating the object has
   to be a non <em>_NC</em> method.

 Apart from the previous points, the behavior and result of invoking a non-checking version
 of a method is the same as of invoking the original version. The only difference is in a higher
 execution performance.

 In debug compilations of module \alibmod_nolink_strings, the non-checking versions of the methods
 perform debug-assertions about the checks that are left out! This means, that it is very safe
 to use non-checking versions, as during development non-checking invocations in situations where
 checks are needed become detected by these assertions.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_details_format
 ## 5.2 Formatting %Strings ##
 Module \alibmod_nolink_strings provides powerful formatting facilities for string data stored in
 \b %AString objects through abstract class \alib{strings::format,FormatterBase,Formatter} and its
 specializations \alib{strings::format,FormatterPythonStyleBase,FormatterPythonStyle} and
 \alib{strings::format,FormatterJavaStyleBase,FormatterJavaStyle}.

 Please consult the reference documentation of sub-namespace \ref aworx::lib::strings::format
 for details on string formatting features.

 \note
   The formatting features are only available if the module is used together with
   module \alibmod_boxing. The rational for this is that the formatting methods accept
   arbitrary typed objects using the C++ elipsis keyword <c>"..."</c>. Nevertheless, the goal was
   to have 100% type safeness when processing the objects passed. Such type safeness is the core
   feature of \alibmod_boxing.

 \~Comment ####################################################################################### \~
 \anchor alib_strings_details_compat
 ## 5.3 Compatibility with External Libraries ##

 \alib provides compatibility extensions with different class libraries. Currently the following
 extensions are provided:
 ### Standard C++ Library ###
 - Trough header file <em>"alib/compatibility/std_string.hpp"</em>
   - Implements template struct
     \ref aworx::lib::strings::T_String "T_String" to allow (implicit) conversion of objects of type
     \c std::string to type \b %aworx::NString and
     \c std::wstring to type \b %aworx::WString
   - If languages standard C++ 17 is in place, implements template struct
     \ref aworx::lib::strings::T_String "T_String" to allow (implicit) conversion of objects of type
     \c std::string_view to type \b %aworx::NString and
     \c std::wstring_view to type \b %aworx::WString
   - Implements template struct
     \alib{strings,T_Apply} to allow the concatenation of objects of type
     \e std::string to type aworx::AString.
   - Implements namespace function
     \ref aworx::lib::strings::thirdparty::std::ToStdString "ToStdString" (with various
     overloads) to convert an object of type aworx::String to type \e std::string.

 <p>
 - Trough header file <em>"alib/compatibility/std_iostream.hpp"</em>
   - Provides \e applicable class \alib{strings::thirdparty::std,ISReadLineBase} and corresponding
     specialization of \alib{strings,T_Apply} to allow reading lines of text from
     a \e std::istream into an object of type aworx::AString.
   - Provides namespace functions
     \alib{strings::thirdparty::std,operator>>(std::istream&, AString&)} and
     \alib{strings::thirdparty::std,operator>>(std::istream*, AString&)}
     which are using aforementioned applicable class \b ISReadLine with default parameters
     to read a line from a \e std::istream into an object of type aworx::AString.
   - Provides namespace functions
     \alib{strings::thirdparty::std,operator<<(std::ostream& os, const String& as)} and
     \alib{strings::thirdparty::std,operator<<(std::ostream* os, const String& as)}
     to write an object of type aworx::String into a \e std::ostream.

 ### QT Library ###
 - %String classes of the [QT library](https://www.qt.io) are
   supported trough header file <em>"alib/compatibility/qt.hpp"</em>.

   General information about the use of QT strings with \alox is given in documentation of
   sub-namespace   \ref aworx::lib::strings::thirdparty::qt "thirdparty::qt".


 ### Other Libraries ###
 \note Support for other Libraries to come... Feel free to send us your work!


 \~Comment ####################################################################################### \~
 \anchor alib_strings_details_debug
 ## 5.4 Debugging %ALib %String Classes ##

 In some situations some additional debug checking is helpful when working with \alib strings.
 Among such situations are:
 - development of types derived from \alib string types
 - specializing template structs
   \ref aworx::lib::strings::T_String "T_String" or
   \alib{strings,T_Apply} to add support for user defined string types or
   formatting operations to class \b %AString.
 - external manipulation of \b %AString string data
 - provision of external data buffers to class \b %AString

 In these and similar situations, it is advised to using the compiler symbol
 \ref ALIB_DEBUG_STRINGS_ON. This enables internal consistency checks with almost any method \alib
 string invoked. By default this is disabled, as it generates a quite huge drop of runtime
 performance. When string debugging is enabled, macro ALIB_STRING_DBG_CHK can be used to check
 the consistency of \alib string classes. Furthermore, macro #ALIB_STRING_DBG_UNTERMINATE <em>has
 to</em> be used when changing the length of a terminatable string without using method
 \alib{strings,AStringBase::SetLength,AString::SetLength<false>} (e.g. in custom, derived
 classes).

 With compilation symbol \ref ALIB_DEBUG_STRINGS set to true, the string buffer allocated
 by class \b %AString is extended by 32 bytes, 16 bytes at the front and 16 bytes at the end.
 Therefore, code that:
 1. uses method \alib{strings,AStringBase::SetBuffer,AString::SetBuffer} to set an external buffer, and
 2. transfers responsibility to \alib by setting parameter \p{responsibility} of that method
    accordingly, and where
 3. this external buffer gets replaced later when the string's size increases,

 then the buffer passed has to to be allocated accordingly. This means, the buffer has to be
 32 bytes larger than specified and the starting address of the heap allocation has to be
 16 bytes before what parameter \p{extBuffer} points to.<br>


 \~Comment ####################################################################################### \~
 # Reference Documentation #

***************************************************************************************************/

namespace strings {

ALIB_WARNINGS_RESTORE

void Strings::init( Phases phase )
{
    if( phase == Phases::resourceset )
    {
        #if ALIB_MODULE_BOXING
            // set boxer built-in interface implementations
            aworx::lib::boxing::DefineInterface<char              , true  , boxing::IIsLess_Tchar_arr    >();

            // set boxer interfaces of type IApply
            aworx::lib::boxing::DefineDefaultInterface<IApply<char   >>();
            aworx::lib::boxing::DefineDefaultInterface<IApply<wchar_t>>();

//! [DOX_ALIB_BOXING_SAMPLE_TEMPLATED_INTERFACE]
    aworx::lib::boxing::DefineInterface<bool       , false, boxing::IApply_TApplicable<nchar, bool       >>();
    aworx::lib::boxing::DefineInterface<bool       , false, boxing::IApply_TApplicable<wchar, bool       >>();
    aworx::lib::boxing::DefineInterface<nchar      , false, boxing::IApply_TApplicable<nchar, nchar      >>();
    aworx::lib::boxing::DefineInterface<nchar      , false, boxing::IApply_TApplicable<wchar, nchar      >>();
    aworx::lib::boxing::DefineInterface<wchar      , false, boxing::IApply_TApplicable<nchar, wchar      >>();
    aworx::lib::boxing::DefineInterface<wchar      , false, boxing::IApply_TApplicable<wchar, wchar      >>();
    aworx::lib::boxing::DefineInterface<strangeChar, false, boxing::IApply_TApplicable<nchar, strangeChar>>();
    aworx::lib::boxing::DefineInterface<strangeChar, false, boxing::IApply_TApplicable<wchar, strangeChar>>();
    aworx::lib::boxing::DefineInterface<boxed_int  , false, boxing::IApply_TApplicable<nchar, boxed_int  >>();
    aworx::lib::boxing::DefineInterface<boxed_int  , false, boxing::IApply_TApplicable<wchar, boxed_int  >>();
    aworx::lib::boxing::DefineInterface<boxed_uint , false, boxing::IApply_TApplicable<nchar, boxed_uint >>();
    aworx::lib::boxing::DefineInterface<boxed_uint , false, boxing::IApply_TApplicable<wchar, boxed_uint >>();
    aworx::lib::boxing::DefineInterface<double     , false, boxing::IApply_TApplicable<nchar, double     >>();
    aworx::lib::boxing::DefineInterface<double     , false, boxing::IApply_TApplicable<wchar, double     >>();
//! [DOX_ALIB_BOXING_SAMPLE_TEMPLATED_INTERFACE]

            aworx::lib::boxing::DefineInterface<nchar             , true  , boxing::IApply_TncharArr<nchar>>();
            aworx::lib::boxing::DefineInterface<nchar             , true  , boxing::IApply_TncharArr<wchar>>();
            aworx::lib::boxing::DefineInterface<wchar             , true  , boxing::IApply_TwcharArr<nchar>>();
            aworx::lib::boxing::DefineInterface<wchar             , true  , boxing::IApply_TwcharArr<wchar>>();
            aworx::lib::boxing::DefineInterface<strangeChar       , true  , boxing::IApply_TstrangeCharArr<nchar>>();
            aworx::lib::boxing::DefineInterface<strangeChar       , true  , boxing::IApply_TstrangeCharArr<wchar>>();
            aworx::lib::boxing::DefineInterface<BoxedAs<NAString> , false , boxing::IApply_BoxedAs_TApplicable<nchar, NAString>>();
            aworx::lib::boxing::DefineInterface<BoxedAs<WAString> , false , boxing::IApply_BoxedAs_TApplicable<wchar, WAString>>();

            ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE( format::Exceptions )


        // add exception of formatter (only when boxing, because without boxing no formatters)
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3]
Res->AddBulk( ResourceCategory.ToCString(),

    ASTR("FmtExceptionsPrefix")  ,      ASTR("format::") ,
    ASTR("FmtExceptionsPostfix") ,      ASTR("")         ,

    ASTR("FmtExceptions"),

        // general formatter errors
         ASTR("11,ArgumentIndexIs0"                      ",FMG11,"
              "12,ArgumentIndexOutOfBounds"              ",FMG12,"
              "13,IncompatibleTypeCode"                  ",FMG13,"

             // Formatter Python Style
             "101,MissingClosingBracket"                 ",FPS101,"
             "102,MissingPrecisionValuePS"               ",FPS102,"
             "103,DuplicateTypeCode"                     ",FPS103,"
             "104,UnknownTypeCode"                       ",FPS104,"
             "105,ExclamationMarkExpected"               ",FPS105,"
             "106,UnknownConversionPS"                   ",FPS106,"
             "107,PrecisionSpecificationWithInteger"     ",FPS107,"
             "108,MissingReplacementStrings"             ",FPS108,"

             // Formatter Java Style
             "201,NegativeValuesInBracketsNotSupported"  ",FJS101,"
             "202,MissingPrecisionValueJS"               ",FJS102,"
             "203,HexadecimalFloatFormatNotSupported"    ",FJS103,"
             "204,NoAlternateFormOfConversion"           ",FJS104,"
             "205,NoPrecisionWithConversion"             ",FJS105,"
             "206,UnknownDateTimeConversionSuffix"       ",FJS106,"
             "207,UnknownConversionJS"                   ",FJS107,"

             // PropertyFormatter and PropertyFormatters
             "501,UnknownPropertyInFormatString"         ",PFM501,"
             "502,ErrorInResultingFormatString"          ",PFM502,"
             "510,MissingConfigurationVariable"          ",PFM510,"

             // SimpleText
             "601,UnknownMarker"                         ",ST601,"
             "602,EndmarkerWithoutStart"                 ",ST602"     ),

    // general formatter errors
    ASTR("FMG11"), ASTR("Argument index 0 not allowed."
                        "\\nIn: {!Q}\\n     {!Fill}^"                                         ),
    ASTR("FMG12"), ASTR("Argument index #{} is out of bounds. {} arguments are available."
                        "\\nIn: {!Q}\\n     {!Fill}^"                                         ),
    ASTR("FMG13"), ASTR("Incompatible type code '{}' for argument #{} which is of type {!Q}."
                        "\\nIn: {!Q}\\n     {!Fill}^"                                         ),

    // Formatter Python Style
    ASTR("FPS101"), ASTR("Closing bracket '}}' of placeholder not found (or syntax error)."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                        ),
    ASTR("FPS102"), ASTR("Missing precision value after '.' character."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS103"), ASTR("Duplicate type code '{}' given (previous was '{}')."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS104"), ASTR("Unknownw type code '{}' given."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS105"), ASTR("Excpected '!' in continuation of placeholder."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS106"), ASTR("Unknown conversion \"!{}\"."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS107"), ASTR("Precision not allowed with integer format."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FPS107"), ASTR("Missing pair of replacement strings \")<src><repl>\" after \"!Replace\"."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),


    // Formatter Java Style
    ASTR("FJS101"), ASTR("Brackets for negative values not implemented/supported."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS102"), ASTR("Missing precision value after '.' character."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS103"), ASTR("Hexadecimal float format not implemented/supported."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS104"), ASTR("Alternate form '#' not supported with conversion {!Q}."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS105"), ASTR("Precision specification (\".{}\") is not supported with conversion {!Q}."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS106"), ASTR("Unknown date/time conversion suffix '{}'."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),
    ASTR("FJS107"), ASTR("Unknown conversion character '{}'."
                         "\\nIn: {!Q}\\n     {!Fill}^"                                                  ),


        // PropertyFormatter and PropertyFormatters
    ASTR("PFM501"), ASTR("Unknown property identifier \"{}{}\" in format string.\\n"
                         "Format string: {!Q}.\\n"
                         "Allowed identifiers: "                                                                ),
    ASTR("PFM502"), ASTR("Error in resulting format string.\\nSource format: {!Q}.")                             ,
    ASTR("PFM510"), ASTR("Missing or empty configuration variable {!Q} containing user defined format string." ),

        // SimpleText
    ASTR("ST601"),  ASTR("Unknown marker at position {}.\\n"
                         "  Marked text: {!Q}.\\n"
                         "                {!Fill}^\\n"                                                          ),
    ASTR("ST602"),  ASTR("Found an end-marker without a start marker at position {}.\\n"
                         "  Marked text: {!Q}.\\n"
                         "                {!Fill}^\\n"                                                          ),


    ASTR("APPLY_T"), ASTR("true"),
    ASTR("APPLY_F"), ASTR("false"),

// end of AddBulk()
nullptr );
//! [DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3]

        #endif // ALIB_MODULE_BOXING

        #if ALIB_MODULE_BOXING
            defaultFormatter.reset( new FormatterPythonStyle() );
        #endif

    }

}

void Strings::terminationCleanUp()
{
    #if ALIB_MODULE_BOXING
        ALIB_ASSERT_WARNING( defaultFormatter.use_count() == 1,
                             ASTR("defaultFormatter still saved somewhere else.") );
        defaultFormatter.reset();
    #endif
}

#if ALIB_MODULE_BOXING
void    Strings::ReplaceDefaultFormatter( Formatter* newFormatter )
{
    defaultFormatter.reset( newFormatter );
}
#endif


//! @endcond
}}}// namespace [aworx::lib::strings]

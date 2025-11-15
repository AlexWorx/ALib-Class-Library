// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_strings    ALib Module Strings - Programmer's Manual

\tableofcontents

\I{################################################################################################}
# 1. Introduction # {#alib_strings_intro}

C++ developers do not need to get motivated much about using a 3rd-party string library. This is
because the language itself does not offer powerful built-in types.
Both C++ standard library types \c std::string and \c std::string_view are not designed to
really process string data. For example, only with C++20 method \c starts_with() and \c ends_with()
were added, and only C++23 brings method \c contains().
It seems like the standard committee does not want to empower these types and this is an acceptable
design decision, which is perfectly understandable.

The situation is even a little worse, because in C++:
- There is more than one character type defined.
- The language keywords for narrow and wide characters do not specify a distinct size but are
  platform- and compiler-dependent.
- C++ string literals are zero-terminated arrays (which for
  \https{good reasons,queue.acm.org/detail.cfm?id=2010365} can be considered to be a legacy design
  mistake).

Because of this, every general purpose C++ library tends to invent it's own character string type
and while \alib is no exception, this constitutes a problem in itself.
It is a true dilemma: C++ developers need to rely on some external string library, but each new
string library increases the problem of adding complexity to this very basic and fundamental
domain.

Yes, a C++ developer lives in a string hell! And therefore, the main design goal of module
\alib_strings_nl is to mitigate these problems.

\I{################################################################################################}
## 1.1 Library Design Goals ## {#alib_strings_intro_goals}

The design goals of module \alib_strings_nl are:

<b>1. Mitigate the "C++ string problem":</b>
\par
  The most important design goal behind module \alib_strings_nl, is to have the string
  classes introduced be as <b>open and interchangeable</b> as possible with just any other
  string type, may this be legacy zero-terminated arrays, the standard library types or just any
  3rd-party typed that "smells like a string".<br>
  Instead of aiming to offer a next prominent string type to C++ developers, \alib strings
  should try and "hide" themselves. For example, if interface functions of an API accepted string
  objects as arguments and likewise API functions returned string data, then in both directions
  arbitrary types of string objects should be able to be passed - preferably without the need of
  explicit conversion code.<br>
  This goal is in perfect alignment with the general design principle of the \alib library to be
  "least intrusive".<br>

<b>2. Mitigate the "C++ character width problem":</b>
\par
   The width of characters used with \alib strings should be transparent. Only within special code
   sections, character widths should be needed to be explicitly chosen. As one result, user
   code should be free of preprocessor directives (code selection) while still compiling on any platform
   that defaults to any character width.

<b>3. Abandon the use of zero-terminated strings:</b>
\par
   Zero-terminated \e "c-strings" are often insecure and inefficient.
   Their use is to be reduced to the required minimum, for example, in the context of system API calls.
   At the same time, their usage should be transparent, safe and convenient.

<b>4. Use of unicode and UTF-encoding</b>
\par
   All string data should be in unicode using UTF-8, UTF-16 or 32-bit encoding.
   Conversion of strings to a different width should be transparent and rather implicit
   than explicit.

<b>5. Low and high level string features</b>
\par
   Of course, \alib strings should be "complete" in the sense that they offer all features
   that a modern character string API usually offers.
   This final goal was reached with the evolution of the library. Furthermore, in addition to what is
   provided here, other modules of \aliblong even extend such functionality, especially
   module \alib_format.


\I{################################################################################################}
## 1.2 Module ALib Characters ## {#alib_strings_intro_characters}

The primary goals listed in the previous section are reachable best with the use of
the design pattern which is quickly introduced in chapter \ref alib_manual_appendix_tca.

In earlier versions of this library, type-traits needed to implement powerful string classes had
been introduced along with the \alib string types. However, it turned out that
there are very good reasons to "generalize" and extract the type-traits into a separate module which
is completely independent of string processing.

Instead of looking at character strings, the traits are rather about <em>"character arrays"</em>.
The difference lies in the angle of perspective: With \alib, character strings are a higher-level
concept than character arrays.
Strings may be constructed from character arrays, may export their data as character arrays and
interpret or manipulate the array data. Hence, the arrays are seen as the foundational data structure
used by strings.

With this conceptual distinction, it became possible to separate the definition of type-traits
to separate module \alib_characters. While this module \alib_strings_nl builds on
\alib_characters_nl, there is no dependency in the other direction: module \alib_characters_nl
does not "know" about module \alib_strings_nl.

For a thorough understanding of all aspects, reading the
\ref alib_mod_characters "Programmer's Manual Of ALib Module Characters" before the manual you
are currently reading, is of course helpful. But for a normal, straight forward use of the string
classes, this is not needed.
Therefore, the advice for the reader is to continue reading this manual about strings, and only
start investigating into module \alib_characters, when noted in later chapters.

This brief summary of what module \alib_characters_nl offers may suffice for the time being:
1. Character Types:
   - As the width of wide character type \c wchar_t is compiler dependent, type \alib{characters;wchar}
     is introduced.
     This may be equivalent to \c wchar_t but may also be one of \c char16_t or \c char32_t.
     With type \b wchar, the responsibility of what a wide character is removed from the compiler
     and given to \alib (and its platform defaults and compilation options).
   - The "other" wide character type, which has different width than \c wchar_t is aliased
     by type \alib{characters;xchar}.
   - Finally, just for completion, an alias type for \c char is given with \alib{characters;nchar}.
   - Together, this makes three new character types that denote the width:
     \b nchar, \b wchar and \b xchar.

2. Logical Types:
   - Usually a programmer should not be bothered with choosing character width. Therefore,
     type \alib{characters;character} is given as the first "logical" character type.
     This one is the most important and frequently used.<br>
     Type \b character either corresponds to type \alib{characters;nchar} or \alib{characters;wchar},
     again depending on platform and \alib compilation options.
   - To explicitly address the non-standard character type, type \alib{characters;complementChar}
     is given. This type is equivalent to type \b nchar if type \b character is equivalent to
     type \b wchar - and vice versa.
   - To finalize the set of \alib character types, a logical type name for the "strange", non-standard
     wide type is given with \alib{characters;strangeChar}. This is of 2-byte size when type \b wchar
     is of 4-byte size and vice versa.
   - Together, this makes three logical character types that do not provide any information about
     the actual width: \b character, \b complementChar and \b strangeChar.<br>
     Similar to the fact that explicit type \b nchar is always equivalent to built-in type \c char,
     the logical type \b strangeChar is always equivalent to explicit type \b xchar.

3. Character Array Type-Traits:<br>
   The traits, if given for a custom type \b T, answer the following questions:
   - Does type \b T contain, implement or otherwise represent character array data and
     does it provide access to that data?
   - Should such access granted implicitly, only explicitly or even only with mutable objects of
     type \b T?
   - May values of type \b T be constructed from character arrays?
   - Is such construction implicitly allowed or only when explicitly expressed?

   In addition, similar traits are available that answer the very same questions in respect to
   <b>zero-terminated</b> character arrays.


\I{################################################################################################}
## 1.3 UTF Encoding ## {#alib_strings_intro_utf}
The use of UTF encoding was named a "design goal" in section \ref alib_strings_intro_goals.
In fact, this is much more: It is a mandatory constrain that the software process that invokes
code of this \alibmod, uses UTF in general. The module has to rely on that fact, because unfortunately
most of today's operating systems and system class libraries (that \alib builds on), use a
global (process wide) approach with setting configuration parameters of provided character conversion
functions like \c wcsnrtombs or \c mbsnrtowcs.
On GNU/Linux, such settings are made with function \c setlocale.

\attention
  Therefore, it has to be ensured that the process's locale settings are "UTF-8" compatible.
  If not, \alib may not function properly. With debug-builds, runtime warnings or assertions
  might be raised. In release compilations, mixing strings of different character width might lead
  to undefined behavior.

This should not be seen to be a huge restriction, because there are no good reasons for any modern
software to use any other character encoding than UTF.
However, environment variables (or \alib variable \alib{camp::Variables;LOCALE} in a configuration
source) has to be set to a UTF-8 encoding.


\I{################################################################################################}
# 2. ALib String Classes # {#alib_strings_classes}

This module provides five different string classes:
- \b String
- \b CString
- \b AString
- \b Substring
- \b LocalString<TCapacity>

A string object's underlying character type is defined using a template parameter named \p{TChar}.
The different string classes are located in namespace #alib::strings and their type names
include a prefix letter <b>'T'</b>.
As a result, the list of base classes is:

- \ref alib::strings::TString<TChar>
- \ref alib::strings::TCString<TChar>
- \ref alib::strings::TAString<TChar>
- \ref alib::strings::TSubstring<TChar>
- \ref alib::strings::TLocalString<TChar,TCapacity>

As it is described in the documentation of outer namespace #alib, it is common practice for
any \alibmod to define "alias types" of all important classes in that namespace. For each of the string
classes, \b four alias types are defined which are using character types
\alib{characters;character}, \alib{characters;nchar}, \alib{characters;wchar} and \alib{characters;xchar}.
With the latter three \ref alib_characters_chars_by_width "explicit character types", the alias
names replace the prefix letter <b>'T'</b> by letters <b>'N'</b>, <b>'W'</b> and <b>'X'</b>.

As a result, the following table lists all alias names in namespace #alib:

String Type/Character Type | character | nchar | wchar | xchar
---------------------------|-----------|-------|-------|------
TString<TChar>          | String    | NString     | WString     | XString
TCString<TChar>         | CString   | NCString    | WCString    | XCString
TAString<TChar>         | AString   | NAString    | WAString    | XAString
TSubstring<TChar>       | Substring | NSubstring  | WSubstring  | XSubstring
TLocalString<TChar,N>   | LocalString<N>  | NLocalString<N>   | WLocalString<N>   | XLocalString<N>

Within this manual, most of the time, the simple names like \b String, \b CString or \b AString are
used, even when the corresponding templated class is meant. Likewise, if the names are linked, then the
link target resolves the template type and not the simple alias.
For example, this link: \alib{strings;TAString;AString}, links to class \b TAString.

The following subsections of this chapter introduce the main string types. This is done without
going into the details of each type's functionality but rather by explaining the principal
differences of the types.

\I{################################################################################################}
## 2.1 Class String ## {#alib_strings_classes_string}

The (only!) advantage of zero-terminated arrays, is that all that is needed to determine a
string is a pointer to the start of the array. Otherwise, along with that pointer, the length
of the string has to be given.

These two values, the pointer to the first character and the length of the string, are the only two
field members of class \alib{strings;TString;String}.
It could be said, that the main purpose of this class is to provide a pair of the two values, which
comprise a non-zero-terminated string and hence the type should be considered a
<b>"lightweight pointer to constant string data"</b>.

The terms "lightweight", "pointer" and "constant data" imply that class \b String is a simple C++
pod-type, with all benefits like having defaulted copy and move constructors, no destructor and
of course no virtual functions.

For example, if a \b String instance is created and deleted on the stack like this:

        {
            String s= "Hello";
        }

this is the same effort for the compiler (CPU) as creating a character pointer and a
simple integral value:

        {
            const char* cp    = "Hello";
            integer     length= 5;
        }

It is important to understand that creating, deleting and copying objects (values) of
type \b String is equivalent to doing the same with objects of type <c>std::pair<const char*, integer></c>.
Likewise, string values can simply be overwritten:

        String s= "Hello";
        s       = "World";
        s       = String( s.Buffer() + s.Length() / 4, s.Length() - s.Length() / 2 );

The last sample shows that reducing the length of the represented string by cutting portions
from either the front or the end of the string would be allowed operations: the resulting objects
still represents valid string data. However, none of the interface functions of the class
changes the pointer to the character array or the string's length. Such modifications are only
implemented with derived types introduced later. This way, class \c String does not only represent a buffer of
constant character data, but also the pointer to the buffer and the defined length are constant
themselves.

\I{################################################################################################}
## 2.2 Class CString ## {#alib_strings_classes_cstring}

A first type derived of class \b String is class \alib{strings;TCString;CString}.
The name of the class means "C language string": objects of this class represent zero-terminated
character arrays.

In all other aspects, this class is the same as its parent.

With this class derived, it becomes obvious why parent class \b String must not allow operations
that reduce the length of the string: The resulting shortened string would not be zero-terminated.
In other-words: If class \b String allowed operations that shortened the represented string, then
class \b CString could not be derived from it.

Class \b AString introduced in the next section, imposes a similar rationale why operations that
cut portions from the front of a \b String are likewise not allowed.

\I{################################################################################################}
## 2.3 Class AString ## {#alib_strings_classes_astring}

A second type derived of class \b String is class \alib{strings;TAString;AString}.
The prefix character <b>"A"</b> here simply stands for <b>"ALib"</b>. The class implements a
"heavy weight" string type, namely one that does not only "represent a string" but actively
allocates memory for the string data and manages that resource internally.
If a \b String object gets "assigned" to an object of this type, then the string data is
entirely copied into the character array buffer that class \b AString manages.

Consequently this class provides a huge set of interface functions that allow to modify the
contents of the array, and if the content is inserted that exceeds the capacity of the internal
buffer, a larger buffer is allocated and that allows storing the concatenated string data.

Cutting data from the end of the string is performed in constant time (<b>"O(1)"</b>) as only the
value of inherited field \alib{strings::TString;Length} needs to be decreased.
Cutting data from the start of the string is "linear" effort (<b>"O(N)"</b>): The remaining portion
of the string is copied to the start of the buffer and the string's length is adjusted.

\note
  To finalize a thought from the previous section: The latter now explains why parent class \b String
  does not offer an interface that decreases the represented string's length by cutting pieces from
  the start.

\I{################################################################################################}
## 2.4 Class Substring ## {#alib_strings_classes_substring}

Finally, a third type derived of class \b String is implemented with class
\alib{strings;TSubstring;Substring}.

It has in all respects the same properties as its base class \c String, especially it has the same
lightweight nature, it "represents" strings rather than "implementing" those and the string data
is represented is constant.

The only difference is that this type allows shortening the represented string and such shorting
can be done from both ends in constant time (<b>"O(1)"</b>): Same as with class \b AString,
"removing" data from the end is just about changing inherited field \b Length. Removing data
from the front also decreases the length and in parallel increases the pointer to the start of the
array.

An important use case of this type is to "parse" data from a string. Here, an object of this
type is created from an "input string" of just any other type and then the string is shortened
in a loop. Inside the loop, alternating operations of either parsing numbers, tokens or other values
or recognizing and removing delimiters and whitespaces is performed - until the string is empty.
The majority of interface methods offered therefore is named with the prefix <b>"Consume"</b>.
This indicates that not only some "parsing" takes place, but also that the corresponding characters
the substring are removed from it.


\I{################################################################################################}
## 2.5 Summary Of String Types ## {#alib_strings_classes_summary}

In the previous three sections, base class \alib{strings;TString;String} and three derived types
\alib{strings;TCString;CString},
\alib{strings;TAString;AString} and
\alib{strings;TSubstring;Substring} have been introduced.

\note A fifth type, class \alib{strings;TLocalString;LocalString} is only introduced in later
      chapter \ref alib_strings_assembly_localstring.

With that introduction, it was explained why the base class is limited in respect to changing the
string: Simply spoken, derived type \b CString disallows cutting substrings from the back, because
this would result in a non-zero-terminated string and derived type \c AString disallows cutting
substrings from the front, because its simple implementation of the memory management forces the
class to copy the remaining string data to the front of the allocated buffer.

All these explanations have been given to make the design rationale of the family of \alib string
classes completely transparent and understood. In other, higher level programming languages
this all would be an unnecessary complication of things. Even in C++ a more simple to understand
and to use design would be possible, for example, by using a abstract classes with virtual functions.

The design given here, aims to leverage the speed and efficiency of the C++ language. Once
the differences of the string classes is understood, choosing the right type becomes a very
clear, unambiguous and straightforward task in any programming situation.

To recap:

- Class \b String:<br>
  Lightweight constant pointer to an array of constant characters of constant length.<br>
  Used to represent and copy string values, especially as method arguments and return values.<br>
  This class is conceptually comparable to type <c>std::string_view</c>, which was introduced
  by the C++ standard library with <b>version 17</b>.

- Class \b CString:<br>
  Same as \b String but represents zero-terminated character strings.

- Class \b AString:<br>
  String buffer class that allows creating new strings and modify them in arbitrary ways.<br>
  Used to assemble complex strings for user messages, externalization of data, etc.<br>
  This class is conceptually comparable to type <c>std::string</c>, of the standard C++ library.

- Class \b Substring:<br>
  Same as \b String but the pointer to the start of the array may be increased and the
  length of the string may be decreased. The represented string data is still constant.<br>
  Primarily used to interpret string data, aka to "parse" strings.


\I{################################################################################################}
# 3. String Construction and Type Conversion # {#alib_strings_cc}

\I{################################################################################################}
## 3.1 Construction ## {#alib_strings_cc_construction}

\I{################################################################################################}
### 3.1.1 String Construction ### {#alib_strings_cc_construction_string}

This module makes use of the "character array traits" defined with dependency module \alib_characters.
For the use of the string classes, a developer does not need to know all details of these traits and
it is sufficient to understand what is said in the introductory chapter \ref alib_strings_intro_characters
of this manual.

The following table lists the constructors of class \b String. All constructors are inline
and mostly are compiled in the shortest code possible, which only copies the right values to
fields \alib{strings::TString;buffer} and \alib{strings::TString;length}.

No|Parameter(s) | Description
-|-------------|------------
1|\e None|Default constructor, sets field \b buffer to \c nullptr and length to \c 0.
2|\c nullptr (C++ keyword)|Sets field \b buffer to \c nullptr and length to \c 0.
3|<c>const TChar*</c>, <c>integer</c>|Sets fields \b buffer and \b length to the given values.
4|\c const \c T\c & with<br><c>ArrayTraits<T>::Access == Policy::Implicit</c>|Sets field \b buffer to the result of <c>ArrayTraits<T>::Buffer(src)</c> and field \b length to the result of <c>ArrayTraits<T>::Length(src)</c>
5|\c const \c T\c & with<br><c>ArrayTraits<T>::Access == Policy::ExplicitOnly</c>|Same as 4), but defined using keyword <c>explicit</c>.
6|\c T\c & with<br><c>ArrayTraits<T>::Access == Policy::MutableOnly</c>|Same as 4) but using keyword <c>explicit</c> and a mutable parameter.

Constructors \c 4, \c 5 and \c 6 are selected by the compiler in the case that an object of template
type \b T is given and an according specialization of type trait \alib{characters;ArrayTraits}
exists. Each of these constructors implements one the three elements of enumeration \alib{characters;Policy}
that classify the possible access of the character array data given with type \b T.

This set of constructors allow very intuitive and convenient construction of \alib strings
from 3rd-party string types.
Especially the case of implicit construction is interesting: If a method argument is declared as
a constant reference type, the C++ compiler will perform one "implicit conversion", if a different
type is passed for such argument.<br>
As a sample, we have function \b foo defined as:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PASSING_FUNC
With this, an invocation passing just any string type (that allows implicit access) is possible:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_PASSING

\note
   In consideration that the type-traits can be defined for arbitrary 3rd-party string types, the
   implicit string construction is a very remarkable achievement of this approach!
   What is demonstrated in the sample above constitutes a tool to developers to unify different
   string types defined in different libraries.

\note
   Without this solution, a developer would either need to provide different overloaded versions of
   a method (which each accepts a different string type), or she would have to place explicit string
   argument conversion code to each invocation of a method!

\note
   Therefore, this feature is in perfect alignment with the
   \ref alib_strings_intro_goals "primary design goal" of this module, which is to
   "Mitigate the C++ string problem", as well with one of the overall design goals of \alib
   to be \ref mainpage_goals "least-intrusive".


\I{################################################################################################}
### 3.1.2 CString Construction### {#alib_strings_cc_construction_cstring}
The exact same set of constructors that are listed in the table of the previous section for
class \alib{strings;TString;String}, are implemented with class \alib{strings;TCString;CString}.
The only difference is that constructors \c 4 to \c 6 are testing for a specialization of
struct \alib{characters;ZTArrayTraits} instead of \alib{characters;ArrayTraits}.

Therefore, all that was explained in respect to construction of type \b String from templated types
that represent character arrays, is equivalently true for the construction of type \b CString
from types that represent <em>zero-terminated</em> character arrays!

\I{################################################################################################}
### 3.1.3 AString Construction ### {#alib_strings_cc_construction_astring}
In contrast to \b String and \b CString, type \alib{strings;TAString;AString} does not allow
implicit construction. All constructors are explicit with the exception of the move-constructor.
This design decision was made because of the heavy-weight nature of the class.

Apart from the need to be explicit, construction of the class is even more flexible than the
construction of the lightweight string types: Type-traits functor \alib{strings;AppendableTraits} allows
creating string representations for objects of custom types. In addition to the character array types
that base class \b %String accepts, these types are accepted by a templated constructor of the class
as well.
All details about this template struct are given with chapter
\ref alib_strings_assembly "5. String Assembly".

\see
   Paragraph \ref alib_ns_strings_astring_copymove "Copy/Move Constructor and Assignment"
   of this class's reference documentation. This provides some rationale for the explicit
   nature of \b AString construction.

\I{################################################################################################}
### 3.1.4 Substring Construction ### {#alib_strings_cc_construction_substring}
Class \b Substring simply inherits all constructors of its base class \b String and therefore,
all that had been written in previous chapter \ref alib_strings_cc_construction_string, is
true for this class. This includes that the type <c>const alib::Substring&</c> may be used as
method arguments to accept any type of string of fitting character size, without explicit
conversion.

\note
  The rationale for this design decision is as follows:
  As explained before, class \alib{strings;TSubstring;Substring} specializes class \b String
  by adding features that remove characters from the start and the end of the string.
  If class \c String did not play the role of being the base class for types \c CString and \c AString,
  these features could be implemented with class \c String itself and class \b Substring would not
  be even needed. In this respect, class \b Substring is not a specialization of \b String but
  more a "continuation". With that in mind, it makes a lot of sense that all parental constructors
  are exposed and usable.


\I{################################################################################################}
## 3.2 Casting Strings To Other Types ## {#alib_strings_cc_cast}

The previous chapter talked about how the different \alib string types are constructed.
This chapter now discusses the opposite: the string types implement C++ cast operators that allow to
construct values of arbitrary string types from those.

Again, the cast is performed using the type-traits defined with dependency module \alib_characters.
This time, the value of field \alib{characters::ArrayTraits;Construction} of specializations
of \alib{characters;ArrayTraits} respectively \alib{characters;ZTArrayTraits} are tested.
Possible values are given with enumeration \alib{characters;Policy}. With that
casting string types to a specific custom type is either not allowed, implicitly allowed or
allowed only if explicitly performed.


\I{################################################################################################}
### 3.2.1 Casting From String And Substring ### {#alib_strings_cc_cast_string}

Class \b String implements an implicit cast operator to values of template type \b T if
a specialization of \alib{characters;ArrayTraits} exists that defines field \b Construction
to be \alib{characters;Policy::Implicit}. Likewise, an explicit operator is available
if \alib{characters;Policy::ExplicitOnly} is given.<br>
Of course, the construction of the casted object is performed by invoking
\alib{characters;ArrayTraits::Construct}, passing the string's fields \b buffer and \b length.

With the same rationale as given in \ref alib_strings_cc_construction_substring, class
\b Substring behaves 100% the same as parent class \b String in respect to casting options.

\I{################################################################################################}
### 3.2.2 Casting From CString ### {#alib_strings_cc_cast_cstring}

Class \b CString implements the very same casts operators as class \b String, with the only difference
that the struct \alib{characters;ZTArrayTraits} is used instead of \b ArrayTraits.

\note
  With the built-in specialization of \b ZTArrayTraits for C++ type <c>const char*</c> that
  defines implicit casts, objects of type \b CString can be passed to "old school" interface
  methods that expect a zero-terminated character array as an argument, without an explicit cast.

\I{################################################################################################}
### 3.2.3 Casting From AString ### {#alib_strings_cc_cast_astring}

Class \b AString implements each of the cast methods that are provided with class \c String and
\b CString. This is due to the fact that the class always reserves space in the allocated buffer
for a terminating character. This way, for the preparation for casting to an arbitrary
zero-terminated array type is performed in constant time, as no string data has to be moved to
a newly allocated buffer.

The four casts methods makes this class the most flexible of the \alib string types, in respect to
implicitly or explicitly creating external character array types.

\I{################################################################################################}
### 3.2.4 Suppressing Casts ### {#alib_strings_cc_cast_suppress}
Casts, especially implicit ones, in some situations may impose ambiguities, which lead to
compilation failures. To mitigate such, the implicit cast implementations of all three classes
\b %String, \b %CString and \b %AString are conditionally selected by the compiler using the struct
\alib{strings;NoAutoCastTraits}.

\alib specializes this struct to prevent the casting of \b AString objects to types
\c String and \b CString, which the type-traits \b %ArrayTraits and \b %ZTArrayTraits of course,
would indicate to be allowed.
This is ambiguous in respect to the implicit cast that is also allowed.

Custom specializations should only be needed in similar situations, where a custom string type
allows auto-casts based on the type-traits provided by \alib.

\I{################################################################################################}
## 3.3 Built-In Conversions ## {#alib_strings_cc_builtin}
This module \alib_strings_nl is not "responsible" for defining the built-in conversion rules
for C++ and 3rd-party types, because in fact, these rules are defined already with the
specializations of the structs \b ArrayTraits and \b ZTArrayTraits given in dependency module
\alib_characters.

While these specializations are described in the corresponding Programmer's Manual section
\ref alib_characters_builtintraits "4. Built-In Character Array Traits" of that module,
only a summary of the rules from the perspective of \alib string classes is given here.

<b>Fixed-length Character Arrays</b>:<br>
- Implicit construction of \b String objects.
- Implicit construction of \b CString objects (because of string literals being fixed-length arrays).
- No casts from any \alib type.

<c>const TChar*</c>:<br>
- Implicit construction of \b String and CString because constant character pointers are
  considered zero-terminated (design decision along C++ language standards).
- Explicit casts from \b String objects: a programmer that converts a \b String to this type
  needs to be sure that either the originating string is zero-terminated or the converted
  pointer is not expected to point to a zero-terminated string.
- Implicit casts from \b CString and \c AString.

<c>TChar*</c>:<br>
In general this library considers mutable character pointers a "dubious" type and unlike their
constant counterparts, arrays pointed to by this type are not considered zero-terminated.
Therefore, all conversion functions are explicit.

<c>std::string_view</c>:<br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Implicit cast from \b String and \b CString objects because of the lightweight nature of the type.

<c>std::string</c>:<br>
- Implicit construction of \b String objects.
- Implicit construction of \b CString objects, because accessing the internal buffer automatically
  zero-terminates it.
- Implicit cast from \b String and \b CString although heap-memory allocation and the copying
  of string data is involved. The rationale for this decision lies in technical reasons,
  as \ref alib_characters_builtintraits_std "explained here".

<c>std::vector<TChar></c>:<br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Explicit cast from \b String and \b CString objects because of the heap-memory allocation
  and data copy involved.

\c QStringView:   <br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Implicit casts from \b String.
- Implicit casts from \b CString.

\c QString:       <br>
- Implicit construction of \b String objects.
- Implicit construction of \b CString objects.
- Explicit casts from \b String.
- Explicit casts from \b CString.

\c QLatin1String: <br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Implicit casts from \b String.
- Implicit casts from \b CString.

\c QByteArray:    <br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Explicit casts from \b String.
- Explicit casts from \b CString.

\c QVector<uint>: <br>
- Implicit construction of \b String objects.
- Explicit construction of \b CString objects.
- Explicit casts from \b String.
- Explicit casts from \b CString.

\I{################################################################################################}
## 3.4 Adopting 3rd-Party String Types## {#alib_strings_cc_3rdparty}

In the previous sections a quite remarkable and unique feature of this module, namely the
possibility of (implicit) conversions of arbitrary C++ string types to and from \alib string types,
has been described.
These features contribute fundamentally to a major \ref alib_strings_intro_goals "design goal"
of this module, by relieving a programmer from the burden to convert string types when mixing
libraries that expect different strings.

\note
  This is true at least for the case that the string types that become mixed are based on the
  same \ref alib_characters_chars "character type".
  In later chapters of this manual, further tool types are introduced, which in addition
  mitigate the problem of necessary string conversions if different character widths are involved.

With the previous descriptions it has been mentioned that the documentation of dependency module
\alib_characters is not required to be read if \alib string types are to be just used.

To adopt custom string types to become "compatible" with \alib strings all that has to be done
is to specialize the type trait \b ArrayTraits and, in the case that a type represents zero-terminated
strings, also struct \b ZTArrayTraits.
While this is done with only a few lines of code, still it is advised to start reading the
\ref alib_mod_characters "Programmer's Manual of module ALib Characters". If not from the beginning
then at least chapter \ref alib_characters_arrays "4. Character Arrays". Together with the
information provided in the previous sections of this manual, the complete picture should
be given and the adoption of own types be a straight forward task.

In addition header-files
- "alib/compatibility/chararray_std.hpp" and
- "alib/compatibility/chararray_qt.hpp"

can be used as a good template to use for the adoption of own string types.

\note
   The approach taken here is suitable only for types that are something very close to a string
   type. While the concept might be "misused" to implement a sort of <b>"ToString()"</b> function
   for custom types, this is not recommended. For the latter, the suitable mechanism is
   provided with "appending objects to type AString", which is described in chapter
   \ref alib_strings_assembly "5. String Assembly".


\I{################################################################################################}
# 4. Non-Zero-Terminated String Detection # {#alib_strings_nzt}

\I{################################################################################################}
## 4.1 Ambiguities With Overloaded Functions ## {#alib_strings_nzt_problem}

Implicit string construction as discussed in the previous chapter allows creating method
interfaces that accept "arbitrary" custom string types. It was explained that type-traits
\b ArrayTraits and \b ZTArrayTraits might be specialized for custom types and with that
string classes \b %String and \b %CString might be created implicitly from objects of those.

With these two types given, it is not possible to create an API interface that clearly separates
between custom types that are zero-terminated and those that are not. This problem is best explained
with a sample.

Imagine a namespace function called \b IsDirectory that should accept a constant directory path
string and should return \c true if the argument represents an existing directory in the filesystem
and \c false if not. The function declaration would be like this:

            bool IsDirectory(const String& path);

Now, many actual implementations of the function (for example, on the GNU/Linux operating system),
would need to pass a zero-terminated string to a corresponding operating system call.
To create that, the accepted string argument is needed to be copied to a buffer that can
be terminated. This effort is redundant if a user invoked the function like this:

            auto result= IsDirectory( "/usr/bin" )

because the string literal given is already zero-terminated.
To avoid this, an overloaded function definition could fetch zero-terminated strings and pass
those without the copy and termination overhead:

            bool IsDirectory(const CString& path);


But with these two methods in place, the compiler complains about an ambiguity as soon as
zero-terminated string types are passed. The reason for this is simply because the normal
string type \b %String can be implicitly constructed from zero-terminated string types as well.

\I{################################################################################################}
## 4.2 Class StringNZT ## {#alib_strings_nzt_solution}

As a way out of the ambiguity described in the previous section, class
\alib{strings;TStringNZT;StringNZT} is given with the library.
The "NZT" suffix stands for "non-zero-terminated". The type extends class \b String and all it does
is to deny implicit construction by objects of types that would likewise construct type \b CString.

With that, the two overloaded namespace functions:

        bool  IsDirectory( const StringNZT&  path );
        bool  IsDirectory( const CString&    path );

are not ambiguous. The first function's implementation would usually copy and terminate the given
non-terminated string, for example, by just creating an \b AString object from the given
non-zero-terminated string. Then it would invoke the second method passing the \c AString, which
becomes zero-terminated on the fly when converted to \b CString.

\I{################################################################################################}
## 4.3 Summary ## {#alib_strings_nzt_summary}
The following bullets summarize and refine what was sampled in this chapter:
- Class \alib{strings;TStringNZT;StringNZT} can be constructed from types with corresponding
  specialization of the struct \alib{characters;ArrayTraits}, but only if complementary struct
  \alib{characters;ZTArrayTraits} is not specialized in parallel.
- This is in contrast to its base class \alib{strings;TString;String} which constructs if either
  of the type-traits is given.
- Therefore, offering \b StringNZT and alternatively \b CString in two overloaded functions
  avoids ambiguities and allows explicit treatment of zero-terminated and non-zero-terminated
  strings.
- The use of class \b StringNZT should be limited to this and similar use case.
- Consequently, the existence of an interface method using type \b StringNZT for an argument type
  indicates the existence of an overloaded alternative using \b CString.

Finally it should be mentioned that the use of zero-terminated strings is not recommended. \alib itself
does that only in very specific situations. An example is class \alib{system;Path}. The class
interfaces with the operating system that expects zero-terminated strings, like it was sampled in
the previous section.



\I{################################################################################################}
# 5. String Assembly # {#alib_strings_assembly}

Often, software needs to assemble strings. May it be human-readable text, data serialization
or for the implementation of communication protocols. For that, a string type is needed that
manages a data buffer and provides interface methods that allow the concatenation of data to
existing strings. Furthermore typical methods like searching and replacing substrings, letter
case conversion, etc. has to be offered.

As already introduced, for this purpose class \alib{strings;TAString;AString} is provided with
this module. Therefore, this chapter dedicated to the topic of string assembly is mostly a chapter
about class \b AString.


\I{################################################################################################}
## 5.1 Appending Custom Types ## {#alib_strings_assembly_ttostring}

In the previous chapters of this manual it was explained how the lightweight \alib string types
\c String, \c CString and \c Substring are constructable using values of C++ types which are
equipped with "character array traits". Those traits are nothing else but meta-information about
these types which is provided by corresponding specializations of templated structs \b ArrayTraits
and \b ZTArrayTraits. The character array type-traits are introduced with module \alib_characters.

Some high level object-oriented programming languages offer a root class which provides a common
interface for just any derived type and such interface may contain a method that creates a
string representation from an instance. For example, the \b JAVA language defines class \c Object
which provides method \c toString() for such purpose.<br>

The two concepts (\alib character array traits and the <c>Object.toString()</c> method of \b Java)
are fundamentally different: Character array traits are meant to be given for types whose
main purpose is to represent or implement character arrays, while the \c toString() method may
be implemented for just any type.

Class \b AString, which is designed to support the assembly of strings, offers a feature
that much more corresponds the \c toString() concept. Again, type-traits are used, this time
not for accessing (existing) character array data, but for appending a string representation
of any object to an \b AString.

\note
  With C++20, the function <c>std::format</c> and the type traits struct <c>std::formatter</c>
  have been introduced.
  Class \b AString supports the direct use of such formatters to write into its buffer.
  (Meaning without the need of creating an intermediate <c>std::string</c>.)
  Furthermore, specializations made for custom types to support <c>std::format</c> can be used
  directly with class \alib{strings;TAString} and, the other way round, the customizations
  explained in the upcomming sections, likewise allow to use the custom type with
  <c>std::format</c>.
  <br>
  Thus, only one customization has to be made, as long as no placeholder format string
  is to be parsed.
  All about this is explained in the later chapter \ref alib_strings_stdformat.
 
\I{################################################################################################}
### 5.1.2 Type-Traits Functor AppendableTraits ### {#alib_strings_assembly_ttostring_ttostring}

Type-traits "functor" \alib{strings;AppendableTraits<TAppendable,TChar,TAllocator>} by default is empty.
To allow the creation of a string representation of objects of a custom type \b TAppendable, a
specialization of the struct has to be defined that implements method
\doxlinkproblem{structalib_1_1strings_1_1T__Append.html;a3266f5aa56dcdb368c9b2aa269f346b9;AppendableTraits::operator\(\)(TAString<TChar>&); const TAppendable&)}.

Besides specifying the type that is adopted with template type \b TAppendable, the character type
\b TChar of the destination \b AString object may be given with a specialization.
If omitted, it defaults to type \alib{characters;character}.

As the name of functor \b AppendableTraits suggests, the implementation of the operator usually
appends a string representation of the object given with parameter \p{src} to the \b AString given
with parameter \p{target}.
Nevertheless, an implementation is free to modify the given \b AString in any way.
For example, built-in type \alib{strings;TEscape;Escape} searches and replaces
"escape-characters" when "appended" to an <b>AString</b>!

\I{################################################################################################}
### 5.1.3 Method AString::Append (And Aliases)### {#alib_strings_assembly_ttostring_append}

Once type-traits functor \alib{strings;AppendableTraits<TAppendable,TChar,TAllocator>} is specialized for
a type \b TAppendable, objects of that type may be appended to objects of
\b TAString<TChar,TAllocator>. This can be done using the following methods:

- \alib{strings;TAString::TAString(const TAppendable&);AString(const TAppendable&)} (A constructor taking the appendable type)
- \alib{strings;TAString::Append(const TAppendable&);Append(const TAppendable&)}
- \alib{strings;TAString::_(const TAppendable&);_(const TAppendable&)} (A method named solely <c>"_"</c>. Provided for compatibility with JAVA and C# versions of \alib.)
- \alib{strings;TAString::operator<<(const TAppendable&);operator<<(const TAppendable&)}

Methods \b Append and <b>'_'</b>, as well as operator <b>'<<'</b>, each return a reference to the \b AString
that they were invoked on. This allows concatenated calls, like in:

        AString aString;
        aString << "The result is: " << 42;


\I{################################################################################################}
### 5.1.4 Built-In Appendable Types ### {#alib_strings_assembly_ttostring_builtin}
The specializations of functor \b AppendableTraits that come with the \alib library can be
grouped into four areas:

<b>1. Fundamental C++ Types:</b><br>
Specializations for all fundamental C++ types like \b int, \b double, etc. are provided.

<b>2. Special Formatting Helpers:</b><br>
A set of types exist that can be used to wrap values that are to be appended
to a string, with the single aim to change their formatting.
For integral and floating point numbers, such formatters are \alib{strings;TDec},
\alib{strings;TBin}, \alib{strings;THex} and \alib{strings;TOct}.

Further similar types are \alib{strings;TTab}, \alib{strings;TField} and \alib{strings;TEscape}.

All these types are very lightweight and are supposed to be created locally, right with the
invocation of the append-methods.
Furthermore, each type has three corresponding non-templated alias-types in namespace #alib.
For example, with class \b TTab, the types
- alib::Tab,
- alib::NTab, and
- alib::WTab

come along.
 
As a quick example, the use of \alib{strings;TField} is showcased:

\snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_FIELD
The code above which produces the following output:

\verbinclude "DOX_APPEND_FIELD.txt"


<b>3. Other ALib Types:</b><br>
For various types found in other \alibmods, specializations of \b AppendableTraits are provided.<br>
All elements of important enum types are appendable.For more information,
see the section \ref alib_enums_records_details_serialization "4.3.1 Serialization/Deserialization"
of the Programmer's Manual of module \alib_enumrecords.


<b>4. 3rd-Party Types:</b><br>
The special \ref alib_manual_modules_impludes_further "compatibility headers" contain
specializations of \b AppendableTraits for types of the C++ standard library (namespace \c std),
as well of types of 3rd-party libraries.

\note
  While the C++ language demands to implement specializations of templated structs within the
  namespace that the original struct was defined in, the reference documentation "fakes" these
  specializations into the (otherwise non-existent!) inner namespace \ref alib::strings::APPENDABLES.
  Other \alib modules do the same documentation trick, and hence all specializations of \b AppendableTraits
  (of all four areas described above) can be found with the
  \ref alib::strings::APPENDABLES "reference documentation" of that namespace (and inner namespaces).


\I{################################################################################################}
### 5.1.5 Sample Implementation ### {#alib_strings_assembly_ttostring_sample}

The following code snippet demonstrates how to implement the specialization of functor
\b AppendableTraits for internal \alib class \alib{time;DateTime} to print out a formatted date:

\snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_DEFINITION

With this definition included, a code unit might now append \b DateTime objects to strings:
\snippet "DOX_ASTRING_APPEND.cpp"     DOX_APPEND_USE

The output would be for example:

\verbinclude "DOX_ASTRING_APPEND.txt"

The following macros are provided to simplify the specialization of \b AppendableTraits and make the code
more readable:
- \ref ALIB_STRINGS_APPENDABLE_TYPE
- \ref ALIB_STRINGS_APPENDABLE_TYPE_N
- \ref ALIB_STRINGS_APPENDABLE_TYPE_W
- \ref ALIB_STRINGS_APPENDABLE_TYPE_DEF
- \ref ALIB_STRINGS_APPENDABLE_TYPE_DEF_N
- \ref ALIB_STRINGS_APPENDABLE_TYPE_DEF_W
- \ref ALIB_STRINGS_APPENDABLE_TYPE_INLINE
- \ref ALIB_STRINGS_APPENDABLE_TYPE_INLINE_N
- \ref ALIB_STRINGS_APPENDABLE_TYPE_INLINE_W



\I{################################################################################################}
## 5.2 Construction Of AStrings ## {#alib_strings_assembly_astringconstr}
Class \b %AString hides all parent constructors and offers re-implementations that rather
<b>copy</b> the data that is passed. Consequently - as this copying is not considered a lightweight
operation - all constructors are explicit.
By the same token, the assignment operator is not applicable with initializations as well.<br>
The following code will not compile:
\code{.cpp}
AString as=  "This will not compile";
\endcode
 * Instead, explicit construction has to be chosen, as shown here:
\code{.cpp}
AString as("This will compile");

// or alternatively
AString as;
as= "This will compile";
\endcode


As already noticed in chapter \ref alib_strings_assembly_ttostring, with templated constructor
\alib{strings;TAString::TAString(const TAppendable&);AString(const TAppendable&)},
class \b %AString accepts any type of object that a specialization of functor \alib{strings;AppendableTraits}
exists for. This makes construction very flexible.<br>

Copy constructor, move constructor and move assignment are well defined, which allows \b %AString
objects to be used (as efficiently as possible) as value types in containers of the standard library,
for example, as in <c>std::vector\<AString\></c>.

\I{################################################################################################}
## 5.3 Buffer Management ## {#alib_strings_assembly_buffermgmnt}
As mentioned before, class \b %AString provides logic to manage its own buffer.
During the assembly of strings, the buffer "automatically" grows as needed. If a certain minimum
size can be foreseen as a result of a string assembly, before performing the assembly operations,
the necessary buffer size might be reserved by invoking method
\alib{strings::TAString;SetBuffer;SetBuffer(integer)}. This avoids the automatic growth process
which may take place in several steps and each steps may involve to copy the current buffer to
a new memory location.

Once grown, the allocated buffer size is never reduced, unless method
\alib{strings::TAString;SetBuffer;SetBuffer(integer)} is explicitly invoked providing a smaller
size than currently allocated.

Besides this internal, automatic memory allocation, the class can also work on <b>external buffers</b>.
For this, overloaded method
\doxlinkproblem{classalib_1_1strings_1_1TAString.html;a55cf0dcbe43b1563f8a6b006e52685a0;TAString::SetBuffer;TAString::SetBuffer(TAString::SetBuffer)}.
allows providing such external memory.
The life-cycle of an external buffer is not bound to the life-cycle of the \b %AString object itself.
At the moment that the size of an external buffer is not sufficient to allow a requested extension
of the managed string, the class replaces the external buffer by a larger, self-managed one.

For details on using external buffers, see the reference documentation of overloaded method
\doxlinkproblem{classalib_1_1strings_1_1TAString.html;a55cf0dcbe43b1563f8a6b006e52685a0;TAString::SetBuffer;TAString::SetBuffer(TAString::SetBuffer)}.
Class \alib{strings;TLocalString;LocalString}, which is discussed in the next section, makes use
of this feature and provides the possibility to have local (stack based) allocations of strings.

\I{################################################################################################}
## 5.4 Class LocalString ## {#alib_strings_assembly_localstring}

Template class \alib{strings;TLocalString;LocalString<TChar, TCapacity>}, derived from class \b %AString
uses an internal character array of a length specified by template parameter \p{TCapacity}
to store the string data. During construction, the memory address of this character array member is
passed to method
\doxlinkproblem{classalib_1_1strings_1_1TAString.html;a55cf0dcbe43b1563f8a6b006e52685a0;TAString::SetBuffer;TAString::SetBuffer(TAString::SetBuffer)}.
The huge benefit of using the class lies in performance: The performance impact of <em>heap allocations</em> is often
underestimated by software developers. Therefore, for local string operations with foreseeable
maximum string buffer sizes, class \b %LocalString should be considered as a faster
alternative of class \b %AString.


\I{################################################################################################}
### 5.4.1 Exceeding the Buffer Capacity ### {#alib_strings_assembly_localstring_exceed}
Although the internal buffer size is fixed at compile-time and hence cannot be expanded, a user
of the class must not fear '<em>buffer overflows</em>'. If the internal buffer capacity
is exceeded, a new buffer from the <em>free memory</em> (aka '\e heap') will be allocated.

With debug-builds of \alib, parent class \alib{strings;TAString;AString} provides a
\alib{strings;TAString::DbgDisableBufferReplacementWarning;warning mechanism} that allows the
easy detection of such (probably unwanted) replacements of the local buffer.
There are two scenarios how this mechanism might be used during development:
- If the buffer should never be replaced, the capacity of a %LocalString has to be increased
  step-by-step (during the software development/testing cycle) at the moment the warning is issued.
  This has to be done until the member-buffer is huge enough and no more warning is raised.
- If it is OK that the buffer is replaced "every once in a while" because special
  situations with higher capacity requirements may well occur but are still rather seldom,
  then the warning should be switched off for the specific instance. By switching the warning off, a developer
  places the information in the code that the internal buffer size might be too small in some occasions.
  Having this explicit information helps to understand the intentions of the software developer.

If the latter case applies, then the warning can be disabled using the inherited method
\alib{strings::TAString;DbgDisableBufferReplacementWarning}. This inline method is empty in
release-compilations and this way optimized out by the compiler.

\I{################################################################################################}
### 5.4.2 Implicit construction ### {#alib_strings_assembly_localstring_construction}
While class \b %AString (as \ref alib_strings_assembly_astringconstr "noted above") does not provide
implicit construction, class \b LocalString re-implements the common constructors of
\b %AString and exposes them as <em>implicit</em>.
The rationale here is that although the data is copied (which might not be a very lightweight task),
still the performance impact is far less compared to constructing an \b %AString that uses a
heap-allocated buffer. The design decision behind that takes into account that a
\b %LocalString copies an argument to its local buffer without the explicit exposure of this operation.

The following method, as a sample, takes three different \alib string types as parameters:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_AS_PARAMETERS_1
The following code will \e not compile:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_AS_PARAMETERS_2
Class AString has to be explicitly created, the others don't:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_AS_PARAMETERS_3

In addition, besides having implicit construction, the default assign operator is defined as well
with \b LocalString.
This allows using objects of this type as class members that are initialized within the class
declaration as shown here:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_ASPREALLOC_MEMBER
Such members are not allowed to be initialized in the declaration if their type is \b %AString.

\I{################################################################################################}
### 5.4.3 No Move Constructor ### {#alib_strings_assembly_localstring_moveconstr}
Class \b %LocalString provides no <em>move constructor</em> and thus is very inefficient
in scenarios where objects of the class could rather be moved than copied. Consequently such
situations are to be avoided. The use of \b %LocalString should instead be
very determined and it should not be subject to copy and move operations.

\I{################################################################################################}
### 5.4.4 Aliases For Frequently Used Sizes ### {#alib_strings_assembly_localstring_aliases}
Within namespace \ref alib, some convenient alias type definitions are available that define
local strings of frequently uses sizes:
- \alib{String8}, \alib{String16},  \alib{String32},\alib{String64},\alib{String128},\alib{String256},\alib{String512}, \alib{String1K},  \alib{String2K},  \alib{String4K},
- \alib{NString8},\alib{NString16}, \alib{NString32},\alib{NString64},\alib{NString128},\alib{NString256},\alib{NString512}, \alib{NString1K}, \alib{NString2K}, \alib{NString4K}, and
- \alib{WString8},\alib{WString16}, \alib{WString32},\alib{WString64},\alib{WString128},\alib{WString256},\alib{WString512}, \alib{WString1K}, \alib{WString2K}, \alib{WString4K}.

\I{################################################################################################}
# 6. Other Aspects Of ALib String Types # {#alib_strings_details}

\I{################################################################################################}
## 6.1 Nulled Strings ## {#alib_strings_details_nulled}

\I{################################################################################################}
### 6.1.1 Nulled Vs. Empty Strings ### {#alib_strings_details_nulled_vsempty}

An important aspect of the family of string types provided by this module and library, is concept of
<em>"nullable"</em>  strings.
An object of base class \b String is \e nulled, when constructed:
- with keyword <em>nullptr</em>, or
- with a likewise \e nulled object of character array type.

\note
  The default constructor is \e defaulted and leaves a <b>String</b>'s members undefined!

An existing string can be set to \e nulled state, by assigning keyword \c nullptr or another
nulled object of character array type.<br>
Precisely, a string is \e nulled, when the internal pointer to the character array evaluates to
\c nullptr.

The concept of nullable strings differs from the concept of having empty strings. The latter
refers to string objects of zero length.

While \e nulled strings are always also \e empty (hence have length of zero). The other way round,
empty strings are not necessarily \e nulled. An empty string that is not \e nulled does not equal
an empty string that is \e nulled.

Inline methods
\alib{strings;TString::IsNull;IsNull},
\alib{strings;TString::IsNotNull;IsNotNull},
\alib{strings;TString::IsEmpty;IsEmpty} and
\alib{strings;TString::IsNotEmpty;IsNotEmpty}
of base class \alib{strings;TString;String} test strings objects for being \e nulled
or \e empty.

The following code runs fine (with no assertion):
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_NULLED_AND_EMPTY

Especially the last line of this code is important to understand: a \e nulled string is different
from an empty string.


\I{################################################################################################}
### 6.1.2 Nulled AStrings ### {#alib_strings_details_nulled_astrings}

The concept of having \e nulled strings is equally available with derived string type
\alib{strings;TAString;AString}:
An object of type \b AString is \e nulled when no internal buffer is allocated and likewise no
\ref alib_strings_assembly_buffermgmnt "external buffer" is set.

If default constructed, constructed with zero size, with keyword \c nullptr or any other \e nulled
string, no buffer is created.
Consequently, it makes a difference if an \b %AString is constructed using <c>%AString()</c> or
<c>%AString(\"\")</c>.

\note This is a difference to standard C++ class \c std::string, which always allocates a buffer
      and thus does not support a \e nulled state.

The allocated buffer of a \e non-nulled \b AString can be disposed by invoking
\alib{strings;TAString::SetBuffer;SetBuffer(0)} or by invoking
\alib{strings;TAString::SetBuffer;SetNull} on the instance.

To make this more clear, note the following <b>sample code</b> which does not throw an assertion:
\snippet "DOX_STRINGS.cpp"    DOX_ASTRING_NULLED

\note
  Unlike lightweight type \b String which allows the assignment of \c nullptr to set the string
  to \b nulled state, class \b AString does not support any assignment operator, but the
  C++ copy assignment.<br>
  To remove an existing buffer from an \b AString and this way to set a \e non-nulled instance back
  to \e nulled state, is a rare and unusual use case.
  The code above is rather provided for demonstration and completeness.<br>


\I{################################################################################################}
### 6.1.3 Pros And Cons ### {#alib_strings_details_nulled_proscons}

What was said in the previous two sections might not need any further explanation and experienced
programmers might skip to the next chapter. However, because of the fact that many string types
of other libraries behave differently, some further notes should be given:

The fact that string objects can be \e nulled allows "transporting" a piece of information along with
the string that can be used in APIs. For example, if a method should receive a string object
according to a key-property, a \e nulled result may indicate that no data existed to the given
key. This is in contrast to returning an empty string, which indicates that data was found, but that
the result just is an empty string. If \alib strings types were not \e nullable and in this sample
empty strings should be allowed as a valid answer, a second return value had to be defined for the
API function that indicates if a string existed for a given key-property.
Such API design paradigm is used frequently across various \alibmods.

On the other hand, when string values are used as input data, some caution has to be taken to
ensure that method invocations on a given input string is even allowed. Some methods may
produce undefined behavior when invoked on \e nulled string objects.

To maximize code performance, explicit tests for \e nulled strings should be avoided if not
necessary, which sometimes can be an obligation to the programmer that uses the string types.
More on this topic is given in the next section.


\I{################################################################################################}
## 6.2 "Non-Checking" Methods Of ALib String Classes ## {#alib_strings_details_nonchecking}

Several of the methods found in the different string classes of \alib are templated with
a boolean template parameter named \p{TCheck}. This template parameter is defaulted with
the tag-type \alib{CHK} which hides the whole concept it in "normal" code. Consider the following
snippet:
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_NONCHECKING_1

Two string methods are used in this code sample:
\doxlinkproblem{classalib_1_1strings_1_1TString.html;a7c6e1eba65efe930eedd3f64d8e70540;TString::IndexOf;TString::IndexOf(const TString&,integer)}
and
\alib{strings;TSubstring::ConsumeChars;Substring::ConsumeChars}. Both methods support templated parameter
\p{TCheck}! The following code provides the parameter in its default value, and hence for the
compiler is equivalent to the previous snippet:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_NONCHECKING_11

The exact impact of the value of template parameter \p{TCheck} is documented with each function that
supports it.
In general, with \alib{CHK}, the string object that a method is invoked on is checked, for example,
for not being \e nulled.
Furthermore the parameters given are checked, for example, to not being \e nulled, to be in valid
ranges, and so on.

In the sampled case of method
\doxlinkproblem{classalib_1_1strings_1_1TString.html;a7c6e1eba65efe930eedd3f64d8e70540;TString::IndexOf;TString::IndexOf(const TString&,integer)},
the documentation tells us that
- parameter \p{needle} must not be empty.
- Parameter \c startIdx must be in the range of \c 0 and the string's length minus the needle's
  length.

The latter cannot be guaranteed for the sample's method argument \p{line} and therefore the check
has to be performed. As a side effect, this check implicitly tests for a given \e nulled string,
because in the case that the given string is shorter than the token <c>"<start>"</c>, the
method returns \c -1.
This way, no user code for checking the input argument is needed in this sample code.

The implementation of method \alib{strings::TSubstring;ConsumeChars} by default checks if the string
is long enough to cut the given number of characters from the front. In other words, it tests
whether parameter \p{regionLength} is in the range of \c zero and the length of the string.
Obviously, this check is \b redundant in this sample. The method is invoked only if method
\b IndexOf had found the token <c>"<start>"</c> in the string!<br>
To avoid the redundant check, for this invocation the non-checking version of
method \b ConsumeChars may be used by providing \c false for the template value:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_NONCHECKING_2

The obvious goal of using non-checking method versions lies in avoiding redundant code, hence
to reduce code size and improve execution performance. As a majority of string methods are inlined,
the C++ compiler often is able to detect and remove redundant checks on its own. In these cases,
the use of the non-checking version of a method has no effect in optimized release compilations.<br>
However, there are many occasions where the compiler is lacking information on the state of
variables that a programmer might know about and then, non-checking versions might have a huge impact
when used in loops and other critical code sections. Also, in the sample above, it is very doubtful that
any of today's C++ compilers "knows" what it needs to know to optimize the redundant checks out.

So, what is that "something" that we phrased as "a programmer knows" and a "compiler does not know" above?
In computer science, such information is referred to as \b "invariants". Usually, \e invariants are
used to prove the correctness of algorithms. \e Invariants are expressions on variables that always evaluate
\c true when program execution hits a specific line of code.<br>
In the sample above, the relevant \e invariant that allows us to use the non-checking version of method
\b ConsumeChars, could be phrased as:

        The length of string "line" is as least as long as "idx" plus the length of token "<start>".

Now, by using the non-checking version and appending <c>"<NC>"</c> to the method invocation,
not only do we help the compiler to create shorter and faster code, we also put information
about the \e invariant into the code. And this is a benefit that should not be under-estimated!
By just looking at this single code line:

            myString.ConsumeChars<NC>( 5 );

a reader understands that string \c myString is at least \c 5 characters long.
This is valuable information that a reader otherwise found out only by inspecting
the context of the code line, which sometimes may become a quite complex task.
From here, one could easily conclude that after this code line, an \e invariant for variable
\c myString would be

            myString may be empty but is not nulled

To conclude this chapter, it has to be mentioned that in debug-compilations of the library,
the non-checking versions of the code still implement checks!
Exactly these conditions that are documented to be checked in the regular method versions are
checked.
If the check fails, debug assertions are raised by the non-checking method versions.
This approach and the concept of \e invariants go along very well:
If an invariant is \c false, the algorithm is considered wrong, and the code asserts.<br>
In release compilations, invoking non-checking method versions with a breach of a corresponding
\e invariant leads to undefined behavior (probably a process crash).

\note
  In the \https{C# and Java versions,alexworx.github.io/ALox-Logging-Library/index.html}
  of \alib, where such template methods are not available, still some non-checking methods are
  provided, but less than in the C++ implementation.
  In these languages, some methods exist twice with the same method base name and the
  non-checking version named with suffix <c>"_NC"</c>.


\I{################################################################################################}
## 6.3 String Constants ## {#alib_strings_details_constants}
\I{################################################################################################}
### 6.3.1 NULL_STRING ### {#alib_strings_details_constants_nullstring}
The following <c>constexpr</c> variables are defined in namespace #alib:
- \alib{NULL_STRING}, \alib{NULL_COMPLEMENT_STRING}, \alib{NULL_STRANGE_STRING},
  \alib{NULL_NSTRING}, \alib{NULL_WSTRING}, \alib{NULL_XSTRING}, and
- \alib{EMPTY_STRING}, \alib{EMPTY_COMPLEMENT_STRING}, \alib{EMPTY_STRANGE_STRING},
  \alib{EMPTY_NSTRING}, \alib{EMPTY_WSTRING} and \alib{EMPTY_XSTRING}.

Each simply represents a \e nulled, respectively an empty string.
The rationale for the provision of the \c nulled versions is purely to increase the readability
of the source code.
The following lines of code are equivalent in all respects:

        String  myString;
        String  myString= nullptr;
        String  myString= NULL_STRING;
        
With variable \alib{EMPTY_STRING} and its siblings things are a little more complicated: Here
the right C++ string literal has to be chosen.
This is achieved with the template type \alib{strings;StringConstantsTraits} and its specializations
for character types \alib{characters;nchar}, \alib{characters;wchar}, and \alib{characters;xchar}.
If a user of this library writes entities that are templated on the character type, then
the use of this helper-struct is advised.

### 6.3.2 CString Constants ### {#alib_strings_details_constants_cstrings}
Templated helper-struct \alib{strings;CStringConstantsTraits} provides static \c constexpr methods
for a few frequently used string constants.

While the methods can be explicitly accessed by providing the templated character type, in
addition, for each six character types a corresponding is variable given in namespace #alib.
For example, for member method \alib{strings;CStringConstantsTraits<TChar>::DefaultWhitespaces},
corresponding variables
- \alib{DEFAULT_WHITESPACES},
- \alib{COMPLEMENT_DEFAULT_WHITESPACES},
- \alib{STRANGE_DEFAULT_WHITESPACES},
- \alib{NDEFAULT_WHITESPACES},
- \alib{WDEFAULT_WHITESPACES}, and
- \alib{XDEFAULT_WHITESPACES}

are defined.

Same as with helper-struct \alib{strings;StringConstantsTraits} introduced in the previous chapter,
if a user of the library writes entities that are templated on the character type, the
use of helper-struct \b CStringConstantsTraits is advised.

\I{################################################################################################}
## 6.4 Debugging Strings ## {#alib_strings_details_debugging}

In some situations additional debug checking is helpful when working with \alib strings.
Among such situations are:
- Development of the library module itself.
- Development of types derived from \alib string types.
- Specializing template struct \alib{characters;ArrayTraits} or functor \alib{strings;AppendableTraits}
  to add support for user-defined string types or append operations to class \b %AString.
- External manipulation of \b AString buffer retrieved with method
  \alib{strings;TAString::VBuffer;AString::VBuffer}.
- The provision of external data buffers to class \b %AString.

In these and similar situations, it may be helpful to define preprocessor symbol
\ref ALIB_DEBUG_STRINGS. This symbol enables internal consistency checks with almost any method
invoked on string types. By default this feature is disabled, as it consumes quite a lot of run-time
performance. When string debugging is enabled, macro \ref ALIB_STRING_DBG_CHK can be used to check
the consistency of \alib string classes.

With string debugging, the string buffer allocated by class \b %AString is extended by 32 characters,
16 characters at the front and 16 characters at the end. A "magic" number is written in this
padding memory, and accidental (illegal) write operations across the borders of the allocated
space are detected.

Therefore, code that:
1. Uses method \alib{strings;TAString::SetBuffer;AString::SetBuffer} to set an external buffer, and
2. transfers responsibility to \alib by setting parameter \p{responsibility} of that method,

has to allocate the buffer passed accordingly.
This means the buffer has to be 32 characters larger than specified and the starting address of the
heap allocation has to be 16 characters before what parameter \p{extBuffer} points to.<br>
Such external buffer allocation should therefore be conditionally implemented using code selection
symbol \ref ALIB_DEBUG_STRINGS.

Further details of the built-in debug mechanisms are not documented.
Please refer to the source code of the \alib string classes, especially by investigating to code
locations that use selection symbol \ref ALIB_DEBUG_STRINGS.


\I{################################################################################################}
## 6.5 Signed String Length ## {#alib_strings_details_signedlength}
The string types introduced with this module are using type \alib{integer} to store the
string's length. This is a signed type - in contrast to what the C++ standard library suggests by
using type \c size_t for the length of the type \c std::string!

There are very good reasons to consider this as a wrong design decision. Negative string length
is impossible, and thus, this is an artificial, non-necessary restriction, because \alib strings
cannot be longer than only half of the virtually addressable memory (on standard hardware).

Honestly, the main argument for taking this restriction into account is to avoid a lot of clutter
code when it comes to subtraction of string length values.
\alib compiles with almost all reasonable compiler warnings enabled. Being signed, many static
casts for converting signed and unsigned integral values would be needed to avoid warnings.
This would not only be true in the library code itself, but with all code that uses the strings,
and that also uses a similar restrictive warning policy with compilation.

However, besides this confession of a certain level of laziness, there is also a true benefit in
this decision: Types derived from class \b String may use this unused sign bit, to encode a
binary piece of information in it.
As a sample, \alib class \alib{strings;TAString;AString} leverages this option already:
The information if a currently used buffer is of \alib{strings;TAString::SetBuffer;external or internal}
allocation is determined by storing a positive or negative value in the likewise signed field \b capacity.
This way, no additional boolean value is needed, which of course reduces the memory footprint of the class.


\I{################################################################################################}
# 7. Strings And Character Widths # {#alib_strings_width}

\I{################################################################################################}
## 7.1 String Literals ## {#alib_strings_width_literals}

As elaborated in the introductory chapter \ref alib_strings_intro_characters, in respect to
character type definitions and character array traits, this module completely relies
on module \alib_characters_nl. While all string classes are templated, the character types
that are used by the template instantiations are all defined in this underlying module.

The alias types for each string class (defined in namespace #alib) enumerate all possible types
by adding a prefix character or word, for example, \alib{NString}, \alib{WString} or
\alib{ComplementString}.
The aliases without any prefix, like \alib{String}, \alib{Substring} or \alib{AString} use the
width of the generic and "agnostic" type \alib{characters;character}.

Now, when using string literals, the following code is not platform agnostic::

        String myString= "Hello World";

While it might compile on some platforms or with the right compiler-symbols for \alib in place,
in the case that type \b character is a wide type, a compilation error is generated. Therefore,
all non-narrow string literals need to be given by using a corresponding macro. The set of macros
are also provided with the underlying module \alib_characters_nl. The "agnostic" macro
needed in the sample above is simply \ref A_CHAR "A_CHAR":

        String myString= A_CHAR( "Hello World" );

As long as only strings of standard width are used, all that is needed to know is that each
and every C++ string literal needs to be enclosed in this macro.

Further macros that define string literals of specific width are given with
- \ref A_CCHAR,
- \ref A_SCHAR,
- \ref A_NCHAR,
- \ref A_WCHAR and
- \ref A_XCHAR.

\see
- For more information on the macros, please consult chapter \ref alib_characters_prepro_literals
  of the \ref alib_mod_characters "Programmer's Manual" of module \alib_characters_nl.
- For information about how to change the default character width, see chapter
  \ref alib_characters_prepro "3. Type Selection And Character Literals".
- To really grasp all aspects of \alib characters and strings, of course both Programmer's
 Manuals should be reviewed. This might be a good point in time to do this.

\I{################################################################################################}
## 7.2 Platform-Independent Conversion ## {#alib_strings_width_pfi_conversion}
Sometimes a code unit expects a string of a defined width and has to handle strings of logical
types, or vice versa. For example, if an interface method accepts standard string type, while
internally narrow strings are used.

In such situations, the straight forward approach to this could be to use code selector
symbol \ref ALIB_CHARACTERS_WIDE and provide two different code versions.

To avoid this, the following macros are provided:

- \ref ALIB_STRINGS_TO_NARROW
- \ref ALIB_STRINGS_TO_NARROW_ARG
- \ref ALIB_STRINGS_TO_WIDE
- \ref ALIB_STRINGS_TO_WIDE_ARG
- \ref ALIB_STRINGS_FROM_NARROW
- \ref ALIB_STRINGS_FROM_NARROW_ARG
- \ref ALIB_STRINGS_FROM_WIDE
- \ref ALIB_STRINGS_FROM_WIDE_ARG

In principal the macros define a new identifier, which in the case that a conversion is needed,
uses a local string where the source string is appended, while in the case that the character widths
are equal, a simple reference to the given type is created. The latter will be optimized out
by a C++ compiler and thus, no performance penalty occurs.

For details, consult the reference documentation of the macros.

\I{################################################################################################}
# 8. String Utility Classes # {#alib_strings_util}
This user manual concentrates on the general and fundamental aspects of the string types provided
by this module.

There is a whole list of utility types available with this module that are not covered by this
manual. Instead, for those types an adequate and complete introduction and description is provided
with the reference documentation of each.
The types, for example, implement token parsing, a wildcard and regular-expression matcher.
To separate the fundamental string types from the utility classes, a dedicated inner namespace
\b "util" is defined where these classes are grouped.

To investigate into the functionality and tools offered in the area of string handling, please
consult the class list provided in the reference documentation of inner namespace
\ref alib::strings::util.

\I{################################################################################################}
# 9. C++20 std::format # {#alib_strings_stdformat}
With the inclusion of the header \implude{Compatibility.StdStrings}, specializations of
the traits \alib{characters;ArrayTraits} are given, which allow string types of the
C++ Standard Library to be implicitly or explicitly converted to and from different
\alib string types.
\note
  This was already explained in the previous chapter \ref alib_strings_cc_builtin.

In addition, this header makes \alib string types compatible with the C++20 function
<c>std::format</c>. And this is done in <b>three ways</b>, each covered by a next subsection.

\I{################################################################################################}
## 9.1 Using ALib String Types as Arguments to std::format # {#alib_strings_stdformat_args}
The type traits <c>std::formatter</c> is specialized for all \alib string types to enable
their use in formatting calls like this:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_STDFORMAT_ARGS

Due to their implicit conversion to class <c>std::string_view</c>, any \alib string type can also
be used as a format string.
The following sample uses a \alib{strings;TSubstring}:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_STDFORMAT_ARGS_FMT

\I{################################################################################################}
## 9.2 Using TAppend with std::format # {#alib_strings_stdformat_appendables}
In Chapter \ref alib_strings_assembly_ttostring it was demonstrated how custom types can be
made "appendable" to class TAString by specializing traits struct \alib{strings;AppendableTraits}.

Now, with the inclusion of the header file \implude{Compatibility.StdStrings} any of such
custom type can be used with <c>std::format</c>, without specializing struct <c>std::formatter</c>.
For this, the objects to format have to be wrapped in the special wrapper-type
\alib{strings::APPENDABLES;Appendable}.

\note
  Due to a limitation in C++, it isn’t possible to conditionally specialize \c std::formatter
  based on whether a specialization already exists.
  This means that if we tried to provide a generic specialization for types that have a
  specialization of \b AppendableTraits, it could conflict with the standard library’s
  specializations (e.g., for \c int).
  To avoid such ambiguities and ensure clear, unambiguous formatting behavior, it is required
  that types meant to work with \b AString be explicitly wrapped when used with std::format.

As a simple sample, we use \c std::format to write an \ref alib_enums_records "ALib Enum Record"
into a <c>std::string</c>:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_STDFORMAT_APPENDABLE
 

\I{################################################################################################}
## 9.3 Using std::format To Write Into Class AString # {#alib_strings_stdformat_into_astring}
With C++20 and the introduction of types found in the header <c>\<format\></c>, developers
get motivated to make their own types printable by specializing the type traits struct
<c>std::formatter</c> - just as we demonstrated for the \alib string types in the previous
section.
 
Such an effort is very similar to what class \alib{strings;TAString} provides with traits struct
\alib{strings;AppendableTraits} (see chapter \ref alib_strings_assembly_ttostring).

Because class \b TAString supports appending instances of type <c>std::string</c> the result
of C++20 formatting operations can right away be appended to a \b %TAString, as shown
in the following sample:
 
\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_STDFORMAT_APPEND_NAIVE
                 
While this might be done like this, it is very inefficient, because this creates an
anonymous intermediate instance of class <c>std::string</c>, which dynamically allocates
and deallocates memory, just to append the result of the format to the likewise dynamic
\alib type.

The following change of code prevents this:

\snippet "DOX_STRINGS.cpp"     DOX_STRINGS_STDFORMAT_APPEND_STDFORMAT

So, what is going on here? Let us walk through it step-by step:
- Templated struct \alib{strings::APPENDABLES;TStdFormat}, with its
  aliases \ref alib::StdFormat, \ref alib::NStdFormat and \ref alib::WStdFormat, is a small helper
  type defined in the header \implude{Compatibility.StdStrings}.
- The type is enabled to collect a format string together with a set of variadic template types,
  just as accepted by the function <c>std::format</c>.
- To allow comfortable construction of the type, the header in addition provides a
  \https{C++17 deduction guide,en.cppreference.com/w/cpp/language/class_template_argument_deduction}.
- Next, the header specializes the type traits struct \alib{strings;AppendableTraits}.
  Its implementation then calls <c>std::vformat_to</c> passing the \b AString as an output iterator.

With that, the unnecessary creation of a <c>std::string</c> is avoided, and any custom type
that has a specialization of <c>std::formatter</c> can be likewise formatted targeting an
\b AString with the same efficiency.

\I{################################################################################################}
# 10. String Formatting # {#alib_strings_format}
Almost any standard library of modern programming languages provides functionality that allows
formatting a list of variadic arguments using a format string that follows a certain
"placeholder syntax".
The most prominent sample is the good old \c printf function of the standard \b C library.

\alib offers mechanics to define and process variadic argument lists in a type-safe fashion
with its module \alib_boxing. Now, to keep module \alib_strings_nl independent of module
\alib_boxing_nl, formatting features as described above have been placed in a separated
module, namely \alib_format. With that, a powerful implementation of formatting tools
is provided. These are even supporting different standards of a format string's placeholder syntax,
namely \alib{format;FormatterJavaStyle;printf and Java style} as well as
\alib{format;FormatterPythonStyle;Python style}.

<br><br><br><br><br><br> */

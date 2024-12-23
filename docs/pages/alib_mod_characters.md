// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_mod_characters   ALib Module Characters - Programmer's Manual

\tableofcontents


 \I{################################################################################################}
# 1. Introduction # {#alib_characters_intro}

Module \alib_characters_nl is very foundational. The main goal of the module is not to provide
algorithms and functionality, but rather to provide clarity and consistency. Both are needed in
two related areas, where the C++ language itself turns out to be a little unclear and fighting with
legacy compatibility issues.<br>
The two areas are <b>"characters"</b> and <b>"character arrays"</b>.
The latter may also be called "character strings", however in the context of this module, the
more general term "array" is preferred.

As it is discussed in the next chapters, both areas, partly for historical reasons, partly due to
the abstract nature of C++, contain some of pitfalls and difficulties in respect to creating
compatible, platform-independent and secure software.

This module introduces own type definitions and "type traits" aiming to overcome the difficulties
while still using the same strictness and clarity that a programmer is used from the C++ language.

And here, the scope of the module already ends! Further functionality is only found with
separated modules like \alib_strings, \alib_boxing or \alib_basecamp.
We think that the combination of the modules together form an unrivalled team and no other
C++ library that we know of, makes character and string handling as convenient, seamless, compatible,
readable and flexible as these.


\I{################################################################################################}
# 2. Character Types # {#alib_characters_chars}
What is a "character" in computer science? What is a "character" for humans? Both questions
do not have simple answers. Both idioms underwent some historical development and change.

The good news is: Today, the challenge of representing characters of human written languages in computer
systems is at least well understood. But it is complex.

\I{################################################################################################}
## 2.1 The Challenges Of C++ Character Types ## {#alib_characters_chars_challenge}
The right definition of the character type creates a lot of confusion and platform dependencies.
This Programmer's Manual cannot elaborate in all depth on this topic. To make best and most
efficient use of this module \alib_characters_nl and those modules that depend on it, it is probably
enough what is written in this and the following manual sections.<br>
To get deep knowledge and to understand the rationals behind the design decisions of this library,
some links are provided here for the interested user:

- \https{ASCII (Wikipedia),en.wikipedia.org/wiki/ASCII)}
- \https{Unicode (Wikipedia),en.wikipedia.org/wiki/Unicode)}
- \https{UTF-8 ,en.wikipedia.org/wiki/UTF-8},
  \https{UTF-16,en.wikipedia.org/wiki/UTF-16},
  \https{UTF-32,en.wikipedia.org/wiki/UTF-32} (all Wikipedia)
- \https{UTF-8 Everywhere,utf8everywhere.org/}, a "manifesto" on the use of UTF-8 written by
  people that have a strong opinion. Absolutely worth to read.
- \https{ICU Library,site.icu-project.org/}. See the more general and introduction/tutorial
  parts of their documentation.

The following simple observations already indicate that choosing "the right" character width,
which then leads to an efficient implementation of C++ strings, is a non-trivial task:
1. GNU C++ and Clang compilers define \c wchar_t as a <b>4-byte</b> integral type under GNU/Linux.
2. Microsoft Visual Studio C++ compiler for Windows defines \c wchar_t as a <b>2-byte</b> integral type.
3. Famous and wide-spread \https{QT Class Library,www.qt.io} uses a <b>2-byte</b> integral type
   on both platforms, GNU/Linux and Windows OS.

While we have never seen a compiler doing this, the C++ language standard officially even allows
the size of \c wchar_t to be one byte wide.

\I{################################################################################################}
## 2.2 ALib Explicit Character Types ## {#alib_characters_chars_by_width}
In general, this \alib module differs between two ways of choosing the underlying character width
of a string.

The ways to choose are:
1. Explicit choice
2. Logical choice

The explicit choice of the character width is the less frequent approach and rather used
internally in the library and when it comes to interacting with system and 3rd-party libraries.
For this, type definitions
- \ref alib::nchar
- \ref alib::wchar

are provided. While \b nchar is always an identity type of C++ type \c char (hence a simple alias name),
type \c wchar might resolve to either of built-in types \c wchar_t, \c char16_t or \c char32_t.

Note that the "explicit choice of the character width" with type \b nchar and \b wchar, is very
similar to the built-in C++ choice of types \c char and \c wchar_t. The slight but crucial
difference is that the final definition of what a "wide character" is (a \b 2- or \b 4-byte integral),
is not made by the compiler/platform anymore.
Instead, the \alib library decides. This comprises the first step towards platform independence.


\I{################################################################################################}
## 2.3 ALib Logical Character Types ## {#alib_characters_chars_by_logic}

While the two explicit character types \alib{nchar} and \alib{wchar} are
aliasing two of the four built-in types (\c char, \c wchar_t, \c char16_t or \c char32_t.),
the two types themselves are aliased once more by two further type definitions:

- \ref alib::character and
- \ref alib::complementChar.

Depending on the compiler, the platform defaults and the compiler symbol
\ref ALIB_CHARACTERS_WIDE, one of these types aliases equals \b nchar and the other \b wchar.

This logical naming means that a \alib{character} is the default type for characters used with
\alib and type \ref alib::complementChar "complementChar" is just the counterpart.
It is recommended to just use the logical type \b character for all string operations
of a normal software entity. Its use expresses "I don't care, don't bother me!" and \alib will
help that even if string-types based on other characters come in, this is not even noticed
much in a user's code!

All together this results in a three-level type/alias scheme. For example, the
<b>default</b> on <b>GNU/Linux</b> platform, is:

       Logical Type                     Explicit Type               C++ Type
       --------------------------------------------------------------------------
       alib::character         <==>    alib::nchar     <==>       char
       alib::complementChar    <==>    alib::wchar     <==>       wchar_t

In contrast to this, under <b>Windows OS</b> using the Microsoft compiler the scheme is:

       alib::character         <==>    alib::wchar     <==>       wchar_t
       alib::complementChar    <==>    alib::nchar     <==>       char

The attentive reader now notices that an \b alib::character under Windows OS is not equivalent
to an alib::complementChar GNU/Linux, although both are aliasing C++ type <c>wchar_t</c>:
As explained above, this type is platform-dependent, hence might be 2-bytes or 4-bytes.
(Note that in theory, it might be even only 1-byte wide, which is not supported by \alib.)

To write software that is able to cope with just all possible character types, the two types
"narrow" and "wide" obviously are not sufficient. Therefore, this  \alibmod defines a \b third
logical character type, which holds this third possible width:

- \ref alib::strangeChar

This (strange!) name was chosen to express that this third type is neither compatible with
\alib narrow characters nor with \alib wide characters.
By default it is also neither equivalent to C++ \c char nor \c wchar_t. However, this might
be changed, as we will see later.

Now, just for completeness, this \e logical type has an \e explicit counterpart with \ref alib::xchar.
The underlying C++ type of logical type \b strangeChar and explicit type \b xchar are always the same.
Both types are identical.

We now have covered
- all three possible widths, namely \c 1, \c 2 and \c 4 bytes, in
- all three "type domains", namely <b>"ALib Logical"</b>, <b>"ALib Explict"</b> and <b>C++-Type</b>


With this, the \ref ALIB_CHARACTERS_SIZEOF_WCHAR "default" on GNU/Linux is:

       Logical Type                     Explicit Type               C++ Type
       -----------------------------------------------------------------------------------
       alib::character         <==>    alib::nchar     ==         char      (1 byte)
       alib::complementChar    <==>    alib::wchar    <==>        wchar_t   (4 bytes)
       alib::strangeChar        ==     alib::xchar    <==>        char16_t  (2 bytes)

On windows with Microsoft compiler:

       alib::character         <==>    alib::wchar    <==>        wchar_t   (2 bytes)
       alib::complementChar    <==>    alib::nchar     ==         char      (1 byte)
       alib::strangeChar        ==     alib::xchar    <==>        char32_t  (4 bytes)

Each table uses two times <em>'=='</em>. This should illustrate that the pairs of types:
1. \b nchar and \c char, and
2. \b strangeChar and \c xchar

are always synonyms. For all other pairs of types in the tables above (where symbol <em>'<==>'</em> is used)
this is never sure and depends on the compiler, platform and library compilation settings.

\note
  From this perspective, if mixing \alib characters with the character definition found in
  \https{QT Class Library,www.qt.io},  \c QChar is equivalent to logical type \alib{strangeChar}
  under GNU/Linux,  while it is equivalent to type \alib{character}
  on Windows OS. This is true, because on any platform, type \c QChar is defined to have a width of
  of 2 bytes!

While it is obvious that the default character type \ref alib::character is needed
to write platform-independent code, it might not be so simple to imagine a use case for types
\c complementChar and \c strangeChar.
Mostly those two are needed internally, but they are also invaluable when it
comes to template meta-programming with the aim of creating functionality that supports any type of
character or string types.
Samples of this will be seen in the Programmer's Manual of module \alib_strings.



\I{################################################################################################}
## 2.4 Character Type Summary ## {#alib_characters_chars_summary}
Let us quickly summarize what was said in this chapter about character types:
- Four C++ built-in types exist: \c char, \c char16_t or \c char32_t, \c wchar_t.<br>
  None of the four have a defined size in bits.
- Explicit definitions: Three alias types are provided for explicit choice:
   - \ref alib::nchar for narrow characters, always an alias to \c char, and
   - \ref alib::wchar for wide characters, not necessarily an alias to \c wchar_t.
   - \ref alib::xchar, for the third possible size that is neither narrow nor wide and can be
     considered as an <em>"incompatible wide character"</em>.

- Logical definitions: Three alias types are provided, which allow a logical choice:
   - \ref alib::character is the default character type chosen with compiling the library.
     This either resolves to \b nchar or \b wchar.
   - \ref alib::complementChar providing the complementary character to \b %character, hence
     it aliases \b wchar if \b character aliases \b nchar and vice versa.
   - \ref alib::strangeChar providing the third possible character width which is neither
     \b nchar nor \b wchar.

While these definitions might sound like introducing a next complexity level on top of the already
confusing situation that C++ alone creates, this approach proves to simplify character handling
a lot. Of course, this is only true if a programmer embraces these \alib definitions and starts
using type \b character everywhere instead of type \c char or \c wchar_t.

\I{################################################################################################}
\I{################################################################################################}
# 3. Type Selection And Character Literals # {#alib_characters_prepro}

\I{################################################################################################}
## 3.1 Changing The Library Defaults ## {#alib_characters_prepro_defaults}
While the main goal of this module is to enable a programmer to mostly forget about the character
width that she is using with her \alib enabled software, and furthermore to transparently
(aka without adding special code) convert external strings to the preferred width used, it may still
be a worth consideration which character width is chosen for the logical type \alib{characters;character}.

As it was explained in the previous section, the default for GNU/Linux and GCC is to use 1-byte
narrow \c char type, while the library's default choice with Microsoft compiler under Windows OS
is the using the built-in 2-byte \c wchar_t.

Now, if for example, a software mixes \alib with the \https{QT Class Library,www.qt.io}
to avoid a lot of (transparent) string conversions, it would be preferable to use the same 2-byte
wide character type that \b QT uses for its strings, independent of the platform that the software
is compiled at.

To achieve this, two compiler symbols may be passed:
1. \ref ALIB_CHARACTERS_WIDE<br>
   As its name indicates, this symbol controls the use of either narrow type \c char
   or any of the wide character types \c wchar_t, \c char16_t or \c char32_t as the default
   logical character type \alib{characters;character}.

2. \ref ALIB_CHARACTERS_SIZEOF_WCHAR<br>
   This symbol enables to control the width of wide characters to be \b 2- or \b 4-bytes.

\note
  With the current \alib platform implementations, by default, type \alib{characters;wchar}
  always corresponds to C++ type \c wchar_t. Type \alib{characters;xchar} consequently defaults
  to either \c char16_t or \c char32_t, just the one with the different width than \c wchar_t.<br>
  When the use of a compiler symbol changes the width of type \alib{characters;wchar} to
  be different to the width of \c wchar_t, then the assignment just changes: \b wchar becomes either
  \c char16_t or \c char32_t while \b xchar becomes \c wchar_t.




\I{################################################################################################}
## 3.2 Code Selection Symbols ## {#alib_characters_prepro_codeselect}
General purpose user code should not be affected on how the library is compiled in respect to
character width. Especially all \alibmod compile and run independent of the compiler
symbols introduced in the previous section. Furthermore, module \alib_strings (which of course
heavily relies on this module) provides various features to keep a users code clean and transparent
in respect to the character selection.

However, still in some situations, different code may be selected dependent on the library compilation
settings. For this, a few corresponding symbols are provided:

- \ref ALIB_CHARACTERS_WIDE<br>
  This symbol evaluates to \c false (\c 0) when type \alib{characters;character} is equivalent to
  C++ type \c char and to \c false (\c 1) if the type is a wide character.<br>
  This symbol may also be passed to the compiler to control the setting.

- \ref ALIB_CHARACTERS_SIZEOF_WCHAR<br>
  Provides the width of \alib type \alib{characters;wchar}.<br>
  The symbol is a counterpart to symbol \ref ALIB_SIZEOF_WCHAR_T which holds the compiler-dependent
  size of C++ type \c wchar_t.<br>
  This symbol may also be passed to the compiler to control the setting.

- \ref ALIB_CHARACTERS_NATIVE_WCHAR<br>
  This symbol evaluates to \c true (\c 1) when type \alib{characters;wchar} is equivalent to
  C++ type \c wchar_t and to \c false (\c 0) if the type is any of
  \c char16_t or \c char32_t and has a different width than \c wchar_t.


\I{################################################################################################}
## 3.3 Character And String Literals ## {#alib_characters_prepro_literals}

In C++, character and character string literals are enclosed in single, respectively double
quote characters. For character types of widths other than the single-byte type \c char,
in addition a correspondent prefix character <c>'L'</c>, <c>'u'</c> or <c>'U'</c> is needed.

Let us look at some samples:

\snippet "DOX_STRINGS.cpp"     DOX_CHARACTERS_LITERALS_1

While this code compiles, the sample is incorrect in so far, that an implicit character conversion
is performed with the assignment of variable \b wc. This is possible, as the compiler detects
that \b wchar_t is a wider integral than \b char.<br>
Therefore, we switch to string literals, which create zero-terminated character arrays:

\snippet "DOX_STRINGS.cpp"     DOX_CHARACTERS_LITERALS_2

Here, the prefix <c>'L'</c> must not be omitted to create a zero-terminated array of \c wchar_t,
because arrays of wider element size cannot be simply casted.
Now taking all C++ native character types into account, we are forced to also use prefixes
<c>'u'</c> and <c>'U'</c>:

\snippet "DOX_STRINGS.cpp"     DOX_CHARACTERS_LITERALS_3

We recall that \alib introduces six alias character types:
- Three "explicit" character types, namely \alib{characters;nchar}, \alib{characters;wchar} and \alib{characters;xchar}, and
- three "logical" character types, namely \alib{characters;character}, \alib{characters;complementChar} and \alib{characters;strangeChar}.

Apart from type \b nchar (which always is equivalent to built-in type \c char) we do not know what
underlying types are chosen - which of course is the goal of the whole exercise.
To define string literals, this module provides six corresponding preprocessor macros:

- Three for the explict character types: \ref A_NCHAR, \ref A_WCHAR and \ref A_XCHAR, and
- three for the logical character types: \ref A_CHAR, \ref A_CCHAR and \ref A_SCHAR.

Dependent on the definitions of the corresponding character types, the macros simply prepend the
right prefix character, or none in case of the narrow, 1-byte character.

\note
   - Macro \ref A_NCHAR is never adding a prefix, because the corresponding type \alib{characters;nchar} is always
     equivalent to type \c char. Therefore, its use is a matter of taste. Other \alibmods do not use it,
     because for a programmer, reading a string literal without a prefix or macro, it should be evident
     that a single byte string is created by the compiler.
   - As types \alib{characters;strangeChar} and \alib{characters;xchar} are always identical,
     macros \ref A_SCHAR and \ref A_XCHAR are synonyms.

With this in place, literals of the default character can easily be defined like this:

\snippet "DOX_STRINGS.cpp"     DOX_CHARACTERS_LITERALS_4


To conclude this section, this last snippet shows the use of all six macros. Instead of
constant character pointers, this sample uses the string classes introduced with separate module
\alib_strings. These classes are templated in respect to the character type and consequently,
six aliases types are offered:

\snippet "DOX_STRINGS.cpp"     DOX_CHARACTERS_LITERALS_5



\I{################################################################################################}
\I{################################################################################################}
# 4. Character Arrays # {#alib_characters_arrays}

The previous chapter talked about challenges that are caused by built-in character type definitions
of the C++ language, may it be due to legacy/compatibility reasons or due the language respecting
platform defaults and providing this general openness and flexibility.

Unfortunately, this chapter about C++ character arrays, again talks about challenges...

\I{################################################################################################}
## 4.1 The Challenges Of C++ Character Arrays ## {#alib_characters_arrays_challenge}

While this module is about "characters" and arrays of those, the term "string" is used
for character arrays likewise. And within the C++ language, this already imposes the
first irritating inconsistency:

        char arrayA[3]  {'A','B','C'};
        char arrayB[3]= "ABC";   // Error: initializing string too long

A C++ string literal always includes a terminating zero (<c>'\0'</c>) character.
Therefore, only a string with a length of \c 2, fits to the sampled array of length \c 3:

        char arrayA[3]  {'A','B','C'};
        char arrayB[3]= "AB";

Now, with the second array, the information about its size exists twice. The information about
whether one the two arrays is zero-terminated is non-existent.
Of course, we can convert both to a character pointer:

        char *cpA= arrayA;
        char *cpB= arrayB;

Now, the information about the array length is gone with the first array, with the second
it is preserved due to zero termination.<br>
Already with the following lines of code, we have undefined behavior:

        std::cout << "array A: " << cpA << std::endl;
        std::cout << "array B: " << cpB << std::endl;

While C++ is considered a type-safe language, only a few lines of code (that only do legal type
conversions), may lead to program crashes. The whole reason for this is that C++ aims to
keep compatible with older language versions and with the C-language, that many decades ago
proposed zero-terminated strings.

\note
  We, the makers of \alib, very much agree to the theses that the invention of zero-terminated
  strings during the previous millennium, was one of the
  \https{most expensive one-byte mistakes,queue.acm.org/detail.cfm?id=2010365} of IT-history!

A next irritating observation is about assigning string literals to const and non-const character pointers:

        const char*  pointer2= "AB";   // OK
        char*        pointer1= "AB";   // Warning: "ISO C++11 does not allow conversion from string literal to 'char *'"
        char         array[3]= "AB";   // OK(!)
        char*        pointer3= array;  // OK, this avoided the warning from above, without using an explicit cast!


Before we conclude, a last question: How do you detect the length of a C++ string literal provided
in an external macro, like this:

    #define STR_LIT  "A String that may change with the next library version"

The approach of most programmers would probably be:

    size_t length= strlen(STR_LIT);

Well, this is very inefficient, because it does not use the compiler's knowledge about the
length of the character array. There is a <c>"constexpr"</c> and hence zero-cost solution available:

    constexpr size_t length= std::extent<std::remove_reference<decltype(STR_LIT)>::type>::value - 1;

Let us summarize what types of "character arrays" are available in the C++ core language:

- Arrays of compile-time defined (fixed) size
- Pointers to arrays of unspecified size
- String literals which are both: pointers to arrays and arrays of fixed size. The compiler
  chooses their feature just as needed. And, in memory, the resulting strings become zero-terminated.

Besides that, there are tons of libraries available that define their own string types.

\I{################################################################################################}
## 4.2 Character Array Type Traits ## {#alib_characters_arrays_traits}

The concept of "type traits" in C++ is used to annotate types with attributes that can be
evaluated at compile type. To implement this, templated structs are used which are specialized
for the types in question, and these specializations provide compile-time information for a type.

With this \alibmod, type traits for character arrays are introduced. As described in the previous
section, the C++ language is very unclear about how a character array "looks like" and many class
libraries for this reason use their own lightweight or heavy string classes. The goal is to
be able to use different sorts of character arrays in a type-safe and transparent manner.

This module introduces struct \alib{characters;T_CharArray<TString,TChar>} which offers type
traits for types that "implement" character arrays.

As seen, the name of the template parameter that denotes the C++ type to provide type traits for is
<b>"TString"</b>. This name indicates that usually character array type traits are provided for
"string classes" (e.g., \c std::string or \c QString). Along these lines,  this documentation often
uses the verb "to implement" in respect to the relationship of type \p{TString} and character arrays.
This terminology may be misleading. Instead, it could also be that objects of type \p{TString}
"represent" a character array (e.g., in respect to C++ 17 class \c std::string_view) or that types
"contain" a character array, or even create and provide one only on request.

Besides template parameter \p{TString} that denotes the type to provide traits for, a second
template parameter \p{TChar} needs to be given to denote the character type (width) of the character
arrays that are "implemented" by
type \p{TString}.


\I{################################################################################################}
## 4.3  Array Data Access ## {#alib_characters_arrays_traits_access}

Specializations of \b T_CharArray need to define <c>static constexpr</c> field
\alib{characters::T_CharArray;Access} of enumeration \alib{characters;AccessType}.
In the non-specialized version, value \b NONE is given, which usually indicates that a type is not
an array-like type.
Precisely it means, that character array data cannot be accessed from instances of the type.

Specializations usually specify one of the three other values:

- \alib{characters::AccessType;Implicit}<br>
  This flag defines that a pointer to a character array and the array length are allowed to be
  received from mutable or constant instances of \p{TString}, in an implicit or explicit fashion.
  For example, if a class needed a character array for construction, an (overloaded) constructor might
  accept an argument of type \p{TString} and implicitly access its internal array.

- \alib{characters::AccessType;ExplicitOnly}<br>
  This flag indicates that the array buffer and length are allowed to be received from mutable or
  constant instances of \p{TString}, but only in an explicit fashion.
  For example, if a class needed a character array for construction, an (overloaded) constructor
  might accept an argument of type \p{TString} and access its internal array.
  However, such constructor had to be declared with keyword \c explicit.

- \alib{characters::AccessType;MutableOnly}<br>
  This flag indicates that the array buffer and length are allowed to be received only from
  mutable instances of \p{TString}. For example, this flag may be used for types that need to
  perform non-constant preparations before allowing access to the character array of an instance.


If one of the three values is given with a specialization, two static methods need to be defined
which implement the type-specific access to the character array:
- \alib{characters;T_CharArray::Buffer} and
- \alib{characters;T_CharArray::Length}

While in the documentation of the two methods, the parameter \p{src} of the static access methods is of type
<c>const TString</c>, in the case of using access flag \alib{characters::AccessType;MutableOnly},
the method has to be defined using a mutable reference to \p{TString}.

\I{################################################################################################}
## 4.4 Custom Object Creation From Array Data ## {#alib_characters_arrays_traits_construct}

A second <c>static constexpr</c> member that a specialization of struct \b %T_CharArray
needs to define, is field \alib{characters::T_CharArray;Construction}.
It determines whether and how an instance of type \p{TString} may be created from existing
character array data.

The default value (the one given in the non-specialized struct) is \alib{characters::ConstructionType;NONE},
which determines that objects of type \p{TString} cannot be created from arrays.
Specialization here may provide:

- \alib{characters::ConstructionType;Implicit}<br>
  Allows implicit or explicit creation of values of \p{TString} from array-like objects.
- \alib{characters::ConstructionType;ExplicitOnly}<br>
  Allows creation of values of \p{TString} from array-like objects, but only in an explicit
  fashion.

If any of the two values is set, static method \alib{characters;T_CharArray::Construct} has to be
provided with the specialization of the struct. The implementation of this method needs
to create a value of type \p{TString} from the character array provided with the method's arguments.

\note
  A reader might wonder how type traits might be used to influence whether construction of
  objects might be performed implicitly or explicitly, especially in the case of third party
  classes residing a library.<br>
  This irritation will be resolved with module \alib_strings.
  As a quick outlook: \alib string classes define up to four templated cast operators: the
  permutation of implicit and explicit and normal and zero-terminated character array construction
  options. To have the compiler select one (or none) of the four operators, the character
  array traits information is used. This way, the traits can very well "decide" about if an
  external 3rd-party string type is created implicitly or not. In the case of explicit construction,
  an <em>explicit cast statement</em> has to be given.


\I{################################################################################################}
## 4.4 Type Traits For Zero-Terminated Character Array ## {#alib_characters_arrays_traits_zt}

The type traits template struct \alib{characters;T_CharArray<TString,TChar>} introduced in the
previous section is used to answer questions like:
- Is type \p{TString} implementing (or representing or containing, etc.) an array of character
  type \p{TChar}?
- How can the array data be accessed?
- May objects of type \p{TString} be constructed from character arrays? If yes, how?

And if positive answers to such questions are given, static methods
- \alib{characters;T_CharArray::Buffer},
- \alib{characters;T_CharArray::Length} and
- \alib{characters;T_CharArray::Construct}

are to be provided along with the specialization to implement the array access, respectively
object construction.

With sibling struct \alib{characters;T_ZTCharArray<TString,TChar>}, the same compile-time information
and method implementations are provided for <b>zero-terminated</b> character arrays.
Apart from the prefix <b>"ZT"</b> in the TMP struct's name, which stands for "zero-terminated",
all rules for specializations are the very same.

Of course, a type \p{TString} that represents a zero-terminated array type may (and should)
specialize both templated traits structs. Hereby it might use different flags and implementations
of the static methods for simple character arrays and for zero-terminated ones.

\I{################################################################################################}
## 4.5 Helper Macros To Specialize The Traits Structs ## {#alib_characters_arrays_traits_macros}
The following C++ preprocessor macros are defined by this module to support the correct specialization
of template traits struct \b T_CharArray:

- \ref ALIB_CHARACTER_ARRAY
- \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER
- \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH
- \ref ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE
- \ref ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE
- \ref ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT

Again, an equal set of macros is defined to support the correct specialization of template traits
struct \b T_ZTCharArray:

- \ref ALIB_CHARACTER_ZT_ARRAY
- \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER
- \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH
- \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE
- \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE
- \ref ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT

\I{################################################################################################}
## 4.6 Tool Struct TT_CharArrayType  ## {#alib_characters_arrays_traits_tool_arraytype}

The fact that the C++ does not provide a distinct "character" type, implies that traits structs
\alib{characters;T_CharArray} and \alib{characters;T_ZTCharArray} have the character type
\p{TChar} as a second template parameter that has to be named along with the main parameter \p{TString}.<br>
As an example, for type

        std::string

which is an alias to

        std::basic_string<char>

the specialization of \b T_CharArray will be:

        T_CharArray<std::string, char>

This sometimes imposes a little complication, at the moment that code wants to selectively compile
based on the information that a type implements just any sort of character array, instead of a
distinct type.

For this, helper-struct \alib{characters;TT_CharArrayType} is provided. Its inner
type definition \alib{characters::TT_CharArrayType;TChar} provides the character type that a
given type \p{TString} implements an array for. With that, for example to test the array access
type of \p{TString} without knowing (or caring) about the character type of the array that is
accessed, expression

        T_CharArray<TString, typename TT_CharArrayType<TString>::TChar>::Access

can be used.

With sibling helper-struct \alib{characters;TT_ZTCharArrayType}, the same is provided for traits
struct \b T_CharArray.

\I{################################################################################################}
\I{################################################################################################}
# 5. Built-In Character Array Traits # {#alib_characters_builtintraits}

As stated before, this module \alib_characters_nl, has a very foundational nature and
does not provide algorithms and functionality, but rather type definitions and type traits.
The rationale for this is that several modules of \alib, especially \alib_strings and
\alib_boxing, independently of each other benefit from the foundation provided here.
Consequently, this module had to be independent of each.

However, in the context of a custom set of string classes, the meaning of the array type traits is
much more easy to understand. As this chapter provides information about which built-in specializations
of the traits structs are provided (and why), we quickly want to anticipate what is found
with separated module \alib_strings:

- A type \alib{strings;TString<TChar>;String} representing a (not zero-terminated) character string.<br>
  For custom types \p{TString}, this string class tests flags in specializations of
  \alib{characters;T_CharArray;T_CharArray<TString>}:
    - This string type is implicitly constructible by objects of \p{TString}, in case that flag \alib{characters;AccessType::Implicit} is set.
- This string type is explicitly constructible by objects of \p{TString}, in case that flag \alib{characters;AccessType::ExplicitOnly} is set.
- This string type is explicitly constructible by mutable objects of \p{TString}, in case that flag \alib{characters;AccessType::MutableOnly} is set.
- Objects of type \b %String are implicitly casted to objects of type \p{TString}, in case that flag \alib{characters;ConstructionType::Implicit} is set.
- Objects of type \b %String may be explicitly casted to objects of type \p{TString}, in case that flag \alib{characters;ConstructionType::ExplicitOnly} is set.

In addition to that, type \alib{strings;TCString<TChar>;CString} represents a
<b>zero-terminated</b> character string.
The exact same rules apply to this class, but it is using the flags in specialization of \alib{characters::T_ZTCharArray} instead.
Both string types, \b %String and \b %CString, are very lightweight. Both do not manage a character
string array, they neither allocate or deallocate memory. All they do is the provision of two
data elements, the pointer to the array and the length of the string. As such, they are similar
to classes <c>std::string_view</c> introduced with C++ 17.<br>
A third type, class \alib{strings;TAString<TChar>;AString} implements a "heavy weight" string,
namely one that manages its own allocated character buffer. While the character arrays of this
class are not zero-terminated by default, the class always reserves space for a zero-termination
character. This allows interface method \alib{strings;TAString<TChar>::Terminate;AString::Terminate}
to be defined as a constant operation. Likewise types \b String and \b CString, the class provides
cast operators (which may terminate the internal buffer) and allow the concatenation of
string-like objects, again using the type traits.

The combination of all of this, provides the huge gain that comes with using character array traits:
Objects of any string type, may it be C++ string literals, <c>std::string</c> objects, \alib strings
or any custom 3rd-party library string type like \https{QString,www.qt.io}, <b>become seamlessly interchangeable!</b>
For example, if implicit access is allowed for an external string type \b XYZString and a method expects a constant \alib string
reference, such method can be invoked with objects of type \b XYZString seamlessly, meaning without
any explicit conversions. The other way round, if a third party method expects such constant reference
of its \b XYZString, \alib strings might be passed without explicit conversion.

With this in mind, the following subchapters provide information about the built-in array
specializations of this module.

\I{################################################################################################}
## 5.1 C++ Types ## {#alib_characters_builtintraits_cpp}

There are probably four different built-in C++ character array types:
1. Fixed-length character arrays.
2. Constant character pointers.
3. Mutable character pointers.
4. String literals.

The reason why we separate constant from mutable character pointers is given in the previous chapter
\ref alib_characters_arrays_challenge. In short: String literals can be assigned to constant character
pointers, but not to mutable ones.

The compiler treats string literals as either constant character pointers or
fixed length arrays, dependent on the context. The advantage of the latter is that their length is
available at compile-time. As they are not distinguishable from the other types, the number of
built-in types is reduced to three.

The following table lists the access and construction traits for the three types:

|Type                |     Character Arrays                               | Zero-Terminated C. Arrays                          |Notes
|--------------------|----------------------------------------------------|----------------------------------------------------|-----
|<c>TChar[N]    </c> | Implicit&nbsp;Access<br>No&nbsp;Construction       | Implicit&nbsp;Access<br>No&nbsp;Construction       | The decision to allow implicit access even in the case of zero-terminated arrays (which probably are \b not zero-terminated), lies in the fact that C++ string literals are fixed length arrays which are zero-terminated. Of course, this implies that some care has to be taken when using "real" C++ character arrays with \alib strings. This conflict is unavoidable due to the C++ language definition.
|<c>const TChar*</c> | Implicit&nbsp;Access<br>Explicit&nbsp;Construction | Implicit&nbsp;Access<br>Implicit&nbsp;Construction | Constant character pointers in C++ are "presumably" zero-terminated. This is this at least this librarie's interpretation of the C++ language standard. And this how most operating system's API calls expect a string value. Therefore, the type traits allow implicit access. Note that for the determination of the array length, <c>const TChar*</c> arrays have to be zero-terminated!<br>For the same reason, construction (here: conversion to) from non-zero-terminated arrays has to be performed explicitly, while construction from zero-terminated arrays is implicitly possible.
|<c>TChar*      </c> | Explicit&nbsp;Access<br>Explicit&nbsp;Construction | Explicit&nbsp;Access<br>Explicit&nbsp;Construction | Mutable character pointers in C++ are "presumably" \b not zero-terminated. Therefore, the type traits demand explicit access and code that explicitly uses a mutable character pointer with method that use character traits (like the corresponding explicit constructor of class \alib{strings;TString<TChar>;String} does), needs to ensure that the array is zero-terminated.<br>Mutable character pointers should not be used in the context of character array processing. Therefore, conversions to this type is possible only in an explicit fashion.

\note
  Confusion might occur in respect to zero-termination: It is only possible to access character array data
  represented by a plain character pointer (mutable or constant), if the array is zero-terminated.
  This is independent of whether a zero-terminated array is requested or not!<br>
  For example, pointers to character arrays are allowed to be passed to the constructor of class
  \alib{strings;TString<TChar>;String} only if the array provided is zero-terminated, even while
  class \b %String does not use zero-termination itself. This is because the determination of the
  array length, which is performed by specialized methods \alib{characters::T_CharArray;Length}
  of TMP structs \alib{characters;T_CharArray} and \alib{characters;T_ZTCharArray}, is searching
  for the termination character.<br>
  In other words: passing a pointer to a non-zero-terminated character array to the constructor
  of class \b %String is <b>undefined behavior!</b><br>
  All that the character array traits in respect to plain pointer types can do is to provide a
  hint about whether it could be expected that a pointer points to a zero-terminated array or not.
  The recommendation of avoiding the use of plain pointer types remains the same.
  The character array type traits introduced by this \alibmod_nl cannot fix the general problem.
  It just eases the use of pointers in unavoidable cases, for example with the operating system API
  calls or processing the zero-terminated string arguments of function <c>main()</c>.

Unlike the default definitions for types of the standard C++ library and other 3rd-party types
(as documented in the next chapters), the built-in definitions for these three types are not
selected by the inclusion of an optional header file. Instead, these definitions are fixed
and not customizable.

\I{################################################################################################}
## 5.2 Standard C++ Library Types ## {#alib_characters_builtintraits_std}
\note
  The built-in (default) specializations of the character array traits structs for the string types
  of the C++ standard library are optional and available with the inclusion of header file
  \c alib/compatibility/chararray_std.hpp.


The C++ standard library provides templated class <c>std::basic_string<TChar></c> which implements
a heavy weight string (aka a string type that allocates heap memory for the string data).
The corresponding lightweight class, <c>std::basic_string_view<TChar></c> is only available with C++
version 17. In addition, character arrays are implemented with class \c std::vector<TChar>.

The usual approach of this module is to allow implicit creation of lightweight string classes
from character arrays, while heavy-weight string types need to be created explicitly.
However, this approach is not possible here: At the moment that C++ 17 is active and this way class
\c std::string_view becomes available, the standard library performs some TMP code that allows
implicit creations of type \c std::string with internal conversions using the lightweight type!
Hence, the implicit creation cannot be avoided if in parallel we want to allow type
\c std::string_view to be implicitly created. Because of this, the definition is made as follows:
- With C++ version 17 or higher in place, the conversion of character arrays to type \c std::string
  is defined explicit, because otherwise an ambiguity would occur. However, effectively implicit
  creation is allowed due to the TMP programming of the standard library.
- With C++ version 11 or 14 (when \c std::string_view is not in place), the creation of
  class \c std::string from character arrays is defined to be implicit.
  This is done in favor to have compilations of the library behave compatible independent
  from the availability of the type \c std::string_view.
- Creation of type \c std::vector<TChar> is defined to be explicit.


In respect to character access of non zero-terminated arrays, all three classes allow implicit access.
Access to a zero-terminated character array is still implicitly done with type <c>std::string</c>
as this class terminates their buffer anyhow when it is accessed with method \c std::string::data().
For classes \c std::string_view and \c std::vector, such access is to be made explicitly, as usually
these types do not represent zero-terminated strings.

The definitions for the three types is summarized in the following table:

|Type             |  Character Arrays                               | Zero-Terminated C. Arrays
|-----------------|-------------------------------------------------|-------------------------------------------------
|\c std::string_view | Implicit&nbsp;Access<br>Implicit&nbsp;Construction | Explicit&nbsp;Access<br>Implicit&nbsp;Construction
|\c std::string      | Implicit&nbsp;Access<br>Explicit&nbsp;Construction (Effectively becomes implicit due to the TMP implementation of the type itself)   | Implicit&nbsp;Access<br>Explicit&nbsp;Construction (Effectively becomes implicit due to the TMP implementation of the type itself)
|\c std::vector      | Implicit&nbsp;Access<br>Explicit&nbsp;Construction | Explicit&nbsp;Access<br>Explicit&nbsp;Construction


\note
  Of course, corresponding specializations of TMP structs \b T_CharArray and \b T_ZTCharArray
  are made for the three classes and for all three character widths, \alib{nchar}, \alib{wchar} and \alib{xchar}.
  Therefore, altogether 18 specializations are made.



\I{################################################################################################}
## 5.3 Third-Party Types ## {#alib_characters_builtintraits_3rdparty}

\I{################################################################################################}
### 5.3.1 QT String Types ### {#alib_characters_builtintraits_3rdparty_qt}

\note
  Character array traits for types of the \https{QT Class Library,www.qt.io} is optional
  and available with the inclusion of header file \c alib/compatibility/chararray_qt.hpp.<br>
  The rationale for choosing these traits definitions are similar to those discussed in the
  previous chapters and therefore not repeated here.


The following character array type traits are made for string and character array
types of the \https{QT Class Library,www.qt.io}:

|Type             |  Character Arrays                               | Zero-Terminated C. Arrays
|-----------------|-------------------------------------------------|-------------------------------------------------
|\c QStringView   | Implicit&nbsp;Access<br>Implicit&nbsp;Construction | Explicit&nbsp;Access<br>Implicit&nbsp;Construction
|\c QString       | Implicit&nbsp;Access<br>Explicit&nbsp;Construction | Implicit&nbsp;Access<br>Explicit&nbsp;Construction
|\c QLatin1String | Implicit&nbsp;Access<br>Implicit&nbsp;Construction | Explicit&nbsp;Access<br>Implicit&nbsp;Construction
|\c QByteArray    | Implicit&nbsp;Access<br>Explicit&nbsp;Construction | Explicit&nbsp;Access<br>Explicit&nbsp;Construction
|\c QVector<uint> | Implicit&nbsp;Access<br>Explicit&nbsp;Construction | Explicit&nbsp;Access<br>Explicit&nbsp;Construction

If module \alib_strings is included in the \alibdist, in addition to these type traits, the
inclusion of header file alib/compatibility/chararray_qt.hpp provides a specialization of
\alib{strings;T_Append} for \b QT type \b QChar.


\I{################################################################################################}
\I{################################################################################################}
# 6. Helper-Types # {#alib_characters_helpers}

In addition to the type definitions, type traits structs and helper-struct
\alib{characters;TT_CharArrayType} discussed in detail in the previous manual chapters, the
types listed below are available with this module.

Please consult the reference documentation of these types for more information:

- \alib{characters;TT_CharOfSize}
- \alib{characters;TT_IsChar}
- \alib{characters;TT_ComplementChar}
- \alib{characters;AlignedCharArray}

\I{################################################################################################}
# 7. Namespace Functions # {#alib_characters_nsfuncs}

On the level of namespace alib::characters, a bunch of functions are implemented that provide 
common algorithms working on arrays of arbitrary character types. 
All functions have templated type \p{TChar}, which in most cases is deduced by the compiler
and thus does not need to be given.     

The functions are similar to what is found with traits struct 
\https{std::char_traits,en.cppreference.com/w/cpp/string/char_traits}. 
While some functions are just inline wrappers to specializations of this this struct, 
versions that do no exist in the standard were added. 

\I{################################################################################################}
# 8. Class AlignedCharArray # {#alib_characters_alginedchararray}

Finally this small but very important \alibmod_nl introduces class 
\alib{characters;AlignedCharArray}. 
Please refer to its reference documentation for further information about it. 

<br><br><br><br><br><br> */

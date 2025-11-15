//==================================================================================================
/// \file
/// This header-file is part of module \alib_characters of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace characters {

//##################################################################################################
// Narrow type: nchar
//##################################################################################################

/// This type represents a narrow character in \alib. This is an alias for built-in C++
/// type \c char.
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using nchar=    char;

//##################################################################################################
// Wide types: wchar, xchar
//##################################################################################################


#if DOXYGEN
/// This type represents a wide character in \alib.
/// Usually this is an alias for built-in C++ type \c wchar_t.
/// As the width of the latter is compiler-specific (the width may vary with different compilers even
/// on the same platform), the compiler-symbol \ref ALIB_CHARACTERS_SIZEOF_WCHAR may be used to
/// manipulate its width.
///
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using wchar=    PLATFORM_SPECIFIC;

/// This type represents a second wide character type which has a width complementary to
/// that of type \alib{characters;wchar}: If \b %wchar is 2 bytes wide, then this type aliases
/// \c char32_t and if its width is 4 bytes, this type aliases \c char16_t.
///
/// While together with types \ref alib::nchar and \ref alib::wchar it forms
/// the group of "explicit character types", it is always identical to logical type
/// \ref alib::strangeChar.
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using xchar     =  PLATFORM_SPECIFIC;

#else // !DOXYGEN

#   if ALIB_CHARACTERS_SIZEOF_WCHAR == ALIB_SIZEOF_WCHAR_T
#      if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
           using wchar     =  wchar_t;
           using xchar     =  char32_t;
#      else
           using wchar     =  wchar_t;
           using xchar     =  char16_t;
#      endif
#   else
#      if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
           using wchar     =  char16_t;
           using xchar     =  wchar_t;
#      else
           using wchar     =  char32_t;
           using xchar     =  wchar_t;
#      endif
#   endif


#endif // !DOXYGEN


//##################################################################################################
// Logical types: character, complementChar and strangeChar
//##################################################################################################

#if DOXYGEN
/// This type represents a standard \alib character.
/// The width (size) of a character is dependent on the platform and compilation flags.
///
/// This is why this type is called a "logical" type. The two other logical character types are
/// #complementChar and #strangeChar.
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using character= PLATFORM_SPECIFIC;

/// This type represents a non-standard \alib character.
/// If \ref alib::character is defined to implement a narrow character type, then this type
/// implements a wide character and vice versa.
/// Note, that the width of a wide character is not defined as well. It might be \c 2 or \c 4
/// bytes wide.
///
/// This type is called a "logical" type. The two other logical character types are
/// #character and #strangeChar.
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using complementChar= PLATFORM_SPECIFIC;

/// Besides types \ref alib::character and \ref alib::complementChar, this is the third
/// logical character type defined by \alib.<br>
/// independent of compiler defaults and optional the compiler-symbols provided, this type always
/// is equivalent to type \alib{characters;xchar}
///
/// \see
///   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
///   Programmer's Manual of module \alib_characters_nl.
using strangeChar= PLATFORM_SPECIFIC;

#define ALIB_CHAR_TYPE_ID_CHARACTER
#define ALIB_CHAR_TYPE_ID_COMPLEMENT
#define ALIB_CHAR_TYPE_ID_STRANGE


#elif !ALIB_CHARACTERS_WIDE
    using character=        nchar;
    using complementChar=   wchar;
    using strangeChar=      xchar;
#else
    using character=        wchar;
    using complementChar=   nchar;
    using strangeChar=      xchar;
#endif
} // namespace alib[::character]


/// Type alias in namespace \b alib.
using  nchar         = characters::nchar;

/// Type alias in namespace \b alib.
using  wchar         = characters::wchar;

/// Type alias in namespace \b alib.
using  xchar         = characters::xchar;

/// Type alias in namespace \b alib.
using  character     = characters::character;

/// Type alias in namespace \b alib.
using  complementChar= characters::complementChar;

/// Type alias in namespace \b alib.
using  strangeChar   = characters::strangeChar;

} // namespace [alib]

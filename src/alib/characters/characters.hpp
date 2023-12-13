/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_characters of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CHARACTERS_CHARACTERS
#define HPP_ALIB_CHARACTERS_CHARACTERS 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_MODULE(CHARACTERS)

#if !defined(HPP_ALIB_TOOLS) && !defined(ALIB_DOX)
#   include "alib/lib/tools.hpp"
#endif

#if !defined(HPP_ALIB_FEATURES) && !defined(ALIB_DOX)
#   include "alib/lib/features.hpp"
#endif

#if !defined(HPP_ALIB_TMP) && !defined(ALIB_DOX)
#   include "alib/lib/tmp.hpp"
#endif

#if !defined(HPP_ALIB_INTEGERS)
#   include "alib/lib/integers.hpp"
#endif

#if !defined (_GLIBCXX_STRING) && !defined(_STRING_)
#   include <string>
#endif


//##################################################################################################
// Compiler Symbols
//##################################################################################################

#if !defined(ALIB_CHARACTERS_WIDE)
#   if defined(_MSC_VER)
#      define ALIB_CHARACTERS_WIDE 1
#   else
#      define ALIB_CHARACTERS_WIDE 0
#   endif
#endif

#if !defined(ALIB_CHARACTERS_SIZEOF_WCHAR)
#   define ALIB_CHARACTERS_SIZEOF_WCHAR   ALIB_SIZEOF_WCHAR_T
#elif  (ALIB_CHARACTERS_SIZEOF_WCHAR != 2) && (ALIB_CHARACTERS_SIZEOF_WCHAR != 4 )
#   error "Illegal value for symbol ALIB_CHARACTERS_SIZEOF_WCHAR given. Allowed is 2 or 4."
#endif


namespace aworx { namespace lib { namespace characters {


// #################################################################################################
// Narrow type: nchar
// #################################################################################################

/**
 * This type represents a narrow character in \alib. This is an alias for built-in C++
 * type \c char.
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using nchar=    char;

// #################################################################################################
// Wide types: wchar, xchar
// #################################################################################################
#if defined(ALIB_CHARACTERS_NATIVE_WCHAR)
#   error "Preprocessor symbol ALIB_CHARACTERS_NATIVE_WCHAR must not be passed to the compiler. It is deduced in ALib headers."
#endif



#if defined(ALIB_DOX)
/**
 * This type represents a wide character in \alib.
 * Usually this is an alias for built-in C++ type \c wchar_t.
 * As the width of the latter is compiler-specific (the width may vary with different compilers even
 * on the same platform), compiler symobls \ref ALIB_CHARACTERS_SIZEOF_WCHAR may be used to
 * manipulate its width.
 *
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using wchar=    PLATFORM_SPECIFIC;

/**
 * This type represents a second wide character type which has a width complementary to
 * that of type \alib{characters,wchar}: If \b %wchar is 2 bytes wide, then this type aliases
 * \c char32_t and if its width is 4 bytes, this type aliases \c char16_t.
 *
 * While together with types \ref aworx::nchar and \ref aworx::wchar it forms
 * the group of "explicit character types", it is always identical to logical type
 * \ref aworx::strangeChar.
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using xchar     =  PLATFORM_SPECIFIC;

#else // !defined(ALIB_DOX)

    #define A_NCHAR(STR)  STR


    #if ALIB_CHARACTERS_SIZEOF_WCHAR == ALIB_SIZEOF_WCHAR_T

    #   define ALIB_CHARACTERS_NATIVE_WCHAR 1

    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
            using wchar     =  wchar_t;
            using xchar     =  char32_t;
            #define A_WCHAR(STR)  L ## STR
            #define A_XCHAR(STR)  U ## STR
            #define A_SCHAR(STR)  U ## STR
    #   else
            using wchar     =  wchar_t;
            using xchar     =  char16_t;
            #define A_WCHAR(STR)  L ## STR
            #define A_XCHAR(STR)  u ## STR
            #define A_SCHAR(STR)  u ## STR
    #   endif

    #else

    #   define ALIB_CHARACTERS_NATIVE_WCHAR 0

    #   if ALIB_CHARACTERS_SIZEOF_WCHAR == 2
            using wchar     =  char16_t;
            using xchar     =  wchar_t;
            #define A_WCHAR(STR)  u ## STR
            #define A_XCHAR(STR)  L ## STR
            #define A_SCHAR(STR)  L ## STR
    #   else
            using wchar     =  char32_t;
            using xchar     =  wchar_t;
            #define A_WCHAR(STR)  U ## STR
            #define A_XCHAR(STR)  L ## STR
            #define A_SCHAR(STR)  L ## STR
    #   endif

    #endif


#endif // !defined(ALIB_DOX)


// #################################################################################################
// Logical types: character, complementChar and strangeChar
// #################################################################################################

#if defined(ALIB_DOX)
/**
 * This type represents a standard \alib character.
 * The width (size) of a character is dependent on the platform and compilation flags.
 *
 * This is why this type is called a "logical" type. The two other logical character types are
 * #complementChar and #strangeChar.
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using character= PLATFORM_SPECIFIC;

/**
 * This type represents a non-standard \alib character.
 * If \ref aworx::character is defined to implement a narrow character type, then this type
 * implements a wide character and vice versa.
 * Note, that the width of a wide character is not defined as well. It might be \c 2 or \c 4
 * bytes wide.
 *
 * This type is called a "logical" type. The two other logical character types are
 * #character and #strangeChar.
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using complementChar= PLATFORM_SPECIFIC;

/**
 * Besides types \ref aworx::character and \ref aworx::complementChar, this is the third
 * logical character type defined by \alib.<br>
 * Independent from compiler defaults and optional compiler symbols provided, this type always
 * is equivalent to type \alib{characters,xchar}
 *
 * \see
 *   For details, see chapter \ref alib_characters_chars "2. Character Types" of the
 *   Programmer's Manual of module \alib_characters_nl.
 */
using strangeChar= PLATFORM_SPECIFIC;

#elif !ALIB_CHARACTERS_WIDE
    using character=        nchar;
    using complementChar=   wchar;
    using strangeChar=      xchar;

    #define A_CHAR(STR)     STR
    #define A_CCHAR(STR)    A_WCHAR(STR)
#else
    using character=        wchar;
    using complementChar=   nchar;
    using strangeChar=      xchar;

    #define A_CHAR(STR)     A_WCHAR(STR)
    #define A_CCHAR(STR)    STR

#endif

// #################################################################################################
// T_CharArray and T_ZTCharArray
// #################################################################################################

/**
 * Enumeration of possible values for field \b %Access used with traits types
 * \alib{characters,T_CharArray} and \alib{characters,T_ZTCharArray}.
 * The elements of this enumeration indicate if and how the data of an
 * (non-zero terminated, respectively zero terminated) array-like type may be accessed.
 */
enum class AccessType
{
    /**
     * Data may not be received. This value usually indicates that a custom type does not implement
     * a character array at all. Hence, this is the default value of the non-specialized versions of
     * \alib{characters,T_CharArray} and \alib{characters,T_ZTCharArray}.
     *
     */
    NONE,

    /** Allows implicit (and explicit) access of the character array data from mutable or constant
     *  objects. */
    Implicit,

    /** Allows explicit access of the character array data from mutable or constant objects. */
    ExplicitOnly,

    /** Allows explicit access of the character array data from mutable objects. */
    MutableOnly,
};

/**
 * Enumeration of possible values for field \b %Construction used with traits types
 * \alib{characters,T_CharArray} and \alib{characters,T_ZTCharArray}.
 * The elements of this enumeration indicate if and how values an array-like type may be
 * constructed from a (non-zero terminated, respectively zero terminated) character array.
 */
enum class ConstructionType
{
    /**
     * The custom type may not be constructed from character arrays.
     * This is the default value of the non-specialized versions of \alib{characters,T_CharArray}
     * and \alib{characters,T_ZTCharArray} but may very well be used with specializations as well.
     */
    NONE,

    /** Allows implicit (and explicit) construction of objects from character array data. */
    Implicit,

    /** Allows explicit construction of objects from character array data. */
    ExplicitOnly,

};

/** ************************************************************************************************
 * This type provides type traits for character arrays. Specializations of this struct for
 * a custom type \p{TString}, expose information about that type representing a character array
 * and how the array data may be accessed.<br>
 * In addition, information about how the type may be constructed from character array data
 * may be provided with specializations of this type.
 *
 * \see
 *   For detailed information, see chapter \ref alib_characters_arrays "4. Character Arrays" of
 *   the Programmer's Manual of module \alib_characters.
 *
 * @tparam TString     The type for which specializations of this struct provide array type traits.
 * @tparam TChar       The character type of the character array that specializations provide
 *                     type traits for.
 * @tparam TEnableIf   Optional TMP parameter to allow templated and/or selective specializations.
 * ************************************************************************************************/
template<typename TString, typename TChar, typename TEnableIf= void>
struct T_CharArray
{
    /** Provides information about how the character array data of instances of type \p{TString}
     *  may be accessed */
    static constexpr    AccessType         Access      =   AccessType::NONE;

    /** Provides information about if and how instances of type \p{TString} may be created from
     *  character array data.*/
    static constexpr    ConstructionType   Construction=   ConstructionType::NONE;

    #if defined(ALIB_DOX)
    /** ********************************************************************************************
     * In specialized versions, this method has to be provided in case that field #Access is
     * not equal to \alib{characters,AccessType::NONE}.
     * In addition to this static method, sibling method #Length has to be provoided.
     *
     * For example, in a specialization for standard C++ class \c std::string, this method returns
     * the result of method <c>std::string::data()</c>.
     *
     * Note, that in the case of access type \alib{characters,AccessType::MutableOnly}, the
     * signature of this method needs to be slightly different with the specialization: argument
     * \p{src} in this case must be of type <c>TString&</c>, hence must not be \c const.
     *
     * @param  src The source object of external type \p{TString}.
     * @returns Specializatins have to return a pointer to the start of the character array
     *          represented by the given object \p{src} of type \p{TString}.
     **********************************************************************************************/
    static
    const TChar* Buffer( const TString& src );

    /** ********************************************************************************************
     * In specialized versions, this method has to be provided in case that field #Access is
     * not equal to \alib{characters,AccessType::NONE}.
     * In addition to this static method, sibling method #Buffer has to be provoided.
     *
     * For example, in a specialization for standard C++ class \c std::string, this method returns
     * the result of method <c>std::string::size()</c>.
     *
     * Note, that in the case of access type \alib{characters,AccessType::MutableOnly}, the
     * signature of this method needs to be slightly different with the specialization: argument
     * \p{src} in this case must be of type <c>TString&</c>, hence must not be \c const.
     *
     * @param  src The source object of external type \p{TString}.
     * @returns Specializatins have to return the length of the character array
     *          represented by the given object \p{src} of type \p{TString}.
     **********************************************************************************************/
    static
    integer Length( const TString& src );

    /** ********************************************************************************************
     * In specialized versions, this method has to be provided in case that field #Construction is
     * not equal to \alib{characters,ConstructionType::NONE}.
     *
     * If so, this method needs to construct and return an instance of type \p{TString}, created
     * from the character array specified by arguments \p{array} and \p{length}
     *
     * @param  array  The external array to be used to create the return value.
     * @param  length The length of the external array.
     * @returns A new instance (value) of type \p{TString}.
     **********************************************************************************************/
    static
    TString      Construct(  const TChar* array, integer length );
#endif
};

/** ************************************************************************************************
 * This type traits struct is in all aspects equivalent to \alib{characters,T_CharArray}, only that
 * this struct provides traits on zero-terminated character arrays, while \b % T_CharArray is
 * about non-zero terminated arrays.
 *
 * Please, therefore consult the documentation of type \alib{characters,T_CharArray}.
 *
 * \see
 *   For detailed information, see chapter \ref alib_characters_arrays "4. Character Arrays" of
 *   the Programmer's Manual of module \alib_characters.
 *
 * @tparam TString     The type for which specializations of this struct provide array type traits.
 * @tparam TChar       The character type of the character array that specializations provide
 *                     type traits for.
 * @tparam TEnableIf   Optional TMP parameter to allow templated and/or selective specializations.
 * ************************************************************************************************/
template<typename TString, typename TChar, typename TEnableIf= void>
struct T_ZTCharArray
{
    #if !defined(ALIB_DOX)
        static constexpr    AccessType         Access      =   AccessType::NONE;
        static constexpr    ConstructionType   Construction=   ConstructionType::NONE;
    #else
    /** ********************************************************************************************
     * Same as corresponding method \alib{characters::T_CharArray,Buffer} of sibling struct
     * \b T_CharArray.
     *
     * @param  src The source object of external type \p{TString}.
     * @returns Specializatins have to return a pointer to the start of the zero-terminated
     *          character array represented by the given object \p{src} of type \p{TString}.
     **********************************************************************************************/
    static
    const TChar* Buffer( const TString& src );

    /** ********************************************************************************************
     * Same as corresponding method \alib{characters::T_CharArray,Length} of sibling struct
     * \b T_CharArray.
     *
     * @param  src The source object of external type \p{TString}.
     * @returns Specializatins have to return the length of the character array
     *          represented by the given object \p{src} of type \p{TString}.
     **********************************************************************************************/
    static
    integer Length( const TString& src );

    /** ********************************************************************************************
     * Same as corresponding method \alib{characters::T_CharArray,Construct} of sibling struct
     * \b T_CharArray.
     *
     * @param  array  The external zero-terminated array to be used to create the return value.
     * @param  length The length of the external array.
     * @returns A new instance (value) of type \p{TString}.
     **********************************************************************************************/
    static
    TString      Construct(  const TChar* array, integer length );
#endif

};


/** ************************************************************************************************
 * Helper TMP struct that determines if a specialization of TMP struct \alib{characters,T_CharArray}
 * exists for type \p{TString}, and if so, for which character type such specialization was
 * made.
 *
 * If a specialization exists, the class inherits <c>std::true_type</c>, otherwise
 * <c>std::false_type</c>.
 *
 * \see
 *   For details see Programmer's Manual chapter \ref alib_characters_arrays_traits_tool_arraytype.
 *   A sibling helper struct exists with \alib{characters,TT_ZTCharArrayType}.
 *
 * @tparam TString      The custom type to test.
 * @tparam TEnableIf    Helper parameter used for templated specializations of this struct.
 **************************************************************************************************/
template<typename TString, typename TEnableIf= void>
struct TT_CharArrayType : std::false_type
{
    /** The character type of the character array that is implemented with type \p{TString}. */
    using TChar= void;
};

/** ************************************************************************************************
 * Helper TMP struct that determines if a specialization of TMP struct \alib{characters,T_ZTCharArray}
 * exists for type \p{TString}, and if so, for which character type such specialization was
 * made.
 *
 * If a specialization exists, the class inherits <c>std::true_type</c>, otherwise
 * <c>std::false_type</c>.
 *
 * \see
 *   For details see Programmer's Manual chapter \ref alib_characters_arrays_traits_tool_arraytype.
 *   A sibling helper struct exists with \alib{characters,TT_CharArrayType}.
 *
 * @tparam TString      The custom type to test.
 * @tparam EnableIf  Helper parameter used for templated specializations of this struct.
 **************************************************************************************************/
template<typename TString, typename EnableIf= void>
struct TT_ZTCharArrayType : std::false_type
{
    /** The character type of the zero-terminated character array that is implemented with type
     * \p{TString}. */
    using TChar= void;
};

#if !defined(ALIB_DOX)
template<typename T> struct TT_CharArrayType  <T, typename std::enable_if<characters::T_CharArray  <T,nchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= nchar; };
template<typename T> struct TT_CharArrayType  <T, typename std::enable_if<characters::T_CharArray  <T,wchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= wchar; };
template<typename T> struct TT_CharArrayType  <T, typename std::enable_if<characters::T_CharArray  <T,xchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= xchar; };
template<typename T> struct TT_ZTCharArrayType<T, typename std::enable_if<characters::T_ZTCharArray<T,nchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= nchar; };
template<typename T> struct TT_ZTCharArrayType<T, typename std::enable_if<characters::T_ZTCharArray<T,wchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= wchar; };
template<typename T> struct TT_ZTCharArrayType<T, typename std::enable_if<characters::T_ZTCharArray<T,xchar>::Access != AccessType::NONE>::type>: std::true_type { using TChar= xchar; };
#endif


// #################################################################################################
// Helper macros to specialize T_CharArray/T_ZTCharArray
// #################################################################################################

#if !defined(ALIB_DOX)

#define ALIB_CHARACTER_ARRAY_internal( C_ZTC, TString, TChar, Const, pAccess, pConstruction)       \
template<>  struct     T_ ## C_ZTC ## harArray<TString, TChar>                                     \
{                                                                                                  \
    static constexpr    AccessType               Access=    AccessType::       pAccess;            \
    static constexpr    ConstructionType   Construction=    ConstructionType:: pConstruction;      \
                                                                                                   \
    static inline       const TChar*          Buffer(  TString Const &  src );                     \
    static inline       integer               Length(  TString Const &  src );                     \
                                                                                                   \
    static inline       TString               Construct(  const TChar* array, integer length );    \
};

#endif


#define ALIB_CHARACTER_ARRAY(            TString, TChar,        Access     , Construction )        \
ALIB_CHARACTER_ARRAY_internal(     C ,   TString, TChar, const, Access     , Construction )

#define ALIB_CHARACTER_ARRAY_MUTABLE(    TString, TChar,                     Construction )        \
ALIB_CHARACTER_ARRAY_internal(     C ,   TString, TChar,      , MutableOnly, Construction )

#define ALIB_CHARACTER_ZT_ARRAY(         TString, TChar,        Access     , Construction )        \
ALIB_CHARACTER_ARRAY_internal(    ZTC,   TString, TChar, const, Access     , Construction )

#define ALIB_CHARACTER_ZT_ARRAY_MUTABLE( TString, TChar,                     Construction )        \
ALIB_CHARACTER_ARRAY_internal(    ZTC,   TString, TChar,      , MutableOnly, Construction )



#define ALIB_CHARACTER_ARRAY_IMPL_BUFFER(           TString, TChar, ... )                         \
const TChar*  T_CharArray<TString,TChar>::Buffer(TString const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_LENGTH(           TString, TChar, ... )                         \
integer       T_CharArray<TString,TChar>::Length(TString const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_BUFFER_MUTABLE(   TString, TChar, ... )                          \
const TChar*  T_CharArray<TString,TChar>::Buffer(TString      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_LENGTH_MUTABLE(   TString, TChar, ... )                          \
integer       T_CharArray<TString,TChar>::Length(TString      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ARRAY_IMPL_CONSTRUCT(        TString, TChar, ... )                          \
    TString  T_CharArray  <TString,TChar>::Construct( const TChar* array, integer length ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER(        TString, TChar, ... )                          \
const TChar* T_ZTCharArray<TString,TChar>::Buffer( TString const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH(        TString, TChar, ... )                          \
integer      T_ZTCharArray<TString,TChar>::Length( TString const&  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_BUFFER_MUTABLE(TString, TChar, ... )                          \
const TChar* T_ZTCharArray<TString,TChar>::Buffer( TString      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_LENGTH_MUTABLE(TString, TChar, ... )                          \
integer      T_ZTCharArray<TString,TChar>::Length( TString      &  src ) { __VA_ARGS__ }

#define ALIB_CHARACTER_ZT_ARRAY_IMPL_CONSTRUCT(     TString, TChar, ... )                          \
    TString  T_ZTCharArray<TString,TChar>::Construct( const TChar* array, integer length ) { __VA_ARGS__ }


// #################################################################################################
// Specializations of T_CharArray and T_ZTCharArray for const and non-const character pointer types
// #################################################################################################

#if !defined(ALIB_DOX)
// Fixed length arrays
template<size_t TCapacity, typename TChar>
struct T_CharArray<TChar[TCapacity], TChar>
{
    static constexpr AccessType     Access       =       AccessType::Implicit;
    static constexpr const TChar*   Buffer( TChar const (&src) [TCapacity] ) { return src; }
    static constexpr integer        Length( TChar const (&   ) [TCapacity] ) { return TCapacity -1; }
};


template<size_t TCapacity, typename TChar>
struct T_ZTCharArray<TChar[TCapacity], TChar>
{
    static constexpr AccessType     Access       =       AccessType::Implicit;
    static constexpr const TChar*   Buffer( TChar const (&src) [TCapacity] ) { return src; }
    static constexpr integer        Length( TChar const (&   ) [TCapacity] ) { return TCapacity -1; }
};

// constant character pointers
template<typename TChar> struct T_CharArray<TChar const*, TChar>
{
    static constexpr AccessType        Access       =       AccessType::Implicit;
    static constexpr ConstructionType  Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*      Buffer(const TChar* const & src       ) { return src; }
    static constexpr integer           Length(const TChar* const & src       ) { return src ? static_cast<integer>( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr const TChar*      Construct(const TChar* array, integer ) { return array; }
};

template<typename TChar> struct T_ZTCharArray<TChar const*, TChar>
{
    static constexpr AccessType        Access       =       AccessType::Implicit;
    static constexpr ConstructionType  Construction = ConstructionType::Implicit;
    static constexpr const TChar*      Buffer(const TChar* const & src       ) { return src; }
    static constexpr integer           Length(const TChar* const & src       ) { return src ? static_cast<integer>( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr const TChar*      Construct(const TChar* array, integer ) { return array; }
};

// mutable character pointers:
template<typename TChar> struct T_CharArray<TChar*, TChar>
{
    static constexpr AccessType        Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType  Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*      Buffer(      TChar* const & src       ) { return src;  }
    static constexpr integer           Length(      TChar* const & src       ) { return src ? static_cast<integer>( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr       TChar*      Construct(const TChar* array, integer ) { return const_cast<TChar*>( array ); }
};

// mutable character pointers:
template<typename TChar> struct T_ZTCharArray<TChar*, TChar>
{
    static constexpr AccessType        Access       =       AccessType::ExplicitOnly;
    static constexpr ConstructionType  Construction = ConstructionType::ExplicitOnly;
    static constexpr const TChar*      Buffer(     TChar* const & src        ) { return src;  }
    static constexpr integer           Length(     TChar* const & src        ) { return src ? static_cast<integer>( std::char_traits<TChar>::length(src) ) : 0; }
    static constexpr       TChar*      Construct(const TChar* array, integer ) { return const_cast<TChar*>( array ); }
};

#endif

// #################################################################################################
// TMP tool structs
// #################################################################################################

/**
 * This simple template struct provides inner type definition <c>type</c>, which in the two
 * specializations for types \ref aworx::nchar and \ref aworx::wchar aliases the respective other
 * type.
 *
 * The struct may be used to generalize code that is templated in respect to the character type to
 * work on and that needs to refer to the character type that is complementary to the one currently
 * compiled.
 *
 * The struct may be used in code via shortcut macro \ref ATMP_CHAR_COMPLEMENT.
 */
template<typename>  struct TT_ComplementChar
{
    using type=  void;        ///< Default is void
};

#if !defined(ALIB_DOX)
template<>          struct TT_ComplementChar<nchar> { using type=  wchar; };
template<>          struct TT_ComplementChar<wchar> { using type=  nchar; };


#define ATMP_CHAR_COMPLEMENT( TChar )    typename TT_ComplementChar<TChar>::type

#endif



/**
 * This helper struct is used to determine the corresponding \alib character type, in cases that
 * a fixed size of characters is required. Specializations for size values \c 1, \c 2 and \c 4
 * exists.
 *
 * As an example, the \https{QT Class Library,www.qt.io} uses a 2-byte character width,
 * independent from compiler and platform. Therefore, to convert a \b QT character value to
 * an \alib character value, the destination type is:
 *
 *          typename TT_CharOfSize<2>::type
 *
 * @tparam TCharSize The size of the required character type in bytes.
 */
template<int TCharSize> struct TT_CharOfSize : public std::false_type
{
    #if defined(ALIB_DOX)
    /** One of the \alib character types, \alib{characters,nchar}, \alib{characters,wchar} or
     *  \alib{characters,xchar}. */
    using type= void;
    #endif
};

#if !defined(ALIB_DOX)
template<>   struct TT_CharOfSize<sizeof(nchar)> : public std::true_type     { using type= nchar; };
template<>   struct TT_CharOfSize<sizeof(wchar)> : public std::true_type     { using type= wchar; };
template<>   struct TT_CharOfSize<sizeof(xchar)> : public std::true_type     { using type= xchar; };
#endif

/**
 * Helper struct that in its non-specialized version inherits from \c std::false_type.
 * For the three \alib character types \alib{characters,nchar}, \alib{characters,wchar} and
 * \alib{characters,xchar} (respectively \alib{characters,character},
 * \alib{characters,complementChar} and  \alib{characters,strangeChar}), a specialization exists
 * that inherits \c std::true_type.
 *
 * With that, the class may be used to decide if template type \p{TChar} is an \alib character type.
 *
 * @tparam TChar The type to test for being an \alib character type.
 */
template<typename TChar> struct TT_IsChar : public std::false_type {};

#if !defined(ALIB_DOX)
template<>   struct TT_IsChar<nchar> : public std::true_type {};
template<>   struct TT_IsChar<wchar> : public std::true_type {};
template<>   struct TT_IsChar<xchar> : public std::true_type {};
#endif

}} // namespace aworx[::lib::character]


/// Type alias in namespace #aworx.
using  nchar         = lib::characters::nchar;

/// Type alias in namespace #aworx.
using  wchar         = lib::characters::wchar;

/// Type alias in namespace #aworx.
using  xchar         = lib::characters::xchar;

/// Type alias in namespace #aworx.
using  character     = lib::characters::character;

/// Type alias in namespace #aworx.
using  complementChar= lib::characters::complementChar;

/// Type alias in namespace #aworx.
using  strangeChar   = lib::characters::strangeChar;

} // namespace [aworx]





#endif // HPP_ALIB_CHARACTERS_CHARACTERS


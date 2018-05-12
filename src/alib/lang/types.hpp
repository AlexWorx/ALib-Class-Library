// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_TYPES)
    #error "include alib/alib.hpp instead of this header"
#endif

#ifndef HPP_ALIB_LANG_TYPES
#define HPP_ALIB_LANG_TYPES 1

// #################################################################################################
// Namespace documentation
// #################################################################################################


namespace aworx { namespace lib { namespace lang {


// #################################################################################################
// types integer, uinteger, intGap_t and uintGap_t
// #################################################################################################
#if defined(DOX_PARSER)
/**
 * This type specifies platform independent integer values of the 'natural' bit-size of the
 * underlying platform. In general, on 32-bit systems this will be 32-bit wide, on 64-bit systems,
 * 64-bits. Hence, on standard architectures, it has the same bit-size and signedness as
 * \b std::ptrdiff_t.
 *
 * To declare constants of this type, a user defined literal is provided with
 * \ref aworx::literals::operator "" _integer(unsigned long long int idx) "_integer".
 *
 * The type can be considered as a signed version of \c std::size_t. It is needed because
 * standard type \c 'int' is not specified in respect to its size. E.g. GNU C++ and Clang compiler
 * use 32-Bit integers for type \c int, even on 64-Bit platforms.
 *
 * See also \ref aworx::lib::lang::uinteger "aworx::uinteger".
 *
 * \note This documentation is generated using the 64-Bit version of the library. In fact, the
 *       definition as \c int64_t is not guaranteed platform specific.
 */

using  integer =                    platform_specific;

/**
 * Unsigned version of \ref aworx::lib::lang::integer "aworx::integer".
 * This type should be the same as \c std::size_t on all platforms.
 */
using uinteger =                    platform_specific;


/**
 * This type, together with its counterpart
 * \ref aworx::lib::lang::uintGap_t "aworx::uintGap_t" is used to fill a gap that occurs
 * when method overloads or template specialization is needed for integer types.
 * The rational behind and use of this pair of types is best explained with a sample.
 *
 * Consider the following code:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_INTXX_DECLARATION
 *
 * When this is run under 64 Bit - Linux, GNU compiler, the following output is produced:
 *   \snippet "DOX_ALIB_LANG_INTXX.txt"     OUTPUT
 *
 * This is not what many C++ programmers would expect: Although type <c>long long</c> is the same
 * 64-bit type as <c>long</c>, the template method is not seen as specialized by the compiler.
 * Therefore, we have a "gap" in the definition of specializations for types
 * <c>long long</c> and <c>unsigned long long</c>.
 *
 * When compiling and running the same sample code under GNU compiler 32-bit or under
 * MSVC (Microsoft compiler), 32 or 64-bit, then the gap "moves" to be with types
 * <c>long</c> and <c>unsigned long</c> instead.
 * Here, this hurts a lot, because code that uses a simple integer constant \c 1L is not fetched by
 * the template specializations!
 *
 * The lesson learned is that two more specializations are needed and that their types are
 * dependent on the compiler and library used. Because it is not allowed to specialize
 * simply with all possible extra variants (this would lead to doubly defined methods),
 * a preprocessor switch that chooses the right types to fill the gap is needed.
 *
 * This type, together with #uintGap_t, does exactly this: using the preprocessor to select
 * the right "missing" type.
 *
 * To fix the sample above, the following two specializations of the template method need to
 * be added:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_INTXX_DECLARATION2
 *
 * When overloading functions with integer types, similar rules apply: To have the complete set
 * of integer types covered, 10 overloads are needed: from type \b int8_t to type \b int64_t,
 * type \b %intGap_t and then those five types in two versions, signed and unsigned.
 * Only with all overloads in place, compiler warnings (on high warning levels) and/or the
 * necessity of explicit type conversions are avoided.
 */
using  intGap_t=                    platform_specific;

 /**
  * Used to complete overwriting methods and template specializations.<br>
  * See signed sibling type \ref aworx::lib::lang::intGap_t "aworx::intGap_t" for more information.
  */
using uintGap_t=                    platform_specific;


// 64-Bit platforms
#elif  (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8 ) || defined(_WIN64)

            using  integer =                               int64_t;
            using uinteger =                              uint64_t;

    #if    defined(_WIN64) || defined(__APPLE__)
            using  intGap_t=                                  long;
            using uintGap_t=                    unsigned      long;

    #elif  defined(__GNUC__) || defined(__clang__)

            using  intGap_t=                             long long;

            using uintGap_t=                    unsigned long long;
    #else
        #error "Unknown compiler/OS"
    #endif

// 32-Bit platforms
#elif (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4 ) || defined(_WIN32)

            using  integer =                               int32_t;
            using uinteger =                              uint32_t;

    #if defined(__APPLE__)
        #error "Apple 32 OS(es) not tested, yet"

    #elif defined(__GNUC__)  || defined(__clang__) || defined(_MSC_VER)

            using  intGap_t=                                  long;
            using uintGap_t=                         unsigned long;
    #else
        #error "Unknown compiler/OS"
    #endif

#else
    #error "Can not detect size of pointer"
#endif

}}}// namespace [aworx::lib::lang]


namespace aworx {

/**
 * This is an \e "inline" namespace for user defined C++ literals. Due to being inline,
 * it is sufficient to have a using statement
 *
 *      using namespace aworx;
 *
 * in the code, which is - due to the habit of "mirroring" type aliases into this namespace
 * usually all that is needed to use A-Worx classes (e.g. from libraries
 * \ref aworx::lib "ALib" or
 * \ref aworx::lib::lox "ALox").
 *
 * In case of collisions, alternatively, a statement
 *
 *      using namespace aworx::literals;
 *
 * may be used.
 *
 * This is a sample of using A-Worx %literals:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_INTEGER_LITERAL
 */
inline namespace literals {
    /**
     * User defined literal to denote numbers of type \ref aworx::lib::lang::integer
     * (aliased as \b %aworx::integer).
     *
     * \see Namespace documentation \ref aworx::literals for information about using
     * \alib %literals.
     *
     * @param value The number literal to cast to \ref aworx::lib::lang::integer "aworx::integer".
     * @return The number casted to type \ref aworx::lib::lang::integer "aworx::integer".
     */
    constexpr aworx::lib::lang::integer operator "" _integer(unsigned long long int value)
    {
        return static_cast<aworx::lib::lang::integer>( value );
    }
}} // namespace [aworx::literals]



namespace aworx {

/// Type alias in namespace #aworx.
using integer=          aworx::lib::lang::integer;

/// Type alias in namespace #aworx.
using uinteger=         aworx::lib::lang::uinteger;

/// Type alias in namespace #aworx.
using intGap_t=         aworx::lib::lang::intGap_t;

/// Type alias in namespace #aworx.
using uintGap_t=        aworx::lib::lang::uintGap_t;


/**
 * This type represents a narrow character in \alib. Usually this is an alias for built-in C++
 * type \c char.
 */
using nchar=    char;

/**
 * This type represents a wide character in \alib. Usually this is an alias for built-in C++
 * type \c wchar_t.
 */
using wchar=    wchar_t;

#if ALIB_NARROW_STRINGS || defined(DOX_PARSER)
    /**
     * This type represents a standard \alib character. The width (size) of a character is dependent
     * on the platform and compilation flags.
     */
    using character=        nchar;
    /**
     * This type represents a non-standard \alib character. If \ref aworx::character is defined
     * to implement a narrow character type, then this type implements a wide character and vice
     * versa.
     */
    using complementChar=   wchar;
#else
    using character=        wchar;
    using complementChar=   nchar;
#endif


#if ALIB_SIZEOF_WCHAR_T == 4  || defined(DOX_PARSER)
    /**
     * Besides types \ref aworx::character and \ref aworx::complementChar, this is the third
     * character type defined by \alib. Its definition depends on the width of built-in type
     * \c wchar_t: If its width is 2 bytes, then this type aliases \c char32_t and if its width is
     * 4 bytes, this type aliases \c char16_t.
     */
    using strangeChar= char16_t;

    #define COMPLSTR(STR)        u ## STR
#else
    using strangeChar= char32_t;

    #define COMPLSTR(STR)        U ## STR
#endif

/**
 * This simple template struct encapsulates inner type definition <c>type</c>, which in the two
 * existing specializations for types \ref aworx::nchar and \ref aworx::wchar denote in each case
 * the respective other type.
 *
 * The struct is useful in code that is templated in respect to the character type to work on and
 * that needs to refer to the character type that is complementary to the one currently compiled.
 */
template<typename>  struct T_ComplementChar;

/** See template type \ref aworx::T_ComplementChar. */
template<>          struct T_ComplementChar<nchar>
{
    using type=  wchar;  ///< The complement character to \c nchar is \c wchar.
};

/** See template type \ref aworx::T_ComplementChar. */
template<>          struct T_ComplementChar<wchar>
{
    using type=  nchar;  ///< The complement character to \c wchar is \c nchar.
};


} // namespace [aworx]




#endif // HPP_ALIB_LANG_TYPES

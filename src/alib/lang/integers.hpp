/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_INTEGERS
#define HPP_ALIB_LANG_INTEGERS 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

#if !defined (_GLIBCXX_CSTDINT) && !defined (_CSTDINT_)
#   include <cstdint>
#endif

#if !defined (_GLIBCXX_CSTDDEF) && !defined (_CSTDDEF_)
#   include <cstddef>
#endif

namespace alib { namespace lang {

// #################################################################################################
// types integer, uinteger, intGap_t and uintGap_t
// #################################################################################################
#if defined(ALIB_DOX)

    #define ALIB_SIZEOF_INTEGER

/**
 * This type specifies platform independent integral values of the 'natural' bit-size of the
 * underlying platform. In general, on 32-bit systems this will be 32-bit wide, on 64-bit systems,
 * 64-bits. Hence, on standard architectures, it has the same bit-size and signedness as
 * \b std::ptrdiff_t.
 *
 * The type can be considered as a signed version of \c std::size_t. It is needed because
 * standard type \c 'int' is not specified in respect to its size. E.g. GNU C++ and Clang compiler
 * use 32-Bit integers for type \c int, even on 64-Bit platforms.
 *
 * See also \ref alib::uinteger "alib::uinteger".
 *
 * \note This documentation is generated using the 64-Bit version of the library. In fact, the
 *       definition as \c int64_t shown here, is not guaranteed platform specific.
 *
 */

using  integer =                    platform_specific;

/**
 * Unsigned version of \ref alib::integer "alib::integer".
 * This type should be the same as \c std::size_t on all platforms.
 */
using uinteger =                    platform_specific;


/**
 * This type, together with its counterpart
 * \ref alib::uintGap_t "alib::uintGap_t" is used to fill a gap that occurs
 * when method overloads or template specialization are needed for integer types.
 * The rationale behind and use of this pair of types is best explained with a sample.
 *
 * Consider the following code:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_INTXX_DECLARATION
 *
 * When this is run under 64 Bit - Linux, GNU compiler, the following output is produced:
 *   \verbinclude "DOX_ALIB_INTXX.txt"
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
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_INTXX_DECLARATION2
 *
 * When overloading functions with integer types, similar rules apply: To have the complete set
 * of integer types covered, 10 overloads are needed: from type \b int8_t to type \b int64_t,
 * type \b %intGap_t and then those five types in two versions, signed and unsigned.
 * Only with all overloads in place, compiler warnings (on high warning levels),
 * compiler errors due to ambiguouties and/or the necessity of explicit type conversions are
 * avoided.
 *
 * \see
 *   Along with these definitions, preprocessor symbol \ref ALIB_SIZEOF_INTGAP is defined.
 */
using  intGap_t=                    platform_specific;

 /**
  * Used to complete overwriting methods and template specializations.<br>
  * See signed sibling type \ref alib::intGap_t "alib::intGap_t" for more information.
  */
using uintGap_t=                    platform_specific;



#else // ALIB_DOX


//------------- One of the 5 symbols given from outside? ---------------------
#if         defined(ALIB_SIZEOF_INTEGER   )          \
        ||  defined(ALIB_SIZEOF_INTGAP    )          \
        ||  defined(ALIB_INTGAP_TYPE      )          \
        ||  defined(ALIB_SIZEOF_LONGDOUBLE_REPORTED) \
        ||  defined(ALIB_SIZEOF_LONGDOUBLE_WRITTEN)

    // just check if all are given, that's it for now
    #if         !defined(ALIB_SIZEOF_INTEGER)             \
            ||  !defined(ALIB_SIZEOF_INTGAP)              \
            ||  !defined(ALIB_INTGAP_TYPE)                \
            ||  !defined(ALIB_SIZEOF_LONGDOUBLE_REPORTED) \
            ||  !defined(ALIB_SIZEOF_LONGDOUBLE_WRITTEN)

        #error "If one of the Compiler Symbols \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
is given (instead of letting ALib detect them), then the whole group has to be given!"
    #endif


//------------- None of the 5 symbols given from outside: Platform detection ---------------------
#else

    // 64-Bit platforms
    #if  (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8 ) || defined(_WIN64)

        #define ALIB_SIZEOF_INTEGER         8

        #if    defined(_WIN32)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4

        #elif  defined(__APPLE__)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      8

        #elif  defined(__GNUC__) || defined(__clang__)
            #define ALIB_INTGAP_TYPE        long long;
            #define ALIB_SIZEOF_INTGAP      8
        #else
        #   error "Can not detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

        #endif

        #if defined(_MSC_VER)  || defined(__APPLE__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   8
        #else
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   16
        #endif

    // 32-Bit platforms
    #elif (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4 ) || defined(_WIN32) || defined( __arm__)

        #define ALIB_SIZEOF_INTEGER   4

        #if defined(__APPLE__)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4

        #elif defined(__GNUC__)  || defined(__clang__) || defined(_MSC_VER)
            #define ALIB_INTGAP_TYPE        long;
            #define ALIB_SIZEOF_INTGAP      4
        #else
        #error "Can not detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

        #endif

        #if defined(_MSC_VER) || defined( __arm__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   8
        #elif defined(__APPLE__)
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   16
        #else
        #   define ALIB_SIZEOF_LONGDOUBLE_REPORTED   12
        #endif

    // unrecognized platform
    #else
            #error "Can not detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

    #endif   //64, 32, unrecognized

    // deduct stuff
    #if (ALIB_SIZEOF_LONGDOUBLE_REPORTED == 8)
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      8
    #elif defined(__aarch64__)
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      16
    #else
    #   define     ALIB_SIZEOF_LONGDOUBLE_WRITTEN      10
    #endif


#endif // No symbol given

// #############   set according to symbols     #############

#if ALIB_SIZEOF_INTEGER   == 4
    using  integer =                    int32_t;
    using uinteger =                   uint32_t;
#elif ALIB_SIZEOF_INTEGER == 8
    using  integer =                    int64_t;
    using uinteger =                   uint64_t;
#else
    #error "Compiler symbol 'ALIB_SIZEOF_INTEGER' supports only values 4 and 8."
#endif

using  intGap_t=                 ALIB_INTGAP_TYPE;
using uintGap_t=       unsigned  ALIB_INTGAP_TYPE;



// #############   checks     #############
#define ERROR_DETECTING \
"Can not detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"

static_assert( sizeof(integer)     == sizeof(uinteger )              , "\nSize mismatch in definition of alib::[u]integer on this platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(integer)     == sizeof(size_t   )              , "\nSize mismatch in definition of alib::[u]integer on this platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(integer)     == sizeof(ptrdiff_t)              , "\nSize mismatch in definition of alib::[u]integer on this platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(integer)     == sizeof(void*    )              , "\nSize mismatch in definition of alib::[u]integer on this platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(integer)     == ALIB_SIZEOF_INTEGER            , "\nSize mismatch in definition of alib::[u]integer on this platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(intGap_t)    == ALIB_SIZEOF_INTGAP             , "\nDefinition of symbol ALIB_SIZEOF_INTGAP not adjusted to platform/compiler.\n"                      ERROR_DETECTING);
static_assert( sizeof(long double) == ALIB_SIZEOF_LONGDOUBLE_REPORTED, "\nSize mismatch in definition of macro ALIB_SIZEOF_LONGDOUBLE_REPORTED on this platform/compiler.\n" ERROR_DETECTING);



#if !defined(ALIB_SIZEOF_LONGDOUBLE_WRITTEN)
        #error "Can not detect compilation platform. Please provide Symbols  \
'ALIB_SIZEOF_INTEGER', \
'ALIB_SIZEOF_INTGAP', \
'ALIB_INTGAP_TYPE', \
'ALIB_SIZEOF_LONGDOUBLE_REPORTED' or \
'ALIB_SIZEOF_LONGDOUBLE_WRITTEN' \
as documented with ALib User Manual at https://alib.dev"
#endif

#endif // not ALIB_DOX

} // namespace alib[::lang]

/// Type alias in namespace \b alib.
using  integer       = lang::integer;

/// Type alias in namespace \b alib.
using uinteger       = lang::uinteger;

/// Type alias in namespace \b alib.
using intGap_t       = lang::intGap_t;

/// Type alias in namespace \b alib.
using uintGap_t      = lang::uintGap_t;

} // namespace [alib]

#endif // HPP_ALIB_LANG_INTEGERS

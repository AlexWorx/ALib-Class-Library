// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_INTEGERS
#define HPP_ALIB_LIB_INTEGERS 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(INTEGERS)


#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

#if !defined (_GLIBCXX_CSTDINT) && !defined (_CSTDINT_)
#   include <cstdint>
#endif

#if !defined (_GLIBCXX_CSTDDEF) && !defined (_CSTDDEF_)
#   include <cstddef>
#endif

namespace aworx { namespace lib {

// #################################################################################################
// types integer, uinteger, intGap_t and uintGap_t
// #################################################################################################
#if ALIB_DOCUMENTATION_PARSER

    #define ALIB_SIZEOF_INTEGER


/**
 * This type specifies platform independent integer values of the 'natural' bit-size of the
 * underlying platform. In general, on 32-bit systems this will be 32-bit wide, on 64-bit systems,
 * 64-bits. Hence, on standard architectures, it has the same bit-size and signedness as
 * \b std::ptrdiff_t.
 *
 * The type can be considered as a signed version of \c std::size_t. It is needed because
 * standard type \c 'int' is not specified in respect to its size. E.g. GNU C++ and Clang compiler
 * use 32-Bit integers for type \c int, even on 64-Bit platforms.
 *
 * See also \ref aworx::lib::uinteger "aworx::uinteger".
 *
 * \note This documentation is generated using the 64-Bit version of the library. In fact, the
 *       definition as \c int64_t shown here, is not guaranteed platform specific.
 *
 */

using  integer =                    platform_specific;

/**
 * Unsigned version of \ref aworx::lib::integer "aworx::integer".
 * This type should be the same as \c std::size_t on all platforms.
 */
using uinteger =                    platform_specific;


/**
 * This type, together with its counterpart
 * \ref aworx::lib::uintGap_t "aworx::uintGap_t" is used to fill a gap that occurs
 * when method overloads or template specialization are needed for integer types.
 * The rationale behind and use of this pair of types is best explained with a sample.
 *
 * Consider the following code:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_INTXX_DECLARATION
 *
 * When this is run under 64 Bit - Linux, GNU compiler, the following output is produced:
 *   \snippet "DOX_ALIB_INTXX.txt"     OUTPUT
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
  * See signed sibling type \ref aworx::lib::intGap_t "aworx::intGap_t" for more information.
  */
using uintGap_t=                    platform_specific;




// 64-Bit platforms
#elif  (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8 ) || defined(_WIN64)
    #define ALIB_SIZEOF_INTEGER   8
            using  integer =                               int64_t;
            using uinteger =                              uint64_t;

    #if    defined(_WIN64) || defined(__APPLE__)
            using  intGap_t=                                  long;
            using uintGap_t=                    unsigned      long;
            #define ALIB_SIZEOF_INTGAP   4
            static_assert( sizeof(intGap_t) == ALIB_SIZEOF_INTGAP,
                      "Definition of symbol ALIB_SIZEOF_INTGAP not adjusted to platform/compiler" );
    #elif  defined(__GNUC__) || defined(__clang__)

            using  intGap_t=                             long long;
            using uintGap_t=                    unsigned long long;
            #define ALIB_SIZEOF_INTGAP   8
            static_assert( sizeof(intGap_t) == ALIB_SIZEOF_INTGAP,
                      "Definition of symbol ALIB_SIZEOF_INTGAP not adjusted to platform/compiler" );
    #else
        #error "Unknown compiler/OS"
    #endif

    #if defined(_MSC_VER)
    #   define ALIB_SIZEOF_LONGDOUBLE   8
    #else
    #   define ALIB_SIZEOF_LONGDOUBLE   16
    #endif

// 32-Bit platforms
#elif (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4 ) || defined(_WIN32)

    #define ALIB_SIZEOF_INTEGER   4
            using  integer =                               int32_t;
            using uinteger =                              uint32_t;

    #if defined(__APPLE__)
        #error "Apple 32 OS(es) not tested, yet"

    #elif defined(__GNUC__)  || defined(__clang__) || defined(_MSC_VER)
            using  intGap_t=                                  long;
            using uintGap_t=                         unsigned long;
            #define ALIB_SIZEOF_INTGAP   4
            static_assert( sizeof(intGap_t) == ALIB_SIZEOF_INTGAP,
                      "Definition of symbol ALIB_SIZEOF_INTGAP not adjusted to platform/compiler" );
    #else
        #error "Unknown compiler/OS"
    #endif

    #if defined(_MSC_VER)
    #   define ALIB_SIZEOF_LONGDOUBLE   8
    #else
    #   define ALIB_SIZEOF_LONGDOUBLE   12
    #endif


#else
    #error "Can not detect size of pointer"
#endif

// #############   checks     #############
static_assert(     sizeof(integer) == sizeof(uinteger      )
               &&  sizeof(integer) == sizeof(size_t        )
               &&  sizeof(integer) == sizeof(ptrdiff_t     )
               &&  sizeof(integer) == sizeof(void*         )
               &&  sizeof(integer) == ALIB_SIZEOF_INTEGER,
               "Size mismatch in definition of aworx::[u]integer on this platform/compiler" );

static_assert( sizeof(long double) == ALIB_SIZEOF_LONGDOUBLE,
               "Size mismatch in definition of macro ALIB_SIZEOF_LONGDOUBLE on this platform/compiler" );


} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using  integer       = aworx::lib::integer;

/// Type alias in namespace #aworx.
using uinteger       = aworx::lib::uinteger;

/// Type alias in namespace #aworx.
using intGap_t       = aworx::lib::intGap_t;

/// Type alias in namespace #aworx.
using uintGap_t      = aworx::lib::uintGap_t;


} // namespace [aworx]


#endif // HPP_ALIB_LIB_INTEGERS

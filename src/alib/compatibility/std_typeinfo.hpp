/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.<br>
 * With the inclusion of this header compatibility features between \alib and the C++ standard
 * library are provided.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".                                                                                 >
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_STD_TYPEINFO
#define HPP_ALIB_COMPATIBILITY_STD_TYPEINFO 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

#if !defined (_TYPEINFO) && !defined(_TYPEINFO_)
    #include <typeinfo>
#endif

#if !defined(_GLIBCXX_TYPEINDEX) && !defined(_TYPEINDEX_)
    #include <typeindex>
#endif

#if !defined(_GLIBCXX_FUNCTIONAL) && !defined(_FUNCTIONAL_)
    #include <functional>
#endif

#include <stddef.h>


namespace alib {

/**
 * This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
 * \alib that are not specific to dedicated \alibmods..<br>
 * The entities of those namespaces become available with the inclusion of optional "compatibility"
 * headers found in folder \alibsrcdir{compatibility}.
 */
namespace compatibility {

/**
 * This namespace documents compatibility features of \alib (that are not specific to  one of the
 * \alibmods) and the standard C++ class library found in namespace \c std.
 */
namespace std {


/** ************************************************************************************************
 * This is a simple struct that features the use of <c>std::type_info</c> as key values of
 * container types like hash tables and sets. For this, this struct provides inner
 * functors \b hash, \b equal_to and \b less.
 *
 * If for example a <c>std::unordered_map</c> should be instantiated with run-time type
 * information as the hash key, a definition looks as follows:
 *
 *      std::unordered_map<TypeFunctors::Key,      MyMappedType,
 *                         TypeFunctors::Hash,
 *                         TypeFunctors::EqualTo                  > myMap;
 *
 * Note that the key type evaluates to <c>const std::type_info*</c>, hence pointers to
 * the structs have to be given when interfacing with the container.
 **************************************************************************************************/
struct TypeFunctors
{
    /** The key type */
    using Key= const ::std::type_info*;

    /** ********************************************************************************************
     * Hash code functor for type <c>const std::type_info*</c>.
     **********************************************************************************************/
    struct Hash
    {
        /**
         * Invokes <c>std::type_info::hash_code</c> on the wrapped type.
         * @param typeinfo  Pointer to the run-time type information struct.
         * @return The hash code
         */
        size_t operator()( Key typeinfo ) const
        {
            return typeinfo->hash_code();
        }
    };

    /** ********************************************************************************************
     * Comparison functor for type <c>const std::type_info*</c>.
     **********************************************************************************************/
    struct EqualTo
    {
        /**
         * Invokes <c>operator ==</c> with \p{lhs} and \p{rhs}.
         * @param lhs  The left-hand side value.
         * @param rhs  The right-hand side value.
         * @return \c true if the objects represent the same type, \c false otherwise.
         */
        bool operator()( Key lhs, Key rhs ) const
        {
            return *lhs == *rhs;
        }
    };

    /** ********************************************************************************************
     * Comparison functor for type <c>const std::type_info*</c>.
     **********************************************************************************************/
    struct Less
    {
        /**
         * Invokes <c>std::type_index::operator\<</c> on corresponding temporaries created from
         * \p{lhs} and \p{rhs}.
         * @param lhs  The left-hand side value.
         * @param rhs  The right-hand side value.
         * @return \c true if \p{lhs} is "less" than \p{rhs}, \c false otherwise.
         */
        bool operator()( Key lhs, Key rhs ) const
        {
            return ::std::type_index( *lhs ) < ::std::type_index( *rhs );
        }
    };
}; // struct TypeFunctors

}} // namespace alib[::compatibility::std]

/// Type alias in namespace \b alib.
using     TypeFunctors=       compatibility::std::TypeFunctors;

} // namespace [alib]

#endif // HPP_ALIB_COMPATIBILITY_STD_TYPEINFO

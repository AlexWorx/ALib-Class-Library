// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ENUMS_ENUM_ARITHMETICAL
#define HPP_ALIB_ENUMS_ENUM_ARITHMETICAL 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

#if !defined (HPP_ALIB_LIB_PREDEF_TMP)
#   include "alib/lib/predef_tmp.hpp"
#endif

namespace aworx { namespace lib { namespace enums {

// #################################################################################################
// struct T_EnumIsArithmetical
// #################################################################################################


/**
 * # Introduction #
 *
 * With scoped enums, introduced by C++ 11, arithmetic operators are not supported. This seems a
 * little contradiction as still enums have their underlying integer type and with a little
 * static casting, these integer values are accessible. Also, the other way round, an enum value
 * in C++ can be initialized by passing arbitrary integer values (even those that no elements
 * exist for) like this:
 *
 *          auto myElement= MyEnum( 42 );
 *
 * Therefore, if operators are needed, the common approaches are to either
 * - define operators for each scoped enum type that need them, or to
 * - provide a set of templated operator functions that handle all scoped enum types.
 *
 * The first approach includes a lot of duplicate code, the second has the disadvantage that it
 * undermines the intention of the C++ 11 language standard: The compiler would allow the operators
 * even custom scoped enum types that are not considered to be "aritmetical".
 *
 * # %ALib Arithmetic %Enum Operators #
 *
 * This extremely simple TMP struct is used to overcome the disadvantages of the approaches discussed
 * above. The default implementation is empty and just inherits from \c std::false_type.
 *
 * Specializations of the struct for a certain enum type should be performed using macro
 * \ref ALIB_ENUM_IS_ARITHMETICAL. The macro needs to be located in a header file, probably
 * close to the enum type declaration itself, but \b outside of any namespace (aka in the default
 * namespace).
 *
 * With this specialization, the compiler accepts a set of templated operator functions which
 * get conditionally selected using TMP struct \c std::enable_if which of-course checks for
 * this struct to be specialized for the type in question.
 *
 * The following \alib set of operators are applicable to scoped enumerations if marked arithmetic:
 *
 * - \ref operator<
 * - \ref operator<=
 * - \ref operator>
 * - \ref operator>=
 * - \ref operator+
 * - \ref operator-
 * - \ref operator+=
 * - \ref operator-=
 *
 * Each operator exists in two versions: one accepting an enum element for both operands and
 * a second that accepts the right hand side operand of the integer type that is underlying the
 * scoped or non-scoped enumeration.
 *
 * \attention
 *   While in this documentation the operators are appearing under namespace <c>aworx::lib::enums</c> ,
 *   in reality they are defined in <b>no namespace</b> (global namespace).
 *   This was done to have the documentation of the operator functions and this struct just in one
 *   place!
 * \attention
 *   Due to the template meta programming, this does not 'clutter' the global namespace and does
 *   not interfere with any existing user code, while still there is the advantage of
 *   not needing to put a using statement like
 *
 *          using namespace aworx::lib::enums;
 *   to each source location that uses the operators.
 *
 *
 * \~Comment #################################################################################### \~
 * # Arithmetic Enums and other %ALib %Enum Features #
 *
 * This TMP struct must not be specialized in parallel with TMP struct \alib{enums,T_EnumIsBitwise}.
 * Other \alib enum features can used in parallel:
 * - \alib{enums,T_EnumIsIteratable}
 * - \alib{resources,T_EnumMetaDataDecl} and corresponding related TMP structs.
 *
 * \~Comment #################################################################################### \~
 * # Restrictions #
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * @tparam TEnum       The enum type to provide information for.
 * @tparam TCheckEnum  This parameter has a default expressions and <b>must not</b> be provided
 *                     with specializations of this struct.
 *                     It is used internally to ensure that only enum types are to be used with
 *                     the first template parameter \p{TEnum}.
 */
template<typename TEnum,
         typename TCheckEnum= ATMP_VOID_IF(std::is_enum<TEnum>::value)>
struct T_EnumIsArithmetical : public std::false_type {};

}} // namespace aworx[::lib::enums]
/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumIsArithmetical=           lib::enums::T_EnumIsArithmetical<TEnum>;

} // namespace [aworx]


// #################################################################################################
// Helper Macros
// #################################################################################################

#define  ALIB_ENUM_IS_ARITHMETICAL(TEnum)                                                          \
namespace aworx { namespace lib { namespace enums {                                                \
template<> struct T_EnumIsArithmetical<TEnum> : public std::true_type {};}}}                       \


// #################################################################################################
// Arithmetic Operators
// #################################################################################################

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib::enums
#if ALIB_DOCUMENTATION_PARSER
namespace aworx { namespace lib { namespace enums {
#endif


/**
 * Comparison operator \e less.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) < TValue(rhs);
}

/**
 * Comparison operator \e less between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) < rhs ;
}

/**
 * Comparison operator <em>less or equal</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<=  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) <= TValue(rhs);
}

/**
 * Comparison operator <em>less or equal</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) <= rhs;
}

/**
 * Comparison operator <em>greater</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) > TValue(rhs);
}

/**
 * Comparison operator <em>greater</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) > rhs;
}

/**
 * Comparison operator <em>greater or equal</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>=  (TEnum lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) >= TValue(rhs) ;
}


/**
 * Comparison operator <em>greater or equal</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
bool
#else
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) >= rhs ;
}


/**
 * Add operator between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator+  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + TValue(rhs) );
}

/**
 * Add operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator+  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + rhs );
}


/**
 * Add assignment operator between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator+=  (TEnum& lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + TValue(rhs) );
}

/**
 * Add assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator+=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + rhs );
}

/**
 * Prefix increment operator.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value )
#endif
operator++  (TEnum&  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) + 1 );
}

/**
 * Postfix increment operator.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @tparam TEnum       Enumeration type.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator++  (TEnum&  arg, int) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    TEnum tmp= arg;
    arg= TEnum( TValue(arg) + 1 );
    return tmp;
}

/**
 * Subtract operator between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator-  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - TValue(rhs) );
}


/**
 * Subtract operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator-  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - rhs );
}

/**
 * Subtract assignment operator between two enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator-=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - TValue(rhs) );
}

/**
 * Subtract assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator-=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - rhs );
}

/**
 * Prefix decrement operator.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator--  (TEnum&  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) - 1 );
}


/**
 * Postfix decrement operator.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @tparam TEnum       Enumeration type.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator--  (TEnum&  arg, int) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    TEnum tmp= arg;
    arg= TEnum( TValue(arg) - 1 );
    return tmp;
}

/**
 * Unary plus operator for enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  arg         Operand.
 * @tparam TEnum       Enumeration type.
 * @return Parameter \p{arg} (identical value).
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator+  (TEnum  arg) noexcept(true)
{
    return arg;
}

/**
 * Unary minus operator for enum elements.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  arg         Operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator-  (TEnum  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( - TValue(arg) );
}


/**
 * Multiplication operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator*  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) * rhs );
}

/**
 * Multiplication assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator*= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) * rhs );
}

/**
 * Division operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator/  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) / rhs );
}

/**
 * Division assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator/= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) / rhs );
}


/**
 * Modulo operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator%  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) % rhs );
}

/**
 * Modulo assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator%= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) % rhs );
}


/**
 * Shift-left operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/**
 * Shift-left assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator<<= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) << rhs );
}


/**
 * Shift-right operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/**
 * Shift-right assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if ALIB_DOCUMENTATION_PARSER
TEnum
#else
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value)
#endif
operator>>= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) >> rhs );
}


// Reset documentation fake
#if ALIB_DOCUMENTATION_PARSER
}}} // namespace [aworx::lib::enums]
#endif


#endif // HPP_ALIB_ENUMS_ENUM_ARITHMETICAL

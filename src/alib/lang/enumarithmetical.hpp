// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_ENUM_ARITHMETICAL)
    #error "Header already included"
#endif

#ifndef HPP_ALIB_LANG_ENUM_ARITHMETICAL
//! @cond NO_DOX
#define HPP_ALIB_LANG_ENUM_ARITHMETICAL 1
//! @endcond


namespace aworx { namespace lib { namespace lang {

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
 * even non-bitfield custom scoped enum types.
 *
 * # %ALib Arithmetic %Enum Operators #
 *
 * This extremely simple TMP struct is used to overcome the disadvantages of the approaches discussed
 * above. The default implementation is empty and just inherits from \c std::false_type.
 *
 * Specializations of the struct for a certain enum type should be performed using macro
 * \ref ALIB_LANG_ENUM_IS_ARITHMETICAL. The macro needs to be located in a header file, probably
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
 *   While in this documentation the operators are appearing under namespace <c>aworx::lib::lang</c> ,
 *   in reality they are defined in <b>no namespace</b> (global namespace).
 *   This was done to have the documentation of the operator functions and this struct just in one
 *   place!
 * \attention
 *   Due to the template meta programming, this does not 'clutter' the global namespace and does
 *   not interfere with any existing user code, while still there is the advantage of
 *   not needing to put a using statement like
 *
 *          using namespace aworx::lib::lang;
 *   to each source location that uses the operators.
 *
 *
 * \~Comment #################################################################################### \~
 * # Arithmetic Enums and other %ALib %Enum Features #
 *
 * This TMP struct must not be specialized in parallel with TMP struct \alib{lang,T_EnumIsBitwise}.
 * Other \alib enum features can used in parallel:
 * - \alib{lang,T_EnumIsIteratable}
 * - \alib{lang,T_EnumMetaDataDecl} and corresponding related TMP structs.
 *
 *
 * @tparam TEnum       The enum type to provide information for.
 * @tparam TCheckEnum  This parameter has a default expressions and <b>must not</b> be provided
 *                     with specializations of this struct.
 *                     It is used internally to ensure that only enum types are to be used with
 *                     the first template parameter \p{TEnum}.
 */
template<typename TEnum,
         typename TCheckEnum= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsArithmetical : public std::false_type {};

}} // namespace [lib::lang]
/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumIsArithmetical=           lib::lang::T_EnumIsArithmetical<TEnum>;
namespace lib { namespace lang {


// #################################################################################################
// Helper Macros
// #################################################################################################


#define  ALIB_LANG_ENUM_IS_ARITHMETICAL(TEnum)                                                     \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_EnumIsArithmetical<TEnum> : public std::true_type {};}}}                       \

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib::lang
#if !defined(DOX_PARSER)
}}} // namespace [aworx::lib::lang]
#endif



// #################################################################################################
// Arithmetic Operators
// #################################################################################################

/**
 * Comparison operator \e less.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool >::type
operator<  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) < TValue(rhs);
}

/**
 * Comparison operator \e less between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) < rhs ;
}

/**
 * Comparison operator <em>less or equal</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator<=  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) <= TValue(rhs);
}

/**
 * Comparison operator <em>less or equal</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator<=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) <= rhs;
}

/**
 * Comparison operator <em>greater</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator>  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) > TValue(rhs);
}

/**
 * Comparison operator <em>greater</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) > rhs;
}

/**
 * Comparison operator <em>greater or equal</em> between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator>=  (TEnum lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) >= TValue(rhs) ;
}


/**
 * Comparison operator <em>greater or equal</em> between an enum element and an integer value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, bool>::type
operator>=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) >= rhs ;
}


/**
 * Add operator between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + TValue(rhs) );
}

/**
 * Add operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + rhs );
}


/**
 * Add assignment operator between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+=  (TEnum& lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + TValue(rhs) );
}

/**
 * Add assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + rhs );
}

/**
 * Prefix increment operator.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator++  (TEnum&  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) + 1 );
}

/**
 * Postfix increment operator.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @tparam TEnum       Enumeration type.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
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
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - TValue(rhs) );
}


/**
 * Subtract operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - rhs );
}

/**
 * Subtract assignment operator between two enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - TValue(rhs) );
}

/**
 * Subtract assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - rhs );
}

/**
 * Prefix decrement operator.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator--  (TEnum&  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) - 1 );
}


/**
 * Postfix decrement operator.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @tparam TEnum       Enumeration type.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
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
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  arg         Operand.
 * @tparam TEnum       Enumeration type.
 * @return Parameter \p{arg} (identical value).
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+  (TEnum  arg) noexcept(true)
{
    return arg;
}

/**
 * Unary minus operator for enum elements.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  arg         Operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-  (TEnum  arg) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( - TValue(arg) );
}


/**
 * Multiplication operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator*  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) * rhs );
}

/**
 * Multiplication assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator*= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) * rhs );
}

/**
 * Division operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator/  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) / rhs );
}

/**
 * Division assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator/= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) / rhs );
}


/**
 * Modulo operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator%  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) % rhs );
}

/**
 * Modulo assignment operator between an enum element and an integer value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator%= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) % rhs );
}


/**
 * Shift-left operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator<<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/**
 * Shift-left assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator<<= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) << rhs );
}


/**
 * Shift-right operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator>>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/**
 * Shift-right assignment operator between an enum element and an integer value of underlying type.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator>>= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs) noexcept(true)
{
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) >> rhs );
}


// Reset documentation fake
#if defined(DOX_PARSER)
}}} // namespace [aworx::lib::lang]
#endif


#endif // HPP_ALIB_LANG_ENUM_ARITHMETICAL

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_ARITHMETICAL
#define HPP_ALIB_ENUMS_ARITHMETICAL 1

#if !defined(HPP_ALIB_COMPILERS) && !defined(ALIB_DOX)
#   include "alib/lib/compilers.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_TMP) && !defined(ALIB_DOX)
#   include "alib/lib/tmp.hpp"
#endif

namespace aworx { namespace lib { namespace enums {

// #################################################################################################
// struct T_EnumIsArithmetical
// #################################################################################################


#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Simple TMP struct that inherits <c>std::false_type</c> by default. If specialized for an
 * enumeration type \p{TEnum} to inherit <c>std::true_type</c>, the following operators  set of
 * \alib operators become applicable to elements of \p{TEnum}:
 *
 * - \alib{enums::arithmetical,operator<}
 * - \alib{enums::arithmetical,operator<=}
 * - \alib{enums::arithmetical,operator>}
 * - \alib{enums::arithmetical,operator>=}
 * - \alib{enums::arithmetical,operator+}
 * - \alib{enums::arithmetical,operator-}
 * - \alib{enums::arithmetical,operator+=}
 * - \alib{enums::arithmetical,operator-=}
 *
 * \attention
 *   Likewise with the operators introduced with TMP struct \alib{enums,T_EnumIsBitwise},
 *   this documentation "fakes" the operators into namespace <c>aworx::lib::enums</c>,
 *   while in fact they are defined in the global namespace!<br>
 *   See \ref alib_enums_arithmethic_standard "corresponding note" in the Programmer's Manual
 *   for details.
 *
 * <b>Restrictions</b><br>
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * \see
 *   - Macro \ref ALIB_ENUMS_MAKE_ARITHMETICAL, which specializes this TMP struct for a given
 *     enumeration type.
 *   - For details and a source code sample see chapter \ref alib_enums_arithmethic_standard
 *     of the Programmer's Manual of module \alib_enums.
 *
 * @tparam TEnum      The enum type to enable arithmetical operators for.
 * @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
 *                    with specializations of this struct.
 *                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
 **************************************************************************************************/
template<typename TEnum,typename TEnableIf>
struct T_EnumIsArithmetical : public std::false_type {};
#else
template<typename TEnum,
         typename TEnableIf= ATMP_VOID_IF(std::is_enum<TEnum>::value)>
struct T_EnumIsArithmetical : public std::false_type {};
#endif


}} // namespace aworx[::lib::enums]
/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumIsArithmetical=           lib::enums::T_EnumIsArithmetical<TEnum>;

} // namespace [aworx]


// #################################################################################################
// Helper Macros
// #################################################################################################

#define  ALIB_ENUMS_MAKE_ARITHMETICAL(TEnum)                                                        \
namespace aworx { namespace lib { namespace enums {                                                \
template<> struct T_EnumIsArithmetical<TEnum> : public std::true_type {};}}}                       \


// #################################################################################################
// Arithmetic Operators
// #################################################################################################

// For documentation, all operators and enum related template functions are faked into namespace
// aworx::lib::enums
#if defined(ALIB_DOX)
namespace aworx { namespace lib { namespace enums {

/**
 * Operators available to elements of enumerations if \alib{enums,T_EnumIsArithmetical} is
 * specialized.
 *
 * \note
 *   This namespace exits only in the documentation to collect the operators.
 *   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b>.
 */
namespace arithmetical {
#endif


/**
 * Comparison operator \e less between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Comparison operator <em>less or equal</em> between an enum element and an integral value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Comparison operator <em>greater</em> between an enum element and an integral value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Comparison operator <em>greater or equal</em> between an enum element and an integral value of
 * underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The result of the comparison.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Add operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Add assignment operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @tparam TEnum       Enumeration type.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  arg Reference to the enum value to be incremented.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
#if defined(ALIB_DOX)
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
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Subtract operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Subtract assignment operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @return The new value of \p{lhs} which is set the resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  arg Reference to the enum value to be decremented.
 * @return The old value of \p{arg}.
 */
template<typename TEnum>
ALIB_CPP14_CONSTEXPR
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param  arg         Operand.
 * @return Parameter \p{arg} (identical value).
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * @tparam TEnum       Enumeration type.
 * @param  arg         Operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Multiplication operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Multiplication assignment operator between an enum element and an integral value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Division operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Division assignment operator between an enum element and an integral value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Modulo operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Modulo assignment operator between an enum element and an integral value of underlying
 * type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Shift-left operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Shift-left assignment operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Shift-right operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
 * Shift-right assignment operator between an enum element and an integral value of underlying type.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsArithmetical} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The resulting enum element.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
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
#if defined(ALIB_DOX)
}}}} // doxygen namespace [aworx::lib::enums::arithmetical]
#endif


#endif // HPP_ALIB_ENUMS_ARITHMETICAL

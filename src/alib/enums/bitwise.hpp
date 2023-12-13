/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_BITWISE
#define HPP_ALIB_ENUMS_BITWISE 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_TOOLS)
#   include "alib/lib/tools.hpp"
#endif

#if !defined(HPP_ALIB_ENUMS_ARITHMETICAL)
#   include "alib/enums/arithmetical.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL)
#   include "alib/enums/underlyingintegral.hpp"
#endif

#if !defined(HPP_ALIB_BITS)
#   include "alib/lib/bits.hpp"
#endif

namespace aworx { namespace lib {

/**
 * This is the reference documentation of sub-namespace \c enums of the \aliblink, which
 * holds types of library module \alib_enums_nl.
 *
 * Extensive documentation for this module is provided with
 * \ref alib_mod_enums "ALib Module Enums - Programmer's Manual".
 *
 * \attention
 *   All operators are declared in the global namespace, other than this
 *   namespace documentation indicates!
 */
namespace enums {

// #################################################################################################
// struct T_EnumIsBitwise
// #################################################################################################
#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Simple TMP struct that inherits <c>std::false_type</c> by default. If specialized for an
 * enumeration type \p{TEnum} to inherit <c>std::true_type</c>, the following operators  set of
 * \alib operators become applicable to elements of \p{TEnum}:
 *
 * - \alib{enums::bitwise,operator&}
 * - \alib{enums::bitwise,operator&=}
 * - \alib{enums::bitwise,operator|}
 * - \alib{enums::bitwise,operator|=}
 * - \alib{enums::bitwise,operator^}
 * - \alib{enums::bitwise,operator^=}
 * - \alib{enums::bitwise,operator~}
 * - \alib{enums::bitwise,operator+}  (Alias for \alib{enums::bitwise,operator|})
 * - \alib{enums::bitwise,operator-}  (Alias for a combination of operators
 *   \alib{enums::bitwise,operator&} and \alib{enums::bitwise,operator~})
 * - \alib{enums::bitwise,operator+=} (An alias for \alib{enums::bitwise,operator|=})
 * - \alib{enums::bitwise,operator-=} (Removes given bit(s) )
 *
 * \attention
 *   Likewise with the operators introduced with TMP struct \alib{enums,T_EnumIsArithmetical},
 *   this documentation "fakes" the operators into namespace <c>aworx::lib::enums</c>,
 *   while in fact they are defined in the global namespace!<br>
 *   See \ref alib_enums_arithmetic_standard "corresponding note" in the Programmer's Manual
 *   for details.
 *
 * <b>Restrictions</b><br>
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * \see
 *   - Macro \ref ALIB_ENUMS_MAKE_BITWISE, which specializes this TMP struct for a given
 *     enumeration type.
 *   - For details and a source code sample see chapter \ref alib_enums_arithmetic_bitwise
 *     of the Programmer's Manual of module \alib_enums.
 *
 * @tparam TEnum      The enum type to enable arithmetical operators for.
 * @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
 *                    with specializations of this struct.
 *                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
 *
*  \I{#############################################################################################}
 * # Restrictions #
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * # Reference Documentation #
 *
 * @tparam TEnum      The enum type to enable bitwise operators for.
 * @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
 *                    with specializations of this struct.
 *                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf>
struct T_EnumIsBitwise : public std::false_type {};
#else
template<typename TEnum,
         typename TEnableIf= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsBitwise : public std::false_type {};
#endif

}} // namespace aworx[::lib::enums]

/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumIsBitwise=       lib::enums::T_EnumIsBitwise<TEnum>;

} // namespace [aworx]

// #################################################################################################
// Helper Macros
// #################################################################################################

#define  ALIB_ENUMS_MAKE_BITWISE(TEnum)                                                            \
namespace aworx { namespace lib { namespace enums {                                                \
template<>                                                                                         \
struct T_EnumIsBitwise<TEnum> : public std::true_type {};}}}



// #################################################################################################
// Bitwise Operators
// #################################################################################################

// For documentation, all operators and enum related template functions are faked into namespace
// aworx::lib::enums
#if defined(ALIB_DOX)
namespace aworx { namespace lib { namespace enums {
/**
 * Operators available to elements of enumerations if \alib{enums,T_EnumIsBitwise} is
 * specialized.
 *
 * \note
 *   This namespace exits only in the documentation to collect the operators.
 *   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b> and
 *   functions \alib{enums::bitwise,HasBits}, \alib{enums::bitwise,CountElements},
 *   \alib{enums::bitwise,ToBitwiseEnumeration} and \alib{enums::bitwise,ToSequentialEnumeration}
 *   reside in namespace \ref aworx.
 *
 * As required, when parsed by a C++ compiler, the operators reside in the global namespace.
 */
namespace bitwise {

#endif


/**
 * Bitwise \b and operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of a bitwise and operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator&  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) & TBits(rhs));
}

/**
 * Bitwise assignment operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set to <c>lhs & rhs</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator&=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & TBits(rhs) );
}

/**
 * Bitwise \b or operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of a bitwise or operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator|  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/**
 * Bitwise <b>or assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
bool
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator|=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/**
 * Bitwise \b xor operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of a bitwise xor operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator^  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) ^ TBits(rhs));
}

/**
 * Bitwise <b>xor assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set to <c>lhs ^ rhs</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator^=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) ^ TBits(rhs) );
}

/**
 * Bitwise \b not operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * \note To remove one or more bits from a scoped enum value, operator <b>&=</b> with this operator
 *       applied to \p{op} can be used.
 *       A shortcut to this is given with \ref operator-=.
 *
 * @tparam TEnum      Enumeration type.
 * @param  op         The operand to be negated.
 * @return The result of a bitwise negation of \p{op}.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<aworx::lib::enums::T_EnumIsBitwise<TEnum>::value, TEnum>::type
#endif
operator~ (TEnum  op) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( ~ TBits(op) );
}


/**
 * Alias to bitwise \b or operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enums,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @tparam TEnum      Enumeration type.
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @return The result of a bitwise or operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<    aworx::lib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator+  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/**
 * Alias for bitwise <b>or assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enums,T_EnumIsArithmetical}
 * is \b not specialized to inherit \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<    aworx::lib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator+=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/**
 * Removes bit(s) found in \p{rhs} from \p{lhs} an returns result. This is a shortcut to:
 *
 *      lhs & !rhs
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enums,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @tparam TEnum       Enumeration type.
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @return The result of <c>lhs & !rhs</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<    aworx::lib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator-  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( TBits(lhs) & (~TBits(rhs)) );
}

/**
 * Removes bit(s) found in \p{rhs} from \p{lhs}. This is a shortcut to:
 *
 *      lhs &= !rhs
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enums,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @tparam TEnum       Enumeration type.
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @return The new value of \p{lhs} which is set to <c>lhs & ( ~rhs )</c>.
 */
template<typename TEnum>
constexpr
#if defined(ALIB_DOX)
TEnum
#else
typename  std::enable_if<    aworx::lib::enums::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::enums::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
#endif
operator-=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & (~TBits(rhs)) );
}

#if !defined(ALIB_DOX)
namespace aworx {
#endif

#if defined(ALIB_DOX)
/**
 * Tests if the integral value of the given enum \p{element} contains all bits set in
 * \p{selection}.
 * In other words, returns result of:
 *
 *       ( element & selection ) == selection
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @tparam TEnum      Enumeration type. Deduced by the compiler.
 * @param  element    Bitset to be tested.
 * @param  selection  Second operand.
 * @return \c true if all bits of \p{selection} are set in \p{element}.
 */
template<typename TEnum>
constexpr
bool HasBits(TEnum  element, TEnum selection)                           noexcept;
#else
template<typename TEnum>
constexpr
ATMP_T_IF(bool, aworx::lib::enums::T_EnumIsBitwise<TEnum>::value)
HasBits(TEnum  element, TEnum selection)                           noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(element) & TBits(selection) ) == TBits(selection);
}
#endif

#if defined(ALIB_DOX)
/**
 * Returns the bitwise enumeration element of \p TEnum from a given sequential enumeration.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * \see The reverse function #ToSequentialEnumeration.
 *
 * @tparam TEnum An enumeration type which is defined to be "bitwise".
 * @param number A sequentially enumerated number, for which the corresponding bitwise
 *               enumeration element is requested.
 *               \ref alib_enums_arithmetic_bitwise "bitwise enumeration".
 * @return Returns <c>1 << number</c>.
 */
template<typename TEnum>
constexpr
TEnum ToBitwiseEnumeration(typename std::underlying_type<TEnum>::type number );
#else

}

#if !defined (HPP_ALIB_ENUMS_ITERABLE)
#   include "alib/enums/iterable.hpp"
#endif

namespace aworx {
template<typename TEnum>
ALIB_CONSTEXPR17
ATMP_T_IF(TEnum, aworx::lib::enums::T_EnumIsBitwise<TEnum>::value)
ToBitwiseEnumeration(typename std::underlying_type<TEnum>::type number )
{
    #if !defined (HPP_ALIB_ENUMS_ITERABLE)
        ALIB_ASSERT_ERROR( number >= 0
                           && (   !lib::enums::T_EnumIsIterable<TEnum>::value
                                || lib::enums::T_EnumIsIterable<TEnum>::End  > TEnum( 1 << number ) ),
                                "ENUMS",  "Number out of bounds."                                      )
    #endif
    return TEnum( 1 << number );
}
#endif

#if defined(ALIB_DOX)

/**
 * Returns the sequentially enumerated number derived from the given bitwise enumeration
 * value.
 * In other words, the positon of the most significant bit set in the underlying integral of the
 * given enum \p element is returned.<br>
 * In debug-compilations an \alib assertion is raised in case that the given value is not a
 * single enum element but a combination of bits.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * \see The reverse function #ToBitwiseEnumeration.
 * @tparam TEnum  A bitwise defined enumeration type. Deduced by the compiler.
 * @param element An enumeration value.
 * @return The sequential number of an element of an enum type which is defined bitwise.
 */
template<typename TEnum>
static constexpr
typename std::underlying_type<TEnum>::type
ToSequentialEnumeration( TEnum element );
#else
template<typename TEnum>
ALIB_CONSTEXPR17
ATMP_T_IF(typename std::underlying_type<TEnum>::type, aworx::lib::enums::T_EnumIsBitwise<TEnum>::value)
ToSequentialEnumeration( TEnum element )
{
    ALIB_ASSERT_ERROR(               UnderlyingIntegral(element)  != 0, "ENUMS",
                       "No bits set in given enum value"  )
    ALIB_ASSERT_ERROR( lib::BitCount(UnderlyingIntegral(element)) == 1, "ENUMS",
                       "Multiple bits given with enum value"  )
    return static_cast<typename std::underlying_type<TEnum>::type>(
           aworx::lib::MSB( UnderlyingIntegral(element) ) - 1    );
}
#endif

#if defined(ALIB_DOX)
/**
 * Returns the number of bitwise enumeration elements set in the given value.
 * In other words, the bits given in \p value are counted and the number is returned.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param value A single or composite selection of bits.
 * @return The result of a call to #aworx::lib::BitCount().
 */
template<typename TEnum>
static constexpr
int CountElements( TEnum value );
#else
template<typename TEnum>
constexpr
ATMP_T_IF(int, aworx::lib::enums::T_EnumIsBitwise<TEnum>::value)
CountElements( TEnum value )
{
    return lib::BitCount(UnderlyingIntegral(value));
}
#endif


#if defined(ALIB_DOX)
}}}} // doxygen namespace [aworx::lib::enums::bitwise]
#else
} // namespace [aworx]
#endif

#endif // HPP_ALIB_ENUMS_BITWISE

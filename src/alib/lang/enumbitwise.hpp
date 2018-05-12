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
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_ENUM_BITWISE)
    #error "include alib/alib.hpp instead of this header"
#endif

#ifndef HPP_ALIB_LANG_ENUM_BITWISE
#define HPP_ALIB_LANG_ENUM_BITWISE 1

#if !defined(HPP_ALIB_LANG_ENUM_ARITHMETICAL)
#   include "enumarithmetical.hpp"
#endif

namespace aworx { namespace lib { namespace lang {

// #################################################################################################
// struct T_EnumIsBitwise
// #################################################################################################

/**
 * # Introduction #
 *
 * With scoped enums, introduced by C++ 11, logical operators are not supported. This seems a
 * little in contradiction as still enums can be defined bitwise. What we mean with the term
 * "bitwise" is that the elements of enums get numbers assigned which each have set a different
 * bit in the underlying integer type.
 *
 * As a sample, consider the following two scoped enum types:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_NORMAL
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_DECLARATION
 *
 * \note The first enum is taken from the sample found in documentation of TMP struct
 *       \alib{lang,T_EnumMetaDataDecl}. We have applied macro \ref ALIB_LANG_ENUM_NAMED also to the
 *       second, bitwise enum type. For further
 *       information refer to \alib{lang,T_EnumMetaDataDecl,ALib Enum Meta Information} and below.
 *
 * The second enum type obviously is bitwise. And as the identifier indicates that values
 * of this enum represent "states of windows of a window manager", it is obvious that
 * enum values with multiple bits set might occur.
 * Nevertheless, the following code
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLEERROR
 *
 *  leads to a compilation error similar to this:
 *
 *       error: invalid operands to binary expression ('WindowManager::States' and 'WindowManager::States')
 *
 * Therefore, if operators are needed, the common approaches are to either
 * - define operators for each scoped enum type that need them, or to
 * - provide a set of templated operator functions that handle all scoped enum types.
 *
 * The first approach includes a lot of duplicate code, the second has the disadvantage that it
 * undermines the intention of the C++ 11 language standard: The compiler would allow the operators
 * even non-bitfield custom scoped enum types.
 *
 * # %ALib Bitwise %Enum Operators #
 *
 * This extremely simple TMP struct is used to overcome the disadvantages of the approaches discussed
 * above. The default implementation is empty and just inherits from \c std::false_type.
 *
 * Specializations of the struct for a certain enum type should be performed using macro
 * \ref ALIB_LANG_ENUM_IS_BITWISE. The macro needs to be located in a header file, probably
 * close to the enum type declaration itself, but \b outside of any namespace (aka in the default
 * namespace). In our sample, we simply put the code line:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_SPECIALIZE
 *
 * at the end of the header file (again: outside of the namespace!). All that the macro does, is
 * specializing this struct to inherit from \c std::true_type. (With the current version of \alib,
 * still the struct is empty.)
 *
 * With this specialization, the compiler accepts a set of templated operator functions which
 * get conditionally selected using TMP struct \c std::enable_if which of-course checks for
 * this struct to be specialized for the type in question.
 *
 * The following code snippet now compiles:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_OPERATORS_SAMPLE_1
 *
 * The following \alib set of operators are applicable to scoped enumerations of bitwise type:
 *
 * - \b   operator&   (Link to documentation of ampersand operator currently impossible for technical reasons.)
 * - \b   operator&=
 * - \ref operator|
 * - \ref operator|=
 * - \ref operator^
 * - \ref operator^=
 * - \ref operator~
 * - \ref operator+  (An alias for \ref operator|=)
 * - \ref operator-  (Removes bits of rhs in lhs)
 * - \ref operator+= (An alias for \ref operator|=)
 * - \ref operator-= (Removes given bit(s) )
 *
 * In addition, helper function
 * - \ref EnumContains
 *
 * is available.
 *
 * Independent from this TMP struct, for all C++ scoped enum type, in addition, helper function
 * - \ref EnumValue
 *
 * is provided with including this header file.
 *
 * \attention
 *   While in this documentation the operators are appearing under namespace <c>aworx::lib::lang</c> ,
 *   in reality they are defined in <b>no namespace</b> (global namespace).
 *   This was done to have the documentation of the operator functions and this struct just in one place!
 * \attention
 *   Due to the template meta programming, this does not 'clutter' the global namespace and does
 *   not interfere with any existing user code, while still there is the advantage of
 *   not needing to put a using statement like
 *
 *          using namespace aworx::lib::lang;
 *   to each source location that use the operators.
 *
 *
 * \~Comment #################################################################################### \~
 * # Bitwise Enums and %ALib %Enum Meta Data #
 *
 * This concept of supporting bitwise operations on selected types, goes very well along with
 * the \alib{lang,T_EnumMetaDataDecl,ALib Enum Meta Information} features.
 * However the use of a "element to enum identifier name translation table" as described and sampled
 * in this context, needs a little more thinking. Because bitwise enum values might consist of
 * mixed elements, the simple approach to find just one corresponding meta data table entry
 * for converting an enum element into a string representation fails. In contrast, probably more
 * than one element has to be written or combinations of bits set as once, should even lead to
 * new words, not defined in the enum class itself (e.g. "Maximized" in the sample above).
 *
 * Therefore, \alib provides two different specializations of TMP struct \alib{strings,T_Apply},
 * namely
 * - \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>}
 *   for applying non-bitwise enum types to \b %AStrings.
 * - \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}
 *   for applying bitwise enum types to \b %AStrings.
 *
 * Again, they are selected by the compiler automatically dependent on the availability of
 * a specialization of this struct \b %T_EnumIsBitwise, using \c std::enable_if.
 * That is good news, of-course, but when defining the "translation table", some special
 * rules (and options!) have to_be kept in mind. Those are discussed and sampled with
 * documentation of \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}.
 *
 *
 * # Reference Documentation #
 *
 * @tparam TEnum       The enum type to provide information for.
 * @tparam TCheckEnum  This parameter has a default expressions and <b>must not</b> be provided
 *                     with specializations of this struct.
 *                     It is used internally to ensure that only enum types are to be used with
 *                     the first template parameter \p{TEnum}.
 */
template<typename TEnum,
         typename TCheckEnum= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsBitwise : public std::false_type {};

}} // namespace [lib::lang]
/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumIsBitwise=           lib::lang::T_EnumIsBitwise<TEnum>;
namespace lib { namespace lang {


// #################################################################################################
// Helper Macros
// #################################################################################################


#define  ALIB_LANG_ENUM_IS_BITWISE(TEnum)                                                          \
namespace aworx { namespace lib { namespace lang {                                                 \
template<>                                                                                         \
struct T_EnumIsBitwise<TEnum> : public std::true_type {};}}}                                       \

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib::lang
#if !defined(DOX_PARSER)
}}} // namespace [aworx::lib::lang]
#endif



// #################################################################################################
// Bitwise Operators
// #################################################################################################

/**
 * Bitwise \b and operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of a bitwise and operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator&  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) & TBits(rhs));
}

/**
 * Bitwise assignment operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set to <c>lhs & rhs</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator&=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & TBits(rhs) );
}

/**
 * Bitwise \b or operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of a bitwise or operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator|  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/**
 * Bitwise <b>or assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator|=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/**
 * Bitwise \b xor operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of a bitwise xor operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator^  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) ^ TBits(rhs));
}

/**
 * Bitwise <b>xor assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set to <c>lhs ^ rhs</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator^=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) ^ TBits(rhs) );
}

/**
 * Bitwise \b not operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * \note To remove one or more bits from a scoped enum value, operator <b>&=</b> with this operator
 *       applied to \p{op} can be used.
 *       A shortcut to this is given with \ref operator-=.
 *
 * @param  op         The operand to be negated.
 * @tparam TEnum      Enumeration type.
 * @return The result of a bitwise negation of \p{op}.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, TEnum>::type
operator~ (TEnum  op) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( ~ TBits(op) );
}


/**
 * Alias to bitwise \b or operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type and if \alib{lang,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @param  lhs        First operand.
 * @param  rhs        Second operand.
 * @tparam TEnum      Enumeration type.
 * @return The result of a bitwise or operation of the underlying enum values.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<    aworx::lib::lang::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator+  (TEnum  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/**
 * Alias for bitwise <b>or assignment</b> operator useable with scoped enum types.
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{lang,T_EnumIsArithmetical}
 * is \b not specialized to inherit \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<    aworx::lib::lang::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
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
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{lang,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @param  lhs         First operand.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The result of <c>lhs & !rhs</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<    aworx::lib::lang::T_EnumIsBitwise     <TEnum>::value
                         && !aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
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
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type and if \alib{lang,T_EnumIsArithmetical}
 * is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
 * situations where an enum is both, arithmetical and bitwise.
 *
 * @param[in,out]  lhs Reference to the first operand. Receives the result.
 * @param  rhs         Second operand.
 * @tparam TEnum       Enumeration type.
 * @return The new value of \p{lhs} which is set to <c>lhs & ( ~rhs )</c>.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<    aworx::lib::lang::T_EnumIsBitwise     <TEnum>::value
                             && !aworx::lib::lang::T_EnumIsArithmetical<TEnum>::value, TEnum>::type
operator-=  (TEnum&  lhs, TEnum rhs) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & (~TBits(rhs)) );
}


/**
 * Tests if given scoped enum value \p{tested} <b>xor assignment</b> contains (all) bit(s) set
 * in scoped enum value \p{testFor}. In other words, returns result of:
 *
 *       ( tested & testFor ) == testFor
 *
 * Selected by the compiler only if \alib{lang,T_EnumIsBitwise} is specialized for
 * template enum type \p{TEnum} to inherit \c std::true_type.
 *
 * @param  tested        Bitset to be tested.
 * @param  testFor       Second operand.
 * @tparam TEnum         Enumeration type.
 * @return \c true if all bits of \p{testFor} are set in \p{tested}.
 */
template<typename TEnum>
constexpr
typename  std::enable_if<aworx::lib::lang::T_EnumIsBitwise<TEnum>::value, bool>::type
EnumContains(TEnum  tested, TEnum testFor) noexcept(true)
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(tested) & TBits(testFor) ) == TBits(testFor);
}


// Reset documentation fake
#if defined(DOX_PARSER)
}}} // namespace [aworx::lib::lang]
#endif


#endif // HPP_ALIB_LANG_ENUM_BITWISE

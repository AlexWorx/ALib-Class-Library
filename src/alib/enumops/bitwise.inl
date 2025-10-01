//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {

/// This is the reference documentation of sub-namespace \c enumops of the \aliblink, which
/// holds types of library module \alib_enumops.
///
/// Extensive documentation for this module is provided with
/// \ref alib_mod_enums "ALib Module Enums - Programmer's Manual".
///
/// \attention
///   All operators are declared in the global namespace, other than this
///   namespace documentation indicates!
namespace enumops {

// #################################################################################################
// struct BitwiseTraits
// #################################################################################################

//==================================================================================================
/// Simple type trait that inherits <c>std::false_type</c> by default.
/// If specialized for an enumeration type \p{TEnum} to inherit <c>std::true_type</c>, the
/// following operators set of \alib operators become applicable to elements of \p{TEnum}:
///
/// - \alib{enumops::bitwise;operator&}
/// - \alib{enumops::bitwise;operator&=}
/// - \alib{enumops::bitwise;operator|}
/// - \alib{enumops::bitwise;operator|=}
/// - \alib{enumops::bitwise;operator^}
/// - \alib{enumops::bitwise;operator^=}
/// - \alib{enumops::bitwise;operator~}
/// - \alib{enumops::bitwise;operator+}  (Alias for \alib{enumops::bitwise;operator|})
/// - \alib{enumops::bitwise;operator-}  (Alias for a combination of operators
///   \alib{enumops::bitwise;operator&} and \alib{enumops::bitwise;operator~})
/// - \alib{enumops::bitwise;operator+=} (An alias for \alib{enumops::bitwise;operator|=})
/// - \alib{enumops::bitwise;operator-=} (Removes given bit(s) )
///
/// \attention
///   Likewise with the operators introduced with the type trait \alib{enumops;ArithmeticalTraits},
///   this documentation "fakes" the operators into namespace <c>alib::enumops</c>,
///   while in fact they are defined in the global namespace.<br>
///   See \ref alib_enums_arithmetic_standard "corresponding note" in the Programmer's Manual
///   for details.
///
/// <b>Restrictions</b><br>
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// \see
///   - Macro \ref ALIB_ENUMS_MAKE_BITWISE, which specializes this type trait for a given
///     enumeration type.
///   - For details and a source code sample see chapter \ref alib_enums_arithmetic_bitwise
///     of the Programmer's Manual of the module \alib_enumops.
///
///  \I{#############################################################################################}
/// # Restrictions #
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// # Reference Documentation #
///
/// @tparam TEnum      The enum type to enable bitwise operators for.
//==================================================================================================
template<typename TEnum>
requires std::is_enum<TEnum>::value
struct BitwiseTraits : std::false_type {};

ALIB_WARNINGS_IGNORE_DOCS

/// Concept that is satisfied if the type trait \alib{enumops;BitwiseTraits}
/// is specialized for type \p{TEnum} to inherit <c>std::true_type</c>.
/// @tparam TEnum The type to test.
template<typename TEnum>
concept IsBitwise = BitwiseTraits<TEnum>::value;

ALIB_WARNINGS_RESTORE

} // namespace alib[::enumops]
} // namespace [alib]

// #################################################################################################
// Bitwise Operators
// #################################################################################################
// For documentation, all operators and enum related template functions are faked into namespace
// alib::enumops
#if DOXYGEN
namespace alib {  namespace enumops {
/// Operators available to elements of enumerations if \alib{enumops;BitwiseTraits} is
/// specialized.
///
/// \note
///   This namespace exits only in the documentation to collect the operators.
///   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b> and
///   functions \alib{enumops::bitwise;HasBits}, \alib{enumops::bitwise;CountElements},
///   \alib{enumops::bitwise;ToBitwiseEnumeration} and \alib{enumops::bitwise;ToSequentialEnumeration}
///   reside in namespace \ref alib.
///
/// As required, when parsed by a C++ compiler, the operators reside in the global namespace.
ALIB_EXPORT namespace bitwise {
#endif


/// Bitwise \b and operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise and operation of the underlying enum values.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator&  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) & TBits(rhs));
}

/// Bitwise assignment operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs & rhs</c>.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator&=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & TBits(rhs) );
}

/// Bitwise \b or operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise or operation of the underlying enum values.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator|  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/// Bitwise <b>or assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator|=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/// Bitwise \b xor operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise xor operation of the underlying enum values.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator^  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) ^ TBits(rhs));
}

/// Bitwise <b>xor assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs ^ rhs</c>.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator^=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) ^ TBits(rhs) );
}

/// Bitwise \b not operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// \note To remove one or more bits from a scoped enum value, operator <b>&=</b> with this operator
///       applied to \p{op} can be used.
///       A shortcut to this is given with \ref operator-=.
///
/// @tparam TEnum      Enumeration type.
/// @param  op         The operand to be negated.
/// @return The result of a bitwise negation of \p{op}.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr TEnum operator~ (TEnum  op) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( ~ TBits(op) );
}


/// Alias to bitwise \b or operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enumops;ArithmeticalTraits}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of a bitwise or operation of the underlying enum values.
ALIB_EXPORT
template<typename TEnum>
requires (     alib::enumops::IsBitwise     <TEnum>
           && !alib::enumops::IsArithmetical<TEnum> )
constexpr TEnum operator+  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum(TBits(lhs) | TBits(rhs));
}

/// Alias for bitwise <b>or assignment</b> operator usable with scoped enum types.<br>
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enumops;ArithmeticalTraits}
/// is \b not specialized to inherit \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs | rhs</c>.
ALIB_EXPORT
template<typename TEnum>
requires (     alib::enumops::IsBitwise     <TEnum>
           && !alib::enumops::IsArithmetical<TEnum> )
constexpr TEnum operator+=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) | TBits(rhs) );
}

/// Removes bit(s) found in \p{rhs} from \p{lhs} an returns result. This is a shortcut to:
///
///      lhs & !rhs
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type  and if \alib{enumops;ArithmeticalTraits}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The result of <c>lhs & !rhs</c>.
ALIB_EXPORT
template<typename TEnum>
requires (     alib::enumops::IsBitwise     <TEnum>
           && !alib::enumops::IsArithmetical<TEnum> )
constexpr TEnum operator-  (TEnum  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return TEnum( TBits(lhs) & (~TBits(rhs)) );
}

/// Removes bit(s) found in \p{rhs} from \p{lhs}. This is a shortcut to:
///
///      lhs &= !rhs
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type and if \alib{enumops;ArithmeticalTraits}
/// is \b not specialized to inherit  \c std::true_type. The latter is to avoid ambiguities in
/// situations where an enum is both, arithmetical and bitwise.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to <c>lhs & ( ~rhs )</c>.
ALIB_EXPORT
template<typename TEnum>
requires (     alib::enumops::IsBitwise     <TEnum>
           && !alib::enumops::IsArithmetical<TEnum> )
constexpr TEnum operator-=  (TEnum&  lhs, TEnum rhs) noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TBits(lhs) & (~TBits(rhs)) );
}

#if !DOXYGEN
ALIB_EXPORT namespace alib {
#endif

/// Tests if the integral value of the given enum \p{element} contains all bits set in
/// \p{selection}.
/// In other words, returns result of:
///
///       ( element & selection ) == selection
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type. Deduced by the compiler.
/// @param  element    Bitset to be tested.
/// @param  selection  Second operand.
/// @return \c true if all bits of \p{selection} are set in \p{element}.
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr bool HasBits(TEnum  element, TEnum selection)                           noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(element) & TBits(selection) ) == TBits(selection);
}

/// Tests if the integral value of the given enum \p{element} contains at least one of the bits
/// set in \p{selection}.
/// In other words, returns result of:
///
///       ( element & selection ) != 0
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type. Deduced by the compiler.
/// @param  element    Bitset to be tested.
/// @param  selection  Second operand.
/// @return \c true if one of the bits of \p{selection} are set in \p{element}.
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr bool HasOneOf(TEnum  element, TEnum selection)                                                   noexcept
{
    using TBits= typename std::underlying_type<TEnum>::type;
    return ( TBits(element) & TBits(selection) ) != TBits(0);
}

/// Returns the number of bitwise enumeration elements set in the given value.
/// In other words, the bits given in \p value are counted and the number is returned.
///
/// Selected by the compiler only if \alib{enumops;BitwiseTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @param value A single or composite selection of bits.
/// @return The result of a call to #alib::lang::BitCount().
template<typename TEnum>
requires alib::enumops::IsBitwise<TEnum>
constexpr int CountElements( TEnum value )
{ return lang::BitCount(UnderlyingIntegral(value)); }

#if DOXYGEN
}}} // doxygen namespace [alib::enumops::bitwise]
#else
} // namespace [alib]
#endif



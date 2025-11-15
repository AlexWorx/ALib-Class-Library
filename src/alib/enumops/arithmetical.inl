//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::enumops {

//##################################################################################################
// struct ArithmeticalTraits
//##################################################################################################

//==================================================================================================
/// A simple type trait that inherits <c>std::false_type</c> by default. If specialized for an
/// enumeration type \p{TEnum} to inherit <c>std::true_type</c>, the following operators set of
/// \alib operators become applicable to elements of \p{TEnum}:
///
/// - \alib{enumops::arithmetical;operator<}
/// - \alib{enumops::arithmetical;operator<=}
/// - \alib{enumops::arithmetical;operator>}
/// - \alib{enumops::arithmetical;operator>=}
/// - \alib{enumops::arithmetical;operator+}
/// - \alib{enumops::arithmetical;operator-}
/// - \alib{enumops::arithmetical;operator++}
/// - \alib{enumops::arithmetical;operator++(TEnum&;std::underlying_type<TEnum>::type)}
/// - \alib{enumops::arithmetical;operator--}
/// - \alib{enumops::arithmetical;operator--(TEnum&;std::underlying_type<TEnum>::type)}
/// - \alib{enumops::arithmetical;operator+=}
/// - \alib{enumops::arithmetical;operator-=}
///
/// \attention
///   Likewise with the operators introduced with the type trait \alib{enumops;BitwiseTraits},
///   this documentation "fakes" the operators into namespace <c>alib::enumops</c>,
///   while in fact they are defined in the global namespace!<br>
///   See \ref alib_enums_arithmetic_standard "corresponding note" in the Programmer's Manual
///   for details.
///
/// <b>Restrictions</b><br>
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// \see
///   - Macro \ref ALIB_ENUMS_MAKE_ARITHMETICAL, which specializes this type trait for a given
///     enumeration type.
///   - For details and a source code sample see chapter \ref alib_enums_arithmetic_standard
///     of the Programmer's Manual of the module \alib_enumops.
///
/// @tparam TEnum      The enum type to enable arithmetical operators for.
//==================================================================================================
template<typename TEnum>
requires std::is_enum<TEnum>::value
struct ArithmeticalTraits : std::false_type {};

ALIB_WARNINGS_IGNORE_DOCS

/// Concept that is satisfied if the type trait \alib{enumops;ArithmeticalTraits}
/// is specialized for type \p{TEnum} to inherit <c>std::true_type</c>.
/// @tparam TEnum The type to test.
template<typename TEnum>
concept IsArithmetical = ArithmeticalTraits<TEnum>::value;

ALIB_WARNINGS_RESTORE
} // namespace [alib::enumops]


//##################################################################################################
// Arithmetic Operators
//##################################################################################################

// For documentation, all operators and enum related template functions are faked into namespace
// alib::enumops
#if DOXYGEN
namespace alib {  namespace enumops {

/// Operators available to elements of enumerations if \alib{enumops;ArithmeticalTraits} is
/// specialized.
///
/// \note
///   This namespace exits only in the documentation to collect the operators.
///   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b>.
namespace arithmetical {
#endif


/// Comparison operator \e less between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of the comparison.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr bool operator<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)    noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) < rhs ;
}

/// Comparison operator <em>less or equal</em> between an enum element and an integral value of
/// the underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The result of the comparison.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr bool operator<=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs)    noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) <= rhs;
}

/// Comparison operator <em>greater</em> between an enum element and an integral value of
/// underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum      Enumeration type.
/// @param  lhs        First operand.
/// @param  rhs        Second operand.
/// @return The result of the comparison.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr bool operator>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)    noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) > rhs;
}

/// Comparison operator <em>greater or equal</em> between an enum element and an integral value of
/// the underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The result of the comparison.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr bool operator>=  (TEnum lhs, typename std::underlying_type<TEnum>::type rhs)    noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TValue(lhs) >= rhs ;
}


/// Add-operator between two enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator+  (TEnum  lhs, TEnum rhs)                                        noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + TValue(rhs) );
}

/// Add-operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator+  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) + rhs );
}


/// Add-assignment-operator between two enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator+=  (TEnum& lhs, TEnum rhs)                                       noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + TValue(rhs) );
}

/// Add-assignment-operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @tparam TEnum       Enumeration type.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator+=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) + rhs );
}

/// Prefix increment operator.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  arg Reference to the enum value to be incremented.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator++  (TEnum&  arg)                                                 noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) + 1 );
}

/// Postfix increment operator.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  arg Reference to the enum value to be incremented.
/// @return The old value of \p{arg}.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator++  (TEnum&  arg, typename std::underlying_type<TEnum>::type)     noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    TEnum tmp= arg;
    arg= TEnum( TValue(arg) + 1 );
    return tmp;
}

/// Subtract operator between two enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator-  (TEnum  lhs, TEnum rhs)                                        noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - TValue(rhs) );
}


/// Subtract operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator-  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) - rhs );
}

/// Subtract assignment operator between two enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator-=  (TEnum&  lhs, TEnum rhs)                                      noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - TValue(rhs) );
}

/// Subtract assignment operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator-=  (TEnum&  lhs, typename std::underlying_type<TEnum>::type rhs) noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) - rhs );
}

/// Prefix decrement operator.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  arg Reference to the enum value to be decremented.
/// @return The new value of \p{lhs} which is set to the resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator--  (TEnum&  arg)                                                 noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return arg= TEnum( TValue(arg) - 1 );
}


/// Postfix decrement operator.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  arg Reference to the enum value to be decremented.
/// @return The old value of \p{arg}.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator--  (TEnum&  arg, typename std::underlying_type<TEnum>::type)     noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    TEnum tmp= arg;
    arg= TEnum( TValue(arg) - 1 );
    return tmp;
}

/// Unary plus operator for enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  arg         Operand.
/// @return Parameter \p{arg} (identical value).
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator+  (TEnum  arg)                                     noexcept { return arg; }

/// Unary minus operator for enum elements.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  arg         Operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator-  (TEnum  arg)                                                   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( - TValue(arg) );
}


/// Multiplication operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator*  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) * rhs );
}

/// Multiplication assignment operator between an enum element and an integral value of underlying
/// type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator*= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) * rhs );
}

/// Division operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator/  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) / rhs );
}

/// Division assignment operator between an enum element and an integral value of underlying
/// type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator/= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) / rhs );
}


/// Modulo operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator%  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) % rhs );
}

/// Modulo assignment operator between an enum element and an integral value of underlying
/// type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator%= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs)   noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) % rhs );
}


/// Shift-left operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator<<  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)  noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/// Shift-left assignment operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator<<= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs)  noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) << rhs );
}


/// Shift-right operator between an enum element and an integral value of underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param  lhs         First operand.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator>>  (TEnum  lhs, typename std::underlying_type<TEnum>::type rhs)  noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return TEnum( TValue(lhs) << rhs );
}

/// Shift-right assignment operator between an enum element and an integral value of
/// the underlying type.
///
/// Selected by the compiler only if \alib{enumops;ArithmeticalTraits} is specialized for
/// template enum type \p{TEnum} to inherit \c std::true_type.
///
/// @tparam TEnum       Enumeration type.
/// @param[in,out]  lhs Reference to the first operand. Receives the result.
/// @param  rhs         Second operand.
/// @return The resulting enum element.
ALIB_EXPORT
template<typename TEnum>
requires alib::enumops::IsArithmetical<TEnum>
constexpr TEnum operator>>= (TEnum& lhs, typename std::underlying_type<TEnum>::type rhs)  noexcept {
    using TValue= typename std::underlying_type<TEnum>::type;
    return lhs= TEnum( TValue(lhs) >> rhs );
}

// Reset documentation fake
#if DOXYGEN
}}}} // doxygen namespace [alib::enumops::arithmetical]
#endif

//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

ALIB_EXPORT namespace alib {

// For documentation, all operators and enum related template functions are faked into namespace
// alib::enumops
#if DOXYGEN
 namespace enumops{
#endif

/// Simple alias to the nested statement <c>static_cast</c> and type trait
/// <c>std::underlying_type</c>, which
/// - is selected by the compiler only if template parameter \p{TEnum} represents an enum type, and
/// - is probably better readable.
///
/// \alib uses this function only in templated code, where the destination type is not determined.
/// In other cases, \alib chooses to convert enum values directly using function-style casts.
///
/// \note
///   While documented in namespace <c>%alib::enumops</c> in fact this function resides in
///   namespace #alib.
///
/// \see
///   Class \alib{boxing;Enum} introduced with module \alib_boxing.
///
/// @tparam TEnum     Enumeration type. Deduced by the compiler.
/// @param  element   The enumeration element.
/// @return \c true if all bits of \p{testFor} are set in \p{tested}.
template<typename TEnum>
requires std::is_enum_v<TEnum>
std::underlying_type_t<TEnum> constexpr UnderlyingIntegral(TEnum  element) noexcept
{ return static_cast<typename std::underlying_type<TEnum>::type>( element ); }

// Reset documentation fake
#if DOXYGEN
} // namespace alib[::enumops::]
#endif

} // namespace [alib]


/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL
#define HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined(HPP_ALIB_LANG_TMP) && !defined(ALIB_DOX)
#   include "alib/lang/tmp.hpp"
#endif


#define ALIB_ENUMS_UNDERLYING_TYPE(TEnum)  typename std::underlying_type<TEnum>::type

namespace alib {

// For documentation, all operators and enum related template functions are faked into namespace
// alib
#if defined(ALIB_DOX)
 namespace enums {
#endif

#if defined(ALIB_DOX)
/**
 * Simple alias to nested statment <c>static_cast</c> and type traits <c>std::underlying_type</c>,
 * which
 * - is selected by the compiler only if template parameter \p{TEnum} represents an enum type, and
 * - is probably better readable.
 *
 * \note
 *   While documented in namespace <c>%alib::enums</c> in fact this function resides in
 *   namespace #alib.
 *
 * \see
 *   Class \alib{boxing,Enum} introduced with module \alib_boxing.
 *
 * @tparam TEnum     Enumeration type. Deduced by the compiler.
 * @param  element   The enumeration element.
 * @return \c true if all bits of \p{testFor} are set in \p{tested}.
 */
template<typename TEnum>
constexpr
typename std::underlying_type<TEnum>::type    UnderlyingIntegral(TEnum  element) noexcept(true);
#else
template<typename TEnum>
constexpr
ATMP_T_IF(ALIB_ENUMS_UNDERLYING_TYPE(TEnum), std::is_enum<TEnum>::value)
UnderlyingIntegral(TEnum  element) noexcept(true)
{
    return static_cast<typename std::underlying_type<TEnum>::type>( element );
}
#endif

// Reset documentation fake
#if defined(ALIB_DOX)
} // namespace alib[::enums]
#endif

} // namespace [alib]

#endif // HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL

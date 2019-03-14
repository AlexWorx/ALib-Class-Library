// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ENUMS_ENUM_VALUE
#define HPP_ALIB_ENUMS_ENUM_VALUE 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif


#if !defined(_GLIBCXX_TYPE_TRAITS) || !defined(_TYPE_TRAITS_)
#   include <type_traits>
#endif

namespace aworx {

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib
#if ALIB_DOCUMENTATION_PARSER
namespace lib { namespace enums {
#endif

#if ALIB_DOCUMENTATION_PARSER
/**
 * Returns the "underlying" integer value of an enumeration element. This is useful for example,
 * to return result codes to callers of different programming languages or command line programs.
 *
 * Selected by the compiler only if template parameter \p{TEnum} represents an enum type.
 *
 * \note While documented in namespace <c>%aworx::lib::enums</c> in fact this function resides in
 *       namespace #aworx.
 *
 * \see A different, and much more far-reaching approach is implemented with class
 *      \alib{boxing,Enum}.
 *
 * @param  element   The enumeration element.
 * @tparam TEnum     Enumeration type.
 * @tparam TEnableIf Internal. Do \b not specify!<br>
 *                   (Defaults to \c std::enable_if type, to enable the compiler to select this
 *                   operator only for types that have \alib{enums,T_EnumIsBitwise} set.)
 * @return \c true if all bits of \p{testFor} are set in \p{tested}.
 */
template<typename TEnum, typename TEnableIf= void >
constexpr
typename std::underlying_type<TEnum>::type    EnumValue(TEnum  element) noexcept(true);

#else
template<typename TEnum
        ,typename TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type
        >
constexpr
typename std::underlying_type<TEnum>::type    EnumValue(TEnum  element) noexcept(true)
{
    return static_cast<typename std::underlying_type<TEnum>::type>( element );
}
#endif

// Reset documentation fake
#if ALIB_DOCUMENTATION_PARSER
}} // namespace aworx[::lib::enums]
#endif

} // namespace [aworx]

#endif // HPP_ALIB_ENUMS_ENUM_VALUE


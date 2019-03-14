// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL
#define HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)

#if !defined (HPP_ALIB_STRINGS_ASTRING)
    #include "alib/strings/astring.hpp"
#endif

#if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
    #include <unordered_map>
#endif

// #################################################################################################
// #################################################################################################
// #### Namespace dox
// #################################################################################################
// #################################################################################################
namespace aworx { namespace lib { namespace strings {
/**
 * This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
 * module \alibmod_nolink_strings.<br>
 * The entities of those namespaces become available with the inclusion of optional "compatibility"
 * headers found in folder \b "alib/compatibility".
 */
namespace compatibility {

/**
 * This namespace documents compatibility features of \alibmod_nolink_strings and the
 * standard C++ class library found in namespace \c std.
 */
namespace std {
}}}}}


// #################################################################################################
// #################################################################################################
// #### std::hash and std::equal_to for ALib String types
// #################################################################################################
// #################################################################################################
#if ALIB_DOCUMENTATION_PARSER
    namespace aworx { namespace lib { namespace strings { namespace compatibility { namespace std {
#else
    namespace std {
#endif

/** ************************************************************************************************
 * Specialization of functor <c>std::hash</c> for type \alib{strings,TString<TChar>,String}.
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the string.
 **************************************************************************************************/
template<typename TChar>
struct hash<aworx::lib::strings::TString<TChar>>
{
    /**
     * Calculates the hash code for \alib strings.
     * @param src The string object to hash.
     * @return The hash code.
     */
    inline
    std::size_t operator()(const aworx::lib::strings::TString<TChar>& src) const
    {
        std::size_t result= 0xc70f6907UL;

        for (aworx::integer i = 0; i < src.Length(); i++)
            result = 31*result + static_cast<std::size_t>( src.template CharAt<false>(i++) );

        return static_cast<std::size_t>( result );
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::equal_to</c> for type \alib{strings,TString<TChar>,String}.
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 **************************************************************************************************/
template<typename TChar>
struct equal_to<aworx::lib::strings::TString<TChar>>
{
    /**
     * Invokes  \alib{strings,TString::Equals,String::Equals} on \p{lhs}, passing \p{rhs}
     * and returns the result.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::strings::TString<TChar>& lhs,
                    const aworx::lib::strings::TString<TChar>& rhs   ) const
    {
        return lhs.Equals( rhs );
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::less</c> for type \alib{boxing,Box}.
 *
 * For the comparison, box-function \alib{boxing,FIsLess} is invoked on \p{lhs} with passing
 * \p{rhs}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 **************************************************************************************************/
template<typename TChar>
struct less<aworx::lib::strings::TString<TChar>>
{
    /**
     * Invokes box-function \alib{boxing,FIsLess} on \p{lhs} and returns the result.
     * @param lhs The left-hand side box.
     * @param rhs The right-hand side box.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::strings::TString<TChar>& lhs,
                    const aworx::lib::strings::TString<TChar>& rhs) const
    {
        return  lhs <  rhs;
    }
};


/** ************************************************************************************************
 * Functor that can be used as an explicitly given replacement for <c>std::hash</c> with template
 * types (containers) found in namespace \c std.<br>
 * While the specialization of <c>std::hash</c> for type \alib{strings,TString<TChar>,String},
 * which is used by the default values of template arguments of the corresponding types in namespace
 * \c std, performs a case sensitive hash value calculation, this version creates the same
 * hash value for two strings that only differ in the letter case of one or more characters.
 *
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This functor is provided with the inclusion of header file
 * <c>alib/compatibility/std_strings_functional.hpp</c>.
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the string.
 **************************************************************************************************/
template<typename TChar>
struct hash_string_ignore_case
{
    /**
     * Calculates the hash code \alib strings, ignoring the letter case.
     * @param src The string object to hash.
     * @return The hash code.
     */
    inline
    size_t operator()(const aworx::lib::strings::TString<TChar>& src) const
    {
        std::size_t result= 0xc70f6907UL;

        for (aworx::integer i = 0; i < src.Length(); i++)
            result = 31*result + static_cast<std::size_t>( toupper(src.template CharAt<false>(i++)) );

        return static_cast<std::size_t>( result );
    }
};

/** ************************************************************************************************
 * Functor that can be used as an explicitly given replacement for <c>std::equal_to</c> with template
 * types (containers) found in namespace \c std.<br>
 * While the specialization of <c>std::equal_to</c> for type \alib{strings,TString<TChar>,String},
 * which is used by the default values of template arguments of the corresponding types in namespace
 * \c std, performs a case sensitive comparison, this version ignores the letter case of the
 * characters of the given strings.
 *
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This functor is provided with the inclusion of header file
 * <c>alib/compatibility/std_strings_functional.hpp</c>.
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 *                Defaults to \alib{characters,character}.
 **************************************************************************************************/
template<typename TChar= aworx::character>
struct equal_to_string_ignore_case
{
    /**
     * Invokes \alib{strings,TString::Equals,String::Equals} on \p{lhs}, passing \p{rhs}
     * and returns the result.
     *
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::strings::TString<TChar>& lhs,
                    const aworx::lib::strings::TString<TChar>& rhs  ) const
    {
        return lhs.template Equals<aworx::Case::Ignore>( rhs );
    }
};

#if !ALIB_DOCUMENTATION_PARSER
}  namespace aworx {
#endif


/**
 * Simple type definition for a <c>std::unordered_map</c> using a
 * \alib{strings,TString<TChar>,String} as the key type.
 * Note that the corresponding specialization of structs <c>std::hash</c> and <c>std::equal_to</c>
 * are provided, hence other container types of namespace \c std that need such specialization
 * may be used likewise.
 *
 * This type definition, as well as the functors are provided with the inclusion of header
 * file <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace \ref aworx.
 *
 * @tparam TValue The value type of the map.
 * @tparam TChar  The character type of the key string. Defaults to #aworx::character.
 */
template<typename TValue, typename TChar= aworx::character>
using UnorderedStringMap = std::unordered_map< aworx::lib::strings::TString<TChar>,
                                               TValue                                  >;

/**
 * Simple type definition for a <c>std::unordered_map</c> using a
 * \alib{strings,TString<TChar>,String} as the key type, while ignoring the letter case of the
 * characters of that string.
 *
 * The definition uses templated functors
 * \alib{strings::compatibility::std,hash_string_ignore_case} and
 * \alib{strings::compatibility::std,equal_to_string_ignore_case}
 * which may be used for other container types of namespace \c std that need such specialization
 * likewise.
 *
 * This type definition, as well as the functors are provided with the inclusion of header
 * file <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace \ref aworx.
 *
 * @tparam TValue The value type of the map.
 * @tparam TChar  The character type of the key string. Defaults to #aworx::character.
 */
template<typename TValue, typename TChar= aworx::character>
using UnorderedStringMapIgnoreCase= std::unordered_map< aworx::lib::strings::TString<TChar>,
                                                        TValue,
                                                        std::hash_string_ignore_case    <TChar>,
                                                        std::equal_to_string_ignore_case<TChar>  >;

/**
 * Simple type definition for a <c>std::unordered_map</c> using an
 * \alib{strings,TAString<TChar>,AString} as the key type.
 * Note that the specialization of structs <c>std::hash</c> and <c>std::equal_to</c> for
 * base type \alib{strings,TString<TChar>,String} are used.
 * Other container types of namespace \c std that need such specialization for type \b %AString
 * may be declared using these functors of type \b %String likewise.
 *
 * This type definition, as well as the functors are provided with the inclusion of header
 * file <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace \ref aworx.
 *
 * @tparam TValue The value type of the map.
 * @tparam TChar  The character type of the key string. Defaults to #aworx::character.
 */
template<typename TValue, typename TChar= aworx::character>
using UnorderedAStringMap= std::unordered_map< aworx::lib::strings::TAString<TChar>,
                                               TValue,
                                               std::hash    <aworx::lib::strings::TString<TChar>>,
                                               std::equal_to<aworx::lib::strings::TString<TChar>> >;

/**
 * Simple type definition for a <c>std::unordered_map</c> using an
 * \alib{strings,TAString<TChar>,AString} as the key type, while ignoring the letter case of the
 * characters of that string.
 *
 * The definition uses templated functors
 * \alib{strings::compatibility::std,hash_string_ignore_case} and
 * \alib{strings::compatibility::std,equal_to_string_ignore_case}
 * which work on base type \alib{strings,TString<TChar>,String}.
 * Other container types of namespace \c std that need such specialization for type \b %AString
 * may be declared using these functors of type \b %String likewise.
 *
 * Note that the templated structs <c>std::hash</c> and <c>std::equal_to</c> for
 * base type \alib{strings,TString<TChar>,String} are used.
 * Other container types of namespace \c std that need such specialization
 * may be declared using these functors likewise.
 *
 * This type definition, as well as the functors are provided with the inclusion of header
 * file <c>alib/compatibility/std_strings_functional.hpp</c>.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>aworx::lib::strings::compatibility::std</c>,
 *   the true location is namespace \ref aworx.
 *
 * @tparam TValue The value type of the map.
 * @tparam TChar  The character type of the key string. Defaults to #aworx::character.
 */
template<typename TValue, typename TChar= aworx::character>
using UnorderedAStringMapIgnoreCase= std::unordered_map< aworx::lib::strings::TAString<TChar>,
                                                         TValue,
                                                         std::hash_string_ignore_case    <TChar>,
                                                         std::equal_to_string_ignore_case<TChar>  >;

#if ALIB_DOCUMENTATION_PARSER
}}}}} // namespace [aworx::lib::strings::compatibility::std]
#else
} // namespace aworx
#endif

#endif // HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL

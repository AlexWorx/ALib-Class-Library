/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong.<br>
 * With the inclusion of this header compatibility features between \alib and the C++ standard
 * library are provided.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL
#define HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL 1

#if !defined(HPP_ALIB) && !defined(ALIB_DOX)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)

#if !defined (HPP_ALIB_STRINGS_STRING)
    #include "alib/strings/string.hpp"
#endif

#if !defined(_GLIBCXX_FUNCTIONAL) && !defined(_FUNCTIONAL_)
    #include <functional>
#endif


// #################################################################################################
// #################################################################################################
// #### Namespace dox
// #################################################################################################
// #################################################################################################
namespace alib {  namespace strings {
/**
 * This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
 * module \alib_strings_nl.<br>
 * The entities of those namespaces become available with the inclusion of optional "compatibility"
 * headers found in folder \alibsrcdir{compatibility}.
 */
namespace compatibility {

/**
 * This namespace documents compatibility features of \alib_strings_nl and the
 * standard C++ class library found in namespace \c std.
 */
namespace std {
}}}}


// #################################################################################################
// #################################################################################################
// #### std::hash and std::equal_to for ALib String types
// #################################################################################################
// #################################################################################################
#if defined(ALIB_DOX)
    namespace alib {  namespace strings { namespace compatibility { namespace std {
#else
    namespace std {
#endif

/** ************************************************************************************************
 * Specialization of functor <c>std::hash</c> for type \alib{strings,TString<TChar>,String}.
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This specialization is provided with the inclusion of header file
 * \alibheader{compatibility/std_strings_functional.hpp}.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the string.
 **************************************************************************************************/
template<typename TChar>
struct hash<alib::strings::TString<TChar>>
{
    /**
     * Calculates the hash code for \alib strings.
     * @param src The string object to hash.
     * @return The hash code.
     */
    std::size_t operator()(const alib::strings::TString<TChar>& src) const
    {
        return src.Hashcode();
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::equal_to</c> for type \alib{strings,TString<TChar>,String}.
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This specialization is provided with the inclusion of header file
 * \alibheader{compatibility/std_strings_functional.hpp}.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 **************************************************************************************************/
template<typename TChar>
struct equal_to<alib::strings::TString<TChar>>
{
    /**
     * Invokes  \alib{strings,TString::Equals,String::Equals} on \p{lhs}, passing \p{rhs}
     * and returns the result.
     * @param lhs The first string object.
     * @param rhs The second string object.
     * @return The result of the comparison.
     */
    bool operator()(const alib::strings::TString<TChar>& lhs,
                    const alib::strings::TString<TChar>& rhs   ) const
    {
        return lhs.Equals( rhs );
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::less</c> for strings of templated character type.
 *
 * For the comparison, \alib{strings::TString,CompareTo} is invoked on \p{lhs} with passing
 * \p{rhs}.
 *
 * This specialization is provided with the inclusion of header file
 * \alibheader{compatibility/std_strings_functional.hpp}.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 **************************************************************************************************/
template<typename TChar>
struct less<alib::strings::TString<TChar>>
{
    /**
     * Compares the two given strings.
     * @param lhs The left-hand side string.
     * @param rhs The right-hand side string.
     * @return The result of the comparison.
     */
    bool operator()(const alib::strings::TString<TChar>& lhs,
                    const alib::strings::TString<TChar>& rhs) const
    {
        return  lhs.CompareTo(rhs) < 0;
    }
};

#if !defined(ALIB_DOX)
} // namespace [std]
namespace alib {
#endif


/** ************************************************************************************************
 * Functor that can be used as an explicitly given replacement for <c>std::hash</c> with template
 * types (containers) found in namespace \c std.<br>
 * While the specialization of <c>std::hash</c> for type \alib{strings,TString<TChar>,String},
 * which is used by the default values of template arguments of the corresponding types in namespace
 * \c std, performs a case sensitive hash code calculation, this version creates the same
 * hash code for two strings that only differ in the letter case of one or more characters.
 *
 * Note that this specialization can also be used in combination with derived string types, like
 * \alib{strings,TAString<TChar>,AString}.
 *
 * This functor is provided with the inclusion of header file
 * \alibheader{compatibility/std_strings_functional.hpp}.
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the string. Defaults to \alib{characters,character}.
 **************************************************************************************************/
template<typename TChar= characters::character>
struct hash_string_ignore_case
{
    /**
     * Calculates the hash code \alib strings, ignoring the letter case.
     * @param src The string object to hash.
     * @return The hash code.
     */
    size_t operator()(const strings::TString<TChar>& src) const
    {
        return src.HashcodeIgnoreCase();
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
 * \alibheader{compatibility/std_strings_functional.hpp}.
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>alib</c>.
 *
 * @tparam TChar  The character type of the strings to compare.
 *                Defaults to \alib{characters,character}.
 **************************************************************************************************/
template<typename TChar= characters::character>
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
    bool operator()(const strings::TString<TChar>& lhs,
                    const strings::TString<TChar>& rhs  ) const
    {
        return lhs.template Equals<true, lang::Case::Ignore>( rhs );
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::less</c> for strings of templated character type.
 *
 * For the comparison, \alib{strings::TString,CompareTo,CompareTo<Case::Ignore>} is invoked on
 * \p{lhs} with passing \p{rhs}.
 *
 * This specialization is provided with the inclusion of header file
 * \alibheader{compatibility/std_strings_functional.hpp}.<br>
 *
 * \note
 *   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
 *   the true location is namespace <c>std</c> (as mandatory).
 *
 * @tparam TChar  The character type of the strings to compare.
 *                Defaults to \alib{characters,character}.
 **************************************************************************************************/
template<typename TChar= characters::character>
struct less_string_ignore_case
{
    /**
     * Compares the two given strings.
     * @param lhs The left-hand side string.
     * @param rhs The right-hand side string.
     * @return \c true if \p{lhs} is less than \p{rhs}.
     */
    bool operator()(const strings::TString<TChar>& lhs,
                    const strings::TString<TChar>& rhs) const
    {
        return  lhs.template CompareTo<lang::Case::Ignore>( rhs ) < 0;
    }
};


#if defined(ALIB_DOX)
}}}} // namespace [alib::strings::compatibility::std]
#else
} // namespace alib
#endif

#endif // HPP_ALIB_COMPATIBILITY_STD_STRINGS_FUNCTIONAL

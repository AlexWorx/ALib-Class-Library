// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL
#define HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

#if !defined(HPP_ALIB_BOXING_ENUM)
#   include "alib/boxing/enum.hpp"
#endif

#if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
    #include <unordered_map>
#endif


// #################################################################################################
// #################################################################################################
// #### std::hash, std::equal_to, std::is_less for classes Box and Enum
// #################################################################################################
// #################################################################################################
#if ALIB_DOCUMENTATION_PARSER
    namespace aworx { namespace lib { namespace boxing { namespace compatibility { namespace std {
#else
    namespace std {
#endif

/** ************************************************************************************************
 * Specialization of functor <c>std::hash</c> for type \alib{boxing,Box}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct hash<aworx::lib::boxing::Box>
{
    /**
     * Invokes and returns the result of box-function \alib{boxing,FHashcode}.
     * @param src The box object to hash.
     * @return The hash code.
     */
    inline
    size_t operator()(const aworx::lib::boxing::Box& src) const
    {
        return src.Hashcode();
    }
};


/** ************************************************************************************************
 * Specialization of functor <c>std::equal_to</c> for type \alib{boxing,Box}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct equal_to<aworx::lib::boxing::Box>
{
    /**
     * Invokes \alib{boxing,Box::operator==} passing \p{lhs} and \p{rhs}.
     * @param lhs The left-hand side box.
     * @param rhs The right-hand side box.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::boxing::Box& lhs,
                    const aworx::lib::boxing::Box& rhs  ) const
    {
        return  lhs.operator==( rhs );
    }

};

/** ************************************************************************************************
 * Specialization of functor <c>std::less</c> for type \alib{boxing,Box}.
 *
 * For the comparison, box-function \alib{boxing,FIsLess} is invoked on \p{lhs} with passing
 * \p{rhs}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct less<aworx::lib::boxing::Box>
{
    /**
     * Invokes box-function \alib{boxing,FIsLess} on \p{lhs} and returns the result.
     * @param lhs The left-hand side box.
     * @param rhs The right-hand side box.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::boxing::Box& lhs,
                    const aworx::lib::boxing::Box& rhs) const
    {
        return  lhs.operator<( rhs );
    }

};

/** ************************************************************************************************
 * Specialization of functor <c>std::hash</c> for type \alib{boxing,Enum}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct hash<aworx::lib::boxing::Enum>
{
    /**
     * Calculates the hash code for class \b Enum.
     * @param src The \b %Enum object to hash.
     * @return The hash code.
     */
    size_t operator()(const aworx::lib::boxing::Enum& src) const
    {
        return src.Hashcode();
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::equal_to</c> for type \alib{boxing,Enum}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct equal_to<aworx::lib::boxing::Enum>
{
    /**
     * Invokes \alib{boxing,Enum::operator==} on \p{lhs} and \p{rhs}.
     * @param lhs The left-hand side \b %Enum.
     * @param rhs The right-hand side \b %Enum.
     * @return The result of the comparison.
     */
    bool operator()(const aworx::lib::boxing::Enum lhs,
                    const aworx::lib::boxing::Enum rhs) const
    {
        return lhs == rhs;
    }
};

/** ************************************************************************************************
 * Specialization of functor <c>std::less</c> for type \alib{boxing,Enum}.
 *
 * This specialization is provided with the inclusion of header file
 * <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the
 * specialization is (as needed) implemented in namespace <c>std</c>.
 **************************************************************************************************/
template<> struct less<aworx::lib::boxing::Enum>
{
    /**
     * Invokes box-function \alib{boxing,FIsLess} on \p{lhs} and returns the result.
     * @param lhs The left-hand side \b Enum.
     * @param rhs The right-hand side \b Enum.
     * @return The result of the comparison.
     */
    inline
    bool operator()(const aworx::lib::boxing::Enum& lhs,
                    const aworx::lib::boxing::Enum& rhs) const
    {
        return  lhs < rhs;
    }

};

#if ALIB_DOCUMENTATION_PARSER
}}}}} // namespace [aworx::lib::boxing::compatibility::std]
#else
} // namespace std
#endif



namespace aworx {

/**
 * Simple type definition for a <c>std::unordered_map</c> using a \alib{boxing,Box} as the key type.
 * Note that the corresponding specialization of structs <c>std::hash</c> and <c>std::equal_to</c>
 * are provided, hence other container types of namespace \c std that need such specialization
 * may be used likewise.
 *
 * This type definition, as well as the functors are provided with the inclusion of header
 * file <c>alib/compatibility/std_boxing_functional.hpp</c>.<br>
 * While the documentation indicates namespace <c>aworx::lib::compatibility::std</c>, the true
 * definition is located in namespace <c>aworx</c>.
 *
 * @tparam TValue The value type of the map.
 * @tparam TChar  The character type of the key string. Defaults to #aworx::character.
 */
template<typename TValue, typename TChar= aworx::character>
using UnorderedBoxMap = std::unordered_map< aworx::lib::boxing::Box, TValue >;

} // namespace [aworx]

#endif // HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL

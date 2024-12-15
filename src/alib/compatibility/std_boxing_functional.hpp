//==================================================================================================
/// \file
/// This header file is part of the \aliblong.<br>
/// With the inclusion of this header compatibility features between \alib and the C++ standard
/// library are provided.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL
#define HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(BOXING)

#include "alib/boxing/enum.hpp"

// #################################################################################################
// #################################################################################################
// #### std::hash, std::equal_to, std::is_less for classes Box and Enum
// #################################################################################################
// #################################################################################################
#if DOXYGEN
    namespace alib::boxing::compatibility::std {
#else
    namespace std {
#endif

//==================================================================================================
/// Specialization of functor <c>std::hash</c> for type \alib{boxing;Box}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct hash<alib::boxing::Box>
{
    /// Invokes and returns the result of box-function \alib{boxing;FHashcode}.
    /// @param src The box object to hash.
    /// @return The hash code.
    size_t operator()(const alib::boxing::Box& src) const
    {
        return src.Hashcode();
    }
};


//==================================================================================================
/// Specialization of functor <c>std::equal_to</c> for type \alib{boxing;Box}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct equal_to<alib::boxing::Box>
{
    /// Invokes \alib{boxing;Box::operator==} passing \p{lhs} and \p{rhs}.
    /// @param lhs The left-hand side box.
    /// @param rhs The right-hand side box.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Box& lhs,
                    const alib::boxing::Box& rhs  ) const
    {
        return  lhs.operator==( rhs );
    }

};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for type \alib{boxing;Box}.
///
/// For the comparison, box-function \alib{boxing;FIsLess} is invoked on \p{lhs} with passing
/// \p{rhs}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct less<alib::boxing::Box>
{
    /// Invokes box-function \alib{boxing;FIsLess} on \p{lhs} and returns the result.
    /// @param lhs The left-hand side box.
    /// @param rhs The right-hand side box.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Box& lhs,
                    const alib::boxing::Box& rhs) const
    {
        return  lhs.operator<( rhs );
    }

};

//==================================================================================================
/// Specialization of functor <c>std::hash</c> for type \alib{boxing;Enum}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct hash<alib::boxing::Enum>
{
    /// Calculates the hash code for class \b Enum.
    /// @param src The \b %Enum object to hash.
    /// @return The hash code.
    size_t operator()(const alib::boxing::Enum& src) const
    {
        return src.Hashcode();
    }
};

//==================================================================================================
/// Specialization of functor <c>std::equal_to</c> for type \alib{boxing;Enum}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct equal_to<alib::boxing::Enum>
{
    /// Invokes \alib{boxing;Enum::operator==} on \p{lhs} and \p{rhs}.
    /// @param lhs The left-hand side \b %Enum.
    /// @param rhs The right-hand side \b %Enum.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Enum lhs,
                    const alib::boxing::Enum rhs) const
    {
        return lhs == rhs;
    }
};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for type \alib{boxing;Enum}.
///
/// This specialization is provided with the inclusion of header file
/// \alibheader{compatibility/std_boxing_functional.hpp}.<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct less<alib::boxing::Enum>
{
    /// Invokes box-function \alib{boxing;FIsLess} on \p{lhs} and returns the result.
    /// @param lhs The left-hand side \b Enum.
    /// @param rhs The right-hand side \b Enum.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Enum& lhs,
                    const alib::boxing::Enum& rhs) const
    {
        return  lhs < rhs;
    }

};


} // namespace [std]


#endif // HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL


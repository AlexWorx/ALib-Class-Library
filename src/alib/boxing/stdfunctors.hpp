//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if DOXYGEN
    namespace alib::boxing::compatibility::std {
#else
ALIB_EXPORT namespace std {
#endif

//==================================================================================================
/// Specialization of functor <c>std::hash</c> for type #"Box".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct hash<alib::boxing::Box> {
    /// Invokes and returns the result of box-function #"FHashcode".
    /// @param src The box object to hash.
    /// @return The hash code.
    size_t operator()(const alib::boxing::Box& src)                 const { return src.Hashcode(); }
};

//==================================================================================================
/// Specialization of functor <c>std::equal_to</c> for type #"Box".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct equal_to<alib::boxing::Box> {
    /// Invokes #"Box::operator==" passing \p{lhs} and \p{rhs}.
    /// @param lhs The left-hand side box.
    /// @param rhs The right-hand side box.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Box& lhs,
                    const alib::boxing::Box& rhs  )         const { return  lhs.operator==( rhs ); }

};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for type #"Box".
///
/// For the comparison, box-function #"FIsLess" is invoked on \p{lhs} with passing
/// \p{rhs}.
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct less<alib::boxing::Box> {
    /// Invokes box-function #"FIsLess" on \p{lhs} and returns the result.
    /// @param lhs The left-hand side box.
    /// @param rhs The right-hand side box.
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Box& lhs,
                    const alib::boxing::Box& rhs)            const { return  lhs.operator<( rhs ); }

};

//==================================================================================================
/// Specialization of functor <c>std::hash</c> for type #"boxing::Enum".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct hash<alib::boxing::Enum> {
    /// Calculates the hash code for class #"%Enum".
    /// @param src The #"%Enum" object to hash.
    /// @return The hash code.
    size_t operator()(const alib::boxing::Enum& src)                const { return src.Hashcode(); }
};

//==================================================================================================
/// Specialization of functor <c>std::equal_to</c> for type #"boxing::Enum".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct equal_to<alib::boxing::Enum> {
    /// Invokes #"Enum::operator==(const)" on \p{lhs} and \p{rhs}.
    /// @param lhs The left-hand side #"%Enum".
    /// @param rhs The right-hand side #"%Enum".
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Enum lhs,
                    const alib::boxing::Enum rhs)                       const { return lhs == rhs; }
};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for type #"boxing::Enum".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Boxing.StdFunctors.H".<br>
/// While the documentation indicates namespace <c>alib::compatibility::std</c>, the
/// specialization is (as needed) implemented in namespace <c>std</c>.
//==================================================================================================
template<> struct less<alib::boxing::Enum> {
    /// Invokes box-function #"FIsLess" on \p{lhs} and returns the result.
    /// @param lhs The left-hand side #"%Enum".
    /// @param rhs The right-hand side #"%Enum".
    /// @return The result of the comparison.
    bool operator()(const alib::boxing::Enum& lhs,
                    const alib::boxing::Enum& rhs)                      const { return  lhs < rhs; }

};

} // namespace [std]

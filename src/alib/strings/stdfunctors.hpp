//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if DOXYGEN
    namespace alib::strings {
    /// This namespace contains sub-namespaces that provide compatibility of 3rd-party types and
    /// module \alib_strings_nl.<br>
    /// The entities of those namespaces become available with the inclusion of specific headers
    /// that import a certain C++20-Module or inject the functionality into a namespace in a
    /// traditional fashion, for example, header #"F;ALib.Strings.StdFunctors.H".
    namespace compatibility {

    /// This namespace documents compatibility features of \alib_strings_nl and the
    /// standard C++ class library found in namespace \c std.
    namespace std {
    }}}
#endif

//##################################################################################################
// #### std::hash and std::equal_to for ALib string types
//##################################################################################################
#if DOXYGEN
    namespace alib::strings::compatibility::std {
#else
ALIB_EXPORT namespace std {
#endif

//==================================================================================================
/// Specialization of functor <c>std::hash</c> for type #"^String".
/// Note that this specialization can also be used in combination with derived string-types, like
/// #"^AString".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Strings.StdFunctors.H".<br>
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>std</c> (as mandatory).
///
/// @tparam TChar  The character type of the string.
//==================================================================================================
template<typename TChar>
struct hash<alib::strings::TString<TChar>> {
    /// Calculates the hash code for \alib strings.
    /// @param src The string object to hash.
    /// @return The hash code.
    std::size_t operator()(const alib::strings::TString<TChar>& src)                           const
    { return src.Hashcode(); }
};

//==================================================================================================
/// Specialization of functor <c>std::equal_to</c> for type #"^String".
/// Note that this specialization can also be used in combination with derived string-types, like
/// #"^AString".
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Strings.StdFunctors.H".<br>
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>std</c> (as mandatory).
///
/// @tparam TChar  The character type of the strings to compare.
//==================================================================================================
template<typename TChar>
struct equal_to<alib::strings::TString<TChar>>
{
    /// Invokes #"TString::Equals;String::Equals" on \p{lhs}, passing \p{rhs}
    /// and returns the result.
    /// @param lhs The first string object.
    /// @param rhs The second string object.
    /// @return The result of the comparison.
    bool operator()(const alib::strings::TString<TChar>& lhs,
                    const alib::strings::TString<TChar>& rhs   )                               const
    { return lhs.Equals( rhs ); }
};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for strings of templated character type.
///
/// For the comparison, #"TString::CompareTo(const TString< TChar >&)" is invoked on \p{lhs}
/// with passing \p{rhs}.
///
/// This specialization is provided with the inclusion of the header-file
/// #"F;ALib.Strings.StdFunctors.H".<br>
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>std</c> (as mandatory).
///
/// @tparam TChar  The character type of the strings to compare.
//==================================================================================================
template<typename TChar>
struct less<alib::strings::TString<TChar>>
{
    /// Compares the two given strings.
    /// @param lhs The left-hand side string.
    /// @param rhs The right-hand side string.
    /// @return The result of the comparison.
    bool operator()(const alib::strings::TString<TChar>& lhs,
                    const alib::strings::TString<TChar>& rhs)                                  const
    { return  lhs.CompareTo(rhs) < 0; }
};

#if !DOXYGEN
} // namespace [std]
ALIB_EXPORT namespace alib {
#endif

//==================================================================================================
/// Functor that can be used as an explicitly given replacement for <c>std::hash</c> with template
/// types (containers) found in namespace \c std.<br>
/// While the specialization of <c>std::hash</c> for type #"^String",
/// which is used by the default values of template arguments of the corresponding types in namespace
/// \c std, performs a case-sensitive hash code calculation, this version creates the same
/// hash code for two strings that only differ in the letter case of one or more characters.
///
/// Note that this specialization can also be used in combination with derived string-types, like
/// #"^AString".
///
/// This functor is provided with the inclusion of header-file
/// #"F;ALib.Strings.StdFunctors.H".
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>std</c> (as mandatory).
///
/// @tparam TChar  The character type of the string. Defaults to #"characters::character".
//==================================================================================================
template<typename TChar= characters::character>
struct hash_string_ignore_case {
    /// Calculates the hash code \alib strings, ignoring the letter case.
    /// @param src The string object to hash.
    /// @return The hash code.
    size_t operator()(const strings::TString<TChar>& src) const { return src.HashcodeIgnoreCase(); }
};

//==================================================================================================
/// Functor that can be used as an explicitly given replacement for <c>std::equal_to</c> with template
/// types (containers) found in namespace \c std.<br>
/// While the specialization of <c>std::equal_to</c> for type #"^String",
/// which is used by the default values of template arguments of the corresponding types in namespace
/// \c std, performs a case-sensitive comparison, this version ignores the letter case of the
/// characters of the given strings.
///
/// Note that this specialization can also be used in combination with derived string-types, like
/// #"^AString".
///
/// This functor is provided with the inclusion of header-file
/// #"F;ALib.Strings.StdFunctors.H".
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>alib</c>.
///
/// @tparam TChar  The character type of the strings to compare.
///                Defaults to #"characters::character".
//==================================================================================================
template<typename TChar= characters::character>
struct equal_to_string_ignore_case {
    /// Invokes #"TString::Equals;String::Equals" on \p{lhs}, passing \p{rhs}
    /// and returns the result.
    ///
    /// @param lhs The first string object.
    /// @param rhs The second string object.
    /// @return The result of the comparison.
    bool operator()(const strings::TString<TChar>& lhs,
                    const strings::TString<TChar>& rhs  )                                      const
    { return lhs.template Equals<alib::CHK, lang::Case::Ignore>( rhs ); }
};

//==================================================================================================
/// Specialization of functor <c>std::less</c> for strings of templated character type.
///
/// For the comparison, #"CompareTo(const TString&, integer, integer);CompareTo<Case::Ignore>" is
/// invoked on \p{lhs} with passing \p{rhs}.
///
/// This specialization is provided with the inclusion of header-file
/// #"F;ALib.Strings.StdFunctors.H".<br>
///
/// \note
///   While the documentation indicates namespace <c>alib::strings::compatibility::std</c>,
///   the true location is namespace <c>std</c> (as mandatory).
///
/// @tparam TChar  The character type of the strings to compare.
///                Defaults to #"characters::character".
//==================================================================================================
template<typename TChar= characters::character>
struct less_string_ignore_case {
    /// Compares the two given strings.
    /// @param lhs The left-hand side string.
    /// @param rhs The right-hand side string.
    /// @return \c true if \p{lhs} is less than \p{rhs}.
    bool operator()(const strings::TString<TChar>& lhs,
                    const strings::TString<TChar>& rhs)                                        const
    { return  lhs.template CompareTo<lang::Case::Ignore>( rhs ) < 0; }
};

} // namespace alib

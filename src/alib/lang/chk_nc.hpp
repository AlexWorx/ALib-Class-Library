//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {
/// This tag-type is used across \alib to indicate that no check of the input parameters should
/// be performed.
/// Methods affected expose a template parameter named \p{TCheck} which usually defaults to
/// this types' counterpart #"CHK;2".
///
/// For example, various methods of the \alib string types, which perform range-checks on
/// string-indices, use this template parameter.
/// With debug-compilations the checks are still made, and if they fail, the parameters are not
/// corrected, but instead an \alib_assertion is raised.
/// In release builds, however, no checks are performed, which improves execution speed.
///
/// A programmer does not notice the existence of this option, unless he consults the
/// corresponding method's reference documentation, because of the default-value #"CHK;2".
/// Only with code critical to performance or size, the parameter might be crucial to give.
///
/// \note This type, as well as sibling type #"CHK;2" should rather reside in namespace
///       #"alib::lang;2", but was placed in the libraries root namespace for convenience and
///       shortness of string-concatenation sequences of the using-code.
struct NC   : std::false_type {};

/// See sibling type #"NC".
struct CHK  : std::true_type {};
} // namespace [alib]

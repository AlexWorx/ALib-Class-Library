//==================================================================================================
/// \file
/// This header file is part of the \aliblong. It does not belong to an \alibmod and is
/// included in any \alibdist.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_STRINGS_ASTRINGMA
#define HPP_ALIB_MONOMEM_STRINGS_ASTRINGMA
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
ALIB_ASSERT_MODULE(STRINGS)
#include "alib/monomem/monoallocator.hpp"

#if !DOXYGEN

#define HPP_ALIB_STRINGS_TASTRING_INLINING

#define HPP_ALIB_STRINGS_TASTRING_INLINING
#include "alib/strings/detail/tastring.inl"
#include "alib/strings/format.inl"
#ifndef HPP_ALIB_STRINGS_TASTRING_INLINING
#define HPP_ALIB_STRINGS_TASTRING_INLINING
#endif

#define ASTR_TALLOC_SPEC    MonoAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_X
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    MonoAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    MonoAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.inl"

#endif //if !DOXYGEN


namespace alib
{

/// Type alias in namespace \b alib.
using  AStringMA          =     strings::TAString<character     , MonoAllocator >;

/// Type alias in namespace \b alib.
using  StrangeAStringMA   =     strings::TAString<strangeChar   , MonoAllocator >;

/// Type alias in namespace \b alib.
using  ComplementAStringMA=     strings::TAString<complementChar, MonoAllocator >;

/// Type alias in namespace \b alib.
using  NAStringMA         =     strings::TAString<nchar         , MonoAllocator >;

/// Type alias in namespace \b alib.
using  WAStringMA         =     strings::TAString<wchar         , MonoAllocator >;

/// Type alias in namespace \b alib.
using  XAStringMA         =     strings::TAString<xchar         , MonoAllocator >;


/// Type alias in namespace \b alib.
using  StringMALengthResetter        =     strings::TStringLengthResetter<character     , MonoAllocator>;

/// Type alias in namespace \b alib.
using  ComplementStringMALengthResetter=   strings::TStringLengthResetter<complementChar, MonoAllocator>;

/// Type alias in namespace \b alib.
using  StrangeStringMALengthResetter =     strings::TStringLengthResetter<strangeChar   , MonoAllocator>;

/// Type alias in namespace \b alib.
using  NStringMALengthResetter       =     strings::TStringLengthResetter<nchar         , MonoAllocator>;

/// Type alias in namespace \b alib.
using  WStringMALengthResetter       =     strings::TStringLengthResetter<wchar         , MonoAllocator>;

/// Type alias in namespace \b alib.
using  XStringMALengthResetter       =     strings::TStringLengthResetter<xchar         , MonoAllocator>;


} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_STRINGS_ASTRINGMA


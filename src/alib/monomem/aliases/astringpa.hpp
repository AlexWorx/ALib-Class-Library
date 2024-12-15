//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_STRINGS_ASTRINGPA
#define HPP_ALIB_MONOMEM_STRINGS_ASTRINGPA
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
ALIB_ASSERT_MODULE(STRINGS)
#include "alib/monomem/poolallocator.hpp"

#if !DOXYGEN

#define HPP_ALIB_STRINGS_TASTRING_INLINING
#include "alib/strings/detail/tastring.inl"
#include "alib/strings/format.inl"
#ifndef HPP_ALIB_STRINGS_TASTRING_INLINING
#define HPP_ALIB_STRINGS_TASTRING_INLINING
#endif

#define ASTR_TALLOC_SPEC    PoolAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_X
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    PoolAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    PoolAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.inl"

#endif //if !DOXYGEN

namespace alib
{

/// Type alias in namespace \b alib.
using  AStringPA          =     strings::TAString<character     , PoolAllocator >;

/// Type alias in namespace \b alib.
using  StrangeAStringPA   =     strings::TAString<strangeChar   , PoolAllocator >;

/// Type alias in namespace \b alib.
using  ComplementAStringPA=     strings::TAString<complementChar, PoolAllocator >;

/// Type alias in namespace \b alib.
using  NAStringPA         =     strings::TAString<nchar         , PoolAllocator >;

/// Type alias in namespace \b alib.
using  WAStringPA         =     strings::TAString<wchar         , PoolAllocator >;

/// Type alias in namespace \b alib.
using  XAStringPA         =     strings::TAString<xchar         , PoolAllocator >;


/// Type alias in namespace \b alib.
using  StringPALengthResetter        =     strings::TStringLengthResetter<character     , PoolAllocator>;

/// Type alias in namespace \b alib.
using  ComplementStringPALengthResetter=   strings::TStringLengthResetter<complementChar, PoolAllocator>;

/// Type alias in namespace \b alib.
using  StrangeStringPALengthResetter =     strings::TStringLengthResetter<strangeChar   , PoolAllocator>;

/// Type alias in namespace \b alib.
using  NStringPALengthResetter       =     strings::TStringLengthResetter<nchar         , PoolAllocator>;

/// Type alias in namespace \b alib.
using  WStringPALengthResetter       =     strings::TStringLengthResetter<wchar         , PoolAllocator>;

/// Type alias in namespace \b alib.
using  XStringPALengthResetter       =     strings::TStringLengthResetter<xchar         , PoolAllocator>;


} // namespace [alib]


#endif // HPP_ALIB_MONOMEM_STRINGS_ASTRINGPA


//==================================================================================================
/// \file
/// This header file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_STRINGS_ASTRING
#define HPP_ALIB_STRINGS_ASTRING 1
#pragma once

#define HPP_ALIB_STRINGS_TASTRING_INLINING

#include "alib/strings/detail/tastring.inl"
#include "alib/strings/format.inl"

#if !DOXYGEN

#define HPP_ALIB_STRINGS_TASTRING_INLINING

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_X
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.inl"

#undef HPP_ALIB_STRINGS_TASTRING_INLINING
#endif //if !DOXYGEN


#endif // HPP_ALIB_STRINGS_ASTRING


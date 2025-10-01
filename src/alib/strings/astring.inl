//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#define ALIB_STRINGS_TASTRING_INSTANTIATION

#if !DOXYGEN

#define ALIB_STRINGS_TASTRING_INSTANTIATION

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_X
#include "alib/strings/detail/tastring_ca_decl.inl.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.inl.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.inl.inl"

#undef ALIB_STRINGS_TASTRING_INSTANTIATION
#endif //if !DOXYGEN




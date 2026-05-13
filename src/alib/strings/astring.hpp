//==================================================================================================
/// \file
/// This header-file is part of module \alib_strings of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#define ALIB_STRINGS_TASTRING_INSTANTIATION

#if !DOXYGEN

#define ALIB_STRINGS_TASTRING_INSTANTIATION

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_X
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_W
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#define ASTR_TALLOC_SPEC    lang::HeapAllocator
#define ASTR_TCHAR_SPEC     ALIB_CHAR_TYPE_ID_N
#include "alib/strings/detail/tastring_ca_decl.hpp.inl"

#undef ALIB_STRINGS_TASTRING_INSTANTIATION
#endif //if !DOXYGEN

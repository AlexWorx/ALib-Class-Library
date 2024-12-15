// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"


#if !DOXYGEN

#include "alib/strings/astring.hpp"
#include "alib/monomem/poolallocator.hpp"


#define HPP_ALIB_STRINGS_TASTRING_INLINING
#   include "alib/strings/detail/tastringimpl.inl"
#   undef  HPP_ALIB_STRINGS_TASTRING_INLINING
#endif

#define HPP_ALIB_STRINGS_TASTRING_INLINING
#   define ASTR_TALLOC_SPEC     MonoAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     MonoAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     MonoAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl"


#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl"

#undef HPP_ALIB_STRINGS_TASTRING_INLINING

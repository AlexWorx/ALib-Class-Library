// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN

#define HPP_ALIB_STRINGS_TASTRING_INLINING
#    include "alib/strings/detail/tastringimpl.inl"
#undef  HPP_ALIB_STRINGS_TASTRING_INLINING

// #################################################################################################
// Template instantiations
// #################################################################################################

#define HPP_ALIB_STRINGS_TASTRING_INLINING

// the order of instantiations needs to be kept likewise!
#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl"

#   define ASTR_TALLOC_SPEC     lang::HeapAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl"


#undef HPP_ALIB_STRINGS_TASTRING_INLINING


namespace alib::strings {


} // namespace [alib::strings]


#endif // !DOXYGEN


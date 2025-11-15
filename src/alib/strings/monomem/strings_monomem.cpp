//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/monomem/monomem.prepro.hpp"
#if ALIB_MONOMEM && ALIB_DEBUG
#   include <cstring>
#endif
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Strings;
    import   ALib.Lang;
    import   ALib.Monomem;
#else
#   include "ALib.Strings.H"
#   include "ALib.Lang.H"
#   include "ALib.Monomem.H"
#endif

//========================================== Implementation ========================================
#if ALIB_MONOMEM && ALIB_DEBUG && !DOXYGEN

//##################################################################################################
// Instantiation of monomem::DbgDumpStatistics for TMonoAllocator<lang::HeapAllocator>.
//##################################################################################################
#include "alib/strings/monomem/strings_monomem.t.inl"

namespace alib::monomem {
    template ALIB_DLL NAString monomem::DbgDumpStatistics(TMonoAllocator<lang::HeapAllocator> &ma);
}

#endif //ALIB_MONOMEM && ALIB_DEBUG && !DOXYGEN

//==================================================================================================
//==== AString versions using Mono- and PoolAllocator (instantiations)
//==================================================================================================
#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   include "alib/strings/detail/tastringimpl.inl"

#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"


#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.inl.inl"

#undef ALIB_STRINGS_TASTRING_INSTANTIATION

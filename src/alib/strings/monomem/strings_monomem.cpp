#if ALIB_MONOMEM && ALIB_DEBUG && !DOXYGEN

//##################################################################################################
// Instantiation of monomem::DbgDumpStatistics for TMonoAllocator<lang::HeapAllocator>.
//##################################################################################################
#include "alib/strings/monomem/strings_monomem.t.hpp"

namespace alib::monomem {
    template ALIB_DLL NAString monomem::DbgDumpStatistics(TMonoAllocator<lang::HeapAllocator> &ma);
}

#endif //ALIB_MONOMEM && ALIB_DEBUG && !DOXYGEN

//==================================================================================================
//==== AString versions using Mono- and PoolAllocator (instantiations)
//==================================================================================================
#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   include "alib/strings/detail/tastringimpl.hpp"

#define ALIB_STRINGS_TASTRING_INSTANTIATION
#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"

#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"

#   define ASTR_TALLOC_SPEC     monomem::TMonoAllocator<lang::HeapAllocator>
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"


#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_X
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_W
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"

#   define ASTR_TALLOC_SPEC     PoolAllocator
#   define ASTR_TCHAR_SPEC      ALIB_CHAR_TYPE_ID_N
#   include "alib/strings/detail/tastring_ca_impl.hpp.inl"

#undef ALIB_STRINGS_TASTRING_INSTANTIATION

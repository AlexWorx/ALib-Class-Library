// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/monomem/monomem.prepro.hpp"
#if ALIB_DEBUG_MEMORY
#   include <atomic>
#endif
#if ALIB_DEBUG_MEMORY && ALIB_DEBUG_ALLOCATIONS
#   pragma message "Symbols ALIB_DEBUG_MEMORY and ALIB_DEBUG_ALLOCATIONS are set in parallel. Statistics on allocatoions will be fairly wrong."
#endif

#include <cstring>
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Monomem;
#else
#   include "ALib.Monomem.H"
#endif
// ======================================   Implementation   =======================================

namespace alib {
/// This \alibmod implements the concept of <em>"monotonic allocation"</em> by providing the central
/// classes \alib{monomem;TMonoAllocator} and \alib{monomem;TPoolAllocator}.
///
/// Please consult \ref alib_mods_contmono "ALib Module Memory - Programmer's Manual"
/// for further information.
namespace monomem {
#if !DOXYGEN
                 TMonoAllocator<lang::HeapAllocator>   GLOBAL_ALLOCATOR(ALIB_DBG("Global",) nullptr);
#   if !ALIB_SINGLE_THREADED
                 RecursiveLock                         GLOBAL_ALLOCATOR_LOCK;
#   endif
#endif
}}


#if !DOXYGEN
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE] )
// #################################################################################################
// Instantiation of MonoAllocator, which is TMonoAllocator<lang::HeapAllocator>.
// #################################################################################################
#include "alib/monomem/monoallocator.t.inl"

namespace alib::monomem {  template ALIB_DLL class TMonoAllocator<lang::HeapAllocator>;  }
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE] )

#endif // !DOXYGEN


#if !DOXYGEN
// Including the definition of the non-inlined methods of TPoolAllocator.
// After that, the two predefined versions are explicitly instantiated.
#include "alib/monomem/poolallocator.t.inl"

namespace alib::monomem {

template ALIB_DLL class TPoolAllocator<lang::HeapAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
template ALIB_DLL class TPoolAllocator<      MonoAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;

} // namespace [alib::monomem]

#endif // !DOXYGEN

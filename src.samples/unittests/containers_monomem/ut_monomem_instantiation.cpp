
//DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE_IMPL] )
// ================================================================================================#
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// ================================================================================================#
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/monomem/monomem.prepro.hpp"

// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.Monomem;
#else
#   include "ALib.Monomem.H"
#endif

// =================================================================================================
// Instantiation of TMonoAllocator for PoolAllocatorHA and its inner type
// detail::MAFields<PoolAllocatorHA>.
// =================================================================================================

// Under windows, we cannot do the following within the unit tests project, which imports the
// ALib as a DLL. With Windows OS, it has to be done in an external lib.
#if !defined(_WIN32)

#include "alib/monomem/monoallocator.t.inl"

namespace alib::monomem {  template class TMonoAllocator<alib::PoolAllocatorHA>;  }

#endif // !defined(_WIN32)

//DOX_MARKER([ALIB_MANUAL_T_HPP_FILE_USAGE_IMPL])


// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "alib/monomem/poolallocator.hpp"
#if !DOXYGEN

#if ALIB_DEBUG_MONOMEM && ALIB_DEBUG_ALLOCATIONS
#   pragma message "Symbols ALIB_DEBUG_MONOMEM and ALIB_DEBUG_ALLOCATIONS are set in parallel. Statistics on allocatoions will be fairly wrong."
#endif

// Including the definintions of the non-inlined methods of TPoolAllocator.
// After that, the two predefined versions are explicitly instantiated.

#include "alib/monomem/poolallocator.t.hpp"

namespace alib::monomem {

template ALIB_API class TPoolAllocator<lang::HeapAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
template ALIB_API class TPoolAllocator<      MonoAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;

} // namespace [alib::monomem]

#endif // !DOXYGEN

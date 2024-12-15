// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if ALIB_DEBUG_MONOMEM
#   include <atomic>
#endif
#if ALIB_DEBUG_MONOMEM && ALIB_DEBUG_ALLOCATIONS
#   pragma message "Symbols ALIB_DEBUG_MONOMEM and ALIB_DEBUG_ALLOCATIONS are set in parallel. Statistics on allocatoions will be fairly wrong."
#endif

namespace alib {
/// This \alibmod implements the concept of <em>"monotonic allocation"</em> by providing the central
/// classes \alib{monomem;TMonoAllocator} and \alib{monomem;TPoolAllocator}.
///
/// Please consult \ref alib_mods_contmono "ALib Module Memory - Programmer's Manual"
/// for further information.
namespace monomem {}
}


#if !DOXYGEN

DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE] )
// #################################################################################################
// Instantiation of MonoAllocator, which is TMonoAllocator<lang::HeapAllocator>.
// #################################################################################################
#include "alib/monomem/monoallocator.t.hpp"

namespace alib::monomem {  template ALIB_API class TMonoAllocator<lang::HeapAllocator>;  }
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE] )

#include "alib/monomem/globalallocator.hpp"

// #################################################################################################
// Globals
// #################################################################################################
namespace alib::monomem {

                 MonoAllocator   GLOBAL_ALLOCATOR(ALIB_DBG("Global",) nullptr);
IF_ALIB_THREADS( RecursiveLock   GLOBAL_ALLOCATOR_LOCK;              )

} // namespace [alib::monomem]

#endif // !DOXYGEN

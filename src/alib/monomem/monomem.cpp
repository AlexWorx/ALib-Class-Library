
namespace alib {
/// This \alibmod implements the concept of <em>"monotonic allocation"</em> by providing the central
/// classes #"TMonoAllocator" and #"TPoolAllocator".
///
/// Please consult #"alib_mods_contmono;ALib Module Memory - Programmer's Manual"
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
//##################################################################################################
// Instantiation of MonoAllocator, which is TMonoAllocator<lang::HeapAllocator>.
//##################################################################################################
#include "alib/monomem/monoallocator.t.hpp"

namespace alib::monomem {  template ALIB_DLL class TMonoAllocator<lang::HeapAllocator>;  }
DOX_MARKER( [ALIB_MANUAL_T_HPP_FILE_USAGE] )

#endif // !DOXYGEN


#if !DOXYGEN
// Including the definition of the non-inlined methods of TPoolAllocator.
// After that, the two predefined versions are explicitly instantiated.
#include "alib/monomem/poolallocator.t.hpp"

namespace alib::monomem {

template ALIB_DLL class TPoolAllocator<lang::HeapAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;
template ALIB_DLL class TPoolAllocator<      MonoAllocator, ALIB_MONOMEM_POOLALLOCATOR_DEFAULT_ALIGNMENT>;

} // namespace [alib::monomem]

#endif // !DOXYGEN

//==================================================================================================
/// \file
/// This header file is part of module \alib_monomem of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_MONOMEM_GLOBALALLOCATOR
#define HPP_ALIB_MONOMEM_GLOBALALLOCATOR 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(MONOMEM)
#include "alib/monomem/monoallocator.hpp"

#if ALIB_THREADS
#   include "alib/threads/recursivelock.hpp"
#endif

namespace alib::monomem {

/// This is the global monotonic allocator singleton instance.
/// It's initial size defaults to \  128 kilobytes. This can be tweaked by performing a
/// placement-new on this instance before  \ref alib_manual_bootstrapping "bootstrapping ALib".
///
/// \see
///  - Chapter \ref alib_contmono_globalinstance of the Programmer's Manual of this \alibmod_nl.
///  - \b Lock instance \alib{monomem::GLOBAL_ALLOCATOR_LOCK}
extern ALIB_API     MonoAllocator       GLOBAL_ALLOCATOR;

#if ALIB_THREADS
    /// This <em>mutex</em> is used to protect the \alib{monomem;GLOBAL_ALLOCATOR} from racing
    /// conditions in multithreaded software.<br>
    /// If compiler symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is set, this lock will be attached
    /// to the \alib{lang;DbgCriticalSections} instance in #GLOBAL_ALLOCATOR during bootstrap.
    /// Thus, an assertion will be raised if the \alib{monomem;GLOBAL_ALLOCATOR} is used without
    /// locking this mutex.<br>
    /// This is a recursive lock.
    /// This allows using the \alib{lang;AllocatorInterface;high-level allocation interface} with
    /// allocations, which constructs objects.
    /// These objects might recursively use the global allocator for their members.
    extern ALIB_API    RecursiveLock    GLOBAL_ALLOCATOR_LOCK;
#endif

} // namespace [alib::monomem]

#endif // HPP_ALIB_MONOMEM_GLOBALALLOCATOR


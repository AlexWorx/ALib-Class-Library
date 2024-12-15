//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_DETAIL_THREADMAP
#define HPP_ALIB_THREADS_DETAIL_THREADMAP 1
#pragma once
#include "alib/threads/thread.hpp"
#include <mutex>

#if ALIB_MONOMEM && ALIB_CONTAINERS
#   include "alib/monomem/monoallocator.hpp"
#   include "alib/containers/hashtable.hpp"
#else
#   include <unordered_map>
#endif



namespace alib {  namespace threads { namespace detail {

#if ALIB_MONOMEM && ALIB_CONTAINERS

    /// The hash map used to find the current thread object.
    /// The object uses default values for \alib{containers;HashTable::BaseLoadFactor} and
    /// \alib{containers;HashTable::MaxLoadFactor}. Depending on the use of threads by a
    /// using application, these values might be modified \e before starting the first phase of
    /// \ref alib_manual_bootstrapping "bootstrapping ALib".
    ///
    /// If so, an initial call to \alib{containers;HashTable::Reserve} may be likewise be performed.
    ///
    /// \note
    ///   With the absence of module \alib_monomem in the \alibdist, the type of this object
    ///   is <c>std::unordered_map</c>.
    extern ALIB_API HashMap<MonoAllocator, std::thread::id, Thread*>    THREAD_MAP;

#else
    extern ALIB_API std::unordered_map<    std::thread::id, Thread*>    THREAD_MAP;
#endif

#if (!ALIB_MONOMEM || !ALIB_CONTAINERS) || DOXYGEN
/// The internal mutex used with operations like starting, ending or finding thread objects.
///
/// \note
///   With the presence of module \alib_monomem in the \alibdist, this object is not available.
///   Instead, \alib{monomem;GLOBAL_ALLOCATOR_LOCK} is used, because this likewise protects
///   modifications on object \alib{threads;detail::THREAD_MAP}, which uses this allocator.
extern ALIB_API std::mutex                                          MODULE_LOCK;
#endif

/// Internal method to start a thread.
/// @param thread The \alib thread object to start.
void threadStart( Thread* thread );

}}} // namespace [alib::threads::detail]

#endif // HPP_ALIB_THREADS_DETAIL_THREADMAP


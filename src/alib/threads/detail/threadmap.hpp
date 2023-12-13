/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_threads of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_THREADS_DETAIL_THREADMAP
#define HPP_ALIB_THREADS_DETAIL_THREADMAP 1

#if !defined(HPP_ALIB_THREADS_THREAD)
#   include "alib/threads/thread.hpp"
#endif

#if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
#   include <mutex>
#endif

#if ALIB_MONOMEM
#   if !defined(HPP_ALIB_MONOMEM_HASHMAP)
#      include "alib/monomem/hashmap.hpp"
#   endif
#else
#   if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#      include <unordered_map>
#   endif
#endif



namespace aworx { namespace lib { namespace threads { namespace detail {

#if ALIB_MONOMEM

    /**
     * The hash map used to find the current thread object.
     * The object uses default values for \alib{monomem,HashTable::BaseLoadFactor} and
     * \alib{monomem,HashTable::MaxLoadFactor}. Depending on the use of threads by a
     * using application, these values might be modified \e prior to starting the first phase of
     * \ref alib_manual_bootstrapping "bootstrapping ALib".
     *
     * If so, an initial call to \alib{monomem,HashTable::Reserve} may be likewise be performed.
     *
     * \note
     *   With the absence of module \alib_monomem in the \alibdist, the type of this object
     *   is <c>std::unordered_map</c>.
     */
    extern ALIB_API HashMap           <std::thread::id, Thread*>    threadMap;

#else
    extern ALIB_API std::unordered_map<std::thread::id, Thread*>    threadMap;
#endif

#if !ALIB_MONOMEM || defined(ALIB_DOX)
/**
 * The internal mutex used with operations like starting, ending or finding thread objects.
 *
 * \note
 *   With the presence of module \alib_monomem in the \alibdist, this object is not available.
 *   Instead, \alib{monomem,GlobalAllocatorLock} is used, because this likewise protects
 *   modifications on object \alib{threads,detail::threadMap}, which uses this allocator.
 */
extern ALIB_API std::mutex                                      moduleLock;
#endif


/**
 * Retrieves the \alib object associated with the given C++ 11 thread id.
 * In case the threadID is not found in the hash table, the given id is considered a system
 * thread and is entered into the hash table.
 *
 * @param c11ID  The native thread id.
 * @return The associated \alib thread object.
 */
ALIB_API Thread* getThread(std::thread::id c11ID );

/**
 * Internal method to start a thread.
 * @param thread The \alib thread object to start.
 */
void threadStart( Thread* thread );

}}}} // namespace [aworx::lib::threads::detail]

#endif // HPP_ALIB_THREADS_DETAIL_THREADMAP

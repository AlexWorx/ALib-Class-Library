//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_RECURSIVELOCK
#define HPP_ALIB_THREADS_RECURSIVELOCK 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(THREADS)
#if ALIB_DEBUG
#   include "alib/threads/dbglockasserter.hpp"
#endif
#if ALIB_DEBUG_CRITICAL_SECTIONS
#   include "alib/lang/dbgcriticalsections.hpp"
#endif

#include <mutex>

namespace alib {  namespace threads {

// =================================================================================================
/// This class is a simple wrapper around C++ standard library type \c std::recursive_mutex.
/// Thus, it is used to implement <em>mutual exclusive access</em> to resources by protecting
/// critical code sections from being executed in parallel in concurrent threads.
///
/// When a pair of #AcquireRecursive and #ReleaseRecursive invocations is performed within the same
/// code block, then it is recommended to use a stack instantiation of class
/// \alib{lang;OwnerRecursive} to acquire and release objects of this class.
/// Such a use is highly simplified with macros \ref ALIB_LOCK_RECURSIVE and
/// \ref ALIB_LOCK_RECURSIVE_WITH.
///
/// Nested acquisitions are supported with this type.
/// An instance of this class is released when an equal number of invocations to #AcquireRecursive
/// and #ReleaseRecursive have been performed.<br>
/// This class has slightly reduced performance in comparison to non-recursive type
/// \alib{threads;Lock}. Not only for this reason, non-nested locking is the preferred technique.
/// But there are situations where nested locks are just unavoidable.
///
/// \par Debug-Features
/// Public member #Dbg is available with debug-compilations. It offers the following features:
/// - The object stores the actual owning thread and the source code position of the last
///   acquirement.
/// - Releasing non-acquired instances, as well as destructing acquired one, raise an assertion.
/// - A warning threshold for the number of nested acquisitions can be defined with public member
///   \alib{threads;DbgLockAsserter::RecursionLimit}.
/// - Field \alib{threads;DbgLockAsserter;WaitTimeLimit} enables the raise of \alib warnings in case a
///   certain wait time is exceeded.
///   Note that instead of wrapping \c std::recursive_mutex, with debug-compilations class
///   \c std::recursive_timed_mutex is wrapped.
///
/// @see
///  - Chapter \ref alib_threads_locks of the Programmer's Manual of the module \alib_threads_nl.
///  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
// =================================================================================================
class RecursiveLock
#if ALIB_DEBUG_CRITICAL_SECTIONS
: public lang::DbgCriticalSections::AssociatedLock
#endif
{
  protected:
  #if !ALIB_DEBUG && !DOXYGEN
    std::recursive_mutex            mutex; // the only member in release compilations

  #else
    #if DOXYGEN
        /// The internal object to lock on.
        /// \note With debug-compilations, this is of type <c>std::recursive_timed_mutex</c>.
        std::recursive_mutex        mutex;
    #else
        std::recursive_timed_mutex  mutex;
    #endif

  public:
    /// The debug tool instance.
    DbgLockAsserter                  Dbg;
  #endif


    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. With debug-compilations, asserts that this lock is not acquired.
        ~RecursiveLock() override
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_API virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_API virtual bool DCSIsSharedAcquired()                                  const override;
    #elif ALIB_DEBUG
        ~RecursiveLock()
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }
    #endif

  // ###############################################################################################
  // Interface
  // ###############################################################################################
  public:

  #if ALIB_DEBUG || DOXYGEN
    //==============================================================================================
    /// Thread which invokes this method gets registered  as the current owner of this object,
    /// until the same thread releases the ownership invoking #ReleaseRecursive.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.
    /// Multiple (nested) calls to this method are counted and the object is only released when
    /// the same number of Release() calls have been made.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    ALIB_API
    void        AcquireRecursive( ALIB_DBG_TAKE_CI );

    /// Tries to acquire this lock.
    /// Successful calls to this method are counted, as if #AcquireRecursive was called and an
    /// according invocation of #ReleaseRecursive has to be performed.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ///
    /// @return \c true if the lock was not acquired by a different thread and thus, this
    ///         call was successful. \c false otherwise.
    ALIB_API
    [[nodiscard]]
    bool TryAcquire( ALIB_DBG_TAKE_CI );
  #else
    ALIB_FORCE_INLINE
                   void  AcquireRecursive()                                        { mutex.lock(); }
    [[nodiscard]]  bool  TryAcquire()                                   { return mutex.try_lock(); }
  #endif

  #if ALIB_DEBUG || DOXYGEN
    /// Releases ownership of this object. If #AcquireRecursive was called multiple times before,
    /// the same number of calls to this method has to be performed to release ownership.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_API
    void        ReleaseRecursive( ALIB_DBG_TAKE_CI );
  #else
    ALIB_FORCE_INLINE
    void       ReleaseRecursive()                                                { mutex.unlock(); }
  #endif
};


} // namespace alib[::threads]

/// Type alias in namespace \b alib.
using     RecursiveLock=   threads::RecursiveLock;

} // namespace [alib]

#endif // HPP_ALIB_THREADS_RECURSIVELOCK


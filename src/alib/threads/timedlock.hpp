//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_TIMEDLOCK
#define HPP_ALIB_THREADS_TIMEDLOCK 1
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

#include "alib/time/ticks.hpp"
#include <mutex>

namespace alib {  namespace threads {
// =================================================================================================
/// This class is a simple wrapper around C++ standard library type \c std::timed_mutex.
/// Thus, it is used to implement <em>mutual exclusive access</em> to resources by protecting
/// critical code sections from being executed in parallel in concurrent threads.<br>
/// With release-compilations, the only difference to using \c std::timed_mutex directly
/// is that "spurious wake-ups" are detected and mitigated by this implementation.
///
/// When a pair of #Acquire and #Release invocations is performed within the same code block, then
/// it is recommended to use a stack instantiation of class \alib{lang;Owner} to acquire and release
/// objects of this class. Such a use is highly simplified with macros \ref ALIB_LOCK and
/// \ref ALIB_LOCK_WITH.
///
/// This class does not allow nested calls to the method #Acquire - method #Release has to be
/// invoked (from within the same thread that acquired this mutex), before any other thread can
/// again gain access. Nested acquisitions constitute undefined behavior.
///
/// \par Debug-Features
/// Public member #Dbg is available with debug-compilations. It offers the following features:
/// - An assertion is raised when nested use is performed.
/// - The object stores the actual owning thread and the source code position of the last
///   acquirement.
/// - Releasing non-acquired instances, as well as destructing acquired one, raise an assertion.
/// - Field \alib{threads;DbgLockAsserter;WaitTimeLimit} enables the raise of \alib warnings
///   in case a certain wait time is exceeded.
///
/// @see
///  - Chapter \ref alib_threads_locks of the Programmer's Manual of the module \alib_threads_nl.
///  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
// =================================================================================================
class TimedLock
#if ALIB_DEBUG_CRITICAL_SECTIONS
: public lang::DbgCriticalSections::AssociatedLock
#endif
{
  protected:
    /// The internal object to lock on.
    std::timed_mutex    mutex;

  public:
  #if ALIB_DEBUG
    /// The debug tool instance.
    DbgLockAsserter     Dbg;
  #endif

    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. With debug-compilations, asserts that this lock is not acquired.
        ~TimedLock() override
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_API virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_API virtual bool DCSIsSharedAcquired()                                  const override;
    #elif ALIB_DEBUG
        ~TimedLock()
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }
    #endif

  #if ALIB_DEBUG || DOXYGEN
    /// Same as #TryAcquireTimed(const Ticks::Duration&, const CallerInfo&)
    /// but misses the parameter \p{waitTime}. Using this method, the behavior is equivalent to that
    /// of sibling type \alib{threads;Lock}.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_API
    void  Acquire( ALIB_DBG_TAKE_CI );

    /// Tries to acquire this lock.
    /// Multiple (nested) successful calls to this method or method #Acquire are not supported and
    /// lead to undefined behavior.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was not acquired by a different thread and thus, this
    ///         call was successful. \c false otherwise.
    ALIB_API
    [[nodiscard]]
    bool TryAcquire( ALIB_DBG_TAKE_CI );

    /// A thread which invokes this method gets registered as the current owner of this object,
    /// until the same thread releases the ownership invoking #Release.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.
    /// Multiple (nested) calls to this method are not supported and lead to undefined behavior.
    ///
    /// @param waitDuration The maximum time-span to wait.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{waitDuration} expired
    ///         without successful acquisition
    ALIB_API
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks::Duration& waitDuration, const CallerInfo& ci );

    /// Same as overloaded sibling, but expects a C++ standard library duration type
    /// rather than an \b Ticks::Duration.
    /// @param waitDuration The point in time, when this method stops waiting.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{pointInTime} was reached
    ///         without successful acquisition.
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks::Duration::TDuration& waitDuration, const CallerInfo& ci )
    { return TryAcquireTimed( Ticks::Duration(waitDuration), ci);  }

    /// Same as overloaded sibling, but expects a point in time rather than an \b Ticks::Duration.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{pointInTime} was reached
    ///         without successful acquisition.
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks& pointInTime, const CallerInfo& ci )
    { return TryAcquireTimed( pointInTime - Ticks::Now(), ci);  }

    /// Same as overloaded sibling, but expects a C++ standard library point in time type
    /// rather than an \b Ticks::Duration.
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{pointInTime} was reached
    ///         without successful acquisition.
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks::TTimePoint& pointInTime, const CallerInfo& ci )
    { return TryAcquireTimed( Ticks(pointInTime), ci);  }

    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired, in debug-compilations an
    /// assertion is raised. In release compilations, this leads to undefined behavior.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_API
    void Release( ALIB_DBG_TAKE_CI );

  #else // release
                            void Acquire()                                         { mutex.lock(); }
             [[nodiscard]]  bool TryAcquire()                           { return mutex.try_lock(); }
    ALIB_API [[nodiscard]]  bool TryAcquireTimed( const Ticks::Duration&            waitDuration );
             [[nodiscard]]  bool TryAcquireTimed( const Ticks::Duration::TDuration& waitDuration ) { return TryAcquireTimed( Ticks::Duration(waitDuration) );  }
             [[nodiscard]]  bool TryAcquireTimed( const Ticks&                      pointInTime  ) { return TryAcquireTimed( pointInTime - Ticks::Now() );  }
             [[nodiscard]]  bool TryAcquireTimed( const Ticks::TTimePoint&          pointInTime  ) { return TryAcquireTimed( Ticks(pointInTime) );  }
                            void Release()                                       { mutex.unlock(); }

  #endif
};

} // namespace alib[::threads]

/// Type alias in namespace \b alib.
using     TimedLock= threads::TimedLock;

} // namespace [alib]

#endif // HPP_ALIB_THREADS_TIMEDLOCK


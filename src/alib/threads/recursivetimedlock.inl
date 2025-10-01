//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#if !ALIB_SINGLE_THREADED
ALIB_EXPORT namespace alib {  namespace threads {

// =================================================================================================
/// This class is a simple wrapper around C++ standard library type \c std::recursive_timed_mutex.
/// Thus, it is used to implement <em>mutual exclusive access</em> to resources by protecting
/// critical code sections from being executed in parallel in concurrent threads.<br>
/// With release-compilations, the only difference to using \c std::recursive_timed_mutex directly
/// is that "spurious wake-ups" are detected and mitigated by this implementation.
///
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
///
/// \par Availability
/// This type is not available if the compiler-symbol \ref ALIB_SINGLE_THREADED is set.
///
/// @see
///  - Chapter \ref alib_threads_locks of the Programmer's Manual of the module \alib_threads_nl.
///  - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
// =================================================================================================
class RecursiveTimedLock
#if ALIB_DEBUG_CRITICAL_SECTIONS
: public lang::DbgCriticalSections::AssociatedLock
#endif
{
  protected:
    /// The internal object to lock on.
    std::recursive_timed_mutex  mutex; // the only member in release compilations

  public:
  #if ALIB_DEBUG
    /// The debug tool instance.
    DbgLockAsserter             Dbg;
  #endif

    #if ALIB_DEBUG_CRITICAL_SECTIONS
        /// Destructor. With debug-compilations, asserts that this lock is not acquired.
        ~RecursiveTimedLock() override
        { Dbg.AssertNotOwned( ALIB_CALLER, ALIB_CALLER, "Destructing acquired lock" ); }

        /// @return \c true if the lock is acquired (in non-shared mode), \c false otherwise.
        ALIB_DLL virtual bool DCSIsAcquired()                                        const override;

        /// @return \c true if the lock is shared-acquired (by at least any thread).
        ///            Otherwise, returns \c false.
        ALIB_DLL virtual bool DCSIsSharedAcquired()                                  const override;
    #elif ALIB_DEBUG
        ~RecursiveTimedLock()
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
    ALIB_DLL
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
    ALIB_DLL
    [[nodiscard]]
    bool TryAcquire( ALIB_DBG_TAKE_CI );

    /// A thread which invokes this method gets registered as the current owner of this object,
    /// until the same thread releases the ownership invoking #ReleaseRecursive.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.<br>
    /// Successful calls to this method are counted, as if #AcquireRecursive was called and an
    /// according invocation of #ReleaseRecursive has to be performed.
    ///
    /// @param waitDuration The maximum time-span to wait.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{waitDuration} expired
    ///         without successful acquisition
    ALIB_DLL
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks::Duration& waitDuration, const CallerInfo& ci );

    /// Same as overloaded sibling, but expects a C++ standard library duration type
    /// rather than an \b Ticks::Duration.
    ///
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
    ///
    /// @param pointInTime  The point in time, when this method stops waiting.
    /// @param ci           Caller information.
    ///                     Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    /// @return \c true if the lock was acquired, \c false if the \p{pointInTime} was reached
    ///         without successful acquisition.
    [[nodiscard]]
    bool  TryAcquireTimed( const Ticks::TTimePoint& pointInTime, const CallerInfo& ci )
    { return TryAcquireTimed( Ticks(pointInTime), ci);  }

  #else
                            void AcquireRecursive()                                { mutex.lock(); }
             [[nodiscard]]  bool TryAcquire()                           { return mutex.try_lock(); }
    ALIB_DLL [[nodiscard]]  bool TryAcquireTimed( const Ticks::Duration&            waitDuration );
             [[nodiscard]]  bool TryAcquireTimed( const Ticks::Duration::TDuration& waitDuration ) { return TryAcquireTimed( Ticks::Duration(waitDuration) );  }
             [[nodiscard]]  bool TryAcquireTimed( const Ticks&                      pointInTime  ) { return TryAcquireTimed( pointInTime - Ticks::Now() );  }
             [[nodiscard]]  bool TryAcquireTimed( const Ticks::TTimePoint&          pointInTime  ) { return TryAcquireTimed( Ticks(pointInTime) );  }
  #endif

  #if ALIB_DEBUG || DOXYGEN
    /// Releases ownership of this object. If #AcquireRecursive was called multiple times before,
    /// the same number of calls to this method has to be performed to release ownership.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    ALIB_DLL
    void ReleaseRecursive( ALIB_DBG_TAKE_CI );
  #else
    void ReleaseRecursive()                                                      { mutex.unlock(); }
  #endif
};


} // namespace alib[threads]

/// Type alias in namespace \b alib.
using     RecursiveTimedLock=   threads::RecursiveTimedLock;

} // namespace [alib]
#endif // !ALIB_SINGLE_THREADED



//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
#if !ALIB_SINGLE_THREADED
ALIB_EXPORT namespace alib {  namespace threads {


/// This struct wraps C++ standard library types \c std::mutex and \c std::condition_variable.
/// It allows one or more threads to sleep until any other thread satisfies a condition and
/// wakes one or all threads up.
///
/// \par Abstract Template Type:
/// This class is an <em>"abstract template type"</em>, which means that it invokes methods
/// which are not declared and thus have to be declared by a type that derives from this class.
/// The derived type needs to pass its own type name to template parameter \p{TDerived}.<br>
/// With that, this type can cast its <c>this</c>-pointer to that type and access its interface.
/// The derived type has to provide only one function member:
///
///     bool isConditionMet()
/// \par
/// This has to return \c true if the condition is fulfilled, otherwise \c false.
///
/// \par Acquisition Rules:
/// Before using one of the overloaded blocking sleep methods
/// #"WaitForNotification(ALIB_DBG_TAKE_CI)", the object has to be locked by calling #".Acquire".
/// Such acquirement is guaranteed to be held after the method returns.
/// \par
/// A notifying thread has to likewise acquire this object before calling either
/// #"ReleaseAndNotify" or #"ReleaseAndNotifyAll".
/// However, as the method names indicate, with the return from the notification call, the
/// internal mutex is released.
/// \par
/// Similar to class #"threads::Lock", nested calls to the method #".Acquire" are not allowed and
/// constitute undefined behavior.
///
/// \par Usage Recipe:
/// Being an <em>"abstract"</em> type, the class is not designed for direct use.
/// Instead, a derived type should expose its own, dedicated interface, which replaces
/// this type's interface.
/// As a sample, let us look at provided derived type #"threads::Event", which implements
/// simple event-style waiting in two flavors:
/// - level-triggered waits that do not clear the internal flag and therefore never miss a
///   prior notification, and
/// - explicit edge-triggered waits that first clear the flag and then wait (see
///   methods named <em>ResetAndWait...</em> below).
///
/// The simplest possible use-case of this class is implemented as follows:
/// - The type has protected inheritance to this struct.
/// - Protected method #"Event::isConditionMet" is defined, which returns \c true if
///   the instance was notified.
/// - This parent struct is made a friend to enable access to the protected method.
/// - Own interface methods are provided. In this case methods
///   #"Event::Set", #"Event::SetAll", #"Event::Reset"
///   and #"Event::Wait(ALIB_DBG_TAKE_)". For edge-triggered semantics, use the
///   #"Event::ResetAndWait(ALIB_DBG_TAKE_)" overloads.
/// \par
/// A more sophisticated but still simple implementation can be reviewed by analyzing the
/// source of type job-queues in classes #"DedicatedWorker" and
/// #"ThreadPool".
/// Its corresponding interface methods are <em>push</em> and <em>pop</em> alike,
/// which do not resemble too much to the terms wait/notify anymore.
///
/// \par Debug-Information:
/// With debug-builds, several assertions are made to avoid wrong use of the type.
/// This is implemented by adding the field member #".Dbg".
/// The output format of assertions should be 'clickable' inside a users' IDE.
/// The default output string is optimized for
/// \https{JetBrains CLion,www.jetbrains.com/clion} and can be changed by manipulating
/// the static field member #"DbgConditionAsserter::ASSERTION_FORMAT;*".
///
/// \par Availability
/// This type is not available if the configuration macro #"ALIB_SINGLE_THREADED" is set.
///
/// @see
///   - Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.
///   - For this class, a #"alibtools_debug_helpers_gdb;pretty printer" for the
///     GNU debugger is provided.
///
/// @tparam TDerived The derived type which provides method \b isConditionMet.
template<typename TDerived>
struct TCondition {
    /// The mutex used for locking this instance.
    std::mutex              mutex;

    /// The condition variable used for blocking and notification.
    std::condition_variable conditionVariable;

    /// Helper to statically casts this object to the derived type.
    /// @return <c>*this</c> cast to a reference of \p{TDerived}.
    TDerived& cast()                                       { return static_cast<TDerived&>(*this); }


  #if ALIB_DEBUG
    /// The debug tool instance.
    DbgConditionAsserter        Dbg;

    /// Constructor.
    /// @param dbgName The name of this instance.<br>
    ///                Available only with debug-builds.
    ///                With release-builds, this type is default
    ///                constructed. Hence, this parameter has to be provided using macro
    ///                #"ALIB_DBG", which prunes the name away.
    TCondition(const character* dbgName)                                      { Dbg.Name= dbgName; }
    #else
        TCondition()                                                                      = default;
    #endif

    /// A thread which invokes this method gets registered as the current owner of this object,
    /// until the same thread releases the ownership invoking #"Release".
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.<p>
    /// Multiple (nested) calls to this method are <b>not supported</b> and constitute
    /// undefined behavior. In debug-compilations, an #"alib_mod_assert;error is raised".
    ///
    /// An instance has to be acquired before invoking any of the notifiy- or wait-methods.
    /// When return from a notification method, the instance is released.
    /// With return from a wait method, the instance is still held.
    ///
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void  Acquire( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( Dbg.Owner != Thread::GetCurrent(), ALIB_CALLER, ci,
                "Acquire: Multiple acquirements of TCondition are forbidden." );
        #endif

        mutex.lock();

        #if ALIB_DEBUG
            Dbg.Assert( Dbg.Owner==nullptr, ALIB_CALLER, ci,
                "Acquire: Owner is (still) set, after std::mutex.lock()." );
            Dbg.Acq.CI      = ci;
            Dbg.Acq.ActionNo= Dbg.ActionCounter.fetch_add(1);
            Dbg.Owner    = Thread::Get(ci.ThreadID);
        #endif
    }

    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired or acquired by a different
    /// thread, in debug-compilations an #"alib_mod_assert;error is raised".
    /// In release compilations, this leads to undefined behavior.
    /// @see Method #".Acquire".
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void Release( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci, "Release: Not acquired." );
            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                                                 "Release: Ownership is with a different thread" );
            Dbg.Rel.CI      = ci;
            Dbg.Rel.ActionNo= Dbg.ActionCounter.fetch_add(1);
            Dbg.Owner= nullptr;
        #endif

        mutex.unlock();
    }

    /// Unblock a next waiting thread.<p>
    /// As the method name indicates, with this implementation, it is necessary to acquire this
    /// object before invoking this method.
    /// Internally, the mutex will be released, and thus no separate call to #"Release" is necessary,
    /// respectively allowed.
    ///
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void ReleaseAndNotify( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "ReleaseAndNotify called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "ReleaseAndNotify: Ownership is with a different thread" );

            Dbg.Owner= nullptr;
            Dbg.Notify.CI      = ci;
            Dbg.Notify.ActionNo= Dbg.ActionCounter.fetch_add(1);

            Dbg.Owner= nullptr;
        #endif

        mutex.unlock();
        conditionVariable.notify_one();
    }

    /// Releases the internal mutex and wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void ReleaseAndNotifyAll( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNull(Dbg.AssertExclusiveWaiter), ALIB_CALLER,  ci,
                "An exclusive waiter is set. Thus, notifying 'all' is not allowed.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "ReleaseAndNotify called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "ReleaseAndNotify: Ownership is with a different thread" );

            Dbg.Owner= nullptr;
            Dbg.Notify.CI      = ci;
            Dbg.Notify.ActionNo= Dbg.ActionCounter.fetch_add(1);
        #endif

        mutex.unlock();
        conditionVariable.notify_all();
    }

    /// Waits for notification (for an unlimited time).<br>
    /// Before invoking this method, this object has to be #"TCondition::Acquire;acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released by the owner.<br>
    /// It is allowed to create (endless) loops that never actively release this instance
    /// but call one of the waiting methods instead.<br>
    /// Note that "spurious wake-ups" are internally caught with this implementation.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void WaitForNotification( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert(  lang::IsNull(Dbg.AssertExclusiveWaiter)
                      || Dbg.AssertExclusiveWaiter == std::this_thread::get_id(), ALIB_CALLER,  ci,
            "WaitForNotification called by a different thread than granted with 'Dbg.AssertExclusiveWaiter'.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "WaitForNotification called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "WaitForNotification: Ownership is with a different thread" );

            ++Dbg.CntWaiters;
            Dbg.Wait.CI      = ci;
            Dbg.Wait.ActionNo= Dbg.ActionCounter.fetch_add(1);
            Dbg.Owner= nullptr;
        #endif

        std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
        conditionVariable.wait( lock, [this]{ return cast().isConditionMet(); } );
        lock.release();

        #if ALIB_DEBUG
            Dbg.Owner= Thread::Get(ci.ThreadID);
            --Dbg.CntWaiters;
        #endif
    }

    #if !ALIB_DEBUG
    void WaitForNotification( const Ticks::Duration::TDuration& maxWaitTimeSpan)
    {
    #else
    /// Same as #"WaitForNotification(ALIB_DBG_TAKE_CI)", but takes a C++ time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void WaitForNotification( const Ticks::Duration::TDuration& maxWaitTimeSpan,
                              const CallerInfo&                 ci ) {
            Dbg.Assert(  lang::IsNull(Dbg.AssertExclusiveWaiter)
                      || Dbg.AssertExclusiveWaiter == std::this_thread::get_id(), ALIB_CALLER,  ci,
            "WaitForNotification called by a different thread than granted with 'Dbg.AssertExclusiveWaiter'.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "WaitForNotification called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "WaitForNotification: Ownership is with a different thread" );

            ++Dbg.CntWaiters;
            Dbg.Wait.CI      = ci;
            Dbg.Wait.ActionNo= Dbg.ActionCounter.fetch_add(1);
            Dbg.Owner= nullptr;
        #endif

        std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
        conditionVariable.wait_for( lock, maxWaitTimeSpan, [this]{ return cast().isConditionMet(); } );
        lock.release();

        #if ALIB_DEBUG
            Dbg.Owner= Thread::Get(ci.ThreadID);
            --Dbg.CntWaiters;
        #endif
    }

    #if ALIB_DEBUG
    /// Same as #"WaitForNotification(ALIB_DBG_TAKE_CI)", but takes a time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void WaitForNotification( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { WaitForNotification( maxWaitTimeSpan.Export(), ci ); }
    #else
        void WaitForNotification( const Ticks::Duration& maxWaitTimeSpan )
        { WaitForNotification( maxWaitTimeSpan.Export() ); }
    #endif


    #if !ALIB_DEBUG
    void WaitForNotification( const Ticks& wakeUpTime )
    {
    #else
    /// Same as #"WaitForNotification(ALIB_DBG_TAKE_CI)", but takes a point in time at which
    /// waiting stops.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void WaitForNotification( const Ticks& wakeUpTime, const CallerInfo& ci ) {
            Dbg.Assert(  lang::IsNull(Dbg.AssertExclusiveWaiter)
                      || Dbg.AssertExclusiveWaiter == std::this_thread::get_id(), ALIB_CALLER,  ci,
            "WaitForNotification called by a different thread than granted with 'Dbg.AssertExclusiveWaiter'.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "WaitForNotification called without prior acquisition" );

            Dbg.Assert(  Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "WaitForNotification: Ownership is with a different thread" );


            ++Dbg.CntWaiters;
            Dbg.Wait.CI      = ci;
            Dbg.Wait.ActionNo= Dbg.ActionCounter.fetch_add(1);
            Dbg.Owner= nullptr;
        #endif

        std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
        conditionVariable.wait_until( lock, wakeUpTime.Export(),
                                      [this]{ return cast().isConditionMet(); } );
        lock.release();

        #if ALIB_DEBUG
            Dbg.Owner= Thread::Get(ci.ThreadID);
            --Dbg.CntWaiters;
        #endif
    }
};

/// Extends <em>"abstract template type"</em> #"TCondition".
/// This implementation constitutes the simplest possible derivate, by
/// 1. holding just a boolean member, and
/// 2. by providing a similar generic interface.
/// @see Chapter #"alib_manual_appendix_callerinfo" of the General Programmer's Manual.
class Event : protected TCondition<Event> {
  #if !DOXYGEN
    // The parent type needs to be able to call protected method #".isConditionMet".
    friend struct TCondition<Event>;
  #endif

  protected:
    /// Boolean member which records notifications. Defaults to not-notified.
    bool        notified                                                                    = false;

    /// In general, derivates of #"TCondition" have to return \c true if the former
    /// reason for blocking a thread is now fulfilled.
    ///
    /// @return This implementation returns the value of field member #".notified".
    bool isConditionMet()                                                       { return notified; }

  public:
    using  TCondition<Event>::Acquire;
    using  TCondition<Event>::Release;

  //################################################################################################
  //### Debug implementation
  //################################################################################################
    #if ALIB_DEBUG
    /// Defaulted default constructor.
    /// @param dbgName A name for the condition. Only available with debug-compilations.
    Event(const character* dbgName) : TCondition(dbgName)                                         {}
    #else
      Event()                                                                         = default;
    #endif

    /// Sets the condition (flag becomes true) and wakes up the next sleeping thread.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void Set( ALIB_DBG_TAKE_CI ) {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotify(ALIB_DBG(ci));
    }

    /// Sets the condition (flag becomes true) and wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void SetAll( ALIB_DBG_TAKE_CI ) {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotifyAll(ALIB_DBG(ci));
    }

    /// Resets the condition (flag becomes false). No notification is sent.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void Reset( ALIB_DBG_TAKE_CI ) {
        Acquire(ALIB_DBG(ci));
        notified= false;
        Release(ALIB_DBG(ci));
    }

    /// Waits until the condition is set (for an unlimited time).
    /// This method does not modify the condition state.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void Wait( ALIB_DBG_TAKE_CI ) {
      #if ALIB_DEBUG
        Acquire(ci);
          WaitForNotification(ci);
        Release(ci);
      #else
        ALIB_LOCK
            WaitForNotification();
      #endif
    }

    /// Clears the condition first and then waits (for an unlimited time).
    /// This provides edge-triggered semantics comparable to the previous implementation of Wait.
    /// \par Debug Parameter:
    ///   Pass macro #"ALIB_CALLER_PRUNED" with invocations.
    void ResetAndWait( ALIB_DBG_TAKE_CI ) {
      #if ALIB_DEBUG
        Acquire(ci);
          notified= false;
          WaitForNotification(ci);
        Release(ci);
      #else
        ALIB_LOCK
            notified= false;
            WaitForNotification();
      #endif
    }

    #if ALIB_DEBUG
    /// Same as #".Wait(const Ticks::Duration&, const CallerInfo&)", but only up to a given
    /// duration. This method does not modify the condition state.
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void Wait( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci ) {
        Acquire(ci);
          WaitForNotification(maxWaitTimeSpan, ci);
        Release(ci);
    }

    /// Clears the condition first and then waits for up to a given duration.
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void ResetAndWait( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci ) {
        Acquire(ci);
          notified= false;
          WaitForNotification(maxWaitTimeSpan, ci);
        Release(ci);
    }
    #else
        void Wait( const Ticks::Duration::TDuration& maxWaitTimeSpan )
        {ALIB_LOCK
            WaitForNotification(maxWaitTimeSpan);
        }

        void ResetAndWait( const Ticks::Duration::TDuration& maxWaitTimeSpan )
        {ALIB_LOCK
            notified= false;
            WaitForNotification(maxWaitTimeSpan);
        }
    #endif


    #if ALIB_DEBUG
    /// Waits for notification but only for a given duration.
    ///
    /// Before invoking this method, this object has to be #"TCondition::Acquire;acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void Wait( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { Wait( maxWaitTimeSpan.Export(), ci ); }

    /// Clears the condition first and then waits for up to a given duration.
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void ResetAndWait( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { ResetAndWait( maxWaitTimeSpan.Export(), ci ); }
    #else
      void Wait( const Ticks::Duration& maxWaitTimeSpan )
      { Wait( maxWaitTimeSpan.Export() ); }

      void ResetAndWait( const Ticks::Duration& maxWaitTimeSpan )
      { ResetAndWait( maxWaitTimeSpan.Export() ); }
    #endif

    #if ALIB_DEBUG
    /// Waits for notification, but only until a given point in time.
    ///
    /// Before invoking this method, this object has to be #"TCondition::Acquire;acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    /// Waits until the condition is set, but only until a given point in time.
    /// This method does not modify the condition state.
    void Wait( const Ticks& wakeUpTime, const CallerInfo& ci ) {
        Acquire(ci);
          WaitForNotification(wakeUpTime, ci);
        Release(ci);
    }

    /// Clears the condition first and then waits until a given point in time.
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use the macro #"ALIB_COMMA_CALLER_PRUNED" with invocations.
    void ResetAndWait( const Ticks& wakeUpTime, const CallerInfo& ci ) {
        Acquire(ci);
          notified= false;
          WaitForNotification(wakeUpTime, ci);
        Release(ci);
    }
    #else
        void Wait( const Ticks& wakeUpTime )
        {ALIB_LOCK
            WaitForNotification(wakeUpTime);
        }

        void ResetAndWait( const Ticks& wakeUpTime )
        {ALIB_LOCK
            notified= false;
            WaitForNotification(wakeUpTime);
        }
    #endif
};

} // namespace alib[threads]

/// Type alias in namespace #"%alib".
template<typename T= bool>
using     TCondition=   threads::TCondition<T>;

/// Type alias in namespace #"%alib".
using     Event=        threads::Event;

} // namespace [alib]


#endif // !ALIB_SINGLE_THREADED

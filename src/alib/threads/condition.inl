//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
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
/// Before using one of the overloaded blocking sleep methods #WaitForNotification, the object
/// has to be locked by calling #Acquire.
/// Such acquirement is guaranteed to be held after the method returns.
/// \par
/// A notifying thread has to likewise acquire this object before calling either
/// #ReleaseAndNotify or #ReleaseAndNotifyAll.
/// However, as the method names indicate, with the return from the notification call, the
/// internal mutex is released.
/// \par
/// Similar to class \alib{threads;Lock}, nested calls to the method #Acquire are not allowed and
/// constitute undefined behavior.
///
/// \par Usage Recipe:
/// Being an <em>"abstract"</em> type, the class is not designed for direct use.
/// Instead, a derived type should expose its own, dedicated interface, which replaces
/// this type's interface.
/// As a sample, let us look at provided derived type \alib{threads;Condition}, which implements
/// the simplest possible use-case of this class:
/// - The type has protected inheritance to this struct.
/// - Protected method \alib{threads::Condition;isConditionMet} is defined, which returns \c true if
///   the instance was notified.
/// - This parent struct is made a friend too enable access to the protected method.
/// - Own interface methods are provided. In this case methods
///   \alib{threads::Condition;Notify}, \alib{threads::Condition;NotifyAll}, and
///   \alib{threads::Condition;Wait}.
/// \par
/// A more sophisticated but still simple implementation can be reviewed by analyzing the
/// source of type job-queues in classes \alib{threadmodel;DedicatedWorker} and
/// \alib{threadmodel;ThreadPool}.
/// Its corresponding interface methods are <em>push</em> and <em>pop</em> alike,
/// which do not resemble too much to the terms wait/notify anymore.
///
/// \par Debug-Information:
/// With debug-builds, several assertions are made to avoid wrong use of the type.
/// This is implemented by adding the field member #Dbg.
/// The output format of assertions should be 'clickable' inside a users' IDE.
/// The default output string is optimized for
/// \https{JetBrains CLion,www.jetbrains.com/clion} and can be changed by manipulating
/// the static field member \alib{threads;DbgConditionAsserter::ASSERTION_FORMAT}.
///
/// \par Availability
/// This type is not available if the compiler-symbol \ref ALIB_SINGLE_THREADED is set.
///
/// @see
///   - Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
///   - For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///     GNU debugger is provided.
///
/// @tparam TDerived The derived type which provides method \b isConditionMet.
template<typename TDerived>
struct TCondition
{
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
    ///                \ref ALIB_DBG, which prunes the name away.
    TCondition(const character* dbgName)                                      { Dbg.Name= dbgName; }
    #else
        TCondition()                                                                      = default;
    #endif

    /// A thread which invokes this method gets registered as the current owner of this object,
    /// until the same thread releases the ownership invoking #Release.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.<p>
    /// Multiple (nested) calls to this method are <b>not supported</b> and constitute
    /// undefined behavior. In debug-compilations, an \ref alib_mod_assert "error is raised".
    ///
    /// An instance has to be acquired before invoking any of the notifiy- or wait-methods.
    /// When return from a notification method, the instance is released.
    /// With return from a wait method, the instance is still held.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void  Acquire( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( Dbg.Owner != Thread::GetCurrent(), ALIB_CALLER, ci,
                "Acquire: Multiple acquirements of TCondition are forbidden." );
        #endif

        mutex.lock();

        #if ALIB_DEBUG
            Dbg.Assert( Dbg.Owner==nullptr, ALIB_CALLER, ci,
                "Acquire: Owner is (still) set, after std::mutex.lock()." );
            Dbg.AcqCI    = ci;
            Dbg.Owner    = Thread::Get(ci.ThreadID);
        #endif
    }

    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired or acquired by a different
    /// thread, in debug-compilations an \ref alib_mod_assert "error is raised".
    /// In release compilations, this leads to undefined behavior.
    /// @see Method #Acquire.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void Release( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci, "Release: Not acquired." );
            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                                                 "Release: Ownership is with a different thread" );
            Dbg.RelCI    = ci;
            Dbg.Owner= nullptr;
        #endif

        mutex.unlock();
    }

    /// Unblock a next waiting thread.<p>
    /// As the method name indicates, with this implementation, it is necessary to acquire this
    /// object before invoking this method.
    /// Internally, the mutex will be released, and thus no separate call to #Release is necessary,
    /// respectively allowed.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void ReleaseAndNotify( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "ReleaseAndNotify called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "ReleaseAndNotify: Ownership is with a different thread" );

            Dbg.Owner= nullptr;
            Dbg.NotifyCI= ci;
            Dbg.Owner= nullptr;
        #endif

        mutex.unlock();
        conditionVariable.notify_one();
    }

    /// Releases the internal mutex and wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void ReleaseAndNotifyAll( ALIB_DBG_TAKE_CI ) {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNull(Dbg.AssertExclusiveWaiter), ALIB_CALLER,  ci,
                "An exclusive waiter is set. Thus, notifying 'all' is not allowed.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "ReleaseAndNotify called without prior acquisition" );

            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "ReleaseAndNotify: Ownership is with a different thread" );

            Dbg.Owner= nullptr;
            Dbg.NotifyCI= ci;
        #endif

        mutex.unlock();
        conditionVariable.notify_all();
    }

    /// Waits for notification (for an unlimited time).<br>
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released by the owner.<br>
    /// It is allowed to create (endless) loops that never actively release this instance
    /// but call one of the waiting methods instead.<br>
    /// Note that "spurious wake-ups" are internally caught with this implementation.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
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
            Dbg.WaitCI= ci;
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
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a C++ time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
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
            Dbg.WaitCI= ci;
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
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
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
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a point in time at which
    /// waiting stops.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    void WaitForNotification( const Ticks& wakeUpTime, const CallerInfo& ci ) {
            Dbg.Assert(  lang::IsNull(Dbg.AssertExclusiveWaiter)
                      || Dbg.AssertExclusiveWaiter == std::this_thread::get_id(), ALIB_CALLER,  ci,
            "WaitForNotification called by a different thread than granted with 'Dbg.AssertExclusiveWaiter'.");

            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci,
                "WaitForNotification called without prior acquisition" );

            Dbg.Assert(  Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                "WaitForNotification: Ownership is with a different thread" );


            ++Dbg.CntWaiters;
            Dbg.WaitCI= ci;
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

/// Extends <em>"abstract template type"</em> \alib{threads;TCondition}.
/// This implementation constitutes the simplest possible derivate, by
/// 1. holding just a boolean member, and
/// 2. by providing a similar generic interface.
/// @see Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
class Condition : protected TCondition<Condition>
{
    /// the parent type needs to be able to call protected method #isConditionMet.
    friend struct TCondition<Condition>;

  protected:
    /// Boolean member which records notifications. Defaults to not-notified.
    bool        notified                                                                    = false;

    /// In general, derivates of \alib{threads;TCondition} have to return \c true if the former
    /// reason for blocking a thread is now fulfilled.
    ///
    /// @return This implementation returns the value of field member #notified.
    bool isConditionMet()                                                       { return notified; }

  public:
    using  TCondition<Condition>::Acquire;
    using  TCondition<Condition>::Release;
    
  //################################################################################################
  //### Debug implementation
  //################################################################################################
    #if ALIB_DEBUG
    /// Defaulted default constructor.
    /// @param dbgName A name for the condition. Only available with debug-compilations.
    Condition(const character* dbgName) : TCondition(dbgName)                                     {}
    #else
      Condition()                                                                         = default;
    #endif

    /// Wakes up the next sleeping thread.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void Notify( ALIB_DBG_TAKE_CI ) {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotify(ALIB_DBG(ci));
    }

    /// Wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void NotifyAll( ALIB_DBG_TAKE_CI ) {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotifyAll(ALIB_DBG(ci));
    }

    /// Waits for notification (for an unlimited time).<p>
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void Wait( ALIB_DBG_TAKE_CI ) {
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
    /// Same as #Wait(const Ticks::Duration&, const CallerInfo&), but takes a
    /// C++ time span.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    void Wait( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci ) {
        Acquire(ci);
          notified= false;
          WaitForNotification(maxWaitTimeSpan, ci);
        Release(ci);
    }
    #else
        void Wait( const Ticks::Duration::TDuration& maxWaitTimeSpan )
        {ALIB_LOCK
            notified= false;
            WaitForNotification(maxWaitTimeSpan);
        }
    #endif


    #if ALIB_DEBUG
    /// Waits for notification but only for a given duration.
    ///
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    void Wait( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { Wait( maxWaitTimeSpan.Export(), ci ); }
    #else
      void Wait( const Ticks::Duration& maxWaitTimeSpan )
      { Wait( maxWaitTimeSpan.Export() ); }
    #endif

    #if ALIB_DEBUG
    /// Waits for notification, but only until a given point in time.
    ///
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    void Wait( const Ticks& wakeUpTime, const CallerInfo& ci ) {
        Acquire(ci);
          notified= false;
          WaitForNotification(wakeUpTime, ci);
        Release(ci);
    }
    #else
        void Wait( const Ticks& wakeUpTime )
        {ALIB_LOCK
            notified= false;
            WaitForNotification(wakeUpTime);
        }
    #endif
};

} // namespace alib[threads]

/// Type alias in namespace \b alib.
template<typename T= bool>
using     TCondition=   threads::TCondition<T>;

/// Type alias in namespace \b alib.
using     Condition=   threads::Condition;

} // namespace [alib]


#endif // !ALIB_SINGLE_THREADED

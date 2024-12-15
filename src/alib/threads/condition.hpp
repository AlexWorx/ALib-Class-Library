//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_SLEEPER
#define HPP_ALIB_THREADS_SLEEPER 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(THREADS)
#include "alib/time/ticks.hpp"
#include "alib/lang/owner.hpp"

#if ALIB_DEBUG
#   include "alib/threads/thread.hpp"
#   include "alib/threads/dbglockasserter.hpp"
#   include <atomic>
#endif


#include <mutex>
#include <condition_variable>
namespace alib {  namespace threads {


#if ALIB_DEBUG
//==================================================================================================
/// This type is used for debugging and asserting types \alib{threads;TCondition}.
/// With debug compilations that class holds one member of this struct, which aggregates all
/// debug information.
//==================================================================================================
struct DbgConditionAsserter
{
    String            Name;                  ///< The name of this instance.
    Thread*           Owner{nullptr};        ///< Tracks the current owner.
    CallerInfo        AcqCI ;                ///< Source location of the most recent acquirement.
    CallerInfo        RelCI ;                ///< Source location of the most recent release.
    CallerInfo        WaitCI;                ///< The most recent call to \b WaitForNotification.
    CallerInfo        NotifyCI;              ///< The most recent call to \b ReleaseAndNotify or
    std::atomic<int>  CntWaiters{0};         ///< The number of currently waiting threads.
    std::thread::id   AssertExclusiveWaiter; ///< If set from outside, methods \b WaitForNotification
                                             ///< will raise an assertion in the case they are called
                                             ///< from a different thread.
#if ALIB_CAMP
    /// The format string used to write exceptions to the console.
    /// This string can be changed if the source information is not "clickable" in a user's
    /// development environment.<br>
    ///
    /// The default string is optimized for
    /// \https{JetBrains CLion,www.jetbrains.com/clion} and is defined as:
    /**  \verbatim
Assertion failed in method TCondition::{2:sm}
                 Message: {0}
                Instance: {1}

               Called By: {3:ya}
                      At: {3:sf:sl}
                  Thread: {3:ta}

           Current Owner: {4}
             #Of Waiters: {5}
        Exclusive Waiter: {6}

   Latest Acquisition By: {7:ya}
                      At: {7:sf:sl}
                  Thread: {7:ta}
       Latest Release By: {8:ya}
                      At: {8:sf:sl}
                  Thread: {8:ta}

          Latest Wait By: {9:ya}
                      At: {9:sf:sl}
                  Thread: {9:ta}
        Latest Notify By: {10:ya}
                      At: {10:sf:sl}
                  Thread: {10:ta}
      \endverbatim
      <p>
      The placeholder fields that this format string refers to are set as follows:
      - \c 0: Headline.
      - \c 1: Debug-name of the condition.
      - \c 2: \b %CallerInfo of assertion.
      - \c 3: \b %CallerInfo of caller.
      - \c 4: \b %CallerInfo of latest acquisition.
      - \c 5: The number of waiting threads.
      - \c 6: <c>std::thread::id</c> of an exclusive waiter thread (which can be optionally set).
      - \c 7: \b %CallerInfo of latest acquisition.
      - \c 8: \b %CallerInfo of latest release.
      - \c 9: \b %CallerInfo of latest wait call.
      - \c 10: \b %CallerInfo of latest notification.

      The format specification of the type \c CallerInfo is defined with class
      \alib{lang::format;FMTCallerInfo}.
      \par Availability:
        This field is only available with the inclusion of \alib_basecamp in the \alibdist.<br> */
    ALIB_API
    static NString ASSERTION_FORMAT;
#endif

    /// Writes assertion info and calls \ref ALIB_ASSERT.
    /// @see
    ///    Field #ASSERTION_FORMAT which allows changing the output format to achieve 'clickable'
    ///    assertion messages.
    /// @param cond     The condition that is to be met.
    /// @param assertCI Location where the assertion is placed.
    /// @param ci       Location of the call to the method that asserted.
    /// @param headline The message.
    ALIB_API
    void Assert( bool cond, const CallerInfo& assertCI, const CallerInfo& ci,
                 const NString& headline );

    /// Returns \c true if the current owner is the current thread.<p>
    /// Available only in debug-builds.
    /// @return \c true, if the lock is owned by this thread, \c false if it is owned by
    ///            another thread or not owned.
    bool            IsOwnedByCurrentThread()                                                   const
    { return Owner != nullptr &&  Owner == Thread::GetCurrent() ; }

}; // struct DbgConditionAsserter
#endif // ALIB_DEBUG


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
    TCondition(const String& dbgName)                                         { Dbg.Name= dbgName; }
    #else
        TCondition()                                                                      = default;
    #endif

    //==============================================================================================
    /// A thread which invokes this method gets registered as the current owner of this object,
    /// until the same thread releases the ownership invoking #Release.
    /// In the case that this object is already owned by another thread, the invoking thread is
    /// suspended until ownership can be gained.<p>
    /// Multiple (nested) calls to this method are <b>not supported</b> and constitute
    /// undefined behavior. In debug-compilations, an assertion is raised.
    ///
    /// An instance has to be acquired before invoking any of the notifiy- or wait-methods.
    /// When return from a notification method, the instance is released.
    /// With return from a wait method, the instance is still held.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    void  Acquire( ALIB_DBG_TAKE_CI )
    {
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

    //==============================================================================================
    /// Releases ownership of this object.
    /// If this method is invoked on an object that is not acquired or acquired by a different
    /// thread, in debug-compilations an assertion is raised.
    /// In release compilations, this leads to undefined behavior.
    /// @see Method #Acquire.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    void Release( ALIB_DBG_TAKE_CI )
    {
        #if ALIB_DEBUG
            Dbg.Assert( lang::IsNotNull(Dbg.Owner), ALIB_CALLER, ci, "Release: Not acquired." );
            Dbg.Assert( Dbg.Owner == Thread::GetCurrent(), ALIB_CALLER, ci,
                                                 "Release: Ownership is with a different thread" );
            Dbg.RelCI    = ci;
            Dbg.Owner= nullptr;
        #endif

        mutex.unlock();
    }

    //==============================================================================================
    /// Unblock a next waiting thread.<p>
    /// As the method name indicates, with this implementation, it is necessary to acquire this
    /// object before invoking this method.
    /// Internally, the mutex will be released, and thus no separate call to #Release is necessary,
    /// respectively allowed.
    ///
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    void ReleaseAndNotify( ALIB_DBG_TAKE_CI )
    {
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

    //==============================================================================================
    /// Releases the internal mutex and wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    void ReleaseAndNotifyAll( ALIB_DBG_TAKE_CI )
    {
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

    //==============================================================================================
    /// Waits for notification (for an unlimited time).<br>
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released by the owner.<br>
    /// It is allowed to create (endless) loops that never actively release this instance
    /// but call one of the waiting methods instead.<br>
    /// Note that "spurious wake-ups" are internally caught with this implementation.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    //==============================================================================================
    void WaitForNotification( ALIB_DBG_TAKE_CI )
    {
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
    //==============================================================================================
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a C++ time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
    void WaitForNotification( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci )
    {
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
    //==============================================================================================
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a time span that defines
    /// a maximum wait time.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
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
    //==============================================================================================
    /// Same as #WaitForNotification(ALIB_DBG_TAKE_CI), but takes a point in time at which
    /// waiting stops.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
    void WaitForNotification( const Ticks& wakeUpTime, const CallerInfo& ci )
    {
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
/// @see
///  Chapter \ref alib_manual_appendix_callerinfo of the General Programmer's Manual.
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
    void Notify( ALIB_DBG_TAKE_CI )
    {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotify(ALIB_DBG(ci));
    }

    /// Wakes up all sleeping threads.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void NotifyAll( ALIB_DBG_TAKE_CI )
    {
        Acquire(ALIB_DBG(ci));
        notified= true;
        ReleaseAndNotifyAll(ALIB_DBG(ci));
    }

    /// Waits for notification (for an unlimited time).<p>
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    void Wait( ALIB_DBG_TAKE_CI )
    {
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
    //==============================================================================================
    /// Same as #Wait(const Ticks::Duration&, const CallerInfo&), but takes a
    /// C++ time span.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
    void Wait( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci )
    {
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
    //==============================================================================================
    /// Waits for notification but only for a given duration.
    ///
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    ///
    /// @param maxWaitTimeSpan  The maximum time to wait.
    /// @param ci               Caller information.
    ///                         Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
    void Wait( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { Wait( maxWaitTimeSpan.Export(), ci ); }
    #else
      void Wait( const Ticks::Duration& maxWaitTimeSpan )
      { Wait( maxWaitTimeSpan.Export() ); }
    #endif

    #if ALIB_DEBUG
    //==============================================================================================
    /// Waits for notification, but only until a given point in time.
    ///
    /// Before invoking this method, this object has to be \ref Acquire "acquired".
    /// After the wake-up call, the internal mutex is (again) acquired and thus has to be
    /// released later.
    ///
    /// @param wakeUpTime  The point in time to wake up, even if not notified.
    /// @param ci          Caller information.
    ///                    Use macro \ref ALIB_COMMA_CALLER_PRUNED with invocations.
    //==============================================================================================
    void Wait( const Ticks& wakeUpTime, const CallerInfo& ci )
    {
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

} // namespace alib[::threads]

/// Type alias in namespace \b alib.
template<typename T= bool>
using     TCondition=   threads::TCondition<T>;

/// Type alias in namespace \b alib.
using     Condition=   threads::Condition;

} // namespace [alib]

#endif // HPP_ALIB_THREADS_SLEEPER


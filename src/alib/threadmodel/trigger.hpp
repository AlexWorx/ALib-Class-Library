//==================================================================================================
/// \file
/// This header file is part of module \alib_threadmodel of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADMODEL_TRIGGER
#define HPP_ALIB_THREADMODEL_TRIGGER
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(THREADMODEL)

#include "alib/threads/thread.hpp"
#include "alib/threads/condition.hpp"
#include "alib/containers/list.hpp"


namespace alib { namespace threadmodel {

// forward declaration
class Trigger;

//==================================================================================================
/// This class declares a simple virtual interface for objects that allow to be triggered
/// periodically.
/// The use of this class is recommended to avoid to either creating a dedicated thread for
/// simple, periodic tasks, or to add invocations to perform such tasks to other threads of the
/// application.
///
/// Instead, class \alib{threadmodel;Trigger} will be the only responsible entity to
/// trigger such actions within its own execution thread.
///
/// @see Chapter \ref alib_thrmod_trigger of this module's Programmer's Manual
///      provides a quick source code sample that demonstrates the use this class.
//==================================================================================================
class Triggered
{
    protected:
        /// Class Trigger is the only one that will trigger us and hence has access to
        /// our protected functions.
        friend class Trigger;

        /// The name of the triggered object. This is mainly used for log output and some
        /// convenience methods.
        const   String      Name;

        /// Constructor. Stores the name in constant member #Name. Usually these names are
        /// hard-coded C++ character arrays. If programmatically created, it has to be assured that
        /// the life-cycle of the string supersedes the lifecycle of the instnace of this class.
        /// @param pName The name of this object.
                Triggered(const String& pName)  : Name( pName )                                   {}

       /// Virtual empty destructor. Needed with any virtual class.
        virtual ~Triggered()                                                                      {}
        
        /// Implementations need to return the sleep time, between two trigger events.
        /// Precisely, this method is called after #trigger has been executed and defines the
        /// next sleep time.
        /// @return The desired sleep time between two trigger events.
        virtual Ticks::Duration triggerPeriod()                                                 = 0;

        ///  Implementations need to implement this function and perform their trigger actions
        ///  here.
        virtual void            trigger()                                                       = 0;

}; // interface class Triggered

//==================================================================================================
/// The \b %Trigger class provides a mechanism to periodically "trigger" actions on objects
/// that implement the `Triggered` interface without requiring dedicated threads per object
/// or manual additions of actions in existing threads.
///
/// The class manages a collection of `Triggered` objects and ensures that their virtual
/// \b trigger() methods are called periodically based on the time interval given with their
/// respective method \b triggerPeriod().
///
/// Internally, \b %Trigger operates its own thread to handle the timing and execution of
/// the triggers, adhering to the specified conditions.
///
/// This design helps in simplifying periodic task management within an application,
/// avoiding thread proliferation and minimizing resource overhead.
///
/// \par Key responsibilities of the class:
/// - Maintain and manage a list of `Triggered` objects.
/// - Schedule and execute periodic triggers for the registered objects.
/// - Provide the ability to add or remove `Triggered` objects dynamically.
///
/// \par Usage:
/// - Users register their implementations of the \b %Triggered interface with the #Add() method
///   to begin periodic triggers.
/// - Objects can be unregistered using the `Remove()` method.
/// - The #Stop() method terminates the execution of the internal thread.
///
/// Intended for scenarios where lightweight, periodic task scheduling is needed
/// without creating additional complexity or significant overhead.
//==================================================================================================
class Trigger :   protected   Thread,
                  protected   TCondition<Trigger>
{
    friend struct threads::TCondition<Trigger>;
    friend class  lang::Owner<Trigger&>;

  protected:
    /// The entry type used with field #triggerList.
    struct TriggerEntry
    {
        /// Constructor.
        /// @param target      Assigned to #Target.
        /// @param nextWakeup  Assigned to #NextWakeup.
        TriggerEntry(Triggered* target, const Ticks& nextWakeup )
        : Target(target), NextWakeup(nextWakeup)                                            {}

        /// Deleted copy constructor (to avoid working on copies accidentally).
        TriggerEntry(TriggerEntry& )= delete;

        Triggered*  Target;     ///< The triggered object.
        Ticks       NextWakeup; ///< The next wakeup time.
    };

    /// The list of registered triggered objects.
    List<MonoAllocator, TriggerEntry>   triggerList;

    /// The condition requested by parent class \alib{threads;TCondition} via a call to
    /// #isConditionMet.
    bool                                wakeUpCondition                                     = false;

    /// Denotes whether or not the trigger is currently used in internal thread mode.
    bool                                internalThreadMode                                  = false;

    /// Implementation of the interface needed by parent \alib{threads;TCondition}.
    /// @return Member #wakeUpCondition
    bool isConditionMet()                                 const noexcept { return wakeUpCondition; }

  public:
    /// Constructor.
    ALIB_API                Trigger();

    /// Destructor.
    ALIB_API  virtual      ~Trigger()                                                      override;

    using Thread::Start;
    
    /// Implementation of the parent interface (virtual abstract).
    ALIB_API  virtual void  Run()                                                          override;

    /// Executes the processing of triggers for up to a given maximum time.
    /// If the internal thread is not used, this method may be called manually inside an external
    /// loop to execute triggering operations within the specified timeframe.
    ///
    /// If the internal thread was created and is running, with debug-compilations, an \alib
    /// error will be raised.
    /// @param until  Defines the future point in time until triggering is performed.
    ALIB_API void Do( Ticks until );

    /// Invokes #Do(Ticks) by adding the given \p{duration} to the current point in time.
    /// @param until  Defines the maximum duration for which this method will execute the trigger
    ///               logic.
    void Do( Ticks::Duration until )                                 { Do( Ticks::Now() + until ); }

    #if !DOXYGEN
    void Do( Ticks::Duration::TDuration until )                      { Do( Ticks::Now() + until ); }
    #endif

    /// Stops the trigger thread and joins it.
    ALIB_API  virtual void  Stop();

    /// Add an object to be triggered.
    /// @param triggered        The object to be triggered.
    /// @param initialWakeup    If \c true, the first wakeup is scheduled right away.
    ///                         Defaults to \c false.
    ALIB_API void           Add( Triggered& triggered, bool initialWakeup= false);

    /// Remove a previously added triggered object.
    /// @param triggered  The object to be removed from the list.
    ALIB_API void           Remove( Triggered& triggered);

}; // Trigger


} // namespace alib[::threadmodel]

/// Type alias in namespace \b alib.
using      Trigger          = threadmodel::Trigger;

/// Type alias in namespace \b alib.
using      Triggered        = threadmodel::Triggered;

}  // namespace [alib]


#endif // HPP_ALIB_THREADMODEL_TRIGGER


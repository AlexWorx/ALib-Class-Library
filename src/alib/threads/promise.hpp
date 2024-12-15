//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_PROMISE
#define HPP_ALIB_THREADS_PROMISE 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(THREADS)
#include "alib/time/ticks.hpp"
#if ALIB_DEBUG
#   include "alib/strings/localstring.hpp"
#endif
#if ALIB_CAMP
#   include "alib/lang/system/calendar.hpp"
#endif

#include <future>

namespace alib {  namespace threads {

//==================================================================================================
/// A simple class which aggregates C++ standard library mechanics provided with <c>std::promise</c>
/// and <c>std::future</c> into one simple interface.
///
/// The following features and facts are notable:
/// 1. The class is not designed for multiple threads to wait on this promise to be fulfilled.
///    Only one thread is allowed to wait.<br>
///    For other use-cases, see alternative type \alib{threads;Condition}
///
/// 2. When fulfillment is acknowledged, a standardized \e state can be given.
///    Besides the default state \alib{threads::Promise;State;Fulfilled}, two other states are
///    built-in. Custom states can be defined and given.<br>
///    An application might agree that, for example, each thread has to stop in the case that
///    somewhere state \alib{threads::Promise;State;EmergencyStop} was set.
///    Note that some mechanics have to be implemented which ensures that such an emergency state is
///    propagated to all relevant entities.
///
/// The class cannot be copied (just like <c>std::promise</c> can't be). Therefore, usually
/// a pointer to an object is passed to the fulfilling thread and the waiting thread is responsible
/// for ensuring the lifecycle of the object survived until the promise is fulfilled.
///
/// With debug-compilations, the field #DbgWaitTimeLimit enables the raise of \alib warnings in
/// case a certain wait time is exceeded when using the unlimited blocking method
///
/// Furthermore, two \alib warnings may be raised with destruction:
/// 1. When the promise was not fulfilled.
///    This warning can be silenced by setting <b>DbgFulfillCI.Line</b> to a positive value.
/// 2. When the promise was not awaited.
///    This warning can be silenced by setting <b>DbgWaitCI.Line</b> to a positive value.
//==================================================================================================
class Promise
{
  public:
    /// Enumerates possible states. With construction, #Unfulfilled is set.
    /// \b Error or a custom value could be used if the promise could not be fulfilled for any
    /// reason. \b EmergencyStop could be the right choice if the whole application should stop.
    /// But this is all up to the using code.
    enum class State
    {
        Unfulfilled   ,    ///< The state after construction.
        OK            ,    ///< The default state of successful fulfillment.
        Error         ,    ///< A default error state. (Use-case dependent.)
        EmergencyStop ,    ///< A state indicating that everything is to be stopped.
                           ///< (Use-case dependent.)
        Custom        ,    ///< The first element defining a custom state. Further custom states
                           ///< with higher underlying integral values can be defined.
    };

  protected:

    std::promise<State> promise;    ///< Used for implementation.
    std::future<State>  future;     ///< Used for implementation.

  public:
  #if ALIB_DEBUG
    /// This is a threshold that causes the non-timed #Wait method to raise a
    /// \alib warning in debug-builds in case a thread is blocked longer than the given duration.
    ///
    /// To disable warnings in cases that high block times are suitable, set this value to \c 0.
    /// The default value is two seconds.
    Ticks::Duration     DbgWaitTimeLimit                  = Ticks::Duration::FromAbsoluteSeconds(2);

    /// Debug-information about the first caller to #Fulfill.
    /// A second (forbidden) call will be asserted with information about where the first invocation
    /// was made.
    lang::CallerInfo    DbgFulfillCI;

    /// Debug-information about the first caller to a successful wait.
    /// A second call will be asserted with information about where the first invocation to a
    /// successful wait was made.
    lang::CallerInfo    DbgWaitCI;

  #endif // ALIB_DEBUG


    /// Default constructor. Sets the state to \alib{threads::Promise;State;State::Unfulfilled}.
    Promise()
    {
        #if ALIB_DEBUG
            DbgFulfillCI.Line= -1;
            DbgWaitCI   .Line= -1;
        #endif
        future= promise.get_future();
    }

    /// Destructor.
    ~Promise()
    {
        ALIB_ASSERT_WARNING( DbgFulfillCI.Line != -1, "THREADS",
            "Promise not fulfilled on destruction." )
        ALIB_ASSERT_WARNING( DbgWaitCI   .Line != -1, "THREADS",
            "Promise not awaited on destruction." )
    }

    /// With debug-compilations, a warning is raised on destruction, in case either
    /// #Fulfill was not called or a waiting method was not called (or both).
    /// With an invocation of this method, such warnings can be omitted.<br>
    /// Note that the function is available in release-builds as well, but empty and optimized-out.
    void        DbgOmitDestructionWarning()
    {
        #if ALIB_DEBUG
            DbgFulfillCI.Line =
            DbgWaitCI   .Line = 0;
        #endif
    }

    #if ALIB_DEBUG
    /// This is to be invoked by the "fulfilling" thread which received this object, for example
    /// as a part of a command, to signal that the promise is considered fulfilled.<br>
    /// A thread waiting with methods #Wait, #WaitUntil, or #WaitFor will be woken up.
    /// @param ci    Caller information. Only available with debug-builds.
    ///              Use macro \ref ALIB_CALLER_PRUNED to pass this parameter, respectively.
    ///              Use macro \ref ALIB_CALLER_PRUNED_COMMA if \p{state} should be non-defaulted.
    /// @param state The result to set.
    ///              Defaults to \alib{threads;Promise::State;OK}.
    void        Fulfill(const CallerInfo& ci,  State state= State::OK)
    {
        ALIB_ASSERT_ERROR(DbgFulfillCI.File == nullptr, "THREADS",
            NString2K()
            << "Promise was already fulfilled. Repeated calls not allowed.\n"
            << "  This call:    " << ci<< "\n"
               "  Earlier call: " << DbgFulfillCI )
        DbgFulfillCI= ci;
        
        promise.set_value(state);
    }
    #else
        void    Fulfill(State state= State::OK)                       { promise.set_value(state); }
    #endif

    /// Waits an unlimited time for the promise to become fulfilled.
    /// \par Debug Parameter:
    ///   Pass macro \ref ALIB_CALLER_PRUNED with invocations.
    /// @return The state given by the second thread with #Fulfill.
    State Wait(ALIB_DBG_TAKE_CI)
    {
        #if ALIB_DEBUG
            ALIB_ASSERT_ERROR(DbgWaitCI.File == nullptr, "THREADS",
                NString2K()
                << "Promise was already awaited. Repeated calls not allowed.\n"
                << "  Received with: " << DbgWaitCI )

            if ( !DbgWaitTimeLimit.IsZero() )
            {
                Ticks::Duration waitDuration=  DbgWaitTimeLimit;
                Ticks overallTimer;
                Ticks waitTimer;
                while ( future.wait_for( (waitDuration - waitTimer.Age()).Export() )
                        != std::future_status::ready )
                {
                    if ( waitTimer.Age() < waitDuration )
                        continue; // spurious wakeup

                    NString1K msg;
                    msg << "Waiting for a Promise since " << overallTimer.Age()
                    #if !ALIB_CAMP
                        .InAbsoluteMilliseconds() << "ms"
                    #endif
                    ;
                    DbgSimpleALibMsg( ci, 1, "THREADS", msg  );
                    waitTimer.Reset();
                }
            }

            DbgWaitCI= ci;
        #endif

        return future.get();
    }

    #if ALIB_DEBUG
    /// Waits for the promise to become fulfilled, but only for a given duration.
    /// @param maxWaitTimeSpan    The maximum time to wait.
    /// @param ci                 Caller information. Only available with debug-builds.
    ///                           Use macro \ref ALIB_COMMA_CALLER_PRUNED to pass this parameter.
    /// @return Either <b>State::Unfulfilled</b>, or the state given by the second thread
    ///         with #Fulfill.
    State WaitFor( const Ticks::Duration::TDuration& maxWaitTimeSpan, const CallerInfo& ci )
    {
        ALIB_ASSERT_ERROR(DbgWaitCI.File == nullptr, "THREADS",
            NString2K()
            << "Promise was already awaited. Repeated calls not allowed.\n"
            << "  Received with: " << DbgWaitCI )

        if ( future.wait_for(maxWaitTimeSpan) == std::future_status::timeout )
             return State::Unfulfilled;

        DbgWaitCI= ci;
        return future.get();

    }
    #else
        State WaitFor( const Ticks::Duration::TDuration& maxWaitTimeSpan )
        {
            return future.wait_for(maxWaitTimeSpan) == std::future_status::timeout
                    ? State::Unfulfilled
                    : future.get();
        }
    #endif

    #if ALIB_DEBUG
    /// Waits for the promise to become fulfilled, but only for a given duration.
    /// @param maxWaitTimeSpan    The maximum time to wait.
    /// @param ci                 Caller information. Only available with debug-builds.
    ///                           Use macro \ref ALIB_COMMA_CALLER_PRUNED to pass this parameter.
    /// @return Either <b>State::Unfulfilled</b>, or the state given by the second thread
    ///         with #Fulfill.
    State WaitFor( const Ticks::Duration& maxWaitTimeSpan, const CallerInfo& ci )
    { return WaitFor( maxWaitTimeSpan.Export(), ci ); }
    #else
        State WaitFor( const Ticks::Duration& maxWaitTimeSpan )
        { return WaitFor( maxWaitTimeSpan.Export() ); }
    #endif

    #if ALIB_DEBUG
    /// Waits for the promise to become fulfilled, but only until a given point in time.
    /// @param wakeUpTime    The point in time to wake up, even if not notified.
    /// @param ci            Caller information. Only available with debug-builds.
    ///                      Use macro \ref ALIB_COMMA_CALLER_PRUNED to pass this parameter.
    /// @return Either <b>State::Unfulfilled</b>, or the state given by the second thread
    ///         with #Fulfill.
    State WaitUntil( const Ticks& wakeUpTime, const CallerInfo& ci )
    {
        ALIB_ASSERT_ERROR(DbgWaitCI.File == nullptr, "THREADS",
            NString2K()
            << "Promise was already awaited. Repeated calls not allowed.\n"
            << "  Received with: " << DbgWaitCI )

        if ( future.wait_until(wakeUpTime.Export()) == std::future_status::timeout )
            return State::Unfulfilled;

        DbgWaitCI= ci;
        return future.get();

    }
    #else
        State WaitUntil( const Ticks& wakeUpTime )
        {
            return future.wait_until(wakeUpTime.Export()) == std::future_status::timeout
                    ? State::Unfulfilled
                    : future.get();
        }
    #endif
};  // class Promise


} // namespace alib[::threads]

/// Type alias in namespace \b alib.
using     Promise=   threads::Promise;

} // namespace [alib]

#endif // HPP_ALIB_THREADS_PROMISE


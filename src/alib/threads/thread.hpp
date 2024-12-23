//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_THREADS_THREAD
#define HPP_ALIB_THREADS_THREAD 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif
ALIB_ASSERT_MODULE(THREADS)

#include "alib/strings/astring.hpp"
#include "alib/time/ticks.hpp"
#if ALIB_ENUMS
#   include "alib/enums/records.hpp"
#endif
#include <thread>

namespace alib {  namespace threads {

// forwards
class     Thread;
namespace detail
{
    void  threadStart( Thread* t );
}


/// The ALib thread identifier type
using ThreadID=    integer;

//==================================================================================================
///  This is a value that may be passed as a value indicating an undefined thread
//==================================================================================================
static constexpr   ThreadID  UNDEFINED = 0;


//==================================================================================================
/// Initializes \alib thread logic.
/// Multiple invocations of this method are forbidden.
///
/// The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Bootstrap will call this function.
///
/// \see
///   For information about using this method, consult chapter
///   \ref alib_manual_bootstrapping_nocamps of the \ref alib_manual.
//==================================================================================================
ALIB_API
void        Bootstrap();

//==================================================================================================
/// Frees resources and shuts down \alib thread logic.
/// Multiple invocations of this method are forbidden.
/// The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib, hence the (overloaded)
/// functions \ref alib::Shutdown will call this function.
///
/// \see
///   Sibling function \alib{threads;Bootstrap}.
//==================================================================================================
ALIB_API
void        Shutdown();

//==================================================================================================
/// This is a virtual abstract (interface) type that provides the executable method
/// for an associated \alib{threads;Thread}.
///
/// Objects of this type may be passed to the constructor of class \alib{threads;Thread}. The
/// default implementation of method \alib{threads;Thread::Run} will then invoke
/// method #Run of this class.
//==================================================================================================
class Runnable
{
    public:
        /// Virtual destructor.
        virtual                   ~Runnable()                                                     {}

        /// The method invoked by the default implementation of \alib{threads;Thread::Run}
        /// of the thread object that this runnable is associated with.
        virtual        void        Run()                                                        = 0;
};

//==================================================================================================
/// This class provides a limited (minimal) abstraction of C++ type <c>std::thread</c>.
/// As elaborated in the module's documentation, this class is not considered full featured
/// but - as of today- is meant for simple use cases only.
///
/// For general information of multi-threading support provided by \alib,  please consult
/// the \ref alib_mod_threads.
///
/// ## Friends ##
/// function \alib{threads;Bootstrap}
/// function \alib{threads;Shutdown}
/// @see
///   For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///   GNU debugger is provided.
//==================================================================================================
class Thread : public Runnable
{
    #if !DOXYGEN
        friend void    ALIB_API Bootstrap        ();
        friend void    ALIB_API Shutdown         ();
        friend void    detail::threadStart( Thread* t );
    #endif

  public:
    /// States that follow lifecycle of the thread. The states are accessible with method
    /// #GetState.
    enum class State
    {
        Unstarted   =  0  ///< Not started and no call to #Start was performed, yet.
                          ///< This is the state after construction of an instance.
       ,Started     =  1  ///< Method #Start was invoked but not running, yet.
       ,Running     =  2  ///< The thread's #Run method is currently processed.
       ,Done        =  3  ///< The run method is processed, and the thread is ready to be
                          ///< terminated.
       ,Terminated  =  4  ///< The thread is terminated.
    };

  // ###############################################################################################
  // Protected fields
  // ###############################################################################################
  protected:
    /// The internal C++ thread object.
    std::thread*        c11Thread                                                          =nullptr;

    /// The internal C++ thread id.
    std::thread::id     nativeID;

    /// The runnable to execute.
    Runnable*           runnable                                                           =nullptr;

    /// The id of the thread.
    ThreadID            id                                                                       =0;

    /// The name of the thread.
    AString             name;

    /// Internal flag to indicate if the thread is alive.
    State              state                                                   = State::Unstarted;


  // ###############################################################################################
  // Constructors, destructor
  // ###############################################################################################
  public:
    //==============================================================================================
    /// Constructor without a parameter specifying a Runnable. Such thread will not execute any
    /// code unless a specialized class is derived that overrides virtual method #Run.
    ///
    /// @param pName  (Optional) The designated name of the thread. If the name provided is,
    ///               empty the name of the thread will be set to match a string representation
    ///               of the thread id.
    //==============================================================================================
    ALIB_API
                       Thread( const String& pName= EMPTY_STRING )
    : Thread( nullptr, pName )                                                                    {}

    //==============================================================================================
    /// Constructor with provision of a Runnable 'target'. The Run method of 'target' will be
    /// executed upon thread start, unless this class is specialized an its own Run() method is
    /// overwritten.
    /// @param target The target to execute when the thread runs.
    /// @param pName  (Optional) The designated name of the thread. If the name provided is,
    ///               empty the name of the thread will be set to match a string representation
    ///               of the thread id.
    //==============================================================================================
    ALIB_API            Thread(Runnable* target, const String& pName= EMPTY_STRING );


    //==============================================================================================
    /// Before destruction, #Join has to be called.
    /// The destructor blocks if the thread was started and is still running.
    /// Blocking lasts until the thread's end of execution.<br>
    //==============================================================================================
    ALIB_API  virtual  ~Thread()                                                           override;

    /// Deleted copy constructor.
    Thread(const Thread& )                                                                 = delete;


    // #############################################################################################
    // Runnable Interface implementation
    // #############################################################################################

        /// If we have a runnable, its \b run() method is executed. Otherwise nothing is done.
        ///
        /// Hence, to have the thread execute something, either a \alib{threads;Runnable}
        /// has to be provided or a derived version of this class replaces this method.
        ///
        virtual void            Run()                 override { if (runnable)    runnable->Run(); }

  // ###############################################################################################
  // Interface
  // ###############################################################################################

    //==============================================================================================
    /// Returns the id of this Thread. Systems threads have IDs below 0, ALIB generated threads
    /// have positive IDs and start with 1.
    ///
    /// @return    The \alib id of the thread.
    //==============================================================================================
    ThreadID            GetID()                                                 const { return id; }

    //==============================================================================================
    /// Returns the id of this Thread. Systems threads have IDs below 0, ALIB generated threads
    /// have positive IDs and start with 1.
    ///
    /// @return    The \alib id of the thread.
    //==============================================================================================
    std::thread::id     GetNativeID()                                     const { return nativeID; }

    //==============================================================================================
    /// Returns the name of the thread. An \alib thread can have any name that is given to it and
    /// such name can be changed any time. In fact, names are for debugging and logging
    /// purposes only.
    ///
    /// @return  Returns the name of the thread.
    //==============================================================================================
    virtual
    const CString       GetName()                                             const { return name; }

    //==============================================================================================
    /// Sets the name of the thread. An \alib thread can have any name that is given to it and
    /// such name can be changed any time.
    /// Thread names are meant for debugging and logging purposes only.
    /// @param newName    The name that the Thread should hold.
    //==============================================================================================
     virtual void       SetName( const String& newName )                   { name.Reset( newName); }

    //==============================================================================================
    /// Returns the state of the thread. The states are given in enumeration #State and
    /// during the lifecycle of the thread, the state transitions from
    /// \alib{threads::Thread;State::Unstarted} to \alib{threads::Thread;State::Terminated}.
    /// \note
    ///   For system threads (the thread that executed function <c>main thread</c> and those
    ///   not created using \alib thread features) \alib{threads::Thread;State::Running} is
    ///   returned.
    ///   For those, it can't be determined if the thread is stared, alive or not.
    /// @return The current state within the thread's lifecycle.
    //==============================================================================================
    State              GetState()                                                 { return state; }

    //==============================================================================================
    /// A shortcut to
    /// <c>GetState().first == State::Started  || GetState().first == State::Running</c>.
    ///
    /// @return \c true if the current state of the thread is
    ///            \alib{threads;Thread;State::Running}.
    //==============================================================================================
    bool                IsAlive()                            { return    state == State::Started
                                                                      || state == State::Running; }

    //==============================================================================================
    /// Starts the execution of the thread. Method #Run is invoked by the new system thread,
    /// which - if not overwritten - invokes the method \alib{threads;Runnable::Run}.
    /// Of course, this method immediately returns, and after invocation, method #IsAlive will
    /// usually return \c true (unless the executed thread is not finished already).
    //==============================================================================================
    ALIB_API
    virtual void        Start();

    //==============================================================================================
    /// Terminates a thread using <c>std::join()</c>.
    /// When this method is called, the thread should be in state <b>State::Done</b>, which
    /// is the case after method \b Run has exited.
    /// Otherwise, an \alib warning is raised.
    //==============================================================================================
    ALIB_API
    virtual void        Join();

  // ###############################################################################################
  // Static interface
  // ###############################################################################################
  public:
    /// Static method that returns an object representing the thread that invoked this call.
    /// @return A pointer to the current thread.
    static
    Thread*             GetCurrent()                   { return Get( std::this_thread::get_id() ); }

    /// Static method that returns an object representing the thread identified by the given
    /// system ID \p{nativeID}. \c nullptr is returned only if:
    /// - parameter \p{nativeID} is nulled, or
    /// - during \ref alib_manual_bootstrapping "bootstrapping of ALib", which multithreaded
    ///   applications have to perform before starting threads, \c nullptr might be returned as
    ///   well.
    ///
    /// In any other situations, either,
    /// - \p{nativeID} is received from a thread that was started using this class \b Thread, then
    ///   a pointer to that instance is returned, or
    /// - the thread was started without the use of \alib, then an instance of this class is
    ///   created (once) and registered.
    ///
    /// @param nativeID The C++ Standard Libraray identifier.
    /// @return A pointer to the instance associated with the native thread ID.
    static
    ALIB_API
    Thread*             Get(std::thread::id nativeID);

    //==============================================================================================
    /// Static method that returns the thread that initialized the library.
    /// Such thread this is supposedly the "main" thread as bootstrapping usually is performed
    /// by the process before any other threads are started.
    ///
    /// @return A pointer to the main thread.
    //==============================================================================================
    ALIB_API static
    Thread*             GetMain();

    //==============================================================================================
    /// Proactively offers the system to suspend the current thread.
    /// Invokes <c>std::this_thread::yield</c>.
    /// \note Must not be named 'Yield', because this is a macro name with MSVC.
    //==============================================================================================
    static
    void                YieldToSystem()                              { std::this_thread::yield();  }

    /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
    /// \see Variants #SleepMicros, #SleepNanos, #Sleep and #SleepUntil
    ///  @param milliseconds    Sleep time in milliseconds.
    static void         SleepMillis     ( int milliseconds )
    { std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) ); }


    /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
    /// \see Variants #SleepMillis, #SleepNanos, #Sleep and #SleepUntil
    /// @param microseconds    Sleep time in microseconds.
    static void         SleepMicros     ( int64_t microseconds )
    { std::this_thread::sleep_for( std::chrono::microseconds( microseconds ) );  }

    /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
    /// \see Variants #SleepMicros, #SleepMillis, #Sleep and #SleepUntil
    /// @param nanoseconds    Sleep time in nanoseconds.
    static void         SleepNanos      ( int64_t nanoseconds )
    { std::this_thread::sleep_for( std::chrono::nanoseconds( nanoseconds ) ); }

     /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
     /// \see Variants #SleepUntil, #SleepMicros, #SleepMillis and #SleepNanos.
     /// @param duration    Sleep duration.
     static void        Sleep       ( const Ticks::Duration& duration )
     { std::this_thread::sleep_for( duration.Export() ); }

     /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
     /// \see Variants #SleepUntil, #SleepMicros, #SleepMillis and #SleepNanos.
     /// @param duration    Sleep duration.
     static void        Sleep       ( const Ticks::Duration::TDuration& duration )
     { std::this_thread::sleep_for( duration ); }

     /// Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
     /// \see Variants #Sleep, #SleepMicros, #SleepMillis and #SleepNanos
     /// @param time    Sleep duration.
     static void        SleepUntil  ( const Ticks& time )
     { std::this_thread::sleep_until( time.Export() ); }
}; // class Thread

#if ALIB_BOXING && !DOXYGEN
inline
void FAppend_thread(const boxing::Box& self, strings::TAString<character,lang::HeapAllocator>& target ) {
    const Thread* src= self.Unbox<Thread*>();
    if ( src )
        target << src;
    else
        target << "nullptr";
}
#endif



} // namespace alib[::threads]

/// Type alias in namespace \b alib.
using     Runnable=     threads::Runnable;

/// Type alias in namespace \b alib.
using     Thread=       threads::Thread;

/// Type to store thread identifiers.
using     ThreadID=     threads::ThreadID;



} // namespace [alib]

ALIB_STRINGS_APPENDABLE_TYPE_N(alib::threads::Thread)
ALIB_STRINGS_APPENDABLE_TYPE_W(alib::threads::Thread)
ALIB_STRINGS_APPENDABLE_TYPE_N(alib::threads::Thread*)
ALIB_STRINGS_APPENDABLE_TYPE_W(alib::threads::Thread*)

#if ALIB_ENUMS && ALIB_BOXING
    ALIB_ENUMS_ASSIGN_RECORD(alib::threads::Thread::State, alib::enums::ERSerializable )
#endif


#endif // HPP_ALIB_THREADS_THREAD


// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_THREADS_LIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_THREADS_LIB
//! @cond NO_DOX
#define HPP_ALIB_THREADS_LIB 1
//! @endcond


// #################################################################################################
// class Runnable
// #################################################################################################


// #################################################################################################
// includes
// #################################################################################################
#if ALIB_FEAT_THREADS

    #if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
        #include <map>
    #endif

    #if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
        #include <mutex>
    #endif

    #if !defined (_GLIBCXX_CONDITION_VARIABLE) && !defined(_CONDITION_VARIABLE_)
        #include <condition_variable>
    #endif

    #if !defined (_GLIBCXX_THREAD) && !defined (_THREAD_ )
        #include <thread>
    #endif

#endif // ALIB_FEAT_THREADS

namespace aworx { namespace lib {


/**
 *  This namespace of \alib holds classes that are providing an interface into
 *  multi threaded programming. The design of the classes tries to mimic the basic thread
 *  interface of the Java language.
 *
 *  It is important to understand that
 *  (following \ref aworx::lib "the principle design goals of \alib")
 *  multi threading support of \alib is non intrusive in respect to multi-threading
 *  mechanisms of other C++ libraries and/or the underlying operating system.
 *  This means that a software entity that uses this library is not at all bound to use the thread
 *  classes found within this namespace.
 *
 *  In contrast, using the classes found here, is of-course optional. \alib usually
 *  provides a certain level of "source compatibility" between the programming languages
 *  C#, Java and C++.
 *
 *  It is no problem, to have threads started using different methods and libraries than the
 *  one found here, as long that such threads become "native" operation system threads.
 *  If later such a thread uses method  \alib{threads,Threads::CurrentThread}",
 *  a corresponding \alib{threads,Thread} object of \alib  is created internally and returned.
 *  This way, the externally created thread is automatically "registered" with \alib.
 *  If not stated differently in the detailed documentation, from this point in time,
 *  the thread can be interfaced with \alib classes in the same manner as if it was created
 *  using them. The same or similar should be \c true for the opposite situation.
 *
 *  Thread support of \alib can be disabled, respectively enabled, using compiler symbols
 *  #ALIB_FEAT_THREADS_OFF and #ALIB_FEAT_THREADS_ON.
 *  When support is switched off, classes
 *  \alib{threads,Thread} and
 *  \alib{threads,Runnable} "Runnable" are not available. However, the classes
 *  \alib{threads,ThreadLockNR} "ThreadLockNR" and
 *  \alib{threads,ThreadLock}  are.
 *  Their interface remains the same, but they do not perform any locking! Due to this behavior,
 *  it is possible to write code that implements due locking while still, in single threaded
 *  compilation, the code does not suffer any performance drawbacks, because lock code  will be
 *  <em>pruned</em>.
 *
 */
namespace threads {

// forward declarations
class Thread;

/** ************************************************************************************************
 *  This is a value that may be passed as a value indicating an undefined thread
 **************************************************************************************************/
static constexpr   int                  NullThreadId= 0;


/** ************************************************************************************************
 * The library class for namespace \ref aworx::lib::threads.
 **************************************************************************************************/
class Threads  : public lang::Library
{
    /// Class \b %Thread is our friend.
    friend class aworx::lib::threads::Thread;

    // #############################################################################################
    // internals
    // #############################################################################################
    protected:
        /// A counter to provide thread IDs for System threads
        int                                     nextSystemThreadId                             = -1;

        /// A counter to provide thread IDs for ALIB threads
        int                                     nextThreadId                                   =  1;

        #if ALIB_FEAT_THREADS
            /// A mutex to protect the static stuff.
            std::mutex                          mutex;

            /// The internal mapping of C++ 11 thread ids to Thread objects.
            std::unordered_map<std::thread::id, Thread*>  threadMap;
        #else
            Thread*                             noThreadsCompilationMainThread;
        #endif


    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        inline  Threads()
        : lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_THREADS") )
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns an object representing the thread that invoked this call.
         *
         * @return A pointer to the current Thread.
         ******************************************************************************************/
        ALIB_API
        Thread*             CurrentThread();


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes \alib thread logic.
         * This method should not be called directly. use
         * \ref aworx::lib::ALib::Init "ALib::Init" to initialize all <b>%ALib Modules</b> included in the
         * distribution.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources and shuts down \alib thread logic.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class threads::Library

} // namespace aworx::lib[::threads]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::threads.
 **************************************************************************************************/
ALIB_API
extern threads::Threads THREADS;

namespace threads {

/** ************************************************************************************************
 *  This class mimics Interface Runnable of the Java runtime library.
 **************************************************************************************************/
class Runnable
{
    public:
        /// Destructor. Virtual as this is an abstract interface class.
        virtual                   ~Runnable()        {}

        /// The method invoked by the Thread that this runnable is associated with.
        virtual        void        Run()                                    = 0;
};

// #################################################################################################
// class Thread
// #################################################################################################

/** ************************************************************************************************
 *  This class mimics class Thread of the Java runtime library. For general information of
 *  multi-threading support provided by \alib, see #aworx::lib::threads.
 **************************************************************************************************/
class Thread : public Runnable
{
    /// Library class \b %Threads is our friend, we may access its protected values.
    friend class aworx::lib::threads::Threads;

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        #if ALIB_FEAT_THREADS
            /// The internal C++ 11 thread object.
            std::thread*        c11Thread                                                  =nullptr;

            /// The internal C++ 11 thread id.
            std::thread::id     c11ID;
        #else
            void*               c11Thread                                                  =nullptr;
        #endif

        /// Internal flag to indicate if thread is alive.
        bool                    isAliveFlag                                                  =false;

        /// The runnable to execute.
        Runnable*               runnable                                                   =nullptr;

        /// The id of the thread.
        int                     id                                                               =0;

        /// The name of the thread.
        String64                name;

        /** ****************************************************************************************
         *  Our best friend: the c function that starts us
         * @param t    Internal parameter
         ******************************************************************************************/
        friend    void _Thread__Start( Thread* t );


    // #############################################################################################
    // Constructors, destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         *  Internal constructor for Thread objects representing system threads (aka threads that
         *  were not created using class Thread from ALIB.
         *
         * @param internal    An internal value.
         ******************************************************************************************/
        Thread( bool internal );

    public:
        /** ****************************************************************************************
         *  Constructor without parameters. As no runnable was provided, such thread will not
         *  execute any code unless a specialized class is derived that overrides virtual method
         *  #Run.
         *  The name of the thread will be set to match a string representation of the thread id.
         ******************************************************************************************/
        ALIB_API            Thread()
                            : Thread( nullptr, ASTR("") )       {}

        /** ****************************************************************************************
         *  Constructor without a parameter specifying a Runnable. Such thread will not execute any
         *  code unless a specialized class is derived that overrides virtual method #Run.
         *
         * @param threadName The designated name of the thread. If the name provided is empty or,
         *                   \c nullptr, the name of the thread will be set to match a string
         *                   representation of the thread id.
         ******************************************************************************************/
        ALIB_API            Thread( const String& threadName ) : Thread( nullptr, threadName )     {}

        /** ****************************************************************************************
         *  Constructor with provision of a Runnable 'target'. The Run method of 'target' will be
         *  executed upon thread start, unless this class is specialized an its own Run() method is
         *  overwritten.
         * @param target  The target to execute when the thread runs.
         * @param name    (Optional) The designated name of the thread. If the name provided is,
         *                empty the name of the thread will be set to match a string representation
         *                of the thread id.
         ******************************************************************************************/
        ALIB_API            Thread(Runnable* target , const String& name= EmptyString );


        /** ****************************************************************************************
         *  The destructor blocks, if the thread was started and is still running. Blocking lasts
         *  until the thread's end of execution.
         *  Declared virtual, as inherited from Runnable.
         ******************************************************************************************/
        ALIB_API  virtual   ~Thread();


    // #############################################################################################
    // Runnable Interface implementation
    // #############################################################################################

        /// If we have a runnable, execute its run() method. Does nothing else.
        virtual void        Run()                           { if (runnable)    runnable->Run();    }


    // #############################################################################################
    // Interface
    // #############################################################################################

        /** ****************************************************************************************
         * Returns the id of this Thread. Systems threads have IDs below 0, ALIB generated threads
         * have positive IDs and start with 1.
         *
         * @return    The \alib id of the thread.
         ******************************************************************************************/
        inline
        int            GetId()                              { return id;          }

        /** ****************************************************************************************
         * Returns the name of the thread. An ALIB thread can have any name that is given to it and
         * such name can be changed any time. In fact, such names are for debugging and logging
         * purposes only.
         *
         * @return  Returns the name of the thread.
         ******************************************************************************************/
        inline
        const TString& GetName()                            { return name; }

        /** ****************************************************************************************
         * Sets the name of the thread. An ALIB thread can have any name that is given to it and
         * such name can be changed any time. In fact, such names are for debugging and logging
         * purposes only.
         *
         * @param newName    The name that the Thread should hold.
         ******************************************************************************************/
         inline
         void          SetName( const String& newName )     { name= newName;   }

        /** ****************************************************************************************
         * Returns \c true, if this thread was started and is still running. If \c false is
         * returned, the thread object can be safely deleted, without causing a blocking operation.
         * \note
         *   For system threads (those not created using \alib thread features) \c true is
         *   returned. It can't be determined if the thread is still alive or not.
         *
         *
         * @return \c true if this thread was started previously and is still running.
         ******************************************************************************************/
         inline
         bool          IsAlive()                            { return id <= 0 || isAliveFlag; }

        /** ****************************************************************************************
         *  Starts the execution of the thread. The thread's Run method is invoked, which
         *  (if not overwritten) invokes the Run method of any given Runnable. After returning from
         *  the method, unless the thread #IsAlive will return \c true.
         ******************************************************************************************/
        ALIB_API
        void          Start();
};


}} // namespace lib::threads

/// Type alias in namespace #aworx.
using     Runnable=     aworx::lib::threads::Runnable;

/// Type alias in namespace #aworx.
using     Thread=       aworx::lib::threads::Thread;

}  // namespace aworx

#endif // HPP_ALIB_THREADS_LIB

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_threads of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_THREADS_THREAD
#define HPP_ALIB_THREADS_THREAD 1

#ifndef HPP_ALIB_STRINGS_ASTRING
#   include "alib/strings/astring.hpp"
#endif

#if !defined (_GLIBCXX_THREAD) && !defined (_THREAD_ )
#   include <thread>
#endif


ALIB_ASSERT_MODULE(THREADS)

namespace aworx { namespace lib { namespace threads {

// forwards
class     Thread;
namespace detail
{
    ALIB_API Thread* getThread(std::thread::id c11ID );
               void  threadStart( Thread* t );
}


/** Type to store thread identifiers.  */
using ThreadID=    integer;

/** ************************************************************************************************
 *  This is a value that may be passed as a value indicating an undefined thread
 **************************************************************************************************/
static constexpr   ThreadID  UNDEFINED = 0;


/** ************************************************************************************************
 * Initializes \alib thread logic.
 * Multiple invocations of this method are ignored.
 *
 * The \ref alib_manual_bootstrapping "standard bootstrap" code of \alib will invoke this function.
 * Only if fileset \alibfs_modules is not included in the \alibdist_nl, this
 * function has to be invoked "manually".
 *
 * \see
 *   For information about using this method, consult chapter
 *   \ref alib_manual_bootstrapping_smallmods of the \ref alib_manual.
 **************************************************************************************************/
void        Bootstrap();

/** ************************************************************************************************
 * Frees resources and shuts down \alib thread logic.
 * Multiple invocations of this method are ignored.
 *
 * \see
 *   Sibling function \alib{threads,Bootstrap}.
 **************************************************************************************************/
void        Shutdown();

/** ************************************************************************************************
 * This is a virtual abstract (interface) type that provides the executable method
 * for an associated \alib{threads,Thread}.
 *
 * Objects of this type may be passed to the constructor of class \alib{threads,Thread}. The
 * default implementation of method \alib{threads,Thread::Run} will then invoke
 * method #Run of this class.
 **************************************************************************************************/
class Runnable
{
    public:
        /** Virtual destructor. */
        virtual                   ~Runnable()        {}

        /**
         * The method invoked by the default implementation of \alib{threads,Thread::Run}
         * of the thread object that this runnable is associated with.
         */
        virtual        void        Run()                                    = 0;
};

/** ************************************************************************************************
 * This class provides a limited (minimal) abstraction of C++ 11 threads.
 * As elaborated in the module's documentation, this class is not considered full featured
 * but - as of today- is meant for simple use cases only.
 *
 * For general information of multi-threading support provided by \alib,  please consult
 * the \ref alib_mod_threads.
 *
 * ## Friends ##
 * function \alib{threads,Bootstrap}
 * function \alib{threads,Shutdown}
 **************************************************************************************************/
class Thread : public Runnable
{
    #if !defined(ALIB_DOX)
        friend void    Bootstrap        ();
        friend void    Shutdown         ();
        friend void    detail::threadStart( Thread* t );
        friend Thread* detail::getThread  (std::thread::id c11ID );
    #endif

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** The internal C++ 11 thread object. */
        std::thread*        c11Thread                                                      =nullptr;

        /** The internal C++ 11 thread id. */
        std::thread::id     c11ID;

        /** Internal flag to indicate if thread is alive. */
        bool                isAliveFlag                                                      =false;

        /** The runnable to execute. */
        Runnable*           runnable                                                       =nullptr;

        /** The id of the thread. */
        ThreadID            id                                                                   =0;

        /** The name of the thread. */
        AString             name;


    // #############################################################################################
    // Constructors, destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor without a parameter specifying a Runnable. Such thread will not execute any
         * code unless a specialized class is derived that overrides virtual method #Run.
         *
         * @param pName  (Optional) The designated name of the thread. If the name provided is,
         *               empty the name of the thread will be set to match a string representation
         *               of the thread id.
         ******************************************************************************************/
        ALIB_API            Thread( const String& pName= EmptyString() )
        : Thread( nullptr, pName )
        {}

        /** ****************************************************************************************
         * Constructor with provision of a Runnable 'target'. The Run method of 'target' will be
         * executed upon thread start, unless this class is specialized an its own Run() method is
         * overwritten.
         * @param target The target to execute when the thread runs.
         * @param pName  (Optional) The designated name of the thread. If the name provided is,
         *               empty the name of the thread will be set to match a string representation
         *               of the thread id.
         ******************************************************************************************/
        ALIB_API            Thread(Runnable* target, const String& pName= EmptyString() );


        /** ****************************************************************************************
         *  The destructor blocks, if the thread was started and is still running. Blocking lasts
         *  until the thread's end of execution.
         *  Declared virtual, as inherited from Runnable.
         ******************************************************************************************/
        ALIB_API  virtual  ~Thread();


    // #############################################################################################
    // Runnable Interface implementation
    // #############################################################################################

        /**
         * If we have a runnable, its \b run() method is executed. Otherwise nothing is done.
         *
         * Hence, to have the thread execute something, either a \alib{threads,Runnable}
         * has to be provided or a derived version of this class replaces this method.
         *  */
        virtual void    Run()                                { if (runnable)    runnable->Run(); }


    // #############################################################################################
    // Interface
    // #############################################################################################

        /** ****************************************************************************************
         * Returns the id of this Thread. Systems threads have IDs below 0, ALIB generated threads
         * have positive IDs and start with 1.
         *
         * @return    The \alib id of the thread.
         ******************************************************************************************/
        ThreadID        GetId()                              { return id;          }

        /** ****************************************************************************************
         * Returns the name of the thread. An \alib thread can have any name that is given to it and
         * such name can be changed any time. In fact, such names are for debugging and logging
         * purposes only.
         *
         * @return  Returns the name of the thread.
         ******************************************************************************************/
        const CString   GetName()                            { return name; }

        /** ****************************************************************************************
         * Sets the name of the thread. An \alib thread can have any name that is given to it and
         * such name can be changed any time. In fact, such names are for debugging and logging
         * purposes only.
         *
         * @param newName    The name that the Thread should hold.
         ******************************************************************************************/
         void           SetName( const String& newName )     { name.Reset( newName);   }

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
         bool           IsAlive()                            { return isAliveFlag; }

        /** ****************************************************************************************
         * Starts the execution of the thread. Method #Run is invoked by the new system thread,
         * which - if not overwritten - invokes the method \alib{threads,Runnable::Run}.
         * Of-course, this method immediately returns and after invocation, method #IsAlive will
         * usually return \c true (unless the executed thread is not finished already).
         ******************************************************************************************/
        ALIB_API
        void            Start();

    // #############################################################################################
    // Static interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Static method that returns an object representing the thread that invoked this call.
         *
         * @return A pointer to the current thread.
         ******************************************************************************************/
        static
        Thread*             GetCurrent()
        {
            return detail::getThread( std::this_thread::get_id() );
        }

        /** ****************************************************************************************
         * Static method that returns the thread that initialized the library.
         * Such thread this is supposedly the "main" thread as bootstrapping usually is performed
         * by the process before any other threads are started.
         *
         * @return A pointer to the main thread.
         ******************************************************************************************/
        ALIB_API static
        Thread*             GetMain();

        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * Variants of this method are #SleepMicros and #SleepNanos.
         *
         *  @param milliseconds    Sleep time in milliseconds.
         ******************************************************************************************/
        static
        void                SleepMillis( int milliseconds )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
        }


        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * Variants of this method are #SleepMillis and #SleepNanos.
         *
         * @param microseconds    Sleep time in microseconds.
         ******************************************************************************************/
        static
        void                SleepMicros( int64_t microseconds )
        {
            std::this_thread::sleep_for( std::chrono::microseconds( microseconds ) );
        }

        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * Variants of this method are #SleepMicros and #SleepMillis.
         *
         * @param nanoseconds    Sleep time in nanoseconds.
         ******************************************************************************************/
        static
        void                SleepNanos( int64_t nanoseconds )
        {
            std::this_thread::sleep_for( std::chrono::nanoseconds( nanoseconds ) );
        }
};

}} // namespace aworx[::lib::threads]

/// Type alias in namespace #aworx.
using     Runnable=     lib::threads::Runnable;

/// Type alias in namespace #aworx.
using     Thread=       lib::threads::Thread;

/** Type to store thread identifiers.  */
using     ThreadID=     lib::threads::ThreadID;

}  // namespace [aworx]

#endif // HPP_ALIB_THREADS_THREAD

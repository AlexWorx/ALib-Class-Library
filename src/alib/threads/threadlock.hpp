/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_threads of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_THREADS_THREADLOCK
#define HPP_ALIB_THREADS_THREADLOCK 1

#if !defined (HPP_ALIB_THREADS_THREADLOCKNR)
    #include "alib/threads/threadlocknr.hpp"
#endif

#if !defined (_GLIBCXX_CONDITION_VARIABLE) && !defined(_CONDITION_VARIABLE_)
    #include <condition_variable>
#endif

#if !defined (_GLIBCXX_THREAD) && !defined (_THREAD_ )
#   include <thread>
#endif


namespace aworx { namespace lib { namespace threads {

// forwards
class     Thread;
namespace detail { ALIB_API Thread* getThread(std::thread::id c11ID ); }

/** ************************************************************************************************
 * While this class does not inherit from \alib{threads,ThreadLockNR}, it copies and extends its
 * interface and functionality.
 * With this lock, nested acquisitions are supported with this type.
 * An instance of this class is released when an equal amount of invocations to #Acquire and
 * #Release have been performed.
 *
 * The object stores the actual owning thread and this thread may be queried. Such
 * queries are \b not thread safe and should be performed only to perform tasks that are
 * not mission critical, for example to create log output, usage statistics or similar things.
 * In other words, a software's algorithmic logic should by principle never use information about
 * the thread that currently owns a lock.
 *
 * With debug builds, a warning threshold for the number of repeated acquisitions can be
 * defined with public member #DbgRecursionWarningThreshold. As the member's name indicates, it
 * is \e assumed that too many repeated locks are caused by a recursive calls. Usually, locking
 * data access should not be done in recursive code.<br>
 * Furthermore, field #DbgWarningAfterWaitTimeInMillis enables the raise of \alib warnings in case
 * certain wait time is exceeded. Along with the warning, the owner and waiting threads' names
 * and IDs are given, along with both source code location of the acquisition, respectively, the
 * failed acquistion.
 **************************************************************************************************/
class ThreadLock
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** Thread ID of the current owner. */
        std::thread::id             owner;

        /** The internal object to lock on. */
        mutable
        std::mutex                  mutex;

        /** The internal object to lock on. */
        std::condition_variable     mutexNotifier;

        /** Counter for the number of Acquire() calls of the current thread. */
        uint16_t                    cntAcquirements                                              =0;

        /** The safeness setting. */
        Safeness                    safeness;

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        #if ALIB_DEBUG
            /**
             * This is a threshold that causes Acquire() to raise an \alib warning in debug builds,
             * if acquiring this lock takes longer than the given number of milliseconds.
             * Such warning is often a quick first hint for a racing condition.
             *
             * To disable such messages, set this value to 0.
             * The default value is <b>2,000</b> (two seconds), which seems "very long", but can
             * happen on systems with heavy load.
             */
            integer                 DbgWarningAfterWaitTimeInMillis                          =2000L;

            /** Source location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFile                                           =nullptr;

            /** Source location of acquirement. (Available only in debug-builds.). */
            int                     DbgOwnerLine;

            /** Source location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFunc                                           =nullptr;

            /**
             * Limit of recursions. If limit is reached or a multiple of it, an error is passed
             * to \alib{results,ReportWriter}.
             * Defaults is \c 10. To disable, set to \c 0.
             * Available only in debug versions of \alib.
             */
            uint16_t               DbgRecursionWarningThreshold                                 =10;
        #endif

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructor.
         *
         * @param safeness  (Optional) Defaults to \c Safeness::Safe.
         *                  See #SetSafeness for more information.
         ******************************************************************************************/
        ALIB_API
        explicit          ThreadLock( Safeness safeness=  Safeness::Safe );

        /** ****************************************************************************************
         *  Destructor.
         ******************************************************************************************/
        ALIB_API         ~ThreadLock();

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Thread which invokes this method gets registered  as the current owner of this object,
         * until the same thread releases the ownership invoking #Release.
         * In the case that this object is already owned by another thread, the invoking thread is
         * suspended until ownership can be gained.
         * Multiple (nested) calls to this method are counted and the object is only released when
         * the same number of Release() calls have been made.
         *
         * \note
         *   In debug-compilations of the library, this method accepts three parameters,
         *   providing information about the caller. In the release version these parameters do not
         *   exist. Therefore use macro #ALIB_CALLER_PRUNED to provide the parameters:
         *
         *          sample.Acquire( ALIB_CALLER_PRUNED );
         *
         * @param dbgFile  Caller information. Available only with debug builds.
         * @param dbgLine  Caller information. Available only with debug builds.
         * @param dbgFunc  Caller information. Available only with debug builds.
         ******************************************************************************************/
         ALIB_API
         #if ALIB_DEBUG
            void        Acquire( const NCString& dbgFile, int dbgLine, const NCString& dbgFunc );
         #else
            void        Acquire();
         #endif

        /** ****************************************************************************************
         * Releases ownership of this object. If #Acquire was called multiple times before, the same
         * number of calls to this method have to be performed to release ownership.
         ******************************************************************************************/
        ALIB_API
        void            Release();

        /** ****************************************************************************************
         * Returns \c true if the next invocation of #Release will release the lock, otherwise
         * \c false. In other words, returns \c true if this lock is \ref Acquire "acquired" exactly
         * \c 1.
         *
         * \note
         *   This method is not (and can not) be synchronized. Consequently, a reliable result
         *   is only guaranteed if #IsOwnedByCurrentThread returns \c true.
         *   This method is therefore deemed to be used only in situations where it is assured
         *   that this lock is owned by the current thread.
         *
         * @return \c true if locked exactly once.
         ******************************************************************************************/
        bool            WillRelease()                                                          const
        {
            return cntAcquirements == 1;
        }

        /** ****************************************************************************************
         * Returns the current owner of this lock.
         * If not acquired, \c nullptr is returned.
         *
         * @return The thread that owns this lock.
         ******************************************************************************************/
        bool            IsOwnedByCurrentThread()                                              const
        {
            return owner == std::this_thread::get_id();
        }

        /** ****************************************************************************************
         * Returns the current owner of this lock. If not acquired, \c nullptr is returned.
         *
         * \see Method #IsOwnedByCurrentThread.
         * @return The thread that owns this lock, \c nullptr if not acquired.
         ******************************************************************************************/
        Thread*         GetOwner()                                                            const
        {
            // copying member owner once, makes this method sort of "thread safe"
            std::thread::id id= owner;
            if( id== std::thread::id() )
                return nullptr;
            return detail::getThread( id );
        }

        /** ****************************************************************************************
         * Returns the number of acquirements of this ThreadLock.
         *
         * @return The number of (recursive) acquirements.
         ******************************************************************************************/
        int           CountAcquirements()                                                      const
        {
            return cntAcquirements;
        }

        /** ****************************************************************************************
         *  If parameter is \c Safeness::Unsafe, the whole locking system is disabled.
         *  The only objective here is to gain execution speed, as thread synchronization causes
         *  relatively expensive system calls.
         *  Use this method only if you are 100% sure that your (otherwise) critical section are
         *  executed in a single threaded environment. And: "relative expensive" means: they are not
         *  really expensive. This is provided only for the rare case that your critical section is
         *  very, very frequently executed.
         *
         * @param safeness  Determines if this object should use a mutex (\c Safeness::Safe)
         *                  or just do nothing (\c Safeness::Unsafe).
         ******************************************************************************************/
        ALIB_API
        void          SetSafeness( Safeness safeness );

        /** ****************************************************************************************
         *  Query if this instance was set to unsafe mode.
         * @return A value of type aworx::lib::Safeness "Safeness"
         ******************************************************************************************/
        Safeness      GetSafeness()                                                            const
        {
            return safeness;
        }
};


}} // namespace aworx[::lib::threads]

/// Type alias in namespace #aworx.
using     ThreadLock=   lib::threads::ThreadLock;

}  // namespace [aworx]

#endif // HPP_ALIB_THREADS_THREADLOCK

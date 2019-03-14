// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_THREADS_THREADLOCK
#define HPP_ALIB_THREADS_THREADLOCK 1

#if !defined (HPP_ALIB_THREADS_THREADLOCKNR)
    #include "alib/threads/threadlocknr.hpp"
#endif

#if !defined (_GLIBCXX_CONDITION_VARIABLE) && !defined(_CONDITION_VARIABLE_)
    #include <condition_variable>
#endif


namespace aworx {


namespace lib { namespace threads { class Thread; }}

using Thread = aworx::lib::threads::Thread;
namespace lib { namespace threads {

/** ************************************************************************************************
 * While this class does not inherit from \alib{threads,ThreadLockNR}, it copies and extends its
 * interface and functionality.
 * Repeated acquisitions without prior invocations of #Release are supported with this type.
 *
 * An instance of this class is released when an equal amount of invocations to #Acquire and
 * #Release have been performed.
 *
 * The object stores the actual owning thread and this thread may be queried. Such
 * queries are \b not thread safe and should be performed only to perform tasks that are
 * not mission critical, for example to create log output, usage statistics or similar things.
 * In other words, a software's algorithmic logic should by principle never use information about
 * the thread that currently owns a lock.
 *
 * In debug compilations, a warning threshold for the number of repeated acquisitions can be
 * defined with public member #RecursionWarningThreshold. As the member's name indicates, it
 * is \e assumed that too many repeated locks are caused by a recursive calls. Usually, locking
 * data access should not be done in recursive code.
 **************************************************************************************************/
class ThreadLock
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** Counter for the number of Acquire() calls of the current thread. */
        int                         cntAcquirements                                              =0;

        /** Thread ID of the current owner. */
        Thread*                     owner;

        /** The internal object to lock on. */
        std::mutex                  mutex;

        /** The internal object to lock on. */
        std::condition_variable     mutexNotifier;

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * This is a threshold that causes Acquire() to send a warning to
         * \alib{results,ReportWriter} if
         * acquiring the access takes longer than the given number of milliseconds.
         * To disable such messages, set this value to 0. Default is 1 second.
         */
        integer                     WaitWarningTimeLimitInMillis                             =1000L;

        #if ALIB_DEBUG
            /** Debug information on acquirement location. */
            NCString                acquirementSourcefile                                  =nullptr;

            /** Debug information on acquirement location. */
            int                     acquirementLineNumber;

            /** Debug information on acquirement location. */
            NCString                acquirementMethodName                                  =nullptr;

            /**
             * Limit of recursions. If limit is reached or a multiple of it, an error is is passed
             * to \alib{results,ReportWriter}. Defaults is \c 10 in case of recursive instances,
             * to \c 1 otherwise.
             * Available only in debug versions of \alib.
             */
            int                     RecursionWarningThreshold                                   =10;
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
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         ******************************************************************************************/
         ALIB_API
         #if ALIB_DEBUG
                  void          Acquire( const NCString& file, int line, const NCString& func );
         #else
                  void          Acquire();
         #endif

        /** ****************************************************************************************
         * Releases ownership of this object. If #Acquire was called multiple times before, the same
         * number of calls to this method have to be performed to release ownership.
         ******************************************************************************************/
        ALIB_API  void          Release();

        /** ****************************************************************************************
         * Returns \c true if the next invocation of #Release will release the lock, otherwise
         * \c false. In other words, returns \c true if this lock is \ref Acquire "acquired" exactly
         * once.
         * and \c 1.
         *
         * @return \c true if locked exactly once.
         ******************************************************************************************/
        inline    bool          WillRelease()   const
        {
            return cntAcquirements == 1;
        }

        /** ****************************************************************************************
         * Returns the current owner of this lock.
         * If not acquired, \c nullptr is returned.
         *
         * @return The thread that owns this lock.
         ******************************************************************************************/
        inline    Thread*       Owner()         const
        {
            return reinterpret_cast<integer>(owner) == 1   ?  nullptr
                                                           :  owner;
        }

        /** ****************************************************************************************
         * Returns the number of acquirements of this ThreadLock.
         *
         * @return The number of (recursive) acquirements.
         ******************************************************************************************/
        inline int              CountAcquirements()   const
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
        ALIB_API  void          SetSafeness( Safeness safeness );

        /** ****************************************************************************************
         *  Query if this instance was set to unsafe mode.
         * @return A value of type aworx::lib::Safeness "Safeness"
         ******************************************************************************************/
        inline
        Safeness                GetSafeness()  const
        {
            return reinterpret_cast<integer>(owner) == 1 ? Safeness::Unsafe
                                                         : Safeness::Safe;
        }
};


}} // namespace aworx[::lib::threads]

/// Type alias in namespace #aworx.
using     ThreadLock=   aworx::lib::threads::ThreadLock;

}  // namespace [aworx]

#endif // HPP_ALIB_THREADS_THREADLOCK

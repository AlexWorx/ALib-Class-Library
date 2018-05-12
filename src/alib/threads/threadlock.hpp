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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_THREADS_THREADLOCK)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_THREADS_THREADLOCK
//! @cond NO_DOX
#define HPP_ALIB_THREADS_THREADLOCK 1
//! @endcond

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_THREADS_THREADLOCKNR)
    #include "alib/threads/threadlocknr.hpp"
#endif
#if !defined ( HPP_ALIB_THREADS_LIB )
    #include "threadlib.hpp"
#endif



namespace aworx { namespace lib { namespace threads {

/** ************************************************************************************************
 * This class allows *mutual exclusive access* to resources shared by different threads.
 * In other words, access to certain data that is accessed by different threads, can
 * be protected if each thread (aka critical section code) uses the same ThreadLock to control the
 * access to such data.
 *
 * If an Acquire is not followed by a corresponding Release(), other threads will not be able to
 * gain control to this object and will wait endlessly (deadlock situation).
 * When the Acquire and Release invocations happen within the same code block, then it is
 * highly recommended to use a stack instantiation of class ALIB::Owner to acquire and release
 * objects of this class.
 *
 * ThreadLock uses an internal counter to allow multiple calls to Acquire() and to be freed only
 * when a same amount of Release() calls are performed. This behavior can be switched off by a
 * constructor parameter. If switched off, each recursive Acquire() call will not be counted and
 * each call to Release() will instantly free the mutex. This mode is not very recommended,
 * the standard use is recursive mode.
 *
 * Furthermore, ThreadLock allows to disable locking using setUnsafe(). The objective here is to
 * gain execution speed, as thread synchronization causes relatively expensive system calls.
 * Nevertheless, it is sometimes obvious that the same code may run in a thread safe mode
 * one time and without thread locking the next time. Therefore, for performance critical
 * code, it is quite useful to be able to control this behavior.
 **************************************************************************************************/
class ThreadLock
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * This is a threshold that causes Acquire() to send a warning to
         * \ref aworx::lib::lang::ReportWriter "ReportWriter" if
         * acquiring the access takes longer than the given number of milliseconds.
         * To disable such messages, set this value to 0. Default is 1 second.
         */
        integer                       WaitWarningTimeLimitInMillis                           =1000L;

        #if ALIB_DEBUG
            /// Debug information on acquirement location.
            NTString                  acquirementSourcefile                                =nullptr;

            /// Debug information on acquirement location.
            int                       acquirementLineNumber;

            /// Debug information on acquirement location.
            NTString                  acquirementMethodName                                =nullptr;

            /**
             * Limit of recursions. If limit is reached or a multiple of it, an error is is passed
             * to \alib{lang,ReportWriter}. Defaults is \c 10 in case of recursive instances,
             * to \c 1 otherwise.
             * Available only in debug versions of \alib.
             */
            int                       RecursionWarningThreshold                                 =10;
        #endif
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// Flag if recursion support is on. If not, nested locks are not counted.
        lang::LockMode                lockMode;

        /// Counter for the number of Acquire() calls of the current thread.
        int                           cntAcquirements                                            =0;

        /// Thread ID of the current owner.
        Thread*                       owner                                                =nullptr;

        #if ALIB_FEAT_THREADS

            /// The internal object to lock on.
            std::mutex*               mutex                                                =nullptr;

            /// The internal object to lock on.
            std::condition_variable*  mutexNotifier                                        =nullptr;
        #else
            void*                     mutex                                                =nullptr;
        #endif

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Create a ThreadLock that allows recursion. A warning will be given (ALIB Error) when the
         * given recursion level is reached (and each multiple of it). In addition the lock can be
         * initialized to be unsafe, which means the locking critical sections is disabled.
         *
         * @param lockMode  (Optional) Flag if recursion support is on (the default).
         *                  If not, nested locks are not counted.
         * @param safeness  (Optional) Defaults to \c Safeness::Safe.
         *                  See #SetSafeness for more information.
         ******************************************************************************************/
        ALIB_API
        explicit          ThreadLock(  lang::LockMode lockMode=  lang::LockMode::Recursive,
                                       lang::Safeness safeness=  lang::Safeness::Safe);

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
         *   In the debug-compilation of an application, this method accepts the parameters,
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
                  void          Acquire( const NTString& file, int line, const NTString& func );
         #else
                  void          Acquire();
         #endif

        /** ****************************************************************************************
         * Releases ownership of this object. If #Acquire was called multiple times before, the same
         * number of calls to this method have to be performed to release ownership.
         ******************************************************************************************/
        ALIB_API  void          Release();

        /** ****************************************************************************************
         * Returns \c true if the next invocation of #Release will release the lock.
         * Returns \c false, if recursive acquirements have been performed.
         *
         * @return \c true if locked exactly once.
         ******************************************************************************************/
        inline    bool          WillRelease()   const   { return cntAcquirements == 1; }

        /** ****************************************************************************************
         * Returns the current owner of this lock.
         * If not acquired, \c nullptr is returned.
         *
         * @return The thread that owns this lock.
         ******************************************************************************************/
        inline    Thread*       Owner()         const   { return owner; }

        /** ****************************************************************************************
         * Returns the number of acquirements of this ThreadLock.
         *
         * @return The number of (recursive) acquirements.
         ******************************************************************************************/
        inline int           CountAcquirements()   const
        {
            return cntAcquirements;
        }

        /** ****************************************************************************************
         *  If parameter is \c Safeness::Unsafe, the whole locking system is disabled.
         *  The only objective here is to to gain execution speed, as thread synchronization causes
         *  relatively expensive system calls.
         *  Use this method only if you are 100% sure that your (otherwise) critical section are
         *  executed in a single threaded environment. And: "relative expensive" means: they are not
         *  really expensive. This is provided only for the rare case that your critical section is
         *  very, very frequently executed.
         *
         * @param safeness  Determines if this object should use a mutex (\c Safeness::Safe)
         *                  or just do nothing (\c Safeness::Unsafe).
         ******************************************************************************************/
        ALIB_API  void          SetSafeness( lang::Safeness safeness );

        /** ****************************************************************************************
         *  Query if this instance was set to unsafe mode.
         * @return A value of type aworx::lib::lang::Safeness "Safeness"
         ******************************************************************************************/
        inline
        lang::Safeness          GetSafeness()  const
        {
            return mutex == nullptr ? lang::Safeness::Unsafe
                                    : lang::Safeness::Safe;
        }

        /** ****************************************************************************************
         *  Query if this instance was set to work recursively.
         * @return A value of type aworx::lib::lang::LockMode "LockMode"
         ******************************************************************************************/
        inline
        lang::LockMode          GetMode()     const           { return lockMode; }

};


}} // namespace lib::threads

/// Type alias in namespace #aworx.
using     ThreadLock=   aworx::lib::threads::ThreadLock;

}  // namespace aworx

#endif // HPP_ALIB_THREADS_THREADLOCK

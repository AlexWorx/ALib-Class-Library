/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_threads of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_THREADS_SLEEPER
#define HPP_ALIB_THREADS_SLEEPER 1

#if !defined (HPP_ALIB_LANG_OWNER)
    #include "alib/lang/owner.hpp"
#endif

#if !defined(HPP_ALIB_THREADS_THREAD)
#   include "alib/threads/thread.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#    include "alib/strings/localstring.hpp"
#endif

#if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
    #include <mutex>
#endif

#if !defined (_GLIBCXX_CONDITION_VARIABLE) && !defined(_CONDITION_VARIABLE_)
    #include <condition_variable>
#endif

namespace alib {  namespace threads {

/** ************************************************************************************************
 * This class is a simple wrapper around C++ standard library types \c std::mutex and
 * \c std::condition_variable and allows a thread to sleep until another thread wakes
 * the thread up.
 *
 * Prior to using one of the overloaded sleep methods #WaitForNotification, the object
 * has to be \ref Acquire "acquired". During sleep, a notifying thread may, but in most
 * situations should not acquire this object. Further details are given with the documentation
 * of method #Notify.
 *
 * This class does not allow repeated calls to method #Acquire without prior invocations of
 * #Release. Repeated acquisitions cause undefined behavior.
 * With debug builds, an assertion is raised when #Acquire is invoked while the lock is already
 * acquired. In this respect, this class compares to that of \alib{threads,ThreadLockNR} and not
 * to that of \alib{threads,ThreadLock}
 *
 * Multiple threads may sleep in parallel using a single instance of this object.
 * The notification thread may either wake up one of them or wake all up by using
 * #NotifyAll.
 **************************************************************************************************/
class Sleeper
{
    protected:
        /** The mutex used for locking. */
        typename std::mutex         mutex;

        /** The condition variable used for sleeping and notification to wake up. */
        std::condition_variable     event;

        /** Flag used to detect "spurious" wake-ups. */
        bool                        wasNotified;

        #if ALIB_DEBUG
            /** Source location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFile                                           =nullptr;

            /** Source location of acquirement. (Available only in debug-builds.). */
            int                     DbgOwnerLine;

            /** Source location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFunc                                           =nullptr;


            /** Id of a thread that currently acquired this object's mutex, used to detect
             *  invocations of methods #WaitForNotification and #Release without prior acquirement,
             *  as well as forbidden multiple (nested) acquirements.s
             *  Available only in debug compilations. */
            std::thread::id         dbgIsAcquiredBy;
        #endif

    public:
        /** ****************************************************************************************
         * Default constructor.
         ******************************************************************************************/
        Sleeper() = default;

        /** ****************************************************************************************
         * A thread which invokes this method gets registered as the current owner of this object,
         * until the same thread releases the ownership invoking #Release.
         * In the case that this object is already owned by another thread, the invoking thread is
         * suspended until ownership can be gained.<p>
         * Multiple (nested) calls to this method are <b>not supported</b> and lead to
         * undefined behavior. In debug-compilations, an assertion is raised.
         *
         * \note
         *   In debug-compilations of the library, this method accepts three parameters,
         *   providing information about the caller. In the release version these parameters do not
         *   exist. Therefore use macro \ref ALIB_CALLER_PRUNED to provide the parameters:
         *
         *          sample.Acquire( ALIB_CALLER_PRUNED );
         *
         * @param dbgFile  Caller information. Available only with debug builds.
         * @param dbgLine  Caller information. Available only with debug builds.
         * @param dbgFunc  Caller information. Available only with debug builds.
         ******************************************************************************************/
         #if ALIB_DEBUG
            void  Acquire( const NCString& dbgFile, int dbgLine, const NCString& dbgFunc )
            {
                DbgOwnerFile= dbgFile;
                DbgOwnerLine= dbgLine;
                DbgOwnerFunc= dbgFunc;
         #else
            void  Acquire()
            {
         #endif
                ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::this_thread::get_id(),
                                   "THREADS", "Multiple acquirements of Sleeper are forbidden." )
                mutex.lock();
                ALIB_DBG( dbgIsAcquiredBy= std::this_thread::get_id(); )
            }

        /** ****************************************************************************************
         * Releases ownership of this object.
         * If this method is invoked on an object that is not acquired or acquired by a different
         * thread, in debug-compilations an assertion is raised.
         * In release compilations, this leads to undefined behavior.
         ******************************************************************************************/
        void Release()
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Release without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Release while ownership is with a different thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"
                               )
            ALIB_DBG( dbgIsAcquiredBy= std::thread::id(); )
            mutex.unlock();
        }


        /** ****************************************************************************************
         * Wakes up the next sleeping thread.<p>
         * It is not necessary and not even recommended to acquiring this object, prior to waking
         * the next thread up. However, in some situations, it should be done. Further
         * explanation on this is given with the documentation of the standard C++ library,
         * which is quoted here:<p>
         * <em>
         * "The notifying thread does not need to hold the lock on the same mutex as the one held
         * by the waiting thread(s); in fact doing so is a pessimization, since the notified thread
         * would immediately block again, waiting for the notifying thread to release the lock.
         * However, some implementations (in particular many implementations of pthreads) recognize
         * this situation and avoid this "hurry up and wait" scenario by transferring the waiting
         * thread from the condition variable's queue directly to the queue of the mutex within
         * the notify call, without waking it up.<p>
         * Notifying while under the lock may nevertheless be necessary when precise scheduling of
         * events is required, e.g. if the waiting thread would exit the program if the condition
         * is satisfied, causing destruction of the notifying thread's condition variable.
         * A spurious wakeup after mutex unlock but before notify would result in notify called
         * on a destroyed object.
         * </em>
         ******************************************************************************************/
        void Notify()
        {
            wasNotified= true;
            event.notify_one();
        }

        /** ****************************************************************************************
         * Wakes up all sleeping threads.<p>
         * \see For explanations about the necessity of acquiring this object prior to
         *      to notification, see documentation of method #Notify.
         ******************************************************************************************/
        void NotifyAll()
        {
            wasNotified= true;
            event.notify_all();
        }

        /** ****************************************************************************************
         * Waits for notification (for an unlimited time).<p>
         * Prior to invoking this method, this object has to be \ref Acquire "acquired".
         * After the wake up call, the internal mutex is (again) acquired and thus has to be
         * released later.
         ******************************************************************************************/
        void WaitForNotification()
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Wait method called without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Wait method called while owned by another thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"
                               )

            std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
            wasNotified= false;
            event.wait( lock, [this]{ return wasNotified; } );
        }

#if ALIB_TIME
        /** ****************************************************************************************
         * Waits for notification, but only fore a given duration..<p>
         * Prior to invoking this method, this object has to be \ref Acquire "acquired".
         * After the wake up call, the internal mutex is (again) acquired and thus has to be
         * released later.
         *
         * \note In the absense of module \alib_time, this method has template parameters and
         *       parameter \p maxSleepTime will be defined like in
         *       <c>std::condition_variable::wait_for</c> as
         *       <c>const std::chrono::duration<Rep, Period>&</c>.
         *
         * @param maxSleepTime    The maximum time to wait.
         ******************************************************************************************/
        void WaitForNotification( const Ticks::Duration& maxSleepTime )
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Wait method called without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Wait method called while owned by another thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"       )
           std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
           wasNotified= false;
           event.wait_for( lock, maxSleepTime.Export(), [this]{ return wasNotified; } );
        }

        /** ****************************************************************************************
         * Waits for notification, but only fore a given duration.
         * Prior to invoking this method, this object has to be \ref Acquire "acquired".
         * After the wake up call, the internal mutex is (again) acquired and thus has to be
         * released later.
         *
         * \note In the absense of module \alib_time, this method has template parameters and
         *       parameter \p maxSleepTime will be defined like in
         *       <c>std::condition_variable::WaitForNotification</c> as
         *       <c>const std::chrono::time_point<Clock, Duration>&</c>.
         *
         * @param wakeUpTime    The point in time to wake up, even if not notified.
         ******************************************************************************************/
        void WaitForNotification( const Ticks& wakeUpTime )
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Wait method called without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Wait method called while owned by another thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"
                               )
            std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
            wasNotified= false;
            event.wait_until( lock, wakeUpTime.Export(), [this]{ return wasNotified; } );
        }
#else
        template< class Rep, class Period >
        void WaitForNotification( const std::chrono::duration<Rep, Period>& maxSleepTime )
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Wait method called without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Wait method called while owned by another thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"       )
           std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
           wasNotified= false;
           event.wait_for( lock, maxSleepTime, [this]{ return wasNotified; } );
        }

        template< class Clock, class Duration >
        void WaitForNotification( const std::chrono::time_point<Clock, Duration>& wakeUpTime )
        {
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy != std::thread::id() ,
                               "THREADS", "Wait method called without prior acquisition" )
            ALIB_ASSERT_ERROR( dbgIsAcquiredBy == std::this_thread::get_id(), "THREADS",
                               NString256() <<
                               "Wait method called while owned by another thread.\n" <<
                               "   This thread: " << Thread::GetCurrent()->GetName() << " (ID: "
                                                  << Thread::GetCurrent()->GetId()   << ")\n"
                               "         Owner: " << detail::getThread(dbgIsAcquiredBy)->GetName() << " (ID: "
                                                  << detail::getThread(dbgIsAcquiredBy)->GetId()   << ")"
                               )
            std::unique_lock<std::mutex> lock(mutex, std::adopt_lock);
            wasNotified= false;
            event.wait_until( lock, wakeUpTime , [this]{ return wasNotified; } );
        }
#endif

};


} // namespace alib[::threads]

/// Type alias in namespace \b alib.
using     Sleeper=   threads::Sleeper;

} // namespace [alib]

#endif // HPP_ALIB_THREADS_SLEEPER

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_threads of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_THREADS_THREADLOCKNR
#define HPP_ALIB_THREADS_THREADLOCKNR 1

#if !defined (HPP_ALIB_FS_OWNER_OWNER)
    #include "alib/lib/fs_owner/owner.hpp"
#endif

ALIB_ASSERT_MODULE(THREADS)

#if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
    #include <mutex>
#endif

// #################################################################################################
// Macros
// #################################################################################################
#define   ALIB_LOCK                     ALIB_OWN(*this)
#define   ALIB_LOCK_WITH(lock)          ALIB_OWN(lock)


namespace aworx { namespace lib { namespace threads {
/** ************************************************************************************************
 * This class is a simple wrapper around C++ standard library type \c std::mutex and hence allows
 * <em>mutual exclusive access</em> to resources, by protecting data from concurrent thread access.
 *
 * When a pair of #Acquire and #Release invocations is performed within the same code block, then
 * it is recommended to use a stack instantiation of class \alib{Owner} to acquire and release
 * objects of this class. Such use is highly simplified with macros \ref ALIB_LOCK and
 * \ref ALIB_LOCK_WITH.
 *
 * This class allows to be "disabled" with method #SetSafeness. The objective here is to
 * gain execution speed, as thread synchronization causes "relatively" expensive system calls.
 * An interface of a class might this way be designed to be "thread safe" by default, but in the
 * case that a user of such class assures that an individual instance is used in a context that
 * is free of race conditions, a corresponding lock might be disabled.
 *
 * This class does not allow repeated calls to method #Acquire without prior invocations of
 * #Release. Repeated acquisitions cause undefined behavior.
 * With debug builds, an assertion is raised when #Acquire is invoked while the lock is already
 * acquired.
 *
 * Due to this limitation, the class performs several times faster than sibling class
 * \alib{threads,ThreadLock}. For very time critical code sections which are invoked
 * often in relation to their length, the use of this class might be considered, taking its
 * limitation into account.
 **************************************************************************************************/
class ThreadLockNR
{
    protected:
        /** The std::mutex used for locking. */
        typename std::mutex         mutex;

        /** The safeness mode. */
        Safeness                    safeness;

        #if ALIB_DEBUG
            /** Location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFile                                           =nullptr;

            /** Location of acquirement. (Available only in debug-builds.). */
            int                     DbgOwnerLine;

            /** Location of acquirement. (Available only in debug-builds.). */
            NCString                DbgOwnerFunc                                           =nullptr;


            /** Counter of (forbidden!) recursive acquirements. Available only in debug
             *  compilations. */
            bool                    dbgIsAcquired= false;
        #endif

    public:
        /** ****************************************************************************************
         * Simple constructor.
         *
         * @param pSafeness  The safeness mode. See #SetSafeness for more information.
         ******************************************************************************************/
        explicit
        ThreadLockNR( Safeness pSafeness= Safeness::Safe )
        : safeness( pSafeness )
        {}

        /** ****************************************************************************************
         * A thread which invokes this method gets registered as the current owner of this object,
         * until the same thread releases the ownership invoking #Release.
         * In the case that this object is already owned by another thread, the invoking thread is
         * suspended until ownership can be gained.
         * Multiple (nested) calls to this method are not supported and lead to undefined behavior.
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

                if ( safeness == Safeness::Safe )
                    mutex.lock();

                ALIB_DBG( dbgIsAcquired= true; )
            }

        /** ****************************************************************************************
         * Releases ownership of this object.
         * If this method is invoked on an object that is not acquired, in debug-compilations an
         * assertion is raised. In release compilations, this leads to undefined behavior.
         ******************************************************************************************/
        void Release()
        {
            ALIB_ASSERT_ERROR( dbgIsAcquired,  "Release without prior acquisition" )
            ALIB_DBG( dbgIsAcquired= false; )
            if ( safeness == Safeness::Safe )
                mutex.unlock();
        }

        /** ****************************************************************************************
         * If parameter is \c Unsafe, the whole locking system is disabled.
         * The only objective here is to gain execution speed, as thread synchronization causes
         * relatively expensive system calls. Use this method only if you are 100% sure that your
         * (otherwise) critical section is executed in a single threaded environment or otherwise
         * it is assured that no concurrent thread access is performed.
         *
         * Note that "relative expensive" means: locking is not "really" expensive.
         * This is only for the rare case that your critical section is very, very frequently
         * executed.
         *
         * @param pSafeness    Denotes the new safeness mode.
         ******************************************************************************************/
        void SetSafeness( Safeness pSafeness )
        {
            ALIB_ASSERT_ERROR( !dbgIsAcquired, "Changing safeness while acquired" )
            safeness= pSafeness;
        }

        /** ****************************************************************************************
         * Query the safeness mode of this object.
         * @return   The safeness mode of this object.
         ******************************************************************************************/
        Safeness GetSafeness()    const
        {
            return  safeness;
        }
};


}} // namespace aworx[::lib::threads]

/// Type alias in namespace #aworx.
using     ThreadLockNR= lib::threads::ThreadLockNR;

}  // namespace [aworx]

#endif // HPP_ALIB_THREADS_THREADLOCKNR

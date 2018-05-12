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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_THREADS_THREADLOCKNR)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_THREADS_THREADLOCKNR
//! @cond NO_DOX
#define HPP_ALIB_THREADS_THREADLOCKNR 1
//! @endcond

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_LANG_OWNER)
    #include "alib/lang/owner.hpp"
#endif

#if !defined (_GLIBCXX_MUTEX) && !defined(_MUTEX_)
    #include <mutex>
#endif

// #################################################################################################
// Macros (for technical reasons, doxed in file alib.cpp)
// #################################################################################################
#define   ALIB_LOCK                     ALIB_OWN(*this)
#define   ALIB_LOCK_WITH(lock)          ALIB_OWN(lock)


namespace aworx { namespace lib { namespace threads {
/** ************************************************************************************************
 * This class is a simple wrapper around std::mutex (provided solely to reach compatibility of ALIB
 * across programming languages) and hence allows *mutual exclusive access* to resources, by
 * protecting data from concurrent access by different threads.
 *
 * Unlike class ThreadLock, this class ThreadLockNR does not use an internal counter to allow
 * recursive calls to Acquire() and Release(). Also, no owner is recorded, no ALIB
 * \ref aworx::lib::lang::ReportWriter "ReportWriter"
 * is invoked on misuse and no time limit warnings are supported.
 * Consider using ThreadLock in most cases. The advantage of ThreadLockNR is that it operates
 * several times faster than ThreadLock. So, for very time critical sections with frequent
 * Lock.Release()use of ThreadLockNR can be taken into consideration.
 *
 * \attention Nested (recursive) acquisitions are not supported and lead to undefined behavior.
 **************************************************************************************************/
class ThreadLockNR
{
    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /// The internal object to lock on.
        #if ALIB_FEAT_THREADS
            std::mutex*             mutex                                                 = nullptr;
        #else
            void*                   mutex                                                 = nullptr;
        #endif

        #if ALIB_DEBUG
            /// Debug information on acquirement location.
            NTString                acquirementSourcefile                                  =nullptr;

            /// Debug information on acquirement location.
            int                     acquirementLineNumber;

            /// Debug information on acquirement location.
            NTString                acquirementMethodName                                  =nullptr;

            /// Counter of (forbidden!) recursive acquirements. Available only in debug compilations.
            bool                    dbgIsAcquired= false;
        #endif

    // #############################################################################################
    // constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Create a ThreadLockNR that does not allow recursion. Consider using class ThreadLock
         *  instead of this.
         * @param safeness  (Optional) Can be used to set this unsafe mode. See \#SetMode
         *                  for more information.
         *
         ******************************************************************************************/
        explicit
        ThreadLockNR( lang::Safeness safeness= lang::Safeness::Safe )
        {
            mutex= safeness == lang::Safeness::Unsafe ? nullptr
                                                   #if ALIB_FEAT_THREADS
                                                       : new std::mutex();
                                                   #else
                                                       : reinterpret_cast<void*>(1);
                                                   #endif
        }

        /** ****************************************************************************************
         *   Destructor.    *
         ******************************************************************************************/
        virtual ~ThreadLockNR()
        {
            #if ALIB_FEAT_THREADS
                if( mutex != nullptr )
                    delete mutex;
            #endif
        }

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Thread which invokes this method gets registered  as the current owner of this object,
         * until the same thread releases the ownership invoking #Release.
         * In the case that this object is already owned by another thread, the invoking thread is
         * suspended until ownership can be gained.
         * Multiple (nested) calls to this method are not supported and lead to undefined behavior.
         * @param file  Caller information. Available only in debug compilations.
         * @param line  Caller information. Available only in debug compilations.
         * @param func  Caller information. Available only in debug compilations.
         ******************************************************************************************/
         #if ALIB_DEBUG
            virtual void  Acquire( const NTString& file, int line, const NTString& func )
            {
                acquirementSourcefile= file;
                acquirementLineNumber= line;
                acquirementMethodName= func;

         #else
            virtual void  Acquire()
            {
         #endif

                ALIB_ASSERT_ERROR( !dbgIsAcquired,
                        ASTR("Must not be recursively acquired. Use class ThreadLock if recursion is needed") );

                #if ALIB_FEAT_THREADS
                    if ( mutex != nullptr )
                        mutex->lock();
                #endif

                ALIB_DBG( dbgIsAcquired= true; )
            }

        /** ****************************************************************************************
         * Releases ownership of this object. If #Acquire was called multiple times before, the same
         * number of calls to this method have to be performed to release ownership.
         ******************************************************************************************/
        virtual void Release()
        {
            ALIB_ASSERT_ERROR( dbgIsAcquired,  ASTR("Release without prior acquisition") );
            #if ALIB_FEAT_THREADS
                if ( mutex != nullptr )
                        mutex->unlock();
            #endif
            ALIB_DBG( dbgIsAcquired= false; )
        }

        /** ****************************************************************************************
         *  If parameter is \c true, the whole locking system is disabled. The only objective here is
         *  to gain execution speed, as thread synchronization causes relatively expensive system
         *  calls. Use this method only if you are 100% sure that your (otherwise) critical section
         *  are executed in a single threaded environment. And: "relative expensive" means:
         *  they are not really expensive. This is only for the rare case that your critical
         *  section is very, very frequently executed.
         * @param safeness   Denotes the safeness mode.
         ******************************************************************************************/
        void SetSafeness( lang::Safeness safeness )
        {
            #if ALIB_FEAT_THREADS
                if( safeness == lang::Safeness::Unsafe && mutex != nullptr )
                {
                    delete mutex;
                    mutex= nullptr;
                    return;
                }

                if( safeness == lang::Safeness::Safe && mutex == nullptr )
                {
                    mutex= new std::mutex();
                    return;
                }
            #else
                mutex= safeness == lang::Safeness::Safe ? reinterpret_cast<void*>(1) : nullptr;
            #endif
        }

        /** ****************************************************************************************
         * Query if this instance was set to unsafe mode.
         * @return   The safeness mode of this object.
         ******************************************************************************************/
        lang::Safeness GetSafeness()    const
        {
            return  mutex == nullptr ? lang::Safeness::Unsafe
                                     : lang::Safeness::Safe;
        }
};


}} // namespace lib::threads

/// Type alias in namespace #aworx.
using     ThreadLockNR= aworx::lib::threads::ThreadLockNR;

}  // namespace aworx

#endif // HPP_ALIB_THREADS_THREADLOCKNR

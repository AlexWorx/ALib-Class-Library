// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlock.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_DETAIL_THREADMAP)
    #include "alib/threads/detail/threadmap.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif
#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib { namespace threads {

ThreadLock::ThreadLock( Safeness pSafeness )
: owner(std::thread::id())
, safeness(pSafeness)
{}

ThreadLock::~ThreadLock()
{
    ALIB_ASSERT_WARNING( cntAcquirements == 0, "THREADS", "Lcok destruction while locked" )
}


#if !defined(ALIB_DOX)

ALIB_REL_DBG(
void ThreadLock::Acquire() ,
void ThreadLock::Acquire( const NCString& dbgFile, int dbgLine, const NCString& dbgFunc )   )
{
    // are we in unsafe mode?
    if ( GetSafeness() == Safeness::Unsafe )
    {
        // we are still increasing the cntAcquirements
        ++cntAcquirements;

        // reached warning limit?
        ALIB_ASSERT_WARNING(   cntAcquirements                                   != 0
                            && DbgRecursionWarningThreshold                      != 0
                            && (cntAcquirements % DbgRecursionWarningThreshold)  != 0,
          "THREADS", "Recursion depth warning.\n"
          "To prevent this, change ThreadLock.recursionWarningThreshold or fix your code.\n"
          "Depth: ", cntAcquirements )

        // end of unsafe version of this method
        return;
    }

    auto thisThreadID= std::this_thread::get_id();

    // this thread already owns the lock
    if( thisThreadID == owner  )
    {
        // we are still increasing the cntAcquirements
        ++cntAcquirements;

        // reached warning limit?
        ALIB_ASSERT_WARNING(   cntAcquirements                                   != 0
                            && DbgRecursionWarningThreshold                      != 0
                            && (cntAcquirements % DbgRecursionWarningThreshold)  != 0,
          "THREADS", "Recursion depth warning.\n"
          "To prevent this, change ThreadLock.recursionWarningThreshold or fix your code.\n"
          "Depth: ", cntAcquirements )

        // that's it
        return;
    }

    #if ALIB_DEBUG
        bool    hasWarned= false;
        std::chrono::steady_clock::time_point   waitTime= std::chrono::steady_clock::now();
    #endif

    // synchronize on mutex
    {std::unique_lock<std::mutex> lock(mutex);

        // if we do not own this object, wait until it is set free by somebody else
        while( owner != std::thread::id() ) // needs a loop due to spurious wakeups
        {
            // wait unconditional
    #if ALIB_DEBUG
          if( DbgWarningAfterWaitTimeInMillis <= 0 || hasWarned )
    #endif

            mutexNotifier.wait( lock ); // releases the lock and waits on the notifier


    #if ALIB_DEBUG
            // wait with time limit
            else
            {
                mutexNotifier.wait_for( lock, std::chrono::milliseconds(DbgWarningAfterWaitTimeInMillis) );

                auto milliseconds= std::chrono::duration_cast<std::chrono::milliseconds>(
                                              std::chrono::steady_clock::now() - waitTime ).count();

                if ( milliseconds >= DbgWarningAfterWaitTimeInMillis )
                {
                    // release the lock to allow the warning. But copy the owner's id before.
                    auto actualOwner= owner;
                    lock.unlock();

                    hasWarned= true;
                    Thread* thisThread = Thread::GetCurrent();
                    Thread* ownerThread= detail::getThread(actualOwner);
                    ALIB_WARNING( "THREADS", NString1K()
                        << "Waiting on ThreadLock since " << milliseconds << " ms. Reasons might be "
                           "a dead lock, an non-optimized critical section\n"
                           "or simply too much load on the executing machine. More Info:"
                        << "\n  Owner       : " << ownerThread->GetId() << '/' << ownerThread->GetName()
                        << " at: " << DbgOwnerFile << ':' << DbgOwnerLine << " " << DbgOwnerFunc << "()."
                        << "\n  This thread : " << thisThread ->GetId() << '/' << thisThread ->GetName()
                        << " at: " << dbgFile      << ':' << dbgLine      << " " << dbgFunc      << "()."
                        )
                    lock.lock();
                }
            }
    #endif

        } // while loop

        // take control
        owner=           thisThreadID;
        cntAcquirements= 1;
    #if ALIB_DEBUG
        DbgOwnerFile= dbgFile;
        DbgOwnerLine= dbgLine;
        DbgOwnerFunc= dbgFunc;
    #endif

    }
}

#endif //!defined(ALIB_DOX)

void ThreadLock::Release()
{
    // are we in unsafe mode?
    if ( GetSafeness() == Safeness::Unsafe )
    {
        // not locked
        if( cntAcquirements == 0 )
        {
            ALIB_ERROR( "THREADS", "Release without acquire (unsafe mode)."
                        "Note: This must never happen, check your code, set lock to safe mode!" )
        }

        // we are still decreasing the cntAcquirements
        --cntAcquirements;

        // end of unsafe version of this method
        return;
    }

    // synchronize on mutex
    {
        std::unique_lock<std::mutex> lock(mutex);

        // not locked?
        ALIB_ASSERT_ERROR( cntAcquirements != 0, "THREADS", "Illegal release without acquire (safe mode)." )

        // decreasing the cntAcquirements
        --cntAcquirements;

        // release and notify next waiting thread
        if( cntAcquirements == 0 )
        {
            owner=    std::thread::id();
            mutexNotifier.notify_one();
        }
    } // synchronized
}


void ThreadLock::SetSafeness( Safeness newSafeness )
{
    // are we in unsafe mode?
    if ( safeness == Safeness::Unsafe )
    {
        // already locked? ALIB Error
        if( cntAcquirements != 0 )
        {
            ALIB_ERROR( "THREADS", "Cannot switch safeness mode while already locked.\n"
                        "  Current mode: unsafe, requested mode: ",
                        (newSafeness == Safeness::Safe ? "Safe" : "Unsafe" )
                      )

            return;
        }

        safeness= newSafeness;
        return;
    }

    // we are currently in safe mode: synchronize on mutex
    {
        std::unique_lock<std::mutex> lock(mutex);

        // already locked? Assertion
        if ( owner != std::thread::id() )
        {
            lock.unlock();
            ALIB_DBG(Thread* ownerThread= GetOwner() );
            ALIB_ERROR( "THREADS", NString256() <<
               "Cannot switch safeness mode while already locked.\n"
               "  Current mode: safe, requested mode: "  <<
                (safeness == Safeness::Safe ? "Safe" : "Unsafe" )
                << "\n"
               "  Owner: " << ownerThread->GetId() << '/' << ownerThread->GetName()    )
            return;
        }

        //  switch off?
        safeness= newSafeness;
    }
}


}}}// namespace [aworx::lib::threads]



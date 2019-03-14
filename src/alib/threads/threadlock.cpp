// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/threads/threadlock.hpp"


#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_THREAD)
    #include "alib/threads/thread.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

namespace aworx { namespace lib { namespace threads {

ThreadLock::ThreadLock( Safeness pSafeness )
// writing 1 to field owner marks this object to be "unsafe"
: owner(pSafeness == Safeness::Safe ? nullptr : reinterpret_cast<Thread*>(1) )
{}

ThreadLock::~ThreadLock()
{
    ALIB_ASSERT_WARNING( cntAcquirements == 0, "Destruction while locked" )
}

#if !ALIB_DOCUMENTATION_PARSER

ALIB_REL_DBG(
void ThreadLock::Acquire() ,
void ThreadLock::Acquire( const NCString& file, int line, const NCString& func )   )
{
    // are we in unsafe mode?
    if ( GetSafeness() == Safeness::Unsafe )
    {
        // we are still increasing the cntAcquirements
        cntAcquirements++;

        // reached warning limit?
        ALIB_ASSERT_WARNING( cntAcquirements > 0 && cntAcquirements % RecursionWarningThreshold != 0,
                            "Recursion depth warning.\n"
                            "To prevent this, change ThreadSafe.recursionWarningThreshold or fix your code.\n"
                            "Depth: ", cntAcquirements )

        // end of unsafe version of this method
        return;
    }


    // get current thread
    Thread* thisThread= Thread::GetCurrent();

    // synchronize on mutex
    std::unique_lock<std::mutex> lock(mutex);  {

        // we already own the thread
        if( owner == thisThread )
        {
            // we are still increasing the cntAcquirements
            cntAcquirements++;
            // reached warning limit?
            ALIB_ASSERT_WARNING( cntAcquirements > 0  && cntAcquirements % RecursionWarningThreshold != 0,
                                "Recursion depth warning.\n"
                                "To prevent this, change ThreadSafe.recursionWarningThreshold or fix your code.\n"
                                "Depth: ", cntAcquirements )

            lock.unlock();
            return;
        }

        // we do not own this thread, wait until lock is free
        bool hasWarned= false;
        while( owner != nullptr )
        {
            // wait unconditional
            if ( WaitWarningTimeLimitInMillis <= 0 || hasWarned )
                mutexNotifier.wait( lock );

            // wait with time limit
            else
            {
                Ticks   waitTime;
                mutexNotifier.wait_for( lock,  std::chrono::milliseconds(WaitWarningTimeLimitInMillis) );
                auto millis=   waitTime.Age().InAbsoluteMilliseconds();
                if ( millis >= WaitWarningTimeLimitInMillis )
                {
                    hasWarned= true;
                    ALIB_WARNING( NString1K() <<
                           "Timeout acquiring lock (" << millis
                        << "). Change your code's critical section length if possible."
                        << "\n  This thread: " << thisThread->GetId() << '/' << thisThread->GetName()
                        << "\n  Owner:       " <<      owner->GetId() << '/' <<      owner->GetName()
                        << "\n  Location of acquirement: " << acquirementSourcefile
                                                           << '[' << acquirementLineNumber << "]: "
                                                           << acquirementMethodName << "()."
                        )
                }
            }
        }

        // take control
        owner=           thisThread;
        cntAcquirements= 1;
        ALIB_DBG(  acquirementSourcefile= file;
                   acquirementLineNumber= line;
                   acquirementMethodName= func;      )

    } lock.unlock(); // synchronized
}

#endif //!ALIB_DOCUMENTATION_PARSER

void ThreadLock::Release()
{
    // are we in unsafe mode?
    if ( GetSafeness() == Safeness::Unsafe )
    {
        // not locked
        if( cntAcquirements == 0 )
        {
            ALIB_ERROR( "Release without acquire (unsafe mode)."
                        "Note: This must never happen, check your code, set lock to safe mode!" );
        }

        // we are still decreasing the cntAcquirements
        cntAcquirements--;

        // end of unsafe version of this method
        return;
    }

    // synchronize on mutex
    std::unique_lock<std::mutex> lock(mutex); {

        // not locked?
        ALIB_ASSERT_ERROR( cntAcquirements != 0, "Illegal release without acquire (safe mode)." )

        // decreasing the cntAcquirements
        cntAcquirements--;

        // release and notify next waiting thread
        if( cntAcquirements == 0 )
        {
            owner=    nullptr;
            mutexNotifier.notify_one();
        }
    } lock.unlock(); // synchronized
}


void ThreadLock::SetSafeness( Safeness safeness )
{
    //

    // are we in unsafe mode?
    if ( reinterpret_cast<integer>(owner) == 1 )
    {
        // already locked? ALIB Error
        if( cntAcquirements != 0 )
        {
            ALIB_ERROR( "Cannot switch safeness mode while already locked.\n"
                        "  Current mode: unsafe, requested mode: ",
                        (safeness == Safeness::Safe ? "Safe" : "Unsafe" )
                      )

            return;
        }

        //  switch on?
        if( safeness == Safeness::Safe )
        {
            owner= nullptr;
        }

        // end of unsafe version of this method
        return;
    }

    // synchronize on mutex
    std::unique_lock<std::mutex> lock(mutex); {
        // already locked? Assertion
        if ( owner != nullptr )
        {
            ALIB_ERROR( NString256() <<
               "Cannot switch safeness mode while already locked.\n"
               "  Current mode: safe, requested mode: "  <<
                (safeness == Safeness::Safe ? "Safe" : "Unsafe" )
                << "\n"
               "  Owner: " << owner->GetId() << '/' << owner->GetName()  )
            return;
        }

        //  switch off?
        if( safeness == Safeness::Unsafe )
        {
            owner= reinterpret_cast<Thread*>(1);
            lock.unlock();
            return;
        }
    } lock.unlock(); // synchronized
}


}}}// namespace [aworx::lib::threads]



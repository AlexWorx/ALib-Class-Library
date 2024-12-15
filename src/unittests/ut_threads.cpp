// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_THREADS

#include "alib/alox.hpp"

#if ALIB_TIME
#   include "alib/time/timepointbase.hpp"
#   include "alib/time/ticks.hpp"
#endif
#if ALIB_MONOMEM
#   include "alib/monomem/sharedmonoval.hpp"
#endif

#include "alib/threads/thread.hpp"
#include "alib/threads/lock.hpp"
#include "alib/threads/timedlock.hpp"
#include "alib/threads/recursivelock.hpp"
#include "alib/threads/recursivetimedlock.hpp"
#include "alib/threads/sharedlock.hpp"
#include "alib/threads/sharedtimedlock.hpp"
#include <iostream>

#define TESTCLASSNAME       UT_Threads
#include "unittests/aworx_unittests.hpp"

using namespace alib;
using namespace std;
using namespace std::literals::chrono_literals;
namespace ut_aworx {

UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- helper-classes
//--------------------------------------------------------------------------------------------------

class Test_ThreadLock_SharedInt
{
    public:        int val= 0;
};

class Test_ThreadLock_TestThread : public Thread
{
    public:
        AWorxUnitTesting* UT;
        RecursiveLock*    Mutex;
        int               HoldTime;
        int               Repeats;
        bool              Verbose;
        int               TResult= 1;
        Test_ThreadLock_SharedInt* Shared;

    Test_ThreadLock_TestThread( AWorxUnitTesting* pUT, const String& tname, RecursiveLock* lock, int holdTime, int repeats, bool verbose, Test_ThreadLock_SharedInt* shared )
    :Thread( tname )
    {
        this->UT=        pUT;
        this->Mutex=     lock;
        this->HoldTime=  holdTime;
        this->Repeats=   repeats;
        this->Verbose=   verbose;
        this->Shared=    shared;
    }

    public: void Run()                                                                      override
    {
        AWorxUnitTesting &ut= *UT;
        UT_EQ( GetID(), Thread::GetCurrent()->GetID() )

        for ( int i= 0; i < Repeats ; ++i )
        {
            if (Verbose) { UT_PRINT( "Thread {!Q} acquiring lock...",GetName() ) }
            Mutex->AcquireRecursive(ALIB_CALLER_PRUNED);
            if (Verbose) { UT_PRINT( "Thread {!Q} has lock.", GetName() ) }

                int sVal= ++Shared->val;

                Thread::SleepMicros( HoldTime );

                Shared->val= sVal -1;

            if (Verbose) { UT_PRINT( "Thread {!Q} releasing lock.", GetName() ) }
            Mutex->ReleaseRecursive(ALIB_CALLER_PRUNED);
            if (Verbose) { UT_PRINT( "Thread {!Q} released lock." , GetName() ) }
        }

        TResult= 0;
        UT_PRINT( "Thread {!Q} terminates.", GetName() )

    }
};

//--------------------------------------------------------------------------------------------------
//--- Locks
//--------------------------------------------------------------------------------------------------
UT_METHOD( Locks )
{
    UT_INIT()

    //==============================  Assert release class footprint  ==============================
    #if !ALIB_DEBUG
        static_assert( sizeof(Lock              ) == sizeof(std::mutex                ) );
        static_assert( sizeof(TimedLock         ) == sizeof(std::timed_mutex          ) );
        static_assert( sizeof(RecursiveLock     ) == sizeof(std::recursive_mutex      ) );
        static_assert( sizeof(RecursiveTimedLock) == sizeof(std::recursive_timed_mutex) );
        static_assert( sizeof(SharedLock        ) == sizeof(std::shared_mutex         ) );
        static_assert( sizeof(SharedTimedLock   ) == sizeof(std::shared_timed_mutex   ) );
    #endif

    //==============================  Test Owners & Macros  ==============================
    {  Lock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)

        { ALIB_LOCK_WITH(lock) }
        { lang::OwnerTry<decltype(lock)> owner(lock                                   ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }
    {  TimedLock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)
        { ALIB_LOCK_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  RecursiveLock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)
        { ALIB_LOCK_RECURSIVE_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  RecursiveTimedLock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)
        { ALIB_LOCK_RECURSIVE_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  SharedLock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)
        { ALIB_LOCK_WITH(lock) }
        { ALIB_LOCK_SHARED_WITH(lock) }
        { lang::OwnerTry      <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerShared   <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
        { lang::OwnerTryShared<decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
    }

    {  SharedTimedLock lock;
       ALIB_DBG(lock.Dbg.Name= "UTLock";)
        { ALIB_LOCK_WITH(lock) }
        { ALIB_LOCK_SHARED_WITH(lock) }
        { lang::OwnerShared     <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
        { lang::OwnerTry        <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTryShared  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }

        { lang::OwnerTimed      <decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    //==============================  The same with TSharedMonoVal of module Monomem!  ==============================
    #if ALIB_MONOMEM
    {  TSharedMonoVal<int, HeapAllocator, Lock> lock(1, 100);
        { ALIB_LOCK_WITH(lock) }
        { lang::OwnerTry<decltype(lock)> owner(lock                                   ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }
    {  TSharedMonoVal<int, HeapAllocator, TimedLock> lock(1, 100);
        { ALIB_LOCK_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  TSharedMonoVal<int, HeapAllocator, RecursiveLock> lock(1, 100);
        { ALIB_LOCK_RECURSIVE_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  TSharedMonoVal<int, HeapAllocator, RecursiveTimedLock> lock(1, 100);
        { ALIB_LOCK_RECURSIVE_WITH(lock) }
        { lang::OwnerTry  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }

    {  TSharedMonoVal<int, HeapAllocator, SharedLock> lock(1, 100);
        { ALIB_LOCK_WITH(lock) }
        { ALIB_LOCK_SHARED_WITH(lock) }
        { lang::OwnerTry      <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerShared   <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
        { lang::OwnerTryShared<decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
    }

    {  TSharedMonoVal<int, HeapAllocator, SharedTimedLock> lock(1, 100);
        { ALIB_LOCK_WITH(lock) }
        { ALIB_LOCK_SHARED_WITH(lock) }
        { lang::OwnerShared     <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }
        { lang::OwnerTry        <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTryShared  <decltype(lock)> owner(lock                                 ALIB_COMMA_CALLER_PRUNED); }

        { lang::OwnerTimed      <decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerTimed      <decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, Ticks::Duration(1ms)           ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, 1ms                            ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, Ticks::Now() + 1ms             ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
        { lang::OwnerSharedTimed<decltype(lock)> owner(lock, (Ticks::Now() + 1ms).Export()  ALIB_COMMA_CALLER_PRUNED); UT_TRUE( owner.IsOwning() ) }
    }
    #endif // ALIB_MONOMEM


    //==============================  A few tests on debug-features   ==============================
#if ALIB_DEBUG
    lang::Report::GetDefault().PushHaltFlags( false, false );
    UT_PRINT("Class Lock")
    {
        {
            Lock lock;                           ALIB_DBG( UT_FALSE ( lock.Dbg.IsOwnedByCurrentThread() ); UT_TRUE ( nullptr             == lock.Dbg.GetOwner() ) )
            lock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_TRUE  ( lock.Dbg.IsOwnedByCurrentThread() ); UT_TRUE ( Thread::GetCurrent()== lock.Dbg.GetOwner() ) )
            lock.Release(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_FALSE ( lock.Dbg.IsOwnedByCurrentThread() ); UT_TRUE ( nullptr             == lock.Dbg.GetOwner() ) )

            #if ALIB_DEBUG
                UT_PRINT( "Two errors should follow: release without acquisition:" )
                lock.Release(ALIB_CALLER_PRUNED);
                UT_PRINT( "One error should follow: destructing acquired lock" )
            #endif
       }
        {
            Lock lock;                           ALIB_DBG( UT_FALSE ( lock.Dbg.IsOwnedByCurrentThread() ); UT_TRUE ( nullptr             == lock.Dbg.GetOwner() ) )
            lock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_TRUE  ( lock.Dbg.IsOwnedByCurrentThread() ); UT_TRUE ( Thread::GetCurrent()== lock.Dbg.GetOwner() ) )
            UT_PRINT( "One error should follow: destructing acquired lock" )
        }
    }
    
    UT_PRINT("Class RecursiveLock")
    {
        RecursiveLock recursiveLock;
        recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, recursiveLock.Dbg.CntAcquirements ) )
        recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, recursiveLock.Dbg.CntAcquirements ) )

        #if ALIB_DEBUG
            UT_PRINT( "One error should follow: release without acquisition:" )
            recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);   UT_TRUE (  recursiveLock.Dbg.CntAcquirements < 0)

            recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 0, recursiveLock.Dbg.CntAcquirements )
            recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 1, recursiveLock.Dbg.CntAcquirements )
            recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 0, recursiveLock.Dbg.CntAcquirements )
            recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 1, recursiveLock.Dbg.CntAcquirements )
            recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 0, recursiveLock.Dbg.CntAcquirements )
            recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);   UT_EQ ( 1, recursiveLock.Dbg.CntAcquirements )
            UT_PRINT( "One error should follow:destructing acquired lock" )
        #endif
    }

    // RecursiveLock warnings (10) locks:
    #if ALIB_DEBUG
    {
        RecursiveLock recursiveLock;
        UT_PRINT( "Two warnings should follow: " )
        for (int i= 0; i<20; ++i)
            recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);
        UT_TRUE ( recursiveLock.Dbg.CntAcquirements > 0 )
        for (int i= 0; i<20; ++i)
            recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);
        UT_EQ ( 0, recursiveLock.Dbg.CntAcquirements )
    }
    #endif

    UT_PRINT("Class SharedLock")
    {
      {
        SharedLock sharedLock;
        sharedLock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntAcquirements ) )
        sharedLock.Release(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
      }
      {
        SharedLock sharedLock;
        sharedLock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntAcquirements ) )
        sharedLock.Release(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )

        #if ALIB_DEBUG
            UT_PRINT( "Two errors should follow: not acquired / not owned:" )
            sharedLock.Release(ALIB_CALLER_PRUNED);   UT_TRUE (  sharedLock.Dbg.CntAcquirements < 0)
        #endif
      }
      {
        SharedLock sharedLock;
        ALIB_DBG(sharedLock.Dbg.Name= "UTSharedLock");
        sharedLock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntAcquirements ) )
        sharedLock.Release(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
        sharedLock.Acquire(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntAcquirements ) )

        #if ALIB_DEBUG
          // this test must not be done, because under linux, after the exception  (which is disabled here)
          // the mutex would just block and we would get a warning every two seconds, that's it.
          //  UT_PRINT( "One error should follow: nested acquirement:" )
          //  sharedLock.Acquire(ALIB_CALLER_PRUNED);
        #endif
        UT_PRINT( "One error should follow: Destructing acquired lock:" )
      }

      {
        SharedLock sharedLock;
        sharedLock.AcquireShared(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
                                                         ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntSharedAcquirements.load() ) )
        UT_PRINT( "One error should follow: Destructing shared-acquired lock:" )
      }

      {
        SharedLock sharedLock;
        sharedLock.Acquire(      ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntAcquirements ) )

        // we must not do this test: after the assertion (which is disabled here), it would go
        // to a blocking loop under linux.
        //UT_PRINT( "One error should follow: Shared-Acquire while acquired" )
        //sharedLock.AcquireShared(ALIB_CALLER_PRUNED);
      }

      {
        SharedLock sharedLock;
        sharedLock.AcquireShared(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
                                                         ALIB_DBG( UT_EQ ( 1, sharedLock.Dbg.CntSharedAcquirements.load() ) )
        sharedLock.ReleaseShared(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
                                                         ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntSharedAcquirements.load() ) )

        UT_PRINT( "One error should follow: shared release without acquisition:" )
        sharedLock.ReleaseShared(ALIB_CALLER_PRUNED);    ALIB_DBG( UT_EQ ( 0, sharedLock.Dbg.CntAcquirements ) )
      }
    }
    lang::Report::GetDefault().PopHaltFlags();
#endif //ALIB_DEBUG

}

//--------------------------------------------------------------------------------------------------
//--- SpeedTest
//--------------------------------------------------------------------------------------------------
#if ALIB_TIME && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
UT_METHOD( LockSpeedTest )
{
    UT_INIT()
    int64_t   repeats=      1000;
    int       outerRepeats=    3;

    for ( int run= 1; run <= outerRepeats; ++run )
    {
        UT_PRINT( "Run {}/{}", run, outerRepeats )

        //================== normal ==================
        // Lock
        {
            Lock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                lock.Acquire(ALIB_CALLER_PRUNED);
                lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT( "              Lock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // Lock Try
        {
            Lock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void)  lock.TryAcquire(ALIB_CALLER_PRUNED);
                        lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT( "              Lock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // TimedLock
        {
            TimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                lock.Acquire(ALIB_CALLER_PRUNED);
                lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "         TimedLock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // TimedLock Try
        {
            TimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "         TimedLock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // TimedLock Try(L)
        {
            TimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "         TimedLock.Try(L): {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // TimedLock 1ms
        {
            TimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "    TimedLock(1ms)       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // TimedLock +1ms
        {
            TimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Now() + Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "    TimedLock(+1ms)      : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        //================== Recursive ==================
        // RecursiveLock
        {
            RecursiveLock recursiveLock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);
                recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT( "     RecursiveLock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveLock Try
        {
            RecursiveLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "     RecursiveLock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveTimedLock
        {
            RecursiveTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                lock.AcquireRecursive(ALIB_CALLER_PRUNED);
                lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "RecursiveTimedLock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveTimedLock Try
        {
            RecursiveTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "RecursiveTimedLock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveTimedLock Try(L)
        {
            RecursiveTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            lock.AcquireRecursive(ALIB_CALLER_PRUNED);
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            auto duration= stopwatch.Age();
            UT_PRINT(  "RecursiveTimedLock.Try(L): {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveTimedLock 1ms
        {
            RecursiveTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "RecursiveTimedLock(1ms)  : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // RecursiveTimedLock +1ms
        {
            RecursiveTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Now() + Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.ReleaseRecursive(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "RecursiveTimedLock(+1ms) : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }


        //================== Shared ==================
        // SharedLock
        {
            SharedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                lock.Acquire(ALIB_CALLER_PRUNED);
                lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT( "        SharedLock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // SharedLock Try
        {
            SharedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void)  lock.TryAcquire(ALIB_CALLER_PRUNED);
                        lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT( "        SharedLock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // SharedTimedLock
        {
            SharedTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                lock.Acquire(ALIB_CALLER_PRUNED);
                lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "   SharedTimedLock       : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // SharedTimedLock Try
        {
            SharedTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "   SharedTimedLock.Try   : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // SharedTimedLock Try(L)
        {
            SharedTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquire(ALIB_CALLER_PRUNED);
                       lock.Release   (ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "   SharedTimedLock.Try(L): {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }

        // SharedTimedLock 1ms
        {
            SharedTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "   SharedTimedLock(1ms)  : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }
        
        // SharedTimedLock +1ms
        {
            SharedTimedLock lock;
            Ticks stopwatch= Ticks::Now();
            for ( int i= 0; i < repeats; ++i )
            {
                (void) lock.TryAcquireTimed(Ticks::Now() + Ticks::Duration::FromMicroseconds(1) ALIB_COMMA_CALLER_PRUNED);
                       lock.Release(ALIB_CALLER_PRUNED);
            }
            auto duration= stopwatch.Age();
            UT_PRINT(  "   SharedTimedLock(+1ms) : {} lock/unlock ops: {:>6,} -> {} per critical section",
                       repeats, duration, duration / repeats )
        }
    }
}
#endif // #if ALIB_TIME && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

//--------------------------------------------------------------------------------------------------
//--- ThreadSimple
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
UT_METHOD( ThreadSimple )
{
    UT_INIT()

    // create and delete
    {
        Thread t;
        UT_PRINT( "Thread object on stack, not started. State= ", t.GetState() )
    }
    {
        Thread* t= new Thread();
        UT_PRINT( "Thread object on heap, not started. State= ", t->GetState() )
        UT_PRINT( "Terminating unstarted thread. One warning should follow in debug compilations" )
        t->Join();
        delete t;
    }
    {
        Thread* t= new Thread();
        t->Start();
        UT_PRINT( "Empty Thread object, started. State= ", t->GetState() )
        t->Join();
        delete t;
    }

    // simple runnable
    #if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wshadow"
    #endif
    class runner: public Runnable
    {
        public:
        AWorxUnitTesting *pUT;
        int a= 0;
        runner(AWorxUnitTesting *put) { this->pUT= put; }
        virtual void Run()                                                                  override
        {
            AWorxUnitTesting& ut= *pUT;
            UT_PRINT( "Runnable running in thread ", Thread::GetCurrent()->GetID() )  Thread::SleepMillis(1); ++a;
        }
    };
    #if defined(__clang__)
        #pragma clang diagnostic pop
    #endif

    {
        runner r(&ut);
        {
            Thread t(&r);
            t.Start();
            t.Join();
        }

        UT_EQ( 1, r.a ) // thread is deleted, runner should have executed here
        {
            Thread t(&r);
            t.Start();
            UT_EQ( 1, r.a ) // runner waits a millisec, we should be faster
            int cntWait= 0;
            auto* currentThread= Thread::GetCurrent();
            while( t.IsAlive() )
            {
                #if defined(_WIN32)
                    Thread::SleepMillis(1);   // strange: SleepMicros does not seem to work under
                                              //          windows. At least not while waiting for
                                              //          a new thread...
                #else
                    Thread::SleepMicros(250);
                #endif
                ++cntWait;
            }
            UT_PRINT( "  Thread {!Q}({}) was waiting {} x 250 micros for thread {!Q}({}) to finish",
                         currentThread->GetName(),
                         currentThread->GetID  (),
                         cntWait,
                         t             .GetName(),
                         t             .GetID  ()   )

            UT_TRUE( cntWait < 10 )

            UT_PRINT( "  Result should be 2: ", r.a  )
            UT_EQ( 2, r.a )
            t.Join();
        }
    }
}
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)


//--------------------------------------------------------------------------------------------------
//--- ThreadLockThreaded
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
#if ALIB_DEBUG
UT_METHOD( ThreadLockWarning )
{
    UT_INIT()

    lang::Report::GetDefault().PushHaltFlags( false, false );

        RecursiveLock recursiveLock;
        Test_ThreadLock_SharedInt* shared= new Test_ThreadLock_SharedInt();
        UT_PRINT( "Lowering RecursiveLock::DbgWarningAfterWaitTime to only 1ms" )
        recursiveLock.Dbg.WaitTimeLimit= 1ms;
        recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);
        Test_ThreadLock_TestThread* t= new Test_ThreadLock_TestThread( &ut, A_CHAR("A Thread"), &recursiveLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We block for 5 milliseconds. This should give a warning. " )
        Thread::SleepMillis( 5 );
        recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);

        // wait until t ended
        while (t->IsAlive())
            Thread::SleepMillis( 1 );

        // now we do the same with a lower wait limit, no error should come
        UT_PRINT( "Raising RecursiveLock::Dbg.WaitTimeLimit to only 2 seconds" )
        recursiveLock.Dbg.WaitTimeLimit= 2s;
        recursiveLock.AcquireRecursive(ALIB_CALLER_PRUNED);
        t->Join();
        delete t;
        t= new Test_ThreadLock_TestThread( &ut, A_CHAR("A Thread"), &recursiveLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We block 5 milliseconds. This should NOT give a warning this time. " )
        Thread::SleepMillis( 5 );
        recursiveLock.ReleaseRecursive(ALIB_CALLER_PRUNED);

        // wait until t ended
        while (t->IsAlive())
            Thread::SleepMillis( 1 );
        t->Join();
        delete t;
        delete shared;

    lang::Report::GetDefault().PopHaltFlags();
}
#endif // ALIB_DEBUG
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)


//--------------------------------------------------------------------------------------------------
//--- HeavyLoad
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
UT_METHOD( HeavyLoad )
{
    UT_INIT()

    RecursiveLock recursiveLock;

    // uncomment this for unsafe mode
    //recursiveLock.SetUnsafe( true ); recursiveLock.UseAssertions= false;

    Test_ThreadLock_SharedInt   shared;
    int                         holdTime=    0;
    int                         repeats=    5000;
    bool                        verbose=    false;

    Test_ThreadLock_TestThread* t1= new Test_ThreadLock_TestThread( &ut, A_CHAR("A"), &recursiveLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t2= new Test_ThreadLock_TestThread( &ut, A_CHAR("B"), &recursiveLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t3= new Test_ThreadLock_TestThread( &ut, A_CHAR("C"), &recursiveLock, holdTime, repeats, verbose, &shared );
    UT_PRINT( "starting three threads" )
    t1->Start();
    t2->Start();
    t3->Start();

    // wait until all ended
    while ( t1->IsAlive() || t2->IsAlive() || t3->IsAlive() )
        Thread::SleepMillis( 1 );

    UT_PRINT( "All threads ended. Shared value=", shared.val )
    UT_EQ( 0, shared.val )
    t1->Join();
    t2->Join();
    t3->Join();
    delete t1;
    delete t2;
    delete t3;
}
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)



#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_THREADS

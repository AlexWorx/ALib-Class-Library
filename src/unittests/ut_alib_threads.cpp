// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_THREADS

#include "alib/alox.hpp"


#if !defined (HPP_ALIB_TIME_TIMEPOINTBASE)
    #include "alib/time/timepointbase.hpp"
#endif

#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_SMARTLOCK)
    #include "alib/threads/smartlock.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_THREAD)
#   include "alib/threads/thread.hpp"
#endif


using namespace aworx;

#include <iostream>

#define TESTCLASSNAME       CPP_ALib_Threads
#include "unittests/aworx_unittests.hpp"

using namespace std;

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- helper classes
//--------------------------------------------------------------------------------------------------

class Test_ThreadLock_SharedInt
{
    public:        int val= 0;
};

class Test_ThreadLock_TestThread : public Thread
{
    public:
        AWorxUnitTesting* UT;
        ThreadLock*       Mutex;
        int               HoldTime;
        int               Repeats;
        bool              Verbose;
        int               TResult= 1;
        Test_ThreadLock_SharedInt* Shared;

    Test_ThreadLock_TestThread( AWorxUnitTesting* pUT, const String& tname, ThreadLock* lock, int holdTime, int repeats, bool verbose, Test_ThreadLock_SharedInt* shared )
    :Thread( tname )
    {
        this->UT=        pUT;
        this->Mutex=     lock;
        this->HoldTime=  holdTime;
        this->Repeats=   repeats;
        this->Verbose=   verbose;
        this->Shared=    shared;
    }

    public: void Run()
    {
        AWorxUnitTesting &ut= *UT;
        UT_EQ( GetId(), Thread::GetCurrent()->GetId() )

        for ( int i= 0; i < Repeats ; ++i )
        {
            if (Verbose) { UT_PRINT( "Thread {!Q} acquiring lock...",GetName() ) }
            Mutex->Acquire(ALIB_CALLER_PRUNED);
            if (Verbose) { UT_PRINT( "Thread {!Q} has lock.", GetName() ) }

                int sVal= ++Shared->val;

                Thread::SleepMicros( HoldTime );

                Shared->val= sVal -1;

            if (Verbose) { UT_PRINT( "Thread {!Q} releasing lock.", GetName() ) }
            Mutex->Release();
            if (Verbose) { UT_PRINT( "Thread {!Q} released lock." , GetName() ) }
        }

        TResult= 0;
        UT_PRINT( "Thread {!Q} terminates.", GetName() )

    }
};

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
        UT_PRINT( "Thread object on stack, not started. Alive= ", t.IsAlive() )
    }
    {
        Thread* t= new Thread();
        UT_PRINT( "Thread object on heap, not started. Alive= ", t->IsAlive() )
        delete t;
    }
    {
        Thread* t= new Thread();
        t->Start();
        UT_PRINT( "Empty Thread object, started. Alive= ", t->IsAlive() )
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
        virtual void Run()
        {
            AWorxUnitTesting& ut= *pUT;
            UT_PRINT( "Runnable running in thread ", Thread::GetCurrent()->GetId() )  Thread::SleepMillis(1); ++a;
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
        }

        UT_EQ( 1, r.a ) // thread is deleted, runner should have executed here
        {

            Thread t(&r);
            t.Start();
            UT_EQ( 1, r.a ) // runner waits a millisec, we should be quicker
            int cntWait= 0;
            auto* currentThread= Thread::GetCurrent();
            while( t.IsAlive() )
            {

                UT_PRINT( "  Thread {!Q}({}) is waiting for thread {!Q}({}) to finish",
                           currentThread->GetName(),
                           currentThread->GetId  (),
                           t             .GetName(),
                           t             .GetId  ()   )
                Thread::SleepMicros(250);
                ++cntWait;
            }
            UT_TRUE( cntWait < 10 )
            UT_PRINT( "  Result should be 2: ", r.a  )
            UT_EQ( 2, r.a )
        }
    }
}
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

//--------------------------------------------------------------------------------------------------
//--- ThreadLockSimple
//--------------------------------------------------------------------------------------------------
UT_METHOD( ThreadLockSimple )
{
    UT_INIT()

    lib::results::Report::GetDefault().PushHaltFlags( false, false );

    // lock a recursive lock
    {
        ThreadLock aLock;
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() )
        aLock.Release();                      UT_EQ ( 0, aLock.CountAcquirements() )

        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 1, aLock.CountAcquirements() )
        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 2, aLock.CountAcquirements() )
        aLock.Release();                      UT_EQ ( 1, aLock.CountAcquirements() )

        aLock.Acquire(ALIB_CALLER_PRUNED);    UT_EQ ( 2, aLock.CountAcquirements() )
        aLock.Release();                            UT_EQ ( 1, aLock.CountAcquirements() )
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() )

        // set unsafe
        aLock.SetSafeness( Safeness::Unsafe );      UT_EQ ( 0, aLock.CountAcquirements() )
        aLock.SetSafeness( Safeness::Safe   );      UT_EQ ( 0, aLock.CountAcquirements() )

        aLock.SetSafeness( Safeness::Unsafe );      UT_EQ ( 0, aLock.CountAcquirements() )
        aLock.Acquire(ALIB_CALLER_PRUNED);          UT_EQ ( 1, aLock.CountAcquirements() )
        aLock.Release();                            UT_EQ ( 0, aLock.CountAcquirements() )

        // unsafe
        aLock.Acquire(ALIB_CALLER_PRUNED);          UT_EQ ( 1, aLock.CountAcquirements() )
        UT_PRINT( "Expecting error: set unsafe when already locked" )
        aLock.SetSafeness( Safeness::Safe   );      UT_EQ ( 1, aLock.CountAcquirements() )
        UT_PRINT( "Expecting error: destruction while locked" )
    }

    // safe (new lock)
    {
        ThreadLock aLock;
        aLock.Acquire(ALIB_CALLER_PRUNED);       UT_EQ ( 1, aLock.CountAcquirements() )
        UT_PRINT( "Expecting error: set unsafe when already locked" )
        aLock.SetSafeness( Safeness::Unsafe );   UT_EQ ( 1, aLock.CountAcquirements() )
        aLock.Release();                         UT_EQ ( 0, aLock.CountAcquirements() )
        UT_PRINT( "Expecting error: release without lock" )
        aLock.Release();                         UT_EQ ( static_cast<uint16_t>(-1), aLock.CountAcquirements() )
    }

    // test warnings (10) locks:
    {
        ThreadLock aLock;
        UT_PRINT( "Two warnings should come now: " )
        for (int i= 0; i<20; ++i)
            aLock.Acquire(ALIB_CALLER_PRUNED);
        UT_TRUE ( aLock.CountAcquirements() > 0 )
        for (int i= 0; i<20; ++i)
            aLock.Release();
        UT_EQ ( 0, aLock.CountAcquirements() )
    }

    lib::results::Report::GetDefault().PopHaltFlags();
}

//--------------------------------------------------------------------------------------------------
//--- ThreadLockThreaded
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
#if ALIB_DEBUG
UT_METHOD( ThreadLockWarning )
{
    UT_INIT()

    lib::results::Report::GetDefault().PushHaltFlags( false, false );

        ThreadLock aLock;
        Test_ThreadLock_SharedInt* shared= new Test_ThreadLock_SharedInt();
        UT_PRINT( "Lowering ThreadLock::DbgWarningAfterWaitTimeInMillis to only 1ms" )
        aLock.DbgWarningAfterWaitTimeInMillis= 1;
        aLock.Acquire(ALIB_CALLER_PRUNED);
        Test_ThreadLock_TestThread* t= new Test_ThreadLock_TestThread( &ut, A_CHAR("A Thread"), &aLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We block for 5 milliseconds. This should give a warning. " )
        Thread::SleepMillis( 5 );
        aLock.Release();

        // wait until t ended
        while (t->IsAlive())
            Thread::SleepMillis( 1 );

        // now we do the same with a lower wait limit, no error should come
        UT_PRINT( "Raising ThreadLock::DbgWarningAfterWaitTimeInMillis to only 2 seconds" )
        aLock.DbgWarningAfterWaitTimeInMillis= 2000;
        aLock.Acquire(ALIB_CALLER_PRUNED);
        delete t;
        t= new Test_ThreadLock_TestThread( &ut, A_CHAR("A Thread"), &aLock, 10, 1, true, shared );
        t->Start();
        UT_PRINT( "We block 5 milliseconds. This should NOT give a warning this time. " )
        Thread::SleepMillis( 5 );
        aLock.Release();

        // wait until t ended
        while (t->IsAlive())
            Thread::SleepMillis( 1 );
        delete t;
        delete shared;

    lib::results::Report::GetDefault().PopHaltFlags();
}
#endif // ALIB_DEBUG
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

//--------------------------------------------------------------------------------------------------
//--- SmartLockTest
//--------------------------------------------------------------------------------------------------
#if ALIB_ALOX // in release, no ALIB report is sent.
#if ALOX_DBG_LOG
UT_METHOD( SmartLockTest )
{
    UT_INIT()

    lib::results::Report::GetDefault().PushHaltFlags( false, false );
    ut.lox.GetLogCounter()= 0;

    // SmartLock with null-users
    {
        SmartLock sl;                          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
    }

    // SmartLock with threadlocks
    {
        ThreadLock tl1, tl2, tl3;
        SmartLock  sl;                         UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
    }

    // mixed
    {
        ThreadLock tl1, tl2, tl3;
        SmartLock  sl;                         UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.AddAcquirer   ( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl1    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Safe   )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "Three warnings should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl3    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( &tl2    );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 0 ) ut.lox.GetLogCounter()= 0;
        UT_PRINT( "One warning should follow" ) ut.lox.GetLogCounter()= 0;
        sl.RemoveAcquirer( nullptr );          UT_TRUE( sl.GetSafeness() == Safeness::Unsafe )  UT_TRUE( ut.lox.GetLogCounter()== 1 ) ut.lox.GetLogCounter()= 0;
    }
    lib::results::Report::GetDefault().PopHaltFlags();
}
#endif
#endif // ALIB_ALOX

//--------------------------------------------------------------------------------------------------
//--- HeavyLoad
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
UT_METHOD( HeavyLoad )
{
    UT_INIT()

    ThreadLock aLock;

    // uncomment this for unsafe mode
    //aLock.SetUnsafe( true ); aLock.UseAssertions= false;

    Test_ThreadLock_SharedInt   shared;
    int                         holdTime=    0;
    int                         repeats=    5000;
    bool                        verbose=    false;

    Test_ThreadLock_TestThread* t1= new Test_ThreadLock_TestThread( &ut, A_CHAR("A"), &aLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t2= new Test_ThreadLock_TestThread( &ut, A_CHAR("B"), &aLock, holdTime, repeats, verbose, &shared );
    Test_ThreadLock_TestThread* t3= new Test_ThreadLock_TestThread( &ut, A_CHAR("C"), &aLock, holdTime, repeats, verbose, &shared );
    UT_PRINT( "starting three threads" )
    t1->Start();
    t2->Start();
    t3->Start();

    // wait until all ended
    while ( t1->IsAlive() || t2->IsAlive() || t3->IsAlive() )
        Thread::SleepMillis( 1 );

    UT_PRINT( "All threads ended. Shared value=", shared.val )
    UT_EQ( 0, shared.val )
    delete t1;
    delete t2;
    delete t3;
}
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

//--------------------------------------------------------------------------------------------------
//--- SpeedTest
//--------------------------------------------------------------------------------------------------
#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
UT_METHOD( LockSpeedTest )
{
    UT_INIT()

    ThreadLock aLock;

    int        repeats=    10000;
    int        rrepeats=       3;

    Ticks stopwatch;
    for ( int run= 1; run <= rrepeats; ++run )
    {
        UT_PRINT( "Run {}/{}", run, rrepeats )

        aLock.SetSafeness( Safeness::Safe );
        stopwatch= Ticks::Now();
        for ( int i= 0; i < repeats; ++i )
        {
            aLock.Acquire(ALIB_CALLER_PRUNED);
            aLock.Release();
        }
        auto time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  Safe mode:    {} lock/unlock ops: {}\u00B5s", repeats, time ) // greek 'm' letter

        aLock.SetSafeness( Safeness::Unsafe );
        stopwatch= Ticks::Now();
        volatile int ii= 0;
        for ( int i= 0; i < repeats; ++i )
        {
            aLock.Acquire(ALIB_CALLER_PRUNED);
            aLock.Release();
        }
        time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  Unsafe mode:  {} lock/unlock ops: {}\u00B5s", repeats, time ) //greek 'm' letter;
        if (ii != 0 )
            UT_PRINT( "Never true! Just to stop compiler from pruning ii" ) //µs


        ThreadLockNR tNR;
        stopwatch= Ticks::Now();
        for ( int i= 0; i < repeats; ++i )
        {
            tNR.Acquire(ALIB_CALLER_PRUNED);
            tNR.Release();
        }
        time= stopwatch.Age().InAbsoluteMicroseconds();
        UT_PRINT( "  ThreadLockNR: {} lock/unlock ops: {}\u00B5s", repeats, time ) //greek 'm' letter;
    }
}
#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_THREADS

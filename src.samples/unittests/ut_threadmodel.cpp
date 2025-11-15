// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"

#if ALIB_UT_THREADMODEL

#include "ALib.ALox.H"
#include "ALib.Time.H"
#include "ALib.Strings.H"
#include "ALib.Threads.H"
#include "ALib.ThreadModel.H"
#include <iostream>

using namespace alib;
using namespace std;
using namespace std::literals::chrono_literals;

namespace {

ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION
//--------------------------------------------------------------------------------------------------
//--- Trigger sample
//--------------------------------------------------------------------------------------------------
#include "ALib.Lang.CIFunctions.H"
void   TriggerSample() {

Log_Info( "Trigger sample:" )
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Info, "/" )

DOX_MARKER( [DOX_THREADMODEL_TRIGGER_SIMPLE])
// My triggered type
struct MyTriggered : threadmodel::Triggered
{
    // Uses a fixed sleep time in this sample.
    // Note that in general, the method triggerPeriod() may return a different value
    // with every invocation.
    Ticks::Duration     sleepTime;

    // We count the calls, that' it
    int                 ctdTriggerCalls = 0;

    // Constructor. Parameter name needed for parent interface class Triggered
    MyTriggered(const String& name, Ticks::Duration::TDuration pSleepTime)
    : Triggered(name)
    , sleepTime(pSleepTime)   {}

    // Mandatory to overwrite. Has to return the next sleep duration.
    virtual Ticks::Duration triggerPeriod()   override { return sleepTime; }

    // The virtual method called to trigger.
    virtual void            trigger()         override {
        Log_Verbose( "I got triggered. I am: {} Sleep-period: ", this->Name, this->triggerPeriod() )
        ctdTriggerCalls++;
    }
};

// Create a trigger instance and attach two triggered 'clients'
// Note that it is allowed to attach triggered objects also while the trigger-thread is
// already running.
Trigger trigger;

MyTriggered t1( A_CHAR("MyTriggered 1"),  10us );
MyTriggered t2( A_CHAR("MyTriggered 2"),  30us );
trigger.Add( t1 );
trigger.Add( t2 );

// First, we start the trigger as an own thread. We wait a 10ms and then stop the thread
Log_Info( "Starting Trigger" )
trigger.Start();
Thread::Sleep(10ms);
trigger.Stop();

// We will see that t1 was roughly three times more often called than t2:
Log_Info( "Trigger calls t1: ", t1.ctdTriggerCalls )
Log_Info( "Trigger calls t2: ", t2.ctdTriggerCalls )

// Second, we run the trigger manually for 10 ms
Log_Info( "Running trigger 'manually'" )
trigger.Do(10ms);

// We will see that both triggered objects where called roughly double as often then before
Log_Info( "Trigger calls t1: ", t1.ctdTriggerCalls )
Log_Info( "Trigger calls t2: ", t2.ctdTriggerCalls )
DOX_MARKER( [DOX_THREADMODEL_TRIGGER_SIMPLE])
}

//--------------------------------------------------------------------------------------------------
//--- DedicatedWorker sample
//--------------------------------------------------------------------------------------------------

DOX_MARKER([DOX_THREADMODEL_MYJOB])
// Define a custom job type
struct MyJob : alib::JPromise  {

    int input = 0; // the input given with construction
    int result= 0; // the result calculated in Do()

    // Constructor.
    // Passes the typeid of this class to parent constructor, which in turn
    // passes it to base type Job.
    MyJob(int i) : JPromise(typeid(MyJob))
                 , input{i}
    {}

    // Mandatory to overwrite. Has to return this type's size.
    virtual size_t  SizeOf()  override { return sizeof(MyJob); }

    // Job logic goes here:
    bool Do() override  {

        // The work
        result= 2 * input;

        // Notify the sender
        Fulfill(ALIB_CALLER_PRUNED);

        // Pool jobs always have to return true
        return true;
    }
};
DOX_MARKER([DOX_THREADMODEL_MYJOB])


void   DedicatedWorkerSample() {

Log_Info( "DedicatedWorker sample:" )
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Info, "/" )
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Error, Lox::InternalDomains )

DOX_MARKER( [DOX_THREADMODEL_DEDICATED_WORKER_SIMPLE])
// Derive my own dedicated worker type
struct MyDedicatedWorker : threadmodel::DedicatedWorker {

    // Constructor. Passes a name to the parent type. The name is passed to grand-parent
    // class "alib::Thread".
    MyDedicatedWorker()  : DedicatedWorker(A_CHAR("My-DW"))  {}

    // Dedicated interface exposed to users of this type.
    MyJob& DoMySuperJob( int input ) {

        return Schedule<MyJob>( threadmodel::Priority::Standard,
                                input ); // <- constructor parameters of JobWait
    }

    // Same as previous method but does not provide sender with result value
    // (Caring for the result value might be a "burdon" in some cases)
    void  DoMySuperJobVoid( int input ) {

        ScheduleVoid<MyJob>( threadmodel::Priority::Standard,
                               input ); // <- constructor parameters of JobWait
    }

};

// Create the worker and start it by adding it to the manager singleton
MyDedicatedWorker dw;
DWManager::GetSingleton().Add(dw);

// Push a job by using that interface method that returns the job object
// on which we can wait.
Log_Info( "Pushing a job" )
MyJob&    req= dw.DoMySuperJob( 21 );

Log_Info( "Waiting for job execution" )
req.Wait(ALIB_CALLER_PRUNED);

// When wait returned, we can access the resul
Log_Info( "Job executed. Calculated result is: ", req.result )

// Now we have to dispose the job object. If not done, this is a memory leak.
dw.DeleteJob( req );

// Now we use the second interface method that does not return the job.
// Hence, we can't wait but we also are not burdend with deletion of the object
dw.DoMySuperJobVoid(123);

// Remove our dedicated worker from the manager. This waits for execution of all open job and
// terminates (joins) the thread.
DWManager::GetSingleton().Remove( dw );
Log_Info( "Max queue length (gives 1): ",  dw.DbgMaxQueuelength  )
Log_Info( "Jobs open (gives 0):        ",  dw.Load()  )
DOX_MARKER( [DOX_THREADMODEL_DEDICATED_WORKER_SIMPLE])
}


DOX_MARKER( [DOX_THREADMODEL_DEDICATED_WORKER_USING_PROCESS])
// Derive my own dedicated worker type
struct MyDedicatedWorkerV2 : threadmodel::DedicatedWorker {

    // Constructor. Passes a name to the parent type. The name is passed to grand-parent
    // class "alib::Thread".
    MyDedicatedWorkerV2()  : DedicatedWorker(A_CHAR("My-DW-V2"))  {}

    // Dedicated interface exposed to users of this type.
    MyJob& DoMySuperJob( int input ) {

        return Schedule<MyJob>( threadmodel::Priority::Standard,
                                input ); // <- constructor parameters of JobWait
    }


    // Override the process-method. If this returns true, then the method Do() of the job
    // is not executed.
    bool            process(Job& job) override {

        // check job type
        if ( job.Is<MyJob>() ) {

            // cast job using special templated method Cast()
            MyJob& myJob= job.Cast<MyJob>();

            // calculate the result (we tripple instead of double to be able to check which
            // method is in fact called)
            myJob.result= 3 * myJob.input;

            // set job processed
            myJob.Fulfill(ALIB_CALLER_PRUNED);
            return true;
        }

        // job not processed
        return false;
    }
};
DOX_MARKER( [DOX_THREADMODEL_DEDICATED_WORKER_USING_PROCESS])


//--------------------------------------------------------------------------------------------------
//--- ThreadPool sample
//--------------------------------------------------------------------------------------------------
void   ThreadPoolSample() {

DOX_MARKER( [DOX_THREADMODEL_POOL_SIMPLE])
// Create a thread pool
alib::ThreadPool pool;

// Schedule a job with input '21'
auto& myJob= pool.Schedule<MyJob>(21);

// wait for execution and print result
myJob.Wait(ALIB_CALLER_PRUNED);
Log_Info( "MyJob(21) result: {}", myJob.result )

// delete the job instance with the pool (otherwise, its a memory leak)
pool.DeleteJob(myJob);

// Schedule a job without caring for the job execution and its result.
// With this version of scheduling a job we can't see the result and don't know when it
// is executed.
// The benefit is that job-deletion is performed automatically and thus we do not need to wait
// for execution to do it ourselves.
pool.ScheduleVoid<MyJob>(123);

// Wait a max of 1 minute for all threads to finish (with debug, warn after 10 milliseconds)
pool.WaitForAllIdle( 1min  ALIB_DBG(, 10ms) );

// Shutdown the pool. Because this also waits until all jobs are processed and workers are
// idle, we wouldn't have needed the line above.
pool.Shutdown();
DOX_MARKER( [DOX_THREADMODEL_POOL_SIMPLE])
}

}// anonymous namespace
#include "ALib.Lang.CIMethods.H"
ALIB_WARNINGS_RESTORE // UNUSED_FUNCTION

#if !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)

#define TESTCLASSNAME       UT_ThreadModel
#include "aworx_unittests.hpp"

namespace ut_aworx {

UT_CLASS

UT_METHOD( ThreadmodelSamples )
{
    UT_INIT()

    TriggerSample();
    DedicatedWorkerSample();
    ThreadPoolSample();

    MyDedicatedWorkerV2 dw;
    DWManager::GetSingleton().Add(dw);

    UT_PRINT( "Pushing a job" )
    MyJob&    req= dw.DoMySuperJob( 21 );

    UT_PRINT( "Waiting for job execution" )
    req.Wait(ALIB_CALLER_PRUNED);
    UT_PRINT( "Job executed. Calculated result is: ", req.result )

    UT_EQ( 63, req.result )

    
    UT_PRINT( "Disposing job instance" ) // if not done, this is a memory leak.
    dw.DeleteJob( req );

    // Remove our dedicated worker from the manager. This terminates the thread.
    DWManager::GetSingleton().Remove( dw );
    ALIB_DBG(
    UT_PRINT( "Max queue length (gives 1): ",  dw.DbgMaxQueuelength  )  )
    UT_PRINT( "Jobs open (gives 0):        ",  dw.Load()  )

}

#endif // !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)



#include "aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_THREADMODEL

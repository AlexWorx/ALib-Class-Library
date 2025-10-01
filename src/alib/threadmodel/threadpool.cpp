// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/alib.inl"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ThreadModel;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
    import   ALib.Boxing;
#else
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.ThreadModel.H"
#endif
// ======================================   Implementation   =======================================
using namespace std::literals::chrono_literals;


namespace alib::threadmodel {

struct PWorker : protected Thread
{
    friend class ThreadPool;
    ThreadPool&  tp;
#if ALIB_STRINGS
    String16     nameBuffer;
    PWorker(ThreadPool& ptp, const character* threadName )
    : Thread(threadName)
    , tp(ptp)
    , nameBuffer(threadName)
    { SetName(nameBuffer); } // fix the name to local pointer
#else
    PWorker(ThreadPool& ptp )
    : Thread("Poolworker")
    , tp(ptp)
    {}
#endif

    void Run()                                                                              override
    {
        ALIB_MESSAGE("MGTHR", "PWorker \"{}\" is running", GetName() )
        for (;;)
        {
            // await next job. Break if null.
            auto  queueEntry= tp.pop(this);
            if ( queueEntry.job == nullptr )
                break;


            // Custom method, implemented with Job::Do()
            if ( queueEntry.job->Do() )
                goto CONTINUE;

            // not processed!
            ALIB_ERROR( "MGTHR",
                "Job of type <{}> passed to thread pool has no Job::Do() implementation!",
                &queueEntry.job->ID )

            CONTINUE:
            // delete job?
            if ( !queueEntry.keep )
            {ALIB_LOCK_WITH(tp)
                auto size=  queueEntry.job->SizeOf();
                queueEntry.job->~Job();
                tp.GetPoolAllocator().free(  queueEntry.job,  size );
            }
        }

        #if ALIB_DEBUG
            ALIB_MESSAGE( "MGTHR", "PWorker \"{}\" is stopping (leaving method Run())",
                                   GetName() )
        #endif
    }
}; // class PWorker


ThreadPool::ThreadPool()
: TCondition     (ALIB_DBG(A_CHAR("ThreadPool")))
, ma             (ALIB_DBG(       "ThreadPool",) 16)
, pool           (ma)
, workers        (ma)
#if ALIB_DEBUG
, DbgKnownJobs   (ma)
#endif
, queue          (pool)
{
    #if ALIB_DEBUG
        Dbg.Name= A_CHAR("DWManager");
    #endif
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        ma.DbgCriticalSectionsPH.Get()->DCSLock= this;
    #endif
}

ThreadPool::~ThreadPool()
{
    ALIB_ASSERT_ERROR( IsIdle(), "MGTHR",
        "ThreadPool destruction while not idle. Please call WaitForAllIdle().\n"
        "There are still {} workers running. Open jobs: ", ctdWorkers-ctdIdle,  ctdOpenJobs )

    ALIB_ASSERT_WARNING( ctdWorkers == 0, "MGTHR",
        "ThreadPool destructor: There are still {} threads running.\n"
        "While ThreadPool::Shutdown is called now, it is recommended to explicitly "
        "shutdown the pool before destruction.", ctdWorkers )

    if (ctdWorkers > 0)
        Shutdown();

    // check if there are still objects in the pool allocator
    #if ALIB_DEBUG_ALLOCATIONS
    NString2K warning;
    for (int i = 2; i < 32; ++i)
    {
        auto qtyObjects= pool.DbgCountedOpenAllocations(1L << i);
        if ( qtyObjects > 0)
            warning <<
                "ThreadPool destructor: There is(are) still " << qtyObjects << " object(s) of size "
                << (1L << i) << " in the PoolAllocator.\n";
    }
    if ( warning.IsNotEmpty() )
    {
        warning <<
            "  Hint:\n"
            "  This indicates that Job-objects have not been deleted during the run.\n"
            "  Alternatively, certain jobs used the pool allocator without freeing their data\n"
            "  This is a potential memory leak.\n"
            "  Known Job-types and their sizes are:\n";
        DbgDumpKnownJobs(warning, "    ");
        ALIB_WARNING( "MGTHR", warning )
        pool.DbgSuppressNonFreedObjectsWarning();
    }
    #endif
}

#if ALIB_DEBUG
#if ALIB_DEBUG_CRITICAL_SECTIONS
bool ThreadPool::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool ThreadPool::DCSIsSharedAcquired()    const { return Dbg.IsOwnedByCurrentThread();   }
#endif

#if ALIB_STRINGS
int ThreadPool::DbgDumpKnownJobs(NAString& target, const NString& linePrefix )
{
    int i= 0;
    for ( auto job : DbgKnownJobs )
    {
        target << linePrefix << NField( ++i, 2) << ": "
               << *job.TID << NTab(30,-1)
               << NField( job.JobSize, 3) << " (PA "
               << NField( PoolAllocator::GetAllocationSize(
                                      PoolAllocator::GetAllocInformation(job.JobSize)), 3) << ")  "
                  "Usage: " << NField(job.Usage, 5) << "\n";
    }
    return i;
}
#endif
#endif // ALIB_DEBUG

void ThreadPool::addThread()
{
    ALIB_MESSAGE( "MGTHR/STRGY", "Pool({}/{} -> {}/{}) adding one thread",
                                  ctdOpenJobs, ctdStatJobsScheduled, ctdIdle, ctdWorkers )

    // first check if the pool was already used once and is now restarted
    if ( lastThreadToJoin )
    {
        ALIB_ASSERT_ERROR( ctdWorkers == 0, "MGTHR",
        "ThreadPool::AddThread: Found a last thread to join but there the number of workers is {}\n"
        "instead of 0. This should never happen!", ctdWorkers )

        lastThreadToJoin->Join();
        delete lastThreadToJoin;
        lastThreadToJoin= nullptr;
    }

#if ALIB_STRINGS
    auto* newWorker= new PWorker( *this, String128("PWorker") << Dec(nextWorkerID++, 3, nullptr) );
#else
    auto* newWorker= new PWorker( *this );
#endif
    workers.InsertUnique( newWorker );
    ++ctdWorkers;
    newWorker->Start();
}


namespace {
    // An internal job used to task the next worker to join a thread that stopped.
    // Note: The last thread will add itself to #lastThreadToJoin, which will be joined
    //       with the method #Shutdown or when a new thread is added.
    struct JobJoin          : Job
    {
        PWorker*   workerToJoin;
        JobJoin()                  : Job( typeid(JobJoin) ), workerToJoin(nullptr)  {}
    };

    // we only need one instance
    JobJoin JOB_JOIN;

    // An internal job used by #Shutdown. It is only emplaced if the queue is empty and
    // all types are idle
    struct JobStop  : Job { JobStop() :Job(typeid(JobStop)) {} };

    // we only need one instance 
    JobStop JOB_STOP;
}

ThreadPool::QueueEntry  ThreadPool::pop(PWorker* caller)
{
    START:
    Acquire(ALIB_CALLER_PRUNED);
        ++ctdIdle;
            WaitForNotification(ALIB_CALLER_PRUNED);
        ALIB_ASSERT_ERROR(ctdOpenJobs != 0, "MGTHR", "Job pipe empty after wakeup" )

        // check for JobJoin singleton
        if ( queue.back().job == &JOB_JOIN )
        {
            queue.back().job->Cast<JobJoin>().workerToJoin->Join();
            queue.pop_back();
            --ctdOpenJobs;
//            ALIB_ASSERT(queue.IsNotEmpty())
        }
    
        // check for JobStop singleton
        else if ( queue.back().job == &JOB_STOP )
        {
            queue.pop_back();
            --ctdOpenJobs;
            ALIB_ASSERT(queue.empty(), "MGTHR")
        }

        // check if we need to change the pool size
        int targetSize= Strategy.GetSize( ctdWorkers,
                                          ctdIdle--,
                                          ctdOpenJobs,
                                          timeOfLastSizeChange    );


        // leaving pool?
        if ( targetSize < ctdWorkers )
        {
            ALIB_MESSAGE( "MGTHR/STRGY", "Pool({}/{} -> {}/{}) leaving pool ({}->{})" ,
                ctdOpenJobs, ctdStatJobsScheduled, ctdIdle, ctdWorkers,
                ctdWorkers, targetSize )

            // make sure the calling thread is joined. This is either done by the next
            // worker, or by the (main-) thread that calls Shutdown, or if a new thread
            // is added later
            if (ctdWorkers > 1)
            {
                // put 'myself' in the JobJoin singleton and add me to the front of the pipe.
                JOB_JOIN.workerToJoin= caller;
                queue.push_back({&JOB_JOIN, false});
                ctdOpenJobs++;
            }
            else
                lastThreadToJoin= caller;

            // remove myself from the worker list
            workers.erase(workers.Find(caller));
            --ctdWorkers;

            // in any case, mark the caller as done already to avoid a warning if joining comes
            // faster than the exit:
            caller->state= Thread::State::Done;

            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED);
            return QueueEntry{nullptr, false};
        }

        // increasing pool?
        else if ( targetSize > ctdWorkers )
        {
            addThread();
            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED);
            goto START;
        }


        // start working
        auto entry= queue.back();
        queue.pop_back();
        ALIB_MESSAGE( "MGTHR/QUEUE", "Pool({}/{} -> {}/{}) Job({}) popped",
            ctdOpenJobs, ctdStatJobsScheduled, ctdIdle, ctdWorkers, &entry.job->ID )

        --ctdOpenJobs;

        // Sync-job with optional deferred job deletion
        if ( entry.job->Is<JobSyncer>() )
        {
            auto& job=  entry.job->Cast<JobSyncer>();

            if ( job.JobToDelete )
            {
                size_t size= job.JobToDelete->SizeOf();
                job.JobToDelete->PrepareDeferredDeletion();
                job.JobToDelete->~Job();
                GetPoolAllocator().free( job.JobToDelete, size );
            }

            GetPoolAllocator().free( &job,  sizeof(JobSyncer) );
            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED); // wakeup others (all are idle)
            goto START;
        }
    
    Release(ALIB_CALLER_PRUNED);
    return entry;
}

bool ThreadPool::WaitForAllIdle( Ticks::Duration timeout
                      ALIB_DBG(, Ticks::Duration dbgWarnAfter) )
{
    ALIB_MESSAGE("MGTHR", "ThreadPool: Waiting for all jobs to be processed." )

    Ticks waitStart= Ticks::Now();
    ALIB_DBG( Ticks nextWarning= waitStart + dbgWarnAfter; )
    while(true)
    {
        if ( CountedOpenJobs() == 0 && CountedIdleWorkers() == CountedWorkers() )
        {
            ALIB_MESSAGE("MGTHR", "ThreadPool: All are idle.  Pool({}/{} -> {}/{})",
                     CountedOpenJobs()   , StatsCountedScheduledJobs(),
                     CountedIdleWorkers(), CountedWorkers()             )
            return true;
        }
        #if ALIB_DEBUG
            if( nextWarning.Age() > dbgWarnAfter)
            {
                ALIB_WARNING( "MGTHR", 
                     "ThreadPool: Waiting for all workers to be come idle. Pool({}/{} -> {}/{})",
                     CountedOpenJobs()   , StatsCountedScheduledJobs(),
                     CountedIdleWorkers(), CountedWorkers()             )
                nextWarning= Ticks::Now();
            }
        #endif

        // check timeout
        if (waitStart.Age() > timeout)
        {
            ALIB_WARNING("MGTHR", "ThreadPool: Timeout while waiting for idle" )
            return false;
        }

        // sleep
        Thread::SleepMicros( 50 );
    }
}

void ThreadPool::Shutdown()
{
    ALIB_MESSAGE("MGTHR", "ThreadPool::Shutdown: Pool({}/{} -> {}/{}) ",
            CountedOpenJobs()   , StatsCountedScheduledJobs(),
            CountedIdleWorkers(), CountedWorkers()             )


    ALIB_ASSERT_ERROR( ctdOpenJobs == 0, "MGTHR",
        "ThreadPool::Shutdown called while {} jobs are open. "
        "Call WaitForAllIdle() before shutdown.", ctdOpenJobs )

    // Schedule a stop-job.
    // We do this here to meet the wakeup condition without adding
    // another term to that condition.
    Acquire(ALIB_CALLER_PRUNED);
        Strategy.WorkersMax= 0;
        queue.push_back( {&JOB_STOP, false} );
        ++ctdOpenJobs;
    ReleaseAndNotify(ALIB_CALLER_PRUNED);


    // wait for all workers to exit
    ALIB_DBG(Ticks waitTime);
    while( ctdWorkers > 0 )
    {
        Thread::SleepMicros( 50 );
        ALIB_DBG( if (waitTime.Age().InAbsoluteSeconds() == 1) { waitTime.Reset();  )
            ALIB_MESSAGE("MGTHR",
                "ThreadPool::Shutdown. Waiting for workers to exit. Pool({}/{} -> {}/{})",
                CountedOpenJobs()   , StatsCountedScheduledJobs(),
                CountedIdleWorkers(), CountedWorkers()             )

        ALIB_DBG( } )

    }

    // join the last thread
    ALIB_ASSERT_ERROR(lastThreadToJoin != nullptr, "MGTHR",
        "lastThreadToJoin is null. This must not happen (internal error)." )

    lastThreadToJoin->Join();
    delete lastThreadToJoin;
    lastThreadToJoin= nullptr;

    // Success
    ALIB_MESSAGE("MGTHR",  "ThreadPool::Shutdown completed. Pool({}/{} -> {}/{})",
                           CountedOpenJobs()   , StatsCountedScheduledJobs(),
                           CountedIdleWorkers(), CountedWorkers()             )
}


} // namespace [alib::threadmodel]



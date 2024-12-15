// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/threadmodel/threadpool.hpp"
#include "alib/threads/thread.hpp"
#include "alib/strings/localstring.hpp"
#include "alib/alox.hpp"


using namespace std::literals::chrono_literals;


namespace alib::threadmodel {

struct PWorker : protected Thread
{
    friend class ThreadPool;
    ThreadPool&  tp;
    PWorker(ThreadPool& ptp, const String& threadName) : Thread(threadName ), tp(ptp) {}

    void Run()                                                                             override
    {
        ALIB_MESSAGE("MGTHR", NString256( "PWorker \"") << GetName() << "\" is running" )
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
            ALIB_ERROR( "MGTHR", NString512()
                << "Job of type <" << queueEntry.job->ID
                << "> passed to thread pool has no Job::Do() implementation!" )

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
            NString4K msg( "PWorker \""); msg << GetName() << "\" is stopping (leaving method Run()).";
            ALIB_MESSAGE( "MGTHR", msg )
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
    ALIB_ASSERT_ERROR( IsIdle(), "MGTHR", NString256() <<
        "ThreadPool destruction while not idle. Please call WaitForAllIdle().\n"
        "There are still " << (ctdWorkers-ctdIdle)<< " workers running. "
        "Open jobs: " << ctdOpenJobs )

    ALIB_ASSERT_WARNING( ctdWorkers == 0, "MGTHR", NString256() <<
        "ThreadPool destructor: There are still " << ctdWorkers<< " threads running.\n"
        "While ThreadPool::Shudown is called now, it is recommended to explicitly "
        "shutdown the pool before destruction." )

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
            "  This indicates that Job-objects have not been deleted during run.\n"
            "  Alternatively, certain jobs used the pool allocator without freeing the their data\n"
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

int ThreadPool::DbgDumpKnownJobs(NAString& target, const NString& linePrefix )
{
    int i= 0;
    for ( auto job : DbgKnownJobs )
    {
        target << linePrefix << NFormat::Field( ++i, 2) << ": "
               << *job.TID << NFormat::Tab(30,-1)
               << NFormat::Field( job.JobSize, 3) << " (PA "
               << NFormat::Field( PoolAllocator::GetAllocationSize(
                                      PoolAllocator::GetAllocInformation(job.JobSize)), 3) << ")  "
                  "Usage: " << NFormat::Field(job.Usage, 5) << "\n";
    }
    return i;
}
#endif // ALIB_DEBUG

void ThreadPool::addThread()
{
    ALIB_MESSAGE( "MGTHR/STRGY", NString2K() <<
        "Pool(" << ctdOpenJobs << "/" << ctdStatJobsScheduled <<
        " -> " << ctdIdle << "/" << ctdWorkers << ") "
        " adding one thread"  )

    // first check if the pool was already used once and is now restarted
    if ( lastThreadToJoin )
    {
        ALIB_ASSERT_ERROR( ctdWorkers == 0, "MGTHR", NString2K() <<
            "ThreadPool::AddThread: Found a last thread to join but there the number of workers is "
            << ctdWorkers << "\ninstead of 0. This should never happen" )

        lastThreadToJoin->Join();
        delete lastThreadToJoin;
        lastThreadToJoin= nullptr;
    }

    auto* newWorker= new PWorker( *this, String128("PWorker") << Format(nextWorkerID++, 3, nullptr) );
    workers.InsertUnique( newWorker );
    ++ctdWorkers;
    newWorker->Start();
}


namespace {
    // An internal job used to task the next worker to join a thread that stopped.
    // Note: The last thread will add itself to #lastThreadToJoin, which will be joined
    //       with the method #Shudown or when a new thread is added.
    struct JobJoin          : Job
    {
        PWorker*   workerToJoin;
        JobJoin()                  : Job( typeid(JobJoin) ), workerToJoin(nullptr)  {}
    };

    // we only need one instance
    JobJoin JOB_JOIN;

    // An internal job used by #Shudown. It is only emplaced if the queue is empty and
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
        if ( queue.Back().job == &JOB_JOIN )
        {
            queue.Back().job->Cast<JobJoin>().workerToJoin->Join();
            queue.PopBack();
            --ctdOpenJobs;
//            ALIB_ASSERT(queue.IsNotEmpty())
        }
    
        // check for JobStop singleton
        else if ( queue.Back().job == &JOB_STOP )
        {
            queue.PopBack();
            --ctdOpenJobs;
            ALIB_ASSERT(queue.IsEmpty())
        }

        // check if we need to change the pool size
        int targetSize= Strategy.GetSize( ctdWorkers,
                                          ctdIdle--,
                                          ctdOpenJobs,
                                          timeOfLastSizeChange    );


        // leaving pool?
        if ( targetSize < ctdWorkers )
        {
            ALIB_MESSAGE( "MGTHR/STRGY", NString2K() <<
                "Pool(" << ctdOpenJobs << "/" << ctdStatJobsScheduled <<
                " -> "  << ctdIdle      << "/" << ctdWorkers << ") "
                " leaving pool  (" << ctdWorkers << "->" << targetSize << ")" )


            // make sure the calling thread is joined. This is either done by the next
            // worker, or by the (main-) thread that calls Shutdown, or if a new thread
            // is added later
            if (ctdWorkers > 1)
            {
                // put 'myself' in the JobJoin singleton and add me to the front of the pipe.
                JOB_JOIN.workerToJoin= caller;
                queue.PushBack({&JOB_JOIN, false});
                ctdOpenJobs++;
            }
            else
                lastThreadToJoin= caller;

            // remove myself from the worker list
            workers.Erase(workers.Find(caller));
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
        auto entry= queue.Back();
        queue.PopBack();
        ALIB_MESSAGE( "MGTHR/QUEUE", NString2K() <<
            "Pool(" << ctdOpenJobs << "/" << ctdStatJobsScheduled <<
            " -> " << ctdIdle << "/" << ctdWorkers << ") "
            "Job(" << entry.job->ID << ") popped" )

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
            ALIB_MESSAGE("MGTHR", NString256() <<
                "ThreadPool: All are idle.  Pool("
                     << CountedOpenJobs() << "/" << StatsCountedScheduledJobs() << "->"
                     << CountedIdleWorkers()  << "/"
                     << CountedWorkers()      << ")"  )
            return true;
        }
        #if ALIB_DEBUG
            if( nextWarning.Age() > dbgWarnAfter)
            {
                ALIB_WARNING( "MGTHR", NString256() <<
                     "Waiting for all workers to be come idle. Pool("
                     << CountedOpenJobs() << "/" << StatsCountedScheduledJobs() << "->"
                     << CountedIdleWorkers()  << "/"
                     << CountedWorkers()      << ")"  )
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
    ALIB_MESSAGE("MGTHR", NString256() <<
        "ThreadPool::Shutdown:  Pool("
             << CountedOpenJobs() << "/" << StatsCountedScheduledJobs() << "->"
             << CountedIdleWorkers()  << "/"
             << CountedWorkers()      << ")"  )

    ALIB_ASSERT_ERROR( ctdOpenJobs == 0, "MGTHR", NString256() <<
        "ThreadPool::Shutdown called while " << ctdOpenJobs << " jobs are open. "
        " Call WaitForAllIdle() before shutdown." )

    // Schedule a stop-job.
    // We do this here to meet the wakeup condition without adding
    // another term to that condition.
    Acquire(ALIB_CALLER_PRUNED);
        Strategy.WorkersMax= 0;
        queue.PushBack( {&JOB_STOP, false} );
        ++ctdOpenJobs;
    ReleaseAndNotify(ALIB_CALLER_PRUNED);


    // wait for all workers to exit
    ALIB_DBG(Ticks waitTime);
    while( ctdWorkers > 0 )
    {
        Thread::SleepMicros( 50 );
        ALIB_DBG( if (waitTime.Age().InAbsoluteSeconds() == 1) { waitTime.Reset();  )
            ALIB_MESSAGE("MGTHR", NString256() <<
                "ThreadPool::Shutdown. Waiting for workers to exit.  Pool("
                     << CountedOpenJobs() << "/" << StatsCountedScheduledJobs() << "->"
                     << CountedIdleWorkers()  << "/"
                     << CountedWorkers()      << ")"  )
        ALIB_DBG( } )

    }

    // join the last thread
    ALIB_ASSERT_ERROR(lastThreadToJoin != nullptr, "MGTHR",
        "ThreadPool::Shutdown: lastThreadToJoin is null. This must not happen (internal error)." )

    lastThreadToJoin->Join();
    delete lastThreadToJoin;
    lastThreadToJoin= nullptr;

    // Success
    ALIB_MESSAGE("MGTHR", NString256() <<
        "ThreadPool::Shutdown completed.  Pool("
             << CountedOpenJobs() << "/" << StatsCountedScheduledJobs() << "->"
             << CountedIdleWorkers()  << "/"
             << CountedWorkers()      << ")"  )
}


} // namespace [alib::threadmodel]



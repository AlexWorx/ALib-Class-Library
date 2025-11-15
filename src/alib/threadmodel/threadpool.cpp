//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/alib.inl"
//============================================== Module ============================================
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
//========================================== Implementation ========================================
using namespace std::literals::chrono_literals;

#if ALIB_STRINGS
void alib::strings::AppendableTraits<alib::threadmodel::ThreadPool,
                                     alib::character,
                                     alib::lang::HeapAllocator>::operator()(
         alib::strings::TAString<character, alib::HeapAllocator>&   target,
         const alib::threadmodel::ThreadPool&                       tp       ) {
    target << "TPool(Jobs: " <<  tp.CountedOpenJobs() << '/' << tp.StatsCountedScheduledJobs()
           << " Wrks: "      <<  tp.CountedWorkers() - tp.CountedIdleWorkers()
           << "/["           <<  tp.Strategy.WorkersMin
           << " <= "         <<  tp.CountedWorkers()
           << " <= "         <<  tp.Strategy.WorkersMax
           << "]";
}
#endif


namespace alib::threadmodel {

void PoolWorker::Run()  {
    ALIB_MESSAGE("TMOD/WORKER", "PoolWorker \"{}\" is running", GetName() )
    for (;;) {
        // await next job. Break if null.
        auto  queueEntry= threadPool.pop(this);
        if ( queueEntry.job == nullptr )
            break;

        // Prepare job (a noop with default pool worker) and call Do()
        PrepareJob(queueEntry.job);
        if ( queueEntry.job->Do() )
            goto CONTINUE;

        // not processed!
        ALIB_ERROR( "TMOD",
            "Job of type <{}> passed to thread pool has no Job::Do() implementation!",
            &queueEntry.job->ID )

      CONTINUE:
        // delete job?
        if ( !queueEntry.keep )
        {ALIB_LOCK_WITH(threadPool)
            auto size=  queueEntry.job->SizeOf();
            queueEntry.job->~Job();
            threadPool.GetPoolAllocator().free(  queueEntry.job,  size );
    }   }

    #if ALIB_DEBUG
        ALIB_MESSAGE( "TMOD/WORKER", "PoolWorker \"{}\" is stopping (leaving method Run())",
                               GetName() )
    #endif
}


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


    #if ALIB_DEBUG
        assert::RegisterPrintable(typeid(ThreadPool*), [](const std::any& any, std::string& s) {
        auto& tp= *std::any_cast<ThreadPool*>(any);
        #if ALIB_STRINGS
            String256 serialize(tp);
            ALIB_STRINGS_TO_NARROW(serialize, ns, 256)
            s+= ns;
        #else
            std::ostringstream oss;
            oss << "TPool(Jobs: " <<  tp.CountedOpenJobs() << '/' << tp.StatsCountedScheduledJobs()
                << " Wrks: "      <<  tp.CountedWorkers() - tp.CountedIdleWorkers()
                << "/["           <<  tp.Strategy.WorkersMin
                << " <= "         <<  tp.CountedWorkers()
                << " <= "         <<  tp.Strategy.WorkersMax
                << "]";
            s+= oss.str();
        #endif
        });
    #endif

}

ThreadPool::~ThreadPool() {
    ALIB_ASSERT_ERROR( IsIdle(), "TMOD",
        "{}: Destruction while not idle. Please call WaitForAllIdle().\n"
        "There are still {} workers running. Open jobs: ", this, ctdWorkers - ctdIdle,  ctdOpenJobs )

    ALIB_ASSERT_WARNING( ctdWorkers == 0, "TMOD",
        "{}: There are still {} threads running (whil in destructor).\n"
        "While Shutdown is called now, it is recommended to explicitly "
        "shutdown the pool before destruction.", this, ctdWorkers )

    if (ctdWorkers > 0)
        Shutdown();

    // check if there are still objects in the pool allocator
    #if ALIB_DEBUG_ALLOCATIONS
    NString2K warning;
    for (int i = 2; i < 32; ++i) {
        auto qtyObjects= pool.DbgCountedOpenAllocations(1L << i);
        if ( qtyObjects > 0)
            warning <<
                "ThreadPool destructor: There is(are) still " << qtyObjects << " object(s) of size "
                << (1L << i) << " in the PoolAllocator.\n";
    }
    if ( warning.IsNotEmpty() ) {
        warning <<
            "  Hint:\n"
            "  This indicates that Job-objects have not been deleted during the run.\n"
            "  Alternatively, certain jobs used the pool allocator without freeing their data\n"
            "  This is a potential memory leak.\n"
            "  Known Job-types and their sizes are:\n";
        DbgDumpKnownJobs(warning, "    ");
        ALIB_WARNING( "TMOD", warning )
        pool.DbgSuppressNonFreedObjectsWarning();
    }
    #endif
}

#if ALIB_DEBUG
#if ALIB_DEBUG_CRITICAL_SECTIONS
bool ThreadPool::DCSIsAcquired()                      const { return Dbg.IsOwnedByCurrentThread(); }
bool ThreadPool::DCSIsSharedAcquired()                const { return Dbg.IsOwnedByCurrentThread(); }
#endif

#if ALIB_STRINGS
int ThreadPool::DbgDumpKnownJobs(NAString& target, const NString& linePrefix ) {
    int i= 0;
    for ( auto job : DbgKnownJobs ) {
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

PoolWorker* ThreadPool::CreateWorker() {
    #if ALIB_STRINGS
        return pool().New<PoolWorker>( *this,
                                       String128("PoolWorker") << Dec(nextWorkerID++, 3, nullptr) );
    #else
        return pool().New<PoolWorker>( *this );
    #endif
}
void     ThreadPool::DisposeWorker( PoolWorker* poolWorker )          { pool().Delete(poolWorker); }

void     ThreadPool::addThread()    {
    ALIB_MESSAGE( "TMOD/STRGY", "{}: Adding one thread", this )

    // first check if the pool was already used once and is now restarted
    if ( lastThreadToJoin ) {
        ALIB_ASSERT_ERROR( ctdWorkers == 0, "TMOD",
        "{}: Found a last thread to join but there the number of workers is {}\n"
        "instead of 0. This should never happen!", this, ctdWorkers )

        lastThreadToJoin->Join();
        delete lastThreadToJoin;
        lastThreadToJoin= nullptr;
    }

    auto* newWorker= CreateWorker();

    workers.InsertUnique( newWorker );
    ++ctdWorkers;
    newWorker->Start();
}


namespace {

// An internal job used to task the next worker to join a thread that stopped.
// Note: The last thread will add itself to #lastThreadToJoin, which will be joined
//       with the method #Shutdown or when a new thread is added.
struct JobJoin  : Job
{
    PoolWorker*   workerToJoin;
    JobJoin() :
        Job( typeid(JobJoin) )
      , workerToJoin(nullptr)                                                                     {}
};

// we only need one instance
JobJoin JOB_JOIN;

// An internal job used by #Shutdown. It is only emplaced if the queue is empty and
// all types are idle
struct JobStop  : Job {
    JobStop() :Job(typeid(JobStop))                                                               {}
};

// we only need one instance
JobStop JOB_STOP;

}

ThreadPool::QueueEntry  ThreadPool::pop(PoolWorker* caller) {
    START:
    Acquire(ALIB_CALLER_PRUNED);
        ++ctdIdle;
            WaitForNotification(ALIB_CALLER_PRUNED);
        --ctdIdle;
        ALIB_ASSERT_ERROR(ctdOpenJobs != 0, "TMOD", "{}: Job pipe empty after wakeup.", this )

        // check for JobJoin singleton
        if ( queue.back().job == &JOB_JOIN ) {
            auto& job= queue.back().job->Cast<JobJoin>();
            job.workerToJoin->Join();

            DisposeWorker(job.workerToJoin);
            
            queue.pop_back();
            --ctdOpenJobs;
        }
    
        // check for JobStop singleton
        else if ( queue.back().job == &JOB_STOP ) {
            queue.pop_back();
            --ctdOpenJobs;
            ALIB_ASSERT(queue.empty(), "TMOD")
        }

        // check if we need to change the pool size
        int targetSize= Strategy.GetSize( ctdWorkers, ctdIdle, ctdOpenJobs, timeOfLastSizeChange );


        // leaving pool?
        if ( targetSize < ctdWorkers ) {
            ALIB_MESSAGE( "TMOD/STRGY", "{}: Leaving pool ({}->{})",
                                        this, ctdWorkers, targetSize )

            // make sure the calling thread is joined. This is either done by the next
            // worker, or by the (main-) thread that calls Shutdown, or if a new thread
            // is added later
            if (ctdWorkers > 1) {
                // put 'myself' in the JobJoin singleton and add me to the front of the pipe.
                JOB_JOIN.workerToJoin= caller;
                queue.push_back({&JOB_JOIN, false});
                ctdOpenJobs++;
            }
            else
                lastThreadToJoin= caller;

            // remove the caller from the worker list
            workers.erase(workers.Find(caller));
            --ctdWorkers;

            // in any case, mark the caller as done already to avoid a warning if joining comes
            // faster than the exit:
            caller->state= Thread::State::Done;

            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED);
            return QueueEntry{nullptr, false};
        }

        // increasing pool?
        if ( targetSize > ctdWorkers ) {
            addThread();
            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED);
            goto START;
        }


        // start working
        auto entry= queue.back();
        queue.pop_back();
        ALIB_MESSAGE( "TMOD/QUEUE", "{}: Job({}) popped", this, &entry.job->ID )

        --ctdOpenJobs;

        // Sync-job with optional deferred job deletion
        if ( entry.job->Is<JobSyncer>() ) {
            auto& job=  entry.job->Cast<JobSyncer>();

            if ( job.JobToDelete ) {
                size_t size= job.JobToDelete->SizeOf();
                job.JobToDelete->PrepareDeferredDeletion();
                job.JobToDelete->~Job();
                GetPoolAllocator().free( job.JobToDelete, size );
            }

            GetPoolAllocator().free( &job,  sizeof(JobSyncer) );
            ALIB_ASSERT(ctdIdle + 1 == ctdWorkers, "TMOD")
            ReleaseAndNotifyAll(ALIB_CALLER_PRUNED); // wakeup others (all are idle)
            goto START;
        }
    
    Release(ALIB_CALLER_PRUNED);
    return entry;
}

bool ThreadPool::WaitForAllIdle( Ticks::Duration timeout
                      ALIB_DBG(, Ticks::Duration dbgWarnAfter) ) {
    ALIB_MESSAGE("TMOD", "{}: Waiting for all jobs to be processed.", this )

    Ticks waitStart= Ticks::Now();
    ALIB_DBG( Ticks nextWarning= waitStart + dbgWarnAfter; )
    while(true) {
        if ( CountedOpenJobs() == 0 && CountedIdleWorkers() == CountedWorkers() ) {
            ALIB_MESSAGE("TMOD", "{}: All workers are idle.", this )
            return true;
        }
        #if ALIB_DEBUG
            if( nextWarning.Age() > dbgWarnAfter) {
                ALIB_MESSAGE( "TMOD",
                     "{}: Waiting for all workers to be come idle.", this )
                if ( CountedOpenJobs() ) {
                    for (auto& job : queue )
                        ALIB_MESSAGE( "TMOD", "  Job: {}", &job.job->ID)
                }
                nextWarning= Ticks::Now();
            }
        #endif

        // check timeout
        if (waitStart.Age() > timeout) {
            ALIB_WARNING("TMOD", "{} timeout while waiting for idle", this )
            return false;
        }

        // sleep
        Thread::Sleep( IdleWaitTime );
}   }

void ThreadPool::Shutdown() {
    ALIB_MESSAGE("TMOD", "{}: Shutdown", this)

    ALIB_ASSERT_ERROR( ctdOpenJobs == 0, "TMOD",
        "{}: Shutdown called while {} jobs are open. "
        "Call WaitForAllIdle() before shutdown.", this, ctdOpenJobs )

    if (CountedWorkers() == 0) {
        ALIB_MESSAGE("TMOD",  "{}: Shutdown, no workers alive.", this)
        return;
    }
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
    while( CountedWorkers() > 0 ) {
        Thread::SleepMicros( 50 );
        ALIB_DBG( if (waitTime.Age().InAbsoluteSeconds() == 1) { waitTime.Reset();  )
            ALIB_MESSAGE("TMOD", "{}: Waiting for workers to exit.", this)

        ALIB_DBG( } )

    }

    // join the last thread
    ALIB_ASSERT_ERROR(lastThreadToJoin != nullptr, "TMOD",
        "{}: The 'lastThreadToJoin' is null. This must not happen (internal error)."
        , this )

    lastThreadToJoin->Join();
    DisposeWorker( lastThreadToJoin );
    lastThreadToJoin= nullptr;

    // Success
    ALIB_MESSAGE("TMOD",  "{}: Shutdown completed.) ", this)
}


} // namespace [alib::threadmodel]

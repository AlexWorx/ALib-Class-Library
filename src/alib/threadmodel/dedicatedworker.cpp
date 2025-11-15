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
#if ALIB_DEBUG
#   include <vector>
#   include <any>
#endif
#include <algorithm>
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.ThreadModel;
#else
#   include "ALib.ThreadModel.H"
#endif
//========================================== Implementation ========================================

namespace alib::threadmodel {

//##################################################################################################
// DWManager
//##################################################################################################

DWManager::DWManager()
: ma             (ALIB_DBG("DWManager",) 16)
, pool           (ma)
, workers     (ma) {
    #if ALIB_DEBUG
        Lock::Dbg.Name= "DWManager";
    #endif
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        ma.DbgCriticalSectionsPH.Get()->DCSLock= this;
    #endif
}


void DWManager::Add(DedicatedWorker& thread )                                             {ALIB_LOCK
    #if ALIB_DEBUG
        for( DedicatedWorker* it : workers )
            ALIB_ASSERT_ERROR( it != &thread, "TMOD", "Thread already added" )
    #endif

    workers.push_back( &thread );
    thread.Start();
}

bool  DWManager::Remove( DedicatedWorker& thread, Priority stopPriority ) {
    {ALIB_LOCK
        auto it= std::find( workers.begin(), workers.end(), &thread );
        if( it == workers.end() ) {
            ALIB_WARNING( "TMOD", "Thread \"{}\" to remove not found", thread.GetName())
            return false;
        }
        workers.erase( it );
    }

    if( !thread.StopIsScheduled() )
        thread.ScheduleStop( stopPriority );

    #if ALIB_DEBUG
    Ticks waitCheck= Ticks::Now();
    int nextWarnSecond= 1;
    #endif
    while(thread.GetState() < Thread::State::Done) {
        Thread::SleepMicros( 10 );
        #if ALIB_DEBUG
        if( waitCheck.Age().InAbsoluteSeconds() == nextWarnSecond ) {
            ALIB_WARNING( "TMOD",
                "DWManager::Remove: Waiting on thread \"{}\" to stop. State::{}, Load: ",
                thread.GetName(), thread.GetState(), thread.Load()  )
            nextWarnSecond++;
        }
        #endif
    }

    if ( thread.GetState() != Thread::State::Terminated )
        thread.Join();

    return true;
}

bool DWManager::WaitForAllIdle( Ticks::Duration timeout
                         ALIB_DBG(, Ticks::Duration dbgWarnAfter) ) {
    ALIB_MESSAGE( "TMOD", "DWManager::StopAndJoinAll" )

    Ticks waitStart= Ticks::Now();
    ALIB_DBG( Ticks nextWarning= waitStart + dbgWarnAfter; )
    while(true) {
        // check that all threads are stopped
        int cntRunning= 0;
        {ALIB_LOCK
            for( DedicatedWorker* it : workers )
                if( it->Load() > 0  )
                    cntRunning++;
        }
        if( cntRunning == 0 )
            return true;

        #if ALIB_DEBUG
            if( nextWarning.Age() > dbgWarnAfter) {
                std::vector<std::any> args; args.reserve(32);
                args.emplace_back( "Waiting on {} thread(s) to become idle.\n");
                args.emplace_back( cntRunning );
                int tNr= 0;
                {ALIB_LOCK
                    for( DedicatedWorker* it : workers  )
                        if ( it->Load() > 0  ) {
                            args.emplace_back( ++tNr );
                            args.emplace_back( ": {},\tState::{},\t Load: \n" );
                            args.emplace_back( it->GetName() );
                            args.emplace_back( it->state );
                            args.emplace_back( it->Load() );
                }   }
                assert::raise( ALIB_CALLER_PRUNED, 1, "TMOD", args );
                nextWarning= Ticks::Now();
            }
        #endif

        // check timeout
        if (waitStart.Age() > timeout)
            return false;

        // sleep
        Thread::SleepMicros( 50 );
}   }

void DWManager::RemoveAll( Priority stopPriority ) {
    ALIB_MESSAGE( "TMOD", "DWManager::StopAndJoinAll" )

    ALIB_DBG( Ticks waitCheck= Ticks::Now();
              int nextWarnSecond= 1; )

    // send stop to those unstopped
    for( DedicatedWorker* it : workers )
        if( !it->StopIsScheduled() )
            it->ScheduleStop( stopPriority );

    ALIB_DBG( waitCheck= Ticks::Now(); nextWarnSecond= 1; )
    int cntRunning;
    while(true) {
        // check that all threads are stopped
        cntRunning= 0;
        for( DedicatedWorker* it : workers )
            if( it->GetState() < Thread::State::Done  )
                cntRunning++;
        if( cntRunning == 0 )
            break;

        Thread::SleepMicros( 10 );
        #if ALIB_DEBUG
            if( waitCheck.Age().InAbsoluteSeconds() == nextWarnSecond ) {
                std::vector<std::any> args; args.reserve(32);
                args.emplace_back( "DWManager Termination: Waiting on {} thread(s) to stop.\n");
                args.emplace_back( cntRunning );
                int tNr= 0;
                for( DedicatedWorker* it : workers  ) {
                    args.emplace_back( ++tNr );
                    args.emplace_back( ": {},\tState::{},\t Load: \n" );
                    args.emplace_back( it->GetName() );
                    args.emplace_back( it->state );
                    args.emplace_back( it->Load() );
                }
                assert::raise( ALIB_CALLER_PRUNED, 1, "TMOD", args );
                nextWarnSecond++;
            }
        #endif
    }

    // terminate all registered MThreads and remove them from our the list.
    for( DedicatedWorker* it : workers )
        it->Join();
    workers.Clear();
}

//##################################################################################################
// DedicatedWorker
//##################################################################################################
void DedicatedWorker::pushAndRelease(QueueElement&& jobInfo) {
    Acquire(ALIB_CALLER_PRUNED);

    // search the first element with equal or higher priority
    auto  it=   queue.begin();
    while(it != queue.end()) {
        if(jobInfo.priority <= it->priority )
            break;
        it++;
    }

    // insert before found
    queue.Insert(it, jobInfo );

    #if ALIB_DEBUG
        std::vector<std::any> args; args.reserve(32);
        args.emplace_back( "Queue({}) Job({}) pushed. P::{} Keep: ");
        args.emplace_back( queue.size() );
        args.emplace_back(  &jobInfo.job->ID  );
        args.emplace_back(  jobInfo.priority );
        args.emplace_back(  jobInfo.keepJob  );
        assert::raise( ALIB_CALLER_PRUNED, 2, "TMOD/QUEUE", args );
    #endif

    ++length;
    ALIB_DBG( DbgMaxQueuelength= (std::max)(DbgMaxQueuelength, length); )

    ReleaseAndNotify(ALIB_CALLER_PRUNED);
}

std::pair<Job*, bool>  DedicatedWorker::pop()                                             {ALIB_LOCK
    WaitForNotification(ALIB_CALLER_PRUNED);
    ALIB_ASSERT_ERROR(length != 0, "TMOD", "Job pipe empty after wakeup" )
    ALIB_MESSAGE( "TMOD/QUEUE", "Queue--, size: ", length )

    std::pair<Job*, bool> result= { queue.back().job, queue.back().keepJob };
    ALIB_DBG( auto dbgPriority= queue.back().priority; )

    queue.pop_back();
    --length;

    #if ALIB_DEBUG
        std::vector<std::any> args; args.reserve(32);
        args.emplace_back( "Queue({}) Job({}) pushed. P::{} Keep: ");
        args.emplace_back( length            );
        args.emplace_back( &result.first->ID );
        args.emplace_back( dbgPriority       );
        args.emplace_back( result.second     );
        assert::raise( ALIB_CALLER_PRUNED, 2, "TMOD/QUEUE", args );
    #endif

    return result;
}

void DedicatedWorker::Run() {
    ALIB_MESSAGE("TMOD", "DedicatedWorker \"{}\" is running", GetName() )

    while(!stopJobExecuted) {
        auto jobInfo = pop();
        
        // Deferred job-deletion job
        if (jobInfo.first->Is<JobDeleter>() ) {
            auto& job= jobInfo.first->Cast<JobDeleter>();
            job.JobToDelete->PrepareDeferredDeletion();

            ALIB_ASSERT(jobInfo.second == false, "TMOD")
            ALIB_LOCK_WITH(manager)
                auto size= job.JobToDelete->SizeOf();
                job.JobToDelete->~Job();
                manager.GetPoolAllocator().free( job.JobToDelete, size );
                manager.GetPoolAllocator().free( &job,  sizeof(JobDeleter) );

            goto CONTINUE;
        }


        // overloaded custom process?
        if ( process(*jobInfo.first) )
            goto CONTINUE;


        // Stop!
        if (jobInfo.first->Is<JobStop>() ) {
            stopJobExecuted= true;
            ALIB_ASSERT(jobInfo.second == false, "TMOD")
            ALIB_LOCK_WITH(manager)
                manager.GetPoolAllocator().free( jobInfo.first,  sizeof(JobStop) );
            goto CONTINUE;
        }

        // Custom method, implemented with Job::Do()
        if ( jobInfo.first->Do() ) {
            // delete?
            if ( !jobInfo.second )
            {ALIB_LOCK_WITH(manager)
                auto size=  jobInfo.first->SizeOf();
                jobInfo.first->~Job();
                manager.GetPoolAllocator().free(  jobInfo.first,  size );
            }

            goto CONTINUE;
        }

        // Not processed!
        ALIB_ERROR("TMOD",
            "Job of type <{}> passed to DedicatedWorker, which was neither recognized by\n"
            "the specialist nor has it a Job::Do() implementation!", &jobInfo.first->ID )

        CONTINUE:
        statLastJobExecution.Reset();
    }

    ALIB_ASSERT_WARNING( Load() == 0, "TMOD",
        "DedicatedWorker \"{}\" has jobs still queued when stopped!", GetName(), Load() )
    
    ALIB_MESSAGE( "TMOD", "DedicatedWorker \"{}\" is stopping (leaving method Run()).", GetName() )
}

} // namespace [alib::threadmodel]

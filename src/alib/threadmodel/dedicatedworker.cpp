// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/threadmodel/dedicatedworker.hpp"
#include "alib/enums/serialization.hpp"

namespace alib::threadmodel {

// #################################################################################################
// threadmodel::Bootstrap()
// #################################################################################################
#if !ALIB_CAMP
#if ALIB_DEBUG && !DOXYGEN
    namespace{ unsigned int initFlag= 0; }
#endif // !DOXYGEN


#include "alib/lang/callerinfo_functions.hpp"
    void Bootstrap()
    {
        ALIB_ASSERT_ERROR( initFlag == 0, "ENUMS", "This method must not be invoked twice." )
        ALIB_DBG(initFlag= 0x92A3EF61;)

        #if !ALIB_CAMP
        alib::enums::EnumRecords<Priority>::Bootstrap(
        {
            { Priority::Lowest          , A_CHAR("Lowest"             ), 4 },
            { Priority::DeferredDeletion, A_CHAR("DeferredDeletion"   ), 1 },
            { Priority::Low             , A_CHAR("Low"                ), 1 },
            { Priority::Standard        , A_CHAR("Standard"           ), 1 },
            { Priority::Highest         , A_CHAR("Highest"            ), 5 },
            { Priority::High            , A_CHAR("High"               ), 1 },
        } );
    #endif // !ALIB_CAMP
    }
#include "alib/lang/callerinfo_methods.hpp"
#endif // !ALIB_CAMP

// #################################################################################################
// DWManager
// #################################################################################################

DWManager::DWManager()
: ma             (ALIB_DBG("DWManager",) 16)
, pool           (ma)
, workers     (ma)
{
    #if ALIB_DEBUG
        Lock::Dbg.Name= "DWManager";
    #endif
    #if ALIB_DEBUG_CRITICAL_SECTIONS
        ma.DbgCriticalSectionsPH.Get()->DCSLock= this;
    #endif
}


void DWManager::Add(DedicatedWorker& thread )
{ALIB_LOCK
    #if ALIB_DEBUG
        for( auto it : workers )
            ALIB_ASSERT_ERROR( it != &thread, "MGTHR", "Thread already added" )
    #endif

    workers.PushBack( &thread );
    thread.Start();
}

bool  DWManager::Remove( DedicatedWorker& thread, Priority stopPriority ) {
    {ALIB_LOCK
        auto it= std::find( workers.begin(), workers.end(), &thread );
        if( it == workers.end() ) {
            ALIB_WARNING( "MGTHR", NString256("Thread \"") << thread.GetName()
                                    << "\"to remove not found ")
            return false;
        }
        workers.Erase( it );
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
        if( waitCheck.Age().InAbsoluteSeconds() == nextWarnSecond )
        {
            ALIB_WARNING( "MGTHR",  NString4K("DWManager::Remove: Waiting on thread \"" )
                        << thread.GetName() << "\" to stop. "
                           "State::"  <<  thread.GetState()
                        << ", Load: " <<  thread.Load() )
            nextWarnSecond++;
        }
        #endif
    }

    if ( thread.GetState() != Thread::State::Terminated )
        thread.Join();

    return true;
}

bool DWManager::WaitForAllIdle( Ticks::Duration timeout
                         ALIB_DBG(, Ticks::Duration dbgWarnAfter) )
{
    ALIB_MESSAGE("MGTHR", "DWManager::StopAndJoinAll" )

    Ticks waitStart= Ticks::Now();
    ALIB_DBG( Ticks nextWarning= waitStart + dbgWarnAfter; )
    while(true)
    {
        // check that all threads are stopped
        int cntRunning= 0;
        {ALIB_LOCK
            for( auto it : workers )
                if( it->Load() > 0  )
                    cntRunning++;
        }
        if( cntRunning == 0 )
            return true;

        #if ALIB_DEBUG
            if( nextWarning.Age() > dbgWarnAfter)
            {
                NString4K dbgThreadList;
                dbgThreadList << "Waiting on " << cntRunning << " thread(s) to become idle:\n";
                int tNr= 0;
                {ALIB_LOCK
                    for( auto it : workers  )
                        if ( it->Load() > 0  )
                            dbgThreadList << ++tNr << ": " << it->GetName()
                                                   << ",\tState::" <<  it->state
                                                   << ",\t Load: " <<  it->Load()  << NEW_LINE;
                }
                ALIB_WARNING( "MGTHR", dbgThreadList )
                nextWarning= Ticks::Now();
            }
        #endif

        // check timeout
        if (waitStart.Age() > timeout)
            return false;

        // sleep
        Thread::SleepMicros( 50 );
    }
}

void DWManager::RemoveAll( Priority stopPriority )
{
    ALIB_MESSAGE("MGTHR", "DWManager::StopAndJoinAll" )

    ALIB_DBG( Ticks waitCheck= Ticks::Now();
              int nextWarnSecond= 1; )

    // send stop to those unstopped
    for( auto it : workers )
        if( !it->StopIsScheduled() )
            it->ScheduleStop( stopPriority );

    ALIB_DBG( waitCheck= Ticks::Now(); nextWarnSecond= 1; )
    int cntRunning;
    while(true)
    {
        // check that all threads are stopped
        cntRunning= 0;
        for( auto it : workers )
            if( it->GetState() < Thread::State::Done  )
                cntRunning++;
        if( cntRunning == 0 )
            break;

        Thread::SleepMicros( 10 );
        #if ALIB_DEBUG
            if( waitCheck.Age().InAbsoluteSeconds() == nextWarnSecond )
            {
                NString4K dbgThreadList("DWManager Termination: Waiting on " );
                dbgThreadList << cntRunning << " Threads to stop. List of threads: " << NEW_LINE;
                int tNr= 0;
                for( auto it : workers  )
                {
                    dbgThreadList << ++tNr << ": " << it->GetName()
                                           << ",\tState::" <<  it->state
                                           << ",\t Load: " <<  it->Load()  << NEW_LINE;
                }
            ALIB_WARNING( "MGTHR", dbgThreadList.NewLine().Terminate() )
                nextWarnSecond++;
            }
        #endif
    }

    // terminate all registered MThreads and remove them from our the list.
    for( auto it : workers )
        it->Join();
    workers.Clear();
}

// #################################################################################################
// DedicatedWorker
// #################################################################################################
void DedicatedWorker::pushAndRelease(QueueElement&& jobInfo)
{
    Acquire(ALIB_CALLER_PRUNED);

    // search the first element with equal or higher priority
    auto  it=   queue.begin();
    while(it != queue.end())
    {
        if(jobInfo.priority <= it->priority )
            break;
        it++;
    }

    // insert before found
    queue.Insert(it, jobInfo );

    ALIB_MESSAGE( "MGTHR/QUEUE", NString512() <<
            "Queue("  << queue.Count() << ") "
             "Job("   << jobInfo.job->ID << ") pushed. "
             "P::"    << jobInfo.priority << ", "
             "Keep: " << jobInfo.keepJob         )

    ++length;
    ALIB_DBG( DbgMaxQueuelength= (std::max)(DbgMaxQueuelength, length); )

    ReleaseAndNotify(ALIB_CALLER_PRUNED);
}

std::pair<Job*, bool>  DedicatedWorker::pop()
{ALIB_LOCK
    WaitForNotification(ALIB_CALLER_PRUNED);
    ALIB_ASSERT_ERROR(length != 0, "MGTHR", "Job pipe empty after wakeup" )
    ALIB_MESSAGE( "MGTHR/QUEUE", "Queue--, size: ", length )

    std::pair<Job*, bool> result= { queue.Back().job, queue.Back().keepJob };
    ALIB_DBG( auto dbgPriority= queue.Back().priority; )

    queue.PopBack();
    --length;

    ALIB_MESSAGE( "MGTHR/QUEUE", NString512() <<
            "Queue("  << length << ") "
             "Job("   << result.first->ID << ") popped. "
             "P::"    << dbgPriority << ", "
             "Keep: " << result.second         )


    return result;
}

void DedicatedWorker::Run()
{
    ALIB_MESSAGE("MGTHR", NString256( "DedicatedWorker \"") << GetName() << "\" is running" )

    while(!stopJobExecuted)
    {
        auto jobInfo = pop();
        
        // Deferred job-deletion job
        if (jobInfo.first->Is<JobDeleter>() )
        {
            auto& job= jobInfo.first->Cast<JobDeleter>();
            job.JobToDelete->PrepareDeferredDeletion();

            ALIB_ASSERT(jobInfo.second == false)
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
        if (jobInfo.first->Is<JobStop>() )
        {
            stopJobExecuted= true;
            ALIB_ASSERT(jobInfo.second == false)
            ALIB_LOCK_WITH(manager)
                manager.GetPoolAllocator().free( jobInfo.first,  sizeof(JobStop) );
            goto CONTINUE;
        }

        // Custom method, implemented with Job::Do()
        if ( jobInfo.first->Do() )
        {
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
        ALIB_ERROR("MGTHR", NString512() <<
            "Job of type <" << jobInfo.first->ID << ">"
            "passed to DedicatedWorker, which was neither recognized by\n"
            "the specialist nor has it a Job::Do() implementation!" )

        CONTINUE:
        statLastJobExecution.Reset();
    }

    ALIB_ASSERT_WARNING( Load() == 0, "MGTHR", NString256() <<
        "DedicatedWorker \"" << GetName() << "\" has " << Load() <<
        " jobs still queued when stopped!\n" )
    
    ALIB_MESSAGE( "MGTHR", NString256() <<
        "DedicatedWorker \"" << GetName() << "\" is stopping (leaving method Run())." )
}

} // namespace [alib::threadmodel]



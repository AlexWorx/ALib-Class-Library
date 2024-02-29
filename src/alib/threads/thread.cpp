// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined(HPP_ALIB_THREADS_DETAIL_THREADMAP)
#   include "alib/threads/detail/threadmap.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

#if !defined (_GLIBCXX_CONDITION_VARIABLE) && !defined(_CONDITION_VARIABLE_)
#   include <condition_variable>
#endif

#if !defined (_GLIBCXX_ATOMIC) && !defined(_ATOMIC_)
#   include <atomic>
#endif


#if ALIB_ENUMS
#   if !defined(HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#       include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#endif


#endif // !defined(ALIB_DOX)

namespace alib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \c threads of the \aliblink, which
 * holds types of library module \alib_threads.
 *
 * \attention
 *   This module must not be omitted from an \alibdist if the using software does not make
 *   direct use of this module. If omitted, all other \alibmods will <b>silently (!)</b> drop
 *   the protection of their resources against multi-threaded access.<br>
 *
 * Further documentation is provided with
 * - \ref alib_mod_threads "ALib Module Threads - Programmer's Manual".
 * - Chapter \ref alib_manual_modules_impact_singlethreaded of the Programmer's Manual of \alib.
 *
 **************************************************************************************************/
namespace threads {

// #################################################################################################
// Anonymous data
// #################################################################################################
#if !defined(ALIB_DOX)
namespace {

    ThreadID                                        nextSystemThreadId  = static_cast<ThreadID>(-1);
    std::atomic<ThreadID>                           nextThreadIdx(1);
    Thread*                                         mainThread          = nullptr;

} // anonymous namespace
#endif


// #################################################################################################
// Details
// #################################################################################################
/**
 * Details of namespace #alib::threads.
 */
namespace detail {

#if ALIB_MONOMEM
    HashMap           <std::thread::id, Thread *>    threadMap(&monomem::GlobalAllocator);
    #define LOCK_THREADS  ALIB_OWN( monomem::GlobalAllocatorLock )

#else
    std::unordered_map<std::thread::id, Thread *>      threadMap;
    std::mutex                                         moduleLock;
    #define LOCK_THREADS  std::unique_lock<std::mutex> uniqueLock( moduleLock );
#endif

void threadStart( Thread* thread )
{
    thread->state           = Thread::State::Running;
    thread->Run();
    thread->state           = Thread::State::Stopped;
}

Thread* getThread(std::thread::id c11ID )
{
    ALIB_ASSERT_ERROR( mainThread, "THREADS", "ALib Module Threads not initialized." )
    Thread* result= nullptr;

    // search current in map
    {
        LOCK_THREADS
#if ALIB_MONOMEM
        auto search=  threadMap.Find( c11ID );
#else
        auto search=  threadMap.find( c11ID );
#endif

        // found
        if ( search != threadMap.end() )
            result= search->second;

        // not found, this is a system thread!
        else
        {
            result                 = new Thread( reinterpret_cast<Runnable*>(-1), nullptr );
            result->id             = nextSystemThreadId--;
            result->state          = Thread::State::Running;           // we just "guess" here, as documented
            result->SetName( String64(A_CHAR("SYS_")) << result->id ); // with method Thread::GetState()
#if ALIB_MONOMEM
            threadMap.EmplaceUnique( c11ID, result );
#else
            threadMap.insert( std::make_pair( c11ID, result) );
#endif
        }
    }

    return result;
}


} // namespace alib::threads[::detail];

using namespace detail;

// #################################################################################################
// Namespace functions
// #################################################################################################
void Bootstrap()
{
    // already invoked?
    if( mainThread )
        return;

    mainThread=  new Thread();
    mainThread->id= static_cast<ThreadID>(-1);
    mainThread->SetName(A_CHAR("MAIN_THREAD"));
    mainThread->state = Thread::State::Running;

#if ALIB_MONOMEM
    threadMap.EmplaceUnique( std::this_thread::get_id(), mainThread );
#else
    threadMap.insert( std::make_pair( std::this_thread::get_id(), mainThread) );
#endif

    ALIB_ASSERT_ERROR( mainThread->GetId() == static_cast<ThreadID>(-1), "THREADS",
       "Error initializing threads. Probably forbidden repeated initialization from different thread." )

    // Assign enum records (not resourced, because Threads is not a Camp Module)
    #if ALIB_ENUMS && ALIB_BOXING && !ALIB_CAMP
        alib::EnumRecords<alib::threads::Thread::State>::Bootstrap(
        {
            { alib::threads::Thread::State::Unstarted , A_CHAR("Unstarted" )  },
            { alib::threads::Thread::State::Started   , A_CHAR("Started"   )  },
            { alib::threads::Thread::State::Running   , A_CHAR("Running"   )  },
            { alib::threads::Thread::State::Stopped   , A_CHAR("Stopped"   )  },
            { alib::threads::Thread::State::Terminated, A_CHAR("Terminated")  },
        } );

        #if ALIB_BOXING
            ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::threads::Thread::State   )
        #endif
    #endif

}


void Shutdown()
{
    // already invoked?
    if( mainThread == nullptr )
        return;

    {
        LOCK_THREADS

        // we should have exactly one thread and this is the system thread
        #if ALIB_MONOMEM
            auto qtyThreads= threadMap.Size();
        #else
            auto qtyThreads= threadMap.size();
        #endif

        if( qtyThreads != 1 )
        {
            #if ALIB_DEBUG
                NString4K dbgThreadList("ALib Termination: More than one thread running: ");
                dbgThreadList << static_cast<integer>(qtyThreads) << NewLine();
                int tNr= 0;
                for( auto it : threadMap  )
                    dbgThreadList << ++tNr << ": " << it.second->GetName()
                                           << ",\tState::" <<
                #if ALIB_ENUMS && ALIB_BOXING
                                                                it.second->state
                #else
                                                                int(it.second->state )
                #endif
                                           << NewLine();
                ALIB_WARNING( dbgThreadList.Terminate() )
            #endif

            delete mainThread;
            mainThread= nullptr;
            return;
        }

        Thread* lastThread= threadMap.begin()->second;
        ALIB_ASSERT_WARNING( lastThread->id == static_cast<ThreadID>(-1), "THREADS",
                             "threads::Shutdown: last thread is not the main system thread detected "
                             "in threads::Bootstrap" )
        delete lastThread;

        mainThread= nullptr;
    }
}

// #################################################################################################
// class Thread
// #################################################################################################
Thread::Thread( Runnable* target , const String& pName )
: runnable(target)
, name(pName)
{
    // get myself an ID
    id=  nextThreadIdx++;
    if ( name.IsEmpty() )
        name << '(' << id << ')';
}

Thread::~Thread()
{
    if( c11Thread !=nullptr )
    {
        NString512 msg;
        msg << "Thread \"" << GetName()
            << "\" was not terminated before destruction. Use Thread::Terminate() to "
               "avoid this message.";
        ALIB_WARNING( msg.Terminate()  )
        Terminate();
    }
}

void Thread::Terminate()
{
    if( c11Thread !=nullptr )
    {
        if( state != State::Stopped )
        {
            NString512 msg;
            msg << "Terminating thread \"" << GetName() << "\" which is not in state 'Stopped'. State: '"
                #if ALIB_ENUMS && ALIB_BOXING
                    << state        << "'.";
                #else
                    << int(state)   << "'.";
                #endif
            ALIB_WARNING( "THREADS", msg.Terminate() )
        }

        // join
        if( c11Thread->joinable() )
            c11Thread->join();
        else
        {
            NString512 msg;
            msg << "Thread \"" << GetName() << "\" not joinable. State: '"
                #if ALIB_ENUMS && ALIB_BOXING
                    << state        << "'.";
                #else
                    << int(state)   << "'.";
                #endif

            ALIB_WARNING( "THREADS", msg.Terminate() )
        }

        // erase from thread map
        {
            LOCK_THREADS

            #if ALIB_MONOMEM
                ALIB_ASSERT_RESULT_EQUALS( threadMap.Erase( c11ID )   , 1)
            #else
                ALIB_ASSERT_RESULT_EQUALS( threadMap.erase( c11ID )   , 1)
            #endif
        }

        delete c11Thread;
        c11Thread= nullptr;
        state           = State::Terminated;
    }
    else
    {
        if( state == State::Terminated )
            ALIB_WARNING( "THREADS",
                          "Double invocation of Thread::Terminate for thread {!Q}",
                          NString256(GetName()) )
        else
            ALIB_WARNING( "THREADS",
                          "Terminating thread {!Q} which is not started or not managed by ALIB",
                          NString256(GetName()) )
    }
}



void  Thread::Start()
{
    if ( c11Thread != nullptr )
    {
        ALIB_ERROR( "THREADS", NString128("Thread already started. ID: ")
                                << static_cast<integer>(GetId()) )
        return;
    }

    if ( id <= 0 )
    {
        ALIB_ERROR( "THREADS", NString128("System threads can not be started. ID: ")
                                << static_cast<integer>(GetId()) )
        return;
    }

    state           = Thread::State::Started;

    {
        LOCK_THREADS // <- locks on monomem::GlobalAllocatorLock or, without memory, on singleton "moduleLock"
            c11Thread=    new std::thread( threadStart, this );
            c11ID=        c11Thread->get_id();

            #if ALIB_MONOMEM
                threadMap.EmplaceUnique( c11ID, this );
            #else
                threadMap.insert( std::make_pair( c11ID, this) );
            #endif
    }
}


// #################################################################################################
// static methods
// #################################################################################################
Thread* Thread::GetMain()
{
    return mainThread;
}


}} // namespace [alib::threads]

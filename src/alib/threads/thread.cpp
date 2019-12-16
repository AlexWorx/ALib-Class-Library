// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
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

#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \b threads of the \aliblink which
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
 * Details of namespace #aworx::lib::threads.
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
    thread->Run();
    thread->isAliveFlag= false;
}

Thread* getThread(std::thread::id c11ID )
{
    ALIB_ASSERT_ERROR( mainThread, "ALib Module Threads not initialized." )
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
            result              = new Thread( reinterpret_cast<Runnable*>(-1), nullptr );
            result->id          = nextSystemThreadId--;
            result->isAliveFlag = true;
            result->SetName( String64(A_CHAR("SYS_")) << result->id );
#if ALIB_MONOMEM
            threadMap.EmplaceUnique( c11ID, result );
#else
            threadMap.insert( std::make_pair( c11ID, result) );
#endif
        }
    }

    return result;
}


} // namespace aworx::lib::threads[::detail];

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
#if ALIB_MONOMEM
    threadMap.EmplaceUnique( std::this_thread::get_id(), mainThread );
#else
    threadMap.insert( std::make_pair( std::this_thread::get_id(), mainThread) );
#endif

    ALIB_ASSERT_ERROR( mainThread->GetId() == static_cast<ThreadID>(-1),
       "Error initializing threads. Probably forbidden repeated initialization from different thread." )
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
            delete mainThread;
            mainThread= nullptr;
            ALIB_WARNING( "ALib Termination: More than one thread running: ", static_cast<integer>(qtyThreads) )
            return;
        }

        Thread* lastThread= threadMap.begin()->second;
        ALIB_ASSERT_WARNING( lastThread->id == static_cast<ThreadID>(-1),
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
    if (c11Thread)
    {
        if( c11Thread->joinable() )
            c11Thread->join();
        {
            LOCK_THREADS

            #if ALIB_MONOMEM
                ALIB_ASSERT_RESULT_EQUALS( threadMap.Erase( c11ID )   , 1)
            #else
                ALIB_ASSERT_RESULT_EQUALS( threadMap.erase( c11ID )   , 1)
            #endif
        }

        delete c11Thread;
    }
}


void  Thread::Start()
{
    if ( c11Thread != nullptr )
    {
        ALIB_ERROR( "Thread already started. ID: ", static_cast<integer>(GetId()) )
        return;
    }

    if ( id <= 0 )
    {
        ALIB_ERROR( "System threads can not be started. ID: ", static_cast<integer>(GetId()) )
        return;
    }

    isAliveFlag= true;

    {
        LOCK_THREADS
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


}}}// namespace [aworx::lib::threads]



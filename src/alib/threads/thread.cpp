// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(HPP_ALIB_THREADS_THREAD)
#   include "alib/threads/thread.hpp"
#endif

#if !defined(HPP_ALIB_TIME_TIME)
#   include "alib/time/time.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

#if !defined(_GLIBCXX_UNORDERED_MAP) && !defined(_UNORDERED_MAP_)
#   include <unordered_map>
#endif

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \b threads of the \aliblink which
 * holds types of library module \alibmod_threads.
 *
 * Further documentation is provided with
 * \ref alib_mod_threads "ALib Module Threads - Programmer's Manual".
 **************************************************************************************************/
namespace threads {

// #################################################################################################
// Anonymous data
// #################################################################################################
#if !ALIB_DOCUMENTATION_PARSER
namespace {

    ThreadID                                          nextSystemThreadId                   = static_cast<ThreadID>(-1);

    ThreadID                                          nextThreadId                         =  1;

    std::mutex                                        mutex;

    std::unordered_map<std::thread::id, Thread*>      threadMap;

    Thread*                                           mainThread                          = nullptr;
} // anonymous namespace
#endif

// #################################################################################################
// Namespace functions
// #################################################################################################
void Init()
{
    // already invoked?
    if( mainThread )
        return;

    mainThread=  new Thread();
    mainThread->id= static_cast<ThreadID>(-1);
    mainThread->SetName(A_CHAR("MAIN_THREAD"));
    threadMap.insert( std::make_pair( std::this_thread::get_id(), mainThread) );

    ALIB_ASSERT_ERROR( mainThread->GetId() == static_cast<ThreadID>(-1),
       "Error initializing threads. Probably forbidden repeated initialization from different thread." )
}


void TerminationCleanUp()
{
    mutex.lock();
        // already invoked?
        if( mainThread == nullptr )
            return;

        // we should have exactly one thread and this is the system thread
        size_t qtyThreads= threadMap.size();
        if( qtyThreads != 1 )
        {
            ALIB_WARNING( "ALib Termination: More than one thread running: ", static_cast<integer>(qtyThreads) );
            delete mainThread;
            mainThread= nullptr;
            mutex.unlock();
            return;
        }

        Thread* main= threadMap.begin()->second;
        #if ALIB_DEBUG
            ThreadID mainThreadID= main->id;
            if ( mainThreadID != static_cast<ThreadID>(-1) )
            {
                mutex.unlock();
                    ALIB_ASSERT_WARNING( main->id == static_cast<ThreadID>(-1),
                                         "ALib Termination: last thread id != -1 (not system thread)" )
                mutex.lock();
            }
        #endif
        delete main;

    mainThread= nullptr;
    mutex.unlock();
}

// #################################################################################################
// class Thread
// #################################################################################################
Thread::Thread()
: Thread( nullptr, A_CHAR("") )
{}

Thread::Thread( const String& threadName )
: Thread( nullptr, threadName )
{}

Thread::Thread( Runnable* target , const String& pName )
: runnable(target)
, name(pName)
{
    // special internal use of constructor (encoded by setting target pointer to -1)
    if( target == reinterpret_cast<Runnable*>(-1) )
        runnable= nullptr;
    else
    {
        // get myself an ID
        mutex.lock();
        {
            id=  nextThreadId++;
            if ( name.IsEmpty() )
                name << '(' << id << ')';
        }
        mutex.unlock();
    }
}

Thread::~Thread()
{
    if (c11Thread)
    {
        if( c11Thread->joinable() )
            c11Thread->join();
        mutex.lock();
        {
            ALIB_ASSERT_RESULT_EQUALS( threadMap.erase( c11ID ), 1);
        }
        mutex.unlock();

        delete c11Thread;
    }
}

//! @cond NO_DOX
void _Thread__Start( Thread* t )
{
    t->Run();
    t->isAliveFlag= false;
}
//! @endcond

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

    mutex.lock();
    {
        c11Thread=    new std::thread( _Thread__Start, this );
        c11ID=        c11Thread->get_id();

        threadMap.insert( std::make_pair( c11ID, this) );
    }
    mutex.unlock();
}


// #################################################################################################
// static methods
// #################################################################################################
Thread* Thread::GetCurrent()
{
    ALIB_ASSERT_ERROR( mainThread, "ALib Module Threads not initialized." )
    Thread* result= nullptr;

    // search current in map
    auto    c11ID=  std::this_thread::get_id();
    mutex.lock();
    {
        auto search=  threadMap.find( c11ID );

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
            threadMap.insert( std::make_pair( c11ID, result) );
        }
    }
    mutex.unlock();

    return result;
}

Thread* Thread::GetMain()
{
    return mainThread;
}


}}}// namespace [aworx::lib::threads]



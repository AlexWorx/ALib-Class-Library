// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_THREADS_LIB)
    #include "threadlib.hpp"
#endif


namespace aworx { namespace lib {

// The singleton of the library class located in namespace aworx::lib.
threads::Threads THREADS;

namespace threads {

// #################################################################################################
// class Threads (Library class)
// #################################################################################################


// static library initialization code ( invoked by ALIB::Init() )
void Threads::init( Phases phase )
{
    if( phase == Phases::resourceset )
    {
        Thread* main;

        #if !ALIB_FEAT_THREADS
            main=
            Threads::noThreadsCompilationMainThread= new Thread();
            Threads::noThreadsCompilationMainThread->id= -1;
            Threads::noThreadsCompilationMainThread->SetName(ASTR("SINGLE"));
        #else
            // create the current thread as an object within our thread map
            (main= THREADS.CurrentThread())->SetName( ASTR("MAIN") );
        #endif

        ALIB_ASSERT_ERROR( main->GetId() == -1,
           ASTR("Error initializing threads. Probably forbidden repeated initialization from different thread.") );
        (void) main;
    }
}


// static library destruction code ( invoked by ALIB::TerminationCleanUp() )
void Threads::terminationCleanUp()
{
    #if ALIB_FEAT_THREADS
        mutex.lock();
            ALIB_ASSERT_WARNING( threadMap.size() > 0,
                                 ASTR("Thread system cleanup without prior initialization.") );

            // we should have exactly one thread and this is the system thread
            size_t qtyThreads= threadMap.size();
            #if ALIB_DEBUG
                if( qtyThreads == 0 )
                {
                    mutex.unlock();
                        ALIB_WARNING( ASTR("ALib termination: main thread not registered.") );
                    mutex.lock();
                }
                if( qtyThreads > 1 )
                {
                    mutex.unlock();
                        ALIB_WARNING( ASTR("ALib termination: Can not shutdown gracefully due to open threads.") );
                    mutex.lock();
                }
            #endif

            if( qtyThreads != 1 )
            {
                mutex.unlock();
                return;
            }

            Thread* main= threadMap.begin()->second;
            #if ALIB_DEBUG
                int mainThreadID= main->id;
                if ( mainThreadID != -1 )
                {
                    mutex.unlock();
                        ALIB_ASSERT_WARNING( main->id == -1, ASTR("ALib Termination: last thread id != -1 (not system thread)") );
                    mutex.lock();
                }
            #endif
            delete main;

        mutex.unlock();
    #else
        ALIB_ASSERT_ERROR( noThreadsCompilationMainThread != nullptr,
                           ASTR("Thread system cleanup without prior initialization.") );
        if ( noThreadsCompilationMainThread != nullptr )
        {
            delete noThreadsCompilationMainThread;
            noThreadsCompilationMainThread= nullptr;
        }
    #endif
}

Thread* Threads::CurrentThread()
{
    Thread* result= nullptr;

    #if ALIB_FEAT_THREADS
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
                result=        new Thread( true );
                result->id=    nextSystemThreadId--;
                result->SetName( String64(ASTR("SYS_")) << result->id );
                threadMap.insert( std::make_pair( c11ID, result) );
            }
        }
        mutex.unlock();
    #else
        result= noThreadsCompilationMainThread;
    #endif

    return result;
}

// #################################################################################################
// class Thread
// #################################################################################################
Thread::Thread( bool BecomesInternalThread )
{
    (void) BecomesInternalThread;

    // this is a system thread, no matter what the parameter says. In fact, the parameter is only
    // for providing a different, internal constructor
    ALIB_ASSERT_ERROR ( BecomesInternalThread, ASTR("Parameter must not be false. Illegal use") );
}



Thread::Thread( Runnable* target , const String& pName )
: runnable(target)
, name(pName)
{
    // get myself an ID
    #if ALIB_FEAT_THREADS
    THREADS.mutex.lock();
    #endif
    {
        id=  THREADS.nextThreadId++;
        if ( name.IsEmpty() )
            name << '(' << id << ')';
    }
    #if ALIB_FEAT_THREADS
    THREADS.mutex.unlock();
    #endif
}

Thread::~Thread()
{
    #if ALIB_FEAT_THREADS
    if (c11Thread)
    {
        if( c11Thread->joinable() )
            c11Thread->join();
        THREADS.mutex.lock();
        {
            ALIB_ASSERT_RESULT_EQUALS( THREADS.threadMap.erase( c11ID ), 1);
        }
        THREADS.mutex.unlock();

        delete c11Thread;
    }
    #endif
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
        ALIB_ERROR( ASTR("Thread already started. ID: "), GetId() )
        return;
    }

    if ( id <= 0 )
    {
        ALIB_ERROR( ASTR("System threads can not be started. ID: "), GetId() )
        return;
    }

    isAliveFlag= true;

    #if ALIB_FEAT_THREADS
        THREADS.mutex.lock();
        {
            c11Thread=    new std::thread( _Thread__Start, this );
            c11ID=        c11Thread->get_id();

            THREADS.threadMap.insert( std::make_pair( c11ID, this) );
        }
        THREADS.mutex.unlock();
    #else
            c11Thread=    this;
            isAliveFlag=  false;
            ALIB_WARNING( ASTR("Starting Thread not supported. ALib is compiled with compilation symbol ALIB_FEAT_THREADS_OFF") )
    #endif
}


}}}// namespace [aworx::lib::threads]



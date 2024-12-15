// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/threads/detail/threadmap.hpp"
#   include "alib/strings/localstring.hpp"
#   include <condition_variable>
#   include <atomic>
#   if ALIB_ENUMS
#      include "alib/enums/recordbootstrap.hpp"
#      include "alib/enums/serialization.hpp"
#   endif
#endif // !DOXYGEN


namespace alib {

//==================================================================================================
/// This is the reference documentation of sub-namespace \c threads of the \aliblink, which
/// holds types of library module \alib_threads.
///
/// \attention
///   This module must not be omitted from an \alibdist if the using software does not make
///   direct use of this module. If omitted, all other \alibmods will <b>silently (!)</b> drop
///   the protection of their resources against multithreaded access.<br>
///
/// Further documentation is provided with
/// - \ref alib_mod_threads "ALib Module Threads - Programmer's Manual".
/// - Chapter \ref alib_manual_modules_impact_singlethreaded of the Programmer's Manual of \alib.
///
//==================================================================================================
namespace threads {

// #################################################################################################
// Anonymous data
// #################################################################################################
#if !DOXYGEN
namespace {

    ThreadID                                        nextSystemThreadId  = static_cast<ThreadID>(-1);
    std::atomic<ThreadID>                           nextThreadIdx(1);
    Thread*                                         MAIN_THREAD         = nullptr;

} // anonymous namespace
#endif


// #################################################################################################
// Details
// #################################################################################################
/// Details of namespace #alib::threads.
namespace detail {

#if (ALIB_MONOMEM && ALIB_CONTAINERS) || DOXYGEN
    #if DOXYGEN
    /// Directory which assigns system thread IDs to \alib Thread objects.
      HashMap  <MonoAllocator, std::thread::id, Thread *>    THREAD_MAP;
    #else
      HashMap  <MonoAllocator, std::thread::id, Thread *>    THREAD_MAP(monomem::GLOBAL_ALLOCATOR);
    #endif
    #define LOCK_THREADS  ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
#else
    std::unordered_map<std::thread::id, Thread *>            THREAD_MAP;
    std::mutex                                               MODULE_LOCK;
    #define LOCK_THREADS  std::unique_lock<std::mutex>       uniqueLock( MODULE_LOCK );
#endif

void threadStart( Thread* thread )
{
    thread->state           = Thread::State::Running;
    thread->Run();
    thread->state           = Thread::State::Done;
}

#include "alib/lang/callerinfo_functions.hpp"

} // namespace alib::threads[::detail];

using namespace detail;

// #################################################################################################
// Namespace functions
// #################################################################################################
#if ALIB_DEBUG && !DOXYGEN
    namespace{ unsigned int initFlag= 0; }
#endif // !DOXYGEN

void Bootstrap()
{
    ALIB_ASSERT_ERROR( initFlag == 0, "THREADS", "This method must not be invoked twice." )
    ALIB_DBG(initFlag= 0x92A3EF61);

    // already invoked?
    if( MAIN_THREAD )
        return;

    Thread* mainThread=  new Thread();
    mainThread->id= static_cast<ThreadID>(-1);
    mainThread->SetName(A_CHAR("MAIN_THREAD"));
    mainThread->state = Thread::State::Running;

    #if ALIB_MONOMEM && ALIB_CONTAINERS
        THREAD_MAP.EmplaceUnique( std::this_thread::get_id(), mainThread );
    #else
        THREAD_MAP.insert( std::make_pair( std::this_thread::get_id(), mainThread) );
    #endif

    // Assign enum records (not resourced, because Threads is not a Camp Module)
    #if ALIB_ENUMS && ALIB_BOXING && !ALIB_CAMP
        alib::EnumRecords<alib::threads::Thread::State>::Bootstrap(
        {
            { alib::threads::Thread::State::Unstarted , A_CHAR("Unstarted" )  },
            { alib::threads::Thread::State::Started   , A_CHAR("Started"   )  },
            { alib::threads::Thread::State::Running   , A_CHAR("Running"   )  },
            { alib::threads::Thread::State::Done      , A_CHAR("Done"      )  },
            { alib::threads::Thread::State::Terminated, A_CHAR("Terminated")  },
        } );

        #if ALIB_BOXING
            ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( alib::threads::Thread::State   )
        #endif
    #endif

    MAIN_THREAD= mainThread; // to this only here, as this flags the availability
}


void Shutdown()
{
    ALIB_ASSERT_ERROR( initFlag == 0x92A3EF61, "THREADS", "Not initialized when calling shutdown." )
    ALIB_DBG(initFlag= 1);

    // already invoked?
    if( MAIN_THREAD == nullptr )
        return;

    {
        LOCK_THREADS

        // we should have exactly one thread and this is the system thread
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            auto qtyThreads= THREAD_MAP.Size();
        #else
            auto qtyThreads= THREAD_MAP.size();
        #endif

        if( qtyThreads != 1 )
        {
            #if ALIB_DEBUG
                String4K dbgThreadList("ALib Termination: More than one thread running: ");
                dbgThreadList << static_cast<integer>(qtyThreads) << NEW_LINE;
                int tNr= 0;
                for( auto it : THREAD_MAP  )
                    dbgThreadList << ++tNr << ": " << it.second << ",\tState::" <<
                #if ALIB_ENUMS && ALIB_BOXING
                                                                it.second->state
                #else
                                                                int(it.second->state )
                #endif
                                           << NEW_LINE;
                ALIB_STRINGS_TO_NARROW(dbgThreadList, nstring, 4096)
                ALIB_WARNING( "THREADS", nstring.Terminate() )
            #endif

            delete MAIN_THREAD;
            MAIN_THREAD= nullptr;
            return;
        }

        Thread* lastThread= THREAD_MAP.begin()->second;
        ALIB_ASSERT_WARNING( lastThread->id == static_cast<ThreadID>(-1), "THREADS",
                             "threads::Shutdown: last thread is not the main system thread detected "
                             "in threads::Bootstrap" )
        delete lastThread;

        MAIN_THREAD= nullptr;
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            THREAD_MAP.Reset();
        #endif
    }
}

#include "alib/lang/callerinfo_methods.hpp"

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
    {
        // use local string to avoid higher capacity allocation.
        name._( String128()<< '(' << id << ')');
    }
}

Thread::~Thread()
{
    ALIB_ASSERT_WARNING( GetState() != Thread::State::Unstarted , "MGTHR", NString256() <<
     "Thread \""<< this <<"\" destructed, while it was never started." )

    if( c11Thread != nullptr )
    {
        ALIB_WARNING( "THREADS", NString512() <<
          "Thread \"" << this << "\" was not terminated before destruction.\n"
          "Use Thread::Join() to avoid this message. Joining now!..." )
        Join();
    }
}

void Thread::Join()
{
    if( c11Thread != nullptr )
    {
        if( state != State::Done )
        {
            NString512 msg;
            msg << "Terminating thread \"" << this << "\" which is not in state 'Stopped'. State: '"
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
            msg << "Thread \"" << this << "\" not joinable. State: '"
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

            #if ALIB_MONOMEM && ALIB_CONTAINERS
                ALIB_ASSERT_RESULT_EQUALS( THREAD_MAP.Erase( nativeID )   , 1)
            #else
                ALIB_ASSERT_RESULT_EQUALS( THREAD_MAP.erase( nativeID )   , 1)
            #endif
        }

        delete c11Thread;
        c11Thread= nullptr;
        state           = State::Terminated;
    }

    // c11Thread == nullptr
    else
    {
        if( state == State::Terminated )
            ALIB_WARNING( "THREADS",
                          "Double invocation of Thread::Terminate for thread {!Q}",
                          NString256(this) )
        else
            ALIB_WARNING( "THREADS",
                          "Terminating thread {!Q} which is not started or not managed by ALIB",
                          NString256(this) )
    }
}



void  Thread::Start()
{
    if ( c11Thread != nullptr )
    {
        ALIB_ERROR( "THREADS", NString128("Thread already started. ID: ")
                                << static_cast<integer>(GetID()) )
        return;
    }

    if ( id <= 0 )
    {
        ALIB_ERROR( "THREADS", NString128("System threads cannot be started. ID: ")
                                << static_cast<integer>(GetID()) )
        return;
    }

    state           = Thread::State::Started;

    {
        LOCK_THREADS // <- locks on monomem::GLOBAL_ALLOCATOR_LOCK or, without monomem, on singleton "MODULE_LOCK"
            c11Thread=    new std::thread( threadStart, this );
            nativeID=        c11Thread->get_id();

            #if ALIB_MONOMEM && ALIB_CONTAINERS
                THREAD_MAP.EmplaceUnique( nativeID, this );
            #else
                THREAD_MAP.insert( std::make_pair( nativeID, this) );
            #endif
    }
}


// #################################################################################################
// static methods
// #################################################################################################
#include "alib/lang/callerinfo_functions.hpp"
Thread* Thread::GetMain()           { return MAIN_THREAD; }
Thread* Thread::Get(std::thread::id nativeID )
{
    // check for nulled nativeID or not initialized
    if( lang::IsNull(nativeID) || !MAIN_THREAD )
        return nullptr ;

    Thread* result= nullptr;

    // search current in map
    {
        LOCK_THREADS
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            auto search=  THREAD_MAP.Find( nativeID );
        #else
            auto search=  THREAD_MAP.find( nativeID );
        #endif

        // found
        if ( search != THREAD_MAP.end() )
            result= search->second;

        // not found, this is a system thread!
        else
        {
            result                 = new Thread( reinterpret_cast<Runnable*>(-1), nullptr );
            result->id             = nextSystemThreadId--;
            result->state          = Thread::State::Running;           // we just "guess" here, as documented
            result->SetName( String64(A_CHAR("SYS_")) << result->id ); // with method Thread::GetState()
            #if ALIB_MONOMEM && ALIB_CONTAINERS
                THREAD_MAP.EmplaceUnique( nativeID, result );
            #else
                THREAD_MAP.insert( std::make_pair( nativeID, result) );
            #endif
        }
    }

    return result;
}
#include "alib/lang/callerinfo_methods.hpp"



}} // namespace [alib::threads]

ALIB_STRINGS_APPENDABLE_TYPE_DEF_N(alib::threads::Thread*,
{ target << src->GetName() << '(' << src->GetID() << ')';} )

ALIB_STRINGS_APPENDABLE_TYPE_DEF_N(alib::threads::Thread,
{ target << src.GetName()  << '(' << src.GetID() << ')';} )

ALIB_STRINGS_APPENDABLE_TYPE_DEF_W(alib::threads::Thread*,
{ target << src->GetName() << '(' << src->GetID() << ')';} )

ALIB_STRINGS_APPENDABLE_TYPE_DEF_W(alib::threads::Thread,
{ target << src.GetName()  << '(' << src.GetID() << ')';} )


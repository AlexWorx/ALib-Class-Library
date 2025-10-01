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
#include "alib/boxing/boxing.prepro.hpp"
#if !ALIB_SINGLE_THREADED
#   include <condition_variable>
#   include <atomic>
#endif // !ALIB_SINGLE_THREADED

#include <unordered_map>
#include <future>
#if ALIB_DEBUG
#   include <vector>
#   include <any>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module   ALib.Threads;
    import   ALib.Lang;
#if ALIB_CONTAINERS
    import   ALib.Containers.HashTable;
#endif
#  if ALIB_BOXING
    import   ALib.Boxing;
#  endif
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
#  if ALIB_ENUMRECORDS
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
#  endif
#  if ALIB_RESOURCES
    import   ALib.Resources;
#  endif
#else
#   include "ALib.Threads.H"
#   include "ALib.Lang.H"
#   include "ALib.Containers.HashTable.H"
#   include "ALib.Boxing.H"
#   include "ALib.Strings.H"
#   include "ALib.Monomem.H"
#   include "ALib.EnumRecords.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Resources.H"
#endif
// ======================================   Implementation   =======================================
#if !ALIB_SINGLE_THREADED
namespace alib {

//==================================================================================================
/// This is the reference documentation of namespace threads of the \aliblink, which
/// holds types of library module \alib_threads.
///
/// \attention
///   At the moment the compiler-symbol \ref ALIB_SINGLE_THREADED is set with an \alibbuild,
///   this module will remain in the build, but only as skeletons. Also, the corresponding
///   preprocessor macros, like \ref ALIB_LOCK, are emptied.<br>
///   This allows writing dual-use code which compiles in either mode, without checking
///   symbol \b ALIB_SINGLE_THREADED in the using code too often.
///
/// Further documentation is provided with
/// - \ref alib_mod_threads "ALib Module Threads - Programmer's Manual".
/// - Details of CMake variable \ref ALIB_CMAKE_SKIP_THREAD_LIB_SEARCH.
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

    #if ALIB_MONOMEM && ALIB_CONTAINERS
        HashMap  <MonoAllocator, std::thread::id, Thread *>   THREAD_MAP(monomem::GLOBAL_ALLOCATOR);
        #define LOCK_THREADS  ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
    #else
        std::unordered_map<    std::thread::id, Thread*>    THREAD_MAP;
        Lock                                                MODULE_LOCK;
        #define LOCK_THREADS   ALIB_LOCK_WITH( MODULE_LOCK );
    #endif
} // anonymous namespace
#endif


// #################################################################################################
// Details
// #################################################################################################
/// Details of namespace #alib::threads.
namespace detail {

void threadStart( Thread* thread )
{
    THIS_THREAD= thread;
    thread->state           = Thread::State::Running;
    thread->Run();
    thread->state           = Thread::State::Done;
}

#   include "ALib.Lang.CIFunctions.H"

} // namespace alib::threads[::detail];

using namespace detail;

// #################################################################################################
// Namespace functions
// #################################################################################################
#if ALIB_DEBUG && !DOXYGEN
    namespace{ unsigned int initFlag= 0; }
#endif // !DOXYGEN

void BootstrapThreadMap( integer qty ) {
    #if ALIB_MONOMEM && ALIB_CONTAINERS
        THREAD_MAP.Reserve( qty, lang::ValueReference::Absolute );
    #else
        THREAD_MAP.reserve( size_t(qty) );
    #endif
}
void bootstrap() {
    ALIB_ASSERT_ERROR( initFlag == 0, "THREADS", "This method must not be invoked twice." )
    ALIB_DBG(initFlag= 0x92A3EF61);

    // already invoked?
    if( MAIN_THREAD )
        return;

    Thread* mainThread=  new Thread();
    mainThread->id= ThreadID(-1);
    mainThread->SetName(A_CHAR("MAIN_THREAD"));
    mainThread->state = Thread::State::Running;

    #if ALIB_MONOMEM && ALIB_CONTAINERS
        THREAD_MAP.EmplaceUnique( std::this_thread::get_id(), mainThread );
    #else
        THREAD_MAP.insert( std::make_pair( std::this_thread::get_id(), mainThread) );
    #endif

    MAIN_THREAD= mainThread; // This flags the availability of the module
}


void shutdown()
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
                std::vector<std::any> msgs;
                msgs.reserve( 50 );
                msgs.emplace_back( "ALib Termination: Still {} threads running.\n" );
                msgs.emplace_back( qtyThreads);
                int tNr= 0;
                for( auto& it : THREAD_MAP  ) {
                    msgs.emplace_back("  {}: {},\tState::{}\n");
                    msgs.emplace_back(++tNr);
                    msgs.emplace_back(it.second); 
                    msgs.emplace_back(it.second->state);
                }
                assert::raise( ALIB_CALLER_PRUNED, 1,  "THREADS", msgs );
            #endif

            delete MAIN_THREAD;
            MAIN_THREAD= nullptr;
            return;
        }

        Thread* lastThread= THREAD_MAP.begin()->second;
        ALIB_ASSERT_WARNING( lastThread->id == static_cast<ThreadID>(-1), "THREADS",
          "Last thread {} is not the main system thread detected in bootstrap.", lastThread->id )
        delete lastThread;

        MAIN_THREAD= nullptr;
        #if ALIB_MONOMEM && ALIB_CONTAINERS
            THREAD_MAP.Reset();
        #endif
    }
}

#   include "ALib.Lang.CIMethods.H"

// #################################################################################################
// class Thread
// #################################################################################################
thread_local Thread* THIS_THREAD                                                          = nullptr;

Thread::Thread( Runnable* target , const character* pName )
: runnable(target)
, name(pName)
{
    // get myself an ID
    id=  nextThreadIdx++;
}

void Thread::destruct()
{
    ALIB_ASSERT_WARNING( GetState() != Thread::State::Unstarted , "MGTHR",
     "Thread \"{}\" destructed, while it was never started.", this )

    if( c11Thread != nullptr )
    {
        ALIB_WARNING( "THREADS",
          "Thread \"{}\" was not terminated before destruction.\n"
          "Use Thread::Join() to avoid this message. Joining now...", this )
        Join();
    }
}

#if defined(_MSC_VER)
Thread*    Thread::GetCurrent() {
    if (THIS_THREAD == nullptr )
        THIS_THREAD= Get( std::this_thread::get_id() );
    return THIS_THREAD;
}
#endif

void Thread::Join()
{
    if( c11Thread != nullptr )
    {
        ALIB_ASSERT_WARNING( state == State::Done, "THREADS",
          "Terminating thread \"{}\" which is not in state 'Done'. State: {}.",
          this, state )

        // join
        if( c11Thread->joinable() )
            c11Thread->join();
        else
            ALIB_WARNING( "THREADS", "Thread \"{}\" not joinable. State is '{}'.",  this, state )
        
        // erase from thread map
        {
            LOCK_THREADS

            #if ALIB_MONOMEM && ALIB_CONTAINERS
                ALIB_ASSERT_RESULT_EQUALS( THREAD_MAP.erase( nativeID )   , 1)
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
                          "Double invocation of Thread::Terminate for thread \"{}\".", this )
        else
            ALIB_WARNING( "THREADS",
                          "Terminating thread \"{}\" which is not started or not managed by ALIB.",
                          this )
    }
}



void  Thread::Start()
{
    if ( c11Thread != nullptr )
    {
        ALIB_ERROR( "THREADS", "Thread with ID {} was already started.", GetID() )
        return;
    }

    if ( id <= 0 )
    {
        ALIB_ERROR( "THREADS", "System threads cannot be started. (ID={}).", GetID() )
        return;
    }

    state= Thread::State::Started;
    {
        LOCK_THREADS // <- locks on monomem::GLOBAL_ALLOCATOR_LOCK or, without monomem, on singleton "MODULE_LOCK"
            c11Thread=    new std::thread( threadStart, this );
            nativeID=     c11Thread->get_id();

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
#   include "ALib.Lang.CIFunctions.H"
Thread* Thread::GetMain()           { return MAIN_THREAD; }
Thread* Thread::Get( std::thread::id nativeID )
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
            result->state          = Thread::State::Running;  // we just "guess" here, as documented
            result->SetName(A_CHAR("<SYSTEM_THREAD>"));       // with method Thread::GetState()
            #if ALIB_MONOMEM && ALIB_CONTAINERS
                THREAD_MAP.EmplaceUnique( nativeID, result );
            #else
                THREAD_MAP.insert( std::make_pair( nativeID, result) );
            #endif
        }
    }

    return result;
}

}} // namespace [alib::threads]

#endif // !ALIB_SINGLE_THREADED

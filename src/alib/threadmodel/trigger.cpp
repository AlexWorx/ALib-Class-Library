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
#include "alib/alib.inl"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ThreadModel;
#  if ALIB_MONOMEM
    import   ALib.Monomem;
#  endif
#else
#   include "ALib.Monomem.H"
#   include "ALib.ThreadModel.H"
#endif
// ======================================   Implementation   =======================================
using namespace std::literals::chrono_literals;
namespace alib::threadmodel {


Trigger::Trigger()
: Thread         (A_CHAR("Trigger"))
, TCondition     (ALIB_DBG(A_CHAR("Trigger")))
, triggerList    (monomem::GLOBAL_ALLOCATOR)
{}

Trigger::~Trigger()
{
    if( state <= State::Started )
    {
        ALIB_WARNING("MGTHR", "Trigger destroyed without having run" )
    }
    else if( state != State::Terminated )
    {
        ALIB_ERROR("MGTHR", "Trigger destroyed without being terminated" )
        Start();
    }
}

void Trigger::Add(Triggered& triggered, bool initialWakeup)
{
    Acquire(ALIB_CALLER_PRUNED);

        // find if not already registered
        bool isExisting= false;
        for ( auto& it : triggerList )
            if ( it.Target == &triggered )
            {
                isExisting= true;
                break;
            }

        Ticks now;
        if(!isExisting )
        { ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            triggerList.emplace_back( &triggered,
                                     now + ( initialWakeup ? Ticks::Duration()
                                                           : triggered.triggerPeriod() )  );
        }
        else
#if ALIB_STRINGS
            ALIB_WARNING( "MGTHR", "Duplicate registration of triggered object \"{}\".",
                                    triggered.Name )
#endif
            ALIB_WARNING( "MGTHR", "Duplicate registration of triggered object." )

    // Wake us (thread manager) up and perform a first trigger
    wakeUpCondition= true;
    ReleaseAndNotify(ALIB_CALLER_PRUNED);
}

void Trigger::Remove(Triggered &triggered)
{
    bool found= false;

    { ALIB_LOCK
        for(auto it= triggerList.begin() ; it != triggerList.end() ; it++)
        {
            if(it->Target == &triggered)
            {
                found= true;
                triggerList.erase(it);
                break;
            }
        }
    }

    // check
    if(!found)
    {
        #if ALIB_STRINGS
            ALIB_MESSAGE("MGTHR",
             "Managed thread \"{}\" not found for de-registering with trigger list", triggered.Name)
        #else
            ALIB_MESSAGE("MGTHR", "Managed thread not found for de-registering with trigger list")
        #endif
    }
}

void Trigger::Run()
{
    ALIB_MESSAGE( "MGTHR",  "Internal trigger-thread started" )
    internalThreadMode= true;

    while(internalThreadMode)
        Do( Ticks::EndOfEpoch() );

    internalThreadMode= false;

    ALIB_MESSAGE( "MGTHR", "Internal trigger-thread exiting" )
}

void Trigger::Stop()
{
    Acquire(ALIB_CALLER_PRUNED);
      internalThreadMode = false;
      wakeUpCondition    = true;
    ReleaseAndNotify(ALIB_CALLER_PRUNED);
    Join();
}



void Trigger::Do(Ticks until)
{ ALIB_LOCK
    ALIB_ASSERT_ERROR( lang::IsNull(Dbg.AssertExclusiveWaiter),    "MGTHR",
        "Trigger::Do() called twice!\n"
        "Hint: Did you start the trigger thread and in addition 'manually' invoked Do()?\n"
        "      Only on sort of execution is allowed." )

    ALIB_DBG( Dbg.AssertExclusiveWaiter= std::this_thread::get_id(); )

    // wait loop
    bool calledByInternalThread= internalThreadMode;
    Ticks  now(lang::Initialization::Nulled);
    while(     (!calledByInternalThread || internalThreadMode )
            &&  now.Reset() < until                                 )
    {
        // trigger loop
        Ticks  nextTriggerTime= until;
        for( auto& it : triggerList )
        {
            // Trigger current?
            if( it.NextWakeup <= now  )
            {
                it.Target->trigger();
                now.Reset(); // first we increase now, then we calculate the next wakeup
                it.NextWakeup= now + it.Target->triggerPeriod();
            }

            // If earlier than others, use this as the new trigger time
            nextTriggerTime= (std::min)(nextTriggerTime, it.NextWakeup);
        }

        // sleep 
        wakeUpCondition= false;
        WaitForNotification( nextTriggerTime  ALIB_COMMA_CALLER_PRUNED  );
    }
    
    ALIB_DBG( lang::SetNull(Dbg.AssertExclusiveWaiter); )
}




} // namespace [alib::threadmodel]



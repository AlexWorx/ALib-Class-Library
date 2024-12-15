// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/threads/lock.hpp"
#   include "alib/threads/timedlock.hpp"
#   include "alib/threads/recursivelock.hpp"
#   include "alib/threads/recursivetimedlock.hpp"
#   include "alib/threads/sharedlock.hpp"
#   include "alib/threads/sharedtimedlock.hpp"
#   include "alib/lang/dbgtypedemangler.hpp"
#   include "alib/strings/localstring.hpp"
#endif // !DOXYGEN

#if ALIB_MONOMEM
#   include "alib/monomem/globalallocator.hpp"
#endif
#include    "alib/time/ticks.hpp"
#if ALIB_DEBUG
#   include "alib/threads/condition.hpp"
#endif
#if ALIB_CAMP
#   include "alib/lang/format/formatterpythonstyle.hpp"
#   include "alib/lang/system/calendar.hpp"
#endif
#if ALIB_ALOX
#   include "alib/alox.hpp"
#   include "alib/alox/textlogger/textlogger.hpp"
#endif


namespace alib {  namespace threads {

// #################################################################################################
// Class DbgLockAsserter
// #################################################################################################
#if ALIB_DEBUG
Thread*    DbgLockAsserter::GetOwner()                                                         const
{
    if ( CntAcquirements == 0 )
        return nullptr;
    return Thread::Get(AcqCI.ThreadID);
}

#if ALIB_CAMP && !DOXYGEN // otherwise doxygen will write a bad version of the initial value
NString DbgLockAsserter::ASSERTION_FORMAT=
"Multi-Threadding {} in Lock {!Q}"          "\n"
"                Message: {}"               "\n"
"   In (Member-)Function: {:ya}"            "\n"
"               Is Owned: {7}"              "\n"
                                            "\n"
"              Called By: {4:ya}"           "\n"
"                     At: {4:sf:sl}"        "\n"
"                 Thread: {4:ta}"           "\n"
                                            "\n"
"  Latest Acquisition By: {5:ya}"           "\n"
"                     At: {5:sf:sl}"        "\n"
"                 Thread: {5:ta}"           "\n"
"      Latest Release By: {6:ya}"           "\n"
"                     At: {6:sf:sl}"        "\n"
"                 Thread: {6:ta}"           "\n"
;

NString DbgSharedLockAsserter::ASSERTION_FORMAT_SHARED=
"Multi-Threadding {} in Lock {!Q}"    "\n"
"                       Message: {}"         "\n"
"          In (Member-)Function: {:ya}"      "\n"
"                      Is Owned: {7}"        "\n"
"               Is Shared Owned: {10}"        "\n"
                                             "\n"
"                     Called By: {4:ya}"     "\n"
"                            At: {4:sf:sl}"  "\n"
"                        Thread: {4:ta}"     "\n"
                                             "\n"
"         Latest Acquisition By: {5:ya}"     "\n"
"                            At: {5:sf:sl}"  "\n"
"                        Thread: {5:ta}"     "\n"
"             Latest Release By: {6:ya}"     "\n"
"                            At: {6:sf:sl}"  "\n"
"                        Thread: {6:ta}"     "\n"
                                             "\n"
"  Latest Shared Acquisition By: {8:ya}"     "\n"
"                            At: {8:sf:sl}"  "\n"
"                        Thread: {8:ta}"     "\n"
"       Latest SharedRelease By: {9:ya}"     "\n"
"                            At: {9:sf:sl}"  "\n"
"                        Thread: {9:ta}"     "\n"
;
#endif // ALIB_CAMP && !DOXYGEN


void DbgLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                const NString& headline )
{
    String4K msg;

    #if !ALIB_CAMP
    msg  << (type== 0 ? "Assertion" : "Warning")
         << " with Mutex/Locking in " << Name      << "\n"
            "              Message: " << headline  << "\n"
            "               Caller: " << ci        << "\n"
            "             Is Owned: " << (CntAcquirements>0 ? "true" : "false") << " (" << CntAcquirements << ")\n"
            "   Latest Acquirement: " << AcqCI     << "\n"
            "       Latest Release: " << RelCI     << "\n\n"
     "Note: Include ALib module BaseCamp in the ALib-Distribution to get nicer assertion output.\n";
    #else
        String256 acquirementInfo;
        acquirementInfo << (CntAcquirements>0 ? "true" : "false") << " (" <<CntAcquirements<< ')';

        FormatterPythonStyle fmt;
        fmt.Format( msg , ASSERTION_FORMAT,
                    (type== 0 ? "Assertion" : "Warning"), // 0
                    Name    , headline,                   // 1 2
                    ciAssert, ci,                         // 3 4
                    AcqCI   , RelCI,                      // 5 6
                    acquirementInfo   );                  // 7
        
    #endif
    #if ALIB_ALOX
     int oldMode= 1;
     if ( Log::DebugLogger )
     {
         oldMode= Log::DebugLogger->GetFormatMultiLine().Mode;
         Log::DebugLogger->GetFormatMultiLine().Mode= 3;
     }
    #endif

    ALIB_STRINGS_TO_NARROW(msg, nmsg, 8192) // needs to be quite big for max conversions
    DbgSimpleALibMsg( ciAssert, type, "THREADS", nmsg  );

    #if ALIB_ALOX
     if ( Log::DebugLogger )
        Log::DebugLogger->GetFormatMultiLine().Mode= oldMode;
    #endif
 }

void DbgSharedLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                      const NString& headline )
{
    String4K msg;

    #if !ALIB_CAMP
    msg  << (type== 0 ? "Assertion" : "Warning")
         << " with Mutex/Locking in " << Name << "\n"
            "                    Message: " << headline  << "\n"
            "                     Caller: " << ci        << "\n"
            "                   Is Owned: " << (CntAcquirements>0              ? "true " : "false") << " (" << CntAcquirements         << ")\n"
            "            Is Shared-Owned: " << (CntSharedAcquirements.load()>0 ? "true " : "false") << " (" << CntSharedAcquirements.load() << ")\n\n"
            "         Latest Acquirement: " << AcqCI     << "\n"
            "             Latest Release: " << RelCI     << "\n\n"
            "  Latest Shared Acquirement: " << SAcqCI    << "\n"
            "      Latest Shared Release: " << SRelCI    << "\n\n"
     "Note: Include ALib module BaseCamp in the ALib-Distribution to get nicer assertion output.\n";
    #else
        String256 acquirementInfo;
        acquirementInfo       << (CntAcquirements>0              ? "true " : "false") << " (" <<CntAcquirements<< ')';
        String256 sharedAcquirementInfo;
        sharedAcquirementInfo << (CntSharedAcquirements.load()>0 ? "true " : "false") << " (" <<CntSharedAcquirements.load()<< ')';

        FormatterPythonStyle fmt;
        fmt.Format( msg, ASSERTION_FORMAT_SHARED,
                    (type== 0 ? "Assertion" : "Warning"),      // 0
                    Name    , headline,                        // 1 2
                    ciAssert, ci,                              // 3 4
                    AcqCI   , RelCI, acquirementInfo,          // 5 6 7
                    SAcqCI  ,SRelCI, sharedAcquirementInfo);   // 8 9 10

    #endif
    #if ALIB_ALOX
     int oldMode= 1;
     if ( Log::DebugLogger )
     {
         oldMode= Log::DebugLogger->GetFormatMultiLine().Mode;
         Log::DebugLogger->GetFormatMultiLine().Mode= 3;
     }
    #endif

    ALIB_STRINGS_TO_NARROW(msg, nmsg, 4096)
    DbgSimpleALibMsg( ciAssert, type, "THREADS", nmsg  );

    #if ALIB_ALOX
     if ( Log::DebugLogger )
        Log::DebugLogger->GetFormatMultiLine().Mode= oldMode;
    #endif

 }

#if ALIB_CAMP && !DOXYGEN // otherwise doxygen will write a bad version of the initial value
NString DbgConditionAsserter::ASSERTION_FORMAT=
"Assertion failed in method TCondition::{2:sm}"    "\n"
"                 Message: {0}"        "\n"
"                Instance: {1}"        "\n"
                                       "\n"
"               Called By: {3:ya}"     "\n"
"                      At: {3:sf:sl}"  "\n"
"                  Thread: {3:ta}"     "\n"
                                       "\n"
"           Current Owner: {4}"        "\n"
"             #Of Waiters: {5}"        "\n"
"        Exclusive Waiter: {6}"        "\n"
                                       "\n"
"   Latest Acquisition By: {7:ya}"     "\n"
"                      At: {7:sf:sl}"  "\n"
"                  Thread: {7:ta}"     "\n"
"       Latest Release By: {8:ya}"     "\n"
"                      At: {8:sf:sl}"  "\n"
"                  Thread: {8:ta}"     "\n"
                                       "\n"
"          Latest Wait By: {9:ya}"     "\n"
"                      At: {9:sf:sl}"  "\n"
"                  Thread: {9:ta}"     "\n"
"        Latest Notify By: {10:ya}"    "\n"
"                      At: {10:sf:sl}" "\n"
"                  Thread: {10:ta}"    "\n"
;
#endif // ALIB_CAMP && !DOXYGEN

void DbgConditionAsserter::Assert( bool cond,  const CallerInfo& ciAssert, const CallerInfo& ci,
                                   const NString& headline )
{
    if (cond )
        return;
    String4K msg;

    #if !ALIB_CAMP
        msg << "Assertion failed in method TCondition::" << ciAssert.Func << "()\n"
               "                  Message: " << headline              << "\n"
               "                 Instance: " << Name                  << "(0x" << Format::Hex(reinterpret_cast<integer>(this)) << ")\n\n"
               "                   Caller: " << ci                    << "\n\n"
               "            Current Owner: " << Owner                 << "\n"
               "              #Of Waiters: " << CntWaiters.load()     << "\n"
               "         Exclusive Waiter: " << AssertExclusiveWaiter << "\n\n"
               "  Most Recent Acquirement: " << AcqCI                 << "\n"
               "      Most Recent Release: " << RelCI                 << "\n"
               "         Most Recent Wait: " << WaitCI                << "\n"
               "       Most Recent Notify: " << NotifyCI              << "\n"
     "Note: Include ALib module BaseCamp in the ALib-Distribution to get nicer assertion output.\n";

    #else
        NString128 name; name << Name << "(0x" << NFormat::Hex(reinterpret_cast<integer>(this)) << ')';
        FormatterPythonStyle fmt;
        fmt.Format( msg, ASSERTION_FORMAT,
                    headline, name,                                    // 0 1
                    ciAssert, ci,                                      // 2 3
                    Owner   , CntWaiters.load(),  AssertExclusiveWaiter,     // 4 5 6
                    AcqCI   , RelCI, WaitCI, NotifyCI              );  // 7 8 9 10
                    
    #endif
    #if ALIB_ALOX
     int oldMode= 1;
     if ( Log::DebugLogger )
     {
         oldMode= Log::DebugLogger->GetFormatMultiLine().Mode;
         Log::DebugLogger->GetFormatMultiLine().Mode= 3;
     }
    #endif

    ALIB_STRINGS_TO_NARROW(msg, nmsg, 4096)
    DbgSimpleALibMsg( ciAssert, 0, "THREADS", nmsg  );

    #if ALIB_ALOX
     if ( Log::DebugLogger )
        Log::DebugLogger->GetFormatMultiLine().Mode= oldMode;
    #endif

 }

#if ALIB_DEBUG_CRITICAL_SECTIONS
bool Lock              ::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool Lock              ::DCSIsSharedAcquired()    const { return Dbg.IsOwnedByCurrentThread();   }
bool TimedLock         ::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool TimedLock         ::DCSIsSharedAcquired()    const { return Dbg.IsOwnedByCurrentThread();   }
bool RecursiveLock     ::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool RecursiveLock     ::DCSIsSharedAcquired()    const { return Dbg.IsOwnedByCurrentThread();   }
bool RecursiveTimedLock::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool RecursiveTimedLock::DCSIsSharedAcquired()    const { return Dbg.IsOwnedByCurrentThread();   }
bool SharedLock        ::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool SharedTimedLock   ::DCSIsAcquired()          const { return Dbg.IsOwnedByCurrentThread();   }
bool SharedLock        ::DCSIsSharedAcquired()    const { return    Dbg.IsSharedOwnedByAnyThread()
                                                                 || Dbg.IsOwnedByCurrentThread();  }
bool SharedTimedLock   ::DCSIsSharedAcquired()    const { return    Dbg.IsSharedOwnedByAnyThread()
                                                                 || Dbg.IsOwnedByCurrentThread();  }

#endif

#endif  // ALIB_DEBUG


// #################################################################################################
// Globals
// #################################################################################################
/// This global mutex is acquired by \alib-types, whenever data is written to either
/// <c>std::cout</c> or <c>std::cerr</c>.
/// This is, for example, acquired by type \alib{lang;ReportWriterStdIO} and by loggers of
/// module \alib_alox that log to the console.
Lock STD_IOSTREAMS_LOCK;

// #################################################################################################
// DEBUG implementations
// #################################################################################################

// #################################################################################################
// Class Lock
// #################################################################################################
#if ALIB_DEBUG

void  Lock::Acquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
}

bool  Lock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;

    return true;
}

void Lock::Release( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Acquired by a different thread");

    Dbg.CntAcquirements--;
    Dbg.RelCI= ci;
    mutex.unlock();
}

#endif // ALIB_DEBUG

// #################################################################################################
// Class TimedLock
// #################################################################################################
#if !ALIB_DEBUG
bool  TimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration )
{
    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }
    return true;
}
#else

void  TimedLock::Acquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString256 msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
}

bool  TimedLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );

    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    return true;
}

bool  TimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration,
                                  const CallerInfo& ci )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );

    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    return true;
}
void TimedLock::Release( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Acquired by a different thread");

    Dbg.CntAcquirements--;
    Dbg.RelCI= ci;
    mutex.unlock();
}


#endif // ALIB_DEBUG


// #################################################################################################
// Class RecursiveLock
// #################################################################################################
#if ALIB_DEBUG
void RecursiveLock::AcquireRecursive( ALIB_DBG_TAKE_CI )
{
    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
                     NString256() << Dbg.CntAcquirements <<  " recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold"  );
}

bool  RecursiveLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
                     NString256() << Dbg.CntAcquirements <<  " recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold"  );
    return true;
}


void RecursiveLock::ReleaseRecursive(const CallerInfo& ci)
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Not owned");

    Dbg.RelCI = ci;
    if(--Dbg.CntAcquirements >= 0)
        mutex.unlock();
}
#endif // ALIB_DEBUG


// #################################################################################################
// Class RecursiveTimedLock
// #################################################################################################
#if !ALIB_DEBUG
bool  RecursiveTimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration )
{
    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }
    return true;
}
#else
void RecursiveTimedLock::AcquireRecursive( ALIB_DBG_TAKE_CI )
{
    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
                     NString256() << Dbg.CntAcquirements <<  " recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold"  );
}

bool  RecursiveTimedLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
                     NString256() << Dbg.CntAcquirements <<  " recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold"  );
    return true;
}

bool  RecursiveTimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration,
                                           const CallerInfo& ci )
{
    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
                     NString256() << Dbg.CntAcquirements <<  " recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold"  );
    return true;
}

void RecursiveTimedLock::ReleaseRecursive(const CallerInfo& ci)
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Not owned");

    Dbg.CntAcquirements--;
    Dbg.RelCI= ci;
    mutex.unlock();
}
#endif // ALIB_DEBUG


// #################################################################################################
// Class SharedLock (writer)
// #################################################################################################
#if ALIB_DEBUG
void  SharedLock::Acquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
}

bool  SharedLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    return true;
}

void SharedLock::Release(const CallerInfo& ci)
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Not owned");

    Dbg.CntAcquirements--;
    Dbg.RelCI= ci;
    mutex.unlock();
}

// #################################################################################################
// Class SharedLock (reader)
// #################################################################################################
void  SharedLock::AcquireShared( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci,
            "AcquireShared while already owning. (This is not allowed with std::shared_lock)" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_shared_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock_shared();

    if ( Dbg.CntSharedAcquirements.fetch_add(1) >= DbgWarningMaximumShared )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
            "Too many parallel shared acquisitions detected. "
            "A reason might be that shared acquirers do not call ReleaseShared" );

    Dbg.SAcqCI= ci;
}

bool SharedLock::TryAcquireShared(const CallerInfo& ci)
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci,
            "AcquireShared while already owning. (This is not allowed with std::shared_lock)" );

    if ( !mutex.try_lock_shared() )
        return false;


    if ( Dbg.CntSharedAcquirements.fetch_add(1) >= DbgWarningMaximumShared )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
            "Too many parallel shared acquisitions detected. "
            "A reason might be that shared acquirers do not call ReleaseShared" );

    Dbg.SAcqCI= ci;
    return true;
}

void SharedLock::ReleaseShared(const CallerInfo& ci)
{
    auto prevCounter= Dbg.CntSharedAcquirements.fetch_sub(1);
    if ( prevCounter <= 0 )
        Dbg.DoAssert( 0,  ALIB_CALLER, ci,
             "Too many invocations of ReleaseShared (from any thread) without prior acquisition" );

    Dbg.SRelCI= ci;
    mutex.unlock_shared();
}

#endif // ALIB_DEBUG


// #################################################################################################
// Class SharedTimedLock (writer)
// #################################################################################################
#if !ALIB_DEBUG
bool  SharedTimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration )
{
    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }
    return true;
}
#else
void  SharedTimedLock::Acquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
}
bool  SharedTimedLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    return true;
}

bool  SharedTimedLock::TryAcquireTimed( const Ticks::Duration& waitDuration,
                                        const CallerInfo& ci )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci, "Illegal nested acquisition" );

    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    return true;
}

void SharedTimedLock::Release(const CallerInfo& ci)
{
    Dbg.AssertOwned ( ALIB_CALLER, ci, "Not acquired" );
    Dbg.AssertOwning( ALIB_CALLER, ci, "Not owned");

    Dbg.CntAcquirements--;
    Dbg.RelCI= ci;
    mutex.unlock();
}
#endif

// #################################################################################################
// Class SharedTimedLock (reader)
// #################################################################################################
#if !ALIB_DEBUG
bool  SharedTimedLock::TryAcquireSharedTimed( const Ticks::Duration& waitDuration )
{
    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_shared_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }
    return true;
}

#else

void  SharedTimedLock::AcquireShared( ALIB_DBG_TAKE_CI )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci,
            "AcquireShared while already owning. (This is not allowed with std::shared_lock)" );

    if ( !Dbg.WaitTimeLimit.IsZero() )
    {
        Ticks::Duration waitDuration=  Dbg.WaitTimeLimit;
        Ticks overallTimer;
        Ticks waitTimer;
        while (!mutex.try_lock_shared_for( (waitDuration - waitTimer.Age()).Export() ) )
        {
            if ( waitTimer.Age() < waitDuration )
                continue; // spurious wakeup

            NString1K msg;
            msg << "Waiting to acquire a lock since "
            #if ALIB_CAMP
              <<  overallTimer.Age();
            #else
              << Dbg.WaitTimeLimit.InAbsoluteMilliseconds() << "ms";
            #endif
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);

            waitTimer.Reset();
        }
    }
    else
        mutex.lock_shared();

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );

    if ( Dbg.CntSharedAcquirements.fetch_add(1) >= DbgWarningMaximumShared )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
            "Too many parallel shared acquisitions detected. "
            "A reason might be that shared acquirers do not call ReleaseShared" );

    Dbg.SAcqCI= ci;
}

bool SharedTimedLock::TryAcquireShared(const CallerInfo& ci)
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci,
            "AcquireShared while already owning. (This is not allowed with std::shared_lock)" );

    if ( !mutex.try_lock_shared() )
        return false;

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );

    if ( Dbg.CntSharedAcquirements.fetch_add(1) >= DbgWarningMaximumShared )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
            "Too many parallel shared acquisitions detected. "
            "A reason might be that shared acquirers do not call ReleaseShared" );

    Dbg.SAcqCI= ci;
    return true;
}

bool  SharedTimedLock::TryAcquireSharedTimed( const Ticks::Duration& waitDuration,
                                              const CallerInfo& ci )
{
    Dbg.AssertNotOwning( ALIB_CALLER, ci,
            "AcquireShared while already owning. (This is not allowed with std::shared_lock)" );

    Ticks::Duration remainingDuration= waitDuration;
    Ticks timer;
    while (!mutex.try_lock_shared_for( remainingDuration.Export() ) )
    {
        remainingDuration= waitDuration - timer.Age();
        if ( remainingDuration.IsPositive() )
            continue; // spurious wakeup
        return false;
    }

    Dbg.AssertNotOwned( ALIB_CALLER, ci, "Still owned after locking" );

    if ( Dbg.CntSharedAcquirements.fetch_add(1) >= DbgWarningMaximumShared )
        Dbg.DoAssert( 1, ALIB_CALLER, ci,
            "Too many parallel shared acquisitions detected. "
            "A reason might be that shared acquirers do not call ReleaseShared" );

    Dbg.SAcqCI= ci;
    return true;
}

void SharedTimedLock::ReleaseShared(const CallerInfo& ci)
{
    auto prevCounter= Dbg.CntSharedAcquirements.fetch_sub(1);
    if ( prevCounter <= 0 )
        Dbg.DoAssert( 0,  ALIB_CALLER, ci,
             "Too many invocations of ReleaseShared (from any thread) without prior acquisition" );

    Dbg.SRelCI= ci;
    mutex.unlock_shared();
}

#endif // ALIB_DEBUG

}} // namespace [alib::threads]

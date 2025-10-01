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
#if ALIB_DEBUG && !ALIB_STRINGS
#   include <format>
#endif
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module   ALib.Threads;
    import   ALib.Lang;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#else
#   include "ALib.Threads.H"
#   include "ALib.Strings.H"
#   include "ALib.Lang.H"
#endif
// ======================================   Implementation   =======================================
#if !ALIB_SINGLE_THREADED

namespace alib {  namespace threads {

// #################################################################################################
// Globals
// #################################################################################################
/// This global mutex is acquired by \alib-types, whenever data is written to either
/// <c>std::cout</c> or <c>std::cerr</c>.
/// This is, for example, acquired by function #alib::assert::Raise and by loggers of
/// module \alib_alox
/// that log to the console.
Lock STD_IOSTREAMS_LOCK;

// #################################################################################################
// Debug-Versions of Lock-Types
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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 ) {
            #if ALIB_STRINGS
            NAString msg; msg << Dbg.CntAcquirements <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg; msg+= std::format("{}", Dbg.CntAcquirements);
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
        }
}

bool  RecursiveLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 ) {
            #if ALIB_STRINGS
            NAString msg; msg << Dbg.CntAcquirements <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg; msg+= std::format("{}", Dbg.CntAcquirements);
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
        }
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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

            waitTimer.Reset();
        }
    }
    else
        mutex.lock();

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 ) {
            #if ALIB_STRINGS
            NAString msg; msg << Dbg.CntAcquirements <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg; msg+= std::format("{}", Dbg.CntAcquirements);
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
        }
}

bool  RecursiveTimedLock::TryAcquire( ALIB_DBG_TAKE_CI )
{
    if (!mutex.try_lock() )
        return false;

    Dbg.AssertNotOwnedOrMe( ALIB_CALLER, ci, "Still owned after locking" );
    Dbg.CntAcquirements++;
    Dbg.AcqCI= ci;
    if(     Dbg.RecursionLimit                         != 0
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 ) {
            #if ALIB_STRINGS
            NAString msg; msg << Dbg.CntAcquirements <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg; msg+= std::format("{}", Dbg.CntAcquirements);
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
        }
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
        && (Dbg.CntAcquirements % Dbg.RecursionLimit)  == 0 ) {
            #if ALIB_STRINGS
            NAString msg; msg << Dbg.CntAcquirements <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg; msg+= std::format("{}", Dbg.CntAcquirements);
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif
        }
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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

            #if ALIB_STRINGS
            NAString msg("Waiting to acquire a lock since "); msg << overallTimer.Age();
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg);
            #else
            std::string msg("Waiting to acquire a lock since ");
                        msg+= std::format("{}", overallTimer.Age().InAbsoluteMilliseconds());
                        msg+= " ms";
            Dbg.DoAssert( 1, ALIB_CALLER, ci, msg.c_str());
            #endif

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

#if ALIB_DEBUG_CRITICAL_SECTIONS
bool Lock              ::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool Lock              ::DCSIsSharedAcquired()    const { return    Dbg.IsOwnedByCurrentThread();  }
bool TimedLock         ::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool TimedLock         ::DCSIsSharedAcquired()    const { return    Dbg.IsOwnedByCurrentThread();  }
bool RecursiveLock     ::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool RecursiveLock     ::DCSIsSharedAcquired()    const { return    Dbg.IsOwnedByCurrentThread();  }
bool RecursiveTimedLock::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool RecursiveTimedLock::DCSIsSharedAcquired()    const { return    Dbg.IsOwnedByCurrentThread();  }
bool SharedLock        ::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool SharedLock        ::DCSIsSharedAcquired()    const { return    Dbg.IsSharedOwnedByAnyThread()
                                                                 || Dbg.IsOwnedByCurrentThread();  }
bool SharedTimedLock   ::DCSIsAcquired()          const { return    Dbg.IsOwnedByCurrentThread();  }
bool SharedTimedLock   ::DCSIsSharedAcquired()    const { return    Dbg.IsSharedOwnedByAnyThread()
                                                                 || Dbg.IsOwnedByCurrentThread();  }
#endif // ALIB_DEBUG_CRITICAL_SECTIONS

}} // namespace [alib::threads]

#endif // !ALIB_SINGLE_THREADED

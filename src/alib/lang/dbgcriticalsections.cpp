//==================================================================================================
/// \file
/// This header file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#include "alib/alib_precompile.hpp"
#include "alib/alib.hpp"

#if ALIB_DEBUG_CRITICAL_SECTIONS
#include "alib/lang/dbgcriticalsections.hpp"
#include "alib/threads/thread.hpp"
#include "alib/threads/lock.hpp"
#include "alib/threads/timedlock.hpp"
#include "alib/threads/recursivelock.hpp"
#include "alib/threads/recursivetimedlock.hpp"
#include "alib/threads/sharedlock.hpp"
#include "alib/threads/sharedtimedlock.hpp"
#include "alib/strings/localstring.hpp"

#if ALIB_CAMP
#   include "alib/lang/format/formatterpythonstyle.hpp"
#endif

#if ALIB_ALOX
#   include "alib/alox.hpp"
#   include "alib/alox/textlogger/textlogger.hpp"
#endif

namespace alib::lang {


#if ALIB_CAMP && !DOXYGEN // otherwise doxygen will write a bad version of the initial value
const char* DbgCriticalSections::ASSERTION_FORMAT=
"Assertion in Critical Section {!Q}"         "\n"
"                       Message: {}"         "\n"
"          In (Member-)Function: {2:ya}"     "\n"
"                      Is Owned: {6}"        "\n"
"               Is Shared Owned: {9}"        "\n"
                                             "\n"
"                     Called By: {3:ya}"     "\n"
"                            At: {3:sf:sl}"  "\n"
"                        Thread: {3:ta}"     "\n"
                                             "\n"
"         Latest Acquisition By: {4:ya}"     "\n"
"                            At: {4:sf:sl}"  "\n"
"                        Thread: {4:ta}"     "\n"
"             Latest Release By: {5:ya}"     "\n"
"                            At: {5:sf:sl}"  "\n"
"                        Thread: {5:ta}"     "\n"
                                             "\n"
"  Latest Shared Acquisition By: {7:ya}"     "\n"
"                            At: {7:sf:sl}"  "\n"
"                        Thread: {7:ta}"     "\n"
"       Latest SharedRelease By: {8:ya}"     "\n"
"                            At: {8:sf:sl}"  "\n"
"                        Thread: {8:ta}"     "\n"
;
#endif // ALIB_CAMP && !DOXYGEN

void DbgCriticalSections::doAssert( bool cond, const CallerInfo& ciAssert, const CallerInfo& ci,
                                    const char* headline )    const
{
    if (cond) return;

    String4K msg;
    #if !ALIB_CAMP
    msg  << "Assertion in Critical Section " << DCSName << "\n"
            "                    Message: "  << headline  << "\n"
            "                     Caller: "  << ci        << "\n"
            "                   Is Owned: "  << (DCSWriterCnt.load()>0 ? "true " : "false") << " (" << DCSWriterCnt.load() << ")\n"
            "            Is Shared-Owned: "  << (DCSReaderCnt.load()>0 ? "true " : "false") << " (" << DCSReaderCnt.load() << ")\n\n"
            "         Latest Acquirement: "  << DCSAcq    << "\n"
            "             Latest Release: "  << DCSRel    << "\n\n"
            "  Latest Shared Acquirement: "  << DCSSAcq   << "\n"
            "      Latest Shared Release: "  << DCSSRel   << "\n\n"
     "Note: Include ALib module BaseCamp in the ALib-Distribution to get nicer assertion output.\n";
    #else
        String256 acquirementInfo;
        acquirementInfo       << (DCSWriterCnt.load()>0 ? "true " : "false") << " (" <<DCSWriterCnt.load()<< ')';
        String256 sharedAcquirementInfo;
        sharedAcquirementInfo << (DCSReaderCnt.load()>0 ? "true " : "false") << " (" <<DCSReaderCnt.load()<< ')';

        FormatterPythonStyle fmt;
        fmt.Format( msg, ASSERTION_FORMAT,
                    DCSName, headline,
                    ciAssert, ci,
                    DCSAcq  , DCSRel , acquirementInfo,
                    DCSSAcq , DCSSRel, sharedAcquirementInfo);

    #endif
    #if ALIB_ALOX
     int oldMode= 1;
     if ( Log::DebugLogger )
     {
         oldMode= Log::DebugLogger->GetFormatMultiLine().Mode;
         Log::DebugLogger->GetFormatMultiLine().Mode= 3;
     }
    #endif

    ALIB_STRINGS_TO_NARROW(msg, nmsg, 16*1024) // needed due to MB_CUR_MAX in conversion
    DbgSimpleALibMsg( ciAssert, 0, "THREADS", nmsg  );

    #if ALIB_ALOX
     if ( Log::DebugLogger )
        Log::DebugLogger->GetFormatMultiLine().Mode= oldMode;
    #endif
}

void DbgCriticalSections::Acquire( const CallerInfo& ci )                                   const
{
    doAssert( !DCSLock || DCSLock->DCSIsAcquired(), ALIB_CALLER, ci, "Acquire: Associated Lock not acquired." );
    doAssert(   DCSWriterCnt.load()  == 0
             || DCSAcq.ThreadID == std::this_thread::get_id(), ALIB_CALLER, ci, "Acquired by other thread.");
    doAssert( DCSReaderCnt.load() == 0, ALIB_CALLER, ci, "Acquired by reader.");

    DCSWriterCnt.fetch_add(1);
    DCSAcq= ci;
    yieldOrSleep();
}

void  DbgCriticalSections::Release( const CallerInfo& ci )                                  const
{
    doAssert(!DCSLock || DCSLock->DCSIsAcquired()         , ALIB_CALLER, ci, "Release: Associated lock not acquired." );
    doAssert(DCSAcq.ThreadID == std::this_thread::get_id(), ALIB_CALLER, ci, "Release: Acquired by other thread.");
    yieldOrSleep();
    DCSRel= ci;
    DCSWriterCnt.fetch_sub(1);
}

void DbgCriticalSections::AcquireShared( const CallerInfo& ci )      const
{
    doAssert( !DCSLock || DCSLock->DCSIsSharedAcquired(), ALIB_CALLER, ci,
        "AcquireShared: Associated lock not shared-acquired." );

    yieldOrSleep();
    int wCnt    = DCSWriterCnt.load();
    doAssert(wCnt <= 0 || DCSAcq.ThreadID == std::this_thread::get_id(), ALIB_CALLER, ci,
        "AcquireShared: Acquired by different thread.");
    DCSReaderCnt.fetch_add(1);
    DCSSAcq= ci;
}

void DbgCriticalSections::ReleaseShared( const CallerInfo& ci )      const
{
    yieldOrSleep();
    doAssert( !DCSLock || DCSLock->DCSIsSharedAcquired(), ALIB_CALLER, ci,
        "ReleaseShared: Associated lock not shared-acquired." );

    doAssert(DCSWriterCnt.load() <= 0 || DCSAcq.ThreadID == std::this_thread::get_id(), ALIB_CALLER, ci,
         "ReleaseShared: Internal error. Acquired by different thread while shared release.");
    int prevRCnt= DCSReaderCnt.fetch_sub(1);
    doAssert(prevRCnt > 0, ALIB_CALLER, ci, "ReleaseShared: No shared acquirement.");
    DCSSRel= ci;
}
    
} // namespace [alib::lang]

#endif // HPP_ALIB_LANG_DBGCRITICALSECTIONS


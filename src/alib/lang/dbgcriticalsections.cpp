//==================================================================================================
/// \file
/// This header-file is part of module \alib_threads of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
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
    module ALib.Lang;
#else
#   include "ALib.Lang.H"
#endif
// ======================================   Implementation   =======================================

#if ALIB_DEBUG_CRITICAL_SECTIONS
namespace alib::lang {

unsigned int DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS;


const char* DbgCriticalSections::ASSERTION_FORMAT=
"Assertion in Critical Section \"{}\""       "\n"
"                       Message: {}"         "\n"
"          In (Member-)Function: {}"         "\n"
"                      Is Owned: {} ({})"    "\n"
"               Is Shared Owned: {} ({})"    "\n"
                                             "\n"
"                     Called By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                        Thread: {}"         "\n"
                                             "\n"
"         Latest Acquisition By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                        Thread: {}"         "\n"
"             Latest Release By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                        Thread: {}"         "\n"
                                             "\n"
"  Latest Shared Acquisition By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                        Thread: {}"         "\n"
"       Latest SharedRelease By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                        Thread: {}"         "\n"
;

void DbgCriticalSections::doAssert( bool cond, const CallerInfo& ciAssert, const CallerInfo& ci,
                                    const char* headline )    const
{
    if (cond) return;

    assert::Raise( ciAssert, 0, "THREADS", ASSERTION_FORMAT,
                   DCSName, headline, ciAssert.Func,
                   DCSWriterCnt.load() > 0, DCSWriterCnt.load(),
                   DCSReaderCnt.load() > 0, DCSReaderCnt.load(),
                       ci.TypeInfo,     ci.Func,     ci.File,     ci.Line,     ci.ThreadID,
                   DCSAcq.TypeInfo, DCSAcq.Func, DCSAcq.File, DCSAcq.Line, DCSAcq.ThreadID,
                   DCSRel.TypeInfo, DCSRel.Func, DCSRel.File, DCSRel.Line, DCSRel.ThreadID,
                  DCSSAcq.TypeInfo,DCSSAcq.Func,DCSSAcq.File,DCSSAcq.Line,DCSSAcq.ThreadID,
                  DCSSRel.TypeInfo,DCSSRel.Func,DCSSRel.File,DCSSRel.Line,DCSSRel.ThreadID   );
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

#endif // ALIB_DEBUG_CRITICAL_SECTIONS


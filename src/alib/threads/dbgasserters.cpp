//##################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//##################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
//========================================= Global Fragment ========================================
#include "alib/alib.inl"
//============================================== Module ============================================
#if ALIB_C20_MODULES
    module ALib.Threads;
    import ALib.Lang;
#else
#   include "ALib.Lang.H"
#   include "ALib.Threads.H"
#   include "ALib.Strings.H"
#endif
//========================================== Implementation ========================================
#if !ALIB_SINGLE_THREADED && ALIB_DEBUG

namespace alib::threads {

//##################################################################################################
// Class DbgLockAsserter
//##################################################################################################
Thread*    DbgLockAsserter::GetOwner()                                                       const {
    if ( CntAcquirements.load() == 0 )
        return nullptr;
    return Thread::Get(AcqCI.ThreadID);
}

const char* DbgLockAsserter::ASSERTION_FORMAT=
"Multi-Threading {} in Lock \"{}\""   "\n"
"                Message: {}"         "\n"
"   In (Member-)Function: {}"         "\n"
"               Is Owned: {} ({})"    "\n"
                                      "\n"
"              Called By: {}::{}"     "\n"
"                     At: {}:{}"      "\n"
"                 Thread: {}"         "\n"
                                      "\n"
"  Latest Acquisition By: {}::{}"     "\n"
"                     At: {}:{}"      "\n"
"                 Thread: {}"         "\n"
"      Latest Release By: {}:{}"      "\n"
"                     At: {}:{}"      "\n"
"                 Thread: {}"         "\n"
;

const char* DbgSharedLockAsserter::ASSERTION_FORMAT_SHARED=
"Multi-Threading {} in Shared-Lock \"{}\""   "\n"
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


void DbgLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                const char* headline ) {
    assert::Raise( ciAssert, type, "THREADS", ASSERTION_FORMAT,
                   (type== 0 ? "Assertion" : "Warning"),             // 0
                   Name    , headline,                               // 1 2
                   ciAssert.Func,                                    // 3
                   (CntAcquirements.load() > 0 ? "true" : "false"),  // 4
                   CntAcquirements .load(),                          // 5

                      ci.TypeInfo,    ci.Func,    ci.File,    ci.Line,    ci.ThreadID,
                   AcqCI.TypeInfo, AcqCI.Func, AcqCI.File, AcqCI.Line, AcqCI.ThreadID,
                   RelCI.TypeInfo, RelCI.Func, RelCI.File, RelCI.Line, RelCI.ThreadID  );
 }

void DbgSharedLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                      const char* headline ) {
    assert::Raise( ciAssert, type, "THREADS", ASSERTION_FORMAT_SHARED,
                   (type== 0 ? "Assertion" : "Warning"),                          // 0
                   Name    , headline,                                            // 1 2
                   ciAssert.Func,                                                 // 3
                   CntAcquirements      .load() >0,       CntAcquirements.load(), // 4, 5
                   CntSharedAcquirements.load() >0, CntSharedAcquirements.load(), // 6, 7

                       ci.TypeInfo,     ci.Func,     ci.File,     ci.Line,     ci.ThreadID,
                    AcqCI.TypeInfo,  AcqCI.Func,  AcqCI.File,  AcqCI.Line,  AcqCI.ThreadID,
                    RelCI.TypeInfo,  RelCI.Func,  RelCI.File,  RelCI.Line,  RelCI.ThreadID,
                   SAcqCI.TypeInfo, SAcqCI.Func, SAcqCI.File, SAcqCI.Line, SAcqCI.ThreadID,
                   SRelCI.TypeInfo, SRelCI.Func, SRelCI.File, SRelCI.Line, SRelCI.ThreadID
                );
}

const char* DbgConditionAsserter::ASSERTION_FORMAT=
"Assertion failed in method TCondition::{}"    "\n"
"                 Message: {}"                 "\n"
"                Instance: {}"                 "\n"
                                               "\n"
"               Called By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"                  Thread: {}"                 "\n"
                                               "\n"
"           Current Owner: {}"                 "\n"
"             #Of Waiters: {}"                 "\n"
"        Exclusive Waiter: {}"                 "\n"
                                               "\n"
"   Latest Acquisition By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"                  Thread: {}"                 "\n"
"       Latest Release By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"                  Thread: {}"                 "\n"
                                               "\n"
"          Latest Wait By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"                  Thread: {}"                 "\n"
"        Latest Notify By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"                  Thread: {}"                 "\n"
;

void DbgConditionAsserter::Assert( bool cond,  const CallerInfo& ciAssert, const CallerInfo& ci,
                                   const char* headline ) {
    if (cond)
        return;
    assert::Raise( ciAssert, 0, "THREADS", ASSERTION_FORMAT,
                   ciAssert.Func, headline, Name,
                         ci.TypeInfo,       ci.Func,       ci.File,       ci.Line,       ci.ThreadID,
                   Owner, CntWaiters.load(), AssertExclusiveWaiter,

                      AcqCI.TypeInfo,    AcqCI.Func,    AcqCI.File,    AcqCI.Line,    AcqCI.ThreadID,
                      RelCI.TypeInfo,    RelCI.Func,    RelCI.File,    RelCI.Line,    RelCI.ThreadID,
                     WaitCI.TypeInfo,   WaitCI.Func,   WaitCI.File,   WaitCI.Line,   WaitCI.ThreadID,
                   NotifyCI.TypeInfo, NotifyCI.Func, NotifyCI.File, NotifyCI.Line, NotifyCI.ThreadID  );

}


void DbgLockAsserter::SetRecursiveOwner (const CallerInfo& assertCI, const CallerInfo& requestCI ) {
    if( CntAcquirements.load() > 0 && requestCI.ThreadID != AcqCI.ThreadID )
        DoAssert( 0, assertCI, requestCI, "Already (still) owned." );
    AcqCI= requestCI;
    CntAcquirements.fetch_add(1);

    if(     RecursionLimit                     != 0
        && (CntAcquirements.load() % RecursionLimit)  == 0 ) {
        #if ALIB_STRINGS
            NAString msg; msg << CntAcquirements.load() <<  "  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            DoAssert( 1, ALIB_CALLER, requestCI, msg);
        #else
            std::string msg; msg+= std::format("{}", CntAcquirements.load());
                        msg+="  recursive acquisitions."
              " Warning limit can be adopted with field DbgRecursionWarningThreshold";
            DoAssert( 1, ALIB_CALLER, requestCI, msg.c_str());
        #endif
    }

}

} // namespace [alib::threads]

#endif // !ALIB_SINGLE_THREADED && ALIB_DEBUG

#if !ALIB_SINGLE_THREADED && ALIB_DEBUG

namespace alib::threads {

//##################################################################################################
// Class DbgLockAsserter
//##################################################################################################
Thread*    DbgLockAsserter::GetOwner()                                                       const {
    if ( CntAcquirements.load() == 0 )
        return nullptr;
    return Thread::Get(Acq.CI.ThreadID);
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
"           Seq / Thread: {} / {}"    "\n"
"      Latest Release By: {}:{}"      "\n"
"                     At: {}:{}"      "\n"
"           Seq / Thread: {} / {}"    "\n"
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
"                  Seq / Thread: {} / {}"    "\n"
"             Latest Release By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                  Seq / Thread: {} / {}"    "\n"
                                             "\n"
"  Latest Shared Acquisition By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                  Seq / Thread: {} / {}"    "\n"
"       Latest SharedRelease By: {}::{}"     "\n"
"                            At: {}:{}"      "\n"
"                 Seq / Thread: {} / {}"    "\n"
;


void DbgLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                const char* headline ) {
    assert::Raise( ciAssert, type, "THREADS", ASSERTION_FORMAT,
                   (type== 0 ? "Assertion" : "Warning"),             // 0
                   Name    , headline,                               // 1 2
                   ciAssert.Func,                                    // 3
                   (CntAcquirements.load() > 0 ? "true" : "false"),  // 4
                   CntAcquirements .load(),                          // 5

                       ci.TypeInfo,     ci.Func,     ci.File,     ci.Line,                   ci.ThreadID,
                   Acq.CI.TypeInfo, Acq.CI.Func, Acq.CI.File, Acq.CI.Line, Acq.ActionNo, Acq.CI.ThreadID,
                   Rel.CI.TypeInfo, Rel.CI.Func, Rel.CI.File, Rel.CI.Line, Rel.ActionNo, Rel.CI.ThreadID  );
 }

void DbgSharedLockAsserter::DoAssert( int type, const CallerInfo& ciAssert, const CallerInfo& ci,
                                      const char* headline ) {
    assert::Raise( ciAssert, type, "THREADS", ASSERTION_FORMAT_SHARED,
                   (type== 0 ? "Assertion" : "Warning"),                          // 0
                   Name    , headline,                                            // 1 2
                   ciAssert.Func,                                                 // 3
                   CntAcquirements      .load() >0,       CntAcquirements.load(), // 4, 5
                   CntSharedAcquirements.load() >0, CntSharedAcquirements.load(), // 6, 7
                                                                                            
                        ci.TypeInfo,      ci.Func,      ci.File,      ci.Line,                     ci.ThreadID,
                    Acq.CI.TypeInfo,  Acq.CI.Func,  Acq.CI.File,  Acq.CI.Line,  Acq.ActionNo,  Acq.CI.ThreadID,
                    Rel.CI.TypeInfo,  Rel.CI.Func,  Rel.CI.File,  Rel.CI.Line,  Rel.ActionNo,  Rel.CI.ThreadID,
                   SAcq.CI.TypeInfo, SAcq.CI.Func, SAcq.CI.File, SAcq.CI.Line, SAcq.ActionNo, SAcq.CI.ThreadID,
                   SRel.CI.TypeInfo, SRel.CI.Func, SRel.CI.File, SRel.CI.Line, SRel.ActionNo, SRel.CI.ThreadID
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
"            Seq / Thread: {} / {}"            "\n"
"       Latest Release By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"            Seq / Thread: {} / {}"            "\n"
                                               "\n"
"          Latest Wait By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"            Seq / Thread: {} / {}"            "\n"
"        Latest Notify By: {}::{}"             "\n"
"                      At: {}:{}"              "\n"
"            Seq / Thread: {} / {}"            "\n"
;

void DbgConditionAsserter::Assert( bool cond,  const CallerInfo& ciAssert, const CallerInfo& ci,
                                   const char* headline ) {
    if (cond)
        return;
    assert::Raise( ciAssert, 0, "THREADS", ASSERTION_FORMAT,
        ciAssert.Func, headline, Name,
              ci.TypeInfo,       ci.Func,       ci.File,       ci.Line,       ci.ThreadID,
        Owner, CntWaiters.load(), AssertExclusiveWaiter,

           Acq.CI.TypeInfo,    Acq.CI.Func,    Acq.CI.File,    Acq.CI.Line,    Acq.ActionNo,    Acq.CI.ThreadID,
           Rel.CI.TypeInfo,    Rel.CI.Func,    Rel.CI.File,    Rel.CI.Line,    Rel.ActionNo,    Rel.CI.ThreadID,
          Wait.CI.TypeInfo,   Wait.CI.Func,   Wait.CI.File,   Wait.CI.Line,   Wait.ActionNo,   Wait.CI.ThreadID,
        Notify.CI.TypeInfo, Notify.CI.Func, Notify.CI.File, Notify.CI.Line, Notify.ActionNo, Notify.CI.ThreadID  );

}


void DbgLockAsserter::SetRecursiveOwner (const CallerInfo& assertCI, const CallerInfo& requestCI ) {
    if( CntAcquirements.load() > 0 && requestCI.ThreadID != Acq.CI.ThreadID )
        DoAssert( 0, assertCI, requestCI, "Already (still) owned." );
    Acq.CI= requestCI;
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

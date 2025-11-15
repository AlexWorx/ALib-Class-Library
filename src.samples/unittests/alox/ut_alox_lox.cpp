// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_ALOX

#include "ALib.ALox.H"
#include "ALib.Time.H"
#include "ALib.EnumOps.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Strings.H"
#include "ALib.Variables.H"
#include "ALib.Camp.H"
#include "ALib.Lang.CIMethods.H"

#if !ALIB_SINGLE_THREADED
#   include   "ALib.Threads.H"
#endif

using namespace std;
using namespace alib;

namespace ut_alox {

#include "ALib.Lang.CIFunctions.H"
// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest6();
void ScopeInfoCacheTest1();
void ScopeInfoCacheTest6() { Log_Info("Test Method 6") }
void ScopeInfoCacheTest1() { Log_Info("Test Method 1") }
extern void ScopeInfoCacheTest2();
extern void ScopeInfoCacheTest3();
extern void ScopeInfoCacheTest4();
extern void ScopeInfoCacheTest5();
extern void ScopeInfoCacheTest6();
extern void ScopeInfoCacheTest7();

#if !ALIB_SINGLE_THREADED
    class TThread : public Thread
    {
        public: TThread( const character* pname, int pcntLoops, int psleepMicros )
                : Thread( pname )
                {
                    this->cntLoops=     pcntLoops;
                    this->sleepMicros=  psleepMicros;
                }
        int cntLoops;
        int sleepMicros;

        virtual void Run()                                                                  override
        {
            Log_SetDomain( "TEST/THREAD2", Scope::Method )

            for ( int i= 0 ; i < cntLoops ; ++i )
            {
                Log_Info( String64( "This is a " ) << name << ". Cnt= " << i )
                Thread::SleepMillis( 3 );
            }
        }
    };
#endif

}

#define TESTCLASSNAME       CPP_ALox_Lox
#include "aworx_unittests.hpp"
using namespace ut_alox;
using namespace alib::system;

namespace ut_aworx {

#include "ALib.Lang.CIFunctions.H"
void check_MemLogStartsWith( const alib::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog );
void check_MemLogStartsWith( const alib::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog= true )
{
    if (doLog)
    {
        Log_Info("")
    }

    UT_PRINT( String256("MemLog result: <<<") << memlog.MemoryLog << ">>> expected: " << exp)

    if( DIRECTORY_SEPARATOR == '/' )
    {
        if( !memlog.MemoryLog.StartsWith<CHK,lang::Case::Ignore>( exp ) )
        {
            UT_PRINT( "Expected start: {} Given: {}", exp, memlog.MemoryLog )
            UT_TRUE( false )
        }
    }
    else
    {
        String256 expCorrected( exp );
        expCorrected.SearchAndReplace( '/', '\\'  );
        if( !memlog.MemoryLog.StartsWith<CHK,lang::Case::Ignore>( expCorrected ) )
        {
            UT_PRINT( "Expected start: {} Given: {}", expCorrected, memlog.MemoryLog )
            UT_TRUE( false )
        }
    }

    memlog.MemoryLog.Reset();
}

void check_MemLogContains( const alib::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog );
void check_MemLogContains( const alib::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog= true )
{
    if (doLog)
    {
        Log_Info("")
    }
    if( DIRECTORY_SEPARATOR == '/' )
    {
        UT_TRUE( memlog.MemoryLog.IndexOf<CHK ALIB_COMMA lang::Case::Ignore>( exp, 0 ) >=0 )
    }
    else
    {
        String256 expCorrected( exp );
        expCorrected.SearchAndReplace( '/', '\\'  );
        UT_TRUE( memlog.MemoryLog.IndexOf<CHK ALIB_COMMA lang::Case::Ignore>( expCorrected, 0 ) >=0 )
    }

    memlog.MemoryLog.Reset();
}


/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/
#include "aworx_callerinfo_ut.hpp"

UT_CLASS

/** ********************************************************************************************
 * AddLogger
 **********************************************************************************************/
#if ALOX_DBG_LOG
UT_METHOD(AddLogger)
{
    UT_INIT()

    // twice
    {
        ConsoleLogger con;
        Log_SetVerbosity( &con,      Verbosity::Verbose, Lox::InternalDomains )
        MemoryLogger checkCnt;
        Log_SetVerbosity( &checkCnt, Verbosity::Warning, Lox::InternalDomains )

        MemoryLogger mem1( "MEM" );
        MemoryLogger mem2( "MEM" );
        int checkVal=0;
        Log_SetVerbosity( &mem1,     Verbosity::Info ) UT_EQ( checkVal+= 0, checkCnt.CntLogs )
        Log_SetVerbosity( &mem2,     Verbosity::Info ) UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_SetVerbosity( "XYZ",     Verbosity::Info ) UT_EQ( checkVal+= 1, checkCnt.CntLogs )

        // get unknown
        Log_GetLogger( result, "XYZ" )                 UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_Prune( (void) result; )

        Log_RemoveLogger( &mem2     )                  UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_RemoveLogger( &mem1     )                  UT_EQ( checkVal+= 0, checkCnt.CntLogs )
        Log_RemoveLogger( &mem1     )                  UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_RemoveLogger( "XYZ"     )                  UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_RemoveLogger( &con      )                  UT_EQ( checkVal+= 0, checkCnt.CntLogs )
        Log_RemoveLogger( &con      )                  UT_EQ( checkVal+= 1, checkCnt.CntLogs )
        Log_RemoveLogger( &checkCnt )                  UT_EQ( checkVal+= 0, checkCnt.CntLogs )
    }
}
#endif

/** ********************************************************************************************
 * Log_LogLevelSetting
 **********************************************************************************************/
#if ALOX_DBG_LOG && ALOX_DBG_LOG_CI
UT_METHOD(Log_LogLevelSetting)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_SetDomain( "TLLS_DF", Scope::Method )

    // Test Verbosity setting
    integer  logLinesBefore= Log::DebugLogger->CntLogs;

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "" )
    Log_Verbose  (          "This Verbose line should be logged" )
    Log_Info     (          "This Info    line should be logged" )
    Log_Warning  (          "This WARN    line should be logged" )
    Log_Error    (          "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Info, "" )
    Log_Verbose  (          "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     (          "This Info    line should be logged" )
    Log_Warning  (          "This Warning line should be logged" )
    Log_Error    (          "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "" )
    Log_Verbose  (          "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     (          "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  (          "This Warning line should be logged" )
    Log_Error    (          "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Error, "" )
    Log_Verbose  (          "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     (          "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  (          "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Error    (          "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Off, "" )
    Log_Verbose  (          "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     (          "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  (          "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Error    (          "This Error   line should NOT be logged. !!!!!Test Error!!!!!" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "/TLLS" )
    Log_Verbose  ( "/TLLS", "This Verbose line should be logged" )
    Log_Info     ( "/TLLS", "This Info    line should be logged" )
    Log_Warning  ( "/TLLS", "This WARN    line should be logged" )
    Log_Error    ( "/TLLS", "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Info, "/TLLS" )
    Log_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     ( "/TLLS", "This Info    line should be logged" )
    Log_Warning  ( "/TLLS", "This Warning line should be logged" )
    Log_Error    ( "/TLLS", "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "/TLLS" )
    Log_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  ( "/TLLS", "This Warning line should be logged" )
    Log_Error    ( "/TLLS", "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Error, "/TLLS" )
    Log_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  ( "/TLLS", "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Error    ( "/TLLS", "This Error   line should be logged" )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Off, "/TLLS" )
    Log_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Warning  ( "/TLLS", "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )
    Log_Error    ( "/TLLS", "This Error   line should NOT be logged. !!!!!Test Error!!!!!" )

    UT_EQ( 20, Log::DebugLogger->CntLogs - logLinesBefore )
}
#endif

/** ********************************************************************************************
 * Log_SimpleScopeDomain
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
UT_METHOD(Log_SimpleScopeDomain)
{
    UT_INIT()

    integer  cntLL;
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )
    Log_Error("No domain and nothing set")

    Log_SetDomain( "REPLACED",  Scope::Method )
    Log_SetDomain( "DFLT",      Scope::Method )

    String testOK=  A_CHAR("This line has to appear");
    String testERR= A_CHAR("This line must not appear");

    // Test Verbosity setting
    Log_SetVerbosity( Log::DebugLogger,    Verbosity::Warning, "" )
    cntLL= Log::DebugLogger->CntLogs;    Log_Info       ( testERR )    UT_EQ( 0, Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_Warning    ( testOK  )    UT_EQ( 1, Log::DebugLogger->CntLogs - cntLL )

    Log_SetVerbosity( Log::DebugLogger,   Verbosity::Error )
    cntLL= Log::DebugLogger->CntLogs;    Log_Warning    ( testERR )    UT_EQ( 0, Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_Error      ( testOK  )    UT_EQ( 1, Log::DebugLogger->CntLogs - cntLL )

    // test subdomains
    Log_Prune( MemoryLogger* testML= new MemoryLogger(); )
    Log_SetVerbosity( testML, Verbosity::Off )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )
    Log_LogState ( "/TEST",      Verbosity::Info, A_CHAR("Dumping Log Configuration:") )

    Log_SetVerbosity ( testML,           Verbosity::Info       , "/DFLT"      )
    Log_SetVerbosity ( testML,           Verbosity::Warning    , "/DFLT/WARN" )
    Log_SetVerbosity ( testML,           Verbosity::Error      ,       "ERR"  )
    Log_SetVerbosity ( Log::DebugLogger, Verbosity::Info       , "/DFLT"      )
    Log_SetVerbosity ( Log::DebugLogger, Verbosity::Warning    , "/DFLT/WARN" )
    Log_SetVerbosity ( Log::DebugLogger, Verbosity::Error      ,       "ERR"  )

    Log_LogState ( "/TEST",      Verbosity::Info, A_CHAR("Dumping Log Configuration:") )

    // log with leading "/" on domain
    cntLL= testML->CntLogs;    Log_Verbose( "/DFLT",        testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "/DFLT/ERR",    testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "/DFLT/WARN",   testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Info   ( "/DFLT",        testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "/DFLT/ERR",    testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "/DFLT/WARN",   testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Warning( "/DFLT",        testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "/DFLT/WARN",   testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "/DFLT/ERR",    testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Error  ( "/DFLT",        testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "/DFLT/WARN",   testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "/DFLT/ERR",    testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )

    // log without leading "/" on domain (of course, this is quite an error of using ALox)
    cntLL= testML->CntLogs;    Log_Verbose( "DFLT",         testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "DFLT/ERR",     testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "DFLT/WARN",    testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Info   ( "DFLT",         testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "DFLT/ERR",     testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "DFLT/WARN",    testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Warning( "DFLT",         testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "DFLT/WARN",    testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "DFLT/ERR",     testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Error  ( "DFLT",         testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "DFLT/WARN",    testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "DFLT/ERR",     testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )


    // relative addressing
    cntLL= testML->CntLogs;    Log_Verbose(                 testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "",             testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "ERR",          testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Verbose( "WARN",         testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Info   (                 testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "",             testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "ERR",          testERR )     UT_EQ( 0, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Info   ( "WARN",         testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Warning(                 testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "",             testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "WARN",         testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Warning( "ERR",          testERR )     UT_EQ( 0, testML->CntLogs - cntLL )

    cntLL= testML->CntLogs;    Log_Error  (                 testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "",             testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "WARN",         testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )
    cntLL= testML->CntLogs;    Log_Error  ( "ERR",          testOK  )     UT_EQ( 1, testML->CntLogs - cntLL )

    Log_RemoveLogger( testML )
    Log_Prune( delete testML; )
}
#endif

#if !ALIB_SINGLE_THREADED && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
/** ********************************************************************************************
 * Log_Threads
 **********************************************************************************************/
UT_METHOD(Log_Threads)
{
    UT_INIT()

    Log_SetDomain( "/TEST/THREAD1", Scope::Method )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "" )

    // if this gets commented out, the test might crash. At least the console will
    // become scrambled!
    //Log_LOX.Lock.setUnsafe( true );
    {
        TThread thread2( A_CHAR("Thread2") , 50, 0 );
        thread2.Start();
        TThread thread3( A_CHAR("Thread3") , 50, 0 );
        thread3.Start();

        for ( int i= 0 ; i < 50 ; ++i )
        {
            Log_Info( "This is the main thread ", i )
            Thread::SleepMicros( 1 );
        }

        thread2.Join();
        thread3.Join();
    }
}
#endif // !ALIB_SINGLE_THREADED && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)


/** ********************************************************************************************
 * Log_TestAssertAndIf
 **********************************************************************************************/
#if ALOX_DBG_LOG
UT_METHOD(Log_TestAssertAndIf)
{
    UT_INIT()

    Log_Prune( String testOK=  A_CHAR("OK");    )
    Log_Prune( String testERR= A_CHAR("Error"); )

    // Test Verbosity setting
    Log_AddDebugLogger()
    integer cntLL= Log::DebugLogger->CntLogs;
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Info, "ASSERT")

    #ifdef ALOX_DBG_LOG
        int oneLine= 1;
    #else
        int oneLine= 0;
    #endif

    Log_SetDomain( "/ASSERT", Scope::Method )

    cntLL= Log::DebugLogger->CntLogs;    Log_Assert( true,  "/ASSERT",                  testERR )  UT_EQ( 0,        Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_Assert( false, "/ASSERT",                  testOK  )  UT_EQ( oneLine,  Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_Assert( true,                              testERR )  UT_EQ( 0,        Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_Assert( false,                             testOK  )  UT_EQ( oneLine,  Log::DebugLogger->CntLogs - cntLL )


    cntLL= Log::DebugLogger->CntLogs;    Log_If(     true,  "/ASSERT", Verbosity::Info, testOK  )  UT_EQ( oneLine,  Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_If(     false, "/ASSERT", Verbosity::Info, testERR )  UT_EQ( 0,        Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_If(     true,             Verbosity::Info, testOK  )  UT_EQ( oneLine,  Log::DebugLogger->CntLogs - cntLL )
    cntLL= Log::DebugLogger->CntLogs;    Log_If(     false,            Verbosity::Info, testERR )  UT_EQ( 0,        Log::DebugLogger->CntLogs - cntLL )
}
#endif

/** ********************************************************************************************
 * Log_ScopeInfoCacheTest
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
UT_METHOD(Log_ScopeInfoCacheTest)
{
    UT_INIT()

    Log_AddDebugLogger()

    Log_Prune( MemoryLogger memLogger;  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    if( DIRECTORY_SEPARATOR == '/' )
        { Log_Prune( memLogger.GetFormatMetaInfo().Format.Reset( A_CHAR("%Sp/%SF@" ) ); ) }
    else
        { Log_Prune( memLogger.GetFormatMetaInfo().Format.Reset( A_CHAR("%Sp\\%SF@") ); ) }


    ScopeInfoCacheTest1(); check_MemLogContains( A_CHAR("alox/ut_alox_lox.cpp")               , ut, memLogger, false );
    ScopeInfoCacheTest2(); check_MemLogContains( A_CHAR("alox/ut_alox_log_domains.cpp")       , ut, memLogger, false );
    ScopeInfoCacheTest3(); check_MemLogContains( A_CHAR("alox/ut_alox_log_rel.cpp")           , ut, memLogger, false );
    ScopeInfoCacheTest2(); check_MemLogContains( A_CHAR("alox/ut_alox_log_domains.cpp")       , ut, memLogger, false );
    ScopeInfoCacheTest3(); check_MemLogContains( A_CHAR("alox/ut_alox_log_rel.cpp")           , ut, memLogger, false );
    ScopeInfoCacheTest4(); check_MemLogContains( A_CHAR("alox/ut_alox_log_scopes.cpp")        , ut, memLogger, false );
    ScopeInfoCacheTest5(); check_MemLogContains( A_CHAR("alox/ut_alox_log_scopes_helper.cpp") , ut, memLogger, false );
    ScopeInfoCacheTest6(); check_MemLogContains( A_CHAR("alox/ut_alox_lox.cpp")               , ut, memLogger, false );
#if ALIB_UT_DOCS
    ScopeInfoCacheTest7(); check_MemLogContains( A_CHAR("alox/ut_alox_dox.cpp")           , ut, memLogger, false );
#endif

    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )

}
#endif


/** ********************************************************************************************
 * Log_SetSourcePathTrimRuleTest
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
UT_METHOD(Log_SetSourcePathTrimRuleTest)
{
    UT_INIT()

    Log_AddDebugLogger()

    Log_Prune( MemoryLogger memLogger;  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_Prune( memLogger.GetFormatMetaInfo().Format.Reset( A_CHAR("%Sp@") );   )

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )


    #if defined(_WIN32)
        Log_Info( ""); UT_TRUE( memLogger.MemoryLog.CharAt(1) == ':' );
        Log_Prune( memLogger.MemoryLog._();   )

    #elif defined(__unix__)
        check_MemLogStartsWith( A_CHAR("/")               , ut, memLogger );
    #endif

    Log_SetSourcePathTrimRule( "*"                        , lang::Inclusion::Include     )  // illegal rule, not stored (debug into)
    Log_SetSourcePathTrimRule( "**"                       , lang::Inclusion::Include     )  // illegal rule, not stored (debug into)
    Log_SetSourcePathTrimRule( "*/src.samples/unittests/" , lang::Inclusion::Include     )  check_MemLogStartsWith( A_CHAR("alox@")     , ut, memLogger );
    Log_SetSourcePathTrimRule( "*"                        , lang::Inclusion::Include     )  // illegal rule, not stored (debug into)

    Log_SetSourcePathTrimRule( "**"                       , lang::Inclusion::Include     )  // illegal rule, not stored (debug into)

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Include, -3 )  check_MemLogStartsWith( A_CHAR("es/unittests/alox@")  , ut, memLogger );
    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Include, 2  )  check_MemLogStartsWith( A_CHAR("ittests/alox@")       , ut, memLogger );

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Exclude     )  check_MemLogStartsWith( A_CHAR("/src")            , ut, memLogger );

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    #if !defined(ALOX_UNITTESTS_QMAKE_BUILD)
        Log_SetSourcePathTrimRule( "*/unittests/*", lang::Inclusion::Exclude, -2 )  check_MemLogStartsWith( A_CHAR("es/unittests")         , ut, memLogger );

        Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
        Log_SetSourcePathTrimRule( "*/unittests/*", lang::Inclusion::Exclude, -2 )  check_MemLogStartsWith( A_CHAR("es/unittests")         , ut, memLogger );

        // clear only local rule. (the above rule was global)
        Log_ClearSourcePathTrimRules( lang::Reach::Local, false )
                                                                      check_MemLogStartsWith( A_CHAR("es/unittests")         , ut, memLogger );
    #endif

    // set local rules
    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Exclude, 0, lang::Case::Ignore, nullptr, lang::Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Exclude, 0, lang::Case::Ignore, nullptr, lang::Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    // check non ignore case
    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Exclude, 0, lang::Case::Ignore, nullptr, lang::Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    Log_ClearSourcePathTrimRules( lang::Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src.samples/", lang::Inclusion::Exclude, 0, lang::Case::Sensitive, nullptr, lang::Reach::Local )

    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )
}
#endif

/** ********************************************************************************************
 * Log_SetSourcePathTrimRuleExternal
 **********************************************************************************************/
/* SNIPPIT FOR Dox
DOX_MARKER( [Man_SourcePathTrimming])
[ALOX]
GLOBAL_SOURCE_PATH_TRIM_RULES= *src/            , Include                            ; \
                               /usr/local/lib/  , Exclude, 9, Sensitive, /usr/lib/
DOX_MARKER( [Man_SourcePathTrimming])
*/

#if ALOX_REL_LOG && ALOX_REL_LOG_CI
UT_METHOD(Log_SetSourcePathTrimRuleExternal)
{
    UT_INIT()
    // for the unit tests to reset, we need an extra lox to be able to clear the global rules
    // before we initialize our main lox
    Lox clearLox("ClearingRules");

    // global rule
    clearLox.ClearSourcePathTrimRules( lang::Reach::Global, false );
    {
        // preset variables
        Variable var= variables::CampVariable(ALOX);
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            var.Declare( A_CHAR("ALOX/TESTML/FORMAT")                , A_CHAR("ALOXFMI") ); (void) var.Define(); var.Get<lox::textlogger::FormatMetaInfo>().Format.Reset(A_CHAR("%Sp"));
            var.Declare( A_CHAR("ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES"), A_CHAR("S"      ) ); (void) var.Define(); var= String(A_CHAR("*/src.samples/, true")) ;
        }
        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" );
            UT_TRUE(ml.MemoryLog.StartsWith( String16(A_CHAR("unittests")) << DIRECTORY_SEPARATOR ))

            ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }


    // local rule
    clearLox.ClearSourcePathTrimRules( lang::Reach::Global, false );
    {
        // preset variables
        Variable var= variables::CampVariable(ALOX);
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            var.Declare( A_CHAR("ALOX/TESTML/FORMAT")                , A_CHAR("ALOXFMI") ); (void) var.Define(); var.Get<lox::textlogger::FormatMetaInfo>().Format.Reset(A_CHAR("%Sp"));
            var.Declare( A_CHAR("ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES"), A_CHAR("S"      ) ); (void) var.Define(); var= String(A_CHAR("*;**; *unit*, include ;*;**"));
        }
        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" ); UT_EQ( A_CHAR("tests/alox")  , ml.MemoryLog ) ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }

    clearLox.ClearSourcePathTrimRules( lang::Reach::Global, false );
    {
        // preset variables
        Variable var= variables::CampVariable(ALOX);
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            var.Declare( A_CHAR("ALOX/TESTML/FORMAT")               , A_CHAR("ALOXFMI") ); (void) var.Define(); var.Get<lox::textlogger::FormatMetaInfo>().Format.Reset(A_CHAR("%Sp"));
            var.Declare( A_CHAR("ALOX/T_LOX/SOURCE_PATH_TRIM_RULES"), A_CHAR("S"      ) ); (void) var.Define(); var= String(A_CHAR("*unit, excl, 2, sens"));
        }
        // test
        Lox lox("T_LOX", false);
        #undef LOX_LOX
        #define LOX_LOX lox

        Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
        Lox_SetVerbosity( "CONSOLE" , Verbosity::Verbose )
        Lox_SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains )

        MemoryLogger ml("TESTML");
        Lox_SetVerbosity( &ml, Verbosity::Verbose )

        Lox_Info( "" )
        UT_EQ( A_CHAR("ittests/alox")  , ml.MemoryLog )   ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();


        Lox_SetSourcePathTrimRule( "*unit", lang::Inclusion::Exclude, 0, lang::Case::Ignore, "REPLACE/", lang::Reach::Local, Priority::Protected )
        Lox_Info( "" )
        UT_EQ( ml.MemoryLog, A_CHAR("REPLACE/unittests/alox") )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

        Lox_RemoveLogger( &ml )
        Lox_RemoveLogger( "CONSOLE" )
        delete consoleLogger;
    }

    // ignore case
    clearLox.ClearSourcePathTrimRules( lang::Reach::Global, false );
    {
        // preset variables
        Variable var= variables::CampVariable(ALOX);
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            var.Declare( A_CHAR("ALOX/TESTML/FORMAT")               , A_CHAR("ALOXFMI") ); (void) var.Define(); var.Get<lox::textlogger::FormatMetaInfo>().Format.Reset(A_CHAR("%Sp"));
            var.Declare( A_CHAR("ALOX/T_LOX/SOURCE_PATH_TRIM_RULES"), A_CHAR("S"      ) ); (void) var.Define(); var= String(A_CHAR("*UnIt, exc, 2, ign"));
        }
        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" ); UT_EQ( A_CHAR("ittests/alox")  , ml.MemoryLog ) ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }

    clearLox.ClearSourcePathTrimRules( lang::Reach::Global, false );
    {
        // preset variables
        Variable var= variables::CampVariable(ALOX);
        {ALIB_LOCK_WITH(ALOX.GetConfig())
            var.Declare( A_CHAR("ALOX/TESTML/FORMAT")               , A_CHAR("ALOXFMI") ); (void) var.Define(); var.Get<lox::textlogger::FormatMetaInfo>().Format.Reset(A_CHAR("%Sp"));
            var.Declare( A_CHAR("ALOX/T_LOX/SOURCE_PATH_TRIM_RULES"), A_CHAR("S"      ) ); (void) var.Define(); var= String(A_CHAR("*UnIt, excl, 2, insens"));
        }

        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" );
            { UT_EQ( A_CHAR("ittests/alox") , ml.MemoryLog )  }

            ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }
}
#endif

/** ********************************************************************************************
 * Log_MultipleLogables
 **********************************************************************************************/
UT_METHOD(Log_MultipleLogables)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )
    Log_SetVerbosity  ( Log::DebugLogger, Verbosity::Verbose, "BOXES" )
    Log_SetVerbosity  ( &memLogger      , Verbosity::Verbose, "BOXES" )
    Log_Prune( memLogger.GetFormatMetaInfo().Format.Reset( A_CHAR("") );   )

    Log_Info ( "BOXES", "Test" )
    Log_Prune( check_MemLogStartsWith( A_CHAR("Test")     , ut, memLogger );  )

{
    Boxes logables;
    logables.Add( "Hello {}", "World" );
    Log_Info ( "BOXES", logables )
    Log_Prune( check_MemLogStartsWith( A_CHAR("Hello World")     , ut, memLogger ); )

    logables.clear();
    logables.Add( "Just {}!", "once" );
    Log_Once( "BOXES", Verbosity::Info, logables, A_CHAR("GroupBoxes") )

    Log_Prune( check_MemLogStartsWith( A_CHAR("Just once!")     , ut, memLogger ) );
}

{
DOX_MARKER( [DOX_ALOX_LOX_ONCE])
// passing an array
{
    Log_Prune( Box logables[3]=  { "One - {} - {}!", "two", 3 }; )
    Log_Once( logables )
}

// passing a vector of boxes (less efficient than above, if the container object is used only once)
{
    Log_Prune( Boxes logables; )
    Log_Prune( logables.Add("One - {} - {}!", "two", 3 ) );
    Log_Once(  logables )
}
DOX_MARKER( [DOX_ALOX_LOX_ONCE])

    Log_RemoveLogger( &memLogger )
}

}


/** ********************************************************************************************
 * Log_ChangeStartTime
 **********************************************************************************************/
UT_METHOD(Log_ChangeStartTime)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_SetVerbosity  ( Log::DebugLogger, Verbosity::Verbose, "/S-TIME" )

    Log_Info ( "/S-TIME", "This is the first log with normal start time" )

    Log_Prune( Ticks newTime;                     )
    Log_Prune( newTime-= Ticks::Duration::FromMinutes(20);   )
    Log_SetStartTime( newTime, Log::DebugLogger->GetName() )
    Log_Info ( "/S-TIME", "Starttime set to 20 minutes ago" )
    Log_SetStartTime( Ticks(),  Log::DebugLogger->GetName() )
    Log_Info ( "/S-TIME", "Starttime set to 'now'" )
}

/** ********************************************************************************************
 * Log_GetState
 **********************************************************************************************/
UT_METHOD(Log_GetState)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )

    // reduce meta-information to limit output width
    Log_Prune( Log::DebugLogger->GetFormatMetaInfo().Format.Reset( A_CHAR("[%tN]%V[%D](%#): ") );  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_Prune(         memLogger.GetFormatMetaInfo().Format.Reset( A_CHAR("[%tN]%V[%D](%#): ") );  )
    Log_Prune(         memLogger.GetFormatMultiLine().Mode= 3; )

    // OK, let's use ALox
    Log_SetDomain( "PNS"   ,   Scope::Path + 1 )
    Log_SetDomain( "PATH",     Scope::Path )
    Log_SetDomain( "FN",       Scope::Filename )
    Log_SetDomain( "THREAD",   Scope::ThreadOuter )

    Log_SetVerbosity( "MONOMEM",        Verbosity::Off      , "/CON"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Verbose              )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Off      , "/MEM"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Error    , "/UI"     )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Info     , "/UI/DLG" )

    Log_Info( "This goes to both loggers" )
    Log_Info( "/MEM", "This goes only to the memory logger" )
    Log_Info( "/CON", "This goes only to the console logger" )

    Log_Once( "Will we see this in the config?" )
    Log_Once( "Will we see this in the config?", A_CHAR("ONCEKEY"), Scope::Filename )

    Log_Store("MyData 1" ,            Scope::Method )
    Log_Store("MyData 2" , "DataKey", Scope::Method )
    Log_Store(3          , "DataKey", Scope::Filename )
    Log_Store(4          , "DataKey", Scope::ThreadOuter )

    Log_SetPrefix( "TPre: "  , Scope::ThreadOuter )
    Log_SetPrefix( "MPre: "  , Scope::Method )
    Log_SetPrefix( "DomPre: " )
    Log_SetPrefix( "Mouse: ", "/UI/MOUSE" )
    Log_SetPrefix( ESC::RED,  "/ERRORS", lang::Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )

    // now, log the current config
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(ALL):") )

    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(Domains):"), lox::StateInfo::Domains )
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(Loggers):"), lox::StateInfo::Loggers )


    // cleanup
    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )
}

/** ********************************************************************************************
 * Log_DumpStateOnExit
 **********************************************************************************************/
#if ALOX_DBG_LOG

UT_METHOD(Log_DumpStateOnExit)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )


    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    UT_EQ( 1, Log::DebugLogger->CntLogs )
    Log_RemoveLogger( &memLogger )
    UT_EQ(1, Log::DebugLogger->CntLogs )

    integer cntLogs;

    Variable vc= variables::CampVariable(ALOX,
        String128(A_CHAR("ALOX/")) << LOG_LOX.GetName() << A_CHAR("/DUMP_STATE_ON_EXIT"), A_CHAR("S"));
    (void) vc.Define(Priority::DefaultValues);
    vc= String(A_CHAR("domain=/TEST, verbosity = e, sptr, basic"));

    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    vc= String(A_CHAR("verbosity = e, domains, basic"));
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    vc= String(A_CHAR("domains, loggers"));
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    vc= String(A_CHAR(""));
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs == cntLogs )

   Log_RemoveDebugLogger()
}

/** ********************************************************************************************
 * Log_WriteVerbosities
 **********************************************************************************************/
UT_METHOD(Log_WriteVerbosities)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger("MYLGGR") ;  )

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )

    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )

    Box replacements[2]= { LOG_LOX.GetName(), memLogger.GetName()  };
    Variable vc= variables::CampVariable(ALOX, lox::Variables::VERBOSITY, replacements );
    UT_TRUE( vc.IsDefined() )

    lox::CVVerbosities& vcv= vc.Get<lox::CVVerbosities>();
    UT_PRINT( "Variable written: Size=", vcv.Size() )
    for ( auto it : vcv )
        UT_PRINT( it )
    UT_EQ( 1, vcv.Size()  )

    Log_RemoveDebugLogger()
}

#endif

#include "aworx_unittests_end.hpp"
} // namespace

#endif // ALIB_UT_ALOX


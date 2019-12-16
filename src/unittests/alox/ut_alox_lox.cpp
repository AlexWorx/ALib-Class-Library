// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_ALOX


#include "alib/alox.hpp"
#include "alib/alox/loggers/consolelogger.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/alox/loggers/memorylogger.hpp"

#if !defined (HPP_ALIB_ALOXMODULE)
#   include "alib/alox/aloxmodule.hpp"
#endif
#if ALIB_THREADS
#   include "alib/threads/thread.hpp"
#endif

// conditional expression is constant (which is a great thing for the compiler by the way ;-)
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif


// Fix the method name of logging (needed for unity builds)
ALIB_WARNINGS_MACRO_NOT_USED_OFF
ALIB_WARNINGS_IGNORE_UNUSED_PARAMETER
#undef  ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#else
#   define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#endif

using namespace std;
using namespace aworx;

namespace ut_alox {

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

#if ALIB_THREADS
    class TThread : public Thread
    {
        public: TThread( const String& pname, int pcntLoops, int psleepMicros )
                : Thread( pname )
                {
                    this->cntLoops=     pcntLoops;
                    this->sleepMicros=  psleepMicros;
                    this->name.Reset(   pname );
                }
        int cntLoops;
        int sleepMicros;

        virtual void Run()
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
#include "unittests/aworx_unittests.hpp"
using namespace ut_alox;

namespace ut_aworx {

void check_MemLogStartsWith( const aworx::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog );
void check_MemLogStartsWith( const aworx::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog= true )
{
    if (doLog)
    {
        Log_Info("")
    }

UT_PRINT( String256("MemLog result: <<<") << memlog.MemoryLog << ">>> expected: " << exp)

    if( DirectorySeparator == '/' )
    {
        if( !memlog.MemoryLog.StartsWith<true,Case::Ignore>( exp ) )
        {
            UT_PRINT( "Expected start: {} Given: {}", exp, memlog.MemoryLog )
            UT_TRUE( false )
        }
    }
    else
    {
        String256 expCorrected( exp );
        expCorrected.SearchAndReplace( '/', '\\'  );
        if( !memlog.MemoryLog.StartsWith<true,Case::Ignore>( expCorrected ) )
        {
            UT_PRINT( "Expected start: {} Given: {}", expCorrected, memlog.MemoryLog )
            UT_TRUE( false )
        }
    }

    memlog.MemoryLog.Reset();
}

void check_MemLogContains( const aworx::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog );
void check_MemLogContains( const aworx::CString& exp, AWorxUnitTesting& ut, MemoryLogger& memlog, bool doLog= true )
{
    if (doLog)
    {
        Log_Info("")
    }
    if( DirectorySeparator == '/' )
    {
        UT_TRUE( memlog.MemoryLog.IndexOf<true ALIB_COMMA Case::Ignore>( exp, 0 ) >=0 )
    }
    else
    {
        String256 expCorrected( exp );
        expCorrected.SearchAndReplace( '/', '\\'  );
        UT_TRUE( memlog.MemoryLog.IndexOf<true ALIB_COMMA Case::Ignore>( expCorrected, 0 ) >=0 )
    }

    memlog.MemoryLog.Reset();
}


/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

// with GTEST macros it all gets wild. Fix the method name
#if ALIB_DEBUG
#   undef  ALIB_CALLER
#   define ALIB_CALLER     __FILE__, __LINE__,  UT_GET_TEST_NAME
#endif

UT_CLASS()

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

    // debug and realease lox
    #if ALIB_THREADS
    {
        Log_AddDebugLogger()

        Lox lox("ReleaseLox");

        UT_TRUE( Log::DebugLogger->GetSafeness() == Safeness::Unsafe )

        lox.Acquire(ALIB_CALLER);
            lox.SetVerbosity( Log::DebugLogger , Verbosity::Verbose );
        lox.Release();

        UT_TRUE( Log::DebugLogger->GetSafeness() == Safeness::Safe   )

        lox.Acquire(ALIB_CALLER);
            lox.RemoveLogger( Log::DebugLogger );
        lox.Release();

        UT_TRUE( Log::DebugLogger->GetSafeness() == Safeness::Unsafe )

        Log_RemoveDebugLogger()
    }
    #endif

    // two release loxes
    #if ALOX_REL_LOG && ALIB_THREADS
    {
        Lox lox1("Lox1");
        Lox lox2("Lox2");
        TextLogger*  cl= Lox::CreateConsoleLogger();

        UT_TRUE( cl->GetSafeness() == Safeness::Unsafe )

        #undef  LOX_LOX
        #define LOX_LOX lox1
        Lox_SetVerbosity( cl, Verbosity::Verbose )

        UT_TRUE( cl->GetSafeness() == Safeness::Unsafe )

        #undef  LOX_LOX
        #define LOX_LOX lox2
        Lox_SetVerbosity( cl, Verbosity::Verbose )


        UT_TRUE( cl->GetSafeness() == Safeness::Safe  )

        #undef  LOX_LOX
        #define LOX_LOX lox1
        Lox_RemoveLogger( cl )

        UT_TRUE( cl->GetSafeness() == Safeness::Unsafe )


        #undef  LOX_LOX
        #define LOX_LOX lox2
        Lox_RemoveLogger( cl )

        UT_TRUE( cl->GetSafeness() == Safeness::Unsafe )


        delete cl;
    }
    #endif
}
#endif

/** ********************************************************************************************
 * Log_LogLevelSetting
 **********************************************************************************************/
#if ALOX_DBG_LOG && ALOX_DBG_LOG_CI  && !ALIB_DEBUG_MONOMEM
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

        // test sub domains
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

        // log without leading "/" on domain (of-course, this is quite an error of using ALox)
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

        // clean the config (for subsequent tests)
        lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Clear();
        lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Clear();
    }
#endif

#if ALIB_THREADS && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)
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
    }
}
#endif // ALIB_THREADS && !defined(ALIB_UT_ROUGH_EXECUTION_SPEED_TEST)


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
    if( DirectorySeparator == '/' )
        { Log_Prune( memLogger.MetaInfo->Format.Reset( A_CHAR("%Sp/%SF@" ) ); ) }
    else
        { Log_Prune( memLogger.MetaInfo->Format.Reset( A_CHAR("%Sp\\%SF@") ); ) }


    ScopeInfoCacheTest1(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_lox.cpp")               , ut, memLogger, false );
    ScopeInfoCacheTest2(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_domains.cpp")       , ut, memLogger, false );
    ScopeInfoCacheTest3(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_rel.cpp")           , ut, memLogger, false );
    ScopeInfoCacheTest2(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_domains.cpp")       , ut, memLogger, false );
    ScopeInfoCacheTest3(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_rel.cpp")           , ut, memLogger, false );
    ScopeInfoCacheTest4(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_scopes.cpp")        , ut, memLogger, false );
    ScopeInfoCacheTest5(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_log_scopes_helper.cpp") , ut, memLogger, false );
    ScopeInfoCacheTest6(); check_MemLogContains( A_CHAR("unittests/alox/ut_alox_lox.cpp")               , ut, memLogger, false );
#if ALIB_UT_DOCS
    ScopeInfoCacheTest7(); check_MemLogContains( A_CHAR("unittests/alox/ut_dox_tutorial.cpp")           , ut, memLogger, false );
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
    Log_Prune( memLogger.MetaInfo->Format.Reset( A_CHAR("%Sp@") );   )

    Log_ClearSourcePathTrimRules( Reach::Global, false )


    #if defined(_WIN32)
        Log_Info( ""); UT_TRUE( memLogger.MemoryLog.CharAt(1) == ':' );
        Log_Prune( memLogger.MemoryLog._();   )

    #else
        #if !defined(ALOX_UNITTESTS_QMAKE_BUILD) && defined(__unix__)
            check_MemLogStartsWith( A_CHAR("/home")               , ut, memLogger );
        #endif
    #endif

    Log_SetSourcePathTrimRule( "*"         , Inclusion::Include     )  // illegal rule, not stored (debug into)
    Log_SetSourcePathTrimRule( "**"        , Inclusion::Include     )  // illegal rule, not stored (debug into)
    Log_SetSourcePathTrimRule( "*/src/"    , Inclusion::Include     )  check_MemLogStartsWith( A_CHAR("unittests/alox@")     , ut, memLogger );
    Log_SetSourcePathTrimRule( "*"         , Inclusion::Include     )  // illegal rule, not stored (debug into)

// 171209: This single test line 'suddenly' did not work any more with valgrind.
// We have no clue why! Even more strange is that this test runs
// with valgrind if executed exclusively or in a limited set of tests.
// Is this now a bug in ALox? We do not guess so, as it only appears when all > 100 tests are run in valgrind.
// More likely a valgrind effect.
// No time to investigate right now...postponing investigation, or a new valgrind version will fix it again?
#if !defined(ALIB_AVOID_ANALYZER_WARNINGS)
    Log_SetSourcePathTrimRule( "**"        , Inclusion::Include     );  // illegal rule, not stored (debug into)
                                                                        check_MemLogStartsWith( A_CHAR("unittests/alox@")     , ut, memLogger );
#endif

    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Include, -3 )  check_MemLogStartsWith( A_CHAR("rc/unittests/alox@")  , ut, memLogger );
    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Include, 2  )  check_MemLogStartsWith( A_CHAR("ittests/alox@")       , ut, memLogger );

    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude     )  check_MemLogStartsWith( A_CHAR("/src")            , ut, memLogger );

    Log_ClearSourcePathTrimRules( Reach::Global, false )
    #if !defined(ALOX_UNITTESTS_QMAKE_BUILD)
        Log_SetSourcePathTrimRule( "*/unittests/*", Inclusion::Exclude, -2 )  check_MemLogStartsWith( A_CHAR("rc/unittests")         , ut, memLogger );

        Log_ClearSourcePathTrimRules( Reach::Global, false )
        Log_SetSourcePathTrimRule( "*/unittests/*", Inclusion::Exclude, -2 )  check_MemLogStartsWith( A_CHAR("rc/unittests")         , ut, memLogger );

        // clear only local rule. (the above rule was global)
        Log_ClearSourcePathTrimRules( Reach::Local, false )
                                                                      check_MemLogStartsWith( A_CHAR("rc/unittests")         , ut, memLogger );
    #endif

    // set local rules
    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, nullptr, Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, nullptr, Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    // check non ignore case
    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, nullptr, Reach::Local )  check_MemLogStartsWith( A_CHAR("/src")         , ut, memLogger );

    Log_ClearSourcePathTrimRules( Reach::Global, false )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Sensitive, nullptr, Reach::Local )

    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )

    // clean the config (for subsequent tests)
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Clear();
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Clear();
}
#endif

/** ********************************************************************************************
 * Log_SetSourcePathTrimRuleExternal
 **********************************************************************************************/
#if ALOX_REL_LOG && ALOX_REL_LOG_CI
UT_METHOD(Log_SetSourcePathTrimRuleExternal)
{
    UT_INIT()
    // for the unit tests to reset, we need an extra lox to be able to clear the global rules
    // beforewe initialize our main lox
    Lox clearLox("ClearingRules");

    // global rule
    clearLox.ClearSourcePathTrimRules( Reach::Global, false );
    {
        // store default values
        Variable var;
        lib::ALOX.GetConfig().Store( var.Declare( A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT")                 ), A_CHAR("%Sp") );
        lib::ALOX.GetConfig().Store( var.Declare( A_CHAR("ALOX"), A_CHAR("GLOBAL_SOURCE_PATH_TRIM_RULES") ), A_CHAR("*/src., true") );

        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" );
            if( DirectorySeparator == '/' )
                { UT_TRUE(ml.MemoryLog.StartsWith(A_CHAR("cpp/")) );  }
            else
                { UT_TRUE(ml.MemoryLog.StartsWith(A_CHAR("cpp\\")) ); }

            ml.MemoryLog._(); ml.AutoSizes.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }


    // local rule
    clearLox.ClearSourcePathTrimRules( Reach::Global, false );
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::lib::config::InMemoryPlugin>(Priorities::DefaultValues)->Clear();
    {
        // store default values
        Variable var;
        lib::ALOX.GetConfig().Store(var.Declare( A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT")                     ), A_CHAR("%Sp") );
        // default values, 0, ignore" the * will be removed. two illegal rules before and after
        lib::ALOX.GetConfig().Store(var.Declare( A_CHAR("ALOX"), A_CHAR("T_LOX_SOURCE_PATH_TRIM_RULES"),';'  ), A_CHAR("*;**; *alox.u*, include ;*;**") );


        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" ); UT_EQ( A_CHAR("nittests")  , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }

    clearLox.ClearSourcePathTrimRules( Reach::Global, false );
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::lib::config::InMemoryPlugin>(Priorities::DefaultValues)->Clear();
    {
        // create iniFile
        InMemoryPlugin iniFile("UnitTest");
        Variable var;
        iniFile.Store( var.Declare( A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT") ),  A_CHAR("%Sp") );
        iniFile.Store( var.Declare( A_CHAR("ALOX"), A_CHAR("T_LOX_SOURCE_PATH_TRIM_RULES"),';'),
                       A_CHAR("*alox.u, excl, 2, sens") );
        lib::ALOX.GetConfig().InsertPlugin(&iniFile, Priorities::Standard);

        // test
        Lox lox("T_LOX", false);
        #undef LOX_LOX
        #define LOX_LOX lox

        Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
        Lox_SetVerbosity( "CONSOLE" , Verbosity::Verbose );
        Lox_SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

        MemoryLogger ml("TESTML");
        Lox_SetVerbosity( &ml, Verbosity::Verbose );

        Lox_Info( "" ); UT_EQ( A_CHAR("ox.unittests")  , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();


        // overwrite with source priority
        Lox_SetSourcePathTrimRule( "*alox.u", Inclusion::Exclude, 0, Case::Ignore, A_CHAR("REPLACE_1/"), Reach::Local );
        Lox_Info( "" ); UT_EQ( A_CHAR("ox.unittests")  , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
        Lox_SetSourcePathTrimRule( "*alox.u", Inclusion::Exclude, 0, Case::Ignore, A_CHAR("REPLACE_2/"), Reach::Local, Priorities::ProtectedValues );
        Lox_Info( "" ); UT_TRUE( ml.MemoryLog.StartsWith( A_CHAR("REPLACE_2/") ) ); ml.MemoryLog._(); ml.AutoSizes.Reset();

        Lox_RemoveLogger( &ml );
        Lox_RemoveLogger( "CONSOLE" );
        delete consoleLogger;
        lib::ALOX.GetConfig().RemovePlugin(&iniFile);
    }

    // ignore case
    clearLox.ClearSourcePathTrimRules( Reach::Global, false );
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::lib::config::InMemoryPlugin>(Priorities::DefaultValues)->Clear();
    {
        // store default values
        Variable var;
        lib::ALOX.GetConfig().Store(var.Declare( A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT")                   ), A_CHAR("%Sp") );
        lib::ALOX.GetConfig().Store(var.Declare( A_CHAR("ALOX"), A_CHAR("T_LOX_SOURCE_PATH_TRIM_RULES"),';'), A_CHAR("*aLOX.U, exc, 2, ign")   );

        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" ); UT_EQ( A_CHAR("ox.unittests")  , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }

    clearLox.ClearSourcePathTrimRules( Reach::Global, false );
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::lib::config::InMemoryPlugin>(Priorities::DefaultValues)->Clear();
    {
        // store default values
        Variable var;
        lib::ALOX.GetConfig().Store( var.Declare( A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT")                     ) , A_CHAR("%Sp")  );
        lib::ALOX.GetConfig().Store( var.Declare( A_CHAR("ALOX"), A_CHAR("T_LOX_SOURCE_PATH_TRIM_RULES") , ';') , A_CHAR("*aLOX.U, excl, 2, insens")  );

        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);

            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
            lox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Verbose );

            lox.Info( "" );
            { UT_EQ( A_CHAR("ox.unittests") , ml.MemoryLog );  }

            ml.MemoryLog._(); ml.AutoSizes.Reset();

            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;

        lox.Release();
    }

    // clean the config (for subsequent tests)
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Clear();
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Clear();
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
    Log_Prune( memLogger.MetaInfo->Format.Reset( A_CHAR("") );   )
    Log_SetVerbosity  ( Log::DebugLogger, Verbosity::Verbose, "BOXES" )
    Log_SetVerbosity  ( &memLogger      , Verbosity::Verbose, "BOXES" )

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
//! [DOX_ALOX_LOX_ONCE]
// passing an array
{
    Log_Prune( Box logables[3]=  { "One - {} - {}!", "two", 3 } );
    Log_Once( logables )
}

// passing a container (less efficient than above, if the container object is used only once)
{
    Boxes logables;
    logables.Add("One - {} - {}!", "two", 3 );
    Log_Once( logables )
}
//! [DOX_ALOX_LOX_ONCE]

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

    // reduce meta information to limit output width
    Log_Prune( Log::DebugLogger->MetaInfo->Format.Reset( A_CHAR("[%tN]%V[%D](%#): ") );  )
    Log_Prune(         memLogger.MetaInfo->Format.Reset( A_CHAR("[%tN]%V[%D](%#): ") );  )
    Log_Prune(         memLogger.MultiLineMsgMode= 3; )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )

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
    Log_SetPrefix( ESC::RED,  "/ERRORS", Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )

    // now, log the current config
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(ALL):") )

    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(Domains):"), lib::lox::StateInfo::Domains )
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("State(Loggers):"), lib::lox::StateInfo::Loggers )


    // cleanup
    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )
}

/** ********************************************************************************************
 * Log_DumpStateOnExit
 **********************************************************************************************/
#if ALOX_DBG_LOG  && !ALIB_DEBUG_MONOMEM

UT_METHOD(Log_DumpStateOnExit)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )


    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    UT_TRUE( Log::DebugLogger->CntLogs == 0 )
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs == 0 )

    Variable var( A_CHAR("ALOX"), String128() << LOG_LOX.GetName() << "_DUMP_STATE_ON_EXIT",
                  ',' );
    integer cntLogs;


    lib::ALOX.GetConfig().Store( var,  A_CHAR("domain=/TEST, verbosity = e, sptr, basic") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    lib::ALOX.GetConfig().Store( var,  A_CHAR("verbosity = e, domains, basic") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    lib::ALOX.GetConfig().Store( var,  A_CHAR("domains, loggers") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    cntLogs= Log::DebugLogger->CntLogs;
    Log_RemoveLogger( &memLogger )
    UT_TRUE( Log::DebugLogger->CntLogs > cntLogs )

    lib::ALOX.GetConfig().Store( var,  A_CHAR("") );
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

    AString varName( LOG_LOX.GetName() );
    varName._(A_CHAR("_MYLGGR_VERBOSITY"));

    Variable var;
    var.Declare( A_CHAR("ALOX"), varName, ',' );
    Variable varBack;
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )

    // test writing into other variable with variable name error
    UT_PRINT( "An error message should follow (wrong variable format): " )
    lib::ALOX.GetConfig().Store( var,  A_CHAR("writeback MY_") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )

    // test writing into other variable
    lib::ALOX.GetConfig().Store( var,  A_CHAR("writeback MY_VAR") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )
    lib::ALOX.GetConfig().Load( varBack.Declare( A_CHAR("MY"),  A_CHAR("VAR")) );
    UT_PRINT( "Variable written: ", varBack.GetString() )
    UT_TRUE( varBack.GetString().Length() > 0 )

    // test writing into other variable without cat
    lib::ALOX.GetConfig().Store( var,  A_CHAR("writeback ANON") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )
    lib::ALOX.GetConfig().Load( varBack.Declare( nullptr,  A_CHAR("ANON")) );
    UT_PRINT( "Variable written: ", varBack.GetString() )
    UT_TRUE( varBack.GetString().Length() > 0 )

    // test writing into other variable without cat and with underscores in name
    lib::ALOX.GetConfig().Store( var,  A_CHAR("writeback _2ND_ANON") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )
    lib::ALOX.GetConfig().Load( varBack.Declare( nullptr,  A_CHAR("2ND_ANON")) );
    UT_PRINT( "Variable written: ", varBack.GetString() )
    UT_TRUE( varBack.GetString().Length() > 0 )

    // test writing into other the variable itself
    lib::ALOX.GetConfig().Store( var,  A_CHAR("writeback") );
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_RemoveLogger( &memLogger )

    lib::ALOX.GetConfig().Load( var );
    UT_PRINT( "Variable written: ", var.GetString() )
    UT_TRUE( var.GetString().Length() > 0 )

    Log_RemoveDebugLogger()
}

#endif

#include "unittests/aworx_unittests_end.hpp"
} // namespace

ALIB_WARNINGS_RESTORE
ALIB_WARNINGS_RESTORE

#endif // ALIB_UT_ALOX

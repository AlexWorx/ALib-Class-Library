// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_ALOX


#include "alib/alox.hpp"

#include "alib/alox/loggers/memorylogger.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace aworx;


// Fix the method name of logging (needed for unity builds)
ALIB_WARNINGS_MACRO_NOT_USED_OFF
#undef  ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#else
#   define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#endif
ALIB_WARNINGS_RESTORE

namespace ut_alox {
// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest3();
void ScopeInfoCacheTest3() { Log_Info("Test Method 3") }

}

#define TESTCLASSNAME       CPP_ALox_Lox_Release
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {

#if ALOX_REL_LOG

#define LOX_LOX lox

/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

UT_CLASS

/** ********************************************************************************************
 * Lox_TestVerbositySetting
 **********************************************************************************************/
UT_METHOD(Lox_TestVerbositySetting)
{
    UT_INIT()
    Lox lox("ReleaseLox");
    TextLogger*  cl= Lox::CreateConsoleLogger();

    Lox_SetDomain( "TLLS_DF", Scope::Method )

    // Test Verbosity setting
    integer  logLinesBefore= cl->CntLogs;
    int cntLoggers= 0;

    Lox_SetVerbosity( cl, Verbosity::Verbose )
    Lox_Verbose  (         "This Verbose line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Verbose ) UT_EQ( 1, cntLoggers )
    Lox_Info     (         "This Info    line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Info    ) UT_EQ( 1, cntLoggers )
    Lox_Warning  (         "This WARN    line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Warning ) UT_EQ( 1, cntLoggers )
    Lox_Error    (         "This Error   line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Error   ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Info )
    Lox_Verbose  (         "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Verbose ) UT_EQ( 0, cntLoggers )
    Lox_Info     (         "This Info    line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Info    ) UT_EQ( 1, cntLoggers )
    Lox_Warning  (         "This Warning line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Warning ) UT_EQ( 1, cntLoggers )
    Lox_Error    (         "This Error   line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Error   ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Warning )                                                  Lox_IsActive(cntLoggers, Verbosity::Verbose ) UT_EQ( 0, cntLoggers )
    Lox_Verbose  (         "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Info    ) UT_EQ( 0, cntLoggers )
    Lox_Info     (         "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Warning ) UT_EQ( 1, cntLoggers )
    Lox_Warning  (         "This Warning line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Error   ) UT_EQ( 1, cntLoggers )
    Lox_Error    (         "This Error   line should be logged" )

    Lox_SetVerbosity( cl, Verbosity::Error )
    Lox_Verbose  (         "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Verbose ) UT_EQ( 0, cntLoggers )
    Lox_Info     (         "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Info    ) UT_EQ( 0, cntLoggers )
    Lox_Warning  (         "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Warning ) UT_EQ( 0, cntLoggers )
    Lox_Error    (         "This Error   line should be logged" )                               Lox_IsActive(cntLoggers, Verbosity::Error   ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Off )
    Lox_Verbose  (         "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Verbose ) UT_EQ( 0, cntLoggers )
    Lox_Info     (         "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Info    ) UT_EQ( 0, cntLoggers )
    Lox_Warning  (         "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Warning ) UT_EQ( 0, cntLoggers )
    Lox_Error    (         "This Error   line should NOT be logged. !!!!!Test Error!!!!!" )     Lox_IsActive(cntLoggers, Verbosity::Error   ) UT_EQ( 0, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Verbose, "/TLLS" )
    Lox_Verbose  ( "/TLLS", "This Verbose line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Verbose, "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Info     ( "/TLLS", "This Info    line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Info   , "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Warning  ( "/TLLS", "This WARN    line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Warning, "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Error    ( "/TLLS", "This Error   line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Error  , "/TLLS" ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Info, "/TLLS" )
    Lox_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Verbose, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Info     ( "/TLLS", "This Info    line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Info   , "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Warning  ( "/TLLS", "This Warning line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Warning, "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Error    ( "/TLLS", "This Error   line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Error  , "/TLLS" ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Warning, "/TLLS" )
    Lox_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Verbose, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Info   , "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Warning  ( "/TLLS", "This Warning line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Warning, "/TLLS" ) UT_EQ( 1, cntLoggers )
    Lox_Error    ( "/TLLS", "This Error   line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Error  , "/TLLS" ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Error, "/TLLS" )
    Lox_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Verbose, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Info   , "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Warning  ( "/TLLS", "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Warning, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Error    ( "/TLLS", "This Error   line should be logged" )                              Lox_IsActive(cntLoggers, Verbosity::Error  , "/TLLS" ) UT_EQ( 1, cntLoggers )

    Lox_SetVerbosity( cl, Verbosity::Off, "/TLLS" )
    Lox_Verbose  ( "/TLLS", "This Verbose line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Verbose, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Info     ( "/TLLS", "This Info    line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Info   , "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Warning  ( "/TLLS", "This Warning line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Warning, "/TLLS" ) UT_EQ( 0, cntLoggers )
    Lox_Error    ( "/TLLS", "This Error   line should NOT be logged. !!!!!Test Error!!!!!" )    Lox_IsActive(cntLoggers, Verbosity::Error  , "/TLLS" ) UT_EQ( 0, cntLoggers )

    UT_EQ( 20, cl->CntLogs - logLinesBefore )

    Lox_RemoveLogger( cl )
    delete cl;
}

/** ********************************************************************************************
 * Lox_TestScopeDomain
 **********************************************************************************************/
#if ALOX_REL_LOG_CI
    UT_METHOD(Lox_TestScopeDomain)
    {
        UT_INIT()
        int  cntLL;
        MemoryLogger ml;
        Lox lox("ReleaseLox");
        TextLogger*  cl= Lox::CreateConsoleLogger();
        Lox_Error("No domain and nothing set");

        Lox_SetDomain( "REPLACED",  Scope::Method );
        Lox_SetDomain( "DFLT",      Scope::Method );

        String testOK=  "OK";
        String testERR= "Error";

        // Test Verbosity setting
        Lox_SetVerbosity( cl,    Verbosity::Warning, "" );
        cntLL= cl->CntLogs;    Lox_Info       ( testERR );    UT_EQ( 0, cl->CntLogs - cntLL );
        cntLL= cl->CntLogs;    Lox_Warning    ( testOK  );    UT_EQ( 1, cl->CntLogs - cntLL );

        Lox_SetVerbosity( cl,  Verbosity::Error );
        cntLL= cl->CntLogs;    Lox_Warning    ( testERR );    UT_EQ( 0, cl->CntLogs - cntLL );
        cntLL= cl->CntLogs;    Lox_Error      ( testOK  );    UT_EQ( 1, cl->CntLogs - cntLL );

        // test sub domains
        Lox_SetVerbosity( cl, Verbosity::Verbose, Lox::InternalDomains );

        // Lox_LogState ( "/TEST",      Verbosity::Info, "Dumping Log Configuration:" );

        Lox_SetVerbosity( cl,  Verbosity::Info, "/DFLT"      );
        Lox_SetVerbosity( cl,  Verbosity::Warning    , "/DFLT/WARN" );
        Lox_SetVerbosity( cl,  Verbosity::Error               ,       "ERR"  );
        Lox_SetVerbosity( &ml, Verbosity::Info, "/DFLT"      );
        Lox_SetVerbosity( &ml, Verbosity::Warning    , "/DFLT/WARN" );
        Lox_SetVerbosity( &ml, Verbosity::Error               ,       "ERR"  );

        //Lox_LogState ( "/TEST",      Verbosity::Info, "Dumping Log Configuration:" );

        // log with leading "/" on domain
        cntLL= ml.CntLogs;    Lox_Verbose ( "/DFLT",        testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "/DFLT/ERR",    testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "/DFLT/WARN",   testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Info    ( "/DFLT",        testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "/DFLT/ERR",    testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "/DFLT/WARN",   testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Warning ( "/DFLT",        testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "/DFLT/WARN",   testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "/DFLT/ERR",    testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Error   ( "/DFLT",        testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "/DFLT/WARN",   testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "/DFLT/ERR",    testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );

        // log without leading "/" on domain (of-course, this is quite an error of using ALox
        cntLL= ml.CntLogs;    Lox_Verbose ( "DFLT",         testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "DFLT/ERR",     testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "DFLT/WARN",    testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Info    ( "DFLT",         testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "DFLT/ERR",     testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "DFLT/WARN",    testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Warning ( "DFLT",         testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "DFLT/WARN",    testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "DFLT/ERR",     testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Error   ( "DFLT",         testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "DFLT/WARN",    testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "DFLT/ERR",     testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );


        // relative addressing with "~"domain
        cntLL= ml.CntLogs;    Lox_Verbose (                 testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "",             testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "ERR",          testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Verbose ( "WARN",         testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Info    (                 testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "",             testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "ERR",          testERR );    UT_EQ( 0, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Info    ( "WARN",         testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Warning (                 testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "",             testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "WARN",         testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Warning ( "ERR",          testERR );    UT_EQ( 0, ml.CntLogs - cntLL );

        cntLL= ml.CntLogs;    Lox_Error   (                 testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "",             testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "WARN",         testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );
        cntLL= ml.CntLogs;    Lox_Error   ( "ERR",          testOK  );    UT_EQ( 1, ml.CntLogs - cntLL );

        Lox_RemoveLogger( cl );
        delete cl;
    }
#endif

/** ********************************************************************************************
 * Lox_TestAssert
 **********************************************************************************************/
UT_METHOD(Lox_TestAssert)
{
    UT_INIT()

    Lox lox("ReleaseLox");
    TextLogger*  cl= Lox::CreateConsoleLogger();

    Lox_Prune( String testOK=  A_CHAR("OK");    )
    Lox_Prune( String testERR= A_CHAR("Error"); )

    // Test Verbosity setting
    integer cntLL= cl->CntLogs;
    Lox_SetVerbosity( cl, Verbosity::Info, "/" )

    cntLL= cl->CntLogs;    Lox_Assert( true,  "/ASSERT",                  testERR ) UT_EQ( 0,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_Assert( false, "/ASSERT",                  testOK  ) UT_EQ( 1,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_Assert( true,                              testERR ) UT_EQ( 0,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_Assert( false,                             testOK  ) UT_EQ( 1,  cl->CntLogs - cntLL )

    cntLL= cl->CntLogs;    Lox_If(     true,  "/ASSERT", Verbosity::Info, testOK  ) UT_EQ( 1,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_If(     false, "/ASSERT", Verbosity::Info, testERR ) UT_EQ( 0,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_If(     true,             Verbosity::Info, testOK  ) UT_EQ( 1,  cl->CntLogs - cntLL )
    cntLL= cl->CntLogs;    Lox_If(     false,            Verbosity::Info, testERR ) UT_EQ( 0,  cl->CntLogs - cntLL )

    Lox_RemoveLogger( cl )
    delete cl;
}


/** ********************************************************************************************
 * Lox_ChangeStartTime
 **********************************************************************************************/
UT_METHOD(Lox_ChangeStartTime)
{
    UT_INIT()

    Lox lox("ReleaseLox");
    TextLogger*  cl= Lox::CreateConsoleLogger();

    Lox_SetVerbosity( cl, Verbosity::Verbose, "/S-TIME" )

    Lox_Info ( "/S-TIME", "This is the first log with normal start time" )

    Ticks newTime;
    newTime-= Ticks::Duration::FromMinutes(20);

    Lox_SetStartTime( newTime, cl->GetName() )
    Lox_Info ( "/S-TIME", "Starttime set to 20 minutes ago" )
    Lox_SetStartTime( Ticks(),  cl->GetName() )
    Lox_Info ( "/S-TIME", "Starttime set to 'now'" )

    Lox_RemoveLogger( cl )
    delete cl;
}

#include "unittests/aworx_unittests_end.hpp"

#endif // ALOX_REL_LOG

} // namespace


#endif // ALIB_UT_ALOX

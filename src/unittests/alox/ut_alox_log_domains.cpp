// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_ALOX

#include "alib/compatibility/std_strings.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/aloxcamp.hpp"
#include "alib/config/inifilefeeder.hpp"

#include <iostream>
#include <fstream>

using namespace std;

using namespace alib;
using namespace alib::lox::textlogger;


namespace ut_alox {

#include "alib/lang/callerinfo_functions.hpp"
// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest2();
void ScopeInfoCacheTest2() { Log_Info("Test Method 2") }
#include "alib/lang/callerinfo_methods.hpp"

#if ALIB_DEBUG  
#define LOG_CHECK( d, s, ml,lox )    {                  \
        ml.MemoryLog._();                               \
        ml.GetAutoSizes().Main.Reset();                 \
        ml.GetAutoSizes().LogMessage.Reset();           \
        lox.Acquire(ALIB_CALLER);                       \
        lox.GetLogableContainer().Add("");              \
        lox.Entry( d, Verbosity::Info );                \
        lox.Release();                                  \
        UT_EQ(A_CHAR(s), ml .MemoryLog);                \
        }
#endif
}

#define TESTCLASSNAME       CPP_ALox_Log_Domains
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {

/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

UT_CLASS

/** ********************************************************************************************
 * Lox_IllegalDomainNames
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
UT_METHOD(Lox_IllegalDomainNames)
{
    UT_INIT()

    Log_AddDebugLogger()
    MemoryLogger ml;
    Log_SetVerbosity(&ml, Verbosity::Verbose )
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )
    ml.GetFormatMetaInfo().Format.Reset("<%D>");

    Lox& lox=  *lox::Log::Get();

    LOG_CHECK( ""        , "</>"              , ml,lox)
    LOG_CHECK( "LOC"     , "</LOC>"           , ml,lox)
    LOG_CHECK( "%"       , "</#>"             , ml,lox)
    LOG_CHECK( "\x03"    , "</#>"             , ml,lox)
    LOG_CHECK( "<"       , "</#>"             , ml,lox)
    LOG_CHECK( ">"       , "</#>"             , ml,lox)
    LOG_CHECK( "?"       , "</#>"             , ml,lox)
    LOG_CHECK( ","       , "</#>"             , ml,lox)
    LOG_CHECK( "-"       , "</->"             , ml,lox)
    LOG_CHECK( "_"       , "</_>"             , ml,lox)
    LOG_CHECK( "@"       , "</#>"             , ml,lox)
    LOG_CHECK( "."       , "</>"              , ml,lox)
    LOG_CHECK( ".."      , "</>"              , ml,lox)
    LOG_CHECK( "CU.."    , "</CU##>"          , ml,lox)

    LOG_CHECK( "$%! "    , "</####>"          , ml,lox)

    Log_SetDomain( "METH", Scope::Method )
    LOG_CHECK( "$%! "    , "</METH/####>"     , ml,lox)

    Log_SetDomain( "A\"C" , Scope::Method )
    LOG_CHECK( ""        , "</A#C>"           , ml,lox)
    LOG_CHECK( "LOC"     , "</A#C/LOC>"       , ml,lox)
    LOG_CHECK( "*X*"     , "</A#C/#X#>"       , ml,lox)

    Log_RemoveLogger( &ml )
}
#endif

/** ********************************************************************************************
 * Lox_DomainsRelative
 **********************************************************************************************/
#if ALOX_REL_LOG_CI
UT_METHOD(Lox_DomainsRelative)
{
    UT_INIT()
    Lox lox("ReleaseLox");
    #define LOX_LOX lox
    MemoryLogger ml;

    Lox_SetVerbosity ( &ml, Verbosity::Verbose )
    ml.GetFormatMetaInfo().Format.Reset("@%D#");
    Lox_SetDomain( "/D1/D2/D3", Scope::ThreadOuter )

    Lox_Info( "D4"                 , "" ) UT_EQ(  A_CHAR("@/D1/D2/D3/D4#")        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();
    Lox_Info( "./D4"               , "" ) UT_EQ(  A_CHAR("@/D1/D2/D3/D4#")        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();
    Lox_Info( "../D4"              , "" ) UT_EQ(  A_CHAR("@/D1/D2/D4#"   )        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();
    Lox_Info( ".././.././D4"       , "" ) UT_EQ(  A_CHAR("@/D1/D4#"      )        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();
    Lox_Info( "../../../../../D4"  , "" ) UT_EQ(  A_CHAR("@/D4#"         )        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();
    Lox_Info( "../D4/../D5"        , "" ) UT_EQ(  A_CHAR("@/D1/D2/D5#"   )        , ml.MemoryLog )  ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

    Lox_RemoveLogger( &ml )

    #undef LOX_LOX
}
#endif


/** ********************************************************************************************
 * Log_DomainSubstitutions
 **********************************************************************************************/
#if ALOX_DBG_LOG

UT_METHOD(Log_DomainSubstitutions)
{
    UT_INIT()

    Log_AddDebugLogger()
    MemoryLogger ml;
    Log_SetVerbosity(&ml, Verbosity::Verbose )
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Info, Lox::InternalDomains )
    ml.GetFormatMetaInfo().Format.Reset("<%D>");
    Lox& lox=  *lox::Log::Get();

        LOG_CHECK( ""     , "</>"                    , ml,lox)
        LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)

    // wrong rules
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Warning, Lox::InternalDomains ) integer cntLogs= Log::DebugLogger->CntLogs;
    Log_SetDomainSubstitutionRule( "LOC"        , "X"       )  UT_TRUE( cntLogs + 1 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "*LOC/"      , "X"       )  UT_TRUE( cntLogs + 2 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "LOC/*"      , "X"       )  UT_TRUE( cntLogs + 3 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "LOC*"       , "X"       )  UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "*LOC*"      , "X"       )  UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "*/LOC*"     , "X"       )  UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs )
    Log_SetDomainSubstitutionRule( "*/LOC/*"    , "X"       )  UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs )

    Log_SetVerbosity(Log::DebugLogger, Verbosity::Info, Lox::InternalDomains )


    // Exact match
    Log_SetDomainSubstitutionRule( nullptr       , nullptr   )
    Log_SetDomainSubstitutionRule( "/LOC"        , "X"       )   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox)
    Log_SetDomainSubstitutionRule( "/LOC"        , ""        )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)
    Log_SetDomainSubstitutionRule( "/LO"         , "/X"      )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)
    Log_SetDomainSubstitutionRule( "/LOCX"       , "/X"      )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)

    Log_SetDomainSubstitutionRule( nullptr     , nullptr     )

    // postfix match
    Log_SetDomainSubstitutionRule( nullptr       , nullptr   )
    Log_SetDomainSubstitutionRule( "*/LOC"       , "X"       )   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox)
    Log_SetDomainSubstitutionRule( "*/LOC"       , ""        )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)
    Log_SetDomainSubstitutionRule( "*LOC"        , "X"       )   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox)
    Log_SetDomainSubstitutionRule( "*LOC"        , ""        )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)
    Log_SetDomainSubstitutionRule( "*C"          , "X"       )   LOG_CHECK( "LOC"  , "</LOX>"                 , ml,lox)
    Log_SetDomainSubstitutionRule( "*C"          , ""        )   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)

    Log_SetDomainSubstitutionRule( "*C"          , "XY"      )   LOG_CHECK( "LOC/BC" , "</LOC/BXY>"           , ml,lox)
    Log_SetDomainSubstitutionRule( "*C"          , ""        )   LOG_CHECK( "LOC/BC" , "</LOC/BC>"            , ml,lox)
    Log_SetDomainSubstitutionRule( "*/BC"        , "/"       )   LOG_CHECK( "LOC/BC" , "</LOC>"               , ml,lox)
    Log_SetDomainSubstitutionRule( "*/BC"        , ""        )   LOG_CHECK( "LOC/BC" , "</LOC/BC>"            , ml,lox)
    Log_SetDomainSubstitutionRule( "*C/BC"       , "/VE"     )   LOG_CHECK( "LOC/BC" , "</LO/VE>"             , ml,lox)

    Log_SetDomainSubstitutionRule( nullptr       , nullptr   )

    // prefix match
    #if ALOX_DBG_LOG_CI
        Log_SetDomainSubstitutionRule( nullptr     , nullptr     )
        Log_SetDomainSubstitutionRule( "/LOC*"       , "X"       )   LOG_CHECK( "LOC"  , "</X>"          , ml,lox)
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        )   LOG_CHECK( "LOC"  , "</LOC>"        , ml,lox)
        Log_SetDomainSubstitutionRule( "/LOC*"       , "/X"      )   LOG_CHECK( "LOC"  , "</X>"          , ml,lox)
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        )   LOG_CHECK( "LOC"  , "</LOC>"        , ml,lox)

        Log_SetDomain( "METH", Scope::Filename )            LOG_CHECK( "LOC"  , "</METH/LOC>"            , ml,lox)
        Log_SetDomainSubstitutionRule( "/LOC*"       , "/X"      )   LOG_CHECK( "LOC"  , "</METH/LOC>"   , ml,lox)
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        )   LOG_CHECK( "LOC"  , "</METH/LOC>"   , ml,lox)
        Log_SetDomainSubstitutionRule( "/METH*"      , "/X"      )   LOG_CHECK( "LOC"  , "</X/LOC>"      , ml,lox)
        Log_SetDomainSubstitutionRule( "/METH*"      , "/Y"      )   LOG_CHECK( "LOC"  , "</Y/LOC>"      , ml,lox)
        Log_SetDomainSubstitutionRule( "/METH*"      , "/A/B/C"  )   LOG_CHECK( "LOC"  , "</A/B/C/LOC>"  , ml,lox)
        Log_SetDomain( "", Scope::Filename )                LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox)
    #endif

    // recursive (just for test, not useful), breaks after 10 recursions
    Log_SetDomainSubstitutionRule( "/R*"         , "/RR"      )   LOG_CHECK( "/REC" , "</RRRRRRRRRRREC>" , ml,lox)
                                                                  LOG_CHECK( "/REC" , "</RRRRRRRRRRREC>" , ml,lox)
    Log_SetDomainSubstitutionRule( "/R*"         , "/S"       )   LOG_CHECK( "/R"   , "</S>"             , ml,lox)
    Log_SetDomainSubstitutionRule( "/S*"         , "/R"       )   LOG_CHECK( "/R"   , "</R>"             , ml,lox)
    Log_SetDomainSubstitutionRule( "/S*"         , "/T"       )   LOG_CHECK( "/R"   , "</T>"             , ml,lox)

    //Log_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

    // substring rule
    Log_SetDomainSubstitutionRule( "*B*"         , "X"        )   LOG_CHECK( "ABC"  , "</AXC>"           , ml,lox)
    Log_SetDomainSubstitutionRule( "*B*"         , ""         )   LOG_CHECK( "ABC"  , "</ABC>"           , ml,lox)
    // substring rule
    Log_SetDomainSubstitutionRule( "*/ABC*"      , "DEF"      )   LOG_CHECK( "ABC"  , "</DEF>"           , ml,lox)
    Log_SetDomainSubstitutionRule( "*EF*"        , "ZZZ"      )   LOG_CHECK( "ABC"  , "</DZZZ>"          , ml,lox)
    Log_SetDomainSubstitutionRule( "*Z*"         , "EE"       )   LOG_CHECK( "ABC"  , "</DEEEEEE>"       , ml,lox)

    Log_SetDomainSubstitutionRule( "*/Q*"        , "V"        )   LOG_CHECK( "Q"    , "</V>"             , ml,lox)

                                                                   LOG_CHECK( "/_/ABC", "</_DEEEEEE>"    , ml,lox)

    // delete all rules
    Log_SetDomainSubstitutionRule( nullptr       , nullptr    )  LOG_CHECK( "/_/abc", "</_/###>"         , ml,lox)
                                                                 LOG_CHECK( "Q"     , "</Q>"             , ml,lox)
                                                                 LOG_CHECK( "ABC"   , "</ABC>"           , ml,lox)

    //Log_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

    Log_RemoveLogger( &ml )
}
#endif

/** ********************************************************************************************
 * Log_DomainSubstitutions_IniFile
 **********************************************************************************************/
#if ALOX_DBG_LOG
// SNIPPET FOR Dox
#if defined(NEVER_DEFINED)

DOX_MARKER( [Man_DomSubst_Config])
[ALOX]
MYLOX_DOMAIN_SUBSTITUTION= /A_DOM -> /BETTER_NAME   ; \
                           /UI    -> /LIBS/UI
DOX_MARKER( [Man_DomSubst_Config])

DOX_MARKER( [Man_DomSubst_Config_Prevent])
// Note: the name will be converted to upper case for searching configuration settings
Lox myLox( "MyLox" );

// clear rules which eventually just got read from external configuration
myLox.SetDomainSubstitutionRule( null, null );
DOX_MARKER( [Man_DomSubst_Config_Prevent])
#endif

UT_METHOD(Log_DomainSubstitutions_IniFile)
{
    UT_INIT()

    // create INI file
    const char* iniFileContents=
        "[ALOX/TESTMEMLOGGER]\n"
        "FORMAT= \"<%D>\"\n"
        "[ALOX/MYLOX]\n"
         "DOMAIN_SUBSTITUTION = /A_DOM -> /BETTER_NAME  ;\\ \n"
                       "        /UI    -> /LIBS/UI    \n"
         "x\n";

    Path fileName(SystemFolders::Current);
    fileName._("/unittest_testiniFile.cfg");

    // write sample config file
    {
        std::ofstream iniFile;
        ALIB_STRINGS_TO_NARROW(fileName,nFileName,1024)
        iniFile.open ( nFileName );
        iniFile << iniFileContents;
        iniFile.close();
    }

    // feed to config
    IniFileFeeder iniFile(ALOX.GetConfig());
    { ALIB_LOCK_WITH(ALOX.GetConfigLock())
        iniFile.ImportStart( fileName );
        iniFile.ImportAll();
        iniFile.ImportEnd();
    }

    // create lox, loggers
    Lox myLox( "MyLox" ); // name will be upper case
    { ALIB_OWN(myLox)

        Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
        myLox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
        myLox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, Lox::InternalDomains );

        MemoryLogger ml("TESTMEMLOGGER");
        myLox.SetVerbosity ( &ml, Verbosity::Verbose );

        LOG_CHECK( "DOM"    , "</DOM>"              , ml,myLox)
        LOG_CHECK( "A_DOM"  , "</BETTER_NAME>"      , ml,myLox)
        LOG_CHECK( "UI"     , "</LIBS/UI>"          , ml,myLox)

        //myLox.State( "", Verbosity.Info, "Configuration now is:" );

        myLox.RemoveLogger( &ml );
        myLox.RemoveLogger( "CONSOLE" );
        delete consoleLogger;
    }
}
#endif

/** ********************************************************************************************
 * Log_Domain_IniFile
 **********************************************************************************************/
#if ALOX_REL_LOG

UT_METHOD(Log_Domain_IniFile)
{
    UT_INIT()

    // Without priorities
    {ALIB_LOCK_WITH(ALOX.GetConfigLock())
        // create variables
        Variable var(ALOX);
        Box replacements[2]= { "T_LOX", "TESTML"  };
        var.Declare( lox::Variables::FORMAT, replacements );
        if( var.Define(alib::config::Priority::ConfigFile) )
            var.Import( A_CHAR("%Sp" ), alib::config::Priority::ConfigFile );

        var.Declare( lox::Variables::VERBOSITY, replacements);
        if( var.Define(alib::config::Priority::ConfigFile) )
            var.Import( A_CHAR("/DOM_VERB  = VerboseXX  ;" // xx is allowed!
                               "/DOM_INFO  = Info       ;"
                               "/DOM_WARN  = WARNING    ;"
                               "/DOM_ERR   = erRor      ;"
                               "/DOM_OFF   = off        ;"
                               "/ATSTART*  = Info       ;"
                               "*ATEND     = Info       ;"
                               "*SUBSTR*   = Info       ;"
                               "/OVERWRITE = Info       ;") ,
                        alib::config::Priority::ConfigFile  );
    }

    // test
    Lox lox("T_LOX", false);
    lox.Acquire(ALIB_CALLER);
        Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");

        lox.SetVerbosity( consoleLogger, Verbosity::Verbose, "CONSOLE" );
        lox.SetVerbosity( "CONSOLE"    , Verbosity::Verbose, Lox::InternalDomains );

        // pre-create one of the domains to test if loggers added later get config for existing domains
        lox.Verbose( "DOM_INFO"     , "test" );

        MemoryLogger ml("TESTML");
        lox.SetVerbosity( &ml, Verbosity::Off );

        lox.Info   (                  "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info                 ))
        lox.Error  ( "NOSETTING"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Error  , "NOSETTING" ))

        lox.Verbose( "DOM_VERB"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Verbose, "DOM_VERB"  ))

        lox.Verbose( "DOM_INFO"     , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "DOM_INFO"  ))
        lox.Info   ( "DOM_INFO"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "DOM_INFO"  ))
        lox.Info   ( "DOM_WARN"     , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info   , "DOM_WARN"  ))
        lox.Warning( "DOM_WARN"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Warning, "DOM_WARN"  ))

        lox.Warning( "DOM_ERR"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Warning, "DOM_ERR"   ))
        lox.Error  ( "DOM_ERR"      , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Error  , "DOM_ERR"   ))

        lox.Error  ( "DOM_OFF"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Error  , "DOM_OFF"   ))

        lox.Verbose( "ATSTART"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "ATSTART"   ))
        lox.Info   ( "ATSTART"      , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "ATSTART"   ))
        lox.Verbose( "ATSTARTXX"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "ATSTARTXX" ))
        lox.Info   ( "ATSTARTXX"    , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "ATSTARTXX" ))
        lox.Verbose( "XXATSTART"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XXATSTART" ))
        lox.Info   ( "XXATSTART"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info   , "XXATSTART" ))
        lox.Verbose( "XATSTARTX"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XATSTARTX" ))
        lox.Info   ( "XATSTARTX"    , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info   , "XATSTARTX" ))

        lox.Verbose( "ATEND"        , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "ATEND"     ))
        lox.Info   ( "ATEND"        , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "ATEND"     ))
        lox.Verbose( "ATENDXX"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "ATENDXX"   ))
        lox.Info   ( "ATENDXX"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info   , "ATENDXX"   ))
        lox.Verbose( "XXATEND"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XXATEND"   ))
        lox.Info   ( "XXATEND"      , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "XXATEND"   ))
        lox.Verbose( "XATENDX"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XATENDX"   ))
        lox.Info   ( "XATENDX"      , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info   , "XATENDX"   ))


        lox.Verbose( "SUBSTR"       , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "SUBSTR"    ))
        lox.Info   ( "SUBSTR"       , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "SUBSTR"    ))
        lox.Verbose( "SUBSTRXX"     , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "SUBSTRXX"  ))
        lox.Info   ( "SUBSTRXX"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "SUBSTRXX"  ))
        lox.Verbose( "XXSUBSTR"     , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XXSUBSTR"  ))
        lox.Info   ( "XXSUBSTR"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "XXSUBSTR"  ))
        lox.Verbose( "XSUBSTRX"     , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose, "XSUBSTRX"  ))
        lox.Info   ( "XSUBSTRX"     , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info   , "XSUBSTRX"  ))

        // overwrite config
        lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose   , "/OVERWRITE"  ))
        lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info      , "/OVERWRITE"  ))

        lox.SetVerbosity( &ml , Verbosity::Warning, "/OVERWRITE" ); // does not overwrite
        lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose   , "/OVERWRITE"  ))
        lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info      , "/OVERWRITE"  ))

        lox.SetVerbosity( &ml , Verbosity::Warning, "/OVERWRITE", Priority::Protected -1 ); // does overwrite
        lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose   , "/OVERWRITE"  ))
        lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info      , "/OVERWRITE"  ))
        lox.Warning( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Warning   , "/OVERWRITE"  ))

        // overwrite non-config
        lox.Error  ( "/A"           , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Error   , "/A"       ))
        lox.Error  ( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Error   , "/A/B"     ))
        lox.Error  ( "/A/C"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Error   , "/A/C"     ))

        lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priority::Standard -1 ); // does not overwrite
        lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose , "/A/B"     ))
        lox.Info   ( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Info    , "/A/B"     ))

        lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priority::Standard ); // does overwrite
        lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose , "/A/B"     ))
        lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info    , "/A/B"     ))

        lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priority::Standard + 1 ); // one higher
        lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose , "/A/B"     ))
        lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info    , "/A/B"     ))

        lox.SetVerbosity( &ml , Verbosity::Verbose, "/A" );
        lox.Verbose( "/A"           , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Verbose , "/A"       ))
        lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(0, lox.IsActive( Verbosity::Verbose , "/A/B"     ))
        lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Info    , "/A/B"     ))
        lox.Verbose( "/A/C"         , "test" );    UT_EQ(  1, ml.CntLogs )  ml.CntLogs= 0;    UT_EQ(1, lox.IsActive( Verbosity::Verbose , "/A/C"     ))

        //lox.State( "/CONSOLE", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.GetAutoSizes().Main.Reset();

        lox.RemoveLogger( &ml );
        lox.RemoveLogger( "CONSOLE" );
        delete consoleLogger;
    lox.Release();
}

#endif



#include "unittests/aworx_unittests_end.hpp"


} // namespace


#endif // ALIB_UT_ALOX

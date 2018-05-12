// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "alib/compatibility/std_string.hpp"

#include "alib/alox/loggers/memorylogger.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#if !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#define TESTCLASSNAME       CPP_ALox_Log_Domains
#include "../aworx_unittests.hpp"

using namespace std;
using namespace ut_aworx;

using namespace aworx;
using namespace aworx::lib::lox::core::textlogger;


namespace ut_alox {

// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest2();
void ScopeInfoCacheTest2() { Log_Info("Test Method 2"); }

#if ALIB_DEBUG
#define LOG_CHECK( d, s, ml,lox )    {                  \
        ml.MemoryLog._();                               \
        ml.AutoSizes.Reset();                           \
        lox.Acquire(ALIB_CALLER);                       \
        lox.GetLogableContainer().Add("");              \
        lox.Entry( d, Verbosity::Info );                \
        lox.Release();                                  \
        UT_EQ(ASTR(s), ml .MemoryLog);                  \
        }
#endif


/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

// with GTEST macros it all gets wild. Fix the method name
#undef  ALIB_CALLER
#define ALIB_CALLER     __FILE__, __LINE__, UT_GET_TEST_NAME

UT_CLASS()



/** ********************************************************************************************
 * Lox_IllegalDomainNames
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
UT_METHOD(Lox_IllegalDomainNames)
{
    UT_INIT();

    Log_AddDebugLogger();
    MemoryLogger ml;
    ml.MetaInfo->Format._()._("<%D>");
    Log_SetVerbosity(&ml, Verbosity::Verbose );
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Verbose, ALox::InternalDomains );

    Lox& lox=  *lib::ALOX.Log();
    LOG_CHECK( ""        , "</>"              , ml,lox);
    LOG_CHECK( "LOC"     , "</LOC>"           , ml,lox);
    LOG_CHECK( "%"       , "</#>"             , ml,lox);
    LOG_CHECK( "\x03"    , "</#>"             , ml,lox);
    LOG_CHECK( "<"       , "</#>"             , ml,lox);
    LOG_CHECK( ">"       , "</#>"             , ml,lox);
    LOG_CHECK( "?"       , "</#>"             , ml,lox);
    LOG_CHECK( ","       , "</#>"             , ml,lox);
    LOG_CHECK( "-"       , "</->"             , ml,lox);
    LOG_CHECK( "_"       , "</_>"             , ml,lox);
    LOG_CHECK( "@"       , "</#>"             , ml,lox);
    LOG_CHECK( "."       , "</>"              , ml,lox);
    LOG_CHECK( ".."      , "</>"              , ml,lox);
    LOG_CHECK( "CU.."    , "</CU##>"          , ml,lox);

    LOG_CHECK( "$%! "    , "</####>"          , ml,lox);

    Log_SetDomain( "METH", Scope::Method );
    LOG_CHECK( "$%! "    , "</METH/####>"     , ml,lox);

    Log_SetDomain( "A\"C" , Scope::Method );
    LOG_CHECK( ""        , "</A#C>"           , ml,lox);
    LOG_CHECK( "LOC"     , "</A#C/LOC>"       , ml,lox);
    LOG_CHECK( "*X*"     , "</A#C/#X#>"       , ml,lox);

    Log_RemoveLogger( &ml );
}
#endif

/** ********************************************************************************************
 * Lox_DomainsRelative
 **********************************************************************************************/
#if ALOX_REL_LOG_CI
UT_METHOD(Lox_DomainsRelative)
{
    UT_INIT();
    Lox lox("ReleaseLox");
    #define LOX_LOX lox
    MemoryLogger ml;

    Lox_SetVerbosity ( &ml, Verbosity::Verbose );
    ml.MetaInfo->Format._()._("@%D#");
    Lox_SetDomain( "/D1/D2/D3", Scope::ThreadOuter );

    Lox_Info( "D4"                 , "" ); UT_EQ(  "@/D1/D2/D3/D4#"        , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
    Lox_Info( "./D4"               , "" ); UT_EQ(  "@/D1/D2/D3/D4#"        , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
    Lox_Info( "../D4"              , "" ); UT_EQ(  "@/D1/D2/D4#"           , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
    Lox_Info( ".././.././D4"       , "" ); UT_EQ(  "@/D1/D4#"              , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
    Lox_Info( "../../../../../D4"  , "" ); UT_EQ(  "@/D4#"                 , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();
    Lox_Info( "../D4/../D5"        , "" ); UT_EQ(  "@/D1/D2/D5#"           , ml.MemoryLog ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Lox_RemoveLogger( &ml );

    #undef LOX_LOX
}
#endif


/** ********************************************************************************************
 * Log_DomainSubstitutions
 **********************************************************************************************/
#if ALOX_DBG_LOG

UT_METHOD(Log_DomainSubstitutions)
{
    UT_INIT();

    Log_AddDebugLogger();
    MemoryLogger ml;
    ml.MetaInfo->Format._()._("<%D>");
    Log_SetVerbosity(&ml, Verbosity::Verbose );
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Info, ALox::InternalDomains );
    Lox& lox=  *lib::ALOX.Log();


        LOG_CHECK( ""     , "</>"                    , ml,lox);
        LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);

    // wrong rules
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Warning, ALox::InternalDomains ); int cntLogs= Log::DebugLogger->CntLogs;
    Log_SetDomainSubstitutionRule( "LOC"        , "X"       );   ;                        UT_TRUE( cntLogs + 1 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "*LOC/"      , "X"       );   ;                        UT_TRUE( cntLogs + 2 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "LOC/*"      , "X"       );   ;                        UT_TRUE( cntLogs + 3 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "LOC*"       , "X"       );   ;                        UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "*LOC*"      , "X"       );   ;                        UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "*/LOC*"     , "X"       );   ;                        UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs );
    Log_SetDomainSubstitutionRule( "*/LOC/*"    , "X"       );   ;                        UT_TRUE( cntLogs + 4 == Log::DebugLogger->CntLogs );

    Log_SetVerbosity(Log::DebugLogger, Verbosity::Info, ALox::InternalDomains );


    // Exact match
    Log_SetDomainSubstitutionRule( nullptr       , nullptr    );
    Log_SetDomainSubstitutionRule( "/LOC"        , "X"       );   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox);
    Log_SetDomainSubstitutionRule( "/LOC"        , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "/LO"         , "/X"      );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "/LOCX"       , "/X"      );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);

    Log_SetDomainSubstitutionRule( nullptr     , nullptr    );

    // postfix match
    Log_SetDomainSubstitutionRule( nullptr       , nullptr    );
    Log_SetDomainSubstitutionRule( "*/LOC"       , "X"       );   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox);
    Log_SetDomainSubstitutionRule( "*/LOC"       , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "*LOC"        , "X"       );   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox);
    Log_SetDomainSubstitutionRule( "*LOC"        , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "*C"          , "X"       );   LOG_CHECK( "LOC"  , "</LOX>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "*C"          , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);

    Log_SetDomainSubstitutionRule( "*C"          , "XY"      );   LOG_CHECK( "LOC/BC" , "</LOC/BXY>"           , ml,lox);
    Log_SetDomainSubstitutionRule( "*C"          , ""        );   LOG_CHECK( "LOC/BC" , "</LOC/BC>"            , ml,lox);
    Log_SetDomainSubstitutionRule( "*/BC"        , "/"       );   LOG_CHECK( "LOC/BC" , "</LOC>"               , ml,lox);
    Log_SetDomainSubstitutionRule( "*/BC"        , ""        );   LOG_CHECK( "LOC/BC" , "</LOC/BC>"            , ml,lox);
    Log_SetDomainSubstitutionRule( "*C/BC"       , "/VE"     );   LOG_CHECK( "LOC/BC" , "</LO/VE>"             , ml,lox);

    Log_SetDomainSubstitutionRule( nullptr     , nullptr    );

    // prefix match
    #if ALOX_DBG_LOG_CI
        Log_SetDomainSubstitutionRule( nullptr     , nullptr    );
        Log_SetDomainSubstitutionRule( "/LOC*"       , "X"       );   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox);
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
        Log_SetDomainSubstitutionRule( "/LOC*"       , "/X"      );   LOG_CHECK( "LOC"  , "</X>"                   , ml,lox);
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        );   LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);

        Log_SetDomain( "METH", Scope::Filename );           LOG_CHECK( "LOC"  , "</METH/LOC>"            , ml,lox);
        Log_SetDomainSubstitutionRule( "/LOC*"       , "/X"      );   LOG_CHECK( "LOC"  , "</METH/LOC>"            , ml,lox);
        Log_SetDomainSubstitutionRule( "/LOC*"       , ""        );   LOG_CHECK( "LOC"  , "</METH/LOC>"            , ml,lox);
        Log_SetDomainSubstitutionRule( "/METH*"      , "/X"      );   LOG_CHECK( "LOC"  , "</X/LOC>"               , ml,lox);
        Log_SetDomainSubstitutionRule( "/METH*"      , "/Y"      );   LOG_CHECK( "LOC"  , "</Y/LOC>"               , ml,lox);
        Log_SetDomainSubstitutionRule( "/METH*"      , "/A/B/C"  );   LOG_CHECK( "LOC"  , "</A/B/C/LOC>"           , ml,lox);
        Log_SetDomain( "", Scope::Filename );               LOG_CHECK( "LOC"  , "</LOC>"                 , ml,lox);
    #endif

    // recursive (just for test, not useful), breaks after 10 recursions
    Log_SetDomainSubstitutionRule( "/R*"         , "/RR"      );   LOG_CHECK( "/REC" , "</RRRRRRRRRRREC>"       , ml,lox);
                                                                   LOG_CHECK( "/REC" , "</RRRRRRRRRRREC>"       , ml,lox);
    Log_SetDomainSubstitutionRule( "/R*"         , "/S"       );   LOG_CHECK( "/R"   , "</S>"                   , ml,lox);
    Log_SetDomainSubstitutionRule( "/S*"         , "/R"       );   LOG_CHECK( "/R"   , "</R>"                   , ml,lox);
    Log_SetDomainSubstitutionRule( "/S*"         , "/T"       );   LOG_CHECK( "/R"   , "</T>"                   , ml,lox);

    //Log_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    // substring rule
    Log_SetDomainSubstitutionRule( "*B*"         , "X"        );   LOG_CHECK( "ABC"  , "</AXC>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "*B*"         , ""         );   LOG_CHECK( "ABC"  , "</ABC>"                 , ml,lox);
    // substring rule
    Log_SetDomainSubstitutionRule( "*/ABC*"      , "DEF"      );   LOG_CHECK( "ABC"  , "</DEF>"                 , ml,lox);
    Log_SetDomainSubstitutionRule( "*EF*"        , "ZZZ"      );   LOG_CHECK( "ABC"  , "</DZZZ>"                , ml,lox);
    Log_SetDomainSubstitutionRule( "*Z*"         , "EE"       );   LOG_CHECK( "ABC"  , "</DEEEEEE>"             , ml,lox);

    Log_SetDomainSubstitutionRule( "*/Q*"        , "V"        );   LOG_CHECK( "Q"    , "</V>"                   , ml,lox);

                                                                   LOG_CHECK( "/_/ABC", "</_DEEEEEE>"           , ml,lox);

    // delete all rules
    Log_SetDomainSubstitutionRule( nullptr     , nullptr    );   LOG_CHECK( "/_/abc", "</_/###>"              , ml,lox);
                                                                 LOG_CHECK( "Q"     , "</Q>"                  , ml,lox);
                                                                 LOG_CHECK( "ABC"   , "</ABC>"                , ml,lox);

    //Log_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Log_RemoveLogger( &ml );
}
#endif

/** ********************************************************************************************
 * Log_DomainSubstitutions_IniFile
 **********************************************************************************************/
#if ALOX_DBG_LOG

UT_METHOD(Log_DomainSubstitutions_IniFile)
{
    UT_INIT();

    // create INI file
    const char* iniFileContents=
        "[ALOX]\n"
         "TESTMEMLOGGER_FORMAT= \"<%D>\"  \n"
         "MYLOX_DOMAIN_SUBSTITUTION = /A_DOM -> /BETTER_NAME  ;\\ \n"
                                 "    /UI    -> /LIBS/UI    \n"
         "x\n";

    AString fileName;
    Directory::CurrentDirectory( fileName );
    fileName._("/unittest_testiniFile.cfg");

    // write sample config file
    {
        std::ofstream iniFile;
        ALIB_STD_TO_NARROW_TSTRING(fileName,nFileName)
        iniFile.open ( nFileName.ToCString() );
        iniFile << iniFileContents;
        iniFile.close();
    }

    aworx::IniFile iniFile( fileName );
    iniFile.ReadFile();

    // add to config
    lib::ALOX.Config->InsertPlugin( &iniFile, Priorities::Standard );

    // create lox, loggers
    Lox myLox( "MyLox" ); // name will be upper case
    myLox.Acquire(ALIB_CALLER);

        Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");
        myLox.SetVerbosity( "CONSOLE" , Verbosity::Verbose );
        myLox.SetVerbosity( "CONSOLE" , Verbosity::Verbose, ALox::InternalDomains );

        MemoryLogger ml("TESTMEMLOGGER");
        myLox.SetVerbosity ( &ml, Verbosity::Verbose );

        LOG_CHECK( "DOM"    , "</DOM>"              , ml,myLox);
        LOG_CHECK( "A_DOM"  , "</BETTER_NAME>"      , ml,myLox);
        LOG_CHECK( "UI"     , "</LIBS/UI>"          , ml,myLox);

        //myLox.State( "", Verbosity.Info, "Configuration now is:" );

        myLox.RemoveLogger( &ml );
        myLox.RemoveLogger( "CONSOLE" );
        delete consoleLogger;
    myLox.Release();
    lib::ALOX.Config->RemovePlugin( &iniFile );
}
#endif

/** ********************************************************************************************
 * Log_Domain_IniFile
 **********************************************************************************************/
#if ALOX_REL_LOG

UT_METHOD(Log_Domain_IniFile)
{
    UT_INIT();

    // Without priorities
    {
        // create iniFile
        IniFile iniFile(ASTR("*")); // don't read
        Variable var;
        iniFile.Store( var.Declare( ASTR("ALOX"), ASTR("TESTML_FORMAT")),  ASTR("%Sp") );
        iniFile.Store( var.Declare( ASTR("ALOX"), ASTR("T_LOX_TESTML_VERBOSITY"),';'),
                    ASTR("/DOM_VERB  = VerboseXX  ;" // xx is allowed!
                         "/DOM_INFO  = Info       ;"
                         "/DOM_WARN  = WARNING    ;"
                         "/DOM_ERR   = erRor      ;"
                         "/DOM_OFF   = off        ;"
                         "/ATSTART*  = Info       ;"
                         "*ATEND     = Info       ;"
                         "*SUBSTR*   = Info       ;"
                         "/OVERWRITE = Info       ;")
                    );
        lib::ALOX.Config->InsertPlugin( &iniFile, Priorities::Standard );


        // test
        Lox lox("T_LOX", false);
        lox.Acquire(ALIB_CALLER);
            Logger* consoleLogger= Lox::CreateConsoleLogger("CONSOLE");

            lox.SetVerbosity( consoleLogger, Verbosity::Verbose, "CONSOLE" );
            lox.SetVerbosity( "CONSOLE"    , Verbosity::Verbose, ALox::InternalDomains );

            // pre-create one of the domains to test if loggers added later get config for existing domains
            lox.Verbose( "DOM_INFO"     , "test" );

            MemoryLogger ml("TESTML");
            lox.SetVerbosity( &ml, Verbosity::Off );

            lox.Info   (                  "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Error  ( "NOSETTING"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;

            lox.Verbose( "DOM_VERB"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.Verbose( "DOM_INFO"     , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "DOM_INFO"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.Info   ( "DOM_WARN"     , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Warning( "DOM_WARN"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.Warning( "DOM_ERR"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Error  ( "DOM_ERR"      , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.Error  ( "DOM_OFF"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;

            lox.Verbose( "ATSTART"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "ATSTART"      , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "ATSTARTXX"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "ATSTARTXX"    , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XXATSTART"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XXATSTART"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XATSTARTX"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XATSTARTX"    , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;

            lox.Verbose( "ATEND"        , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "ATEND"        , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "ATENDXX"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "ATENDXX"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XXATEND"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XXATEND"      , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XATENDX"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XATENDX"      , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;


            lox.Verbose( "SUBSTR"       , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "SUBSTR"       , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "SUBSTRXX"     , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "SUBSTRXX"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XXSUBSTR"     , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XXSUBSTR"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "XSUBSTRX"     , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "XSUBSTRX"     , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            // overwrite config
            lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Warning, "/OVERWRITE" ); // does not overwrite
            lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Warning, "/OVERWRITE", Priorities::ProtectedValues -1 ); // does overwrite
            lox.Verbose( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/OVERWRITE"   , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Warning( "/OVERWRITE"   , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;


            // overwrite non-config
            lox.Error  ( "/A"           , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Error  ( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Error  ( "/A/C"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priorities::DefaultValues -1 ); // does not overwrite
            lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priorities::DefaultValues ); // does overwrite
            lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Info, "/A/B", Priorities::DefaultValues + 1 ); // one higher
            lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            lox.SetVerbosity( &ml , Verbosity::Verbose, "/A" );
            lox.Verbose( "/A"           , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "/A/B"         , "test" );    UT_EQ(  0, ml.CntLogs ); ml.CntLogs= 0;
            lox.Info   ( "/A/B"         , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;
            lox.Verbose( "/A/C"         , "test" );    UT_EQ(  1, ml.CntLogs ); ml.CntLogs= 0;

            //lox.State( "/CONSOLE", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

            lib::ALOX.Config->RemovePlugin( &iniFile );
            lox.RemoveLogger( &ml );
            lox.RemoveLogger( "CONSOLE" );
            delete consoleLogger;
        lox.Release();
    }
}
#endif

UT_CLASS_END


} // namespace



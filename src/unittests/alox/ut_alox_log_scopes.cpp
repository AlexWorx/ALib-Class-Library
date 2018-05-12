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

// conditional expression is constant (which is a great thing for the compiler by the way ;-)
#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4127 )
#endif


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#if !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif

#define TESTCLASSNAME       CPP_ALox_Log_Scopes
#include "../aworx_unittests.hpp"

using namespace std;
using namespace ut_aworx;

using namespace aworx;
using namespace aworx::lib::lox::core::textlogger;


namespace ut_alox {

// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest4();
void ScopeInfoCacheTest4() { Log_Info("Test Method 4"); }

void LSD()      ;
void LSD_A()    ;
void LSD_A_B()  ;
void LSD2_A_B() ;
void LSD2_A()   ;
void LSD2()     ;
void LSD()      {  Log_SetDomain( "LSD",  Scope::Method );    Log_Info( "" );    }
void LSD_A()    {  Log_SetDomain( "A",    Scope::Method );    Log_Info( "" );    }
void LSD_A_B()  {  Log_SetDomain( "B",    Scope::Method );    Log_Info( "" );    }
void LSD2_A_B() {  Log_SetDomain( "B2",   Scope::Method );    Log_Info( "" );    }
void LSD2_A()   {  Log_SetDomain( "A2",   Scope::Method );    Log_Info( "" );    }
void LSD2()     {  Log_SetDomain( "LSD2", Scope::Method );    Log_Info( "" );    }


#if ALOX_DBG_LOG
    extern void Log_ScopeDomains_Helper();
    extern void Log_ScopeDomains_Helper2();
    void Log_ScopeDomains_Helper2B();
    void Log_ScopeDomains_Helper2B()
    {
        Log_Info("");
    }

    class DomainTestThread : public Thread
    {
        virtual void Run()
        {
            Log_Info( "DTT", "" );
        }
    };

    class LogOnceTestThread : public Thread
    {
        virtual void Run()
        {
            Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - 2nd thread", Scope::ThreadOuter, 2 );
            Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - 2nd thread", Scope::ThreadOuter, 2 );
            Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - 2nd thread", Scope::ThreadOuter, 2 );
            Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - 2nd thread", Scope::ThreadOuter, 2 );
        }
    };

    class StoreDataTestThread : public Thread
    {
        public:
        AWorxUnitTesting& ut;
        StoreDataTestThread( AWorxUnitTesting& pUT ) :ut(pUT) {}
        virtual void Run()
        {

            Log_Store( "2nd Thread Data"       ,                   Scope::ThreadOuter   );
            Log_Store( "2nd Thread Data, keyed",   ASTR("mykey"),  Scope::ThreadOuter   );

            { Log_Retrieve( data,                 Scope::ThreadOuter ); UT_TRUE( data.Unbox<NString>().Equals( "2nd Thread Data"        ) ); }
            { Log_Retrieve( data,  ASTR("mykey"), Scope::ThreadOuter ); UT_TRUE( data.Unbox<NString>().Equals( "2nd Thread Data, keyed" ) ); }
        }
    };

#endif

#if ALOX_REL_LOG
    extern void Lox_ScopeDomains_Helper( Lox& lox );

    class DomainTestThreadRL : public Thread
    {
        public:
        Lox* lox;
        DomainTestThreadRL( Lox* pLox ) { this->lox= pLox; }
        virtual void Run()
        {
            #define LOX_LOX (*lox)
                Lox_Info( "DTT", "" );
            #undef LOX_LOX
        }
    };
#endif

#include "ut_alox_log_scopes.hpp"

/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

// with GTEST macros it all gets wild. Fix the method name
#undef  ALIB_CALLER
#define ALIB_CALLER     __FILE__, __LINE__, UT_GET_TEST_NAME

UT_CLASS()


/** ********************************************************************************************
 * Log_LineFormat
 **********************************************************************************************/
#if ALOX_DBG_LOG
UT_METHOD(Log_LineFormat)
{
    UT_INIT();

    Log_AddDebugLogger();
    Log_SetVerbosity  ( Log::DebugLogger, Verbosity::Off );
    Log_Prune( MemoryLogger* testML= new MemoryLogger(); )
    Log_SetVerbosity  ( testML, Verbosity::Off );

    Log_SetDomain( "FMT", Scope::Method );
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose);
    Log_SetVerbosity( testML,           Verbosity::Verbose);

    Log_Info( "This is the default ConsoleLogger meta info" );

    String64  lf;
    lf= "%SF(%SL):%SM()%A3[%D][%TD][%TC +%TL][%tN]%V[%D]<%#>: ";   Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%D][%TD][%TC +%TL][%tN]%V[%D]<%#>: ";        Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%TD][%TC +%TL][%tN]%V[%D]<%#>: ";            Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%TC +%TL][%tN]%V[%D]<%#>: ";                 Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[+%TL][%tN]%V[%D]<%#>: ";                     Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%tN]%V[%D]<%#>: ";                           Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3%V[%D]<%#>: ";                                Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%D]<%#>: ";                                  Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF(%SL):%A3[%D]: ";                                      Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "%SF:%A3[%D]: ";                                           Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "[%D]: ";                                                  Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );
    lf= "";                                                        Log::DebugLogger->MetaInfo->Format= lf;    Log_Info( String128("LineFormat set to= \"") << lf << '\"' );

    #if ALOX_DBG_LOG
        Log::DebugLogger->MetaInfo->Format= "%TD@";
                  testML->MetaInfo->Format= "%TD@";
        const char* df;
        df= ">yy-MM-dd<";    Log::DebugLogger->MetaInfo->DateFormat= df;                                                Log_Info( String128("Date test. Format: \"") << df << '\"' );
        testML->MemoryLog.Clear();
        df= ">yyyy/dd/MM<";  Log::DebugLogger->MetaInfo->DateFormat= df;  testML->MetaInfo->DateFormat= df;             Log_Info( "FMT", String128("Date test. Format: \"") << df << '\"' );
        UT_TRUE( testML->MemoryLog.SearchAndReplace( '/', '@') == 4 );
        Log::DebugLogger->MetaInfo->Format= "%TT@";
                  testML->MetaInfo->Format= "%TT@";
        df= ">HH:mm:ss<";    Log::DebugLogger->MetaInfo->TimeOfDayFormat= df;                                           Log_Info( "FMT", String128("Time of day test Format: \"") << df << '\"' );
        testML->MemoryLog.Clear();
        df= ">HH-mm-ss<";    Log::DebugLogger->MetaInfo->TimeOfDayFormat= df;  testML->MetaInfo->TimeOfDayFormat= df;   Log_Info( "FMT", String128("Time of day test. Format: \"") << df << '\"' );
        UT_TRUE( testML->MemoryLog.SearchAndReplace( '-', '@') == 4 );

        Log::DebugLogger->MetaInfo->Format= "%tI@";
                  testML->MetaInfo->Format= "%tI@";
        testML->MemoryLog.Clear();   testML->AutoSizes.Reset();
        Log_Info("");
        UT_EQ( ASTR("-1@"), testML->MemoryLog );

        Log::DebugLogger->MetaInfo->Format=
                  testML->MetaInfo->Format= ASTR("%P");
        #if defined( _WIN32 )
            testML->MemoryLog.Clear();  testML->AutoSizes.Reset(); Log_Info("");     UT_TRUE(    testML->MemoryLog.Equals(ASTR("te.processhost.managed.exe"))
                                                                                              || testML->MemoryLog.Equals(ASTR("testhost.exe")              )
                                                                                              || testML->MemoryLog.Equals(ASTR("vstest.executionengine.exe"))
                                                                                              || testML->MemoryLog.Equals(ASTR("vstest.executionengine.x86.exe")));
        #else
            testML->MemoryLog.Clear();  testML->AutoSizes.Reset(); Log_Info("");     UT_TRUE(       testML->MemoryLog.Equals( ASTR("ALib_UT"))
                                                                                                ||  testML->MemoryLog.StartsWith( ASTR("QTC_ALox_UnitTe") )  // QMake project
                                                                                                ||  testML->MemoryLog.StartsWith( ASTR("memcheck-") )         // valgrind
                                                                                            );
        #endif


        Log::DebugLogger->MetaInfo->Format=
        testML->MetaInfo->Format=               ASTR("%LX");
        testML->MemoryLog.Clear();  testML->AutoSizes.Reset(); Log_Info("");     UT_EQ( ASTR("LOG")         , testML->MemoryLog );

        Log::DebugLogger->MetaInfo->Format=
        testML->MetaInfo->Format=               ASTR("%LG");
        testML->MemoryLog.Clear();  testML->AutoSizes.Reset(); Log_Info("");     UT_EQ( ASTR("MEMORY")       , testML->MemoryLog );
    #endif

    Log_RemoveLogger( testML );
    Log_Prune( delete testML );
}
#endif


/** ********************************************************************************************
 * Log_Prefix
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
#define PFXCHECK( s, ml )        {                            \
        Log_Info( "*msg*" );                                  \
        UT_EQ(ASTR(s), ml .MemoryLog); ml .MemoryLog._();     \
        }                                                     \

UT_METHOD(Log_Prefix)
{
    UT_INIT();
    aworx::lib::boxing::InitStdString();

    // we have to clear  all trim rules and set a new one to have a longer path
    Log_ClearSourcePathTrimRules(Reach::Global, false );
    Log_SetSourcePathTrimRule( "*/alox/src/", Inclusion::Exclude );


    Log_AddDebugLogger();
    MemoryLogger ml;
    ml.MetaInfo->Format._();
    Log_SetVerbosity(&ml, Verbosity::Verbose );
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Verbose, ALox::InternalDomains );

    Log_SetDomain( "/PREFIX", Scope::Method );

//! [DOX_ALOX_LOX_SETPREFIX]
Boxes prefixes("One, ", "two, ", 3 );
Log_SetPrefix( prefixes,  Scope::Global   );
//! [DOX_ALOX_LOX_SETPREFIX]
PFXCHECK( "One, two, 3*msg*"    ,ml );

    // src scopes
    Log_SetPrefix( "REPLACE:",    Scope::Global   );  PFXCHECK( "REPLACE:*msg*"          ,ml );
    Log_SetPrefix( "GLOBAL:",     Scope::Global   );  PFXCHECK( "GLOBAL:*msg*"           ,ml );
    Log_SetPrefix( nullptr,       Scope::Global   );  PFXCHECK( "*msg*"                  ,ml );
    Log_SetPrefix( "REPLACE:",    Scope::Filename );  PFXCHECK( "REPLACE:*msg*"          ,ml );
    Log_SetPrefix( "FILE:",       Scope::Filename );  PFXCHECK( "FILE:*msg*"             ,ml );
    Log_SetPrefix( "REPLACE:",    Scope::Method   );  PFXCHECK( "FILE:REPLACE:*msg*"     ,ml );

    Log_SetPrefix( "METHOD:",     Scope::Method   );  PFXCHECK( "FILE:METHOD:*msg*"      ,ml );
    Log_SetPrefix( nullptr,       Scope::Method   );  PFXCHECK( "FILE:*msg*"             ,ml );
    Log_SetPrefix( "METHOD:",     Scope::Method   );  PFXCHECK( "FILE:METHOD:*msg*"      ,ml );

    // remove with empty string
    Log_SetPrefix( "",            Scope::Method   );  PFXCHECK( "FILE:*msg*"             ,ml );

    // external prefix AStrings (C++ only)
    AString extPL("Ext:");
    Log_SetPrefix( extPL,         Scope::Method   );  PFXCHECK( "FILE:Ext:*msg*"         ,ml );
    extPL= "CHANGED:";                                PFXCHECK( "FILE:Ext:*msg*"         ,ml );

    Log_SetPrefix( BoxedAs<AString>(extPL),
                                  Scope::Method   );  PFXCHECK( "FILE:CHANGED:*msg*"     ,ml );
    extPL= "Ext2:";                                   PFXCHECK( "FILE:Ext2:*msg*"        ,ml );


    // external prefix AStrings (C++ only)
    std::string extPL2("STDSTR:");
    Log_SetPrefix( extPL2,         Scope::Method   );  PFXCHECK( "FILE:STDSTR:*msg*"   ,ml );
    extPL2= "CHANGED:";                                PFXCHECK( "FILE:STDSTR:*msg*"   ,ml );

    Log_SetPrefix( BoxedAs<std::string>(extPL2),
                                  Scope::Method   );  PFXCHECK( "FILE:CHANGED:*msg*"     ,ml );
    extPL2= "Ext2:";                                   PFXCHECK( "FILE:Ext2:*msg*"        ,ml );


    // back to internal mode
    Log_SetPrefix( "METHOD:",     Scope::Method   );  PFXCHECK( "FILE:METHOD:*msg*"      ,ml );


    // domain related
    Log_SetPrefix( "DOM1:" );                         PFXCHECK( "FILE:METHOD:DOM1:*msg*"            ,ml );
    Boxes domPfx("DO", "M2:" ); // set two logables at once!
    Log_SetPrefix( domPfx );                          PFXCHECK( "FILE:METHOD:DOM1:DOM2:*msg*"       ,ml );
    Log_SetPrefix( "DOM3:" );                         PFXCHECK( "FILE:METHOD:DOM1:DOM2:DOM3:*msg*"  ,ml );
    Log_SetPrefix( ""      );                         PFXCHECK( "FILE:METHOD:DOM1:DOM2:*msg*"       ,ml );
    Log_SetPrefix( ""      );                         PFXCHECK( "FILE:METHOD:DOM1:*msg*"            ,ml );
    Log_SetPrefix( "DOMR:", "/" );                    PFXCHECK( "FILE:METHOD:DOMR:DOM1:*msg*"       ,ml );
    Log_SetPrefix( "DOMX:", "", Inclusion::Exclude ); PFXCHECK( "DOMX:*msg*"                        ,ml );
    Log_SetPrefix( ""      );                         PFXCHECK( "FILE:METHOD:DOMR:DOM1:*msg*"       ,ml );
    Log_SetPrefix( "DRX:", "/", Inclusion::Exclude ); PFXCHECK( "DRX:DOM1:*msg*"                    ,ml );
    Log_SetPrefix( ""    , "/"  );                    PFXCHECK( "FILE:METHOD:DOMR:DOM1:*msg*"       ,ml );

    // source path
    Log_SetPrefix( "REPLACE:",    Scope::Path     );  PFXCHECK( "REPLACE:FILE:METHOD:DOMR:DOM1:*msg*"               ,ml );
    Log_SetPrefix( "PATH:",       Scope::Path     );  PFXCHECK( "PATH:FILE:METHOD:DOMR:DOM1:*msg*"                  ,ml );
    Log_SetPrefix( "REPLACE:",    Scope::Path + 1 );  PFXCHECK( "REPLACE:PATH:FILE:METHOD:DOMR:DOM1:*msg*"          ,ml );
    Log_SetPrefix( "PO1:",        Scope::Path + 1 );  PFXCHECK( "PO1:PATH:FILE:METHOD:DOMR:DOM1:*msg*"              ,ml );
    Log_SetPrefix( "REPLACE:",    Scope::Path + 2 );  PFXCHECK( "REPLACE:PO1:PATH:FILE:METHOD:DOMR:DOM1:*msg*"      ,ml );
    Log_SetPrefix( "PO2:",        Scope::Path + 2 );  PFXCHECK( "PO2:PO1:PATH:FILE:METHOD:DOMR:DOM1:*msg*"          ,ml );
    Log_SetPrefix( "REPLACE:",    Scope::Global   );  PFXCHECK( "REPLACE:PO2:PO1:PATH:FILE:METHOD:DOMR:DOM1:*msg*"  ,ml );

    // remove all previous scope domains
    Log_SetPrefix( nullptr ,Scope::Global      );
    Log_SetPrefix( nullptr ,Scope::Path        );
    Log_SetPrefix( nullptr ,Scope::Path    + 1 );
    Log_SetPrefix( nullptr ,Scope::Path    + 2 );
    Log_SetPrefix( nullptr ,Scope::Filename    );
    Log_SetPrefix( nullptr ,Scope::Method      );

    Log_SetPrefix( nullptr      ); // domain "/PREFIX"
    Log_SetPrefix( nullptr      ); // domain "/PREFIX" nothing here any more
    Log_SetPrefix( nullptr, "/" ); // domain "/"
                                                   PFXCHECK( "*msg*"                        ,ml );

    // Thread-related
    Log_SetPrefix( "TO:"   ,Scope::ThreadOuter );  PFXCHECK( "TO:*msg*"                     ,ml );
    Log_SetPrefix( "GL:"   ,Scope::Global      );  PFXCHECK( "GL:TO:*msg*"                  ,ml );
    Log_SetPrefix( "MET:"  ,Scope::Method      );  PFXCHECK( "GL:TO:MET:*msg*"              ,ml );
    Log_SetPrefix( ":TI"   ,Scope::ThreadInner );  PFXCHECK( "GL:TO:MET:*msg*:TI"           ,ml );

    Log_SetPrefix( "TO2:"  ,Scope::ThreadOuter );  PFXCHECK( "GL:TO:TO2:MET:*msg*:TI"       ,ml );
    Log_SetPrefix( ":TI2"  ,Scope::ThreadInner );  PFXCHECK( "GL:TO:TO2:MET:*msg*:TI:TI2"   ,ml );

    Log_SetPrefix( "TO3:"  ,Scope::ThreadOuter );  PFXCHECK( "GL:TO:TO2:TO3:MET:*msg*:TI:TI2"    ,ml );
    Log_SetPrefix( ":TI3"  ,Scope::ThreadInner );  PFXCHECK( "GL:TO:TO2:TO3:MET:*msg*:TI:TI2:TI3",ml );


    Log_SetPrefix( nullptr ,Scope::ThreadInner );  PFXCHECK( "GL:TO:TO2:TO3:MET:*msg*:TI:TI2"    ,ml );
    Log_SetPrefix( nullptr ,Scope::ThreadInner );  PFXCHECK( "GL:TO:TO2:TO3:MET:*msg*:TI"   ,ml );
    Log_SetPrefix( nullptr ,Scope::ThreadOuter );  PFXCHECK( "GL:TO:TO2:MET:*msg*:TI"       ,ml );
    Log_SetPrefix( nullptr ,Scope::ThreadOuter );  PFXCHECK( "GL:TO:MET:*msg*:TI"           ,ml );
    Log_SetPrefix( nullptr ,Scope::ThreadOuter );  PFXCHECK( "GL:MET:*msg*:TI"              ,ml );
    Log_SetPrefix( nullptr ,Scope::Global      );  PFXCHECK( "MET:*msg*:TI"                 ,ml );
    Log_SetPrefix( nullptr ,Scope::Method      );  PFXCHECK( "*msg*:TI"                     ,ml );
    Log_SetPrefix( nullptr ,Scope::ThreadInner );  PFXCHECK( "*msg*"                        ,ml );

    // check if breaking dom-related, removes all thread inner correctly
    Log_SetPrefix( ":TI"   ,Scope::ThreadInner );     PFXCHECK( "*msg*:TI"                  ,ml );
    Log_SetPrefix( "DOM1:", "", Inclusion::Include ); PFXCHECK( "DOM1:*msg*:TI"             ,ml );
    Log_SetPrefix( "DOMX:", "", Inclusion::Exclude ); PFXCHECK( "DOMX:*msg*"                ,ml );
    Log_SetPrefix( ":TI"   ,Scope::ThreadInner );     PFXCHECK( "DOMX:*msg*"                ,ml );
    Log_SetPrefix( nullptr, ""                     ); PFXCHECK( "DOM1:*msg*:TI:TI"          ,ml );

    Log_RemoveLogger( &ml );
    #if ALIB_DEBUG
        UT_PRINT( "Statistics on the language-related scope store (using a StringTree with Block allocation):" )
        UT_PRINT( LOG_LOX.DbgGetStoreAllocator(3).DbgStats() )
    #endif

}
#endif


/** ********************************************************************************************
 * Log_ScopeDomains
 **********************************************************************************************/
#if ALOX_DBG_LOG_CI
#define DDCHECK( d, s, ml )        {Log_Info( d, "" );      UT_EQ(ASTR(s), ml .MemoryLog); ml .MemoryLog._(); ml .AutoSizes.Reset();}


UT_METHOD(Log_ScopeDomains)
{
    UT_INIT();

    // we have tell alox to include more directories in the scope path
    Log_ClearSourcePathTrimRules( Reach::Global, false );
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, "/test/test2/test3", Reach::Global );

    Log_AddDebugLogger();
    MemoryLogger ml;
    ml.MetaInfo->Format._()._("@%D#");
    Log_SetVerbosity(&ml, Verbosity::Verbose );
    Log_SetVerbosity(Log::DebugLogger, Verbosity::Verbose, ALox::InternalDomains );

    // test methods with extending names
    LSD();         UT_EQ( ASTR("@/LSD#"),      ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    LSD_A();       UT_EQ( ASTR("@/A#"),        ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    LSD_A_B();     UT_EQ( ASTR("@/B#"),        ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    LSD2_A_B();    UT_EQ( ASTR("@/B2#"),       ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    LSD2_A();      UT_EQ( ASTR("@/A2#"),       ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    LSD2();        UT_EQ( ASTR("@/LSD2#"),     ml.MemoryLog );   ml.MemoryLog._(); ml.AutoSizes.Reset();
    DDCHECK( "","@/#"              ,ml );

    // scope global
    Log_SetDomain( "REPLACE",    Scope::Global   );  DDCHECK( "","@/REPLACE#"                     ,ml );
    Log_SetDomain( "GLOBAL",     Scope::Global   );  DDCHECK( "","@/GLOBAL#"                      ,ml );

    Log_SetDomain( nullptr,      Scope::Global   );  DDCHECK( "","@/#"                            ,ml );

    // scope source
    Log_SetDomain( "REPLACE",    Scope::Filename );  DDCHECK( "","@/REPLACE#"                     ,ml );
    Log_SetDomain( "FILE",       Scope::Filename );  DDCHECK( "","@/FILE#"                        ,ml );

    // scope method
    Log_SetDomain( "REPLACE",    Scope::Method   );  DDCHECK( "","@/FILE/REPLACE#"                ,ml );
    Log_SetDomain( "METHOD",     Scope::Method   );  DDCHECK( "","@/FILE/METHOD#"                 ,ml );
    Log_SetDomain( "/ABS",       Scope::Method   );  DDCHECK( "","@/ABS#"                         ,ml );

    // unset method with nullptr
    Log_SetDomain( nullptr,      Scope::Method   );  DDCHECK( "","@/FILE#"                        ,ml );

    // unset method with ""
    Log_SetDomain( "/METHOD",    Scope::Method   );  DDCHECK( "","@/METHOD#"                      ,ml );
    Log_SetDomain( "",           Scope::Method   );  DDCHECK( "","@/FILE#"                        ,ml );
    Log_SetDomain( "METHOD",     Scope::Method   );  DDCHECK( "","@/FILE/METHOD#"                 ,ml );

    // source path
    Log_SetDomain( "REPLACE",    Scope::Path     );  DDCHECK( "","@/REPLACE/FILE/METHOD#"         ,ml );
    Log_SetDomain( "PATH",       Scope::Path     );  DDCHECK( "","@/PATH/FILE/METHOD#"            ,ml );
    Log_SetDomain( "REPLACE",    Scope::Path + 1 );  DDCHECK( "","@/REPLACE/PATH/FILE/METHOD#"    ,ml );
    Log_SetDomain( "PO1",        Scope::Path + 1 );  DDCHECK( "","@/PO1/PATH/FILE/METHOD#"        ,ml );

    Log_SetDomain( "REPLACE",    Scope::Path + 2 );  DDCHECK( "","@/REPLACE/PO1/PATH/FILE/METHOD#",ml );

    Log_SetDomain( "PO2",        Scope::Path + 2 );  DDCHECK( "","@/PO2/PO1/PATH/FILE/METHOD#"    ,ml );
    Log_SetDomain( "REPLACE",    Scope::Path + 50);  DDCHECK( "","@/REPLACE/PO2/PO1/PATH/FILE/METHOD#"     ,ml );
    Log_SetDomain( "PO50",       Scope::Path + 50);  DDCHECK( "","@/PO50/PO2/PO1/PATH/FILE/METHOD#"        ,ml );

    Log_LogState( "", Verbosity::Info, ASTR("Configuration now is:") ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Log_SetDomain( "GLOBAL",     Scope::Global   );  DDCHECK( "","@/GLOBAL/PO50/PO2/PO1/PATH/FILE/METHOD#" , ml );

                                                     DDCHECK( "","@/GLOBAL/PO50/PO2/PO1/PATH/FILE/METHOD#" , ml );

    Log_ScopeDomains_Helper();    UT_EQ( ASTR("@/GLOBAL/PO50/PO2/PO1/PATH/HFILE/HMETHOD#"), ml.MemoryLog );    ml.MemoryLog._(); ml.AutoSizes.Reset();

                                                     DDCHECK( "","@/GLOBAL/PO50/PO2/PO1/PATH/FILE/METHOD#" , ml );

    Log_ScopeDomains_Helper2();   UT_EQ( ASTR("@/GLOBAL/PO50/PO2/PO1/PATH/H2FILE/H2METHOD#"), ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();

                                                     DDCHECK( "","@/GLOBAL/PO50/PO2/PO1/PATH/FILE/METHOD#" , ml );

    Log_ScopeDomains_Helper2B();  UT_EQ( ASTR("@/GLOBAL/PO50/PO2/PO1/PATH/FILE#")           , ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();

    Log_ScopeDomains_HPPHelper(); UT_EQ( ASTR("@/GLOBAL/PO50/PO2/PO1/PATH/FILE#"),          ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();

                                                     DDCHECK( "","@/GLOBAL/PO50/PO2/PO1/PATH/FILE/METHOD#"  , ml );

    // remove all previous scope domains
    Log_SetDomain( "",     Scope::Global      );
    Log_SetDomain( "",     Scope::Path        );
    Log_SetDomain( "",     Scope::Path  + 1   );
    Log_SetDomain( "",     Scope::Path  + 2   );
    Log_SetDomain( "",     Scope::Path  + 45  ); // same as 50 above! (test)
    Log_SetDomain( "",     Scope::Filename    );
    Log_SetDomain( "",     Scope::Method      );  DDCHECK( "LOC", "@/LOC#"                ,ml );



    // Thread-related
    Log_SetDomain( "T_O",  Scope::ThreadOuter );  DDCHECK( ""   ,"@/T_O#"                 ,ml );
    Log_SetDomain( "GL",   Scope::Global      );  DDCHECK( ""   ,"@/GL/T_O#"              ,ml );
    Log_SetDomain( "MET",  Scope::Method      );  DDCHECK( ""   ,"@/GL/T_O/MET#"          ,ml );
    Log_SetDomain( "MET",  Scope::Method      );  DDCHECK( "LOC","@/GL/T_O/MET/LOC#"      ,ml );
    Log_SetDomain( "T_I",  Scope::ThreadInner );  DDCHECK( ""   ,"@/GL/T_O/MET/T_I#"      ,ml );
                                                            DDCHECK( "LOC","@/GL/T_O/MET/LOC/T_I#"  ,ml );
    Log_SetDomain( "T_O2", Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/T_O/T_O2/MET/T_I#" ,ml );
    Log_SetDomain( "T_I2", Scope::ThreadInner );  DDCHECK( ""   ,"@/GL/T_O/T_O2/MET/T_I/T_I2#" ,ml );

    Log_SetDomain( "/T_O3",Scope::ThreadOuter );  DDCHECK( ""   ,"@/T_O3/MET/T_I/T_I2#"   ,ml );
    Log_SetDomain( "/T_I3",Scope::ThreadInner );  DDCHECK( ""   ,"@/T_I3#"                ,ml );


    Log_SetDomain         ( ""     ,Scope::ThreadInner );  DDCHECK( ""   ,"@/T_O3/MET/T_I/T_I2#"   ,ml );
    Log_RemoveThreadDomain( "T_IXX",Scope::ThreadInner );  DDCHECK( ""   ,"@/T_O3/MET/T_I/T_I2#"   ,ml );
    Log_RemoveThreadDomain( "T_I"  ,Scope::ThreadInner );  DDCHECK( ""   ,"@/T_O3/MET/T_I2#"       ,ml );
    Log_RemoveThreadDomain( ""     ,Scope::ThreadInner );  DDCHECK( ""   ,"@/T_O3/MET/T_I2#"       ,ml );
    Log_SetDomain         ( ""     ,Scope::ThreadInner );  DDCHECK( ""   ,"@/T_O3/MET#"            ,ml );
    Log_SetDomain         ( ""     ,Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/T_O/T_O2/MET#"     ,ml );
    Log_SetDomain         ( "T_O3", Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/T_O/T_O2/T_O3/MET#",ml );
    Log_RemoveThreadDomain( "T_O2" ,Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/T_O/T_O3/MET#"     ,ml );
    Log_RemoveThreadDomain( "T_O"  ,Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/T_O3/MET#"         ,ml );
    Log_SetDomain         ( ""     ,Scope::ThreadOuter );  DDCHECK( ""   ,"@/GL/MET#"              ,ml );
    Log_SetDomain         ( ""     ,Scope::Global      );  DDCHECK( ""   ,"@/MET#"                 ,ml );
    Log_SetDomain         ( ""     ,Scope::Method      );  DDCHECK( ""   ,"@/#"                    ,ml );


    // second thread
    #if ALIB_FEAT_THREADS
        DomainTestThread thread;
        Log_SetDomain( "THIS_THREAD",   Scope::ThreadOuter );
        Log_SetDomain( "OTHER_THREAD",  Scope::ThreadOuter, &thread );
        thread.Start();
        while( thread.IsAlive() )
            ALib::SleepMillis(1);
                               UT_EQ( ASTR("@/OTHER_THREAD/DTT#"), ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();
        Log_Info( "ME", "" );  UT_EQ( ASTR("@/THIS_THREAD/ME#")  , ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();
    #endif

    //Log_LogState( "", Verbosity::Info, ASTR("Configuration now is:") ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Log_RemoveLogger( &ml );

    #if ALIB_DEBUG
        UT_PRINT( "Statistics on the language-related scope store (using a StringTree with Block allocation):" )
        UT_PRINT( LOG_LOX.DbgGetStoreAllocator(1).DbgStats() )
    #endif

}
#endif

/** ********************************************************************************************
 * Lox_ScopeDomains
 **********************************************************************************************/
#if ALOX_REL_LOG
#define LOX_LOX lox
#define DDCHECK_RL( d, s, ml )        {Lox_Info( d, "" );      UT_EQ(ASTR(s), ml .MemoryLog); ml .MemoryLog._(); ml .AutoSizes.Reset();}

#if ALOX_REL_LOG_CI
    #define CICHECK_RL( d, s, ml )    DDCHECK_RL(d,s,ml)
#else
    #define CICHECK_RL( d, s, ml )    {Lox_Info( d, "" );                               ml .MemoryLog._(); ml .AutoSizes.Reset();}
#endif

UT_METHOD(Lox_ScopeDomains)
{
    UT_INIT();

    Lox lox("ReleaseLox");

    // we have to tell alox to include more directories in the scope path
    Lox_ClearSourcePathTrimRules( Reach::Global, false );
    Lox_SetSourcePathTrimRule( "*/src/", Inclusion::Exclude, 0, Case::Ignore, "/test/test2/test3" );

    aworx::TextLogger* consoleLogger= Lox::CreateConsoleLogger();
    MemoryLogger ml;
    ml.MetaInfo->Format._()._("@%D#");
    Lox_SetVerbosity(&ml, Verbosity::Verbose );
    Lox_SetVerbosity(consoleLogger, Verbosity::Verbose );
    Lox_SetVerbosity(consoleLogger, Verbosity::Verbose, ALox::InternalDomains );

    // scope global
    Lox_SetDomain( "REPLACE",    Scope::Global   );  DDCHECK_RL( "","@/REPLACE#"                     ,ml );
    Lox_SetDomain( "GLOBAL",     Scope::Global   );  DDCHECK_RL( "","@/GLOBAL#"                      ,ml );

    Lox_SetDomain( nullptr,      Scope::Global   );  DDCHECK_RL( "","@/#"                            ,ml );

    // scope source
    Lox_SetDomain( "REPLACE",    Scope::Filename );  CICHECK_RL( "","@/REPLACE#"                     ,ml );
    Lox_SetDomain( "FILE",       Scope::Filename );  CICHECK_RL( "","@/FILE#"                        ,ml );

    // scope method
    Lox_SetDomain( "REPLACE",    Scope::Method   );  CICHECK_RL( "","@/FILE/REPLACE#"                ,ml );
    Lox_SetDomain( "METHOD",     Scope::Method   );  CICHECK_RL( "","@/FILE/METHOD#"                 ,ml );
    Lox_SetDomain( "/ABS",       Scope::Method   );  CICHECK_RL( "","@/ABS#"                         ,ml );

    // unset method with nullptr
    Lox_SetDomain( nullptr,      Scope::Method   );  CICHECK_RL( "","@/FILE#"                        ,ml );

    // unset method with ""
    Lox_SetDomain( "/METHOD",    Scope::Method   );  CICHECK_RL( "","@/METHOD#"                      ,ml );
    Lox_SetDomain( "",           Scope::Method   );  CICHECK_RL( "","@/FILE#"                        ,ml );
    Lox_SetDomain( "METHOD",     Scope::Method   );  CICHECK_RL( "","@/FILE/METHOD#"                 ,ml );

    // source path
    Lox_SetDomain( "REPLACE",    Scope::Path     );  CICHECK_RL( "","@/REPLACE/FILE/METHOD#"         ,ml );
    Lox_SetDomain( "PATH",       Scope::Path     );  CICHECK_RL( "","@/PATH/FILE/METHOD#"            ,ml );
    Lox_SetDomain( "REPLACE",    Scope::Path + 1 );  CICHECK_RL( "","@/REPLACE/PATH/FILE/METHOD#"    ,ml );
    Lox_SetDomain( "PO1",        Scope::Path + 1 );  CICHECK_RL( "","@/PO1/PATH/FILE/METHOD#"        ,ml );
    Lox_SetDomain( "REPLACE",    Scope::Path + 2 );  CICHECK_RL( "","@/REPLACE/PO1/PATH/FILE/METHOD#",ml );
    Lox_SetDomain( "PO2",        Scope::Path + 2 );  CICHECK_RL( "","@/PO2/PO1/PATH/FILE/METHOD#"    ,ml );

    Lox_SetDomain( "GLOBAL",     Scope::Global   );  CICHECK_RL( "","@/GLOBAL/PO2/PO1/PATH/FILE/METHOD#"  , ml );

    #if ALOX_REL_LOG_CI
        Lox_ScopeDomains_Helper   ( lox ); UT_EQ( "@/GLOBAL/PO2/PO1/PATH/HFILE/HMETHOD#", ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();
        Lox_ScopeDomains_HPPHelper( lox ); UT_EQ( "@/GLOBAL/PO2/PO1/PATH/FILE#",          ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();

                                                                   DDCHECK_RL( "","@/GLOBAL/PO2/PO1/PATH/FILE/METHOD#"  , ml );
    #endif

    //Lox_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    // remove all previous scope domains
    Lox_SetDomain( "",     Scope::Global      );
    Lox_SetDomain( "",     Scope::Path        );
    Lox_SetDomain( "",     Scope::Path  + 1   );
    Lox_SetDomain( "",     Scope::Path  + 2   );
    Lox_SetDomain( "",     Scope::Filename    );
    Lox_SetDomain( "",     Scope::Method      );  DDCHECK_RL( "LOC", "@/LOC#"                ,ml );

    // Thread-related
    Lox_SetDomain( "T_O",  Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/T_O#"                 ,ml );
    Lox_SetDomain( "GL",   Scope::Global      );  DDCHECK_RL( ""   ,"@/GL/T_O#"              ,ml );
    Lox_SetDomain( "MET",  Scope::Method      );  CICHECK_RL( ""   ,"@/GL/T_O/MET#"          ,ml );
                                                            CICHECK_RL( "LOC","@/GL/T_O/MET/LOC#"      ,ml );
    Lox_SetDomain( "T_I",  Scope::ThreadInner );  CICHECK_RL( ""   ,"@/GL/T_O/MET/T_I#"      ,ml );
                                                            CICHECK_RL( "LOC","@/GL/T_O/MET/LOC/T_I#"  ,ml );
    Lox_SetDomain( "T_O2", Scope::ThreadOuter );  CICHECK_RL( ""   ,"@/GL/T_O/T_O2/MET/T_I#" ,ml );
    Lox_SetDomain( "T_I2", Scope::ThreadInner );  CICHECK_RL( ""   ,"@/GL/T_O/T_O2/MET/T_I/T_I2#" ,ml );

    Lox_SetDomain( "/T_O3",Scope::ThreadOuter );  CICHECK_RL( ""   ,"@/T_O3/MET/T_I/T_I2#"   ,ml );
    Lox_SetDomain( "/T_I3",Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_I3#"                ,ml );

    //Lox_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Lox_SetDomain( "",     Scope::Method      );  DDCHECK_RL( ""   ,"@/T_I3#"                ,ml );


    Lox_SetDomain         ( ""     ,Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_O3/T_I/T_I2#"   ,ml );
    Lox_RemoveThreadDomain( "T_IXX",Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_O3/T_I/T_I2#"   ,ml );
    Lox_RemoveThreadDomain( "T_I"  ,Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_O3/T_I2#"       ,ml );
    Lox_RemoveThreadDomain( ""     ,Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_O3/T_I2#"       ,ml );
    Lox_SetDomain         ( ""     ,Scope::ThreadInner );  DDCHECK_RL( ""   ,"@/T_O3#"            ,ml );
    Lox_SetDomain         ( ""     ,Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/GL/T_O/T_O2#"     ,ml );
    Lox_SetDomain         ( "T_O3", Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/GL/T_O/T_O2/T_O3#",ml );
    Lox_RemoveThreadDomain( "T_O2" ,Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/GL/T_O/T_O3#"     ,ml );
    Lox_RemoveThreadDomain( "T_O"  ,Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/GL/T_O3#"         ,ml );
    Lox_SetDomain         ( ""     ,Scope::ThreadOuter );  DDCHECK_RL( ""   ,"@/GL#"              ,ml );
    Lox_SetDomain         ( ""     ,Scope::Global      );  DDCHECK_RL( ""   ,"@/#"                 ,ml );


    // second thread
    #if ALIB_FEAT_THREADS
        DomainTestThreadRL thread( &lox );
        Lox_SetDomain( "THIS_THREAD",   Scope::ThreadOuter );
        Lox_SetDomain( "OTHER_THREAD",  Scope::ThreadOuter, &thread );
        thread.Start();
        while( thread.IsAlive() )
            ALib::SleepMillis(1);
                               UT_EQ( ASTR("@/OTHER_THREAD/DTT#"), ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();
        Lox_Info( "ME", "" );  UT_EQ( ASTR("@/THIS_THREAD/ME#")  , ml.MemoryLog );  ml.MemoryLog._(); ml.AutoSizes.Reset();
    #endif

    // cleanup
    Lox_RemoveLogger( &ml );
    Lox_RemoveLogger( consoleLogger );
    delete consoleLogger;
}
#undef LOX_LOX
#endif


/** ********************************************************************************************
 * Log_Once_Test
 **********************************************************************************************/
#if ALOX_DBG_LOG


// restore original macro used by ALox
#undef  ALIB_CALLER
#define ALIB_CALLER     __FILE__, __LINE__, __func__

void logOnceMethod()
{
    Log_Once( Verbosity::Info, "Once(Scope::Filename) 4x -from other method", Scope::Filename, 4 );
}

// with GTEST macros it all gets wild. Fix the method name
#undef  ALIB_CALLER
#define ALIB_CALLER     __FILE__, __LINE__, UT_GET_TEST_NAME

UT_METHOD(Log_Once_Test)
{
    UT_INIT();

    Log_AddDebugLogger();
    MemoryLogger ml;

    Log_SetVerbosity ( &ml, Verbosity::Verbose );
    Log_SetVerbosity( Log::DebugLogger,  Verbosity::Verbose, ALox::InternalDomains );
    Log_SetDomain( "ONCE", Scope::Global );

    //-------------------- associated to scope method-----------------
    #if ALOX_DBG_LOG_CI
        for (int i= 0; i < 5 ; i++ )
            Log_Once( Verbosity::Info, "Once(Scope) 1x", Scope::Method );
        Log_Once( Verbosity::Info, "Once(Scope) 1x", Scope::Method );

        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

        //-------------------- associated to scope filename -----------------

        for (int i= 0; i < 5 ; i++ )
        {
            Log_Once( "Subdom", Verbosity::Info, "Once(Scope::Filename) 4x", Scope::Filename, 4 );
            logOnceMethod();
        }
        Log_Once( Verbosity::Info, "Once(Scope::Filename) 4x", Scope::Filename, 4 );

        UT_EQ( 4, ml.CntLogs ); ml.CntLogs= 0;
    #endif

    //-------------------- associated to scope thread -----------------
    #if ALIB_FEAT_THREADS
        Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - main thread", Scope::ThreadOuter, 2 );
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;
        LogOnceTestThread thread;
        thread.Start();
        while( thread.IsAlive() )
            ALib::SleepMicros(1);
        UT_EQ( 2, ml.CntLogs ); ml.CntLogs= 0;
        Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - main thread", Scope::ThreadOuter, 2 );
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;
        Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - main thread", Scope::ThreadOuter, 2 );
        UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;
        Log_Once( Verbosity::Info, "Once(Scope::ThreadOuter) 2x - main thread", Scope::ThreadOuter, 2 );
        UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

        // different group
        Log_Once( Verbosity::Info, "Once(key, Scope::ThreadOuter) 2x - main thread", ASTR("group"), Scope::ThreadOuter, 1 );
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;
        Log_Once( Verbosity::Info, "Once(key, Scope::ThreadOuter) 2x - main thread", ASTR("group"), Scope::ThreadOuter, 1 );
        UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;
    #endif

    //-------------------- associated to line  -----------------
    #if ALOX_DBG_LOG_CI
        for (int i= 0; i < 5 ; i++ )
            Log_Once( Verbosity::Info, "Once(line) 1x" );
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 5; i++ )
            Log_Once( Verbosity::Info, "Once(line) 2x", 2 );
        UT_EQ( 2, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 5; i++ )
            Log_Once( Verbosity::Info, "Once(line) 1x", 1 );
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 5; i++ )
            Log_Once( Verbosity::Info, "Once(line) 0x", 0 );
        UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 5; i++ )
            Log_Once( "Once(line) Just msg and number", 2 );
        UT_EQ( 2, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 5; i++ )
            Log_Once( "Once(line) Just msg ");
        UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;
    #endif

    //-------------------- associated to group -----------------
    for (int i= 0; i < 5 ; i++ )
        Log_Once( Verbosity::Info, "Once(\"a group\") 1x", ASTR("a group") );
    UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;
    Log_Once( Verbosity::Info, "Once(\"a group\") 1x but tricked up", ASTR("a group"), 2 );
    UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;


    Log_Once( Verbosity::Info, "Once(\"a group\") 1x", ASTR("a group") );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once(                   "Once(\"b group\") 1x", ASTR("b group") );
    UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"b group\") 1x", ASTR("b group") );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"c group\") 2x", ASTR("c group"), 2 );
    UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"c group\") 2x", ASTR("c group"), 2 );
    UT_EQ( 1, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"c group\") 2x", ASTR("c group"), 2 );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"a group\") 1x", ASTR("a group") );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"b group\") 1x", ASTR("b group") );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    Log_Once( Verbosity::Info, "Once(\"c group\") 2x", ASTR("c group"), 2 );
    UT_EQ( 0, ml.CntLogs ); ml.CntLogs= 0;

    //-------------------- Log every Nth -----------------
    #if ALOX_DBG_LOG_CI
        for (int i= 0; i < 10 ; i++ )
            Log_Once( Verbosity::Info, "Every 2nd ", -2 );
        UT_EQ( 5, ml.CntLogs ); ml.CntLogs= 0;

        for (int i= 0; i < 10 ; i++ )
            Log_Once( Verbosity::Info, "Every 3rd ", -3 );
        UT_EQ( 4, ml.CntLogs ); ml.CntLogs= 0;
    #endif


    //Log_LogState( "", Verbosity::Info, "Configuration now is:" ); ml.MemoryLog._(); ml.AutoSizes.Reset();

    Log_RemoveLogger( &ml );

    #if ALIB_DEBUG
        UT_PRINT( "Statistics on the language-related scope store (using a StringTree with Block allocation):" )
        UT_PRINT( LOG_LOX.DbgGetStoreAllocator(2).DbgStats() )
    #endif

}
#endif

/** ********************************************************************************************
 * Log_Store_Test
 **********************************************************************************************/
#if ALOX_DBG_LOG
UT_METHOD(Log_Store_Test)
{
    UT_INIT();

    Log_AddDebugLogger();
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, ALox::InternalDomains );
    Log_SetDomain( "STORE", Scope::Method );

    // without key
    Log_Store( nullptr       ,    Scope::Global       );
    Log_Store( "Replaced"    ,    Scope::Global       );
    Log_Store( nullptr       ,    Scope::Global       );
    Log_Store( "Replaced"    ,    Scope::Global       );
    Log_Store( "Global"      ,    Scope::Global       );
    Log_Store( "Replaced"    ,    Scope::ThreadOuter  );
    Log_Store( "ThreadOuter" ,    Scope::ThreadOuter  );

#if ALOX_DBG_LOG_CI
    Log_Store( "Replaced"    ,    Scope::Path + 1     );
    Log_Store( "Path1"       ,    Scope::Path + 1     );
    Log_Store( "Replaced"    ,    Scope::Path         );
    Log_Store( "Path"        ,    Scope::Path         );
    Log_Store( "Replaced"    ,    Scope::Filename     );
    Log_Store( "FileName"    ,    Scope::Filename     );
    Log_Store( "Replaced"    ,    Scope::Method       );
    Log_Store( "Method"      ,    Scope::Method       );
    Log_Store( "Replaced"    ,    Scope::ThreadInner  );
#endif

    Log_Store( "ThreadInner" ,    Scope::ThreadInner    );

    { Log_Retrieve( data,  Scope::Global       ); UT_TRUE( data.Unbox<NString>().Equals( "Global" ) ); }
    { Log_Retrieve( data,  Scope::ThreadOuter  ); UT_TRUE( data.Unbox<NString>().Equals( "ThreadOuter"   ) ); }

#if ALOX_DBG_LOG_CI
    { Log_Retrieve( data,  Scope::Path + 1     ); UT_TRUE( data.Unbox<NString>().Equals( "Path1"         ) ); }
    { Log_Retrieve( data,  Scope::Path         ); UT_TRUE( data.Unbox<NString>().Equals( "Path"          ) ); }
    { Log_Retrieve( data,  Scope::Filename     ); UT_TRUE( data.Unbox<NString>().Equals( "FileName"      ) ); }
    { Log_Retrieve( data,  Scope::Method       ); UT_TRUE( data.Unbox<NString>().Equals( "Method"        ) ); }
#endif

    { Log_Retrieve( data,  Scope::ThreadInner  ); UT_TRUE( data.Unbox<NString>().Equals( "ThreadInner"   ) ); }

    // wit key
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::Global         );
    Log_Store( "Global"       ,   ASTR("mykey"),  Scope::Global         );
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::ThreadOuter    );
    Log_Store( "ThreadOuter"  ,   ASTR("mykey"),  Scope::ThreadOuter    );
#if ALOX_DBG_LOG_CI
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::Path + 1       );
    Log_Store( "Path1"        ,   ASTR("mykey"),  Scope::Path + 1       );
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::Path           );
    Log_Store( "Path"         ,   ASTR("mykey"),  Scope::Path           );
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::Filename       );
    Log_Store( "FileName"     ,   ASTR("mykey"),  Scope::Filename       );
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::Method         );
    Log_Store( "Method"       ,   ASTR("mykey"),  Scope::Method         );
#endif
    Log_Store( "Replaced"     ,   ASTR("mykey"),  Scope::ThreadInner    );
    Log_Store( "ThreadInner"  ,   ASTR("mykey"),  Scope::ThreadInner    );


    { Log_Retrieve( data,  ASTR("mykey"), Scope::Global       ); UT_TRUE( data.Unbox<NString>().Equals( "Global")        ); }
    { Log_Retrieve( data,  ASTR("mykey"), Scope::ThreadOuter  ); UT_TRUE( data.Unbox<NString>().Equals( "ThreadOuter")   ); }
#if ALOX_DBG_LOG_CI
    { Log_Retrieve( data,  ASTR("mykey"), Scope::Path + 1     ); UT_TRUE( data.Unbox<NString>().Equals( "Path1")         ); }
    { Log_Retrieve( data,  ASTR("mykey"), Scope::Path         ); UT_TRUE( data.Unbox<NString>().Equals( "Path")          ); }
    { Log_Retrieve( data,  ASTR("mykey"), Scope::Filename     ); UT_TRUE( data.Unbox<NString>().Equals( "FileName")      ); }
    { Log_Retrieve( data,  ASTR("mykey"), Scope::Method       ); UT_TRUE( data.Unbox<NString>().Equals( "Method")        ); }
#endif
    { Log_Retrieve( data,  ASTR("mykey"), Scope::ThreadInner  ); UT_TRUE( data.Unbox<NString>().Equals( "ThreadInner")   ); }


    // threaded
    Log_Store( "Main Thread Data"        ,                   Scope::ThreadOuter   );
    Log_Store( "Main Thread Data, keyed" ,   ASTR("mykey"),  Scope::ThreadOuter   );

    StoreDataTestThread thread(ut);
    thread.Start();
    while( thread.IsAlive() )
        ALib::SleepMicros(1);

    { Log_Retrieve( data,                 Scope::ThreadOuter ); UT_TRUE( data.Unbox<NString>().Equals( "Main Thread Data")         ); }
    { Log_Retrieve( data,  ASTR("mykey"), Scope::ThreadOuter ); UT_TRUE( data.Unbox<NString>().Equals( "Main Thread Data, keyed")  ); }

    #if ALIB_DEBUG
        UT_PRINT( "Statistics on the language-related scope store (using a StringTree with Block allocation):" )
        UT_PRINT( LOG_LOX.DbgGetStoreAllocator(4).DbgStats() )
    #endif
    //Log_LogState( "", Verbosity::Info, "Configuration now is:" );

    // test if everything gets deleted nicely
    Log_Prune( LOG_LOX.Reset() );
}
#endif


UT_CLASS_END


} // namespace

#if defined(_MSC_VER)
    #pragma warning( pop )
#endif


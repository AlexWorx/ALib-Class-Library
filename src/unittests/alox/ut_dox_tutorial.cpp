// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_ALOX

//! [Tut_include_statement]
#include "alib/alox.hpp"
#include "alib/alox/aloxmodule.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/loggers/ansilogger.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
//! [Tut_include_statement]

#include "alib/lib/fs_commonenums/commonenumdefs_aliased.hpp"

#include <iostream>
#include <fstream>
#include <string>

//! [Tut_using_statement]
using namespace std;
using namespace aworx;
//! [Tut_using_statement]

extern void HelloALox();

namespace ut_alox {

// used with unit test Log_ScopeInfoCacheTest
void ScopeInfoCacheTest7();
void ScopeInfoCacheTest7() { Log_Info("Test Method 7") }
}

namespace  { void process( int ) {} }
extern
void notCompiledConditionalLogging();
void notCompiledConditionalLogging()
{
    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
    basic_string<character>* array= nullptr;
    int len= 5;
    basic_string<character> search= A_CHAR("");

    //! [Tut_ConditionalLogging]
    int i= 0;
    while( i < len )
    {
        if ( array[i] == search )
        {
            process( i );
            break;
        }
        ++i;
    }
    if ( i == len )
    {
        Log_Error( "Nothing found :-(" )
    }
    //! [Tut_ConditionalLogging]

    //! [Tut_ConditionalLogging2]
    Log_Assert( i != len, "Nothing found :-(" )
    //! [Tut_ConditionalLogging2]

    //! [Tut_ConditionalLogging3]
    Log_If( i == len, Verbosity::Error, "Nothing found :-(" )
    //! [Tut_ConditionalLogging3]

    //! [Tut_ConditionalLoggingOnce]
    Log_Once( "I tell you this now only once!" )
    //! [Tut_ConditionalLoggingOnce]
    ALIB_WARNINGS_RESTORE
}

// #################################################################################################
// Tut_ScopeDomains
// #################################################################################################
#if ALOX_DBG_LOG
class TutScopeDom
{
    public:
    //! [Tut_ScopeDomains]
    void* Extract( const aworx::NString& fileName, void* buffer )
    {
        Log_SetDomain( "ZIP/EXTRACT", Scope::Method ) // set Scope Domain path for this method
        //...
        Log_Info( "Extracting {!Q}", fileName )
        //...
        //...
        Log_Info( "Success" ) // a nice, clear, local, copyable log statement!
        //...
        return buffer;
    }
    //! [Tut_ScopeDomains]
};

//! [Tut_ScopeDomains_Zipper]
class Zipper
{
    public:
    Zipper()
    {
        Log_SetDomain( "ZIP", Scope::Filename ) // set Scope Domain path for this class (filename)
        //...
        Log_Info( "Zipper created" ) // domain "ZIP"
        //...
    }

    void* Compress( const aworx::NString& fileName, void* buffer )
    {
        Log_SetDomain( "COMPRESS", Scope::Method ) // set Scope Domain path for this method
        //...
        Log_Info( "Compressing {!Q}", fileName )
        //...
        //...
        Log_Info( "Success" ) // domain "ZIP/COMPRESS"
        //...
        return buffer;
    }

    void* Extract( const aworx::NString& fileName, void* buffer )
    {
        Log_SetDomain( "EXTRACT", Scope::Method ) // set Scope Domain path for this method
        //...
        Log_Info( "Extracting {!Q}", fileName )
        //...
        //...
        Log_Info( "Success" ) // domain "ZIP/EXTRACT"
        //...
        return buffer;
    }
};
//! [Tut_ScopeDomains_Zipper]

// #################################################################################################
// Tut_LogData
// #################################################################################################
//! [Tut_LogData]
class FileIO
{
    public:

    void Read( const aworx::NString& fileName )
    {
        Log_SetDomain( "READ", Scope::Method )
        Log_Info( "Reading {!Q}", fileName )

        String fileVersion= nullptr;
        //...
        //...
        // Identified file version
        fileVersion= A_CHAR("3.1");

        Log_Store( fileVersion, "FILE_VERSION" )

        //...
        //...
        Log_Info( "Success" )
    }
};
//! [Tut_LogData]

#endif

#define TESTCLASSNAME       CPP_Dox_Tutorial
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {

/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

UT_CLASS


UT_METHOD(MiniMumAlox)
{
    UT_INIT()
    HelloALox();
}

// #################################################################################################
// Hello_ALox
// #################################################################################################

UT_METHOD(Hello_ALox)
{
    UT_INIT()

    Log_Prune( if ( Log::DebugLogger != nullptr ) )
                   Log_RemoveDebugLogger()

    Log_Prune( MemoryLogger memLog; )



    //! [Tut_ALox_Logger_1]
    Log_AddDebugLogger()
    //! [Tut_ALox_Logger_1]

    Log_RemoveDebugLogger()

    Log_SetVerbosity( &memLog, Verbosity::Verbose )

    //! [Tut_ALox_Logger_2]
    Log_AddDebugLogger()
    Log_Info ( "Hello ALox" )
    //! [Tut_ALox_Logger_2]

    Log_Prune( ut.WriteResultFile( "Tut_ALox_Logger.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}





// moved to 3-digit line number, to have log output formatted well :-)

// #################################################################################################
// Tut_Verbosity
// #################################################################################################
UT_METHOD(Tut_Verbosity)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    //! [Tut_Verbosity]
    Log_AddDebugLogger()

    Log_Error  ( "A severe error happened :-(" )
    Log_Warning( "This is a warning :-/ Maybe an error follows?" )
    Log_Info   ( "Just for your further information!" )
    Log_Verbose( "Today, I am in the mood to talk..." )
    //! [Tut_Verbosity]

    //! [Tut_Verbosity_SetVerbosity]
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning )
    //! [Tut_Verbosity_SetVerbosity]
    //! [Tut_Verbosity_SetVerbosity_2]
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Warning )
    //! [Tut_Verbosity_SetVerbosity_2]

    Log_Prune( ut.WriteResultFile( "Tut_Verbosity.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )
    Log_Prune( Log_RemoveDebugLogger();  )

    Log_SetVerbosity( &memLog, Verbosity::Warning )

    //! [Tut_Verbosity_2]
    Log_AddDebugLogger()

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning )

    Log_Error  ( "A severe error happened :-(" )
    Log_Warning( "This is a warning :-/ Maybe an error follows?" )
    Log_Info   ( "Just for your further information!" )
    Log_Verbose( "Today, I am in the mood to talk..." )
    //! [Tut_Verbosity_2]

    Log_Prune( ut.WriteResultFile( "Tut_Verbosity_2.txt", memLog.MemoryLog, EmptyNString() ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}


// #################################################################################################
// Tut_Domains
// #################################################################################################
UT_METHOD(Tut_Domains)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    //! [Tut_Domains]
    Log_AddDebugLogger()
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose ) // the default anyhow
    //...
    Log_Verbose( "HTTP", "Connected" )
    //...
    //...
    //...
    Log_Verbose( "UI",   "Somebody moved the mouse!" )
    //...
    //! [Tut_Domains]


    Log_Prune( ut.WriteResultFile( "Tut_Domains.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    Log_Prune( Log_RemoveDebugLogger();  )

    Log_SetVerbosity( &memLog, Verbosity::Verbose,  "HTTP" ) // our interest
    Log_SetVerbosity( &memLog, Verbosity::Error,    "UI"   ) // only if ouch!

    //! [Tut_Domains_2]
    Log_AddDebugLogger()

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose,  "HTTP" ) // our interest
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Error,    "UI"   ) // only if ouch!
    //...
    Log_Verbose( "HTTP", "Connected" )
    //...
    //...
    //...
    Log_Verbose( "UI",   "Somebody moved the mouse!" )
    //...
    //! [Tut_Domains_2]

    Log_Prune( ut.WriteResultFile( "Tut_Domains_2.txt", memLog.MemoryLog, EmptyNString() ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()

    // clean the config (for subsequent tests)
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Clear();
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Clear();
}

// #################################################################################################
// Tut_Domains
// #################################################################################################
UT_METHOD(Tut_HierDom)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    //! [Tut_DomainsHierarchical]
    Log_AddDebugLogger()
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose ) // the default anyhow
    //...
    Log_Info   ( "UI/MOUSE",   "A mouse click" )
    //...
    Log_Verbose( "UI/MOUSE",   "Somebody moved the mouse!" )
    //...
    //...
    Log_Info   ( "UI/DLG",     "About dialog opend" )
    //...
    Log_Verbose( "UI/DLG",     "About dialog, link to product page pressed." )
    //...
    //! [Tut_DomainsHierarchical]


    Log_Prune( ut.WriteResultFile( "Tut_DomainsHierarchical.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )
    Log_RemoveDebugLogger()

    //! [Tut_DomainsHierarchical_2]
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "UI"  ) // Always sets all sub-domains!
    //! [Tut_DomainsHierarchical_2]

    //! [Tut_DomainsHierarchical_3]
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "UI"       ) // First set parent...
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "UI/MOUSE" ) // ...then children!
    //! [Tut_DomainsHierarchical_3]

    Log_RemoveLogger( &memLog )

    // clean the config (for subsequent tests)
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::DefaultValues   )->Clear();
    lib::ALOX.GetConfig().GetPluginTypeSafe<aworx::InMemoryPlugin>( Priorities::ProtectedValues )->Clear();
}


// #################################################################################################
// Tut_ScopeDomains
// #################################################################################################
#if ALOX_DBG_LOG
UT_METHOD(Tut_ScopeDomains)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)
    Log_AddDebugLogger()

    TutScopeDom tsd;
    tsd.Extract( "myfile.zip", nullptr );

    Log_Prune( ut.WriteResultFile( "Tut_ScopeDomains.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )

    // do it once to set the tab positions of the meta info...
    {
        Zipper zip;
        zip.Compress( "myfile.zip", nullptr );
        zip.Extract( "myfile.zip", nullptr );
        Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    }
    // ...and again
    {
        Zipper zip;
        zip.Compress( "myfile.zip", nullptr );
        zip.Extract( "myfile.zip", nullptr );
    }

    Log_Prune( ut.WriteResultFile( "Tut_ScopeDomains_Zipper.txt", memLog.MemoryLog, EmptyNString() ); )

    //---------- with scope path ---------------

    //! [Tut_ScopeDomains_Path]
    Log_SetDomain( "UTIL", Scope::Path )
    //! [Tut_ScopeDomains_Path]

    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )
    // do it once to set the tab positions of the meta info...
    {
        Zipper zip;
        zip.Compress( "myfile.zip", nullptr );
        zip.Extract( "myfile.zip", nullptr );
        Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    }
    // ...and again
    {
        Zipper zip;
        zip.Compress( "myfile.zip", nullptr );
        zip.Extract( "myfile.zip", nullptr );
    }
    Log_Prune( ut.WriteResultFile( "Tut_ScopeDomains_Zipper_Path.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )

    Log_SetDomain( nullptr, Scope::Path )
    Log_SetDomain( nullptr, Scope::Filename )

    //! [Tut_ScopeDomains_ParamDom]
    Log_SetDomain( "METHOD", Scope::Method )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )
    //! [Tut_ScopeDomains_ParamDom]

    // clear autosizes, repeat it twice
    Log_Prune( memLog.AutoSizes.Reset(); )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )

    Log_Prune( ut.WriteResultFile( "Tut_ScopeDomains_ParamDom.txt", memLog.MemoryLog, EmptyNString() ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )


    //! [Tut_ScopeDomains_ParamDom_2]
    Log_SetDomain( "READ", Scope::Method )
    Log_Info( "Reading file" )
    //...
    //...
    Log_Info( "/CONFIG",   "Path not found." )
    //...
    //! [Tut_ScopeDomains_ParamDom_2]
    Log_Prune( ut.WriteResultFile( "Tut_ScopeDomains_ParamDom_2.txt", memLog.MemoryLog, EmptyNString() ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}
#endif


// #################################################################################################
// Tut_Prefix
// #################################################################################################
UT_METHOD(Tut_Prefix)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)
    Log_AddDebugLogger()

    //! [Tut_Prefix]
    Log_SetPrefix( "ALOX TUTORIAL: ", Scope::Method )

    Log_Info( "Well, just a sample" )
    //! [Tut_Prefix]

    Log_Prune( ut.WriteResultFile( "Tut_Prefix.txt", memLog.MemoryLog, EmptyNString() ); )

    //! [Tut_Prefix_2]
    Log_SetPrefix( ESC::BG_MAGENTA, Scope::Filename )
    //! [Tut_Prefix_2]

    Log_Warning( "magenta" )

    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}

// #################################################################################################
// Tut_ThreadName
// #################################################################################################
UT_METHOD(Tut_ThreadName )
{
    UT_INIT()

    Log_Prune ( MemoryLogger memLog; )
    Log_SetVerbosity   ( &memLog, Verbosity::Verbose )
    Log_Prune ( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )


    //! [Tut_MapThreadName]
    Log_MapThreadName( A_CHAR("BKGRND") )
    Log_Info ( "Hello ALox" )
    //! [Tut_MapThreadName]
    Log_MapThreadName( A_CHAR("MAIN") )


    Log_RemoveLogger( & memLog)
    Log_Prune( memLog.MemoryLog.SearchAndReplace( A_CHAR("MONOMEM"), A_CHAR("CONSOLE") );            )
    Log_Prune( ut.WriteResultFile( "Tut_ThreadName.txt", memLog.MemoryLog, EmptyNString() ); )

}


// #################################################################################################
// Tut_ConditionalLogging
// #################################################################################################

// restore original ALib param macro
#undef  ALIB_CALLER
#if ALOX_DBG_LOG
    #define ALIB_CALLER     __FILE__, __LINE__, __func__
#endif


// #################################################################################################
// Tut_LogState
// #################################################################################################
UT_METHOD(Tut_LogState)
{
    UT_INIT()

    //! [Tut_LogState]
    // create two different loggers
    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )

    // reduce meta information to limit tutorial output width
    Log_Prune( Log::DebugLogger->MetaInfo->Format.Reset( A_CHAR("[%tN]%V[%D](%#): " ) );  )
    Log_Prune( memLogger.MetaInfo->Format.Reset(         A_CHAR("[%tN]%V[%D](%#): " ) );  )
    Log_Prune( memLogger.MultiLineMsgMode= 3; )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )

    // OK, let's use ALox
    Log_SetDomain( "PNS"   ,   Scope::Path + 1 )
    Log_SetDomain( "PATH",     Scope::Path )
    Log_SetDomain( "FN",       Scope::Filename )
    Log_SetDomain( "THREAD",   Scope::ThreadOuter )

    Log_SetVerbosity( "MEMORY",        Verbosity::Off      , "/CON"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Verbose              )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Off      , "/MEM"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Error    , "/UI"     )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Info     , "/UI/DLG" )

    Log_Info( "This goes to both loggers" )
    Log_Info( "/MEM", "This goes only to the memory logger" )
    Log_Info( "/CON", "This goes only to the console logger" )

    Log_Once( "Will we see this in the config?" )
    Log_Once( "Will we see this in the config?", A_CHAR("ONCEKEY"), Scope::Filename )

    Log_Store( "MyData 1" ,            Scope::Method )
    Log_Store( "MyData 2" , "DataKey", Scope::Method )
    Log_Store( 3          , "DataKey", Scope::Filename )
    Log_Store( 4          , "DataKey", Scope::ThreadOuter )

    Log_SetPrefix( "TPre: "  , Scope::ThreadOuter )
    Log_SetPrefix( "MPre: "  , Scope::Method )
    Log_SetPrefix( "DomPre: " )
    Log_SetPrefix( "Mouse: ", "/UI/MOUSE" )
    Log_SetPrefix( ESC::RED,  "/ERRORS", Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )

    // now, log the current config
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("The current configuration of this Lox is:") )
    //! [Tut_LogState]

    Log_Prune( ut.WriteResultFile( "Tut_LogState.txt", memLogger.MemoryLog, EmptyNString() ); )
    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )
}


UT_METHOD(Tut_LogInternalDomains)
{
    UT_INIT()

    {
    //! [Tut_LogInternalDomains]
    // This is the very same code as above...
    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )

    Log_Prune( Log::DebugLogger->MetaInfo->Format.Reset("[%tN]%V[%D](%#): ");   )
    Log_Prune(         memLogger.MetaInfo->Format.Reset("[%tN]%V[%D](%#): ");   )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )

    // ... with one difference: we are activating the internal domain
    Log_SetVerbosity( &memLogger,       Verbosity::Verbose, Lox::InternalDomains )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )

    Log_SetDomain( "PNS"   ,   Scope::Path + 1    )
    Log_SetDomain( "PATH",     Scope::Path        )
    Log_SetDomain( "FN",       Scope::Filename    )
    Log_SetDomain( "THREAD",   Scope::ThreadOuter )

    Log_SetVerbosity( "MEMORY",        Verbosity::Off      , "/CON"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Verbose              )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Off      , "/MEM"    )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Error    , "/UI"     )
    Log_SetVerbosity( "DEBUG_LOGGER" , Verbosity::Info     , "/UI/DLG" )

    Log_Once( "Will we see this in the config?" )
    Log_Once( "Will we see this in the config?", A_CHAR("ONCEKEY"), Scope::Filename )

    Log_Store( "MyData 1" ,            Scope::Method )
    Log_Store( "MyData 2" , "DataKey", Scope::Method )
    Log_Store( 3          , "DataKey", Scope::Filename )
    Log_Store( 4          , "DataKey", Scope::ThreadOuter )

    Log_SetPrefix( "TPre: "  , Scope::ThreadOuter )
    Log_SetPrefix( "MPre: "  , Scope::Method )
    Log_SetPrefix( "DomPre: " )
    Log_SetPrefix( "Mouse: ", "/UI/MOUSE" )
    Log_SetPrefix( ESC::RED,  "/ERRORS", Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )
    //! [Tut_LogInternalDomains]
    Log_Prune( ut.WriteResultFile( "Tut_LogInternalDomains.txt", memLogger.MemoryLog, EmptyNString() ); )

    Log_RemoveDebugLogger()
    Log_RemoveLogger( "MEMORY" )
    }
}


#if ALOX_DBG_LOG
UT_METHOD(Tut_LogData)
{
    UT_INIT()


    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )

    // set auto tabs
    {
        Log_Info( "X" )
        Log_Prune( memLogger.MemoryLog.Reset(); )
    }


    FileIO fileIo;
    fileIo.Read( "myfile.dat" );

    //! [Tut_LogData_2]
    Log_Retrieve( dbgFileVersion, "FILE_VERSION" )
    Log_Info( "Working on file version {!Q}", dbgFileVersion.Unbox<String>() )
    //! [Tut_LogData_2]

    Log_Prune( ut.WriteResultFile( "Tut_LogData.txt", memLogger.MemoryLog, EmptyNString() ); )

    //! [Tut_LogData_3]
    Log_Info( "Working on file version {!Q}", LOG_LOX.Retrieve("FILE_VERSION").Unbox<String>() )
    //! [Tut_LogData_3]


    Log_RemoveDebugLogger()
    Log_RemoveLogger( "MEMORY" )

}

UT_METHOD(Tut_Format)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, Lox::InternalDomains )


    //! [Tut_Format_1]
    Log_Info( "Value=", 5 )
    //! [Tut_Format_1]
    Log_Prune( ut.WriteResultFile( "Tut_Format_1.txt", memLogger.MemoryLog, EmptyNString() ); )

    //! [Tut_Format_P]
    Log_Info( "Value={}", 5 )
    //! [Tut_Format_P]

    //! [Tut_Format_J]
    Log_Info( "Value=%s", 5 )
    //! [Tut_Format_J]


    //! [Tut_Format_Multi]
    Log_Info( "One-", "Two-", "Three" )
    Log_Info( "{}-{}-{}", "One", "Two", "Three" )
    Log_Info( "{}-{}-"  , "One", "Two", "Three" )
    Log_Info( "{}-"  , "One", "{}-", "Two", "{}", "Three" )
    //! [Tut_Format_Multi]

    Log_Prune( memLogger.MemoryLog.Reset(); )
    //! [Tut_Format_Mix]
    Log_Info( "Python Style: {!s}","PS", " - ", "Java Style: \"%s\"", "JS" )
    //! [Tut_Format_Mix]
    Log_Prune( ut.WriteResultFile( "Tut_Format_Mix.txt", memLogger.MemoryLog, EmptyNString() ); )

    Log_Prune( memLogger.MemoryLog.Reset(); )
    //! [Tut_Format_Sample_1]
    Log_Info( ">{:<10}<" , "left" )
    Log_Info( ">{:>10}<" , "right" )
    Log_Info( ">{:^10}<" , "center" )
    Log_Info( ">{:10.3}<", 12.3456789 )

    Log_Info( "Tab:{!Tab12}", "Stop" )

    Log_Info( "Auto Tab:{!ATab}", "Stop" )
    Log_Info( "Auto Tab XXX:{!ATab}", "Stop" )
    Log_Info( "Auto Tab:{!ATab}", "Stop" )

    Log_Info( "A quoted {!Q} string", "Placeholder" )
    Log_Info( "A quoted {!Q} number", 395 )

    Log_Info( "Upper {0!Q!up} and lower {0!Q!lo} conversion", "CaSe" )

    Log_Info( "Hex: {:#x}. With group chars: {0:x,}", 0x11FF22EE )
    Log_Info( "Oct: {:#o}. With group chars: {0:o,}", 012345670 )
    Log_Info( "Bin: {:#b}. With group chars: {0:b,}", 145 )
    //! [Tut_Format_Sample_1]
    Log_Prune( ut.WriteResultFile( "Tut_Format_Sample_1.txt", memLogger.MemoryLog, EmptyNString() ); )


    Log_RemoveDebugLogger()
    Log_RemoveLogger( "MEMORY" )
}

#endif


#include "unittests/aworx_unittests_end.hpp"

} // namespace

#endif // ALIB_UT_ALOX

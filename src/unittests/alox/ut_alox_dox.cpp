// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_ALOX

DOX_MARKER([ALoxTut_include_statement])
#include "alib/alox.hpp"
#include "alib/alox/aloxcamp.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/loggers/ansilogger.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/lang/callerinfo_functions.hpp"
DOX_MARKER([ALoxTut_include_statement])

#include <iostream>
#include <fstream>
#include <string>
#include <list>



DOX_MARKER([ALoxTut_using_statement])
using namespace std;
using namespace alib;
DOX_MARKER([ALoxTut_using_statement])


DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER_0])
#include "alib/alox/reportwriter.hpp"
DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER_0])

void docSampleInstallReportWriter();
void docSampleInstallReportWriter()
{
Log_Prune( Lox lox("RELEASE");               )
Log_Prune( Lox_Prune( Logger* myReleaseLogger= nullptr; ) )

#if ALOX_DBG_LOG
    #define LOX_LOX lox
#endif

DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER])
Log_Prune( Log::AddALibReportWriter( &LOX_LOX ); )
Log_Prune( Lox_SetVerbosity( myReleaseLogger, Verbosity::Info, lox::ALoxReportWriter::LogDomain() ); )
DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER])

DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER_2])
Log_Prune( Log::RemoveALibReportWriter() );
DOX_MARKER([DOX_ALOX_INSTALL_REPORT_WRITER_2])
}

void docSampleESC();
void docSampleESC()
{
DOX_MARKER([DOX_ALOX_ESC])
Log_Info( "The result is: ", ESC::RED, 42 )
 DOX_MARKER([DOX_ALOX_ESC])
}



#define main           tutAloxMain
int main( int, const char** );
DOX_MARKER([ALoxTut_Minimum_Hello])
#include "alib/alox.hpp"

int main( int, const char** )
{
    // bootstrap ALib
    alib::Bootstrap();

    // the main program
    Log_Info ( "Hello ALox!" )

    // alib termination
    alib::Shutdown();
    return 0;
}
DOX_MARKER([ALoxTut_Minimum_Hello])
#undef main
#define main    tutAloxMain2
int main( int argc, const char** argv);

DOX_MARKER([ALoxTut_Minimum_Hello2])
#include "alib/alox.hpp"

int main( int argc, const char** argv)
{
    // bootstrap ALib
    alib::ARG_C  = argc;
    alib::ARG_VN = argv;
    alib::Bootstrap();

    // the main program
    Log_Info ( "Hello ALox!" )

    // alib termination
    alib::Shutdown();
    return 0;
}
DOX_MARKER([ALoxTut_Minimum_Hello2])
#undef main

// #################################################################################################
// #################################################################################################
// ################################       ALox Manual Samples       ################################
// #################################################################################################
// #################################################################################################
namespace ut_alox_manual {

void Domains_Hierarchical();
void Domains_Hierarchical()
{
DOX_MARKER([Man_DOMAINS___1])
Log_SetVerbosity( Log::DebugLogger, Verbosity::Error,   "/" ) // could also just omit parameter "/"
DOX_MARKER([Man_DOMAINS___1])

DOX_MARKER([Man_DOMAINS___2])
Log_SetVerbosity( Log::DebugLogger, Verbosity::Info,    "/UI" )
Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "/UI/DIALOGS" )
DOX_MARKER([Man_DOMAINS___2])

DOX_MARKER([Man_DOMAINS___2reverse])
Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "/UI/DIALOGS" )
Log_SetVerbosity( Log::DebugLogger, Verbosity::Info,    "/UI" )
DOX_MARKER([Man_DOMAINS___2reverse])
}


void MyMethod();
DOX_MARKER([Man_DOMAINS___3])
void MyMethod()
{
    Log_SetDomain( "/MYDOM", Scope::Method )

    Log_Info( "This log statment uses domain /MYDOM" )
}
DOX_MARKER([Man_DOMAINS___3])


struct ManualSampleCode2
{
static
DOX_MARKER([Man_DOMAINS___4])
void MyMethod()
{
    Log_SetDomain( "/MYDOM", Scope::Method )

    Log_Info(           "This log statment uses domain '/MYDOM'" )
    Log_Info( "/MYDOM", "Of course we can still use domains explicitly" )
}
DOX_MARKER([Man_DOMAINS___4])
};


struct ManualSampleCode5
{
static
DOX_MARKER([Man_DOMAINS___5])
void MyMethod()
{
    Log_SetDomain( "/MYDOM", Scope::Method )

    Log_Info(          "This log statment uses domain 'MYDOM'" )
    Log_Info( "MYDOM", "Oooops, this goes to '/MYDOM/MYDOM'!" )
}
DOX_MARKER([Man_DOMAINS___5])

static
void PrefixLogablesNotCalled()
{
DOX_MARKER([Man_PREFIXLOGABLES_1])
Log_SetPrefix( ">>> " )
Log_Info( "With prefix" )

Log_SetPrefix( lox::ESC::MAGENTA )
Log_Info( "With prefix, now also in magenta" )
DOX_MARKER([Man_PREFIXLOGABLES_1])

DOX_MARKER([Man_PREFIXLOGABLES_2])
Log_Prune(  BoxesHA myPrefixes;
            myPrefixes.Add( "Never forget: 6 * 7 = " );
            myPrefixes.Add( 6 * 7 );
            myPrefixes.Add( ": "  );                       )
Log_SetPrefix( myPrefixes )
//...
//...
//...
// Somewhere else:
Log_Info( "Let's hope the lifecycle of the myPrefixes encloses this log statement!" )
DOX_MARKER([Man_PREFIXLOGABLES_2])
}

static
void PrefixLogablesLifecycle()
{
DOX_MARKER([Man_PREFIXLOGABLES_Lifecycle])
// Adding a string buffer as prefix
Log_Prune(      AString prefix( "Orig: " );                              )
Log_SetPrefix(  prefix                                                   )
Log_Info(       "Testlog before change of AString"                       )

// changing the buffer, does NOT change the prefix, because ALib volunteered
// to copy the string buffer.
Log_Prune(      prefix.Reset("Changed: ");                               )
Log_Info(       "Testlog after change of AString (was not effecitve)"    )

// remove the previous prefix and add it again wrapped as reference
Log_SetPrefix(  nullptr                                                  )
Log_Prune(      prefix.Reset("Orig: ");                                  )
Log_SetPrefix(  std::reference_wrapper(prefix)                           )
Log_Info(       "Testlog before change of AString"                       )

// now changing the buffer, does also change the prefix.
// But: We are now responsible that the lifecycle of our string buffer
// supersedes all log statements!
Log_Prune(      prefix.Reset("Changed: ");                               )
Log_Info(       "Testlog after change of AString (now it is effecitve!)" )
DOX_MARKER([Man_PREFIXLOGABLES_Lifecycle])
Log_SetPrefix(  nullptr                                                  )
}


void TrimSource()
{
DOX_MARKER([Man_TRIM_SOURCE_PATH])
// hard-coded trim rule
Log_SetSourcePathTrimRule( "*/myapp/src/", lang::Inclusion::Include )
DOX_MARKER([Man_TRIM_SOURCE_PATH])
}
};

void ReadChangeAndWriteBack();

DOX_MARKER([Man_DOMAINS___RelativePaths])
void ReadChangeAndWriteBack()
{
    Log_SetDomain( "/IO", Scope::Method )

    // Reading file
    Log_Info( "READ",   "Reading file" )
    // ...
    // ...
    // ...

    // Process file
    Log_Info( "PROCESS",  "Processing data" )
    // ...
    // ...
    // ...

    // Writing file
    Log_Info( "./WRITE",  "Writing file" ) // note relative-path-prefix "./", same as if omitted (!)
    // ...
    // ...
    // ...

    Log_Info(  "Success!" )
}
DOX_MARKER([Man_DOMAINS___RelativePaths])

void unnamed();
void unnamed()
{
    DOX_MARKER([Man_DOMAINS___RelativePaths_Set])
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "./" )
    DOX_MARKER([Man_DOMAINS___RelativePaths_Set])
}

DOX_MARKER([Man_DOMAINS___nested_scope])
struct IO
{
    // constructor setting the Scope Domain for this file (class) once
    IO()
    {
        Log_SetDomain( "IO", Scope::Filename )
    }

    // interface
    void ReadChangeAndWriteBack()
    {
        checkSetup();
        read();
        process();
        write();
        writeStats();
    }

    // private methods
  private:
    void checkSetup()
    {
        Log_SetDomain( "/CHECKS", Scope::Method )

        Log_Info(  "Setup OK!" )
    }


    void read()
    {
        Log_SetDomain( "READ", Scope::Method )

        Log_Info("Reading file" )
    }

    void process()
    {
        Log_SetDomain( "PROCESS", Scope::Method )

        Log_Info( "Processing data" )
    }

    void write()
    {
        Log_SetDomain( "./WRITE", Scope::Method )

        Log_Info( "Writing file" )
    }

    void writeStats()
    {
        Log_SetDomain( "../STATS", Scope::Method )

        Log_Info(  "Statistics" )
    }
};
DOX_MARKER([Man_DOMAINS___nested_scope])


// #################################################################################################
// ### Log_Once Samples
// #################################################################################################

class ManualSampleCodeLogOnce
{
void LogOnce()
{
DOX_MARKER([Man_LogOnce___1])
Log_Once( "Kilroy was here!" )
DOX_MARKER([Man_LogOnce___1])

DOX_MARKER([Man_LogOnce___1_wrapup])
Log_Once( "This is what happened: ..." )
DOX_MARKER([Man_LogOnce___1_wrapup])

DOX_MARKER([Man_LogOnce___2])
Log_Once( "IO", Verbosity::Error, "Directory given in config.ini not found. Using default." )
DOX_MARKER([Man_LogOnce___2])

DOX_MARKER([Man_LogOnce___4])
Log_Once( "This is logged 10 times. After that, never again.", 10 )
DOX_MARKER([Man_LogOnce___4])

DOX_MARKER([Man_LogOnce___5])
Log_Once( "This is logged the first time and then every 100th invocation.", -100 )
DOX_MARKER([Man_LogOnce___5])
}

DOX_MARKER([Man_LogOnce___11])
void ReadData()
{
    //...

    // directory not found
    Log_Once( "IO", Verbosity::Warning,
              "Directory given in config.ini not found. Using default." )
    //...
}

void WriteData()
{
    //...

    // directory not found
    Log_Once( "IO", Verbosity::Warning,
              "Directory given in config.ini not found. Using default." )
    //...
}
DOX_MARKER([Man_LogOnce___11])

}; // class ManualSampleCodeLogOnce

class ManualSampleCodeLogOnce2
{
DOX_MARKER([Man_LogOnce___12])
void ReadData()
{
    //...

    // directory not found
    Log_Once( "IO", Verbosity::Warning,
              "Directory given in config.ini not found. Using default.",
              A_CHAR("INI_DIR_ERROR") )
    //...
}
void WriteData()
{
    //...

    // directory not found
    Log_Once( "IO", Verbosity::Warning,
              "Directory given in config.ini not found. Using default.",
              A_CHAR("INI_DIR_ERROR") )
    //...
}
DOX_MARKER([Man_LogOnce___12])
}; // class


DOX_MARKER([Man_LogOnce___21])
class MyIOManager
{
    MyIOManager()
    {
        // bind all log statements of this file to domain path 'IO'
        Log_SetDomain( "IO", Scope::Filename )
    }

    void ReadData()
    {
        //...

        // directory not found
        Log_Once( Verbosity::Warning,
                  "Directory given in config.ini not found. Using default.",
                  Scope::Filename )
        //...
    }
    void WriteData()
    {
        //...

        // directory not found
        Log_Once( Verbosity::Warning,
                  "Directory given in config.ini not found. Using default.",
                  Scope::Filename )
        //...
    }

}; // class
DOX_MARKER([Man_LogOnce___21])



// #################################################################################################
// ### Log_SetPrefix Samples
// #################################################################################################
void LogSetPrefix();
void LogSetPrefix()
{
DOX_MARKER([Man_LogSetPrefix___1])
Log_SetPrefix( "Data File: ", Scope::Filename )
//...
//...
Log_Info( "Opened." )
//...
//...
Log_Info( "Read." )
//...
//...
Log_Info( "Closed." )
DOX_MARKER([Man_LogSetPrefix___1])
Log_SetPrefix( nullptr, Scope::Filename )

}

struct RecursiveDataType
{
    String                       Name;
    std::list<RecursiveDataType> Children;

    RecursiveDataType() { Name= A_CHAR(""); }


DOX_MARKER([Man_LogSetPrefix___2])
RecursiveDataType* Search( String name )
{
    Log_SetPrefix( "  ", Scope::ThreadOuter ) // add indent

    Log_Info( "Inspecting object: ", Name )

    if ( Name.Equals ( name ) )
    {
        Log_SetPrefix( nullptr, Scope::ThreadOuter ) // remove indent
        return this;
    }

    // recursion
    RecursiveDataType* returnValue= nullptr;
    for( RecursiveDataType& child : Children )
        if( (returnValue= child.Search( name )) != nullptr )
            break;

    Log_SetPrefix( nullptr, Scope::ThreadOuter ) // remove indent
    return returnValue;
}
DOX_MARKER([Man_LogSetPrefix___2])

}; //class ManualSampleCodeSetPrefix


// #################################################################################################
// ### Man.InternalDomains Samples
// #################################################################################################

class Manual_InternalDomains
{
void x()
{
DOX_MARKER([Man_InternalDomains___1])
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, "/" )
DOX_MARKER([Man_InternalDomains___1])

DOX_MARKER([Man_InternalDomains___2])
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, Lox::InternalDomains )
DOX_MARKER([Man_InternalDomains___2])

DOX_MARKER([Man_InternalDomains___3])
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, "$/" )
DOX_MARKER([Man_InternalDomains___3])

DOX_MARKER([Man_InternalDomains___4])
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Off    , Lox::InternalDomains )
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, NString64(Lox::InternalDomains) << "DMN" )
DOX_MARKER([Man_InternalDomains___4])

DOX_MARKER([Man_InternalDomains_ext___3])
Log_Info( NString64(Lox::InternalDomains) << "MYDOM", "Hello, this logs 'on' internal domain '$/MYDOM'!" )
DOX_MARKER([Man_InternalDomains_ext___3])

}

}; //class Manual_InternalDomains

} // namespace ut_alox_manual

// #################################################################################################
// #################################################################################################
// ### START of Unit tests
// #################################################################################################
// #################################################################################################


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

    DOX_MARKER([ALoxTut_ConditionalLogging])
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
    DOX_MARKER([ALoxTut_ConditionalLogging])

    DOX_MARKER([ALoxTut_ConditionalLogging2])
    Log_Assert( i != len, "Nothing found :-(" )
    DOX_MARKER([ALoxTut_ConditionalLogging2])

    DOX_MARKER([ALoxTut_ConditionalLogging3])
    Log_If( i == len, Verbosity::Error, "Nothing found :-(" )
    DOX_MARKER([ALoxTut_ConditionalLogging3])

    DOX_MARKER([ALoxTut_ConditionalLoggingOnce])
    Log_Once( "I tell you this now only once!" )
    DOX_MARKER([ALoxTut_ConditionalLoggingOnce])
    ALIB_WARNINGS_RESTORE
}

// #################################################################################################
// ALoxTut_ScopeDomains
// #################################################################################################
#if ALOX_DBG_LOG
class TutScopeDom
{
    public:
    DOX_MARKER([ALoxTut_ScopeDomains])
    void* Extract( const alib::NString& fileName, void* buffer )
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
    DOX_MARKER([ALoxTut_ScopeDomains])
};

DOX_MARKER([ALoxTut_ScopeDomains_Zipper])
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

    void* Compress( const alib::NString& fileName, void* buffer )
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

    void* Extract( const alib::NString& fileName, void* buffer )
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
DOX_MARKER([ALoxTut_ScopeDomains_Zipper])

// #################################################################################################
// ALoxTut_LogData
// #################################################################################################
DOX_MARKER([ALoxTut_LogData])
class FileIO
{
    public:

    void Read( const alib::NString& fileName )
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
DOX_MARKER([ALoxTut_LogData])

#endif

#define TESTCLASSNAME       CPP_Dox_Tutorial
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {

/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/

UT_CLASS

// #################################################################################################
// Hello_ALox
// #################################################################################################

UT_METHOD(Hello_ALox)
{
    UT_INIT()

    Log_Prune( if ( Log::DebugLogger != nullptr ) )
                   Log_RemoveDebugLogger()

    Log_Prune( MemoryLogger memLog; )



    DOX_MARKER([ALoxTut_Logger_1])
    Log_AddDebugLogger()

    DOX_MARKER([ALoxTut_Logger_1])

    Log_RemoveDebugLogger()

    Log_SetVerbosity( &memLog, Verbosity::Verbose )

    DOX_MARKER([ALoxTut_Logger_2])
    // Dont forget to bootstrap on top of main()! It is removed here, because this code runs
    // in the unit tests, where bootstrapping was already performed.
    // alib::Bootstrap();

    Log_AddDebugLogger()
    Log_Info ( "Hello ALox" )
    DOX_MARKER([ALoxTut_Logger_2])

    Log_Prune( ut.WriteResultFile( "ALoxTut_Logger.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}

// #################################################################################################
// ALoxTut_Verbosity
// #################################################################################################
UT_METHOD(ALoxTut_Verbosity)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    DOX_MARKER([ALoxTut_Verbosity])
    Log_AddDebugLogger()

    Log_Error  ( "A severe error happened :-(" )
    Log_Warning( "This is a warning :-/ Maybe an error follows?" )
    Log_Info   ( "Just for your further information!" )
    Log_Verbose( "Today, I am in the mood to talk..." )
    DOX_MARKER([ALoxTut_Verbosity])

    DOX_MARKER([ALoxTut_Verbosity_SetVerbosity])
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning )
    DOX_MARKER([ALoxTut_Verbosity_SetVerbosity])

    DOX_MARKER([ALoxTut_Verbosity_SetVerbosity_2])
    Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Warning )
    DOX_MARKER([ALoxTut_Verbosity_SetVerbosity_2])

    Log_Prune( ut.WriteResultFile( "ALoxTut_Verbosity.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )
    Log_Prune( Log_RemoveDebugLogger();  )

    Log_SetVerbosity( &memLog, Verbosity::Warning )

    DOX_MARKER([ALoxTut_Verbosity_2])
    Log_AddDebugLogger()

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning )

    Log_Error  ( "A severe error happened :-(" )
    Log_Warning( "This is a warning :-/ Maybe an error follows?" )
    Log_Info   ( "Just for your further information!" )
    Log_Verbose( "Today, I am in the mood to talk..." )
    DOX_MARKER([ALoxTut_Verbosity_2])

    Log_Prune( ut.WriteResultFile( "ALoxTut_Verbosity_2.txt", memLog.MemoryLog, EMPTY_NSTRING ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}


// #################################################################################################
// ALoxTut_Domains
// #################################################################################################
UT_METHOD(ALoxTut_Domains)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    DOX_MARKER([ALoxTut_Domains])
    Log_AddDebugLogger()
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose ) // the default anyhow
    //...
    Log_Verbose( "HTTP", "Connected" )
    //...
    //...
    //...
    Log_Verbose( "UI",   "Somebody moved the mouse!" )
    //...
    DOX_MARKER([ALoxTut_Domains])


    Log_Prune( ut.WriteResultFile( "ALoxTut_Domains.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    Log_Prune( Log_RemoveDebugLogger();  )

    Log_SetVerbosity( &memLog, Verbosity::Verbose,  "HTTP" ) // our interest
    Log_SetVerbosity( &memLog, Verbosity::Error,    "UI"   ) // only if ouch!

    DOX_MARKER([ALoxTut_Domains_2])
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
    DOX_MARKER([ALoxTut_Domains_2])

    Log_Prune( ut.WriteResultFile( "ALoxTut_Domains_2.txt", memLog.MemoryLog, EMPTY_NSTRING ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}

// #################################################################################################
// ALoxTut_Domains
// #################################################################################################
UT_METHOD(Tut_HierDom)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)

    DOX_MARKER([ALoxTut_DomainsHierarchical])
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
    DOX_MARKER([ALoxTut_DomainsHierarchical])


    Log_Prune( ut.WriteResultFile( "ALoxTut_DomainsHierarchical.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_Prune( memLog.MemoryLog.Reset(); memLog.CntLogs= 0; )
    Log_RemoveDebugLogger()

    DOX_MARKER([ALoxTut_DomainsHierarchical_2])
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "UI"  ) // Always sets all Subdomains!
    DOX_MARKER([ALoxTut_DomainsHierarchical_2])

    DOX_MARKER([ALoxTut_DomainsHierarchical_3])
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Warning, "UI"       ) // First set parent...
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, "UI/MOUSE" ) // ...then children!
    DOX_MARKER([ALoxTut_DomainsHierarchical_3])

    Log_RemoveLogger( &memLog )
}


// #################################################################################################
// ALoxTut_ScopeDomains
// #################################################################################################
#if ALOX_DBG_LOG
UT_METHOD(ALoxTut_ScopeDomains)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)
    Log_AddDebugLogger()

    TutScopeDom tsd;
    tsd.Extract( "myfile.zip", nullptr );

    Log_Prune( ut.WriteResultFile( "ALoxTut_ScopeDomains.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
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

    Log_Prune( ut.WriteResultFile( "ALoxTut_ScopeDomains_Zipper.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

    //---------- with scope path ---------------

    DOX_MARKER([ALoxTut_ScopeDomains_Path])
    Log_SetDomain( "UTIL", Scope::Path )
    DOX_MARKER([ALoxTut_ScopeDomains_Path])

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
    Log_Prune( ut.WriteResultFile( "ALoxTut_ScopeDomains_Zipper_Path.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )

    Log_SetDomain( nullptr, Scope::Path )
    Log_SetDomain( nullptr, Scope::Filename )

    DOX_MARKER([ALoxTut_ScopeDomains_ParamDom])
    Log_SetDomain( "METHOD", Scope::Method )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )
    DOX_MARKER([ALoxTut_ScopeDomains_ParamDom])

    // clear autosizes, repeat it twice
    Log_Prune( memLog.GetAutoSizes().Main.Reset(); )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )
    Log_Info(          "No domain parameter given" )
    Log_Info( "PARAM", "Domain parameter \"PARAM\" given" )

    Log_Prune( ut.WriteResultFile( "ALoxTut_ScopeDomains_ParamDom.txt", memLog.MemoryLog, EMPTY_NSTRING ); )
    Log_Prune( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )


    DOX_MARKER([ALoxTut_ScopeDomains_ParamDom_2])
    Log_SetDomain( "READ", Scope::Method )
    Log_Info( "Reading file" )
    //...
    //...
    Log_Info( "/CONFIG",   "Path not found." )
    //...
    DOX_MARKER([ALoxTut_ScopeDomains_ParamDom_2])
    Log_Prune( ut.WriteResultFile( "ALoxTut_ScopeDomains_ParamDom_2.txt", memLog.MemoryLog, EMPTY_NSTRING ); )


    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}
#endif


// #################################################################################################
// ALoxTut_Prefix
// #################################################################################################
UT_METHOD(ALoxTut_Prefix)
{
    UT_INIT()

    MemoryLogger memLog;
    Log_SetVerbosity( &memLog, Verbosity::Verbose)
    Log_AddDebugLogger()

    DOX_MARKER([ALoxTut_Prefix])
    Log_SetPrefix( "ALOX TUTORIAL: ", Scope::Method )

    Log_Info( "Well, just a sample" )
    DOX_MARKER([ALoxTut_Prefix])

    Log_Prune( ut.WriteResultFile( "ALoxTut_Prefix.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

    DOX_MARKER([ALoxTut_Prefix_2])
    Log_SetPrefix( ESC::BG_MAGENTA, Scope::Filename )
    DOX_MARKER([ALoxTut_Prefix_2])

    Log_Warning( "magenta" )

    Log_RemoveLogger( &memLog )
    Log_RemoveDebugLogger()
}

// #################################################################################################
// ALoxTut_ThreadName
// #################################################################################################
UT_METHOD(ALoxTut_ThreadName )
{
    UT_INIT()

    Log_Prune ( MemoryLogger memLog; )
    Log_SetVerbosity   ( &memLog, Verbosity::Verbose )
    Log_Prune ( memLog.MemoryLog.Reset();  memLog.CntLogs= 0; )


    DOX_MARKER([ALoxTut_MapThreadName])
    Log_MapThreadName( A_CHAR("BKGRND") )
    Log_Info ( "Hello ALox" )
    DOX_MARKER([ALoxTut_MapThreadName])
    Log_MapThreadName( A_CHAR("MAIN") )


    Log_RemoveLogger( & memLog)
    Log_Prune( memLog.MemoryLog.SearchAndReplace( A_CHAR("MONOMEM"), A_CHAR("CONSOLE") );            )
    Log_Prune( ut.WriteResultFile( "ALoxTut_ThreadName.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

}


// #################################################################################################
// ALoxTut_ConditionalLogging
// #################################################################################################

#include "alib/lang/callerinfo_methods.hpp"


// #################################################################################################
// ALoxTut_LogState
// #################################################################################################
UT_METHOD(ALoxTut_LogState)
{
    UT_INIT()

    DOX_MARKER([ALoxTut_LogState])
    // create two different loggers
    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )

    // reduce meta-information to limit tutorial output width
    Log_Prune( Log::DebugLogger->GetFormatMetaInfo().Format.Reset( A_CHAR("[%tN]%V[%D](%#): " ) );  )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_Prune( memLogger.GetFormatMetaInfo().Format.Reset(         A_CHAR("[%tN]%V[%D](%#): " ) );  )
    Log_Prune( memLogger.GetFormatMultiLine().Mode= 3; )

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
    Log_SetPrefix( ESC::RED,  "/ERRORS", lang::Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )

    // now, log the current config
    Log_LogState( nullptr, Verbosity::Info, A_CHAR("The current configuration of this Lox is:") )
    DOX_MARKER([ALoxTut_LogState])

    Log_Prune( ut.WriteResultFile( "ALoxTut_LogState.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )
    Log_RemoveDebugLogger()
    Log_RemoveLogger( &memLogger )
}


UT_METHOD(ALoxTut_LogInternalDomains)
{
    UT_INIT()

    {
    DOX_MARKER([ALoxTut_LogInternalDomains])
    // This is the very same code as above...
    Log_AddDebugLogger()
    Log_Prune( MemoryLogger memLogger;  )

    Log_Prune( Log::DebugLogger->GetFormatMetaInfo().Format.Reset("[%tN]%V[%D](%#): ");   )
    Log_SetVerbosity( &memLogger, Verbosity::Verbose )
    Log_Prune(         memLogger.GetFormatMetaInfo().Format.Reset("[%tN]%V[%D](%#): ");   )

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
    Log_SetPrefix( ESC::RED,  "/ERRORS", lang::Inclusion::Exclude )

    Log_MapThreadName( A_CHAR("TUTORIAL") )
    DOX_MARKER([ALoxTut_LogInternalDomains])
    Log_Prune( ut.WriteResultFile( "ALoxTut_LogInternalDomains.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    Log_RemoveDebugLogger()
    Log_RemoveLogger( "MEMORY" )
    }
}


#if ALOX_DBG_LOG
UT_METHOD(ALoxTut_LogData)
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

    DOX_MARKER([ALoxTut_LogData_2])
    Log_Retrieve( dbgFileVersion, "FILE_VERSION" )
    Log_Info( "Working on file version {!Q}", dbgFileVersion.Unbox<String>() )
    DOX_MARKER([ALoxTut_LogData_2])

    Log_Prune( ut.WriteResultFile( "ALoxTut_LogData.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    DOX_MARKER([ALoxTut_LogData_3])
    Log_Info( "Working on file version {!Q}", LOG_LOX.Retrieve("FILE_VERSION").Unbox<String>() )
    DOX_MARKER([ALoxTut_LogData_3])


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


    DOX_MARKER([ALoxTut_Format1])
    Log_Info( "Value=", 5 )
    DOX_MARKER([ALoxTut_Format1])
    Log_Prune( ut.WriteResultFile( "ALoxTut_Format1.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    DOX_MARKER([ALoxTut_FormatP])
    Log_Info( "Value={}", 5 )
    DOX_MARKER([ALoxTut_FormatP])

    DOX_MARKER([ALoxTut_FormatJ])
    Log_Info( "Value=%s", 5 )
    DOX_MARKER([ALoxTut_FormatJ])


    DOX_MARKER([ALoxTut_Format_Multi])
    Log_Info( "One-", "Two-", "Three" )
    Log_Info( "{}-{}-{}", "One", "Two", "Three" )
    Log_Info( "{}-{}-"  , "One", "Two", "Three" )
    Log_Info( "{}-"  , "One", "{}-", "Two", "{}", "Three" )
    DOX_MARKER([ALoxTut_Format_Multi])

    Log_Prune( memLogger.MemoryLog.Reset(); )
    DOX_MARKER([ALoxTut_Format_Mix])
    Log_Info( "Python Style: {!s}","PS", " - ", "Java Style: \"%s\"", "JS" )
    DOX_MARKER([ALoxTut_Format_Mix])
    Log_Prune( ut.WriteResultFile( "ALoxTut_Format_Mix.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    Log_Prune( memLogger.MemoryLog.Reset(); )
    DOX_MARKER([ALoxTut_Format_Sample_1])
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
    DOX_MARKER([ALoxTut_Format_Sample_1])
    Log_Prune( ut.WriteResultFile( "ALoxTut_Format_Sample_1.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    Log_Prune( memLogger.MemoryLog.Reset(); )
    DOX_MARKER([ALoxTut_Format_Sample_2])
    Log_Info   ( "Custom Date Format: {:yyyy * MM * dd}", DateTime() )
    DOX_MARKER([ALoxTut_Format_Sample_2])
    Log_Prune( ut.WriteResultFile( "ALoxTut_Format_Sample_2.txt", memLogger.MemoryLog, EMPTY_NSTRING ); )

    Log_RemoveDebugLogger()
    Log_RemoveLogger( "MEMORY" )
}


// #################################################################################################
// #################################################################################################
// ################################       ALox Manual Samples       ################################
// #################################################################################################
// #################################################################################################

// #################################################################################################
// ### Create manual sample output
// #################################################################################################

    UT_METHOD(ALox_Manual)
    {
        UT_INIT()
    Log_Prune( MemoryLogger memLog;  )
    Log_SetVerbosity( &memLog, Verbosity::Verbose )

        // 3
        Log_SetVerbosity( &memLog, Verbosity::Verbose )

        ut_alox_manual::MyMethod();

        Log_Prune( ut.WriteResultFile( "ALoxMan_Domains_3.txt", memLog.MemoryLog, EMPTY_NSTRING ); )



        // 5
        //Log_Reset();
        Log_SetVerbosity( &memLog, Verbosity::Verbose )

        // trick to pre-size domain field
        Log_Info("/MYDOM/MYDOM", "x" )
        memLog.MemoryLog.Reset();

        ut_alox_manual::ManualSampleCode5::MyMethod();

        Log_Prune( ut.WriteResultFile( "ALoxMan_Domains_5.txt", memLog.MemoryLog, EMPTY_NSTRING ); )



        // relativePaths
        //Log_Reset();
        Log_SetVerbosity( &memLog, Verbosity::Verbose )

        // trick to pre-size domain field
        Log_Info("/IO/PROCESS", "x" )
        memLog.MemoryLog.Reset();

        ut_alox_manual::ReadChangeAndWriteBack();

        Log_Prune( ut.WriteResultFile( "ALoxMan_Domains_RelativePaths.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

        // Man_DOMAINS___nested_scope
        //Log_Reset();
        Log_SetVerbosity( &memLog, Verbosity::Verbose )

        // trick to pre-size domain field
        memLog.GetAutoSizes().Main.Reset();
        Log_Info("/IO/PROCESS", "x" )
        memLog.MemoryLog.Reset();

        ut_alox_manual::IO io;
        io.ReadChangeAndWriteBack();
        Log_Prune( ut.WriteResultFile( "ALoxMan_DOMAINS___nested_scope.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

        memLog.GetAutoSizes().Main.Reset();
        memLog.MemoryLog.Reset();
        ut_alox_manual::ManualSampleCode5::PrefixLogablesLifecycle();
        Log_Prune( ut.WriteResultFile( "ALoxMan_PREFIXLOGABLES_Lifecycle.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

        memLog.GetAutoSizes().Main.Reset();
        memLog.MemoryLog.Reset();
        ut_alox_manual::LogSetPrefix();
        Log_Prune( ut.WriteResultFile( "ALoxMan_LogSetPrefix___1.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

        Log_RemoveLogger( &memLog )

    } // UT_METHOD(ALox_Manual)


    UT_METHOD(ALox_Manual2)
    {
        UT_INIT()
    Log_Prune( MemoryLogger memLog;  )
    Log_SetVerbosity( &memLog, Verbosity::Verbose )



DOX_MARKER([MAN_EXT_VERBOSITY_CONFIG_1])
// switching on with default priority
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Info, "/MYDOM" )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will not be logged" )

// switching on with higher priority
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Info, "/MYDOM", config::Priority::Standard +1 )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will still be logged. Domain was not switched off!" )
DOX_MARKER([MAN_EXT_VERBOSITY_CONFIG_1])

// Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, "$/" );

//! [MAN_EXT_VERBOSITY_CONFIG_2]
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, Lox::InternalDomains )
//! [MAN_EXT_VERBOSITY_CONFIG_2]

    Log_RemoveDebugLogger()
    Log_RemoveLogger(&memLog)

//---------- ALoxMan_ExtVerbosityConfig_1.txt ------------
        memLog.GetAutoSizes().Main.Reset();
        memLog.MemoryLog.Reset();

// switching on with default priority
Log_SetVerbosity( &memLog, Verbosity::Info, "/MYDOM" )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( &memLog, Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will not be logged" )

// switching on with higher priority
Log_SetVerbosity( &memLog, Verbosity::Info, "/MYDOM", config::Priority::Standard +1 )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( &memLog, Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will still be logged. Domain was not switched off!" )

    memLog.MemoryLog.SearchAndReplace(A_CHAR("\"MEMORY\":")  , A_CHAR("\"DEBUG_LOGGER\":") );
    Log_Prune( ut.WriteResultFile( "ALoxMan_ExtVerbosityConfig_1.txt", memLog.MemoryLog, EMPTY_NSTRING ); )

//---------- ALoxMan_ExtVerbosityConfig_2.txt ------------
        memLog.GetAutoSizes().Main.Reset();
        memLog.MemoryLog.Reset();
    Log_RemoveLogger(&memLog)
    Log_SetVerbosity( &memLog, Verbosity::Verbose )

    Log_SetVerbosity( &memLog, Verbosity::Verbose, Lox::InternalDomains )

// switching on with default priority
Log_SetVerbosity( &memLog, Verbosity::Info, "/MYDOM" )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( &memLog, Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will not be logged" )

// switching on with higher priority
Log_SetVerbosity( &memLog, Verbosity::Info, "/MYDOM", config::Priority::Standard +1 )
Log_Info( "MYDOM", "This line will be logged" )

// switching off with default priority
Log_SetVerbosity( &memLog, Verbosity::Off, "/MYDOM" )
Log_Info( "MYDOM", "This line will still be logged. Domain was not switched off!" )

        memLog.MemoryLog.SearchAndReplace( A_CHAR("\"MEMORY\""), A_CHAR("\"DEBUG_LOGGER\"") );
        Log_Prune( ut.WriteResultFile( "ALoxMan_ExtVerbosityConfig_2.txt", memLog.MemoryLog, EMPTY_NSTRING ); )


//! [MAN_EXT_VERBOSITY_CONFIG_3]
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, "/UI/MOUSE"        , config::Priority::DefaultValues + 1 )
Log_SetVerbosity( "DEBUG_LOGGER", Verbosity::Verbose, "/UI/DIALOGS/MOUSE", config::Priority::DefaultValues + 1 )
//! [MAN_EXT_VERBOSITY_CONFIG_3]

    Log_RemoveLogger( &memLog )

    }  // UT_METHOD(ALox_Manual2)
    
#endif // #if ALOX_DBG_LOG


#include "unittests/aworx_unittests_end.hpp"

} // namespace

#endif // ALIB_UT_ALOX

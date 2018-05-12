// #################################################################################################
//  ALox Samples
//
//  Copyright 2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################


#include "alib/alox.hpp"
#include "alib/alox/loggers/memorylogger.hpp"
#include "alib/alox/loggers/textfilelogger.hpp"
#include "alib/config/inifile.hpp"

#include <iostream>


using namespace std;
using namespace aworx;

// prototypes
extern String64 autoSizes;
void DebugLog();
void ReleaseLog();
void PerformanceTest();
void ALoxSampleReset();
void PerformanceTestRL();
void Formatting();
void LogColors();
void WCharTest();
void textFileLogger();
void SampleALibReport();
int main( int argc, char *argv[] );

// globals
String64 autoSizes;

void DebugLog()
{
    cout << "cout: Do some debug logging:" <<  endl;

    Log_Info("Hello ALox, this is debug logging" );


    #if !ALOX_DBG_LOG
        cout << "cout: should not have logged something (this is the release version)" <<  endl;
    #endif

    cout <<  endl;
}


void ReleaseLog()
{
    cout << "cout: Do some release logging:" <<  endl;

    Lox_Prune( Lox lox("ReleaseLox"); )
    #define LOX_LOX lox

    // let the system choose an appropriate console logger
    Lox_Prune( TextLogger* releaseLogger= Lox::CreateConsoleLogger(); )

    // In debug compilations, we still install a report writer.
    Log_Prune( Log::AddALibReportWriter( &LOX_LOX ); )
    Log_Prune( Lox_SetVerbosity( releaseLogger, Verbosity::Verbose, ALoxReportWriter::LogDomain() ); )
    Log_Prune( Lox_SetPrefix( "ALib Report: ", ALoxReportWriter::LogDomain() ); )

    // if makefile did not specify scope info for release logging (which is standard behavior),
    // we set a format string without scope information.
    #if !ALOX_REL_LOG_CI
        Lox_Prune( releaseLogger->MetaInfo->Format= "[%TC+%TL][%tN]%V[%D]%A1(%#): "; )
    #endif

    Lox_SetVerbosity( releaseLogger, Verbosity::Info );
    Lox_Info ( "Hello ALox, this is release logging" );

    ALIB_MESSAGE ( ASTR("And this is an ALib report message. Appears on release lox but only in debug compilation.") );


    // shutdown
    Log_Prune( Log::RemoveALibReportWriter() );  // in debug compilation only

    Lox_RemoveLogger( releaseLogger );
    Lox_Prune( delete releaseLogger; )

    #if !ALOX_REL_LOG
        cout << "cout: should not have logged something (release logging was disabled, obviously ALOX_REL_LOG_OFF was passed to the compiler)" <<  endl;
    #endif
    cout <<  endl;
}


// #################################################################################################
// performanceTest()
// #################################################################################################
void PerformanceTest()
{
    Log_AddDebugLogger();
    Lox_Prune( MemoryLogger ml( nullptr, true, false); )

    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->AutoSizes.Import( Substring(autoSizes), CurrentData::Keep );  )

                                    Log_SetVerbosity( Log::DebugLogger, Verbosity::Off    , "/MEM", Priorities::ProtectedValues   );
    Log_Prune( if (Log::IDELogger ) Log_SetVerbosity( Log::IDELogger  , Verbosity::Off    , "/MEM", Priorities::ProtectedValues   ) );
                                    Log_SetVerbosity( &ml,              Verbosity::Verbose, "/MEM", Priorities::ProtectedValues   );

    Log_Info( "Logging simple info lines" );


    int64_t fastest=       (std::numeric_limits<int64_t>::max)();

    #if !ALIB_DEBUG_STRINGS
        int qtyLines=   100;
        int qtyLoops=   100;
    #else
        int qtyLines=  100;
        int qtyLoops=   10;
    #endif

    if ( lib::ALIB.IsDebuggerPresent() )
        qtyLoops= 10;


    for ( int i= 0 ; i < qtyLoops ; i++ )
    {
        #if ALOX_REL_LOG
            ml.MemoryLog.Clear();
        #endif

        Ticks tt;
            for ( int l= 0 ; l < qtyLines ; l++ )
            {
                Log_Info ( "/MEM", "Test Line ", l  );
                Log_Prune( if( i== 0 && l == 0) Log_Info( "/CON", "Sample Output:\\n{}", ml.MemoryLog ); )
            }
        auto t= tt.Age();
        auto recent= t.InAbsoluteMicroseconds();

        if ( fastest > recent  )
        {
            fastest= recent;
            Log_Info( "/CON", "Pass {:03} is new fastest: {:4} micros per {} logs.", i, fastest,  qtyLines );
        }
    }

    Log_Prune( double microsPerLog=  static_cast<double>( fastest ) / qtyLines; )
    Log_Prune( int    logsPerSecond= static_cast<int>( 1000000.0 / microsPerLog);                    )

    Log_Info( "/CON", ESC::MAGENTA, "  Fastest debug logging: {} micros per log (resp. {:,} logs per second) ",
               microsPerLog, logsPerSecond );


    Log_RemoveLogger( &ml );
}

// #################################################################################################
// performanceTest()
// #################################################################################################
void PerformanceTestRL()
{
    Lox_Prune( Lox lox("ReleaseLox"); )
    #define LOX_LOX lox

    Lox_Prune( TextLogger*  releaseLogger= Lox::CreateConsoleLogger();  )
    Lox_Prune( MemoryLogger ml( nullptr, true, false);                  )

    // if makefile did not specify scope info for release logging (which is standard behavior),
    // we set a format string without scope information.
    #if !ALOX_REL_LOG_CI
        Lox_Prune( releaseLogger->MetaInfo->Format= ASTR("[%TC+%TL][%tN]%V[%D]%A1(%#): "); )
        Lox_Prune( ml.            MetaInfo->Format= ASTR("[%TC+%TL][%tN]%V[%D]%A1(%#): "); )
    #endif

//Lox_SetVerbosity( releaseLogger, Verbosity::Verbose,  ALox::InternalDomains, Config::PriorityOf(Priorities::ProtectedValues) );
    Lox_SetVerbosity( releaseLogger, Verbosity::Verbose,   "/CON", Priorities::ProtectedValues );
    Lox_SetVerbosity( releaseLogger, Verbosity::Off,       "/MEM", Priorities::ProtectedValues );
    Lox_SetVerbosity( &ml,           Verbosity::Verbose,   "/MEM", Priorities::ProtectedValues );

    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Lox_Prune(     releaseLogger->AutoSizes.Import( Substring(autoSizes) );                           )

    Lox_Info( "/CON", "Logging simple info lines (release logging)" );

    #if defined( _MSC_VER )
        int64_t fastest=       MAXINT;
    #else
        int64_t fastest=       std::numeric_limits<int>::max();
    #endif
    #if !ALIB_DEBUG_STRINGS
        int qtyLines=   100;
        int qtyLoops=   100;
    #else
        int qtyLines=  100;
        int qtyLoops=   10;
    #endif
    if ( lib::ALIB.IsDebuggerPresent() )
        qtyLoops= 10;

    for ( int i= 0 ; i < qtyLoops ; i++ )
    {
        #if ALOX_DBG_LOG || ALOX_REL_LOG
            ml.MemoryLog.Clear();
        #endif
        Ticks tt;
            for ( int l= 0 ; l < qtyLines ; l++ )
            {
                Lox_Info ( "/MEM", "Test Line ", l  );
                Lox_Prune( if( i== 0 && l == 0) Lox_Info( "/CON", "Sample Output:\\n{}", ml.MemoryLog ); )

            }
        auto t= tt.Age();
        auto recent= t.InAbsoluteMicroseconds();

        if ( fastest > recent  )
        {
            fastest= recent;
            Log_Info( "/CON", "Pass {:03} is new fastest: {:4} micros per {} logs.", i, fastest,  qtyLines );

        }
    }

    Log_Prune( double microsPerLog=  static_cast<double>( fastest ) / qtyLines; )
    Log_Prune( int    logsPerSecond= static_cast<int   >( 1000000.0 / microsPerLog );              )
    Log_Info( "/CON", ESC::MAGENTA, "  Fastest release logging: {} micros per log (resp. {:,} logs per second) ",
               microsPerLog, logsPerSecond );


    Lox_RemoveLogger( &ml );
    Lox_RemoveLogger( releaseLogger );
    Lox_Prune( delete releaseLogger; )
}

void Formatting()
{
    cout << "cout: Use the predefined formatters:" <<  endl;

    Log_Info("Python-Style Format: {}, {:08.3}, *{:^12}*", "Hello", 12.3456789, "Centered" );
    Log_Info("Java-Style Format:   %s, %08.3f, *%^12s*"  , "Hello", 12.3456789, "Centered" );

    Log_Info("We can mix styles: {}-Style", "Python", " and %s-Style in one log statement", "Java" );
    Log_Info("But mixing must not be done within one format string: {} %s", "Right", "Wrong" );


    cout <<  endl;
}

void LogColors()
{
    Log_AddDebugLogger();
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->AutoSizes.Import( Substring(autoSizes), CurrentData::Keep );  )

    cout << "cout: Colorful logging:" <<  endl;


    Log_Info(    "Let us do some color test. First, lets see if different verbosities have different colors:" );
    Log_Warning( "Is this line colored (verbosity warning)?" );
    Log_Error(   "Is this line colored differently (verbosity error)?" );
    Log_Verbose( "Is this line colored differently (verbosity verbose)?" );
    Log_Info(    "Color table:");
    Log_Info( String256() << "FG Colors:  "
                          << ">>>" << ESC::RED     << "RED"        << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::GREEN   << "GREEN"      << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::BLUE    << "BLUE"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::CYAN    << "CYAN"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::MAGENTA << "MAGENTA"    << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::YELLOW  << "YELLOW"     << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::GRAY    << "GRAY"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::WHITE   << "WHITE"      << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::BLACK   << "BLACK"      << ESC::FG_RESET << "<<<"   );

    Log_Info( String256() << "BG Colors:  "
                          << ">>>" << ESC::BG_RED     << "RED"        << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_GREEN   << "GREEN"      << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_BLUE    << "BLUE"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_CYAN    << "CYAN"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_MAGENTA << "MAGENTA"    << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_YELLOW  << "YELLOW"     << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_GRAY    << "GRAY"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_WHITE   << "WHITE"      << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_BLACK   << "BLACK"      << ESC::BG_RESET << "<<<"  );

    Log_Info( String256() << "FG/BG same: "
                          << ">>>" << ESC::RED     << ESC::BG_RED     << "RED"        << ESC::RESET << "<<<"
                          << ">>>" << ESC::GREEN   << ESC::BG_GREEN   << "GREEN"      << ESC::RESET << "<<<"
                          << ">>>" << ESC::BLUE    << ESC::BG_BLUE    << "BLUE"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::CYAN    << ESC::BG_CYAN    << "CYAN"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::MAGENTA << ESC::BG_MAGENTA << "MAGENTA"    << ESC::RESET << "<<<"
                          << ">>>" << ESC::YELLOW  << ESC::BG_YELLOW  << "YELLOW"     << ESC::RESET << "<<<"
                          << ">>>" << ESC::GRAY    << ESC::BG_GRAY    << "GRAY"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::WHITE   << ESC::BG_WHITE   << "WHITE"      << ESC::RESET << "<<<"
                          << ">>>" << ESC::BLACK   << ESC::BG_BLACK   << "BLACK"      << ESC::RESET << "<<<" );

    Log_Info( String256() << "Styles: "
                          << ">>>" << ESC::BOLD     << "Bold"          << ESC::STYLE_RESET << "<<<"
                          << ">>>" << ESC::ITALICS  << "Italics"       << ESC::STYLE_RESET << "<<<"
                          << ">>>" << ESC::BOLD
                          << ">>>" << ESC::ITALICS  << "Bold/Italics"  << ESC::STYLE_RESET << "<<<  Styles do not work in Windows consoles" );


    #if !ALOX_DBG_LOG
        cout << "cout: should not have logged something (this is the release version)" <<  endl;
    #endif

    cout <<  endl;
}

void WCharTest()
{
    Log_AddDebugLogger();
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->AutoSizes.Import( Substring(autoSizes), CurrentData::Keep );  )

    Log_SetDomain( "WCHAR", Scope::Method );

    String256 ms;
    ms.Clear() << "ASCII String as wide: " <<  L"AString";                          Log_Info( ms );



    ms.Clear() << "Euro sign:            " <<  L"\u20AC";                           Log_Info( ms );


    ms.Clear() << "Greek characters:     " <<  L"\u03B1\u03B2\u03B3\u03B4\u03B5";   Log_Info( ms );


    // from https://msdn.microsoft.com/en-us/library/69ze775t.aspx
    ms.Clear() << "Smileys:              " <<  L"😉 = \U0001F609 is ;-)";           Log_Info( ms );
    ms.Clear() << "                      " <<  L"😇 = \U0001F607 is O:-)";          Log_Info( ms );
    ms.Clear() << "                      " <<  L"😃 = \U0001F603 is :-D";           Log_Info( ms );
    ms.Clear() << "                      " <<  L"😎 = \U0001F60E is B-)";           Log_Info( ms );
    ms.Clear() << "                      " <<  L"( ͡° ͜ʖ ͡°) = ( \U00000361\U000000b0 \U0000035c\U00000296 \U00000361\U000000b0)";            Log_Info( ms );


    Log_Info( String64() << "sizeof wchar: "  << sizeof(wchar_t) );
    Log_Info( String64() << "Max wchar:    "  << WCHAR_MAX);
    Log_Info( String64() << "Min wchar:    "  << WCHAR_MIN);
}

void textFileLogger()
{
    Log_AddDebugLogger();
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->AutoSizes.Import( Substring(autoSizes), CurrentData::Keep );  )

    Log_Info( "Creating a text file logger with file 'Test.log.txt'" );

    Log_SetDomain( "TEXTFILE_TEST", Scope::Method    );

    Log_Prune( TextFileLogger tfl( ASTR("Test.log.txt") ) );
    Log_SetVerbosity( &tfl, Verbosity::Verbose );
    Log_SetVerbosity( &tfl, Verbosity::Error, ALox::InternalDomains );

    Log_Verbose( "A verbose message (goes to textfile logger as well)" );
    Log_Info   ( "An info message  (goes to textfile logger as well)" );
    Log_Warning( "A warning message  (goes to textfile logger as well)" );
    Log_Error  ( "An error message (goes to textfile logger as well)" );
    Log_Info   ( "Multi-line part 1...\n....part 2" );

    Log_RemoveLogger( &tfl )
}

void SampleALibReport()
{
    Log_AddDebugLogger();
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->AutoSizes.Import( Substring(autoSizes), CurrentData::Keep );  )

    Log_Info( "Sample: ALib Report Writer\n"
              "Method \"Log::AddDebugLogger()\" by default creates a replacement for the\n"
              "standard ALib report writer. If this is a debug compilation, let's have a try and\n"
              "create an 3 Messages:"  );

    // must be done only in debug compiles
    #if ALIB_DEBUG

    lib::lang::Report::GetDefault().PushHaltFlags( false, false );

        ALIB_ERROR(   "This is an error report!" );
        ALIB_WARNING( "And this is a warning!"   );
        AString test("Four");
        test.SetLength<false>(10);

    lib::lang::Report::GetDefault().PopHaltFlags();

    #endif

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, ALox::InternalDomains );
    ALIB_MESSAGE( ASTR("This is an ALib Report. Types other than '0' and '1' are user defined.\n"
                       "Verbosity of ALox::InternalDomains has to be increased to see them when using"
                       " ALoxReportWriter." ) );

    Log_Info( String256() <<   "Note the domain prefix '" << ALox::InternalDomains << "'. This addresses "
             << "the tree of internal domains\nof the Lox, which the report writer is just "
             << "using." );
}


void ALoxSampleReset()
{
    #if ALOX_DBG_LOG
        if ( Log::DebugLogger != nullptr )
        {
            Log::DebugLogger->AutoSizes.Export( autoSizes.Clear() );
            Log_RemoveDebugLogger();
        }
    #endif

    Log_Prune( LOG_LOX.Reset(); )
    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Include );
}


int main( int argc, char *argv[] )
{
    #if defined(__clang__)
        #pragma message "Info: Clang Compiler (not a warning, just for an information)"
    #elif defined(__GNUC__)
        #pragma message "Info: GNU Compiler (not a warning, just for an information)"
    #elif defined(_MSC_VER)
        #pragma message ("Info: MS Compiler (not a warning, just for an information)")
    #endif

    // Partly initialize ALib/ALox, to have configuration and default resources in place
    lib::ALIB.Init( Library::InitLevels::PrepareConfig );


    // first attach INI file to config system...
    IniFile iniFile;
    if ( iniFile.FileComments.IsEmpty() )
    {
        iniFile.FileComments._(
        "##################################################################################################\n"
        "# ALox Samples INI file (created when running ALox Samples)\n"
        "#\n"
        "# Copyright 2013-2018 A-Worx GmbH, Germany\n"
        "# Published under 'Boost Software License' (a free software license, see LICENSE.txt)\n"
        "##################################################################################################\n"
        );
    }

    lib::ALIB.Config->InsertPlugin( &iniFile, Priorities::Standard );

    //... and then initialize ALib completely
    lib::ALIB.Init( argc, argv );

    Log_SetSourcePathTrimRule( "*/src/", Inclusion::Include );

    // Suppress setting "writeback" for verbosities. We need to do this as this main()
    // method invokes a list of independent samples. Those would now read from the INI file wrong
    // values written in other sample methods and thus the samples would not work any more
    // (because INI file settings overrules settings in the code)
    Variable var;
    lib::ALOX.Config->Store( var.Declare( ASTR("ALOX"), ASTR("LOG_DEBUG_LOGGER_VERBOSITY")  ),  ASTR("") );
    lib::ALOX.Config->Store( var.Declare( ASTR("ALOX"), ASTR("RELEASELOX_CONSOLE_VERBOSITY")),  ASTR("") );
    lib::ALOX.Config->Store( var.Declare( ASTR("ALOX"), ASTR("LOG_MEMORY_VERBOSITY")        ),  ASTR("") );
    lib::ALOX.Config->Store( var.Declare( ASTR("ALOX"), ASTR("RELEASELOX_MEMORY_VERBOSITY") ),  ASTR("") );
    lib::ALOX.Config->Store( var.Declare( ASTR("ALOX"), ASTR("LOG_TEXTFILE_VERBOSITY")      ),  ASTR("") );

    DebugLog();                 ALoxSampleReset();
    ReleaseLog();               ALoxSampleReset();
    PerformanceTest();          ALoxSampleReset();
    PerformanceTestRL();        ALoxSampleReset();
    Formatting();               ALoxSampleReset();
    LogColors();                ALoxSampleReset();
    SampleALibReport();         ALoxSampleReset();
    WCharTest();                ALoxSampleReset();
    textFileLogger();           ALoxSampleReset();

    // cleanup resources to make Valgrind happy
    lib::ALIB.Config->RemovePlugin( &iniFile );
    lib::ALIB.Config->FetchFromDefault( iniFile );
    iniFile.WriteFile();

    lib::ALIB.TerminationCleanUp();
    cout << "ALox Samples finished" << endl;
    return 0;
}




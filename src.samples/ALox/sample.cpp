// #################################################################################################
//  ALox Samples
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "ALib.ALox.H"
#include "ALib.ALox.Impl.H"
#include "ALib.Camp.Base.H"
#include "ALib.Variables.IniFile.H"
#include "ALib.Compatibility.StdStrings.H"
#include "ALib.Bootstrap.H"

#include <iostream>
#include <filesystem>


using namespace std;
using namespace alib;

// local prototypes
extern String128 autoSizes;
void DebugLog();
void ReleaseLog();
void PerformanceTest();
void ALoxSampleReset();
void PerformanceTestRL();
void Formatting();
void LogColors();
void WCharTest();
void textFileLogger();
void SampleALibAssertion();
int main( int argc, const char *argv[] );

// globals
String128 autoSizes;

#include "ALib.Lang.CIFunctions.H"

void DebugLog()
{
    cout << "cout: Do some debug logging:" <<  endl;

    Log_Info("Hello ALox, this is debug logging" )


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

    // With debug-builds, we still install the assertion plug-in.
    Log_Prune( Log::SetALibAssertionPlugin( &LOX_LOX ); )
    Log_Prune( Lox_SetVerbosity( releaseLogger, Verbosity::Verbose, lox::ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX ); )
    Log_Prune( Lox_SetPrefix( "alib::assert::Assert(): ", lox::ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX ); )

    // if makefile did not specify scope info for release logging (which is standard behavior),
    // we set a format string without scope information.
    Lox_SetVerbosity( releaseLogger, Verbosity::Info )
    #if !ALOX_REL_LOG_CI
        Lox_Prune( releaseLogger->GetFormatMetaInfo().Format.Reset( "[%TC+%TL][%tN]%V[%D]%A1(%#): " ); )
    #endif

    Lox_Info ( "Hello ALox, this is release logging" )

    ALIB_MESSAGE ( "SAMPLE", "And this is an ALib assertion/warning message. "
                   "Appears on release lox but only with debug-builds." )


    // shutdown
    Log_Prune( Log::SetALibAssertionPlugin(nullptr) );  // with debug-builds only

    Lox_RemoveLogger( releaseLogger )
    Lox_Prune( delete releaseLogger; )

    #if !ALOX_REL_LOG
        cout << "cout: should not have logged something (release logging was disabled, obviously "
                "ALOX_REL_LOG=0 was passed to the compiler)" <<  endl;
    #endif
    cout <<  endl;
}


// #################################################################################################
// performanceTest()
// #################################################################################################
void PerformanceTest()
{
    Log_AddDebugLogger()
    Lox_Prune( MemoryLogger ml( nullptr, true, false); )

    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->GetAutoSizes().Main.Import( Substring(autoSizes), lang::CurrentData::Keep );  )

                                    Log_SetVerbosity( Log::DebugLogger, Verbosity::Off    , "/MEM", Priority::Protected   )
    Log_Prune( if (Log::IDELogger ) Log_SetVerbosity( Log::IDELogger  , Verbosity::Off    , "/MEM", Priority::Protected   ) )
                                    Log_SetVerbosity( &ml,              Verbosity::Verbose, "/MEM", Priority::Protected   )

    Log_Info( "Logging simple info lines" )


    int64_t fastest=       (std::numeric_limits<int64_t>::max)();

    #if !ALIB_DEBUG_STRINGS
        int qtyLines=   100;
        int qtyLoops=   100;
    #else
        int qtyLines=  100;
        int qtyLoops=   10;
    #endif

    if ( BASECAMP.IsDebuggerPresent() )
        qtyLoops= 10;


    for ( int i= 0 ; i < qtyLoops ; ++i )
    {
        #if ALOX_REL_LOG
            ml.MemoryLog.Reset();
        #endif

        Ticks tt;
            for ( int l= 0 ; l < qtyLines ; ++l )
            {
                Log_Info ( "/MEM", "Test Line ", l  )
                Log_Prune( if( i== 0 && l == 0) Log_Info( "/CON", "Sample Output:\n{}", ml.MemoryLog ); )
            }
        auto t= tt.Age();
        auto recent= t.InAbsoluteMicroseconds();

        if ( fastest > recent  )
        {
            fastest= recent;
            Log_Info( "/CON", "Pass {:03} is new fastest: {:4} micros per {} logs.", i, fastest,  qtyLines )
        }
    }

    Log_Prune( double microsPerLog=  double( fastest ) / qtyLines;   )
    Log_Prune( int    logsPerSecond= int( 1000000.0 / microsPerLog); )

    Log_Info( "/CON", ESC::MAGENTA, "  Fastest debug logging: {} micros per log (resp. {:,} logs per second) ",
               microsPerLog, logsPerSecond )

    Log_RemoveLogger( &ml )
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

    //Lox_SetVerbosity( releaseLogger, Verbosity::Verbose,  Lox::InternalDomains, Priority::Protected )
    Lox_SetVerbosity( releaseLogger, Verbosity::Off,       "/"   , Priority::Protected )
    Lox_SetVerbosity( releaseLogger, Verbosity::Verbose,   "/CON", Priority::Protected )
    Lox_SetVerbosity( &ml,           Verbosity::Verbose,   "/"   , Priority::Protected )

    // if makefile did not specify scope info for release logging (which is standard behavior),
    // we set a format string without scope information.
    #if !ALOX_REL_LOG_CI
        Lox_Prune( releaseLogger->GetFormatMetaInfo().Format.Reset( A_CHAR("[%TC+%TL][%tN]%V[%D]%A1(%#): ")); )
        Lox_Prune( ml.            GetFormatMetaInfo().Format.Reset( A_CHAR("[%TC+%TL][%tN]%V[%D]%A1(%#): ")); )
    #endif


    Lox_Info( "/CON", "Logging simple info lines (release logging)" )

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
    if ( BASECAMP.IsDebuggerPresent() )
        qtyLoops= 10;

    for ( int i= 0 ; i < qtyLoops ; ++i )
    {
        #if ALOX_DBG_LOG || ALOX_REL_LOG
            ml.MemoryLog.Reset();
        #endif
        Ticks tt;
            for ( int l= 0 ; l < qtyLines ; ++l )
            {
                Lox_Info ( "Test Line ", l  )
                Lox_Prune( if( i== 0 && l == 0) Lox_Info( "/CON", "Sample Output:\n{}", ml.MemoryLog ); )
            }
        auto t= tt.Age();
        auto recent= t.InAbsoluteMicroseconds();

        if ( fastest > recent  )
        {
            fastest= recent;
            Lox_Info( "/CON", "Pass {:03} is new fastest: {:4} micros per {} logs.", i, fastest,  qtyLines )

        }
    }

    Lox_Prune( double microsPerLog=  double( fastest ) / qtyLines; )
    Lox_Prune( int    logsPerSecond= int   ( 1000000.0 / microsPerLog );              )
    Lox_Info( "/CON", ESC::MAGENTA, "  Fastest release logging: {} micros per log (resp. {:,} logs per second) ",
               microsPerLog, logsPerSecond )


    Lox_RemoveLogger( &ml )
    Lox_RemoveLogger( releaseLogger )
    Lox_Prune( delete releaseLogger; )
}

void Formatting()
{
    cout << "cout: Use the predefined formatters:" <<  endl;

    Log_Info("Python-Style Format: {}, {:08.3}, *{:^12}*", "Hello", 12.3456789, "Centered" )
    Log_Info("Java-Style Format:   %s, %08.3f, *%^12s*"  , "Hello", 12.3456789, "Centered" )

    Log_Info("We can mix styles: {}-Style", "Python", " and %s-Style in one log statement", "Java" )
    Log_Info("But mixing must not be done within one format string: {} %s", "Right", "Wrong" )


    cout <<  endl;
}

void LogColors()
{
    Log_AddDebugLogger()
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->GetAutoSizes().Main.Import( Substring(autoSizes), lang::CurrentData::Keep );  )

    cout << "cout: Colorful logging:" <<  endl;


    Log_Info(    "Let us do some color test. First, lets see if different verbosities have different colors:" )
    Log_Warning( "Is this line colored (verbosity warning)?" )
    Log_Error(   "Is this line colored differently (verbosity error)?" )
    Log_Verbose( "Is this line colored differently (verbosity verbose)?" )
    Log_Info(    "Color table:")
    Log_Info( String256() << "FG Colors:  "
                          << ">>>" << ESC::RED     << "RED"        << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::GREEN   << "GREEN"      << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::BLUE    << "BLUE"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::CYAN    << "CYAN"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::MAGENTA << "MAGENTA"    << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::YELLOW  << "YELLOW"     << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::GRAY    << "GRAY"       << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::WHITE   << "WHITE"      << ESC::FG_RESET << "<<<"
                          << ">>>" << ESC::BLACK   << "BLACK"      << ESC::FG_RESET << "<<<"   )

    Log_Info( String256() << "BG Colors:  "
                          << ">>>" << ESC::BG_RED     << "RED"        << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_GREEN   << "GREEN"      << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_BLUE    << "BLUE"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_CYAN    << "CYAN"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_MAGENTA << "MAGENTA"    << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_YELLOW  << "YELLOW"     << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_GRAY    << "GRAY"       << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_WHITE   << "WHITE"      << ESC::BG_RESET << "<<<"
                          << ">>>" << ESC::BG_BLACK   << "BLACK"      << ESC::BG_RESET << "<<<"  )

    Log_Info( String256() << "FG/BG same: "
                          << ">>>" << ESC::RED     << ESC::BG_RED     << "RED"        << ESC::RESET << "<<<"
                          << ">>>" << ESC::GREEN   << ESC::BG_GREEN   << "GREEN"      << ESC::RESET << "<<<"
                          << ">>>" << ESC::BLUE    << ESC::BG_BLUE    << "BLUE"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::CYAN    << ESC::BG_CYAN    << "CYAN"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::MAGENTA << ESC::BG_MAGENTA << "MAGENTA"    << ESC::RESET << "<<<"
                          << ">>>" << ESC::YELLOW  << ESC::BG_YELLOW  << "YELLOW"     << ESC::RESET << "<<<"
                          << ">>>" << ESC::GRAY    << ESC::BG_GRAY    << "GRAY"       << ESC::RESET << "<<<"
                          << ">>>" << ESC::WHITE   << ESC::BG_WHITE   << "WHITE"      << ESC::RESET << "<<<"
                          << ">>>" << ESC::BLACK   << ESC::BG_BLACK   << "BLACK"      << ESC::RESET << "<<<" )

    Log_Info( String256() << "Styles: "
                          << ">>>" << ESC::BOLD     << "Bold"          << ESC::STYLE_RESET << "<<<"
                          << ">>>" << ESC::ITALICS  << "Italics"       << ESC::STYLE_RESET << "<<<"
                          << ">>>" << ESC::BOLD
                          << ">>>" << ESC::ITALICS  << "Bold/Italics"  << ESC::STYLE_RESET << "<<<  Styles do not work in Windows consoles" )


    #if !ALOX_DBG_LOG
        cout << "cout: should not have logged something (this is the release version)" <<  endl;
    #endif

    cout <<  endl;
}

void WCharTest()
{
    Log_AddDebugLogger()
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->GetAutoSizes().Main.Import( Substring(autoSizes), lang::CurrentData::Keep );  )

    Log_SetDomain( "WCHAR", Scope::Method )

    Log_Info( "ASCII String as wide: ",  L"AString")
    Log_Info( "Euro sign:            ",  L"\u20AC" )
    Log_Info( "Greek characters:     ",  L"\u03B1\u03B2\u03B3\u03B4\u03B5" )


    // from https://msdn.microsoft.com/en-us/library/69ze775t.aspx
    Log_Info( "Smileys:              ", L"😉 = \U0001F609 is ;-)"   )
    Log_Info( "                      ", L"😇 = \U0001F607 is O:-)"  )
    Log_Info( "                      ", L"😃 = \U0001F603 is :-D"   )
    Log_Info( "                      ", L"😎 = \U0001F60E is B-)"   )
    Log_Info( "                      ", L"( ͡° ͜ʖ ͡°) = ( \U00000361\U000000b0 \U0000035c\U00000296\U00000361\U000000b0)" )


    Log_Info( "sizeof wchar: ",  sizeof(wchar_t) )
    Log_Info( "Max wchar:    ",  WCHAR_MAX       )
    Log_Info( "Min wchar:    ",  WCHAR_MIN       )
}

void textFileLogger()
{
    Log_AddDebugLogger()
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->GetAutoSizes().Main.Import( Substring(autoSizes), lang::CurrentData::Keep );  )

    Log_Info( "Creating a text file logger with file 'Test.log.txt'" )

    Log_SetDomain( "TEXTFILE_TEST", Scope::Method    )

    Log_Prune( TextFileLogger tfl( A_CHAR("Test.log.txt") ) );
    Log_SetVerbosity( &tfl, Verbosity::Verbose )
    Log_SetVerbosity( &tfl, Verbosity::Error, Lox::InternalDomains )

    Log_Verbose( "A verbose message (goes to textfile logger as well)"  )
    Log_Info   ( "An info message (goes to textfile logger as well)"    )
    Log_Warning( "A warning message (goes to textfile logger as well)"  )
    Log_Error  ( "An error message (goes to textfile logger as well)"   )
    Log_Info   ( "Multi-line part 1...\n....part 2"                     )

    Log_RemoveLogger( &tfl )
}

void SampleALibAssertion()
{
    Log_AddDebugLogger()
    // to align all samples nicely, we are manually adding the autosizes from the config.
    // This is not needed for standard applications that create one debug logger at the start and
    // use this till the end
    Log_Prune( Log::DebugLogger->GetAutoSizes().Main.Import( Substring(autoSizes), lang::CurrentData::Keep );  )

    Log_Info( "Sample: ALib Assert Plug-in\n"
              "Method \"Log::AddDebugLogger()\" by default sets the plugin alib::assert::PLUGIN.\n"
              "If this is a debug build, let's have a try and create an 3 Messages:"  )

    // must be done only in debug compiles
    #if ALIB_DEBUG
    assert::GetHaltFlagAndCounters().HaltOnErrors=false;
        ALIB_ERROR(   "SAMPLE", "This is an error!" )
        ALIB_WARNING( "SAMPLE", "And this is a warning. A next one should follow:"   )
        AString test("12345");
        test.GrowBufferAtLeastBy(1);
    assert::GetHaltFlagAndCounters().HaltOnErrors=true;
    #endif

    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose, alib::lox::ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX )
    ALIB_MESSAGE( "SAMPLE",
      "This is an ALib Message. Types other than 'message', 'warning' and 'error' are user-defined.\n"
      "Verbosity of the domain given with alib::lox::ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX has to be increased\n"
      "to see them when raising an ALib assertion."  )
}


void ALoxSampleReset()
{
    #if ALOX_DBG_LOG
        if ( Log::DebugLogger != nullptr )
        {
            Log::DebugLogger->GetAutoSizes().Main.Export( autoSizes.Reset() );
            Log_RemoveDebugLogger()
        }
    #endif

    Log_Prune( LOG_LOX.Reset(); )
    Log_SetSourcePathTrimRule( "*/src/", lang::Inclusion::Include )
}

constexpr String INIFileName = ALIB_BASE_DIR   A_CHAR("/docs/pages/generated/ALoxSample.ini");

int main( int argc, const char** argv)
{
    alib::ARG_C  = argc;
    alib::ARG_VN = argv;

    // we have to clear the ini-file before running the sample
    // (otherwise different platform tests writewrite wrong defaults to each other)
    filesystem::remove(filesystem::path(ALIB_BASE_DIR "/docs/pages/generated/ALoxSample.ini"));

DOX_MARKER([DOXYGEN_CREATE_INIFILE])
// Partly initialize ALib/ALox, to have configuration and default resource pool in place
// (This also invokes Configuration::PreloadVariables() for Basecamp and ALox variables.)
alib::Bootstrap(BootstrapPhases::PrepareConfig );

// Open an INI file (if open fails, nothing is imported)
{
    // import variables
    IniFileFeeder iniFileFeeder(*BASECAMP.GetConfig());
    iniFileFeeder.ImportStart( INIFileName );
    iniFileFeeder.ImportAll();
    iniFileFeeder.ImportEnd();
}

//... and then bootstrap ALib completely
Bootstrap();
DOX_MARKER([DOXYGEN_CREATE_INIFILE])

    Log_SetSourcePathTrimRule( "*/src/", lang::Inclusion::Include )


    DebugLog();                 ALoxSampleReset();
    ReleaseLog();               ALoxSampleReset();
    PerformanceTest();          ALoxSampleReset();
    PerformanceTestRL();        ALoxSampleReset();
    Formatting();               ALoxSampleReset();
    LogColors();                ALoxSampleReset();
    SampleALibAssertion();      ALoxSampleReset();
    WCharTest();                ALoxSampleReset();
    textFileLogger();           ALoxSampleReset();

    // cleanup resources to make Valgrind happy
DOX_MARKER([DOXYGEN_REMOVE_INIFILE])
// announce the shutdown (first shutdown phase) and remove the ini-file
alib::Shutdown( alib::ShutdownPhases::Announce );

{
    // Open INI-file (if open fails, we do not care)
    IniFileFeeder iniFileFeeder(*BASECAMP.GetConfig());
    iniFileFeeder.ExportStart( INIFileName );

    // export variables that are not existing in the INI-file yet
    int cntChanges= 0;
    cntChanges+= iniFileFeeder.ExportSubTree(A_CHAR("ALIB"));
    cntChanges+= iniFileFeeder.ExportSubTree(A_CHAR("ALOX"));
    cntChanges+= iniFileFeeder.ExportSubTree(A_CHAR("/"));

    // add section comments (if not existing)
    cntChanges+= iniFileFeeder.AddResourcedSectionComments(*BASECAMP.GetResourcePool(), BASECAMP.ResourceCategory, "INI_CMT_" );
    cntChanges+= iniFileFeeder.AddResourcedSectionComments(*ALOX    .GetResourcePool(), ALOX    .ResourceCategory, "INI_CMT_" );

    // add file comments
    auto& iniFile= iniFileFeeder.GetIniFile();
    if ( iniFile.FileComments.IsEmpty() )
    {
        iniFileFeeder.GetIniFile().FileComments.Allocate( iniFileFeeder.GetIniFile().Allocator,
        A_CHAR(
        "######################################################################################\n"
        "# ALox Samples INI-file (created when running ALox Samples)\n"
        "#\n"
        "# Copyright 2013-2025 A-Worx GmbH, Germany\n"
        "# Published under \"Boost Software License\" (a free software license, see LICENSE.txt)\n"
        "######################################################################################\n"
         ) );

        cntChanges++;
    }

    // write INI-file, if changed.
    if( cntChanges > 0 )
        iniFileFeeder.ExportEnd( INIFileName );
    else
        iniFileFeeder.ExportEnd();
}

// finalize ALib termination
alib::Shutdown();
DOX_MARKER([DOXYGEN_REMOVE_INIFILE])
    cout << "ALox Samples finished" << endl;
    return 0;
}

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


#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/alox.hpp"
#include "alib/alox/loggers/consolelogger.hpp"
#include "alib/alox/loggers/ansilogger.hpp"
#include "alib/alox/loggers/windowsconsolelogger.hpp"

#if ALOX_DBG_LOG

#include "alib/alox/loggers/memorylogger.hpp"

#if !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

#include <iostream>
#include <fstream>

// Fix the method name of logging (needed for unity builds)
#undef  ALIB_CALLER
#if defined( __GNUC__ )
#   define ALIB_CALLER    __FILE__, __LINE__, __func__
#else
#   define ALIB_CALLER    __FILE__, __LINE__, __FUNCTION__
#endif

using namespace std;
using namespace alib;

namespace ut_reclog
{
    class AppendLog
    {
        public:
         alib::String Text;
         int           Mode; // 0, no logging, 1 simple, 2 with formatter
         AppendLog(const alib::String& text, int mode)
         : Text(text)
         , Mode(mode)
         {}
    };

}

namespace alib {  namespace strings {

    template<> struct T_Append<ut_reclog::AppendLog,character>
    {
        void operator()( AString& target, const ut_reclog::AppendLog& src )
        {
            if( src.Mode == 1 )
            {
                String128 output("Logging object >");
                output << src.Text << "<";
                Log_Info(output)
            }
            else if( src.Mode == 2 )
            {
                Log_Info("{}{}{}", "Logging object >", src.Text, "<" )
            }

            target._<false>( src.Text );
        }
    };

    template<> struct T_Append<ut_reclog::AppendLog*,character>
    {
        void operator()( AString& target, const ut_reclog::AppendLog* src )
        {
            if( src->Mode == 1 )
            {
                String128 output("Logging object >");
                output << src->Text << "<";
                Log_Info(output)
            }
            else if( src->Mode == 2 )
            {
                Log_Info("{}{}{}", "Logging object >", src->Text, "<" )
            }

            target._<false>( src->Text );
        }
    };


}} // namespace [alib::lox::strings]


#define TESTCLASSNAME       CPP_ALox_Logger
#include "unittests/aworx_unittests.hpp"

using namespace ut_aworx;

namespace ut_alox {



/** ********************************************************************************************
* UT_CLASS
**********************************************************************************************/
UT_CLASS


/** ********************************************************************************************
 * Log_Replacements
 **********************************************************************************************/
UT_METHOD(Log_Replacements)
{
    UT_INIT()

    Log_AddDebugLogger()
    Log_SetDomain( "REPL", Scope::Method )
    Log_Prune( MemoryLogger* testML= new MemoryLogger(); )
    Log_SetVerbosity( testML , Verbosity::Verbose)

    Log_Prune( testML->SetReplacement( A_CHAR("F..."), A_CHAR("F-Word")   ); )

    Log_Info( "What the F...!" )

    UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("F..."))   < 0 )
    UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("F-Word")) > 0 )

    Log_RemoveLogger( testML )
    Log_Prune( delete testML; )
}


/** ********************************************************************************************
 * Log_Exception.
 **********************************************************************************************/
UT_METHOD(Log_Multiline)
{
    UT_INIT()

    Log_AddDebugLogger()

    Log_SetDomain( "/MLINE", Scope::Method )
    Log_SetVerbosity( Log::DebugLogger, Verbosity::Verbose )

//        Log::DebugLogger->MetaInfo->MsgPrefixInfo=   "$$$";
//        Log::DebugLogger->MetaInfo->MsgPrefixInfo=   ESC::MAGENTA;
//        Log::DebugLogger->MetaInfo->MsgSuffix=       "###";
//        Log::DebugLogger->MetaInfo->MsgSuffix=       ESC::FG_RESET;
//    Log::DebugLogger->FmtMultiLineSuffix=            "<<";



    Log::DebugLogger->MultiLineMsgMode= 0;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->MultiLineMsgMode= 0;
    Log::DebugLogger->MultiLineDelimiterRepl= "~|~";
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) with delimiter replacement set to ~|~ --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->MultiLineMsgMode= 0;
    Log::DebugLogger->MultiLineDelimiter.Reset();
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) with delimiter set to \"\" (stops multi line processing) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )
    Log::DebugLogger->MultiLineDelimiter.SetNull();

    Log::DebugLogger->MultiLineMsgMode= 1;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 1 (multi line, all meta info per line) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->MultiLineMsgMode= 2;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 2 (multi line, meta info blanked) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->MultiLineMsgMode= 3;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 3 (multi line, print headline with info, text starts at pos 0) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->MultiLineMsgMode= 4;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 4 (pure multi line, no meta info, no headline, starts at pos 0)) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )
}


/** ********************************************************************************************
* Log_ColorsAndStyles.
**********************************************************************************************/

#if ALOX_DBG_LOG && ALOX_DBG_LOG_CI
UT_METHOD(Log_ColorsAndStyles)
{
    UT_INIT()

    Log_AddDebugLogger()

    Log_SetDomain( "COLORS", Scope::Method )

    Log_Prune( MemoryLogger* testML= new MemoryLogger(); )
    Log_SetVerbosity( testML, Verbosity::Verbose )

    Log_Info( "Playing with colors. Depending on the test environment, the colors might "
              "be visible or not" )
    Log_Info( "In the memory logger, all color/style codes have to disappear (by default)" )
    Log_Verbose( "A verbose message" )
    Log_Info   ( "An info message" )
    Log_Warning( "A warning message" )
    Log_Error  ( "An error message" )
    integer mlPos= testML->MemoryLog.IndexOf(A_CHAR("/COLORS") );
    UT_TRUE( mlPos > 0 )
    mlPos+= 8;


    Log::DebugLogger->MetaInfo->Format.Reset( A_CHAR("") );
    Log_Info(    String256("FG Colors:  ")
                                << ">>>" << ESC::RED     << "RED"        << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::GREEN   << "GREEN"      << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::BLUE    << "BLUE"       << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::CYAN    << "CYAN"       << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::MAGENTA << "MAGENTA"    << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::YELLOW  << "YELLOW"     << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::GRAY    << "GRAY"       << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::WHITE   << "WHITE"      << ESC::FG_RESET << "<<<"
                                << ">>>" << ESC::BLACK   << "BLACK"      << ESC::FG_RESET << "<<<"
                                )

    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("BG Colors:  ")
                                << ">>>" << ESC::BG_RED     << "RED"        << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_GREEN   << "GREEN"      << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_BLUE    << "BLUE"       << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_CYAN    << "CYAN"       << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_MAGENTA << "MAGENTA"    << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_YELLOW  << "YELLOW"     << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_GRAY    << "GRAY"       << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_WHITE   << "WHITE"      << ESC::BG_RESET << "<<<"
                                << ">>>" << ESC::BG_BLACK   << "BLACK"      << ESC::BG_RESET << "<<<"
                                )

    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("FG/BG same: ")
                            << ">>>" << ESC::RED     << ESC::BG_RED     << "RED"        << ESC::RESET << "<<<"
                            << ">>>" << ESC::GREEN   << ESC::BG_GREEN   << "GREEN"      << ESC::RESET << "<<<"
                            << ">>>" << ESC::BLUE    << ESC::BG_BLUE    << "BLUE"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::CYAN    << ESC::BG_CYAN    << "CYAN"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::MAGENTA << ESC::BG_MAGENTA << "MAGENTA"    << ESC::RESET << "<<<"
                            << ">>>" << ESC::YELLOW  << ESC::BG_YELLOW  << "YELLOW"     << ESC::RESET << "<<<"
                            << ">>>" << ESC::GRAY    << ESC::BG_GRAY    << "GRAY"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::WHITE   << ESC::BG_WHITE   << "WHITE"      << ESC::RESET << "<<<"
                            << ">>>" << ESC::BLACK   << ESC::BG_BLACK   << "BLACK"      << ESC::RESET << "<<<"
                            )

    Log_GetLogger( acl, "DEBUG_LOGGER" )
    Log_Prune(
      TextLogger::LightColorUsage oldVal1= TextLogger::LightColorUsage::Never;
      if ( acl != nullptr && acl->GetTypeName() == "ANSI_CONSOLE")
      {
          oldVal1= static_cast<AnsiConsoleLogger*>(acl)->UseLightColors;
          static_cast<AnsiConsoleLogger*>(acl)->UseLightColors= static_cast<AnsiConsoleLogger*>(acl)->UseLightColors == TextLogger::LightColorUsage::Foreground
                                                                 ? TextLogger::LightColorUsage::Background
                                                                 : TextLogger::LightColorUsage::Foreground;
      }
    )

    #if defined(_WIN32 )
    Log_GetLogger( wcl, "WINDOWS_CONSOLE" );
    Log_Prune(
      TextLogger::LightColorUsage oldVal2= TextLogger::LightColorUsage::Never;
      if ( wcl != nullptr)
      {
          oldVal2= static_cast<WindowsConsoleLogger*>(wcl)->UseLightColors;
          static_cast<WindowsConsoleLogger*>(wcl)->UseLightColors= static_cast<AnsiConsoleLogger*>(wcl)->UseLightColors == TextLogger::LightColorUsage::Foreground
                                                                 ? TextLogger::LightColorUsage::Background
                                                                 : TextLogger::LightColorUsage::Foreground;
      }
    )
    #endif

    Log_Info(    String256("Same rev.:  ")
                            << ">>>" << ESC::RED     << ESC::BG_RED     << "RED"        << ESC::RESET << "<<<"
                            << ">>>" << ESC::GREEN   << ESC::BG_GREEN   << "GREEN"      << ESC::RESET << "<<<"
                            << ">>>" << ESC::BLUE    << ESC::BG_BLUE    << "BLUE"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::CYAN    << ESC::BG_CYAN    << "CYAN"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::MAGENTA << ESC::BG_MAGENTA << "MAGENTA"    << ESC::RESET << "<<<"
                            << ">>>" << ESC::YELLOW  << ESC::BG_YELLOW  << "YELLOW"     << ESC::RESET << "<<<"
                            << ">>>" << ESC::GRAY    << ESC::BG_GRAY    << "GRAY"       << ESC::RESET << "<<<"
                            << ">>>" << ESC::WHITE   << ESC::BG_WHITE   << "WHITE"      << ESC::RESET << "<<<"
                            << ">>>" << ESC::BLACK   << ESC::BG_BLACK   << "BLACK"      << ESC::RESET << "<<<"
                            )
    Log_Prune( if ( acl != nullptr && acl->GetTypeName() == "ANSI_CONSOLE")  static_cast<AnsiConsoleLogger*>(acl) ->UseLightColors= oldVal1; )
    #if defined(_WIN32 )
    Log_Prune( if ( wcl != nullptr )  ((WindowsConsoleLogger*) wcl) ->UseLightColors= oldVal2; )
    #endif

    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE

    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();
    Log_Info(    String256("Reset FG/BG: ")
                                << ESC::BG_RED     << "BG RED"
                                << ESC::GREEN      << "FG GREEN"
                                << ESC::FG_RESET   << "FG Reset"
                                << ESC::BG_RESET   << "BG Reset"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Reset FG/BG: ")
                                << ESC::BG_RED     << "BG RED"
                                << ESC::GREEN      << "FG GREEN"
                                << ESC::BG_RESET   << "BG Reset"
                                << ESC::FG_RESET   << "FG Reset"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Reset FG/BG: ")
                                << ESC::BG_RED     << "BG RED"
                                << ESC::GREEN      << "FG GREEN"
                                << ESC::RESET      << "All Reset"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Styles: ")
                                << ">>>" << ESC::BOLD     << "Bold"          << ESC::STYLE_RESET << "<<<"
                                << ">>>" << ESC::ITALICS  << "Italics"       << ESC::STYLE_RESET << "<<<"
                                << ">>>" << ESC::BOLD
                                << ">>>" << ESC::ITALICS  << "Bold/Italics"  << ESC::STYLE_RESET << "<<<  Styles do not work in Windows consoles"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Reset Style/FG/BG: ")
                                << ESC::BOLD        << "Bold"
                                << ESC::BG_RED      << "BG RED"
                                << ESC::GREEN       << "FG GREEN"
                                << ESC::BG_RESET    << "BG Reset"
                                << ESC::FG_RESET    << "FG Reset"
                                << ESC::STYLE_RESET << "Style Reset"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Reset Style/FG/BG: ")
                                << ESC::BOLD        << "Bold"
                                << ESC::BG_RED      << "BG RED"
                                << ESC::GREEN       << "FG GREEN"
                                << ESC::RESET       << "Reset All"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("Reset Style/FG/BG: ")
                                << ESC::BOLD        << "Bold"
                                << ESC::BG_RED      << "BG RED"
                                << ESC::GREEN       << "FG GREEN"
                                << ESC::STYLE_RESET << "Style Reset"
                                << ESC::BG_RESET    << "BG Reset"
                                << ESC::FG_RESET    << "FG Reset"
                                << "   <<< This does not work on ANSI terminals: Styl reset, resets ALL here"
                                )
    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    Log_Info(    String256("An URL: ")
                                << ESC::URL_START   << "www.darwin.heaven"
                                << ESC::URL_END     << ", your resource for happyness!"
                                )

    Log_Verbose( testML->MemoryLog.Terminate() + mlPos )
    UT_TRUE( testML->MemoryLog.IndexOf('\033') < 0 ) testML->MemoryLog.Reset();

    ALIB_WARNINGS_RESTORE

    Log_RemoveLogger( testML )
    Log_Prune( delete testML; )
}
#endif // ALOX_REL_LOG_CI

/** ********************************************************************************************
* Log_TextLogger_RegisterStdStreamLocks.
**********************************************************************************************/
#if ALOX_DBG_LOG && ALIB_THREADS
UT_METHOD(Log_TextLogger_RegisterStdStreamLocks)
{
    UT_INIT() // This already registers the unittest logger. Therefore, the console lock in \alib
               // is occupied once already, but not in Safe mode, yet
                               UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Unsafe )
    Log_AddDebugLogger()
    UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe )
    Log_RemoveDebugLogger()    UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Unsafe )
    Log_AddDebugLogger()       UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe )

    #if ALOX_REL_LOG
        #define LOX_LOX lox
        Lox lox("ReleaseLox");

        // a memory logger must not change anything!
        Lox_SetVerbosity( Log::DebugLogger, Verbosity::Verbose )  UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        MemoryLogger ml;
        Lox_SetVerbosity( &ml,              Verbosity::Verbose )  UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Lox_RemoveLogger( Log::DebugLogger)                       UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Log_RemoveDebugLogger()                                   UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Unsafe )
        Lox_RemoveLogger( &ml )                                   UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Unsafe )


        // while a console logger does
        Log_AddDebugLogger()                                      UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Lox_SetVerbosity( Log::DebugLogger, Verbosity::Verbose )  UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        ConsoleLogger cl;
        Lox_SetVerbosity( &cl,              Verbosity::Verbose )  UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Log_SetVerbosity( &cl,              Verbosity::Verbose )  UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )

        Lox_RemoveLogger( Log::DebugLogger)                       UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Log_RemoveLogger( &cl)                                    UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
        Lox_RemoveLogger( &cl )                                   UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Safe   )
     #endif
     Log_RemoveDebugLogger()                                      UT_TRUE( SmartLock::StdOutputStreams.GetSafeness() == lang::Safeness::Unsafe )
}
#endif // ALOX_DBG_LOG

/** ********************************************************************************************
* Log_TextLogger_FormatConfig
**********************************************************************************************/
void testFormatConfig( AWorxUnitTesting& ut, const String& testFormat,
                       const String& expFmt,
                       const String& expFmtError    = NullString(),
                       const String& expFmtWarning  = NullString(),
                       const String& expFmtInfo     = NullString(),
                       const String& expFmtVerbose  = NullString()
                     )
{
    Variable var;
    BASECAMP.GetConfig().Store( var.Declare(A_CHAR("ALOX"), A_CHAR("TESTML_FORMAT"), ','), testFormat  );
    MemoryLogger ml("TESTML");

    Lox lox("T", false );
    lox.Acquire( "ut_alox_logger.cpp", 425, "testFormatConfig" );
    lox.SetVerbosity( &ml, Verbosity::Info );

                                      UT_EQ( expFmt       , ml.MetaInfo->Format )
    if( expFmtError  .IsNotNull() ) { UT_EQ( expFmtError  , ml.MetaInfo->VerbosityError   ) }
    if( expFmtWarning.IsNotNull() ) { UT_EQ( expFmtWarning, ml.MetaInfo->VerbosityWarning ) }
    if( expFmtInfo   .IsNotNull() ) { UT_EQ( expFmtInfo   , ml.MetaInfo->VerbosityInfo    ) }
    if( expFmtVerbose.IsNotNull() ) { UT_EQ( expFmtVerbose, ml.MetaInfo->VerbosityVerbose ) }

    lox.RemoveLogger(&ml);
    lox.Release();

}


UT_METHOD(Log_TextLogger_FormatConfig)
{
    UT_INIT()
    testFormatConfig( ut, A_CHAR("Test")                                , A_CHAR("Test")                     );
    testFormatConfig( ut, A_CHAR("\"Test")                              , A_CHAR("\"Test")                   );
    testFormatConfig( ut, A_CHAR("\\\"Test")                            , A_CHAR("\"Test")                   );
    testFormatConfig( ut, A_CHAR("\"Test\"")                            , A_CHAR("Test")                     );
    testFormatConfig( ut, A_CHAR("  \" Test \"        X ")              , A_CHAR("\" Test \"        X")      );
    testFormatConfig( ut, A_CHAR("\"  Te\"st \"")                       , A_CHAR("  Te\"st ")                );

    testFormatConfig( ut, A_CHAR(" Test , a ,b,   c,d  ")               , A_CHAR("Test"), A_CHAR("a"),A_CHAR("b"),A_CHAR("c"),A_CHAR("d")    );
    testFormatConfig( ut, A_CHAR(" Test , a ,b,\" ,  c\",d  ")          , A_CHAR("Test"), A_CHAR("a"),A_CHAR("b"),A_CHAR(" ,  c"),A_CHAR("d"));
}

/** ********************************************************************************************
 * Log_LogLevelSetting
 **********************************************************************************************/
class TestMetaInfo : public lox::detail::textlogger::MetaInfo
{
    public:       void    t(AString& buf, int64_t diff)     { writeTimeDiff( buf, diff ); }
};

UT_METHOD(Log_TextLoggerTimeDiff)
{
    UT_INIT()

    TestMetaInfo mi;
    mi.TimeDiffMinimum= 0;
    AString ms;
    int64_t diff;
    int64_t micros=     1000L;
    int64_t millis=     1000L * micros;
    int64_t secs=       1000L * millis;
    int64_t mins=          60 * secs;
    int64_t hours=         60 * mins;
    int64_t days=          24 * hours;


    mi.TimeDiffMinimum= 0;
    diff= 0;                            ms.Reset(); mi.t( ms, diff );
    UT_EQ( String16( "000" )._( mi.TimeDiffNanos    ),   ms )
    diff= 15;                           ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "015"  )._( mi.TimeDiffNanos    ),    ms )
    diff= 99;                           ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "099"  )._( mi.TimeDiffNanos    ),    ms )
    diff= 600;                          ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "600"  )._( mi.TimeDiffNanos    ),    ms )
    diff= 999;                          ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.TimeDiffNanos    ),    ms )
    diff= 1000;                         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.TimeDiffMicros   ),    ms )
    mi.TimeDiffMinimum= 700;
    diff= 600;                          ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(          )._( mi.TimeDiffNone    ),    ms )
    diff= 700;                          ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "700"  )._( mi.TimeDiffNanos    ),    ms )
    diff= 999;                          ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.TimeDiffNanos    ),    ms )
    mi.TimeDiffMinimum= 1000;
    diff= 1000;                         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.TimeDiffMicros   ),    ms )
    diff= 15 * micros;                  ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "015"  )._( mi.TimeDiffMicros   ),    ms )
    diff= 99 * micros;                  ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "099"  )._( mi.TimeDiffMicros   ),    ms )
    diff= 600* micros;                  ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "600"  )._( mi.TimeDiffMicros   ),    ms )
    diff= 999* micros;                  ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.TimeDiffMicros   ),    ms )
    diff= 1   * millis;                 ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.TimeDiffMillis   ),    ms )
    diff= 999 * millis;                 ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.TimeDiffMillis   ),    ms )
    diff= 1   * secs;                   ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "1.00" )._( mi.TimeDiffSecs     ),    ms )

    diff= 2   * secs + 344 * millis;    ms.Reset(); mi.t( ms, diff );
    UT_EQ( String16(  "2.34" )._( mi.TimeDiffSecs     ),    ms )

    diff= 3   * secs + 345 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "3.35" )._( mi.TimeDiffSecs     ),    ms )
    diff= 9   * secs + 994 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.TimeDiffSecs     ),    ms )
    diff= 9   * secs + 995 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.TimeDiffSecs     ),    ms )
    diff= 9   * secs + 999 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.TimeDiffSecs     ),    ms )
    diff= 10  * secs + 940 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.9" )._( mi.TimeDiffSecs     ),    ms )
    diff= 10  * secs + 950 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "11.0" )._( mi.TimeDiffSecs     ),    ms )

    diff= 99  * secs + 900 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffSecs     ),    ms )
    diff= 99  * secs + 949 * millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffSecs     ),    ms )


    diff= 2  * mins + 0 * secs;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "2.00" )._( mi.TimeDiffMins     ),    ms )
    diff= 2  * mins + 30 * secs;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "2.50" )._( mi.TimeDiffMins     ),    ms )
    diff= 9  * mins + 45 * secs;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.75" )._( mi.TimeDiffMins     ),    ms )
    diff= 9  * mins + 59 * secs;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.TimeDiffMins     ),    ms )
    diff= 9  * mins + 59500 * millis;   ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.TimeDiffMins     ),    ms )
    diff= 9  * mins + 59999 * millis;   ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.TimeDiffMins     ),    ms )

    diff= 99 * mins + 0 * secs;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.0" )._( mi.TimeDiffMins     ),    ms )
    diff= 99 * mins + 30* secs;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.5" )._( mi.TimeDiffMins     ),    ms )
    diff= 99 * mins + 59* secs;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffMins     ),    ms )
    diff= 99 * mins + 59500* millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffMins     ),    ms )
    diff= 99 * mins + 59999* millis;    ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "1.66" )._( mi.TimeDiffHours    ),    ms )
    diff= 1 * hours + 30* mins;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "90.0" )._( mi.TimeDiffMins     ),    ms )

    diff= 5 * hours + 30* mins;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "5.50" )._( mi.TimeDiffHours    ),    ms )

    diff= 9 * hours + 45* mins;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.75" )._( mi.TimeDiffHours    ),    ms )
    diff= 9 * hours + 59* mins;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.TimeDiffHours    ),    ms )
    diff= 9 * hours + 3540* secs;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.TimeDiffHours    ),    ms )
    diff= 9 * hours + 3580* secs;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.TimeDiffHours    ),    ms )
    diff= 9 * hours + 3599* secs;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.TimeDiffHours    ),    ms )
    diff= 9 * hours + 3600* secs;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.TimeDiffHours    ),    ms )

    diff= 50 * hours + 15 *mins;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "50.2" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 45 *mins;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.7" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 48 *mins;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.8" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 59 *mins;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 3540* secs;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 3580* secs;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 3599* secs;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffHours    ),    ms )
    diff= 99 * hours + 3600* secs;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "4.16" )._( mi.TimeDiffDays     ),    ms )

    diff= 1 * days + 12* hours;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "36.0" )._( mi.TimeDiffHours    ),    ms )

    diff= 5 * days + 18* hours;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "5.75" )._( mi.TimeDiffDays     ),    ms )
    diff= 9 * days + 23* hours;         ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.95" )._( mi.TimeDiffDays     ),    ms )
    diff= 9 * days + 1380 * mins;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.95" )._( mi.TimeDiffDays     ),    ms )
    diff= 9 * days + 1400 * mins;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.97" )._( mi.TimeDiffDays     ),    ms )
    diff= 9 * days + 1439 * mins;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.TimeDiffDays     ),    ms )
    diff= 9 * days + 1440 * mins;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.TimeDiffDays     ),    ms )
    diff= 15 * days + 6 * hours;        ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "15.2" )._( mi.TimeDiffDays     ),    ms )
    diff= 99 * days + 18 * hours;       ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.7" )._( mi.TimeDiffDays     ),    ms )
    diff= 99 * days + 1439 * mins;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.TimeDiffDays     ),    ms )
    diff= 99 * days + 1440 * mins;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16( "100.0" )._( mi.TimeDiffDays     ),    ms )

    diff= 13452 * days+ 12* hours;      ms.Reset(); mi.t( ms, diff ); UT_EQ( String16("13452.5")._( mi.TimeDiffDays     ),    ms )
}


/** ********************************************************************************************
 * Log_Recursive
 **********************************************************************************************/
#if !ALIB_DEBUG_MONOMEM
UT_METHOD(Log_Recursive)
{
    UT_INIT()

    ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( ut_reclog::AppendLog* )

    Log_SetDomain( "RECURSION", Scope::Method )
    MemoryLogger* testML= new MemoryLogger();
    Log_SetVerbosity( testML , Verbosity::Verbose)


    // tests without the use of the formatter when logging recursively
    {
        integer oldCntLogs= testML->CntLogs;
        String32 test;
        test << ut_reclog::AppendLog(A_CHAR("Test"), 1);
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("Logging object >Test<"))  > 0 )
        UT_EQ( 1, testML->CntLogs - oldCntLogs )
        UT_PRINT( testML->MemoryLog )
        testML->MemoryLog.Reset();

        oldCntLogs= testML->CntLogs;
        Log_Info( "outer>{}<log", ut_reclog::AppendLog(A_CHAR("Test"), 1) )
        UT_EQ( 2, testML->CntLogs - oldCntLogs )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("outer>Test<log"))  > 0 )
        UT_PRINT( testML->MemoryLog )
        testML->MemoryLog.Reset();


        oldCntLogs= testML->CntLogs;
        Log_Info( "123{:^8}456--abc{!UP}efg", ut_reclog::AppendLog(A_CHAR("Test"), 1), ut_reclog::AppendLog(A_CHAR("lowerTest"), 1) )
        UT_EQ( 3, testML->CntLogs - oldCntLogs )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("123  Test  456")     )  > 0 )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("abcLOWERTESTefg"))  > 0 )
        UT_PRINT( testML->MemoryLog )
    }

    // same tests, now using formatter recursively
    {
        integer oldCntLogs= testML->CntLogs;
        String32 test;
        test << ut_reclog::AppendLog(A_CHAR("Test"), 2);
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("Logging object >Test<"))  > 0 )
        UT_EQ( 1, testML->CntLogs - oldCntLogs )
        UT_PRINT( testML->MemoryLog )
        testML->MemoryLog.Reset();

        oldCntLogs= testML->CntLogs;
        Log_Info( "outer>{}<log", ut_reclog::AppendLog(A_CHAR("Test"), 2) )
        UT_EQ( 2, testML->CntLogs - oldCntLogs )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("outer>Test<log"))  > 0 )
        UT_PRINT( testML->MemoryLog )
        testML->MemoryLog.Reset();


        oldCntLogs= testML->CntLogs;
        Log_Info( "123{:^8}456--abc{!UP}efg", ut_reclog::AppendLog(A_CHAR("Test"), 2), ut_reclog::AppendLog(A_CHAR("lowerTest"), 2) )
        UT_EQ( 3, testML->CntLogs - oldCntLogs )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("123  Test  456")     )  > 0 )
        UT_TRUE( testML->MemoryLog.IndexOf( A_CHAR("abcLOWERTESTefg"))  > 0 )
        UT_PRINT( testML->MemoryLog )
    }


    Log_RemoveLogger( testML )
    Log_Prune( delete testML );
}

#endif // !ALIB_DEBUG_MONOMEM

#include "unittests/aworx_unittests_end.hpp"


} // namespace

#endif // ALOX_DBG_LOG


#endif // ALIB_UT_ALOX

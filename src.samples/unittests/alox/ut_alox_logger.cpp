// #################################################################################################
//  Unit Tests - ALox Logging Library
//  (Unit Tests to create tutorial sample code and output)
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_ALOX

#include "ALib.Camp.H"
#include "ALib.Camp.Base.H"
#include "ALib.ALox.H"

#if ALOX_DBG_LOG
#include "ALib.Strings.H"
#include "ALib.Strings.Escaper.H"
#include "ALib.Variables.IniFile.H"
#include "ALib.Variables.H"

#include <iostream>
#include <fstream>

#include "ALib.Lang.CIMethods.H"

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

    template<typename TAllocator> struct AppendableTraits<ut_reclog::AppendLog,character,TAllocator>
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

            target._<NC>( src.Text );
        }
    };

    template<typename TAllocator> struct AppendableTraits<ut_reclog::AppendLog*,character, TAllocator>
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

            target._<NC>( src->Text );
        }
    };


}} // namespace [alib::lox::strings]


#define TESTCLASSNAME       CPP_ALox_Logger
#include "aworx_unittests.hpp"

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



    Log::DebugLogger->GetFormatMultiLine().Mode= 0;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->GetFormatMultiLine().Mode= 0;
    Log::DebugLogger->GetFormatMultiLine().DelimiterReplacement.Reset("~|~");
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) with delimiter replacement set to ~|~ --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->GetFormatMultiLine().Mode= 0;
    Log::DebugLogger->GetFormatMultiLine().Delimiter.Reset();
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 0 (single line) with delimiter set to \"\" (stops multi line processing) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )
    Log::DebugLogger->GetFormatMultiLine().Delimiter.SetNull();

    Log::DebugLogger->GetFormatMultiLine().Mode= 1;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 1 (multi line, all meta info per line) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->GetFormatMultiLine().Mode= 2;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 2 (multi line, meta info blanked) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->GetFormatMultiLine().Mode= 3;
    Log_Info( "" )
    Log_Info( "-------- ML Mode = 3 (multi line, print headline with info, text starts at pos 0) --------" )
    Log_LogState( "MLINE", Verbosity::Info, A_CHAR("Our Log configuration is:") )

    Log::DebugLogger->GetFormatMultiLine().Mode= 4;
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


    Log::DebugLogger->GetFormatMetaInfo().Format.Reset( A_CHAR("") );
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

    Log_GetLogger( pacl, "DEBUG_LOGGER" )
    Log_Prune(
      lox::textlogger::ColorfulLoggerParameters::LightColorUsage oldVal1= lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Never;
      if ( pacl != nullptr && pacl->GetTypeName() == "ANSI_CONSOLE")
      {
          auto& acl= *static_cast<AnsiConsoleLogger*>(pacl);
          oldVal1= acl.CFP.LCU;
          acl.CFP.LCU= acl.CFP.LCU == lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground
                                      ? lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Background
                                      : lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground;
      }
    )

    #if defined(_WIN32 )
    Log_GetLogger( pwcl, "WINDOWS_CONSOLE" );
    Log_Prune(
      lox::textlogger::ColorfulLoggerParameters::LightColorUsage oldVal2= lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Never;
      if ( pwcl != nullptr)
      {
          auto& wcl= *static_cast<WindowsConsoleLogger*>(pwcl);
          oldVal2= wcl.CFP.LCU;
          wcl.CFP.LCU= wcl.CFP.LCU == lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground
                                      ? lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Background
                                      : lox::textlogger::ColorfulLoggerParameters::LightColorUsage::Foreground;
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
    Log_Prune( if ( pacl != nullptr && pacl->GetTypeName() == "ANSI_CONSOLE")  static_cast<AnsiConsoleLogger*>(pacl) ->CFP.LCU= oldVal1; )
    #if defined(_WIN32 )
    Log_Prune( if ( pwcl != nullptr )  ((WindowsConsoleLogger*) pwcl) ->CFP.LCU= oldVal2; )
    #endif

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

    Log_RemoveLogger( testML )
    Log_Prune( delete testML; )
}
#endif // ALOX_REL_LOG_CI

/** ********************************************************************************************
* Log_TextLogger_FormatConfig
**********************************************************************************************/
#include "ALib.Lang.CIFunctions.H"

void testFormatConfig( AWorxUnitTesting& ut, const String& testFormat,
                       const String& expFmt,
                       const String& expFmtError    = NULL_STRING,
                       const String& expFmtWarning  = NULL_STRING,
                       const String& expFmtInfo     = NULL_STRING,
                       const String& expFmtVerbose  = NULL_STRING
                     )
{
    Variable vc= variables::CampVariable( BASECAMP,
                                          A_CHAR("ALOX/TESTML/FORMAT"), A_CHAR("ALOXFMI") );
    StringEscaperStandard escaper;
    vc.Import(testFormat, Priority::DefaultValues, &escaper );

    MemoryLogger ml("TESTML");

    Lox lox("T", false );
    lox.Acquire( LOX_CI );
    lox.SetVerbosity( &ml, Verbosity::Info );

                                      UT_EQ( expFmt       , ml.GetFormatMetaInfo().Format )
    if( expFmtError  .IsNotNull() ) { UT_EQ( expFmtError  , ml.GetFormatMetaInfo().VerbosityError   ) }
    if( expFmtWarning.IsNotNull() ) { UT_EQ( expFmtWarning, ml.GetFormatMetaInfo().VerbosityWarning ) }
    if( expFmtInfo   .IsNotNull() ) { UT_EQ( expFmtInfo   , ml.GetFormatMetaInfo().VerbosityInfo    ) }
    if( expFmtVerbose.IsNotNull() ) { UT_EQ( expFmtVerbose, ml.GetFormatMetaInfo().VerbosityVerbose ) }

    lox.RemoveLogger(&ml);
    lox.Release();
}
#include "ALib.Lang.CIMethods.H"


UT_METHOD(Log_TextLogger_FormatConfig)
{
    UT_INIT()
    testFormatConfig( ut, A_CHAR("Test")                                , A_CHAR("Test")                     );
    testFormatConfig( ut, A_CHAR("\"Test")                              , A_CHAR("\"Test")                   );
    testFormatConfig( ut, A_CHAR("\\\"Test")                            , A_CHAR("\"Test")                   );
    testFormatConfig( ut, A_CHAR("\"Test\"")                            , A_CHAR("Test")                     );
    testFormatConfig( ut, A_CHAR("Te\"st")                              , A_CHAR("Te\"st")                   );
    testFormatConfig( ut, A_CHAR("  \" Test \"        X ")              , A_CHAR("\" Test \"        X")      );
    testFormatConfig( ut, A_CHAR("\"  Te\"st \"")                       , A_CHAR("  Te\"st ")                );

    testFormatConfig( ut, A_CHAR(" Test , a ,b,   c,d  ")               , A_CHAR("Test"), A_CHAR("a"),A_CHAR("b"),A_CHAR("c"),A_CHAR("d")    );
    testFormatConfig( ut, A_CHAR(" Test , a ,b,\" ,  c\",d  ")          , A_CHAR("Test"), A_CHAR("a"),A_CHAR("b"),A_CHAR(" ,  c"),A_CHAR("d"));
}

#include "aworx_callerinfo_ut.hpp"

/** ********************************************************************************************
 * Log_LogLevelSetting
 **********************************************************************************************/
struct TestTextLogger : public lox::textlogger::TextLogger
{
    void    t(AString& buf, int64_t diff)     { writeTimeDiff( buf, diff ); }
    void logText(  lox::detail::Domain&    ,
                   Verbosity               ,
                   AString&                ,
                   lox::detail::ScopeInfo& ,
                   int                  )      override{}
    void notifyMultiLineOp( lang::Phase )      override{}
    
    TestTextLogger(AWorxUnitTesting& ut) : TextLogger("","", false)
    {
        ALIB_LOCK_WITH(ALOX.GetConfig())
            varFormatTimeDiff.Declare(A_CHAR("ALOX/FORMAT_TIME_DIFF"), A_CHAR("ALOXFTD"));
            (void)varFormatTimeDiff.Define(alib::variables::Priority::DefaultValues);
        #if ALIB_SINGLE_THREADED
            (void) ut;
        #endif
    }
};

UT_METHOD(Log_TextLogger_TimeDiff)
{
    UT_INIT()

    TestTextLogger ttl(ut);
    auto& mi= ttl.GetFormatTimeDiff();
    mi.Minimum= 0;
    AString ms;
    int64_t diff;
    int64_t micros=     1000L;
    int64_t millis=     1000L * micros;
    int64_t secs=       1000L * millis;
    int64_t mins=          60 * secs;
    int64_t hours=         60 * mins;
    int64_t days=          24 * hours;


    mi.Minimum= 0;
    diff= 0;                            ms.Reset(); ttl.t( ms, diff );
    UT_EQ( String16( "000" )._( mi.Nanos    ),   ms )
    diff= 15;                           ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "015"  )._( mi.Nanos    ),    ms )
    diff= 99;                           ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "099"  )._( mi.Nanos    ),    ms )
    diff= 600;                          ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "600"  )._( mi.Nanos    ),    ms )
    diff= 999;                          ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.Nanos    ),    ms )
    diff= 1000;                         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.Micros   ),    ms )
    mi.Minimum= 700;
    diff= 600;                          ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(          )._( mi.None    ),    ms )
    diff= 700;                          ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "700"  )._( mi.Nanos    ),    ms )
    diff= 999;                          ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.Nanos    ),    ms )
    mi.Minimum= 1000;
    diff= 1000;                         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.Micros   ),    ms )
    diff= 15 * micros;                  ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "015"  )._( mi.Micros   ),    ms )
    diff= 99 * micros;                  ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "099"  )._( mi.Micros   ),    ms )
    diff= 600* micros;                  ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "600"  )._( mi.Micros   ),    ms )
    diff= 999* micros;                  ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.Micros   ),    ms )
    diff= 1   * millis;                 ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "001"  )._( mi.Millis   ),    ms )
    diff= 999 * millis;                 ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "999"  )._( mi.Millis   ),    ms )
    diff= 1   * secs;                   ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "1.00" )._( mi.Secs     ),    ms )

    diff= 2   * secs + 344 * millis;    ms.Reset(); ttl.t( ms, diff );
    UT_EQ( String16(  "2.34" )._( mi.Secs     ),    ms )

    diff= 3   * secs + 345 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "3.35" )._( mi.Secs     ),    ms )
    diff= 9   * secs + 994 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.Secs     ),    ms )
    diff= 9   * secs + 995 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.Secs     ),    ms )
    diff= 9   * secs + 999 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.Secs     ),    ms )
    diff= 10  * secs + 940 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.9" )._( mi.Secs     ),    ms )
    diff= 10  * secs + 950 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "11.0" )._( mi.Secs     ),    ms )

    diff= 99  * secs + 900 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Secs     ),    ms )
    diff= 99  * secs + 949 * millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Secs     ),    ms )


    diff= 2  * mins + 0 * secs;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "2.00" )._( mi.Mins     ),    ms )
    diff= 2  * mins + 30 * secs;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "2.50" )._( mi.Mins     ),    ms )
    diff= 9  * mins + 45 * secs;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.75" )._( mi.Mins     ),    ms )
    diff= 9  * mins + 59 * secs;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.Mins     ),    ms )
    diff= 9  * mins + 59500 * millis;   ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.Mins     ),    ms )
    diff= 9  * mins + 59999 * millis;   ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.Mins     ),    ms )

    diff= 99 * mins + 0 * secs;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.0" )._( mi.Mins     ),    ms )
    diff= 99 * mins + 30* secs;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.5" )._( mi.Mins     ),    ms )
    diff= 99 * mins + 59* secs;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Mins     ),    ms )
    diff= 99 * mins + 59500* millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Mins     ),    ms )
    diff= 99 * mins + 59999* millis;    ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "1.66" )._( mi.Hours    ),    ms )
    diff= 1 * hours + 30* mins;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "90.0" )._( mi.Mins     ),    ms )

    diff= 5 * hours + 30* mins;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "5.50" )._( mi.Hours    ),    ms )

    diff= 9 * hours + 45* mins;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.75" )._( mi.Hours    ),    ms )
    diff= 9 * hours + 59* mins;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.Hours    ),    ms )
    diff= 9 * hours + 3540* secs;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.98" )._( mi.Hours    ),    ms )
    diff= 9 * hours + 3580* secs;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.Hours    ),    ms )
    diff= 9 * hours + 3599* secs;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.Hours    ),    ms )
    diff= 9 * hours + 3600* secs;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.Hours    ),    ms )

    diff= 50 * hours + 15 *mins;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "50.2" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 45 *mins;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.7" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 48 *mins;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.8" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 59 *mins;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 3540* secs;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 3580* secs;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 3599* secs;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Hours    ),    ms )
    diff= 99 * hours + 3600* secs;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "4.16" )._( mi.Days     ),    ms )

    diff= 1 * days + 12* hours;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "36.0" )._( mi.Hours    ),    ms )

    diff= 5 * days + 18* hours;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "5.75" )._( mi.Days     ),    ms )
    diff= 9 * days + 23* hours;         ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.95" )._( mi.Days     ),    ms )
    diff= 9 * days + 1380 * mins;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.95" )._( mi.Days     ),    ms )
    diff= 9 * days + 1400 * mins;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.97" )._( mi.Days     ),    ms )
    diff= 9 * days + 1439 * mins;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "9.99" )._( mi.Days     ),    ms )
    diff= 9 * days + 1440 * mins;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "10.0" )._( mi.Days     ),    ms )
    diff= 15 * days + 6 * hours;        ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "15.2" )._( mi.Days     ),    ms )
    diff= 99 * days + 18 * hours;       ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.7" )._( mi.Days     ),    ms )
    diff= 99 * days + 1439 * mins;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16(  "99.9" )._( mi.Days     ),    ms )
    diff= 99 * days + 1440 * mins;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16( "100.0" )._( mi.Days     ),    ms )

    diff= 13452 * days+ 12* hours;      ms.Reset(); ttl.t( ms, diff ); UT_EQ( String16("13452.5")._( mi.Days     ),    ms )
}


/** ********************************************************************************************
 * Log_Recursive
 **********************************************************************************************/
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

#include "aworx_unittests_end.hpp"


} // namespace

#endif // ALOX_DBG_LOG


#endif // ALIB_UT_ALOX

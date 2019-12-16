// #################################################################################################
//  aworx::lib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_INI_FILE)
#      include "alib/config/inifile.hpp"
#   endif
#   if !defined (HPP_ALOX_DETAIL_LOGGER)
#      include "alib/alox/detail/logger.hpp"
#   endif
#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif

#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#      include "alib/resources/resources.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif

ALIB_BOXING_VTABLE_DEFINE( aworx::lib::lox::Verbosity               , vt_lox_verbosity )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::lox::Scope                   , vt_lox_scope     )
ALIB_BOXING_VTABLE_DEFINE( aworx::lib::lox::detail::Logger*         , vt_lox_logger    )
ALIB_BOXING_VTABLE_DEFINE( std::pair<aworx::lib::lox::Verbosity
                          ALIB_COMMA aworx::lib::config::Priorities>, vt_lox_pair_verby_prio )

#endif // !defined(ALIB_DOX)

namespace aworx { namespace lib {

lox::ALox ALOX;

/** ************************************************************************************************
 * This is the \b C++ version of <b>%ALox Logging Library</b>, which has been integrated
 * as one of many modules into the <b>ALib C++ Class Library</b>.<br>
 *
 * Please check out the \ref alib_mod_alox "documentation of ALib Module ALox" for more
 * information.
 **************************************************************************************************/
namespace lox {


constexpr const NString  Lox::InternalDomains;


ALox::ALox()
: Module( ALIB_VERSION, ALIB_REVISION, "ALOX" )
{
    ALIB_ASSERT_ERROR( this == &ALOX,
        "Instances of class Alox must not be created. Use singleton aworx::lib::ALOX" )
}

// #################################################################################################
// Compilation Flags
// #################################################################################################

// check compiler symbols, give warning once (therefore not in HPP)
#if !ALOX_DBG_LOG && ALOX_DBG_LOG_CI
#   pragma message ( "Warning: ALox compiler symbol mismatch: ALOX_DBG_LOG_CI is true, while ALOX_DBG_LOG is false." )
#endif
#if !ALOX_REL_LOG && ALOX_REL_LOG_CI
#   pragma message ( "Warning: ALox compiler symbol mismatch: ALOX_REL_LOG_CI is true, while ALOX_REL_LOG is false" )
#endif

// #################################################################################################
// Lox management
// #################################################################################################
#if !defined(ALIB_DOX)
    namespace
    {
        List<Lox*>      loxes( &lib::monomem::GlobalAllocator );
    }

#if ALOX_DBG_LOG
    Lox*                theDebugLox = nullptr;  // will be created in ALox::bootstrap
#endif

#endif


// The lox singletons for debug and release logging
Lox*     ALox::Get( const NString& name, CreateIfNotExists create )
{
    ALIB_LOCK_WITH(lib::monomem::GlobalAllocatorLock)

    // search
    for( auto* it : loxes )
        if( it->GetName().Equals<Case::Ignore>( name ) )
            return it;


    // create?
    if ( create == CreateIfNotExists::Yes )
    {
        Lox* newLox= new Lox ( name, false );
        loxes.EmplaceBack( newLox );
        return newLox;
    }

    // not found
    return nullptr;
}

void     ALox::Register( Lox* lox, ContainerOp operation )
{
    ALIB_LOCK_WITH(lib::monomem::GlobalAllocatorLock)

    // check
    if ( lox == nullptr )
    {
        ALIB_ERROR( "Nullptr given" )
        return;
    }

    // remove
    if( operation == ContainerOp::Remove )
    {
        for( auto search= loxes.begin() ; search != loxes.end() ; ++search )
            if ( *search == lox )
            {
                loxes.Erase( search );
                return;
            }
        ALIB_WARNING( "A lox named {!Q} could not be found for removal.",
                      lox != nullptr ? lox->GetName() : "<null>"          )
    }

    // insert
    else
    {
        for( auto* it : loxes )
            if( it->GetName().Equals( lox->GetName() ) )
            {
                ALIB_ERROR( "A lox named {!Q} was already registered. Registration ignored.",
                            lox->GetName() )
                return;
            }
        loxes.EmplaceBack( lox );
   }
}


// #################################################################################################
// ALox module initialization
// #################################################################################################

void  ALox::bootstrap( BootstrapPhases phase, int, const char**, const wchar_t** )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_verbosity       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_scope           )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_logger          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_pair_verby_prio )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        "Var0" ,   A_CHAR( "1|ALOX|NO_IDE_LOGGER|"                           "VD01||||VC01" )  ,
        "Var1" ,   A_CHAR( "2|ALOX|CONSOLE_TYPE|"                            "VD02||||VC02" )  ,
        "Var2" ,   A_CHAR( "3|ALOX|%1_%2_VERBOSITY|"                      "VD03|;|=|1|VC03" )  ,
        "Var3" ,   A_CHAR( "4|ALOX|GLOBAL_SOURCE_PATH_TRIM_RULES|"          "ES|;|=|1|VC04" )  ,
        "Var4" ,   A_CHAR( "5|ALOX|%1_SOURCE_PATH_TRIM_RULES|"              "ES|;|=|1|VC05" )  ,
        "Var5" ,   A_CHAR( "6|ALOX|%1_DOMAIN_SUBSTITUTION|"                   "|;|->|1|"    )  ,
        "Var6" ,   A_CHAR( "7|ALOX|%1_PREFIXES|"                            "ES|;|=|1|VC07" )  ,
        "Var7" ,   A_CHAR( "8|ALOX|%1_DUMP_STATE_ON_EXIT|"                  "VD08|,|||VC08" )  ,
        "Var8" ,   A_CHAR("20|ALOX|%1_AUTO_SIZES|"                               "||||VC20" )  ,
        "Var9" ,   A_CHAR("21|ALOX|%1_FORMAT|"                                 "|,||1|VC21" )  ,
        "Var10",   A_CHAR("22|ALOX|%1_FORMAT_DATE_TIME|"                        "|,|||VC22" )  ,
        "Var11",   A_CHAR("23|ALOX|%1_FORMAT_MULTILINE|"                        "|,|||VC23" )  ,
        "Var12",   A_CHAR("24|ALOX|%1_FORMAT_TIME_DIFF|"                        "|,|||VC24" )  ,
        "Var13",   A_CHAR("25|ALOX|%1_MAX_ELAPSED_TIME|"                    "VD25|,|||VC25" )  ,
        "Var14",   A_CHAR("26|ALOX|%1_REPLACEMENTS|"                            "|,|||VC26" )  ,
        "Var15",   A_CHAR("27|ALOX|CONSOLE_LIGHT_COLORS|"                      "ES||||VC27" )  ,
        #if defined(_WIN32)
        "Var16",   A_CHAR("28|ALOX|CODEPAGE|"                                "VD28||||VC28" )  ,
        #endif


        // Empty string. This is set with variables that want to be written into blank files.
        "ES",         A_CHAR(""),

        // configuration variable default values
        "VD01",       A_CHAR("false"),
        "VD02",       A_CHAR("default"),
        "VD03",       A_CHAR("writeback"),
        "VD08",       A_CHAR("none, verbosity=info, domain=/ALOX"),

        "VD25",       A_CHAR("0, limit=59"),
        #if defined(_WIN32)
        "VD28",       A_CHAR("65001"),
        #endif

        // configuration variable comments
        "VC01",       A_CHAR("If true, the creation of an additional, ide-specific debug logger is suppressed." "\n"
                           "(In particular suppresses DebugLogger (C#) and VStudioLogger (C++))" ),

        "VC02",       A_CHAR("Influences the type of console logger to be created by method"      "\n"
                           "Lox::CreateConsoleLogger which is also used by Log::AddDebugLogger" "\n"
                           "Possible values are: default, plain, ansi, windows, noqtcreator"         ),

        "VC03",       A_CHAR("The verbosities of logger \"%2\" in lox \"%1\". Use 'writeback [VAR_NAME] ;'"   "\n"
                           "to enable automatic writing on application exit."                  ),

        "VC04",       A_CHAR("Defines global source path trim rules (applicable for all Lox instances)."        "\n"
                           "   Format: [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; \u2026 ]" ),

        "VC05",       A_CHAR("Defines source path trim rules for Lox \"%1\". "           "\n"
                           "   Format: [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; \u2026 ]" ),

        "VC07",       A_CHAR("Prefix strings for log domains of lox \"%1\".\n"
                           "   Format: [*]domainpath[*] = prefixstring [, inclusion] [ ; \u2026 ] " ),

        "VC08",       A_CHAR("Log information about lox \"%1\" on exit. Comma separated list of arguments define" "\n"
                           "verbosity, domain and content of output. Possible values content arguments are:"    "\n"
                           "  All, " "Basic, " "Version, " "SPTR, " "Loggers, " "Domains, " "InternalDomains"   "\n"
                           "  ScopeDomains, " "DSR, " "PrefixLogables" "Once, " "LogData, " "ThreadMappings, "  "\n"
                           "  CompilationFlags." " If NONE is given nothing is dumped." ),

        "VC20",       A_CHAR("Auto size values of last run of Logger '%1' (generated and temporary values)."),

        "VC21",       A_CHAR("Meta info format of text logger \"%1\", including signatures for verbosity strings and" "\n"
                           "an optional string added to the end of each log statement."                             "\n"
                           "   Format: metaInfoFormat [, Error [, Warning [, Info [, Verbose [, MsgSuffix ]]]]]"),

        "VC22",       A_CHAR("Meta info date and time format of text logger \")%1\"."                         "\n"
                           "   Format: DateFormat [, TimeOfDayFormat [, TimeElapsedDays ]]]"),

        "VC23",       A_CHAR("Multi-line format of text logger \"%1\"."       "\n"
                           "   Format: MultiLineMsgMode [, FmtMultiLineMsgHeadline [, FmtMultiLinePrefix [, FmtMultiLineSuffix\n"
                           "           [, MultiLineDelimiter [, MultiLineDelimiterRepl ]]]]]"),

        "VC24",       A_CHAR("Meta info time difference entities of text logger \"%1\"."       "\n"
                           "   Format: TimeDiffMinimum [, TimeDiffNone [, TimeDiffNanos [, TimeDiffMicros [, TimeDiffMillis \n"
                           "           [, TimeDiffSecs [, TimeDiffMins [, TimeDiffHours [,  TimeDiffDays  ]]]]]]]]"),

        "VC25",       A_CHAR("Maximum elapsed time of all runs of Logger '%1'. To reset elapsed time display""\n"
                           "width, set this to 0 manually. Generated and temporary value.)" ),

        "VC26",       A_CHAR("Pairs of search and replacement strings for text logger \"%1\"."               "\n"
                           "   Format: search, replacement [, search, replacement] [,...]"),

        "VC27",       A_CHAR("Evaluated by colorful loggers that dispose about light and dark colors. Those may"  "\n"
                           "adjust their foreground and background color accordingly. If not given, under Windows OS" "\n"
                           "the right value is detected. Otherwise the value defaults to \"foreground\". In some"     "\n"
                           "occasions, the (detected or set) runtime environment might also indicate a different"     "\n"
                           "default value.  Possible values are 'foreground', 'background' and 'never'."),

        #if defined(_WIN32)
        "VC28",       A_CHAR("Code page used by class WindowsConsoleLogger. Defaults to 65001."           "\n"
                           "(Only used on Windows OS)" ),
        #endif

        //######################################    Enums    #######################################
        "Verbosity",        A_CHAR("0,Verbose,1,"
                                 "1,Info,1,"
                                 "2,Warning,1,"
                                 "2,Warnings,1," //allow with trailing s when reading
                                 "3,Error,1,"
                                 "3,Errors,1,"   //allow with trailing s when reading
                                 "4,Off,1"    ),

        "Scope",            A_CHAR("0,Global,1,"
                                 "1,ThreadOuter,7,"
                                 "2,Filename,1,"
                                 "3,Method,1,"
                                 "4,ThreadInner,7,"
                                 "5,Path,7"   ),

        "StateInfo",        A_CHAR("0" ","   "NONE"              ",1,"
                                   "1" ","   "Basic"             ",1,"
                                   "2" ","   "Version"           ",1,"
                                  "^9" ","   "LogData"           ",4,"
                                   "4" ","   "Loggers"           ",1,"
                                  "^6" ","   "DSR"               ",2,"
                                   "8" ","   "Domains"           ",1,"
                                  "^4" ","   "InternalDomains"   ",1,"
                            "0x100000" ","   "SPTR"              ",2,"
                                  "^5" ","   "ScopeDomains"      ",1,"
                                  "^7" ","   "PrefixLogables"    ",1,"
                                  "^8" ","   "Once"              ",1,"
                                 "^10" ","   "ThreadMappings"    ",1,"
                                 "^21" ","   "CompilationFlags"  ",1,"
                          "0xFFFFFFFF" ","   "All"               ",1"  ),

        "LightColorUsage",    A_CHAR("0,Auto,1,"
                                    "1,Never,1,"
                                    "2,Foreground,1,"
                                    "3,Background,1"  ),

        //#####################################    Various    ######################################
        "TLFmtExc",        A_CHAR("\nAn exception occurred during formatting ALox logables:\n" ),

        "INI_CMT_ALOX",    A_CHAR("@>'/// '@HL-Settings controlling ALox log output.\n@HL-"),

        // end of BootstrapBulk()
        nullptr  );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS

        // Add box-functions
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(aworx::lib::lox::Verbosity)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(aworx::lib::lox::Scope)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(aworx::lib::lox::detail::Logger*)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(std::pair<Verbosity ALIB_COMMA Priorities>)

        return;
    } // BootstrapPhases::PrepareResources

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Verbosity                  >::Bootstrap( *this, "Verbosity"        );
        EnumRecords<Scope                      >::Bootstrap( *this, "Scope"            );
        EnumRecords<StateInfo                  >::Bootstrap( *this, "StateInfo"        );
        EnumRecords<TextLogger::LightColorUsage>::Bootstrap( *this, "LightColorUsage"  );

        EnumRecords<Variables                  >::Bootstrap( '|' );
    }

    else if( phase == BootstrapPhases::Final )
    {
        #if ALIB_DEBUG
            auto& ga=
            lib::monomem::AcquireGlobalAllocator( ALIB_CALLER_PRUNED );
                theDebugLox= ga.Emplace<Lox>( "LOG");
            lib::monomem::ReleaseGlobalAllocator();
        #endif
    }
}


void ALox::shutdown( ShutdownPhases phase )
{
    if( phase == ShutdownPhases::Announce )
        IniFile::AddResourcedSectionComments( GetConfig(),
                                              GetResourcePool(), ResourceCategory, "INI_CMT_" );
    #if ALOX_DBG_LOG
    else if( phase == ShutdownPhases::Destruct )
    {
        if ( Log::DebugLogger != nullptr )
        Log_RemoveDebugLogger()

        monomem::Destruct( theDebugLox );

        while ( loxes.Size() > 0 )
        {
            detail::LI::Destruct( loxes.Back() );
            loxes.PopBack();
        }
    }
    #endif
}


void        ALox::Reset()
{
    Lox("trimruleresetlox", false).Reset(); // this clears the global source path trim rules

    #if ALOX_DBG_LOG
        if (Log::DebugLogger != nullptr )
            Log_RemoveDebugLogger()

        monomem::Destruct( theDebugLox);
    #endif

    ALIB_ASSERT_ERROR(loxes.Size()           == 0, "A Lox remained from the last test" )
    ALIB_ASSERT_ERROR(config->CountPlugins() == 4, "A config plug-in remained from the last test" )

    dynamic_cast<InMemoryPlugin*>( config->GetPlugin( lib::config::Priorities::DefaultValues  ) )->Clear();
    dynamic_cast<InMemoryPlugin*>( config->GetPlugin( lib::config::Priorities::ProtectedValues) )->Clear();

    #if ALOX_DBG_LOG
        new ( theDebugLox) Lox( "LOG");
    #endif
}

// #################################################################################################
// ESC codes
// #################################################################################################
#if defined(_MSC_VER)
// MSC  (as of 12/2015):
// C4579: in-class initialization for type 'const aworx::character[11]'
// is not yet implemented; static member will remain uninitialized at run-time but
// use in constant-expressions is supported
          character  ESC::RED        [4] { A_CHAR("\033c0") }; ///< Select red color for foreground.
          character  ESC::GREEN      [4] { A_CHAR("\033c1") }; ///< Select green color for foreground.
          character  ESC::YELLOW     [4] { A_CHAR("\033c2") }; ///< Select yellow color for foreground.
          character  ESC::BLUE       [4] { A_CHAR("\033c3") }; ///< Select blue color for foreground.
          character  ESC::MAGENTA    [4] { A_CHAR("\033c4") }; ///< Select magenta color for foreground.
          character  ESC::CYAN       [4] { A_CHAR("\033c5") }; ///< Select cyan color for foreground.
          character  ESC::BLACK      [4] { A_CHAR("\033c6") }; ///< Select black color for foreground.
          character  ESC::WHITE      [4] { A_CHAR("\033c7") }; ///< Select white color for foreground.
          character  ESC::GRAY       [4] { A_CHAR("\033c8") }; ///< Select gray color for foreground.
          character  ESC::FG_RESET   [4] { A_CHAR("\033c9") }; ///< Select std color for foreground.4

          character  ESC::BG_RED     [4] { A_CHAR("\033C0") }; ///< Select red color for background.
          character  ESC::BG_GREEN   [4] { A_CHAR("\033C1") }; ///< Select green color for background.
          character  ESC::BG_YELLOW  [4] { A_CHAR("\033C2") }; ///< Select yellow color for background.
          character  ESC::BG_BLUE    [4] { A_CHAR("\033C3") }; ///< Select blue color for background.
          character  ESC::BG_MAGENTA [4] { A_CHAR("\033C4") }; ///< Select blue color for background.
          character  ESC::BG_CYAN    [4] { A_CHAR("\033C5") }; ///< Select blue color for background.
          character  ESC::BG_BLACK   [4] { A_CHAR("\033C6") }; ///< Select red color for background.
          character  ESC::BG_WHITE   [4] { A_CHAR("\033C7") }; ///< Select blue color for background.
          character  ESC::BG_GRAY    [4] { A_CHAR("\033C8") }; ///< Select gray color for background.
          character  ESC::BG_RESET   [4] { A_CHAR("\033C9") }; ///< Select std color for background.

          character  ESC::BOLD       [4] { A_CHAR("\033sB") }; ///< Select bold font style.
          character  ESC::ITALICS    [4] { A_CHAR("\033sI") }; ///< Select italics font style.
          character  ESC::STYLE_RESET[4] { A_CHAR("\033sr") }; ///< Select standard font style.
          character  ESC::RESET      [4] { A_CHAR("\033sa") }; ///< Reset color and style.

          character  ESC::URL_START  [4] { A_CHAR("\033lS") }; ///< Mark the start of an URL.
          character  ESC::URL_END    [4] { A_CHAR("\033lE") }; ///< Mark the end of an URL.
          character  ESC::TAB        [4] { A_CHAR("\033t0") }; ///< Go to next tab. Usually, text loggers will increase the tab position automatically.

          character  ESC::EOMETA     [4] { A_CHAR("\033A0") }; ///< End of meta information in log string

#else
constexpr character  ESC::RED        [4];
constexpr character  ESC::GREEN      [4];
constexpr character  ESC::YELLOW     [4];
constexpr character  ESC::BLUE       [4];
constexpr character  ESC::MAGENTA    [4];
constexpr character  ESC::CYAN       [4];
constexpr character  ESC::BLACK      [4];
constexpr character  ESC::WHITE      [4];
constexpr character  ESC::GRAY       [4];
constexpr character  ESC::FG_RESET   [4];

constexpr character  ESC::BG_RED     [4];
constexpr character  ESC::BG_GREEN   [4];
constexpr character  ESC::BG_YELLOW  [4];
constexpr character  ESC::BG_BLUE    [4];
constexpr character  ESC::BG_MAGENTA [4];
constexpr character  ESC::BG_CYAN    [4];
constexpr character  ESC::BG_BLACK   [4];
constexpr character  ESC::BG_WHITE   [4];
constexpr character  ESC::BG_GRAY    [4];
constexpr character  ESC::BG_RESET   [4];

constexpr character  ESC::BOLD       [4];
constexpr character  ESC::ITALICS    [4];
constexpr character  ESC::STYLE_RESET[4];
constexpr character  ESC::RESET      [4];

constexpr character  ESC::URL_START  [4];
constexpr character  ESC::URL_END    [4];
constexpr character  ESC::TAB        [4];

constexpr character  ESC::EOMETA     [4];
#endif

void ESC::ReplaceToReadable( AString& target, integer startIdx )
{
    while( (startIdx= target.IndexOf( '\033', startIdx ) ) >= 0 )
    {
        String32 val("{ESC::");
        character c=  target.CharAt( startIdx + 1 );
        character c2= target.CharAt( startIdx + 2 );

        const character* code= A_CHAR("ERROR");

        // colors
        if( c == 'c' || c == 'C' )
        {
            if ( c == 'C' )
                val._<false>( A_CHAR("BG_") );
            switch( c2 - '0' )
            {
                case 0:  code= A_CHAR("RED")     ; break;
                case 1:  code= A_CHAR("GREEN")   ; break;
                case 2:  code= A_CHAR("YELLOW")  ; break;
                case 3:  code= A_CHAR("BLUE")    ; break;
                case 4:  code= A_CHAR("MAGENTA") ; break;
                case 5:  code= A_CHAR("CYAN")    ; break;
                case 6:  code= A_CHAR("BLACK")   ; break;
                case 7:  code= A_CHAR("WHITE")   ; break;
                case 8:  code= A_CHAR("GRAY")    ; break;
                case 9:  code= A_CHAR("RESET")   ; break;
                default: code= A_CHAR("COL_ERR"); break;
            }

        }

        // styles
        else if( c == 's' )
        {
            switch( c2 )
            {
                case 'B': code= A_CHAR("BOLD")         ; break;
                case 'I': code= A_CHAR("ITALICS")      ; break;
                case 'r': code= A_CHAR("STYLE_RESET")  ; break;
                case 'a': code= A_CHAR("RESET")        ; break;
                default:  code= A_CHAR("STYLE_ERR")    ; break;
            }
        }

        // styles
        else if( c == 'l' )
        {
            switch( c2 )
            {
                case 'S': code= A_CHAR("URL_START")    ; break;
                case 'E': code= A_CHAR("URL_END")      ; break;
                default:  code= A_CHAR("URL_ERR")      ; break;
            }
        }

        // others
        else if( c == 't' && c2 == '0' )    code= A_CHAR("TAB");
        else if( c == 'A' && c2 == '0' )    code= A_CHAR("EOMETA");

        // Replace
        val._<false>(code)._('}');
        target.ReplaceSubstring<false>( val, startIdx, 3 );
        startIdx+= 3;
    }
}

}}} // namespace [aworx::lib::lox]

#if !defined(ALIB_DOX)
namespace aworx { namespace lib { namespace strings {
void    T_Append<Scope,nchar>::operator()( TAString<nchar>& target, const lox::Scope src )
{
    Scope scope= src;
    int pathLevel= UnderlyingIntegral( scope - Scope::Path );
    if(pathLevel > 0 )
        scope= Scope::Path;

    target << "Scope::" << enums::GetRecord(scope).EnumElementName;

    if( pathLevel > 0 )
        target << '+' << pathLevel;
}

void T_Append<std::pair<Verbosity, Priorities>,nchar>::operator()( TAString<nchar>& target, const std::pair<Verbosity, Priorities>& src )
{
    target._( NFormat::Field( src.first, 7, Alignment::Left) );
    target._( '(' )._( src.second );
    target.InsertAt( ")", target.LastIndexOfAny<Inclusion::Exclude>( NDefaultWhitespaces() )  + 1 );
}
}}}
#endif // !defined(ALIB_DOX)

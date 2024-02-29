// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
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

#   if !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#      include "alib/lang/resources/resources.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_FORMAT)
#       include "alib/strings/format.hpp"
#   endif
#   if !defined(HPP_ALIB_CAMP_MESSAGE_REPORT)
#      include "alib/lang/message/report.hpp"
#   endif

ALIB_BOXING_VTABLE_DEFINE( alib::lox::Verbosity               , vt_lox_verbosity )
ALIB_BOXING_VTABLE_DEFINE( alib::lox::Scope                   , vt_lox_scope     )
ALIB_BOXING_VTABLE_DEFINE( alib::lox::detail::Logger*         , vt_lox_logger    )
ALIB_BOXING_VTABLE_DEFINE( std::pair<alib::lox::Verbosity
                          ALIB_COMMA alib::config::Priorities>, vt_lox_pair_verby_prio )

#endif // !defined(ALIB_DOX)

namespace alib {

lox::ALox ALOX;

/** ************************************************************************************************
 * This is the \b C++ version of <b>%ALox Logging Library</b>, which has been integrated
 * as one of many modules into the <b>ALib C++ Class Library</b>.<br>
 *
 * Please check out the \ref alib_mod_alox "documentation of ALib Module ALox" for more
 * information.
 **************************************************************************************************/
namespace lox {

#if ALIB_DEBUG && !defined(ALIB_DOX)
    namespace { integer dbgCheckQtyConfigPlugins; }
#endif


ALox::ALox()
: Camp( "ALOX" )
{
    ALIB_ASSERT_ERROR( this == &ALOX, "ALOX",
        "Instances of class ALox must not be created. Use singleton alib::ALOX" )
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
        List<Lox*>      loxes( &monomem::GlobalAllocator );
    }

#if ALOX_DBG_LOG
    Lox*                theDebugLox = nullptr;  // will be created in ALox::bootstrap
#endif

#endif


// The lox singletons for debug and release logging
Lox*     ALox::Get( const NString& name, lang::CreateIfNotExists create )
{
    ALIB_LOCK_WITH(monomem::GlobalAllocatorLock)

    // search
    for( auto* it : loxes )
        if( it->GetName().Equals<true, lang::Case::Ignore>( name ) )
            return it;


    // create?
    if ( create == lang::CreateIfNotExists::Yes )
    {
        Lox* newLox= new Lox ( name, false );
        loxes.EmplaceBack( newLox );
        return newLox;
    }

    // not found
    return nullptr;
}

void     ALox::Register( Lox* lox, lang::ContainerOp operation )
{
    ALIB_LOCK_WITH(monomem::GlobalAllocatorLock)

    // check
    if ( lox == nullptr )
    {
        ALIB_ERROR( "ALOX", "Nullptr given" )
        return;
    }

    // remove
    if( operation == lang::ContainerOp::Remove )
    {
        for( auto search= loxes.begin() ; search != loxes.end() ; ++search )
            if ( *search == lox )
            {
                loxes.Erase( search );
                return;
            }
        ALIB_WARNING( "ALOX", "Given lox named {!Q} could not be found for removal.",
                      lox != nullptr ? lox->GetName() : "<null>"          )
    }

    // insert
    else
    {
        for( auto* it : loxes )
            if( it->GetName().Equals<false>( lox->GetName() ) )
            {
                ALIB_ERROR( "ALOX", "Given lox named {!Q} was already registered. Registration ignored.",
                            lox->GetName() )
                return;
            }
        loxes.EmplaceBack( lox );
   }
}


// #################################################################################################
// ALox module initialization
// #################################################################################################

void  ALox::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_verbosity       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_scope           )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_logger          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_pair_verby_prio )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        "Var0" ,   A_CHAR( "1|ALOX|NO_IDE_LOGGER|"                           "||"  )  ,
        "Var1" ,   A_CHAR( "2|ALOX|CONSOLE_TYPE|"                            "||"  )  ,
        "Var2" ,   A_CHAR( "3|ALOX|%1_%2_VERBOSITY|"                      ";|=|1"  )  ,
        "Var3" ,   A_CHAR( "4|ALOX|GLOBAL_SOURCE_PATH_TRIM_RULES|"        ";|=|1"  )  ,
        "Var4" ,   A_CHAR( "5|ALOX|%1_SOURCE_PATH_TRIM_RULES|"            ";|=|1"  )  ,
        "Var5" ,   A_CHAR( "6|ALOX|%1_DOMAIN_SUBSTITUTION|"               ";|->|1" )  ,
        "Var6" ,   A_CHAR( "7|ALOX|%1_PREFIXES|"                          ";|=|1"  )  ,
        "Var7" ,   A_CHAR( "8|ALOX|%1_DUMP_STATE_ON_EXIT|"                  ",||"  )  ,
        "Var8" ,   A_CHAR("20|ALOX|%1_AUTO_SIZES|"                           "||"  )  ,
        "Var9" ,   A_CHAR("21|ALOX|%1_FORMAT|"                             ",||1"  )  ,
        "Var10",   A_CHAR("22|ALOX|%1_FORMAT_DATE_TIME|"                    ",||"  )  ,
        "Var11",   A_CHAR("23|ALOX|%1_FORMAT_MULTILINE|"                    ",||"  )  ,
        "Var12",   A_CHAR("24|ALOX|%1_FORMAT_TIME_DIFF|"                    ",||"  )  ,
        "Var13",   A_CHAR("25|ALOX|%1_MAX_ELAPSED_TIME|"                    ",||"  )  ,
        "Var14",   A_CHAR("26|ALOX|%1_REPLACEMENTS|"                        ",||"  )  ,
        "Var15",   A_CHAR("27|ALOX|CONSOLE_LIGHT_COLORS|"                    "||"  )  ,
        #if defined(_WIN32)
        "Var16",   A_CHAR("28|ALOX|CODEPAGE|"                                "||" )  ,
        #endif

        // configuration variable default values
        "Var_D1",       A_CHAR("false"),
        "Var_D2",       A_CHAR("default"),
        "Var_D3",       A_CHAR("writeback"),
        "Var_D8",       A_CHAR("none, verbosity=info, domain=/ALOX"),

        "Var_D25",      A_CHAR("0, limit=59"),
        #if defined(_WIN32)
        "Var_D28",      A_CHAR("65001"),
        #endif

        // configuration variable comments
        "Var_C1",   A_CHAR("If true, the creation of an additional, ide-specific debug logger is suppressed." "\n"
                           "(In particular suppresses DebugLogger (C#) and VStudioLogger (C++))" ),

        "Var_C2",   A_CHAR("Influences the type of console logger to be created by method"      "\n"
                           "Lox::CreateConsoleLogger which is also used by Log::AddDebugLogger" "\n"
                           "Possible values are: default, plain, ansi, windows, noqtcreator"         ),

        "Var_C3",   A_CHAR("The verbosities of logger \"%2\" in lox \"%1\". Use 'writeback [VAR_NAME] ;'"   "\n"
                           "to enable automatic writing on application exit."                  ),

        "Var_C4",   A_CHAR("Defines global source path trim rules (applicable for all Lox instances)."        "\n"
                           "   Format: [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; \u2026 ]" ),

        "Var_C5",   A_CHAR("Defines source path trim rules for Lox \"%1\". "           "\n"
                           "   Format: [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] [ ; \u2026 ]" ),

        "Var_C7",   A_CHAR("Prefix strings for log domains of lox \"%1\".\n"
                           "   Format: [*]domainpath[*] = prefixstring [, inclusion] [ ; \u2026 ] " ),

        "Var_C8",   A_CHAR("Log information about lox \"%1\" on exit. Comma separated list of arguments define" "\n"
                           "verbosity, domain and content of output. Possible values content arguments are:"    "\n"
                           "  All, " "Basic, " "Version, " "SPTR, " "Loggers, " "Domains, " "InternalDomains"   "\n"
                           "  ScopeDomains, " "DSR, " "PrefixLogables" "Once, " "LogData, " "ThreadMappings, "  "\n"
                           "  CompilationFlags." " If NONE is given nothing is dumped." ),

        "Var_C20",  A_CHAR("Auto size values of last run of Logger '%1' (generated and temporary values)."),

        "Var_C21",  A_CHAR("Meta info format of text logger \"%1\", including signatures for verbosity strings and" "\n"
                           "an optional string added to the end of each log statement."                             "\n"
                           "   Format: metaInfoFormat [, Error [, Warning [, Info [, Verbose [, MsgSuffix ]]]]]"),

        "Var_C22",  A_CHAR("Meta info date and time format of text logger \")%1\"."                         "\n"
                           "   Format: DateFormat [, TimeOfDayFormat [, TimeElapsedDays ]]]"),

        "Var_C23",  A_CHAR("Multi-line format of text logger \"%1\"."       "\n"
                           "   Format: MultiLineMsgMode [, FmtMultiLineMsgHeadline [, FmtMultiLinePrefix [, FmtMultiLineSuffix\n"
                           "           [, MultiLineDelimiter [, MultiLineDelimiterRepl ]]]]]"),

        "Var_C24",  A_CHAR("Meta info time difference entities of text logger \"%1\"."       "\n"
                           "   Format: TimeDiffMinimum [, TimeDiffNone [, TimeDiffNanos [, TimeDiffMicros [, TimeDiffMillis\n"
                           "           [, TimeDiffSecs [, TimeDiffMins [, TimeDiffHours [,  TimeDiffDays  ]]]]]]]]"),

        "Var_C25",  A_CHAR("Maximum elapsed time of all runs of Logger '%1'. To reset elapsed time display""\n"
                           "width, set this to 0 manually. Generated and temporary value.)" ),

        "Var_C26",  A_CHAR("Pairs of search and replacement strings for text logger \"%1\"."               "\n"
                           "   Format: search, replacement [, search, replacement] [,...]"),

        "Var_C27",  A_CHAR("Evaluated by colorful loggers that dispose about light and dark colors. Those may"  "\n"
                           "adjust their foreground and background color accordingly. If not given, under Windows OS" "\n"
                           "the right value is detected. Otherwise the value defaults to \"foreground\". In some"     "\n"
                           "occasions, the (detected or set) runtime environment might also indicate a different"     "\n"
                           "default value.  Possible values are 'foreground', 'background' and 'never'."),

        #if defined(_WIN32)
        "Var_C28",   A_CHAR("Code page used by class WindowsConsoleLogger. Defaults to 65001."           "\n"
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
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Verbosity)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Scope)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::detail::Logger*)
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
            monomem::AcquireGlobalAllocator( ALIB_CALLER_PRUNED );
                theDebugLox= ga.Emplace<Lox>( "LOG");
            monomem::ReleaseGlobalAllocator();

            dbgCheckQtyConfigPlugins= config->CountPlugins();
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
            detail::LI::Destruct( loxes.Back() );
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

    ALIB_ASSERT_ERROR(loxes.Size()           == 0, "ALOX", "A Lox remained from the last test" )
    ALIB_ASSERT_ERROR(config->CountPlugins() == dbgCheckQtyConfigPlugins,
                                                   "ALOX", "A config plug-in remained from the last test" )

    dynamic_cast<InMemoryPlugin*>( config->GetPlugin( config::Priorities::DefaultValues  ) )->Clear();
    dynamic_cast<InMemoryPlugin*>( config->GetPlugin( config::Priorities::ProtectedValues) )->Clear();

    #if ALOX_DBG_LOG
        new ( theDebugLox) Lox( "LOG");
    #endif
}

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

}} // namespace [alib::lox]

#if !defined(ALIB_DOX)
namespace alib {  namespace strings {
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
    target._( NFormat::Field( src.first, 7, lang::Alignment::Left) );
    target._( '(' )._( src.second );
    target.InsertAt( ")", target.LastIndexOfAny<lang::Inclusion::Exclude>( NDefaultWhitespaces() )  + 1 );
}
}}
#endif // !defined(ALIB_DOX)

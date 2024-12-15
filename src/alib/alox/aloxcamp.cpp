// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !DOXYGEN
#   include "alib/alox/aloxcamp.hpp"
#   include "alib/alox/alox.hpp"
#   include "alib/alox/detail/logger.hpp"
#   include "alib/alox/textlogger/textlogger.hpp"

#   include "alib/lang/basecamp/basecamp.hpp"
#   include "alib/lang/basecamp/camp_inlines.hpp"
#   include "alib/enums/serialization.hpp"
#   include "alib/lang/resources/resources.hpp"
#   include "alib/enums/recordbootstrap.hpp"
#   include "alib/lang/message/report.hpp"
#   include "alib/containers/list.hpp"
#   include "alib/monomem/localallocator.hpp"

    ALIB_BOXING_VTABLE_DEFINE( alib::lox::Verbosity               , vt_lox_verbosity )
    ALIB_BOXING_VTABLE_DEFINE( alib::lox::Scope                   , vt_lox_scope     )
    ALIB_BOXING_VTABLE_DEFINE( alib::lox::detail::Logger*         , vt_lox_logger    )
    ALIB_BOXING_VTABLE_DEFINE( std::pair<alib::lox::Verbosity
                              ALIB_COMMA alib::config::Priority>, vt_lox_pair_verby_prio )
#endif // !DOXYGEN

namespace alib {

lox::ALoxCamp ALOX;

//==================================================================================================
/// This is the \b C++ version of <b>%ALox Logging Library</b>, which has been integrated
/// as one of many modules into the <b>ALib C++ Class Library</b>.<br>
///
/// Please check out the \ref alib_mod_alox "documentation of ALib Module ALox" for more
/// information.
//==================================================================================================
namespace lox {



ALoxCamp::ALoxCamp()
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
#if !DOXYGEN
    namespace
    {
        List<MonoAllocator, Lox*>      loxes(monomem::GLOBAL_ALLOCATOR);
    }

#if ALOX_DBG_LOG
    Lox*                theDebugLox = nullptr;  // will be created in ALoxCamp::Bootstrap
#endif

#endif


// The lox singletons for debug and release logging
Lox*     ALoxCamp::Get( const NString& name, lang::CreateIfNotExists create )
{
    ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)

    // search
    for( auto* it : loxes )
        if( it->GetName().Equals<CHK, lang::Case::Ignore>( name ) )
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

void     ALoxCamp::Register( Lox* lox, lang::ContainerOp operation )
{
    ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)

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
                (void) loxes.Erase( search );
                return;
            }
        ALIB_WARNING( "ALOX", "Given lox named {!Q} could not be found for removal.",
                      lox != nullptr ? lox->GetName() : "<null>"          )
    }

    // insert
    else
    {
        for( auto* it : loxes )
            if( it->GetName().Equals<NC>( lox->GetName() ) )
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

void  ALoxCamp::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_verbosity       )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_scope           )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_logger          )
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_lox_pair_verby_prio )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

        "Var0" ,   A_CHAR( "1|ALOX/NO_IDE_LOGGER|"                        "B"       )  ,
        "Var1" ,   A_CHAR( "2|ALOX/CONSOLE_TYPE|"                         "S"       )  ,
DOX_MARKER([DOX_CONFIG_REPLACEMENTS1])
"Var2",  A_CHAR( "3|ALOX/%2/VERBOSITY_WITH_%1|ALOXV"),
DOX_MARKER([DOX_CONFIG_REPLACEMENTS1])
        "Var3" ,   A_CHAR( "4|ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES|"        "S"       )  ,
        "Var4" ,   A_CHAR( "5|ALOX/%1/SOURCE_PATH_TRIM_RULES|"            "S"       )  ,
        "Var5" ,   A_CHAR( "6|ALOX/%1/DOMAIN_SUBSTITUTION|"               "SV;"     )  ,
        "Var6" ,   A_CHAR( "7|ALOX/%1/PREFIXES|"                          "S"       )  ,
        "Var7" ,   A_CHAR( "8|ALOX/%1/DUMP_STATE_ON_EXIT|"                "S"       )  ,
        "Var8" ,   A_CHAR("20|ALOX/%1/AUTO_SIZES|"                        "ALOXAS"  )  ,
        "Var9" ,   A_CHAR("21|ALOX/%1/FORMAT|"                            "ALOXFMI" )  ,
        "Var10",   A_CHAR("22|ALOX/%1/FORMAT_DATE_TIME|"                  "ALOXFDT" )  ,
        "Var11",   A_CHAR("23|ALOX/%1/FORMAT_MULTILINE|"                  "ALOXFML" )  ,
        "Var12",   A_CHAR("24|ALOX/%1/FORMAT_TIME_DIFF|"                  "ALOXFTD" )  ,
        "Var13",   A_CHAR("25|ALOX/%1/FORMAT_OTHER|"                      "ALOXFO"  )  ,
        "Var14",   A_CHAR("26|ALOX/%1/REPLACEMENTS|"                      "ALOXSR"  )  ,
        "Var15",   A_CHAR("27|ALOX/CONSOLE_LIGHT_COLORS|"                 "ALOXCLP" )  ,
        "Var16",   A_CHAR("28|ALOX/CODEPAGE|"                             "I"       )  ,

        // configuration variable default values
        "Var_D1",       A_CHAR("false"),
        "Var_D2",       A_CHAR("default"),
        "Var_D8",       A_CHAR("none, verbosity=info, domain=/ALOX"),


// standard textlogger format string
#if ALOX_DBG_LOG_CI || ALOX_REL_LOG_CI
        "Var_D21",      A_CHAR("%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%# %V: ")
#else
        "Var_D21",      A_CHAR("[%TC +%TL][%tN][%D]%A1#%# %V: ")
#endif
                        A_CHAR(",[ERR],[WRN],     ,[***]")           ,

// ANSI textlogger format string
#if ALOX_DBG_LOG_CI || ALOX_REL_LOG_CI
        "Var_D21A",      A_CHAR("%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%#: %V")
#else
        "Var_D21A",      A_CHAR("[%TC +%TL][%tN][%D]%A1#%#:%V")
#endif
                         A_CHAR(   ",\033c0,\033c3,,\033c8,\033[0m"  )           ,

        "Var_D22",       A_CHAR("yyyy-MM-dd,HH:mm:ss, Days ")                      ,
        "Var_D23",       A_CHAR("2,ALox: Multi line message follows: ,> ,,nulled,\\r")   ,


        "Var_D24",       A_CHAR("1000"  ",---   "  ", ns"  ", \u00B5s" )
                         A_CHAR(", ms" ", s"  ", m"  ", h"  ", days"   )    ,

        "Var_D25",       A_CHAR("---, ---, 3"),
        #if defined(_WIN32)
        "Var_D28",      A_CHAR("65001"),
        #endif

        // configuration variable comments
        "Var_C1",   A_CHAR("If true, the creation of an additional, ide-specific debug logger is suppressed." "\n"
                           "(In particular suppresses DebugLogger (C#) and VStudioLogger (C++))" ),

        "Var_C2",   A_CHAR("Influences the type of console logger to be created by method"      "\n"
                           "Lox::CreateConsoleLogger which is also used by Log::AddDebugLogger" "\n"
                           "Possible values are: default, plain, ansi, windows, noqtcreator"         ),

        "Var_C3",   A_CHAR("The verbosities of logger \"%2\" in lox \"%1\". Supports wildcards for domain paths."   "\n"
                           "  Format: [ExportAll ;] [[*]domainpath[*] = Verbosity]  [;...] "                  ),

        "Var_C4",   A_CHAR("Defines global source path trim rules (applicable for all Lox instances)."        "\n"
                           "   Format: [ [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] ] [;...]" ),

        "Var_C5",   A_CHAR("Defines source path trim rules for Lox \"%1\". "           "\n"
                           "   Format: [ [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] ] [;...]" ),

        "Var_C7",   A_CHAR("Prefix strings for log domains of lox \"%1\".\n"
                           "   Format: [ [*]domainpath[*] = prefixstring [, inclusion] ] [;...] " ),

        "Var_C8",   A_CHAR("Log information about lox \"%1\" on exit. Comma separated list of arguments define" "\n"
                           "verbosity, domain and content of output. Possible values content arguments are:"    "\n"
                           "  All, " "Basic, " "Version, " "SPTR, " "Loggers, " "Domains, " "InternalDomains"   "\n"
                           "  ScopeDomains, " "DSR, " "PrefixLogables" "Once, " "LogData, " "ThreadMappings, "  "\n"
                           "  CompilationFlags." " If NONE is given nothing is dumped." ),

        "Var_C20",  A_CHAR("Detected field sizes and tabulator positions of the meta-information portion and the " "\n"
                           "those of the log output, separated by ';'. (These is a generated and temporary values)."),

        "Var_C21",  A_CHAR("Meta info format of logger \"%1\", including signatures for verbosity strings and" "\n"
                           "astring added to the end of each log statement."                             "\n"
                           "   Format: MetaInfo,Error,Warning,Info,Verbose,MsgSuffix"),

        "Var_C22",  A_CHAR("Date and time format of logger \"%1\"."                         "\n"
                           "   Format: Date,Time,ElapsedDays"),

        "Var_C23",  A_CHAR("Multi-line format of logger \"%1\"."       "\n"
                           "   Format: Mode,Headline,Prefix,Suffix,Delimiter,DelimiterReplacement"),

        "Var_C24",  A_CHAR("Time difference entities of logger \"%1\"."       "\n"
                           "   Format: Minimum,None,Nanos,Micros,Millis,Secs,Mins,Hours, Days"),

        "Var_C25",  A_CHAR("Specifies three further format values: 1. A replacement string if no file name is available, \n"
                           "2. A replacement string if no method name is available and 3. The minimum digits of the\n"
                           "log counter."                      ),

        "Var_C26",  A_CHAR("Pairs of search and replacement strings for text logger \"%1\"."               "\n"
                           "   Format: [search,replacement] [,...]"),

        "Var_C27",  A_CHAR("Evaluated by colorful loggers that dispose about light and dark colors. Those may adjust"  "\n"
                           "their foreground and background color accordingly. If not given, under Windows OS the right" "\n"
                           "value is detected. Otherwise the value defaults to \"foreground\". In some occasions, the"     "\n"
                           "(detected or set) runtime environment might also indicate a different default value."     "\n"
                           "Possible values are 'Auto', Foreground', 'Background' and 'Never'."),

        #if defined(_WIN32)
        "Var_C28",  A_CHAR("Code page used by class WindowsConsoleLogger. Defaults to 65001."           "\n"
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

        "LCU",    A_CHAR( "0,Auto,1,"
                          "1,Never,1,"
                          "2,Foreground,2,"
                          "3,Background,2,"
                          "2,ForeGround,1,"
                          "3,BackGround,1"  ),

        //#####################################    Various    ######################################
        "VVEA",               A_CHAR("ExportAll" ), // Variable VERBOSITIES keyword 'ExportAll'
        "TLFmtExc",           A_CHAR("\nAn exception occurred during formatting ALox logables:\n" ),

        "INI_CMT_ALOX",              A_CHAR("@>'/// '@HL-Settings controlling ALox log output.\n@HL-"),
#if ALIB_DEBUG
        "INI_CMT_ALOX/LOG",          A_CHAR("@>'/// '@HL-Settings of the Lox-instance used with debug-logging.\n@HL-"),
        "INI_CMT_ALOX/DEBUG_LOGGER", A_CHAR("@>'/// '@HL-Settings of the debug-logger.\n@HL-"),
#endif

        // end of BootstrapBulk()
        nullptr  );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS

        // Add box-functions
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Verbosity)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::Scope)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(alib::lox::detail::Logger*)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE_N(std::pair<Verbosity ALIB_COMMA Priority>)

    } // BootstrapPhases::PrepareResources

DOX_MARKER([DOX_CONFIG_DEFINETYPE3])
//...
//...
else if( phase == BootstrapPhases::PrepareConfig )
{
    ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatMetaInfo )
    //...
    //...
DOX_MARKER([DOX_CONFIG_DEFINETYPE3])
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( CVVerbosities            )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatDateTime           )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatTimeDiff           )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatMultiLine          )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatOther              )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( FormatAutoSizes          )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( Replacements             )
        ALIB_CONFIG_VARIABLE_REGISTER_TYPE( ColorfulLoggerParameters )

        // Parse enum records
        EnumRecords<Verbosity                   >::Bootstrap( *this, "Verbosity"        );
        EnumRecords<Scope                       >::Bootstrap( *this, "Scope"            );
        EnumRecords<StateInfo                   >::Bootstrap( *this, "StateInfo"        );
        EnumRecords<textlogger::
                    ColorfulLoggerParameters::
                    LightColorUsage             >::Bootstrap( *this, "LCU"  );

        EnumRecords<Variables                   >::Bootstrap( '|' );

        // preload all variable with declarations (and without placeholders)
        config->PreloadVariables<lox::Variables>();
    }

    else if( phase == BootstrapPhases::Final )
    {
        #if ALOX_DBG_LOG
            if ( !theDebugLox )
                theDebugLox=monomem::GLOBAL_ALLOCATOR().New<Lox>("LOG");
        #endif
    }
}


void ALoxCamp::shutdown( ShutdownPhases phase )
{
    (void) phase;
    #if ALOX_DBG_LOG
    if( phase == ShutdownPhases::Destruct )
    {
        if ( Log::DebugLogger != nullptr )
            Log_RemoveDebugLogger()

        if ( !theDebugLox )
            lang::Destruct( *theDebugLox );

        while ( loxes.IsNotEmpty() )
            detail::LI::Destruct( loxes.Back() );
        loxes.Reset();
    }
    #endif
}


void  ALoxCamp::Reset()
{
    #if ALOX_DBG_LOG
        if (Log::DebugLogger != nullptr )
            Log_RemoveDebugLogger()

        lang::Destruct(*theDebugLox);
    #endif

    ALIB_ASSERT_ERROR(loxes.Count() == 0, "ALOX", "A Lox remained from the last test" )

    #if ALOX_DBG_LOG
        new (theDebugLox) Lox( "LOG" );
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
                val._<NC>( A_CHAR("BG_") );
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
        val._<NC>(code)._('}');
        target.ReplaceSubstring<NC>( val, startIdx, 3 );
        startIdx+= 3;
    }
}

}} // namespace [alib::lox]

#if !DOXYGEN
namespace alib {  namespace strings {
void    T_Append<Scope,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const lox::Scope src )
{
    Scope scope= src;
    int pathLevel= UnderlyingIntegral( scope - Scope::Path );
    if(pathLevel > 0 )
        scope= Scope::Path;

    target << "Scope::" << enums::GetRecord(scope).EnumElementName;

    if( pathLevel > 0 )
        target << '+' << pathLevel;
}

void T_Append<std::pair<Verbosity, Priority>,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const std::pair<Verbosity, Priority>& src )
{
    target._( NFormat::Field( src.first, 7, lang::Alignment::Left) );
    target._( '(' )._( src.second );
    target.InsertAt( ")", target.LastIndexOfAny<lang::Inclusion::Exclude>( NDEFAULT_WHITESPACES )  + 1 );
}
}}

// CVVerbosities
namespace alib::config::detail {
ALIB_API void  VMeta_CVVerbosities::imPort(VDATA* data, Configuration&, const StringEscaper& esc, const String& src)
{
    auto& cvVerbosities= data->As<alib::lox::CVVerbosities>();
    auto& exportAllKeyWord= alib::ALOX.GetResource("VVEA");
    cvVerbosities.ExportAll= false;

    LocalAllocator4K la;
    StringVectorMA results(la);
    esc.UnescapeTokens(results, src, A_CHAR(";"));
    cvVerbosities.Clear();
    for( auto it : results )
    {
        Substring value= it;
        value.ConsumeChar('\n');
        value.ConsumeChar('\r');
        value.ConsumeCharFromEnd('\n');
        value.ConsumeCharFromEnd('\r');

        if( value.Equals<CHK, lang::Case::Ignore>(exportAllKeyWord) )
        {
            cvVerbosities.ExportAll= true;
            continue;
        }
        cvVerbosities.Add( value );
    }
}

ALIB_API void  VMeta_CVVerbosities::exPort(VDATA* data, Configuration&, const StringEscaper& esc, AString& dest)
{
    auto& cvVerbosities= data->As<alib::lox::CVVerbosities>();
    auto& exportAllKeyWord= alib::ALOX.GetResource("VVEA");
    if( cvVerbosities.ExportAll)
        dest << exportAllKeyWord << " ;" << NEW_LINE;

    if( !cvVerbosities.IsEmpty()) // well, this can only be empty if a user makes mistakes
        for( auto it= cvVerbosities.begin() ;; )
        {
            esc.Escape(*it, dest, A_CHAR(";"));

            if( ++it == cvVerbosities.end() )
                break;

            dest << " ;";
            dest << NEW_LINE;
        }
}
} //namespace [alib::config::detail]

#endif // !DOXYGEN


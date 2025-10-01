// #################################################################################################
//  alib::lox::detail - ALox Logging Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/variables/variables.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
#include "alib/alox/alox.prepro.hpp"

#include "ALib.Strings.Vector.H"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.ALox.Impl;
    import   ALib.ALox;
    import   ALib.Lang;
    import   ALib.EnumOps;
    import   ALib.Containers.List;
    import   ALib.Strings;
    import   ALib.Boxing;
    import   ALib.EnumRecords;
    import   ALib.EnumRecords.Bootstrap;
    import   ALib.Variables;
    import   ALib.Camp;
    import   ALib.Camp.Base;
#else
#   include "ALib.Lang.H"
#   include "ALib.ALox.H"
#   include "ALib.Containers.List.H"
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.EnumRecords.Bootstrap.H"
#   include "ALib.Variables.H"
#   include "ALib.Camp.H"
#   include "ALib.Camp.Base.H"
#   include "ALib.ALox.H"
#   include "ALib.ALox.Impl.H"
#endif
// ======================================   Implementation   =======================================


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
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( this == &ALOX, "ALOX",
          "Instances of class ALox must not be created. Use singleton alib::ALOX" )
    #endif
}

void  ALoxCamp::Reset()
{
    #if ALOX_DBG_LOG
        if (Log::DebugLogger != nullptr )
            Log_RemoveDebugLogger()

        lang::Destruct(*DEBUG_LOX);
    #endif

    ALIB_ASSERT_ERROR(detail::dbgCountLoxes() == 0, "ALOX", "A Lox remained from the last test" )

    #if ALOX_DBG_LOG
        new (DEBUG_LOX) Lox( "LOG" );
    #endif
}

// #################################################################################################
// Compilation Flags
// #################################################################################################

// check the compiler-symbols, give warning once (therefore not in HPP)
#if !ALOX_DBG_LOG && ALOX_DBG_LOG_CI
#   pragma message ( "Warning: ALox compiler-symbol mismatch: ALOX_DBG_LOG_CI is true, while ALOX_DBG_LOG is false." )
#endif
#if !ALOX_REL_LOG && ALOX_REL_LOG_CI
#   pragma message ( "Warning: ALox compiler-symbol mismatch: ALOX_REL_LOG_CI is true, while ALOX_REL_LOG is false" )
#endif

// #################################################################################################
// ALox module initialization
// #################################################################################################

void  ALoxCamp::Bootstrap()
{
    if( GetBootstrapState() == BootstrapPhases::PrepareResources )
    {
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
        resourcePool->BootstrapBulk( ResourceCategory,

        "Var0" ,   A_CHAR( "1|ALOX/NO_IDE_LOGGER|"                        "B"       )  ,
        "Var1" ,   A_CHAR( "2|ALOX/CONSOLE_TYPE|"                         "S"       )  ,
DOX_MARKER([DOX_VARIABLES_REPLACEMENTS1])
"Var2",  A_CHAR( "3|ALOX/%2/VERBOSITY_WITH_%1|ALOXV"),
DOX_MARKER([DOX_VARIABLES_REPLACEMENTS1])
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

        "Var_C27",  A_CHAR("Evaluated by colorful loggers that dispose of light and dark colors. Those may adjust"  "\n"
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
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

    } // BootstrapPhases::PrepareResources

DOX_MARKER([DOX_VARIABLES_DEFINETYPE3])
//...
//...
else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )
{
    ALIB_VARIABLES_REGISTER_TYPE( FormatMetaInfo )
    //...
    //...
DOX_MARKER([DOX_VARIABLES_DEFINETYPE3])
        ALIB_VARIABLES_REGISTER_TYPE( CVVerbosities            )
        ALIB_VARIABLES_REGISTER_TYPE( FormatDateTime           )
        ALIB_VARIABLES_REGISTER_TYPE( FormatTimeDiff           )
        ALIB_VARIABLES_REGISTER_TYPE( FormatMultiLine          )
        ALIB_VARIABLES_REGISTER_TYPE( FormatOther              )
        ALIB_VARIABLES_REGISTER_TYPE( FormatAutoSizes          )
        ALIB_VARIABLES_REGISTER_TYPE( Replacements             )
        ALIB_VARIABLES_REGISTER_TYPE( ColorfulLoggerParameters )

        // Parse enum records
        enumrecords::bootstrap::Bootstrap<Verbosity   >( *this, "Verbosity"        );
        enumrecords::bootstrap::Bootstrap<Scope       >( *this, "Scope"            );
        enumrecords::bootstrap::Bootstrap<StateInfo   >( *this, "StateInfo"        );
        enumrecords::bootstrap::Bootstrap<textlogger::
                                    ColorfulLoggerParameters::
                                    LightColorUsage             >( *this, "LCU"  );

        enumrecords::bootstrap::Bootstrap<Variables                   >( '|' );

        // preload all variable with declarations (and without placeholders)
        config->PreloadVariables<lox::Variables>();
    }

    else if( GetBootstrapState() == BootstrapPhases::Final )
    {
        #if ALOX_DBG_LOG
            if ( !DEBUG_LOX )
                DEBUG_LOX=monomem::GLOBAL_ALLOCATOR().New<Lox>("LOG");
        #endif
    }
}


void ALoxCamp::Shutdown( ShutdownPhases phase )
{
    (void) phase;
    #if ALOX_DBG_LOG
    if( phase == ShutdownPhases::Destruct )
    {
        if ( Log::DebugLogger != nullptr )
            Log_RemoveDebugLogger()

        if ( DEBUG_LOX )
            lang::Destruct( *DEBUG_LOX );

        detail::shutdownLoxes();
    }
    #endif
}


}} // namespace [alib::lox]

#if !DOXYGEN
namespace alib {  namespace strings {
void    AppendableTraits<Scope,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const lox::Scope src )
{
    Scope scope= src;
    int pathLevel= int( scope - Scope::Path );
    if(pathLevel > 0 )
        scope= Scope::Path;

    target << "Scope::" << enumrecords::GetRecord(scope).EnumElementName;

    if( pathLevel > 0 )
        target << '+' << pathLevel;
}

void AppendableTraits<Pair<Verbosity, Priority>,nchar, lang::HeapAllocator>::operator()( TAString<nchar, lang::HeapAllocator>& target, const Pair<Verbosity, Priority>& src )
{
    target._( NField( src.First, 7, lang::Alignment::Left) );
    target._( '(' )._( src.Second );
    target.InsertAt( ")", target.LastIndexOfAny<lang::Inclusion::Exclude>( NDEFAULT_WHITESPACES )  + 1 );
}
}}

// CVVerbosities
namespace alib::variables::detail {
ALIB_DLL void  VMeta_CVVerbosities::imPort(VDATA* data, Configuration&, const StringEscaper& esc, const String& src)
{
    auto& cvVerbosities= data->As<alib::lox::CVVerbosities>();
    auto& exportAllKeyWord= alib::ALOX.GetResource("VVEA");
    cvVerbosities.ExportAll= false;

    LocalAllocator4K la;
    StringVectorMA results(la);
    esc.UnescapeTokens(results, src, A_CHAR(";"));
    cvVerbosities.Clear();
    for( auto& it : results )
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

ALIB_DLL void  VMeta_CVVerbosities::exPort(VDATA* data, Configuration&, const StringEscaper& esc, AString& dest)
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
} //namespace [alib::variables::detail]

#endif // !DOXYGEN


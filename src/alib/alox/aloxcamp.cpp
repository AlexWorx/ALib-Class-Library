

namespace alib {

lox::ALoxCamp ALOX;

//==================================================================================================
/// This is the \b C++ version of <b>ALox Logging Library</b>, which has been integrated
/// as one of many modules into the <b>ALib C++ Framework</b>.<br>
///
/// Please check out the #"alib_mod_alox;documentation of ALib Module ALox" for more
/// information.
//==================================================================================================
namespace lox {

#if !DOXYGEN
namespace {
void bulkloadResources(ResourcePool& resourcePool, const NCString& resourceCategory) {
    resourcePool.BootstrapBulk( resourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "Var0"             , A_CHAR("1|ALOX/NO_IDE_LOGGER|B"),
    "Var1"             , A_CHAR("2|ALOX/CONSOLE_TYPE|S"),
    "Var2"             , A_CHAR("3|ALOX/%2/VERBOSITY_WITH_%1|ALOXV"),
    "Var3"             , A_CHAR("4|ALOX/GLOBAL_SOURCE_PATH_TRIM_RULES|S"),
    "Var4"             , A_CHAR("5|ALOX/%1/SOURCE_PATH_TRIM_RULES|S"),
    "Var5"             , A_CHAR("6|ALOX/%1/DOMAIN_SUBSTITUTION|SV;"),
    "Var6"             , A_CHAR("7|ALOX/%1/PREFIXES|S"),
    "Var7"             , A_CHAR("8|ALOX/%1/DUMP_STATE_ON_EXIT|S"),
    "Var8"             , A_CHAR("20|ALOX/%1/AUTO_SIZES|ALOXAS"),
    "Var9"             , A_CHAR("21|ALOX/%1/FORMAT|ALOXFMI"),
    "Var10"            , A_CHAR("22|ALOX/%1/FORMAT_DATE_TIME|ALOXFDT"),
    "Var11"            , A_CHAR("23|ALOX/%1/FORMAT_MULTILINE|ALOXFML"),
    "Var12"            , A_CHAR("24|ALOX/%1/FORMAT_TIME_DIFF|ALOXFTD"),
    "Var13"            , A_CHAR("25|ALOX/%1/FORMAT_OTHER|ALOXFO"),
    "Var14"            , A_CHAR("26|ALOX/%1/REPLACEMENTS|ALOXSR"),
    "Var15"            , A_CHAR("27|ALOX/CONSOLE_LIGHT_COLORS|ALOXCLP"),
    "Var16"            , A_CHAR("28|ALOX/CODEPAGE|I"),
    "Var_D1"           , A_CHAR("false"),
    "Var_D2"           , A_CHAR("default"),
    "Var_D8"           , A_CHAR("none, verbosity=info, domain=/ALOX"),
    "Var_D22"          , A_CHAR("yyyy-MM-dd,HH:mm:ss, Days"),
    "Var_D23"          , A_CHAR("2,ALox: Multi-line message follows: ,> ,,nulled,\\r"),
    "Var_D24"          , A_CHAR("1000,---   , ns, \u03BCs, ms, s, m, h, days"),
    "Var_D25"          , A_CHAR("---, ---, 3"),
    "Var_C1"           , A_CHAR("If true, the creation of an additional, ide-specific debug logger is suppressed."),
    "Var_C2"           , A_CHAR("Influences the type of console logger to be created by method\n")
                         A_CHAR("Lox::CreateConsoleLogger which is also used by Log::AddDebugLogger\n")
                         A_CHAR("Possible values are: default, plain, ansi, and windows."),
    "Var_C3"           , A_CHAR("The verbosities of logger %2 in lox %1. Supports wildcards for domain paths.\n")
                         A_CHAR("Format: [ExportAll ;] [[*]domainpath[*] = Verbosity]  [;...]"),
    "Var_C4"           , A_CHAR("Defines global source path trim rules (applicable for all Lox instances).\n")
                         A_CHAR("Format: [ [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] ] [;...]"),
    "Var_C5"           , A_CHAR("Defines source path trim rules for Lox %1.\n")
                         A_CHAR("Format: [ [*]sourcepath [, inclusion, trimoffset, sensitivity, replacement] ] [;...]"),
    "Var_C7"           , A_CHAR("Prefix strings for log domains of lox %1.\n")
                         A_CHAR("Format: [ [*]domainpath[*] = prefixstring [, inclusion] ] [;...]"),
    "Var_C8"           , A_CHAR("Log information about lox %1 on exit. Comma separated list of arguments define\n")
                         A_CHAR("verbosity, domain and content of output. Possible values content arguments are:\n")
                         A_CHAR("All,  Basic,  Version,  SPTR,  Loggers,  Domains,  InternalDomains\n")
                         A_CHAR("ScopeDomains,  DSR,  PrefixLogables Once,  LogData,  ThreadMappings,\n")
                         A_CHAR("CompilationFlags.  If NONE is given nothing is dumped."),
    "Var_C20"          , A_CHAR("Detected field sizes and tabulator positions of the meta-information portion and the\n")
                         A_CHAR("those of the log output, separated by ';'. (These is a generated and temporary values)."),
    "Var_C21"          , A_CHAR("Meta info format of logger %1, including signatures for verbosity strings and\n")
                         A_CHAR("a string added to the end of each log statement.\n")
                         A_CHAR("Format: MetaInfo,Error,Warning,Info,Verbose,MsgSuffix"),
    "Var_C22"          , A_CHAR("Date and time format of logger %1.\n")
                         A_CHAR("Format: Date,Time,ElapsedDays"),
    "Var_C23"          , A_CHAR("Multi-line format of logger %1.\n")
                         A_CHAR("Format: Mode,Headline,Prefix,Suffix,Delimiter,DelimiterReplacement"),
    "Var_C24"          , A_CHAR("Time difference entities of logger %1.\n")
                         A_CHAR("Format: Minimum,None,Nanos,Micros,Millis,Secs,Mins,Hours, Days"),
    "Var_C25"          , A_CHAR("Specifies three further format values: 1. A replacement string if no file name is available,\n")
                         A_CHAR("2. A replacement string if no method name is available and 3. The minimum digits of the\n")
                         A_CHAR("log counter."),
    "Var_C26"          , A_CHAR("Pairs of search and replacement strings for text logger %1.\n")
                         A_CHAR("Format: [search,replacement] [,...]"),
    "Var_C27"          , A_CHAR("Evaluated by colorful loggers that dispose of light and dark colors. Those may adjust\n")
                         A_CHAR("their foreground and background color accordingly. If not given, under Windows OS the right\n")
                         A_CHAR("value is detected. Otherwise the value defaults to \foreground. In some occasions, the\n")
                         A_CHAR("(detected or set) runtime environment might also indicate a different default value.\n")
                         A_CHAR("Possible values are 'Auto', Foreground', 'Background' and 'Never'."),
    "Verbosity"        , A_CHAR("0,Off,1,1,Error,1,1,Errors,1,2,Warning,1,2,Warnings,1,3,Info,1,4,Verbose,1"),
    "Scope"            , A_CHAR("0,Global,1,1,ThreadOuter,7,2,Filename,1,3,Method,1,4,ThreadInner,7,5,Path,7"),
    "StateInfo"        , A_CHAR("0,NONE,1,1,Basic,1,2,Version,1,^9,LogData,4,4,Loggers,1,^6,DSR,2,8,Domains,1,^4,InternalDomains,1,0x100000,SPTR,2,^5,ScopeDomains,1,^7,PrefixLogables,1,^8,Once,1,^10,ThreadMappings,1,^21,CompilationFlags,1,0xFFFFFFFF,All,1"),
    "LCU"              , A_CHAR("0,Auto,1,1,Never,1,2,Foreground,2,3,Background,2,2,ForeGround,1,3,BackGround,1"),
    "VVEA"             , A_CHAR("ExportAll"),
    "TLFmtExc"         , A_CHAR("\nAn exception occurred during formatting ALox logables:\n"),
    "CFG_SECT_CMT_ALOX", A_CHAR("@>'/// '@HL-Settings controlling ALox log output.\n")
                         A_CHAR("@HL-"),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );
}
} // anonymous namespace
#endif // !DOXYGEN


ALoxCamp::ALoxCamp()
: Camp( "ALOX" ) {
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( this == &ALOX, "ALOX",
          "Instances of class ALox must not be created. Use singleton alib::ALOX" )
    #endif
}

void  ALoxCamp::Reset() {
    #if ALOX_DBG_LOG
        if (Log::DEBUG_LOGGER != nullptr )
            Log_RemoveDebugLogger()

        lang::Destruct(*DEBUG_LOX);
    #endif

    ALIB_ASSERT_ERROR(detail::dbgCountLoxes() == 0, "ALOX", "A Lox remained from the last test" )

    #if ALOX_DBG_LOG
        new (DEBUG_LOX) Lox( "LOG" );
    #endif
}

//##################################################################################################
// Compilation Flags
//##################################################################################################

// check the configuration macros, give warning once (therefore not in HPP)
#if !ALOX_DBG_LOG && ALOX_DBG_LOG_CI
#   pragma message ( "Warning: ALox configuration macro mismatch: ALOX_DBG_LOG_CI is true, while ALOX_DBG_LOG is false." )
#endif
#if !ALOX_REL_LOG && ALOX_REL_LOG_CI
#   pragma message ( "Warning: ALox configuration macro mismatch: ALOX_REL_LOG_CI is true, while ALOX_REL_LOG is false" )
#endif

//##################################################################################################
// ALox module initialization
//##################################################################################################

void  ALoxCamp::Bootstrap() {
    if( GetBootstrapState() == BootstrapPhases::PrepareResources ) {
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
        // now, we just add the resources that parent App did not define for us.
        camp::DevtimeResourceCompiler rc;
        if(!rc.Do( "aloxcamp.alibrc", __FILE__, *this, ResourceCategory, true,
                                      __FILE__ )
                               //       nullptr )
            )
            bulkloadResources(GetResourcePool(), ResourceCategory);

        // a few resources cannot be defined using the resource compiler, as their value
        // depends on the build type.
        resourcePool->BootstrapBulk( ResourceCategory,

        // standard textlogger format string
        #if ALOX_DBG_LOG_CI || ALOX_REL_LOG_CI
            "Var_D21",      A_CHAR("%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%# %V: ")
        #else
            "Var_D21",      A_CHAR("[%TC +%TL][%tN][%D]%A1#%# %V: ")
        #endif
                            A_CHAR(",[ERR],[WRN],     ,[***]")                  ,

        // ANSI textlogger format string
        #if ALOX_DBG_LOG_CI || ALOX_REL_LOG_CI
            "Var_D21A",      A_CHAR("%SF:%SL:%A3%SM %A3[%TC +%TL][%tN][%D]%A1#%#: %V")
        #else
            "Var_D21A",      A_CHAR("[%TC +%TL][%tN][%D]%A1#%#:%V")
        #endif
                           A_CHAR(   ",\033c0,\033c3,,\033c8,\033[0m"  )        ,

        #if defined(_WIN32)
            "Var_D28",  A_CHAR("65001"),
            "Var_C28",  A_CHAR("Code page used by class WindowsConsoleLogger. Defaults to 65001.\n"
                               "(Only used on Windows OS)" )                    ,
        #endif

        #if ALIB_DEBUG
            "CFG_SECT_CMT_ALOX/LOG",          A_CHAR("@>'/// '@HL-Settings of the Lox-instance used with debug-logging.\n@HL-"),
            "CFG_SECT_CMT_ALOX/DEBUG_LOGGER", A_CHAR("@>'/// '@HL-Settings of the debug-logger.\n@HL-"),
        #endif

        // end of BootstrapBulk()
        nullptr  );

#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
    }

DOX_MARKER([DOX_VARIABLES_DEFINETYPE3])
//...
//...
else if( GetBootstrapState() == BootstrapPhases::PrepareConfig ) {
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

    else if( GetBootstrapState() == BootstrapPhases::Final ) {
        #if ALOX_DBG_LOG
            if ( !DEBUG_LOX )
                DEBUG_LOX=monomem::GLOBAL_ALLOCATOR().New<Lox>("LOG");
        #endif
}   }


void ALoxCamp::Shutdown( ShutdownPhases phase ) {
    (void) phase;
    #if ALOX_DBG_LOG
    if( phase == ShutdownPhases::Destruct ) {
        if ( Log::DEBUG_LOGGER != nullptr )
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
void    AppendableTraits<Scope,nchar, lang::HeapAllocator>::operator()( NAString&        target,
                                                                        const lox::Scope src ) {
    Scope scope= src;
    int pathLevel= int( scope - Scope::Path );
    if(pathLevel > 0 )
        scope= Scope::Path;

    target << "Scope::" << enumrecords::GetRecord(scope).EnumElementName;

    if( pathLevel > 0 )
        target << '+' << pathLevel;
}

void AppendableTraits<Pair<Verbosity, Priority>,nchar, lang::HeapAllocator>::operator()(
                                          NAString& target, const Pair<Verbosity, Priority>& src ) {
    target._( NField( src.First, 7, lang::Alignment::Left) );
    target._( '(' )._( src.Second );
    target.InsertAt(")",target.LastIndexOfAny<lang::Inclusion::Exclude>(NDEFAULT_WHITESPACES) + 1 );
}
}}

// CVVerbosities
namespace alib::variables::detail {
ALIB_DLL void  VMeta_CVVerbosities::imPort(VDATA*               data, Configuration&,
                                           const StringEscaper& esc , const String& src) {
    auto& cvVerbosities= data->As<alib::lox::CVVerbosities>();
    auto& exportAllKeyWord= alib::ALOX.GetResource("VVEA");
    cvVerbosities.ExportAll= false;

    LocalAllocator4K la;
    StringVectorMA results(la);
    esc.UnescapeTokens(results, src, A_CHAR(";"));
    cvVerbosities.Clear();
    for( auto& it : results ) {
        Substring value= it;
        value.Trim(A_CHAR("\r\n"));

        if( value.Equals<CHK, lang::Case::Ignore>(exportAllKeyWord) ) {
            cvVerbosities.ExportAll= true;
            continue;
        }
        cvVerbosities.Add( value );
}   }

ALIB_DLL void  VMeta_CVVerbosities::exPort( VDATA*               data, Configuration&,
                                            const StringEscaper& esc , AString&        dest) {
    auto& cvVerbosities= data->As<alib::lox::CVVerbosities>();
    auto& exportAllKeyWord= alib::ALOX.GetResource("VVEA");
    if( cvVerbosities.ExportAll)
        dest << exportAllKeyWord << " ;" << NEW_LINE;

    if( !cvVerbosities.IsEmpty()) // well, this can only be empty if a user makes mistakes
        for( auto it= cvVerbosities.begin() ;; ) {
            esc.Escape(*it, dest, A_CHAR(";"));

            if( ++it == cvVerbosities.end() )
                break;

            dest << " ;";
            dest << NEW_LINE;
}       }
} //namespace [alib::variables::detail]

#endif // !DOXYGEN

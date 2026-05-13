namespace alib { app::AppCamp APP; }

namespace alib::app {

#if !DOXYGEN
namespace {
void bulkloadResources(ResourcePool& resourcePool, const NCString& resourceCategory) {
    resourcePool.BootstrapBulk( resourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "E<"                , A_CHAR("app::"),
    "E0"                , A_CHAR("1,NoCommandGiven,ED1"),
    "E1"                , A_CHAR("2,UnknownCommand,ED2"),
    "E2"                , A_CHAR("10,ParsingOptions,ED10"),
    "E3"                , A_CHAR("11,MissingOptionValue,ED11"),
    "E4"                , A_CHAR("12,IllegalOptionNameContinuation,ED12"),
    "E5"                , A_CHAR("20,ParsingCommand,ED20"),
    "E6"                , A_CHAR("21,MissingParameterValue,ED21"),
    "ED1"               , A_CHAR("No command given."),
    "ED2"               , A_CHAR("Unknown command found with CLI argument{!Q[]}={!Q}"),
    "ED10"              , A_CHAR("Error parsing option. Usage:\n")
                          A_CHAR("{}"),
    "ED20"              , A_CHAR("Error parsing command. Usage: {}\n")
                          A_CHAR("{}"),
    "ED11"              , A_CHAR("Missing argument(s) of option {!Q<>} found with CLI argument{!Q[]}={!Q}.\n")
                          A_CHAR("Expected {}, given {}."),
    "ED12"              , A_CHAR("Option identifier {!Q<>} found at start of CLI argument{!Q[]}={!Q},\n")
                          A_CHAR("while CLI argument string continues with non-argument identifier characters."),
    "ED21"              , A_CHAR("Missing argument(s) of parameter {!Q<>} found with CLI argument{!Q[]}={!Q}.\n")
                          A_CHAR("Quantity expected {}, quantity given {}."),
    "AppVersion"        , A_CHAR("V.{}."),
    "Commands<"         , A_CHAR("Commands::"),
    "Commands"          , A_CHAR("1,help,1,TOPIC,2,version,3,,3,info,4,"),
    "THlpCmdSht_help"   , A_CHAR("Shows general help or help on topics,"),
    "THlpCmdLng_help"   , A_CHAR("Shows general help, respectively detail help on the optionally given TOPIC(s), which can be a command or an option - or a parameter of such.\n")
                          A_CHAR("If command 'help' is given, no further CLI commands are processed and execution is stopped."),
    "THlpCmdSht_version", A_CHAR("Shows the version of this software."),
    "THlpCmdLng_version", A_CHAR("Shows the version of this software."),
    "THlpCmdSht_info"   , A_CHAR("Shows information about this software."),
    "THlpCmdLng_info"   , A_CHAR("Shows information about this software."),
    "Options<,Options::", A_CHAR(""),
    "Options"           , A_CHAR("1,help,4,h,=,0,,2,version,7,,,0,,3,dryrun,6,d,=,0,,4,config,6,c,=,1,,5,verbose,7,v,=,0,"),
    "TOptUsg_help"      , A_CHAR("--help[[=]TOPIC]"),
    "TOptHlp_help"      , A_CHAR("Displays usage information."),
    "TOptUsg_version"   , A_CHAR("--version"),
    "TOptHlp_version"   , A_CHAR("Displays the version of this software."),
    "TOptUsg_dryrun"    , A_CHAR("-d|--dryrun[=yes|no]"),
    "TOptHlp_dryrun"    , A_CHAR("Enables dryrun-mode for sandbox testing. No changes are performed."),
    "TOptUsg_config"    , A_CHAR("--config=filename1[[,filename2],filename3]..."),
    "TOptHlp_config"    , A_CHAR("Allows to change the name (and path) of the configuration file(s)"),
    "TOptUsg_verbose"   , A_CHAR("--verbose[=level]"),
    "TOptHlp_verbose"   , A_CHAR("Changes verbosity to 'VERBOSE' or the optionally given preset."),
    "Parameters"        , A_CHAR("1,TOPIC,1,,=,,-1,1"),
    "Parameters<"       , A_CHAR("Parameters::"),
    "THlpParSht_TOPIC"  , A_CHAR("Optional parameter of command (or option) 'help'."),
    "THlpParLng_TOPIC"  , A_CHAR("Denotes a specific topic that the help command should be verbose about."),
    "ExitCodes<"        , A_CHAR("ExitCodes::"),
    "ExitCodes"         , A_CHAR("0,OK,1,ErrNoCmdGiven,2,ErrUnknownCommand,3,ErrParsingOption,4,ErrParsingCommand,5,ErrMissingCmdParam,6,ErrMissingOptParam,7,ErrBadParamValue,10,ErrConfigFileNotFound,11,ErrConfigFileNotWritable,127,UnknownError"),
    "TExit0"            , A_CHAR("Success (no error)."),
    "TExit1"            , A_CHAR("No command given."),
    "TExit2"            , A_CHAR("Unknown command {!Q} given."),
    "TExit3"            , A_CHAR("Unknown option {!Q} was given."),
    "TExit4"            , A_CHAR("An error occurred when parsing the command {!Q}."),
    "TExit5"            , A_CHAR("Missing a mandatory parameter {!Q} of the given command {!Q}."),
    "TExit6"            , A_CHAR("Missing a mandatory parameter {!Q} of the given option {!Q}."),
    "TExit7"            , A_CHAR("Parameter {!Q} is not suitable for command or option {!Q}."),
    "TExit10"           , A_CHAR("Configuration file {!Q} not found."),
    "TExit11"           , A_CHAR("Configuration file {!Q} not writeable."),
    "TExit127"          , A_CHAR("An unexpected (unhandled) internal error occurred."),
    "HlpHdlOpts"        , A_CHAR("OPTIONS:"),
    "HlpHdlCmds"        , A_CHAR("COMMANDS:"),
    "HlpHdlExtCds"      , A_CHAR("EXIT-CODES:"),
    "HlpHdlUsage"       , A_CHAR("USAGE:"),
    "HlpHdlDscr"        , A_CHAR("DESCRIPTION:"),
    "HlpHdlPDscr"       , A_CHAR("PARAMETER DESCRIPTION:"),
    "HlpHdlTopic"       , A_CHAR("Help on {} {!Q<>}:\n"),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );
}
} // anonymous namespace
#endif // !DOXYGEN

void AppCamp::Bootstrap() {
    if( GetBootstrapState() == BootstrapPhases::PrepareResources) {

#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
        // now, we just add the resources that parent App did not define for us.
        camp::DevtimeResourceCompiler rc;
        if(!rc.Do( "appcamp.alibrc", __FILE__, *this, ResourceCategory, true,
                                     __FILE__ )
                             //       nullptr )
            )
            bulkloadResources(GetResourcePool(), ResourceCategory);
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(App::Commands  )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(App::Parameters)
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(App::Options   )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE(App::ExitCodes )

    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig ) {
        enumrecords::bootstrap::Bootstrap<CLIExceptions     >();
        enumrecords::bootstrap::Bootstrap<App::Commands     >();
        enumrecords::bootstrap::Bootstrap<App::Parameters   >();
        enumrecords::bootstrap::Bootstrap<App::Options      >();
        enumrecords::bootstrap::Bootstrap<App::ExitCodes    >();
}   }

} // namespace [alib::app]

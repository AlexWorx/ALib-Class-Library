namespace enbs= alib::enumrecords::bootstrap;
namespace alib { camp::Basecamp BASECAMP; }

using namespace alib::strings;

namespace alib::camp {
#if !DOXYGEN
namespace {
void bulkloadResources(ResourcePool& resourcePool, const NCString& resourceCategory) {
    resourcePool.BootstrapBulk( resourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "Alignment"        , A_CHAR("0,Left,1,1,Right,1,2,Center,1"),
    "Bool"             , A_CHAR("0,False,1,1,True,1,0,0,1,1,1,1,0,No,1,1,Yes,1,0,Off,2,1,On,2,0,-,1,1,Ok,2"),
    "Caching"          , A_CHAR("0,Disabled,1,1,Enabled,1,1,Auto,1"),
    "Case"             , A_CHAR("0,Sensitive,1,1,Ignore,1"),
    "ContainerOp"      , A_CHAR("0,Insert,1,1,Remove,1,3,GetCreate,4,2,Get,1,4,Create,1"),
    "CurrentData"      , A_CHAR("0,Keep,1,1,Clear,1"),
    "Inclusion"        , A_CHAR("0,Include,1,1,Exclude,1"),
    "Initialization"   , A_CHAR("0,Suppress,1,1,Default,1,2,Nulled,1,0,None,2,1,Initialize,1,2,Zero,1"),
    "LineFeeds"        , A_CHAR("0,None,1,0,Ignore,1,1,Unix,1,2,WindowsOS,1,3,Platform,1"),
    "Phase"            , A_CHAR("0,Begin,1,1,End,1"),
    "Propagation"      , A_CHAR("0,Omit,1,1,ToDescendants,1"),
    "Reach"            , A_CHAR("0,Global,1,1,Local,1"),
    "Responsibility"   , A_CHAR("0,KeepWithSender,1,1,Transfer,1"),
    "Safeness"         , A_CHAR("0,Safe,1,1,Unsafe,1"),
    "Side"             , A_CHAR("0,Left,1,1,Right,1"),
    "SortOrder"        , A_CHAR("0,Ascending,1,1,Descending,1"),
    "SourceData"       , A_CHAR("0,Copy,1,1,Move,1"),
    "Switch"           , A_CHAR("0,Off,2,1,On,2"),
    "Timezone"         , A_CHAR("0,Local,1,1,Local,1"),
    "Timing"           , A_CHAR("0,Async,1,1,Sync,1,0,Asynchronous,1,1,Synchronous,1,1,Synchronized,1"),
    "ValueReference"   , A_CHAR("0,Absolute,1,1,Relative,1"),
    "Whitespaces"      , A_CHAR("0,Trim,1,1,Keep,1"),
    "TSts"             , A_CHAR("0,Unstarted,1,1,Started,3,2,Running,1,3,Done,3,4,Terminated,1"),
    "TMDWP"            , A_CHAR("0,Lowest,4,500,DeferredDeletion,1,1000,Low,1,2000,Standard,1,4000,Highest,5,3000,High,1"),
    "VARE<"            , A_CHAR("variables::"),
    "VARE"             , A_CHAR("0,OK,,1,ErrorOpeningFile,VARERF,2,ErrorWritingFile,VAREWF"),
    "VARERF"           , A_CHAR("Can't open {}file {!Q'} for reading."),
    "VAREWF"           , A_CHAR("Can't open {}file {!Q'} for writing."),
    "VarPrio"          , A_CHAR("0,NONE,1,1000,AutoDetected,1,2000,DefaultValues,1,4000,Standard,1,6000,ConfigFile,2,8000,Environment,1,10000,SessionFile,8,12000,CLI,2,14000,Session,2,max,Protected,1"),
    "CFGPlgEnv"        , A_CHAR("Environment Variables"),
    "CFGPlgCLI"        , A_CHAR("Command Line Arguments"),
    "CFGIniWB"         , A_CHAR("writeback"),
    "CFGBTF"           , A_CHAR("False I 1,True I 1,0 I 1,1 I 1,No I 1,Yes I 1,Off I 3,On I 2,--- I 1,OK I 2"),
    "Var0"             , A_CHAR("1|ALIB/LOCALE|S"),
    "Var1"             , A_CHAR("2|ALIB/WAIT_FOR_KEY_PRESS|B"),
    "Var2"             , A_CHAR("3|ALIB/HAS_CONSOLE_WINDOW|B"),
    "Var_C1"           , A_CHAR("Defines the locale of the application. If empty or not set, the system's locale is used.\n")
                         A_CHAR("If set, a UTF8 locale should be used, for example, 'us_US.UTF-8' or 'de_DE.UTF-8'"),
    "Var_C2"           , A_CHAR("If true, the process waits for a key stroke on termination. If empty, under Windows\n")
                         A_CHAR("the behavior is detected, under other OSes, it defaults to false."),
    "Var_C3"           , A_CHAR("Boolean value that denotes what its name indicates. If empty, under Windows the value is\n")
                         A_CHAR("detected, under other OSes, it defaults to true."),
    "CFG_SECT_CMT_ALIB", A_CHAR("@>'/// '@HL-General ALib library settings.\n")
                         A_CHAR("@HL-)"),
    "EXIT_MSG"         , A_CHAR("ALIB: Press 'Enter' to exit..."),
    "ACAlgos"          , A_CHAR("0,NONE,1,31,ALL,1,1,Uncompressed,1,2,MinMax,1,4,Sparse,1,8,VerySparse,1,16,Incremental,1,32,Huffman,1"),
    "Months"           , A_CHAR("January,February,March,April,May,June,July,August,September,October,November,December"),
    "Days"             , A_CHAR("Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday"),
    "DurUnts"          , A_CHAR("zero time, days, day, hours, hour, minutes, minute, seconds, second, ms, ms, \u03BCs, \u03BCs, ns, ns"),
    "FMTDT"            , A_CHAR("yyyy-MM-dd HH:mm:ss"),
    "FMTCI"            , A_CHAR("[@ sf:sl from ''ya'' by ''ta'']"),
    "FMTCINT"          , A_CHAR("<None>"),
    "FMTCINY"          , A_CHAR("<None>"),
    "FMTCINR"          , A_CHAR("<Null>"),
    "SF"               , A_CHAR("0,Root,1,1,Current,1,3,HomeConfig,5,2,Home,1,4,Module,1,5,Temp,1,6,VarTemp,1"),
    "ExcFmtExc"        , A_CHAR("\nAn exception occurred while formatting another (!) exception:\n"),
    "PHT"              , A_CHAR("0,NotGiven,1,1,String,1,2,Character,1,3,IntBase10,6,4,IntBinary,6,5,IntOctal,6,6,IntHex,6,7,Float,1,8,Bool,1,9,HashCode,1,10,Fill,4"),
    "FE<"              , A_CHAR("format::"),
    "FE0"              , A_CHAR("11,ArgumentIndexIs0,FMG11"),
    "FE1"              , A_CHAR("12,ArgumentIndexOutOfBounds,FMG12"),
    "FE2"              , A_CHAR("13,IncompatibleTypeCode,FMG13"),
    "FE3"              , A_CHAR("101,MissingClosingBracket,FPS101"),
    "FE4"              , A_CHAR("102,MissingPrecisionValuePS,FPS102"),
    "FE5"              , A_CHAR("103,DuplicateTypeCode,FPS103"),
    "FE6"              , A_CHAR("104,UnknownTypeCode,FPS104"),
    "FE7"              , A_CHAR("105,ExclamationMarkExpected,FPS105"),
    "FE8"              , A_CHAR("106,UnknownConversionPS,FPS106"),
    "FE9"              , A_CHAR("107,PrecisionSpecificationWithInteger,FPS107"),
    "FE10"             , A_CHAR("108,MissingReplacementStrings,FPS108"),
    "FE11"             , A_CHAR("201,NegativeValuesInBracketsNotSupported,FJS101"),
    "FE12"             , A_CHAR("202,MissingPrecisionValueJS,FJS102"),
    "FE13"             , A_CHAR("203,HexadecimalFloatFormatNotSupported,FJS103"),
    "FE14"             , A_CHAR("204,NoAlternateFormOfConversion,FJS104"),
    "FE15"             , A_CHAR("205,NoPrecisionWithConversion,FJS105"),
    "FE16"             , A_CHAR("206,UnknownDateTimeConversionSuffix,FJS106"),
    "FE17"             , A_CHAR("207,UnknownConversionJS,FJS107"),
    "FE18"             , A_CHAR("501,UnknownPropertyInFormatString,PFM501"),
    "FE19"             , A_CHAR("502,ErrorInResultingFormatString,PFM502"),
    "FE20"             , A_CHAR("510,MissingConfigurationVariable,PFM510"),
    "FE21"             , A_CHAR("601,UnknownMarker,ST601"),
    "FE22"             , A_CHAR("602,EndmarkerWithoutStart,ST602"),
    "FMG11"            , A_CHAR("Argument index 0 not allowed.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FMG12"            , A_CHAR("Argument index #{} is out of bounds. {} arguments are available.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FMG13"            , A_CHAR("Detected placeholder type does not match given argument.\n")
                         A_CHAR("Placeholder number:    {}\n")
                         A_CHAR("Placeholder type:      {} ({})\n")
                         A_CHAR("Deduced argument type: {!Q}\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS101"           , A_CHAR("Closing bracket '}}' of placeholder not found (or syntax error).\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS102"           , A_CHAR("Missing precision value after '.' character.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS103"           , A_CHAR("Duplicate type code {!Q'} given (previous was {!Q'}).\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS104"           , A_CHAR("Unknown type code {!Q'} given.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS105"           , A_CHAR("Expected '!' in continuation of placeholder.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS106"           , A_CHAR("Unknown conversion \"!{}\".\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS107"           , A_CHAR("Precision not allowed with integer format.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FPS108"           , A_CHAR("Missing pair of replacement strings \"<src><repl>\" after \"!Replace\".\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS101"           , A_CHAR("Brackets for negative values not implemented/supported.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS102"           , A_CHAR("Missing precision value after '.' character.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS103"           , A_CHAR("Hexadecimal float format not implemented/supported.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS104"           , A_CHAR("Alternate form '#' not supported with conversion {!Q}.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS105"           , A_CHAR("Precision specification (.{}) is not supported with conversion {!Q}.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS106"           , A_CHAR("Unknown date/time conversion suffix {!Q'}.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "FJS107"           , A_CHAR("Unknown conversion character {!Q'}.\n")
                         A_CHAR("In: {!Q}\n")
                         A_CHAR("    >{!FillC-}^"),
    "PFM501"           , A_CHAR("Unknown property identifier {}{} in format string.\n")
                         A_CHAR("Format string: {!Q}.\n")
                         A_CHAR("Allowed identifiers:"),
    "PFM502"           , A_CHAR("Error in resulting format string.\n")
                         A_CHAR("Source format: {!Q}."),
    "PFM510"           , A_CHAR("Missing or empty configuration variable {!Q} containing user-defined format string."),
    "ST601"            , A_CHAR("Unknown marker at position {}. \n")
                         A_CHAR("  Marked text: {!Q}. \n")
                         A_CHAR("               >{!FillC-}^"),
    "ST602"            , A_CHAR("Found an end-marker without a start marker at position {}. \n")
                         A_CHAR("  Marked text: {!Q}. \n")
                         A_CHAR("               >{!FillC-}^"),
    "BS"               , A_CHAR("0,B,3,1,KiB,3,2,MiB,3,3,GiB,3,4,TiB,3,5,PiB,3,6,EiB,3,7,ZiB,3,8,YiB,3,9,RiB,3,10,QiB,3,11,B,1,12,kB,2,13,MB,2,14,GB,2,15,TB,2,16,PB,2,17,EB,2,18,ZB,2,19,YB,2,20,RB,2,21,QB,2"),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );
}
} // anonymous namespace
#endif // !DOXYGEN

Basecamp::Basecamp()
: Camp( "ALIB" )
, LocaleFound(nullptr) {
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
        ALIB_ASSERT_ERROR( this == &BASECAMP, "BASECAMP",
            "Instances of class Basecamp must not be created. Use singleton alib::BASECAMP" )
    #endif
}

void Basecamp::Bootstrap() {
    if( GetBootstrapState() == BootstrapPhases::PrepareResources ) {
//! @cond NO_DOX
        #define EOS ,
//! @endcond
#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
        // now, we just add the resources that parent App did not define for us.
        camp::DevtimeResourceCompiler rc;
        if(!rc.Do( "basecamp.alibrc", __FILE__, *this, ResourceCategory, true,
                                      __FILE__ )
                               //       nullptr )
            )
            bulkloadResources(GetResourcePool(), ResourceCategory);
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        // CodeMarker_CommonEnums
        enbs::Bootstrap<alib::lang::Alignment        >( *this, "Alignment"      );
        enbs::Bootstrap<alib::lang::Bool             >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::Caching          >( *this, "Caching"        );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        enbs::Bootstrap<alib::lang::Case>( *this, "Case"  );
DOX_MARKER( [DOX_ENUMS_MAKE_PARSABLE_22] )
        enbs::Bootstrap<alib::lang::ContainerOp      >( *this, "ContainerOp"    );
        enbs::Bootstrap<alib::lang::CreateDefaults   >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::CreateIfNotExists>( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::CurrentData      >( *this, "CurrentData"    );
        enbs::Bootstrap<alib::lang::Inclusion        >( *this, "Inclusion"      );
        enbs::Bootstrap<alib::lang::Initialization   >( *this, "Initialization" );
        enbs::Bootstrap<alib::lang::LineFeeds        >( *this, "LineFeeds"      );
        enbs::Bootstrap<alib::lang::Phase            >( *this, "Phase"          );
        enbs::Bootstrap<alib::lang::Propagation      >( *this, "Propagation"    );
        enbs::Bootstrap<alib::lang::Reach            >( *this, "Reach"          );
        enbs::Bootstrap<alib::lang::Recursive        >( *this, "Bool"           );
        enbs::Bootstrap<alib::lang::Responsibility   >( *this, "Responsibility" );
        enbs::Bootstrap<alib::lang::Safeness         >( *this, "Safeness"       );
        enbs::Bootstrap<alib::lang::Side             >( *this, "Side"           );
        enbs::Bootstrap<alib::lang::SortOrder        >( *this, "SortOrder"      );
        enbs::Bootstrap<alib::lang::SourceData       >( *this, "SourceData"     );
        enbs::Bootstrap<alib::lang::Switch           >( *this, "Switch"         );
        enbs::Bootstrap<alib::lang::Timezone         >( *this, "Timezone"       );
        enbs::Bootstrap<alib::lang::Timing           >( *this, "Timing"         );
        enbs::Bootstrap<alib::lang::ValueReference   >( *this, "ValueReference" );
        enbs::Bootstrap<alib::lang::Whitespaces      >( *this, "Whitespaces"    );

IF_ALIB_BITBUFFER(  enbs::Bootstrap<alib::bitbuffer::ac_v1::ArrayCompressor::Algorithm>( *this, "ACAlgos"  ); )
IF_ALIB_THREADS(    enbs::Bootstrap<alib::threads::Thread::State>( *this, "TSts"     ); )
IF_ALIB_THREADMODEL(enbs::Bootstrap<alib::threadmodel::Priority> ( *this, "TMDWP"    ); )


        enumrecords::bootstrap::Bootstrap<variables::Priority  >();
        enumrecords::bootstrap::Bootstrap<variables::Exceptions>();


DOX_MARKER([DOX_VARIABLES_DECLARATION7])
enbs::Bootstrap<camp::Variables>('|');
DOX_MARKER([DOX_VARIABLES_DECLARATION7])

        enbs::Bootstrap<SystemFolders                     >( *this, "SF"  );
        enbs::Bootstrap<format::FormatterStdImpl::PHTypes >( *this, "PHT" );
        enbs::Bootstrap<format::ByteSizeUnits             >();
        enbs::Bootstrap<alib::format::FMTExceptions       >();
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig ) {

        // preload all variable with declarations (and without placeholders)
        config->PreloadVariables<camp::Variables>();
    }

    else if( GetBootstrapState() == BootstrapPhases::Final ) {
        Substring parser;
        parser= GetResource( "Months" ); for( int i= 0 ; i < 12 ; ++i ) { CalendarDateTime::MONTH_NAMES[i]= parser.ConsumeToken(','); }
        parser= GetResource( "Days"   ); for( int i= 0 ; i <  7 ; ++i ) { CalendarDateTime::  DAY_NAMES[i]= parser.ConsumeToken(','); }

        parser= GetResource( "DurUnts" );
        for( int i= 0 ; i < int(DayTimeUnits::SIZE_OF_UNITS) ; ++i )
            DT_UNITS[i]= parser.ConsumeToken(',');

        ALIB_ASSERT( DT_UNITS[size_t(int(DayTimeUnits::SIZE_OF_UNITS)-1)].IsNotEmpty(), "BASECAMP" )
        ALIB_ASSERT( parser.IsEmpty(), "BASECAMP" )

        format::BYTESIZE_NUMBER_FORMAT= monomem::GLOBAL_ALLOCATOR().New<NumberFormat>(NumberFormat::Global );
        format::BYTESIZE_NUMBER_FORMAT->FractionalPartWidth= 1;

      //######################################### set locale #######################################
        int receivedFrom= 0;
        String256 locale;

        Variable varLocale(*config, Variables::LOCALE);
        if (     varLocale.IsDefined()
             &&  varLocale.GetString().IsNotEmpty()        )
        {
            receivedFrom= 1;
            locale._( varLocale.GetString() );
        }
        else if ( EnvironmentVariables::Get( A_CHAR("LANG")      ,locale ) )  receivedFrom= 2;
        else if ( EnvironmentVariables::Get( A_CHAR("LANGUAGE")  ,locale ) )  receivedFrom= 3;

        if( receivedFrom > 0 && !locale.Equals<NC,lang::Case::Ignore>( A_CHAR("none") ) ) {
            ALIB_STRINGS_TO_NARROW( locale, nLocale, 1024)
            if( !setlocale(LC_ALL, nLocale ) ) {
                NString256 msg( "ALib Error: setlocale(\""); msg << nLocale <<"\") failed. Setting read from ";
                msg << (   receivedFrom == 1 ? "config variable 'ALIB_LOCALE'"   :
                           receivedFrom == 2 ? "environment variable 'LANG'"     :
                           receivedFrom == 3 ? "environment variable 'LANGUAGE'" : "ERROR"  );

                std::cerr.write( msg.Buffer(), msg.Length() );

                std::cerr << ". Trying  'setlocale(LC_ALL, \"\")': ";
                if ( setlocale(LC_ALL, ""  ) )
                    std::cerr << " success.";
                else {
                    std::cerr << "failed. Trying  'setlocale(LC_ALL, \"C\")': ";
                    if ( setlocale(LC_ALL, "C" ) )
                        std::cerr << " success.";
                    else
                        std::cerr << std::endl << "     Panic: No standard locale setting was successful!";
                }
                std::cerr << std::endl;
            }
            else
                LocaleFound.Allocate( monomem::GLOBAL_ALLOCATOR, locale);
        }

        // set the system's locale as the default for our static default number format
        #if ALIB_STRINGS
            NumberFormat::Global.SetFromLocale();
            NumberFormat::Global.Flags+= NumberFormatFlags::WriteGroupChars;
        #endif


        // --- determine if we have a console window ---
        // read configuration
        Variable varHasConsoleWindow(*config, Variables::HAS_CONSOLE_WINDOW );
        if ( varHasConsoleWindow.IsDefined() )
            HasConsoleWindow=  varHasConsoleWindow;
        else {
            #if defined(_WIN32)
                // determine if we have a console window
                HasConsoleWindow= GetConsoleWindow() != NULL;
            #else
                HasConsoleWindow= true;
            #endif
            (void) varHasConsoleWindow.Define();
            varHasConsoleWindow=  HasConsoleWindow;
        }

        // --- determine if we want to wait for a keypress upon termination ---
        Variable varWaitForKeyPress(*config, Variables::WAIT_FOR_KEY_PRESS );
        if ( varWaitForKeyPress.IsDefined() )
            WaitForKeyPressOnTermination= varWaitForKeyPress;
        else {
            #if defined(_WIN32) && ALIB_DEBUG
                WaitForKeyPressOnTermination=    HasConsoleWindow
                                              && IsDebuggerPresent();
            #else
                WaitForKeyPressOnTermination=  false;
            #endif
            (void) varWaitForKeyPress.Define();
            varWaitForKeyPress= WaitForKeyPressOnTermination;
}   }   }

void Basecamp::Shutdown( ShutdownPhases phase ) {
    if( phase == ShutdownPhases::Announce )
        return;

    // ShutdownPhases::Destruct:
    if ( BASECAMP.WaitForKeyPressOnTermination ) {
        #if defined(_WIN32)
            OutputDebugStringA( "\r\nALIB: Waiting for 'Enter' key in the console window.\r\n" );
        #endif

        std::cout << std::endl << GetResource("EXIT_MSG") << std::endl;
        while ( getchar() != '\n' )
            ;
    }

    // alib/lang/message
    #if ALIB_DEBUG
        assert::PLUGIN= nullptr;
    #endif
}

//##################################################################################################
// Environment definition/detection
//##################################################################################################


    #if (defined(__unix__) || defined(__APPLE__)) && !DOXYGEN
namespace { String  DebuggerProcessNames= A_CHAR("gdb|debugserver"); }
    #endif


bool Basecamp::IsDebuggerPresent() {
#if defined(__unix__) || defined(__APPLE__)

    // Unix: search debugger process in parent processes
    const ProcessInfo& pi= ProcessInfo::Current();
    uinteger parentPID= pi.PPID;
    if ( parentPID == 0 )
        return false;

    ProcessInfo actPI( parentPID );
    Tokenizer tknzr( DebuggerProcessNames, '|' );
    while(tknzr.HasNext() ) {
        Substring& debuggerName= tknzr.Next();
        if (actPI.Name.StartsWith( debuggerName ) )
            return true;
    }
    return false;

#elif defined(_WIN32)

    return (bool) ( ::IsDebuggerPresent() != 0 ) ;

   #else
    #pragma message ("Unknown Platform in file: " __FILE__ )
    return false;
   #endif
   }


} // namespace [alib::camp]

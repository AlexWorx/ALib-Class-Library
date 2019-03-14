// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/lib/alibmodules.hpp"


#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif


#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#    include "alib/resources/resources.hpp"
#endif


#if ALIB_MODULE_CONFIGURATION
#   if !defined (HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#      include "alib/system/environment.hpp"
#   endif
#endif

#if  ALIB_MODULE_SYSTEM
#   if !defined (HPP_ALIB_SYSTEM_S)
#      include "alib/system/system.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#      include "alib/system/environment.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
#      include "alib/system/processinfo.hpp"
#   endif
#   if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
#      include "alib/strings/util/tokenizer.hpp"
#   endif
#endif


#if ALIB_MODULE_EXPRESSIONS
#   if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONS)
#      include "alib/expressions/expressions.hpp"
#   endif
#endif

#if ALIB_MODULE_STRINGFORMAT
#   if !defined (HPP_ALIB_STRINGFORMAT_STRINGFORMAT)
#      include "alib/stringformat/stringformat.hpp"
#   endif
#endif

#if ALIB_MODULE_STRINGS
#   if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#      include "alib/strings/numberformat.hpp"
#   endif
#endif

#if ALIB_MODULE_RESULTS
#   if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#      include "alib/results/exception.hpp"
#   endif
#endif

#if ALIB_MODULE_RESULTS
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#endif

#if ALIB_MODULE_ALOX
#   if !defined (HPP_ALIB_ALOX)
#      include "alib/alox/alox.hpp"
#   endif
#endif

#if ALIB_MODULE_CLI
#   if !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/cli.hpp"
#   endif
#endif



namespace aworx {

lib::ALibModules ALIB;

namespace lib {

// #################################################################################################
// Static instance and constructor
// #################################################################################################

ALibModules::ALibModules()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB", ALIB_COMPILATION_FLAGS )
{
    ALIB_ASSERT_ERROR( this == &ALIB,
        "Instances of class ALibModules must not be created. Use singleton aworx::ALIB" )

    CompilationFlagMeanings=
    {
        { "ALIB_DEBUG"                               , ALIB_VFYBIT_DEBUG                          },

        { "ALIB_MODULE_MEMORY"                       , ALIB_VFYBIT_MODULE_MEMORY                  },
        { "ALIB_MODULE_SINGLETONS"                   , ALIB_VFYBIT_MODULE_SINGLETONS              },
        { "ALIB_MODULE_CHARACTERS"                   , ALIB_VFYBIT_MODULE_CHARACTERS              },
        { "ALIB_MODULE_ENUMS"                        , ALIB_VFYBIT_MODULE_ENUMS                   },
        { "ALIB_MODULE_TIME"                         , ALIB_VFYBIT_MODULE_TIME                    },
        { "ALIB_MODULE_BOXING"                       , ALIB_VFYBIT_MODULE_BOXING                  },
        { "ALIB_MODULE_STRINGS"                      , ALIB_VFYBIT_MODULE_STRINGS                 },
        { "ALIB_MODULE_RESOURCES"                    , ALIB_VFYBIT_MODULE_RESOURCES               },
        { "ALIB_MODULE_THREADS"                      , ALIB_VFYBIT_MODULE_THREADS                 },
        { "ALIB_MODULE_STRINGFORMAT"                 , ALIB_VFYBIT_MODULE_STRINGFORMAT            },
        { "ALIB_MODULE_RESULTS"                      , ALIB_VFYBIT_MODULE_RESULTS                 },
        { "ALIB_MODULE_SYSTEM"                       , ALIB_VFYBIT_MODULE_SYSTEM                  },
        { "ALIB_MODULE_CONFIGURATION"                , ALIB_VFYBIT_MODULE_CONFIGURATION           },
        { "ALIB_MODULE_ALOX"                         , ALIB_VFYBIT_MODULE_ALOX                    },
        { "ALIB_MODULE_EXPRESSIONS"                  , ALIB_VFYBIT_MODULE_EXPRESSIONS             },
        { "ALIB_MODULE_CLI"                          , ALIB_VFYBIT_MODULE_CLI                     },

        { "ALIB_FEAT_SINGLETON_MAPPED"               , ALIB_VFYBIT_FEAT_SINGLETON_MAPPED          },

        { "ALIB_CHARACTERS_ARE_NARROW"               , ALIB_VFYBIT_CHARACTERS_ARE_NARROW          },
        { "ALIB_CHARACTERS_WCHAR_IS_4"               , ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4          },

        { "ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS" , ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS },
        { "ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS", ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS},
        { "ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS"    , ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_FLOATS    },

        { "ALIB_STRINGS_DEBUG"                       , ALIB_VFYBIT_DEBUG_STRINGS                  },

        { "ALOX_DBG_LOG"                             , ALOX_VFYBIT_DBG_LOG                        },
        { "ALOX_DBG_LOG_CI"                          , ALOX_VFYBIT_DBG_LOG_CI                     },
        { "ALOX_REL_LOG"                             , ALOX_VFYBIT_REL_LOG                        },
        { "ALOX_REL_LOG_CI"                          , ALOX_VFYBIT_REL_LOG_CI                     },

    };


    //############### add dependencies ################
    #if ALIB_MODULE_EXPRESSIONS
        Dependencies.emplace_back( &lib::EXPRESSIONS );
    #endif

    #if ALIB_MODULE_CLI
        Dependencies.emplace_back( &lib::CLI );
    #endif

    #if ALIB_MODULE_ALOX
        Dependencies.emplace_back( &lib::ALOX );
    #else
        #if ALIB_MODULE_CONFIGURATION
            Dependencies.emplace_back( &lib::CONFIG );
        #endif
    #endif

    #if !ALIB_MODULE_CONFIGURATION && ALIB_MODULE_SYSTEM
        Dependencies.emplace_back( &lib::SYSTEM );
    #endif

    #if !ALIB_MODULE_SYSTEM && !ALIB_MODULE_EXPRESSIONS && !ALIB_MODULE_CLI && ALIB_MODULE_RESULTS
        Dependencies.emplace_back( &lib::RESULTS );
    #endif
}

// #################################################################################################
// Init/ TerminationCleanUp()
// #################################################################################################

void ALibModules::init( InitLevels level, int argc, const char** argv, const wchar_t** wargv)
{
    if( level == InitLevels::PrepareResources )
    {
        ArgC=  argc;
        ArgVN= argv;
        ArgVW= wargv;


        // ------ add resources ------
        // enums
        #if ALIB_MODULE_CONFIGURATION
            Resources->AddBulk(     ResourceCategory,
DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_MULTIPLE])
"Bool",         A_CHAR("0,False,1,"
                      "1,True,1,"
                      "0,0,1,"
                      "1,1,1,"
                      "0,No,1,"
                      "1,Yes,1,"
                      "0,On,2,"
                      "1,Off,2,"
                      "0,-,1,"
                      "1,Ok,2"    ),
DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_MULTIPLE])

DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_3])
"Case",         A_CHAR("0,Sensitive,1,"
                     "1,Ignore,1"     ),
DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_3])

DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_4])
"ContainerOp",  A_CHAR("0,Insert,1,"
                     "1,Remove,1,"
                     "3,GetCreate,4,"
                     "2,Get,1,"
                     "4,Create,1"     ),
DOX_MARKER([DOX_ALIB_ENUMS_PARSABLE_4])

//        catMap.emplace( "Switch0",          "0"  ","  "Off"                ","  "2" );
//        catMap.emplace( "Switch1",          "1"  ","  "On"                 ","  "2" );



        "Switch",          A_CHAR("0"  ","  "Off"                ","  "2" ","
                                  "1"  ","  "On"                 ","  "2"     ),

        "Alignment",       A_CHAR("0"  ","  "Left"               ","  "1" ","
                                  "1"  ","  "Right"              ","  "1" ","
                                  "2"  ","  "Center"             ","  "1"     ),

        "SortOrder",       A_CHAR("0"  ","  "Ascending"          ","  "1" ","
                                  "1"  ","  "Descending"         ","  "1"     ),

        "Inclusion",       A_CHAR("0"  ","  "Include"            ","  "1" ","
                                  "1"  ","  "Exclude"            ","  "1"     ),

        "Reach",           A_CHAR("0"  ","  "Global"             ","  "1" ","
                                  "1"  ","  "Local"              ","  "1"     ),

        "CurrentData",     A_CHAR("0"  ","  "Keep"               ","  "1" ","
                                  "1"  ","  "Clear"              ","  "1"     ),

        "SourceData",      A_CHAR("0"  ","  "Copy"               ","  "1" ","
                                  "1"  ","  "Move"               ","  "1"     ),

        "Safeness",        A_CHAR("0"  ","  "Safe"               ","  "1" ","
                                  "1"  ","  "Unsafe"             ","  "1"     ),

        "Responsibility",  A_CHAR("0"  ","  "KeepWithSender"     ","  "1" ","
                                  "1"  ","  "Transfer"           ","  "1"     ),

        "Timezone",        A_CHAR("0"  ","  "Local"              ","  "1" ","
                                  "1"  ","  "UTC"                ","  "1"     ),

        "Whitespaces",     A_CHAR("0"  ","  "Trim"               ","  "1" ","
                                  "1"  ","  "Keep"               ","  "1"     ),

        "Create",          A_CHAR("0"  ","  "Never"              ","  "1" ","
                                  "1"  ","  "IfNotExistent"      ","  "1"     ),

        "Propagation",     A_CHAR("0"  ","  "Omit"               ","  "1" ","
                                  "1"  ","  "ToDescendants"      ","  "1"     ),

        "Phase",           A_CHAR("0"  ","  "Begin "             ","  "1" ","
                                  "1"  ","  "End"                ","  "1"     ),

        "Initialization",  A_CHAR("0"  ","  "NoInit"             ","  "1" ","
                                  "1"  ","  "Init"               ","  "1" ","
                                  "0"  ","  "suppress"           ","  "1" ","
                                  "1"  ","  "perform"            ","  "1"     ),

        "Timing",          A_CHAR("0"  ","  "Async"              ","  "1" ","
                                  "1"  ","  "Sync"               ","  "1" ","
                                  "0"  ","  "asynchronous"       ","  "1" ","
                                  "1"  ","  "synchronous"        ","  "1" ","
                                  "1"  ","  "synchronized"       ","  "1"     ),

        // variables
        "Var0",    A_CHAR("1|ALIB|LOCALE|ES||||VC1"),
        "Var1",    A_CHAR("2|ALIB|WAIT_FOR_KEY_PRESS|VD2||||VC2"),
        "Var2",    A_CHAR("3|ALIB|HAS_CONSOLE_WINDOW|ES||||VC3"),


        "VC1",
        A_CHAR("Defines the locale of the application. If empty or not set, the system's locale is used.")
        ,
        "VC2",
        A_CHAR("If true, the process waits for a key stroke on termination. If empty, under Windows"   "\n"
             "behavior is detected, under other OSes, defaults to false."                               )
        ,
        "VC3",
        A_CHAR("Boolean value that denotes what its name indicates. If empty, under Windows value is " "\n"
             "detected, under other OSes, defaults to true."                                            )
        ,

        // Empty string. This is set with variables that want to be written into blank files.
        "ES",   A_CHAR(""),

        // default values
        "VD2",  A_CHAR(""),


            // end of AddBulk()
        nullptr );
        #endif
    }

    else if( level == InitLevels::Final )
    {
        #if ALIB_MODULE_CONFIGURATION
            Variable variable;
        #endif

        //############### set locale ###############
        #if ( defined (__GLIBCXX__)  || defined(__APPLE__) )
        {
            int receivedFrom= 0;
            String256 locale;
            #if ALIB_MODULE_CONFIGURATION
                variable.Declare( Variables::LOCALE );
                if (     Config->Load( variable ) != Priorities::NONE
                     &&  variable.GetString().IsNotEmpty()        )
                {
                    receivedFrom= 1;
                    locale._( variable.GetString() );
                }
                else
            #endif

            #if  ALIB_MODULE_SYSTEM
                     if ( system::GetEnvironmentVariable( A_CHAR("LANG")      ,locale ) )        receivedFrom= 2;
                else if ( system::GetEnvironmentVariable( A_CHAR("LANGUAGE")  ,locale ) )        receivedFrom= 3;
            #endif

            if( receivedFrom > 0 && !locale.Equals<Case::Ignore>( A_CHAR("none") ) )
            {
                ALIB_STRINGS_TO_NARROW( locale, nLocale, 1024)
                if( !setlocale(LC_ALL, nLocale ) )
                {
                    NString256 msg( "ALib Error: setlocale(\""); msg << nLocale <<"\") failed. Setting read from ";
                    msg << (   receivedFrom == 1 ? "config variable 'LOCALE'"        :
                               receivedFrom == 2 ? "environment variable 'LANG'"     :
                               receivedFrom == 3 ? "environment variable 'LANGUAGE'" : "ERROR"  );

                    std::cerr.write( msg.Buffer(), msg.Length() );

                    std::cerr << ". Trying  'setlocale(LC_ALL, \"\")': ";
                    if ( setlocale(LC_ALL, ""  ) )
                        std::cerr << " success.";
                    else
                    {
                        std::cerr << "failed. Trying  'setlocale(LC_ALL, \"C\")': ";
                        if ( setlocale(LC_ALL, "C" ) )
                            std::cerr << " success.";
                        else
                            std::cerr << std::endl << "     Panic: No standard locale setting was successful!";
                    }
                    std::cerr << std::endl;
                }
            }
        }
        #endif

        // set the system's locale as the default for our static default number format
        #if ALIB_MODULE_STRINGS
            NumberFormat::Global.SetFromLocale();
            NumberFormat::Global.WriteGroupChars= true;
        #endif


        // --- determine if we want to wait for a keypress upon termination ---
        #if ALIB_MODULE_CONFIGURATION
            variable.Declare( Variables::WAIT_FOR_KEY_PRESS );
            Config->Load( variable );
            if ( variable.Size() > 0 )
                WaitForKeyPressOnTermination= variable.IsTrue();
            else
            {
                #if defined(_WIN32) && ALIB_DEBUG
                    WaitForKeyPressOnTermination=    HasConsoleWindow
                                                  && IsDebuggerPresent();
                #else
                    WaitForKeyPressOnTermination=  false;
                #endif
            }
        #else
            {
                #if defined(_WIN32) && ALIB_DEBUG
                    WaitForKeyPressOnTermination=    HasConsoleWindow
                                                  && IsDebuggerPresent();
                #else
                    WaitForKeyPressOnTermination=  false;
                #endif
            }
        #endif

        // --- determine if we have a console window ---
            // read configuration
        #if ALIB_MODULE_CONFIGURATION
            variable.Declare( Variables::HAS_CONSOLE_WINDOW );
            Config->Load( variable );
            if ( variable.Size() > 0  && variable.GetString().Length() > 0 )
                HasConsoleWindow=  variable.IsTrue();
            else
        #endif // ALIB_MODULE_CONFIGURATION
            {
                #if defined(_WIN32)
                    // determine if we have a console window
                    HasConsoleWindow= GetConsoleWindow() != NULL;
                #else
                    HasConsoleWindow= true;
                #endif
            }

        // test if all modules have been initialized
        #if ALIB_DEBUG
            #if ALIB_MODULE_EXPRESSIONS
                assert( lib::EXPRESSIONS.IsInitialized() );
            #endif

            #if ALIB_MODULE_CLI
                assert( lib::CLI.IsInitialized() );
            #endif

            #if ALIB_MODULE_ALOX
                assert( lib::ALOX.IsInitialized() );
            #endif

            #if ALIB_MODULE_CONFIGURATION
                assert( lib::CONFIG.IsInitialized() );
            #endif

            #if ALIB_MODULE_SYSTEM
                assert( lib::SYSTEM.IsInitialized() );
            #endif

            #if ALIB_MODULE_RESULTS
                assert( lib::RESULTS.IsInitialized() );
            #endif

            #if ALIB_MODULE_STRINGFORMAT
                assert( lib::STRINGFORMAT.IsInitialized() );
            #endif
        #endif // ALIB_DEBUG

    }
}


void ALibModules::terminationCleanUp()
{
    if ( ALIB.WaitForKeyPressOnTermination )
    {
        #if defined(_WIN32)
            OutputDebugStringA( "\r\nALIB: Waiting for 'Enter' key in the console window.\r\n" );
        #endif

        std::cout << std::endl << "ALIB: Press 'Enter' to exit... " << std::endl;
        while ( getchar() != '\n' )
            ;
    }

}


// #############################################################################################
// Environment definition/detection
// #############################################################################################

#if ALIB_MODULE_SYSTEM

//! @cond NO_DOX
    #if defined(__unix__) || defined(__APPLE__)
    namespace {
        String      DebuggerProcessNames= A_CHAR("gdb|debugserver");
    }
    #endif
//! @endcond

    bool ALibModules::IsDebuggerPresent()
    {
    #if defined(__unix__) || defined(__APPLE__)

        // Unix: search debugger process in parent processes
        const ProcessInfo& pi= ProcessInfo::Current();
        uinteger parentPID= pi.PPID;
        if ( parentPID == 0 )
            return false;

        ProcessInfo actPI( parentPID );
        Tokenizer tknzr( DebuggerProcessNames, '|' );
        while(tknzr.HasNext() )
        {
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

#endif // ALIB_MODULE_SYSTEM


// #############################################################################################
// CheckDistribution
// #############################################################################################
void ALibModules::CheckDistribution( int alibVersion, volatile uint64_t compilationFlags )
{
    if (Version != alibVersion )
    {
        std::cout << "!!! Error in ALox library compilation: linked against wrong version of ALib" << std::endl;
        std::cout << "!!! ALib library version:   " << Version       << std::endl;
        std::cout << "!!! ALib requested version: " << alibVersion   << std::endl;
        std::cout << "!!! Exiting with exit(-1)" << std::endl;
        exit(-1);
    }

    // verify the given (actual compilation units') flags against ALib
    if (!VerifyCompilationFlags( compilationFlags ) )
    {
        std::cout << "!!! Error in ALib library compilation: linked library of ALib has different compilation symbols set." << std::endl;
        exit(-1);
    }
}


}} // namespace [aworx::lib]


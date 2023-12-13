// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)

#if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#   include "alib/lib/fs_modules/distribution.hpp"
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif

#   if ALIB_THREADS && ALIB_ENUMS
#      include "alib/threads/thread.hpp"
#   endif

#if ALIB_CONFIGURATION
#   if !defined (HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_INI_FILE)
#      include "alib/config/inifile.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_VARIABLE)
#      include "alib/config/variable.hpp"
#   endif
#   if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#      include "alib/config/configuration.hpp"
#   endif
#   if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#      include "alib/system/environment.hpp"
#   endif
#endif

#if  ALIB_SYSTEM
#   if !defined (HPP_ALIB_SYSTEM_SYSTEM)
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

#if ALIB_EXPRESSIONS
#   if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONS)
#      include "alib/expressions/expressions.hpp"
#   endif
#endif

#if ALIB_TEXT
#   if !defined (HPP_ALIB_TEXT_TEXT)
#      include "alib/text/text.hpp"
#   endif
#endif

#if ALIB_STRINGS
#   if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
#      include "alib/strings/numberformat.hpp"
#   endif
#endif

#if ALIB_BITBUFFER
#   if !defined (HPP_AWORX_ALIB_BITBUFFER_ARRAY_COMPRESSOR)
#      include "alib/bitbuffer/arraycompressor.hpp"
#   endif
#endif

#if ALIB_MONOMEM
#   if !defined (HPP_ALIB_MONOMEM_HASHTABLE)
#      include "alib/monomem/hashtable.hpp"
#   endif
#endif

#if ALIB_RESULTS
#   if !defined (HPP_ALIB_RESULTS_EXCEPTION)
#      include "alib/results/exception.hpp"
#   endif
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#endif

#if ALIB_ALOX
#   if !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif
#endif

#if ALIB_CLI
#   if !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/cli.hpp"
#   endif
#endif

#if !defined(HPP_ALIB_FS_COMMONENUMS)
#   include "alib/lib/fs_commonenums/commonenums.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#   include "alib/enums/recordbootstrap.hpp"
#endif

#endif // !defined(ALIB_DOX)


namespace aworx {

lib::ALibDistribution ALIB;

namespace lib {

// #################################################################################################
// Static instance and constructor
// #################################################################################################

ALibDistribution::ALibDistribution()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB", ALIB_COMPILATION_FLAGS )
, Modules(&monomem::GlobalAllocator)
, LocaleFound(nullptr)
{
   ALIB_ASSERT_ERROR( this == &ALIB, "MODULES",
       "Instances of class ALibDistribution must not be created. Use singleton aworx::ALIB" )

   CompilationFlagMeanings=
   {
       { "ALIB_DEBUG"                               , ALIB_VFYBIT_DEBUG                           },

       { "ALIB_MONOMEM"                             , ALIB_VFYBIT_MODULE_MONOMEM                  },
       { "ALIB_SINGLETONS"                          , ALIB_VFYBIT_MODULE_SINGLETONS               },
       { "ALIB_CHARACTERS"                          , ALIB_VFYBIT_MODULE_CHARACTERS               },
       { "ALIB_ENUMS"                               , ALIB_VFYBIT_MODULE_ENUMS                    },
       { "ALIB_TIME"                                , ALIB_VFYBIT_MODULE_TIME                     },
       { "ALIB_BOXING"                              , ALIB_VFYBIT_MODULE_BOXING                   },
       { "ALIB_STRINGS"                             , ALIB_VFYBIT_MODULE_STRINGS                  },
       { "ALIB_BITBUFFER"                           , ALIB_VFYBIT_MODULE_BITBUFFER                },
       { "ALIB_RESOURCES"                           , ALIB_VFYBIT_MODULE_RESOURCES                },
       { "ALIB_THREADS"                             , ALIB_VFYBIT_MODULE_THREADS                  },
       { "ALIB_TEXT"                                , ALIB_VFYBIT_MODULE_TEXT                     },
       { "ALIB_RESULTS"                             , ALIB_VFYBIT_MODULE_RESULTS                  },
       { "ALIB_SYSTEM"                              , ALIB_VFYBIT_MODULE_SYSTEM                   },
       { "ALIB_CONFIGURATION"                       , ALIB_VFYBIT_MODULE_CONFIGURATION            },
       { "ALIB_ALOX"                                , ALIB_VFYBIT_MODULE_ALOX                     },
       { "ALIB_EXPRESSIONS"                         , ALIB_VFYBIT_MODULE_EXPRESSIONS              },
       { "ALIB_CLI"                                 , ALIB_VFYBIT_MODULE_CLI                      },

       { "ALIB_FEAT_SINGLETON_MAPPED"               , ALIB_VFYBIT_FEAT_SINGLETON_MAPPED           },

       { "ALIB_CHARACTERS_WIDE"                     , ALIB_VFYBIT_CHARACTERS_WIDE                 },
       { "ALIB_CHARACTERS_WCHAR_IS_4"               , ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4           },

       { "ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS"     , ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_INTEGRALS },
       { "ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS"    , ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_CHARACTERS},
       { "ALIB_FEAT_BOXING_BIJECTIVE_FLOATS"        , ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_FLOATS    },

       { "ALIB_DEBUG_STRINGS"                       , ALIB_VFYBIT_DEBUG_STRINGS                   },
       { "ALIB_DEBUG_BOXING"                        , ALIB_VFYBIT_DEBUG_BOXING                    },
       { "ALIB_DEBUG_MONOMEM"                       , ALIB_VFYBIT_DEBUG_MONOMEM_MONOMEM            },
       { "ALIB_DEBUG_RESOURCES"                     , ALIB_VFYBIT_DEBUG_RESOURCES                 },

       { "ALOX_DBG_LOG"                             , ALOX_VFYBIT_DBG_LOG                         },
       { "ALOX_DBG_LOG_CI"                          , ALOX_VFYBIT_DBG_LOG_CI                      },
       { "ALOX_REL_LOG"                             , ALOX_VFYBIT_REL_LOG                         },
       { "ALOX_REL_LOG_CI"                          , ALOX_VFYBIT_REL_LOG_CI                      },

   };
}


// #################################################################################################
// Module dependencies
// #################################################################################################


void ALibDistribution::BootstrapFillDefaultModuleList()
{
    ALIB_ASSERT_ERROR( ALIB.Modules.IsEmpty(), "MODULES",
                       "List Modules::BootstrapModules already set." )


                           ALIB.Modules.PushBack( &TEXT );
                           ALIB.Modules.PushBack( &RESULTS      );
    ALIB_IF_SYSTEM(        ALIB.Modules.PushBack( &SYSTEM       );   )
    ALIB_IF_CONFIGURATION( ALIB.Modules.PushBack( &CONFIG       );   )
    ALIB_IF_ALOX(          ALIB.Modules.PushBack( &ALOX         );   )
    ALIB_IF_CLI(           ALIB.Modules.PushBack( &CLI          );   )
    ALIB_IF_EXPRESSIONS(   ALIB.Modules.PushBack( &EXPRESSIONS  );   )

                           ALIB.Modules.PushBack( &ALIB ); // this!
}


// #################################################################################################
// bootstrap/shutdown implementations
// #################################################################################################



void ALibDistribution::bootstrap( BootstrapPhases phase, int argc, const char** argv, const wchar_t** wargv)
{
   if( phase == BootstrapPhases::PrepareResources )
   {
       ArgC=  argc;
       ArgVN= argv;
       ArgVW= wargv;


       // ------ add resources ------
       // enums
#if !ALIB_RESOURCES_OMIT_DEFAULTS
       resourcePool->BootstrapBulk(     ResourceCategory,

       // CodeMarker_CommonEnums
       "Alignment",         A_CHAR("0"  ","  "Left"               ","  "1" ","
                                   "1"  ","  "Right"              ","  "1" ","
                                   "2"  ","  "Center"             ","  "1"     ),

       "Bool",              A_CHAR("0"  ","  "False"              ","  "1"  ","
                                   "1"  ","  "True"               ","  "1"  ","
                                   "0"  ","  "0"                  ","  "1"  ","
                                   "1"  ","  "1"                  ","  "1"  ","
                                   "0"  ","  "No"                 ","  "1"  ","
                                   "1"  ","  "Yes"                ","  "1"  ","
                                   "0"  ","  "Off"                ","  "2"  ","
                                   "1"  ","  "On"                 ","  "2"  ","
                                   "0"  ","  "-"                  ","  "1"  ","
                                   "1"  ","  "Ok"                 ","  "2"     ),

       "Caching",           A_CHAR("0"  ","  "Disabled"           ","  "1" ","
                                   "1"  ","  "Enabled"            ","  "1" ","
                                   "1"  ","  "Auto"               ","  "1"     ),

       "Case",              A_CHAR("0"  ","  "Sensitive"          ","  "1" ","
                                   "1"  ","  "Ignore"             ","  "1"     ),

       "ContainerOp",       A_CHAR("0"  ","  "Insert"             ","  "1" ","
                                   "1"  ","  "Remove"             ","  "1" ","
                                   "3"  ","  "GetCreate"          ","  "4" ","
                                   "2"  ","  "Get"                ","  "1" ","
                                   "4"  ","  "Create"             ","  "1"     ),

       // CreateDefaults:    using Bool
       // CreateIfNotExists: using Bool

       "CurrentData",       A_CHAR("0"  ","  "Keep"               ","  "1" ","
                                   "1"  ","  "Clear"              ","  "1"     ),

       "Inclusion",         A_CHAR("0"  ","  "Include"            ","  "1" ","
                                   "1"  ","  "Exclude"            ","  "1"     ),

       "Initialization",    A_CHAR("0"  ","  "DontInitialize"     ","  "1" ","
                                   "1"  ","  "Initialize"         ","  "1" ","
                                   "0"  ","  "Suppress"           ","  "1" ","
                                   "1"  ","  "Perform"            ","  "1"     ),

       "Phase",             A_CHAR("0"  ","  "Begin"              ","  "1" ","
                                   "1"  ","  "End"                ","  "1"     ),

       "Propagation",       A_CHAR("0"  ","  "Omit"               ","  "1" ","
                                   "1"  ","  "ToDescendants"      ","  "1"     ),

       "Reach",             A_CHAR("0"  ","  "Global"             ","  "1" ","
                                   "1"  ","  "Local"              ","  "1"     ),

       "Responsibility",    A_CHAR("0"  ","  "KeepWithSender"     ","  "1" ","
                                   "1"  ","  "Transfer"           ","  "1"     ),

       "Safeness",          A_CHAR("0"  ","  "Safe"               ","  "1" ","
                                   "1"  ","  "Unsafe"             ","  "1"     ),

       "Side",              A_CHAR("0"  ","  "Left"               ","  "1" ","
                                   "1"  ","  "Right"              ","  "1"     ),

       "SortOrder",         A_CHAR("0"  ","  "Ascending"          ","  "1" ","
                                   "1"  ","  "Descending"         ","  "1"     ),

       "SourceData",        A_CHAR("0"  ","  "Copy"               ","  "1" ","
                                   "1"  ","  "Move"               ","  "1"     ),

       "Switch",            A_CHAR("0"  ","  "Off"                ","  "2" ","
                                   "1"  ","  "On"                 ","  "2"     ),

       "Timezone",          A_CHAR("0"  ","  "Local"              ","  "1" ","
                                   "1"  ","  "UTC"                ","  "1"     ),

       "Timing",            A_CHAR("0"  ","  "Async"              ","  "1" ","
                                   "1"  ","  "Sync"               ","  "1" ","
                                   "0"  ","  "Asynchronous"       ","  "1" ","
                                   "1"  ","  "Synchronous"        ","  "1" ","
                                   "1"  ","  "Synchronized"       ","  "1"     ),

       "ValueReference",    A_CHAR("0"  ","  "Absolute"           ","  "1" ","
                                   "1"  ","  "Relative"           ","  "1"     ),

       "Whitespaces",       A_CHAR("0"  ","  "Trim"               ","  "1" ","
                                   "1"  ","  "Keep"               ","  "1"     ),


       #if ALIB_THREADS
       "TSts",             A_CHAR( "0"  ","  "Unstarted"          ","  "1" ","
                                   "1"  ","  "Started"            ","  "3" ","
                                   "2"  ","  "Running"            ","  "1" ","
                                   "3"  ","  "Stopped"            ","  "3" ","
                                   "4"  ","  "Terminated"         ","  "1"      ),
       #endif


       #if ALIB_CONFIGURATION
       // variables
       "Var0",    A_CHAR("1|ALIB|LOCALE|||"),
       "Var1",    A_CHAR("2|ALIB|WAIT_FOR_KEY_PRESS|||"),
       "Var2",    A_CHAR("3|ALIB|HAS_CONSOLE_WINDOW|||"),


       "Var_C1",  A_CHAR("Defines the locale of the application. If empty or not set, the system's locale is used."),
       "Var_C2",  A_CHAR("If true, the process waits for a key stroke on termination. If empty, under Windows"   "\n"
                         "behavior is detected, under other OSes, defaults to false."                               ),
       "Var_C3",  A_CHAR("Boolean value that denotes what its name indicates. If empty, under Windows value is " "\n"
                         "detected, under other OSes, defaults to true."                                            ),

       // default values
       "Var_D2",  A_CHAR("No"),
       "Var_D3",  A_CHAR("No"),

       #endif // ALIB_CONFIGURATION

       // INI-File comment for section ALIB
       "INI_CMT_ALIB", A_CHAR("@>'/// '@HL-General ALib library settings.\n@HL-"),

       // Exit message when waiting for key-press requested with variable WAIT_FOR_KEY_PRESS
       "EXIT_MSG"    , A_CHAR( "ALIB: Press 'Enter' to exit..." ),

    #if ALIB_BITBUFFER && ALIB_ENUMS
       "ACAlgos"     , A_CHAR(  "0"    ",NONE"                 ","   "1"      ","
                               "31"    ",ALL"                  ","   "1"      ","
                                "1"    ",Uncompressed"         ","   "1"      ","
                                "2"    ",MinMax"               ","   "1"      ","
                                "4"    ",Sparse"               ","   "1"      ","
                                "8"    ",VerySparse"           ","   "1"      ","
                               "16"    ",Incremental"          ","   "1"      ","
                               "32"    ",Huffman"              ","   "1"            ),
    #endif
       // end of BootstrapBulk()
       nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS

    #if ALIB_BITBUFFER && ALIB_BOXING && ALIB_ENUMS
       ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( aworx::lib::bitbuffer::ac_v1::ArrayCompressor::Algorithm  )
    #endif

   }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        ALIB_IF_CONFIGURATION( EnumRecords<Variables>::Bootstrap('|'); )

#if ALIB_BITBUFFER
        EnumRecords<aworx::lib::bitbuffer::ac_v1::ArrayCompressor::Algorithm>::Bootstrap( *this, "ACAlgos"  );
#endif

        // CodeMarker_CommonEnums
        EnumRecords<aworx::lib::Alignment        >::Bootstrap( *this, "Alignment"      );
        EnumRecords<aworx::lib::Bool             >::Bootstrap( *this, "Bool"           );
        EnumRecords<aworx::lib::Caching          >::Bootstrap( *this, "Caching"        );
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_22] )
        EnumRecords<aworx::lib::Case>::Bootstrap( *this, "Case"  );
DOX_MARKER( [DOX_ALIB_ENUMS_MAKE_PARSABLE_22] )
        EnumRecords<aworx::lib::ContainerOp      >::Bootstrap( *this, "ContainerOp"    );
        EnumRecords<aworx::lib::CreateDefaults   >::Bootstrap( *this, "Bool"           );
        EnumRecords<aworx::lib::CreateIfNotExists>::Bootstrap( *this, "Bool"           );
        EnumRecords<aworx::lib::CurrentData      >::Bootstrap( *this, "CurrentData"    );
        EnumRecords<aworx::lib::Inclusion        >::Bootstrap( *this, "Inclusion"      );
        EnumRecords<aworx::lib::Initialization   >::Bootstrap( *this, "Initialization" );
        EnumRecords<aworx::lib::Phase            >::Bootstrap( *this, "Phase"          );
        EnumRecords<aworx::lib::Propagation      >::Bootstrap( *this, "Propagation"    );
        EnumRecords<aworx::lib::Reach            >::Bootstrap( *this, "Reach"          );
        EnumRecords<aworx::lib::Responsibility   >::Bootstrap( *this, "Responsibility" );
        EnumRecords<aworx::lib::Safeness         >::Bootstrap( *this, "Safeness"       );
        EnumRecords<aworx::lib::Side             >::Bootstrap( *this, "Side"           );
        EnumRecords<aworx::lib::SortOrder        >::Bootstrap( *this, "SortOrder"      );
        EnumRecords<aworx::lib::SourceData       >::Bootstrap( *this, "SourceData"     );
        EnumRecords<aworx::lib::Switch           >::Bootstrap( *this, "Switch"         );
        EnumRecords<aworx::lib::Timezone         >::Bootstrap( *this, "Timezone"       );
        EnumRecords<aworx::lib::Timing           >::Bootstrap( *this, "Timing"         );
        EnumRecords<aworx::lib::ValueReference   >::Bootstrap( *this, "ValueReference" );
        EnumRecords<aworx::lib::Whitespaces      >::Bootstrap( *this, "Whitespaces"    );

#if ALIB_THREADS
        // Enums from non-resourced modules
        EnumRecords<aworx::lib::threads::Thread::State    >::Bootstrap( *this, "TSts"        );
#endif
    }


   else if( phase == BootstrapPhases::Final )
   {
        ALIB_IF_CONFIGURATION( Variable variable; )

        //############### set locale ###############
        int receivedFrom= 0;
        String256 locale;
        #if ALIB_CONFIGURATION
            variable.Declare( Variables::LOCALE );
            if (     config->Load( variable ) != Priorities::NONE
                 &&  variable.GetString().IsNotEmpty()        )
            {
                receivedFrom= 1;
                locale._( variable.GetString() );
            }
            else
        #endif

        ALIB_IF_SYSTEM(
                 if ( EnvironmentVariables::Get( A_CHAR("LANG")      ,locale ) )        receivedFrom= 2;
            else if ( EnvironmentVariables::Get( A_CHAR("LANGUAGE")  ,locale ) )        receivedFrom= 3;  )

        if( receivedFrom > 0 && !locale.Equals<Case::Ignore>( A_CHAR("none") ) )
        {
            ALIB_STRINGS_TO_NARROW( locale, nLocale, 1024)
            if( !setlocale(LC_ALL, nLocale ) )
            {
                NString256 msg( "ALib Error: setlocale(\""); msg << nLocale <<"\") failed. Setting read from ";
                msg << (   receivedFrom == 1 ? "config variable 'ALIB_LOCALE'"   :
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
            else
                LocaleFound= monomem::GlobalAllocator.EmplaceString(locale);
        }

        // set the system's locale as the default for our static default number format
        #if ALIB_STRINGS
            NumberFormat::Global.SetFromLocale();
            NumberFormat::Global.WriteGroupChars= true;
        #endif


        // --- determine if we want to wait for a keypress upon termination ---
        #if ALIB_CONFIGURATION
            variable.Declare( Variables::WAIT_FOR_KEY_PRESS );
            config->Load( variable );
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
        #if ALIB_CONFIGURATION
            variable.Declare( Variables::HAS_CONSOLE_WINDOW );
            config->Load( variable );
            if ( variable.Size() > 0  && variable.GetString().Length() > 0 )
                HasConsoleWindow=  variable.IsTrue();
            else
        #endif // ALIB_CONFIGURATION
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
            ALIB_IF_EXPRESSIONS  ( assert( lib::EXPRESSIONS.IsBootstrapped( ) ); )
            ALIB_IF_CLI          ( assert( lib::CLI        .IsBootstrapped( ) ); )
            ALIB_IF_ALOX         ( assert( lib::ALOX       .IsBootstrapped( ) ); )
            ALIB_IF_CONFIGURATION( assert( lib::CONFIG     .IsBootstrapped( ) ); )
            ALIB_IF_SYSTEM       ( assert( lib::SYSTEM     .IsBootstrapped( ) ); )
            ALIB_IF_RESULTS      ( assert( lib::RESULTS    .IsBootstrapped( ) ); )
            ALIB_IF_TEXT ( assert( lib::TEXT.IsBootstrapped( ) ); )
        #endif // ALIB_DEBUG

        // Set global debug flag about ALib's bootstrap
        #if ALIB_DEBUG
            DbgIsBootstrapped= true;
        #endif
   }
}


void ALibDistribution::shutdown( ShutdownPhases phase )
{
    if( phase == ShutdownPhases::Announce )
    {
        #if ALIB_CONFIGURATION
            IniFile::AddResourcedSectionComments( GetConfig(),
                                                  GetResourcePool(), ResourceCategory, "INI_CMT_" );
        #endif
        return;
    }

    if ( ALIB.WaitForKeyPressOnTermination )
    {
        #if defined(_WIN32)
            OutputDebugStringA( "\r\nALIB: Waiting for 'Enter' key in the console window.\r\n" );
        #endif

        std::cout << std::endl << GetResource("EXIT_MSG") << std::endl;
        while ( getchar() != '\n' )
            ;
    }

    // Set global debug flag about ALib's bootstrap
    #if ALIB_DEBUG
        DbgIsBootstrapped= false;
    #endif

}


// #############################################################################################
// Environment definition/detection
// #############################################################################################

#if ALIB_SYSTEM

//! @cond NO_DOX
   #if defined(__unix__) || defined(__APPLE__)
   namespace {
       String      DebuggerProcessNames= A_CHAR("gdb|debugserver");
   }
   #endif
//! @endcond

   bool ALibDistribution::IsDebuggerPresent()
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

#endif // ALIB_SYSTEM


// #############################################################################################
// CheckDistribution
// #############################################################################################
void ALibDistribution::CheckDistribution( int alibVersion, uint64_t compilationFlags )
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
       std::cout << "!!! Error in ALib library compilation: linked library of ALib has different compiler symbols set." << std::endl;
       exit(-1);
   }
}


}} // namespace [aworx::lib]


/**
 * \dir alib
 * This is the root folder of all source files of \alib.
 */

/**
 * \dir alib/alox
 * This directory contains header files and compilation units of module \alib_alox.
 */

/**
 * \dir alib/alox/detail
 * This directory contains header files and compilation units of module \alib_alox.
 */

/**
 * \dir alib/alox/detail/textlogger
 * This directory contains header files and compilation units of module \alib_alox.
 */

/**
 * \dir alib/alox/loggers
 * This directory contains header files and compilation units of module \alib_alox.
 */

/**
 * \dir alib/bitbuffer
 * This directory contains header files and compilation units of module \alib_bitbuffer.
 */

/**
 * \dir alib/bitbuffer/ac_v1
 * This directory contains detail header files and compilation units of module \alib_bitbuffer.
 */

/**
 * \dir alib/boxing
 * This directory contains header files and compilation units of module \alib_boxing.
 */

/**
 * \dir alib/boxing/detail
 * This directory contains detail header files and compilation units of module \alib_boxing.
 */

/**
 * \dir alib/characters
 * This directory contains header files and compilation units of module \alib_characters.
 */

/**
 * \dir alib/cli
 * This directory contains header files and compilation units of module \alib_cli.
 */

/**
 * \dir alib/compatibility
 * This directory contains optional header files that provide compatibility between
 * \alib types and concepts and those of the C++ standard library as well as 3rd party libraries.
 */

/**
 * \dir alib/config
 * This directory contains header files and compilation units of module \alib_config.
 */

/**
 * \dir alib/enums
 * This directory contains header files and compilation units of module \alib_enums.
 */

/**
 * \dir alib/enums/detail
 * This directory contains detail header files and compilation units of module \alib_enums.
 */

/**
 * \dir alib/expressions
 * This directory contains header files and compilation units of module \alib_expressions.
 */

/**
 * \dir alib/expressions/detail
 * This directory contains detail header files and compilation units of module \alib_expressions.
 */

/**
 * \dir alib/expressions/plugins
 * This directory contains header files and compilation units of module \alib_expressions.
 */

/**
 * \dir alib/expressions/util
 * This directory contains header files and compilation units of module \alib_expressions.
 */

/**
 * \dir alib/lib
 * This directory contains source files of \ref alib_manual_modules_filesets "file sets" and
 * other general (module independent) sources of \aliblong.
 */

/**
 * \dir alib/lib
 * This directory contains headers of \alib that mostly concern preprocessor definitions.
 * In addition, several sub-directories reside here, which correspond to the different
 * \ref alib_manual_modules_filesets "ALib Filesets".
 */

/**
 * \dir alib/lib/fs_commonenums
 * This directory contains files contained in \ref alib_manual_modules_filesets "fileset" "Common Enums".
 * The availability of this directory depends on the \alibdist and can be tested with
 * code selection symbol ALIB_FILESET_COMMON_ENUMS.
 */

/**
 * \dir alib/lib/fs_lists
 * This directory contains files contained in \ref alib_manual_modules_filesets "fileset" "Lists".
 * The availability of this directory depends on the \alibdist and can be tested with
 * code selection symbol ALIB_FILESET_LISTS.
 */

/**
 * \dir alib/lib/fs_modules
 * This directory contains files contained in \ref alib_manual_modules_filesets "fileset" "Modules".
 * The availability of this directory depends on the \alibdist and can be tested with
 * code selection symbol ALIB_FILESET_MODULES.
 */

/**
 * \dir alib/lib/fs_owner
 * This directory contains files contained in \ref alib_manual_modules_filesets "fileset" "Owner".
 * The availability of this directory depends on the \alibdist and can be tested with
 * code selection symbol ALIB_FILESET_OWNER.
 */

/**
 * \dir alib/lib/fs_plugins
 * This directory contains files contained in \ref alib_manual_modules_filesets "fileset" "Plugins".
 * The availability of this directory depends on the \alibdist and can be tested with
 * code selection symbol ALIB_FILESET_PLUGINS.
 */

/**
 * \dir alib/monomem
 * This directory contains header files and compilation units of module \alib_monomem.
 */

/**
 * \dir alib/monomem/detail
 * This directory contains detail header files and compilation units of module \alib_monomem.
 */

/**
 * \dir alib/monomem/util
 * This directory contains header files and compilation units of utility types of module \alib_monomem.
 */

/**
 * \dir alib/resources
 * This directory contains header files and compilation units of module \alib_resources.
 */

/**
 * \dir alib/resources/detail
 * This directory contains detail header files and compilation units of module \alib_resources.
 */

/**
 * \dir alib/results
 * This directory contains header files and compilation units of module \alib_results.
 */

/**
 * \dir alib/singletons
 * This directory contains header files and compilation units of module \alib_singletons.
 */

/**
 * \dir alib/strings
 * This directory contains header files and compilation units of module \alib_strings.
 */

/**
 * \dir alib/strings/detail
 * This directory contains detail header files and compilation units of module \alib_strings.
 */

/**
 * \dir alib/strings/util
 * This directory contains header files and compilation units of utility types of module \alib_strings.
 */

/**
 * \dir alib/system
 * This directory contains header files and compilation units of module \alib_system.
 */

/**
 * \dir alib/text
 * This directory contains header files and compilation units of module \alib_text.
 */

/**
 * \dir alib/threads
 * This directory contains header files and compilation units of module \alib_threads.
 */

/**
 * \dir alib/threads/detail
 * This directory contains detail header files and compilation units of module \alib_threads.
 */

/**
 * \dir alib/time
 * This directory contains header files and compilation units of module \alib_time.
 */

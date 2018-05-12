// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"


#if !defined (HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
    #include "alib/compatibility/std_iostream.hpp"
#endif

#if ALIB_MODULE_STRINGS
    #if !defined (HPP_ALIB_STRINGS_UTIL_SPACES)
        #include "alib/strings/util/spaces.hpp"
    #endif
#endif

#if ALIB_MODULE_CONFIGURATION

    #if !defined (HPP_ALIB_CONFIG_LIB)
    #   include "alib/config/configlib.hpp"
    #endif
    #if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #   include "alib/config/configuration.hpp"
    #endif
    #if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
    #   include "alib/system/environment.hpp"
    #endif
#endif

#if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
    #if !defined (HPP_ALIB_TIME_LIB)
    #   include "alib/time/timelib.hpp"
    #endif
    #if !defined (HPP_ALIB_TIME_TIMEPOINT)
    #   include "alib/time/timepointbase.hpp"
    #endif

    #if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
    #   include "alib/system/process.hpp"
    #endif
#endif


#if ALIB_MODULE_STRINGS
    #if !defined (HPP_ALIB_STRINGS_NUMBERFORMAT)
        #include "alib/strings/numberformat.hpp"
    #endif
#endif


#if ALIB_MODULE_CORE
    #if !defined (HPP_ALIB_THREADS_SMARTLOCK)
        #include "alib/threads/smartlock.hpp"
    #endif

    #if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
        #include "alib/strings/util/tokenizer.hpp"
    #endif
#endif

#if ALIB_MODULE_EXPRESSIONS
    #if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONSLIB)
        #include "alib/expressions/expressionslib.hpp"
    #endif
#endif

#if ALIB_MODULE_ALOX
    #if !defined (HPP_ALIB_ALOX)
        #include "alib/alox/alox.hpp"
    #endif
#endif

#if ALIB_MODULE_CLI
    #if !defined (HPP_ALIB_CLI_LIB)
        #include "alib/cli/clilib.hpp"
    #endif
#endif


#if !defined (_GLIBCXX_THREAD) && !defined (_THREAD_ )
    #include <thread>
#endif

namespace aworx { namespace lib {

#if ALIB_MODULE_STRINGS

// #################################################################################################
// Static instance and constructor
// #################################################################################################
ALib                 ALIB;

ALib::ALib()
: Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB"), ALIB_COMPILATION_FLAGS )
#if ALIB_MODULE_CORE
, StdOutputStreamsLock(* new threads::SmartLock() )
#endif
{
    CompilationFlagMeanings=
    {
        { "ALIB_DEBUG"                  , ALIB_DEBUG_VFYBIT                     },
        { "ALIB_DEBUG_STRINGS"          , ALIB_DEBUG_STRINGS_VFYBIT             },

        { "ALIB_FEAT_THREADS"           , ALIB_FEAT_THREADS_VFYBIT              },
        { "ALIB_FEAT_SINGLETON_MAPPED"  , ALIB_FEAT_SINGLETON_MAPPED_VFYBIT     },
        { "ALIB_FEAT_BOXING_FTYPES"     , ALIB_FEAT_BOXING_FTYPES_VFYBIT        },
        { "ALIB_FEAT_BOXING_STD_VECTOR" , ALIB_FEAT_BOXING_STD_VECTOR_VFYBIT    },
    };

    //############### initialize threads and time ################
    #if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
        dependencies.emplace_back( &THREADS );
        dependencies.emplace_back( &TIME    );
    #endif

    #if ALIB_MODULE_BOXING
        dependencies.emplace_back( &BOXING  );
    #endif
    #if ALIB_MODULE_STRINGS
        dependencies.emplace_back( &STRINGS );
    #endif


    dependencies.emplace_back( &LANG );

    #if ALIB_MODULE_CORE
        dependencies.emplace_back( &SYSTEM );
    #endif

    #if ALIB_MODULE_CONFIGURATION
        dependencies.emplace_back( &CONFIG );
    #endif

    #if ALIB_MODULE_ALOX
        dependencies.emplace_back( &lib::ALOX );
    #endif

    #if ALIB_MODULE_EXPRESSIONS
        dependencies.emplace_back( &lib::EXPRESSIONS );
    #endif

    #if ALIB_MODULE_CLI
        dependencies.emplace_back( &lib::CLI );
    #endif
}


// #################################################################################################
// Environment definition/detection
// #################################################################################################

#if ALIB_MODULE_CORE

//! @cond NO_DOX
    #if defined(__unix__) || defined(__APPLE__)
    namespace {
        String      DebuggerProcessNames= ASTR("gdb|debugserver");
    }
    #endif
//! @endcond

    bool ALib::IsDebuggerPresent()
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

#endif // ALIB_MODULE_CORE

#if ALIB_MODULE_CONFIGURATION
    bool ALib::HasConsoleWindow()
    {
        // read configuration
        bool returnValue;
        Variable variable( Variables::HAS_CONSOLE_WINDOW );
        Config->Load( variable );
        if ( variable.Size() > 0  && variable.GetString()->Length() > 0 )
            returnValue=  variable.IsTrue();
        else

        #if defined(_WIN32)
            // determine if we have a console window
            returnValue= GetConsoleWindow() != NULL;
        #else
            returnValue= true;
        #endif
        return returnValue;
    }
#endif // ALIB_MODULE_CONFIGURATION



// #################################################################################################
// Init/ TerminationCleanUp()
// #################################################################################################

void ALib::init( Phases phase )
{
    if( phase == Phases::resourceset )
    {
        Res->AddBulk( ResourceCategory.ToCString(),


        ASTR("Var0"),    ASTR("1|ALIB|LOCALE|ES||||VC1"),
        ASTR("Var1"),    ASTR("2|ALIB|WAIT_FOR_KEY_PRESS|VD2||||VC2"),
        ASTR("Var2"),    ASTR("3|ALIB|HAS_CONSOLE_WINDOW|ES||||VC3"),


        ASTR("VC1"),
        ASTR("Defines the locale of the application. If empty or not set, the system's locale is used.")
        ,
        ASTR("VC2"),
        ASTR("If true, the process waits for a key stroke on termination. If empty, under Windows"   "\n"
             "behavior is detected, under other OSes, defaults to false."                               )
        ,
        ASTR("VC3"),
        ASTR("Boolean value that denotes what its name indicates. If empty, under Windows value is " "\n"
             "detected, under other OSes, defaults to true."                                            )
        ,

        // Empty string. This is set with variables that want to be written into blank files.
        ASTR("ES"),   ASTR(""),

        // default values
        ASTR("VD2"),  ASTR(""),


        // end of AddBulk()
        nullptr );
    }

    else if( phase == Phases::final )
    {
        //############### set locale ###############
        #if ALIB_MODULE_CONFIGURATION
        Variable variable;
        #endif

        #if ALIB_MODULE_CONFIGURATION && ( defined (__GLIBCXX__)  || defined(__APPLE__) )
        {
            int receivedFrom= 0;
            variable.Declare( Variables::LOCALE );
            AString locale;
            if (     Config->Load( variable ) != Priorities::NONE
                 &&  variable.GetString()->IsNotEmpty()        )
            {
                receivedFrom= 1;
                locale._( variable.GetString() );
            }
            else if ( system::GetEnvironmentVariable( ASTR("LANG")      ,locale ) )        receivedFrom= 2;
            else if ( system::GetEnvironmentVariable( ASTR("LANGUAGE")  ,locale ) )        receivedFrom= 3;

            if( receivedFrom > 0 && !locale.Equals<Case::Ignore>( ASTR("none") ) )
            {
                ALIB_STD_TO_NARROW_TSTRING( locale, nLocale)
                if( !setlocale(LC_ALL, nLocale.ToCString() ) )
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
                    WaitForKeyPressOnTermination=  ALib::HasConsoleWindow() && ALib::IsDebuggerPresent();
                #else
                    WaitForKeyPressOnTermination=  false;
                #endif
            }
        #else
                #if defined(_WIN32) && ALIB_DEBUG
                    WaitForKeyPressOnTermination=  ALib::HasConsoleWindow() && ALib::IsDebuggerPresent();
                #else
                    WaitForKeyPressOnTermination=  false;
                #endif
        #endif
    }
}


void ALib::terminationCleanUp()
{
    if ( ALIB.WaitForKeyPressOnTermination )
    {
        #if defined(_WIN32)
            OutputDebugStringA( "\r\nALIB: Waiting for 'Enter' key in the console window." \
                                "\r\n     (To disable this, set 'ALIB.WaitForKeyPressOnTermination' to 'false'.)\r\n" );
        #endif

        std::cout << std::endl << "ALIB: Press 'Enter' to exit... " \
            "      (To disable this, set 'ALIB.WaitForKeyPressOnTermination to 'false'.)" << std::endl;
        while ( getchar() != '\n' )
            ;
    }


    // remove singletons
    lang::DeleteSingletons();

    // and the cout/cerr lock
    #if ALIB_MODULE_CORE
        delete &StdOutputStreamsLock;
    #endif
}


void ALib::CheckCompatibility( int alibVersion, volatile uint64_t compilationFlags )
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

// #################################################################################################
// SleepX methods
// #################################################################################################

// usleep() (for _WIN32)
#if !ALIB_FEAT_THREADS
    #if defined( __GLIBCXX__ )
        #include <unistd.h>
    #elif defined ( _WIN32 )
        void usleep(__int64 usec)
        {
            HANDLE timer;
            LARGE_INTEGER ft;

            ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

            timer = CreateWaitableTimer(NULL, TRUE, NULL);
            SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
            WaitForSingleObject(timer, INFINITE);
            CloseHandle(timer);
        }
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
#endif

void ALib::SleepMillis( int milliseconds )
{
    #if ALIB_FEAT_THREADS
        std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
    #else
        usleep(static_cast<unsigned int>( milliseconds * 1000 ) );
    #endif
}

void ALib::SleepMicros( int64_t microseconds )
{
    #if ALIB_FEAT_THREADS
        std::this_thread::sleep_for( std::chrono::microseconds( microseconds ) );
    #else
        usleep( static_cast<unsigned int>( microseconds ));
    #endif
}

void ALib::SleepNanos( int64_t nanoseconds )
{
    #if ALIB_FEAT_THREADS
        std::this_thread::sleep_for( std::chrono::nanoseconds( nanoseconds ) );
    #else
        int micros= nanoseconds / 1000;
        usleep( static_cast<unsigned int>( micros > 0 ? micros : 1 ));
    #endif
}

#endif //ALIB_MODULE_STRINGS

}} // namespace [aworx::lib]


// #################################################################################################
//  Doxygen documentation of general macros. (All gathered in one place for technical reasons.)
// #################################################################################################
/**
 * @addtogroup GrpALibMacros
 * @{
 * @name Macros Supporting Various ALib Concepts
 * The macros listed here have a direct relationship with classes defined in \alib
 * and with their use.
 * @{
 *
 * \def  ALIB_OWN
 * This preprocessor macro defines an object \alib{lang,Owner}. The template type is deduced
 * from parameter \p{ownable} using C++ keyword \c decltype.
 *
 * By using this macro there is no need to "invent" an (otherwise unreferenced) identifier for
 * that definition. (Note: this macro exists, because C++ does not support anonymous local
 * instances.)<br>
 * As a sample, without using this macros a piece of code code using class \alib{lang,Owner}
 * could look like this:
 *
 *      {
 *          Owner<MyOwnerType> myOwner( myOwnable );
 *
 *          // do stuff
 *          ...  (this code never refers to "myOwner")
 *      }
 *
 *  With the use of this macro, the code changes to:<br>
 *
 *      {
 *          ALIB_OWN( myOwnable ); // internally created identifier name hidden not shown
 *
 *          // do stuff
 *          ...
 *      }
 *
 * \see Macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH are using this macro to lock
 *      types \alib{threads,ThreadLockNR} and \alib{threads,ThreadLock}.<br>
 *      Macro \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS implements a mechanism to detect and assert
 *      recursive function calls.
 *
 *  @param ownable   The \b %Ownable to acquire and release.
 *
 *
 * \def  ALIB_LOCK
 * Shortcut to macro \ref ALIB_OWN, providing <c>*this</c> as the owner. Usually used with
 * methods of types derived from classes \alib{threads,ThreadLock} or \alib{threads,ThreadLockNR}
 *
 *
 * \def  ALIB_LOCK_WITH
 * Alias name for macro \ref ALIB_OWN to have better readable code if parameter \p{owner} of macro
 * \b %ALIB_OWN is of type \alib{threads,ThreadLock} or \alib{threads,ThreadLockNR}.
 *
 *  @param lock   The \b %Ownable to acquire and release.
 *
 * \def  ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS
 * This macro may be placed at the beginning of a function to detect and assert recursive
 * calls of that function. It is useful when recursion might happen, but must not.<br>
 * The internally uses macro \ref ALIB_OWN and hence class \alib{lang,Owner} that uses C++ stack
 * unwinding to be sure to always clear the flag, even if an exception occurs.
 *
 * In the case that the macro is placed in more than one method, it can be used to assure that none
 * of he methods calls any of the others.
 *
 * A prerequisite of the use of the macro is to have macro
 * \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL placed in the member declaration area of the type.
 *
 * This macro is only active in debug-compilations.
 *
 * \def  ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS_MEMBER_DECL
 * Used with macro \ref ALIB_DBG_PREVENT_RECURSIVE_METHOD_CALLS, usually in header compilation unit
 * to declare a boolean class member that is used for detecting recursions.
 *
 *
 *
 * \def  ALIB_LANG_RESOURCED
 *   Macro used to specialize TMP struct \alib{lang,T_Resourced,T_Resourced<TResourced>}
 *   for given type \p{TResourced}.
 *
 *   This macro is for example used with \alib{lang,T_EnumMetaDataDecl,ALib enum meta data }.
 *
 *   @param TResourced      The type to specialize TMP struct \b %T_Resourced for.
 *   @param ResourceLibrary A reference to the library the type is associated with.
 *   @param NameString      The name of the resources.
 *
 *
 *
 * \def  ALIB_LANG_EXCEPTIONS
 *   Macro used to declare enumeration type \p{TEnum} to be compatible with class
 *   \b %Exception.
 *
 *   \see Class \alib{lang,Exception} for details.
 *
 *   @param TEnum            The enumeration type that is to be declared as an error code compatible
 *                           with class \alib{lang,Exception}.
 *   @param ResourceLibrary  A reference to the \alib{lang,Library} the enum meta data is loaded
 *                           from.
 *   @param ResourceName     The resource name of the meta information of the \p{TEnum}.
 *
 *
 *
 * \def  ALIB_CONFIG_VARIABLES
 *   Macro used to declare enumeration type \p{TEnum} to denote
 *   \ref aworx::lib::config "ALib Configuration Variables".
 *   In particular, the macro includes the following code:
 *   - Specializes TMP struct \alib{lang,T_EnumMetaDataDecl} for type \p{TEnum} and associate
 *     type \alib{config,VariableDecl::TTuple} for its meta data.
 *   - Passes parameters \p{ResourceLibrary} and \p{ResourceName} to macro \ref ALIB_LANG_RESOURCED.
 *   - Specializes method \alib{lang,EnumMetaData::CheckLoad} to use a detail method
 *     for loading the meta data from the resources.
 *
 *   With this - and the corresponding resource data! - in place, elements of \p{TEnum} can be
 *   used to declare configuration variables by passing them to one of following constructors and
 *   methods:
 *   - \alib{config,VariableDecl::VariableDecl(TEnum)}
 *   - \alib{config,Variable::Variable(TEnum)}
 *   - \alib{config,Variable::Variable(TEnum),Variable::Variable(TEnum\,const StringTypes&...)}
 *   - \alib{config,Variable::Declare(TEnum)}
 *   - \alib{config,Variable::Declare(TEnum),Variable::Declare(TEnum\,const StringTypes&...)}
 *
 *   The resource data is a table of values separated by character <b>'|'</b> ("pipe" character), which
 *   has to contain the eight values of tuple type \alib{config,VariableDecl::TTuple}. Elements
 *   \c 3 ("DefaultValue") and \c 7 ("Comments"), in the resources are interpreted as a
 *   resource name. This way, both values are loaded from separated resource strings.
 *   This has the following advantages:
 *   - The values may contain the separation character <b>'|'</b>.
 *   - The values can be manipulated within the resources more easily.
 *
 *   Note that for loading the resources, static template method
 *   \alib{lang,ResourcedTupleLoader::LoadTable} is used.
 *   As documented, this method (optionally) allows to separate each variable declaration entry
 *   into an own, numbered resource string.
 *
 *
 *   \see Namespace documentation \ref aworx::lib::config for information on configuration
 *        data and variables.
 *
 *   @param TEnum            The enumeration type that is to be declared for denoting configuration
 *                           variables.
 *   @param ResourceLibrary  A reference to the \alib{lang,Library} the enum meta data is loaded
 *                           from.
 *   @param ResourceName     The resource name of the meta information of the \p{TEnum}.
 *
 * @}
 * @} */ // GrpALibMacros

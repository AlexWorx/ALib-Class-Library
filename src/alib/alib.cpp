// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined(HPP_ALIB_LANG_PLATFORM_INCLUDES) && !defined(ALIB_DOX)
#      include "alib/lang/platformincludes.hpp"
#   endif
#   if ALIB_CAMP && !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   if ALIB_SINGLETONS && !defined (HPP_ALIB_SINGLETONS_SINGLETON)
#      include "alib/singletons/singleton.hpp"
#   endif
#   if ALIB_BOXING && !defined (HPP_ALIB_BOXING_BOXING)
#      include "alib/boxing/boxing.hpp"
#   endif
#   if ALIB_TIME && !defined (HPP_ALIB_TIME_TIME)
#      include "alib/time/time.hpp"
#   endif
#   if ALIB_ENUMS && !defined (HPP_ALIB_ENUMS_RECORDS)
#      include "alib/enums/records.hpp"
#   endif

#   if ALIB_ALOX && !defined (HPP_ALIB_ALOXMODULE)
#      include "alib/alox/aloxmodule.hpp"
#   endif
#   if ALIB_CONFIGURATION && !defined (HPP_ALIB_CONFIG_CONFIG)
#      include "alib/config/config.hpp"
#   endif
#   if ALIB_CLI && !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/cli.hpp"
#   endif
#   if ALIB_EXPRESSIONS && !defined (HPP_ALIB_EXPRESSIONS_EXPRESSIONS)
#      include "alib/expressions/expressions.hpp"
#   endif
#   if ALIB_FILES && !defined (HPP_ALIB_FILES_CAMP)
#      include "alib/files/filescamp.hpp"
#   endif

#   if ALIB_CAMP && !defined (HPP_ALIB_LANG_CAMP_BOOTSTRAP)
#      include "alib/lang/basecamp/bootstrap.hpp"
#   endif
#endif // !defined(ALIB_DOX)

#   include <iostream>
#   include <iomanip>


// #################################################################################################
// Boostrapping & Shutdown
// #################################################################################################
namespace alib {

int                 ArgC = 0;
const char**        ArgVN= nullptr;
const wchar_t**     ArgVW= nullptr;

bool                NonCampModulesInitialized     = false;

int                 Version             = ALIB_VERSION;
unsigned char       Revision            = ALIB_REVISION;
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
TCompilationFlags   CompilationFlags    = {ALIB_COMPILATION_FLAGS};
ALIB_WARNINGS_RESTORE


void     Bootstrap( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    #if ALIB_CAMP
        if(Camps.IsEmpty())
            BootstrapAddDefaultCamps();
        Bootstrap( BootstrapPhases::Final );
    #else
        if( !NonCampModulesInitialized )
        {
            ALIB_IF_TIME(       time::Bootstrap(); )
            ALIB_IF_BOXING(   boxing::Bootstrap(); )
            ALIB_IF_THREADS( threads::Bootstrap(); )
            ALIB_IF_ENUMS(     enums::Bootstrap(); )
            NonCampModulesInitialized= true;
        }
    #endif
 }


void Shutdown()
{
    #if ALIB_CAMP
        Shutdown(ShutdownPhases::Destruct);
    #else
        ALIB_IF_THREADS(       alib::threads::Shutdown(); )
        ALIB_IF_TIME(             alib::time::Shutdown(); )
        ALIB_IF_SINGLETONS( alib::singletons::Shutdown();   )
    #endif
 }

CompilationFlagMeaningsEntry CompilationFlagMeanings[30]=
          { {  0, "DEBUG"                            },
            {  1, "MONOMEM"                          },
            {  2, "SINGLETONS"                       },
            {  3, "CHARACTERS"                       },
            {  4, "ENUMS"                            },
            {  5, "TIME"                             },
            {  6, "BOXING"                           },
            {  7, "STRINGS"                          },
            {  8, "BITBUFFER"                        },
            {  9, "THREADS"                          },
            { 10, "CAMP"                              },
            { 11, "ALOX"                             },
            { 12, "CLI"                              },
            { 13, "CONFIGURATION"                    },
            { 14, "EXPRESSIONS"                      },
            { 15, "FILES"                            },
            { 16, "FEAT_SINGLETON_MAPPED"            },
            { 17, "CHARACTERS_WIDE"                  },
            { 18, "CHARACTERS_WCHAR_IS_4"            },
            { 19, "FEAT_BOXING_BIJECTIVE_INTEGRALS"  },
            { 20, "FEAT_BOXING_BIJECTIVE_CHARACTERS" },
            { 21, "FEAT_BOXING_BIJECTIVE_FLOATS"     },
            { 22, "DEBUG_BOXING"                     },
            { 23, "DEBUG_STRINGS"                    },
            { 24, "DEBUG_MONOMEM"                    },
            { 25, "DEBUG_RESOURCES"                  },
            { 26, "DBG_LOG"                          },
            { 27, "DBG_LOG_CI"                       },
            { 28, "REL_LOG"                          },
            { 29, "REL_LOG_CI"                       },
          };


ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void AssertALibVersionAndFlags( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify requested ALib version is met
    if (Version != alibVersion || Revision != alibRevision )
    {
        std::cerr << "!!! Error in ALox library compilation: linked against wrong version of ALib" << std::endl;
        std::cerr << "!!! ALib library version:   " <<     Version   << "R" << int(Revision) << std::endl;
        std::cerr << "!!! ALib requested version: " << alibVersion   << "R" << alibRevision << std::endl;
        std::cerr << "!!! Exiting with exit(-1)" << std::endl;
        exit(1);
    }

    // verify the flags
    if (    compilationFlags.bits[0] == CompilationFlags.bits[0]
         && compilationFlags.bits[1] == CompilationFlags.bits[1]
         && compilationFlags.bits[2] == CompilationFlags.bits[2]
         && compilationFlags.bits[3] == CompilationFlags.bits[3] )
        return;

    std::cerr << "!!! Error in ALib library compilation: linked library of ALib has "
                 "different compiler symbols set than the using executable (or library)." << std::endl;

    // dump out the flags
    std::cerr << std::left << std::setw(35) <<  "Symbol" << '|' << std::setw(5) << " Lib" <<'|' << " Comp. Unit"  << std::endl;
    std::cerr<< std::setw(62) <<  std::setfill('-') << ' ' << std::endl <<  std::setfill(' ');
    
    for( auto& p : CompilationFlagMeanings )
    {
        bool libFlag= (CompilationFlags.bits[p.Flag/8] & (1 << (p.Flag % 8))) != 0;
        bool reqFlag= (compilationFlags.bits[p.Flag/8] & (1 << (p.Flag % 8))) != 0;

        std::cerr << std::setw(35) <<  p.Name << '|' << std::setw(5) << (libFlag  ? " On" : " Off")
                                              << "|" << std::setw(5) << (reqFlag  ? " On" : " Off")
                  << ( libFlag != reqFlag ? " <-- Mismatch! " : "")
                  << std::endl;
    }

    // we just quit!
    std::cerr << "!!! Exiting with exit(-1)" << std::endl;
    exit(1);
}
ALIB_WARNINGS_RESTORE


} // namespace [alib]

// #################################################################################################
// Debug functions
// #################################################################################################



#if (ALIB_DEBUG && !ALIB_THREADS && ALIB_EXT_LIB_THREADS_AVAILABLE) || defined(ALIB_DOX)

#   include <thread>

    namespace alib {

#   if !defined(ALIB_DOX)
        namespace  { std::thread::id  threadSeen;
                     bool             inSingleThreadedCheck= false; }
#   endif


    /** ********************************************************************************************
     * This method stores the first thread that invokes it, and if in the future the method is
     * visited by a different thread, it asserts.
     *
     * In release compilations, this function is inlined and empty and therefore has not necessarily
     * be removed by preprocessor macro \ref ALIB_DBG or otherwise.
     *
     * In debug-compilations, this is not empty, if:
     * 1. Module \alib_threads is not included in the \alibdist (what disables thread-safeness
     *    throughout the library), and
     * 2. Compiler symbol \ref ALIB_EXT_LIB_THREADS_AVAILABLE was passed on library compilation,
     *    which allows to use the C++ thread library without causing linker failures.
     *
     * If given, this function is called by macros \ref ALIB_LOCK and \ref ALIB_LOCK_WITH, which
     * otherwise are defined by module \alib_threads_nl. These macros will still be defined
     * in the absence of that module. This exclamatory approach was made with \alib to motivate to
     * write code that compiles with or without multi-threading support.
     *
     * In addition to the invocation by these "remaining macro stubs", some other prominent
     * entities, like \alib{lox::Lox}, \alib{lang::format,Formatter} or
     * \alib{monomem::AcquireGlobalAllocator} invoke this method with acquisition.
     **********************************************************************************************/
    void DbgCheckSingleThreaded()
    {
        if( inSingleThreadedCheck ) // this would happen when the assertion below is raised (ALib Report)
            return;
        inSingleThreadedCheck= true;

        // first time?
        if( threadSeen == std::thread::id() )
        {
            threadSeen= std::this_thread::get_id();
            inSingleThreadedCheck= false;
            return;
        }

        if( threadSeen != std::this_thread::get_id() )
        {
            ALIB_ERROR(  "THREADS",
                         "A second thread was detected using a single-threaded compilation of "
                         "ALib! (Module 'Threads' not included in the ALib distribution)."     )
        }
        inSingleThreadedCheck= false;
    }

    } // namespace [alib]

#endif  // !ALIB_THREADS && ALIB_DEBUG && ALIB_EXT_LIB_THREADS_AVAILABLE



// #################################################################################################
// Debug functions
// #################################################################################################
#if ALIB_DEBUG
#   include <cassert>
#   include <cstring>
#   if defined( _WIN32 ) && !defined(_STRING_)
#      include <string>
#   endif

namespace alib::lang {


void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const char* file, int line, const char* method,
                                    int type, const char* topic, int qtyMsgs, const char** msgs )
       =nullptr;


//! @cond NO_DOX
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* topic,
                       const char* msg1, const char* msg2, const char* msg3,
                       const char* msg4, const char* msg5                         )
{
ALIB_WARNINGS_RESTORE

    int   qtyMsgs=  msg2 != nullptr ? (msg3 != nullptr ? (msg4 != nullptr ? (msg5 != nullptr ? 5 : 4) : 3) : 2) : 1;
    const char* msgs[5]= { msg1, msg2, msg3, msg4, msg5 };

    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, topic, qtyMsgs, msgs );
        return;
    }

    // internal implementation
         if( type == 0 )  std::cout << "ALib Error: ";
    else if( type == 1 )  std::cout << "ALib WARNING: ";
    else                  std::cout << "ALib Message(" << type << "): ";

    // With ALox replacing this method (the usual thing), the first string might auto-detected
    // as an ALox domain name. To keep this consistent wie do a similar effort here
    // (code is copied from ALoxReportWriter::Report())
    int msgNo= 0;
    if(qtyMsgs > 1)
    {
        bool illegalCharacterFound= false;

        auto strLen= strlen(msg1);
        for( size_t idx= 0 ;  idx< strLen ; ++idx )
        {
            char c= msg1[idx];
            if (!    (    isdigit( c )
                       || ( c >= 'A' && c <= 'Z' )
                       || c == '-'
                       || c == '_'
                       || c == '/'
                       || c == '.'
              )      )
            {
                    illegalCharacterFound= true;
                break;
            }
        }

        if(!illegalCharacterFound)
        {
            std::cout << msg1 << ": ";
            msgNo= 1;
        }
    }


    for ( ; msgNo != qtyMsgs; ++msgNo )
        std::cout << msgs[msgNo];
    std::cout << std::endl;
    std::cout << "At        : " << file << ':' << line << ' ' << method << "()" << std::endl;

    assert(type);
}

void DbgSimpleALibMsg( const char* file, int line, const char* method, int type,
                       const char* topic, const char* msg, int intVal )
{
    std::basic_string<char> sIntVal=       std::to_string( intVal );
    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        const char* msgs[2]= { msg, sIntVal.c_str() };
        DBG_SIMPLE_ALIB_MSG_PLUGIN( file, line, method, type, topic, 2, msgs );
        return;
    }

    // internal implementation
    if( type == 0 )
        std::cout << "Error in '" << topic << "': ";
    else
        std::cout << "Warning in '" << topic << "' " << "(type=" << type << "): ";
    std::cout << msg << sIntVal << std::endl;
    std::cout << "At        : " << file << ':' << line << ' ' << method << "()" << std::endl;
    assert(type);
}
//! @endcond
}// namespace [alib::lang]
#endif

// #################################################################################################
// static assertions for the platform
// #################################################################################################

#if !defined(HPP_ALIB_LANG_INTEGERS)
#   include "alib/lang/integers.hpp"
#endif
#if !defined(_GLIBCXX_TYPE_TRAITS) || !defined(_TYPE_TRAITS_)
#   include <type_traits>
#endif


static_assert(         sizeof(alib::integer )       ==         sizeof(alib::uinteger  ),       "Error in ALib type definitions" );
static_assert(         sizeof(alib::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );


// #################################################################################################
// Type De-mangling
// #################################################################################################

#if ALIB_DEBUG && !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_LANG_DBGTYPEDEMANGLER)
#      include "alib/lang/dbgtypedemangler.hpp"
#   endif

#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif

#   if !defined(_GLIBCXX_CSTDLIB) && !defined(_CSTDLIB_)
#      include <cstdlib>
#   endif

#   if defined(_WIN32) && !defined(_CSTRING_)
#      include <cstring>
#   endif

namespace alib {

    #if defined(__GNUC__) || defined(__clang__)

        lang::DbgTypeDemangler::DbgTypeDemangler( const std::type_info& typeInfo )
        {
            int status;
            name= abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status);
            assert( status==0 );
        }

        lang::DbgTypeDemangler::~DbgTypeDemangler()
        {
            if ( name )
                std::free(static_cast<void*>( const_cast<char*>(name) ) );
        }

        const char* lang::DbgTypeDemangler::Get()
        {
            return name;
        }

    #else
        lang::DbgTypeDemangler::DbgTypeDemangler( const type_info& typeInfo )
        {
            name= typeInfo.name();
            if (std::strncmp( name, "class ", 6) == 0)
                name+= 6;
        }

        lang::DbgTypeDemangler::~DbgTypeDemangler()
        {
        }

        const char* lang::DbgTypeDemangler::Get()
        {
            return name;
        }
    #endif

} // namespace [alib}
#endif // ALIB_DEBUG

// #################################################################################################
// Namespace documentation alib and alib
// #################################################################################################
/**
 * # 1. Introduction #
 * This is the root namespace of \alib, a general purpose C++ library. \alib is developed by
 * A-Worx GmbH, Germany, published under <em>Boost Software License</em>.
 *
 * This namespace is almost empty, apart from:
 * - sub-namespaces of \alibmods_nl,
 * - the single unique non-module related sub-namespace \ref alib::lang.
 * - sub-namespace #alib::compatibility, which is not even a real C++ namespace! In fact, it is used
 *   by various modules to place "documentation" of entities that provide compatibility to types of
 *   the standard C++ class library and 3rd-party libraries.
 * - Functions and types related to \ref alib_manual_bootstrapping "bootstrapping ALib".
 *
 * Nevertheless, this namespace is quite bloated with <em>type aliases</em>, which is explained
 * in the next section.
 *
 * \anchor alib_nsalib_type_aliases
 * # 2. Type Aliases In Namespace alib #
 * Once a C++ library becomes more complex, their types get split into different
 * namespaces and sub-namespaces. This is also true for \alib, which is split into various
 * modules. Consequently, the common need to conveniently address all types, is to
 * add several using statements to a compilation unit. With \alib this could
 * for example look like this:
 *
 *      using namespace alib::threads;
 *      using namespace alib::strings;
 *      using namespace alib::expressions;
 *
 * To avoid the need of the right "permutation" of using statements at the top of a users'
 * compilation unit, \alib "mirrors" all important types into this outer namespace with type definition
 * statements. For example, at the end of header file \alibheader{expressions/compiler.hpp},
 * just before closing outer namespace \b alib, the following statement is made:
 *
 *      /// Type alias in namespace \b alib.
 *      using Compiler=  expressions::Compiler;
 *
 * With that, a single:
 *
 *      using namespace alib;
 *
 * fits for all \alib types and in most cases, this single using statement is all that is
 * needed. This approach does not generate conflicts: If for example the using code would have
 * an own class named "Compiler", it can fall back to the standard schematic, by adding just:
 *
 *      using namespace alib::threads;
 *      using namespace alib::strings;
 *
 * and accessing class Compiler explicitly as <c>alib::expressions::Compiler</c>.
 *
 * \note
 *   The reason why this all is possible ist due to (a next great) design decision of C++.
 *   While the  \em CODE in the following snippet:
 *
 *          namespace A { namespace B {  CODE } }
 *
 * \note
 *   "sees" all types in namespace \c A, with this snippet:
 *
 *          using namespace A::B;
 *          CODE
 *
 * \note
 *   the \e CODE does not "see" types of namespace \c A, but only those of \c B.
 *
 * # 3. Domain-Specific Constants and Variables #
 * Similar to the concept of mirroring important interface classes into this <b>namespace alib</b>
 * (which is described in the previous paragraph), it is also agreed to place important global
 * constants here. The impact and benefits are quite the same.<br>
 * As a sample, see
 * \alib{DirectorySeparator} or  \alib{NewLine}, which appear in this namespace as soon as header
 * \alibheader{lang/system/directory.hpp}, respectively \ref alib/strings/cstring.hpp is included.
 *
 * # 4. Documentation #
 * The \ref alib_manual provides extensive documentation, including various separated manuals of
 * \alibmods, tutorials with sample code, information about configuring and compiling the
 * library and so forth.<br>
 * For people new to \alib, the \ref alib_manual is the right place to start!
 *
 * # Reference Documentation #
 */
namespace alib {}

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
 * \dir alib/lang
 * This directory contains general language extension types, functions and preprocessor code
 * of \aliblong, which are to be included in any \alibdist, regardless of the selected module subset.
 */

/**
 * \dir alib/lang/basecamp
 * This directory contains header files that define \alibcamps and extended bootstrapping
 * functionality as well as the compilation units of special \alib module \alib_basecamp.
 */

/**
 * \dir alib/lang/system
 * This directory contains header files and compilation units of module \alib_basecamp.
 */

/**
 * \dir alib/lang/resources
 * This directory contains header files and compilation units of module \alib_basecamp.
 */

/**
 * \dir alib/lang/resources/detail
 * This directory contains detail header files and compilation units of module \alib_basecamp.
 */

/**
 * \dir alib/lang/message
 * This directory contains header files and compilation units of module \alib_basecamp.
 * As an exclamation to the general rules, the types defined herein are exposed to
 * namespace \ref alib::lang.
 */

/**
 * \dir alib/lang/format
 * This directory contains header files and compilation units of module \alib_basecamp.
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


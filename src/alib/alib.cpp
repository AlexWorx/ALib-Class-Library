// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#if !DOXYGEN
#   include "alib/lang/platformincludes.hpp"
#   include "alib/lang/dbgcriticalsections.hpp"
#   if ALIB_CAMP
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#   if ALIB_SINGLETONS
#      include "alib/singletons/singleton.hpp"
#   endif
#   if ALIB_BOXING
#      include "alib/boxing/boxing.hpp"
#   endif
#   if ALIB_TIME
#      include "alib/time/time.hpp"
#   endif
#   if ALIB_ENUMS
#      include "alib/enums/records.hpp"
#      include "alib/enums/detail/enumrecordmap.hpp"
#   endif
#   if ALIB_MONOMEM
#      include "alib/monomem/globalallocator.hpp"
#   endif
#   if ALIB_THREADMODEL
#      include "alib/threadmodel/jobs.hpp"
#   endif
#   if ALIB_THREADS
#      include "alib/threads/thread.hpp"
#      include "alib/strings/localstring.hpp"
#   endif
#   if ALIB_STRINGS
#      include "alib/strings/astring.hpp"
#      include "alib/strings/substring.hpp"
#   endif
#   if ALIB_ALOX
#      include "alib/alox/aloxcamp.hpp"
#   endif
#   if ALIB_CONFIGURATION
#       include "alib/config/configcamp.hpp"
#   endif
#   if ALIB_CLI
#      include "alib/cli/clicamp.hpp"
#   endif
#   if ALIB_EXPRESSIONS
#      include "alib/expressions/expressionscamp.hpp"
#   endif
#   if ALIB_FILES
#      include "alib/files/filescamp.hpp"
#   endif
#   if ALIB_CAMP
#      include "alib/lang/basecamp/bootstrap.hpp"
#      include "alib/lang/message/report.hpp"
#   endif
#endif // !DOXYGEN

#include <iostream>
#include <iomanip>


// #################################################################################################
// Boostrapping & Shutdown
// #################################################################################################
namespace alib {

int                 ARG_C = 0;
const char**        ARG_VN= nullptr;
const wchar_t**     ARG_VW= nullptr;

bool                NonCampModulesInitialized     = false;

int                 VERSION             = ALIB_VERSION;
unsigned char       REVISION            = ALIB_REVISION;
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
TCompilationFlags   COMPILATION_FLAGS    = {ALIB_COMPILATION_FLAGS};
ALIB_WARNINGS_RESTORE


void     Bootstrap( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify ALib
    AssertALibVersionAndFlags( alibVersion, alibRevision, compilationFlags );

    #if ALIB_MONOMEM
        // if the global allocator was not initialized from outside, then we have to do it.
        if (!monomem::GLOBAL_ALLOCATOR.IsInitialized())
            new (&monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("Global",) 128);
    #endif


    #if ALIB_CAMP
        if(CAMPS.IsEmpty())
            BootstrapAddDefaultCamps();
        Bootstrap( BootstrapPhases::Final );
    #else
        if( !NonCampModulesInitialized )
        {
            IF_ALIB_TIME       ( time       ::Bootstrap(); )
            IF_ALIB_BOXING     ( boxing     ::Bootstrap(); )
            IF_ALIB_THREADS    ( threads    ::Bootstrap(); )
            IF_ALIB_ENUMS      ( enums      ::Bootstrap(); )
            IF_ALIB_THREADMODEL( threadmodel::Bootstrap(); )
            NonCampModulesInitialized= true;
        }

DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK])
#if ALIB_DEBUG_CRITICAL_SECTIONS && ALIB_MONOMEM
    monomem::GLOBAL_ALLOCATOR.DbgCriticalSectionsPH.Get()->DCSLock=
                    &monomem::GLOBAL_ALLOCATOR_LOCK;

    monomem::GLOBAL_ALLOCATOR_LOCK.Dbg.Name= "GlobalAllocator";
#endif
DOX_MARKER([DOX_CRITICAL_SECTIONS_ADD_LOCK])
    #endif
 }

void Shutdown()
{
    #if ALIB_CAMP
        Shutdown(ShutdownPhases::Destruct);
    #else
        IF_ALIB_THREADS    ( alib::threads::Shutdown    (); )
        IF_ALIB_TIME       ( alib::time::Shutdown       (); )
        IF_ALIB_ENUMS      ( alib::enums::Shutdown      (); )
        IF_ALIB_BOXING     ( alib::boxing::Shutdown     (); )
        IF_ALIB_SINGLETONS ( alib::singletons::Shutdown (); )
    #endif
 }

CompilationFlagMeaningsEntry COMPILATION_FLAG_MEANINGS[37]=
          {
            { 0, "ALOX"                             },
            { 1, "BITBUFFER"                        },
            { 2, "BOXING"                           },
            { 3, "CAMP"                              },
            { 4, "CHARACTERS"                       },
            { 5, "CLI"                              },
            { 6, "CONFIGURATION"                    },
            { 7, "CONTAINERS"                       },

            {  8, "ENUMS"                            },
            {  9, "EXPRESSIONS"                      },
            { 10, "FILES"                            },
            { 11, "MONOMEM"                          },
            { 12, "SINGLETONS"                       },
            { 13, "STRINGS"                          },
            { 14, "THREADMODEL"                      },
            { 15, "THREADS"                          },

            { 16, "TIME"                             },
            { 23, "DEBUG"                            },

            { 24, "ALIB_DEBUG_ARRAY_COMPRESSION"     },
            { 25, "ALIB_DEBUG_ALLOCATIONS"           },
            { 26, "ALIB_DEBUG_BOXING"                },
            { 27, "ALIB_DEBUG_CONTAINERS"            },
            { 28, "ALIB_DEBUG_CRITICAL_SECTIONS"     },
            { 29, "ALIB_DEBUG_MONOMEM"               },
            { 30, "ALIB_DEBUG_STRINGS"               },
            { 31, "ALIB_DEBUG_RESOURCES"             },


            { 32, "FEAT_SINGLETON_MAPPED"            },
            { 33, "CHARACTERS_WIDE"                  },
            { 34, "CHARACTERS_WCHAR_IS_4"            },
            { 35, "FEAT_BOXING_BIJECTIVE_INTEGRALS"  },
            { 36, "FEAT_BOXING_BIJECTIVE_CHARACTERS" },
            { 37, "FEAT_BOXING_BIJECTIVE_FLOATS"     },
            { 38, "ALIB_FEAT_BOOST_REGEX"            },

            { 40, "DBG_LOG"                          },
            { 41, "DBG_LOG_CI"                       },
            { 42, "REL_LOG"                          },
            { 43, "REL_LOG_CI"                       },
          };


ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void AssertALibVersionAndFlags( int alibVersion, int alibRevision, TCompilationFlags compilationFlags )
{
    // verify requested ALib version is met
    if (VERSION != alibVersion || REVISION != alibRevision )
    {
        std::cerr << "!!! Error in ALox library compilation: linked against wrong version of ALib" << std::endl;
        std::cerr << "!!! ALib library version:   " <<     VERSION   << "R" << int(REVISION) << std::endl;
        std::cerr << "!!! ALib requested version: " << alibVersion   << "R" << alibRevision << std::endl;
        std::cerr << "!!! Exiting with exit(-1)" << std::endl;
        exit(1);
    }

    // verify the flags
    if (    compilationFlags.bits[0] == COMPILATION_FLAGS.bits[0]
         && compilationFlags.bits[1] == COMPILATION_FLAGS.bits[1]
         && compilationFlags.bits[2] == COMPILATION_FLAGS.bits[2]
         && compilationFlags.bits[3] == COMPILATION_FLAGS.bits[3] )
        return;

    std::cerr << "!!! Error in ALib library compilation: linked library of ALib has "
                 "different compiler symbols set than the using executable (or library)." << std::endl;

    // dump out the flags
    std::cerr << std::left << std::setw(35) <<  "Symbol" << '|' << std::setw(5) << " Lib" <<'|' << " Comp. Unit"  << std::endl;
    std::cerr<< std::setw(62) <<  std::setfill('-') << ' ' << std::endl <<  std::setfill(' ');
    
    for( auto& p : COMPILATION_FLAG_MEANINGS )
    {
        bool libFlag= (COMPILATION_FLAGS.bits[p.Flag/8] & (1 << (p.Flag % 8))) != 0;
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
#if (ALIB_DEBUG && !ALIB_THREADS && ALIB_EXT_LIB_THREADS_AVAILABLE) || DOXYGEN

#include <thread>
namespace alib {
#if !DOXYGEN
    namespace  { std::thread::id  dbg_thread_seen;
                 bool             dbg_in_single_threaded_check= false; }
    #include "alib/lang/callerinfo_functions.hpp"
#endif


    //==============================================================================================
    /// This method stores the first thread that invokes it, and if in the future the method is
    /// visited by a different thread, it asserts.
    ///
    /// In release compilations, this function is inlined and empty, and therefore it is not
    /// necessary to remove usages with preprocessor macro \ref ALIB_DBG or similar.
    ///
    /// In debug-compilations, this is not empty if:
    /// 1. Module \alib_threads is not included in the \alibdist (what disables thread-safeness
    ///    throughout the library), and
    /// 2. Compiler symbol \ref ALIB_EXT_LIB_THREADS_AVAILABLE was passed on library compilation,
    ///    which allows using the C++ standard library's threading types without causing linker
    ///    failures.
    ///
    /// If given, this function is, for example, called by macros \ref ALIB_LOCK or
    /// \ref ALIB_DCS, which otherwise are defined to do what they are supposed to do.
    /// This exclamatory approach was made with \alib to motivate to
    /// write \ref alib_threads_intro_agnostic "threading-agnostic software".
    ///
    /// Besides several macros, some other prominent \alib entities, like \alib{lox::Lox},
    /// \alib{lang::format;Formatter} or \alib{monomem;TMonoAllocator} invoke this method with their
    /// acquisition.
    //==============================================================================================
    void DbgAssertSingleThreaded()
    {
        if( dbg_in_single_threaded_check ) // this would happen when the assertion below is raised (ALib Report)
            return;
        dbg_in_single_threaded_check= true;

        // first time?
        if( lang::IsNull(dbg_thread_seen) )
        {
            dbg_thread_seen= std::this_thread::get_id();
            dbg_in_single_threaded_check= false;
            return;
        }

        if( dbg_thread_seen != std::this_thread::get_id() )
        {
            ALIB_ERROR(  "THREADS",
                         "A second thread was detected using a single-threaded compilation of "
                         "ALib! (Module 'Threads' not included in the ALib distribution)."     )
        }
        dbg_in_single_threaded_check= false;
    }

    } // namespace [alib]

#include "alib/lang/callerinfo_methods.hpp"

#endif  // !ALIB_THREADS && ALIB_DEBUG && ALIB_EXT_LIB_THREADS_AVAILABLE


#if ALIB_DEBUG_CRITICAL_SECTIONS
unsigned int alib::lang::DBG_CRITICAL_SECTION_YIELD_OR_SLEEP_TIME_IN_NS;
#endif



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


void (*DBG_SIMPLE_ALIB_MSG_PLUGIN)( const CallerInfo& ci, int type, const char* topic,
                                    int qtyMsgs, const char** msgs )
       =nullptr;


//! @cond NO_DOX
ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
void DbgSimpleALibMsg( const CallerInfo& ci, int type, const char*  topic,
                       const char*  msg1, const char* msg2, const char* msg3,
                       const char*  msg4, const char* msg5                         )
{
ALIB_WARNINGS_RESTORE

    int   qtyMsgs=  msg2 != nullptr ? (msg3 != nullptr ? (msg4 != nullptr ? (msg5 != nullptr ? 5 : 4) : 3) : 2) : 1;
    const char* msgs[5]= { msg1, msg2, msg3, msg4, msg5 };

    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        DBG_SIMPLE_ALIB_MSG_PLUGIN( ci, type, topic, qtyMsgs, msgs );
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
    std::cout << "At        : " << ci.File << ':' << ci.Line << ' ' << ci.Func << "()" << std::endl;

    assert(type);
}

void DbgSimpleALibMsg( const CallerInfo& ci, int type, const char* topic,
                       const char* msg, int intVal )
{
    std::basic_string<char> sIntVal=       std::to_string( intVal );
    if( DBG_SIMPLE_ALIB_MSG_PLUGIN )
    {
        const char* msgs[2]= { msg, sIntVal.c_str() };
        DBG_SIMPLE_ALIB_MSG_PLUGIN( ci, type, topic, 2, msgs );
        return;
    }

    // internal implementation
    if( type == 0 )
        std::cout << "Error in '" << topic << "': ";
    else
        std::cout << "Warning in '" << topic << "' " << "(type=" << type << "): ";
    std::cout << msg << sIntVal << std::endl;
    std::cout << "At        : " << ci.File << ':' << ci.Line << ' ' << ci.Func << "()" << std::endl;
    assert(type);
}

//! @endcond
}// namespace [alib::lang]
#endif

// #################################################################################################
// static assertions for the platform
// #################################################################################################

#include "alib/lang/integers.hpp"
#include <type_traits>


static_assert(         sizeof(alib::integer )       ==        sizeof(alib::uinteger ),          "Error in ALib type definitions" );
static_assert(         sizeof(alib::integer )       ==         sizeof(std::size_t    ),         "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value == std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed< alib::integer>::value != std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value == std::is_signed<std::size_t    >::value,  "Error in ALib type definitions" );
static_assert(std::is_signed<alib::uinteger>::value != std::is_signed<std::ptrdiff_t >::value,  "Error in ALib type definitions" );


// #################################################################################################
// Type De-mangling
// #################################################################################################

#if ALIB_DEBUG && !DOXYGEN
#   include "alib/lang/dbgtypedemangler.hpp"
#   if defined(__GNUC__) || defined(__clang__)
#      include <cxxabi.h>
#      include <cassert>
#   endif
#   include <cstdlib>
#   if defined(_WIN32)
#      include <cstring>
#   endif

namespace alib {

    #if defined(__GNUC__) || defined(__clang__)

        lang::DbgTypeDemangler::DbgTypeDemangler( const std::type_info& typeInfo )
        {
            int status;
            name= abi::__cxa_demangle( typeInfo.name(), nullptr, nullptr, &status);
            assert( status==0 || !name );
            if ((failed= (name==nullptr)))
                name= "<DbgTypeDemangler Error>";
        }

        lang::DbgTypeDemangler::~DbgTypeDemangler()
        { if (!failed) std::free(static_cast<void*>( const_cast<char*>(name) ) ); }

    #else
        lang::DbgTypeDemangler::DbgTypeDemangler( const type_info& typeInfo )
        {
            name= typeInfo.name();
            if (std::strncmp( name, "class ", 6) == 0)
                name+= 6;
        }


    #endif

    const char* lang::DbgTypeDemangler::Get()                                       { return name; }
    
    #if ALIB_STRINGS
        NAString& lang::DbgTypeDemangler::GetShort(NAString& result)
        {
            NString     typeName(Get());

            integer     nameStart= 0;
            bool        startedWithDoubleColon= false;
            for (integer i = 0; i < typeName.Length(); ++i)
            {
                // MSVC adds struct/class/union
                if(typeName.Substring(i, 7).Equals("struct ")) i+= 7;
                if(typeName.Substring(i, 6).Equals("class " )) i+= 6;
                if(typeName.Substring(i, 6).Equals("union " )) i+= 6;

                char c = typeName.CharAt<NC>(i);
                if (c==':')
                {
                    ALIB_ASSERT(typeName.CharAt<NC>(i+1) == ':')
                    nameStart= i+2;
                    ++i;
                    continue;
                }
                if (!(isalnum(c) || c=='_') || i == typeName.Length() - 1) // (c=='<' || c=='>' || c==',' || c=='(')
                {
                    if (startedWithDoubleColon)
                        result << "::";
                    result << typeName.Substring(nameStart, i-nameStart+1);
                    nameStart= i+1;
                    startedWithDoubleColon= typeName.CharAt(nameStart) == ':';
                }
            }

            // some ABIs add a disambiguation-space, others don't (for example some MacOS compiler)
            // Thus we remove the space to have a unique behavior (and have the unit tests succeed).
            result.SearchAndReplace("> >", ">>");
            return result;
        }
    #endif


} // namespace [alib}

#endif // ALIB_DEBUG && !DOXYGEN

// #################################################################################################
// Namespace documentation alib and alib
// #################################################################################################
/// # 1. Introduction #
/// This is the root namespace of \alib, a general purpose C++ library. \alib is developed by
/// A-Worx GmbH, Germany, published under <em>Boost Software License</em>.
///
/// This namespace is almost empty, apart from:
/// - sub-namespaces of \alibmods_nl,
/// - the single unique non-module related sub-namespace \ref alib::lang.
/// - sub-namespace #alib::compatibility, which is not even a real C++ namespace! In fact, it is used
///   by various modules to place "documentation" of entities that provide compatibility to types of
///   the standard C++ class library and 3rd-party libraries.
/// - Functions and types related to \ref alib_manual_bootstrapping "bootstrapping ALib".
///
/// Nevertheless, this namespace is quite bloated with <em>type aliases</em>, which is explained
/// in the next section.
///
/// \anchor alib_nsalib_type_aliases
/// # 2. Type Aliases In Namespace alib #
/// This namespace is overcrowded with types.
/// However, almost all of them are just aliased types created with C++ keyword <c>"using"</c>.
///
/// Motivation for the existence of the aliases and details of their use is given in appendix chapter
/// \ref alib_manual_appendix_typealiases of the General Programmer's Manual of \alib.
///
/// # 3. Domain-Specific Constants and Variables #
/// Similar to the concept of mirroring important interface classes into this <b>namespace alib</b>
/// (which is described in the previous paragraph), it is also agreed to place important global
/// constants here. The impact and benefits are quite the same.<br>
/// As a sample, see
/// \alib{lang::system;DIRECTORY_SEPARATOR} or \alib{NEW_LINE}, which appear in this namespace as
/// soon as header \alibheader{lang/system/path.hpp}, respectively \ref alib/strings/cstring.hpp
/// is included.
///
/// # 4. Documentation #
/// The \ref alib_manual provides extensive documentation, including various separated manuals of
/// \alibmods, tutorials with sample code, information about configuring and compiling the
/// library and so forth.<br>
/// For people new to \alib, the \ref alib_manual is the right place to start!
///
/// # Reference Documentation #
namespace alib {}

/// \dir alib
/// This is the root folder of all source files of \alib.

/// \dir alib/alox
/// This directory contains header files and compilation units of module \alib_alox.

/// \dir alib/alox/detail
/// This directory contains header files and compilation units of module \alib_alox.

/// \dir alib/alox/textlogger
/// This directory contains header files and compilation units of module \alib_alox.

/// \dir alib/alox/loggers
/// This directory contains header files and compilation units of module \alib_alox.

/// \dir alib/bitbuffer
/// This directory contains header files and compilation units of module \alib_bitbuffer.

/// \dir alib/bitbuffer/ac_v1
/// This directory contains detail header files and compilation units of module \alib_bitbuffer.

/// \dir alib/boxing
/// This directory contains header files and compilation units of module \alib_boxing.

/// \dir alib/boxing/detail
/// This directory contains detail header files and compilation units of module \alib_boxing.

/// \dir alib/characters
/// This directory contains header files and compilation units of module \alib_characters.

/// \dir alib/cli
/// This directory contains header files and compilation units of module \alib_cli.

/// \dir alib/compatibility
/// This directory contains optional header files that provide compatibility between
/// \alib types and concepts and those of the C++ standard library as well as 3rd party libraries.

/// \dir alib/config
/// This directory contains header files and compilation units of module \alib_config.

/// \dir alib/containers
/// This directory contains header files and compilation units of module \alib_containers.

/// \dir alib/containers/detail
/// This directory contains detail header files and compilation units of module \alib_containers.

/// \dir alib/enums
/// This directory contains header files and compilation units of module \alib_enums.

/// \dir alib/enums/detail
/// This directory contains detail header files and compilation units of module \alib_enums.

/// \dir alib/expressions
/// This directory contains header files and compilation units of module \alib_expressions.

/// \dir alib/expressions/detail
/// This directory contains detail header files and compilation units of module \alib_expressions.

/// \dir alib/expressions/plugins
/// This directory contains header files and compilation units of module \alib_expressions.

/// \dir alib/expressions/util
/// This directory contains header files and compilation units of module \alib_expressions.

/// \dir alib/lang
/// This directory contains general language extension types, functions and preprocessor code
/// of \aliblong, which are to be included in any \alibdist, regardless of the selected module subset.

/// \dir alib/lang/basecamp
/// This directory contains header files that define \alibcamps and extended bootstrapping
/// functionality as well as the compilation units of special \alib module \alib_basecamp.

/// \dir alib/lang/system
/// This directory contains header files and compilation units of module \alib_basecamp.

/// \dir alib/lang/resources
/// This directory contains header files and compilation units of module \alib_basecamp.

/// \dir alib/lang/resources/detail
/// This directory contains detail header files and compilation units of module \alib_basecamp.

/// \dir alib/lang/message
/// This directory contains header files and compilation units of module \alib_basecamp.
/// As an exception to the general rules, the types defined herein are exposed to
/// namespace \ref alib::lang.

/// \dir alib/lang/format
/// This directory contains header files and compilation units of module \alib_basecamp.

/// \dir alib/monomem
/// This directory contains header files and compilation units of module \alib_monomem.

/// \dir alib/singletons
/// This directory contains header files and compilation units of module \alib_singletons.

/// \dir alib/strings
/// This directory contains header files and compilation units of module \alib_strings.

/// \dir alib/strings/detail
/// This directory contains detail header files and compilation units of module \alib_strings.

/// \dir alib/strings/util
/// This directory contains header files and compilation units of utility types of module \alib_strings.

/// \dir alib/threads
/// This directory contains header files and compilation units of module \alib_threads.

/// \dir alib/threads/detail
/// This directory contains detail header files and compilation units of module \alib_threads.

/// \dir alib/time
/// This directory contains header files and compilation units of module \alib_time.



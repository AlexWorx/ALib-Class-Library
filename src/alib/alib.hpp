// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB
#define HPP_ALIB 1


#define ALIB_VERSION              1805
#define ALIB_REVISION                0

// Uncomment to test for unnecessary includes. Must not be done with cotire unity builds.
// #define HPP_COM_ALIB_TEST_INCLUDES


// #################################################################################################
// basic system headers
// #################################################################################################

#if !defined (_ASSERT_H) && !defined(assert)
    #include <assert.h>
#endif

#if !defined (_GLIBCXX_IOSFWD) && !defined(_IOSFWD_)
    #include <iosfwd>
#endif

// GNUC specific
#if defined( __GNUC__ )

    #if !defined (_GLIBCXX_CSTDINT)
        #include <cstdint>
    #endif

// Windows specific
#elif defined ( _MSC_VER )

    //! @cond NO_DOX
    // Include windows.h. but exclude rarely-used stuff from Windows headers. If these things are
    // needed by the software that incorporates ALib, then windows has to be
    // included before this header file is.
    #if !defined (_INC_WINDOWS)
        #define WIN32_LEAN_AND_MEAN
        #define NOGDI
        #define NOSERVICE
        #define NOMCX
        #define NOIME
        #define NOSOUND
        #define NOCOMM
        #define NOKANJI
        #define NORPC
        #define NOPROXYSTUB
        #define NOIMAGE
        #define NOTAPE
        #include <windows.h>
    #endif
    //! @endcond

    #if !defined (_STDINT)
        #include <stdint.h>
    #endif

// other platforms not tested, yet
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

// ALib basics used with all modules

//! @cond NO_DOX
#define ALIB_PROPER_INCLUSION
//! @endcond

#include "alib/lang/prepro.hpp"


// #################################################################################################
// Include ALib modules found
// #################################################################################################


/**
 * @addtogroup GrpALibCodeSelectorSymbols
 * @{
 * @name ALib Module Selection
 * This group of symbols determine the availability of different <b>%ALib Modules</b> of the
 * \alib distribution used.<br>
 * The symbols are defined in header <c>alib/distribution.hpp</c>, which varies per distribution.
 *
 * For module <b>%ALib Singleton</b> no symbol is defined, as this module is included in any
 *  \alib distribution.
 *
 * \def  ALIB_MODULE_SINGLETON
 *  Denotes if class \alib{lang,Singleton} is available in the library used.
 *
 * \def  ALIB_MODULE_STRINGS
 *  Denotes if module \alibmod_strings is available in the library used.
 *
 * \def  ALIB_MODULE_BOXING
 *  Denotes if module \alibmod_boxing is available in the library used.
 *
 * \def  ALIB_MODULE_CORE
 *  Combines modules \alibmod_boxing and \alibmod_strings and adds further
 *  utility library dependencies, namely all types from namespaces
 *  - aworx::lib::lang,
 *  - aworx::lib::compatibility,
 *  - aworx::lib::util,
 *  - aworx::lib::time,
 *  - aworx::lib::system,
 *  - aworx::lib::threads and
 *  - aworx::lib::debug.
 *
 * \def  ALIB_MODULE_CONFIGURATION
 *  Denotes if module \alibmod_config is available in the library used. In addition it is
 *  assured that modules \alibmod_boxing and \alibmod_strings are included as well as some
 *  necessary from various core namespaces.
 *
 * \def  ALIB_MODULE_ALOX
 *  Denotes if module \alibmod_alox is available in the library used.
 *
 * \def  ALIB_MODULE_EXPRESSIONS
 *  Denotes if module \alibmod_expressions is available in the library used.
 *
 * \def  ALIB_MODULE_CLI
 *  Denotes if module \alibmod_cli is available in the library used.
 *
 * \def  ALIB_MODULE_ALL
 *  Denotes if this is a complete compilation of all \alib modules.
 * @}
 * @}
 */

#if defined(DOX_PARSER)
    #define ALIB_MODULE_ALL
#endif

// #################################################################################################
// Resolve module dependencies
// #################################################################################################


ALIB_WARNINGS_ALLOW_MACRO_REDEFINITION

#if !defined(ALIB_MODULE_ALL)           ||  (0 - ALIB_MODULE_ALL          - 1) == 1
#   define ALIB_MODULE_ALL          0
#endif
#if !defined(ALIB_MODULE_EXPRESSIONS)    ||  (0 - ALIB_MODULE_EXPRESSIONS - 1) == 1
#   define ALIB_MODULE_EXPRESSIONS   0
#endif

#if !defined(ALIB_MODULE_CLI)           ||  (0 - ALIB_MODULE_CLI          - 1) == 1
#   define ALIB_MODULE_CLI          0
#endif

#if !defined(ALIB_MODULE_ALOX)          ||  (0 - ALIB_MODULE_ALOX         - 1) == 1
#   define ALIB_MODULE_ALOX         0
#endif

#if !defined(ALIB_MODULE_CONFIGURATION) ||  (0 - ALIB_MODULE_CONFIGURATION- 1) == 1
#   define ALIB_MODULE_CONFIGURATION 0
#endif

#if !defined(ALIB_MODULE_CORE)          ||  (0 - ALIB_MODULE_CORE         - 1) == 1
#   define ALIB_MODULE_CORE         0
#endif

#if !defined(ALIB_MODULE_STRINGS)       ||  (0 - ALIB_MODULE_STRINGS      - 1) == 1
#   define ALIB_MODULE_STRINGS      0
#endif

#if !defined(ALIB_MODULE_BOXING)        ||  (0 - ALIB_MODULE_BOXING       - 1) == 1
#   define ALIB_MODULE_BOXING       0
#endif

#if !defined(ALIB_MODULE_SINGLETON)     ||  (0 - ALIB_MODULE_SINGLETON    - 1) == 1
#   define ALIB_MODULE_SINGLETON    0
#endif


// No module selection symbol given? -> choose ALL
#if !(  ALIB_MODULE_ALL                 \
      | ALIB_MODULE_EXPRESSIONS         \
      | ALIB_MODULE_CLI                 \
      | ALIB_MODULE_ALOX                \
      | ALIB_MODULE_CONFIGURATION       \
      | ALIB_MODULE_CORE                \
      | ALIB_MODULE_STRINGS             \
      | ALIB_MODULE_BOXING              \
      | ALIB_MODULE_SINGLETON )
    #define ALIB_MODULE_ALL                 1
#endif

#if ALIB_MODULE_ALL
    #define ALIB_MODULE_EXPRESSIONS         1
    #define ALIB_MODULE_CLI                 1
    #define ALIB_MODULE_ALOX                1
#endif

#if ALIB_MODULE_EXPRESSIONS
    #define ALIB_MODULE_CORE                1
#endif

#if ALIB_MODULE_CLI
    #define ALIB_MODULE_CONFIGURATION       1
#endif

#if ALIB_MODULE_ALOX
    #define ALIB_MODULE_CONFIGURATION       1
    #define ALIB_MODULE_CORE                1
#endif

#if ALIB_MODULE_CONFIGURATION
    #define ALIB_MODULE_STRINGS             1
    #define ALIB_MODULE_BOXING              1
#endif

#if ALIB_MODULE_CORE
    #define ALIB_MODULE_STRINGS             1
    #define ALIB_MODULE_BOXING              1
#endif

#if ALIB_MODULE_STRINGS
    #define ALIB_MODULE_SINGLETON           1
#endif

#if ALIB_MODULE_BOXING
    #define ALIB_MODULE_SINGLETON           1
#endif


//! @cond NO_DOX
ALIB_WARNINGS_RESTORE
//! @endcond

// #################################################################################################
// Include module headers
// #################################################################################################

#if ALIB_MODULE_BOXING || ALIB_MODULE_STRINGS
    #include "alib/lang/types.hpp"
    #include "alib/debug/assert.hpp"
#endif

// always needed
#include "alib/lang/rtti.hpp"

// Module: Singleton (the first, independent module, always included)
#include "alib/lang/singleton.hpp"


#if ALIB_MODULE_STRINGS
    #if !defined(HPP_ALIB_LANG_TYPES)
        #include "alib/lang/types.hpp"
    #endif

    #include "alib/lang/enums.hpp"
    #include "alib/strings/strings.hpp"
    #include "alib/strings/astring.hpp"
    #include "alib/strings/preallocatedstring.hpp"

    #include "alib/lang/resources.hpp"
    #include "alib/lang/library.hpp"
    #include "alib/strings/stringlib.hpp"
#else
    #define ALIB_DEBUG_STRINGS  0
#endif



#if ALIB_MODULE_BOXING
    #include "alib/boxing/boxinglib.hpp"
#else
    #define ALIB_FEAT_BOXING_FTYPES      0
    #define ALIB_FEAT_BOXING_STD_VECTOR  0
#endif

#if ALIB_MODULE_STRINGS
#   include "alib/lang/langlib.hpp"
#endif

#if ALIB_MODULE_STRINGS && ALIB_MODULE_BOXING && ALIB_FEAT_BOXING_FTYPES
    #include "alib/strings/boxing/stringboxing.hpp"
#endif

#if ALIB_MODULE_BOXING && ALIB_MODULE_STRINGS
    static_assert( ALIB_FEAT_BOXING_FTYPES,
                  "The full version of ALib must not be compiled with disabled default "
                  "boxing of fundamental types (See ALIB_FEAT_BOXING_FTYPES_OFF)."         );
#endif

#if ALIB_MODULE_CORE
    #include "alib/lang/report.hpp"
#endif

#if ALIB_MODULE_STRINGS  && !defined(HPP_ALIB_LANG_ENUM_META_DATA)
    #include "alib/lang/enumbitwise.hpp"
    #include "alib/lang/enummetadata.hpp"
#endif


#if ALIB_MODULE_CONFIGURATION
    #include "alib/config/configlib.hpp"
#endif


#if ALIB_MODULE_CORE || ALIB_MODULE_CONFIGURATION
    #include "alib/system/systemlib.hpp"
#endif


#undef  ALIB_PROPER_INCLUSION

#if ALIB_MODULE_BOXING || ALIB_MODULE_STRINGS

// #################################################################################################
// Version verification macros
// #################################################################################################

/**
 * @defgroup GrpALibCompilerSymbols ALib Compilation Symbols
 *
 * Symbols (C++ preprocessor macros) listed here, support adding or removing features to \alib when
 * compiling the library and any code unit that uses \alib.
 * These symbols must be used <em>only within makefiles or IDE project files to be passed to the
 * C++ compiler (!) </em> (normally -D option). In other words: they must not be defined or undefined
 * within the source code that uses \alib.
 *
 * For most of the symbols, two versions with suffixes <em>_ON</em> and <em>_OFF</em> exists.
 * These symbols decide about value of a corresponding <em>code selection symbol</em> that is named
 * the same without the suffix.<br>
 * If both are set at the same time, a compiler error is generated by \alib header files.
 * If none of them is passed to the compiler, a default value is assumed.<p>
 *
 * As a convenience, to get default behavior, no symbol needs to be passed to the compiler.<p>
 *
 * \attention
 *   Some of the compilation symbols are changing the definition of classes and methods which
 *   makes binaries incompatible with binaries that do not use the same setting for each of
 *   these symbols.<br>
 *   Symbols that have to be kept equal across compilation units are denoted in this
 *   documentation.
 *
 * @{ @}
 */

/**
 * @addtogroup GrpALibMacros
 * @{
 * @name ALib Version And Feature Verification
 * Macros used to verify that the \alib binary used has the right version is is compiled with
 * the same feature set as the current compilation unit.
 * For this, version and feature flags are compiled statically into the library.
 * The macros are used internally, respectively 'automatically'. Hence, they need not to be used
 * directly clients of the library.
 *
 * \def  ALIB_VERSION
 *  The \alib version number. The value of this macro is written into field
 *  \alib{lang,Library::Version} of singleton \ref aworx::lib::ALIB (and corresponding
 *  sub-library objects as well).
 *
 * \def  ALIB_REVISION
 *  The \alib revision number. The value of this macro is written into field
 *  \alib{lang,Library::Revision} of singleton \ref aworx::lib::ALIB (and corresponding
 *  sub-library singletons as well).
 *
 * \def  ALIB_COMPILATION_FLAGS
 *  Macro to provide a compilation flag verifier value to be used with method
 *  \alib{lang,Library::VerifyCompilationFlags} of singleton \alib{ALIB}.
 *
 * @}
 * @}
 */

#if defined(DOX_PARSER)
    #define  ALIB_COMPILATION_FLAGS
#else
    #define ALIB_DEBUG_VFYBIT                  (1 <<  0)
    #define ALIB_DEBUG_STRINGS_VFYBIT          (1 <<  1)

    #define ALIB_FEAT_THREADS_VFYBIT           (1 << 10)
    #define ALIB_FEAT_SINGLETON_MAPPED_VFYBIT  (1 << 11)
    #define ALIB_FEAT_BOXING_FTYPES_VFYBIT     (1 << 12)
    #define ALIB_FEAT_BOXING_STD_VECTOR_VFYBIT (1 << 13)


    #define    ALIB_COMPILATION_FLAGS                                                            \
    ( ALIB_DEBUG_VFYBIT + ( ALIB_DEBUG_VFYBIT                   * ALIB_DEBUG                  )  \
                        + ( ALIB_FEAT_THREADS_VFYBIT            * ALIB_FEAT_THREADS           )  \
                        + ( ALIB_FEAT_SINGLETON_MAPPED_VFYBIT   * ALIB_FEAT_SINGLETON_MAPPED  )  \
                        + ( ALIB_FEAT_BOXING_FTYPES_VFYBIT      * ALIB_FEAT_BOXING_FTYPES     )  \
                        + ( ALIB_FEAT_BOXING_STD_VECTOR_VFYBIT  * ALIB_FEAT_BOXING_STD_VECTOR )  \
                        + ( ALIB_DEBUG_STRINGS_VFYBIT           * ALIB_DEBUG_STRINGS          )  \
    )
#endif



#if ALIB_MODULE_STRINGS

namespace aworx { namespace lib {


// #################################################################################################
// forward declarations
// #################################################################################################
#if ALIB_MODULE_CONFIGURATION || ALIB_MODULE_CORE
    namespace config  { class  Configuration;
                        struct VariableDecl;        }
    namespace time    { template<typename TClock, typename TDerived>
                        class TimePointBase;        }
    namespace threads { class  ThreadLock;
                        class  SmartLock;           }
#endif

// #################################################################################################
// Variables
// #################################################################################################
#if ALIB_MODULE_CONFIGURATION
    /**
     * Configuration variables of \alib.
     * \note
     *  The variables are declared using \alib{lang,T_EnumMetaDataDecl,enum meta data}
     *  and macro \ref ALIB_CONFIG_VARIABLES.
     *  Hence, all variable categories, names, default values and such can be modified
     *  by software using \alib by modifying the resource data of the
     *  \ref aworx::lib::ALIB "ALib library singleton".
     */
    enum class Variables
    {
        LOCALE               = 1   , ///< \alib Resourced variable.
        WAIT_FOR_KEY_PRESS   = 2   , ///< \alib Resourced variable.
        HAS_CONSOLE_WINDOW   = 3   , ///< \alib Resourced variable.
    };
#endif // ALIB_MODULE_CONFIG


// #################################################################################################
// class ALIB
// #################################################################################################

/** ************************************************************************************************
 * This is the library class for namespace \ref aworx::lib. As \alib is more a collection of
 * libraries than a simple one, in respect to library initialization not too much is performed
 * (in method #init), because the duties are distributed to the sub-libraries.
 *
 * Apart from implementing class \b %Library, this class is a 100% static placeholder for a few
 * global methods and fields.
 *
 * \note In C# and Java implementations of this class, debug \e 'shortcuts' to methods of class
 *       \ref aworx::lib::lang::Report "Report" exist. In the C++ version, such methods are provided as
 *       macros named \e 'ALIB_XXX', e.g #ALIB_ASSERT_ERROR.
 *
 * <p>
 * \note The availability of certain fields and methods of this class is dependent the distribution
 *       version of \alib. With module <b>%ALib Singleton</b>, the class does not exist at
 *       all.<br>
 *       See \ref aworx::lib for more "namespace documentation" for information
 *       about \alib modules and the distributions available.
 **************************************************************************************************/
class ALib  : public lang::Library
{
    // #############################################################################################
    // internal statics
    // #############################################################################################
    protected:

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
        * If \c true, within #TerminationCleanUp, it is waited for a key press in the console
        * window.<br>
        * By default, this flag is enabled when debugging a console application under Visual
        * Studio.<br>
        * This default behavior can be overruled by setting configuration variable
        * [ALIB_WAIT_FOR_KEY_PRESS](../group__GrpALoxConfigVars.html).<br>
        * In addition, this public flag may be modified at runtime (after method #Init was invoked).
        */
        bool         WaitForKeyPressOnTermination=                                            false;

        /**
         * This is a smart mutex that allows to lock an applications' <em>standard output
         * streams</em>.
         *
         * In multi-threaded processes, to protect the output streams from concurrent access,
         * this smart lock might be used by any \e entity that writes data to the streams.
         * Before it can be used (acquired and released), it is needed to register with the object
         * using
         * \ref aworx::lib::threads::SmartLock::AddAcquirer "SmartLock::AddAcquirer".
         * This has to be done once per thread that aims to write to the stream. Then, prior to
         * writing, this object has to be acquired and after writing released.
         *
         * Because often, the standard \e output stream and standard \e error stream are identical,
         * \alib provides one single lock for both, to protect also against interwoven
         * standard output and error information.
         *
         * If the 'entity' that is registering is not of type
         * \ref aworx::lib::threads::ThreadLock "ThreadLock" it is allowed to provide \c nullptr
         * in the parameter of method \b AddAcquirer. In this case, the process of adding and
         * removing \e acquirers is not performed in a thread safe way. Therefore it is advised
         * to register so called anonymous (\c nullptr) \e acquirers only at bootstrap time, when
         * no parallel threads were started, yet.
         *
         * If an application is deemed to always write to the standard output streams from within
         * multiple threads, an alternative to registering each writing entity, is to
         * invoke \b AddAcquirer just two times in a row with \c nullptr at the start of a process
         * and then never do this again (and never de-register). This way, no thread needs
         * to register/de-register but threads may still \b Acquire and \b Release the lock without
         * being registered. In other words, once a smart lock is enabled, subsequent registrations
         * are just used to count and identify the de-registration.
         *
         * \note
         *   The advantage of the SmartLock is that if only one 'entity' registered, no
         *   system \e mutexes will be used with \b Acquire and \b Release, hence there is
         *   a performance gain. Such gain is not noticeable for the 'slow' terminal console output,
         *   but it is for fast, buffered output streams.
         * <p>
         * \note
         *   Logging library \alox, which is built on \alib, will register whenever a \e Logger
         *   is used that writes to the standard output stream. Hence, applications that in
         *   parallel use, e.g. 'std::cout', should register at bootstrap and \e acquire this
         *   instance prior to writing. This way, log output and other application output is
         *   not mixed, but separated in different Lines.
         * <p>
         * \note
         *   This fields is available only in the complete distribution version of \alib.
         */
        #if ALIB_MODULE_CORE
            threads::SmartLock&                         StdOutputStreamsLock;
        #endif

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        ALIB_API        ALib();

    // #############################################################################################
    // Environment definition/detection
    // #############################################################################################
    public:

        #if ALIB_MODULE_CONFIGURATION
            /** ************************************************************************************
             * This determines if the current process has console output window attached.
             * While this is not detectable in Unix like systems (or just with huge efforts), in
             * windows it is. The determination works as follows
             *  - If configuration variable [ALIB_HAS_CONSOLE_WINDOW](../group__GrpALoxConfigVars.html)
             *    is set, its value is returned.
             *  - Otherwise, on Unix like OS\c true is returned
             *  - On Windows OS it is determined if a console window is attached to the process
             *    using system call \e GetConsoleWindow() and compare its result to \e NULL.
             *  @return \c true if the application has a console window attached.
             *
             * \note
             *   This fields is available only with module <b>%ALib Configuration</b> or with the
             *   complete distribution version of \alib.
             **************************************************************************************/
            ALIB_API
            bool                        HasConsoleWindow();
        #endif

        #if ALIB_MODULE_CORE
            /** ************************************************************************************
             * If \c true, the current process is under the hood of a debugger.
             *
             * \note
             *   This fields is available only in the complete distribution version of \alib.
             *
             * @return \c true if the application is run in a debugger.
             **************************************************************************************/
            ALIB_API
            bool                        IsDebuggerPresent();

        #endif

    // #############################################################################################
    // Compilation Flags
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Checks the versions of \alib and if the current compilation unit shares compatible
         * compilation symbols with the \alib compilation.
         *
         * This method should be invoked in the bootstrap code of your processes and libraries
         * to check for compilation/link mismatch.
         * Use macro \ref ALIB_COMPILATION_FLAGS for parameter \p{compilationFlags}
         *
         * @param alibVersion      The \alib version required.
         * @param compilationFlags The flags as defined in invoking compilation unit.
         *                         Use \ref ALIB_COMPILATION_FLAGS.
         ******************************************************************************************/
        ALIB_API
        void     CheckCompatibility( int      alibVersion,
                                     uint64_t compilationFlags );


    // #############################################################################################
    // Other static interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * This is for compatibility with other platform versions of \alib.
         * Variants of this method are #SleepMicros and #SleepNanos.
         *
         *  @param milliseconds    Sleep time in milliseconds.
         ******************************************************************************************/
        ALIB_API  static void SleepMillis( int milliseconds );

        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * This is for compatibility with other platform versions of \alib.
         * Variants of this method are #SleepMillis and #SleepNanos.
         *
         * @param microseconds    Sleep time in milliseconds.
         ******************************************************************************************/
        ALIB_API  static void SleepMicros( int64_t microseconds );

        /** ****************************************************************************************
         * Suspends the current thread by calling <em>std::this_thread::sleep_for</em>.
         * This is for compatibility with other platform versions of \alib.
         * Variants of this method are #SleepMicros and #SleepMillis.
         *
         * @param nanoseconds    Sleep time in nanoseconds.
         ******************************************************************************************/
        ALIB_API  static void SleepNanos( int64_t nanoseconds );

    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \alib{lang,Library::init}.
         *
         * In phase \alib{lang,Library::Phases::final} performs:
         * - In glibc versions of \alib (GNU/unix) probably invoke glibc method
         *   <em>setlocale()</em>, depending on the setting of the environment variables
         *   <em>LANG</em> and <em>LANGUAGE</em>
         *   and depending on \alib configuration variable
         *   [ALIB_LOCALE](../group__GrpALoxConfigVars.html).
         *
         *   In addition, invokes
         *   \alib{strings,NumberFormatBase::SetFromLocale,NumberFormat::SetFromLocale} on static singleton
         *   \alib{strings,NumberFormatBase::Global,NumberFormat::Global}.
         *
         * - Config variable [WAIT_FOR_KEY_PRESS](../group__GrpALoxConfigVars.html)
         *   is read and field #WaitForKeyPressOnTermination set accordingly
         *
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Implementation of
         * \alib{lang,Library::terminationCleanUp}.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;


};// class ALib


/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib.
 **************************************************************************************************/
ALIB_API
extern ALib   ALIB;

} // namespace lib

/// Type alias in namespace #aworx.
using ALib=                     aworx::lib::ALib;

} // namespace aworx


#if ALIB_MODULE_CONFIGURATION
    ALIB_CONFIG_VARIABLES( aworx::lib::Variables, lib::ALIB, ASTR("Var") )
#endif


// #################################################################################################
// Further standard includes (for convenience)
// #################################################################################################
#if !defined(DOX_PARSER)
#define ALIB_PROPER_INCLUSION
#endif

    #include "alib/strings/applicables.hpp"

#undef ALIB_PROPER_INCLUSION

#endif // ALIB_MODULE_STRINGS


#endif // just module ALib Singleton

#endif // HPP_ALIB

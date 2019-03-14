// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_ALIBMODULES
#define HPP_ALIB_LIB_ALIBMODULES 1

#if !defined(HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif

#if ALIB_MODULE_CONFIGURATION && !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif

ALIB_ASSERT_FILESET(MODULES)


// #################################################################################################
// Compilation flags
// Create a bitset from the ALib compilation features.
// For this, we ensure that the individual headers tested are included
// #################################################################################################

#if ALIB_MODULE_SINGLETONS && !defined(HPP_ALIB_SINGLETONS_PREDEF)
#   include "alib/singletons/singletons_predef.hpp"
#endif

#if ALIB_MODULE_BOXING && !defined(HPP_ALIB_BOXING_PREDEF)
#   include "alib/boxing/boxing_predef.hpp"
#endif


#if !ALIB_DOCUMENTATION_PARSER

#   define ALIB_VFYBIT_DEBUG                                   (1LLU <<  0)

#   define ALIB_VFYBIT_MODULE_MEMORY                           (1LLU <<  1)
#   define ALIB_VFYBIT_MODULE_SINGLETONS                       (1LLU <<  2)
#   define ALIB_VFYBIT_MODULE_CHARACTERS                       (1LLU <<  3)
#   define ALIB_VFYBIT_MODULE_ENUMS                            (1LLU <<  4)
#   define ALIB_VFYBIT_MODULE_TIME                             (1LLU <<  5)
#   define ALIB_VFYBIT_MODULE_BOXING                           (1LLU <<  6)
#   define ALIB_VFYBIT_MODULE_STRINGS                          (1LLU <<  7)
#   define ALIB_VFYBIT_MODULE_RESOURCES                        (1LLU <<  8)
#   define ALIB_VFYBIT_MODULE_THREADS                          (1LLU <<  9)
#   define ALIB_VFYBIT_MODULE_STRINGFORMAT                     (1LLU << 10)
#   define ALIB_VFYBIT_MODULE_RESULTS                          (1LLU << 11)
#   define ALIB_VFYBIT_MODULE_SYSTEM                           (1LLU << 12)
#   define ALIB_VFYBIT_MODULE_CONFIGURATION                    (1LLU << 13)
#   define ALIB_VFYBIT_MODULE_ALOX                             (1LLU << 14)
#   define ALIB_VFYBIT_MODULE_EXPRESSIONS                      (1LLU << 15)
#   define ALIB_VFYBIT_MODULE_CLI                              (1LLU << 16)

#   define ALIB_VFYBIT_FEAT_SINGLETON_MAPPED                   (1LLU << 40)

#   define ALIB_VFYBIT_CHARACTERS_ARE_NARROW                   (1LLU << 41)
#   define ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4                   (1LLU << 42)

#   define ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS     (1LLU << 43)
#   define ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS    (1LLU << 44)
#   define ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_FLOATS        (1LLU << 45)

#   define ALIB_VFYBIT_DEBUG_STRINGS                           (1LLU << 46)

#   define ALOX_VFYBIT_DBG_LOG                                 (1LLU << 47)
#   define ALOX_VFYBIT_DBG_LOG_CI                              (1LLU << 48)
#   define ALOX_VFYBIT_REL_LOG                                 (1LLU << 49)
#   define ALOX_VFYBIT_REL_LOG_CI                              (1LLU << 50)




#if ALIB_MODULE_ALOX
#   if !defined(HPP_ALOX_PREDEF)
#       include "alib/alox/alox_predef.hpp"
#   endif
#   define    ALIB_COMPILATION_FLAGS_ALOX                                                          \
            (    ALOX_VFYBIT_DBG_LOG                 * ALOX_DBG_LOG                                \
              +  ALOX_VFYBIT_DBG_LOG_CI              * ALOX_DBG_LOG_CI                             \
              +  ALOX_VFYBIT_REL_LOG                 * ALOX_REL_LOG                                \
              +  ALOX_VFYBIT_REL_LOG_CI              * ALOX_REL_LOG_CI                             \
            )
#else
#   define    ALIB_COMPILATION_FLAGS_ALOX     0
#endif



#   define    ALIB_COMPILATION_FLAGS                                                               \
(   ALIB_VFYBIT_DEBUG                                * ALIB_DEBUG                                  \
                                                                                                   \
 +  ALIB_VFYBIT_MODULE_MEMORY                        * ALIB_MODULE_MEMORY                          \
 +  ALIB_VFYBIT_MODULE_SINGLETONS                    * ALIB_MODULE_SINGLETONS                      \
 +  ALIB_VFYBIT_MODULE_CHARACTERS                    * ALIB_MODULE_CHARACTERS                      \
 +  ALIB_VFYBIT_MODULE_ENUMS                         * ALIB_MODULE_ENUMS                           \
 +  ALIB_VFYBIT_MODULE_TIME                          * ALIB_MODULE_TIME                            \
 +  ALIB_VFYBIT_MODULE_BOXING                        * ALIB_MODULE_BOXING                          \
 +  ALIB_VFYBIT_MODULE_STRINGS                       * ALIB_MODULE_STRINGS                         \
 +  ALIB_VFYBIT_MODULE_RESOURCES                     * ALIB_MODULE_RESOURCES                       \
 +  ALIB_VFYBIT_MODULE_THREADS                       * ALIB_MODULE_THREADS                         \
 +  ALIB_VFYBIT_MODULE_STRINGFORMAT                  * ALIB_MODULE_STRINGFORMAT                    \
 +  ALIB_VFYBIT_MODULE_RESULTS                       * ALIB_MODULE_RESULTS                         \
 +  ALIB_VFYBIT_MODULE_SYSTEM                        * ALIB_MODULE_SYSTEM                          \
 +  ALIB_VFYBIT_MODULE_CONFIGURATION                 * ALIB_MODULE_CONFIGURATION                   \
 +  ALIB_VFYBIT_MODULE_ALOX                          * ALIB_MODULE_ALOX                            \
 +  ALIB_VFYBIT_MODULE_EXPRESSIONS                   * ALIB_MODULE_EXPRESSIONS                     \
 +  ALIB_VFYBIT_MODULE_CLI                           * ALIB_MODULE_CLI                             \
                                                                                                   \
 +  ALIB_VFYBIT_FEAT_SINGLETON_MAPPED                * ALIB_FEAT_SINGLETON_MAPPED                  \
                                                                                                   \
 +  ALIB_VFYBIT_CHARACTERS_ARE_NARROW                * ALIB_CHARACTERS_ARE_NARROW                  \
 +  ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4                * (ALIB_CHARACTERS_SIZEOF_WCHAR == 4 ? 1 : 0) \
                                                                                                   \
 +  ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS  * ALIB_FEAT_BOXING_NON_BIJECTIVE_INTEGRALS    \
 +  ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS * ALIB_FEAT_BOXING_NON_BIJECTIVE_CHARACTERS   \
 +  ALIB_VFYBIT_FEAT_BOXING_NON_BIJECTIVE_FLOATS     * ALIB_FEAT_BOXING_NON_BIJECTIVE_FLOATS       \
                                                                                                   \
 +  ALIB_VFYBIT_DEBUG_STRINGS                        * ALIB_STRINGS_DEBUG                          \
                                                                                                   \
 +  ALIB_COMPILATION_FLAGS_ALOX                                                                    \
)


#endif //ALIB_DOCUMENTATION_PARSER




namespace aworx { namespace lib {

// #################################################################################################
// Variables
// #################################################################################################
#if ALIB_MODULE_CONFIGURATION
    /**
     * Configuration variables of \alib.
     * \note
     *  The variables are declared using \alib{resources,T_EnumMetaDataDecl,enum meta data}
     *  and macro \ref ALIB_CONFIG_VARIABLES.
     *  Hence, all variable categories, names, default values and such can be modified
     *  by modifying the resource data of the singleton of class \aworx{lib,ALibModules}.
     */
    enum class Variables
    {
        LOCALE               = 1   , ///< \alib Resourced variable.
        WAIT_FOR_KEY_PRESS   = 2   , ///< \alib Resourced variable.
        HAS_CONSOLE_WINDOW   = 3   , ///< \alib Resourced variable.
    };
#endif // ALIB_MODULE_CONFIGURATION


/** ************************************************************************************************
 * This module singleton collects all independent modules in its #Dependencies and thus is
 * recommended to be used for library initialization and termination.<br>
 * All details on this is described in chapter
 * \ref alib_manual_bootstrapping "3. Bootstrapping ALib" of the \ref alib_manual.
 *
 * Besides that, the class provides a few utility members that have not found a different home
 * with the current version of the library.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::ALIB.
 **************************************************************************************************/
class ALibModules  : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        ALibModules();

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
         * [ALIB_WAIT_FOR_KEY_PRESS](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html).<br>
         * In addition, this public flag may be modified at run-time (after method #Init was
         * invoked).
         */
        bool            WaitForKeyPressOnTermination                                         =false;

        /**
         * This flag indicates if the current process has console output window attached.
         * While this is not detectable in Unix like systems (or just with huge efforts), in
         * windows it is. The determination is performed in #init and done as follows
         *  - If configuration variable \b ALIB_HAS_CONSOLE_WINDOW
         *    is set, its value is returned.
         *  - Otherwise, on Unix like OS\c true is returned.
         *  - On Windows OS it is determined if a console window is attached to the process
         *    using system call \e GetConsoleWindow() and compare its result to \e NULL.
         *
         * In addition, this public flag may be modified at run-time (after method #Init was
         * invoked).
         *
         * \note
         *   The evaluation of configuration variable \b ALIB_HAS_CONSOLE_WINDOW is done
         *   only if module \alibmod_config is included in the \alibdist.
         */
        bool            HasConsoleWindow;



        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Init(int, nchar**,InitLevels)} or
         * \alib{Module::Init(int, wchar**,InitLevels)}.
         */
        int             ArgC                                                                    = 0;

        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Init(int, char**,InitLevels)}.
         */
        const char**    ArgVN                                                             = nullptr;

        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Init(int, wchar_t**,InitLevels)}.
         */
        const wchar_t** ArgVW                                                             = nullptr;

    // #############################################################################################
    // Environment definition/detection
    // #############################################################################################
    public:

        #if ALIB_MODULE_SYSTEM
            /** ************************************************************************************
             * If \c true, the current process is under the hood of a debugger.
             *
             * \note
             *   This method is available only if module \alibmod_system is included in the
             *   \alibdist.
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
         * Checks the versions of \alib and whether the current compilation unit shares compatible
         * compilation symbols with the \alib binaries. The latter is done by invoking
         * inherited method \alib{Module::VerifyCompilationFlags}.
         *
         * This method should be invoked in the bootstrap code of your processes and libraries
         * to check for compilation and linkage mismatch.
         *
         * Parameter \p{alibVersion} defaults to macro \ref ALIB_VERSION and
         * parameter \p{compilationFlags} to macro \ref ALIB_COMPILATION_FLAGS.
         * Both parameters should not be given when invoked.
         * This value is internally matched against what they were when \alib was compiled.
         *
         * \see
         *   For more information, see chapter
         *   \ref alib_manual_bootstrapping_assertcomp "3.9 Assuring Compilation Compatibility"
         *   of the \ref alib_manual.
         *
         *
         * @param alibVersion      The \alib version required.
         *                         Defaults to \ref ALIB_VERSION and should not be passed.
         * @param compilationFlags The flags as defined in invoking compilation unit.
         *                         Defaults to \ref ALIB_COMPILATION_FLAGS and should not be passed.
         ******************************************************************************************/
        ALIB_API
        void     CheckDistribution( int      alibVersion        = ALIB_VERSION,
                                    uint64_t compilationFlags   = ALIB_COMPILATION_FLAGS );

    // #############################################################################################
    // Methods for being a module ourselves
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \alib{Module::init}.
         *
         * In phase \alib{Module,InitLevels::Final} performs:
         * - In glibc versions of \alib (GNU/unix) probably invoke glibc method
         *   <em>setlocale()</em>, depending on the setting of the environment variables
         *   <em>LANG</em> and <em>LANGUAGE</em>
         *   and depending on \alib configuration variable
         *   [ALIB_LOCALE](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html).
         *
         *   In addition, invokes
         *   \alib{strings,TNumberFormat::SetFromLocale,NumberFormat::SetFromLocale} on static singleton
         *   \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * - Config variable [WAIT_FOR_KEY_PRESS](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html)
         *   is read and field #WaitForKeyPressOnTermination set accordingly
         *
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t** wargv )     override;

        /** ****************************************************************************************
         * Implementation of \alib{Module::terminationCleanUp}.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;


};// class ALibModules

} // namespace aworx[::lib]

/** The singleton of module-class \alib{ALibModules}. */
extern ALIB_API lib::ALibModules ALIB;

} // namespace [aworx]


#if ALIB_MODULE_CONFIGURATION
    ALIB_CONFIG_VARIABLES( aworx::lib::Variables, aworx::ALIB, "Var" )
#endif



#endif // HPP_ALIB_LIB_ALIBMODULES

/** ************************************************************************************************
 * \file
 * This header file is part of file set \alibfs_modules of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_MODULES_DISTRIBUTION
#define HPP_ALIB_FS_MODULES_DISTRIBUTION 1

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

#if ALIB_CONFIGURATION && !defined(HPP_ALIB_CONFIG_VARIABLEDECL)
#   include "alib/config/variabledecl.hpp"
#endif

#if !defined(HPP_ALIB_MONOMEM_LIST)
#   include "alib/monomem/list.hpp"
#endif


ALIB_ASSERT_FILESET(MODULES)

// #################################################################################################
// Compilation flags
// Create a bitset from the ALib compilation features.
// For this, we ensure that the individual headers tested are included
// #################################################################################################

#if ALIB_SINGLETONS && !defined(HPP_ALIB_SINGLETONS_SINGLETON) && !defined(ALIB_DOX)
#   include "alib/singletons/singleton.hpp"
#endif

#if ALIB_BOXING && !defined(HPP_ALIB_BOXING_BOXING) && !defined(ALIB_DOX)
#   include "alib/boxing/boxing.hpp"
#endif


#if !defined(ALIB_DOX)

#   define ALIB_VFYBIT_DEBUG                                   (1LLU <<  0)

#   define ALIB_VFYBIT_MODULE_MONOMEM                          (1LLU <<  1)
#   define ALIB_VFYBIT_MODULE_SINGLETONS                       (1LLU <<  2)
#   define ALIB_VFYBIT_MODULE_CHARACTERS                       (1LLU <<  3)
#   define ALIB_VFYBIT_MODULE_ENUMS                            (1LLU <<  4)
#   define ALIB_VFYBIT_MODULE_TIME                             (1LLU <<  5)
#   define ALIB_VFYBIT_MODULE_BOXING                           (1LLU <<  6)
#   define ALIB_VFYBIT_MODULE_STRINGS                          (1LLU <<  7)
#   define ALIB_VFYBIT_MODULE_BITBUFFER                        (1LLU <<  8)
#   define ALIB_VFYBIT_MODULE_RESOURCES                        (1LLU <<  9)
#   define ALIB_VFYBIT_MODULE_THREADS                          (1LLU << 10)
#   define ALIB_VFYBIT_MODULE_TEXT                             (1LLU << 11)
#   define ALIB_VFYBIT_MODULE_RESULTS                          (1LLU << 12)
#   define ALIB_VFYBIT_MODULE_SYSTEM                           (1LLU << 13)
#   define ALIB_VFYBIT_MODULE_CONFIGURATION                    (1LLU << 14)
#   define ALIB_VFYBIT_MODULE_ALOX                             (1LLU << 15)
#   define ALIB_VFYBIT_MODULE_EXPRESSIONS                      (1LLU << 16)
#   define ALIB_VFYBIT_MODULE_CLI                              (1LLU << 17)

#   define ALIB_VFYBIT_FEAT_SINGLETON_MAPPED                   (1LLU << 40)

#   define ALIB_VFYBIT_CHARACTERS_WIDE                         (1LLU << 41)
#   define ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4                   (1LLU << 42)

#   define ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_INTEGRALS         (1LLU << 43)
#   define ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_CHARACTERS        (1LLU << 44)
#   define ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_FLOATS            (1LLU << 45)

#   define ALIB_VFYBIT_DEBUG_STRINGS                           (1LLU << 46)
#   define ALIB_VFYBIT_DEBUG_BOXING                            (1LLU << 47)
#   define ALIB_VFYBIT_DEBUG_MONOMEM_MONOMEM                    (1LLU << 48)
#   define ALIB_VFYBIT_DEBUG_RESOURCES                         (1LLU << 49)

#   define ALOX_VFYBIT_DBG_LOG                                 (1LLU << 50)
#   define ALOX_VFYBIT_DBG_LOG_CI                              (1LLU << 51)
#   define ALOX_VFYBIT_REL_LOG                                 (1LLU << 52)
#   define ALOX_VFYBIT_REL_LOG_CI                              (1LLU << 53)




#if ALIB_ALOX
#   if !defined(HPP_ALOX_PREDEF) && !defined(HPP_ALIB_ALOX)
#       include "alib/alox/alox.hpp"
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
 +  ALIB_VFYBIT_MODULE_MONOMEM                        * ALIB_MONOMEM                               \
 +  ALIB_VFYBIT_MODULE_SINGLETONS                    * ALIB_SINGLETONS                             \
 +  ALIB_VFYBIT_MODULE_CHARACTERS                    * ALIB_CHARACTERS                             \
 +  ALIB_VFYBIT_MODULE_ENUMS                         * ALIB_ENUMS                                  \
 +  ALIB_VFYBIT_MODULE_TIME                          * ALIB_TIME                                   \
 +  ALIB_VFYBIT_MODULE_BOXING                        * ALIB_BOXING                                 \
 +  ALIB_VFYBIT_MODULE_STRINGS                       * ALIB_STRINGS                                \
 +  ALIB_VFYBIT_MODULE_BITBUFFER                     * ALIB_BITBUFFER                              \
 +  ALIB_VFYBIT_MODULE_RESOURCES                     * ALIB_RESOURCES                              \
 +  ALIB_VFYBIT_MODULE_THREADS                       * ALIB_THREADS                                \
 +  ALIB_VFYBIT_MODULE_TEXT                          * ALIB_TEXT                                   \
 +  ALIB_VFYBIT_MODULE_RESULTS                       * ALIB_RESULTS                                \
 +  ALIB_VFYBIT_MODULE_SYSTEM                        * ALIB_SYSTEM                                 \
 +  ALIB_VFYBIT_MODULE_CONFIGURATION                 * ALIB_CONFIGURATION                          \
 +  ALIB_VFYBIT_MODULE_ALOX                          * ALIB_ALOX                                   \
 +  ALIB_VFYBIT_MODULE_EXPRESSIONS                   * ALIB_EXPRESSIONS                            \
 +  ALIB_VFYBIT_MODULE_CLI                           * ALIB_CLI                                    \
                                                                                                   \
 +  ALIB_VFYBIT_FEAT_SINGLETON_MAPPED                * ALIB_FEAT_SINGLETON_MAPPED                  \
                                                                                                   \
 +  ALIB_VFYBIT_CHARACTERS_WIDE                      * ALIB_CHARACTERS_WIDE                        \
 +  ALIB_VFYBIT_CHARACTERS_WCHAR_IS_4                * (ALIB_CHARACTERS_SIZEOF_WCHAR == 4 ? 1 : 0) \
                                                                                                   \
 +  ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_INTEGRALS      * ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS        \
 +  ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_CHARACTERS     * ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS       \
 +  ALIB_VFYBIT_FEAT_BOXING_BIJECTIVE_FLOATS         * ALIB_FEAT_BOXING_BIJECTIVE_FLOATS           \
                                                                                                   \
 +  ALIB_VFYBIT_DEBUG_STRINGS                        * ALIB_DEBUG_STRINGS                          \
 +  ALIB_VFYBIT_DEBUG_BOXING                         * ALIB_DEBUG_BOXING                           \
 +  ALIB_VFYBIT_DEBUG_MONOMEM_MONOMEM                * ALIB_DEBUG_MONOMEM                          \
 +  ALIB_VFYBIT_DEBUG_RESOURCES                      * ALIB_DEBUG_RESOURCES                        \
                                                                                                   \
 +  ALIB_COMPILATION_FLAGS_ALOX                                                                    \
)
#endif //ALIB_DOX

namespace aworx { namespace lib {

// #################################################################################################
// Variables
// #################################################################################################
#if ALIB_CONFIGURATION
    /**
     * Configuration variables of \alib.
     * \note
     *  As required by module \alib_config, this enumerations is equipped with
     *  \ref alib_enums_records "ALib Enum Records" of type \alib{config,VariableDecl}
     *  and resourced with this module.<br>
     *  Hence, all variable categories, names, default values and such can be modified
     *  by modifying the resource data of the singleton of class \aworx{lib,ALibDistribution}.
     */
    enum class Variables
    {
        LOCALE               = 1   , ///< \alib Resourced variable.
        WAIT_FOR_KEY_PRESS   = 2   , ///< \alib Resourced variable.
        HAS_CONSOLE_WINDOW   = 3   , ///< \alib Resourced variable.
    };
#endif // ALIB_CONFIGURATION


/** ************************************************************************************************
 * This is a special module singleton that provides some "knowledge" about the modules contained
 * in a distribution. It offers method #BootstrapFillDefaultModuleList which is used
 * by method \alib{Module::Bootstrap} in the case that the module list was not externally customized
 * and filled already.
 *
 * Usually, this module is the one that method \b Module::Bootstrap is invoked on.
 * All details on this topic are given in chapter
 * \ref alib_manual_bootstrapping "3. Bootstrapping ALib" of the \ref alib_manual.
 *
 * Besides that, the class provides a few utility members that have not found a different home
 * with the current version of the library.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::ALIB.
 **************************************************************************************************/
class ALibDistribution  : public Module
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * Lists all modules in the order of bootstrapping. Shutdown is performed in reverse order.
         * This list is used by method \alib{Module::Bootstrap}, regardless on which module it was
         * invoked. If the list is empty at the time of invocation, method Module::Bootstrap
         * will use this class's method #BootstrapFillDefaultModuleList to fill it with the default
         * \alib modules in the right order.
         *
         * To customize the list (append own modules), method #BootstrapFillDefaultModuleList may
         * be invoked prior to \b Module::Bootstrap and afterwards the list might be extended.
         *
         * \see Chapter \ref alib_manual_bootstrapping_class_alibdistribution and the following
         *      ones to learn about how to customize bootstrapping in various ways.
         */
        List<Module*>           Modules;

        /**
         * If \c true, within #Shutdown, it is waited for a key press in the console
         * window.<br>
         * By default, this flag is enabled when debugging a console application under Visual
         * Studio.<br>
         * This default behavior can be overruled by setting configuration variable
         * \https{ALIB_WAIT_FOR_KEY_PRESS,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}.
         * <br>In addition, this public flag may be modified at run-time (after method #Bootstrap was
         * invoked).
         */
        bool                    WaitForKeyPressOnTermination                                 =false;

        /**
         * This flag indicates if the current process has console output window attached.
         * While this is not detectable in Unix like systems (or just with huge efforts), in
         * windows it is. The determination is performed in #bootstrap and done as follows
         *  - If configuration variable \b ALIB_HAS_CONSOLE_WINDOW
         *    is set, its value is returned.
         *  - Otherwise, on Unix like OS\c true is returned.
         *  - On Windows OS it is determined if a console window is attached to the process
         *    using system call \e GetConsoleWindow() and compare its result to \e NULL.
         *
         * In addition, this public flag may be modified at run-time (after method #Bootstrap was
         * invoked).
         *
         * \note
         *   The evaluation of configuration variable \b ALIB_HAS_CONSOLE_WINDOW is done
         *   only if module \alib_config is included in the \alibdist.
         */
        bool                    HasConsoleWindow;

        /**
         * This string is set during bootstrapping in the case that a locale information was
         * found (via standard environment variables or \alib variable \aworx{lib,Variables,LOCALE})
         * and successfully set.<br>
         * Otherwise this string is \e nulled.
         */
        String                  LocaleFound;

        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Bootstrap(int, nchar**,BootstrapPhases)} or
         * \alib{Module::Bootstrap(int, wchar**,BootstrapPhases)}.
         */
        int                     ArgC                                                            = 0;

        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Bootstrap(int, char**,BootstrapPhases)}.
         */
        const char**            ArgVN                                                     = nullptr;

        /**
         * The command line arguments stored for convenience.
         * Only available, if (recursive) module initialization was performed with
         * \alib{Module::Bootstrap(int, wchar_t**,BootstrapPhases)}.
         */
        const wchar_t**         ArgVW                                                     = nullptr;

    // #############################################################################################
    // Construction/Destruction
    // #############################################################################################
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        ALibDistribution();

    // #############################################################################################
    // Environment definition/detection
    // #############################################################################################
    public:

        #if ALIB_SYSTEM
            /** ************************************************************************************
             * If \c true, the current process is under the hood of a debugger.
             *
             * \note
             *   This method is available only if module \alib_system is included in the
             *   \alibdist.
             *
             * @return \c true if the application is run in a debugger.
             **************************************************************************************/
            ALIB_API
            bool        IsDebuggerPresent();

        #endif

    // #############################################################################################
    // Compilation Flags
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Checks the versions of \alib and whether the current compilation unit shares compatible
         * compiler symbols with the \alib binaries. The latter is done by invoking
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
        void            CheckDistribution( int      alibVersion        = ALIB_VERSION,
                                           uint64_t compilationFlags   = ALIB_COMPILATION_FLAGS );

    // #############################################################################################
    // Methods for being a module ourselves
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Fills list #Modules, taking the permutation of modules contained in the \alibdist into
         * account.
         ******************************************************************************************/
        ALIB_API
        void            BootstrapFillDefaultModuleList();


    protected:
        /** ****************************************************************************************
         * Implementation of \alib{Module::bootstrap}.
         *
         * In phase \alib{Module,BootstrapPhases::Final} performs:
         * - In glibc versions of \alib (GNU/unix) probably invoke glibc method
         *   <em>setlocale()</em>, depending on the setting of the environment variables
         *   <em>LANG</em> and <em>LANGUAGE</em>
         *   and depending on \alib configuration variable
         *   \https{ALIB_LOCALE,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}.
         *
         *   In addition, invokes
         *   \alib{strings,TNumberFormat::SetFromLocale,NumberFormat::SetFromLocale} on static singleton
         *   \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * - Config variable
         *   \https{WAIT_FOR_KEY_PRESS,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}
         *   is read and field #WaitForKeyPressOnTermination set accordingly
         *
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase,
                                   int argc, const char** argv, const wchar_t** wargv )    override;

        /** ****************************************************************************************
         * Terminates this module.
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                   override;


};// class ALibDistribution

} // namespace aworx[::lib]

/** The singleton of module-class \alib{ALibDistribution}. */
extern ALIB_API lib::ALibDistribution ALIB;

} // namespace [aworx]

#if ALIB_CONFIGURATION
    ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::Variables, aworx::lib::config::VariableDecl )
    ALIB_RESOURCED_IN_MODULE( aworx::lib::Variables, aworx::ALIB, "Var" )
#endif


#endif // HPP_ALIB_FS_MODULES_DISTRIBUTION

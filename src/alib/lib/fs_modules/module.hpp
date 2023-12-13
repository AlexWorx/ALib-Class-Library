/** ************************************************************************************************
 * \file
 * This header file is part of file set \alibfs_modules of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_MODULES_MODULE
#define HPP_ALIB_FS_MODULES_MODULE 1

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL)
#   include "alib/enums/underlyingintegral.hpp"
#endif


ALIB_ASSERT_FILESET(MODULES)

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif
#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif
#if !defined (_GLIBCXX_LIST) && !defined(_LIST_)
#   include <list>
#endif


namespace aworx  {
namespace lib {

namespace resources { class ResourcePool;  }
namespace config    { class Configuration; }


/** ************************************************************************************************
 * This class is used by \alib to
 *
 * - manage library bootstrap and termination,
 * - provide \ref alib_mod_resources "resources" and to
 * - provide Configuration data (e.g. configuration files, command line parameters, etc.).
 *
 * Typically, every "full" \alibmod disposes about a type derived from this class. The singleton
 * instances of these types are collected in namespace \ref aworx::lib and are named in
 * capital letters, for example \ref aworx::lib::ALOX, \ref aworx::lib::EXPRESSIONS, etc.
 *
 * Types found in an \alibmod_nl, will use methods \alib{Module,GetResourcePool} and
 * \alib{Module,GetConfig} of that singleton to access resources and to read and write configuration
 * data.
 * With that, it has to be kept in mind that these objects most probably are shared with other
 * modules. As a consequence, clear and understandable resource and variable categories and names
 * are to be used, to avoid conflicting entries.
 *
 * The \ref alib_manual provides several chapters that explain how
 * \ref alib_manual_bootstrapping "3. Bootstrapping ALib" is performed with the help of this
 * class.<br>
 * Furthermore, source code sample is given with the
 * \ref alib_cli_tut "tutorial of ALib module CLI".
 *
 * Often, types that comprise a "module singleton" (as the derived types shown in the inheritance
 * diagram above) incorporate some further functionality specific to the field of application that
 * a module covers.
 *
 * # Reference Documentation #
 **************************************************************************************************/
class Module
{
    public:
        /** Initialization levels usable with #Bootstrap methods. */
        enum class BootstrapPhases
        {
            PrepareResources = 1,    ///< Creates field #resourcePool and loads its module specific
                                     ///< default values.
            PrepareConfig    = 2,    ///< Initializes up to the creation of a field
                                     ///< \aworx{lib::Module,config}.
            Final            = 3,    ///< The final initialization phase.
        };

        /** Termination levels usable with #Shutdown methods.    */
        enum class ShutdownPhases
        {
            Announce         = 1,    ///< Signals termination. Keeps resources, config, etc.
                                     ///< intact. Usable to write configuration data, stop
                                     ///< background threads, etc.
            Destruct         = 2,    ///< The main phase of termination that destructs everything.
        };

    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

        /** State of initialization, used to avoid double initialization. */
        int                             bootstrapState                                          = 0;

        /** State of initialization of the "micro modules".  */
        static bool                     microModulesInitialized;

        /**
         * Pointer to the resource pool.
         * An instance is created and assigned with the invocation of one of the #Bootstrap methods.
         * This instance is then passed to all dependent libraries (recursively) and this way
         * shared. Dependent libraries that are to an own dedicated instance, have to be initialized
         * explicitly prior to initializing this module.
         */
        resources::ResourcePool*        resourcePool                                      = nullptr;

        /**
         * If a default instance of type \alib{resources,LocalResourcePool} has been created when
         * this module was initialized, this flag evaluates to \c true and the instance stored in
         * #resourcePool will be deleted with destruction of this object.
         */
        bool                            isResourceOwner                                     = false;


        #if ALIB_CONFIGURATION
            /**
             * Pointer to the configuration instance used to load variable configuration data.
             * An instance is created and assigned with the invocation of one of the #Bootstrap methods.
             * This instance is then passed to all dependent libraries (recursively) and this way
             * shared. Dependent libraries that are to an own dedicated instance, have to be
             * initialized explicitly prior to initializing this module.
             *
             * Access to the field is provided with method #GetConfig.
             *
             * \note
             *   This field is available only with \alib module
             *   \ref aworx::lib::config "Configuration" included.
             */
            config::Configuration*      config                                            = nullptr;

            /**
             * If a configuration instance has been created when this module was initialized, this
             * flag evaluates to \c true and the instance stored in
             * #config will be deleted with destruction of this object.
             */
            bool                        isConfigOwner                                       = false;
        #endif // ALIB_CONFIGURATION

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * The module version. Standard \alib versioning scheme is YYMM (as integer number)
         * of the module release date.
         * Besides this version number, field #Revision indicates if this is a revised version
         * of a former release.
         */
        const int                       Version;

        /**
         * The revision number of this release. Usually a module is initially released as
         * revision \e 0. Pure bug-fix releases that do not change the interface of \alib
         * are holding the same #Version but an increased number in this field.
         */
        const int                       Revision;

        /**
         * The name of the configuration category of configuration variables used by this module.<br>
         * \alib itself uses "ALIB". \alib modules use the naming scheme
         * <c>ALIB_{MODULE_NAMESPACE}</c>.<br>
         */
         NCString                       ResourceCategory;

        /**
         * These flags are used internally to detect incompatibilities when linking \alib to
         * binaries that use different compilation flags.
         * Declared public to allow exposure, e.g. in verbose logging.
         * \see Method #VerifyCompilationFlags for more information.
         */
        const uint64_t                  CompilationFlags;

        /**
         * This is for the creation of output on information about the bits found in
         * field #CompilationFlags.
         * Declared public to allow exposure, e.g. in verbose logging.
         * \see Method #VerifyCompilationFlags for more information.
         */
        std::vector<std::pair <const nchar*, uint64_t>>  CompilationFlagMeanings;


    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor. Just stores the given values in corresponding fields.
         * @param version           Value for field #Version.
         * @param revision          Value for field #Revision.
         * @param resourceCategory  Value for field #ResourceCategory.
         * @param compilationFlags  Value for field #CompilationFlags.
         *                          See method #VerifyCompilationFlags.
         *                          Defaults to \c 0.
         ******************************************************************************************/
        Module( int             version,
                int             revision,
                const NCString& resourceCategory,
                uint64_t        compilationFlags = 0 )
        : Version         (version         )
        , Revision        (revision        )
        , ResourceCategory(resourceCategory)
        , CompilationFlags(compilationFlags)
        {}

        /**  Deleted copy constructor.  */
        Module( const Module& )                                                            = delete;

        /**  Deleted move constructor.  */
        Module( Module&& )                                                                 = delete;

        /**  Deleted copy assignment.  */
        void operator=( const Module& )                                                    = delete;

        /**  Deleted move assignment.  */
        void operator=(  Module&& )                                                        = delete;


        /** ****************************************************************************************
         * Virtual destructor to satisfy virtual methods.
         ******************************************************************************************/
        virtual ~Module()
        {
            ALIB_ASSERT_WARNING( bootstrapState == - UnderlyingIntegral( ShutdownPhases::Destruct),
                                 "MODULES",
                                 "Destructing a non-terminated module. "
                                 "Module Resource category: ", ResourceCategory  )
        }

    // #############################################################################################
    // Bootstrap/Shutdown()
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Initializes this module up to the given \p{targetPhase}.
         * If the module was initialized for the given level already, nothing is done and
         * \c false is returned.
         *
         * Checks if module list \alib{ALibDistribution::Modules} of singleton #aworx::ALIB
         * was filled already (by the using software for the purpose to add custom modules).
         * If not, \alib{ALibDistribution::BootstrapFillDefaultModuleList} is invoked to place
         * the \alib modules as contained in the \alibdist_nl.
         *
         * If method \alib{Module,BootstrapSetResourcePool} is \b not invoked prior to invoking this
         * method, an object of type \alib{resources,LocalResourcePool} is created and shared with
         * each lower-level module, until a module is found that has a resource pool set.
         *
         * If method \alib{Module,BootstrapSetConfig} is \b not invoked prior to invoking this
         * method, with parameter \p{targetPhase} set equal or higher than
         * \aworx{lib,Module::BootstrapPhases::PrepareConfig,BootstrapPhases::PrepareConfig}, then
         * an object of type \alib{config,Configuration} is created and shared with each lower-level
         * module, until a module is found that has a configuration instance set.
         *
         * This approach of creating and distributing the resource pool and the configuration,
         * together with the possibility to invoke this method on a module that resides anywhere
         * in list \alib{ALibDistribution::Modules}, allows to customize bootstrapping and module
         * setup in various ways.
         *
         * Parameters \p{argc}, \p{argvN} and \p{argvW} may be used to provide command line
         * arguments. They will be passed to abstract method #bootstrap of all dependent libraries.
         * Special singleton \ref aworx::ALIB of type \alib{ALibDistribution} will store the
         * parameters for convenience in publicly accessible fields.
         * Note that only either of parameters \p{argvN} and \p{argvW} is to be provided
         * (the other should be \e nulled), depending on system/compilation preferences and on the
         * character size that the compilation toolset provides with method \c main().
         *
         * \see
         *   For details on when and how to invoke this method, see the detailed explanations in
         *   chapter \ref alib_manual_bootstrapping "3. Bootstrapping ALib" of the \ref alib_manual.
         *
         * @param targetPhase The initialization level to perform.
         *                    Defaults to <c>BootstrapPhases::Final</c>.
         * @param argc        Parameter usually taken from <em>standard C</em> \c main() method
         *                    (the number of arguments in \p{argv}).
         *                    Defaults to <c>0</c>.
         * @param argvN       The command line parameters given as a zero-terminated list to
         *                    zero-terminated narrow strings.
         *                    Defaults to <c>nullptr</c>.
         * @param argvW       The command line parameters given as a zero-terminated list to
         *                    zero-terminated wide strings.
         *                    Defaults to <c>nullptr</c>.
         * @return \c true if the module was not initialized before, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool            Bootstrap( BootstrapPhases targetPhase = BootstrapPhases::Final,
                                   int             argc        = 0,
                                   const char**    argvN       = nullptr,
                                   const wchar_t** argvW       = nullptr            );

#if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Inline convenience shortcut to method
         * \alib{Module,Bootstrap(BootstrapPhases\,int\,const char**\,const wchar_t**)} accepting
         * either \b narrow or \b wide character command line arguments using one templated
         * method parameter.
         *
         * @tparam TChar      The character type of parameter \p{argv}.
         * @param argc        The number of arguments.
         * @param argv        The command line arguments as \b nchar or \b wchar, constant or not.
         * @param targetPhase The initialization level to perform.
         *                    Defaults to \alib{Module,BootstrapPhases::Final}.
         * @return \c true if the module was not initialized before, \c false otherwise.
         ******************************************************************************************/
        template<typename TChar>
        bool            Bootstrap( int argc, TChar** argv, BootstrapPhases targetPhase= BootstrapPhases::Final );
#else
        template<typename TChar>
        ATMP_T_IF( bool,   ATMP_EQ( ATMP_RC(TChar), char   )
                        || ATMP_EQ( ATMP_RC(TChar), wchar_t)   )
        Bootstrap( int argc, TChar** argv, BootstrapPhases targetPhase= BootstrapPhases::Final )
        {
            if( std::is_same<typename std::remove_const<TChar>::type, char>::value )
                return Bootstrap( targetPhase, argc,
                                  reinterpret_cast<const char   **>(const_cast<const TChar**>(argv)),
                                  nullptr );
            else
                return Bootstrap( targetPhase, argc, nullptr,
                                  reinterpret_cast<const wchar_t**>(const_cast<const TChar**>(argv)));
        }
#endif

        /** ****************************************************************************************
         * De-initializes modules found in list \alib{ALibDistribution::Modules} of singleton
         * #aworx::ALIB# in reverse order.
         *
         * With built-in \alibmods_nl, no urgent actions are performed apart from memory
         * de-allocation, which is anyhow done by modern operating systems with the termination of
         * a process. However, still this method should be invoked in the case of future changes.
         * Also, if using memory analysis tools (such as Valgrind) it is important to remove any
         * internal allocations before a program terminates.
         *
         * Custom modules might use the first of the two termination levels
         * (\alib{Module,ShutdownPhases::Announce}) to close background threads or write
         * configuration data.
         *
         * Only in the second level (\alib{Module,ShutdownPhases::Destruct}) actions are to be taken
         * that disables and destructs a software.
         *
         * @param targetPhase The termination level to perform.
         *                    Defaults to \alib{Module,ShutdownPhases::Destruct}.
         ******************************************************************************************/
        ALIB_API
        void            Shutdown( ShutdownPhases targetPhase= ShutdownPhases::Destruct );


        /** ****************************************************************************************
         * Tests if the module was completely initialized.
         *  @return
         *    \c true if the initialization state is either \alib{Module,BootstrapPhases::Final} or
         *    \alib{Module,ShutdownPhases::Announce}, \c false otherwise.
         ******************************************************************************************/
        bool            IsBootstrapped()
        {
            return     bootstrapState ==   UnderlyingIntegral(BootstrapPhases::Final)
                    || bootstrapState == - UnderlyingIntegral(ShutdownPhases::Announce);
        }

    // #############################################################################################
    // Other public interface methods
    // #############################################################################################
    public:
        #if ALIB_CONFIGURATION
            /** ************************************************************************************
             * Sets the configuration object of this module.
             *
             * \attention
             *   This method must be invoked before method #Bootstrap is invoked with
             *   parameter \p{targetPhase} set equal or higher than
             *   \alib{Module,BootstrapPhases::PrepareConfig},
             *   because in this phase, internally a configuration is created if none is set.
             *
             * \attention
             *   Method #Bootstrap will distribute the given instance to each lower level
             *   module that has not received a different object.
             *
             * \see
             *   Chapter \ref alib_manual_bootstrapping of the Programmer's Manual of \alib.
             *
             * @param pConfig The external configuration object to use.
             **************************************************************************************/
            void  BootstrapSetConfig(config::Configuration* pConfig)
            {
                ALIB_ASSERT_ERROR( config == nullptr, "MODULES",
                                   "This module already has a configuration object set." )
                config= pConfig;
            }

            /** ************************************************************************************
             * Returns a reference to the configuration object.
             * @return The configuration object set with bootstrapping.
             **************************************************************************************/
            config::Configuration&  GetConfig()
            {
                return *config;
            }
        #endif

        /** ****************************************************************************************
         * Sets the resource pool of this module.
         *
         * \attention
         *   This method must be invoked before method #Bootstrap,
         *   because in the first phase \alib{Module,BootstrapPhases::PrepareConfig}, internally a
         *   pool of type \alib{resources,LocalResourcePool} is created if none is set.
         *
         * \attention
         *   Method #Bootstrap will distribute the given instance to each lower level
         *   module that has not received a different object.
         *
         * \see
         *   Chapter \ref alib_manual_bootstrapping of the Programmer's Manual of \alib.
         *
         * @param pool The external resource pool to use.
         ******************************************************************************************/
        void            BootstrapSetResourcePool(resources::ResourcePool* pool)
        {
            ALIB_ASSERT_ERROR( bootstrapState == 0, "MODULES",
                               "A custom resource pool must be set prior to module initialization.")
            resourcePool= pool;
        }

        /** ****************************************************************************************
         * Returns a reference to the resource pool.
         * @return The resource pool set with bootstrapping.
         ******************************************************************************************/
        ResourcePool&   GetResourcePool()
        {
            return *resourcePool;
        }

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,ResourcePool::Bootstrap} on field
         * #resourcePool providing field #ResourceCategory as parameter.
         *
         * @param name  The resource name.
         * @param data  The resource data string.
         ******************************************************************************************/
        void            BootstrapResource( const NString& name, const String& data )
        {
            resourcePool->Bootstrap( ResourceCategory, name, data );
        }

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,ResourcePool::Get} on field
         * #resourcePool providing field #ResourceCategory as parameter.
         *
         * With debug builds, this method asserts that a resource was found. If this is not
         * wanted, use #TryResource.
         *
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        const String&   GetResource( const NString& name )
        {
            return resourcePool->Get( ResourceCategory, name   ALIB_DBG(, true)  );
        }

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,ResourcePool::Get} on field
         * #resourcePool providing field #ResourceCategory as parameter.
         *
         * \note
         *   Usually, it is recommended to use #GetResource, which asserts with debug builds
         *   if a resource was not found.
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        const String&   TryResource( const NString& name )
        {
            return resourcePool->Get( ResourceCategory, name  ALIB_DBG(, false)  );
        }

        /** ****************************************************************************************
         * Compares a given set of compilation flags with the internal set. This is an optional
         * feature of modules which expose compiler definitions that lead to incompatible
         * module versions when mixed.
         *
         * If used, the following steps have to be performed:
         * - By the module that offers the feature:
         *   - Expose a preprocessor macro that collects the bits at compile-time.
         *     As a sample see macro \ref ALIB_COMPILATION_FLAGS.
         *   - The constructor of the derived module needs to be defined in a compilation unit of
         *     the module (aka not inline) and needs to pass the macro as the value for optional
         *     parameter \p{compilationFlags} to this class's constructor.
         *   - The constructor of the derived module needs to fill field #CompilationFlagMeanings.
         *
         * - The user of the module now invokes this method in bootstrap, hence preferably at
         *   the beginning of method #bootstrap with \alib{Module,BootstrapPhases::PrepareResources}.
         *
         * In case they are different in a way that the module gets incompatible (e.g. different
         * class sizes, which results in errors which are confusing the debugger), the flags are
         * written to \c std::cout for comparison and \c false is returned.
         *
         * @param compilationFlags The flags as defined in invoking compilation unit.
         *                         Defaults to \c ALIB_COMPILATION_FLAGS.
         *
         * @return \c true if compatible, \c false else.
         ******************************************************************************************/
        ALIB_API
        bool            VerifyCompilationFlags( uint64_t compilationFlags );


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Abstract method which is invoked during bootstrapping by method #Bootstrap for each
         * phase, with all modules found in list \alib{ALibDistribution::Modules} of singleton
         * #aworx::ALIB.
         *
         * Note that the availability of parameters \p{argc}, \p{argv} and \p{wargv} depend
         * on the using code of the library, namely if there, the arguments are passed to
         * \alib{Module::Bootstrap}.
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase,
                                   int argc, const char** argv, const wchar_t**  wargv )        = 0;

        /** ****************************************************************************************
         * Abstract method invoked by #Shutdown.
         * @param phase  The termination level to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                     = 0;

};// class Module

} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using BootstrapPhases   =  lib::Module::BootstrapPhases;

/// Type alias in namespace #aworx.
using ShutdownPhases =  lib::Module::ShutdownPhases;

} // namespace [aworx]



#endif // HPP_ALIB_FS_MODULES_MODULE

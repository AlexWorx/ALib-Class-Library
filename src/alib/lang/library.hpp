// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB_LIB_LIBRARY
#define HPP_ALIB_LIB_LIBRARY 1

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION)
    #error "include 'alib/alib.hpp' instead of this header"
#endif


namespace aworx  { namespace lib { namespace lang {


/** ************************************************************************************************
 * ## Introduction ##
 *
 * This class is used by \alib to manage the library bootstrap and termination, externalized
 * resources, configuration data and such.
 *
 * As \alib itself is modularized, almost every \alib namespace is designed to be used and
 * distributed solely or with just a smaller foundation part of \alib. Therefore, each \alib module
 * or namespace uses its own instance of this class.<br>
 * Of-course, software using \alib, may use this class in the same way as <b>%ALib Modules</b>,
 * \alox and other A-Worx software does.
 *
 *
 *
 * \~Comment #################################################################################### \~
 * ## %Library Hierarchies ##
 *
 * Software libraries generally depend on other libraries. Such dependencies on a larger scale
 * should be non-circular, but on a lower scale this is often not avoidable. As \alib, with the use
 * of this class treats its namespaces as libraries, there are many circular samples found like:
 * \ref aworx::lib::config relies on \ref aworx::lib::strings while parts of the string utility
 * and formatter classes in turn rely on \ref aworx::lib::config. For modularized distribution,
 * such dependencies often get removed using \ref GrpALibCodeSelectorSymbols and cutting some
 * functionality that only exists in a more complete or full distribution.
 *
 * Now, the approach that \alib takes to bootstrap, share resources and configuration variables
 * is as follows:
 * - Each module/namespace derives a library class from this type. The name of the class
 *   corresponds to the namespace name, but written in capital letters.
 * - A static singleton instance of this library class is placed in namespace
 *   \ref aworx::lib, named the same as the class itself but in capital letters.
 * - With the two rules above, the library singletons are:
 *   - \ref aworx::lib::threads::Thread  \ref aworx::lib::THREADS
 *   - \ref aworx::lib::system::System   \ref aworx::lib::SYSTEM
 *   - \ref aworx::lib::strings::Strings \ref aworx::lib::STRINGS
 *   - \ref aworx::lib::boxing::Boxing   \ref aworx::lib::BOXING
 *   - \ref aworx::lib::config::Config   \ref aworx::lib::CONFIG
 *   - etc... (See class inheritance diagram above for a complete list and namespace documentation
 *     \ref aworx::lib for the singletons.)
 * - In addition to this, there is one central \alib library found with static field:
 *   - \ref aworx::lib::ALib   \ref aworx::lib::ALib
 * - In the constructor, dependency libraries are added to field #dependencies as needed and
 *   by using \ref GrpALibCodeSelectorSymbols with modularized \alib distributions.
 *
 *
 * \~Comment #################################################################################### \~
 * ## %Resources and %Configuration ##
 *
 * This class has pointer fields #Config and #Res. Classes of a library will use these
 * instances by addressing them through the singleton of their library found in namespace
 * \ref aworx::lib (or through wherever a custom library places its singleton).
 *
 * A library has to be aware that these objects may be shared with a lot of other libraries.
 * Therefore, both classes provide a \ref aworx::lib::threads::ThreadLock "thread safe" access
 * when used. Furthermore, a consequence is that clear and understandable resource and variable
 * categories and names are to be used, to avoid conflicting entries.
 *
 * Among other things, the next sections explain how a user may control which libraries share
 * resources and/or configuration singletons, respectively how to initialize libraries independently
 * for not sharing such objects.
 *
 *
 * \~Comment #################################################################################### \~
 * ## Bootstrapping ##
 *
 * The C++ language specification misses rules and clarity about how static data - whose types might
 * execute constructor code - are initialized, especially in which order. Therefore, in this
 * language it is no option to create "self-initializing" libraries.
 * Bootstrapping becomes even more complicated with the use of resources and configuration variables
 * that both may be equipped with custom data settings or even plug-ins that load data from custom
 * places.<br>
 * This class therefore tries to streamline and simplify bootstrapping.
 *
 * Implementations of this class need to implement two protected, \b abstract methods:
 * - #init and
 * - #terminationCleanUp.
 *
 * While the latter is invoked only once, the initialization is done in phases defined with
 * #Phases. There are three phases defined and the implementations of #init need to
 * switch upon them:
 * - \alib{lang,Library::Phases::resourceset,Phases::resourceset}<br>
 *   In this phase is is assured that all static objects of C++ got initialized (because
 *   bootstrapping is called explicitly from a code executed within the C++ <c>main()</c>
 *   function).<br>
 *   A library that uses resources needs to add default values of such resources in this phase
 *   to field #Res. This is done either "hard coded" or using a third party resource
 *   loader.
 * - \alib{lang,Library::Phases::resourceload,Phases::resourceload}<br>
 *   In this phase, resources that are <b>not lazily loadable</b> need to be loaded here explicitly.
 *   Samples of lazily loadable resources are:
 *   - \alib{strings,ResourceString}
 *   - Resourced \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } (when using the built-in
 *     implementation)
 *   - Resourced \alib \ref ALIB_CONFIG_VARIABLES "configuration variable declarations".
 *   - Resourced \alib \ref ALIB_LANG_EXCEPTIONS "exceptions".
 * - \alib{lang,Library::Phases::final,Phases::final}<br>
 *   In this phase, final initialization steps may be performed. Especially, configuration
 *   variables are (only) available now.
 *   As a sample, in this phase, class \ref aworx::lib::ALib sets the processes' \e locale depending
 *   on configuration variable \b ALIB_LOCALE.<br>
 *
 * Now, to initialize a library, one of the overloaded  #Init methods is to be called by the using
 * software on the "highest" level of libraries. This means init has to be invoked on the minimum
 * subset of libraries that together (recursively) through their dependencies just do initialize all
 * others.
 * It is no problem if several libraries share the same dependencies, or circular dependencies
 * exist. Each library is initialized only once.
 *
 * The #Init method will create instances of classes
 * \alib{lang,Resources} and
 * \alib{config,Configuration} and recursively pass these to underlying libraries. Furthermore,
 * the implementations of abstract #init methods will be recursively called (with a depth first
 * algorithm).
 *
 * All overloaded #Init methods have parameter \p{level}, which defaults to
 * \alib{lang,Library::InitLevels::All,InitLevels::All}. In a next section it is explained when
 * to specify this parameter to a different value.
 *
 *
 * \~Comment #################################################################################### \~
 * ## Bootstrapping With Custom %Resources and/or %Configuration Plug-ins##
 *
 * As documented with classes \alib{lang,Resources} and \alib{config,Configuration}, both classes
 * allow to be equipped with custom mechanics for loading external data. To do so, the bootstrapping
 * of \alib libraries need to be explicitly invoked for separated phases, by setting parameter
 * \p{level} of the #Init methods as follows:
 * - <b>To create object #Res and fill it with default values:</b><br>
 *    - #Init needs to be called with parameter \p{level} set to
 *      \alib{lang,Library::InitLevels::PrepareResources,InitLevels::PrepareResources}.
 *    - After that, object #Res may be equipped with custom resource loading mechanics.
 *    - Now, #Init needs to be called again, e.g. with default value of \p{level},
 *      \alib{lang,Library::InitLevels::All,InitLevels::All}
 *
 * - <b>To add custom configuration plug-ins:</b><br>
 *    - #Init needs to be called with parameter \p{phases} set to
 *      \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}
 *    - After that, object #Config may be equipped with a custom plug-in, e.g. of type
 *      \alib{config,IniFile}.
 *    - Now, #Init needs to be called again with \p{phases} set to
 *      \alib{lang,Library::InitLevels::All,InitLevels::All}.
 *
 * Of-course, both approaches can be mixed. In this case, the following order has to be adhered:
 * - Initialize with \alib{lang,Library::InitLevels::PrepareResources,InitLevels::PrepareResources}
 * - Customize #Res
 * - Initialize with \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}
 * - Customize #Config
 * - Initialize with \alib{lang,Library::InitLevels::All,InitLevels::All}
 *
 *
 * \~Comment #################################################################################### \~
 * ## Using Different %Resources and Configuration Instances ##
 *
 * As explained above, the variants of #Init create corresponding class instances for fields
 * #Res and #Config and pass them recursively down to dependent libraries. Now, if
 * different, dedicated instances of these objects should be used with different libraries,
 * the the simple approach is to call the #Init methods explicitly. Of-course, "lower level"
 * libraries need to be initialized first, higher level next, as otherwise the higher level
 * libraries would already initialize the lower ones.
 *
 * The goal of doing so is separate resources and configuration variables of different libraries
 * in different external configuration files, databases or whatsoever.
 *
 * To get a better understanding lets look at a sample. Requirements should be:
 * - An application <b>"MyApp"</b> uses \alox logging library, which is a module of \alib, and
 *   as such is registered as a dependency library.
 * - The application uses \alib library mechanics (hence a singleton of a custom type derived from this class!)
 * - The application uses class \alib{config,IniFile} to load custom \alib{config,Variable} objects.
 * - The application wants to store \alib and \alox variables in separated files, just to
 *   remove "clutter" from its main configuration file and document the use of the \alib/\alox
 *   specific variables nicely separated from the main configuration file's documentation.
 *
 * What has to be done is:
 * - Initialize \alib with level
 *   \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}
 * - Add a custom configuration plug-in (e.g \alib{config,IniFile}) to singleton
 *   #aworx::lib::ALIB.
 * - Initialize \alib with level \alib{lang,Library::InitLevels::All,InitLevels::All}
 *
 * Now \alib and all modules included in the chosen setup are initialized and all share the same
 * configuration and resource objects.
 *
 * - Then do the same with the application specific library singleton and provide a different
 *   custom plug-in to it's configuration.
 *
 * With this approach, both objects resources and configuration are separated.
 * If just the configuration should be separated, the steps to be done are:
 * - Make library singleton \ref #aworx::lib::ALIB a dependency library of <b>"MyApp"</b>.
 *   (This is only an optional step in the previous to do list.)
 * - Initialize <b>"MyApp"</b> with level
 *   \alib{lang,Library::InitLevels::PrepareResources,InitLevels::PrepareResources}.<br>
 *   (Now all libraries share the same resources!)
 * - Initialize \alib with level
 *   \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}
 * - Add custom configuration plug-in (e.g \alib{config,IniFile}) to singleton
 *   #aworx::lib::ALIB.
 * - Initialize \alib with level \alib{lang,Library::InitLevels::All,InitLevels::All}.<br>
 *
 * - Initialize <b>"MyApp"</b> with level
 *   \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}
 * - Add custom configuration plug-in (e.g \alib{config,IniFile}) to <b>"MyApp"</b>
 * - Initialize <b>"MyApp"</b> with level \alib{lang,Library::InitLevels::All,InitLevels::All}
 *
 *
 * \~Comment #################################################################################### \~
 * ## Custom Methods ##
 *
 * A library might decide to add custom library methods, mostly static functionality to the
 * the library specific derived class. In case of static methods, those could alternatively be
 * namespace functions instead. It is up to the library designer what to incorporate. In general,
 * the use of a static method might be more readable in code than just a namespace function.
 * Furthermore, custom library types of \alib itself are \ref aworx "mirrored in namespace aworx"
 * and thus can be used without their full namespace prefix when just statement
 *
 *          using namespace aworx;
 *
 * is placed in a compilation unit, which is always advisable.
 *
 * \~Comment #################################################################################### \~
 * # Reference Documentation #
 *
 **************************************************************************************************/
class Library
{
    // #############################################################################################
    // Phase enumerations
    // #############################################################################################
    public:
        /**
         * Initialization levels usable with #Init methods.
         *
         * \see This classes' main documentation for detailed information.
         */
        enum class InitLevels
        {
            PrepareResources = 1,    ///< Creates field #Res and loads its library specific default
                                     ///< values.
            PrepareConfig    = 3,    ///< Initializes up to the creation of a field
                                     ///< \alib{lang,Library::Config,Config}.
            All              = 7     ///< Initializes all phases not initialized, yet.
        };

        /**
         * Internal version of initialization phases.<br>
         * (Note: The reason why this enum class is public, is that use \alib{lang,T_EnumIsBitwise}
         * mechanics on it).
         */
        enum class Phases
        {
            resourceset      = (1 << 0),    ///< Demands initialization of static data including
                                            ///< default resource data.
            resourceload     = (1 << 1),    ///< Load resources.
            final            = (1 << 2),    ///< Initialize configuration variables and other
                                            ///< things that relies on fully initialized dependent
                                            ///< libraries. Prior to this phase, configuration
                                            ///< plug-ins can be added.


            END_OF_ITERATION = (1 << 4),    ///< End of iteration marker.

            NONE             = 0       ,    ///< None (not started).
            ALL              = 7       ,    ///< Static + ResourceLoad + Variables
            TERMINATED       = (1 << 10)    ///< TerminationCleanUp invoked.
        };
    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

        /// State of initialization, used to avoid double initialization.
        Phases                          initState                                   =  Phases::NONE;

        /// Dependent libraries. Will be initialized prior to us.
        std::vector<Library*>           dependencies;

        /**
         * Denotes whether fields #Res (bit 1) and #Config (bit 2) were locally created.
         * This is usually true if the library is explicitly initialized and false if the library
         * is initialized with the recursive initialization mechanics of libraries in #dependencies.
         */
        int                             isResourceAndConfigOwner                            = false;

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * The library version. Standard \alib versioning scheme is YYMM (as integer numer)
         * of the library release date.
         * Besides this version number, field #Revision indicates if this is a revised version
         * of a former release.
         */
        const int                       Version;

        /**
         * The revision number of this release. Usually a library is initially released as
         * revision \e 0. Pure bug-fix releases that do not change the interface of \alib
         * are holding the same #Version but an increased number in this field.
         */
        const int                       Revision;


        /**
         * Pointer to the resources instance.
         * An instance is created and assigned with the invocation of one of the #Init methods.
         * This instance is then passed to all dependent libraries (recursively) and this way
         * shared. Dependent libraries that are to an own dedicated instance, have to be initialized
         * explicitly prior to initializing this library.
         */
        Resources*                      Res;

        /**
         * The name of the configuration category of configuration variables used by this library.<br>
         * \alib itself uses "ALIB". \alib modules use the naming scheme
         * <c>ALIB_{MODULE_NAMESPACE}</c>.<br>
         */
         TString                        ResourceCategory;

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
        std::vector<std::pair <const char*, uint64_t>>  CompilationFlagMeanings;


        #if ALIB_MODULE_CONFIGURATION
            /**
             * Pointer to the configuration instance used to load variable configuration data.
             * An instance is created and assigned with the invocation of one of the #Init methods.
             * This instance is then passed to all dependent libraries (recursively) and this way
             * shared. Dependent libraries that are to an own dedicated instance, have to be
             * initialized explicitly prior to initializing this library.
             *
             * \note
             *   This field is available only with \alib module
             *   \ref aworx::lib::config "Configuration" included.
             */
            config::Configuration*      Config;

        #endif // ALIB_MODULE_CONFIG

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
                Library( int            version,
                         int            revision,
                         const String&  resourceCategory,
                         uint64_t       compilationFlags = 0 )
                : Version         (version         )
                , Revision        (revision        )
                , ResourceCategory(resourceCategory)
                , CompilationFlags(compilationFlags)
                {}

        /** ****************************************************************************************
         * Virtual destructor to satisfy virtual methods.
         ******************************************************************************************/
        virtual ~Library()
        {
        }




    // #############################################################################################
    // Init/TerminationCleanUp()
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Initializes this library up to the given \p{level}.
         * If the library was initialized for the given level already, nothing is done and
         * \c false is returned.
         *
         * Field #Res is set to point to a newly created \alib{lang,Resources} object.<br>
         * If level includes
         * \alib{lang,Library::InitLevels::PrepareConfig,InitLevels::PrepareConfig}, field #Config
         * is filled with a newly created object, which will be deleted in the destructor.
         *
         * Both objects are recursively distributed to dependent libraries.
         *
         * \see For details consult the \alib{lang,Library,class documentation"}
         *
         * @param level   The initialization level to perform.
         *                Defaults to <c>InitLevels::ALL</c>.
         * @return \c true if the library was not initialized before, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        virtual bool        Init( InitLevels level= InitLevels::All  );

        ALIB_API
        /** ****************************************************************************************
         * Variant of method #Init, accepting command line arguments of type <c>char*</c>.
         *
         * \note On the Windows platform, the Microsoft compiler provides the global variables
         *       <em>__argc</em> and <em>__argv</em> (respectively <em>__wargv</em> for wide
         *       character binaries. These variables a can be used if this method is invoked
         *       outside of the <em>main()</em> method.
         *
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         * @param argv    The command line parameters as \c wchar_t.
         * @param level   The initialization level to perform.
         *                Defaults to <c>InitLevels::ALL</c>.
         * @return \c true if the library was not initialized before, \c false otherwise.
         ******************************************************************************************/
        virtual bool        Init( int argc,  char **argv, InitLevels level= InitLevels::All );

        /** ****************************************************************************************
         * Variant of method #Init, accepting command line arguments of type \c wchar_t.
         *
         * \note On the Windows platform, the Microsoft compiler provides the global variables
         *       <em>__argc</em> and <em>__argv</em> (respectively <em>__wargv</em> for wide
         *       character binaries. These variables a can be used if this method is invoked
         *       outside of the <em>main()</em> method.
         *
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         * @param argv    The command line parameters as \c wchar_t.
         * @param level   The initialization level to perform.
         *                Defaults to <c>InitLevels::ALL</c>.
         * @return \c true if the library was not initialized before, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        virtual bool        Init( int argc, wchar_t **argv, InitLevels level= InitLevels::All );

        /** ****************************************************************************************
         * Cleans up memory on termination. This method is useful if using memory analysis tools
         * (such as Valgrind) to remove any internal allocations before a program terminates.
         ******************************************************************************************/
        ALIB_API
        virtual void        TerminationCleanUp();

        /** ****************************************************************************************
         * Returns state of initialization. Mainly used internally to, to deduplicate init calls,
         *  avoid deadlocks, etc..
         *  @return \c true if was initialized already.
         ******************************************************************************************/
        inline
        bool                IsInitialized()
        {
            return initState == Phases::ALL ;
        }

    // #############################################################################################
    // Other public interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Shortcut method that invokes \alib{lang,Resources::Add} on field
         * #Res providing field #ResourceCategory as parameter.
         *
         * @param name  The resource name.
         * @param data  The resource data string.
         ******************************************************************************************/
        inline
        void                AddResource( const String& name, const String& data )
        {
            Res->Add( ResourceCategory, name, data   ALIB_DBG( , true ) );
        }

        #if defined(DOX_PARSER)
        /** ****************************************************************************************
         * Shortcut method that invokes \alib{lang,Resources::Add} on field
         * #Res providing field #ResourceCategory as parameter.
         *
         * Same as #AddResource, apart from the fact that in debug compilations:
         * - no assertion is raised if the resource was already existing.
         * - an optional expected current string can be passed to verify that the resource replaced
         *   is what is expected to be replaced.
         *
         * This optional third parameter has to be provivided using macro \ref ALIB_DBG
         * (or similar).
         *
         * @param name  The resource name.
         * @param data  The resource data string.
         * @param expectedResource The string that is expected to be replaced.
         ******************************************************************************************/
        inline
        void                ReplaceResource( const String& name, const String& data,
                                             const String& expectedResource= NullString );
        #else
        inline
        void                ReplaceResource( const String& name, const String& data
                                   ALIB_DBG(,const String& expectedResource= NullString ) )
        {
            ALIB_ASSERT_ERROR( expectedResource.IsNull() || expectedResource.Equals( Get(name) ),
                               ASTR( "Failure when replacing resource: Expected original value not found. "
                                     "In resource: " ),
                               ResourceCategory.ToCString(), ASTR(" / "), String128(name).ToCString() );

            Res->Add( ResourceCategory, name, data  ALIB_DBG(, false)  );
        }
        #endif

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{lang,Resources::Get} on field
         * #Res providing field #ResourceCategory as parameter.
         *
         * \note
         *   Usually, it is recommended to use #Get, which asserts in debug compilations
         *   if a resource was not found.
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        inline
        String              GetTry( const String& name )
        {
            return Res->Get( ResourceCategory, name  ALIB_DBG(, false)  );
        }

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{lang,Resources::Get} on field
         * #Res providing field #ResourceCategory as parameter.
         *
         * In debug compilations, this method asserts that a resource was found. If this is not
         * wanted, use #GetTry.
         *
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        inline
        String              Get( const String& name )
        {
            return Res->Get( ResourceCategory, name   ALIB_DBG(, true)  );
        }

        /** ****************************************************************************************
         * Compares a given set of compilation flags with the internal set. This is an optional
         * feature of libraries which expose compiler definitions that lead to incompatible
         * library versions if mixed.
         *
         * If used, the following steps have to be performed:
         * - By the library that offers the feature:
         *   - Expose a preprocessor macro that collects the bits at compile time.
         *     As a sample see macros
         *     \ref ALIB_COMPILATION_FLAGS or
         *     \ref ALOX_COMPILATION_FLAGS.
         *   - The constructor of the library needs to be in a compilation unit of the library
         *     (not inline) and needs to pass the macro exposed as a parameter to this class'
         *     constructor.
         *   - The constructor of the library needs to fill field #CompilationFlagMeanings.
         *
         * - The user of the library now invokes this method in bootstrap, hence preferably at
         *   the beginning of method #init with \alib{lang::Library,Phases::resourceset}.
         *
         * In case they are different in a way that the library gets incompatible (e.g. different
         * class sizes, which results in errors which are confusing the debugger), the flags are
         * written to \e cout for comparison and \c false is returned.
         *
         * @param compilationFlags The flags as defined in invoking compilation unit.
         *                         Defaults to \c ALIB_COMPILATION_FLAGS.
         *
         * @return \c true if compatible, \c false else.
         ******************************************************************************************/
        ALIB_API
        bool    VerifyCompilationFlags( uint64_t compilationFlags );


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Abstract method invoked by #Init.
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                                = 0;

        /** ****************************************************************************************
         * Abstract method invoked by #terminationCleanUp.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                                = 0;

        /** ****************************************************************************************
         * The main initialization method.
         * Called internally by #Init methods, calling abstract #init.
         *
         * @param phases  The initialization phases to perform.
         * @param argc    The number of command line arguments. Defaults to \c 0.
         * @param argv    List of command line arguments as single byte character strings.
         * @param wargv   List of command line arguments as multi-byte character strings.
         * @return \c true if the library was not initialized before, \c false otherwise.
         ******************************************************************************************/
        virtual bool        doInit( Phases phases, int argc, char** argv, wchar_t**  wargv );

        /** ****************************************************************************************
         * Recursive part of #doInit.
         *
         * @param phase  The initialization phases to perform.
         ******************************************************************************************/
        virtual void        doInitRecursion( Phases   phase );

};// class Library


/** ************************************************************************************************
 * Simple TMP struct that provides information to a access resource data for a type of
 * a library.
 *
 * This struct is for example used with \alib{lang,T_EnumMetaDataDecl,ALib enum meta data }.
 *
 * @tparam TResourced    The type to define resource information for.
 **************************************************************************************************/
template<typename TResourced>
struct T_Resourced          :  public std::false_type
{
    ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
    /**
     *  Receives the library object associated with type \p{TResourced}.
     *  @return The library of \p{TResourced}.
     */
    inline static            Library&      Lib() {}

    /**
     * Receives the resource name associated with type \p{TResourced}.
     * @return The resource name of \p{TResourced}.
     */
    inline static constexpr  String        Name()       { return NullString; }
    ALIB_WARNINGS_RESTORE
};

// #################################################################################################
// Macros (for technical reasons, doxed in file alib.cpp)
// #################################################################################################
#define  ALIB_LANG_RESOURCED( TResourced, ResourceLibrary, NameString )                            \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_Resourced<TResourced>  :  public std::true_type                                \
{                                                                                                  \
    inline static            Library&      Lib()          { return ResourceLibrary; }              \
    inline static constexpr  String        Name()         { return NameString; }                   \
};}}}                                                                                              \



}} // namespace aworx[::lib::lang]

/// Type alias in namespace #aworx.
using Library=    aworx::lib::lang::Library;


} // namespace [aworx]

#endif // HPP_ALIB_LIB_LIBRARY

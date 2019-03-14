// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_LIB_MODULE
#define HPP_ALIB_LIB_MODULE 1

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif

ALIB_ASSERT_FILESET(MODULES)

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

namespace aworx  {
namespace lib {

namespace resources { class Resources;     }
namespace config    { class Configuration; }



/** ************************************************************************************************
 * This class is used by \alib to
 *
 * - manage library bootstrap and termination,
 * - provide resources (for example externally managed string resources) and to
 * - provide Configuration data (e.g. configuration files, command line parameters, etc.).
 *
 * Typically, every "full" \alibmod disposes about a type derived from this class, implemented
 * as a \alib{singletons,Singleton,strict singleton}.  This can be nicely seen with the
 * inheritance diagram shown above.
 *
 * Types found in an \alibmod_nolink, will use pointer fields #Config and #Resources of that
 * singleton to access resources and to read and write configuration data.
 * With that, it has to be kept in mind that these objects most probably are shared with other
 * modules. As a consequence, clear and understandable resource and variable categories and names
 * are to be used, to avoid conflicting entries.
 *
 * The \ref alib_manual provides several chapters that explain how
 * \ref alib_manual_bootstrapping "3. Bootstrapping ALib" is performed with the help of this class.
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
        /**
         * Initialization levels usable with #Init methods.
         */
        enum class InitLevels
        {
            PrepareResources = 1,    ///< Creates field #Resources and loads its module specific default
                                     ///< values.
            PrepareConfig    = 2,    ///< Initializes up to the creation of a field
                                     ///< \aworx{lib,Module::Config,Config}.
            Final            = 3,    ///< The final initialization phase.
        };

    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

        /** State of initialization, used to avoid double initialization. */
        int                             initState                                               = 0;

        /** State of initialization of the "micro modules".  */
        static bool                     microModulesInitialized;

        /**
         * Denotes whether fields #Resources (bit 1) and #Config (bit 2) were locally created.
         * This is usually true if the module is explicitly initialized and false if the module
         * is initialized with the recursive initialization mechanics of libraries in #Dependencies.
         */
        int                             isResourceAndConfigOwner                            = false;

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

        /** Dependent libraries. Will be initialized prior to us. */
        std::vector<Module*>            Dependencies;


        /**
         * Pointer to the resources instance.
         * An instance is created and assigned with the invocation of one of the #Init methods.
         * This instance is then passed to all dependent libraries (recursively) and this way
         * shared. Dependent libraries that are to an own dedicated instance, have to be initialized
         * explicitly prior to initializing this module.
         */
        resources::Resources*           Resources                                         = nullptr;

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


        #if ALIB_MODULE_CONFIGURATION
            /**
             * Pointer to the configuration instance used to load variable configuration data.
             * An instance is created and assigned with the invocation of one of the #Init methods.
             * This instance is then passed to all dependent libraries (recursively) and this way
             * shared. Dependent libraries that are to an own dedicated instance, have to be
             * initialized explicitly prior to initializing this module.
             *
             * \note
             *   This field is available only with \alib module
             *   \ref aworx::lib::config "Configuration" included.
             */
            config::Configuration*      Config                                            = nullptr;

        #endif // ALIB_MODULE_CONFIGURATION

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

        /** ****************************************************************************************
         * Defaulted copy constructor.
         ******************************************************************************************/
        Module( const Module& ) = default;

        /** ****************************************************************************************
         * Virtual destructor to satisfy virtual methods.
         ******************************************************************************************/
        virtual ~Module()
        {}
    // #############################################################################################
    // Init/TerminationCleanUp()
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Initializes this module up to the given \p{level}.
         * If the module was initialized for the given level already, nothing is done and
         * \c false is returned.
         *
         * Field #Resources is set to point to a newly created \alib{resources,Resources} object.<br>
         * If level includes
         * \aworx{lib,Module::InitLevels::PrepareConfig,InitLevels::PrepareConfig}, field #Config
         * is filled with a newly created object, which will be deleted in the destructor.
         *
         * Both objects are recursively distributed to dependent libraries.
         *
         * Parameters \p{argc}, \p{argvN} and \p{argvW} may be used to provide command line
         * arguments. They will be passed to abstract method #init of all dependent libraries.
         * Special singleton \alib{ALibModules} will store the parameters for convenience in
         * publicly accessible fields. Note that only either of parameters \p{argvN} and \p{argvW}
         * is to be provided (the other should be \e nulled), depending on system/compilation
         * preferences and on the character size that the compilation toolset provides with method
         * \c main().
         *
         * \see
         *   For details on when and how to invoke this method, see the detailed explanations in
         *   chapter \ref alib_manual_bootstrapping "3. Bootstrapping ALib" of the \ref alib_manual.
         *
         * @param level   The initialization level to perform.
         *                Defaults to <c>InitLevels::Final</c>.
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         *                Defaults to <c>0</c>.
         * @param argvN   The command line parameters given as a zero-terminated list to
         *                zero-terminated narrow strings.
         *                Defaults to <c>nullptr</c>.
         * @param argvW   The command line parameters given as a zero-terminated list to
         *                zero-terminated wide strings.
         *                Defaults to <c>nullptr</c>.
         * @return \c true if the module was not initialized before, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool                Init( InitLevels      level   = InitLevels::Final,
                                  int             argc    = 0,
                                  const char**    argvN   = nullptr,
                                  const wchar_t** argvW   = nullptr            );

        /** ****************************************************************************************
         * Inline convenience shortcut to method
         * \alib{Module,Init(InitLevels\,int\,const char**\,const wchar_t**)} accepting
         * either \b narrow or \b wide character command line arguments using one templated
         * method parameter.
         *
         * @tparam TChar  The character type of parameter \p{argv}.
         * @param argc    The number of arguments.
         * @param argv    The command line arguments as \b nchar or \b wchar, constant or not.
         * @param level   The initialization level to perform.
         *                Defaults to <c>InitLevels::ALL</c>.
         * @return \c true if the module was not initialized before, \c false otherwise.
         ******************************************************************************************/
        template<typename TChar>
        inline
#if ALIB_DOCUMENTATION_PARSER
        bool
#else
        typename std::enable_if<   std::is_same<typename std::remove_const<TChar>::type, char   >::value
                                || std::is_same<typename std::remove_const<TChar>::type, wchar_t>::value,
        bool                   >::type
#endif
        Init( int argc, TChar** argv, InitLevels level= InitLevels::Final )
        {
            if( std::is_same<typename std::remove_const<TChar>::type, char>::value )
                return Init( level, argc,
                             reinterpret_cast<const char**>(
                                   const_cast<const TChar**>( argv  ) ),
                             nullptr );
            else
                return Init( level, argc, nullptr,
                             reinterpret_cast<const wchar_t**>(
                                   const_cast<const TChar**>( argv  ) )  );
        }

        /** ****************************************************************************************
         * Frees resources allocated by this module and recursively with all modules found
         * in #Dependencies.
         *
         * With built-in \alibmods_nolink, no urgent actions are performed apart from memory
         * de-allocation, which is anyhow done by modern operating systems with the termination of
         * a process. However, still this method should be invoked in the case of future changes.
         * Also, if using memory analysis tools (such as Valgrind) it is important to remove any
         * internal allocations before a program terminates.
         ******************************************************************************************/
        ALIB_API
        void                TerminationCleanUp();


        /** ****************************************************************************************
         * Tests if the module was completely initialized.
         *  @return \c true if was initialized already, \c false otherwise.
         ******************************************************************************************/
        inline
        bool                IsInitialized()
        {
            return initState == 3 ;
        }

    // #############################################################################################
    // Other public interface methods
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,Resources::Add} on field
         * #Resources providing field #ResourceCategory as parameter.
         *
         * @param name  The resource name.
         * @param data  The resource data string.
         ******************************************************************************************/
        ALIB_API
        void                AddResource( const NString& name, const String& data );

        #if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,Resources::Add} on field
         * #Resources providing field #ResourceCategory as parameter.
         *
         * Same as #AddResource, apart from the fact that in debug compilations:
         * - no assertion is raised if the resource was already existing.
         * - an optional expected current string can be passed to verify that the resource replaced
         *   is what is expected to be replaced.
         *
         * This optional third parameter has to be provivided using macro \ref ALIB_DBG
         * (or similar), as it is not available in release compilations.
         *
         * @param name  The resource name.
         * @param data  The resource data string.
         * @param expectedResource The string that is expected to be replaced.
         ******************************************************************************************/
        ALIB_API
        void                ReplaceResource( const NString& name, const String& data,
                                             const  String& expectedResource= NullString() );
        #else
        ALIB_API
        void                ReplaceResource( const NString& name, const String& data
                                   ALIB_DBG(,const  String& expectedResource= NullString() ) );
        #endif

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,Resources::Get} on field
         * #Resources providing field #ResourceCategory as parameter.
         *
         * \note
         *   Usually, it is recommended to use #Get, which asserts in debug compilations
         *   if a resource was not found.
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        ALIB_API
        String              TryResource( const NString& name );

        /** ****************************************************************************************
         * Shortcut method that invokes \alib{resources,Resources::Get} on field
         * #Resources providing field #ResourceCategory as parameter.
         *
         * In debug compilations, this method asserts that a resource was found. If this is not
         * wanted, use #TryResource.
         *
         * @param name  The resource name.
         * @return The resource string, respectively a \e nulled string on failure.
         ******************************************************************************************/
        ALIB_API
        String              GetResource( const NString& name );

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
         *   the beginning of method #init with \alib{Module,InitLevels::PrepareResources}.
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
        bool    VerifyCompilationFlags( uint64_t compilationFlags );


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Abstract method which is recursively invoked by method #Init on all modules found in
         * #Dependencies.
         *
         * Note that parameters \p{argc}, \p{argv} and \p{wargv} are preferably provided with
         * the first initialization level, \alib{Module,InitLevels::PrepareResources}.
         * In other words, it is not considered a fault, if those are not provided in later
         * levels. Also, depending on the usage of the library, the parameters might even be not
         * provided at all.
         *
         * @param level  The initialization level to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t**  wargv )         = 0;

        /** ****************************************************************************************
         * Abstract method invoked by #terminationCleanUp.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                                = 0;

        /** ****************************************************************************************
         * Recursive part of #Init.
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        doInitRecursion( InitLevels level,
                                             int argc, const char** argv, const wchar_t** wargv );

};// class Module

} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using InitLevels=    aworx::lib::Module::InitLevels;

} // namespace [aworx]


// #################################################################################################
// T_Resourced Macro
// #################################################################################################
#define  ALIB_RESOURCED_IN_MODULE( TResourced, TModule, ResourceName )                             \
namespace aworx { namespace lib { namespace resources {                                            \
template<> struct T_Resourced<TResourced>                                                          \
{                                                                                                  \
    inline static            Resources* Resource() { return  TModule.Resources;        }           \
    inline static            NString    Category() { return  TModule.ResourceCategory; }           \
    inline static constexpr  NString    Name()     { return  ResourceName;             }           \
};}}}



#endif // HPP_ALIB_LIB_MODULE

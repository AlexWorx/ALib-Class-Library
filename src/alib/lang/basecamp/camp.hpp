/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_CAMP
#define HPP_ALIB_LANG_CAMP 1

#if !defined(HPP_ALIB_STRINGS_CSTRING)
#   include "alib/strings/cstring.hpp"
#endif
ALIB_ASSERT_MODULE(CAMP)


// forwards
namespace alib::lang::resources { class ResourcePool;  }
namespace alib::config          { class Configuration; }
namespace alib::lang            { class Camp; }

namespace alib {

/** Initialization levels usable with \ref alib_manual_bootstrapping_camps "Bootstrapping ALib Camps". */
enum class BootstrapPhases
{
    PrepareResources = 1,    ///< Creates field \alib{lang,Camp::resourcePool} and loads its
                             ///< camp-specific default values.
    PrepareConfig    = 2,    ///< Initializes up to the creation of a field \alib{lang::Camp,config}.
    Final            = 3,    ///< The final initialization phase.
};

/** Termination levels usable with \ref alib_manual_bootstrapping_camps "Bootstrapping ALib Camps". */
enum class ShutdownPhases
{
    Announce         = 1,    ///< Signals termination. Keeps resources, config, etc.
                             ///< intact. Usable to write configuration data, stop
                             ///< background threads, etc.
    Destruct         = 2,    ///< The main phase of termination that destructs everything.
};

// forwards
ALIB_API void Bootstrap(BootstrapPhases, lang::Camp*, int, int, TCompilationFlags);
ALIB_API void Shutdown(  ShutdownPhases , lang::Camp*);


namespace lang {

/** ************************************************************************************************
 * This class is used by \alib to
 *
 * - manage library bootstrap and termination,
 * - provide \ref alib_basecamp_resources "resources" and to
 * - provide Configuration data (e.g. configuration files, command line parameters, etc.).
 *
 * Typically, every "bigger" and "higher level" \alibmod disposes about a type derived from this
 * class. The singleton instances of these types are collected in namespace \ref alib and are named
 * in capital letters, for example \ref alib::ALOX, \ref alib::EXPRESSIONS, etc.
 *
 * Types found in an \alibmod_nl, will use methods \alib{lang::Camp,GetResourcePool} and
 * \alib{lang::Camp,GetConfig} of that singleton to access resources and to read and write
 * configuration data.
 * With that, it has to be kept in mind that these objects most probably are shared with other
 * camps. As a consequence, clear and understandable resource and variable categories and names
 * are to be used, to avoid conflicting entries.
 *
 * The \ref alib_manual provides several chapters that explain how
 * \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" is performed with the
 * help of this class.<br>
 * Furthermore, source code sample is given with the
 * \ref alib_cli_tut "tutorial of ALib Module CLI".
 *
 * Often, types that comprise a "camp singleton" (as the derived types shown in the inheritance
 * diagram above) incorporate some further functionality specific to the field of application that
 * a "module" respectively "camp" covers.
 *
 * # Reference Documentation #
 **************************************************************************************************/
class Camp
{
  protected:
    #if !defined(ALIB_DOX)
    friend void alib::Bootstrap( BootstrapPhases, Camp*, int, int, TCompilationFlags );
    friend void alib::Shutdown(  ShutdownPhases , Camp*);
    #endif

    /** State of initialization, used to avoid double initialization as well as initialization
     * "gaps". */
    int                              bootstrapState                                             = 0;

    /**
     * Pointer to the resource pool.
     * An instance is created and assigned with the invocation of one of the #Bootstrap methods.
     * This instance is then passed to all dependent camps (recursively) and this way
     * shared. Dependent libraries that are to an own dedicated instance, have to be initialized
     * explicitly prior to initializing this \alibcamp.
     */
    lang::resources::ResourcePool*   resourcePool                                         = nullptr;

    /**
     * If a default instance of type \alib{lang::resources,LocalResourcePool} has been created
     * when this camp was initialized, this flag evaluates to \c true and the instance stored
     * in #resourcePool will be deleted with destruction of this object.
     */
    bool                             isResourceOwner                                        = false;


    #if ALIB_CONFIGURATION
        /**
         * Pointer to the configuration instance used to load variable configuration data.
         * An instance is created and assigned with the invocation of one of the #Bootstrap methods.
         * This instance is then passed to all dependent libraries (recursively) and this way
         * shared. Dependent libraries that are to an own dedicated instance, have to be
         * initialized explicitly prior to initializing this \alibcamp.
         *
         * Access to the field is provided with method #GetConfig.
         *
         * \note
         *   This field is available only when \alib_config is included in the \alibdist.
         */
        config::Configuration*       config                                           = nullptr;

        /**
         * If a configuration instance has been created when this \alibcamp was initialized, this
         * flag evaluates to \c true and the instance stored in #config will be deleted with
         * destruction of this object.
         * \note
         *   This field is available only when \alib_config is included in the \alibdist.
         */
        bool                         isConfigOwner                                      = false;
    #endif // ALIB_CONFIGURATION

   // ##############################################################################################
   // Public fields
   // ##############################################################################################
   public:
    /**
     * The name of the resource category of externalized string resources defined and used by
     * this \alibcamp.<br>
     * Special camp \alib_basecamp uses "ALIB". Other \alibcamps_nl use a short unique name.
     */
     NCString                        ResourceCategory;

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    /** ********************************************************************************************
     * Constructor.
     * @param resourceCategory  Value for field #ResourceCategory.
     **********************************************************************************************/
    Camp( const NCString& resourceCategory )
    : ResourceCategory(resourceCategory)
    {}

    /**  Deleted copy constructor.  */
    Camp( const Camp& )                                                                = delete;

    /**  Deleted move constructor.  */
    Camp( Camp&& )                                                                     = delete;

    /**  Deleted copy assignment.  */
    void operator=( const Camp& )                                                      = delete;

    /**  Deleted move assignment.  */
    void operator=(  Camp&& )                                                          = delete;


    /** ********************************************************************************************
     * Virtual destructor to satisfy C++ abstract type rules.
     **********************************************************************************************/
    virtual ~Camp()
    {
        ALIB_ASSERT_WARNING( bootstrapState == -int(ShutdownPhases::Destruct),
                             "CAMPS",
                             "Destructing a non-terminated camp. "
                             "Camp Resource category: ", ResourceCategory  )
    }

  // ###############################################################################################
  // Bootstrap/Shutdown()
  // ###############################################################################################
  public:

    /** ********************************************************************************************
     * Tests if this \alibcamp was completely initialized.
     *  @return
     *    \c true if the initialization state is either \alib{BootstrapPhases::Final}
     *    or \alib{ShutdownPhases::Announce}, \c false otherwise.
     **********************************************************************************************/
    bool            IsBootstrapped()
    {
        return     bootstrapState ==  int(BootstrapPhases::Final)
                || bootstrapState == -int(ShutdownPhases::Announce);
    }

    // #############################################################################################
    // Other public interface methods
    // #############################################################################################
    #if ALIB_CONFIGURATION
        /** ****************************************************************************************
         * Sets the configuration object of this \alibcamp.
         *
         * \attention
         *   This method must be invoked before function
         *   \ref alib::Bootstrap( BootstrapPhases, lang::Camp*, int, int, TCompilationFlags ) "Bootstrap"
         *   is invoked with parameter \p{targetPhase} set equal or higher than
         *   \alib{BootstrapPhases::PrepareConfig},
         *   because in this phase, internally a configuration is created if none is set.
         *
         * \attention
         *   Function \b Bootstrap will distribute the given instance to each lower level
         *   \alibcamp that has not received a different object.
         *
         * \see
         *   Chapter \ref alib_manual_bootstrapping of the Programmer's Manual of \alib.
         *
         * @param pConfig The external configuration object to use.
         ******************************************************************************************/
        void  BootstrapSetConfig(config::Configuration* pConfig)
        {
            ALIB_ASSERT_WARNING( config == nullptr, "CAMPS",
                           "This camp already has a configuration object set.\n"
                           "A custom configuration must be set prior to camp initialization.")
            config= pConfig;
        }

        /** ****************************************************************************************
         * Returns a reference to the configuration object.
         * @return The configuration object set with bootstrapping.
         ******************************************************************************************/
        config::Configuration&  GetConfig()
        {
            return *config;
        }
    #endif

    /** ********************************************************************************************
     * Sets the resource pool of this camp.
     *
     * \attention
     *   This method must be invoked before method #Bootstrap,
     *   because in the first phase \alib{BootstrapPhases::PrepareConfig}, internally a
     *   pool of type \alib{lang::resources,LocalResourcePool} is created if none is set.
     *
     * \attention
     *   Method #Bootstrap will distribute the given instance to each lower level
     *   camp that has not received a different object.
     *
     * \see
     *   Chapter \ref alib_manual_bootstrapping of the Programmer's Manual of \alib.
     *
     * @param pool The external resource pool to use.
     **********************************************************************************************/
    void            BootstrapSetResourcePool(lang::resources::ResourcePool* pool)
    {
        ALIB_ASSERT_WARNING( bootstrapState == 0, "CAMPS",
                           "This camp already has a resource pool object set.\n"
                           "A custom resource pool must be set prior to camp initialization.")
        resourcePool= pool;
    }

    /** ********************************************************************************************
     * Returns a reference to the resource pool.
     * @return The resource pool set with bootstrapping.
     **********************************************************************************************/
    resources::ResourcePool&   GetResourcePool()
    {
        return *resourcePool;
    }

    /** ********************************************************************************************
     * Shortcut method that invokes \alib{lang::resources,ResourcePool::Bootstrap} on field
     * #resourcePool providing field #ResourceCategory as parameter.
     * \note This method is defined only after inclusion of header file
     *       \alibheader{lang/basecamp/camp_inlines.hpp}.
     * @param name  The resource name.
     * @param data  The resource data string.
     **********************************************************************************************/
    inline
    void            BootstrapResource( const NString& name, const String& data );

    /** ********************************************************************************************
     * Shortcut method that invokes \alib{lang::resources,ResourcePool::Get} on field
     * #resourcePool providing field #ResourceCategory as parameter.
     *
     * With debug builds, this method asserts that a resource was found. If this is not
     * wanted, use #TryResource.
     *
     * \note This method is defined only after inclusion of header file
     *       \alibheader{lang/basecamp/camp_inlines.hpp}.
     * @param name  The resource name.
     * @return The resource string, respectively a \e nulled string on failure.
     **********************************************************************************************/
    inline
    const String&   GetResource( const NString& name );

    /** ********************************************************************************************
     * Shortcut method that invokes \alib{lang::resources,ResourcePool::Get} on field
     * #resourcePool providing field #ResourceCategory as parameter.
     *
     * \note
     *   Usually, it is recommended to use #GetResource, which asserts with debug builds
     *   if a resource was not found.
     *
     * \note This method is defined only after inclusion of header file
     *       \alibheader{lang/basecamp/camp_inlines.hpp}.
     * @param name  The resource name.
     * @return The resource string, respectively a \e nulled string on failure.
     **********************************************************************************************/
    inline
    const String&   TryResource( const NString& name );


  // ###############################################################################################
  // protected abstract methods
  // ###############################################################################################
  protected:
    /** ********************************************************************************************
     * Abstract method which is invoked during bootstrapping by function<br>
     * \ref alib::Bootstrap(BootstrapPhases targetPhase, lang::Camp* targetCamp,int,int,TCompilationFlags)
     * for each \p{phase}, and each camp found in list \alib{Camps}.
     *
     * For details on bootstrapping \alibcamps, see the detailed explanations in
     * chapter \ref alib_manual_bootstrapping "4.Bootstrapping And Shutting Down ALib" of
     * the \ref alib_manual.
     *
     * @param phase  The initialization phase to perform.
     **********************************************************************************************/
    virtual void    bootstrap( BootstrapPhases phase )                                      = 0;

    /** ********************************************************************************************
     * Abstract method which is invoked during bootstrapping by function<br>
     * \ref alib::Shutdown(ShutdownPhases targetPhase, lang::Camp* targetCamp)
     * for each \p{phase}, and each camp found in list \alib{Camps}.
     *
     * For details on bootstrapping \alibcamps, see the detailed explanations in
     * chapter \ref alib_manual_bootstrapping "4. Bootstrapping And Shutting Down ALib" of
     * the \ref alib_manual.
     * @param phase  The termination level to perform.
     **********************************************************************************************/
    virtual void    shutdown( ShutdownPhases   phase )                                      = 0;

};// class Camp

} // namespace alib[::lang]

} // namespace [alib]

#endif // HPP_ALIB_LANG_CAMP

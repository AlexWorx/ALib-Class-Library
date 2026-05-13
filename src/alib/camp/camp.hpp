//==================================================================================================
/// \file
/// This header-file is part of module \alib_camp of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {

//==================================================================================================
/// Initialization levels usable with
/// #"alib_mod_bs_camps;Bootstrapping ALib Camps".
//==================================================================================================
enum class BootstrapPhases {
    /// The inherited field #"^Camp::resourcePool" is created when this phase is called for a camp.
    /// Camps are requested to feed in their resources now. When this is done, all entities that
    /// rely exclusively on resources are to be initialized. For example, all resourced enumerations
    /// of a camp should be parsed in this phase.
    PrepareResources = 1,

    /// Field #"Camp::config;*" is created when this phase is called for a camp.
    /// Camps are requested to
    /// #"Configuration::RegisterType;register application-specific variable types" in
    /// this phase. Furthermore modifications on the configuration object itself might be performed,
    /// for example, #"ConfigurationPlugin;custom plugins" might be added to a
    /// configuration or configurable options of default plug-ins can be set.<br>
    /// Finally, this is the right phase to
    /// #"Configuration::PreloadVariables;declare variables" which have a resourced
    /// variable declaration, or likewise perform 'hard-coded' variable declarations including
    /// their default values.
    PrepareConfig    = 2,

    /// The final initialization phase. Here, camps are initializing their custom setup.
    Final            = 3,
};

//==================================================================================================
/// Termination levels usable with #"alib_mod_bs_camps;Bootstrapping ALib Camps".
//==================================================================================================
enum class ShutdownPhases {
    Announce         = 1,    ///< Signals termination. Keeps resources, config, etc.
                             ///< intact. Usable to write configuration data, stop
                             ///< background threads, etc.
    Destruct         = 2,    ///< The main phase of termination that destructs everything.
};

namespace camp {

//==================================================================================================
/// This class is used by \alib to
///
/// - manage library bootstrap and termination,
/// - provide #"alib_mod_resources;resources" and to
/// - provide Configuration data (e.g., configuration files, command-line parameters, etc.).
///
/// Typically, every "bigger" and "higher level" \alibmod disposes of a type derived from this
/// class. The singleton instances of these types are collected in namespace #"alib" and are named
/// in capital letters, for example, #"alib::ALOX", #"alib::EXPRESSIONS", etc.
///
/// Types found in an \alibmod_nl, will use the (inherited) method #"^Camp::GetResourcePool" and
/// the method #"Camp::GetConfig" of that singleton to access resources and to read and write
/// configuration data.
/// With that, it has to be kept in mind that these objects most probably are shared with other
/// camps. As a consequence, clear and understandable resource and variable categories and names
/// are to be used to avoid conflicting entries.
///
/// The #"alib_mod_bs;Programmer's Manual" of the module \alib_bootstrap_nl explains how
/// bootstrapping and shutting down \alib is performed with the help of this class.<br>
/// Furthermore, a source code sample is given with the
/// #"alib_app_cli_tut;tutorial of ALib Module App".
///
/// Often, types that comprise a "camp singleton" (as the derived types shown in the inheritance
/// diagram above) incorporate some further functionality specific to the field of application that
/// a "module" respectively "camp" covers.
///
/// To manage resources, this types derives from #"ResourceHolder".
/// A fixed category name which is used with all resources by this camp, is expected in the
/// constructor. The special camp #"BASECAMP;2" uses <c>"ALIB"</c>.
/// Other \alibcamps_nl use a short unique name.
///
/// # Reference Documentation #
//==================================================================================================
class Camp : public resources::ResourceHolder {
  protected:
    /// State of initialization, used to avoid double initialization as well as initialization
    /// "gaps".
    int                 bootstrapState                                                          = 0;

    /// Pointer to the configuration instance used to load variable configuration data.
    /// An instance is created and assigned with the invocation of one of the #"Bootstrap" methods.
    /// This instance is then passed to all dependent libraries (recursively) and this way
    /// shared. Dependent libraries that are to an own dedicated instance, have to be
    /// initialized explicitly before initializing this \alibcamp.
    ///
    /// Access to the field is provided with method #"GetConfig".
    SharedConfiguration config;


  //################################################################################################
  // Constructor/Destructor
  //################################################################################################
    /// Constructor.
    /// @param resourceCategory  Value for the inherited field #"ResourceHolder::ResourceCategory".
    Camp( const NCString& resourceCategory )
    : ResourceHolder(resourceCategory)                                                            {}

    /// Deleted copy constructor.
    Camp( const Camp& )                                                                     =delete;

    /// Deleted move constructor.
    Camp( Camp&& )                                                                          =delete;

    /// Deleted copy assignment.
    void operator    =( const Camp& )                                                       =delete;

    /// Deleted move assignment.
    void operator    =(  Camp&& )                                                           =delete;


    /// Virtual destructor to satisfy C++ abstract type rules.
    virtual ~Camp() {
        ALIB_ASSERT_WARNING(    bootstrapState == -int(ShutdownPhases::Destruct)
                             || bootstrapState == 0, "CAMPS",
          "Destructing a non-terminated camp. Camp Resource category is: \"{}\"", ResourceCategory )
    }

  //################################################################################################
  // Bootstrap/Shutdown()
  //################################################################################################
  public:

    /// Abstract method which is invoked during bootstrapping by function<br>
    /// #"Bootstrap(BootstrapPhases targetPhase, camp::Camp* targetCamp, int,int,TCompilationFlags)"
    /// for each \p{phase}, and each camp found in list #"CAMPS;2".
    /// Implementations of this function receive the phase to perform with #"GetBootstrapState".
    ///
    /// This method is not to be called from outside, but is internally invoked by function
    /// #"alib::Bootstrap(BootstrapPhases);2".
    ///
    /// For details on bootstrapping\alibcamps, see the detailed explanations in the
    /// #"alib_mod_bs;Programmer's Manual" of module \alib_bootstrap_nl.
    virtual void    Bootstrap()                                                                  =0;

    /// Abstract method which is invoked during bootstrapping by function<br>
    /// #"Shutdown(ShutdownPhases targetPhase, camp::Camp* targetCamp);2"
    /// for each \p{phase}, and each camp found in list #"CAMPS;2".
    ///
    /// This method is not to be called from outside, but is internally invoked by the function
    /// #"alib::Bootstrap(BootstrapPhases);2".
    ///
    /// For details on bootstrapping \alibcamps, see the detailed explanations in the
    /// #"alib_mod_bs;Programmer's Manual" of module \alib_bootstrap_nl.
    /// @param phase  The termination level to perform.
    virtual void    Shutdown( ShutdownPhases   phase )                                           =0;

    /// Returns the bootstrap state of this camp.
    /// @return The internal state flag.
    BootstrapPhases GetBootstrapState()                  { return BootstrapPhases(bootstrapState); }

    /// Sets the (next) bootstrap phase to perform.
    ///  @param phase The upcoming phase.
    void            BootstrapSetPhase(BootstrapPhases phase)         { bootstrapState= int(phase); }

    /// Tests if this \alibcamp was completely initialized.
    /// @return \c true if the initialization state is either #"BootstrapPhases::Final;2"
    ///         or #"ShutdownPhases::Announce;2", \c false otherwise.
    bool            IsBootstrapped()
    {
        return     bootstrapState ==  int(BootstrapPhases::Final)
                || bootstrapState == -int(ShutdownPhases::Announce);
    }

  //################################################################################################
  // Other public interface methods
  //################################################################################################
    /// Sets the configuration instance of this camp.
    ///
    /// \attention
    ///   Note the documentation of the sibling method #"BootstrapSetResourcePool".
    ///   Here, the very same rules apply in respect to the distribution of configuration instances
    ///   between different camps and in respect to the customization of bootstrapping.
    ///
    /// @see Chapter #"alib_mod_bs_customize" of the Programmer's Manual of module
    ///      \alib_bootstrap.
    /// @param pConfig   The configuration to use.
    void            BootstrapSetConfig( const SharedConfiguration& pConfig ) {
        ALIB_ASSERT_WARNING( (config == nullptr) != (pConfig == nullptr), "CAMPS",
          "Double setting or removing a camp's configuration instance.\n"
          "Note: A custom configuration must be set before camp initialization." )
        config= pConfig;
    }

    /// Returns a reference (!) to the configuration object.
    /// @return The configuration object set with bootstrapping.
    SharedConfiguration&     GetConfig()                                          { return config; }

    /// Sets the resource pool of this camp.
    /// By default, this function is called with \p{pResourcePool} holding an instance of type
    /// #"LocalResourcePool" when \alib is bootstrapped.
    ///
    /// In case a custom resource pool type (and/or instance) should be used with this camp,
    /// a custom shared pointer has to be created and the custom pool has to be inserted
    /// (potentially using the method #"InsertDerived(TArgs &&...);SharedPtr::InsertDerived").
    /// Then this method has to be called on the camp prior to the invocation of
    /// #"alib::Bootstrap(BootstrapPhases);2".
    ///
    /// Note that the Function #"%alib"::Bootstrap will distribute the given instance to each
    /// lower-level camp that has not received a different object. If this should be avoided
    /// (to separate the resources of this camp from lower level camps), a
    /// #"alib_mod_bs_customize;further customized bootstrap strategy" has to be implemented.
    /// @param pResourcePool  The resource pool to use.
    void                      BootstrapSetResourcePool( const SPResourcePool& pResourcePool ) {
        ALIB_ASSERT_WARNING( (resourcePool == nullptr) != (pResourcePool == nullptr), "CAMPS",
          "Double setting or removing a camp's resource pool instance.\n"
          "Note: A custom resource pool must be set before camp initialization." )

        ResourceHolder::resourcePool   = pResourcePool;
    }

};// class Camp
}} // namespace [alib::camp]


//==================================================================================================
//=== enumrecords::Bootstrap() functions with Camps
//==================================================================================================
ALIB_EXPORT  namespace alib::enumrecords::bootstrap {

/// This method can be used if a set of enum records is resourced using an \alib
/// #"Camp"'s resource instance.
///
/// Invokes #"Bootstrap(ResourcePool&", const NString&, const NString&, character, character)
/// accepting a #"Camp" and using its #"ResourcePool" and the inherited field
/// #"^Camp::ResourceCategory;*".
///
/// \note
///   This is the preferred overload taken with \alib to load built-in enum records.
///   The only exception is the use of overload #"Bootstrap(character", character)
///   for enum record types that require a specialization of
///   #"ResourcedTraits" to perform "recursive" acquisition of other
///   resources defined by fields of the records.
///
/// \par Availability
///   This namespace function is available only if the module \alib_camp is included in
///   the \alibbuild.
///
/// \see Chapters #"alib_enums_records_resourced_tresourced" and
///      #"alib_enums_records_resourced_from_modules" for more information.
///
/// @tparam TEnum      The enumeration type to load resourced records for.
/// @param  camp       The module to use the resource pool and category name from.
/// @param  name       The resource name of the externalized name. In the case that a
///                    resource with that name does not exist, it is tried to load
///                    a resource with index number \c 0 appended to this name, aiming to
///                    parse a single record. On success, the index is incremented until
///                    no consecutive resource is found.
/// @param  innerDelim The delimiter used for separating the fields of a record.
///                    Defaults to <c>','</c>.
/// @param  outerDelim The character delimiting enum records.
///                    Defaults to <c>','</c>.
template<typename TEnum>
requires enumrecords::HasRecords<TEnum>
void Bootstrap( camp::Camp&        camp,
                const NString&     name,
                character          innerDelim= ',' ,
                character          outerDelim= ','    )
{ Bootstrap<TEnum>(camp.GetResourcePool(), camp.ResourceCategory, name, innerDelim, outerDelim); }

} // namespace [alib::enumrecords::bootstrap]

//==================================================================================================
//=== Configuration variable construction with Camps
//==================================================================================================
#include "ALib.Lang.CIFunctions.H"
ALIB_EXPORT  namespace alib::variables {
/// Creates an undeclared configuration variable attached to an \alibcamp.
/// \note While all other overloads of this shortcut-function lock the configuration instance of
///       the given \p{camp}, with this method it this is not needed and thus not done.
///
/// @param camp An singleton which supplies the #"%Configuration" instance.
/// @return A variable using the #"Configuration" of the given \p{camp}.
inline
variables::Variable CampVariable(camp::Camp& camp) {
    return variables::Variable(camp.GetConfig()->Root());
}

/// Constructs and declares a variable, without using or allocating a declaration struct.
/// \note This shortcut-function locks the configuration instance of the given \p{camp} for
///       declaring the variable. Thus, it must not be locked before the call.
/// @param camp         An \alibcamp An \alibcamp singleton which supplies the
///                     #"Configuration" instance.
/// @param name         The name of the variable.
/// @param typeName     The type of the variable.
/// @param defaultValue An optional default value. Defaults to \e nulled string.
/// @return The variable created.
inline
variables::Variable CampVariable( camp::Camp&  camp,
                                  const String& name, const String& typeName,
                                  const String& defaultValue= NULL_STRING )      {
    ALIB_LOCK_WITH(camp.GetConfig())
    auto var= CampVariable(camp);
    var.Declare( name, typeName, defaultValue );
    return var;
}

/// Constructs and declares a variable.
/// \note This shortcut-function locks the configuration instance of the given \p{camp} for
///       declaring the variable. Thus, it must not be locked before the call.
/// @param camp An \alibcamp singleton which supplies the #"Configuration" instance.
/// @param decl The declaration to use.
/// @return The variable created.
inline
variables::Variable CampVariable(camp::Camp& camp, const Declaration* decl) {
    ALIB_LOCK_WITH(camp.GetConfig())
    auto var= CampVariable(camp);
    var.Declare( decl );
    return var;
}

/// Constructs and declares a variable.
/// \note This shortcut-function locks the configuration instance of the given \p{camp} for
///       declaring the variable. Thus, it must not be locked before the call.
/// @tparam TEnum  The type of parameter \p{Enum}
/// @param  camp An \alibcamp singleton which supplies the #"Configuration" instance.
/// @param  Enum   Element of an enum type that represents resourced configuration variables.
/// @return The variable created.
template<typename TEnum>
variables::Variable CampVariable(camp::Camp& camp, TEnum Enum ) {
    ALIB_LOCK_WITH(camp.GetConfig())
    auto var= CampVariable(camp);
    var.Declare( Declaration::Get( Enum ) );
    return var;
}

/// Constructs and declares a variable.
/// \note This shortcut-function locks the configuration instance of the given \p{camp} for
///       declaring the variable. Thus, it must not be locked before the call.
/// \attention This function might allocate a new record in the mono allocator of \p{cfg}.
///            To avoid memory drain, consult the documentation of method
///            #"Configuration::StoreDeclaration;*".,
/// @tparam TEnum         The type of parameter \p{Enum}
/// @param  camp          An \alibcamp singleton which supplies the #"Configuration"
///                       instance.
/// @param  Enum          Element of an enum type that represents resourced configuration variables.
/// @param  replacements  Replacement values.
/// @return The variable created.
template<typename TEnum>
requires std::is_enum_v<TEnum>
variables::Variable CampVariable(camp::Camp& camp, TEnum Enum, const Box replacements ) {
    ALIB_LOCK_WITH(camp.GetConfig())
    auto var= CampVariable(camp);
    var.Declare( camp.GetConfig()->StoreDeclaration( Declaration::Get( Enum ), replacements ) );
    return var;
}
} // namespace [alib::camp]
#include "ALib.Lang.CIMethods.H"

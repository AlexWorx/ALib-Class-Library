//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lang {

//==================================================================================================
/// This class usually is used as a base class for types that need to manage simple plug-ins.
/// Within \alib itself, for example, classes \alib{variables;Configuration} and
/// \alib{expressions;Compiler} inherit from this class.
///
/// Plug-ins are organized with a prioritization. This means, that plug-ins which are inserted with
/// a higher priority are 'asked' first, and those with a lower value become 'asked' only if higher
/// prioritized plug-ins did not answer.<br>
/// However, a derived class can deviate from this behavior. Using the typical \alib class design,
/// all internal fields are \c protected, hence freely accessible by derived classes.
///
/// @tparam TPlugin     The plug-in type that this class manages. This type is publicly exposed as
///                     #PluginType
///                     This type is publicly exposed as #PrioritiesType.
//==================================================================================================
template<typename TPlugin, typename TPriorities>
class Plugin
{
  public:
    /// This exposes the template parameter \p{TPlugin} to the outer world.
    using PluginType=          TPlugin;

    /// This exposes the template parameter \p{pTPlugin} to the outer world.
    using PrioritiesType=      TPriorities;

  protected:
    /// The priority of this plug-in.
    PrioritiesType              priority;

    /// Constructor which is protected, as this is a bace class.
    /// @param pPriority   The priority that this plug-in uses.
    Plugin( PrioritiesType pPriority)
    : priority (pPriority)                                                                        {}

  public:
    /// Returns the priority of this plug-in which is set during construction.
    /// @return The priority of this plugin.
    PrioritiesType             GetPriority()                              const { return priority; }

}; //class Plugin
    
//==================================================================================================
/// This class usually is used as a base class for types that need to manage simple plug-ins.
/// Within \alib itself, for example, classes \alib{variables;Configuration} and
/// \alib{expressions;Compiler} inherit from this class.
///
/// Plug-ins are organized with a prioritization. This means, that plug-ins which are inserted with
/// a higher priority are 'asked' first, and those with a lower value become 'asked' only if higher
/// prioritized plug-ins did not answer.<br>
/// However, a derived class can deviate from this behavior. Using the typical \alib class design,
/// all internal fields are \c protected, hence freely accessible by derived classes.
///
/// @tparam TPlugin     The type of the plug-ins managed. This type is publicly exposed as
///                     #PluginType.
/// @tparam TPriorities The enumeration type providing the default "plug-in slots" and priorities
///                     of the plug-ins managed. This type is publicly exposed as #PrioritiesType.
//==================================================================================================
template<typename TPlugin, typename TPriorities>
class PluginContainer
{
  //################################################################################################
  // public fields and types
  //################################################################################################
  public:
        /// This exposes the template parameter \p{pTPlugin} to the outer world.
        using PluginType=          TPlugin;

        /// This exposes the template parameter \p{pTPlugin} to the outer world.
        using PrioritiesType=      TPriorities;

    /// Type definition for elements of the list of plug-ins with their priority.
    struct  Slot
    {
        TPlugin*    plugin;    ///< The plug-in.
        bool        owned;     ///< If \c true, this container is responsible for deleting
                               ///< the plug-in object.
    };

        /// The list that stores the plugins.
        using PluginList=          std::vector<Slot>;

  //################################################################################################
  // internal fields
  //################################################################################################
  protected:

    /// The plug-ins we have attached in descending priority order.
    PluginList                 plugins;

  //################################################################################################
  // Constructor/destructor
  //################################################################################################
  public:
    /// Destructor. All plug-ins that were inserted with parameter \p{responsibility} set to
    /// \alib{lang;Responsibility::Transfer} will be deleted.
    ~PluginContainer() {
        for( auto& slot : plugins )
            if( slot.owned )
                delete slot.plugin;
    }



  //################################################################################################
  // interface
  //################################################################################################
  public:
    /// Adds the given plug-in to the list of plug-ins.
    ///
    /// @param plugin          The plug-in to insert.
    /// @param responsibility  If \c Responsibility::Transfer, the given plugin will be deleted
    ///                        with destruction of this object.
    ///                        Defaults to \c Responsibility::KeepWithSender which denotes that
    ///                        the life-cycle of the given external buffer is managed elsewhere.
    void                InsertPlugin( TPlugin* plugin, lang::Responsibility responsibility
                                                     = lang::Responsibility::KeepWithSender ) {
        ALIB_ASSERT_ERROR( plugin != nullptr, "FSPLUGINS", "Nullptr provided for plugin." )

        ALIB_STATIC_ASSERT( Plugin_type_not_virtual,
                               std::has_virtual_destructor<TPlugin>::value
                            || responsibility == lang::Responsibility::KeepWithSender,
        "Can't take responsibility for plug-in destruction. TPlugin has no virtual destructor.")

        // gcc needs this captured, clang warns.
        // Its about the ALIB_CALLER inside ALIB_ASSERT_ERROR
        ALIB_WARNINGS_IGNORE_UNUSED_LAMBDA_CAPTURE
        plugins.insert( std::find_if( plugins.begin(),  plugins.end(),
          [plugin,this]( Slot& ppp)
          {
              ALIB_ASSERT_ERROR( ppp.plugin->GetPriority() != plugin->GetPriority(), "FSPLUGINS",
               "PluginContainer::InsertPlugin(): Plug-in with same priority {} exists",
               ppp.plugin->GetPriority() )

              return ppp.plugin->GetPriority() < plugin->GetPriority();
          } ),
                        Slot { plugin, responsibility == lang::Responsibility::Transfer } );
        ALIB_WARNINGS_RESTORE
    }


    /// Removes the given plug-in from the list of plug-ins.
    ///
    /// Responsibility for deletion of removed plug-ins is passed to the remover in case the
    /// plug-in was inserted with parameter \p{responsibility} set to
    /// \alib{lang;Responsibility::Transfer}.
    ///
    /// @param plugIn  The plug-in to be removed.
    /// @return \c true if the plug-in was removed, else \c false, which indicates that the
    ///         given plug-in was not found.
    bool                RemovePlugin( TPlugin* plugIn ) {
        auto it= std::find_if( plugins.begin(), plugins.end(), [plugIn](Slot& pair)
                                                               {
                                                                   return pair.plugin == plugIn;
                                                               } );
        if( it != plugins.end() ) {
            plugins.erase( it );
            return true;
        }

        ALIB_WARNING("LANG", "PluginContainer::RemovePlugin(): Plug-in not found for removal.")
        return  false;
    }

    /// Remove the plug-in at the given \p{idx} from the list of plug-ins.
    ///
    /// Responsibility for deletion of removed plug-ins is passed to the remover in case the
    /// plug-in was inserted with parameter \p{responsibility} set to
    /// \alib{lang;Responsibility::Transfer}.
    ///
    /// @param idx  The index of the plug-in to remove.
    void                RemovePlugin( integer idx ) {
        ALIB_ASSERT_WARNING( idx < CountPlugins(), "FSPLUGINS",
         "PluginContainer::RemovePlugin(): Index out of bounds: {} >= {}", idx, CountPlugins() )
        plugins.erase( plugins.begin() + idx );
    }


    /// Remove the plug-in with the given priority.
    ///
    /// Responsibility for deletion of removed plug-ins is passed to the remover in case the
    /// plug-in was inserted with parameter \p{responsibility} set to
    /// \alib{lang;Responsibility::Transfer}.
    ///
    /// @param priority  The priority of the plug-in to remove.
    /// @return \c true if the plug-in was removed, else \c false, which indicates that no
    ///         plug-in with the given priority was found.
    TPlugin*            RemovePlugin( TPriorities priority ) {
        TPlugin* plugin= nullptr;
        plugins.erase( std::remove_if(  plugins.begin(), plugins.end(),
                                        [priority, &plugin](Slot& entry)
                                        {
                                            if( entry.priority == priority) {
                                                plugin= entry.plugin;
                                                return true;
                                            }
                                            return false;
                                        } ),
                       plugins.end() );

        ALIB_ASSERT_WARNING( plugin, "FSPLUGINS", "No Plug-in was removed." )

        return  plugin;
    }


    /// Checks if any plug-in is attached. This is useful if optional configuration objects
    /// are used. In case no plug-in was attached (by a third party), the effort to declare and
    /// search a variable can be omitted.
    /// @return \c true if this object has any plugin set, \c false otherwise.
    bool                HasPlugins()                                  { return CountPlugins() > 0; }

    /// Returns the number of plug-ins attached.
    /// @return The quantity of attached plug-ins.
    integer             CountPlugins()                           { return integer(plugins.size()); }

    /// Returns the plug-in with the given internal number. Valid numbers are
    /// 0..[#CountPlugins]. No internal checks for valid plug-in numbers are made.
    ///
    /// @param number The number of the plug-in requested.
    /// @return The plug-in requested.
    TPlugin*            GetPlugin( integer number )       { return plugins[size_t(number)].plugin; }

    /// Returns the priority of the plug-in with the given internal number. Valid numbers are
    /// 0..[#CountPlugins]. No internal checks for valid plug-in numbers are made.
    ///
    /// @param number The number of the plug-in requested.
    /// @return The priortiy of the plug-in.
    TPriorities         GetPriority( integer number )   { return plugins[size_t(number)].priority; }


    /// Returns the plug-in with the given priority. If the plug-in does not exist, \c nullptr
    /// is returned.
    ///
    /// @param priority The priority of the plug-in to return.
    /// @return The plug-in requested or \c nullptr if not available.
    TPlugin*            GetPlugin( TPriorities priority ) {
        auto it = std::find_if( plugins.begin(), plugins.end(),
                                [priority](Slot& pair) { return pair.priority == priority; } );
        if( it != plugins.end() )
            return it->plugin;
        return nullptr;
    }

    /// Same as #GetPlugin, but converts the plug-in found to the template type, which has
    /// to be explicitly provided with the invocation of this method.
    ///
    /// A type-check is performed using standard C++ \c dynamic_cast mechanics.
    /// If the plugin has a different type, \c nullptr is returned.
    ///
    /// @tparam TPluginType The type of the plugin to search.
    /// @param priority The priority of the plug-in to return.
    /// @return The plug-in of requested type and priority. \c nullptr if not available.
    template<typename TPluginType>
    TPluginType*        GetPluginTypeSafe( TPriorities priority )
    { return dynamic_cast<TPluginType*>( GetPlugin( priority ) ); }

    /// Searches the list of plug-ins for the first found with type \p{TPluginType}.
    ///
    /// @tparam TPluginType The type of the plugin to search.
    /// @return The plug-in of requested type. \c nullptr if not available.
    template<typename TPluginType>
    TPluginType*        GetPluginTypeSafe() {
        TPluginType* cast= nullptr;
        for( auto& ppp : plugins )
            if( (cast= dynamic_cast<TPluginType*>( ppp.plugin )) != nullptr )
                break;

        return cast;
    }

}; // class PluginContainer

} // namespace [alib::lang]

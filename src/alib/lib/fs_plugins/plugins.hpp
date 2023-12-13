/** ************************************************************************************************
 * \file
 * This header file is part of file set \alibfs_plugins of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FS_PLUGINS_PLUGINS
#define HPP_ALIB_FS_PLUGINS_PLUGINS 1

#if !defined(HPP_ALIB_MODULES) && !defined(ALIB_DOX)
#   include "alib/lib/modules.hpp"
#endif

ALIB_ASSERT_FILESET(PLUGINS)

#if !defined(HPP_ALIB_TOOLS) && !defined(ALIB_DOX)
#   include "alib/lib/tools.hpp"
#endif

#if !defined(HPP_ALIB_FEATURES) && !defined(ALIB_DOX)
#   include "alib/lib/features.hpp"
#endif

#if !defined(HPP_ALIB_FS_COMMONENUMS_DEFS)
#   include "alib/lib/fs_commonenums/commonenumdefs.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_BITWISE)
    #include "alib/enums/bitwise.hpp"
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib { namespace detail {

/** ************************************************************************************************
 * This class usually is used as a base class for types that need to manage simple plug-ins.
 * Within \alib itself, for example classes \alib{config,Configuration} and
 * \alib{expressions,Compiler} inherit from this class.
 *
 * Plug-ins are organized with a prioritization. This means, that plug-ins which are inserted with
 * a higher priority are 'asked' first, and those with a lower value become 'asked' only if higher
 * prioritized plug-ins did not answer.<br>
 * However, a derived class can deviate from this behavior. Using the typical \alib class design,
 * all internal fields are \c protected, hence freely accessible by derived classes.
 *
 * @tparam pTPlugin     The type of the plug-ins managed. This type is publicly exposed as #TPlugin.
 * @tparam pTPriorities The enumeration type providing the default "plug-in slots" and priorities
 *                      of the plug-ins managed. This type is publicly exposed as #TPriorities.
 **************************************************************************************************/
template<typename pTPlugin, typename pTPriorities>
class PluginContainer
{
    // #############################################################################################
    // public fields and types
    // #############################################################################################
    public:

        /** This exposes the template parameter \p{pTPlugin} to the outer world. */
        using TPlugin=          pTPlugin;

        /** This exposes the template parameter \p{pTPlugin} to the outer world. */
        using TPriorities=      pTPriorities;

        /** Type definition for elements of the list of plug-ins with their priority. */
        struct  Slot
        {
            TPriorities priority;  ///< The priority of the plug-in.
            TPlugin*    plugin;    ///< The plug-in.
            bool        owned;     ///< If \c true, this container is responsible for deleting
                                   ///< the plug-in object.
        };

    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:

        /** The plug-ins we have attached in descending priority order. */
        std::vector<Slot>           plugins;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Destructor. All plug-ins that were inserted with parameter \p{responsibility} set to
         * \alib{Responsibility::Transfer} will be deleted.
         ******************************************************************************************/
                       ~PluginContainer()
        {
            for( auto& slot : plugins )
                if( slot.owned )
                    delete slot.plugin;
        }



    // #############################################################################################
    // interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Adds the given plug-in to the list of plug-ins..
         * Higher numbers for parameter priority, prioritize this plug-in against those with
         * lower values.
         *
         * Default values may be provided in case template type #TPriorities is an
         * \alib{enums,T_EnumIsArithmetical,arithmetical enumeration}.
         *
         * @param plugin          The plug-in to insert.
         * @param priority        The priority of the plug-in.
         * @param responsibility  If \c Responsibility::Transfer, the given plugin will be deleted
         *                        with destruction of this object.
         *                        Defaults to \c Responsibility::KeepWithSender which denotes that
         *                        the life-cycle of the given external buffer is managed elsewhere.
         ******************************************************************************************/
        void                InsertPlugin( TPlugin* plugin, TPriorities priority,
                                          Responsibility responsibility
                                                           = Responsibility::KeepWithSender )
        {
            ALIB_ASSERT_ERROR( plugin != nullptr, "FSPLUGINS", "Nullptr provided for plugin." )

            static_assert(    std::has_virtual_destructor<TPlugin>::value
                           || responsibility == Responsibility::KeepWithSender,
                "Can't take responsibility for plug-in destruction. TPlugin has no virtual destructor." );

            plugins.insert( std::find_if( plugins.begin(),  plugins.end(),
                                          [priority]( Slot& ppp)
                                          {
                                              ALIB_ASSERT_ERROR( ppp.priority != priority, "FSPLUGINS",
                                               "PluginContainer::InsertPlugin(): Plug-in with same priority exists" )

                                              return ppp.priority < priority;
                                          } ),

                            Slot {priority, plugin, responsibility == Responsibility::Transfer}
                          );

        }



        /** ****************************************************************************************
         * Remove the given plug-in from the list of plug-ins.
         *
         * Responsibility for deletion of removed plug-ins is passed to the remover in case the
         * plug-in was inserted with parameter \p{responsibility} set to
         * \alib{Responsibility::Transfer}.
         *
         * @param plugIn  The plug-in to be removed.
         * @return \c true if the plug-in was removed, else \c false, which indicates that the
         *         given plug-in was not found.
         ******************************************************************************************/
        bool                RemovePlugin( TPlugin* plugIn )
        {
            auto it= std::find_if( plugins.begin(), plugins.end(), [plugIn](Slot& pair)
                                                                   {
                                                                       return pair.plugin == plugIn;
                                                                   } );
            if( it != plugins.end() )
            {
                plugins.erase( it );
                return true;
            }

            ALIB_WARNING( "PluginContainer::RemovePlugin(): Plug-in not found for removal." )
            return  false;
        }

        /** ****************************************************************************************
         * Remove the plug-in at the given \p{idx} from the list of plug-ins.
         *
         * Responsibility for deletion of removed plug-ins is passed to the remover in case the
         * plug-in was inserted with parameter \p{responsibility} set to
         * \alib{Responsibility::Transfer}.
         *
         * @param idx  The index of the plug-in to remove.
         ******************************************************************************************/
        void                RemovePlugin( integer idx )
        {
            ALIB_ASSERT_WARNING( idx < CountPlugins(),
                                 "FSPLUGINS: PluginContainer::RemovePlugin(): Index out of bounds: ", idx )
            plugins.erase( plugins.begin() + idx );
        }


        /** ****************************************************************************************
         * Remove the plug-in with the given priority.
         *
         * Responsibility for deletion of removed plug-ins is passed to the remover in case the
         * plug-in was inserted with parameter \p{responsibility} set to
         * \alib{Responsibility::Transfer}.
         *
         * @param priority  The priority of the plug-in to remove.
         * @return \c true if the plug-in was removed, else \c false, which indicates that no
         *         plug-in with the given priority was found.
         ******************************************************************************************/
        TPlugin*            RemovePlugin( TPriorities priority )
        {
            TPlugin* plugin= nullptr;
            plugins.erase( std::remove_if(  plugins.begin(), plugins.end(),
                                            [priority, &plugin](Slot& entry)
                                            {
                                                if( entry.priority == priority)
                                                {
                                                    plugin= entry.plugin;
                                                    return true;
                                                }
                                                return false;
                                            } ),
                           plugins.end() );

            ALIB_ASSERT_WARNING( plugin, "FSPLUGINS",
                                 "PluginContainer::RemovePlugin(): No Plug-in was removed " )

            return  plugin;
        }


        /** ****************************************************************************************
         * Checks if any plug-in is attached. This is useful if optional configuration objects
         * are used. In case no plug-in was attached (by a third party), the effort to declare and
         * search a variable can be omitted.
         * @return \c true if this object has any plugin set, \c false otherwise.
         ******************************************************************************************/
        bool                HasPlugins()
        {
            return CountPlugins() > 0;
        }

        /** ****************************************************************************************
         * Returns the number of plug-ins attached.
         * @return The quantity of attached plug-ins.
         ******************************************************************************************/
        integer             CountPlugins()
        {
            return static_cast<integer>(plugins.size());
        }

        /** ****************************************************************************************
         * Returns the plug-in with the given internal number. Valid numbers are
         * 0..[#CountPlugins]. No internal checks for valid plug-in numbers are made.
         *
         * @param number The number of the plug-in requested.
         * @return The plug-in requested or \c nullptr if not available.
         ******************************************************************************************/
        TPlugin*            GetPlugin( integer number )
        {
            return plugins[static_cast<size_t>(number)].plugin;
        }


        /** ****************************************************************************************
         * Returns the plug-in with the given priority. If the plug-in does not exist, \c nullptr
         * is returned.
         *
         * @param priority The priority of the plug-in to return.
         * @return The plug-in requested or \c nullptr if not available.
         ******************************************************************************************/
        TPlugin*            GetPlugin( TPriorities priority )
        {
            auto it = std::find_if( plugins.begin(), plugins.end(),
                                    [priority](Slot& pair) { return pair.priority == priority; } );
            if( it != plugins.end() )
                return it->plugin;
            return nullptr;
        }

        /** ****************************************************************************************
         * Same as #GetPlugin, but converts the plug-in found to the template type, which has
         * to be explicitly provided with the invocation of this method.
         *
         * A type-check is performed using standard C++ \c dynamic_cast mechanics.
         * If the plugin has a different type, \c nullptr is returned.
         *
         * @tparam TPluginType The type of the plugin to search.
         * @param priority The priority of the plug-in to return.
         * @return The plug-in of requested type and priority. \c nullptr if not available.
         ******************************************************************************************/
        template<typename TPluginType>
        TPluginType*        GetPluginTypeSafe( TPriorities priority )
        {
            return dynamic_cast<TPluginType*>( GetPlugin( priority ) );
        }

        /** ****************************************************************************************
         * Searches the list of plug-ins for the first found with type \p{TPluginType}.
         *
         * @tparam TPluginType The type of the plugin to search.
         * @return The plug-in of requested type. \c nullptr if not available.
         ******************************************************************************************/
        template<typename TPluginType>
        TPluginType*        GetPluginTypeSafe()
        {
            TPluginType* casted= nullptr;
            for( auto& ppp : plugins )
                if( (casted= dynamic_cast<TPluginType*>( ppp.plugin )) != nullptr )
                    break;

            return casted;
        }

}; // class PluginContainer

}}} // namespace [aworx::lib::detail]




#endif // HPP_ALIB_FS_PLUGINS_PLUGINS

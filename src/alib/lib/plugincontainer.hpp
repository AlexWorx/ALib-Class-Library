// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_PLUGINCONTAINER
#define HPP_ALIB_PLUGINCONTAINER 1

#if !defined(HPP_ALIB_LIB_PREDEF_MODULES)
#   include "alib/lib/predef_modules.hpp"
#endif

ALIB_ASSERT_FILESET(PLUGINS)

#if !defined(HPP_ALIB_LIB_PREDEF_COMPILER)
#   include "alib/lib/predef_compiler.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_PLATFORM)
#   include "alib/lib/predef_platform.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_TOOLS)
#   include "alib/lib/predef_tools.hpp"
#endif

#if !defined(HPP_ALIB_LIB_PREDEF_FEATURES)
#   include "alib/lib/predef_features.hpp"
#endif

#if !defined(HPP_ALIB_LIB_ASSERT)
#   include "alib/lib/assert.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ENUM_BITWISE)
    #include "alib/enums/enumbitwise.hpp"
#endif

#if ALIB_MODULE_THREADS && !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlock.hpp"
#endif


#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib {

/** ************************************************************************************************
 * This class usually is used as a base class for types that need to manage simple plug-ins.
 * Within \alib itself, for example classes \alib{config,Configuration} and
 * \alib{expressions,Compiler} inherit from this class.
 *
 * Plug-ins are organized with a prioritization. This means, that plug-ins which are inserted with
 * a higher priority are 'asked' first, and those with a lower value become 'asked' only if higher
 * prioritized plug-ins did not answer.<br>
 * However, a derived class can deviate from this behaviour. Using the typical \alib class design,
 * all internal fields are \c protected, hence freely accessible by derived classes.
 *
 * @tparam pTPlugin     The type of the plug-ins managed. This type is publicly exposed as #TPlugin.
 * @tparam pTPriorities The enumeration type providing the default "plug-in slots" and priorities
 *                      of the plug-ins managed. This type is publicly exposed as #TPriorities.
 **************************************************************************************************/
template<typename pTPlugin, typename pTPriorities>
class PluginContainer
#if ALIB_MODULE_THREADS
    : public threads::ThreadLock
#endif
{
    // #############################################################################################
    // public fields and types
    // #############################################################################################
    public:

        /// This exposes the template parameter \p{pTPlugin} to the outer world.
        using TPlugin=          pTPlugin;

        /// This exposes the template parameter \p{pTPlugin} to the outer world.
        using TPriorities=      pTPriorities;

        /** Type definition for elements of the list of plug-ins with their priority. */
        struct  Slot
        {
            TPriorities priority;  ///< The priority of the plug-in.
            TPlugin*    plugin;    ///< The plug-in.
        };

    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:

        /** The plug-ins we have attached in in descending priority order. */
        std::vector<Slot>           plugins;

        /**
         * This vector of plug-ins contain those plug-ins which are inserted with parameter
         * \p{responsibility} set to \alib{Responsibility::Transfer}.<br>
         * Consequently, plug-ins listed in this vector are deleted in the destructor of this class.
         */
        std::vector<TPlugin*>       pluginsOwned;

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
            for( auto plugin : pluginsOwned )
                delete plugin;
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
            ALIB_LOCK
            ALIB_ASSERT_ERROR( plugin != nullptr, "Nullptr provided for plugin." )

            static_assert(    std::has_virtual_destructor<TPlugin>::value
                           || responsibility == Responsibility::KeepWithSender,
                "Can't take responsibility for plug-in destruction. TPlugin has no virtual destructor." );

            if( responsibility == Responsibility::Transfer )
                pluginsOwned.emplace_back( plugin );

            plugins.insert( std::find_if( plugins.begin(),  plugins.end(),
                                          [priority]( Slot& ppp)
                                          {
                                              ALIB_ASSERT_ERROR( ppp.priority != priority,
                                               "PluginContainer::InsertPlugin(): Plug-in with same priority exists" )

                                              return ppp.priority < priority;
                                          } ),

                            Slot {priority, plugin}  );

        }



        /** ****************************************************************************************
         * Remove the given plug-in from the list of plug-ins.
         *
         * Responsibility for deletion of removed plug-ins is passed to the remover in case the
         * plug-in was inserted with parameter \p{responsibility} set to
         * \alib{Responsibility::Transfer}.
         *
         * @param plugin  The plug-in to be removed.
         * @return \c true if the plug-in was removed, else \c false, which indicates that the
         *         given plug-in was not found.
         ******************************************************************************************/
        bool                RemovePlugin( TPlugin* plugin )
        {
            ALIB_LOCK

            size_t qty= plugins.size();
            plugins.erase( std::remove_if( plugins.begin(), plugins.end(), [plugin](Slot& pair)
                                                                           {
                                                                               return pair.plugin == plugin;
                                                                           } ),
                           plugins.end() );

            ALIB_ASSERT_WARNING( plugins.size() != qty,
                                 "PluginContainer::RemovePlugin(): No Plug-in was removed " )

            // remove from list of managed plug-ins (if exists)
            for( auto it= pluginsOwned.begin() ; it != pluginsOwned.end() ; ++it )
                if( *it == plugin )
                {
                    pluginsOwned.erase( it );
                    break;
                }

            return  plugins.size() < qty;
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
            ALIB_LOCK

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

            ALIB_ASSERT_WARNING( plugin,
                                 "PluginContainer::RemovePlugin(): No Plug-in was removed " )

            return  plugin;
        }


        /** ****************************************************************************************
         * Checks if any plug-in is attached. This is useful if optional configuration objects
         * are used. In case no plug-in was attached (by a third party), the effort to declare and
         * search a variable can be omitted.
         * @return \c true if this object has any plugin set, \c false otherwise.
         ******************************************************************************************/
        inline
        bool                HasPlugins()
        {
            return plugins.size() > 0;
        }

        /** ****************************************************************************************
         * Returns the number of plug-ins attached.
         * @return The quantity of attached plug-ins.
         ******************************************************************************************/
        inline
        size_t              CountPlugins()
        {
            return plugins.size();
        }

        /** ****************************************************************************************
         * Returns the plug-in with the given internal number. Valid numbers are
         * 0..[#CountPlugins]. No internal checks for valid plug-in numbers are made.
         *
         * \note
         *   The method is not thread-safe. At least, the returned plugin may become invalid
         *   after the method returned.
         *   For thread-safe operations, this object needs to be
         *   \alib{threads,ThreadLock::Acquire,acquired} and
         *   \alib{threads,ThreadLock::Release,released} explicitly.
         *
         * @param number The number of the plug-in requested.
         *
         * @return The plug-in requested or \c nullptr if not available.
         ******************************************************************************************/
        inline
        TPlugin*            GetPlugin( size_t number )
        {
            return plugins[number].plugin;
        }


        /** ****************************************************************************************
         * Returns the plug-in with the given priority. If the plug-in does not exist, \c nullptr
         * is returned.
         *
         * \note
         *   The method is not thread-safe. At least, the returned plugin may become invalid
         *   after the method returned.
         *   For thread-safe operations, this object needs to be
         *   \alib{threads,ThreadLock::Acquire,acquired} and
         *   \alib{threads,ThreadLock::Release,released} explicitly.
         *
         * @param priority The priority of the plug-in to return.
         *
         * @return The plug-in requested or \c nullptr if not available.
         ******************************************************************************************/
        TPlugin*            GetPlugin( TPriorities priority )
        {
            ALIB_LOCK

            auto it = std::find_if( plugins.begin(), plugins.end(),
                                    [priority](Slot& pair)
                                    {
                                        return pair.priority == priority;
                                    }                                          );
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
         * @param priority The priority of the plug-in to return.
         * @tparam TPluginType The type of the plugin to search.
         * @return The plug-in of requested type and priority. \c nullptr if not available.
         ******************************************************************************************/
        template<typename TPluginType>
        inline
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

}} // namespace [aworx::lib]




#endif // HPP_ALIB_PLUGINCONTAINER

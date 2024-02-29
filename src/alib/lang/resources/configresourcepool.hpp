/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::resources of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL
#define HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL 1

#if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined (HPP_ALIB_LANG_RESOURCES_DETAIL_RESOURCEMAP)
#   include "alib/lang/resources/detail/resourcemap.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif

#if ALIB_THREADS && !defined (HPP_ALIB_THREADS_THREADLOCK)
#   include "alib/threads/threadlock.hpp"
#endif

namespace alib::lang::resources {


/** ************************************************************************************************
 * This class provides an implementation of abstract interface class \alib{lang::resources,ResourcePool},
 * which does externalize resources by using the mechanics provided with module \alib_config.
 *
 * A (shared) instance of this class can be attached to each \alibmod, with
 * \ref alib_manual_bootstrapping "customization the bootstrapping of ALib".
 *
 * \see
 *  Please consult the \ref alib_basecamp_resources "Programmer's Manual" of module \alib_basecamp_nl
 *  for details on the concept of resources. Details on this specific type are given in chapter
 *  \ref alib_basecamp_resources_interface_config "3.4.2 Class ConfigResourcePool".
 **************************************************************************************************/
class ConfigResourcePool  : public ResourcePool
#if ALIB_THREADS
                       , public threads::ThreadLock
#endif

{
    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:
        /**
         * Monotonic allocator used for the hash map.
         */
        MonoAllocator                   allocator;

        /** A hash map used to store resources that have been acquired.
         *  While this also improves performance, a cache is needed to assure persistence of
         *  the values.                                                                       */
        detail::CachingResourceMap      cache;

        /**  An only locally used object. Declared as field member to be reusable. */
        config::Variable                variable;

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**  The configuration object for custom resource data.          */
        config::Configuration           Config;



    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
    /**
     * Constructor. Creates a configuration object without one plug-in of type
     * \alib{config,InMemoryPlugin} with
     * \alib{config,Priorities::DefaultValues,default priority}.
     * After construction, one or more custom plug-ins have to be added. Those may be emplaced
     * in this object's \alib{monomem,MonoAllocator}
     */
    ALIB_API
    ConfigResourcePool();

    /** Destructor.  */
    ALIB_API
    ~ConfigResourcePool()                                                                  override;

    // #############################################################################################
    // Resource Interface
    // #############################################################################################

    /** ********************************************************************************************
     * Implements abstract method \alib{lang::resources,ResourcePool::BootstrapAddOrReplace}.
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param data       The resource string.
     * @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
     **********************************************************************************************/
    bool BootstrapAddOrReplace( const NString& category,
                                const NString& name,
                                const  String& data        )                               override;


    /** ********************************************************************************************
     * Implements abstract method \alib{lang::resources,ResourcePool::BootstrapBulk}.
     *
     * @param category    Category string of the resource. For technical reasons, this has to be
     *                    of type <c>const nchar*</c>.
     * @param ...         A list of pairs of <b>const nchar*</b> and <b>const character*</b>
     *                    keys and data.
     **********************************************************************************************/
    ALIB_API
    virtual
    void BootstrapBulk( const nchar* category, ... )                                       override;

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Implements abstract method \alib{lang::resources,ResourcePool::Get}.
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was not found.
     * @return The resource string, respectively a \e nulled string on failure.
     **********************************************************************************************/
    NALIB_API
    const String& Get( const NString& category, const NString& name, bool dbgAssert   )    override;
#else
    ALIB_API
    const String& Get( const NString& category, const NString& name
                                                          ALIB_DBG(, bool dbgAssert)  )    override;
#endif

}; // class ConfigResourcePool

} // namespace [alib::lang::resources]



#endif // HPP_ALIB_LANG_RESOURCES_CONFIG_RESOURCE_POOL

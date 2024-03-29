/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::resources of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_RESOURCES_LOCALRESOURCEPOOL
#define HPP_ALIB_LANG_RESOURCES_LOCALRESOURCEPOOL 1

#if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif
#if !defined (HPP_ALIB_LANG_RESOURCES_DETAIL_RESOURCEMAP)
#   include "alib/lang/resources/detail/resourcemap.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif


namespace alib::lang::resources {

/** ************************************************************************************************
 * This class provides a very simple implementation of abstract interface class
 * \alib{lang::resources,ResourcePool}, which does \b not externalize resources.
 *
 * A shared instance of this class is attached to each \alibmod, if the
 * \ref alib_manual_bootstrapping "bootstrapping of ALib" is not customized.
 *
 * With the use of type \alib{monomem,HashTable} for its internal data management, this class uses
 * \ref alib_mod_monomem "monotonically growing memory" taken from the global allocator.
 *
 * Only pointers to the given resources (as well as their associated category and name strings)
 * are stored, hence all string argument's of this class's methods have to be static data.
 *
 * For debug or optimization purposes, method #BootstrapGetInternalHashMap is given that allows
 * fine-tune the performance parameters or to inspect the data. For the latter, in
 * debug-compilations, consider also methods #DbgGetList and #DbgGetCategories.
 *
 * \see
 *  Please consult chapter \ref alib_basecamp_resources "3. Namespace alib::lang::resources" of
 *  the Programmer's Manual of module \alib_basecamp for details on the concept of resources.
 *  Some details on this specific type are given in chapter
 *  \ref alib_basecamp_resources_interface_default.
 *
 **************************************************************************************************/
class LocalResourcePool  : public ResourcePool
{
    protected:
        /** A hash map used to store static resources. */
        detail::StaticResourceMap       data;

    #if ALIB_DEBUG_RESOURCES
    /** If set prior to bootstrapping (e.g. to <c>&std::cout</c>), then each found resource string
     *  is written here. This is very useful to find errors in bulk resource strings, for example
     *  a simple missing comma.
     *
     * ### Availability ###
     * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set.
     *  \see Manual section \ref alib_basecamp_resources_details_debug.
     */
    public: static std::ostream*    DbgResourceLoadObserver;
    #endif

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /** Constructor. */
        LocalResourcePool()
        : data( &monomem::GlobalAllocator )
        {}

        /** Destructor. */
        ALIB_API
        virtual
        ~LocalResourcePool()                                                                override
        {}

        /**
         * Returns the internal table that maps a pair of category and name strings to the resource
         * string.
         *
         * Access to this map may be useful for two purposes:
         * 1. Debug inspection.
         * 2. Modification of the parameters of the hash table. (For example, the use of methods
         *    \alib{monomem::HashTable,Reserve}, \alib{monomem::HashTable,BaseLoadFactor} or
         *    \alib{monomem::HashTable,MaxLoadFactor}).
         *
         * Modifications on the returned object are allowed only while no threads have been started
         * by the software process (respectively no threads that use \alib ), which usually is true
         * during bootstrapping a software. Therefore the prefix in this method's name.
         *
         * @return The internal hash map.
         */
        detail::StaticResourceMap&    BootstrapGetInternalHashMap()
        {
            return data;
        }


    // #############################################################################################
    // ResourcePool Interface
    // #############################################################################################

    /** ********************************************************************************************
     * Implements abstract method \alib{lang::resources,ResourcePool::BootstrapAddOrReplace}.
     * \note
     *   If compiler symbol \ref ALIB_DEBUG_RESOURCES is set on compilation and static field
     *   \alib{lang::resources,LocalResourcePool::DbgResourceLoadObserver} is set prior to bootstrapping,
     *   this method will write information about whether the given data is added or replaced
     *   to the debug output stream.
     *
     * @param category   Category string of the resource to add.
     * @param name       Name string of the resource.
     * @param data       The resource data.
     * @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
     **********************************************************************************************/
    ALIB_API
    virtual
    bool BootstrapAddOrReplace( const NString& category,
                                const NString& name,
                                const  String& data        )                               override;

    /** ********************************************************************************************
     * Implements abstract method \alib{lang::resources,ResourcePool::BootstrapBulk}.
     *
     * @param category  The category of the resources given.
     * @param ...       A list of pairs of <b>const nchar*</b> and <b>const character*</b>
     *                  keys and data, including a terminating \c nullptr value.
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
    virtual
    const String&   Get( const NString& category, const NString& name, bool dbgAssert  )   override;
#else
    ALIB_API
    virtual
    const String&   Get( const NString& category, const NString& name
                                                            ALIB_DBG(, bool dbgAssert) )   override;
#endif

    #if ALIB_DEBUG_RESOURCES
        /** ****************************************************************************************
         * Implements abstract method \alib{lang::resources,ResourcePool::DbgGetList}.
         *
         * ### Availability ###
         * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set.
         *
         * @return The externalized resource string.
         ******************************************************************************************/
        ALIB_API
        virtual
        std::vector<std::tuple<NString, NString, String, integer>>
        DbgGetList()                                                                       override;

        /** ****************************************************************************************
         * Implements abstract method \alib{lang::resources,ResourcePool::DbgGetCategories}.
         *
         * ### Availability ###
         * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set.
         *
         * @return The externalized resource string.
         ******************************************************************************************/
        ALIB_API
        virtual
        std::vector<std::pair<NString, integer>>
        DbgGetCategories()                                                                 override;
    #endif


}; // class LocalResourcePool

} // namespace [alib::lang::resources]


#endif // HPP_ALIB_LANG_RESOURCES_LOCALRESOURCEPOOL

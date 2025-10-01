//==================================================================================================
/// \file
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::resources {

//==================================================================================================
/// This class provides a very simple implementation of abstract interface class
/// \alib{resources;ResourcePool}, which does \b not externalize resources.
///
/// A shared instance of this class is attached to each \alibmod, if the
/// \ref alib_mod_bs "bootstrapping of ALib" is not customized.
///
/// With the use of type \alib{containers;HashTable} for its internal data management, this
/// class uses \ref alib_mods_contmono "monotonically growing memory" taken from the global
/// allocator.
///
/// Only pointers to the given resources (as well as their associated category and name strings)
/// are stored, hence all string argument's of this class's methods have to be static data.
///
/// For debug or optimization purposes, method #BootstrapGetInternalHashMap is given that allows
/// fine-tune the performance parameters or to inspect the data. For the latter, in
/// debug-compilations, consider also methods #DbgGetList and #DbgGetCategories.
///
/// \see
///  Please consult the \ref alib_mod_resources "Programmer's Manual" of module \alib_resources_nl for
///  details on the concept of resources.
///  Some details on this specific type are given in chapter
///  \ref alib_resources_interface_default.
///
//==================================================================================================
class LocalResourcePool  : public ResourcePool
{
    protected:
        /// A hash map used to store static resources.
        detail::StaticResourceMap       data;

    #if ALIB_DEBUG_RESOURCES
    /// If set before bootstrapping (e.g., to <c>&std::cout</c>), then each found resource string
    /// is written here. This is very useful to find errors in bulk resource strings, for example
    /// a simple missing comma.
    ///
    /// \par Availability
    ///  Available only if the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set.
    ///
    /// \see Manual section \ref alib_resources_details_debug.
    public: static std::ostream*    DbgResourceLoadObserver;
    #endif

    // #############################################################################################
    // Constructor/Destructor
    // #############################################################################################
    public:
        /// Constructor.
        LocalResourcePool()
        : data( monomem::GLOBAL_ALLOCATOR )
        {}

        /// Destructor.
        ALIB_DLL
        virtual
        ~LocalResourcePool()                                                                override
        {}

        /// Returns the internal table that maps a pair of category and name strings to the resource
        /// string.
        ///
        /// Access to this map may be useful for two purposes:
        /// 1. Debug inspection.
        /// 2. Modification of the parameters of the hash table. (For example, the use of methods
        ///    \alib{containers::HashTable;Reserve},
        ///    \alib{containers::HashTable;BaseLoadFactor} or
        ///    \alib{containers::HashTable;MaxLoadFactor}).
        ///
        /// Modifications on the returned object are allowed only while no threads have been started
        /// by the software process (respectively no threads that use \alib ), which usually is true
        /// during bootstrapping a process. Therefore , the prefix in this method's name.
        ///
        /// @return The internal hash map.
        detail::StaticResourceMap&    BootstrapGetInternalHashMap()
        {
            return data;
        }


    // #############################################################################################
    // ResourcePool Interface
    // #############################################################################################

    //==============================================================================================
    /// Implements abstract method \alib{resources;ResourcePool::BootstrapAddOrReplace}.
    /// \note
    ///   If the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set on compilation and static field
    ///   \alib{resources;LocalResourcePool::DbgResourceLoadObserver} is set before bootstrapping,
    ///   this method will write information about whether the given data is added or replaced
    ///   to the debug output stream.
    ///
    /// @param category   Category string of the resource to add.
    /// @param name       Name string of the resource.
    /// @param data       The resource data.
    /// @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
    //==============================================================================================
    ALIB_DLL
    virtual
    bool BootstrapAddOrReplace( const NString& category,
                                const NString& name,
                                const  String& data        )                               override;

    //==============================================================================================
    /// Implements abstract method \alib{resources;ResourcePool::BootstrapBulk}.
    ///
    /// @param category  The category of the resources given.
    /// @param ...       A list of pairs of <b>const nchar*</b> and <b>const character*</b>
    ///                  keys and data, including a terminating \c nullptr value.
    //==============================================================================================
    ALIB_DLL
    virtual
    void BootstrapBulk( const nchar* category, ... )                                       override;


#if DOXYGEN
    //==============================================================================================
    /// Implements abstract method \alib{resources;ResourcePool::Get}.
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an \ref alib_mod_assert "error is raised" if the resource
    ///                   was not found.
    /// @return The resource string, respectively a \e nulled string on failure.
    //==============================================================================================
    ALIB_DLL
    virtual
    const String&   Get( const NString& category, const NString& name, bool dbgAssert  )   override;
#else
    ALIB_DLL
    virtual
    const String&   Get( const NString& category, const NString& name
                                                            ALIB_DBG(, bool dbgAssert) )   override;
#endif

    #if ALIB_DEBUG_RESOURCES 
        //==========================================================================================
        /// Returns a vector of tuples for each resourced element. Each tuple contains:
        /// 0. The category name
        /// 1. The resource name
        /// 2. The resource value
        /// 3. The number of requests for the resource performed by using software.
        ///
        /// While being useful to generally inspect the resources, a high number of requests
        /// might indicate a performance penalty. Mitigation can usually be performed in a very
        /// simple fashion by "caching" a resource string in a local or global/static string
        /// variable.
        ///
        /// \par Availability
        ///   Available only if the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set.
        ///
        /// \attention
        ///   This function is implemented only with the default pool instance of type
        ///   \alib{resources;LocalResourcePool} is used.
        ///   Otherwise, an \alib warning is raised and an empty vector is returned.
        ///
        /// \see
        ///   Methods #DbgGetCategories and #DbgDump.
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
        virtual
        std::vector<std::tuple<NString, NString, String, integer>>
        DbgGetList()                                                                       override;


        //==========================================================================================
        /// Implements abstract method \alib{resources;ResourcePool::DbgGetCategories}.
        ///
        /// \par Availability
        ///   Available only if the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set.
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
        virtual
        std::vector<std::pair<NString, integer>>
        DbgGetCategories()                                                                 override;
    #endif
}; // class LocalResourcePool

} // namespace [alib::resources]

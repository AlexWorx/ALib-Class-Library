//==================================================================================================
/// \file
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace resources {
//==================================================================================================
/// This purely abstract class provides an interface to store and retrieve "resourced" string data
/// which are organized in a two-level key hierarchy named <em>"resource category"</em>
/// and <em>"resource name"</em>. The latter are of narrow string-type.
///
/// \see
///   For detailed documentation on when and how this interface is used, please consult the
///   #"alib_mod_resources;Programmer's Manual" of this module \alib_resources_nl.
///
/// \see
///   Two built-in implementations of this pure abstract interface are provided with
///   #"LocalResourcePool" and #"ConfigResourcePool".
///   Please consult their reference documentation for further details.
//==================================================================================================
class ResourcePool {
  public:

    /// Virtual destructor.
    virtual ~ResourcePool()                                                                =default;

    /// Used to store a resource string.
    ///
    /// In the context of \alibmods, which usually are the only areas where instances of this
    /// type are available (used), this method must only be invoked during the process of
    /// #"alib_mod_bs;bootstrapping" \alib (and corresponding custom modules).
    ///
    /// \attention
    ///   The life-cycle of the given string's buffers, have to survive this resource instance.
    ///   Usually the strings passed here are constant C++ string literals, residing in the data
    ///   segment of an executable
    ///
    /// \note
    ///   Usually, the method #"Bootstrap" should be preferred, which asserts in debug-compilations
    ///   if a resource already existed.
    ///   The use of this method is for special cases, for example, to replace (patch) resources
    ///   of dependent modules.
    ///
    /// @param category   Category string of the resource to add.
    /// @param name       Name string of the resource.
    /// @param data       The resource data.
    /// @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
    virtual
    bool BootstrapAddOrReplace(const NString& category, const NString& name, const String& data) =0;

    /// Simple inline method that invokes virtual method #".BootstrapAddOrReplace".
    /// In debug-compilations, it is asserted that a resource with the given key did not exist
    /// already.
    ///
    /// The use of this method is preferred over a direct invocation of #".BootstrapAddOrReplace".
    /// @param category   Category string of the resource to add.
    /// @param name       Name string of the resource.
    /// @param data       The resource data.
    inline
    void Bootstrap( const NString& category, const NString& name, const String& data ) {
        #if ALIB_DEBUG
            bool result=
        #endif
            BootstrapAddOrReplace(category, name, data);

        ALIB_ASSERT_ERROR(!result,"RESOURCES", "Doubly defined resource \"{}\" in category \"{}\".",
                                                name, category )
    }

    /// Same as #"Bootstrap" but accepts an array of name/value pairs to be filled into the given
    /// \p{category}.
    ///
    /// \attention
    ///   <b>The given list has to be finished with a final \c nullptr argument for the next
    ///   name!</b>
    ///
    /// In the context of \alibcamps, which usually are the only areas where instances of this
    /// type are used, this method must only be invoked during the process of
    /// #"alib_mod_bs;bootstrapping" \alib (and corresponding custom modules).
    ///
    /// \attention
    ///   The life-cycle of the given string's buffers, have to survive this resource instance.
    ///   Usually the strings passed here are constant C++ string literals, residing in the data
    ///   segment of an executable
    ///
    /// \note
    ///   The use of variadic C-style arguments <c>"..."</c> in general is \b not recommended
    ///   to be used.
    ///   We still do it here, because, as the name of this method indicates, usually a large
    ///   nummber of resources is loaded with one call.
    ///   This approach saves a lot of otherwise necessary single invocations (reduces code size)
    ///   and allows a rather clean code for the init methods.<br>
    ///   For technical reasons, parameter \p{category } is declared as type <c>const nchar*</c>.
    ///
    /// @param category  The category of the resources given.
    /// @param ...       A list of pairs of <b>const nchar*</b> and <b>const character*</b>
    ///                  keys and data, including a terminating \c nullptr value.
    virtual
    void BootstrapBulk( const nchar* category, ... )                                             =0;

#if DOXYGEN
    //==============================================================================================
    /// Returns a resource.
    /// On failure (resource not found), a \e nulled string is returned.
    ///
    /// \note
    ///   Usually resource pools are associated with #"ResourceHolder" objects and
    ///   resources should be loaded using its "shortcut methods"
    ///   #"ResourceHolder::TryResource" and
    ///   #"ResourceHolder::GetResource".
    ///   If used directly, the argument \p{dbgAssert} has to be enclosed in macro #"ALIB_DBG"
    ///   (including the separating comma).
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an #"alib_mod_assert;error is raised" if the resource
    ///                   was not found.
    /// @return The resource string, respectively a \e nulled string on failure.
    //==============================================================================================
    virtual
    const String&   Get( const NString& category, const NString& name, bool dbgAssert )         = 0;
#else
    virtual
    const String&   Get( const NString& category, const NString& name ALIB_DBG(,bool dbgAssert)) =0;
#endif

#if DOXYGEN
    //==============================================================================================
    /// Convenience inlined method that accepts parameter name as #"characters::character"
    /// instead of #"characters::nchar" based string-type. The rationale for this is that often,
    /// resource name keys are read from other resourced strings and need conversion if used.
    /// This avoids external conversion before invoking this method.
    ///
    /// This method is available only when \alib is compiled with type #"characters::character"
    /// not being equivalent to #"characters::nchar".
    ///
    /// After the string conversion, this method simply returns the result of the virtual method
    /// #".Get(const NString&, const NString&, bool)".
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an #"alib_mod_assert;error is raised" if the resource
    ///                   was not found.
    /// @return The resource string, respectively a \e nulled string on failure.
    //==============================================================================================
    const String&   Get( const NString& category, const String& name, bool dbgAssert );
#else
    #if ALIB_CHARACTERS_WIDE
        const String&   Get( const NString& category, const String& name ALIB_DBG(,bool dbgAssert) )
        {
            NString128 nName( name );
            return Get( category, nName  ALIB_DBG(, dbgAssert ) );
        }
    #endif
#endif

    #if ALIB_DEBUG_RESOURCES
        //==========================================================================================
        /// Returns a vector of tuples for each resourced element. Each tuple contains:
        /// 0. The category name
        /// 1. The resource name
        /// 2. The resource value
        /// 3. The number of requests for the resource performed by a using data.
        ///
        /// While being useful to generaly inspect the resources, a high number of requests
        /// might indicate a performance penalty for a using software. Such can usually be
        /// mitigated in a very simple fashion by "caching" a resource string in a local
        /// or global/static string variable.
        ///
        /// \par Availability
        ///   Available only if the configuration macro #"ALIB_DEBUG_RESOURCES" is set.
        ///
        /// \attention
        ///   This method is implemented only with the default pool instance of type
        ///   #"LocalResourcePool".
        ///   Other implementations raise an \alib_warning and return an empty vector.
        ///
        /// \see
        ///   Method #"DbgGetCategories" and namespace function #"resources::DbgDump".
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
    virtual
    std::vector<std::tuple<NString, NString, String, integer>>  DbgGetList();

        //==========================================================================================
        /// Implements abstract method #"ResourcePool::DbgGetCategories;*".
        ///
        /// \par Availability
        ///   Available only if the configuration macro #"ALIB_DEBUG_RESOURCES" is set.
        ///
        /// \attention
        ///   This method is implemented only with the default pool instance of type
        ///   #"LocalResourcePool".
        ///   Other implementations raise an \alib_warning and return an empty vector.
        ///
        /// \see
        ///   Method #"DbgGetList" and namespace function #"resources::DbgDump".
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
    virtual
    std::vector<std::pair<NString, integer>>                    DbgGetCategories();
    #endif //ALIB_DEBUG_RESOURCES
}; // class ResourcePool

//==================================================================================================
/// A type serving as the base for types that hold a pointer to a #"LocalResourcePool"
/// and a fixed category string.
/// \note The most prominent descendant of this type is class #"Camp".
//==================================================================================================
class ResourceHolder {
  public:
    /// Type definition used for sharing resource-pool instances. With default-bootstrapping,
    /// an instance of type #"LocalResourcePool" is used here, which is
    /// created with allocator instance #"GLOBAL_ALLOCATOR".
    using SPResourcePool = SharedPtr<resources::ResourcePool, MonoAllocator>;

  protected:
    /// Shared pointer to the resource pool.
    SPResourcePool      resourcePool;

  public:
    /// The name of the resource category of externalized string resources defined and used by
    /// this resource holder.<br>
    NCString           ResourceCategory;

    /// Constructor.
    /// @param resourceCategory  Value for field #".ResourceCategory".
    ResourceHolder( const NCString& resourceCategory= nullptr )
    : ResourceCategory(resourceCategory)                                                          {}

    /// Constructor.
    /// @param pool              Value for the field #"resourcePool".
    /// @param resourceCategory  Value for the field #".ResourceCategory".
    ResourceHolder( SPResourcePool& pool,  const NCString& resourceCategory )
    : resourcePool    (pool)
    , ResourceCategory(resourceCategory)                                                          {}

    /// Constructor.
    /// @param pool              Value for the field #".resourcePool".
    /// @param resourceCategory  Value for field #".ResourceCategory". If not given (<em>nulled</em>),
    ///                          then the old value is preserved.
    void            Set( SPResourcePool& pool,  const NCString& resourceCategory= nullptr ) {
        resourcePool= pool;
        if ( resourceCategory.IsNotNull() )
        ResourceCategory= resourceCategory;
    }

    /// Determines if a resource pool is set.
    /// @return \c true if the field #".resourcePool" is not \e nulled, \c false otherwise.
    bool            HasPool()                              { return resourcePool.Get() != nullptr; }

    /// Shortcut method that invokes #"ResourcePool::Bootstrap;*" on field
    /// #".resourcePool" providing field #".ResourceCategory" as a parameter.
    /// @param name  The resource name.
    /// @param data  The resource data string.
    inline
    void            BootstrapResource( const NString& name, const String& data ) {
        ResourceHolder::resourcePool->Bootstrap( ResourceHolder::ResourceCategory, name, data );
    }

    /// Returns a reference (!) to the shared pointer holding the resource pool.
    /// @return The resource pool instance.
    SPResourcePool& GetResourcePoolSP()                                     { return resourcePool; }

    /// Returns a reference (not the shared-pointer!) to the resource pool held in our
    /// #"%SharedPtr".
    /// @return The resource pool instance.
    ResourcePool&   GetResourcePool()                                      { return *resourcePool; }

    /// Shortcut method that invokes #"ResourcePool::Get;*" on field
    /// #".resourcePool" providing field #".ResourceCategory" as parameter.
    ///
    /// With debug-builds, this method asserts that a resource was found. If this is not
    /// wanted, use #".TryResource".
    /// @param name  The resource name.
    /// @return The resource string, respectively a \e nulled string on failure.
    inline
    const String&   GetResource( const NString& name ) {
        return resourcePool->Get( ResourceCategory, name  ALIB_DBG(,  true)  );
    }

    /// Shortcut method that invokes #"ResourcePool::Get;*" on field
    /// #".resourcePool" providing field #".ResourceCategory" as parameter.
    ///
    /// \note
    ///   Usually, it is recommended to use #".GetResource", which asserts with debug-builds
    ///   if a resource was not found.
    ///
    /// @param name  The resource name.
    /// @return The resource string, respectively a \e nulled string on failure.
    inline
    const String&   TryResource( const NString& name ) {
        return resourcePool->Get( ResourceCategory, name  ALIB_DBG(, false)  );
    }
};

//==================================================================================================
/// A simple type trait that associates resource information to the given type \p{T} .
///
/// Extends <c>std::false_type</c> by default to indicate that it is not specialized for a specific
/// type. Specializations need to extend <c>std::true_type</c> instead.
///
/// \see
/// - Helper macros #"ALIB_RESOURCED" and ALIB_RESOURCED_IN_CAMP that specialize this struct.
/// - Helper-type #"ResourcedType".
/// - Manual chapter #"alib_resources_t_resourced"
///   of the Programmer's Manual of this module.
///
/// @tparam T   The type to define resource information for.
//==================================================================================================
template<typename T>
struct ResourcedTraits : std::false_type {
    /// Returns a pointer to the resource pool associated with \p{T}.
    /// @return The resource pool of \p{T}.
    static constexpr  ResourcePool* Pool()                                       { return nullptr; }

    /// Returns a resource category associated with \p{T}.
    /// @return The resource category.
    static constexpr  NString       Category()                              { return NULL_NSTRING; }

    /// Returns a resource name associated with \p{T}.
    /// @return The resource category.
    static constexpr  NString       Name()                                  { return NULL_NSTRING; }
};

ALIB_ALLOW_DOCS

/// A concept to identify whether resources are associated with type \p{T}.
/// These are types for which a specialization of type trait #"ResourcedTraits"
/// is defined.
/// @tparam T   The type to be tested.
template <typename T>
concept HasResources =  ResourcedTraits<T>::value;

ALIB_POP_ALLOWANCE

//==================================================================================================
/// Static helper-struct used to access resources of types that dispose of a specialization of
/// the type trait #"ResourcedTraits".
///
/// @see
///   - Type trait #"ResourcedTraits"
///   - Manual chapter #"alib_resources_t_resourced_resourced" of the
///    Programmer's Manual of this module.
///
/// @tparam T  A type equipped with resource information by a specialization of
///            #"ResourcedTraits".
//==================================================================================================
template<typename T>
struct ResourcedType {
    /// Static method that receives a resource string for a type which has a specialization
    /// of #"ResourcedTraits" defined.
    ///
    /// @tparam TRequires Not to be specified.
    ///                   Used by the compiler to select the availability of this method.
    /// @return The externalized resource string.
    template<typename TRequires= T>
    requires alib::resources::HasResources<TRequires>
    static const String& Get() {
        return ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(),
                                                ResourcedTraits<T>::Name    ()
                                                ALIB_DBG(, true) );
    }

    #if DOXYGEN
    /// Variant of the parameterless version #"ResourcedType::Get()" that
    /// ignores the resource name given for a type with a specialization of
    /// #"ResourcedTraits", but instead uses the name provided.
    ///
    /// @tparam TRequires   Not to be specified. Used by the compiler to select the availability
    ///                     of this method.
    /// @param name         The resource name to use, given as string of narrow character width.
    /// @param dbgAssert    This parameter is available (and to be passed) only in debug mode.
    ///                     If \c true, an #"alib_mod_assert;error is raised" if the resource
    ///                     was not found.
    ///                     Use the macro ALIB_DBG with calls to this method.
    /// @return The externalized resource string.
    template<typename TRequires= T>
    requires alib::resources::HasResources<TRequires>
    static const String&    Get( const NString& name, bool dbgAssert );
    #else
        template<typename TRequires= T>
        requires alib::resources::HasResources<TRequires>
    static const String&    Get( const NString& name  ALIB_DBG(, bool dbgAssert) )  {
        return ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(),
                                                name
                                                ALIB_DBG(, dbgAssert) );
    }
    #endif

    #if DOXYGEN
    /// Variant of the method #"ResourcedType::Get(const NString&; bool)" that
    /// accepts a character string of standard character width instead of a narrow type.
    ///
    /// \par Availability
    ///   Available only if #"ALIB_CHARACTERS_WIDE" evaluates to \c true.
    ///
    /// @tparam TRequires   Not to be specified. Used by the compiler to select the availability
    ///                     of this method.
    /// @param resourceName The resource name to use, given as a string of standard character width.
    /// @param dbgAssert    This parameter is available (and to be passed) only in debug mode.
    ///                     If \c true, an #"alib_mod_assert;error is raised" if the resource
    ///                     was not found.
    /// @return The externalized resource string.
    template<typename TRequires= T>
    requires alib::resources::HasResources<TRequires>
    static const String&    Get( const String& resourceName, bool dbgAssert );
    #else
        #if ALIB_CHARACTERS_WIDE
        template<typename TRequires= T>
        requires alib::resources::HasResources<TRequires>
        static const String&    Get( const String& resourceName    ALIB_DBG(, bool dbgAssert) ) {
            return ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(),
                                                    resourceName
                                                    ALIB_DBG(, dbgAssert) );
        }
        #endif
    #endif

    /// Together with sibling method #".TypeNamePostfix", this method may be used to receive the
    /// first portion of a type's human-readable name.
    ///
    /// The method tries to standardize resourcing names of C++ types along with the resource string
    /// that is defined with the type trait #"ResourcedTraits" for a type.
    ///
    /// The prefix is tried to be retrieved by extending the resource name returned by the method
    /// #"ResourcedTraits::Name;*" by character <c>'<'</c>.
    ///
    /// \alib uses this method internally, for example, with specializations
    /// #"AppendableTraits<TEnum,TChar,TAllocator>"
    /// #"AppendableTraits<TBitwiseEnum,TChar,TAllocator>"
    /// used to write element names of enum types.
    ///
    /// If either #"ResourcedTraits" is \e not specialized for \p{TEnum},
    /// or a resource named \"\p{name}<b>\></b>\" is not found, an empty string is returned.<br>
    ///
    /// @return The prefix string.
    static const String&    TypeNamePrefix() {
        if constexpr( HasResources<T> ) {
            NString256 resourceName( ResourcedTraits<T>::Name() );
                       resourceName << "<";
            auto*       pool=      ResourcedTraits<T>::Pool();
            const auto& category=  ResourcedTraits<T>::Category();
            auto& pf= pool->Get( category, resourceName ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }

        return EMPTY_STRING;
    }

    /// Same as #".TypeNamePrefix" but for the postfix string of a types name.
    /// Consequently, extends the resource string's name searched by character <c>'>'</c>.
    ///
    /// @return The postfix string.
    static const String&    TypeNamePostfix() {
        ALIB_ALLOW_NULL_POINTER_PASSING
        if constexpr( HasResources<T> ) {
            NString256 resourceName( ResourcedTraits<T>::Name() );
                       resourceName << ">";
            auto& pf= ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(), resourceName
                                                    ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }
        ALIB_POP_ALLOWANCE

        return EMPTY_STRING;
    }

}; // struct ResourcedType

/// Utility type that may be used to store resourcing information.
///
/// Besides constructor #"ResourceInfo(ResourcePool*, NString, NString)" and corresponding
/// #".Set(resources::ResourcePool*)" method, templated alternatives exist, which are applicable
/// if #"ResourcedTraits" is specialized for the template type.
struct ResourceInfo {
    /// The resource pool.
    ResourcePool*   Pool;

    /// The resource category within #".Pool".
    NString         Category;

    /// The resource category within #".Pool".
    NString         Name;

    /// Defaulted constructor leaving the fields uninitialized.
    ResourceInfo()                                                                noexcept =default;

    /// Constructor setting the fields of this object as given.
    ///
    /// @param pool      The resource pool.
    /// @param category  The resource category.
    /// @param name      The resource name.
    template<typename T>
    ResourceInfo( ResourcePool* pool, NString category, NString name )
    : Pool    (pool    )
    , Category(category)
    , Name    (name    )                                                                          {}

    /// Templated constructor which sets the fields of this object according to the values provided
    /// with a specialization of #"ResourcedTraits" for type \p{T}.
    ///
    /// @tparam T     Type that disposes of a specialization of #"%ResourcedTraits".
    ///               Deduced by the compiler
    /// @param sample A sample instance of type \p{T}. Exclusively used to have the compiler
    ///               deduce type \p{T} (otherwise ignored).
    template<typename T>
    ResourceInfo(const T& sample)                                                 { Set( sample ); }

    /// Sets the fields of this object as given.
    /// @param pool     The resource pool.
    /// @param category The resource category.
    /// @param name     The resource name.
    void    Set( resources::ResourcePool* pool, NString category, NString name ) {
        Pool =  pool;
        Category  =  category;
        Name      =  name;
    }

    /// Sets the fields of this object according to the values provided with a specialization of
    /// #"ResourcedTraits" for type \p{T}.
    /// @tparam T     Type that disposes of a specialization of #"%ResourcedTraits".
    ///               Deduced by the compiler
    /// @param sample A sample instance of type \p{T}. Exclusively used to have the compiler
    ///               deduce type \p{T} (otherwise ignored).
    template<typename T>
    requires alib::resources::HasResources<T>
    void Set(const T& sample) {
        (void) sample;
        Pool    =  ResourcedTraits<T>::Pool();
        Category=  ResourcedTraits<T>::Category();
        Name    =  ResourcedTraits<T>::Name();
    }

    /// Receives the resource string according to this info object.
    /// @return The externalized resource string.
    const String&   Get()                  { return Pool->Get( Category, Name  ALIB_DBG(, true) ); }


    #if DOXYGEN
    /// Variant of parameterless version #".Get"  that ignores field #".Name" and instead uses given
    /// argument \p{name} .
    ///
    /// @param name       The resource name to use, given as string of narrow character width.
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an #"alib_mod_assert;error is raised" if the resource
    ///                   was not found.
    /// @return The externalized resource string.
    inline
    const String&   Get( const NString& name, bool dbgAssert );
    #else
    const String&   Get( const NString& name  ALIB_DBG(, bool dbgAssert) )
    { return Pool->Get( Category, name    ALIB_DBG(, dbgAssert) );  }
    #endif


    #if  DOXYGEN
        /// Variant of mehtod  Get(const NString&, bool) that accepts a character string of standard
        /// character width instead of a narrow type.
        ///
        /// \par Availability
        ///   Available only if #"ALIB_CHARACTERS_WIDE" evaluates to \c true.
        ///
        /// @param name        The resource name to use, given as string of standard character width.
        /// @param dbgAssert   This parameter is available (and to be passed) only in debug mode.
        ///                    If \c true, an #"alib_mod_assert;error is raised" if the
        ///                    resource was not found.
        /// @return The externalized resource string.
        inline
        const String&   Get( const String& name, bool dbgAssert );
    #else
        #if ALIB_CHARACTERS_WIDE
            const String&   Get( const String& name  ALIB_DBG(, bool dbgAssert) )
            { return Pool->Get( Category, name   ALIB_DBG(, dbgAssert) );  }
        #endif
    #endif
}; // ResourceInfo


} // namespace alib[::resources]

/// Type alias in namespace #"%alib".
using     ResourcePool=     resources::ResourcePool;

/// Type alias in namespace #"%alib".
template<typename T>
using     ResourcedType=    resources::ResourcedType<T>;

/// Type alias in namespace #"%alib".
using     ResourceInfo=     resources::ResourceInfo;

}  // namespace [alib]


#if ALIB_ENUMRECORDS
//==================================================================================================
//==== enumrecords::Bootstrap() functions
//==================================================================================================
ALIB_EXPORT  namespace alib::enumrecords::bootstrap {
#include "ALib.Lang.CIFunctions.H"

/// Reads a list of enum data records from an (externalized) resource string.
///
/// It is possible to provide the record data in two ways:
/// - In one resource string: In this case, the parameter \p{outerDelim} has to specify
///   the delimiter that separates the records.
/// - In an array of resource strings: If the resource string as given is not defined, this
///   method appends an integral index starting with \c 0 to the resource name, parses
///   a single record and increments the index.
///   Parsing ends when a resource with the next higher index is not found.
///
/// The second option is recommended for larger enum sets. While the separation causes
/// some overhead in a resource backend, the external (!) management (translation,
/// manipulation, etc.) is most probably simplified with this approach.
///
/// \par Availability
///   This namespace function is available only if \alib_enumrecords is included in the \alibbuild.
///
/// \see Chapter #"alib_enums_records_resourced" for a sample of how this method
///      can be invoked.
///
/// @tparam TEnum      The enumeration type to load resourced records for.
/// @param  pool       The resource pool to receive the string to parse the records from.
/// @param  category   The resource category of the externalized string.
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
requires alib::enumrecords::HasRecords<TEnum>
void Bootstrap( resources::ResourcePool& pool,
                const NString&           category,
                const NString&           name,
                character                innerDelim= ',',
                character                outerDelim= ','     ) {
    // resources given in the standard, non-indexed way?
    String input= pool.Get( category, name  ALIB_DBG(, false) );
    if( input.IsNotNull() ) {
        // Note:
        //  The parser is initialized here already. The "inner" call to Bootstrap() will not have
        //  the resource information otherwise.
        //  Double initialization is checked inside the parser's initialize method.
        //  (A little crude but OK!)
        EnumRecordParser::Initialize(input, innerDelim, outerDelim, category, name );
        Bootstrap<TEnum>( input, innerDelim, outerDelim );
        return;
    }

    // resources given as name0, name1, name2...
    NString64 nameNr( name);
    int nr= 0;
    auto&  records= detail::EnumRecordHook<TEnum>::GetSingleton();
    auto** lastP  = records.getPointerToLast();
    while(  (input= pool.Get( category, nameNr.Reset( name)._(nr) ALIB_DBG(, false))).IsNotNull()
           || nr== 0 )
    {
        EnumRecordParser::Initialize(input, innerDelim, outerDelim, category, nameNr );

        auto* element=  (*lastP=  monomem::GLOBAL_ALLOCATOR().New<typename detail::EnumRecordHook<TEnum>::Node>());

        EnumRecordParser::Get( element->integral );
        element->record.Parse();

        detail::setEnumRecord( typeid(TEnum), integer(element->integral), &element->record );

        EnumRecordParser::assertEndOfInput();
        // next
        lastP= &element->next;
        ++nr;
    }
    (*lastP)= nullptr;

    // check if there are more coming (a gap in numbered definition)
    #if ALIB_DEBUG
        for( int i= 0 ; i < 35 ; ++i ) {
            ++nr;
            if( pool.Get( category, nameNr.Reset( name)._( nr) ALIB_DBG(, false)).IsNotNull() ) {
                ALIB_ERROR( "ENUMS",
                 "Detected a \"gap\" in numbering of enum records for type <{}>: "
                 "From index {} to {}.\n  Resource: {}/{}",
                 &typeid(TEnum), nr - i - 1, nr - 1, category, name  )
        }   }
    #endif
}

/// This namespace function is available if the type trait #"ResourcedTraits"
/// is specialized for the enum type \p{TEnum}.<br>
/// Invokes
/// #"Bootstrap(resources::ResourcePool&, const NString&, const NString&, character, character)".
///
/// \par Availability
///   This method is available only if \alib_resources is included in the \alibbuild.
///
/// \see Chapter #"alib_enums_records_resourced_tresourced" of the Programmer's Manual
///      of this module.
///
/// @tparam TEnum      The enumeration type to load resourced records for.
/// @param innerDelim The delimiter used for separating the fields of a record.
///                   Defaults to <c>','</c>.
/// @param outerDelim The character delimiting enum records.
///                   Defaults to <c>','</c>.
template<typename TEnum>
requires alib::enumrecords::HasRecords<TEnum>
void Bootstrap( character innerDelim=',', character outerDelim= ',' ) {
    static_assert( resources::HasResources<TEnum>,
                   "No specialization for ResourcedTraits<TEnum> given. Method not applicable." );

    Bootstrap<TEnum>( *resources::ResourcedTraits<TEnum>::Pool(),
                       resources::ResourcedTraits<TEnum>::Category(),
                       resources::ResourcedTraits<TEnum>::Name    (),
                       innerDelim, outerDelim                         );
}
#include "ALib.Lang.CIMethods.H"
} // namespace [alib::enumrecords::bootstrap]
#endif // ALIB_ENUMRECORDS

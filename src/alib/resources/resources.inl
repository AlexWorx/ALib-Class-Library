//==================================================================================================
/// \file
/// This header-file is part of module \alib_resources of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace resources {
//==================================================================================================
/// This purely abstract class provides an interface to store and retrieve "resourced" string data
/// which are organized in a two-level key hierarchy named <em>"resource category"</em>
/// and <em>"resource name"</em>. The latter are of narrow string-type.
///
/// \see
///   For detailed documentation on when and how this interface is used, please consult the
///   \ref alib_mod_resources "Programmer's Manual" of this module \alib_resources_nl.
///
/// \see
///   Two built-in implementations of this pure abstract interface are provided with
///   \alib{resources;LocalResourcePool} and \alib{variables;ConfigResourcePool}.
///   Please consult their reference documentation for further details.
//==================================================================================================
class ResourcePool
{
  public:

    /// Virtual destructor.
    virtual ~ResourcePool()                                                                =default;

    /// Used to store a resource string.
    ///
    /// In the context of \alibmods, which usually are the only areas where instances of this
    /// type are available (used), this method must only be invoked during the process of
    /// \ref alib_mod_bs "bootstrapping" \alib (and corresponding custom modules).
    ///
    /// \attention
    ///   The life-cycle of the given string's buffers, have to survive this resource instance.
    ///   Usually the strings passed here are constant C++ string literals, residing in the data
    ///   segment of an executable
    ///
    /// \note
    ///   Usually, the method #Bootstrap should be preferred, which asserts in debug-compilations
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

    /// Simple inline method that invokes virtual method #BootstrapAddOrReplace.
    /// In debug-compilations, it is asserted that a resource with the given key did not exist
    /// already.
    ///
    /// The use of this method is preferred over a direct invocation of #BootstrapAddOrReplace.
    ///
    /// @param category   Category string of the resource to add.
    /// @param name       Name string of the resource.
    /// @param data       The resource data.
    inline
    void Bootstrap( const NString& category, const NString& name, const String& data ) {
        #if ALIB_DEBUG
            bool result=
        #endif
            BootstrapAddOrReplace(category, name, data);

        ALIB_ASSERT_ERROR( result, "RESOURCES",
            "Doubly defined resource \"{}\" in category \"{}\".",  category, name )
    }

    /// Same as #Bootstrap but accepts an array of name/value pairs to be filled into
    /// the given parameter \p{category}.
    ///
    /// \attention
    ///   <b>The given list has to be finished with a final \c nullptr argument for the next
    ///   name!</b>
    ///
    /// In the context of \alibmods, which usually are the only areas where instances of this
    /// type are available (used), this method must only invoked during the process of
    /// \ref alib_mod_bs "bootstrapping" \alib (and corresponding custom modules).
    ///
    /// \attention
    ///   The life-cycle of the given string's buffers, have to survive this resource instance.
    ///   Usually the strings passed here are constant C++ string literals, residing an the data
    ///   segment of an executable
    ///
    /// \note
    ///   The use of variadic C-style arguments <c>"..."</c> in general is \b not recommended
    ///   to be used.
    ///   We still do it here, because this method is usually used with implementations
    ///   of \alib{camp;Camp::Bootstrap} to load static default values.
    ///   This approach saves a lot of otherwise needed single invocations (reduces code size) and
    ///   allows a clean code for the init methods.<br>
    ///   For technical reasons, parameter \p{category } is declared as type <c>const nchar*</c>.
    ///
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
    ///   Usually resource pools are associated with \alib{camp;Camp} objects and resources should be
    ///   loaded using its "shortcut methods" \alib{camp::Camp;TryResource}
    ///   and \alib{camp::Camp;GetResource}.
    ///   If used directly, the argument \p{dbgAssert} has to be enclosed in macro \ref ALIB_DBG
    ///   (including the separating comma).
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an \ref alib_mod_assert "error is raised" if the resource
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
    /// Convenience inlined method that accepts parameter name as \alib{characters;character}
    /// instead of \alib{characters;nchar} based string-type. The rationale for this is that often,
    /// resource name keys are read from other resourced strings and need conversion if used.
    /// This avoids external conversion before invoking this method.
    ///
    /// This method is available only when \alib is compiled with type \alib{characters;character}
    /// not being equivalent to \alib{characters;nchar}.
    ///
    /// After the string conversion, this method simply returns the result of virtual method
    /// #Get(const NString&, const NString&, bool).
    ///
    /// @param category   Category string of the resource.
    /// @param name       Name string of the resource
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an \ref alib_mod_assert "error is raised" if the resource
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
        ///   Available only if the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set.
        ///
        /// \attention
        ///   This method is implemented only with the default pool instance of type
        ///   \alib{resources;LocalResourcePool}.
        ///   Other implementations raise an \alib_warning and return an empty vector.
        ///
        /// \see
        ///   Methods #DbgGetCategories and #DbgDump.
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
        virtual
        std::vector<std::tuple<NString, NString, String, integer>>  DbgGetList();

        //==========================================================================================
        /// Implements abstract method \alib{resources;ResourcePool::DbgGetCategories}.
        ///
        /// \par Availability
        ///   Available only if the compiler-symbol \ref ALIB_DEBUG_RESOURCES is set.
        ///
        /// \attention
        ///   This method is implemented only with the default pool instance of type
        ///   \alib{resources;LocalResourcePool}.
        ///   Other implementations raise an \alib_warning and return an empty vector.
        ///
        /// \see
        ///   Methods #DbgGetList and #DbgDump.
        ///
        /// @return The externalized resource string.
        //==========================================================================================
        ALIB_DLL
        virtual
        std::vector<std::pair<NString, integer>>                    DbgGetCategories();
    #endif //ALIB_DEBUG_RESOURCES
}; // class ResourcePool

//==================================================================================================
/// Simple the type trait that associates resource information to the given type \p{T} .
///
/// Extends <c>std::false_type</c> by default to indicate that it is not specialized for a specific
/// type. Specializations need to extend <c>std::true_type</c> instead.
///
/// \see
/// - Helper macros \ref ALIB_RESOURCED and ALIB_RESOURCED_IN_MODULE that specialize this struct.
/// - Helper-type \alib{resources;ResourcedType}.
/// - Manual chapter \ref alib_resources_t_resourced "3.5. Indirect Resource Access"
///   of the Programmer's Manual of this module.
///
/// @tparam T   The type to define resource information for.
//==================================================================================================
template<typename T>
struct ResourcedTraits : std::false_type
{
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

ALIB_WARNINGS_IGNORE_DOCS

/// A concept to identify whether resources are associated with type \p{T}.
/// These are types for which a specialization of type trait \alib{resources;ResourcedTraits}
/// is defined.
/// @tparam T   The type to be tested.
template <typename T>
concept HasResources =  ResourcedTraits<T>::value;

ALIB_WARNINGS_RESTORE

//==================================================================================================
/// Static helper-struct used to access resources of types that dispose of a specialization of
/// the type trait \alib{resources;ResourcedTraits}.
///
/// @see
///   - Type trait \alib{resources;ResourcedTraits}
///   - Manual chapter \ref alib_resources_t_resourced_resourced of the
///    Programmer's Manual of this module.
///
/// @tparam T  A type equipped with resource information by a specialization of
///            \alib{resources;ResourcedTraits}.
//==================================================================================================
template<typename T>
struct ResourcedType
{
    /// Static method that receives a resource string for a type which has a specialization
    /// of \alib{resources;ResourcedTraits} defined.
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
    /// Variant of the parameterless version \alib{resources::ResourcedType;Get();Get} that
    /// ignores the resource name given for a type with a specialization of
    /// \alib{resources;ResourcedTraits}, but instead uses the name provided.
    ///
    /// @tparam TRequires   Not to be specified. Used by the compiler to select the availability
    ///                     of this method.
    /// @param name         The resource name to use, given as string of narrow character width.
    /// @param dbgAssert    This parameter is available (and to be passed) only in debug mode.
    ///                     If \c true, an \ref alib_mod_assert "error is raised" if the resource
    ///                     was not found.
    ///                     Use macro ALIB_DBG with calls to this method.
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
    /// Variant of method \alib{resources::ResourcedType;Get(const NString&; bool)} that
    /// accepts a character string of standard character width instead of a narrow type.
    ///
    /// \par Availability
    ///   Available only if \ref ALIB_CHARACTERS_WIDE evaluates to \c true.
    ///
    /// @tparam TRequires   Not to be specified. Used by the compiler to select the availability
    ///                     of this method.
    /// @param resourceName The resource name to use, given as a string of standard character width.
    /// @param dbgAssert    This parameter is available (and to be passed) only in debug mode.
    ///                     If \c true, an \ref alib_mod_assert "error is raised" if the resource
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

    /// Together with sibling method #TypeNamePostfix, this method may be used to receive the
    /// first portion of a type's human-readable name.
    ///
    /// The method tries to standardize resourcing names of C++ types along with the resource string
    /// that is defined with the type trait \alib{resources;ResourcedTraits} for a type.
    ///
    /// The prefix is tried to be retrieved by extending the resource name returned by the method
    /// \alib{resources;ResourcedTraits::Name} by character <c>'<'</c>.
    ///
    /// \alib uses this method internally, for example, with specializations
    /// \alib{strings::APPENDABLES;AppendableTraits<TEnum,TChar,TAllocator>;AppendableTraits<TEnum,TChar,TAllocator>}
    /// \alib{strings::APPENDABLES;AppendableTraits<TBitwiseEnum,TChar,TAllocator>;AppendableTraits<TBitwiseEnum,TChar,TAllocator>}
    /// used to write element names of enum types.
    ///
    /// If either \alib{resources;ResourcedTraits} is \e not specialized for \p{TEnum},
    /// or a resource named \"\p{name}<b>\></b>\" is not found, an empty string is returned.<br>
    ///
    /// @return The prefix string.
    static const String&    TypeNamePrefix() {
        if constexpr( HasResources<T> ) {
            NString256 resourceName( ResourcedTraits<T>::Name() );
                       resourceName << "<";
            auto& pf= ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(), resourceName
                                                    ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }

        return EMPTY_STRING;
    }

    /// Same as #TypeNamePrefix but for the postfix string of a types name.
    /// Consequently, extends the resource string's name searched by character <c>'>'</c>.
    ///
    /// @return The postfix string.
    static const String&    TypeNamePostfix() {
        ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING
        if constexpr( HasResources<T> ) {
            NString256 resourceName( ResourcedTraits<T>::Name() );
                       resourceName << ">";
            auto& pf= ResourcedTraits<T>::Pool()->Get( ResourcedTraits<T>::Category(), resourceName
                                                    ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }
        ALIB_WARNINGS_RESTORE

        return EMPTY_STRING;
    }

}; // struct ResourcedType

/// Utility type that may be used to store resourcing information.
///
/// Besides constructor #ResourceInfo(ResourcePool*, NString, NString) and corresponding #Set method,
/// templated alternatives exist, which are applicable if \alib{resources;ResourcedTraits}
/// is specialized for the template type.
struct ResourceInfo
{
    /// The resource pool.
    ResourcePool*   Pool;

    /// The resource category within #Pool.
    NString         Category;

    /// The resource category within #Pool.
    NString         Name;

    /// Defaulted constructor leaving the fields uninitialized.
    ResourceInfo()                                                                noexcept =default;

    /// Constructor setting the fields of this object as given.
    ///
    /// @param pool      The resource pool.
    /// @param category  The resource category.
    /// @param name      The resource name.
    template<typename T>
    ResourceInfo( resources::ResourcePool* pool, NString category, NString name )
    : Pool    (pool    )
    , Category(category)
    , Name    (name    )                                                                          {}

    /// Templated constructor which sets the fields of this object according to the values provided
    /// with a specialization of \alib{resources;ResourcedTraits} for type \p{T}.
    ///
    /// @tparam T     Type that disposes of a specialization of \b ResourcedTraits.
    ///               Deduced by the compiler
    /// @param sample A sample instance of type \p{T}. Exclusively used to have the compiler
    ///               deduce type \p{T} (otherwise ignored).
    template<typename T>
    ResourceInfo(const T& sample)                                                 { Set( sample ); }

    /// Sets the fields of this object as given.
    ///
    /// @param pool     The resource pool.
    /// @param category The resource category.
    /// @param name     The resource name.
    void    Set( resources::ResourcePool* pool, NString category, NString name ) {
        Pool =  pool;
        Category  =  category;
        Name      =  name;
    }

    /// Sets the fields of this object according to the values provided with a specialization of
    /// \alib{resources;ResourcedTraits} for type \p{T}.
    ///
    /// @tparam T     Type that disposes of a specialization of \b ResourcedTraits.
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
    /// Variant of parameterless version #Get()  that ignores field #Name and instead uses given
    /// argument \p{name} .
    ///
    /// @param name       The resource name to use, given as string of narrow character width.
    /// @param dbgAssert  This parameter is available (and to be passed) only in debug mode.
    ///                   If \c true, an \ref alib_mod_assert "error is raised" if the resource
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
        ///   Available only if \ref ALIB_CHARACTERS_WIDE evaluates to \c true.
        ///
        /// @param name        The resource name to use, given as string of standard character width.
        /// @param dbgAssert   This parameter is available (and to be passed) only in debug mode.
        ///                    If \c true, an \ref alib_mod_assert "error is raised" if the
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

/// Type alias in namespace \b alib.
using     ResourcePool=     resources::ResourcePool;

/// Type alias in namespace \b alib.
template<typename T>
using     ResourcedType=    resources::ResourcedType<T>;

/// Type alias in namespace \b alib.
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
/// - In one resource string: In this case, parameter \p{outerDelim} has to specify
///   the delimiter that separates the records.
/// - In an array of resource strings: If the resource string as given is not defined, this
///   method appends an integral index starting with \c 0 to the resource name, parses
///   a single record and increments the index.
///   Parsing ends when a resource with a next higher index is not found.
///
/// The second option is recommended for larger enum sets. While the separation causes
/// some overhead in a resource backend, the external (!) management (translation,
/// manipulation, etc.) is most probably simplified with this approach.
///
/// \par Availability
///   This namespace function is available only if \alib_enumrecords is included in the \alibbuild.
///
/// \see Chapter \ref alib_enums_records_resourced for a sample of how this method
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

/// This namespace function is available if the type trait \alib{resources;ResourcedTraits}
/// is specialized for the enum type \p{TEnum}.<br>
/// Invokes #Bootstrap(ResourcePool&, const NString&, const NString&, character, character)
///
/// \par Availability
///   This method is available only if \alib_resources is included in the \alibbuild.
///
/// \see Chapter \ref alib_enums_records_resourced_tresourced of the Programmer's Manual
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

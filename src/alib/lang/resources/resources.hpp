/** ************************************************************************************************
 * \file
 * This header file is part of sub-namespace #alib::lang::resources of module \alib_basecamp of
 * the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_RESOURCES_RESOURCES
#define HPP_ALIB_LANG_RESOURCES_RESOURCES 1

#if !defined (HPP_ALIB_STRINGS_STRING)
#   include "alib/strings/string.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif


#if !defined(ALIB_RESOURCES_OMIT_DEFAULTS)
#   define   ALIB_RESOURCES_OMIT_DEFAULTS   0
#endif

#if ALIB_DEBUG_RESOURCES && !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#  include <vector>
#endif



namespace alib { namespace lang::resources {

/** ************************************************************************************************
 * This purely abstract class provides an interface to store and retrieve "resourced" string data
 * which are organized in a two-level key hierarchy named <em>"resource category"</em>
 * and <em>"resource name"</em>. The latter are of narrow string type.
 *
 * \see
 *  For detailed documentation on when and how this interface is used, please consult chapter
 *  \ref alib_basecamp_resources "3. Namespace alib::lang::resources" of the Programmer's Manual of
 *  module \alib_basecamp.
 *
 * \see
 *  Two built-in implementations of this pure abstract interface are provided with
 *  \alib{lang::resources,LocalResourcePool} and \alib{lang::resources,ConfigResourcePool}.
 *  Please consult their reference documentation for further details.
 *
 **************************************************************************************************/
class ResourcePool
{
    public:

    /** Virtual destructor. */
    virtual ~ResourcePool()
    {}


    /** ********************************************************************************************
     * Used to store a resource string.
     *
     * In the context of \alibmods, which usually are the only areas where instances of this
     * type are available (used), this method must only invoked during the process of
     * \ref alib_manual_bootstrapping "bootstrapping" \alib (and corresponding custom modules).
     *
     * \attention
     *   The life-cycle of the given string's buffers, have to survive this resource instance.
     *   Usually the strings passed here are constant C++ string literals, residing an the data
     *   segment of an executable
     *
     * \note
     *   Usually, method #Bootstrap should be preferred, which asserts in debug-compilations,
     *   if a resource already existed. The use of this method is for special cases, for example
     *   to replace (patch) resources of dependent modules.
     *
     * @param category   Category string of the resource to add.
     * @param name       Name string of the resource.
     * @param data       The resource data.
     * @return \c true if the resource did exist and was replaced, \c false if it was an insertion.
     **********************************************************************************************/
    virtual
    bool BootstrapAddOrReplace(const NString& category, const NString& name, const String& data)= 0;

    /** ********************************************************************************************
     * Simple inline method that invokes virtual method #BootstrapAddOrReplace.
     * In debug-compilations, it is asserted that a resource with the given key did not exist
     * already.
     *
     * The use of this method is preferred over a direct invocation of #BootstrapAddOrReplace.
     *
     * @param category   Category string of the resource to add.
     * @param name       Name string of the resource.
     * @param data       The resource data.
     **********************************************************************************************/
    inline
    void Bootstrap( const NString& category, const NString& name, const String& data )
    {
        #if ALIB_DEBUG
            bool result=
        #endif
            BootstrapAddOrReplace(category, name, data);

        ALIB_ASSERT_ERROR( result, "RESOURCES",
                           NString256( "Doubly defined resource \"" ) << name
                                   <<  "\" in category: "             << category     )
    }

    /** ********************************************************************************************
     * Same as #Bootstrap but accepts an array of name/value pairs to be filled into
     * the given parameter \p{category}.
     *
     * \attention
     *   <b>The given list has to be finished with a final \c nullptr argument for the next
     *   name!</b>
     *
     * In the context of \alibmods, which usually are the only areas where instances of this
     * type are available (used), this method must only invoked during the process of
     * \ref alib_manual_bootstrapping "bootstrapping" \alib (and corresponding custom modules).
     *
     * \attention
     *   The life-cycle of the given string's buffers, have to survive this resource instance.
     *   Usually the strings passed here are constant C++ string literals, residing an the data
     *   segment of an executable

     * \note
     *   The use of variadic C-style arguments <c>"..."</c> in general is \b not recommended
     *   to be used.
     *   We still do it here, because this method is usually used with implementations
     *   of \alib{lang,Camp::bootstrap} to load static default values.
     *   This approach saves a lot of otherwise needed single invocations (reduces code size) and
     *   allows a clean code for the init methods.<br>
     *   For technical reasons, parameter \p{category } is declared as type <c>const nchar*</c>.
     *
     *
     * @param category  The category of the resources given.
     * @param ...       A list of pairs of <b>const nchar*</b> and <b>const character*</b>
     *                  keys and data, including a terminating \c nullptr value.
     **********************************************************************************************/
    virtual
    void BootstrapBulk( const nchar* category, ... )                                            = 0;

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Returns a resource.
     * On failure (resource not found), a \e nulled string is returned.
     *
     * \note
     *   Usually resource pools are associated with \alib{lang,Camp} objects and resources should be
     *   loaded using its "shortcut methods" \alib{lang::Camp,TryResource}
     *   and \alib{lang::Camp,GetResource}.
     *   If used directly, argument \p{dbgAssert} has to be enclosed in macro \ref ALIB_DBG
     *   (including the separting comma).
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was not found.
     * @return The resource string, respectively a \e nulled string on failure.
     **********************************************************************************************/
    virtual
    const String&   Get( const NString& category, const NString& name, bool dbgAssert )         = 0;
#else
    virtual
    const String&   Get( const NString& category, const NString& name ALIB_DBG(,bool dbgAssert))= 0;
#endif

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Convenience inlined method that accepts parameter name as \alib{characters,character}
     * instead of \alib{characters,nchar} based string type. The rationale for this is that often,
     * resource name keys are read from other resourced strings and need conversion if used.
     * This avoids external conversion prior to invoking this method.
     *
     * This method is available only when \alib is compiled with type \alib{characters,character}
     * not being equivalent to \alib{characters,nchar}.
     *
     * After string conversion simply returns result of virtual method
     * #Get(const NString&, const NString&, bool).
     *
     * @param category   Category string of the resource.
     * @param name       Name string of the resource
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was not found.
     * @return The resource string, respectively a \e nulled string on failure.
     **********************************************************************************************/
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
        /** ****************************************************************************************
         * Returns a vector of tuples for each resourced element. Each tuple contains:
         * 0. The category name
         * 1. The resource name
         * 2. The resource value
         * 3. The number of requests for the resource performed by a using data.
         *
         * While being useful to generaly inspect the resources, a high number of requests
         * might indicate a performance penality for a using software. Such can usually be
         * mitigated in a very simple fashion by "caching" a resource string in a local
         * or global/static string variable.
         *
         * ### Availability ###
         * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set.
         * \attention
         *   This method is implemented only with the default pool instance of type
         *   \alib{lang::resources,LocalResourcePool} is used. Otherwise, an \alib warning is raised and
         *   an empty vector is returned.
         *
         * \see
         *   Methods #DbgGetCategories and #DbgDump.
         *
         * @return The externalized resource string.
         ******************************************************************************************/
        ALIB_API
        virtual
        std::vector<std::tuple<NString, NString, String, integer>>
        DbgGetList();

        /** ****************************************************************************************
         * Implements abstract method \alib{lang::resources,ResourcePool::DbgGetCategories}.
         *
         * ### Availability ###
         * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set.
         * \attention
         *   This method is implemented only with the default pool instance of type
         *   \alib{lang::resources,LocalResourcePool} is used. Otherwise, an \alib warning is raised and
         *   an empty vector is returned.
         *
         * \see
         *   Methods #DbgGetList and #DbgDump.
         *
         * @return The externalized resource string.
         ******************************************************************************************/
        ALIB_API
        virtual
        std::vector<std::pair<NString, integer>>
        DbgGetCategories();

        #if ALIB_CAMP
            /** ************************************************************************************
             * Writes the list of resources obtainable with #DbgGetList to an \b %AString.
             *
             * ### Availability ###
             * Available only if compiler symbol \ref ALIB_DEBUG_RESOURCES is set and furthermore
             * if module \alib_basecamp is included in the \alibdist.
             *
             * \see
             *   Methods #DbgGetList and #DbgGetCategories.
             *
             * @param list       The list of resources, obtained with #DbgGetList.
             * @param catFilter  Comma-separated list of names of categories to print.
             *                   Defaults to nulled string, which includes all caegories.
             * @param format     The format of a line.
             *                   Defaults to <b>"({3:}) {1}={2!TAB20!ESC<!Q}\\n"</b>.
             * @return The dump of all resources.
             **************************************************************************************/
            ALIB_API
            static
            AString DbgDump( std::vector<std::tuple<NString, NString, String, integer>>& list,
                             const NString& catFilter = nullptr,
                             const String&  format    = A_CHAR("({3:}) {1}={2!TAB20!ESC<!Q}\n") );
        #endif //ALIB_CAMP
    #endif
}; // class ResourcePool

/** ************************************************************************************************
 * Simple TMP struct that associates resource information to given type \p{T} .
 *
 * Extends <c>std::false_type</c> by default to indicate that it is not specialized for a specific
 * type. Specializations need to extend <c>std::true_type</c> instead.
 *
 * \see
 *  - Helper macros \ref ALIB_RESOURCED and ALIB_RESOURCED_IN_MODULE that specialize this struct.
 *  - Helper type \alib{lang::resources,ResourcedType}.
 *  - Manual chapter \ref alib_basecamp_resources_t_resourced "3.5. Indirect Resource Access" of the
 *    Programmer's Manual of this module.
 *
 * @tparam T   The type to define resource information for.
 **************************************************************************************************/
template<typename T>
struct T_Resourced : public std::false_type
{
    /**
     *  Returns a pointer to the resource pool associated with \p{T}.
     *  @return The resource pool of \p{T}.
     */
    static constexpr  ResourcePool* Pool()     { return nullptr;       }

    /**
     *  Returns a resource category associated with \p{T}.
     *  @return The resource category.
     */
    static constexpr  NString       Category() { return NullNString(); }

    /**
     *  Returns a resource name associated with \p{T}.
     *  @return The resource category.
     */
    static constexpr  NString       Name()     { return NullNString(); }
};

/** ************************************************************************************************
 * Static helper struct used to access resources of types that dispose about a specialization of
 * type-traits struct \alib{lang::resources,T_Resourced}.
 *
 * @see
 *  - Type-traits struct \alib{lang::resources,T_Resourced}
 *  - Manual chapter \ref alib_basecamp_resources_t_resourced_resourced of the Programmer's Manual of this
 *    module.
 *
 * @tparam T  A type equipped with resource information by a specialization of
 *            \alib{lang::resources,T_Resourced}.
 **************************************************************************************************/
template<typename T>
struct ResourcedType
{
    #if defined(ALIB_DOX)
    /**
     * Static methodthat receives a resource string for a type which has a specialization
     * of \alib{lang::resources,T_Resourced} defined.
     *
     * @tparam TEnableIf   Not to be specified. Used by the compiler to select the availability
     *                     of this method.
     * @return The externalized resource string.
     */
    template<typename TEnableIf= T>
    static inline
    const String&   Get();
    #else
    template<typename TEnableIf= T>
    static
    ATMP_T_IF( const String&, T_Resourced<TEnableIf>::value )
    Get()
    {
        return T_Resourced<T>::Pool()->Get( T_Resourced<T>::Category(),
                                            T_Resourced<T>::Name    ()    ALIB_DBG(, true) );
    }
    #endif


    #if defined(ALIB_DOX)
    /**
     * Variant of parameterless version  \alib{lang::resources::ResourcedType,Get(),Get} that ignores the
     * resource name given for a type with \alib{lang::resources,T_Resourced}, but instead uses the given
     * name.
     *
     * @tparam TEnableIf   Not to be specified. Used by the compiler to select the availability
     *                     of this method.
     * @param name         The resource name to use, given as string of narrow character width.
     * @param dbgAssert    This parameter is available only in debug mode. If \c true, an assertion
     *                     is raised if the resource was not found.
     * @return The externalized resource string.
     */
    template<typename TEnableIf= T>
    static inline
    const String&   Get( const NString& name, bool dbgAssert );
    #else
    template<typename TEnableIf= T>
    static
    ATMP_T_IF( const String&, T_Resourced<TEnableIf>::value )
    Get( const NString& name    ALIB_DBG(, bool dbgAssert) )
    {
        return T_Resourced<T>::Pool()->Get( T_Resourced<T>::Category(), name  ALIB_DBG(, dbgAssert) );
    }
    #endif


    #if ALIB_CHARACTERS_WIDE && defined(ALIB_DOX)
    /**
     * Variant of method \alib{lang::resources::ResourcedType,Get(const NString&, bool)} that
     * accepts a character string of standard character width instead of a narrow type.
     *
     * ### Availability ###
     * Available only if \ref ALIB_CHARACTERS_WIDE evaluates to \c true.
     *
     * @tparam TEnableIf  Not to be specified. Used by the compiler to select the availability
     *                    of this method.
     * @param name        The resource name to use, given as string of standard character width.
     * @param dbgAssert   This parameter is available only in debug mode. If \c true, an assertion
     *                    is raised if the resource was not found.
     * @return The externalized resource string.
     */
    template<typename TEnableIf= T>
    static inline
    const String&   Get( const String& name, bool dbgAssert );
    #endif
    #if ALIB_CHARACTERS_WIDE && !defined(ALIB_DOX)
    template<typename TEnableIf= T>
    static
    ATMP_T_IF( const String&, T_Resourced<TEnableIf>::value )
    Get( const String& resourceName    ALIB_DBG(, bool dbgAssert) )
    {
        return T_Resourced<T>::Pool()->Get( T_Resourced<T>::Category(),
                                            resourceName     ALIB_DBG(, dbgAssert) );
    }
    #endif

    /**
     * Together with sibling method #TypeNamePostfix, this method may be used to receive the
     * first portion of a type's human readable name.
     *
     * The method tries to standardize resourcing names of C++ types along with the resource string
     * that is defined with type-traits struct \alib{lang::resources,T_Resourced} for a type.
     *
     * The prefix is tried to be retrieved by extending the resource name returned by method
     * \alib{lang::resources,T_Resourced::Name} by character <c>'<'</c>.
     *
     * \alib uses this method internally, for example with specializations
     * \alib{strings::APPENDABLES,T_Append<TEnum\,TChar>,T_Append<TEnum\,TChar>}
     * \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}
     * used to write element names of enum types.
     *
     * If either \alib{lang::resources,T_Resourced} is \e not specialized for \p{TEnum}, or a resource
     * named \"\p{name}<b>\></b>\" is not found, an empty string is returned.<br>
     *
     * @return The prefix string.
     */
    static
    const String&       TypeNamePrefix()
    {
        if constexpr( T_Resourced<T>::value )
        {
            NString256 resourceName( T_Resourced<T>::Name() );
                       resourceName << "<";
            auto& pf= T_Resourced<T>::Pool()->Get( T_Resourced<T>::Category(), resourceName
                                                    ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }

        return EMPTY_STRING;
    }

    /**
     * Same as #TypeNamePrefix but for the postfix string of a types name. Consequently, extends the
     * resource string's name searched by character character <c>'>'</c>.
     *
     * @return The postfix string.
     */
    static
    const String&       TypeNamePostfix()
    {
        ALIB_WARNINGS_ALLOW_NULL_POINTER_PASSING
        if constexpr( T_Resourced<T>::value )
        {
            NString256 resourceName( T_Resourced<T>::Name() );
                       resourceName << ">";
            auto& pf= T_Resourced<T>::Pool()->Get( T_Resourced<T>::Category(), resourceName
                                                    ALIB_DBG(, false) );
            if( pf.IsNotNull() )
                return pf;
        }
        ALIB_WARNINGS_RESTORE

        return EMPTY_STRING;
    }

}; // struct  ResourcedType

/**
 * Utility type that may be used to store resourcing information.
 *
 * Besides constructor #ResourceInfo(ResourcePool*, NString, NString) and corresponding #Set method,
 * templated alternatives exist, which are applicable if \alib{lang::resources,T_Resourced}
 * is specialized for the template type.
 */
struct ResourceInfo
{
    /** The resource pool. */
    resources::ResourcePool* Pool;

    /** The resource category within #Pool. */
    NString                  Category;

    /** The resource category within #Pool. */
    NString                  Name;

    /** Defaulted constructor leaving the fields uninitialized.  */
    ResourceInfo()                                                              noexcept  = default;

    /**
     * Constructor setting the fields of this object as given.
     *
     * @param pool      The resource pool.
     * @param category  The resource category.
     * @param name      The resource name.
     */
    template<typename T>
    ResourceInfo( resources::ResourcePool* pool, NString category, NString name )
    : Pool    (pool    )
    , Category(category)
    , Name    (name    )
    {}

    /**
     * Templated constructor which sets the fields of this object according to the values provided
     * with a specialization of \alib{lang::resources,T_Resourced} for type \p{T}.
     *
     * @tparam T     Type that disposes about a specialization of \b T_Resourced. Deduced by the
     *               compiler
     * @param sample A sample instance of type \p{T}. Exclusively used to have the compiler
     *               deduce type \p{T} (otherwise ignored).
     */
    template<typename T>
    ResourceInfo(const T& sample)
    {
        Set( sample );
    }

    /**
     * Sets the fields of this object as given.
     *
     * @param pool     The resource pool.
     * @param category The resource category.
     * @param name     The resource name.
     */
    void    Set( resources::ResourcePool* pool, NString category, NString name )
    {
        Pool =  pool;
        Category  =  category;
        Name      =  name;
    }

    #if defined(ALIB_DOX)
    /**
     * Sets the fields of this object according to the values provided with a specialization of
     * \alib{lang::resources,T_Resourced} for type \p{T}.
     *
     * @tparam T     Type that disposes about a specialization of \b T_Resourced. Deduced by the
     *               compiler
     * @param sample A sample instance of type \p{T}. Exclusively used to have the compiler
     *               deduce type \p{T} (otherwise ignored).
     */
    template<typename T>
    void    Set(const T& sample)
    #else
    template<typename T>
    ATMP_VOID_IF( T_Resourced<T>::value )
    Set(const T& )
    {
        Pool    =  T_Resourced<T>::Pool();
        Category=  T_Resourced<T>::Category();
        Name    =  T_Resourced<T>::Name();
    }
    #endif

    /**
     * Receives the resource string according to this info object.
     *
     * @return The externalized resource string.
     */
    inline
    const String&   Get()
    {
        return Pool->Get( Category, Name    ALIB_DBG(, true) );
    }


    #if defined(ALIB_DOX)
    /**
     * Variant of parameterless version #Get()  that ignores field #Name and instead uses given
     * argument \p{name} .
     *
     * @param name       The resource name to use, given as string of narrow character width.
     * @param dbgAssert  This parameter is available only in debug mode. If \c true, an assertion
     *                   is raised if the resource was not found.
     * @return The externalized resource string.
     */
    inline
    const String&   Get( const NString& name, bool dbgAssert );
    #else
    const String&   Get( const NString& name  ALIB_DBG(, bool dbgAssert) )
    {
        return Pool->Get( Category, name    ALIB_DBG(, dbgAssert) );
    }
    #endif


    #if ALIB_CHARACTERS_WIDE && defined(ALIB_DOX)
        /**
         * Variant of mehtod  Get(const NString&, bool) that accepts a character string of standard
         * character width instead of a narrow type.
         *
         * ### Availability ###
         * Available only if \ref ALIB_CHARACTERS_WIDE evaluates to \c true.
         *
         * @param name        The resource name to use, given as string of standard character width.
         * @param dbgAssert   This parameter is available only in debug mode. If \c true, an assertion
         *                    is raised if the resource was not found.
         * @return The externalized resource string.
         */
        inline
        const String&   Get( const String& name, bool dbgAssert );
    #endif
    #if ALIB_CHARACTERS_WIDE && !defined(ALIB_DOX)
        const String&   Get( const String& name  ALIB_DBG(, bool dbgAssert) )
        {
            return Pool->Get( Category, name   ALIB_DBG(, dbgAssert) );
        }
    #endif
}; // ResourceInfo


} // namespace alib[::lang::resources]

/// Type alias in namespace \b alib.
using     ResourcePool=     lang::resources::ResourcePool;

/// Type alias in namespace \b alib.
template<typename T>
using   T_Resourced=        lang::resources::T_Resourced<T>;

/// Type alias in namespace \b alib.
template<typename T>
using     ResourcedType=    lang::resources::ResourcedType<T>;

/// Type alias in namespace \b alib.
using     ResourceInfo=     lang::resources::ResourceInfo;

}  // namespace [alib]

// #################################################################################################
// T_Resourced Macro
// #################################################################################################
#define  ALIB_RESOURCED( T, ResPool, ResCategory, ResName )                                        \
namespace alib::lang::resources {                                                                  \
template<> struct T_Resourced<T>  : public std::true_type                                          \
{                                                                                                  \
    static            ResourcePool* Pool()        { return  ResPool;     }                         \
    static constexpr  NString       Category()    { return  ResCategory; }                         \
    static constexpr  NString       Name()        { return  ResName;     }                         \
};}

#if ALIB_CAMP
#   define ALIB_RESOURCED_IN_MODULE( T, Camp, ResName )                                      \
        ALIB_RESOURCED( T, &Camp.GetResourcePool(), Camp.ResourceCategory, ResName  )
#endif


#endif // HPP_ALIB_LANG_RESOURCES_RESOURCES

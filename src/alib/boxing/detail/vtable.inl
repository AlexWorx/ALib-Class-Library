/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_boxing of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BOXING_DETAIL_VTABLE
#define HPP_ALIB_BOXING_DETAIL_VTABLE 1

#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace boxing {

/**
 * This namespace implements internals of namespace #aworx::lib::boxing.
 */
namespace detail {

/** ************************************************************************************************
 * This struct is used internally with \alib_boxing_nl  to provide run-time type information
 * and virtual method invocations to boxed values.
 * A singleton instance of a derived type of this class is attached to each \alib{boxing,Box}.
 * Such derived type is generated using templated derived struct \alib{boxing::detail,VTable}.
 **************************************************************************************************/
struct FunctionTable
{
    // #############################################################################################
    // Fields
    // #############################################################################################
    FHashcode         ::Signature    fHashcode = nullptr;  ///< Entry for built-in function \alib{boxing,FHashcode}.
    FIsNotNull        ::Signature    fIsNotNull= nullptr;  ///< Entry for built-in function \alib{boxing,FIsNotNull}.
    FEquals           ::Signature    fEquals   = nullptr;  ///< Entry for built-in function \alib{boxing,FEquals}.
    FIsLess           ::Signature    fIsLess   = nullptr;  ///< Entry for built-in function \alib{boxing,FIsLess}.
    FIsTrue           ::Signature    fIsTrue   = nullptr;  ///< Entry for built-in function \alib{boxing,FIsTrue}.
#if ALIB_MONOMEM
    FClone            ::Signature    fClone    = nullptr;  ///< Entry for built-in function \alib{boxing,FClone}.
#endif
#if ALIB_STRINGS
    FAppend<character>::Signature    fAppend   = nullptr;  ///< Entry for built-in function \alib{boxing,FAppend,FAppend<character>}.
#endif
#if ALIB_DEBUG
    mutable uinteger DbgCntInvocationsFHashcode =0;   ///< Debug-compilation counter for the number of invocations.
    mutable uinteger DbgCntInvocationsFIsNotNull=0;   ///< Debug-compilation counter for the number of invocations.
    mutable uinteger DbgCntInvocationsFEquals   =0;   ///< Debug-compilation counter for the number of invocations.
    mutable uinteger DbgCntInvocationsFIsLess   =0;   ///< Debug-compilation counter for the number of invocations.
    mutable uinteger DbgCntInvocationsFIsTrue   =0;   ///< Debug-compilation counter for the number of invocations.
 #if ALIB_MONOMEM
    mutable uinteger DbgCntInvocationsFClone    =0;   ///< Debug-compilation counter for the number of invocations.
  #endif
  #if ALIB_STRINGS
    mutable uinteger DbgCntInvocationsFAppend   =0;   ///< Debug-compilation counter for the number of invocations.
  #endif
#endif

    // #############################################################################################
    // Get
    // #############################################################################################
#if defined(ALIB_DOX)
   /**
    * Receives the function of type \p{TFDecl}.
    *
    * @tparam TFDecl       The function type to get.
    * @param  isInvocation If \c true, the invocation counter is increased.
    *                      This parameter is defined only in debug-compilations.
    * @return The function found, or \c nullptr in case it was not defined.
    */
    template<typename TFDecl>
    typename TFDecl::Signature
    Get( bool isInvocation ) const;
#else
    template<typename TFDecl>  ATMP_T_IF(FHashcode         ::Signature, ATMP_EQ(TFDecl, FHashcode         ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFHashcode ; ) return fHashcode ; }
    template<typename TFDecl>  ATMP_T_IF(FIsNotNull        ::Signature, ATMP_EQ(TFDecl, FIsNotNull        ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsNotNull; ) return fIsNotNull; }
    template<typename TFDecl>  ATMP_T_IF(FEquals           ::Signature, ATMP_EQ(TFDecl, FEquals           ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFEquals   ; ) return fEquals   ; }
    template<typename TFDecl>  ATMP_T_IF(FIsLess           ::Signature, ATMP_EQ(TFDecl, FIsLess           ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsLess   ; ) return fIsLess   ; }
    template<typename TFDecl>  ATMP_T_IF(FIsTrue           ::Signature, ATMP_EQ(TFDecl, FIsTrue           ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsTrue   ; ) return fIsTrue   ; }
  ALIB_IF_MONOMEM(
    template<typename TFDecl>  ATMP_T_IF(FClone            ::Signature, ATMP_EQ(TFDecl, FClone            ))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFClone    ; ) return fClone    ; } )
  ALIB_IF_STRINGS(
    template<typename TFDecl>  ATMP_T_IF(FAppend<character>::Signature, ATMP_EQ(TFDecl, FAppend<character>))  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFAppend   ; ) return fAppend   ; } )

    template<typename TFDecl>  ATMP_T_IF(typename TFDecl::Signature,    !ATMP_EQ(TFDecl, FHashcode )
                                                                     && !ATMP_EQ(TFDecl, FIsNotNull)
                                                                     && !ATMP_EQ(TFDecl, FEquals   )
                                                                     && !ATMP_EQ(TFDecl, FIsLess   )
                                                                     && !ATMP_EQ(TFDecl, FIsTrue   )
                                             ALIB_IF_MONOMEM(  && !ATMP_EQ(TFDecl, FClone    )          )
                                             ALIB_IF_STRINGS( && !ATMP_EQ(TFDecl, FAppend<character>)  )
                                        )
    Get(ALIB_DBG( bool isInvocation )) const
    {
        return reinterpret_cast<typename TFDecl::Signature>( getCustom( typeid(TFDecl) ALIB_DBG(, isInvocation ) ) );
    }
#endif



    // #############################################################################################
    // Set
    // #############################################################################################
#if defined(ALIB_DOX)
   /**
    * Registers an implementation of the function of type \p{TFDecl}.
    *
    * @tparam TFDecl         The function type to register.
    * @param  implementation The implementation of the function.
    */
    template<typename TFDecl>
    void  Set( typename TFDecl::Signature implementation );
#else
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FHashcode         ))  Set( FHashcode         ::Signature impl) { fHashcode = impl; }
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FIsNotNull        ))  Set( FIsNotNull        ::Signature impl) { fIsNotNull= impl; }
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FEquals           ))  Set( FEquals           ::Signature impl) { fEquals   = impl; }
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FIsLess           ))  Set( FIsLess           ::Signature impl) { fIsLess   = impl; }
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FIsTrue           ))  Set( FIsTrue           ::Signature impl) { fIsTrue   = impl; }
  ALIB_IF_MONOMEM(
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FClone            ))  Set( FClone            ::Signature impl) { fClone    = impl; } )
  ALIB_IF_STRINGS(
    template<typename TFDecl>  ATMP_VOID_IF(ATMP_EQ(TFDecl, FAppend<character>))  Set( FAppend<character>::Signature impl) { fAppend   = impl; } )


    template<typename TFDecl>  ATMP_VOID_IF(   !ATMP_EQ(TFDecl, FHashcode )
                                            && !ATMP_EQ(TFDecl, FIsNotNull)
                                            && !ATMP_EQ(TFDecl, FEquals   )
                                            && !ATMP_EQ(TFDecl, FIsLess   )
                                            && !ATMP_EQ(TFDecl, FIsTrue   )
                          ALIB_IF_MONOMEM(  && !ATMP_EQ(TFDecl, FClone    )          )
                          ALIB_IF_STRINGS(  && !ATMP_EQ(TFDecl, FAppend<character>)  )
                                           )
    Set( typename TFDecl::Signature impl )
    {
        return setCustom( typeid(TFDecl), reinterpret_cast<void*>( impl ) );
    }
#endif

    // #############################################################################################
    // internals
    // #############################################################################################
    protected:
       #if defined(ALIB_DOX)
       /**
        * Non-inline implementation of #Get used in the case of non-built-in functions.
        * @param  rtti         The \c typeid of the function to get.
        * @param  isInvocation If \c true, the invocation counter is increased.
        *                      This parameter is defined only in debug-compilations.
        * @return The pointer to the box-function.
        */
       ALIB_API void*    getCustom( const std::type_info& rtti, bool isInvocation )  const;
       #else

       ALIB_API void*    getCustom( const std::type_info& rtti
                                             ALIB_DBG(        , bool isInvocation )) const;
       #endif

       /**
        * Non-inline implementation of #Set #Get used in the case of non-built-in functions.
        * @param  rtti           The \c typeid of the function to register.
        * @param  implementation The implementation of the function.
        */
       ALIB_API void     setCustom( const std::type_info& rtti, void* implementation );

}; // FunctionTable

/** ************************************************************************************************
 * The default box-functions set.
 **************************************************************************************************/
extern ALIB_API  FunctionTable DEFAULT_FUNCTIONS;


/** ************************************************************************************************
 * This struct is used internally with \alib_boxing_nl  to provide run-time type information
 * and virtual method invocations to boxed values.
 * A singleton instance of a this struct is attached to each \alib{boxing,Box}.
 **************************************************************************************************/
struct VTable
{
    /**
     * Denotes if the mapped type is value type, a pointer type, an enum or an array.
     * The latter is not specified by an enum element, but rather is \c true for all enum
     * elements that have a value greater than \c 0. The value in this case provides the size
     * of the array's element type.
     */
    enum MappingType :  integer
    {
        Value       =  0, ///< Value boxing.
        Pointer     = -1, ///< Pointer boxing.
        Enum        = -2, ///< Enum type boxing
    };

    /** Information about the encapsulated type. In case of arrays, this field is
     *  equal to \c typeid(void). */
    const std::type_info&   Type;

    /** In case of array types, information about the element type. Otherwise this field is
     *  equal to \c typeid(void). */
    const std::type_info&   ElementType;

    /**
     * Denotes if the mapped type is value type, a pointer type, an enum or an array.
     * In case of array types, the enum element value provides the size of the array's element
     * type (a value greater than \c 0).
     */
    const MappingType       Mapping;

    /**
     * The number of relevant bytes used in the placeholder.
     *
     * \see
     *   The documentation of TMP struct \alib{boxing,T_SizeInPlaceholder} provides details on
     *   and rationals for the existence of this property.
     */
    const unsigned int      PlaceholderUsage;

    /** Box-functions attached with \alib{boxing,BootstrapRegister}.     */
    FunctionTable           Functions;

    #if ALIB_DEBUG
        /**
         * These flags are available in debug-compilations only. Denotes whether this vtable
         * is statically or dynamically created. In case of \b Unregistered, a customized
         * vtable was not properly registered on bootstrap.
         */
        enum class DbgFactoryType
        {
            Unregistered, ///< Not registered, yet.
            Static      , ///< A static VTable is in place.
            Dynamic     , ///< The VTable is created dynamically from templated type \b VTableTT.
        };

        /** Debug information. */
        DbgFactoryType DbgProduction = DbgFactoryType::Unregistered;

        /**
         *  Counter for the number of unbox operations and function invocations made on boxes
         *  that used this \e vtable. Precisely, the counter is increased with invocations
         *  of
         *  \alib{boxing,Box::Unbox},
         *  \alib{boxing,Box::UnboxArray},
         *  \alib{boxing,Box::UnboxElement},
         *  \alib{boxing,Box::TypeID} and
         *  \alib{boxing,Box::GetFunction}.
         *
         *  Available only with debug builds.
         */
        uinteger DbgCntUsage = 0;
    #endif

    /** ****************************************************************************************
     * Constructor.
     *
     * @param type             Type information of the derived \b VTable type.
     * @param elementType      Type information of the elements of the boxed array.
     * @param mapping          The type of boxing: value, pointer, enum or arrays.
     * @param placeholderUsage The size of elements of the boxed array.
     ******************************************************************************************/
    VTable( const std::type_info&   type,
            const std::type_info&   elementType,
            MappingType             mapping,
            unsigned int            placeholderUsage )
     : Type            ( type             )
     , ElementType     ( elementType      )
     , Mapping         ( mapping          )
     , PlaceholderUsage( placeholderUsage )
     {}


    /** ****************************************************************************************
     * Virtual destructor.<br>
     * Note: This is not really needed and empty.  Because derived class \b %VTable derives from
     * virtual type \alib{singletons,Singleton}, having this destructor allows to clean memory
     * on termination "more precisely" and reduce warnings raised by analytics tools like
     * \b valgrind.
     ******************************************************************************************/
    virtual ~VTable()
    {}

    /** ****************************************************************************************
     * Returns \c true if this vtable represents boxed array types. In this case, method
     * \ref aworx::lib::boxing::Box::UnboxLength "Box::UnboxLength"
     * will return the length of the array and
     * \ref aworx::lib::boxing::Box::UnboxElement "Box::UnboxElement" may be used to
     * access elements of the array.
     *
     * @return \c true if this vtable is representing an array type, \c false otherwise.
     ******************************************************************************************/
    bool        IsArray()                                                                  const
    {
        return Mapping > 0;
    }

    /** ****************************************************************************************
     * Returns \c true if this vtable represents mapped pointer type.
     *
     * @return \c true if this vtable is representing a pointer type, \c false otherwise.
     ******************************************************************************************/
    bool        IsPointer()                                                                const
    {
        return Mapping == MappingType::Pointer;
    }

    /** ****************************************************************************************
     * Returns \c true if this vtable represents a scoped or non-scoped enum type.
     *
     * @return \c true if this vtable is representing an enum type, \c false otherwise.
     ******************************************************************************************/
    bool        IsEnum()                                                                   const
    {
        return Mapping == MappingType::Enum;
    }
}; // struct VTable


// #################################################################################################
// VTable
// #################################################################################################
#if ALIB_DEBUG_BOXING

    /** ********************************************************************************************
     * Registers a virtual table for debug purposes.
     * This function is invoked internally, when a dynamic \e vtable is created and
     * when a static \e vtable is registered.
     *
     * Statically created \e vtables have to be registered during bootstrap in debug-compilations.
     * For this, macro \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER is available, which is empty in
     * release compilations.
     *
     * \see
     *   Macros ALIB_BOXING_VTABLE_DECLARE and \ref alib_mod_boxing "Programmer's Manual" chapter
     *   \ref alib_boxing_more_opt_staticvt.
     *
     * @param vtable         The vtable to register.
     * @param productionType Denotes whether the \p{vtable} is a static constexpr object or
     *                       dynamically created from template type \b VTableTT.
     **********************************************************************************************/
    ALIB_API
    void DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType );

#endif // ALIB_DEBUG_BOXING

/** ************************************************************************************************
 * This is a templated derivate of class \b VTable. It is needed to create singletons for arbitrary
 * mapped types.
 *
 * \see
 *   Chapter \ref alib_boxing_more_opt_staticvt_bg
 *   of the \alib_boxing "Programmer's Manual" of module \alib_boxing_nl.
 *
 * ## Friends ##
 * class \alib{singletons,Singleton,Singleton< VTableTT<TPlainOrArray, TMapped> >}
 *
 * @tparam TPlainOrArray   Either type \alib{boxing,detail::TPlain} or \alib{boxing,detail::TArrayOf}.
 * @tparam TMapped         The mapped type.
 **************************************************************************************************/
template<typename TPlainOrArray, typename TMapped>
class VTableTT  : public VTable
                , public Singleton< VTableTT<TPlainOrArray, TMapped> >
{
    #if !defined(ALIB_DOX)
        friend singletons::Singleton< VTableTT<TPlainOrArray, TMapped> >;
    #endif

    private:
        /**
         * Constructor. Passes parameters as expected by parent class \b VTable.
         *
         * This constructor is private and class \b %Singleton is a friend, which makes each
         * template instance of this class being a 'strict' singleton (only that singleton instance
         * exists).
         */
        ALIB_CPP14_REL_CONSTEXPR VTableTT()
        : VTable( ATMP_EQ(TPlainOrArray, TPlain )  ? typeid(TMapped)
                                                   : typeid(TMapped[1]),

                  ATMP_EQ(TPlainOrArray, TPlain )  ? typeid(void)
                                                   : typeid(TMapped),

                  ATMP_EQ(TPlainOrArray, TArrayOf) ? VTable::MappingType(sizeof(TMapped))
                                                   : ATMP_IS_PTR(TMapped)
                                                   ? VTable::MappingType::Pointer
                                                   : ATMP_IS_ENUM(TMapped)
                                                   ? VTable::MappingType::Enum
                                                   : VTable::MappingType::Value    ,


                  ATMP_EQ(TPlainOrArray, TPlain )  ? T_SizeInPlaceholder<TMapped>::value
                                                   : sizeof(Placeholder)
                )
        {
            #if ALIB_DEBUG_BOXING
                DbgRegisterVTable( this, DbgFactoryType::Dynamic );
            #endif
        }
};


#if ALIB_DEBUG_BOXING || defined(ALIB_DOX)

/**
 * Checks if \alib_boxing_nl was correctly initialized.
 * This is done with the creation of \ref alib_boxing_more_opt_staticvt "dynamic vtables",
 * to detect global or static box objects which are initialized with data that is mapped to
 * a type that no static \e vtable is defined for.
 *
 * Available and used only in debug-compilations.
 *
 * \see
 *   Manual chapter \ref alib_boxing_more_static_instances.
 */
ALIB_API void DbgCheckIsInitialized();

/**
 * Checks for doubly-defined \e vtables, as well as for unregistered
 * \ref alib_boxing_more_opt_staticvt "static vtables".
 *
 * Available and used only in debug-compilations.
 * @param vtable                The \e vtable to check.
 * @param increaseUsageCounter  Denotes if this is a use of the vtable or just a check.
 */
ALIB_API void DbgCheckRegistration( detail::VTable* vtable, bool increaseUsageCounter );
#else
inline void DbgCheckIsInitialized()                         {}
inline void DbgCheckRegistration (detail::VTable*, bool )   {}
#endif




/** ************************************************************************************************
 * Method #Get of the default version of this struct creates a \alib{boxing::detail,VTable}
 * dynamically.
 * For this, a corresponding templated \alib{singletons,Singleton} type
 * \alib{boxing::detail,VTableTT}, according to given template parameter \p{TMapping} is returned.
 *
 * For optimization purposes (code size as well as execution performance), this struct might
 * be specialized for selected a mapped types, to return static singleton object.
 *
 * Specializations for all C++ fundamental types are given. Furthermore, various different
 * \alibmods provide such specializations for types they introduced and that are boxed.
 *
 * \see
 *   Chapter \ref alib_boxing_more_opt_staticvt_howto of the Programmer's Manual
 *   of \alib_boxing_nl.
 *
 * @tparam TMapping  The mapped type to receive a \e vtable for. Has to be of either type
 *                   \alib{boxing,TMappedTo<T>} or \alib{boxing,TMappedToArrayOf<T>}.
 **************************************************************************************************/
template<typename TMapping>
struct  T_VTableFactory
{
    /**
     * Returns a strict singleton \e vtable responsible for boxing mapped type \p{TMapping}.
     * @return The requested \e vtable singleton.
     */
    static ALIB_CPP14_CONSTEXPR VTable* Get()
    {
        DbgCheckIsInitialized();

        VTable* result=  &VTableTT<typename TMapping::PlainOrArray, typename TMapping::Type>::GetSingleton();

        DbgCheckRegistration( result, false );

        return result;
    }

};


}}}} // namespace [aworx::lib::boxing::detail]


//##################################################################################################
//#######################################     Macros     ###########################################
//##################################################################################################

#define ALIB_BOXING_VTABLE_DECLARE( TMapped, Identifier )                                          \
namespace aworx { namespace lib { namespace boxing { namespace detail {                            \
extern ALIB_API VTable SNGLTN_ ## Identifier;                                                      \
template<> struct  T_VTableFactory< TMappedTo<TMapped> >                                           \
{ static constexpr VTable* Get() { return  &SNGLTN_ ## Identifier; } };   }}}}                     \

#define ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( TMapped, Identifier )                                \
namespace aworx { namespace lib { namespace boxing { namespace detail {                            \
extern ALIB_API VTable SNGLTN_ ## Identifier;                                                      \
template<> struct  T_VTableFactory< TMappedToArrayOf<TMapped> >                                    \
{ static constexpr VTable* Get() { return  &SNGLTN_ ## Identifier; } };   }}}}                     \


#define ALIB_BOXING_VTABLE_DEFINE( TMapped, Identifier )                                           \
aworx::lib::boxing::detail::VTable aworx::lib::boxing::detail::SNGLTN_ ## Identifier               \
( typeid(TMapped), typeid(void)  ,                                                                 \
   std::is_pointer<TMapped>::value                                                                 \
 ? VTable::MappingType::Pointer                                                                    \
 : std::is_enum<TMapped>::value                                                                    \
 ? VTable::MappingType::Enum                                                                       \
 : VTable::MappingType::Value   ,                                                                  \
 aworx::lib::boxing::T_SizeInPlaceholder<TMapped>::value);                                         \

#define ALIB_BOXING_VTABLE_DEFINE_ARRAYTYPE( TMapped, Identifier )                                 \
aworx::lib::boxing::detail::VTable aworx::lib::boxing::detail::SNGLTN_ ## Identifier               \
(typeid(TMapped[1]) , typeid(TMapped), VTable::MappingType(sizeof(TMapped)), sizeof(Placeholder)); \


#if ALIB_DEBUG_BOXING
#   define ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( Identifier )                                 \
DbgRegisterVTable( &aworx::lib::boxing::detail::SNGLTN_ ## Identifier,                             \
                   aworx::lib::boxing::detail::VTable::DbgFactoryType::Static );
#else
#   define ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( Identifier )
#endif

#endif // HPP_ALIB_BOXING_DETAIL_VTABLE



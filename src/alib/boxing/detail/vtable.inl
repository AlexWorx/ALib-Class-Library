//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::boxing {

/// This namespace implements internals of namespace #alib::boxing.
namespace detail {

//==================================================================================================
/// This struct is used internally with \alib_boxing_nl  to provide run-time type information
/// and virtual method invocations to boxed values.
/// A singleton instance of a derived type of this class is attached to each \alib{boxing;Box}.
/// Such derived type is generated using templated derived struct \alib{boxing::detail;VTable}.
//==================================================================================================
struct FunctionTable
{
    // #############################################################################################
    // Fields
    // #############################################################################################
    FHashcode         ::Signature    fHashcode = nullptr;  ///< Entry for built-in function \alib{boxing;FHashcode}.
    FIsNotNull        ::Signature    fIsNotNull= nullptr;  ///< Entry for built-in function \alib{boxing;FIsNotNull}.
    FEquals           ::Signature    fEquals   = nullptr;  ///< Entry for built-in function \alib{boxing;FEquals}.
    FIsLess           ::Signature    fIsLess   = nullptr;  ///< Entry for built-in function \alib{boxing;FIsLess}.
    FIsTrue           ::Signature    fIsTrue   = nullptr;  ///< Entry for built-in function \alib{boxing;FIsTrue}.
#if ALIB_MONOMEM
    FClone            ::Signature    fClone    = nullptr;  ///< Entry for built-in function \alib{boxing;FClone}.
#endif
#if ALIB_STRINGS
    FAppend<character,lang::HeapAllocator>
                      ::Signature    fAppend   = nullptr;  ///< Entry for built-in function \alib{boxing;FAppend}.
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
#if DOXYGEN
   /// Receives the function of type \p{TFDecl}.
   ///
   /// @tparam TFDecl       The function type to get.
   /// @param  isInvocation If \c true, the invocation counter is increased.
   ///                      This parameter is defined only in debug-compilations.
   /// @return The function found, or \c nullptr in case it was not defined.
    template<typename TFDecl>
    typename TFDecl::Signature
    Get( bool isInvocation ) const;
#else
    template<typename TFDecl>  requires std::same_as<TFDecl, FHashcode >  FHashcode ::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFHashcode ; ) return fHashcode ; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsNotNull>  FIsNotNull::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsNotNull; ) return fIsNotNull; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FEquals   >  FEquals   ::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFEquals   ; ) return fEquals   ; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsLess   >  FIsLess   ::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsLess   ; ) return fIsLess   ; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsTrue   >  FIsTrue   ::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFIsTrue   ; ) return fIsTrue   ; }
  #if ALIB_MONOMEM
    template<typename TFDecl>  requires std::same_as<TFDecl, FClone    >  FClone    ::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFClone    ; ) return fClone    ; }
  #endif
  #if ALIB_STRINGS
    template<typename TFDecl>  requires std::same_as<TFDecl, FAppend<character,lang::HeapAllocator>>
                                              FAppend<character,lang::HeapAllocator>::Signature  Get(ALIB_DBG( bool isInvocation )) const { ALIB_DBG( if(isInvocation) ++DbgCntInvocationsFAppend   ; ) return fAppend   ; }
  #endif

    template<typename TFDecl>  requires (    !std::same_as<TFDecl, FHashcode >
                                          && !std::same_as<TFDecl, FIsNotNull>
                                          && !std::same_as<TFDecl, FEquals   >
                                          && !std::same_as<TFDecl, FIsLess   >
                                          && !std::same_as<TFDecl, FIsTrue   >
  #if ALIB_MONOMEM
                                          && !std::same_as<TFDecl, FClone    >
  #endif
  #if ALIB_STRINGS
                                          && !std::same_as<TFDecl, FAppend<character,lang::HeapAllocator>>
  #endif
                                        ) typename TFDecl::Signature                            Get(ALIB_DBG( bool isInvocation )) const  {
        return reinterpret_cast<typename TFDecl::Signature>( getCustom( typeid(TFDecl) ALIB_DBG(, isInvocation ) ) );
    }
#endif



    // #############################################################################################
    // Set
    // #############################################################################################
#if DOXYGEN
   /// Registers an implementation of the function of type \p{TFDecl}.
   ///
   /// @tparam TFDecl         The function type to register.
   /// @param  implementation The implementation of the function.
    template<typename TFDecl>
    void  Set( typename TFDecl::Signature implementation );
#else
    template<typename TFDecl>  requires std::same_as<TFDecl, FHashcode > void Set( FHashcode ::Signature impl) { fHashcode = impl; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsNotNull> void Set( FIsNotNull::Signature impl) { fIsNotNull= impl; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FEquals   > void Set( FEquals   ::Signature impl) { fEquals   = impl; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsLess   > void Set( FIsLess   ::Signature impl) { fIsLess   = impl; }
    template<typename TFDecl>  requires std::same_as<TFDecl, FIsTrue   > void Set( FIsTrue   ::Signature impl) { fIsTrue   = impl; }
  #if ALIB_MONOMEM
    template<typename TFDecl>  requires std::same_as<TFDecl, FClone    > void Set( FClone    ::Signature impl) { fClone    = impl; }
  #endif
  #if ALIB_STRINGS
    template<typename TFDecl>  requires std::same_as<TFDecl,
                                 FAppend<character,lang::HeapAllocator>> void Set( FAppend<character,lang::HeapAllocator>
                                                                                             ::Signature impl) { fAppend   = impl; }
  #endif

    template<typename TFDecl>  requires   (    !std::same_as<TFDecl, FHashcode >
                                            && !std::same_as<TFDecl, FIsNotNull>
                                            && !std::same_as<TFDecl, FEquals   >
                                            && !std::same_as<TFDecl, FIsLess   >
                                            && !std::same_as<TFDecl, FIsTrue   >
  #if ALIB_MONOMEM
                                            && !std::same_as<TFDecl, FClone    >
  #endif
  #if ALIB_STRINGS
                                            && !std::same_as<TFDecl, FAppend<character,lang::HeapAllocator>   >
  #endif
                                                                       )
    void Set( typename TFDecl::Signature impl )   { return setCustom( typeid(TFDecl), reinterpret_cast<void*>( impl ) ); }
#endif

    // #############################################################################################
    // Cleanup (needed for debug-builds in shutdown)
    // #############################################################################################
#if (ALIB_MONOMEM && ALIB_CONTAINERS && ALIB_DEBUG)  || DOXYGEN
    /// Needs to be called only in debug versions to shut down internal hashtables cleanly.
    ALIB_DLL static void Shutdown();
#else
    inline   static void Shutdown() {}
#endif


    // #############################################################################################
    // internals
    // #############################################################################################
    protected:
       #if DOXYGEN
       /// Non-inline implementation of #Get used in the case of non-built-in functions.
       /// @param  rtti         The \c typeid of the function to get.
       /// @param  isInvocation If \c true, the invocation counter is increased.
       ///                      This parameter is defined only in debug-compilations.
       /// @return The pointer to the box-function.
       ALIB_DLL void*    getCustom( const std::type_info& rtti, bool isInvocation )  const;
       #else

       ALIB_DLL void*    getCustom( const std::type_info& rtti
                                             ALIB_DBG(        , bool isInvocation )) const;
       #endif

       /// Non-inline implementation of #Set #Get used in the case of non-built-in functions.
       /// @param  rtti           The \c typeid of the function to register.
       /// @param  implementation The implementation of the function.
       ALIB_DLL void     setCustom( const std::type_info& rtti, void* implementation );

}; // FunctionTable

//==================================================================================================
/// The default box-functions set.
//==================================================================================================
extern ALIB_DLL  FunctionTable DEFAULT_FUNCTIONS;

//==================================================================================================
/// The custom function hash
//==================================================================================================
#if !DOXYGEN
struct CustomFunctionKey {
    const FunctionTable*    Parent;
    const std::type_info&   Type;
    CustomFunctionKey( const FunctionTable* parent, const std::type_info& type )
    : Parent(parent), Type  (type )                                                           {}
};

struct CustomFunctionMapped {
    void*                   Implementation;
    ALIB_DBG(uinteger       DbgCntInvocations; )

         CustomFunctionMapped( void* implementation ): Implementation    (implementation)
    ALIB_DBG(,DbgCntInvocations  (0             )  )                                                 {}
};

struct CustomFunctionHash {
    std::size_t operator()(const CustomFunctionKey& key) const {
        size_t result=    reinterpret_cast<size_t>(key.Parent)
                        ^ key.Type.hash_code();
        result^= (result << 21 );
        result^= (result >> 11);
        return result;
    }};

struct CustomFunctionEqualTo {
    bool operator()(const CustomFunctionKey& lhs, const CustomFunctionKey& rhs) const {
        return     lhs.Parent == rhs.Parent
                && lhs.Type   == rhs.Type;
    }};

#if ALIB_MONOMEM && ALIB_CONTAINERS
extern HashMap           < MonoAllocator,
                           CustomFunctionKey, CustomFunctionMapped,
                           CustomFunctionHash,
                           CustomFunctionEqualTo >     customFunctionMap;
#else
extern std::unordered_map< CustomFunctionKey, CustomFunctionMapped,
                           CustomFunctionHash,
                           CustomFunctionEqualTo >     customFunctionMap;
#endif
#endif //!DOXYGEN

//==================================================================================================
/// This struct is used internally with \alib_boxing_nl  to provide run-time type information
/// and virtual method invocations to boxed values.
/// A singleton instance of this struct is attached to each \alib{boxing;Box}.
//==================================================================================================
struct VTable
{
    /// Denotes if the mapped type is a value type, a pointer type, an enum or an array.
    /// The latter is not specified by an enum element, but rather is \c true for all enum
    /// elements that have a value greater than \c 0. The value in this case provides the size
    /// of the array's element type.
    enum MappingType :  integer
    {
        Value       =  0, ///< Value boxing.
        Pointer     = -1, ///< Pointer boxing.
        Enum        = -2, ///< Enum type boxing
    };

    /// Information about the encapsulated type. In the case of arrays, this field is
    /// equal to \c typeid(void).
    const std::type_info&   Type;

    /// In case of array types, information about the element type. Otherwise this field is
    /// equal to \c typeid(void).
    const std::type_info&   ElementType;

    /// Denotes if the mapped type is a value type, a pointer type, an enum or an array.
    /// In the case of array types, the enum element value provides the size of the array's element
    /// type (a value greater than \c 0).
    const MappingType       Mapping;

    /// The number of relevant bytes used in the placeholder.
    /// \see
    ///   The documentation of \alib{boxing;SizeTraits} provides details on
    ///   and rationals for the existence of this property.
    const unsigned int      PlaceholderUsage;

    /// Box-functions attached with \alib{boxing;BootstrapRegister}.
    FunctionTable           Functions;

    #if ALIB_DEBUG
        /// These flags are available in debug-compilations only. Denotes whether this vtable
        /// is statically or dynamically created. In the case of \b Unregistered, a customized
        /// vtable was not properly registered on bootstrap.
        enum class DbgFactoryType
        {
            Unregistered, ///< Not registered, yet.
            Static      , ///< A static VTable is in place.
            Dynamic     , ///< The VTable is created dynamically from the templated type
                          ///< \b VTableUnoptimized.
        };

        /// Debug information.
        DbgFactoryType DbgProduction = DbgFactoryType::Unregistered;

        ///  Counter for the number of unbox operations and function invocations made on boxes
        ///  that used this \e vtable. Precisely, the counter is increased with invocations
        ///  of
        ///  \alib{boxing;Box::Unbox},
        ///  \alib{boxing;Box::UnboxArray},
        ///  \alib{boxing;Box::UnboxElement},
        ///  \alib{boxing;Box::TypeID} and
        ///  \alib{boxing;Box::GetFunction}.
        ///
        ///  Available only with debug-builds.
        uinteger DbgCntUsage = 0;
    #endif

    /// Constructor.
    ///
    /// @param type             Type information of the derived \b VTable type.
    /// @param elementType      Type information of the elements of the boxed array.
    /// @param mapping          The type of boxing: value, pointer, enum or arrays.
    /// @param placeholderUsage The size of elements of the boxed array.
    VTable( const std::type_info&   type,
            const std::type_info&   elementType,
            MappingType             mapping,
            unsigned int            placeholderUsage )
     : Type            ( type             )
     , ElementType     ( elementType      )
     , Mapping         ( mapping          )
     , PlaceholderUsage( placeholderUsage )
     {}


    /// Virtual destructor.<br>
    /// Note: This is not really needed and empty.  Because derived class \b %VTable derives from
    /// virtual type \alib{singletons;Singleton}, having this destructor allows cleaning memory
    /// on termination "more precisely" and reduce warnings raised by analytics tools like
    /// \b valgrind.
    virtual ~VTable()
    {}

    /// Returns \c true if this vtable represents boxed array types. In this case, method
    /// \ref alib::boxing::Box::UnboxLength "Box::UnboxLength"
    /// will return the length of the array and
    /// \ref alib::boxing::Box::UnboxElement "Box::UnboxElement" may be used to
    /// access elements of the array.
    ///
    /// @return \c true if this vtable is representing an array type, \c false otherwise.
    bool        IsArray()                                                                  const
    {
        return Mapping > 0;
    }

    /// Returns \c true if this vtable represents mapped pointer type.
    ///
    /// @return \c true if this vtable is representing a pointer type, \c false otherwise.
    bool        IsPointer()                                                                const
    {
        return Mapping == MappingType::Pointer;
    }

    /// Returns \c true if this vtable represents a scoped or non-scoped enum type.
    ///
    /// @return \c true if this vtable is representing an enum type, \c false otherwise.
    bool        IsEnum()                                                                   const
    {
        return Mapping == MappingType::Enum;
    }
}; // struct VTable

#if ALIB_DEBUG
} namespace debug {

/// Registers a virtual table for debug purposes.
/// This function is invoked internally, when a dynamic \e vtable is created and
/// when a static \e vtable is registered.
///
/// Statically created \e vtables have to be registered during bootstrap in debug-compilations.
/// For this, macro \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER is available, which is empty
/// in release compilations.
///
/// \see
///   Macros ALIB_BOXING_VTABLE_DECLARE and \ref alib_mod_boxing "Programmer's Manual" chapter
///   \ref alib_boxing_more_opt_staticvt.
///
/// @param vtable         The vtable to register.
/// @param productionType Denotes whether the \p{vtable} is a static constexpr object or
///                       dynamically created from template type \b VTableUnoptimized.
ALIB_DLL
void DbgRegisterVTable( detail::VTable* vtable, detail::VTable::DbgFactoryType productionType );
#endif

} namespace detail {


//==================================================================================================
/// This is a templated derivate of class \b VTable. It is necessary to create singletons for
/// arbitrary mapped types.
///
/// \see
///   Chapter \ref alib_boxing_more_opt_staticvt_bg
///   of the \alib_boxing "Programmer's Manual" of module \alib_boxing_nl.
///
/// ## Friends ##
/// class \alib{singletons;Singleton;Singleton< VTableUnoptimized<TMapped, TIsArray> >}
///
/// @tparam TMapped    The mapped type.
/// @tparam TIsArray   Denotes whether array-boxing is performed or not.
//==================================================================================================
template<typename TMapped, bool TIsArray>
class VTableUnoptimized  : public VTable
                         , public Singleton< VTableUnoptimized<TMapped, TIsArray> >
{
    #if !DOXYGEN
        friend singletons::Singleton< VTableUnoptimized<TMapped, TIsArray> >;
    #endif

    private:
        /// Constructor. Passes parameters as expected by parent class \b VTable.
        ///
        /// This constructor is private, and class \b %Singleton is a friend, which makes each
        /// template instance of this class being a 'strict' singleton (only that singleton instance
        /// exists).
        constexpr VTableUnoptimized()
        : VTable( !TIsArray  ? typeid(TMapped)
                             : typeid(TMapped[1]),

                  !TIsArray  ? typeid(void)
                             : typeid(TMapped),

                  TIsArray   ? VTable::MappingType(sizeof(TMapped))
                             : std::is_pointer_v<TMapped>
                             ? VTable::MappingType::Pointer
                             : std::is_enum_v<TMapped>
                             ? VTable::MappingType::Enum
                             : VTable::MappingType::Value    ,

                  !TIsArray  ? SizeTraits<TMapped>
                             : sizeof(Placeholder)             )    {
            #if ALIB_DEBUG_BOXING
                debug::DbgRegisterVTable( this, DbgFactoryType::Dynamic );
            #endif
        }
};

} namespace debug {
#if ALIB_DEBUG_BOXING || DOXYGEN

/// Checks if \alib_boxing_nl was correctly initialized.
/// This is done with the creation of \ref alib_boxing_more_opt_staticvt "dynamic vtables",
/// to detect global or static box objects which are initialized with data that is mapped to
/// a type that no static \e vtable is defined for.
///
/// Available and used only in debug-compilations.
///
/// \see
///   Manual chapter \ref alib_boxing_more_static_instances.
ALIB_DLL void DbgCheckIsInitialized();

/// Checks for doubly-defined \e vtables, as well as for unregistered
/// \ref alib_boxing_more_opt_staticvt "static vtables".
///
/// Available and used only in debug-compilations.
/// @param vtable                The \e vtable to check.
/// @param increaseUsageCounter  Denotes if this is a use of the vtable or just a check.
ALIB_DLL void DbgCheckRegistration( detail::VTable* vtable, bool increaseUsageCounter );
#else
inline void DbgCheckIsInitialized()                         {}
inline void DbgCheckRegistration (detail::VTable*, bool )   {}
#endif

} // namespace alib::boxing[::debug]

//==================================================================================================
/// Method #Get of the default version of this struct creates a \alib{boxing::detail;VTable}
/// dynamically.
/// For this, a corresponding templated \alib{singletons;Singleton} of type
/// \alib{boxing::detail;VTableUnoptimized<TMapped, TIsArray>} is returned.
///
/// For optimization purposes (code size as well as execution performance), this struct might
/// be specialized for selected mapped types to return a static singleton object.
///
/// Specializations can be performed with preprocessor macros
///  - \ref ALIB_BOXING_VTABLE_DECLARE,
///  - \ref ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE, and
///  - \ref ALIB_BOXING_VTABLE_DEFINE.
///
/// Furthermore, with debug-compilations, such spezializations should be "registered" during
/// bootstrap using macro \ref ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER.
///
/// Specializations for all C++ fundamental types are given. Furthermore, various different
/// \alibmods provide such specializations for types they introduced and that are boxed.
///
/// \see
///  - Chapter \ref alib_boxing_more_opt_staticvt_howto of the Programmer's Manual
///    of \alib_boxing_nl.
///
/// @tparam TMapped  The mapped type to receive a \e vtable for.
/// @tparam TIsArray Denotes if this is array-boxing or not.
//==================================================================================================
template<typename TMapped, bool TIsArray>
struct VTableOptimizationTraits
{
    /// Functor to return the \b vtable singleton.
    /// @return The requested \e vtable singleton.
    static constexpr detail::VTable* Get() {
        debug::DbgCheckIsInitialized();
        detail::VTable* result=  &detail::VTableUnoptimized<TMapped, TIsArray>::GetSingleton();
        debug::DbgCheckRegistration( result, false );
        return result;
    }
};



#if ALIB_DEBUG && !DOXYGEN
namespace debug {
#   if ALIB_MONOMEM && ALIB_CONTAINERS
    extern HashSet<MonoAllocator, lang::TypeFunctors::Key,                  lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern HashMap<MonoAllocator, lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTables;
    extern HashMap<MonoAllocator, lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTablesArray;
# else
    extern std::unordered_set<    lang::TypeFunctors::Key,                  lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownCustomFunctions;
    extern std::unordered_map<    lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTables;
    extern std::unordered_map<    lang::TypeFunctors::Key, detail::VTable*, lang::TypeFunctors::Hash, lang::TypeFunctors::EqualTo>  DbgKnownVTablesArray;
# endif
   extern ALIB_DLL void DbgLockMaps( bool doLock );
} // namespace alib::boxing[::debug]
#endif

} // namespace [alib::boxing]

// VTables for character arrays. We need them before the constructors of class Box are defined.
// Therefore, declaring them in "boxingcustoms.inl.inl." would be too late.
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char, vt_arr_char )
DOX_MARKER([DOX_BOXING_OPTIMIZE_DECLARE_2])
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( wchar_t   , vt_arr_wchar_t )
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char8_t   , vt_arr_char8_t )
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char16_t  , vt_arr_char16_t)
ALIB_BOXING_VTABLE_DECLARE_ARRAYTYPE( char32_t  , vt_arr_char32_t)




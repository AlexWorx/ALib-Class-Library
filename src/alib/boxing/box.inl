//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace boxing  {

//==================================================================================================
/// This is the central class of \alib_boxing_nl . By making extensive use of template programming
/// and keyword \c requires on overloaded constructors, an object of this class can be created by
/// passing instances of (almost) any C++ type to the constructor.
/// The passed value will be "boxed" within the instance of this class.
///
/// Then, the instances of this class support type checking, value extraction ("unboxing") and the
/// invocation of "virtual methods". All features are customizable via
/// \ref alib_manual_appendix_tca "type traits", and thus the built-in defaulted behavior for a
/// custom type can be changed.
///
/// A thorough introduction to and documentation of all aspects of \alib_boxing_nl  is given
/// with Programmer's Manual \alib_boxing.
///
/// ## Functors In Namespace std ##
/// Functors <c>std::hash</c>, <c>std::equal_to,</c> and <c>std::less</c> are specialized for
/// this type with the inclusion of the header-file \implude{Boxing.StdFunctors}.
//==================================================================================================
class Box
{
  protected:
    // #############################################################################################
    // protected fields and methods
    // #############################################################################################

    /// The singleton of a class derived from class \b %VTable which defines our type and
    /// behavior.
    detail::VTable*     vtable;

    /// The data that we encapsulate.
    Placeholder         data;


    // #############################################################################################
    // Two local macros that check whether T is a valid type to box, respectively unbox.
    // For this, various different cases are (redundantly) checked to produce compiler
    // errors that provide all necessary information about why the operation cannot be
    // performed.
    // Note: Using an empty templated method for this, produced the static assertion only after
    //       other compiler errors. Therefore, a macro is used.
    // #############################################################################################

#if !DOXYGEN
    #define ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(TBoxable, valueBoxing)                     \
    using          TVal          = ALIB_TVALUE(TBoxable);                                          \
    using          TPtr          = TVal*;                                                          \
    constexpr bool isVolatile    = std::is_volatile_v<std::remove_pointer_t<TBoxable>>;            \
    constexpr bool isPointer     = std::is_pointer<TBoxable>::value;                               \
    constexpr bool isValue       = !isPointer;                                                     \
    constexpr bool valIsString   = IsStringType<TVal>;                                             \
    constexpr bool isCustomizedTV= IsCustomized<TVal>;                                             \
    constexpr bool isCustomizedTP= IsCustomized<TPtr>;                                             \
    constexpr bool isBlockedTV   = std::same_as<NotBoxableTag,                                     \
                                                typename BoxTraits<TVal>::Mapping>;                \
    constexpr bool isBlockedTP   = std::same_as<NotBoxableTag,                                     \
                                                typename BoxTraits<TPtr>::Mapping>;                \
                                                                                                   \
    ALIB_STATIC_DENY( GeneralBoxingRule1,  !valueBoxing && isVolatile,                             \
      "Types boxed as pointers cannot be boxed if volatile."      );                               \
                                                                                                   \
    ALIB_STATIC_DENY( GeneralBoxingRule4,  isPointer  && valIsString,                              \
      "String types must not be given as pointers." );                                             \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule3,  isBlockedTV  && isValue,                                 \
      "Customized boxing forbids boxing this value type: "                                         \
      "'BoxTraits<T>::Type == NotBoxable'!"      );                                                \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule4,  isBlockedTP  && isPointer,                               \
      "Customized boxing forbids boxing this pointer type: "                                       \
      "'BoxTraits<T*>::Type == NotBoxable'!"     );                                                \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule5,  isBlockedTV  && !isCustomizedTP  &&  isPointer,          \
      "Customized boxing forbids boxing value type T (BoxTraits<T>::Type == NotBoxable), while "   \
      "no customization for this pointer type T* was given."  );                                   \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule6,  isBlockedTP  && !isCustomizedTV  &&  isValue,            \
      "Customized boxing forbids boxing pointer type T* "                                          \
      "(BoxTraits<T*>::Type == NotBoxable), while no customization for this value type T was "     \
      "given. "                                );                                                  \
                                                                                                   \
    // check IsType/Unbox
    #define ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TUnboxable)                              \
    using          TVal          = ALIB_TVALUE(TUnboxable);                                        \
    using          TPtr          = TVal*;                                                          \
    constexpr bool isConst       = std::is_const_v   <std::remove_pointer_t<TUnboxable>>;          \
    constexpr bool isVolatile    = std::is_volatile_v<std::remove_pointer_t<TUnboxable>>;          \
    constexpr bool isPointer     = std::is_pointer<TUnboxable>::value;                             \
    constexpr bool isValue       = !isPointer;                                                     \
    constexpr bool valuesFit     =   sizeof(std::conditional_t<std::same_as<void,TVal>,void*,TVal>)\
                                  <= sizeof(Placeholder);                                          \
    constexpr bool isCopyConstr  = std::is_copy_constructible<TVal>::value;                        \
    constexpr bool isTrivDest    = std::is_trivially_destructible<TVal>::value;                    \
    constexpr bool isCustomizedTV= IsCustomized<TVal>;                                             \
    constexpr bool isCustomizedTP= IsCustomized<TPtr>;                                             \
    constexpr bool isDefault     = !(isCustomizedTV || isCustomizedTP);                            \
    constexpr bool isBlockedTV   = std::same_as<NotBoxableTag,                                     \
                                                typename BoxTraits<TVal>::Mapping>;                \
    constexpr bool isBlockedTP   = std::same_as<NotBoxableTag,                                     \
                                                typename BoxTraits<TPtr>::Mapping>;                \
    constexpr bool isLockedTV    = IsLocked<TVal>;                                                 \
    constexpr bool isLockedTP    = IsLocked<TPtr>;                                                 \
                                                                                                   \
    /* Redundant type qualifiers */                                                                \
    ALIB_STATIC_DENY( GeneralBoxingRule2, isConst,                                                 \
      "Type qualifier 'const' not allowed with template type TUnboxable. Types boxed as values"    \
      " are always unboxed mutable, types boxed as pointers are always unboxed constant." );       \
                                                                                                   \
    ALIB_STATIC_DENY( GeneralBoxingRule3, isVolatile,                                              \
      "Type qualifier 'volatile' not allowed with template type TUnboxable"               );       \
                                                                                                   \
    /* Default boxing */                                                                           \
    ALIB_STATIC_DENY( DefaultBoxingRule1, isDefault && isValue && !valuesFit,                      \
      "This type cannot be unboxed by value: "                                                     \
      "By default, values that do not fit into boxes are boxed as pointers."              );       \
                                                                                                   \
    ALIB_STATIC_DENY( DefaultBoxingRule2,                                                          \
                      isDefault && isValue && (!isCopyConstr || !isTrivDest),                      \
      "This type cannot be unboxed by value: "                                                     \
      "By default, types that are not copy-constructible or not trivially destructible, "          \
      "are boxed as pointers."                                                            );       \
                                                                                                   \
    ALIB_STATIC_DENY( DefaultBoxingRule3,                                                          \
                      isDefault && isPointer && valuesFit && isCopyConstr && isTrivDest,           \
      "This type cannot be unboxed as pointer: Default boxing of types that fit "                  \
      "into boxes and are copy-constructible and trivially destructible, "                         \
      "is performed by value."                                                            );       \
                                                                                                   \
                                                                                                   \
    /* Custom boxing */                                                                            \
    ALIB_STATIC_DENY( CustomBoxingRule1,  isCustomizedTV && !isCustomizedTP  &&  isPointer,        \
      "This pointer type T* cannot be unboxed, because custom boxing is defined for "              \
      "value type T, while no custom boxing is defined for pointer type T*."             );        \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule2, !isCustomizedTV && isCustomizedTP  &&  isValue,           \
      "This value type T cannot be unboxed, because custom boxing is defined for "                 \
      "pointer type T*, while no custom boxing is defined for value type T."              );       \
                                                                                                   \
                                                                                                   \
    /* Boxing blocked */                                                                           \
    ALIB_STATIC_DENY( CustomBoxingRule3,  isBlockedTV  && isValue,                                 \
      "Customized boxing forbids unboxing (and even boxing) this value type: "                     \
      "'BoxTraits<T>::Type == NotBoxable'!"       );                                               \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule4,  isBlockedTP  && isPointer,                               \
      "Customized boxing forbids unboxing (and even boxing) this pointer type: "                   \
      "'BoxTraits<T*>::Type == NotBoxable'!"      );                                               \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule5,  isBlockedTV  && !isCustomizedTP  &&  isPointer,          \
      "Customized boxing forbids unboxing (and even boxing) value type T "                         \
      "(BoxTraits<T>::Type == NotBoxable), while no customization for this pointer type T* "       \
      "was given."                              );                                                 \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule6,  isBlockedTP  && !isCustomizedTV  &&  isValue,            \
      "Customized boxing forbids unboxing (and even boxing) pointer type T* "                      \
      "(BoxTraits<T*>::Type == NotBoxable), while no customization for this value type T was"      \
      "given."                                  );                                                 \
                                                                                                   \
    /* Unboxing locked */                                                                          \
    ALIB_STATIC_DENY( CustomBoxingRule7,  isLockedTV   && isValue,                                 \
      "Customized boxing forbids unboxing this value type: "                                       \
      "'BoxTraits<T>::Read' returns a different type."                                    );       \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule8,  isLockedTP   && isPointer,                               \
      "Customized boxing forbids unboxing this pointer type: "                                     \
      "'BoxTraits<T*>::Read' returns a different type."                                   );       \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule9,  isLockedTV   && !isCustomizedTP  &&  isPointer,          \
      "Customized boxing forbids unboxing value type T "                                           \
      "('BoxTraits<T>::Read' returns a different type), while no customization for this pointer "  \
      "type T* was given."                                                                );       \
                                                                                                   \
    ALIB_STATIC_DENY( CustomBoxingRule10,  isLockedTP   && !isCustomizedTV  &&  isValue,           \
      "Customized boxing forbids unboxing pointer type T* "                                        \
      "('BoxTraits<T*>::Read' returns a different type), while no customization for this value "   \
      "type T was given."                                                                 );       \

#endif // !DOXYGEN

    /// Shortcut inline method to retrieve the vtable singleton for the template type.
    /// In debug-compilations, the received \e vtable is checked for being registered.
    ///
    /// @tparam TBoxable  The source type to receive the vtable for.
    /// @return The vtable singleton.
    template<typename TBoxable>
    static constexpr detail::VTable* getVTable()
    {
        using TCV= std::remove_cv_t<TBoxable>;
        // not customized?
        if constexpr (std::same_as<typename BoxTraits<TCV>::Mapping, DefaultBoxingTag>)
            return VTableOptimizationTraits<TCV, false>::Get();

        // customized
        return VTableOptimizationTraits<typename BoxTraits<TCV>::Mapping,
                                                 BoxTraits<TCV>::IsArray>::Get();
    }

    /// Helper method that writes data into the placeholder.
    /// @tparam T   The source type to receive the data for.
    /// @param  src The source object to box.
    template<typename T>
    constexpr void initPH(const T& src)                                                     noexcept
    { BoxTraits<std::remove_cv_t<T>>::Write( data, const_cast<T const &>( src ) ); }


  // #############################################################################################
  // Constructors
  // #############################################################################################
  public:

    /// The type of type codes received with #ExportType.
    using    TypeCode= uinteger;

    /// Default constructor.<br>
    /// After creation with this constructor, a call to #IsType<void> returns true.
    /// To reset an instance previously used, assign keyword \c nullptr.
    Box()                           noexcept
    : vtable( nullptr )                                                                           {}

    /// Constructor accepting previously exported values.
    /// @param typeCode     The type code this box will be set to.
    /// @param placeholder  The data this box will be set to.
    Box( TypeCode typeCode, const Placeholder& placeholder  )                               noexcept
    : vtable(reinterpret_cast<detail::VTable*>(typeCode))
    , data  (placeholder)                                                                         {}

  #if DOXYGEN
    /// Constructor to fetch any type of object.<br>
    /// Internally, this constructor is implemented using a set of different constructors
    /// which are selected by the compiler using keyword \c requires.
    ///
    /// Types derived from class \b %Box itself are boxed by copying the internal values
    /// of the box. This means that boxing objects of derived types is similar to
    /// "downcasting" the object to class \b %Box.
    ///
    /// @tparam TBoxable  Any C++ type to be boxed.
    /// @param  src       The src value or pointer type \c T.
    template <typename TBoxable>
    inline  constexpr Box(const  TBoxable& src ) noexcept;
  #else
    // ##################################   Special types   #######################################

    // Keyword 'nullptr'
    constexpr Box(const std::nullptr_t& )                              noexcept : vtable(nullptr) {}

    // C++ arrays
    template<typename T>
    requires std::is_array_v<T>
    constexpr Box( T& src )                                                                 noexcept
    {
        using TElem= std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;
        vtable= VTableOptimizationTraits<TElem, true>::Get();

        constexpr integer length= characters::IsCharacter<TElem> ? std::extent<T>::value - 1
                                                                 : std::extent<T>::value;
        data  = Placeholder( &src, length );
    }

    // Derived Box value types (like copy constructor but fetches derived types)
    template<typename T>
    requires (  std::is_base_of<Box, std::remove_cv_t<T>>::value  )
    constexpr Box( const T& src )                                                           noexcept
    : vtable( src.vtable )
    , data  ( src.data   )                                                                        {}

    // ##########################     Boxing with BoxTraits   ##################################

    // 0) Strings
    template<typename T>
    requires ( IsStringType<std::remove_cv_t<T>> )
    constexpr Box( const T& src )                                                         noexcept {

        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, true)
        if constexpr ( characters::ArrayTraits  <T, nchar>::Access == characters::Policy::Implicit )
        {
            vtable= VTableOptimizationTraits<nchar, true>::Get();
            data  = Placeholder( characters::ArrayTraits<std::remove_cv_t<T>, nchar>::Buffer( src ),
                                 characters::ArrayTraits<std::remove_cv_t<T>, nchar>::Length( src )  );
        }
        else if constexpr ( characters::ArrayTraits  <T, wchar>::Access == characters::Policy::Implicit )
        {
            vtable= VTableOptimizationTraits<wchar, true>::Get();
            data  = Placeholder( characters::ArrayTraits<std::remove_cv_t<T>, wchar>::Buffer( src ),
                                 characters::ArrayTraits<std::remove_cv_t<T>, wchar>::Length( src )  );
        }
        else if constexpr ( characters::ArrayTraits  <T, xchar>::Access == characters::Policy::Implicit )
        {
            vtable= VTableOptimizationTraits<xchar, true>::Get();
            data  = Placeholder( characters::ArrayTraits<std::remove_cv_t<T>, xchar>::Buffer( src ),
                                 characters::ArrayTraits<std::remove_cv_t<T>, xchar>::Length( src )  );
        }

    }

    // 1) Value remains value
    template<typename T>
    requires (    !std::is_pointer_v<T>
               && !IsStringType<std::remove_cv_t<T>>
               && (       IsCustomized<std::decay_t<T> >
                    || ( !IsCustomized<std::decay_t<T>*>  && IsStdPH<T>  ) )
               )
    constexpr Box( const T& src )                                                         noexcept {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, true)
        vtable= getVTable<T>();
        initPH( src );
    }

    // 2) Value converted to pointer
    template<typename T>
    requires(     !std::is_pointer_v<T>
               && !IsStringType<std::remove_cv_t<T>>
               && !std::is_array_v<T>
               && !std::is_base_of_v<Box, T>
               && !IsCustomized<std::decay_t<T> >
               && (        IsCustomized<std::remove_cv_t<T> >
                     || ( !IsCustomized<std::decay_t<T>*> && !IsStdPH<T>    )
                     || (  IsCustomized<std::decay_t<T>*>  )
                   )
            )
    constexpr Box( const T& src )                                                         noexcept {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, true)
        vtable= getVTable<std::remove_cv_t<T>*>();
        initPH( &src );
    }

    // 3) Pointer remains pointer
    template<typename T>
    requires (     std::is_pointer_v<T>
               && !std::is_base_of_v<Box, ALIB_TVALUE(T)>
               &&
               (    IsCustomized<std::remove_cv_t<T>>
                 || !( IsCustomized<ALIB_TVALUE(T)> || IsStdPH<T> )
               ))
    constexpr Box( const T& src )                                                         noexcept {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, false)
        vtable= getVTable<T>();
        initPH( src );
    }

    // 4) Pointer dereferenced to value
    template<typename T>
    requires (     std::is_pointer_v<T>
               && !std::is_base_of_v<Box, ALIB_TVALUE(T)>
               && !IsStringType<std::remove_cv_t<T>>
               && !
               (    IsCustomized<std::remove_cv_t<T>>
                 || !( IsCustomized<ALIB_TVALUE(T)> || IsStdPH<T> )
               )
               )
    constexpr Box( const T& src )                                                         noexcept {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING(T, false)
        using TV= ALIB_TVALUE(T);
        vtable= getVTable<TV>();
        if ( src ) initPH( *src );
        else       data  = Placeholder( sizeof(TV) <= sizeof(integer)
                           ? Placeholder( 0 )
                           : Placeholder( integer(0), integer(0) )              );
    }


    #undef ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_BOXING
    #undef ALIB_TM_IS_DEFAULT_BOXING

  #endif // DOXYGEN

  // #############################################################################################
  // Interface
  // #############################################################################################
    #if ALIB_DEBUG
        /// Returns the \e vtable of this instance that is associated with the currently boxed
        /// type.<br>
        /// Available only with debug-builds.
        ///
        /// \see
        ///    Manual chapter \ref alib_boxing_more_debug of the Programmer's Manual.
        ///
        /// @return The \e vtable of this instance.
        const detail::VTable*    DbgGetVTable()                             const { return vtable; }

    #endif

  #if DOXYGEN
    /// Checks if this box stores a value of type \p{TBoxable}.
    ///
    /// If template parameter \p{TBoxable} it is not unboxable, a compile-time assertion
    /// is given, with specific guidance why the type must not be unboxed and for that
    /// reason must not even be tested for.
    ///
    /// Special type \c void may be given for testing if this box does contain a value at all.
    /// A box does not contain a value, after
    /// - default construction,
    /// - construction with keyword \c nullptr, or
    /// - assignment of  keyword \c nullptr.
    ///
    /// For more information on the "void state" of boxes, see manual chapter
    /// \ref alib_boxing_more_void.
    ///
    ///
    /// @tparam TBoxable  The boxable type guessed.
    /// @return \c true if this box stores a value that is convertible to type \p{TBoxable},
    ///         \c false otherwise.
    template<typename TBoxable>
    bool IsType()                                                                             const;

    #else
        template<typename TBoxable>
        requires (  IsUnboxableStringType<TBoxable>)
        bool IsType()                                                                          const
        {
        if constexpr ( characters::ArrayTraits  <TBoxable, nchar>::Access == characters::Policy::Implicit )
            return vtable == VTableOptimizationTraits<nchar, true>::Get();

        if constexpr ( characters::ArrayTraits  <TBoxable, wchar>::Access == characters::Policy::Implicit )
            return vtable == VTableOptimizationTraits<wchar, true>::Get();

        if constexpr ( characters::ArrayTraits  <TBoxable, xchar>::Access == characters::Policy::Implicit )
            return vtable == VTableOptimizationTraits<xchar, true>::Get();
        }

        template<typename TBoxable>
        requires (   !std::same_as<TBoxable, void>
                  && !IsUnboxableStringType<TBoxable>)
        bool IsType()                                                                          const
        {
            ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TBoxable)
            return vtable == getVTable<TBoxable>();
        }

        template<typename TBoxable>
        requires std::same_as<TBoxable, void>
        bool IsType()                                        const { return vtable == nullptr; }
    #endif

    #if !ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS   || DOXYGEN
        //======================================================================================
        /// Tests if this box contains a signed integral type (one of the C++ fundamental
        /// types of different sizes).
        ///
        /// With compilation that disables
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
        /// simply returns <c>IsType<integer>()</c>.<br>
        /// Otherwise this method will not be inlined and tests for the five different
        /// integer sizes (\c int8_t, \c int16_t, \c int32_t, \c int64_t, and \alib{intGap_t}).
        ///
        /// @return \c true if this box contains a signed integral type, \c false otherwise.
        //======================================================================================
        bool        IsSignedIntegral()                      const { return IsType<integer >(); }

        //======================================================================================
        /// Tests if this box contains an unsigned integral type (one of the C++ fundamental
        /// type of different sizes).
        ///
        /// With default library compilation that disables
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
        /// simply returns <c>IsType<uinteger>()</c>.<br>
        /// Otherwise this method will not be inlined and tests for the five different
        /// integer sizes (\c uint8_t, \c uint16_t, \c uint32_t, \c uint64_t, and
        /// \alib{uintGap_t}).
        ///
        /// @return \c true if this box contains an unsigned integral type, \c false otherwise.
        //======================================================================================
        bool        IsUnsignedIntegral()                    const { return IsType<uinteger>(); }

        //======================================================================================
        /// Unboxes a signed integral.
        ///
        /// With default library compilation that disables
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
        /// simply returns <c>Unbox<integer>()</c>.<br>
        /// Otherwise this method will not be inlined and tests for the five different
        /// integer sizes (1, 2, 4, and 8 bytes size and the #alib::intGap_t) before
        /// unboxing.
        ///
        /// @return The boxed signed integral value.
        //======================================================================================
        integer   UnboxSignedIntegral()                                                        const
        {
            return Unbox<integer >();
        }

        //======================================================================================
        /// Unboxes an unsigned integral.
        ///
        /// With default library compilation that disables
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS, this method will be inlined and
        /// simply returns <c>Unbox<uinteger>()</c>.<br>
        /// Otherwise this method will not be inlined and tests for the five different
        /// integer sizes (1, 2, 4, and 8 bytes size and the #alib::uintGap_t) before
        /// unboxing.
        ///
        /// @return The boxed unsigned integral value.
        //======================================================================================
        uinteger  UnboxUnsignedIntegral()     const
        {
            return Unbox<uinteger >();
        }
    #else
        ALIB_DLL bool            IsSignedIntegral()           const;
        ALIB_DLL bool          IsUnsignedIntegral()           const;
        ALIB_DLL integer      UnboxSignedIntegral()           const;
        ALIB_DLL uinteger   UnboxUnsignedIntegral()           const;
    #endif


    #if !ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS  || DOXYGEN
        /// Tests if this box contains one of types \c char, \c wchar_t, \c char8_t, \c char16_t,
        /// or \c char32_t.
        ///
        /// With default library compilation that disables symbol
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS, this method will be inlined and
        /// simply returns <c>IsType<wchar>()</c>.<br>
        /// Otherwise, this method will not be inlined and tests for all five different
        /// character types.
        ///
        /// @return \c true if this box contains a character type, \c false otherwise.
        bool            IsCharacter()                                                          const
        {
            return IsType<wchar>();
        }

        /// Unboxes one of the types \c char, \c wchar_t, \c char8_t, \c char16_t, or \c char32_t
        /// and converts it to \alib{characters;wchar}.
        ///
        /// With default library compilation that disables
        /// \ref ALIB_FEAT_BOXING_BIJECTIVE_CHARACTERS, this method will be inlined and
        /// simply returns <c>Unbox<wchar>()</c>.<br>
        /// Otherwise, this method will not be inlined and tests for the five different
        /// character types before unboxing.
        ///
        /// @return The stored character.
        wchar           UnboxCharacter()                                                       const
        {
            return Unbox<wchar>();
        }

    #else
        ALIB_DLL bool   IsCharacter()                                                         const;
        ALIB_DLL wchar  UnboxCharacter()                                                      const;
    #endif


    /// Tests if this box contains a floating point type.
    ///
    /// \note
    ///   If \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS is not set, this method will
    ///   test against \c double and <c>long double</c>. If it is set, in addition
    ///   type \c float is tested.
    ///
    /// @return \c true if this box contains a floating point type, \c false otherwise.
    bool            IsFloatingPoint()                                                         const;

    /// Unboxes a floating point value as \c double.
    ///
    /// \note
    ///   If \ref ALIB_FEAT_BOXING_BIJECTIVE_FLOATS is not set, this method will
    ///   test against \c double and <c>long double</c> and convert the latter.
    ///   If it is set, in addition type \c float is tested.
    ///
    /// @return \c true if this box contains a floating point type, \c false otherwise.
    ALIB_DLL
    double          UnboxFloatingPoint()                                                      const;

    /// Returns \c true if this box represents an array of objects.
    /// In this case, method #UnboxLength (usually) will return the length of the array and
    /// #UnboxElement may be used to access elements of the array.
    ///
    /// @return \c true if this box represents an array, \c false otherwise.
    bool            IsArray()                                                                  const
    {
        return vtable && vtable->IsArray();
    }

    /// Returns \c true if this objects represents an array and the element type
    /// equals template parameter \p{TElementType}.
    ///
    /// @tparam TElementType The array element type to compare our element type with.
    /// @return \c true if this box represents an array of given type, \c false otherwise.
    template<typename TElementType>
    bool            IsArrayOf()                                                                const
    {
        return      vtable && typeid(TElementType) == vtable->ElementType;
    }

    /// Returns \c true if this box uses pointer-boxing, otherwise \c false.
    /// The default boxing of pointers will store the pointer to the boxed object
    /// in union \alib{boxing;Placeholder::VoidP}.
    ///
    /// @return \c true if this box contains an object boxed as pointer type, \c false otherwise.
    bool            IsPointer()                                                                const
    {
        return vtable && vtable->IsPointer();
    }

    /// Returns \c true if this box contains an element of a scoped or non-scoped enum type.
    /// Enum element values are always boxed as type \alib{integer} stored in
    /// union field \alib{boxing;Placeholder}.
    ///
    /// @return \c true if this box contains an object boxed as pointer type, \c false otherwise.
    bool            IsEnum()                                                                   const
    {
        return vtable && vtable->IsEnum();
    }

    /// Returns \c true if \p{other} and this object share the same boxed type.
    /// Note, if this box has void state (no value boxed), then this method returns
    /// \c false, even if given \p{other} is void state as well.
    ///
    /// @param other The box to compare our type with.
    /// @return \c true if this box has the same type like \p{other}, \c false otherwise.
    bool            IsSameType(const Box& other)                                               const
    {
        return  vtable && vtable == other.vtable;
    }


    /// This overload unboxes character string types that meet the concept
    /// \alib{boxing;IsUnboxableStringType}.
    /// This is done by testing the availability of \alib{characters;ArrayTraits} (provided with
    /// module \alib_characters) for the given type \p{TString} and invoking its method
    /// \alib{characters::ArrayTraits;Construct} passing the boxed character-array data.
    ///
    /// @tparam TValue The string-type to unbox.
    ///                 If it is not unboxable, a compile-time assertion is raised.
    /// @return The unboxed string instance.
    template<typename TValue>
    requires IsUnboxableStringType<TValue>
    TValue          Unbox()                                                                const
    {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TValue)

        ALIB_ASSERT_ERROR( vtable, "BOXING","Box not initialized. Unboxing is undefined behavior." )
        ALIB_ASSERT_ERROR( IsArrayOf<nchar>() || IsArrayOf<wchar>() || IsArrayOf<xchar>(), "BOXING",
           "Cannot unbox string-type <{}> from mapped type <{}>.", &typeid(TValue), &vtable->Type )
        debug::DbgCheckRegistration( vtable, true );

        if constexpr ( characters::ArrayTraits<TValue, nchar>::Construction == characters::Policy::Implicit )
            return     characters::ArrayTraits<TValue, nchar>::Construct( data.GetPointer<nchar>(), data.GetLength() );

        if constexpr ( characters::ArrayTraits<TValue, wchar>::Construction == characters::Policy::Implicit )
            return     characters::ArrayTraits<TValue, wchar>::Construct( data.GetPointer<wchar>(), data.GetLength() );

        if constexpr ( characters::ArrayTraits<TValue, xchar>::Construction == characters::Policy::Implicit )
            return     characters::ArrayTraits<TValue, xchar>::Construct( data.GetPointer<xchar>(), data.GetLength() );
    }

    /// Creates a value of type \p{TBoxable} from the contents of this box .
    /// By default, this is done by invoking the template method \alib{boxing;Placeholder::Read}
    /// on field #data.
    /// This behavior might be customized by specializing type trait \alib{boxing;BoxTraits}.
    ///
    /// This overload of the method accepts only (non-const) value types.
    /// For details on unboxing values and pointers, see chapter
    /// \ref alib_boxing_classes_constant of the Programmer's Manual of this module \alib_boxing_nl.
    ///   
    /// With debug-builds, it is \ref ALIB_ASSERT_ERROR "asserted" that given \p{TBoxable}
    /// is mapped to the type stored, so that #IsType returned \c true for \p{TBoxable}.
    /// In release compilations, no checks are performed!
    ///
    /// @tparam TValue     The value-type to unbox.
    ///                    If it is not unboxable, a compile-time assertion is raised.
    /// @return The unboxed value.
    template<typename TValue>
    requires (    !std::is_pointer_v<TValue>
               && !IsUnboxableStringType<TValue> )
    TValue          Unbox()                                                                const
    {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TValue)

        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior." )
        ALIB_ASSERT_ERROR( vtable == getVTable<TValue>(), "BOXING",
           "Cannot unbox type <{}> from mapped type <{}>.", &typeid(TValue), &vtable->Type )

        debug::DbgCheckRegistration( vtable, true );
        return BoxTraits<TValue>::Read(data);
    }

    /// Returns a pointer to a constant instance of type \p{TPointer} that this box stored.
    /// By default, this is done by invoking the template method \alib{boxing;Placeholder::Read}
    /// on field #data.
    /// This behavior might be customized by specializing type trait \alib{boxing;BoxTraits}.
    ///
    /// This overload of the method accepts only (non-const) pointer-types.
    /// For details on unboxing values and pointers, see chapter
    /// \ref alib_boxing_classes_constant of the Programmer's Manual of this module \alib_boxing_nl.
    ///
    /// With debug-builds, it is \ref alib_mod_assert "asserted" that given \p{TBoxable}
    /// is mapped to the type stored, so that #IsType returned \c true for \p{TBoxable}.
    /// In release compilations, no checks are performed!
    ///
    /// @tparam TPointer   The pointer-type to unbox.
    ///                    If it is not unboxable, a compile-time assertion is raised.
    /// @return The unboxed pointer to a constant instance of \p{TPointer}.
    template<typename TPointer>
    requires (     std::is_pointer_v<TPointer>
               && !IsUnboxableStringType<TPointer> )
    const std::remove_pointer_t<TPointer>*    Unbox()                                         const
    {
        ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING(TPointer)

        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior.")
        ALIB_ASSERT_ERROR( vtable == getVTable<TPointer>(),
             "Cannot unbox type <{}> from mapped type <{}>.", &typeid(TPointer), &vtable->Type )
        debug::DbgCheckRegistration( vtable, true );
        return BoxTraits<TPointer>::Read(data);
    }

    #undef ALIB_TEMPINTERNAL_STATIC_TYPE_CHECKS_UNBOXING

    /// Convenient method to unbox types boxed as pointers, as a non-<c>const</c> pointer type.
    ///
    /// \see Refer to manual chapter \ref alib_boxing_classes_constant for more information.
    ///
    /// @tparam TPointer The type to unbox.
    ///                  If it is not unboxable, a compile-time assertion is given.
    /// @return The unboxed value of type \p{TPointer} cast to a non-<c>const</c> object.
    template <typename TPointer>
    requires std::is_pointer_v<TPointer>
    TPointer
    UnboxMutable()                                                                             const
    { return const_cast<std::remove_const_t<TPointer>>( Unbox<TPointer>() ); }

    /// Returns the "raw" placeholder of this box.
    ///
    /// In some special situations, this method might be used inspect into the boxed data and
    /// "reinterpret" its contents in a custom way.
    ///
    /// @return The raw contents of this box.
    const Placeholder&  Data()                                                                 const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Cannot access placeholder." )
        return data;
    }

    /// Non-constant variant of #Data, that allows write access to the internal
    /// memory of this box.
    ///
    /// A use case for non-constant access could be the implementation of a
    /// \ref alib_boxing_functions_mutable "non-constant box-function".
    /// In fact, this is the only occasion where within any \alibmod_nl this method was
    /// needed.
    ///
    /// @return The raw contents of this box.
    Placeholder&        Data()
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Cannot access placeholder." )
        return data;
    }

    /// Returns the number of relevant bytes used in the placeholder.
    ///
    /// This method is used with default implementations of box-functions \alib{boxing;FHashcode}
    /// and \alib{boxing;FEquals}.
    ///
    /// \see
    ///   The documentation of \alib{boxing;SizeTraits} provides details on and rationals for
    ///   the existence of this method.
    ///
    /// @return The raw contents of this box.
    unsigned int        GetPlaceholderUsageLength()                                            const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized." )
        return vtable->PlaceholderUsage;
    }

    /// Returns the type of this box as an integral value. This value might be stored and used
    /// to compare types of boxes later.
    ///
    /// @return An identifier of type of this box.
    TypeCode                ExportType()                     const { return  uinteger(vtable); }

    /// Returns the type of this box as an integral value. This value might be stored and used
    /// to compare types of boxes later.
    ///
    /// \note
    ///   This method is provided for "completeness" and only be used in special situations.<br>
    ///   If a box is not initialized (or has \c nullptr assigned, <c>0</c> is
    ///   returned.
    ///
    /// @return An identifier of type of this box.
    Placeholder             ExportValue()                                 const { return data; }

    /// Changes this box to use the given type code previously exported with #ExportType.
    /// The value of this box will be set to \c 0.
    /// @param typeCode The type code this box will be set to.
    void                    Import(TypeCode typeCode)
    {
        vtable= reinterpret_cast<detail::VTable*>(typeCode);
        data.Array.Pointer= nullptr;
        data.Array.Length = 0;
    }

    /// Changes this box to use the given type and data, previously received with methods
    /// #ExportType and ExportValue.
    ///
    /// @param typeCode     The type code this box will be set to.
    /// @param placeholder  The data this box will be set to.
    void                    Import(TypeCode typeCode, const Placeholder& placeholder )
    {
        vtable= reinterpret_cast<detail::VTable*>(typeCode);
        data=   placeholder;
    }

    /// Returns the \c std::type_info struct describing the boxed type.
    /// To get the element type of boxed arrays, use #ElementTypeID.
    ///
    /// \note
    ///   This method is provided for "completeness" and only be used in special situations.
    ///
    /// \note
    ///   If a box is not initialized (or has \c nullptr assigned, <c>typeid(void)</c> is
    ///   returned.
    ///
    /// \note
    ///   In case of arrays, a \c std::type_info reference is returned that corresponds
    ///   to an array of the element type of size \c 1. For example, if an array of type
    ///   \c double of an arbitrary size was boxed, then <c>typeid(double[1])</c>is returned.
    ///
    /// @return The \c std::type_info of the mapped type.
    const std::type_info&   TypeID()                                                           const
    {
        debug::DbgCheckRegistration( vtable, true );
        return  vtable ? vtable->Type : typeid(void);
    }

    /// Returns the \c std::type_info struct describing the element type of mapped array types.
    ///
    /// \note
    ///   This method is provided for "completeness" and only be used in special situations.<br>
    ///   In case this box is not of array type, <c>typeid(void)</c> is returned.
    ///
    /// @return The \c std::type_info of the mapped type.
    const std::type_info&   ElementTypeID()                                                    const
    {
        ALIB_ASSERT_ERROR( vtable   , "BOXING", "Box not initialized. Cannot get type information.")
        return  vtable->ElementType;
    }

    /// Returns the size in bytes of on element of the stored array.
    /// For non-array types, \c 0 is returned.
    ///
    /// @return The size of elements in the array.
    size_t           ArrayElementSize()                                                        const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior.")
        return vtable->Mapping > 0 ? size_t( vtable->Mapping )
                                   : 0;
    }

    /// Returns the pointer to the first array element.
    ///
    /// \note
    ///   With debug-builds, it is \ref ALIB_ASSERT_ERROR "asserted" that #IsArray
    ///   returns \c true and the stored type is the same as requested.
    ///   In release compilations, no checks are performed!
    ///
    /// @tparam TElementType The type of array elements
    /// @return A pointer to the first array element.
    template <typename TElementType>
    TElementType*    UnboxArray()           const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Unboxing is undefined behavior.")
        ALIB_ASSERT_ERROR( IsArray(), "BOXING",
          "Box::UnboxArray() invoked on box of non-array type <{}>.", &vtable->Type )

        ALIB_ASSERT_ERROR( typeid(TElementType) == vtable->ElementType,
          "BOXING: Cannot unbox array type<{}[]> from mapped type<{}[]>.",
          &typeid(TElementType*), &vtable->ElementType )

        debug::DbgCheckRegistration( vtable, true );
        return data.GetPointer<TElementType>();
    }

    /// Returns the length of a boxed Array. While in theory, the length applies only to
    /// arrays, in debug-compilations, \b no run-time type check is performed.
    /// This way, mapped types that use the second "word" of the placeholder to store a
    /// value of type \c integer, may also use this function.<br>
    /// In the latter case, the name of this method might be misleading and therefore, it is
    /// recommended to use <b>Data().integer[1]</b> to denote that a custom interpretation of the
    /// placeholder is performed. The compilation result is the same.
    ///
    /// Some quick rationale for why \alib is generally using signed types for array lengths,
    /// is given \ref alib_strings_details_signedlength "here".
    ///
    /// @return The length of the boxed object.
    integer         UnboxLength()                                                              const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING", "Box not initialized. Cannot access placeholder." )
        return data.GetLength();
    }

    /// Returns a reference to element \p{idx} of the boxed array.
    ///
    /// \note
    ///   With debug-builds, it is \ref alib_mod_assert "asserted" that #IsArray returns
    ///   \c true, that the stored type is the same as the requested type and the provided
    ///   \p{idx} is between \c 0 and the length of the array.
    ///   In release compilations, no checks are performed!
    ///
    /// @tparam TElementType The type of array elements
    /// @param  idx      The index of the element to receive.
    /// @return The value of the element at \p{idx}.
    template <typename TElementType>
    TElementType&    UnboxElement(integer idx)           const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING",
            "Box is void (no contents). Unboxing is undefined behavior." )
        ALIB_ASSERT_ERROR( IsArray(), "BOXING",
            "Box::UnboxElement() invoked on box of non-array type <{}>.", &vtable->Type )

        ALIB_ASSERT_ERROR( typeid(TElementType) == vtable->ElementType,
          "BOXING: Cannot unbox array element type <{}> from mapped type <{}[]>.",
          &typeid(TElementType), &vtable->ElementType )

        ALIB_ASSERT_ERROR( idx >= 0 && idx < UnboxLength(), "BOXING",
          "Box::UnboxElement<{}>(): Index out of bounds.", &typeid(TElementType))

        debug::DbgCheckRegistration( vtable, true );

        return *( data.GetPointer<TElementType>()  + idx );
    }

    #if DOXYGEN
    /// Searches an implementation  of a box-function identified by template parameter
    /// \p{TFDecl}, which has to be implemented according the rules of
    /// \ref alib_boxing_functions_concepts_decl "function declarations".<br>
    /// If found, a <em>non-nulled</em> function pointer is returned, otherwise a \e nulled one.
    ///
    /// On success, the function can be invoked by passing the returned pointer to method
    /// #CallDirect.
    /// This approach avoids further searches that are otherwise to be performed with multiple
    /// invocations of method #Call.
    ///
    /// If parameter \p{defaults} equals \alib{lang;Reach::Local}, functions specific to the mapped
    /// type of this box (registered using \alib{boxing;BootstrapRegister}) are searched.
    /// If \alib{lang;Reach::Global} is given, then a defaulted function (registered using
    /// \alib{boxing;BootstrapRegisterDefault}) is searched, if no specific function was found.
    ///
    /// \note
    ///   \alib{lang;Reach::Local} can be used to detect specific behavior and to avoid the use
    ///   of default functions. This can be useful if the default implementation of a function
    ///   is just not applicable in a certain situation.
    ///
    /// \note
    ///   A second use case of this method are situations where multiple invocations of the
    ///   same function are to be done, on just one or on several boxes of the same mapped type:
    ///
    ///        assert( box1.IsSameType( box2 ) );
    ///
    ///        auto* func= box1.GetFunction<FMyFunc>( Reach::Global );
    ///        if( func != nullptr )
    ///            for( int i= 0; i< 10; ++i )
    ///            {
    ///                box1.CallDirect<FMyFunc>( func, i );
    ///                box2.CallDirect<FMyFunc>( func, i );
    ///            }
    ///
    /// @tparam TFDecl       The \ref alib_boxing_functions_concepts_decl "function declaration"
    ///                      to search for.
    /// @param  searchScope  \alib{lang;Reach::Local} chooses type-specific functions only, while.
    ///                      \alib{lang;Reach::Global} includes default functions in the search.
    /// @param  isInvocation Available only in debug compilations. If \c true, a counter
    ///                      associated with an implementation found is increaed to provide
    ///                      statistics. Defaults to false and should not be given.
    /// @return The function implementation.
    ///         \c nullptr in case that no function is available.
    template <typename TFDecl>
    inline
    typename TFDecl::Signature    GetFunction( Reach searchScope
                                             , bool isInvocation = false  )                   const;
    #else
    template <typename TFDecl>
    typename TFDecl::Signature    GetFunction( lang::Reach searchScope
                                   ALIB_DBG( , bool isInvocation = false)    )                 const
    {
        if( !vtable )
            return nullptr;

         ALIB_DBG( ++vtable->DbgCntUsage );

        auto result= vtable->Functions.Get<TFDecl>( ALIB_DBG(isInvocation) );
        return    result
                ? result
                : searchScope == lang::Reach::Global
                ? detail::DEFAULT_FUNCTIONS.Get<TFDecl>( ALIB_DBG(isInvocation) )
                : nullptr;
    }
    #endif //DOXYGEN



    /// Invokes a function registered for boxes of the mapped type.
    /// The \ref alib_boxing_functions_concepts_decl "function declaration" is provided with the
    /// first template parameter \p{TFDecl}.
    /// The further variadic template parameters do not need to be specified.
    /// They specify the types of the called function's parameters and are matched against
    /// the function signature given with the declaration.
    /// If the types of the given function arguments do not correspond to the types of
    /// the box-function called, a compile-time error is raised.
    ///
    /// \note
    ///   Precisely, the variadic template types denote the function arguments starting from the
    ///   second, as the first argument is always a reference to the box that this method was
    ///   invoked on.
    ///
    /// If no corresponding function \alib{boxing;BootstrapRegister;was registered} for the mapped
    /// type, then a \alib{boxing;BootstrapRegisterDefault;default function}, that is applicable
    /// to any mapped type is searched.
    /// If neither is found, a default value of the return type of the function is returned.
    ///
    /// With debug-builds, an \ref alib_mod_assert "error is raised" if the function type is not
    /// known at all to \alib_boxing_nl. This is true, if an implementation was neither registered with
    /// any other mapped type, nor registered as a default.
    ///
    /// \see
    ///   Description of method #GetFunction to implement two use cases:
    ///   - Repetitive invocation of the same function.
    ///   - Avoidance of default functions
    ///
    /// \see
    ///   A non-constant overload exists, for the seldom case the reference to this box that is
    ///   passed to the function, needs to be of non-constant type.
    ///
    /// @tparam TFDecl  The function type to call. Has to be explicitly specified.
    /// @tparam TArgs   Types of the variadic arguments \p{args}. Do not need to be specified.
    /// @param  args    Variadic arguments forwarded to the function.
    /// @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
    ///         respectively <c>TReturn()</c> if the requested function type was not found for
    ///         this \b %Box.
    template <typename TFDecl,  typename... TArgs>
    decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
    Call(TArgs&&... args)                                                                      const
    {
        auto* func= GetFunction<TFDecl>( lang::Reach::Global    ALIB_DBG(, true) );
        if( func != nullptr )
            return reinterpret_cast<typename TFDecl::Signature>(func)
                   ( *this, std::forward<TArgs>(args)... );


        return  decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(),
                                                                      std::declval<TArgs>()... )) ();
    }

    /// Alternative version of method #Call, which accepts the function's pointer as a first
    /// argument. Such a pointer can be received upfront with method #GetFunction.
    ///
    /// @tparam TFDecl    The function type to call. Has to be explicitly specified.
    /// @tparam TArgs     Types of the variadic arguments \p{args}. Do not need to be specified.
    /// @param  args      Variadic arguments forwarded to the function.
    /// @param  function  The function to invoke.
    /// @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
    ///         respectively <c>TReturn()</c> if the requested function type was not found for
    ///         this \b %Box.
    template <typename TFDecl,  typename... TArgs>
    decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
    CallDirect(typename TFDecl::Signature function, TArgs&&... args)                      const
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING",
            "Box not initialized (does not contain value). Function call not allowed." )
        return reinterpret_cast<typename TFDecl::Signature>(function)
                   ( *this, std::forward<TArgs>(args)... );
    }

    /// Same as method #Call, but usable with interfaces that only accept a mutable
    /// (aka not constant) box. Technically, the only difference between this method and \b Call
    /// is that the latter is declared \c const.
    ///
    /// \note
    ///   The only built-in boxing function that requires a mutable reference
    ///   to a box, is function \alib{boxing;FClone}. This modifies the contents
    ///   of a box by performing deep copies, with the goal to
    ///   \ref alib_boxing_more_iclone "extent the lifecylce of boxes".
    ///
    /// @tparam TFDecl  The function type to call. Has to be explicitly specified.
    /// @tparam TArgs   Types of the variadic arguments \p{args}. Do not need to be specified.
    /// @param  args    Variadic arguments forwarded to the function.
    /// @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
    ///         respectively <c>TReturn()</c> if the requested function type was not found for
    ///         this \b %Box.
    template <typename TFDecl,  typename... TArgs>
    decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
    Call(TArgs&&... args)
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING",
            "Box not initialized (does not contain value). Function call not allowed." )
        auto* func= GetFunction<TFDecl>( lang::Reach::Global ALIB_DBG(, true));
        if( func != nullptr )
            return reinterpret_cast<typename TFDecl::Signature>(func)
                   ( *this, std::forward<TArgs>(args)... );

        return  decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(),
                                                                      std::declval<TArgs>()... )) ();
    }

    /// Alternative version of non-constant version of method #Call, which accepts the function's
    /// pointer as a first argument.
    /// Such a pointer can be received upfront with the method #GetFunction.
    ///
    /// @tparam TFDecl    The function type to call. Has to be explicitly specified.
    /// @tparam TArgs     Types of the variadic arguments \p{args}. Do not need to be specified.
    /// @param  args      Variadic arguments forwarded to the function.
    /// @param  function  The function to invoke.
    /// @return Nothing in case that \p{TReturn} is void, otherwise the result of the invocation,
    ///         respectively <c>TReturn()</c> if the requested function type was not found for
    ///         this \b %Box.
    template <typename TFDecl,  typename... TArgs>
    decltype( std::declval<typename TFDecl::Signature>()( std::declval<Box&>(), std::declval<TArgs>()... ) )
    CallDirect(typename TFDecl::Signature function, TArgs &&... args)
    {
        ALIB_ASSERT_ERROR( vtable, "BOXING",
            "Box not initialized (does not contain value). Function call not allowed." )
        return reinterpret_cast<typename TFDecl::Signature>( function )
                   ( *this, std::forward<TArgs>(args)... );
    }

    /// Comparison operator. Returns the result of invocation of built-in boxing function
    /// \alib{boxing;FEquals}.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    ALIB_DLL
    bool  operator==(Box const& rhs)                                                          const;

    /// Comparison operator. Returns the negated result of #operator==.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    bool  operator!=(const Box& rhs)                                                           const
    {
        return  ! ((*this) == rhs);
    }

    /// Comparison operator. Returns the result of invocation of built-in box-function
    /// \alib{boxing;FIsLess}.
    ///
    /// \see
    ///   Sample code provided with documentation of box-function \alib{boxing;FIsLess}.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
    ALIB_DLL
    bool operator< (Box const& rhs)                                                           const;

    /// Comparison operator. Uses a combination of \c operator< and \c operator==.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
    ALIB_DLL
    bool operator<=(Box const& rhs)                                                           const;

    /// Comparison operator. Uses a combination of \c operator< and \c operator==.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
    ALIB_DLL
    bool operator> (Box const& rhs)                                                           const;

    /// Comparison operator. Returns the negated result of \c operator<.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object is smaller than \p{rhs}, otherwise \c false.
    bool operator>= (Box const& rhs)                                                           const
    {
        return   !( (*this) < rhs);
    }

    /// Explicit cast operator to \c bool. Returns the result of built-in box-function
    /// \alib{boxing;FIsTrue}.
    ///
    /// @return \c true if the boxed value <em>represents value true</em>, \c false otherwise.
    ALIB_DLL
    explicit operator bool()                                                                  const;


    /// Returns the result of a call to built-in boxing function \alib{boxing;FIsNotNull}.
    ///
    /// @return \c false if this object represents a \e nulled object, \c true otherwise.
    ALIB_DLL
    bool   IsNotNull()                                                                        const;

    /// Returns the negated result of a call to built-in boxing function
    /// \alib{boxing;FIsNotNull}.
    ///
    /// @return \c true if this object represents a \e nulled object, \c false otherwise.
    bool     IsNull()                                                                          const
    {
        return  !IsNotNull();
    }

    /// Returns the result of invocation of built-in boxing function \alib{boxing;FHashcode}.
    ///
    /// @return A hashcode for the boxed type and value.
    ALIB_DLL size_t Hashcode()                                                                const;

  #if ALIB_MONOMEM
    /// Returns the result of invocation of built-in boxing function \alib{boxing;FHashcode}.
    ///
    /// \par Availability
    ///   This method is available only if the module \alib_monomem is included in the \alibbuild.
    /// @param memory  A monotonic allocator used for storing cloned data.
    ALIB_DLL void   Clone( MonoAllocator& memory );
  #endif

}; // class Box

} // namespace alib[::boxing]

/// Type alias in namespace \b alib.
using     Box           =   boxing::Box;
} // namespace [alib]

#include "alib/boxing/functiondefs.inl.inl"
#include "alib/boxing/boxingcustoms.inl.inl"

#include "ALib.Lang.CIFunctions.H"
ALIB_EXPORT namespace alib::boxing {
//==================================================================================================
/// Registers  \ref alib_boxing_functions "box-function" \p{function} of type \p{TFDecl} for
/// boxes of mapped type \p{TMapping}.
///
/// \attention
///   Function registration and function invocation are not protected against racing conditions
///   of multithreaded access. For this reason, it is advised to invoke this function exclusively
///   while \ref alib_mod_bs "bootstrapping" software, when no threads are started,
///   yet. Registrations can be made before bootstrapping \alib, respectively during or after
///   phase \alib{BootstrapPhases::PrepareResources}.
///
/// \attention
///   If for any reason registration is performed \b after bootstrapping \alib and module
///   \alib_monomem is included in the \alibbuild, and this function is invoked after
///   \alib was bootstrapped, then before an invocation of this method, mutex
///   \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired. This can be done with:
///           \snippet "ut_monomem.cpp"     DOX_MONOMEM_LOCK_GLOBALALLOCATOR
///
/// \attention
///   Note that even when this lock is set, still multithreaded access to registration and/or
///   box-function invocations is <b>not allowed</b>.
///
/// @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
/// @tparam TMapped   The mapped type that boxes store, which are to be equipped with a specialized
///                   function implementation.
/// @tparam TIsArray  Denotes whether array-boxing is applied. Defaults to \c false.
/// @param  function  Pointer to the function implementation.
//==================================================================================================
template<typename TFDecl, typename TMapped, bool TIsArray= false>
inline
void BootstrapRegister( typename TFDecl::Signature function )
{
    ALIB_ASSERT_ERROR(
        nullptr == VTableOptimizationTraits<TMapped ALIB_COMMA TIsArray>::Get()
                   ->Functions.template Get<TFDecl>(false),
        "BOXING", "Doubly defined function" )

    VTableOptimizationTraits<TMapped, TIsArray>::Get()
    ->Functions.template Set<TFDecl>( function );
}

//==================================================================================================
/// Registers a default implementation of a \ref alib_boxing_functions "box-function", which
/// is invoked if no type-specific implementation is registered for a mapped type.
///
/// \attention
///   Function registration and function invocation are not protected against racing conditions
///   of multithreaded access. For this reason, it is advised to invoke this function exclusively
///   while \ref alib_mod_bs "bootstrapping" software, when no threads are started,
///   yet. Registrations can be made before bootstrapping \alib, respectively during or after
///   phase \alib{BootstrapPhases::PrepareResources}.
///
/// \attention
///   If for any reason registration is performed \b after bootstrapping \alib and module
///   \alib_monomem is included in the \alibbuild, and this function is invoked after
///   \alib was bootstrapped, then, before an invocation of this method, mutex
///   \alib{monomem;GLOBAL_ALLOCATOR_LOCK} has to be acquired. This can be done with:
///           \snippet "ut_monomem.cpp"     DOX_MONOMEM_LOCK_GLOBALALLOCATOR
///
/// \attention
///   Note that even when this lock is set, still multithreaded access to registration and/or
///   box-function invocations is <b>not allowed</b>.
///
/// @tparam TFDecl    The \ref alib_boxing_functions_concepts_decl "type of function" to register.
/// @param  function  Pointer to the function's default implementation.
//==================================================================================================
template<typename TFDecl>
inline
void BootstrapRegisterDefault( typename TFDecl::Signature function )
{
    detail::DEFAULT_FUNCTIONS.Set<TFDecl>( function );
}
} // namespace [alib::boxing]





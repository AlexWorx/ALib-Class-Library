//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace boxing  {

/// This is an empty type used to denote that default boxing is active. The type is
/// used with the default implementation of the type trait \alib{boxing;BoxTraits}.
/// Providing this type in custom specializations, makes such specialization in-effective.
/// \see Templated struct \alib{boxing;BoxTraits} for more information.
struct DefaultBoxingTag  {};

/// This is an empty type used to denote that a type must not be boxed.
/// To disable \alib_boxing_nl for a custom type, a specialization of type-traits
/// struct \alib{boxing;BoxTraits} must use this type to define type alias
/// \alib{boxing;BoxTraits::Mapping}.
///
/// \see
///   Templated struct \alib{boxing;BoxTraits} for more information.
struct NotBoxableTag     {};


//==================================================================================================
/// This template struct is used to define customized behavior for boxing C++ type
/// \p{TBoxable}.
///
/// ### Default Boxing: ###
/// If this struct is \b not specialized for template type \p{TBoxable},
/// default boxing applies.
/// With that, values \b and pointers of a type are boxed in the same way:
/// - They are both boxed to a pointer of \p{TBoxable} if a value of the type does not "fit" into
///   a box's \alib{boxing;Placeholder} or if the type is not copy-constructible or not
///   trivially destructible.
/// - Otherwise, both are boxed as values, hence if a pointer is given to the constructor or
///   assign \c operator= of class \alib{boxing;Box}, indirection \c operator* is applied.
///
/// <br><p>
/// ###Custom Boxing With Specializations Of This Struct: ###
/// The default boxing briefly described above, can be manipulated by providing a specialization
/// of this struct.
/// All three entities of this default implementation have to be provided with such specializations:
///
/// 1. <b>Type alias #Mapping:</b><br>
///    This alias defines the type that a value of \p{TBoxable} is converted to when boxed.
///
///    If special type \alib{boxing;NotBoxableTag} is given, then boxing is disallowed for
///    type \p{TBoxed}.
///    In this case, only declarations of functions #Write and #Read need to be provided, as they
///    are never invoked.
///
///    The default implementation of this struct uses a second special type available,
///    \alib{boxing;DefaultBoxingTag} which denotes default boxing.
///
/// 2. <b>Boolean constexpr value #IsArray:</b><br>
///    If set to \c true, array-boxing is performed. In this case #Mapping denotes the element
///    type of the boxed C++ array.
///
/// 3. <b>Static Method #Write:</b><br>
///    This method is invoked for converting source values to mapped types.
///    Often the default implementation given in the default version of this struct is
///    applicable. In this case it can simply be copied to the specialization.
///
///    Custom implementations may use one of the overloads of method
///    \alib{boxing;Placeholder::Write} or alternatively write to the \c union members of the
///    placeholder directly.
///
/// 4. <b>Static Method #Read:</b><br>
///    This method is invoked for converting the placeholder data back to the boxed source type.
///    Often the default implementation given in the default version of this struct is
///    well suited. In this case it can simply be copied to the specialization.<br>
///    Custom implementations may use one of the overloads of method \alib{boxing;Placeholder::Read}
///    or alternatively read the \c union members of the placeholder directly.
///
///    A type becomes \alib{boxing;IsUnboxable;not unboxable} at the moment this function
///    returns a different type than \p{TBoxable}.<br>
///    This may well be intended, and thus the specialized version of this struct
///    may declare this method to return \c void.
///    In this case a declaration of the method is enough, as it will never be invoked.<br>
///    With conditional specializations of this struct (see  Programmer's Manual
///    chapter \ref alib_boxing_customizing_conditional), the return type might be likewise
///    conditionally set.
///    Such a return type then decides which of the types are unboxable and which are not.
///
/// \note
///   If a specialization uses default type mapping by providing<br>
///
///       using   Mapping=  DefaultBoxingTag;
/// \note
///   in theory, a mixture of default and custom boxing is in place!
///   Note, that the authors of this library and documentation have not seen a use case for this,
///   yet.
///
/// <br><p>
/// ### Helper Macros: ###
///
/// A set of macros for defining specializations exist.
/// The use of the macro is recommended, as besides being less error-prone, their use makes the code
/// more readable. Finally, chances are good that code that uses the macros remains compatible
/// with future versions of module \alib_boxing_nl.
///
/// All macros expect this struct's template type \p{TBoxable} as the first parameter, and most
/// expect the mapped type as the second parameter.
///
/// For more information, see the reference documentation of the macros, which are:
///
/// - \ref ALIB_BOXING_CUSTOMIZE
/// - \ref ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING
/// - \ref ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE
/// - \ref ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE
/// - \ref ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE
/// - \ref ALIB_BOXING_CUSTOMIZE_DENY_BOXING
///
/// <br><p>
/// ### Value Boxing and Nulled Pointers: ###
/// If a type is boxed as value (either with default boxing or custom boxing) and a \e nulled
/// pointer to that type is boxed, method \alib{boxing;Placeholder::Clear} is invoked
/// instead of this struct's method #Write.
///
/// <br><p>
/// ### Avoiding Seldom Compilation Errors: ###
/// For technical reasons, namely to avoid compilation problems, some conditional specialization
/// is internally made, that declare method #Read \c void. This is sometimes needed if a type is
/// not returnable, even if that becomes never be boxed or unboxed.
/// Such a situation might happen, for example, if class \b Box is part of a \c union.<br>
/// The specialization is made for the following types of \p{TBoxable}:
/// - C++ array types<br>
///   Those types are fetched by an overloaded constructor that does not (because it cannot)
///   leverage this struct. Likewise, they cannot be unboxed in the usual fashion.
/// - Function types (selected by <c>std::is_function<TBoxable>::value</c>).
///
/// If a dubious error message about method #Read not being able to return a certain type occurs,
/// even one that a user's code even "actively" tries to box or unbox, then it might be helpful
/// to specialize this struct for such type, without providing implementations of \b Write and
/// \b Read and with the latter to return \c void.
///
/// <br><p>
/// \see More explanation and sample code is given in chapter
///      \ref alib_boxing_customizing "7. Customizing Boxing" of the
///      \ref alib_mod_boxing "Programmer's Manual" of module \alib_boxing_nl.
///
/// @tparam TBoxable    The source type to customize boxing for.
//==================================================================================================
template<typename TBoxable>
struct BoxTraits
{
    /// Defines the mapped type.
    /// Special designator types \alib{boxing;DefaultBoxingTag} and
    /// \alib{boxing;NotBoxableTag} may be given to denote corresponding behavior.
    /// The default implementation specifies designator type \b DefaultBoxingTag, which
    /// disables custom boxing.
    using   Mapping=      DefaultBoxingTag;

    /// Denotes whether type \p{TBoxable} is boxed as an array-type or not.
    static constexpr bool IsArray= false;

    /// Used for boxing a value, precisely writing the boxable portion of a type into
    /// field \alib{boxing;Box::data}, which is given with parameter \p{box}.<br>
    /// The default implementation of this struct implements this method as follows:
    /// \snippet "boxing/boxingtraits.inl"  DOX_BOXING_T_BOXER_WRITE
    ///
    /// This implementation leverages the overloads \alib{boxing;Placeholder::Write}
    /// and is often all that is needed with custom specializations.
    /// @param box The placeholder of the destination box.
    /// @param value  The value to that is to be boxed.
    DOX_MARKER([DOX_BOXING_T_BOXER_WRITE])
    static constexpr void  Write( Placeholder& box, const TBoxable& value ) {
        box.Write( value );
    }
    DOX_MARKER([DOX_BOXING_T_BOXER_WRITE])

    /// Used for unboxing a value, precisely reading the contents of field \alib{boxing;Box::data},
    /// which is given with output parameter \p{box} and creating a value of type \p{TBoxable} from
    /// that.<br>
    /// The default implementation of this struct implements this method as follows:
    /// \snippet "boxing/boxingtraits.inl"  DOX_BOXING_T_BOXER_READ
    ///
    /// This implementation leverages the overloaded method \alib{boxing;Placeholder::Read}
    /// and is often all that is needed with custom specializations.
    ///
    /// If a different type than \p{TBoxable} is returned, then that source type is not unboxable.
    /// To intend such behavior, for example, because \p{TBoxable} is mapped to a reduced type
    /// and therefore unboxing is not possible, specializations may declare the return type to be
    /// \c void and omit a definition of this method.<br>
    /// With conditional customizations, also other return types may be given, which likewise denote
    /// an unboxable source type. A sample of that is given with Programmer's Manual chapter
    /// \ref alib_boxing_customizing_conditional.
    ///
    /// @param box   The Placeholder to unbox the data from
    /// @return \c The unboxed object.
    static
    std::conditional_t<!std::is_abstract<TBoxable>::value, TBoxable, TBoxable&>
    Read( const Placeholder& box)
    {
DOX_MARKER([DOX_BOXING_T_BOXER_READ])
return box.Read<TBoxable>();
DOX_MARKER([DOX_BOXING_T_BOXER_READ])
    }

}; // BoxTraits

// #######################      critical specializations of BoxTraits      ###########################
#if !DOXYGEN

// This is necessary for types that can't be used as return type in any way, for example
// types with extents, functions, etc.
template<typename TBoxable>
requires( std::is_array_v<TBoxable> || std::is_function<TBoxable>::value )
struct BoxTraits<TBoxable>
{
    using                   Mapping=  DefaultBoxingTag;
    static constexpr bool   IsArray=  std::is_array_v<TBoxable>;

    static void             Write( Placeholder& box,  TBoxable& value );
    static void             Read ( const Placeholder& box);
};

// void (needed to pass TMP stuff)
template<>
struct BoxTraits<void>
{
    using                   Mapping=  DefaultBoxingTag;
    static constexpr bool   IsArray=  false;
    static void             Write( Placeholder& box,  const void* value );
    static void*            Read ( const Placeholder& box);
};

#endif

//==================================================================================================
/// This specializable \c constexpr must be set for custom types, if the following applies:
/// - A custom boxing implements \alib{boxing;BoxTraits::Write} in a way that a different number
///   of bytes are used in union \alib{boxing;Placeholder} than the \c sizeof() operator reports
///   on the mapped type's size.
/// - if the standard copy constructor (used with default boxing) writes a different size.
/// - One of the above and no specialized version of both box-functions \alib{boxing;FHashcode}
///   or \alib{boxing;FEquals} are set.
///
/// <b>Background:</b><br>
/// The default implementations of \alib{boxing;FHashcode} and \alib{boxing;FEquals}
/// must use the first N "relevant" bytes of the placeholder only. The non-relevant bytes are
/// not written and therefore must not be taken into account.<br>
/// To receive the number of relevant bytes, they invoke
/// \alib{boxing;Box::GetPlaceholderUsageLength} (at run-time).
/// This value is set at compile-time with the creation of a mapped type's \e vtable.
/// While for array types, the value is set to the overall size of union \alib{boxing;Placeholder},
/// for non-array types, the value of this type trait is used.
///
/// It might be surprising, but a built-in specialization exists for even a C++ fundamental type
/// <c>long double</c>, which is dependent on the compiler/platform.
/// For example, on GNU/Linux 64-bit, GCC reports \c 16 with <c>sizeof(long double)</c>.
/// However, if a <c>long double</c> value is copied, e.g with:
///
///      *pointerToLongDouble= 3.14L;
///
/// then only 10 bytes are written. The reason for this is that <c>sizeof</c> reports the size
/// needed for alignment when placed in an array of that type.
/// In the case of \alib_boxing_nl, 6 bytes of the placeholder remain random and therefore
/// must not be used for hashing or testing values on equality.
///
/// @tparam TMappedPlain The mapped type to modify relevant placeholder length for \b FHashcode
///                      and \b FEquals implementation.
//==================================================================================================
template<typename TMappedPlain>
inline constexpr unsigned int SizeTraits= sizeof(TMappedPlain);

/// Specialization of traits-expression for type <c>long double</c>.
/// The implementation of this type is platform-dependent and may be adopted to a certain
/// compiler/processor combination by passing the compiler-symbol
/// \ref ALIB_SIZEOF_LONGDOUBLE_WRITTEN.
template<>
inline constexpr unsigned int SizeTraits<long double> = ALIB_SIZEOF_LONGDOUBLE_WRITTEN;

ALIB_WARNINGS_IGNORE_DOCS
//==================================================================================================
/// A concept that is satisfied for types which boxing is customized for, hence for types for which
/// a specialization of the type trait \alib{boxing;BoxTraits} exists.
///
/// \see
///   Concepts \alib{boxing;IsUnboxable}, \alib{boxing;IsLocked} and
///   \alib{boxing;IsNotBoxable}.
///
/// @tparam T  The type to check.
//==================================================================================================
template<typename T>
concept IsCustomized
=    (  std::is_pointer_v<T> || !std::same_as<DefaultBoxingTag, typename BoxTraits<            T  >::Mapping> )
  && ( !std::is_pointer_v<T> || !std::same_as<DefaultBoxingTag, typename BoxTraits<ALIB_TVALUE(T)*>::Mapping>);

// Note: besides checking BoxTraits<T>, in the case a pointer is given, we also have to check
// BoxTraits for the type that results when
// - the pointer is removed and then
// - a const is removed and finally
// - the pointer is added again. Otherwise "const T*" is not deferred to "T*"

/// This concept is for internal use. It is satisfied if the given type \p{T} fits into the
/// placeholder, is copy-constructible and trivially destructible.
/// @tparam T       The type to test.
/// @tparam TVal    Decayed version of \p{T}. Deduced by the compiler, must not be given.
template<typename T, typename TVal= ALIB_TVALUE(T)>
concept IsStdPH=        sizeof(std::conditional_t<std::same_as<void, TVal>, void*, TVal>)
                     <= sizeof(Placeholder)
                 &&  std::is_copy_constructible    <TVal>::value
                 &&  std::is_trivially_destructible<TVal>::value;

/// This concept is for internal use. It is satisfied if the given type \p{T}
/// has customized boxing with a \c constexpr version of the method <b>BoxTraits<T>::Write</b>
/// which takes only one parameter, namely an instance of \p{T}.
/// @tparam T       The type to test.
template<typename T>
concept IsConstExprWrite=
    requires(T& t) { { BoxTraits<T>::Write(t)  } -> std::same_as<Placeholder>; };


/// This type trait by default inherits \c std::false_type. If specialized for
/// template type \p{TCharArray} to inherit \c std::true_type, then boxing that type will not be
/// customized automatically with a corresponding specialization of \alib{characters;ArrayTraits}.
/// This keeps the customization of boxing open to be performed in a different way.
///
/// \see
///   See manual chapter \ref alib_boxing_strings "10. Boxing Character Strings" of the
///   Programmer's Manual of module \alib_boxing_nl.
///
/// @tparam TCharArray The type that \alib{characters;ArrayTraits} is specialized for but still no
///                    character array boxing should be performed.
template<typename TCharArray>      struct SuppressCharArrayBoxingTraits  : std::false_type       {};


template<typename T>
concept IsStringType =
       !SuppressCharArrayBoxingTraits<std::remove_cv_t<T>>::value
  && (    characters::ArrayTraits  <std::remove_cv_t<T>, nchar>::Access == characters::Policy::Implicit
       || characters::ArrayTraits  <std::remove_cv_t<T>, wchar>::Access == characters::Policy::Implicit
       || characters::ArrayTraits  <std::remove_cv_t<T>, xchar>::Access == characters::Policy::Implicit
       );

template<typename T>
concept IsUnboxableStringType =
          characters::ArrayTraits  <T, nchar>::Construction == characters::Policy::Implicit
       || characters::ArrayTraits  <T, wchar>::Construction == characters::Policy::Implicit
       || characters::ArrayTraits  <T, xchar>::Construction == characters::Policy::Implicit
       ;


//==================================================================================================
/// A concept that is satisfied for types for which boxing is customized to disable unboxing.
/// In other words, types for which a specialization of the type trait \b %BoxTraits
/// exists which declares method \alib{boxing;BoxTraits::Read} to have a different return type
/// than \p{T} or \p{T&}.
///
/// \see
///   \alib{boxing;IsUnboxable} for an alternative concept that satisfied if a type can be
///   unboxed and methods \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox} do not fail to
///   compile with that type.
///
/// @tparam T  The type to check.
//==================================================================================================
template<typename T>
concept IsLocked =
    !std::same_as<T, std::remove_reference_t<decltype(BoxTraits<T>::Read(std::declval<Placeholder>()))> >;

//==================================================================================================
/// A concept that is satisfied if:
///
/// - boxing was customized for the given type and \alib{boxing;NotBoxableTag} was given as
///   mapped type, or
/// - given type is a value type, no customization is given for it, while the corresponding
///   pointer type has customized boxing with mapped type being \alib{boxing;NotBoxableTag}, or
/// - given type is a pointer  type, no customization is given for it, while the corresponding
///   value type has customized boxing with mapped type being \alib{boxing;NotBoxableTag}.
///
/// If a type is not boxable, it can be neither boxed nor unboxed.
///
/// \see
///   Concepts \alib{boxing;IsCustomized}, \alib{boxing;IsLocked} and
///   \alib{boxing;IsUnboxable}.
///
/// @tparam T  The type to check.
//==================================================================================================
template<typename T>
concept IsNotBoxable =
          std::same_as<NotBoxableTag, typename BoxTraits<T>::Mapping>
    || (    !IsCustomized<T>
         && (     (    !std::is_pointer_v<T>
                    && std::same_as<NotBoxableTag,
                                    typename BoxTraits<        T*>::Mapping> )
              ||  (     std::is_pointer_v<T>
                    && std::same_as<NotBoxableTag,
                                    typename BoxTraits<std::remove_pointer_t<T>>::Mapping> ) ) )
       ;




//==================================================================================================
/// This concept is satisfied if a type can be unboxed and methods \alib{boxing;Box::IsType}
/// and \alib{boxing;Box::Unbox} will not fail to compile with that type.
///
/// With default boxing, one of the types \b T and \b T* are unboxable (depending on value type
/// size and whether the type is copy-constructible and trivially destructible).
///
/// If custom boxing for either or both of types \b T and \b T* is in place, then the given type is
/// not unboxable if:
/// - customization is not in place for the version passed (value or pointer).
/// - customization is in place for the given type but concept \alib{boxing;IsLocked} is satisfied.
/// - The type is mapped to \alib{boxing;NotBoxableTag}.
///
/// \see
///   Concepts \alib{boxing;IsCustomized}, \alib{boxing;IsLocked} and
///   \alib{boxing;IsNotBoxable}.
///
/// @tparam T  The type to check.
//==================================================================================================
template<typename T>
concept  IsUnboxable=
        // default boxing
        (    !IsCustomized<std::remove_pointer_t<T> >
          && !IsCustomized<std::remove_pointer_t<T>*>
          &&    bool(  std::is_pointer_v<T> )
             == bool(     ( sizeof(Placeholder) <  sizeof(ALIB_TVALUE(T))  )
                      ||  !std::is_copy_constructible    <ALIB_TVALUE(T)>::value
                      ||  !std::is_trivially_destructible<ALIB_TVALUE(T)>::value )  )
   ||   // custom boxing
        (     IsCustomized<T>
          && !IsLocked    <T>
          && !IsNotBoxable<T> )

   ||   // string type
        IsUnboxableStringType<T>
;

ALIB_WARNINGS_RESTORE


}} // namespace [alib::boxing]





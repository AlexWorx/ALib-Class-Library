//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_TYPETRAITS
#define HPP_ALIB_BOXING_TYPETRAITS 1
#pragma once
#if !defined(HPP_ALIB_BOXING_BOXING)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace alib {  namespace boxing  {
namespace detail  {
/// This type is used internally with struct \alib{boxing;TMappedTo}.
///
/// \note
///   The type is not used by user code. However, working with a debugger, when investigating
///   to the \alib{boxing::detail;VTable} of a box value, this type will nicely denote that
///   the mapped type is not an array.
struct TPlain  {};

/// This type is used internally with struct \alib{boxing;TMappedToArrayOf}.
///
/// \note
///   The type is not used by user code. However, working with a debugger, when investigating
///   to the \alib{boxing::detail;VTable} of a box value, this type will nicely denote that
///   the mapped type is an array.
struct TArrayOf {};

/// This is an empty type used to denote that default boxing is active. The type is
/// used with the default implementation of TMP struct \alib{boxing;T_Boxer}.
/// Providing this type in custom specializations (wrapped in \alib{boxing;TMappedTo}),
/// make such specialization in-effective.
///
/// \see
///   Templated struct \alib{boxing;T_Boxer} for more information.
struct TDefaultBoxing  {};


/// This is an empty type used to denote that a type must not be boxed.
/// To disable \alib_boxing_nl for a custom type, a specialization of type traits
/// struct \alib{boxing;T_Boxer} must use this type (wrapped in \alib{boxing;TMappedTo}), to
/// define type member \alib{boxing;T_Boxer::Mapping}.
///
/// \see
///   Templated struct \alib{boxing;T_Boxer} for more information.
struct TNotBoxable     {};

}

/// This type is used with to denote type mapping with standard, non-array types.
///
/// Type mappings using either this or sibling struct \alib{boxing;TMappedToArrayOf}, have
/// to be provided with
/// - Specializations of \alib{boxing;T_Boxer}, and
/// - Namespace function \alib{boxing;BootstrapRegister} used to associated box-function implementations
///   with mapped types.
///
/// \see
///   Templated struct \alib{boxing;T_Boxer} for more information.
///
/// @tparam TMapped   The boxed type.
template<typename TMapped>
struct TMappedTo
{
    /// The given type \p{TMapped}.<br>
    using Type         = TMapped;

    /// Denotes that the type is not boxed to an array type.
    using PlainOrArray = detail::TPlain;

};


/// This type is used to define type mappings targeting array-types.
/// Array types consist of an element type as well as a length.
/// Corresponding specializations of \alib{boxing;T_Boxer::Write} and \alib{boxing;T_Boxer::Read}
/// have to use write the pointer to the start of the array and the array's length into
/// the box's \alib{boxing;Placeholder}.
///
/// \see
///   Sibling struct \alib{boxing;TMappedTo} used to denote mapping to standard non-array types.
///
/// @tparam TElement  The boxed array's element type.
template<typename TElement>
struct TMappedToArrayOf
{
    /// The element type given as \p{TElement}.<br>
    using Type         = TElement;

    /// Denotes that the type is boxed to an array type.
    using PlainOrArray = detail::TArrayOf;
};

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
/// 1. <b>Type definition #Mapping:</b><br>
///    The mapped type must be given using helper-struct \alib{boxing;TMappedTo} or
///    \alib{boxing;TMappedToArrayOf}. The mapped type needs to be given as an explicit template
///    parameter of those.
///    In the case of TMappedTo, the template parameter denotes the mapped type, while
///    with TMappedToArrayOf, the template parameter denotes the type of the elements of the boxed
///    C++ array.
///
///    If special type \alib{boxing;detail::TNotBoxable} is given (still wrapped in the helper, hence given
///    as <c>TMappedTo<TNotBoxable></c>), then boxing is disallowed for type \p{TBoxed}.
///    In this case, only declarations of functions #Write and #Read need to be provided, as they
///    are never invoked.
///
///    The default implementation of this struct uses a second special type available,
///    \alib{boxing;detail::TDefaultBoxing} which denotes default boxing.
///
/// 2. <b>Static Method #Write:</b><br>
///    This method is invoked for converting source values to mapped types.
///    Often the default implementation given in the un-specialized version this struct is
///    applicable. In this case it can simply be copied to the specialization.
///
///    Custom implementations may use TMP-enabled overloads of method \alib{boxing;Placeholder::Write}
///    or alternatively write to the \c union members of the placeholder directly.
///
/// 3. <b>Static Method #Read:</b><br>
///    This method is invoked for converting the placeholder data back to the boxed source type.
///    Often the default implementation given in the un-specialized version of this struct is
///    well suited. In this case it can simply be copied to the specialization.<br>
///    Custom implementations may use TMP-enabled overloads of method \alib{boxing;Placeholder::Read}
///    or alternatively read the \c union members of the placeholder directly.
///
///    A type becomes \alib{boxing;TT_IsUnboxable;not unboxable} at the moment this function
///    returns a different type than \p{TBoxable}.<br>
///    This may well be intended and thus the specialized version of this struct
///    may declare this method to return \c void.
///    In this case a declaration of the method is sufficient, as it will never be invoked.<br>
///    With TMP-enabled specializations of this struct (see  Programmer's Manual
///    chapter \ref alib_boxing_customizing_conditional), the return type might be conditionally set.
///    Such TMP-defined return type then decides about which of the TMP-enabled types are unboxable
///    and which are not.
///
/// \note
///   If a specialization uses default type mapping by providing<br>
///
///       using   Mapping=      TMappedTo<TDefaultBoxing>;
/// \note
///   in theory, a mixture of default and custom boxing is in place!
///   Note, that the authors of this library and documentation have not seen a use case for this, yet.
///
/// <br><p>
/// ### Helper Macros: ###
///
/// A set of macros for defining specializations exist.
/// The use of the macro is recommended, as besides being less error prone, their use make the code
/// more readable. Finally, chances are good that code that uses the macros remains compatible
/// with future versions of module \alib_boxing_nl.
///
/// All macros expect this struct's template type \p{TBoxable} as the first parameter, and most
/// expect the mapped type as the second parameter. The latter must not be wrapped in
/// templated helpers \b %TMappedTo respectively \b %TMappedToArrayOf, as the macros internally do that.<br>
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
/// For technical reasons, namely to avoid TMP compilation problems, some conditional specialization
/// is internally made, that declare method #Read \c void. This is sometimes needed, if a type is
/// not returnable, even if that becomes never be boxed or unboxed.
/// Such situation might happen with TMP, for example if class \b Box is part of a \c union.<br>
/// The specialization is made for the following types of \p{TBoxable}:
/// - C++ array types<br>
///   Those types are fetched by an overloaded constructor that does not (because it cannot)
///   leverage this struct. Likewise, they cannot be unboxed in the usual fashion.
/// - Function types (selected by <c>std::is_function<TBoxable>::value</c>).
///
/// If a dubious error message about method #Read not being able to return a certain type occurs, even
/// one that a user's code maybe even does "actively" try to box or unbox, then it might be helpful
/// to specialize this struct for such type, without providing implementations of \b Write and
/// \b Read and with the latter to return \c void.
///
/// <br><p>
/// \see More explanation and sample code is given in chapter
///      \ref alib_boxing_customizing "7. Customizing Boxing" of the
///      \ref alib_mod_boxing "Programmer's Manual" of module \alib_boxing_nl.
///
/// @tparam TBoxable    The source type to customize boxing for.
/// @tparam TEnableIf   Optional TMP parameter to allow conditional specializations.
//==================================================================================================
template<typename TBoxable, typename TEnableIf= void>
struct T_Boxer
{
    /// Defines the mapped type. With specializations, that type has to be wrapped in either
    /// \alib{boxing;TMappedTo;TMappedTo<T>} or \alib{boxing;TMappedTo;TMappedToArrayOf<T>}.
    /// Special designator types \alib{boxing;detail::TDefaultBoxing} and \alib{boxing;detail::TNotBoxable}
    /// may be wrapped, to denote corresponding behavior.
    ///
    /// The default implementation specifies designator type \b TDefaultBoxing, which
    /// disables custom boxing.
    using   Mapping=      TMappedTo<detail::TDefaultBoxing>;


    /// Used for boxing a value, precisely writing the boxable portion of a type into
    /// field \alib{boxing;Box::data}, which is given with parameter \p{target}.<br>
    /// The default implementation of this struct implements this method as follows:
    /// \snippet "alib/boxing/typetraits.inl"  DOX_BOXING_T_BOXER_WRITE
    ///
    /// This implementation leverages the TMP-enabled overloads \alib{boxing;Placeholder::Write}
    /// and is often all that is needed with custom specializations.
    ///
    /// \see
    ///   A possible alternative declaration (signature) of this method is explained with manual
    ///   chapter \ref alib_boxing_more_opt_constexpr.
    ///
    /// @param target The placeholder of the destination box.
    /// @param value  The value to that is to be boxed.
    DOX_MARKER([DOX_BOXING_T_BOXER_WRITE])
    static void  Write( Placeholder& target,  const TBoxable& value )
    {
        target.Write( value );
    }
    DOX_MARKER([DOX_BOXING_T_BOXER_WRITE])

    /// Used for unboxing a value, precisely reading the contents of field \alib{boxing;Box::data},
    /// which is given with parameter \p{src} and creating a value of type \p{TBoxable} from
    /// that.<br>
    /// The default implementation of this struct implements this method as follows:
    /// \snippet "alib/boxing/typetraits.inl"  DOX_BOXING_T_BOXER_READ
    ///
    /// This implementation leverages the TMP-enabled overloads \alib{boxing;Placeholder::Read}
    /// and is often all that is needed with custom specializations.
    ///
    /// If a different type than \p{TBoxable} is returned, then that source type is not unboxable.
    /// To intend such behavior, for example because \p{TBoxable} is mapped to a reduced type
    /// and therefore unboxing is not possible, specializations may declare return type
    /// \c void and omit a definition of this method.<br>
    /// With TMP enabled customizations, also other return types may given, which likewise denote
    /// an unboxable source type. A sample of that is given with Programmer's Manual chapter
    /// \ref alib_boxing_customizing_conditional.
    ///
    ///
    /// @param src   The Placeholder to unbox the data from
    /// @return \c The unboxed object.
    static
    ATMP_IF_T_F( !std::is_abstract<TBoxable>::value, TBoxable, TBoxable& )
    Read( const Placeholder& src)
    {
DOX_MARKER([DOX_BOXING_T_BOXER_READ])
return src.Read<TBoxable>();
DOX_MARKER([DOX_BOXING_T_BOXER_READ])
    }

}; // T_Boxer

// ########################      critical  T_Boxer  specializations     ###########################
#if !DOXYGEN

// This is necessary for types that can't be used as return type in any way, for example
// types with extents, functions, etc.
template<typename TBoxable>  struct T_Boxer<TBoxable,
ATMP_VOID_IF( ATMP_IS_ARR( TBoxable ) || std::is_function<TBoxable>::value )    >
{
    using          Mapping=  TMappedTo<detail::TDefaultBoxing>;
    static void    Write( Placeholder& target,  TBoxable& value );
    static void    Read ( const Placeholder& src);
};

// void (needed to pass TMP stuff)
template<> struct T_Boxer<void>
{
    using          Mapping=  TMappedTo<detail::TDefaultBoxing>;
    static void    Write( Placeholder& target,  const void* value );
    static void*   Read ( const Placeholder& src);
};

#endif

//==================================================================================================
/// This specializable TMP struct must be set for custom types, if the following applies:
/// - A custom boxing implements \alib{boxing;T_Boxer::Write} in a way that a different number
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
/// To receive the number of relevant bytes, they invoke \alib{boxing;Box::GetPlaceholderUsageLength}
/// (at run-time). This value is set at compile-time with the creation of a mapped type's \e vtable.
/// While for array types, the value is set to the overall size of union \alib{boxing;Placeholder},
/// for non-array types, the value of this TMP struct is used.
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
template<typename TMappedPlain> struct T_SizeInPlaceholder
{
    /// The size used with the placeholder
    static constexpr unsigned int value= sizeof(TMappedPlain);
};

#if !DOXYGEN
    template<> struct T_SizeInPlaceholder<long double> { static constexpr unsigned int value= ALIB_SIZEOF_LONGDOUBLE_WRITTEN;  };
#endif


//==================================================================================================
/// Helper-struct that inherits \c std::true_type if boxing is customized for
/// type \p{T}, otherwise \c std::false_type.
///
/// \see
///   Helpers \alib{boxing;TT_IsUnboxable}, \alib{boxing;TT_IsLocked} and
///   \alib{boxing;TT_IsNotBoxable}.
///
/// @tparam T         The type to check.
/// @tparam TEnableIf Used for conditional specializations of this struct.
//==================================================================================================
template<typename T, typename TEnableIf= void> struct TT_IsCustomized          : std::true_type  {};

#if !DOXYGEN

// besides checking T_Boxer<T>, in the case a pointer is given, we also have to check
// T_Boxer for the type that results when
// - the pointer is removed and then
// - a const is removed and finally
// - the pointer is added again. Otherwise "const T*" is not deferred to "T*"

template<typename T> struct TT_IsCustomized  <T, ATMP_VOID_IF(
  !ATMP_IS_PTR(T) && ATMP_EQ(detail::TDefaultBoxing, typename T_Boxer<          T  >::Mapping::Type)
                                                                   )>          : std::false_type {};

template<typename T> struct TT_IsCustomized  <T, ATMP_VOID_IF(
   ATMP_IS_PTR(T) && ATMP_EQ(detail::TDefaultBoxing, typename T_Boxer<ATMP_RCVP(T)*>::Mapping::Type)
                                                                   )>          : std::false_type {};

#endif

//==================================================================================================
/// Helper-struct that inherits \c std::true_type if customized boxing is in place
/// that disallows unboxing, otherwise \c std::false_type.
///
/// This struct is strict in the following sense: It is of \c std::false type, if given \p{T} is a
/// non-customized value type \b TV, while the corresponding pointer type \b TV* type is locked - and
/// vice versa.
/// Note that in these two cases, the type is still not unboxable because it is locked through the
/// corresponding other customization.
///
/// This behavior is intended and needed internally.
///
/// \note
///   Unboxing of a type is disabled, if method \alib{boxing;T_Boxer::Read} of a specialized
///   boxer is declared to return \c void.
///
/// \see
///   \alib{boxing;TT_IsUnboxable} for an alternative that is of \c std::true_type exactly
///   if a type cannot be unboxed and methods \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox}
///   fail to compile with that type.
///
/// @tparam T         The type to check.
/// @tparam TEnableIf Used for conditional specializations of this struct.
//==================================================================================================
template<typename T, typename TEnableIf= void> struct TT_IsLocked              : std::false_type {};

#if !DOXYGEN

template<typename T                          > struct TT_IsLocked<T,
ATMP_VOID_IF( !ATMP_EQ(T, ATMP_RR(decltype(T_Boxer<T>::Read(std::declval<Placeholder>())) ) ) )
                                                                    >          : std::true_type  {};
#endif

//==================================================================================================
/// Helper-struct that inherits \c std::true_type, if
///
/// - boxing was customized for the given type and \alib{boxing;detail::TNotBoxable} was given as
///   mapped type, or
/// - given type is a value type, no customization is given for it, while the corresponding
///   pointer type has customized boxing with mapped type being \alib{boxing;detail::TNotBoxable}, or
/// - given type is a pointer  type, no customization is given for it, while the corresponding
///   value type has customized boxing with mapped type being \alib{boxing;detail::TNotBoxable}.
///
/// If a type is not boxable, it can be neither boxed nor unboxed.
///
/// \see
///   Helpers \alib{boxing;TT_IsCustomized}, \alib{boxing;TT_IsLocked} and
///   \alib{boxing;TT_IsUnboxable}.
///
/// @tparam T         The type to check.
/// @tparam TEnableIf Used for conditional specializations of this struct.
//==================================================================================================
template<typename T, typename TEnableIf= void> struct TT_IsNotBoxable          : std::false_type {};

#if !DOXYGEN

template<typename T>
struct TT_IsNotBoxable<T, typename std::enable_if<
      ATMP_EQ(detail::TNotBoxable, typename T_Boxer<T>::Mapping::Type)
|| (    !TT_IsCustomized<T>::value
     && (     (     !ATMP_IS_PTR(T)
                &&   ATMP_EQ(detail::TNotBoxable   , typename T_Boxer<        T*>::Mapping::Type) )
          ||  (      ATMP_IS_PTR(T)
                &&   ATMP_EQ(detail::TNotBoxable   , typename T_Boxer<ATMP_RP(T)>::Mapping::Type) )
        )
   )
                                                   >::type>                    : std::true_type  {};

#endif


//==================================================================================================
/// This type is of \c std::true_type exactly if a type cannot be unboxed and methods
/// \alib{boxing;Box::IsType} and \alib{boxing;Box::Unbox} will fail to compile with that type.
///
/// With default boxing, one of the types \b T and \b T* are unboxable (depending on value type
/// size and whether the type is copy constructible and trivially destructible).
///
/// If custom boxing for either or both of types \b T and \b T* is in place, then the given type is
/// not unboxable if:
/// - customization is not in place for the version passed (value or pointer).
/// - customization is in place for the given type, but \alib{boxing;TT_IsLocked} is of
///   \c std::true_type.
/// - The type is mapped to \alib{boxing;detail::TNotBoxable}.
///
/// \see
///   Helpers \alib{boxing;TT_IsCustomized}, \alib{boxing;TT_IsLocked} and
///   \alib{boxing;TT_IsNotBoxable}.
///
/// @tparam T         The type to check.
/// @tparam TEnableIf Helper-type for implementing specializations of this struct.
//==================================================================================================
template<typename T, typename TEnableIf=void>
struct   TT_IsUnboxable                                                        : std::false_type {};

#if !DOXYGEN

// default boxing
template<typename T>
struct   TT_IsUnboxable<T, typename std::enable_if<

   !TT_IsCustomized<ATMP_RP(T) >::value
&& !TT_IsCustomized<ATMP_RP(T)*>::value
&&    static_cast<bool>(  ATMP_IS_PTR(T) )
   == static_cast<bool>(     ( sizeof(Placeholder) <  sizeof(ATMP_RCVP(T))  )
                         ||  !std::is_copy_constructible    <ATMP_RCVP(T)>::value
                         ||  !std::is_trivially_destructible<ATMP_RCVP(T)>::value )

                                                  >::type>                     : std::true_type  {};


// custom boxing
template<typename T>
struct   TT_IsUnboxable<T, typename std::enable_if<
    TT_IsCustomized<T>::value
&& !TT_IsLocked    <T>::value
&& !TT_IsNotBoxable<T>::value
                                                  >::type>                     : std::true_type  {};


#endif

}} // namespace [alib::boxing]

//##################################################################################################
//#######################################     Macros     ###########################################
//##################################################################################################

#define ALIB_BOXING_CUSTOMIZE(TSource, TTarget, ...)                                               \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{                                                                                                  \
    using                       Mapping=    TMappedTo<TTarget>;                                    \
                                __VA_ARGS__                                                        \
}; }                                                                                             \

#define ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE(TSource, TElement, ...)                                   \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{                                                                                                  \
    using                       Mapping=    TMappedToArrayOf<TElement>;                            \
                                __VA_ARGS__                                                        \
}; }

#define ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING(TSource, TTarget)                                       \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{ using                         Mapping=    TMappedTo<TTarget>;                                    \
  static void                   Write( Placeholder& target,  TSource const & value )  { target.Write( static_cast<TTarget>( value ) );      } \
  static TSource                Read (const Placeholder& src)                         { return static_cast<TSource>(src.Read<TTarget>());   } \
}; }

#define ALIB_BOXING_CUSTOMIZE_TYPE_MAPPING_CONSTEXPR(TSource, TTarget)                             \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{ using                         Mapping=    TMappedTo<TTarget>;                                    \
  static constexpr Placeholder  Write(  TSource const & value )   { return Placeholder( static_cast<TTarget>( value ) );} \
  static TSource                Read (const Placeholder& src)     { return static_cast<TSource>(src.Read<TTarget>()   );} \
}; }


#define ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE(TSource, TTarget)                                      \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{ using                         Mapping=    TMappedTo<TTarget>;                                    \
  static void                   Write( Placeholder& target,  TSource const & value )  { target.Write( static_cast<typename Mapping::Type>( value ) ); } \
  static void                   Read( const Placeholder& src);                                     \
}; }

#define ALIB_BOXING_CUSTOMIZE_NOT_UNBOXABLE_CONSTEXPR(TSource, TTarget)                            \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{ using                         Mapping=    TMappedTo<TTarget>;                                    \
  static constexpr Placeholder  Write(  TSource const & value )   { return Placeholder( static_cast<typename Mapping::Type>( value ) ); } \
  static void                   Read( const Placeholder& src);                                     \
}; }


#define ALIB_BOXING_CUSTOMIZE_ARRAY_TYPE_NON_UNBOXABLE(TSource, TElement, ...)                     \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{   using                       Mapping=    TMappedToArrayOf<TElement>;                            \
    __VA_ARGS__                                                                                    \
    static void                 Read( const Placeholder& src);                                     \
}; }


#define ALIB_BOXING_CUSTOMIZE_DENY_BOXING(TSource)                                                 \
namespace alib::boxing {                                                                           \
template<>  struct T_Boxer<TSource>                                                                \
{ using                         Mapping=    TMappedTo<detail::TNotBoxable>;                        \
  static void                   Write( Placeholder& target,  TSource const & value );              \
  static void                   Read ( const Placeholder& src);                                    \
}; }

#endif // HPP_ALIB_BOXING_TYPETRAITS


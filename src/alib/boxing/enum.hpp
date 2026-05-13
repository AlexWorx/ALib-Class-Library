//==================================================================================================
/// \file
/// This header-file is part of module \alib_boxing of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace boxing {

//==================================================================================================
/// This class is useful to pass and accept enum values of arbitrary C++ scoped enum types.
/// Based on class #"Box", all interfaces are inherited, including type guessing and
/// unboxing.
///
/// In the constructor, enum elements of an arbitrary type are accepted.
/// The element's underlying integral value is boxed, and thus runtime type-information is added.
/// Having the "original" element stored in protected base class #"%Box", has the following
/// advantages:
///
/// \note
///   The implementation of this class, by deriving from class #"%Box", introduces a
///   small memory overhead (usually 8 bytes per instance on 64-bit system), in respect to a
///   possible alternative "direct" implementation. This is due to the fact that boxing allows
///   one-dimensional array types to be boxed as well as scalar types - which is never the case
///   with this class.<br>
///   But the advantages of doing so certainly surpasses this small drawback.
///
/// \note
///   Class #"%Box" is inherited \c protected instead of public, to hide bigger portions of
///   the base class's interface. While some functions are explicitly made visible with
///   keyword \c using, for others, instances of this class have to be cast first.
///   Casting is done with overloaded methods #"CastToBox".
///
/// ## Functors In Namespace std ##
/// Functors <c>std::hash</c>, <c>std::equal_to</c> and <c>std::less</c> are specialized for this
/// type with the inclusion of the header-file #"F;ALib.Boxing.StdFunctors.H"
/// as documented with namespace #"alib::boxing;2"::compatibility::std.
///
/// ## Friends ##
/// class #"Box"
///
/// @see With the inclusion of the module \alib_enumrecords in the \alibbuild, the namespace
///      functions #"enumrecords::GetRecord" and #"enumrecords::TryRecord" become available, which
///      receive an enum record for a value of this class.
//==================================================================================================
struct Enum : protected Box {
    #if !DOXYGEN
        friend class Box;
    #endif

    /// Default constructor.
    Enum()
    : Box(nullptr)                                                                                {}

    /// Implicit constructor, which accepts arbitrary elements of scoped or non-scoped enum types.
    ///
    /// @tparam TEnum     The external (user specific) enumeration type.
    /// @param  element   The external (user specific) enumeration element.
    template<typename  TEnum>
    requires std::is_enum<TEnum>::value
    constexpr Enum( TEnum element )
    : Box( element )                                                                              {}

    /// This is a shortcut to #"Box::Unbox;Box::Unbox<TEnum>()" to retrieve the
    /// original enum element in a type-safe way.
    ///
    /// Before invoking this, the boxed type can be checked with #"IsType". If the wrong type
    /// is tried to be received, an \alib_assertion is raised.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @return The underlying integral value of the encapsulated enum element.
    template<typename  TEnum>
    requires std::is_enum<TEnum>::value
    TEnum Get()                                                     const { return Unbox<TEnum>(); }

    /// Returns the underlying integral value of the original enum element cast to type
    /// \p{TIntegral}.
    ///
    /// \note
    ///   Boxed enum element values are always
    ///   #"alib_boxing_enums_integer;stored as type integer", regardless of the
    ///   underlying type of the enumeration.
    ///
    /// @tparam TIntegral The requested width of the return type. Defaults to #"lang::integer".
    /// @return The underlying integral value.
    template<typename TIntegral= integer>
    TIntegral  Integral()          const { return static_cast<TIntegral>(data.Integrals.Array[0]); }

    /// Comparison operator.
    ///
    /// @param rhs The right-hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    bool operator==(const Enum& rhs)                                                         const {
        return     Data().VoidP == rhs.Data().VoidP
                &&     TypeID() == rhs.TypeID();
    }

    /// Comparison operator.
    ///
    /// @param rhs The right-hand side argument of the comparison.
    /// @return \c true if this object does not equal \p{rhs}, \c false otherwise.
    bool operator!=(const Enum& rhs)                             const { return !((*this) == rhs); }

    #if DOXYGEN
    /// Imports \c protected base class's method #"Box::TypeID".
    /// @return The \c std::type_info of the mapped \c enum type.
    using Box::TypeID;

    /// Imports \c protected base class's method #"Box::Hashcode".
    /// @return A hashcode for the boxed enum type and value.
    using Box::Hashcode;
    #else
    using Box::TypeID;
    using Box::Hashcode;
    #endif

    /// This method casts an instance of this class to a reference of base class #"%Box".
    /// To hide the bases class's interface, this class inherits class #"%Box" only as
    /// a \c protected base. With this method, this "artificial limitation " (its a design decision)
    /// is lifted.
    ///
    /// @return A mutable reference to this object.
    Box& CastToBox()                                            { return static_cast<Box&>(*this); }

    /// Same as overloaded version, but returns a \c const reference and consequently this method
    /// is declared\c const itself.
    ///
    /// @return A constant reference to this object.
    const Box& CastToBox()                          const { return static_cast<const Box&>(*this); }


    /// Checks if this instance has an enum element of type \p{TEnum} stored.<br>
    /// This method is an inlined, simple alias for \c protected base class's method
    /// #"Box::IsType".
    ///
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @return \c true if the encapsulated enum type of type \p{TEnum}, otherwise \c false.
    template<typename  TEnum>
    requires std::is_enum<TEnum>::value
    bool    IsEnumType()                                      const { return Box::IsType<TEnum>(); }

    /// \note This method overrides the otherwise protected inherited method of class #"%Box"
    ///       but also changes its meaning slightly. Here, no boxing function is called, instead
    ///       it is checked whether the box is set at all. Specifically, this method returns
    ///       <c>Box::IsType<void>()</c>.
    ///
    /// @return \c false if this object contains an enumeration element, \c true otherwise.
    bool    IsNull()                                                const { return IsType<void>(); }

    /// Returns the negated result #"IsNull".
    /// @return \c true if this object contains with an enumeration element, \c false otherwise.
    ALIB_DLL
    bool    IsNotNull()                                                  const { return !IsNull(); }


    /// Comparison operator with enum elements.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @param rhs The right-hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    template<typename  TEnum>
    requires std::is_enum<TEnum>::value
    bool operator==(TEnum rhs)                                                               const {
        return Integral() == static_cast<typename std::underlying_type<TEnum>::type>( rhs )
               && TypeID() == typeid( TEnum );
    }

    /// Comparison operator with enum elements.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @param rhs The right-hand side argument of the comparison.
    /// @return \c true if this object does not equal \p{rhs}, \c false otherwise.
    template<typename  TEnum>
    requires std::is_enum<TEnum>::value
    bool operator!=(TEnum rhs)                                                               const {
        return Integral() != static_cast<typename std::underlying_type<TEnum>::type>( rhs )
               || TypeID() != typeid( TEnum );
    }

    /// Comparison operator with another #"%Enum" object.
    /// The sort order is primarily determined by the enum types that were boxed.
    /// If those are the same, then the underlying integral value of the enum elements is compared.
    ///
    /// This leads to a nested sort order, with the type information being the outer order and
    /// the integral value of the enum being the inner one.
    ///
    /// \note
    ///   It is a matter of the compiler how the outer sort of types is performed and thus this
    ///   cannot be determined by the user code.
    ///
    ///
    /// @param rhs The right-hand side argument of the comparison.
    /// @return If the encapsulated type of this instance is the same as that of \p{rhs}, this
    ///         methods returns \c true if #".Integral()" of this object is smaller than the one of
    ///         \p{rhs} and otherwise \c false. If the types are not the same, than the result is
    ///         dependent on the tool chain (compiler) used for compiling \alib.
    bool operator< (Enum const& rhs)                                                         const {
        return      (    std::type_index(    TypeID() )
                       < std::type_index(rhs.TypeID() )  )
                ||  (    TypeID() == rhs.TypeID()
                         && Integral() < rhs.Integral()         );
    }

}; // class Enum

} // namespace alib[::boxing]

/// Type alias in namespace #"%alib".
using     Enum=              boxing::Enum;

} // namespace [alib]

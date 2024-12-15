//==================================================================================================
/// \file
/// This header file is part of module \alib_boxing of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_BOXING_ENUM
#define HPP_ALIB_BOXING_ENUM 1
#pragma once
#include "alib/boxing/boxing.hpp"

#if ALIB_ENUMS
#   include "alib/enums/records.hpp"
#endif

#include <typeindex>

namespace alib {  namespace boxing {

//==================================================================================================
/// This class is useful to pass and accept enum values of arbitrary C++ scoped enum types.
/// Based on class \alib{boxing;Box}, all interfaces are inherited, including type guessing and
/// unboxing.
///
/// In the constructor, enum elements of an arbitrary type are accepted.
/// The element's underlying integral value is boxed, and thus run-time type-information is added.
/// Having the "original" element stored in protected base class \b Box, has the following
/// advantages:
///
/// \note
///   The implementation of this class, by deriving from class \b Box, introduces a
///   small memory overhead (usually 8 bytes per instance on 64-bit system), in respect to a
///   possible alternative "direct" implementation. This is due to the fact that boxing allows
///   one dimensional array types to be boxed as well as scalar types - which is never the case
///   with this class.<br>
///   But the advantages of doing so certainly surpass this small drawback.
///
/// \note
///   Class \b Box is inherited \c protected instead of public, to hide bigger portions of
///   the base class's interface. While some functions are explicitly made visible with
///   keyword \c using, for others, instances of this class have to be cased using
///   overloaded methods #CastToBox.
///
/// ## Functors In Namespace std ##
/// Functors <c>std::hash</c>, <c>std::equal_to</c> and <c>std::less</c> are specialized for
/// this type with the inclusion of header file \alibheader{compatibility/std_boxing_functional.hpp}
/// as documented with namespace #alib::compatibility::std.
///
/// ## Friends ##
/// class \alib{boxing;Box}
//==================================================================================================
struct Enum : protected Box
{
    #if !DOXYGEN
    friend class Box;
    #endif

    //==============================================================================================
    /// Default constructor.
    //==============================================================================================
    Enum()
    : Box(nullptr)
    {}

    #if DOXYGEN
    //==============================================================================================
    /// Implicit constructor, which accepts arbitrary elements of scoped or non-scoped enum types.
    ///
    /// @tparam TEnum     The external (user specific) enumeration type.
    /// @tparam TEnableIf Selects this constructor only for enum types. Must not be specified
    ///                   (deduced by the compiler).
    /// @param  element   The external (user specific) enumeration element.
    //==============================================================================================
    template<typename  TEnum, typename TEnableIf>
    inline
    constexpr Enum( TEnum element );
    #else
    template<typename  TEnum,
             typename  TEnableIf  = typename std::enable_if<std::is_enum<TEnum>::value>::type >
    constexpr Enum( TEnum element )
    : Box( element )                        {}
    #endif

    #if DOXYGEN
    //==============================================================================================
    /// This is a shortcut to \alib{boxing;Box::Unbox;Box::Unbox<TEnum>()} to retrieve the
    /// original enum element in a type-safe way.
    ///
    /// Before invoking this, the boxed type can be checked with #IsType. If the wrong type
    /// is tried to be received, an \alib assertion is raised.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @return The underlying integral value of the encapsulated enum element.
    //==============================================================================================
    template<typename  TEnum>
    inline
    TEnum   Get()                    const;
    #else
    template<typename  TEnum>
    ATMP_T_IF(TEnum, std::is_enum<TEnum>::value )
    Get()                                                                                      const
    {
        return Unbox<TEnum>();
    }
    #endif

    //==============================================================================================
    /// Returns the underlying integral value of the original enum element cast to type
    /// \alib{integer}.
    ///
    /// \note
    ///   Boxed enum element values are always
    ///   \ref alib_boxing_enums_integer "stored as type integer", regardless of the
    ///   underlying type of the enumeration.
    ///
    /// @return The underlying integral value.
    //==============================================================================================
    integer  Integral()                                                                        const
    {
        return data.Integrals.Int;
    }

    //==============================================================================================
    /// Comparison operator.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    //==============================================================================================
    bool operator==(const Enum& rhs)                                                           const
    {
        return this->CastToBox() == rhs.CastToBox();
    }

    //==============================================================================================
    /// Comparison operator.
    ///
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object does not equal \p{rhs}, \c false otherwise.
    //==============================================================================================
    bool operator!=(const Enum& rhs)                                                           const
    {
        return this->CastToBox() != rhs.CastToBox();
    }

    #if DOXYGEN
    //==============================================================================================
    /// Imports \c protected base class's method \alib{boxing;Box::TypeID}.
    /// @return The \c std::type_info of the mapped \c enum type.
    //==============================================================================================
    using Box::TypeID;

    //==============================================================================================
    /// Imports \c protected base class's method \alib{boxing;Box::Hashcode}.
    /// @return A hashcode for the boxed enum type and value.
    //==============================================================================================
    using Box::Hashcode;
    #else
    using Box::TypeID;
    using Box::Hashcode;
    #endif

    //==============================================================================================
    /// This method casts an instance of this class to a reference of base class \b Box.
    /// To hide the bases class's interface, this class inherits class \b Box only as
    /// a \c protected base. With this method, this "artificial limitation " (its a design decision)
    /// is lifted.
    ///
    /// @return A mutable reference to this object.
    //==============================================================================================
    Box& CastToBox()
    {
        return static_cast<Box&>(*this);
    }

    //==============================================================================================
    /// Same as overloaded version, but returns a \c const reference and consequently this method
    /// is declared\c const itself.
    ///
    /// @return A constant reference to this object.
    //==============================================================================================
    const Box& CastToBox()                                                                     const
    {
        return static_cast<const Box&>(*this);
    }


    #if DOXYGEN
    //==============================================================================================
    /// Checks if this instance has an enum element of type \p{TEnum} stored.<br>
    /// This method is an inlined, simple alias for \c protected base class's method
    /// \alib{boxing;Box::IsType}.
    ///
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @return \c true if the encapsulated enum type of type \p{TEnum}, otherwise \c false.
    //==============================================================================================
    template<typename  TEnum>
    inline
    bool IsEnumType()      const;
    #else
    template<typename  TEnum>
    ATMP_T_IF(bool, std::is_enum<TEnum>::value)
    IsEnumType()                                                                               const
    {
        return Box::IsType<TEnum>();
    }
    #endif

    #if DOXYGEN
    //==============================================================================================
    /// Comparison operator with enum elements.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object equals \p{rhs}, \c false otherwise.
    //==============================================================================================
    template<typename TEnum>
    inline
    bool operator==(TEnum rhs)       const;
    #else
    template<typename  TEnum>
    ATMP_T_IF(bool, std::is_enum<TEnum>::value)
    operator==(TEnum rhs)       const
    {
        return Integral() == static_cast<typename std::underlying_type<TEnum>::type>( rhs )
               && TypeID() == typeid( TEnum );
    }
    #endif

    #if DOXYGEN
    //==============================================================================================
    /// Comparison operator with enum elements.
    ///
    /// @tparam TEnum       The external (user specific) enumeration type.
    /// @param rhs The right hand side argument of the comparison.
    /// @return \c true if this object does not equal \p{rhs}, \c false otherwise.
    //==============================================================================================
    template<typename TEnum>
    inline
    bool operator!=(TEnum rhs)       const;
    #else
    template<typename  TEnum>
    ATMP_T_IF(bool, std::is_enum<TEnum>::value)
    operator!=(TEnum rhs)       const
    {
        return Integral() != static_cast<typename std::underlying_type<TEnum>::type>( rhs )
               || TypeID() != typeid( TEnum );
    }
    #endif


    //==============================================================================================
    /// Comparison operator with another \b Enum object.
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
    /// @param rhs The right hand side argument of the comparison.
    /// @return If the encapsulated type of this instance is the same as that of \p{rhs}, this
    ///         methods returns \c true if #Integral() of this object is smaller than the one of
    ///         \p{rhs} and otherwise \c false. If the types are not the same, than the result is
    ///         dependent on the tool chain (compiler) used for compiling \alib.
    //==============================================================================================
    bool operator< (Enum const& rhs)  const
    {
        return      (    std::type_index(    TypeID() )
                       < std::type_index(rhs.TypeID() )  )
                ||  (    TypeID() == rhs.TypeID()
                         && Integral() < rhs.Integral()         );
    }

    #if ALIB_ENUMS

        //==========================================================================================
        /// Returns the \ref alib_enums_records "ALib Enum Record" associated with this enumeration
        /// element.<br>
        /// In debug-compilations an \alib assertion is raised, if no enum record was defined for
        /// the enumeration element represented by this instance.
        ///
        /// \see
        /// - Namespace function \alib{enums::GetRecord} of module \alib_enums, which provides the
        ///   standard way of accessing enum records for enum elements known at compile-time.
        /// - Sibling method #TryRecord.
        ///
        /// @tparam TRecord  The enumeration record type associated with the enum type.
        ///                  This has to be explicitly provided.
        ///                  It is the caller's obligation to ensure that the requested type equals
        ///                  the one associated. Otherwise this method produces undefined behavior.
        ///
        /// @return The record that is associated with this enumeration element.
        //==========================================================================================
        template<typename TRecord>
        const TRecord&  GetRecord()
        {
            const void* result= enums::detail::getEnumRecord( TypeID(), Integral() );
            #if ALIB_STRINGS
                ALIB_ASSERT_ERROR( result != nullptr, "BOXING",
                  NString128() << "Enum Record for type <" << lang::DbgTypeDemangler(TypeID()).Get()
                               << ">(" << Integral()   << ") not found." )
            #else
                ALIB_ASSERT_ERROR( result != nullptr,
                                   "BOXING: Enum Record for type not found: ",
                                   DbgTypeDemangler(TypeID()).Get() )
            #endif


            return *reinterpret_cast<const TRecord*>( result );
        }

        //==========================================================================================
        /// Returns a pointer to the \ref alib_enums_records "ALib Enum Record" associated with this
        /// the enumeration element represented by this instance.
        /// If no enum record was is defined, \c nullptr is returned.
        ///
        /// \see
        /// - Namespace function \alib{enums::TryRecord} of module \alib_enums, which provides the
        ///   standard way of accessing enum records for enum elements known at compile-time.
        /// - Sibling method #GetRecord.
        ///
        /// @tparam TRecord  The enumeration record type associated with the enum type.
        ///                  This has to be explicitly provided.
        ///                  It is the caller's obligation to ensure that the requested type equals
        ///                  the one associated. Otherwise this method produces undefined behavior.
        ///
        /// @return A pointer to the record that is associated with this enumeration element,
        ///         respectively \c nullptr if no record was found.
        //==========================================================================================
        template<typename TRecord>
        const TRecord*  TryRecord()
        {
            return reinterpret_cast<const TRecord*>( enums::detail::getEnumRecord( TypeID(), Integral() ) );
        }

    #endif // ALIB_ENUMS

}; // class Enum

} // namespace alib[::boxing]

/// Type alias in namespace \b alib.
using     Enum=              boxing::Enum;

} // namespace [alib]

// #################################################################################################
// T_Append<Enum>
// #################################################################################################
#if ALIB_STRINGS

    #if  !defined (HPP_ALIB_STRINGS_ASTRING)
    #   include "alib/strings/astring.hpp"
    #endif


    namespace alib {  namespace strings {
    // Faking all template specializations of namespace strings for doxygen into namespace
    // strings::APPENDABLES to keep the documentation of namespace string clean!
    #if DOXYGEN
    namespace APPENDABLES {
    #endif

        /// Specialization of functor \alib{strings;T_Append} for type \alib{boxing;Enum}.
        /// @tparam TChar      The character type of the target \b %Astring.
        /// @tparam TAllocator The allocator that the target \b %AString uses, as prototyped
        ///                    with \alib{lang;Allocator}.
        template<typename TChar, typename TAllocator>  struct T_Append<boxing::Enum, TChar,TAllocator>
        {
            //======================================================================================
            /// Writes the given boxed object. This is done by invoking box-function
            /// \alib{boxing;FAppend} on box \p{value}.
            ///
            /// @param target  The \b AString that \b Append was invoked on.
            /// @param value   The enum-box to its contents to \p{target}.
            //======================================================================================
            void operator()( TAString<TChar,TAllocator>& target, const boxing::Enum& value )
            {
                value.CastToBox().Call<boxing::FAppend<TChar,TAllocator>>( target );
            }

        };

    #if DOXYGEN
    }    // namespace alib::strings[::appendables]
    #endif
    }}  // namespace [alib::strings]

#endif // ALIB_STRINGS


#endif // HPP_ALIB_BOXING_ENUM


//==================================================================================================
/// \file
/// This header-file is part of the module \alib_enumops of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace enumops{

//##################################################################################################
// struct IterableTraits
//##################################################################################################


//==================================================================================================
/// A simple type trait that - if specialized - enables standard and range-based C++ iteration of the
/// elements of an enumeration.
/// Specializations have to declare <c>constexpr</c> fields #Begin and #End, as documented with this
/// type.
///
/// \note
///    The unspecialized version of this struct is empty.
///
/// If specialized, the following entities become available:
/// - \alib{enumops::iterable;operator+}
/// - \alib{enumops::iterable;operator-}
/// - struct \alib{enumops;EnumIterator}
///
/// \attention
///   Likewise with the operators introduced with other type traits of this module,
///   this documentation "fakes" the operators into namespace
///   <c>alib::enumops::iterable</c>, while in fact they are defined in the global
///   namespace!<br>
///   See \ref alib_enums_arithmetic_standard "corresponding note" in the Programmer's Manual
///   for details.
///
/// <b>Restrictions</b><br>
/// For technical reasons, this concept is not applicable to enum types that are defined as
/// \c private or \c protected inner types of structs or classes.
///
/// \see
///   - Macros \ref ALIB_ENUMS_MAKE_ITERABLE and \ref ALIB_ENUMS_MAKE_ITERABLE_BEGIN_END, which
///     specialize this type trait for a given enumeration type.
///   - Type \alib{enumops;EnumIterator} used to perform iterations.
///   - For details and a source code sample see chapter \ref alib_enums_iter "3. Enum Iteration"
///     of the Programmer's Manual of the module \alib_enumops.
///
/// @tparam TEnum      The enum type to enable iteration for.
//==================================================================================================
template<typename TEnum>
requires std::is_enum_v<TEnum>
struct IterableTraits  : std::false_type
{
    #if DOXYGEN
        /// Specializations have to implement this static method to return the first enum element
        /// of the iteration.
        ///
        /// @return The first enumeration element.
        static constexpr   TEnum    Begin;
    #endif

    #if DOXYGEN
        /// Specializations have to implement this static method to return the element value after
        /// the last enum element of the iteration.
        ///
        /// @return The element after the last enumeration element.
        static constexpr   TEnum    End;
    #endif
};

ALIB_WARNINGS_IGNORE_DOCS

/// A concept to identify "iterable enum types".
/// These are types for which a specialization of type trait \alib{enumops;IterableTraits}
/// is defined.
/// @tparam TEnum   The type to be tested.
template <typename TEnum>
concept IsIterable =  std::same_as< const TEnum, decltype(IterableTraits<TEnum>::Begin) >;

ALIB_WARNINGS_RESTORE

}} // namespace [alib::enumops::]

//##################################################################################################
// Operators for iterable enums
//##################################################################################################

// For documentation, all operators and enum-related template functions are faked into namespace
// alib::enumops.
#if DOXYGEN
namespace alib {  namespace enumops{
/// Operators available to elements of enumerations if \alib{enumops;IterableTraits} is
/// specialized.
///
/// \note
///   This namespace exits only in the documentation to collect the operators.
///   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b>.
namespace iterable {
#endif

/// Add operator usable with scoped enum types and integral values.
///
/// Selected by the compiler only if \alib{enumops;IterableTraits} is specialized for
/// enum type \p{TEnum}.
///
/// @tparam TEnum      Enumeration type.
/// @tparam TRhs       The type of the summand.
/// @param  element    First operand of \p{TEnum} type.
/// @param  summand    The summand.
/// @return The <em>"summand-th"</em> enum element after \p{element} .
ALIB_EXPORT
template<typename TEnum, typename TRhs= int>
requires (  alib::enumops::IsIterable<TEnum>  &&  std::integral<TRhs> )
constexpr TEnum operator+  (TEnum element, TRhs summand)                                  noexcept {
    return TEnum(   alib::UnderlyingIntegral(element)
                  + static_cast<typename std::underlying_type<TEnum>::type>(summand) );
}

/// Subtract operator usable with scoped enum types and integral values.
///
/// Selected by the compiler only if \alib{enumops;IterableTraits} is specialized for
/// enum type \p{TEnum}.
///
/// @tparam TEnum      Enumeration type.
/// @tparam TRhs       The type of the subtrahend.
/// @param  element    First operand of \p{TEnum} type.
/// @param  subtrahend The subtrahend.
/// @return The <em>"subtrahend-th"</em> enum element before \p{element} .
ALIB_EXPORT
template<typename TEnum, typename TRhs= int>
requires (  alib::enumops::IsIterable<TEnum>  &&  std::integral<TRhs> )
constexpr TEnum operator-  (TEnum                                      element,
                            typename std::underlying_type<TEnum>::type subtrahend)        noexcept {
    return TEnum(   alib::UnderlyingIntegral(element)
                  - static_cast<typename std::underlying_type<TEnum>::type>(subtrahend) );
}


// Faking all operators and enum-related template functions to namespace alib::enumops
#if DOXYGEN
}
#else
ALIB_EXPORT namespace alib {  namespace enumops {
#endif

//##################################################################################################
// EnumIterator
//##################################################################################################
/// Implements a \c std::iterator_traits class for scoped and non-scoped enum types.
/// The documentation is found with the type trait \alib{enumops;IterableTraits}, which needs
/// to be specialized for template type \p{TEnum}.
/// For other types, this class is not constructible.
/// @tparam TEnum  The enum type to iterate over.
template<typename TEnum>
requires alib::enumops::IsIterable<TEnum>
struct EnumIterator
{
    /// Default constructor.
    EnumIterator()                                                                         =default;

    /// Implementation of \c std::iterator_traits for enum type \p{TEnum}. This class exposes
    /// #ConstIterator which uses <c>const TEnum*</c> and <c>const TEnum&</c> as
    /// pointer and reference types.
    ///
    /// As the name of the class indicates, this iterator satisfies the C++ standard library concept
    /// \https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator}.
    template<typename TPointer, typename TReference>
    class TRandomAccessIterator
    {
        using iterator_category = std::random_access_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
        using value_type        = TEnum                          ;  ///< Implementation of <c>std::iterator_traits</c>.
        using difference_type   = std::ptrdiff_t                 ;  ///< Implementation of <c>std::iterator_traits</c>.
        using pointer           = TPointer                       ;  ///< Implementation of <c>std::iterator_traits</c>.
        using reference         = TReference                     ;  ///< Implementation of <c>std::iterator_traits</c>.

      protected:
        /// The actual enum element.
        TEnum p;

            /// The underlying integer type.
            using TIntegral= typename std::underlying_type<TEnum>::type;

      public:
        /// Constructor.
        /// @param pp Our initial value
        constexpr
        explicit TRandomAccessIterator( TEnum pp = TEnum(0) ) : p(pp)                             {}

      //############################ To satisfy concept of  InputIterator ##########################

        /// Prefix increment operator.
        /// @return A reference to ourselves.
        TRandomAccessIterator& operator++() {
            if constexpr( !IsBitwise<TEnum> )
                p= p + 1;
            else
                p= TEnum( UnderlyingIntegral( p ) << 1 );

            return *this;
        }

        /// Postfix increment operator.
        /// @return A reference to ourselves.
        TRandomAccessIterator operator++(typename std::underlying_type<TEnum>::type) {
            if constexpr( !IsBitwise<TEnum> )
                return TRandomAccessIterator(p= p + 1);
            else
                return TRandomAccessIterator(p= TEnum( UnderlyingIntegral( p ) << 1) );
        }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and given iterator are equal, \c false otherwise.
        constexpr
        bool operator==(TRandomAccessIterator other)                  const { return p == other.p; }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and given iterator are not equal, \c false otherwise.
        constexpr
        bool operator!=(TRandomAccessIterator other)             const { return !(*this == other); }

        /// Retrieves the enum element that this iterator references.
        /// @return The enum element.
        constexpr
        TEnum  operator*()                                                       const { return p; }


      //######################## To satisfy concept of  BidirectionalIterator ######################

        /// Prefix decrement operator.
        /// @return A reference to ourselves.
        TRandomAccessIterator& operator--() {
            if constexpr( !IsBitwise<TEnum> )
                p= p + 1;
            else
                p= TEnum(  );
            return *this;
        }


        /// Postfix decrement operator.
        /// @return An iterator that with the old value.
        TRandomAccessIterator operator--(typename std::underlying_type<TEnum>::type) {
            if constexpr( !IsBitwise<TEnum> )
                return TRandomAccessIterator(p= p - 1);
            else
                return TRandomAccessIterator(p= TEnum( UnderlyingIntegral( p ) >> 1) );
        }


      //######################## To satisfy concept of  RandomAccessIterator #######################

        /// Addition assignment.
        /// @param n The value to subtract.
        /// @return A reference to ourselves.
        TRandomAccessIterator& operator+=(TIntegral n) {
            if constexpr( !IsBitwise<TEnum> )
                p= p + n;
            else
                p= TEnum( UnderlyingIntegral( p ) << n );
            return *this;
        }

        /// Subtraction assignment.
        /// @param n The value to subtract.
        /// @return A reference to ourselves.
        TRandomAccessIterator& operator-=(TIntegral n) {
            if constexpr( !IsBitwise<TEnum> )
                p= p - n;
            else
                p= TEnum( UnderlyingIntegral( p ) >> n );
        }

        /// Addition.
        /// @param n The value to subtract.
        /// @return A reference to the new iterator.
        TRandomAccessIterator operator+(TIntegral n)                                         const {
            if constexpr( !IsBitwise<TEnum> )
                return TRandomAccessIterator( p + n );
            else
                return TRandomAccessIterator( TEnum( UnderlyingIntegral( p ) << n ) );
        }

        /// Subtraction.
        /// @param n The value to subtract.
        /// @return A reference to the new iterator.
        TRandomAccessIterator operator-(TIntegral n)                                         const {
            if constexpr( !IsBitwise<TEnum> )
                return TRandomAccessIterator( p - n );
            else
                return TRandomAccessIterator( TEnum( UnderlyingIntegral( p ) >> n ) );
        }

        /// Difference (distance) from this iterator to the given one.
        /// @param other  The iterator to subtract
        /// @return The iterator to subtract.
        std::ptrdiff_t operator-(TRandomAccessIterator other)                                const {
            if constexpr( !IsBitwise<TEnum> )
                return static_cast<std::ptrdiff_t>(UnderlyingIntegral(p) - UnderlyingIntegral(other.p));
            else
                 return   static_cast<std::ptrdiff_t>(lang::MSB(UnderlyingIntegral( p )       ))
                        - static_cast<std::ptrdiff_t>(lang::MSB(UnderlyingIntegral( other.p ) ));
        }

        /// Subscript operator.
        /// @param n  The iterator to subtract
        /// @return <c>*( (*this) + n )</c>.
        TEnum operator[]( std::ptrdiff_t n )                                                 const {
            if constexpr( !IsBitwise<TEnum> )
                return ( p + static_cast<TIntegral>( n ) );
            else
                return TEnum( UnderlyingIntegral( p ) << n );
        }

        //#### Comparison operators (also needed to satisfy concept of RandomAccessIterator) ###

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e smaller than \p{other},
        ///         \c false otherwise.
        bool operator<(TRandomAccessIterator other)                    const { return p < other.p; }

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e smaller than or equal to \p{other},
        ///         \c false otherwise.
        bool operator<=(TRandomAccessIterator other)                  const { return p <= other.p; }


        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e greater than \p{other},
        ///         \c false otherwise.
        bool operator>(TRandomAccessIterator other)                    const { return p > other.p; }

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e greater than or equal to \p{other},
        ///         \c false otherwise.
        bool operator>=(TRandomAccessIterator other)                  const { return p >= other.p; }
    };

/// The constant iterator exposed by this class. A Mutable version is not available.
using ConstIterator= TRandomAccessIterator<const TEnum*, const TEnum&>;


    /// Returns an iterator referring to the start of enumeration \p{TEnum}.
    /// @return The start of the enumeration.
    ConstIterator begin()                                                                    const {
        return ConstIterator(   IsBitwise<TEnum>
                             && UnderlyingIntegral( IterableTraits<TEnum>::Begin ) == 0
                                ? TEnum(1)
                                : IterableTraits<TEnum>::Begin );
    }

    /// Returns an iterator referring the first illegal value of enumeration \p{TEnum}.
    /// @return The end of the enumeration.
    ConstIterator end()                                                                        const
    { return ConstIterator( TEnum(0) + UnderlyingIntegral( IterableTraits<TEnum>::End ) ); }
};  // struct EnumIterator




} // namespace alib[::enumops::]

/// Type alias in namespace \b alib.
template<typename TEnum>
requires enumops::IsIterable<TEnum>
using  EnumIterator=     enumops::EnumIterator<TEnum>;

} // namespace [alib]

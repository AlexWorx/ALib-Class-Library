/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_ITERATABLE
#define HPP_ALIB_ENUMS_ITERATABLE 1

#if !defined (HPP_ALIB_ENUMS_BITWISE)
#   include "alib/enums/bitwise.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL)
#   include "alib/enums/underlyingintegral.hpp"
#endif

#if defined(_WIN32) && !defined (__INTRIN_H_)
    #include <intrin.h>
#endif

#if !defined (_GLIBCXX_CSTDINT) && !defined (_CSTDINT_)
#   include <cstdint>
#endif

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif

namespace aworx { namespace lib { namespace enums {

// #################################################################################################
// struct T_EnumIteratable
// #################################################################################################


/** ************************************************************************************************
 * Simple TMP struct that - if specialized - enables standard and range-based C++ iteration of the
 * elements of an enumeration.
 * Specializations have to declare <c>constexpr</c> fields #Begin and #End, as documented with this
 * type.
 *
 * \note
 *    The unspecialized version of this struct is empty.
 *
 * If specialized, the following entities become available:
 * - \alib{enums::iteratable,operator+}
 * - \alib{enums::iteratable,operator-}
 * - struct \alib{enums,EnumIterator}
 *
 * \attention
 *   Likewise with the operators introduced with other TMP structs of this module,
 *   this documentation "fakes" the operators into namespace
 *   <c>aworx::lib::enums::iteratable</c>, while in fact they are defined in the global
 *   namespace!<br>
 *   See \ref alib_enums_arithmethic_standard "corresponding note" in the Programmer's Manual
 *   for details.
 *
 * <b>Restrictions</b><br>
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * \see
 *   - Macros \ref ALIB_ENUMS_MAKE_ITERATABLE and \ref ALIB_ENUMS_MAKE_ITERATABLE_BEGIN_END, which
 *     specialize this TMP struct for a given enumeration type.
 *   - Type \alib{enums,EnumIterator} used to perform iterations.
 *   - For details and a source code sample see chapter \ref alib_enums_iter "3. Enum Iteration"
 *     of the Programmer's Manual of module \alib_enums.
 *
 * @tparam TEnum      The enum type to enable iteration for.
 * @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
 *                    with specializations of this struct.
 *                    It is used to ensure that template parameter \p{TEnum} is an enumeration type.
 **************************************************************************************************/
template<typename TEnum,
         typename TEnableIf= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsIteratable
{
    #if defined(ALIB_DOX)
        /**
         * Specializations have to implement this static method to return the first enum element
         * of the iteration.
         *
         * @return The first enumeration element.
         */
        static constexpr   TEnum    Begin;
    #endif

    #if defined(ALIB_DOX)
        /**
         * Specializations have to implement this static method to return the element value after
         * the last enum element of the iteration.
         *
         * @return The element after the last enumeration element.
         */
        static constexpr   TEnum    End;
    #endif

};

}}} // namespace [aworx::lib::enums]

// #################################################################################################
// Helper Macros
// #################################################################################################

#define  ALIB_ENUMS_MAKE_ITERATABLE_BEGIN_END(TEnum, StartElement, StopElement )                      \
namespace aworx { namespace lib { namespace enums {                                                \
template<> struct T_EnumIsIteratable<TEnum>                                                        \
{                                                                                                  \
    static constexpr   TEnum    Begin    = StartElement;                                           \
    static constexpr   TEnum    End      = StopElement;                                            \
};}}}

#define  ALIB_ENUMS_MAKE_ITERATABLE(TEnum, StopElement )                                              \
ALIB_ENUMS_MAKE_ITERATABLE_BEGIN_END( TEnum, TEnum(0),  StopElement )


// #################################################################################################
// Operators for iteratable enums
// #################################################################################################

// For documentation, all operators and enum related template functions are faked into namespace
// aworx::lib::enums
#if defined(ALIB_DOX)
namespace aworx { namespace lib { namespace enums {
/**
 * Operators available to elements of enumerations if \alib{enums,T_EnumIsIteratable} is
 * specialized.
 *
 * \note
 *   This namespace exits only in the documentation to collect the operators.
 *   When parsed by a C++ compiler, <b>the operators reside in the global namespace</b>.
 */
namespace iteratable {
#endif

#if defined(ALIB_DOX)
/**
 * Add operator useable with scoped enum types and integral values.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsIteratable} is specialized for
 * enum type \p{TEnum}.
 *
 * @tparam TEnum      Enumeration type.
 * @param  element    First operand of \p{TEnum} type.
 * @param  addend     The addend as of \c int type.
 * @return The <em>"addend-th"</em> enum element after \p{element} .
 */
template<typename TEnum>
constexpr
TEnum    operator+  (TEnum element, int addend) noexcept(true);
#else
template<typename TEnum>
constexpr
ATMP_T_IF(TEnum, ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) )
operator+  (TEnum element, int addend) noexcept(true)
{
    return TEnum( aworx::UnderlyingIntegral(element) + addend );
}
#endif

#if defined(ALIB_DOX)
/**
 * Subtract operator useable with scoped enum types and integral values.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsIteratable} is specialized for
 * enum type \p{TEnum}.
 *
 * @tparam TEnum      Enumeration type.
 * @param  element    First operand of \p{TEnum} type.
 * @param  subtrahend The subtrahend as of \c int type.
 * @return The <em>"subtrahend-th"</em> enum element before \p{element} .
 */
template<typename TEnum>
constexpr
TEnum    operator-  (TEnum element, int subtrahend) noexcept(true);
#else

template<typename TEnum>
constexpr
ATMP_T_IF(TEnum, ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) )
operator-  (TEnum element, int subtrahend) noexcept(true)
{
    return TEnum( aworx::UnderlyingIntegral(element) - subtrahend );
}
#endif


// Faking all operators and enum related template functions to namespace aworx::lib::enums
#if defined(ALIB_DOX)
}
#else
    namespace aworx { namespace lib { namespace enums {
#endif


// #################################################################################################
// Enum
// #################################################################################################

#if !defined(ALIB_DOX)

    // anonymous namespace method to calculate the highest bit. Need in bitwise iterator.
    namespace {
        inline int getHighestBit(uint64_t value)
        {
            int digits;
            #if defined(__GNUC__) || defined(_WIN64)

                #if defined(__GNUC__)
                    digits= 64 - __builtin_clzll( value );
                #else
                    unsigned long Index;
                    _BitScanReverse64( &Index, value );
                    digits= Index + 1;
                #endif

            #else
                // rough
                digits =    value < ( static_cast<uint64_t>(1) <<  8) ? 8
                          : value < ( static_cast<uint64_t>(1) << 16) ? 16
                          : value < ( static_cast<uint64_t>(1) << 32) ? 32
                          : value < ( static_cast<uint64_t>(1) << 48) ? 48  : 64;

                // fine with loop
                uint64_t actSize=  static_cast<uint64_t>(1) << (digits - 1);
                while( value < actSize )
                {
                    --digits;
                    actSize >>= 1;
                }
            #endif

            return digits;
        }
    } // namespace [anonymous]

#endif // !defined(ALIB_DOX)

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Implements a \c std::iterator class for scoped and non-scoped enum types.
 * The documentation is found with TMP struct \alib{enums,T_EnumIsIteratable}, which needs to be
 * specialized for template type \p{TEnum}. For other types, this class is not constructible.
 *
 * @tparam TEnum      The enum type to iterate over.
 * @tparam TEnableIf  This parameter has a default expressions and <b>must not</b> be provided
 *                    with specializations of this struct.
 *                    It is used internally to ensure that only if a specialization of
 *                    \alib{enums,T_EnumIsIteratable,T_EnumIsIteratable<TEnum>} exists, this class
 *                    is available.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf>
struct EnumIterator
#else
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) ) >
struct EnumIterator
#endif
{
    /** ********************************************************************************************
     * Default constructor.
     **********************************************************************************************/
    EnumIterator()= default;

    /** ********************************************************************************************
     * Implementation of \c std::iterator for enum type \p{TEnum}. This class exposes
     * #ConstIterator which uses <c>const TEnum*</c> and <c>const TEnum&</c> as
     * pointer and reference types.
     *
     * As the name of the class indicates, this iterator satisfies the C++ standard library concept
     * \https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator}.
     **********************************************************************************************/
    template<typename TPointer, typename TReference>
    class TRandomAccessIterator
        : public std::iterator< std::random_access_iterator_tag,  // iterator_category
                                TEnum,                            // value_type
                                std::ptrdiff_t,                   // distance
                                TPointer,                         // pointer
                                TReference                        // reference
                              >
    {
        protected:
            /** The actual enum element. */
            TEnum p;

            /** The underlying integer type. */
            using TIntegral= typename std::underlying_type<TEnum>::type;


        public:
            /** Constructor.
             *  @param _p Our initial value       */
            constexpr
            explicit TRandomAccessIterator( TEnum _p = TEnum(0) ) : p(_p)
            {
            }

        //######################   To satisfy concept of  InputIterator   ######################

            /** Prefix increment operator.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator++()
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p + 1;
                else
                    p= TEnum( UnderlyingIntegral( p ) << 1 );

                return *this;
            }

            /** Postfix increment operator.
             *  @return A reference to ourselves. */
            TRandomAccessIterator operator++(int)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator(p= p + 1);
                else
                    return TRandomAccessIterator(p= TEnum( UnderlyingIntegral( p ) << 1) );
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are equal, \c false otherwise. */
            constexpr
            bool operator==(TRandomAccessIterator other)             const
            {
                return p == other.p;
            }

            /** Comparison operator.
             *  @param other  The iterator to compare ourselves to.
             *  @return \c true if this and given iterator are not equal, \c false otherwise. */
            constexpr
            bool operator!=(TRandomAccessIterator other)             const
            {
                return !(*this == other);
            }

            /** Retrieves the enum element that this iterator references.
             * @return The enum element.                               */
            constexpr
            TEnum  operator*()                                          const
            {
                return p;
            }


        //##################   To satisfy concept of  BidirectionalIterator   ##################

            /** Prefix decrement operator.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator--()
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p + 1;
                else
                    p= TEnum(  );
                return *this;
            }


            /** Postfix decrement operator.
             *  @return An iterator that with the old value. */
            TRandomAccessIterator operator--(int)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator(p= p - 1);
                else
                    return TRandomAccessIterator(p= TEnum( UnderlyingIntegral( p ) >> 1) );
            }


        //##################   To satisfy concept of  RandomAccessIterator   ###################

            /** Addition assignment.
             *  @param n The value to subtract.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator+=(TIntegral n)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p + n;
                else
                    p= TEnum( UnderlyingIntegral( p ) << n );
                return *this;
            }

            /** Subtraction assignment.
             *  @param n The value to subtract.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator-=(TIntegral n)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p - n;
                else
                    p= TEnum( UnderlyingIntegral( p ) >> n );
            }

            /** Addition.
             *  @param n The value to subtract.
             *  @return A reference to the new iterator. */
            TRandomAccessIterator operator+(TIntegral n)       const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator( p + n );
                else
                    return TRandomAccessIterator( TEnum( UnderlyingIntegral( p ) << n ) );
            }

            /** Subtraction.
             *  @param n The value to subtract.
             *  @return A reference to the new iterator. */
            TRandomAccessIterator operator-(TIntegral n)       const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator( p - n );
                else
                    return TRandomAccessIterator( TEnum( UnderlyingIntegral( p ) >> n ) );
            }

            /** Difference (distance) from this iterator to the given one.
             *  @param other  The iterator to subtract
             *  @return The iterator to subtract.    */
            std::ptrdiff_t operator-(TRandomAccessIterator other)   const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return static_cast<std::ptrdiff_t>(UnderlyingIntegral(p) - UnderlyingIntegral(other.p));
                else
                     return   static_cast<std::ptrdiff_t>(getHighestBit(static_cast<uint64_t>( UnderlyingIntegral( p )       )))
                            - static_cast<std::ptrdiff_t>(getHighestBit(static_cast<uint64_t>( UnderlyingIntegral( other.p ) )));
            }

            /** Subscript operator.
             *  @param n  The iterator to subtract
             *  @return <c>*( (*this) + n )</c>.
             */
            TEnum operator[]( std::ptrdiff_t n )   const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return ( p + static_cast<TIntegral>( n ) );
                else
                    return TEnum( UnderlyingIntegral( p ) << n );
            }

        //#### Comparison operators (also needed to satisfy concept of RandomAccessIterator) ###

            /** Compares this iterator with the given one.
             *  @param other  The iterator to compare
             *  @return \c true if this iterator is \e smaller than \p{other},
             *          \c false otherwise. */
            bool operator<(TRandomAccessIterator other)   const
            {
                return p < other.p;
            }

            /** Compares this iterator with the given one.
             *  @param other  The iterator to compare
             *  @return \c true if this iterator is \e smaller than or equal to \p{other},
             *          \c false otherwise. */
            bool operator<=(TRandomAccessIterator other)   const
            {
                return p <= other.p;
            }


            /** Compares this iterator with the given one.
             *  @param other  The iterator to compare
             *  @return \c true if this iterator is \e greater than \p{other},
             *          \c false otherwise. */
            bool operator>(TRandomAccessIterator other)   const
            {
                return p > other.p;
            }

            /** Compares this iterator with the given one.
             *  @param other  The iterator to compare
             *  @return \c true if this iterator is \e greater than or equal to \p{other},
             *          \c false otherwise. */
            bool operator>=(TRandomAccessIterator other)   const
            {
                return p >= other.p;
            }
    };

    /**
     * The constant iterator exposed by this class. A Mutable version is not available.
     */
    using ConstIterator= TRandomAccessIterator<const TEnum*, const TEnum&>;


    /**
     * Returns an iterator referring to the start of enumeration \p{TEnum}.
     * @return The start of the enumeration.
     */
    ConstIterator begin()                       const
    {
        return ConstIterator(   T_EnumIsBitwise<TEnum>::value
                             && UnderlyingIntegral( T_EnumIsIteratable<TEnum>::Begin ) == 0
                                ? TEnum(1)
                                : T_EnumIsIteratable<TEnum>::Begin );
    }

    /**
     * Returns an iterator referring the first illegal value of enumeration \p{TEnum}.
     * @return The end of the enumeration.
     */
    ConstIterator end()                         const
    {
        return ConstIterator( TEnum(0) + UnderlyingIntegral( T_EnumIsIteratable<TEnum>::End ) );
    }

};
}} // namespace aworx[::lib::enums]

#if defined(ALIB_DOX)
/// Type alias in namespace #aworx.
template<typename TEnum, typename TEnableIf= void>
using  EnumIterator=     lib::enums::EnumIterator<TEnum, TEnableIf>;
#else
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(lib::enums::T_EnumIsIteratable<TEnum>::Begin)) ) >
using  EnumIterator=     lib::enums::EnumIterator<TEnum, TEnableIf>;
#endif

} // namespace [aworx]

#endif // HPP_ALIB_ENUMS_ITERATABLE

// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ENUMS_ENUM_ITERATORS
#define HPP_ALIB_ENUMS_ENUM_ITERATORS 1

#if !defined (HPP_ALIB_ENUMS_ENUM_BITWISE)
#   include "alib/enums/enumbitwise.hpp"
#endif

ALIB_ASSERT_MODULE(ENUMS)

#if !defined (HPP_ALIB_ENUMS_ENUM_VALUE)
#   include "alib/enums/enumvalue.hpp"
#endif

#if !defined (HPP_ALIB_LIB_PREDEF_TMP)
#   include "alib/lib/predef_tmp.hpp"
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


/**
 * # Introduction #
 * Standard iterator functionality  \c std::iterator and C++ 11 keyword <c>for( : )</c> are not
 * supported with C++ enumerations - and this is obvious because enumerations are types and not
 * containers or other iteratable objects.<br>
 *
 * Nevertheless it would still be nice if they were!
 *
 * \~Comment #################################################################################### \~
 * # %ALib %Enum Iteration #
 *
 * To have an easy mechanism for iterating over enum types, this TMP struct may be specialized.
 * The restriction for doing this for template type \p{TEnum} needs to fulfill the following
 * constraint:<br>
 *
 * <b><em>
 *  TEnum is "sparsely" defined, which means the difference of the
 *  integer value which is underlying each of its elements, is always <c>+1</c>, starting from
 *  the first to the last element.<br>
 *  (While the first element does not necessarily need to start with integer value \c 0.)
 * </em></b>
 *
 * \attention \alib is not able to check if this requirement is met for a given type.
 *            It is the users responsibility do ensure this and specialize this TMP struct only for
 *            such types.
 *
 * As a sample, let us look at the simple enum type introduced in documentation of
 * \alib{resources,T_EnumMetaDataDecl}:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_REPEAT
 *
 * It is obvious that this enum fulfills the requirements.
 * In that same sample, a code iterating over all enumerations was presented:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_SAMPLE
 *
 * Here, the enums are stuffed in a \c std::vector to be iteratable. This is inefficient and
 * duplicate code in respect to naming all enums in two places.
 *
 * So let us specialize this struct for \c MyNamespace::Pets. We use helper macro
 * \ref ALIB_ENUM_IS_ITERATABLE to do so:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_MAKE_ITERATABLE
 *
 * Now, we do not need the vector any more. Instead we use template class \alib{enums,EnumIterator},
 * which is provided exactly for this reason, in the \c for statement. The code can be rewritten as
 * follows:
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_SAMPLE_WITH_ITERATOR
 *
 * \~Comment #################################################################################### \~
 * # Details #
 *
 * ### %Enum Types Use "Underlying" Integer Types ###
 * Above we used
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_MAKE_ITERATABLE
 * to announce enum \c Pets to \alib.
 * Besides the enum type, the macro expects the "value of the enum element behind the last one".
 * Well, if you find this phrasing irritating or even wrong, then look at the following
 * correct C++ code:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_IRRITATING
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_IRRITATING_2
 *
 *  Well, that enough to say: It is not our fault, it is no ones fault, C++ is just an effective
 *  language.
 *
 * ### %Add and Subtract Operators ###
 * We look at the same code once more:
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_MAKE_ITERATABLE
 * You might have noticed that the term <c>MyNamespace::Pets::Snake + 1</c> usually is not valid
 * C++ code, as we are adding an integer value to a C++ 11 scoped enum type.
 *
 * The reason why this still compiles is that \alib provides operators
 * - \ref operator+<TEnum, int>
 * - \ref operator-<TEnum, int>
 * which accepts an \p{TEnum} at the left hand side and an \c int on the right.
 *
 * Similar to various operator functions provided with \alib that are only applicable to types that
 * have a specialization of TMP struct \alib{enums,T_EnumIsBitwise} in place, these operators
 * are only applicable to types that have a specialization of this TMP struct,
 * \b %T_EnumIsIteratable in place, what is just happening within that code line!
 *
 *
 * ### Stop Enums ###
 * In the sample discussed,  <c>Pets::Snake + 1</c> was used as the "end value" of an iteration.
 * This is error prone in the respect that if the enumeration type gets extended, our macro
 * invocation might be changes, as <c>Pets::Snake</c> then is not the last in the list.
 *
 * A way out, is to add a "stopper" element to the enumeration and name it special, e.g. in
 * upper case <c>"END_OF_ENUM"</c>. It is then rather unlikely, that some programmer would
 * put a new element behind this one. The macro invocation would never be needed to changed:
 *
 *      ALIB_ENUM_IS_ITERATABLE(MyEnum, MyEnum::END_OF_ENUM )
 *
 * A next advantage is that within the enum declaration itself it becomes obvious that this is
 * an iteratable enum type and somewhere the specialization for \alib{enums,T_EnumIsIteratable}
 * is found.<br>
 * Of-course, the drawback is that an enum element is presented to the C++ compiler that is not
 * an element like the other ones...
 *
 * In consideration to \alib \alib{resources,T_EnumMetaDataDecl,enum meta data feature}, adding
 * such a stopper is no problem. At least for the built-in mechanisms. When custom meta data types
 * are used, then the programmer may judge which way to go.
 *
 * ### Start Enums ###
 * So far, all our samples used macro
 * - \ref ALIB_ENUM_IS_ITERATABLE
 *
 * to specialize this struct. In fact, this macro is just a shortcut to macro
 * - \ref ALIB_ENUM_IS_ITERATABLE_BEGIN_END
 *
 * passing <c>TEnum(0)</c> as a start value.
 *
 * This lifts the restriction of having integer \c 0 underlying the first enum element.
 * To do might be useful when different types  scoped enum values passed around
 * as values (which can be done in a type-safe way using \alib \alib{boxing,Enum}). Of-course,
 * this is never recommended, but there are situations where it is just a must, i.e. if result
 * codes have to be passed to libraries of different programming languages. It is still better
 * to use scope enums internally in the C++ code and "export" their values as needed than
 * using non-scoped enums internally.
 *
 * ### Iterator Type ###
 * The \c std::iterator returned with methods \alib{enums,EnumIterator::begin} and
 * \alib{enums,EnumIterator::begin} implements the standard library concept of "RandomAccessIterator".
 * Hence, it has various operators, including subscript \c operator[].
 *
 * \~Comment #################################################################################### \~
 * ## Bitwise %Enums And Iteration ##
 * Iteration works well, if an enumeration is declared \alib{enums,T_EnumIsBitwise,bitwise}.
 * The restriction described in the introduction above, that the enum must not be "sparsely"
 * defined, in this case means that, every next enum element has the next bit set,
 * hence its internal value is doubled.
 *
 * If this struct is specialized for a bitwise enum type using shortcut macro
 * \ref ALIB_ENUM_IS_ITERATABLE, the start element with integer value \c 1 is chosen.
 * If \ref ALIB_ENUM_IS_ITERATABLE_BEGIN_END is used, iteration might start on higher values.
 *
 * In short: everything said above, equally applies to bitwise enumerations.
 *
 *
 * \~Comment #################################################################################### \~
 * ## Performance Considerations ##
 * Class \alib{enums,EnumIterator} is empty in respect to fields. Created on the stack there
 * is no performance penalty. The same is true for the internal iterator type, which is
 * returned with class \alib{enums,EnumIterator::begin} and class \alib{enums,EnumIterator::end}.
 * This iterator class uses an \p{TEnum} element as its only field member.
 * While the code with operators, casting and conversion seems quite complex, at least with
 * compiler optimizations turned on (release builds), the loop will perform the same as an integer
 * while loop:
 *
 *      int i= 0; int stop= 5;
 *      while( i++ < stop) { ... }
 *
 * This is a matter of the magic of C++ templates!
 *
 *
 * \~Comment #################################################################################### \~
 * # Restrictions #
 * For technical reasons, this concept is not applicable to enum types that are defined as
 * \c private or \c protected inner types of structs or classes.
 *
 * \~Comment #################################################################################### \~
 * # Reference Documentation #
 *
 * \note
 *   The methods of the base version of this TMP struct are \b not defined, but rather
 *   "tricked" into this documentation.
 *   Only if specialized with corresponding macros
 *   \ref ALIB_ENUM_IS_ITERATABLE and
 *   \ref ALIB_ENUM_IS_ITERATABLE_BEGIN_END  they will be implemented.
 *
 * @tparam TEnum       The enum type to provide information for.
 * @tparam TCheckEnum  This parameter has a default expressions and <b>must not</b> be provided
 *                     with specializations of this struct.
 *                     It is used internally to ensure that only enum types are to be used with
 *                     the first template parameter \p{TEnum}.
 */
template<typename TEnum,
         typename TCheckEnum= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumIsIteratable
{
    #if ALIB_DOCUMENTATION_PARSER
        /**
         * Specializations have to implement this static method to return the first enum element
         * of the iteration.
         *
         * @return The first enumeration element.
         */
        static constexpr   TEnum    Begin;
    #endif

    #if ALIB_DOCUMENTATION_PARSER
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

#define  ALIB_ENUM_IS_ITERATABLE_BEGIN_END(TEnum, StartElement, StopElement )                      \
namespace aworx { namespace lib { namespace enums {                                                \
template<> struct T_EnumIsIteratable<TEnum>                                                        \
{                                                                                                  \
    static constexpr   TEnum    Begin    = StartElement;                                           \
    static constexpr   TEnum    End      = StopElement;                                            \
};}}}

#define  ALIB_ENUM_IS_ITERATABLE(TEnum, StopElement )                                              \
ALIB_ENUM_IS_ITERATABLE_BEGIN_END( TEnum, TEnum(0),  StopElement )


// #################################################################################################
// Operators for iteratable enums
// #################################################################################################

// For documentation, we are faking all operators and enum related template functions to namespace
// aworx::lib::enums
#if ALIB_DOCUMENTATION_PARSER
namespace aworx { namespace lib { namespace enums {
#endif

#if ALIB_DOCUMENTATION_PARSER
/**
 * Add operator useable with scoped enum types and integer values.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsIteratable} is specialized for
 * enum type \p{TEnum}.
 *
 * @param  element    First operand of \p{TEnum} type.
 * @param  addend     The addend as of \c int type.
 * @tparam TEnum      Enumeration type.
 * @tparam TEnableIf  Internal. Do \b not specify.<br>
 *                    (Used to select this operator for types that have
 *                    \alib{enums,T_EnumIsIteratable} specialized.)
 * @return The <em>"addend-th"</em> enum element after \p{element} .
 */
template<typename TEnum, typename TEnableIf= void>
constexpr
TEnum    operator+  (TEnum element, int addend) noexcept(true);
#else

template<typename TEnum, typename TEnableIf
#if !ALIB_DOCUMENTATION_PARSER
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) )
#endif
>
constexpr
TEnum    operator+  (TEnum element, int addend) noexcept(true)
{
    return TEnum( aworx::EnumValue(element) + addend );
}
#endif

#if ALIB_DOCUMENTATION_PARSER
/**
 * Subtract operator useable with scoped enum types and integer values.
 *
 * Selected by the compiler only if \alib{enums,T_EnumIsIteratable} is specialized for
 * enum type \p{TEnum}.
 *
 * @param  element    First operand of \p{TEnum} type.
 * @param  subtrahend The subtrahend as of \c int type.
 * @tparam TEnum      Enumeration type.
 * @tparam TEnableIf  Internal. Do \b not specify.<br>
 *                    (Used to select this operator for types that have
 *                    \alib{enums,T_EnumIsIteratable} specialized.)
 * @return The <em>"subtrahend-th"</em> enum element before \p{element} .
 */
template<typename TEnum, typename TEnableIf= void>
constexpr
TEnum    operator-  (TEnum element, int subtrahend) noexcept(true);
#else

template<typename TEnum, typename TEnableIf
#if !ALIB_DOCUMENTATION_PARSER
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) )
#endif
        >
constexpr
TEnum    operator-  (TEnum element, int subtrahend) noexcept(true)
{
    return TEnum( aworx::EnumValue(element) - subtrahend );
}
#endif


// We are faking all operators and enum related template functions to namespace aworx::lib::enums
#if !ALIB_DOCUMENTATION_PARSER
namespace aworx { namespace lib { namespace enums {
#endif


// #################################################################################################
// Enum
// #################################################################################################

#if !ALIB_DOCUMENTATION_PARSER

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
                    digits--;
                    actSize >>= 1;
                }
            #endif

            return digits;
        }
    } // namespace [anonymous]

#endif // !ALIB_DOCUMENTATION_PARSER

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
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) ) >
struct EnumIterator
{

    /** ********************************************************************************************
     * Default constructor.
     **********************************************************************************************/
    EnumIterator()= default;

    /** ****************************************************************************************
     * Implementation of \c std::iterator for enum type \p{TEnum}. This class exposes
     * #ConstIterator which uses <c>const TEnum*</c> and <c>const TEnum&</c> as
     * pointer and reference types.
     *
     * As the name of the class indicates, this iterator satisfies the std library concept
     * [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator).
     ******************************************************************************************/
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
            using TEnumAsInt= typename std::underlying_type<TEnum>::type;


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
                    p= TEnum( EnumValue( p ) << 1 );

                return *this;
            }

            /** Postfix increment operator.
             *  @return A reference to ourselves. */
            TRandomAccessIterator operator++(int)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator(p= p + 1);
                else
                    return TRandomAccessIterator(p= TEnum( EnumValue( p ) << 1) );
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
                    return TRandomAccessIterator(p= TEnum( EnumValue( p ) >> 1) );
            }


        //##################   To satisfy concept of  RandomAccessIterator   ###################

            /** Addition assignment.
             *  @param n The value to subtract.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator+=(TEnumAsInt n)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p + n;
                else
                    p= TEnum( EnumValue( p ) << n );
                return *this;
            }

            /** Subtraction assignment.
             *  @param n The value to subtract.
             *  @return A reference to ourselves. */
            TRandomAccessIterator& operator-=(TEnumAsInt n)
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    p= p - n;
                else
                    p= TEnum( EnumValue( p ) >> n );
            }

            /** Addition.
             *  @param n The value to subtract.
             *  @return A reference to the new iterator. */
            TRandomAccessIterator operator+(TEnumAsInt n)       const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator( p + n );
                else
                    return TRandomAccessIterator( TEnum( EnumValue( p ) << n ) );
            }

            /** Subtraction.
             *  @param n The value to subtract.
             *  @return A reference to the new iterator. */
            TRandomAccessIterator operator-(TEnumAsInt n)       const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return TRandomAccessIterator( p - n );
                else
                    return TRandomAccessIterator( TEnum( EnumValue( p ) >> n ) );
            }

            /** Difference (distance) from this iterator to the given one.
             *  @param other  The iterator to subtract
             *  @return The iterator to subtract.    */
            std::ptrdiff_t operator-(TRandomAccessIterator other)   const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return static_cast<std::ptrdiff_t>(EnumValue(p) - EnumValue(other.p));
                else
                     return   static_cast<std::ptrdiff_t>(getHighestBit(static_cast<uint64_t>( EnumValue( p )       )))
                            - static_cast<std::ptrdiff_t>(getHighestBit(static_cast<uint64_t>( EnumValue( other.p ) )));
            }

            /** Subscript operator.
             *  @param n  The iterator to subtract
             *  @return <c>*( (*this) + n )</c>.                      */
            TEnum operator[]( std::ptrdiff_t n )   const
            {
                if( !T_EnumIsBitwise<TEnum>::value )
                    return ( p + static_cast<TEnumAsInt>( n ) );
                else
                    return TEnum( EnumValue( p ) << n );
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
                             && EnumValue( T_EnumIsIteratable<TEnum>::Begin ) == 0 ? TEnum(1)
                                                                                   : T_EnumIsIteratable<TEnum>::Begin );
    }

    /**
     * Returns an iterator referring the first illegal value of enumeration \p{TEnum}.
     * @return The end of the enumeration.
     */
    ConstIterator end()                         const
    {
        return ConstIterator( TEnum(0) + EnumValue( T_EnumIsIteratable<TEnum>::End ) );
    }


};
}} // namespace aworx[::lib::enums]

#if ALIB_DOCUMENTATION_PARSER
/// Type alias in namespace #aworx.
template<typename TEnum, typename TEnableIf= void>
using  EnumIterator=     aworx::lib::enums::EnumIterator<TEnum, TEnableIf>;
#else
template<typename TEnum, typename TEnableIf
= ATMP_VOID_IF(ATMP_EQ(const TEnum, decltype(aworx::lib::enums::T_EnumIsIteratable<TEnum>::Begin)) ) >
using  EnumIterator=     aworx::lib::enums::EnumIterator<TEnum, TEnableIf>;
#endif

} // namespace [aworx]

#endif // HPP_ALIB_ENUMS_ENUM_ITERATORS

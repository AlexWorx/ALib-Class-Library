//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {
#include "ALib.Lang.CIFunctions.H"

/// This namespace holds types and functions of the module \alib_lang, which are very close to
/// the C++ language itself.
/// The entities found here are always included in any \alibbuild and are accessed by
/// including the header \implude{Lang}.
///
/// # Reference Documentation #
namespace lang {
/// The C++ language defines the right-hand-side argument of bit shift operations to be of
/// type <c>int</c>. To increase code readability we define this type explicitly.
using   ShiftOpRHS = int;


/// Like C++ keyword <c>sizeof</c> but returns the number of bits of the type of the given value.
/// The return type is <c>int</c> instead of <c>size_t</c>, which satisfies \alib code conventions.
///
/// \note To improve code readability, namely to a) indicate that this is an inlined, constant
///       expression and b) to indicate that this is just using keyword <c>sizeof</c>,
///       as an exception from the rules, this function is spelled in lower case.
///
/// \see Macro \ref bitsof "bitsof(type)", which works directly on the type.
///
/// @param  val  The (sample) value to deduce the type from. Otherwise ignored.
/// @tparam T    The type to receive the size in bits from.
/// @return The size of \p{TIntegral} in bits.
template<typename T> inline constexpr int bitsofval(const T& val)
{
    (void) val; return sizeof(T) * 8;
}

/// Inline namespace function that returns a mask with bits set to \c 1 up to the given
/// binary digit, and bits above to \c 0.
/// If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
/// all bits are set in the returned value.
///
/// \see While this is the fully templated version, with
///      #alib::lang::LowerMask<TIntegral>(ShiftOpRHS), a run-time version is given.
///
/// @tparam TWidth    The number of lower bits to set to \c 1.
/// @tparam TIntegral The integral type to operate on.
/// @return The requested mask.
template<ShiftOpRHS TWidth, typename TIntegral>
requires ( std::is_integral<TIntegral>::value  &&  TWidth < bitsof(TIntegral) )
constexpr TIntegral LowerMask()
{
    ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW
    using TUnsigned= typename std::make_unsigned<TIntegral>::type;
    return TIntegral(~(TUnsigned(~TUnsigned(0)) << TWidth) );
    ALIB_WARNINGS_RESTORE
}

/// Overloaded version handling bit-overflow.
/// \see
///  - Original version of this function.
///  - While this is the fully templated version, with
///    #alib::lang::LowerMask<TIntegral>(ShiftOpRHS), a run-time version is given.
///
/// @tparam TWidth    The number of lower bits to set to \c 1.
/// @tparam TIntegral The integral type to operate on.
/// @return The requested mask.
template<ShiftOpRHS TWidth, typename TIntegral>
requires ( std::is_integral<TIntegral>::value  &&  TWidth >= bitsof(TIntegral) )
constexpr TIntegral LowerMask()                                 { return TIntegral(~TIntegral(0)); }


/// Inline namespace function that returns a mask with bits set to \c 1 up to the given
/// binary digit, and bits above to \c 0.
/// Parameter \p{width} <b>must not be greater or equal</b> to the width of the given
/// \p{TIntegral} type.
/// In debug-compilations, an \ref alib_mod_assert "error is raised" in that case.
///
/// \see A fully templated version usable when the number of bits are known at compile time,
///      is given with #LowerMask<ShiftOpRHS,typename>().
///
/// @tparam TIntegral The integral type to operate on.
/// @param  width     The number of lower bits to set in the mask returned.
/// @return The requested mask.
template<typename TIntegral>
requires  std::integral<TIntegral>
constexpr TIntegral LowerMask( ShiftOpRHS width )
{
    ALIB_ASSERT_ERROR( width < bitsof(TIntegral), "ALIB/BITS",
          "Requested mask width wider than integral: {} >= {}", width, bitsof(TIntegral) )
    using TUnsigned= typename std::make_unsigned<TIntegral>::type;
    return TIntegral(~(TUnsigned(~TUnsigned(0))  << width  ));
}


/// Inline namespace function that returns a mask with bits set to \c 0 up to the given
/// binary digit, and bits above to \c 1.
/// If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
/// all bits are set in the returned value.
///
/// \see While this is the fully templated version, with
///      #alib::lang::UpperMask<TIntegral>(ShiftOpRHS), a run-time version is given.
///
/// @tparam TWidth    The number of lower bits to clear to \c 1.
/// @tparam TIntegral The integral type to operate on.
/// @return The requested mask.
template<ShiftOpRHS TWidth, typename TIntegral>
requires ( std::is_integral<TIntegral>::value  && TWidth < bitsof(TIntegral) )
constexpr TIntegral UpperMask()
{
    using TUnsigned= typename std::make_unsigned<TIntegral>::type;
    return TIntegral((TUnsigned(~TUnsigned(0)) << TWidth));
}

/// Overloaded version handling bit-overflow.
/// \see
///  - Original version of this function.
///  - While this is the fully templated version, with
///      #alib::lang::UpperMask<TIntegral>(ShiftOpRHS), a run-time version is given.
///
/// @tparam TWidth    The number of lower bits to set to \c 1.
/// @tparam TIntegral The integral type to operate on.
/// @return The requested mask.
template<ShiftOpRHS TWidth, typename TIntegral>
requires ( std::is_integral<TIntegral>::value  && TWidth >= bitsof(TIntegral) )
constexpr TIntegral UpperMask()                                             { return TIntegral(0); }


/// Inline namespace function that returns a mask with bits set to \c 0 up to the given
/// binary digit, and bits above to \c 1.
/// Parameter \p width <b>must not be greater or equal</b> to the width of the given
/// \p TIntegral type.
/// In debug compilations, an \ref alib_mod_assert "error is raised" in that case.
///
/// \see A fully templated version usable when the number of bits are known at compile time,
///      is given with #UpperMask<ShiftOpRHS,typename>().
///
/// @tparam TIntegral The integral type to operate on.
/// @param  width     The number of lower bits to clear in the mask returned.
/// @return The requested mask.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr TIntegral UpperMask( ShiftOpRHS width )
{
    ALIB_ASSERT_ERROR( width < bitsof(TIntegral), "ALIB/BITS",
          "Requested mask width wider than integral: {} >= {}", width, bitsof(TIntegral) )
    using TUnsigned= typename std::make_unsigned<TIntegral>::type;
    return TIntegral( (TUnsigned(~TUnsigned(0))  << width)  );
}


/// Inline namespace function that keeps the given number of lower bits and masks the higher ones
/// out of the given integral value.
/// If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
/// all bits are returned.
///
/// \see While this is the fully templated version and hence explicitly constexpression for the
///      reader of a code, with
///      #alib::lang::LowerBits<TIntegral>(lang::ShiftOpRHS,TIntegral), a version is given.
///
/// @param  value     The value to mask.
/// @tparam TWidth    The number of lower bits to keep.
/// @tparam TIntegral The integral type to operate on (deduced by the compiler).
/// @return The given value with the upper remaining bits cleared.
template<ShiftOpRHS TWidth, typename TIntegral>
requires std::integral<TIntegral>
constexpr TIntegral LowerBits( TIntegral value )
{
    if constexpr ( TWidth >= bitsof(TIntegral) )
        return value;
    return value & LowerMask<TWidth,TIntegral>( );
}

/// Inline namespace function that keeps the given number of lower bits and masks the higher ones
/// out of the given integral value.
/// Parameter \p width <b>must not be greater or equal</b> to the width of the given
/// \p TIntegral type. In debug compilations, an \ref alib_mod_assert "error is raised".
///
/// \see A fully templated version usable when the number of bits are known at compile time,
///      is given with #LowerBits<ShiftOpRHS,TIntegral>().
///
/// @param  width     The number of lower bits to keep.
/// @param  value     The value to mask.
/// @tparam TIntegral The integral type to operate on (deduced by the compiler).
/// @return The given value with the upper remaining bits cleared.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr TIntegral LowerBits( ShiftOpRHS width, TIntegral value )
{ return value & LowerMask<TIntegral>( width ); }

/// Returns logarithm base 2 for the size in bits of the template given integral type.<p>
/// Precisely, this function returns:
/// - 3 for  8-bit types,
/// - 4 for 16-bit types,
/// - 5 for 32-bit types,
/// - 6 for 64-bit types, and
/// - 7 for 128-bit types.
/// @tparam TIntegral The integral type to count bits in.
/// @return The number of bits needed to count the bits of type \p TIntegral.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int
Log2OfSize()
{
    static_assert(bitsof(TIntegral) <= 128, "Integrals larger than 128 are not supported.");
    if constexpr (bitsof(TIntegral) == 32) return 5;
    if constexpr (bitsof(TIntegral) == 64) return 7;
    if constexpr (bitsof(TIntegral) ==  8) return 3;
    if constexpr (bitsof(TIntegral) == 16) return 4;
    return 8;
}

/// Returns the number of bits set in an integral value.
/// Internally, this method uses C++20 library function <c>std::popcount</c>.
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test.
/// @return The number of bits set in a value.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int BitCount( TIntegral value )
{ return std::popcount( static_cast<typename std::make_unsigned<TIntegral>::type>( value ) ); }

/// Returns the number of the leading 0-bits in an integral type.
/// Internally, this method uses C++20 library function <c>std::countl_zero</c>.
///
/// \attention
///  This function must not be called with a \p value of <c>0</c>!
///  In debug-compilations, this method \ref alib_mod_assert "raises an ALib error" in this case,
///  while in release-compilations, the result is <em>'undefined'</em>.
///  With function \ref MSB0, an alternative is given which returns \c 0 if the input is \c 0.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. Must not be \c 0.
/// @return The highest bit set in \p value.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int CLZ( TIntegral value )
{
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
                "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )
    #endif
    return std::countl_zero( static_cast<typename std::make_unsigned<TIntegral>::type>( value ) );
}


/// Variant of \ref CLZ which tests the given parameter \p value on \c 0 and returns
/// <c>sizeof(TIntegral) * 8</c> in this case.
/// Otherwise, returns the result of \ref CLZ.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. May be \c 0, which results to the number of bits in
///                   \p TIntegral.
/// @return The number of leading zero-bits in \p value.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int CLZ0( TIntegral value)
{
    if( value == 0 )
        return bitsof(TIntegral);
    return CLZ(value);
}

/// Returns the number of the trailing 0-bits in an integral type.
/// Internally, this method uses C++20 library function <c>std::countr_zero</c>.
///
/// \attention
///  This function must not be called with a \p value of <c>0</c>!
///  In debug-compilations, this method \ref alib_mod_assert "raises an ALib error" in this case,
///  while in release-compilations, the result is <em>'undefined'</em>.
///  With function \ref CTZ0, an alternative is given which returns \c 0 if the input is \c 0.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. Must not be \c 0.
/// @return The lowest bit set in \p value.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int CTZ( TIntegral value )
{
    ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
              "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )
    return std::countr_zero( static_cast<typename std::make_unsigned<TIntegral>::type>( value ) );
}

/// Variant of \ref CTZ which tests given parameter \p value on \c 0 and returns
/// <c>sizeof(TIntegral) * 8</c> in this case.
/// Otherwise, returns the result of \ref CLZ.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. May be \c 0, which results to the number of bits in
///                   \p TIntegral.
/// @return The number of trailing zero-bits in \p value. In case the given value is \c 0,
///         <c>sizeof(Tintegral) * 8</c> is returned.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int CTZ0( TIntegral value )
{
    if( value == 0 )
        return bitsof(TIntegral);
    return CTZ(value);
}

/// Returns the number of the most significant bit in an integral type.
/// Internally, this method uses \alib{lang;CLZ} and returns
///
///      int(sizeof(TIntegral)) * 8 - CLZ(value)
///
/// \attention
///  This function must not be called with a \p value of <c>0</c>!
///  In debug-compilations, this method \ref alib_mod_assert "raises an ALib error" in this case,
///  while in release-compilations, the result is <em>'undefined'</em>.
///  With function \ref MSB0, an alternative is given which returns \c 0 if the input is \c 0.
///
///  \note A corresponding function "LSB", to receive the least significant bit is not given.
///        Instead, use <c>CTZ() + 1</c>.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. Must not be \c 0.
/// @return The highest bit set in \p value. The numbering starts with \c 1 and ends with
///         <c>sizeof(Tintegral) * 8</c>.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int MSB( TIntegral value)
{
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
               "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )
    #endif
    return  bitsof(TIntegral) - CLZ(value);
}

/// Variant of \ref MSB which tests given parameter \p value on \c 0 and returns \c 0 in this
/// case. Otherwise, returns the result of \ref MSB.
///
/// @tparam TIntegral The integral type to operate on.
/// @param  value     The value to test. May be \c 0, which results to \c 0.
/// @return The highest bit set in \p value. The numbering starts with \c 1 and ends with
///         <c>sizeof(Tintegral)* 8</c>. If \p{value} is \c 0, hence no bit is set,
///         \c 0 is returned.
template<typename TIntegral>
requires std::integral<TIntegral>
constexpr int MSB0( TIntegral value)
{
    if( value == 0 )
        return 0;
    return MSB(value);
}
#include "ALib.Lang.CIMethods.H"
}} // namespace [alib::lang]




/** ************************************************************************************************
 * \file
 * This header file is part of file set \alibfs_integers of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_BITS
#define HPP_ALIB_BITS 1

#if !defined(HPP_ALIB_INTEGERS)
#   include "alib/lib/integers.hpp"
#endif

#if !defined (HPP_ALIB_TMP) && !defined(ALIB_DOX)
#   include "alib/lib/tmp.hpp"
#endif

#if defined(_WIN64) && !defined (__INTRIN_H_)
    #include <intrin.h>
#endif

#if !defined (HPP_ALIB_TOOLS)
#   include "alib/lib/tools.hpp"
#endif

#if !defined (_GLIBCXX_NUMERIC_LIMITS) && !defined(_LIMITS_)
#   include <limits>
#endif

namespace aworx { namespace lib {

/** The C++ language defines the right-hand-side argument of bit shift operations to be of
 *  type <c>int</c>. To increase code readability we define this type explicitly.*/
using   ShiftOpRHS = int;

#if defined(ALIB_DOX)

/**
 * Inline namespace function that returns a mask with bits set to \c 1 up to the given
 * binary digit, and bits above to \c 0.
 * If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
 * all bits are set in the returned value.
 *
 * \see While this is the fully templated version, with
 *      #aworx::lib::LowerMask<TIntegral>(ShiftOpRHS), a run-time version is given.
 *
 * @tparam TWidth    The number of lower bits to set to \c 1.
 * @tparam TIntegral The integral type to operate on (deduced by the compiler).
 * @return The requested mask.
 */
template<ShiftOpRHS TWidth, typename TIntegral>
inline constexpr
TIntegral LowerMask();

/**
 * Inline namespace function that returns a mask with bits set to \c 1 up to the given
 * binary digit, and bits above to \c 0.
 * If parameter \p width is greater or equal to the width of the given \p TIntegral type, then
 * all bits are set in the returned value.
 *
 * \see A fully templated version usable when the number of bits are known at compile time,
 *      is given with #aworx::lib::monomem::LowerMask<ShiftOpRHS,typename>().
 *
 * @tparam TIntegral The integral type to operate on (deduced by the compiler).
 * @param  width     The number of lower bits to set in the mask returned.
 * @return The requested mask.
 */
template<typename TIntegral>
inline constexpr
TIntegral LowerMask( ShiftOpRHS width );

/**
 * Inline namespace function that keeps the given number of lower bits and masks the higher ones
 * out of the given integral value.
 * If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
 * all bits are returned.
 *
 * \see While this is the fully templated version and hence explicitly constexpression for the
 *      reader of a code, with
 *      #aworx::lib::LowerBits<TIntegral>(ShiftOpRHS,TIntegral), a version is given.
 *
 * @tparam TIntegral The integral type to operate on (deduced by the compiler).
 * @tparam TWidth    The number of lower bits to keep.
 * @param  value     The value to mask.
 * @return The given value with the upper remaining bits cleared.
 */
template<ShiftOpRHS TWidth, typename TIntegral>
inline constexpr
TIntegral LowerBits( TIntegral value );

/**
 * Inline namespace function that keeps the given number of lower bits and masks the higher ones
 * out of the given integral value.
 * If parameter \p width is greater or equal to the width of the given \p TIntegral type, then
 * all bits are returned.
 *
 * \see A fully templated version usable when the number of bits are known at compile time,
 *      is given with #aworx::lib::LowerBits<ShiftOpRHS,typename>().
 *
 * @tparam TIntegral The integral type to operate on (deduced by the compiler).
 * @param  width     The number of lower bits to keep.
 * @param  value     The value to mask.
 * @return The given value with the upper remaining bits cleared.
 */
template<typename TIntegral>
inline constexpr
TIntegral LowerBits( ShiftOpRHS width, TIntegral value );

/**
 * Returns the number of bits needed to count the bits set in a value of an integral type.<p>
 * For example, a 16-Bit type may may have 0 to 16 bits set, hence 17 possible values, which need
 * 5 bits to be counted.<p>
 * Precisely, this function returns:
 * - 4 for  8-bit types,
 * - 5 for 16-bit types,
 * - 6 for 32-bit types and
 * - 7 for 64-bit types.
 * @tparam TIntegral The integral type to count bits in.
 * @return The number of bits needed to count the bits of type \p TIntegral.
 */
template<typename TIntegral>
inline constexpr
int BitCounterWidth();

/**
 * Returns the number of bits set in an integral value.
 * Internally, this method uses:
 * - On gcc/clang: builtin method \c __builtin_popcount (and variations) which translate to a
 *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
 * - With MS-compilers, intrinsic \c popcount is used.
 * - On other platforms, the number is evaluated programatically by a loop.
 *
 * @tparam TIntegral The integral type to operate on.
 * @param  value     The value to test.
 * @return The number of bits set in a value.
 */
template<typename TIntegral>
constexpr
int BitCount( TIntegral value );

/**
 * Returns the number of the leading 0-bits in an integral type.
 * Internally, this method uses:
 * - On gcc/clang: builtin methods \c __builtin_clz (and variations) which translate to a
 *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
 * - With MS-compilers, intrinsics \c _BitScanReverse and \c _BitScanReverse64 are used.
 * - On other platforms, a binary search is performed.
 *
 * \attention
 *  This function must not be called with a \p value of <c>0</c>!
 *  In debug-compilations, this method raises an \alib assertion in this case, while in
 *  release-compilations, the result is <em>'undefined'.</em>.
 *  With function \ref MSB0, an alternative is given which returns \c 0 if the input is \c 0.
 *
 * @tparam TIntegral The integral type to operate on.
 * @param  value     The value to test. Must not be \c 0.
 * @return The highest bit set in \p value.
 */
template<typename TIntegral>
constexpr
int CLZ( TIntegral value );

/**
 * Variant of \ref CLZ which tests given parameter \p value on \c 0 and returns
 * <c>8 * sizeof(TIntegral)</c> in this case.
 * Otherwise returns the result of \ref CLZ.
 *
 * @tparam TIntegral The integral type to operate on.
 * @param  value     The value to test. May be \c 0, which results to the number of bits in
 *                   \p TIntegral.
 * @return The number of leading zero-bits in \p value.
 */
template<typename TIntegral>
constexpr
int CLZ0( TIntegral value);

/**
 * Returns the number of the most significant bit in an integral type.
 * Internally, this method uses
 * - On gcc/clang: builtin methods \c __builtin_clz (and variations) which translate to a
 *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
 * Internally, this method uses:
 * - On gcc/clang: builtin methods \c __builtin_clz (and variations) which translate to a
 *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
 * - With MS-compilers, intrinsics \c _BitScanReverse and \c _BitScanReverse64 are used.
 * - On other platforms, a binary search is performed.
 *
 * \attention
 *  This function must not be called with a \p value of <c>0</c>!
 *  In debug-compilations, this method raises an \alib assertion in this case, while in
 *  release-compilations, the result is <em>'undefined'</em>.
 *  With function \ref MSB0, an alternative is given which returns \c 0 if the input is \c 0.
 *
 * @tparam TIntegral The integral type to operate on.
 * @param  value     The value to test. Must not be \c 0.
 * @return The highest bit set in \p value.
 */
template<typename TIntegral>
constexpr
int MSB( TIntegral value);

/**
 * Variant of \ref MSB which tests given parameter \p value on \c 0 and returns \c 0 in this
 * case. Otherwise returns the result of \ref MSB.
 *
 * @tparam TIntegral The integral type to operate on.
 * @param  value     The value to test. May be \c 0, which results to \c 0.
 * @return The highest bit set in \p value.
 */
template<typename TIntegral>
constexpr
int MSB0( TIntegral value);


#else // ALIB_DOX


template<ShiftOpRHS TWidth, typename TIntegral>
ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
inline
ALIB_CONSTEXPR17
LowerMask()
{
    if ALIB_CONSTEXPR17 ( TWidth >= ShiftOpRHS(sizeof(TIntegral) * 8) )
        return TIntegral((std::numeric_limits<typename std::make_unsigned<TIntegral>::type>::max)());

    ALIB_WARNINGS_IGNORE_INTEGER_OVERFLOW
    return ( TIntegral(1) << TWidth )   - 1;
    ALIB_WARNINGS_RESTORE
}


template<typename TIntegral>
ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
inline
ALIB_CONSTEXPR17
LowerMask( ShiftOpRHS width )
{
    if ( width >= ShiftOpRHS(sizeof(TIntegral) * 8) )
        return TIntegral((std::numeric_limits<typename std::make_unsigned<TIntegral>::type>::max)());
    return TIntegral(( TIntegral(1) << width ) - 1);
}

template<typename TIntegral>
ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
inline
ALIB_CONSTEXPR17
LowerBits( ShiftOpRHS width, TIntegral value )
{
    return value & LowerMask<TIntegral>( width );
}

template<ShiftOpRHS TWidth, typename TIntegral>
ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
inline
ALIB_CONSTEXPR17
LowerBits( TIntegral value )
{
    return value & LowerMask<TWidth,TIntegral>( );
}


template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
inline constexpr
BitCounterWidth()
{
    return (    sizeof( TIntegral ) == 1 ? 3
             :  sizeof( TIntegral ) == 2 ? 4
             :  sizeof( TIntegral ) == 4 ? 5
             :                             6 ) + 1;

}

template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
ALIB_FORCE_INLINE
ALIB_CONSTEXPR17
BitCount( TIntegral value )
{
    #if defined(_WIN64)
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) <= 4 )
            return __popcnt (static_cast<TUnsigned>( value ));
        return __popcnt64 (static_cast<TUnsigned>( value ));


    #elif defined(__has_builtin) && __has_builtin( __builtin_popcount )    

        using TUnsigned = typename std::make_unsigned<TIntegral>::type;

        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(int      )) return int(__builtin_popcount  ( TUnsigned(value)                ));
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(long     )) return int(__builtin_popcountl ( TUnsigned(value)                ));
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(long long)) return int(__builtin_popcountll( TUnsigned(value)                ));
                                                                     return int(__builtin_popcount  ( static_cast<unsigned int>(value)));

    #else
        // Do it manually:
        int result= 0;
        while( value )
        {
            switch( value & 15 )
            {
                case  0:            break;  //  0=0000
                case  1:                    //  1=0001
                case  2:                    //  2=0010
                case  4:                    //  4=0100
                case  8: result++;  break;  //  8=1000

                case  3:                    //  3=0011
                case  5:                    //  5=0101
                case  6:                    //  6=0110
                case  9:                    //  9=1001
                case 10:                    // 10=1010
                case 12: result+=2; break;  // 12=1100

                case  7:                    //  7=0111
                case 11:                    // 11=1011
                case 13:                    // 13=1101
                case 14: result+=3; break;  // 14=1110

                case 15: result+=4; break;  // 15=1111

            }
            value>>=4;
        }
        return result;
    #endif
}

template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
ALIB_FORCE_INLINE
ALIB_CONSTEXPR17
CLZ( TIntegral value )
{
    ALIB_ASSERT_ERROR( value != 0, "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )

    #if defined(_WIN64)
        unsigned long bitScanReverse;
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) <= 4 )
        {
            _BitScanReverse( &bitScanReverse, uint64_t(value) );
            return 31 - bitScanReverse;
        }
        _BitScanReverse64( &bitScanReverse, uint64_t(value) );
        return 63 - bitScanReverse;

    #elif defined(__has_builtin) && __has_builtin( __builtin_clz )     \
                                 && __has_builtin( __builtin_clzl )    \
                                 && __has_builtin( __builtin_clzll )
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(int      )) return int(__builtin_clz  ( TUnsigned(value)                ));
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(long     )) return int(__builtin_clzl ( TUnsigned(value)                ));
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(long long)) return int(__builtin_clzll( TUnsigned(value)                ));
                                                                     return int(__builtin_clz  ( static_cast<unsigned int>(value)));

    #else
        // Do it manually using binary search:
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;
        ShiftOpRHS msb= 63;
        ShiftOpRHS l= 0;
        ShiftOpRHS r= sizeof(TIntegral) * 8;
        while( l <= r  )
        {
            ShiftOpRHS mid= (l + r) / 2;
            if( (TUnsigned(1) << mid) > value )
            {
                msb= mid -1;
                r= mid - 1;
            }
            else
                l= mid + 1;
        }
        return 63 - msb;

    #endif
}

template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
ALIB_FORCE_INLINE
ALIB_CONSTEXPR17
CLZ0( TIntegral value)
{
    if( value == 0 )
        return 0;
    return CLZ(value);
}

template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
ALIB_FORCE_INLINE
ALIB_CONSTEXPR17
MSB( TIntegral value)
{
    ALIB_ASSERT_ERROR( value != 0, "BITS", "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )

    #if defined(_WIN64)
        DWORD bitScanReverse;
        if ALIB_CONSTEXPR17 (sizeof(TIntegral) <= 4 )
        {
            _BitScanReverse( &bitScanReverse, value );
            return int(bitScanReverse + 1);
        }
        _BitScanReverse64( &bitScanReverse, uint64_t(value) );
        return int(bitScanReverse +1) ;

    #elif defined(__has_builtin) && __has_builtin( __builtin_clz)
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;

        if ALIB_CONSTEXPR17 (sizeof(TIntegral) <= sizeof(unsigned int      ))
            return int(sizeof(unsigned int      )) * 8 - int(__builtin_clz( static_cast<unsigned int>(value)  ));

        #if __has_builtin( __builtin_clzl )
            if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(unsigned long     ))
                return int(sizeof(unsigned long     )) * 8 - int(__builtin_clzl ( TUnsigned(value)                ));
        #endif

        #if __has_builtin( __builtin_clzll )
            if ALIB_CONSTEXPR17 (sizeof(TIntegral) == sizeof(unsigned long long))
                return int(sizeof(unsigned long long)) * 8 - int(__builtin_clzll( TUnsigned(value)                ));
        #endif

        {
            ALIB_ERROR( "BITS", "Unhandled integral size. this must never happen. Size =", sizeof(TIntegral) )
            return int(sizeof(unsigned long long)) * 8 - int(__builtin_clzll( TUnsigned(value)                ));
        }

    #else
        // Do it manually using binary search:
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;
        ShiftOpRHS msb  = 1;
        ShiftOpRHS lower= 0;
        ShiftOpRHS upper= sizeof(TIntegral) * 8 + 1;
        while( lower <= upper )
        {
            ShiftOpRHS mid= ( lower + upper) / 2;
            if( (TUnsigned(1) << mid) > TUnsigned(value) )
            {
                msb  =
                upper= mid - 1;
            }
            else
                lower= mid + 1;
        }
        return lower > int(sizeof(TIntegral) * 8) ? sizeof(TIntegral) * 8
                                                  : msb + 1;
    #endif
}



template<typename TIntegral>
ATMP_T_IF(int, std::is_integral<TIntegral>::value )
ALIB_FORCE_INLINE
ALIB_CONSTEXPR17
MSB0( TIntegral value)
{
    if( value == 0 )
        return 0;
    return MSB(value);
}

#endif  // ALIB_DOX

}} // namespace [aworx::lib]


#endif // HPP_ALIB_BITS

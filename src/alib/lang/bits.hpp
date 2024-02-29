/** ************************************************************************************************
 * \file
 * This header file is part of the \aliblong. It does not belong to an \alibmod and is
 * included in any \alibdist.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_BITS
#define HPP_ALIB_LANG_BITS 1

#if !defined(HPP_ALIB_LANG_INTEGERS)
#   include "alib/lang/integers.hpp"
#endif

#if !defined (HPP_ALIB_LANG_TMP) && !defined(ALIB_DOX)
#   include "alib/lang/tmp.hpp"
#endif

#if defined(_WIN32) && !defined (__INTRIN_H_)
    #include <intrin.h>
#endif

#if !defined (_GLIBCXX_ITERATOR) && !defined (_ITERATOR_)
#   include <iterator>
#endif

namespace alib {

/** This namespace holds types and functions, which are very close to the C++ language itself.
 *  The availability of most of the entities found here, is \b not related to the inclusion of
 *  a certain \alibmod within the \alibdist chosen. Instead, the corresponding header files
 *  are \ref alib_manual_modules_common_files "always included" when compiling \alib.
 *
 * With the inclusion of \alib_basecamp in the \alibdist, type \alib{lang,Camp} is found, which is
 * used to organize the library in respect to resource management and bootstrapping.
 * Furthermore, this very special "Camp-Module" adds sub-namespaces
 * - \ref alib::lang::format,
 * - \ref alib::lang::resources and
 * - \ref alib::lang::system
 *
 * to this namespace, as well as a few other basic language-related types like
 * \alib{lang::Exception}.
 * 
 * # Reference Documentation #
 */
namespace lang {
    /** The C++ language defines the right-hand-side argument of bit shift operations to be of
     *  type <c>int</c>. To increase code readability we define this type explicitly.*/
    using   ShiftOpRHS = int;

#define bitsof(type) static_cast<int>(sizeof(type) * 8)

    /**
     * Like C++ keyword <c>sizeof</c> but returns the number of bits of the type of the given value.
     * The return type is <c>int</c> instead of <c>size_t</c>, which satisfies \alib code conventions.
     *
     * \note To improve code readability, namely to a) indicate that this is an inlined, constant
     *       expression and b) to indicate that this is just using keyword <c>sizeof</c>,
     *       as an exception from the rules, this function is spelled in lower case.
     *
     * \see Macro \ref bitsof "bitsof(type)", which works directly on the type.
     *
     * @param  val  The (sample) value to deduce the type from. Otherwise ignored.
     * @tparam T    The type to receive the size in bits from.
     * @return The size of \p{TIntegral} in bits.
     */
    template<typename T> inline constexpr int bitsofval(const T& val)
    {
        (void) val; return sizeof(T) * 8;
    }



#if defined(ALIB_DOX)
    /**
     * Inline namespace function that returns a mask with bits set to \c 1 up to the given
     * binary digit, and bits above to \c 0.
     * If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
     * all bits are set in the returned value.
     *
     * \see While this is the fully templated version, with
     *      #alib::lang::LowerMask<TIntegral>(ShiftOpRHS), a run-time version is given.
     *
     * @tparam TWidth    The number of lower bits to set to \c 1.
     * @tparam TIntegral The integral type to operate on.
     * @return The requested mask.
     */
    template<ShiftOpRHS TWidth, typename TIntegral> inline constexpr TIntegral LowerMask();
#else

    template<ShiftOpRHS TWidth, typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  &&  TWidth >= bitsof(TIntegral) )
    inline constexpr
    LowerMask()
    {
        return TIntegral(~TIntegral(0));
    }

    template<ShiftOpRHS TWidth, typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  &&  TWidth < bitsof(TIntegral) )
    inline constexpr
    LowerMask()
    {
        ALIB_WARNINGS_IGNORE_INTEGRAL_CONSTANT_OVERFLOW
        using TUnsigned= typename std::make_unsigned<TIntegral>::type;
        return TIntegral(~(TUnsigned(~TUnsigned(0)) << TWidth) );
        ALIB_WARNINGS_RESTORE
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Inline namespace function that returns a mask with bits set to \c 1 up to the given
     * binary digit, and bits above to \c 0.
     * Parameter \p width <b>must not be greater or equal</b> to the width of the given
     * \p TIntegral type. In debug compilations, an assertion is raised in that case.
     *
     * \see A fully templated version usable when the number of bits are known at compile time,
     *      is given with #alib::monomem::LowerMask<ShiftOpRHS,typename>().
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  width     The number of lower bits to set in the mask returned.
     * @return The requested mask.
     */
    template<typename TIntegral> inline constexpr TIntegral LowerMask( ShiftOpRHS width );
#else
    template<typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
    inline
    constexpr
    LowerMask( ShiftOpRHS width )
    {
        ALIB_ASSERT_ERROR( width < bitsof(TIntegral), "ALIB/BITS",
              "Requested mask width wider than integral: ", width )
        using TUnsigned= typename std::make_unsigned<TIntegral>::type;
        return TIntegral(~(TUnsigned(~TUnsigned(0))  << width  ));
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Inline namespace function that returns a mask with bits set to \c 0 up to the given
     * binary digit, and bits above to \c 1.
     * If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
     * all bits are set in the returned value.
     *
     * \see While this is the fully templated version, with
     *      #alib::lang::UpperMask<TIntegral>(ShiftOpRHS), a run-time version is given.
     *
     * @tparam TWidth    The number of lower bits to clear to \c 1.
     * @tparam TIntegral The integral type to operate on.
     * @return The requested mask.
     */
    template<ShiftOpRHS TWidth, typename TIntegral> inline constexpr TIntegral UpperMask();
#else

    template<ShiftOpRHS TWidth, typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  && TWidth >= bitsof(TIntegral))
    inline constexpr
    UpperMask()
    {
        return TIntegral(0);
    }
    template<ShiftOpRHS TWidth, typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  && TWidth < bitsof(TIntegral) )
    inline constexpr
    UpperMask()
    {
        using TUnsigned= typename std::make_unsigned<TIntegral>::type;
        return TIntegral((TUnsigned(~TUnsigned(0)) << TWidth));
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Inline namespace function that returns a mask with bits set to \c 0 up to the given
     * binary digit, and bits above to \c 1.
     * Parameter \p width <b>must not be greater or equal</b> to the width of the given
     * \p TIntegral type. In debug compilations, an assertion is raised in that case.
     *
     * \see A fully templated version usable when the number of bits are known at compile time,
     *      is given with #alib::monomem::UpperMask<ShiftOpRHS,typename>().
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  width     The number of lower bits to clear in the mask returned.
     * @return The requested mask.
     */
    template<typename TIntegral> inline constexpr TIntegral UpperMask( ShiftOpRHS width );
#else
    template<typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
    inline constexpr
    UpperMask( ShiftOpRHS width )
    {
        ALIB_ASSERT_ERROR( width < bitsof(TIntegral), "ALIB/BITS",
              "Requested mask width wider than integral: ", width )
        using TUnsigned= typename std::make_unsigned<TIntegral>::type;
        return TIntegral( (TUnsigned(~TUnsigned(0))  << width)  );
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Inline namespace function that keeps the given number of lower bits and masks the higher ones
     * out of the given integral value.
     * If parameter \p TWidth is greater or equal to the width of the given \p TIntegral type, then
     * all bits are returned.
     *
     * \see While this is the fully templated version and hence explicitly constexpression for the
     *      reader of a code, with
     *      #alib::lang::LowerBits<TIntegral>(lang::ShiftOpRHS,TIntegral), a version is given.
     *
     * @param  value     The value to mask.
     * @tparam TWidth    The number of lower bits to keep.
     * @tparam TIntegral The integral type to operate on (deduced by the compiler).
     * @return The given value with the upper remaining bits cleared.
     */
    template<ShiftOpRHS TWidth, typename TIntegral> inline constexpr TIntegral LowerBits( TIntegral value );
#else
    template<ShiftOpRHS TWidth, typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
    inline constexpr
    LowerBits( TIntegral value )
    {
        if constexpr ( TWidth >= bitsof(TIntegral) )
            return value;
        return value & LowerMask<TWidth,TIntegral>( );
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Inline namespace function that keeps the given number of lower bits and masks the higher ones
     * out of the given integral value.
     * Parameter \p width <b>must not be greater or equal</b> to the width of the given
     * \p TIntegral type. In debug compilations, an assertion is raised.
     *
     * \see A fully templated version usable when the number of bits are known at compile time,
     *      is given with #alib::LowerBits<ShiftOpRHS,typename>().
     *
     * @param  width     The number of lower bits to keep.
     * @param  value     The value to mask.
     * @tparam TIntegral The integral type to operate on (deduced by the compiler).
     * @return The given value with the upper remaining bits cleared.
     */
    template<typename TIntegral> inline constexpr TIntegral LowerBits( ShiftOpRHS width, TIntegral value );
#else

    template<typename TIntegral>
    ATMP_T_IF(TIntegral,    std::is_integral<TIntegral>::value  )
    inline constexpr
    LowerBits( ShiftOpRHS width, TIntegral value )
    {
        return value & LowerMask<TIntegral>( width );
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Returns the logarithm base to for the size in bits of the template given integral type.<p>
     * Precisely, this function returns:
     * - 3 for  8-bit types,
     * - 4 for 16-bit types,
     * - 5 for 32-bit types,
     * - 6 for 64-bit types and
     * - 7 for 128-bit types.
     * @tparam TIntegral The integral type to count bits in.
     * @return The number of bits needed to count the bits of type \p TIntegral.
     */
    template<typename TIntegral> inline constexpr int Log2OfSize();
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    inline constexpr
    Log2OfSize()
    {
        static_assert( bitsof(TIntegral) <= 128, "Integrals larger than 128 not supported.");
        if constexpr (bitsof(TIntegral) == 32) return 5;
        if constexpr (bitsof(TIntegral) == 64) return 7;
        if constexpr (bitsof(TIntegral) ==  8) return 3;
        if constexpr (bitsof(TIntegral) == 16) return 4;
        return 8;
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Returns the number of bits set in an integral value.
     * Internally, this method uses:
     * - On gcc/clang: builtin method \c __builtin_popcount (and variations) which translate to a
     *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
     * - With MS-compilers, intrinsic \c popcount is used.
     * - On other platforms, the number is evaluated programatically.
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test.
     * @return The number of bits set in a value.
     */
    template<typename TIntegral> constexpr int BitCount( TIntegral value );
#else

    // a magical popcount function stolen from the internet
    namespace {  inline int countBitsFast(uint32_t n)
    {
        n = (n & 0x55555555u) + ((n >> 1) & 0x55555555u);
        n = (n & 0x33333333u) + ((n >> 2) & 0x33333333u);
        n = (n & 0x0f0f0f0fu) + ((n >> 4) & 0x0f0f0f0fu);
        n = (n & 0x00ff00ffu) + ((n >> 8) & 0x00ff00ffu);
        n = (n & 0x0000ffffu) + ((n >>16) & 0x0000ffffu);
        return int(n);
    }} // anonymous namespace


    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    ALIB_FORCE_INLINE
    constexpr
    BitCount( TIntegral value )
    {
#if defined(_WIN64)
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;
        if constexpr (bitsof(TIntegral) <= 32 )
            return __popcnt (static_cast<TUnsigned>( value ));
        return __popcnt64 (static_cast<TUnsigned>( value ));
#elif defined(__has_builtin) && __has_builtin( __builtin_popcount )
        using TUnsigned = typename std::make_unsigned<TIntegral>::type;

        if constexpr (sizeof(TIntegral) == sizeof(long long)) return int(__builtin_popcountll( TUnsigned(value)                      ));
        if constexpr (sizeof(TIntegral) == sizeof(long     )) return int(__builtin_popcountl ( TUnsigned(value)                      ));
        if constexpr (sizeof(TIntegral) == sizeof(int      )) return int(__builtin_popcount  ( TUnsigned(value)                      ));
        return int(__builtin_popcountll( static_cast<unsigned long long>(value)));
#else
        if constexpr (bitsof(TIntegral) <= 32)
            return countBitsFast(uint32_t(value));
        return     countBitsFast(uint32_t(value) & 0xFFFFFFFF)
                 + countBitsFast(uint32_t(value >> 32));
#endif
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Returns the number of the leading 0-bits in an integral type.
     * Internally, this method uses:
     * - On gcc/clang: builtin methods \c __builtin_clz (or variations) which translate to a
     *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
     * - With MS-compilers, intrinsics \c _BitScanReverse or \c _BitScanReverse64 is used.
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
    template<typename TIntegral> constexpr int CLZ( TIntegral value );
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    constexpr inline
    CLZ( TIntegral value )
    {
        ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
                  "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )

        #if defined(_WIN64)
        unsigned long bitScanResult;          
        if constexpr (bitsof(TIntegral) <= 32 )
        {
            _BitScanReverse( &bitScanResult, static_cast<unsigned long>(value) );
            return bitsof(TIntegral) - bitsof(unsigned long)  + 31 - bitScanResult;
        }
        else
        {
			_BitScanReverse64( &bitScanResult, uint64_t(value) );
			return bitsof(TIntegral) - bitsof(uint64_t) + 63 - bitScanResult;
        }
        #elif defined(__has_builtin) && __has_builtin( __builtin_clz   )    \
                   && __has_builtin( __builtin_clzl  )    \
                   && __has_builtin( __builtin_clzll )
        if constexpr (sizeof(TIntegral) == sizeof(int      )) return int(__builtin_clz  ( static_cast<unsigned int      >(value) ));
        if constexpr (sizeof(TIntegral) == sizeof(long     )) return int(__builtin_clzl ( static_cast<unsigned long     >(value) ));
        if constexpr (sizeof(TIntegral) == sizeof(long long)) return int(__builtin_clzll( static_cast<unsigned long long>(value) ));
        return  bitsof(TIntegral)  -  bitsof(int)       +     int(__builtin_clz  ( static_cast<unsigned int      >(value) ));
        #else
        // Do it manually using binary search:
        ShiftOpRHS msb= bitsof(TIntegral) -1;
        ShiftOpRHS l= 0;
        ShiftOpRHS r= bitsof(TIntegral);
        while( l <= r  )
        {
            ShiftOpRHS mid= (l + r) / 2;
            if( (TIntegral(1) << mid) > value )
            {
                msb= mid -1;
                r= mid - 1;
            }
            else
                l= mid + 1;
        }
        return bitsof(TIntegral) -1  - msb;
        #endif
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Variant of \ref CLZ which tests given parameter \p value on \c 0 and returns
     * <c>sizeof(TIntegral) * 8</c> in this case.
     * Otherwise returns the result of \ref CLZ.
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test. May be \c 0, which results to the number of bits in
     *                   \p TIntegral.
     * @return The number of leading zero-bits in \p value.
     */
    template<typename TIntegral> constexpr int CLZ0( TIntegral value);
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    constexpr inline
    CLZ0( TIntegral value)
    {
        if( value == 0 )
            return bitsof(TIntegral);
        return CLZ(value);
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Returns the number of the trailing 0-bits in an integral type.
     * Internally, this method uses:
     * - On gcc/clang: builtin methods \c __builtin_ctz (and variations) which translate to a
     *   single assembly instruction, and a constexpression in case \p value is known at compile-time.
     * - With MS-compilers, intrinsics \c _BitScanForward or \c _BitScanForward64 is used.
     * - On other platforms, a binary search is performed.
     *
     * \attention
     *  This function must not be called with a \p value of <c>0</c>!
     *  In debug-compilations, this method raises an \alib assertion in this case, while in
     *  release-compilations, the result is <em>'undefined'</em>.
     *  With function \ref CTZ0, an alternative is given which returns \c 0 if the input is \c 0.
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test. Must not be \c 0.
     * @return The lowest bit set in \p value.
     */
    template<typename TIntegral> constexpr  int CTZ( TIntegral value );
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    ALIB_FORCE_INLINE
    CTZ( TIntegral value )
    {
        ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
                  "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )

        #if defined(_WIN64)
        unsigned long bitScanResult;
        if constexpr (bitsof(TIntegral) <= 32 )
        {
            _BitScanForward( &bitScanResult, static_cast<unsigned long>(value) );
            return bitScanResult;
        }
        _BitScanForward64( &bitScanResult, uint64_t(value) );
        return bitScanResult;

#elif defined(__has_builtin) && __has_builtin( __builtin_ctz   )    \
		   && __has_builtin( __builtin_ctzl  )    \
		   && __has_builtin( __builtin_ctzll )
        if constexpr (sizeof(TIntegral) == sizeof(int      )) return int(__builtin_ctz  ( static_cast<unsigned int      >(value) ));
        if constexpr (sizeof(TIntegral) == sizeof(long     )) return int(__builtin_ctzl ( static_cast<unsigned long     >(value) ));
        if constexpr (sizeof(TIntegral) == sizeof(long long)) return int(__builtin_ctzll( static_cast<unsigned long long>(value) ));
        return int(__builtin_ctz  ( static_cast<unsigned int      >(value) ));
#else
        // Do it manually using binary search:
        int result= 0;
        int width=  bitsof(TIntegral) / 2;
        TIntegral mask= LowerMask<TIntegral>(width);
        while( width )
        {
            if( (value & mask) == 0 )
            {
                result+= width;
                value>>= width;
            }
            width/=2;
            mask>>= width;
        }
        return result;
#endif
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Variant of \ref CTZ which tests given parameter \p value on \c 0 and returns
     * <c>sizeof(TIntegral) * 8</c> in this case.
     * Otherwise returns the result of \ref CLZ.
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test. May be \c 0, which results to the number of bits in
     *                   \p TIntegral.
     * @return The number of trailing zero-bits in \p value. In case the given value is \c 0,
     *         <c>sizeof(Tintegral) * 8</c> is returned.
     *
     */
    template<typename TIntegral> constexpr int CTZ0( TIntegral value);
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    constexpr inline
    CTZ0( TIntegral value)
    {
        if( value == 0 )
            return bitsof(TIntegral);
        return CTZ(value);
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Returns the number of the most significant bit in an integral type.
     * Internally, this method uses \alib{lang,CLZ} and returns
     *
     *      int(sizeof(TIntegral)) * 8 - CLZ(value)
     *
     * \attention
     *  This function must not be called with a \p value of <c>0</c>!
     *  In debug-compilations, this method raises an \alib assertion in this case, while in
     *  release-compilations, the result is <em>'undefined'</em>.
     *  With function \ref MSB0, an alternative is given which returns \c 0 if the input is \c 0.
     *
     *  \note A corresponding function "LSB", to receive the least significant bit is not given.
     *        Instead, use <c>CTZ() + 1</c>.
     *
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test. Must not be \c 0.
     * @return The highest bit set in \p value. The numbering starts with \c 1 and ends with
     *         <c>sizeof(Tintegral) * 8</c>.
     */
    template<typename TIntegral> constexpr int MSB( TIntegral value);
#else
    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    constexpr inline
    MSB( TIntegral value)
    {
        ALIB_ASSERT_ERROR( value != 0, "ALIB/BITS",
                           "Illegal value 0 passed to MSB(). Use MSB0() if 0 values need to be handled." )
        return  bitsof(TIntegral) - CLZ(value);
    }
#endif

#if defined(ALIB_DOX)
    /**
     * Variant of \ref MSB which tests given parameter \p value on \c 0 and returns \c 0 in this
     * case. Otherwise returns the result of \ref MSB.
     *
     * @tparam TIntegral The integral type to operate on.
     * @param  value     The value to test. May be \c 0, which results to \c 0.
     * @return The highest bit set in \p value. The numbering starts with \c 1 and ends with
     *         <c>sizeof(Tintegral)* 8</c>. If \p{value} is \c 0, hence no bit is set,
     *         \c 0 is returned.
     */
    template<typename TIntegral>    constexpr  int MSB0( TIntegral value);
#else

    template<typename TIntegral>
    ATMP_T_IF(int, std::is_integral<TIntegral>::value )
    ALIB_FORCE_INLINE
    constexpr
    MSB0( TIntegral value)
    {
        if( value == 0 )
            return 0;
        return MSB(value);
    }
#endif
}} // namespace [alib::lang]

#endif // HPP_ALIB_LANG_BITS

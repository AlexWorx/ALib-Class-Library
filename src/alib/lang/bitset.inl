//==================================================================================================
/// \file
/// This header-file is part of module \alib_lang of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lang {
#include "ALib.Lang.CIFunctions.H"

//==================================================================================================
/// This class is an improved replica of <c>std::bitset</c> and hence stores and exposes a set of
/// bits. The differences (advantages) of this class to the standard type are:
/// - The class provides an efficient bidirectional iterator, which uses intrinsics
///   (on supported platforms, via functions \alib{lang;CLZ} and \alib{lang;CTZ}) to search the
///   next bit set without looping over unset bits.
/// - The size of this type's instances is adjusted to the next fitting integral size, namely
///   <c>char</c>, <c>short</c>, <c>int</c>, <c>long</c> and <c>long long</c>.
///   If even larger bitsets are requested, the size becomes a multiple of the size of
///   <c>long long</c>.
/// - The type that denotes bit-indices, which is used in the interface methods, can optionally
///   be altered from <c>int</c> to a custom type which is statically castable to <c>int</c>, by
///   providing the defaulted template parameter \p{TInterface} accordingly. This avoids the need
///   for statically casting the index type with every invocation of an interface method.<br>
///   The main motivation for providing this was to allow to
///   \ref alib_enums_iter_bitset "address bits with enumeration elements".
/// - Instead of one template parameter defining the fixed size of the bitset, two parameters
///   \p{TBegin} and \p{TEnd} are provided. This allows defining bitsets of a range
///   of indices that do not start with <c>0</c> and in this case avoids the need for subtraction of
///   the (fixed) start value of a range with every invocation of an interface method.
/// - Methods #Set and #Reset allow to pass several bit indices at once using a comma-separated list.
/// - With methods <c>std::bitset<N>::to_ulong() / to_ullong()</c>, the standard type allows
///   the export of the internal value only to 64-bit values (on common platforms).
///   If the higher bits are to be exported, the bitset has to be shifted, which is inefficient
///   and destructive (a copy has to be taken).<br>
///   This implementation provides method #Export, which - dependent on the size of the bitset -
///   offers an additional index to address any higher word. Furthermore, for smaller bitsets
///   the export method returns a fitting smaller integral.
/// - Explicit uninitialized construction is possíble with
///   \alib{lang;TBitSet::TBitSet(const std::nullptr_t&)}, which
///   avoids code and run-time penalty, in cases where the bits are to be initialized at a later
///   stage.
///
/// In respect to performance, this type equals a typical implementation <c>std::bitset</c>.
///
/// @see
///  - Type definition \alib{enumops;EnumBitSet}, which defines a bitset over
///    \ref alib_enums_iter "ALib Iterable Enums" based on this type.<br>
///  - For a quick tutorial on the use of this type (in nice combination with C++ enumerations),
///    see chapter \ref alib_enums_iter_bitset "3.5 Using Class TBitSet with Iterable Enums" of the
///    Programmer's Manual of module \alib_enumops_nl.
///  - Type definition \ref alib::BitSet "BitSet" in namespace \ref alib, which defines a set with
///    interface-type <c>int</c>.
///  - Class \alib{bitbuffer;BitBuffer}, which implements a sort of stream buffer to store
///    bits in dynamic memory.
///
/// @tparam TInterface   The interface type to denote bit position values. This type has to be
///                      statically castable to and constructible from <c>int</c>.
/// @tparam TEnd         The last index of the interface type plus <c>1</c>.
/// @tparam TBegin       The first index of the interface type. (Defaults to <c>0</c>.)
//==================================================================================================
template< typename TInterface, TInterface TEnd, TInterface TBegin= 0>
class TBitSet
{
  protected:
    #if !DOXYGEN
    static_assert( !(TBegin > TEnd), "First Idx greater or equal than last index + 1" );
    #endif
    // forward declaration
    template<typename TBitSetCM, bool isReverse>   class TBidiIterator;

    // --------------------------------------   members   ------------------------------------------

  public:
    /// The number of bits in the range resulting from template parameters \p{TBegin}
    /// and \p{TEnd}.
    static constexpr int   Capacity=    int( TEnd )
                                      - int( TBegin );

    /// The type that is used to store the bits in.
    /// Template programming code chooses the smallest fitting integral.
    /// If #Capacity exceeds the type <c>long long</c>, an array of <c>long long</c>, field
    /// #QtyWords will become greater than \c 1.
    using TWord= std::conditional_t< (Capacity >  bitsof(long)    ),  unsigned long long,
                 std::conditional_t< (Capacity >  bitsof(int)     ),  unsigned long     ,
                 std::conditional_t< (Capacity >  bitsof(short)   ),  unsigned int      ,
                 std::conditional_t< (Capacity >  bitsof(char)    ),  unsigned short    ,
                                                                      unsigned char      >>>>;

    /// The size of the #TWord array containing the bits
    static constexpr int   QtyWords=      Capacity / bitsof(TWord)
                                     + ( (Capacity % bitsof(TWord))  != 0 ? 1 : 0  );

  protected:
    /// The array of integrals containing the bits needed. The unused upper bits in the last word
    /// are always kept <c>0</c>.
    TWord                  words[QtyWords ? QtyWords : 1];

    // --------------------------------------   helpers   ------------------------------------------
    /// Helper to determine the bit index in the actual word.
    /// @param b The absolute index.
    /// @return The word index.
    constexpr static int          wordIdx(TInterface b)                                     noexcept
    { return (int(b) - int(TBegin)) / bitsof(TWord);   }

    /// Helper to determine the bit index in the actual word.
    /// @param b The absolute index.
    /// @return The relative index.
    constexpr static int          bitIdx (TInterface b)                                     noexcept
    {
        ALIB_ASSERT_ERROR(    int(b) >= int(TBegin)
                           && int(b) <  int(TEnd),
            "ALIB/BITS", "Given bit index out of bounds: {} <= {} < {} ",
                         integer(TBegin), integer(b), integer(TEnd) )
        return (int(b) - int(TBegin)) % bitsof(TWord);
    }

    /// Helper receive the word from the array of words.
    /// @param b The bit to address index.
    /// @return A reference to the word.
    constexpr              TWord& word  (TInterface b)        noexcept { return words[wordIdx(b)]; }

    /// Helper receive a \c const reference word from the array of words.
    /// @param b The bit to address index.
    /// @return A reference to the word.
    constexpr        const TWord& word  (TInterface b)  const noexcept { return words[wordIdx(b)]; }

    /// Returns a bitmask with all used bits set.
    /// @param wIdx The index of the word in array #words.
    /// @return A bitmask with all bits set for indices smaller than #QtyWords <c>-1</c>,
    ///         and a mask that covers the remaining bits if \p{wIdx} targets the last word.
    constexpr static       TWord  mask  (int wIdx)                                          noexcept
    {
        return   wIdx < QtyWords- 1 || (Capacity % bitsof(TWord)) == 0
                 ? TWord(~TWord(0))
                 : LowerMask<Capacity % bitsof(TWord), TWord>();
    }

    /// Returns a bitmask with only the given bit set.
    /// @param b The index of the bit to set.
    /// @return A bitmask with one bit set.
    constexpr static       TWord  mask0010(TInterface b) noexcept { return TWord(TWord(1) << bitIdx(b)); }

    /// Returns a bitmask with only the given bit unset.
    /// @param b The index of the bit to unset.
    /// @return A bitmask with all but one bit set.
    constexpr static       TWord  mask1101(TInterface b) noexcept { return ~mask0010(b); }

  public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// A publicly accessible nested class, used as a proxy object to allow users to interact with
    /// individual bits of a bitset.
    /// The primary use of this type is to provide a \e lvalue that can be returned from operator[].
    /// @tparam TBitSetCM A constant or mutable \b TBitSet.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TBitSetCM>
    class Reference
    {
      protected:
        #if !DOXYGEN
        template<typename T, bool isReverse>       friend class TBidiIterator;
        #endif

        TInterface  bit;     ///< The bit number in #bitSet this reference addresses.
        TBitSetCM*  bitSet;  ///< The \b TBitSet this reference addresses.

      public:
        /// Constructor.
        /// \note In contrast to the constructor of <c>std::bitset::reference</c>,
        ///       this constructor is public to allow easy external construction.<br>
        /// @param set  The parent \b TBitSet.
        /// @param b    The bit to represent in the TBitSet.
        constexpr    Reference(TBitSetCM& set, TInterface b)                                noexcept
        : bit(b), bitSet(&set)                                                                    {}

        /// Defaulted copy constructor.
        constexpr    Reference(const Reference&)                                  noexcept= default;

        /// Destructor. (Is declared \c constexpr with C++23.)
        ALIB_CPP_23(constexpr)
        ~Reference()                                                                     noexcept {}

        /// Copy assignment operator. (Defaulted, as otherwise implicitly deleted due to
        /// user-provided destructor.)
        /// @param other The object to copy.
        /// @return A reference to \c this.
        Reference& operator= (const Reference& other)                            noexcept = default;

        /// Returns the represented bit number.
        /// @return The bit that this object refers to.
        constexpr TInterface Bit()                                                    const noexcept
        { return TInterface(bit);  }

        /// Returns the underlying bitset.
        /// @return The \b TBitSet that this object refers to.
        constexpr class TBitSet& TBitSet()                                                    noexcept
        { return bitSet;  }

        /// Returns the underlying bitset.
        /// @return The \b TBitSet that this object refers to.
        constexpr const class TBitSet& TBitSet()                                        const noexcept
        { return bitSet;  }

        /// Sets or resets the represented bit.
        /// @param val The value to assign to the represented bit.
        /// @return A reference to \c this.
        constexpr Reference& operator=(bool val)                                            noexcept
        { bitSet->Set(bit, val); return *this;  }

        /// Implicit conversion to bool.
        /// @return The value of the represented bit.
        constexpr operator bool()                                                           noexcept
        {  return bitSet->Test(bit); }

        /// Does not manipulate the bit, but returns its negated value.
        /// @return The negated value of the represented bit.
        constexpr bool operator~()                                                          noexcept
        { return !bitSet->Test(bit);  }

        /// Flips the represented bit.
        /// @return A reference to \c this.
        constexpr Reference& Flip()                                                         noexcept
        { bitSet->Flip(bit); return *this; }

        /// Compares this object to another.
        /// @param rhs The right hand side of the comparison.
        /// @return \c true if the objects reference the same bit in the same bitset, \c false
        ///         otherwise
        constexpr bool operator== (const Reference& rhs)                   const noexcept = default;
    }; // inner class Reference

    // ------------------------------------   constructors   ---------------------------------------
    /// Default constructor initializing all bits to not set.
    constexpr TBitSet()                                                                     noexcept
    {
        Reset();
    }

    // ------------------------------------   constructors   ---------------------------------------
    /// Constructor taking a tag-type. This constructor omits any value initialization and
    /// <b>has to be called by providing <c>nullptr</c></b> as the argument.
    constexpr TBitSet(const std::nullptr_t&)                                             noexcept {}

    /// Constructor which takes an external #TWord which initializes the first word. If \c constexpr
    /// field #QtyWords is greater than \c 1, the other bits are set to \c 0 and can be set using
    /// method #Import or of course using other interface methods.
    /// @param preset The preset value for the bits.
    constexpr TBitSet( TWord preset )                                                       noexcept
    {
        words[0]= preset & mask(0);
        for (int w = 1; w < QtyWords; ++w)
            words[w]= 0;
    }

    /// Constructor which takes an external #TWord which initializes the first word. If \c constexpr
    /// field #QtyWords is greater than \c 1, the other bits are set to \c 0 and can be set using
    /// method #Import or of course using other interface methods.
    /// @param preset The preset value for the bits.
    constexpr TBitSet( bool preset )                                                       noexcept
    {
        for (int w = 0; w < QtyWords; ++w)
            words[w]= preset ? TWord(~(TWord(0))) & mask(w)
                             : TWord(0);
    }


    // -----------------------------------   set / reset   -----------------------------------------
    /// Sets the \p{bit} to <c>1</c>.
    /// @param bit  The index of the bit to set.
    /// @param val  \c true or \c false to set or clear bit \p{bit}.
    /// @return A reference to \c this object.
    template <typename... T>
    constexpr
    TBitSet& Set( TInterface bit, bool val )                                                noexcept
    {
        if( val )  Set(bit);
        else       Reset(bit);
        return *this;
    }

    /// Sets one or more bits to <c>1</c>.
    /// @param firstBit     The index of the first bit to set.
    /// @param furtherBits  An optional continued list of bit-indices.
    /// @return A reference to \c this object.
    template <typename... T>
    constexpr
    TBitSet& Set( TInterface firstBit, T&&... furtherBits )                                             noexcept
    {
        word(firstBit)|= mask0010(firstBit);
        if constexpr (sizeof...(furtherBits) > 0)
            Set(furtherBits...);
        return *this;
    }

    /// Sets one or more bits to <c>0</c>.
    /// @param firstBit  The index of the first bit to clear.
    /// @param furtherBits      An optional continued list of bit-indices.
    /// @return A reference to \c this object.
    template <typename... T>
    constexpr
    TBitSet& Reset( TInterface firstBit, T&&... furtherBits )                                           noexcept
    {
        word(firstBit)&=  mask1101(firstBit);
        if constexpr (sizeof...(furtherBits) > 0)
            Reset(furtherBits...);
        return *this;
    }

    /// Flips one or more bits from <c>0</c> to <c>1</c> and vice versa.
    /// @param firstBit  The index of the bit to flip.
    /// @param furtherBits An optional continued list of indices of bits to flip.
    /// @return A reference to \c this object.
    template <typename... T>
    constexpr
    TBitSet& Flip( TInterface firstBit, T&&... furtherBits )                                            noexcept
    {
        word(firstBit)^=  mask0010(firstBit);
        if constexpr (sizeof...(furtherBits) > 0)
            Flip(furtherBits...);
        return *this;
    }


    /// Sets all bits to <c>1</c>. @return A reference to this object.
    constexpr TBitSet& Set()    noexcept { for (int w = 0; w < QtyWords; ++w) words[w]=  mask(w); return *this; }
    /// Sets all bits to <c>0</c>. @return A reference to this object.
    constexpr TBitSet& Reset()  noexcept { for (int w = 0; w < QtyWords; ++w) words[w]=  0;       return *this; }
    /// Flips all bits from <c>0</c> to <c>1</c> and vice versa. @return A reference to this object.
    constexpr TBitSet& Flip()   noexcept { for (int w = 0; w < QtyWords; ++w) words[w]^= mask(w); return *this; }

    // --------------------------------------   test   ---------------------------------------------
    /// Returns \c true if the \p{bit} is set, \c false otherwise.
    /// @param bit Denotes the bit to test.
    /// @return The state of the \p{bit}.
    constexpr
    bool Test( TInterface bit )                                                             noexcept
    {
        return 0 != (word(bit) & mask0010(bit)) ;
    }

    // ----------------------------------   count/all/any/none   -----------------------------------
    /// Returns the number of bits set to \c true in this set.
    /// @return The number of bits counted.
    constexpr
    int Count()                                                                       const noexcept
    {
        int result= 0;
        for (int w = 0; w < QtyWords; ++w)
            result+= BitCount(words[w]);
        return result;
    }

    /// Tests if any bit is set.
    /// @return \c true if all bits are set, \c false otherwise.
    constexpr
    bool All()                                                                        const noexcept
    {
        for (int w = 0; w < QtyWords; ++w)
            if(words[w] != mask(w) )
                return false;
        return true;
    }

    /// Tests if any bit is set.
    /// @return \c true if at least one bit is set, \c false otherwise.
    constexpr
    bool Any()                                                                        const noexcept
    {
        for (int w = 0; w < QtyWords; ++w)
            if(words[w] != 0 )
                return true;
        return false;
    }

    /// Tests if not any bit is set.
    /// @return \c false if at least one bit is set, \c true otherwise.
    constexpr
    bool None()                                                                       const noexcept
    {
        return !Any();
    }

    // ---------------------------------   import/export   -----------------------------------------
#if DOXYGEN
    /// Exports the internal integral(s) holding the bits.
    ///
    /// \attention
    ///   Other than this documentation suggests, parameter \p{wordIdx} is only available if
    ///   the bitset size (defined by template parameters \p{TBegin} and \p{TEnd})
    ///   exceeds the number of bits in the widest possible #TWord-type, hence the size
    ///   of <c>sizeof(long long) * 8</c>.  In that case, field #QtyWords provides the number of
    ///   words that are exportable.<br>
    ///   This maximum word size is 64-bit on common platforms. Here, \c 0 adresses bits 0..63,
    ///   \c 1 bits 64..127 and so forth.
    ///
    /// @param wordIdx The subset of bits to receive. (See note above!)
    /// @return A reference to the inner word.
    constexpr
    TWord& Export(int wordIdx)                                                              noexcept
    {}

    /// Const version of #Export.
    /// @param wordIdx The subset of bits to receive.<br>
    ///                \b ATTENTION: See non-constant variant #Export on this parameter!
    /// @return A reference to the inner word.
    constexpr
    const TWord& Export(int wordIdx)                                                 const  noexcept
    {}
#else
    template<bool TRequires=QtyWords==1> requires TRequires       constexpr TWord&   Export()                  noexcept { return words[      0]; }
    template<bool TRequires=QtyWords==1> requires TRequires const constexpr TWord    Export()            const noexcept { return words[      0]; }
    template<bool TRequires=QtyWords!=1> requires TRequires       constexpr TWord&   Export(int wordIdx)       noexcept {
        ALIB_ASSERT_ERROR( wordIdx >= 0 && wordIdx < QtyWords,
                           "ALIB/BITS", "Index out of bounds:  0 <= {} < {} ", wordIdx, QtyWords )                        return words[wordIdx]; }
    template<bool TRequires=QtyWords!=1> requires TRequires const constexpr TWord    Export(int wordIdx) const noexcept {
        ALIB_ASSERT_ERROR( wordIdx >= 0 && wordIdx < QtyWords, "ALIB/BITS", "Index out of bounds."
                           "ALIB/BITS", "Index out of bounds:  0 <= {} < {} ", wordIdx, QtyWords )                        return words[wordIdx]; }
#endif

#if DOXYGEN
    /// Imports the data from (a) given integral(s).
    ///
    /// \attention
    ///   Other than this documentation suggests, parameter \p{wordIdx} is only available if
    ///   the bitset size (defined by template parameters \p{TBegin} and \p{TEnd})
    ///   exceeds the number of bits in the widest possible #TWord-type, hence the size
    ///   of <c>sizeof(long long) * 8</c>.  In that case, field #QtyWords provides the number of
    ///   words that are importable.<br>
    ///   This maximum word size is 64-bit on common platforms. Here, \c 0 address bits 0..63,
    ///   \c 1 bits 64..127 and so forth.
    ///
    /// @param val     The value to import.
    /// @param wordIdx The subset of bits to overwrite. (See note above!)
    constexpr
    void Import(TWord val, int wordIdx)                                                     noexcept
    {}
#else
    template<bool TRequires=QtyWords==1>
    requires TRequires
    void Import(TWord val )                                    noexcept { words[0]= val & mask(0); }

    template<bool TRequires=QtyWords!=1>
    requires TRequires
    void Import(TWord val, int wordIdx)                                                     noexcept
    {
        ALIB_ASSERT_ERROR( wordIdx >= 0 && wordIdx < QtyWords, "ALIB/BITS",
                           "Index out of bounds:  0 <= {} < {} ", wordIdx, QtyWords )
        words[wordIdx]= val &= mask(wordIdx);
    }
#endif

    // ----------------------------------    operators    ------------------------------------------
    /// Compares this bitset with another bitset of equal size.
    /// @param rhs The other \b TBitSet to compare this with.
    /// @return \c true if all bits are equal, \c false otherwise.
    constexpr bool operator==( const TBitSet& rhs ) const noexcept
    {
        for (int w = 0; w < QtyWords; ++w)
            if(words[w] != rhs.words[w] )
                return false;
        return true;
    }

    /// Sets the bits to the result of binary AND on corresponding pairs of bits of \c this and other.
    /// @param rhs The other \b TBitSet to compare this with.
    /// @return A reference to \c this object.
    constexpr TBitSet& operator&=(const TBitSet& rhs) noexcept { for(int w=0;w<QtyWords;++w) words[w]&= rhs.words[w]; return *this; }

    /// Sets the bits to the result of binary OR on corresponding pairs of bits of \c this and other.
    /// @param rhs The other \b TBitSet to compare this with.
    /// @return A reference to \c this object.
    constexpr TBitSet& operator|=(const TBitSet& rhs) noexcept { for(int w=0;w<QtyWords;++w) words[w]|= rhs.words[w]; return *this; }

    /// Sets the bits to the result of binary XOR on corresponding pairs of bits of \c this and other.
    /// @param rhs The other \b TBitSet to compare this with.
    /// @return A reference to \c this object.
    constexpr TBitSet& operator^=(const TBitSet& rhs) noexcept { for(int w=0;w<QtyWords;++w) words[w]^= rhs.words[w]; return *this; }

    /// Returns a temporary copy of \c this with all bits flipped (binary NOT)..
    /// @return A copy of this with flipped bits.
    constexpr TBitSet operator~() const noexcept
    {
        auto result= *this;
        for(int w= 0; w<QtyWords ; ++w)
            result.words[w]= ~words[w] & mask(w);
        return result;
    }

  protected:
    /// Stores a shifted result in \p{target}. This protected method is accessible via
    /// <c>operator<<()</c> (which creates and returns a temporary target) and <c>operator<<=()</c>,
    /// which passes \c this to parameter \p{target}.
    /// @param cnt      The number of positions to shift.
    /// @param target   The target object.
    constexpr void shiftLeft(int cnt, TBitSet& target)                                 const noexcept
    {
        ALIB_ASSERT_ERROR( cnt >= 0, "ALIB/BITS",
            "Negative value {} for TBitSet shift operation given.", cnt )
        // shift out of bounds?
        if( cnt >= Capacity)
        {
            target.Reset();
            return;
        }

        int offW= cnt / bitsof(TWord);
        int offB= cnt % bitsof(TWord);
        // no bit hassle
        if( offB == 0)
            for( int w= QtyWords -1; w >= 0; --w )
                target.words[w]= w - offW >= 0 ? words[w - offW]
                                               : TWord(0);
        // with bit shifts
        else
            for( int w= QtyWords -1; w >= 0; --w )
                target.words[w]= w >= offW  ? (    TWord(words[w - offW] << offB)
                                                 | ( w > offW ? words[w-offW-1] >> (bitsof(TWord) - offB)
                                                              : TWord(0) ) )
                                            : TWord(0);
        target.words[QtyWords - 1]&= LowerMask<Capacity % bitsof(TWord), TWord>();
    }

    /// Stores a shifted result in \p{target}. This protected method is accessible via
    /// <c>operator>>()</c> (which creates and returns a temporary target) and <c>operator>>=()</c>,
    /// which passes \c this to parameter \p{target}.
    /// @param cnt      The number of positions to shift.
    /// @param target   The target object.
    constexpr void shiftRight(int cnt, TBitSet& target)                                 const noexcept
    {
        ALIB_ASSERT_ERROR( cnt >= 0, "ALIB/BITS",
            "Negative value {} for TBitSet shift operation given.", cnt )
        // shift out of bounds?
        if( cnt >= Capacity)
        {
            target.Reset();
            return;
        }

        int  offW= cnt / bitsof(TWord);
        int  offB= cnt % bitsof(TWord);
        // no bit hassle
        if( offB == 0)
            for( int  w= 0; w < QtyWords; ++w )
                target.words[w]= w + offW < QtyWords ? words[w + offW]
                                                     : TWord(0);
        // with bit shifts
        else
            for( int w= 0; w < QtyWords; ++w )
                target.words[w]= w < QtyWords- offW ? (    TWord(words[w + offW] >> offB)
                                                        | ( w < QtyWords- offW - 1 ?  TWord(words[w+offW+1] << (bitsof(TWord) - offB))
                                                                                   :  TWord(0) ) )
                                                    : TWord(0);
        target.words[QtyWords - 1]&= LowerMask<Capacity % bitsof(TWord), TWord>();
    }

  public:
    /// Returns a temporary copy of this with a shift to the left (towards higher index positions).
    /// @param cnt The number of positions to shift.
    /// @return A copy of this with shifted bits.
    constexpr TBitSet operator<<(int cnt)                                              const noexcept
    { TBitSet result= TBitSet(nullptr); shiftLeft(cnt, result); return result; }

    /// Shifts the bits of this object to the left left (towards higher index positions).
    /// @param cnt The number of positions to shift.
    /// @return A reference to this object.
    constexpr TBitSet& operator<<=(int cnt)                                                  noexcept
    { shiftLeft(cnt, *this);  return *this; }

    /// Returns a temporary copy of this with a shift to the right (towards lower index positions).
    /// @param cnt The number of positions to shift.
    /// @return A copy of this with shifted bits.
    constexpr TBitSet operator>>(int cnt)                                              const noexcept
    { TBitSet result= TBitSet(nullptr); shiftRight(cnt, result); return result; }

    /// Shifts the bits of this object to the left right (towards lower index positions).
    /// @param cnt The number of positions to shift.
    /// @return A reference to this object.
    constexpr TBitSet& operator>>=(int cnt)                                                  noexcept
    { shiftRight(cnt, *this);  return *this; }

    /// Returns a reference to a specific bit.
    /// @param bit The bit to create a reference for.
    /// @return A reference to the bit in this object.
    constexpr
    Reference<TBitSet> operator[](TInterface bit)                                            noexcept
    { return Reference<TBitSet>(*this, bit); }

    /// Returns a \c const reference to a specific bit.
    /// @param bit The bit to create a reference for.
    /// @return A reference to the bit in this object.
    constexpr
    const Reference<const TBitSet> operator[](int bit)                                 const noexcept
    { return Reference<const TBitSet>(*this, bit); }

  protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// Implementation of \c std::iterator_traits for class \b %TBitSet.
    /// As the name of the class indicates, this iterator satisfies the C++ standard library
    /// concept of <em>bidirectional iterators</em>. The template parameters allow this
    /// type to act as a <c>const</c> or mutable as well as a forward or backward iterator.<br>
    /// The public available type definitions which specify the template parameters and are
    /// returned from the various \c begin() and \c end() variants of the outer class are:
    /// - \alib{lang::TBitSet;Iterator},
    /// - \alib{lang::TBitSet;ReverseIterator},
    /// - \alib{lang::TBitSet;ConstIterator}, and
    /// - \alib{lang::TBitSet;ConstReverseIterator}.
    ///
    /// @tparam TBitSetCM A constant or mutable TBitSet.
    /// @tparam isReverse If set, this iterator is a swaps ++ and -- operators.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename TBitSetCM, bool isReverse>
    class TBidiIterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;  ///< Implementation of <c>std::iterator_traits</c>.
        using value_type        = TInterface;                       ///< Implementation of <c>std::iterator_traits</c>.
        using difference_type   = int;                              ///< Implementation of <c>std::iterator_traits</c>.
        using pointer           = void;                             ///< Implementation of <c>std::iterator_traits</c>.
        using reference         = Reference<TBitSetCM>&;            ///< Implementation of <c>std::iterator_traits</c>.

      protected:
        /// The bit this iterator currently addresses. This is true for forward and reverse
        /// iterators. (Because this is not using the <c>std::reverse_iterator<</c> which would
        /// implement an reverse iterator that points 'before' the current element.
        Reference<TBitSetCM> ref;

        /// Searches and moves this iterator to the next higher bit is. In case no further bit is
        /// found, the iterator will points to illegal bit number \p{TBitSet::Capacity}
        /// @return A reference to this object.
        TBidiIterator& up()
        {
            // next bit
            ref.bit= TInterface(int(ref.bit) + 1);

            // search next bit from here
            while(int(ref.bit) < int(TEnd))
            {
                int     bIdx= bitIdx(ref.bit);
                TWord   word=   ref.bitSet->word(ref.bit)
                              & UpperMask<TWord>(bIdx);
                // search
                int trailingZeros;
                if(     word == 0
                    ||  ((trailingZeros= CTZ<TWord>( word )) == bitsof(TWord) )   )
                {
                    // not found in this word
                    ref.bit= TInterface( int(ref.bit) + bitsof(TWord) - bIdx );
                    if( int(ref.bit) > int(TEnd))
                        ref.bit= TInterface(TEnd);
                    continue;
                }

                // found one
                ref.bit= TInterface( int(ref.bit) + trailingZeros - bIdx);
                break;
            }
            return *this;
        }

        /// Searches and moves this iterator to the next lower bit is. In case no lower bit is
        /// found, the iterator will points to illegal bit number <c>-1</c>.
        /// @return A reference to this object.
        TBidiIterator& down()
        {
            // next bit
            ref.bit= TInterface(int(ref.bit) - 1);

            // search next bit from here
            while(int(ref.bit) >= int(TBegin))
            {
                int     bIdx= bitIdx(ref.bit);
                TWord   word=   ref.bitSet->word(ref.bit)
                              & TWord( TWord (~TWord(0))  >> (bitsof(TWord)- bIdx -1)  );
                              // a lower mask, but not precisely LowerMask.
                              //...and yes, we need all these castings :-/
                // search
                int leadingZeros;
                if(     word == 0
                    ||  ((leadingZeros= CLZ<TWord>( word )) == bitsof(TWord) )   )
                {
                    // not found in this word
                    ref.bit= TInterface(int(ref.bit) - bIdx -1);
                    if( int(ref.bit) < int(TBegin) -1)
                        ref.bit= TInterface( int(TBegin) - 1);
                    continue;
                }

                // found one
                ref.bit=  TInterface( int(ref.bit)
                          + (bitsof(TWord) - (bIdx + 1) )       // first correct to upper bit position
                          - leadingZeros                   );   // then subtract leading zeros
                break;
            }
            return *this;
        }

    public:
        /// Constructor.
        /// @param bitSet   The bit set to use.
        /// @param startBit The bit number to start with.
        explicit TBidiIterator( TBitSetCM& bitSet, TInterface startBit )
        : ref(bitSet, startBit)
        {}

    //######################   To satisfy concept of  InputIterator   ######################
        /// Prefix increment operator.
        /// @return A reference to this object.
        TBidiIterator& operator++()
        {
            if constexpr (!isReverse)   return up();
                                        return down();
        }

        /// Postfix increment operator.
        /// @return An iterator value that is not increased, yet.
        TBidiIterator operator++(int)
        {
            auto  result= *this;
            ++*this;
            return result;
        }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and the given iterator are pointing to the same bit in the same
        ///         set, \c false otherwise.
        bool operator==(const TBidiIterator& other)                                            const
        {
            return  ref == other.ref;
        }

        /// Comparison operator.
        /// @param other  The iterator to compare ourselves to.
        /// @return \c true if this and given iterator are not equal, \c false otherwise.
        bool operator!=(const TBidiIterator& other)                                            const
        {
            return !(*this == other);
        }

        /// Retrieves a reference to the internal \alib{lang::TBitSet;Reference} member that this
        /// iterator uses.
        /// @return The reference to the bit this iterator is currently addressing.
        reference operator*()
        {
            return ref;
        }

    //##################   To satisfy concept of  BidirectionalIterator   ##################
        /// Prefix decrement operator.
        /// @return A reference to this object.
        TBidiIterator& operator--()
        {
            if constexpr (isReverse)    return up();
                                        return down();
        }

        /// Postfix decrement operator.
        /// @return The iterator value prior the decrement operation.
        TBidiIterator operator--(int)
        {
            auto  result= *this;
            --*this;
            return result;
        }

    //#### Comparison operators  ###
        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e smaller than \p{other},
        ///         \c false otherwise.
        bool operator<(TBidiIterator other)                                                    const
        { return ref.bit < other.ref.bit;   }

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e smaller than or equal to \p{other},
        ///         \c false otherwise.
        bool operator<=(TBidiIterator other)                                                   const
        { return ref.bit <= other.ref.bit;  }

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e greater than \p{other},
        ///         \c false otherwise.
        bool operator>(TBidiIterator other)                                                    const
        { return ref.bit > other.ref.bit;   }

        /// Compares this iterator with the given one.
        /// @param other  The iterator to compare
        /// @return \c true if this iterator is \e greater than or equal to \p{other},
        ///         \c false otherwise.
        bool operator>=(TBidiIterator other)                                                   const
        { return ref.bit >= other.ref.bit;  }
    }; // class TBidiIterator

  public:
    /// Iterator type, implementing the standard library concept of
    /// \https{RandomAccessIterator,en.cppreference.com/w/cpp/concept/RandomAccessIterator}.
    using Iterator              = TBidiIterator<TBitSet, false>;

    /// Same as #Iterator, but working from the end to the start of the \b TBitSet.
    using ReverseIterator       = TBidiIterator<TBitSet, true>;

    /// The constant iterator type.
    using ConstIterator         = TBidiIterator<const TBitSet, false>;

    /// Same as #ConstIterator, but working from the end to the start of the \b TBitSet.
    using ConstReverseIterator  = TBidiIterator<const TBitSet, true>;

    /// Returns an iterator pointing to the first (lowest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The start iterator. In case no bit is set, the same as #end.
    Iterator                begin(int skip= 0)       { return ++Iterator(*this, TInterface(TBegin - 1) + skip); }

    /// Returns an iterator pointing to the non-existing bit behind the highest one.
    /// @return The end iterator.
    Iterator                end()                    { return Iterator(*this, TInterface(TEnd)   ); }

    /// Returns a reverse iterator pointing to the last (highest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The reverse iteration start.
    ReverseIterator         rbegin(int skip= 0)       { return ++ReverseIterator(*this, TInterface(TEnd) - skip   ); }

    /// Returns an iterator pointing to the non-existing bit before the lowest one.
    /// @return The reverse iteratation end.
    ReverseIterator         rend()                  { return ReverseIterator(*this, TInterface(TBegin-1) ); }

    /// Returns a const iterator pointing to the first (lowest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The start iterator. In case no bit is set, the same as #end.
    ConstIterator           begin(int skip= 0) const { return ++ConstIterator(*this, TInterface(TBegin-1) + skip ); }

    /// Returns a const iterator pointing to the first (lowest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The start iterator. In case no bit is set, the same as #end.
    ConstIterator          cbegin(int skip= 0) const { return ++ConstIterator(*this, TInterface(TBegin-1) + skip ); }

    /// Returns a const iterator pointing to the non-existing bit behind the highest one.
    /// @return The end iterator.
    ConstIterator            end()             const { return ConstIterator(*this, TInterface(TEnd) ); }

    /// Returns a const iterator pointing to the non-existing bit behind the highest one.
    /// @return The end iterator.
    ConstIterator           cend()             const { return ConstIterator(*this, TInterface(TEnd) ); }

    /// Returns a const reverse iterator pointing to the last (highest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The reverse iteration start.
    ConstReverseIterator    rbegin(int skip= 0)const { return ++ConstReverseIterator(*this, TInterface(TEnd - skip) ); }

    /// Returns a const iterator pointing to the non-existing bit before the lowest one.
    /// @return The reverse iteratation end.
    ConstReverseIterator    rend()             const { return ConstReverseIterator(*this, TInterface(TBegin-1) ); }

    /// Returns a const reverse iterator pointing to the last (highest) bit set.
    /// @param  skip Defaults to \c 0. If set, denotes the amount of bits which are not examined
    ///              at the beginning of the iteration. Must not be negative.
    /// @return The reverse iteration start.
    ConstReverseIterator    crbegin(int skip=0)const { return ++ConstReverseIterator(*this, TInterface(TEnd - skip) ); }

    /// Returns a const iterator pointing to the non-existing bit before the lowest one.
    /// @return The reverse iteratation end.
    ConstReverseIterator    crend()            const { return ConstReverseIterator(*this, TInterface(TBegin-1) ); }

}; // class TBitSet

} // namespace alib[::lang]

DOX_MARKER([DOX_MANUAL_ALIASES_BITSET])
/// Type alias in namespace \b alib.
template<int TEnd, int TBegin= 0, typename TInterface= int>
using BitSet = lang::TBitSet<int, TEnd, TBegin>;
DOX_MARKER([DOX_MANUAL_ALIASES_BITSET])

} // namespace [alib]

// ------------------------   global operators on class TBitSet    ---------------------------------
// For documentation, these global operators are faked into namespace alib::lang
#if DOXYGEN
namespace alib::lang {
#endif

/// Performs binary AND operation two \b TBitSet objects (of equal size), \p{lhs} and \p{rhs}.
/// \note This operator function is located in the global namespace. The documentation shows
///       namespace <em>alib</em>, which is done for the purposes of organizing the manual
///       index better.
/// @param lhs               The left hand side operand.
/// @param rhs               The right hand side operand.
/// @tparam TEnd  Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TBegin        Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TInterface       Template parameter of class \b TBitSet. Deduced by the compiler.
/// @return A temporary \b TBitSet containing the result of the operation.
ALIB_EXPORT
template<typename TInterface, TInterface TEnd, TInterface TBegin>
constexpr alib::lang::TBitSet<TInterface,TEnd,TBegin> operator&(
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& lhs,
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& rhs   )                 noexcept
{
    alib::lang::TBitSet<TInterface,TEnd,TBegin> result
                   = alib::lang::TBitSet<TInterface,TEnd,TBegin>(nullptr);
    if constexpr (alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords == 1 )
        result.Import( lhs.Export() & rhs.Export() );
    else
        for (int w = 0; w < alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords; ++w)
            result.Import( lhs.Export(w) & rhs.Export(w), w );
    return result;
}

/// Performs binary OR operation two \b TBitSet objects (of equal size), \p{lhs} and \p{rhs}.
/// \note This operator function is located in the global namespace. The documentation shows
///       namespace <em>alib</em>, which is done for the purposes of organizing the manual
///       index better.
/// @param lhs               The left hand side operand.
/// @param rhs               The right hand side operand.
/// @tparam TEnd  Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TBegin        Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TInterface       Template parameter of class \b TBitSet. Deduced by the compiler.
/// @return A temporary \b TBitSet containing the result of the operation.
ALIB_EXPORT
template<typename TInterface, TInterface TEnd, TInterface TBegin>
constexpr alib::lang::TBitSet<TInterface,TEnd,TBegin> operator|(
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& lhs,
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& rhs   )                 noexcept
{
    alib::lang::TBitSet<TInterface,TEnd,TBegin> result
                   = alib::lang::TBitSet<TInterface,TEnd,TBegin>(nullptr);
    if constexpr (alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords == 1 )
        result.Import( lhs.Export() | rhs.Export() );
    else
        for (int w = 0; w < alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords; ++w)
            result.Import( lhs.Export(w) | rhs.Export(w), w );
    return result;
}

/// Performs binary XOR operation two \b TBitSet objects (of equal size), \p{lhs} and \p{rhs}.
/// \note This operator function is located in the global namespace. The documentation shows
///       namespace <em>alib</em>, which is done for the purposes of organizing the manual
///       index better.
/// @param lhs               The left hand side operand.
/// @param rhs               The right hand side operand.
/// @tparam TEnd  Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TBegin        Template parameter of class \b TBitSet. Deduced by the compiler.
/// @tparam TInterface       Template parameter of class \b TBitSet. Deduced by the compiler.
/// @return A temporary \b TBitSet containing the result of the operation.
ALIB_EXPORT
template<typename TInterface, TInterface TEnd, TInterface TBegin>
constexpr alib::lang::TBitSet<TInterface,TEnd,TBegin> operator^(
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& lhs,
    const alib::lang::TBitSet<TInterface,TEnd,TBegin>& rhs   )                 noexcept
{
    alib::lang::TBitSet<TInterface,TEnd,TBegin> result
                   = alib::lang::TBitSet<TInterface,TEnd,TBegin>(nullptr);
    if constexpr (alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords == 1 )
        result.Import( lhs.Export() ^ rhs.Export() );
    else
        for (int w = 0; w < alib::lang::TBitSet<TInterface,TEnd,TBegin>::QtyWords; ++w)
            result.Import( lhs.Export(w) ^ rhs.Export(w), w );
    return result;
}
#include "ALib.Lang.CIMethods.H"
#if DOXYGEN
} // namespace [alib::lang]
#endif



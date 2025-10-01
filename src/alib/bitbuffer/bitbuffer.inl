//==================================================================================================
/// \file
/// This header-file is part of module \alib_bitbuffer of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace bitbuffer {

//==================================================================================================
/// An array of integral values used for serializing and deserializing data on bit-level.
/// While writing and reading bits is performed with associated classes \alib{bitbuffer;BitWriter} and
/// and \alib{bitbuffer;BitReader}, this class is responsible for storing the data and transferring
/// it to an <em>integral</em>-arrays, which may for example be written and read to and from
/// <c>std::[i/o]stream</c> objects.
/// With this, platform independence is guaranteed (in respect to little/big-endian storage and
/// similar matters).
///
/// This class is abstract (pure virtual), and does not perform the allocation.
/// With \alib{bitbuffer;BitBuffer}, \alib{bitbuffer;BitBufferMA} and \alib{bitbuffer;BitBufferLocal},
/// three descendants with different allocation strategies are provided.
/// A customized version may be easily created by implementing pure virtual methods
/// #Capacity and #EnsureCapacity.
///
/// \attention
///   To avoid the use of virtual function calls bit write operations, methods #Capacity and
///   #EnsureCapacity are <b>not invoked automatically!</b>.
///   In contrast, it is the users' responsibility to invoke these methods (or only #EnsureCapacity)
///   before performing data insertions.<br>
///   This behavior is a design decision to maximize execution performance, hence rather a feature.
///
/// Own Implementations have to set field #data when reallocating the internal buffer.
//==================================================================================================
class BitBufferBase
{
    public:
        /// The storage type of bit buffers. This is chosen as being <c>unsigned int</c>, which
        /// should be the "fasted" integral type for any compiler/platform combination.
        using TStorage= unsigned int;

        static_assert( bitsof(TStorage) == 32 || bitsof(TStorage) == 64,
                       "Unsupported size of C++ int type" );

        /// Defines a bit position within outer class \alib{bitbuffer;BitBufferBase}. A bit position is
        /// determined by the index in the storage array along with the number of the currently
        /// written (or read) bit. Types \alib{bitbuffer;BitWriter} and \alib{bitbuffer;BitReader}
        /// use this type to define their current write (read) position.
        ///
        /// Methods #Encode32 and #Encode64 shorten the information, by storing the bit position in
        /// the upper bits of a \e 32, respectively \e 64 bit value. This is useful whenever a
        /// broader number of bit buffer indices are to be stored. The use case to mention here is
        /// "lazy decoding of data", where only the index to the bit buffer is kept in memory.)
        /// positions
        class Index
        {
            #if !DOXYGEN
            friend class BitBufferBase;
            friend class BitReader;
            friend class BitWriter;
            #endif

            uinteger          pos= 0;     ///< Index of the current word to read/write.
            lang::ShiftOpRHS  bit= 0;     ///< Current bit index in the current word.

            public:

            /// Default constructor initializing members #pos and #bit to zero.
            Index() = default;

            /// Constructor
            /// @param pPos The initial value for member #pos.
            /// @param pBit The initial value for member #bit.
            Index( uinteger pPos, lang::ShiftOpRHS pBit)
            : pos(pPos)
            , bit(pBit)
            {}

            /// Returns the index of the actual storage word in the buffer.
            /// @return The index of the current word containing the next bit to read/write.
            uinteger          Pos()           const { return pos; }

            /// Returns the number of the actual bit in the actual word of the buffer buffer.
            /// @return The number of the next bit to read/write.
            lang::ShiftOpRHS  Bit()           const { return bit; }

            /// Returns true, if the next bit to read/write is the first of the current storage
            /// word in the buffer. Alignment of buffers may become important when buffers are
            /// serialized (e.g., to mass storage devices). Method
            /// \alib{bitbuffer;BitBufferBase::Terminate} may be used to receive an aligned
            /// index.
            ///
            /// @return The result of <c>Bit() == 0</c>.
            bool        IsAligned()     const { return bit == 0; }

            /// Sets this index to zero, hence pointing to the first bit in the buffer.
            void        Clear()
            {
                pos= 0;
                bit= 0;
            }

            /// Returns the size of the memory from given \p startIdx to this index occupied by
            /// the internal storage words of the buffer.
            ///
            /// @param startIdx The starting index. Defaults to <c>{0,0}</c>.
            /// @return The size of the buffer (part) in bytes.
            integer GetByteOffset( Index startIdx= Index(0, 0)  )                              const
            {
                ALIB_ASSERT_ERROR(     startIdx.pos < pos
                                   || (startIdx.pos == pos && startIdx.bit < bit), "BITBUFFER",
                     "Given buffer start index is greater than this index." )
                return integer((pos - startIdx.Pos()) * sizeof(TStorage) );
            }

            /// Sets this index to point to the word and bit given by a byte offset.<br>
            /// This method is useful when bit buffers are deserialized from character streams.
            /// @param byteOffset The position within the buffer in bytes.
            void        SetFromByteOffset( uinteger byteOffset )
            {
                pos=  byteOffset / sizeof( TStorage );
                bit= (byteOffset % sizeof( TStorage )) * 8 ;
            }

            /// Returns the number of bits used in respect to this index.
            /// @return The number of bits written or read.
            uinteger    CountBits()                                                            const
            {
                return uinteger( pos * bitsof(TStorage) + uinteger(bit) );
            }

            /// Encodes this index information into a 32-bit variable by using the upper 5 (or 6)
            /// bits for the bit index. As a result, the possible value range of index data is
            /// reduced. The reduction depends on the platform's size of type \c int. In case of
            /// 32-bit, five bits are needed to store the bit position. In the case of 64-bit,
            /// six bits are needed.<br>
            /// As the underlying \e TStorage type changes as well, in both cases, the resulting
            /// addressable storage bytes is limited to the same value:
            /// - TStorage 64-bit: <em>2^(32-6) * 8 bytes = 512 megabytes</em>
            /// - TStorage 32-bit: <em>2^(32-5) * 4 bytes = 512 megabytes</em>
            ///
            /// In case bit buffers grow to over half a gigabyte, 64-bit encoding should be performed
            /// by using alternative method #Encode64.
            ///
            /// @return The encoded index.
            uint32_t    Encode32()
            {
                ALIB_ASSERT_ERROR(pos < (uinteger(1) << (31 - lang::Log2OfSize<TStorage>() ) ),
                    "BITBUFFER",  "32bit too narrow for endcoding BitBuffer::Index." )
                return uint32_t(pos) | (uint32_t(bit) << (31 - lang::Log2OfSize<TStorage>() ));
            }

            /// Encodes this index information into a 64-bit value by using the upper 5 5 (or 6)
            /// bits for the bit index.
            ///
            /// @see For a shorter encoding, limited to bit buffer sizes of 512 megabytes,
            ///      see method #Encode32.
            ///
            /// @return The encoded index.
            uint64_t    Encode64()
            { return uint64_t(pos) | (uint64_t(bit) << (63L - lang::Log2OfSize<TStorage>() ));     }

            /// Static method that decodes an index information, encoded with #Encode32, to an
            /// instance of this class.
            ///
            /// @param code  The encoded information.
            /// @return The decoded index.
            static
            Index       Decode32(uint32_t code)
            {
                return Index { uinteger  (code & lang::LowerMask<  31  - lang::Log2OfSize<TStorage>() , uint32_t>() ),
                               lang::ShiftOpRHS(code            >>(31  - lang::Log2OfSize<TStorage>() ))              };
            }


            /// Static method that decodes an index information, encoded with #Encode64, to an
            /// instance of this class.
            ///
            /// @param code  The encoded information.
            /// @return The decoded index.
            static
            Index       Decode64(uint64_t code)
            { return Index { uinteger  (code & lang::LowerMask<  63L - lang::Log2OfSize<TStorage>() , uint64_t>() ),
                             lang::ShiftOpRHS(code            >>(63L - lang::Log2OfSize<TStorage>()))               }; }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object equals \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator==(const Index& rhs)                                                  const
            {
                return      (pos == rhs.pos)
                        &&  (bit == rhs.bit);
            }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object does not equal \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator!=(const Index& rhs)                                                  const
            {
                return   !(*this == rhs);
            }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object is smaller than \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator<(const Index& rhs)                                                  const
            {
                return (pos < rhs.pos) || ((pos == rhs.pos)
                                           && (bit <  rhs.bit)  );
            }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object is smaller or equal than \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator<=(const Index& rhs)                                                  const
            {
                return (pos < rhs.pos) || ((pos == rhs.pos)
                                           && (bit <=  rhs.bit) );
            }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object is greater or equal than \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator>=(const Index& rhs)                                                  const
            {
                return !(*this < rhs);
            }

            //======================================================================================
            /// Comparison operator.
            ///
            /// @param rhs The right hand side argument of the comparison.
            /// @return \c true if this object is greater than \p{rhs}, \c false otherwise.
            //======================================================================================
            bool operator>(const Index& rhs)                                                  const
            {
                return !(*this <= rhs);
            }
        }; // inner struct Index

    protected:

        /// A pointer to the storage. Implementations of this abstract type have to set this field
        /// when reallocating the internal buffer.
        TStorage*   data;

    public:
        //==========================================================================================
        /// Default Constructor (the only one).
        //==========================================================================================
                            BitBufferBase()  noexcept : data(nullptr) {}

        //==========================================================================================
        /// Virtual destructor (does nothing, needed for abstract virtual class).
        //==========================================================================================
        virtual            ~BitBufferBase()
        {}

        //==========================================================================================
        /// Virtual function to determine the (currently allocated) capacity.
        /// @return The size of the internal storage in bits.
        //==========================================================================================
        ALIB_DLL
        virtual uinteger    Capacity()                                                    const = 0;

        //==========================================================================================
        /// Virtual function to reserve buffer space by optionally increasing the buffer to
        /// enable the writing of the given bits.
        ///
        /// @param bitsRequired The number of bits required.
        /// @param index        The index to which the capacity is currently used.
        /// @return \c true if the space is available or could be made available,
        ///         \c false otherwise.
        //==========================================================================================
        ALIB_DLL
        virtual bool        EnsureCapacity( uinteger bitsRequired, BitBufferBase::Index index ) = 0;

        //==========================================================================================
        /// Returns the storage word at the given position
        /// @param index The index to read the word from. Note that the bit number in this value is
        ///              ignored.
        /// @return The word at the given index.
        //==========================================================================================
        TStorage            GetWord(const Index& index)                                        const
        {
            return data[index.pos];
        }

        //==========================================================================================
        /// Stores the given \p value at the given \p index.
        /// @param index The index to read the word at. Note that the bit number in this value is
        ///              ignored.
        /// @param value The value to store
        //==========================================================================================
        void                SetWord(const Index& index, TStorage value)
        {
            data[index.pos]= value;
        }

        //==========================================================================================
        /// Returns the number of remaining bits in this buffer in relation to a given index.
        /// @param idx  An actual writing/reading position.
        /// @return The number of bits dived remaining in this buffer.
        //==========================================================================================
        uinteger            RemainingSize(const Index& idx)                                    const
        {
            return   Capacity() - idx.CountBits();
        }

        //==========================================================================================
        /// Returns the start of the internal storage.
        /// @return A pointer to the data array provided by the decendent types.
        //==========================================================================================
        TStorage*           Data()                                                             const
        {
            return data;
        }

        //==========================================================================================
        /// Returns the memory address of the internal storage word denoted by \p idx
        /// reinterpreted to C++ type <c>char*</c>.
        /// @param idx The index of the word to point to. The bit position within this index is
        ///            ignored.
        /// @return A \c char pointer to the internal storage word the given index refers to.
        //==========================================================================================
        char* CharStream( Index idx= Index(0, 0) )
        {
            return reinterpret_cast<char*>( &data[idx.pos] );
        }

        //==========================================================================================
        /// Writes a termination bit of value \c 1 and lets this buffer's index point to the next
        /// buffer word.\n
        /// Termination can be undone using the result index of this method with #Unterminate.
        /// This method should be invoked before serializing a buffer and method
        /// #Unterminate may be used after deserialization to continue writing to the buffer without
        /// creating a gap.
        ///
        /// @param writerIndex The index to the last bit before termination.
        ///
        /// @return The \alib{bitbuffer;BitBufferBase::Index::IsAligned;aligned} index after termination
        ///         which is aligned point to the first bit behind the last used storage word.
        ///         Such index may be later fed into method #Unterminate to undo the termination.
        //==========================================================================================
        ALIB_DLL
        Index   Terminate  (Index writerIndex);

        //==========================================================================================
        /// Removes the termination bit found in the word before given \p terminationIndex.
        /// @param terminationIndex The index returned by previous invocation of method #Terminate.
        ///
        /// @return The index of the next bit to write to the now unterminated buffer.
        //==========================================================================================
        ALIB_DLL
        Index   Unterminate(Index terminationIndex);

        //==========================================================================================
        /// Converts the internal storage words into the platform-independent
        /// "Little Endian Encoding", which means it may change the byte order within the storage
        /// words of the buffer.
        ///
        /// This method is recommended to be used before writing buffer contents to a file to
        /// make files system independent.
        ///
        /// \attention The start index needs to be aligned to a storage word. This is asserted
        ///            in debug compilations.
        ///            See method \alib{bitbuffer::BitBufferBase;Index::IsAligned} for more information.
        ///
        /// \note It is recommended to terminate the buffer before using this method.
        ///       and to pass the index returned by method #Terminate as second parameter
        ///       \p endIndex.
        ///
        /// \see Method #FromLittleEndianEncoding.
        ///
        /// @param startIndex The first storage word to convert. (The bit position of the index
        ///                   is ignored).
        /// @param endIndex   The first bit behind the storage to be converted. Hence, if the bit
        ///                   position within this argument is not \c 0, then the whole word that
        ///                   this index points to, will be converted. Otherwise it won't.
        //==========================================================================================
        ALIB_DLL
        void    ToLittleEndianEncoding( const Index&   startIndex,
                                        const Index&   endIndex    );

        //==========================================================================================
        /// The counter-method to #ToLittleEndianEncoding.
        ///
        /// @param startIndex The first storage word to convert. (The bit position of the index
        ///                   is ignored).
        /// @param endIndex   The first bit behind the storage to be converted. Hence, if the bit
        ///                   position within this argument is not \c 0, then the whole word that
        ///                   this index points to, will be converted. Otherwise it won't.
        //==========================================================================================
        ALIB_DLL
        void    FromLittleEndianEncoding( const Index& startIndex, const Index& endIndex );
}; // class BitBufferBase

//==================================================================================================
/// A bit buffer using dynamic allocation.
/// \see
///   - Two alternatives are provided with \alib{bitbuffer;BitBufferMA}, which uses
///     \ref alib_mods_contmono "monotonic allocation" and \alib{bitbuffer;BitBufferLocal}, which uses
///     local (stack) memory.
///   - For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///     GNU debugger is provided.
//==================================================================================================
class BitBuffer: public BitBufferBase
{
    protected:
        /// The vector that holds the data.
        std::vector<TStorage>   storage;

    public:
        //==========================================================================================
        /// Constructor.
        /// @param initialCapacity  The requested initial capacity of the buffer in bits.
        //==========================================================================================
        BitBuffer( uinteger initialCapacity )
        {
            EnsureCapacity(initialCapacity, Index());
        }

        //==========================================================================================
        /// Returns the (currently allocated) capacity.
        /// @return The size of the internal storage in bits.
        //==========================================================================================
        virtual uinteger Capacity()                                                  const  override
        {
            return storage.capacity() * bitsof(TStorage);
        }

        //==========================================================================================
        /// Checks if the given required storage space is internally reserved. If not,
        /// the internal capacity is doubled, or, if more is required, set to the required space.
        ///
        /// @param bitsRequired The number of bits required.
        /// @param idx          The index of current buffer use.
        /// @return \c true if the space is available or could be made available,
        ///         \c false otherwise.
        //==========================================================================================
        virtual bool     EnsureCapacity(uinteger bitsRequired, BitBufferBase::Index idx)    override
        {
            uinteger capacityNeeded= (idx.CountBits() + bitsRequired + (bitsof(TStorage) - 1) )
                                     / bitsof(TStorage);
            if( capacityNeeded > storage.capacity() )
                storage.reserve( (std::max)( capacityNeeded, uinteger(storage.capacity()) * 2 ));
            data= storage.data();

            return true;
        }
}; // class BitBuffer

//==================================================================================================
/// A bit buffer using \ref alib_mods_contmono "monotonic allocation".
/// \see
///   Two alternatives are provided with \alib{bitbuffer;BitBuffer} and \alib{bitbuffer;BitBufferLocal}.
//==================================================================================================
class BitBufferMA  : public BitBufferBase
{
    protected:
        /// The monotonic allocator used internally to allocate the storage. This is provided
        /// with construction.
        MonoAllocator&             ma;

        /// The vector that holds the data.
        StdVectorMono<TStorage>    storage;

    public:
        //==========================================================================================
        /// Constructor taking an external monotonic allocator and the initial capacity.
        /// @param monoAllocator    A reference to a monotonic allocator to use to allocate buffer
        ///                         storage data.
        /// @param initialCapacity  The requested initial capacity of the buffer in bits.
        //==========================================================================================
        BitBufferMA( MonoAllocator& monoAllocator, uinteger initialCapacity )
        : ma( monoAllocator )
        , storage(ma)
        {
            EnsureCapacity(initialCapacity, Index());
        }


        //==========================================================================================
        /// Returns the (currently allocated) capacity.
        /// @return The size of the internal storage in bits.
        //==========================================================================================
        virtual uinteger Capacity()                                                  const  override
        {
            return storage.capacity() * bitsof(TStorage);
        }

        //==========================================================================================
        /// Checks if the given required storage space is internally reserved. If not,
        /// the internal capacity is doubled, or, if more is required, set to the required space.
        ///
        /// @param bitsRequired The number of bits divided required.
        /// @param idx          The index of current buffer use.
        /// @return \c true if the space is available or could be made available,
        ///         \c false otherwise.
        //==========================================================================================
        virtual bool     EnsureCapacity(uinteger bitsRequired, BitBufferBase::Index idx)    override
        {
            uinteger capacityNeeded= (idx.CountBits() + bitsRequired + (bitsof(TStorage) - 1) )
                                     / bitsof(TStorage);
            if( capacityNeeded > storage.capacity() )
                storage.reserve( (std::max)( capacityNeeded, uinteger(storage.capacity()) * 2 ));
            data= storage.data();

            return true;
        }

        //==========================================================================================
        /// Returns the internal monotonic allocator for external use.
        /// @return The monotonic allocator given on construction.
        //==========================================================================================
        MonoAllocator& GetAllocator()
        {
            return ma;
        }
}; // class BitBufferMA

//==================================================================================================
/// A bit buffer using local storage, which means a fixed size internal array.
/// If used as function member, the storage is located on the stack and hence its size has
/// platform-specific limitations.<br>
/// This class is useful to read and write smaller pieces of data, for example header information
/// of binary data files which furthermore are filled/loaded with bit buffers using of other memory
/// allocations.
/// \see
///   Two alternatives are provided with \alib{bitbuffer;BitBuffer} and \alib{bitbuffer;BitBufferMA}.
/// @tparam TCapacity The number of bits to reserve internally
//==================================================================================================
template<uinteger TCapacity>
class BitBufferLocal  : public BitBufferBase
{
    protected:
        /// The array that holds the data.
        TStorage    storage[ (TCapacity + bitsof(TStorage) - 1) / bitsof(TStorage) ];

    public:
        //==========================================================================================
        /// Constructor.
        //==========================================================================================
        BitBufferLocal()                                                                    noexcept
        {
            data= storage;
        }

        //==========================================================================================
        /// Returns the (in this case fixed size!) capacity.
        /// @return The size of the internal storage in bits.
        //==========================================================================================
        virtual uinteger Capacity()                                                  const  override
        {
            return TCapacity;
        }

        //==========================================================================================
        /// Checks if the given required storage space is internally reserved.
        /// If not, in debug compilations, an \alib assertion is raised, as this is a fixed size
        /// buffer.
        ///
        /// @param bitsRequired The number of bits required.
        /// @param idx          The index of current buffer use.
        /// @return \c true if the space is available or could be made available,
        ///         \c false otherwise.
        //==========================================================================================
        virtual bool     EnsureCapacity(uinteger bitsRequired, BitBufferBase::Index idx)    override
        {
            uinteger capacityNeeded= idx.CountBits() + bitsRequired;
            if( capacityNeeded > TCapacity )
            {
                ALIB_ERROR("BITBUFFER", "Local bit buffer cannot expand its capacity" )
                return false;
            }

            return true;
        }
}; // class BitBufferLocal


//==================================================================================================
/// Non-instantiatable base class for types \alib{bitbuffer;BitWriter} and \alib{bitbuffer;BitReader}.
//==================================================================================================
class BitRWBase
{
    protected:
        BitBufferBase&          bb;    ///< The bit buffer to write into. Provided on construction.
        BitBufferBase::Index    idx;   ///< The current reading/writing index within #bb.

        /// Protected constructor, used by derived classes only.
        /// @param buffer The bit buffer to work on.
        explicit BitRWBase( BitBufferBase& buffer )
        : bb  ( buffer )
        {}

    public:
        /// Retrieves the internal bit buffer.
        /// @return The buffer the derived reader or writer works with.
        BitBufferBase&              GetBuffer()                                                const
        {
            return bb;
        }

        //==========================================================================================
        /// Returns a copy of the current index in the bit buffer in respect to writing or
        /// reading progress of derived classes \alib{bitbuffer;BitWriter} \alib{bitbuffer;BitReader}.
        /// Such index elements may be passed to methods
        /// \alib{bitbuffer;BitWriter::Reset(const BitBufferBase::Index&)} and
        /// \alib{bitbuffer;BitReader::Reset(const BitBufferBase::Index&)}
        /// @return The index of the next bit to write.
        //==========================================================================================
        BitBufferBase::Index        GetIndex()                                                 const
        {
            return idx;
        }

        /// Invokes \alib{bitbuffer;BitBufferBase::Index::CountBits} on the index returned by #GetIndex.
        /// @return The number of bits currently read from or written to the buffer.
        uinteger                    Usage()                                                    const
        {
            return idx.CountBits();
        }

        /// Invokes \alib{bitbuffer;BitBufferBase::RemainingSize} on the internal bit buffer, passing
        /// the result of #GetIndex.
        /// @return The number of bits dived by 8 remaining in this buffer.
        uinteger                    RemainingSize()                                            const
        {
            return   bb.RemainingSize(idx);
        }

};

//==================================================================================================
/// Writes bits into a \alib{bitbuffer;BitBufferBase}.
//==================================================================================================
class BitWriter : public BitRWBase
{
    /// Local type alias (shortcut)
    using TStorage= BitBufferBase::TStorage;

    /// The current word, which is partly written and not stored in buffer, yet.
    BitBufferBase::TStorage     word;

    public:
        /// Constructs a bit writer operating on the given bit buffer.
        /// @param buffer The buffer to write to (starting at the beginning).
        explicit BitWriter( BitBufferBase& buffer )
        : BitRWBase( buffer )
        {
            word= 0;
        }

        /// Constructs a bit writer operating on the given bit buffer, starting to write at the
        /// given \alib{bitbuffer::BitBufferBase;Index}.
        /// @param buffer The buffer to write to.
        /// @param index  An index providing the postion of the first bit to (over-) write in
        ///               \p buffer.
        explicit BitWriter( BitBufferBase& buffer, const BitBufferBase::Index& index )
        : BitRWBase  ( buffer )
        {
            idx= index;
            word= bb.GetWord(idx) & lang::LowerMask<TStorage>( idx.bit );
        }

        /// Destructs a bit writer. Invokes #Flush().
        ~BitWriter()
        {
            Flush();
        }

        /// Resets the internal index of this writer to the start of the bit buffer.
        void   Reset()
        {
            idx = BitBufferBase::Index();
            word= 0;
        }

        /// Resets the internal index of this writer to the given one.
        /// @param index The position of the next bit in the buffer to write to.
        void   Reset( const BitBufferBase::Index& index )
        {
            idx.pos= index.pos;
            idx.bit= index.bit;
            word= bb.GetWord(idx) & lang::LowerMask<TStorage>(idx.bit );
        }

        /// Writes the last word of bits into the underlying buffer.
        /// This method has to be called before writing the buffer to a file or similar.
        /// The method is automatically invoked on destruction.
        void    Flush()
        {
            bb.SetWord(idx, word);
        }

    #if DOXYGEN
        /// Writes the given integral value with the given number of bits to the stream.
        /// \note
        ///   Internally, different template functions selected with keyword \c requires
        ///   for the different integral types exist.
        ///
        /// \see
        ///   This method uses a template parameter for the number of bits to write.
        ///   A slightly slower, non-templated version is available with
        ///   #Write<TValue, TMaskValue>( ShiftOpRHS, TValue)
        ///   which is to be used when the value is determined only at run-time.
        ///
        /// @tparam TWidth     The number of bits in \p value to write.
        /// @tparam TValue     The type of \p value to write. (Deduced from parameter \p value.)
        /// @tparam TMaskValue Determines if bits beyond \p width of given \p value may be set and
        ///                    have to be masked out.
        ///                    Defaults to \c false.
        /// @param  value      The value to write.
        template<ShiftOpRHS TWidth, typename TIntegral, bool TMaskValue= false>
        void Write( TIntegral value );
    #else
        // Version 1/2: #Bits to write are less or equal to internal buffer width
        template<lang::ShiftOpRHS TWidth, typename TValue, bool TMaskValue= false>
        requires (      std::unsigned_integral<TValue>
                    && !std::same_as          < TValue, bool>
                    && ( TWidth <= bitsof(TStorage) )            )
        void Write(TValue value )
        {
            static_assert(bitsof(TValue) >= TWidth, "Fixed size given greater than value type.");
            ALIB_ASSERT_ERROR(idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
            ALIB_ASSERT_ERROR(    TMaskValue
                               || TWidth == bitsof(TValue)
                               || value == (value & lang::LowerMask<TValue>(TWidth) ),  "BITBUFFER",
                               "Upper bits dirty while TMaskValue flag not set." )

            if constexpr ( (TWidth < bitsof(TValue)) && TMaskValue )
                value&=  lang::LowerMask<TWidth, TValue >();

            word|=   TStorage(value) << idx.bit ;
            idx.bit+= TWidth;
            if(idx.bit >= bitsof(TStorage) )
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= 0;
                idx.bit-= bitsof(TStorage);
                if( idx.bit )
                    word|= (TStorage(value) >> (TWidth - idx.bit) );
            }
        }

        // Version 2/2: #Bits to write is greater than internal buffer width
        template<lang::ShiftOpRHS TWidth, typename TValue, bool TMaskValue= false>
        requires (      std::unsigned_integral<TValue>
                    && !std::same_as          < TValue, bool>
                    && ( TWidth > bitsof(TStorage) )            )
        void Write(TValue value )
        {
            static_assert(bitsof(TValue) >= TWidth, "Fixed size given greater than value type.");
            ALIB_ASSERT_ERROR(idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
            ALIB_ASSERT_ERROR(    TMaskValue
                               || TWidth == bitsof(TValue)
                               || value == (value & lang::LowerMask<TValue>(TWidth) ),  "BITBUFFER",
                               "Upper bits dirty while TMaskValue not set." )

            if constexpr ( (TWidth <  bitsof(TValue)) && TMaskValue )
                value&=  lang::LowerMask<TWidth, TValue>();

            word|= (TStorage(value) << idx.bit);
            lang::ShiftOpRHS bitsWritten= bitsof(TStorage) - idx.bit;
            value>>= bitsWritten;
            while(true)  // the loop is at least hit once and bit is 0! (but not written in bit)
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= TStorage(value);
                bitsWritten+= bitsof(TStorage);
                if(bitsWritten >= TWidth )
                    break;

                value>>= bitsof(TStorage);
            }

            idx.bit= (idx.bit + TWidth) % bitsof(TStorage);
            if(idx.bit == 0 )  // next buffer value reached?
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= 0;
            }
        }

        // Helper-versions for signed and bool
        template<lang::ShiftOpRHS TWidth, typename TValue, bool TMaskValue= false>
        requires ( std::signed_integral<TValue> && !std::same_as<TValue, bool>  )
        void Write( TValue value)
        {
            using TUI= typename std::make_unsigned<TValue>::type;
            Write<TWidth, TUI, TMaskValue>( static_cast<TUI>( value ) );
        }

        template<typename TValue>
        requires ( std::same_as<TValue, bool>  )
        void Write( TValue value)
        { Write<1, unsigned int, false>( static_cast<unsigned int>( value ) ); }

    #endif // doxygen

    /// Writes the given integral value with the given number of bits to the stream.
    /// \note Two overloads of this method, selected with keyword \c requires,
    ///       for different integral types exist.
    ///       This is version 1/2 which requires that the given value type is smaller than or equal
    ///       to the storage type.
    /// \see
    ///   A method that uses a template parameter for the number of bits to write, is
    ///   available with #Write<TWidth,TIntegral>(TIntegral).
    ///   This might be slightly faster and should be used instead of this method, whenever the
    ///   number of bits to write is known at compilation time.
    ///
    /// @tparam TValue     The integral type of the value to write.
    ///                    (Deduced from parameter \p value.)
    /// @tparam TMaskValue Determines if bits beyond \p width of given \p value may be set and
    ///                    have to be masked out. Defaults to \c false.
    /// @param  width      The number of bits in \p value.
    /// @param  value      The value to write.
    template<typename TValue, bool TMaskValue= false>
    requires ( std::integral<TValue>  && ( sizeof(TValue) <= sizeof(TStorage) ) )
    void Write(lang::ShiftOpRHS width, TValue value)
    {
        ALIB_ASSERT_ERROR(idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
        ALIB_ASSERT_ERROR( width <= bitsof(TValue),
              "BITBUFFER", "BitBufferBase::Write: Width too high: ", width )

        ALIB_ASSERT_ERROR(    TMaskValue
                           || width>=bitsof(TValue)
                           || value == (value & lang::LowerMask<TValue>(width) ),
              "BITBUFFER", "Upper bits dirty while TMaskValue not set.")

         if constexpr (TMaskValue)
            if( width < bitsof(TValue) )
                value&= lang::LowerMask<TValue>(width);

        word|= TStorage(value) << idx.bit ;
        idx.bit+= width;
        if(idx.bit >= bitsof(TStorage) )
        {
            bb.SetWord(idx, word);
            idx.pos++;
            word= 0;
            idx.bit-= bitsof(TStorage);
            if( idx.bit )
                word|= ( TStorage(value) >> (width - idx.bit) );
        }
    }

    /// Writes the given integral value with the given number of bits to the stream.
    /// \note Two overloads of this method, selected with keyword \c requires,
    ///       for different integral types exist.
    ///       This is version 2/2 which requires that the given value type is greater than
    ///       as the storage type.
    ///
    /// @tparam TValue     The integral type of the value to write.
    ///                    (Deduced from parameter \p value.)
    /// @tparam TMaskValue Determines if bits beyond \p width of given \p value may be set and
    ///                    have to be masked out. Defaults to \c false.
    /// @param  width      The number of bits in \p value.
    /// @param  value      The value to write.
    template<typename TValue, bool TMaskValue= false>
    requires ( std::integral<TValue>  && ( sizeof(TValue) > sizeof(TStorage) ) )
    void Write(lang::ShiftOpRHS width, TValue value)
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
        ALIB_ASSERT_ERROR( width <= bitsof(TValue), "BITBUFFER",
                "BitBufferBase::Write: Width too high: ", width )
        ALIB_ASSERT_ERROR(    TMaskValue
                           || width==bitsof(TValue)
                           || value == (value & lang::LowerMask<TValue>(width) ),
                "BITBUFFER", "Upper bits dirty while TMaskValue not set.")

        if constexpr (TMaskValue)
            if( width <= bitsof(TValue) )
                value&= lang::LowerMask<TValue>(width);

        if( width <= bitsof(TStorage) )
        {
            word|= TStorage(value) << idx.bit ;
            idx.bit+= width;
            if(idx.bit >= bitsof(TStorage) )
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= 0;
                idx.bit-= bitsof(TStorage);
                if( idx.bit )
                    word|= ( TStorage(value) >> (width - idx.bit) );
            }
        }
        else
        {
            word|= (TStorage(value) << idx.bit);
            lang::ShiftOpRHS bitsWritten= bitsof(TStorage) - idx.bit;
            value>>= bitsWritten;
            while(true)  // the loop is at least hit once and bit is 0! (but not written in bit)
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= TStorage(value);
                bitsWritten+= bitsof(TStorage);
                if(bitsWritten >= width )
                    break;
                value>>= bitsof(TStorage);
            }
            idx.bit= (idx.bit + width) % bitsof(TStorage);
            if(idx.bit == 0 )  // next buffer value reached?
            {
                bb.SetWord(idx, word);
                idx.pos++;
                word= 0;
            }
        }
    }

    /// Writes the given unsigned integral value to the stream by writing lower values
    /// with smaller sizes. The general assumption behind this is that lower values are more frequent
    /// and the average written size is less than the unencode size - despite the overhead needed
    /// to write information about how a value is encoded.
    ///
    /// The encoding for \e unsigned integrals is as follows:
    /// - For byte value types, a single bit <c>'0'</c> is written if the value is below \c 8,
    ///   followed by the three bits containing the value. Otherwise, a single bit <c>'1'</c> is
    ///   written, followed by the full 8 bits.
    /// - For all other value types (16-, 32- and 64-bit) the number of bytes needed is written
    ///   first (one bit in the case of 16-bit values, two bits in the case of 32-bit values
    ///   and three bits in the case of 64 bit values), and then the corresponding amount of
    ///   full bytes are written.
    ///
    /// \e Signed integrals are converted to unsigned integrals using the sometimes called
    /// "zig-zag coding". Here, all numbers are doubled and negative numbers are turned
    /// positive and uneven. This way, the least significant bit becomes the sign bit.
    /// The advantage of this approach is that small numbers, negative or positive,
    /// remain small in respect to their bitwise representation.<p>
    /// The conversion hence is as follows:
    ///      unsigned =  signed >= 0  ?   signed * 2   :  ( (-signed -1 ) * 2 ) | 1
    ///
    /// @tparam TUIntegral  The type of the given unsigned integral.
    /// @param  value       The value to write.
    template<typename TUIntegral>
    requires ( std::unsigned_integral<TUIntegral> && !std::same_as< TUIntegral, bool> )
    void Write( TUIntegral value )
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
        writeUIntegral(value);
    }

    /// Converts the given \e signed integral to an unsigned one using the so-called
    /// "zig-zag coding". Here, all numbers are doubled and negative numbers are turned
    /// positive and uneven. This way, the least significant bit becomes the sign bit.
    /// The advantage of this approach is that small numbers, negative or positive,
    /// remain small in respect to their bitwise representation.<p>
    /// The conversion hence is as follows:
    ///      unsigned =  signed >= 0  ?   signed * 2   :  ( (-signed -1 ) * 2 ) | 1
    ///
    /// Then calls \alib{bitbuffer::BitWriter;Write<TUIntegral>}.
    /// @tparam TSIntegral  The type of the given signed integral.
    /// @param  value       The value to write.
    template<typename TSIntegral>
    requires ( std::signed_integral<TSIntegral> && !std::same_as< TSIntegral, bool> )
    void Write( TSIntegral value )
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )
        using TUnsigned= typename std::make_unsigned<TSIntegral>::type;
        writeUIntegral(  value >= 0 ? TUnsigned( value << 1)
                                    : TUnsigned( (TUnsigned(-(value+ 1)) << 1) | 1 )   );
    }

  protected:

    /// Internal method that writes a unsigned 8-bit value.
    /// @param value The value to write.
    ALIB_DLL void      writeUIntegral( uint8_t  value );

    /// Internal method that writes a unsigned 16-bit value.
    /// @param value The value to write.
    ALIB_DLL void      writeUIntegral( uint16_t value );

    /// Internal method that writes a unsigned 32-bit value.
    /// @param value The value to write.
    ALIB_DLL void      writeUIntegral( uint32_t value );

    /// Internal method that writes a unsigned 64-bit value.
    /// @param value The value to write.
    ALIB_DLL void      writeUIntegral( uint64_t value );


}; // class BitWriter


//==================================================================================================
/// Reads bits from a \alib{bitbuffer;BitBufferBase}.
//==================================================================================================
class BitReader : public BitRWBase
{
  protected:
    /// Local type alias (shortcut)
    using TStorage= BitBufferBase::TStorage;

    /// The current word, which is partly read and shifted to start with current bit.
    BitBufferBase::TStorage     word;

  public:
    /// Constructs a bit reader using the given bit buffer and starting to read at the beginning.
    /// @param buffer    The buffer to read from.
    explicit BitReader( BitBufferBase& buffer )
    : BitRWBase( buffer )
    {
        word= bb.GetWord(idx);
    }


    /// Constructs a bit reader using the given bit buffer, starting to read at the
    /// given \alib{bitbuffer::BitBufferBase;Index}.
    /// @param buffer The buffer to read from.
    /// @param index  An index providing the postion of the first bit to read in \p buffer.
    explicit BitReader( BitBufferBase& buffer, const  BitBufferBase::Index& index )
    : BitRWBase( buffer )
    {
        idx.pos= index.pos;
        idx.bit= index.bit;
        word= bb.GetWord(idx) >> idx.bit;
    }

    /// Destructs a bit reader. In debug compilations an \alib assertion is raised if the
    /// a read operation passed the end of the underlying buffer was performed.
    ~BitReader()
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(), "BITBUFFER",
              "BitBufferBase overflow detected. Ensure a higher capacity" )
    }

    /// Resets this reader to the start of the bit buffer.
    void   Reset()
    {
        idx.pos= 0;
        idx.bit= 0;
        word= bb.GetWord(idx);
    }

    /// Resets this reader to the given index position and calls #Sync().
    /// @param index The next read position.
    void   Reset( const BitBufferBase::Index& index )
    {
        idx.pos= index.pos;
        idx.bit= index.bit;
        Sync();
    }

    /// Re-reads the currently fetched storage word from the memory.
    /// \note This method is not needed in common use cases and implemented solely for the
    ///       purpose to support unit-tests which write and write in parallel to the same
    ///       bit buffer.
    /// @return A reference to this \c BitReader to allow concatenated operations.
    BitReader&  Sync()
    {
        word= bb.GetWord(idx) >> idx.bit;
        return *this;
    }

    /// Reads the given number of bits from the stream into the given unsigned integral value.
    /// \note
    ///   Two different template functions (selected by keyword \c requires) for the different
    ///   integral types exist.<br>
    ///   This is version 1/2: Bits to read are less or equal to the internal buffer width
    /// \see
    ///   This method uses a template parameter for the number of bits to read.
    ///   A slightly slower, non-templated version is available with #Read<TResult>(ShiftOpRHS),
    ///   which is to be used when the number of bits to write is determined only at run-time.
    /// @tparam TWidth    The number of bits in \p{value} to write.
    /// @tparam TResult   The type of the value to return.
    /// @return The value read.
    template<lang::ShiftOpRHS TWidth, typename TResult= int>
    requires ( std::integral<TResult> &&  ( TWidth <= bitsof(TStorage)) )
    TResult Read()
    {
        ALIB_ASSERT_ERROR(idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )

        TResult     result;

        // the one bit case. Could have been left to the compiler to optimize, but who knows.
        if constexpr ( TWidth == 1 )
        {
            result=  word & 1;
            word>>= 1;
            if(++idx.bit == bitsof(TStorage))
            {
                idx.pos++;
                word= bb.GetWord(idx);
                idx.bit= 0;
            }
            return result;
        }

        static_assert(bitsof(TResult) >= TWidth, "Fixed size to read greater than given result type.");
        if constexpr ( TWidth == bitsof(TStorage)  )
            result= TResult( word );
        else
        {
            result= TResult( word  & lang::LowerMask<TWidth, TStorage>() );
            word>>= TWidth;
        }

        idx.bit+= TWidth;
        if(idx.bit >= bitsof(TStorage))
        {
            idx.pos++;
            word= bb.GetWord(idx);
            idx.bit-= bitsof(TStorage);
            if( idx.bit )
            {
                lang::ShiftOpRHS bitsRead= TWidth - idx.bit;
                if constexpr ( TWidth < bitsof(TStorage)  )
                    result |= TResult( (   word << bitsRead )
                                         & lang::LowerMask<TWidth, TStorage>() );
                else
                    result |= TResult(   word << bitsRead          );
            }

            word>>= idx.bit;
        }

        return result;
    }

    /// Reads the given number of bits from the stream into the given unsigned integral value.
    /// \note
    ///   Two different template functions (selected by keyword \c requires) for the different
    ///   integral types exist.<br>
    ///   This is version 2/2: Bits to read are more than the internal buffer width.
    /// \see
    ///   This method uses a template parameter for the number of bits to read.
    ///   A slightly slower, non-templated version is available with #Read<TResult>(ShiftOpRHS),
    ///   which is to be used when the number of bits to write is determined only at run-time.
    /// @tparam TWidth    The number of bits in \p{value} to write.
    /// @tparam TResult   The type of the value to return.
    /// @return The value read.
    template<lang::ShiftOpRHS TWidth, typename TResult= int>
    requires ( std::integral<TResult> &&  ( TWidth > bitsof(TStorage)) )
    TResult Read()
    {
        static_assert(bitsof(TResult) >= TWidth, "Fixed size to read greater than given result type.");
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(), "BITBUFFER", "BitBufferBase overflow" )

        TResult    result  = word;
        lang::ShiftOpRHS bitsRead= bitsof(TStorage) - idx.bit;
        do     // the loop is at least hit once and bit is 0! (but not written in bit)
        {
            idx.pos++;
            word= bb.GetWord(idx);
            result|= TResult(word) << bitsRead;
            bitsRead+= bitsof(TStorage);
        }
        while( bitsRead < TWidth);

        idx.bit= (idx.bit + TWidth) % bitsof(TStorage);

        // next buffer value reached?
        if(idx.bit == 0 )
            idx.pos++;
        else
            result= lang::LowerBits<TWidth, TResult>( result );

        word= bb.GetWord(idx) >> idx.bit;

        return result;
    }

    /// Reads the given number of bits from the stream into the given unsigned integral value.
    /// \note
    ///   Different template functions for different integral types exist and are
    ///   selected with keyword \c requires.
    ///
    /// \see
    ///   A method that uses a template parameter for the number of bits to read, is available
    ///   with #Read<TWidth,TResult>.
    ///   This might be slightly faster and should be used instead of this method, whenever
    ///   the number of bits to read is known at compilation time.
    ///
    /// @tparam TResult  The type of the value to return. Defaults to type \c int.
    /// @param  width    The number of bits to read.
    /// @return The value read.
    template<typename TResult= int>
    requires ( sizeof(TResult) <= sizeof(TStorage) )
    TResult Read( lang::ShiftOpRHS width )
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(),  "BITBUFFER", "BitBufferBase overflow" )
        ALIB_ASSERT_ERROR( bitsof(TResult) >= width, "BITBUFFER",
                                                     "Read size given greater than value type.")

        TResult     result;
        if ( width < bitsof(TStorage)  )
            result= TResult( word   & lang::LowerMask<TStorage>(width) );
        else
            result= TResult( word  );
        word>>= width;

        idx.bit+= width;
        if(idx.bit >= bitsof(TStorage))
        {
            idx.pos++;
            word= bb.GetWord(idx);
            idx.bit-= bitsof(TStorage);

            if( idx.bit )
            {
                lang::ShiftOpRHS bitsRead= width - idx.bit;
                result |= TResult( (   word << bitsRead )
                                     & lang::LowerMask<TStorage>(width) );
                word>>= idx.bit;
            }
        }

        return result;
    }

    /// Reads the given number of bits from the stream into the given unsigned integral value.
    /// \note
    ///   Different template functions for different integral types exist and are
    ///   selected with keyword \c requires.
    ///
    /// \see
    ///   A method that uses a template parameter for the number of bits to read, is available
    ///   with #Read<TWidth,TResult>.
    ///   This might be slightly faster and should be used instead of this method, whenever
    ///   the number of bits to read is known at compilation time.
    ///
    /// @tparam TResult  The type of the value to return. Defaults to type \c int.
    /// @param  width    The number of bits to read.
    /// @return The value read.
    template<typename TResult= int>
    requires ( sizeof(TResult) > sizeof(TStorage) )
    TResult Read( lang::ShiftOpRHS width )
    {
        ALIB_ASSERT_ERROR( idx.pos < bb.Capacity(),   "BITBUFFER", "BitBufferBase overflow" )
        ALIB_ASSERT_ERROR( bitsof(TResult) >= width , "BITBUFFER",
                                                      "Read size given greater than value type.")

        if( width <= bitsof(TStorage))
        {
            TResult     result;
            if ( width < bitsof(TStorage)  )
                result= TResult( word   & lang::LowerMask<TStorage>(width) );
            else
                result= TResult( word  );
            word>>= width;

            idx.bit+= width;
            if(idx.bit >= bitsof(TStorage))
            {
                idx.pos++;
                word= bb.GetWord(idx);
                idx.bit-= bitsof(TStorage);

                if( idx.bit )
                {
                    lang::ShiftOpRHS bitsRead= width - idx.bit;
                    result |= TResult( (   word << bitsRead )
                                         & lang::LowerMask<TStorage>(width) );
                    word>>= idx.bit;
                }
            }

            return result;
        }
        else
        {
            TResult     result  = TResult( word );
            lang::ShiftOpRHS  bitsRead= bitsof(TStorage) - idx.bit;
            do     // the loop is at least hit once and bit is 0! (but not written in bit)
            {
                idx.pos++;
                word= bb.GetWord(idx);
                result|= TResult(word) << bitsRead;
                bitsRead+= bitsof(TStorage);
            }
            while( bitsRead < width);

            idx.bit= (idx.bit + width) % bitsof(TStorage);

            // next buffer value reached?
            if(idx.bit == 0 )
            {
                idx.pos++;
                word= bb.GetWord(idx);
            }
            else
            {
                result= lang::LowerBits<TResult>( width, result );
                word>>= width;
            }
            return result;
        }
    }

    /// Reads the given integral value from the stream.
    /// Information about the encoding of the values is given with the documentation of
    /// \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.<br>
    /// This overload reads unsigned integral types of 8-bit width.
    /// @tparam TUIntegral The unsigned integral type to read.
    /// @return  The value read from the bit buffer.
    template< typename TUIntegral>
    requires( std::unsigned_integral<TUIntegral> && (bitsof(TUIntegral) == 8) )
    TUIntegral Read()        {   return readUIntegral8();  }

    /// Reads the given integral value from the stream.
    /// Information about the encoding of the values is given with the documentation of
    /// \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.<br>
    /// This overload reads unsigned integral types of 16-bit width.
    /// @tparam TUIntegral The unsigned integral type to read.
    /// @return  The value read from the bit buffer.
    template< typename TUIntegral>
    requires( std::unsigned_integral<TUIntegral> && (bitsof(TUIntegral) == 16) )
    TUIntegral Read()        {   return readUIntegral16();  }


    /// Reads the given integral value from the stream.
    /// Information about the encoding of the values is given with the documentation of
    /// \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.<br>
    /// This overload reads unsigned integral types of 32-bit width.
    /// @tparam TUIntegral The unsigned integral type to read.
    /// @return  The value read from the bit buffer.
    template< typename TUIntegral>
    requires( std::unsigned_integral<TUIntegral> && (bitsof(TUIntegral) == 32) )
    TUIntegral Read()        {   return readUIntegral32();  }


    /// Reads the given integral value from the stream.
    /// Information about the encoding of the values is given with the documentation of
    /// \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.<br>
    /// This overload reads unsigned integral types of 64-bit width.
    /// @tparam TUIntegral The unsigned integral type to read.
    /// @return  The value read from the bit buffer.
    template< typename TUIntegral>
    requires( std::unsigned_integral<TUIntegral> && (bitsof(TUIntegral) == 64) )
    TUIntegral  Read()        {   return readUIntegral64();  }


    /// Reads the given signed integral value from the stream.
    /// Information about the encoding of the values is given with the documentation of
    /// \alib{bitbuffer;BitWriter::Write<TIntegral>(TIntegral)}.<br>
    /// This overload reads signed integral types.
    /// @tparam TSIntegral The signed integral type to read.
    /// @return  The value read from the bit buffer.
    template< typename TSIntegral>
    requires( std::signed_integral<TSIntegral> )
    TSIntegral Read()
    {
        using TUnsigned= typename std::make_unsigned<TSIntegral>::type;
        TUnsigned result= Read<TUnsigned>();
        return    result & 1 ?  -TSIntegral( result >> 1 ) - 1
                             :   TSIntegral( result >> 1 );
    }

  protected:
    /// Internal method that reads a unsigned 8-bit value.
    /// @return The value read.
    ALIB_DLL uint8_t   readUIntegral8();

    /// Internal method that reads a unsigned 16-bit value.
    /// @return The value read.
    ALIB_DLL uint16_t  readUIntegral16();

    /// Internal method that reads a unsigned 32-bit value.
    /// @return The value read.
    ALIB_DLL uint32_t  readUIntegral32();

    /// Internal method that reads a unsigned 64-bit value.
    /// @return The value read.
    ALIB_DLL uint64_t  readUIntegral64();


}; // class BitReader

} // namespace alib[::bitbuffer]

/// Type alias in namespace \b alib.
using       BitBuffer       = bitbuffer::BitBuffer;

/// Type alias in namespace \b alib.
using       BitBufferMA     = bitbuffer::BitBufferMA;

/// Type alias in namespace \b alib.
template<uinteger TCapacity>
using       BitBufferLocal  = bitbuffer::BitBufferLocal<TCapacity>;

/// Type alias in namespace \b alib.
using       BitWriter       = bitbuffer::BitWriter;

/// Type alias in namespace \b alib.
using       BitReader       = bitbuffer::BitReader;

/// Type alias in namespace \b alib.
using       ShiftOpRHS      = int;

}  // namespace [alib]



//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace system {

/// A rather simple text file line-reader. While this is used with the class #"TTextFile",
/// it might well be used as a standalone helper, i.e. in cases where the text file is
/// read but does not need to be stored in a vector.
/// @tparam TLocalBufferSize The size of the local buffer. If lines are wider and the local
///                          buffer is exceeded, it will be replaced by an heap-allocated
///                          buffer, which is then reused for any further line.<br>
///                          Defaults to 1024.
template <size_t TLocalBufferSize= 1024>
struct TextFileLineReader {
    std::ifstream                   IFStream; ///< The input stream opened on construction.
    NLocalString<TLocalBufferSize>  Line;     ///< The line buffer.
    IStreamLineN                    ReadOp;   ///< An <em>AString-appendable</em> object
                                              ///< used for reading.
    std::errc                       Status;   ///< Set after construction. If value is \c 0,
                                              ///< the file was correctly opened.

  protected:
    /// Implementation of the two constructors.
    /// @param filePath   The path of the text-file to read.
    void construct( const CPathString& filePath )  {
        Line.DbgDisableBufferReplacementWarning();
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(filePath, nFilePath, 256)
        IFStream.open( nFilePath );
        Status= std::errc(errno);
        if ( !IFStream.is_open() ) {
            ALIB_WARNING( "FILETREE/TEXTFILE", "Error <{}: \"{}\"> opening input file \"{}\"",
                                            errno, std::errc(errno), filePath)
            return;
        }
        ALIB_MESSAGE( "FILETREE/TEXTFILE", "file \"{}\" opened for reading", filePath)
    }
  public:

    /// Constructor. Opens the file specified by \p{filePath}.
    /// On success, the field #"Status" will hold std::errc(0), an error code otherwise.
    /// @param filePath   The path of the text-file to read.
    TextFileLineReader( const CPathString& filePath )
    : ReadOp{&IFStream}
    , Status{0}                                                             { construct(filePath); }

    /// Reads the next text-line into the field #".Line" and returns a #"%^Substring" pointing to it.
    /// When the end of the file is reached, the returned object is \e nulled.\b
    /// Prior to the invocation, method #"TIStreamLine;IsEOF" may be called to detect the end of
    /// the file actively.
    /// @return The next line read, or #"NULL_STRING" when all lines were read.
    NSubstring NextLine()  {
        Line.Reset( ReadOp );
        if (ReadOp.IsEOF && Line.IsEmpty())
            return NULL_NSTRING;
        return Line;
    }
};


/// Deduction guide fixing the template parameter #\p{TLocalBufferSize} to its default \c 1024.
TextFileLineReader(const CPathString&) -> TextFileLineReader<1024>;

#if !DOXYGEN
#   if ALIB_MONOMEM
#       define TEXTFILE_DEFAULT_ALLOCATOR MonoAllocator
#   else
#       define TEXTFILE_DEFAULT_ALLOCATOR HeapAllocator
#   endif
#endif


/// A rather simple text file reader and writer.
/// @see Reading is performed using the helper type #"TextFileLineReader",
///      which can be used as a standalone helper, i.e. in cases where a text file is
///      read but does not need to be stored in a vector.
/// @tparam TNString    The string-type.
///                     This may also be a type derived from #"NString;2"
///                     which contains further fields available with each line of the file.
/// @tparam TAllocator  The #"lang::Allocator;allocator type" to use. In the presence of
///                     module \alib_monomem, this defaults to #"MonoAllocator", and to
///                     #"HeapAllocator" otherwise.
/// @tparam TLocalBufferSize The size of the local buffer. If lines are wider and the local
///                          buffer is exceeded, it will be replaced by an heap-allocated
///                          buffer, which is then reused for any further line.<br>
///                          Defaults to 1024.
template <typename TNString= NString, typename TAllocator= TEXTFILE_DEFAULT_ALLOCATOR,
                   size_t TLocalBufferSize= 1024>
class TTextFile : public lang::AllocatorMember<TAllocator>
                , public std::vector<TNString, lang::StdAllocator<TNString, TEXTFILE_DEFAULT_ALLOCATOR>> { // StdVectorMA<TNString>
  protected:
    /// The allocator member base-type.
    using base= lang::AllocatorMember<TAllocator>;
  public:
    /// Type definition publishing template parameter \p{TAllocator}.
    using AllocatorType     = TAllocator;

    /// Type definition publishing the type in the <c>std::vector</c> that this type is derived
    /// of.
    /// (As is defined with template parameter \p{TNString}.)
    using StoredType        = TNString;

    /// Type definition publishing the base container type.
    using Vector= std::vector<TNString, lang::StdAllocator<TNString, TEXTFILE_DEFAULT_ALLOCATOR>>;

    /// Constructor.
    /// @param ma The allocator to use.
    TTextFile(AllocatorType& ma)
    : lang::AllocatorMember<TAllocator>(ma)
    , Vector(ma)                                                                                  {}

    /// Returns the vector's size as \alib's signed integral type.
    /// @return The number of lines in this source file.
    constexpr integer       Size()                    const noexcept { return int(Vector::size()); }

    /// Returns the element in the vector at the given position.<br>
    /// Note that this is the same as calling inherited <c>std::vector::at()</c>, but this method
    /// accepts any integral type for parameter \p{idx}.<br>
    /// Furthermore, this method raises an \alib_assertion in debug-builds, instead of throwing
    /// an exception.
    /// @tparam TIntegral The integral type that the index is provided by.
    /// @param  idx The index to retrieve an element for.
    /// @return A reference to the element stored at the given \p{idx}.
    template<typename TIntegral>
    [[nodiscard]]
    constexpr StoredType&   At(TIntegral idx)           noexcept { return Vector::at(size_t(idx)); }

    /// <c>const</c>-version of #"At".
    /// @tparam TIntegral The integral type that the index is provided by.
    /// @param  idx The index to retrieve an element for.
    /// @return A \c const reference to the element stored at the given \p{idx}.
    template<typename TIntegral>
    [[nodiscard]]
    constexpr const StoredType& At(TIntegral idx) const noexcept { return Vector::at(size_t(idx)); }

    /// Reads the file into this vector of lines.
    /// @param filePath             The path of the file.
    /// @return <c>std::errc(0)</c> if all went well, otherwise an error code.
    std::errc  Read(const CPathString& filePath) {
        TextFileLineReader<TLocalBufferSize> reader(filePath);
        if ( reader.Status != std::errc(0) )
            return reader.Status;

        NSubstring line;
        while ( (line= reader.NextLine()).IsNotNull() ) {
            if constexpr (std::is_same_v<StoredType, TNString>)
                Vector::emplace_back( base::GetAllocator(), line );
            else {
                strings::TLocalString<typename StoredType::value_type, 4096> lineStr;
                lineStr._(line);
                Vector::emplace_back( base::GetAllocator(), lineStr );
        }   }

        ALIB_MESSAGE( "FILETREE/TEXTFILE", "File \"{}\", {} lines read", filePath, Vector::size() )

        return std::errc(0);
    }

    /// Writes this text file to the given \p{filePath}.
    /// @param filePath     The path of the file.
    /// @return <c>std::errc(0)</c> if all went well, otherwise an error code.
    ALIB_DLL
    std::errc    Write(const PathString& filePath) {
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(tFilePath, nTFilePath, 256)
        std::ofstream oFile( nTFilePath.Terminate() );
        if ( !oFile.is_open() ) {
            auto result= std::errc(errno);
            ALIB_WARNING( "FILETREE/TEXTFILE", "Error <{}: \"{}\"> opening output file \"{}\"",
                                            errno, result, filePath)
            return result;
        }
        ALIB_MESSAGE( "FILETREE/TEXTFILE", "file \"{}\" opened for writing", filePath)

        for( auto& line : *this )
            oFile << line << std::endl;

        ALIB_MESSAGE( "FILETREE/TEXTFILE",  "File \"{}\", {} lines written", filePath, Vector::size() )

        return std::errc(0);
    }

};

} // namespace alib[::system]

/// Type alias in namespace #"%alib".
/// @tparam TLocalBufferSize The size of the local buffer. If lines are wider and the local
///                          buffer is exceeded, it will be replaced by a heap-allocated
///                          buffer, which is then reused for any further line.<br>
///                          Defaults to 1024.
template <size_t TLocalBufferSize= 1024>
using     TextFileLineReader =   system::TextFileLineReader<TLocalBufferSize>;

/// Type alias in namespace #"%alib".
/// @tparam TLocalBufferSize The size of the local buffer. If lines are wider and the local
///                          buffer is exceeded, it will be replaced by a heap-allocated
///                          buffer, which is then reused for any further line.<br>
///                          Defaults to 1024.
template <size_t TLocalBufferSize= 1024>
using     TextFile           =   system::TTextFile<NString, TEXTFILE_DEFAULT_ALLOCATOR, TLocalBufferSize>;

#if !DOXYGEN
#   undef TEXTFILE_DEFAULT_ALLOCATOR
#endif


} // namespace [alib]

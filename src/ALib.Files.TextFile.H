//==================================================================================================
/// \file
/// This header-file is part of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef H_ALIB_FILES_TEXTFILE
#define H_ALIB_FILES_TEXTFILE
#pragma once
#ifndef INL_ALIB
#   include "alib/alib.inl"
#endif

#if ALIB_FILES
#include <fstream>
#include "ALib.Files.H"

namespace alib { namespace files {

/// A rather simple text file line-reader. While this is used with class \alib{files;TTextFile},
/// it might well be used as a standalone helper, i.e. in cases where the text file is
/// read but does not need to be stored in a vector.
struct TextFileLineReader {
    std::ifstream        IFStream; ///< The input stream opened on construction.
    NString4K            Line;     ///< The line buffer.
    alib::IStreamLine    ReadOp;   ///< An <em>AString-appendable</em> object used for reading.
    system::SystemErrors Status;   ///< Set after construction. If \b SystemErrors::OK, the file
                                   ///< was correctly opened.

  protected:
    /// Implementation of the two constructors.
    /// @param filePath   The path of the text-file to read.
    void construct( const CString& filePath )  {
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(filePath, nFilePath, 256)
        IFStream.open( nFilePath );
        if ( !IFStream.is_open() ) {
            ALIB_WARNING( "FILES/TEXTFILE", "Error <{}: \"{}\"> opening input file \"{}\"",
                                            errno, system::SystemErrors(errno), filePath)
            return;
        }
        ALIB_MESSAGE( "FILES/TEXTFILE", "file \"{}\" opened for reading", filePath)
    }
  public:

    /// Constructor. Opens the file specified by \p{filePath}.
    /// On success, the field #Status will hold \alib{system;SystemErrors;SystemErrors::OK},
    /// an error code otherwise.
    /// @param filePath   The path of the text-file to read.
    TextFileLineReader( const CString& filePath )
    : ReadOp{&IFStream}
    , Status{SystemErrors::OK}                                              { construct(filePath); }

    /// Alternative constructor taking a \alib{files;File} object instead of a file's path string.
    /// @param file  The text-file to read.
    TextFileLineReader(files::File file)
    : ReadOp{&IFStream}
    , Status{SystemErrors::OK}    {
        Path filePath;
        file.AsCursor().AssemblePath(filePath);
        construct(filePath);
    }

    /// Reads the next text-line into the field #Line and returns a \b Substring pointing to it.
    /// When the end of the file is reached, the returned object is \e nulled.\b
    /// Prior to the invocation, method \alib{strings::compatibility::std::TIStreamLine;IsEOF}
    /// may be called to detect the end of the file actively.
    /// @return The next line read, or \alib{NULL_STRING} when all lines were read.
    Substring NextLine()  {
        Line.Reset( ReadOp );
        if (ReadOp.IsEOF && Line.IsEmpty())
            return NULL_STRING;
        return Line;
    }
};

/// A rather simple text file reader and writer.
/// @see Reading is performed using helper type class \alib{files;TextFileLineReader},
///      which be used as a standalone helper, i.e. in cases where a text file is
///      read but does not need to be stored in a vector.
/// @tparam TNString    The string-type.
///                     This may also be a type derived from \alib{NString}
///                     which contains further fields available with each line of the file.
/// @tparam TAllocator  The \alib{lang;Allocator;allocator type} to use.
template <typename TNString= NString, typename TAllocator= MonoAllocator>
class TTextFile : public lang::AllocatorMember<TAllocator>
                , public StdVectorMA<TNString>
{
  protected:
    /// The given allocator
    MonoAllocator&              allocator;

  public:
    /// Type definition publishing template parameter \p{TAllocator}.
    using AllocatorType     = TAllocator;

    /// Type definition publishing the type in the <c>std::vector</c> that this type is derived
    /// of.
    /// (As is defined with template parameter \p{TNString}.)
    using StoredType        = TNString;

    /// Type definition publishing the base container type.
    using Vector= StdVectorMA<TNString>;

    /// Constructor.
    /// @param ma The allocator to use.
    TTextFile(MonoAllocator& ma)
    : lang::AllocatorMember<TAllocator>(ma)
    , Vector(ma)
    , allocator( ma )                                                                             {}

    /// Returns the vector's size as \alib's signed integral type.
    /// @return The number of lines in this source file.
    constexpr integer Size()                          const noexcept { return int(Vector::size()); }

    /// Returns the element in the vector at the given position.<br>
    /// Note that this is the same as calling inherited <c>std::vector::at()</c>, but this method
    /// accepts any integral type for parameter \p{idx}.<br>
    /// Furthermore, this method raises an \alib_assertion in debug-builds, instead of throwing
    /// an exception.
    /// @tparam TIntegral The integral type that the index is provided by.
    /// @param  idx The index to retrieve an element for.
    /// @return A reference to the element stored at the given \p{idx}.
    template<typename TIntegral>
    [[__nodiscard__]]
    constexpr StoredType& At(TIntegral idx)             noexcept { return Vector::at(size_t(idx)); }

    /// <c>const</c>-version of #At.
    /// @tparam TIntegral The integral type that the index is provided by.
    /// @param  idx The index to retrieve an element for.
    /// @return A \c const reference to the element stored at the given \p{idx}.
    template<typename TIntegral>
    [[__nodiscard__]]
    constexpr const StoredType& At(TIntegral idx) const noexcept { return Vector::at(size_t(idx)); }

    /// Reads the file into this vector of lines.
    /// @param filePath             The path of the file.
    /// @return \alib{system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///         error code.
    system::SystemErrors  Read(const CString& filePath) {
        TextFileLineReader reader(filePath);
        if ( reader.Status != system::SystemErrors::OK )
            return reader.Status;

        Substring line;
        while ( (line= reader.NextLine()).IsNotNull() )
            Vector::emplace_back( allocator, line );

        ALIB_MESSAGE( "FILES/TEXTFILE", "File \"{}\", {} lines read", filePath, Vector::size() )

        return system::SystemErrors::OK;
    }

    /// Reads the file into this vector of lines.
    /// @param file  The file to read.
    /// @return \alib{system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///          error code.
    system::SystemErrors  Read(files::File file) {
        Path filePath;
        file.AsCursor().AssemblePath(filePath);
        return Read(filePath);
    }

    /// Writes this text file to the given \p{filePath}.
    /// @param filePath     The path of the file.
    /// @return \alib{system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///          error code.
    ALIB_DLL
    system::SystemErrors  Write(const String& filePath) {
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(tFilePath, nTFilePath, 256)
        std::ofstream oFile( nTFilePath.Terminate() );
        if ( !oFile.is_open() ) {
            auto result= system::SystemErrors(errno);
            ALIB_WARNING( "FILES/TEXTFILE", "Error <{}: \"{}\"> opening output file \"{}\"",
                                            errno, result, filePath)
            return result;
        }
        ALIB_MESSAGE( "FILES/TEXTFILE", "file \"{}\" opened for writing", filePath)

        for( auto& line : *this )
            oFile << line << std::endl;

        ALIB_MESSAGE( "FILES/TEXTFILE",  "File \"{}\", {} lines written", filePath, Vector::size() )

        return system::SystemErrors::OK;
    }

};

} // namespace alib[::files]

/// Type alias in namespace \b alib.
using     TextFileLineReader =   files::TextFileLineReader;

/// Type alias in namespace \b alib.
using     TextFile           =   files::TTextFile<NString>;


} // namespace [alib]

#endif // ALIB_FILES

#endif // H_ALIB_FILES_TEXTFILE

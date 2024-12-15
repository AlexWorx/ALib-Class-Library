//==================================================================================================
/// \file
/// This header file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_FILES_TEXTFILE
#define HPP_ALIB_FILES_TEXTFILE 1
#pragma once

#include "alib/files/ftree.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/monomem/aliases/stdvector.hpp"
#include <fstream>

namespace alib { namespace files {

/// A rather simple text file reader and writer.
/// @tparam TNString    The string-type.
///                     This may also be a type derived from \alib{NString}
///                     which contains further fields available with each line of the file.
/// @tparam TAllocator  The allocator type to use, as prototyped with \alib{lang;Allocator}.
template <typename TNString= NString, typename TAllocator= MonoAllocator>
class TTextFile : public lang::AllocatorMember<TAllocator>
                , public StdVectorMono<TNString>
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
    using Vector= StdVectorMono<TNString>;

    /// Constructor.
    /// @param ma The allocator to use.
    
    TTextFile(MonoAllocator& ma)
    : lang::AllocatorMember<TAllocator>(ma)
    , Vector(ma)
    , allocator( ma ) {}

    /// Reads the file into this vector of lines.
    /// @param filePath             The path of the file.
    /// @return \alib{lang::system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///         error code.
    lang::system::SystemErrors  Read(const CString& filePath)
    {
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(filePath, nFilePath, 256)
        std::ifstream  iFile( nFilePath );
        if ( !iFile.is_open() )
        {
            auto result= lang::system::SystemErrors(errno);
            ALIB_WARNING( "FILES/TEXTFILE", "Error <{}: {!Q}> opening input file {!Q}", errno, result, filePath)
            return result;
        }
        ALIB_MESSAGE( "FILES/TEXTFILE", "file {!Q} opened for reading", filePath)

        alib::ISReadLineN readOp( &iFile );
        NString4K line;                 ALIB_DBG(  line.DbgDisableBufferReplacementWarning();  )
        while( !readOp.IsEOF )
        {
            line.Reset( readOp );
            Vector::emplace_back( allocator, line );
        }

        ALIB_MESSAGE( "FILES/TEXTFILE", "File {!Q}, {} lines read", filePath, Vector::size() )

        return lang::system::SystemErrors::OK;
    }

    /// Reads the file into this vector of lines.
    ///  @param file  The file to read.
    ///  @return \alib{lang::system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///          error code.
    lang::system::SystemErrors  Read(files::File file)
    {
        Path filePath;
        file.AsCursor().AssemblePath(filePath);
        return Read(filePath);
    }

    /// Writes this text file to the given \p{filePath}.
    ///  @param filePath     The path of the file.
    ///  @return \alib{lang::system;SystemErrors;SystemErrors::OK} if all went well, otherwise an
    ///          error code.
    ALIB_API
    lang::system::SystemErrors  Write(const String& filePath)
    {
        errno= 0;
        Path tFilePath(filePath);
        ALIB_STRINGS_TO_NARROW(tFilePath, nTFilePath, 256)
        std::ofstream oFile( nTFilePath.Terminate() );
        if ( !oFile.is_open() )
        {
            auto result= lang::system::SystemErrors(errno);
            ALIB_WARNING( "FILES/TEXTFILE", "Error <{}: {!Q}> opening output file {!Q}", errno, result, filePath)
            return result;
        }
        ALIB_MESSAGE( "FILES/TEXTFILE", "file {!Q} opened for writing", filePath)

        for( auto& line : *this )
            oFile << line << std::endl;

        ALIB_MESSAGE( "FILES/TEXTFILE",  "File {!Q}, {} lines written", filePath, Vector::size() )

        return lang::system::SystemErrors::OK;
    }

};

} // namespace alib[::files]

/// Type alias in namespace \b alib.
using     TextFile          =   files::TTextFile<NString>;

} // namespace [alib]


#endif // HPP_ALIB_FILES_TEXTFILE


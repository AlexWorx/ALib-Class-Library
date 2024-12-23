//==================================================================================================
/// \file
/// This header file is part of the \aliblong.<br>
/// With the inclusion of this header compatibility features between \alib and the C++ standard
/// library are provided.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM
#define HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM 1
#pragma once
#if !defined(DOXYGEN)
#   include "alib/alib.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)

#include "alib/compatibility/std_strings.hpp"
#include <iostream>
#include "alib/strings/localstring.hpp"

#include "alib/lang/callerinfo_functions.hpp"

namespace alib {  namespace strings { namespace compatibility { namespace std {

//==================================================================================================
/// This template type may be specialized to suppress ambiguities for types \p{T} which
/// - have <c>std::operator<<(ostream, const T&)</c> defined, \b and
/// - are \ref alib_strings_assembly_ttostring "appendable" to \alib strings.
///
/// \note
///    The ambiguity occurs due to the definition of <c>std::operator<<</c> for all appendable
///    types.
///
/// If a specialization of this template struct exists that inherits <c>std::true_type</c>,
/// the compiler will not choose the \alib implementation of the operator, which resolves the
/// ambiguity.
///
/// \see
///   Specialization might be done with macro
///   \ref ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR.
///
/// @tparam T The appendable type to suppress
//==================================================================================================
template<typename T>  struct T_SuppressStdOstreamOperator  :  ::std::false_type {};

#define ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(TYPE)                                           \
    namespace alib::strings::compatibility::std {                                                  \
    template<> struct T_SuppressStdOstreamOperator<TYPE>  : ::std::true_type {};      }


//==================================================================================================
/// Parameter class used to append to objects of type \alib{strings;TAString;AString}, which
/// invokes the method of the according specialization of template struct
/// <b>T_Append<ISReadLine,TChar,HeapAllocator></b>.<br>
/// This then reads a line of text from the encapsulated \b std::istream and appends that line to the
/// target \b %AString.
///
/// While, of course, this class can be created 'inline' (for example, similar objects of parameter
/// classes found in \alib{strings;TFormat;Format}), in the usual case that a series of lines are to
/// be read from a \b std::istream, a local object of this type might be created. In case of a
/// reading loop, rather outside such loop.
///
/// Field #IsEOF can be used to detect the end of the input stream.
///
/// \see
/// - \alib{strings;compatibility::std::operator>>(std::istream&;NAString& string)}  and
///   \alib{strings;compatibility::std::operator<<(std::ostream&;const NString&)}.
/// - For a sample, refer to source code of \alib class \b %IniFile, method
///   \alib{config;IniFile::Read}.
///
///
/// @tparam TChar  The character type of the input stream as well as the receiving string.<br>
///                Specializations for character types \alib{characters;nchar},
///                \alib{characters;wchar} exist. Those have corresponding alias type definition
///                shortcuts \alib{ISReadLineN} and \alib{ISReadLineW} in namespace #alib.
//==================================================================================================
template<typename TChar>
struct TISReadLine
{
    /// The input stream to read from.
    ::std::basic_istream<TChar>*    IStream;

    /// If \c CurrentData::KEEP, the target \c %AString is not cleared before the read operation.
    lang::CurrentData               TargetData;

    /// The amount of characters that the buffer is increased while reading parts of the line.
    integer                         BufferSize;

    /// The maximum length of a single line to be read. Longer lines get truncated.
    integer                         MaxLineWidth;

    /// Indicates if the end of the input stream was detected with the last read operation.
    /// If so, a next read operation will not change the string (or clear it, if #TargetData is
    /// \c false
    bool                            IsEOF                                                   = false;

    //==============================================================================================
    /// Constructor.
    ///
    /// @param istream       The input stream to read from.
    /// @param targetData    If \c CurrentData::Keep, the target \c %AString is not cleared
    ///                      before the read operation is performed.
    ///                      Defaults to \c CurrentData::Clear.
    /// @param bufferSize    The amount of characters that the buffer is increased while reading
    ///                      parts of the line. Defaults to 256 characters.
    /// @param maxLineWidth  The maximum length of a single line to be read. Longer lines
    ///                      get truncated. Defaults to 4096 characters.
    //==============================================================================================
    TISReadLine( ::std::basic_istream<TChar>*   istream,
                 lang::CurrentData              targetData   = lang::CurrentData::Clear,
                 integer                        bufferSize   = 256,
                 integer                        maxLineWidth = 4096                      )
    : IStream     (istream),
      TargetData  (targetData),
      BufferSize  (bufferSize),
      MaxLineWidth(maxLineWidth)
    {}
};


}}  // namespace alib::strings[::compatibility::std]

#if DOXYGEN
   namespace APPENDABLES {
#endif

/// Specialization of type-traits struct \alib{strings;T_Append} for type
/// \alib{strings::compatibility::std;TISReadLine}.
/// @tparam TChar The <b>AString</b>'s \ref alib_characters_chars "character type".
template<typename TChar>  struct T_Append<compatibility::std::TISReadLine<TChar>, TChar, lang::HeapAllocator>
{
    //==============================================================================================
    /// Reads a line from a text file and appends the contents to \p{target}.
    /// If the end of the input stream was reached, field
    /// \alib{strings::compatibility::std::TISReadLine;IsEOF} of parameter \p{reader} will be set
    /// to \c true, what indicates that a next read operation would fail if it was performed.
    ///
    /// \note
    ///   For setting field <em>IsEOF</em> the object will be cast to a non-constant reference.
    ///   See functor \alib{strings;T_Append} for an explanation why it is OK to do so.
    ///
    /// @param  target     The AString object to read into.
    /// @param  reader     The object holding the \b std::istream and some parameters.
    //==============================================================================================
    void operator()( TAString<TChar, lang::HeapAllocator>&                     target,
                     const compatibility::std::TISReadLine<TChar>&   reader  );
};

#if DOXYGEN
} namespace alib::strings[::APPENDABLES]
#endif
namespace compatibility { namespace std {

//==================================================================================================
/// This class is a simple helper-class that converts strings of the
/// \alib{characters;character;default character type} to narrow strings as expected by
/// \c std::ostream objects.
///
/// The basic goal of this class is to avoid preprocessor directives for code selection when
/// the default string-type of \alib uses wide characters.
///
/// For example, to write string data into a file, the following approach is advised:
///
/// 1. Create an output stream of (derived) type \c std::ostream.
/// 2. Create an object of this class and pass the output stream via #SetStream.
/// 3. Write string data stored in objects of platform-dependent type alias \ref alib::AString
///    by passing those to method #Write.
///
/// Within step 3, the compiler chooses the right overloaded version of the method #Write.
/// Hence, the potentially necessary conversion of the string data is done transparently and only
/// if needed.
///
/// \see Class \alib{strings::compatibility::std;StringReader}.
//==================================================================================================
class StringWriter
{
    protected:
        /// The string buffer used for conversion.
        NAString                converter;

    public:
        /// The output stream as provided with #SetStream. Will be set to the \c std::cout,
        /// respectively \c std::wcout in the constructor.
        ::std::ostream*         ostream;

    public:
        //==========================================================================================
        /// Constructor.
        /// Invokes #SetStream passing \c std::cout.
        //==========================================================================================
                        StringWriter()
        : ostream( &::std::cout )
        {}


        //==========================================================================================
        /// Sets the output stream.
        ///
        /// @param os  Pointer to the output stream to write to.
        //==========================================================================================
        void            SetStream( ::std::ostream * os)                             { ostream= os; }

        //==========================================================================================
        /// Returns the output stream previously set with #SetStream.
        ///
        /// @return  The output stream set with #SetStream.
        //==========================================================================================
        ::std::ostream* GetStream()                                              { return ostream; }

        //==========================================================================================
        /// Write the given narrow string to the stream and return the length of the string given
        /// if it was converted to wide characters.
        ///
        /// \note
        ///  The return value is the length of the given string if it was converted to wide string,
        ///  even though in this overloaded version it is unnecessary to be converted.
        ///  This is useful to determine the "real" output width when an output text is formatted.
        ///  Of course, in many locales, this is still not the real output width because even
        ///  uni-code characters are not guaranteed to represent exactly one printable character.
        ///  But still, this value is already a much better approximation than the length of the
        ///  given narrow string.
        ///
        /// @param src  The string to write.
        /// @return Returns the length of the given string as wide string.
        //==========================================================================================
        ALIB_API
        integer         WriteAndGetWideLength( const NString& src );

        //==========================================================================================
        /// Write the given wide string to the stream and returns the length of the string given
        /// @param src  The string to write.
        /// @return Returns the length of the given wide string.
        //==========================================================================================
        ALIB_API
        integer         WriteAndGetWideLength( const WString& src );

        //==========================================================================================
        /// Write the given narrow string to the stream.
        /// @param src  The string to write.
        //==========================================================================================
        ALIB_API
        void            Write( const NString& src );

        //==========================================================================================
        /// Write the given narrow character repeatedly to the stream.
        /// @param fillChar The character to write.
        /// @param count    The number of characters to write.
        //==========================================================================================
        ALIB_API
        void            WriteChars( const nchar fillChar, integer count );

        //==========================================================================================
        /// Write the given wide string to the stream.
        /// @param src  The string to write.
        //==========================================================================================
        ALIB_API
        void            Write( const WString& src );
};

//==================================================================================================
/// This class is a helper-class that converts narrow string data read from an object of
/// type \c std::istream to the \alib{characters;character;default character type}.
///
/// \see Further explanations are given with sibling class
///      \alib{strings::compatibility::std;StringWriter}.
//==================================================================================================
class StringReader
{
    protected:
        /// The string buffer used for conversion.
        NAString                                    converter;

        /// The input stream as provided with #SetStream. Will be set to the \c std::cin,
        /// respectively \c std::win in the constructor.
        compatibility::std::TISReadLine<nchar>      readOp;


    public:
        //==========================================================================================
        /// Constructor.Invokes #SetStream passing \c std::cin.
        //==========================================================================================
                            StringReader()
        : readOp( &::std::cin )
        {}


        //==========================================================================================
        /// Sets the input stream.
        ///
        /// @param is  Pointer to the input stream to read from to.
        //==========================================================================================
        void                SetStream( ::std::istream* is )                   { readOp.IStream= is;}

        //==========================================================================================
        /// Returns the input stream previously set with #SetStream.
        ///
        /// @return  The input stream set with #SetStream.
        //==========================================================================================
        ::std::istream*     GetStream()                                   { return readOp.IStream; }

        //==========================================================================================
        /// Returns \c true if the input stream signaled its end, \c false otherwise.
        ///
        /// @return \c true if the input stream is known to be at its end, \c false otherwise.
        //==========================================================================================
        bool                IsEOF()                                         { return readOp.IsEOF; }


        //==========================================================================================
        /// Reads one line of text from the input stream into a narrow string.
        ///
        /// @param target  The storage buffer for the string to read. This string will be cleared
        ///                independent of the availability of input data.
        //==========================================================================================
        void                Read( NAString& target )
        {
            target.Reset( readOp );
        }

        //==========================================================================================
        /// Reads one line of text from the internal input stream into a wide string.
        ///
        /// @param target  The storage buffer for the string to read. This string will be cleared
        ///                independent of the availability of input data.
        //==========================================================================================
        void                Read( WAString& target )
        {
            target.Reset();
            converter.Reset( readOp );
            target << converter;
        }
};

}}} // namespace alib[::strings::compatibility::std]

/// Type alias in namespace \b alib.
using  ISReadLine     =   strings::compatibility::std::TISReadLine<character>;

/// Type alias in namespace \b alib.
using  ISReadLineN    =   strings::compatibility::std::TISReadLine<char>;

/// Type alias in namespace \b alib.
using  ISReadLineW    =   strings::compatibility::std::TISReadLine<wchar_t>;

/// Type alias in namespace \b alib.
using  StringWriter   =   strings::compatibility::std::StringWriter;

/// Type alias in namespace \b alib.
using  StringReader   =   strings::compatibility::std::StringReader;


// #################################################################################################
// ####################           std::ostream& operator<<           ###############################
// #################################################################################################
#if DOXYGEN
 namespace strings { namespace compatibility { namespace std {
#else
} // namespace [alib]
#endif

//==================================================================================================
/// Copies the contents of the given \b %NString to into the \c std::ostream given as reference.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::ostream& operator<<( std::ostream& stream, const alib::NString& string )
{
    if ( string.IsNotEmpty() )
        stream.write( string.Buffer(), string.Length() );
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \b %NString to into the \c std::ostream given as pointer.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::ostream* operator<<( std::ostream* stream, const alib::NString& string )
{
    stream->write( string.Buffer(), string.Length() );
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \b %WString to into the \c std::ostream given as reference.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
ALIB_API std::ostream& operator<<( std::ostream& stream, const alib::WString& string );

//==================================================================================================
/// Copies the contents of the given \b %WString to into the \c std::ostream given as pointer.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline   std::ostream* operator<<( std::ostream* stream, const alib::WString& string )
{
    (*stream) << string;
    return  stream;
}

//==================================================================================================
/// Copies the contents of the given \b %NString to into the \c std::wostream given as reference.
///
/// \note
///   This operator uses a local string buffer of 256 bytes size to convert the given narrow string
///   to an string of \c wchar_t characters that the output stream accepts. In case that
///   the given \p{string} is larger, a dynamic memory allocation has to be made.<br>
///   In performance-critical code that writes larger string data, a custom conversion method,
///   that for example reuses a buffer, may be appropriate.
///
/// <p>
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
ALIB_API std::wostream& operator<<( std::wostream& stream, const alib::NString& string );

//==================================================================================================
/// Copies the contents of the given \b %NString to into the \c std::wostream given as pointer.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// \see   The notes on memory efficiency, documented with operator
///        \alib{strings::compatibility::std;operator<<(std::wostream&,const NString&)}
///        which this operator uses inline.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline   std::wostream* operator<<( std::wostream* stream, const alib::NString& string )
{
    (*stream) << string;
    return  stream;
}

//==================================================================================================
/// Copies the contents of the given \b %WString to into the \c std::wostream given as reference.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::wostream& operator<<( std::wostream& stream, const alib::WString& string )
{
    if ( string.IsNotEmpty() )
    {
        #if ALIB_CHARACTERS_NATIVE_WCHAR
            stream.write( string.Buffer(), string.Length() );
        #else
            alib::XLocalString<1024> converter( string );
            converter.DbgDisableBufferReplacementWarning();
            stream.write( converter.Buffer(), converter.Length() );
        #endif
    }
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \b %WString to into the \c std::wostream given as pointer.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The ostream object to write the given  String into.
/// @param  string The String to write into the given ostream.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::wostream* operator<<( std::wostream* stream, const alib::WString& string )
{
    (*stream) << string;
    return stream;
}

//==================================================================================================
/// Clears the given \b %NAString and extracts data from the std::istream into it. The extractions
/// ends with either the end of the std::istream or when reading a newline character.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The istream object to extract data from.
/// @param  string The AString to receive data.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::istream& operator>>( std::istream& stream, alib::NAString& string )
{
    string << alib::strings::compatibility::std::TISReadLine<alib::nchar>( &stream,
                                                                   alib::lang::CurrentData::Clear );
    return stream;
}

//==================================================================================================
/// Clears the given \b %NAString and extracts data from the std::istream into it. The extractions
/// ends with either the end of the std::istream or when reading a newline character.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The istream object to extract data from.
/// @param  string The AString to receive data.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::istream* operator>>( std::istream* stream, alib::NAString& string )
{
    ALIB_ASSERT_WARNING( stream != nullptr, "STRINGS", "Given std::IStream is nullptr" )

    if (stream != nullptr)
        string << alib::strings::compatibility::std::TISReadLine<alib::nchar>( stream,
                                                                   alib::lang::CurrentData::Clear );
    return stream;
}

//==================================================================================================
/// Clears the given \b %WAString and extracts data from the std::istream into it. The extractions
/// ends with either the end of the std::istream or when reading a newline character.
///
/// \note
///   If code selection symbol \ref ALIB_CHARACTERS_NATIVE_WCHAR evaluates to false, a local buffer
///   is used to convert the string of \c wchar_t characters that the input stream provides.
///   In case that the string read from the stream is larger, a dynamic memory allocation has to
///   be made.<br>
///   In performance-critical code that receives larger string data, a custom conversion method
///   that, for example, reuses a buffer may be appropriate.
///
/// <p>
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
/// @param  stream The istream object to extract data from.
/// @param  string The AString to receive data.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::basic_istream<wchar_t>& operator>>( std::basic_istream<wchar_t>& stream,
                                                alib::WAString&              string )
{
    #if ALIB_CHARACTERS_NATIVE_WCHAR
        string << alib::strings::compatibility::std::TISReadLine<wchar_t>( &stream,
                                                                   alib::lang::CurrentData::Clear );
    #else
        alib::XLocalString<1024> converter;
        converter.DbgDisableBufferReplacementWarning();
        converter << alib::strings::compatibility::std::TISReadLine<wchar_t>( &stream,
                                                                   alib::lang::CurrentData::Keep );
        string.Reset( converter );
    #endif
    return stream;
}

//==================================================================================================
/// Clears the given \b %WAString and extracts data from the std::istream into it. The extractions
/// ends with either the end of the std::istream or when reading a newline character.
///
/// \note  Unlike this documentation indicates, the operator is defined in the global namespace.
///
/// \see   The notes on memory efficiency, documented with operator
///        \alib{strings::compatibility::std;operator>>(std::basic_istream<wchar_t>&, WAString& )}
///        which this operator uses inline.
/// @param  stream The istream object to extract data from.
/// @param  string The AString to receive data.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
inline std::basic_istream<wchar_t>* operator>>( std::basic_istream<wchar_t>* stream,
                                                alib::WAString& string )
{
    ALIB_ASSERT_WARNING ( stream != nullptr, "STRINGS", "Given std::istream is nullptr" )

    if (stream != nullptr)
        (*stream) >> string;
    return stream;
}


//==================================================================================================
/// Copies the contents of the given \ref alib_strings_assembly_ttostring "appendable type"
/// the \c std::ostream given as reference.
///
/// \note Unlike this documentation indicates, this operator is defined in the global namespace.
///
/// @tparam TAppendable The appendable type.
/// @param  stream      The \c std::ostream object to write the given  String into.
/// @param  appendable  The object whose contents is to be written into the given \p{stream}.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
template<typename TAppendable,
         typename TEnableIf= typename  std::enable_if<
            alib::strings::TT_IsAppendable<TAppendable,alib::nchar,alib::lang::HeapAllocator>::value
        && !alib::strings::compatibility::std::T_SuppressStdOstreamOperator<TAppendable>::value
                                                     >::type>
std::ostream& operator<<( std::ostream& stream, const TAppendable& appendable )
{
    alib::NString256 buf;
    buf.DbgDisableBufferReplacementWarning();

    if ( buf._(appendable).IsNotEmpty() )
        stream.write( buf.Buffer(), buf.Length() );
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \alib{strings;T_Append;appendable type} the \c std::ostream
/// given as pointer.
///
/// \note Unlike this documentation indicates, this operator is defined in the global namespace.
///
/// @tparam TAppendable The appendable type.
/// @param  stream      The \c std::ostream object to write the given  String into.
/// @param  appendable  The object whose contents is to be written into the given \p{stream}.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
template<typename TAppendable,
         typename TEnableIf=  typename  std::enable_if<
            alib::strings::TT_IsAppendable<TAppendable,alib::nchar,alib::lang::HeapAllocator>::value
        && !alib::strings::compatibility::std::T_SuppressStdOstreamOperator<TAppendable>::value
                                                      >::type>

std::ostream* operator<<( std::ostream* stream, const TAppendable& appendable )
{
    if (stream != nullptr)
        operator<<( * stream, appendable );
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \alib{strings;T_Append;appendable type} the \c std::ostream
/// given as reference.
///
/// \note Unlike this documentation indicates, this operator is defined in the global namespace.
///
/// @tparam TAppendable The appendable type.
/// @param  stream      The \c std::ostream object to write the given  String into.
/// @param  appendable  The object whose contents is to be written into the given \p{stream}.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
template<typename TAppendable,
         typename TEnableIf=  typename  std::enable_if<
            alib::strings::TT_IsAppendable<TAppendable,alib::wchar,alib::lang::HeapAllocator>::value
        && !alib::strings::compatibility::std::T_SuppressStdOstreamOperator<TAppendable>::value
                                                      >::type>

std::wostream& operator<<( std::wostream& stream, const TAppendable& appendable )
{
    #if ALIB_CHARACTERS_NATIVE_WCHAR
        alib::WLocalString<256> buf;
    #else
        alib::XLocalString<256> buf;
    #endif
    buf.DbgDisableBufferReplacementWarning();

    if ( buf._(appendable).IsNotEmpty() )
        stream.write( buf.Buffer(), buf.Length() );
    return stream;
}

//==================================================================================================
/// Copies the contents of the given \alib{strings;T_Append;appendable type} the \c std::ostream
/// given as pointer.
///
/// \note Unlike this documentation indicates, this operator is defined in the global namespace.
///
/// @tparam TAppendable The appendable type.
/// @tparam TAllocator  The allocator type, as prototyped with \alib{lang;Allocator}.
/// @param  stream      The \c std::ostream object to write the given  String into.
/// @param  appendable  The object whose contents is to be written into the given \p{stream}.
/// @returns The ostream to allow concatenated operations.
//==================================================================================================
template<typename TAppendable, typename TAllocator,
         typename TEnableIf=  typename  std::enable_if<    alib::strings::TT_IsAppendable<TAppendable,alib::wchar,TAllocator>::value
                                                       && !alib::strings::compatibility::std::T_SuppressStdOstreamOperator<TAppendable>::value
                                                      >::type>
std::wostream* operator<<( std::wostream* stream, const TAppendable& appendable )
{
    if (stream != nullptr)
        operator<<( * stream, appendable );
    return stream;
}



#if DOXYGEN
    }} namespace APPENDABLES {
#else
  namespace alib {  namespace strings { // the real namespace
#endif


extern template ALIB_API   void strings::T_Append<strings::compatibility::std::TISReadLine<char   >, char   , lang::HeapAllocator>
        ::operator()( TAString<char   , lang::HeapAllocator>& target, const strings::compatibility::std::TISReadLine<char   >&  reader );

extern template ALIB_API   void strings::T_Append<strings::compatibility::std::TISReadLine<wchar_t>, wchar_t, lang::HeapAllocator>
        ::operator()( TAString<wchar_t, lang::HeapAllocator>& target, const strings::compatibility::std::TISReadLine<wchar_t>&  reader );

#if DOXYGEN
    }} // namespace alib[::strings::APPENDABLES]
#else
    }  // namespace alib[::strings]
#endif

} // namespace [alib]

#include "alib/lang/callerinfo_methods.hpp"

ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::string)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::wstring)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::u16string)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::u32string)

ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::string_view)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::wstring_view)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::u16string_view)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(::std::u32string_view)

#endif // HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM


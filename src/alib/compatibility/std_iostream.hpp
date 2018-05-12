// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
    #error "Header already included"
#endif

#ifndef HPP_ALIB_COMPATIBILITY_STD_IOSTREAM
//! @cond NO_DOX
#define HPP_ALIB_COMPATIBILITY_STD_IOSTREAM 1
//! @endcond

#if ALIB_MODULE_STRINGS

// #################################################################################################
// includes
// #################################################################################################

#if !defined (_GLIBCXX_IOSTREAM )   && !defined(_IOSTREAM_)
    #include <iostream>
#endif

// #################################################################################################
// String / AString IO
// #################################################################################################

namespace aworx { namespace lib { namespace strings
{
    namespace thirdparty { namespace std {


    /** ****************************************************************************************
     * Parameter class used for \e application to objects of type
     * \alib{strings,AStringBase,AString} with specialization of template struct
     * <b>T_Apply<ISReadLine,TChar></b>.<br>
     * Reads a line of text from a \b std::istream and appends it to the target \b %AString.<br>
     * While, of-course, this class can be created 'inline' (similar objects of parameter classes
     * found in \alib{strings,FormatBase,Format}), if a series of lines are to be read
     * from a \b std::istream, a local object of this type might be created outside the loop
     * and used inside.
     * In this case, the 'output parameter' #IsEOF, can be used e.g. as a loop criteria.
     * \see
     * - \ref aworx::lib::strings::thirdparty::std::operator>>(std::istream&,NAString& string) "operator>>(istream&, AString&)"  and
     *   \ref aworx::lib::strings::thirdparty::std::operator<<(std::ostream&,const NString&)   "operator<<(ostream&, String&)".
     * - For a sample, refer to source code of \alib class \b %IniFile, method
     *   \ref aworx::lib::config::IniFile::ReadFile "IniFile::ReadFile".
     *
     * @tparam TChar  The character type of the input stream and string.
     ******************************************************************************************/
    template<typename TChar>
    class ISReadLineBase
    {
        public:

        /// The input stream to read from.
        ::std::basic_istream<TChar>*    IStream;

        /// If \c CurrentData::KEEP, the target \c %AString is not cleared before the read operation.
        lang::CurrentData               TargetData;

        /// The amount of characters that the buffer is increased while reading parts of the line.
        integer                         BufferSize;

        /// The maximum length of a single line to be read. Longer lines get truncated.
        integer                         MaxLineWidth;

        /**
         * Indicates if the end of the input stream was detected with the last read operation.
         * If so, a next read operation will not change the string (or clear it, if #TargetData is
         * \c false
         */
        bool                            IsEOF                                               = false;

        /** ****************************************************************************************
         * Constructor.
         *
         * @param istream       The input stream to read from.
         * @param targetData    If \c CurrentData::Keep, the target \c %AString is not cleared
         *                      before the read operation is performed.
         *                      Defaults to \c CurrentData::Clear.
         * @param bufferSize    The amount of characters that the buffer is increased while reading
         *                      parts of the line. Defaults to 256 characters.
         * @param maxLineWidth  The maximum length of a single line to be read. Longer lines
         *                      get truncated. Defaults to 4096 characters.
         ******************************************************************************************/
        ISReadLineBase( ::std::basic_istream<TChar>*   istream,
                        lang::CurrentData              targetData   = lang::CurrentData::Clear,
                        integer                        bufferSize   = 256,
                        integer                        maxLineWidth = 4096                        )
        : IStream     (istream),
          TargetData  (targetData),
          BufferSize  (bufferSize),
          MaxLineWidth(maxLineWidth)
        {}
    };

    // We are faking all template specializations of namespace strings for doxygen into namespace
    // thirdparty::std to keep the documentation of namespace string clean!
    #if !defined(DOX_PARSER)
      }} //namespace aworx::lib::strings [::thirdparty::std]
    #endif


    /** ********************************************************************************************
     * Copies the contents of the given \b %NString to into the \c std::ostream given as reference.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::ostream& operator<<( std::ostream& stream, const NString& string )
    {
        if ( string.IsNotEmpty() )
            stream.write( string.Buffer(), string.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \b %NString to into the \c std::ostream given as pointer.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::ostream* operator<<( std::ostream* stream, const NString& string )
    {
        stream->write( string.Buffer(), string.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \b %WString to into the \c std::ostream given as reference.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    ALIB_API std::ostream& operator<<( std::ostream& stream, const WString& string );

    /** ********************************************************************************************
     * Copies the contents of the given \b %WString to into the \c std::ostream given as pointer.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline   std::ostream* operator<<( std::ostream* stream, const WString& string )
    {
        (*stream) << string;
        return  stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \b %NString to into the \c std::wostream given as reference.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    ALIB_API std::wostream& operator<<( std::wostream& stream, const NString& string );

    /** ********************************************************************************************
     * Copies the contents of the given \b %NString to into the \c std::wostream given as pointer.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline   std::wostream* operator<<( std::wostream* stream, const NString& string )
    {
        (*stream) << string;
        return  stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \b %WString to into the \c std::wostream given as reference.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::wostream& operator<<( std::wostream& stream, const WString& string )
    {
        if ( string.IsNotEmpty() )
            stream.write( string.Buffer(), string.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \b %WString to into the \c std::wostream given as pointer.
     * @param  stream The ostream object to write the given  String into.
     * @param  string The String to write into the given ostream.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::wostream* operator<<( std::wostream* stream, const WString& string )
    {
        stream->write( string.Buffer(), string.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Clears the given \b %NAString and extracts data from the std::istream into it. The extractions
     * ends with either the end of the std::istream or when reading a newline character.
     * @param  stream The istream object to extract data from.
     * @param  string The AString to receive data.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::istream& operator>>( std::istream& stream, NAString& string )
    {
        string << thirdparty::std::ISReadLineBase<char>( &stream, lang::CurrentData::Clear );
        return stream;
    }

    /** ********************************************************************************************
     * Clears the given \b %NAString and extracts data from the std::istream into it. The extractions
     * ends with either the end of the std::istream or when reading a newline character.
     * @param  stream The istream object to extract data from.
     * @param  string The AString to receive data.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::istream* operator>>( std::istream* stream, NAString& string )
    {
        ALIB_ASSERT_WARNING ( stream != nullptr, ASTR("Given std::IStream is nullptr") );

        if (stream != nullptr)
            string << thirdparty::std::ISReadLineBase<char>( stream, lang::CurrentData::Clear );
        return stream;
    }

    /** ********************************************************************************************
     * Clears the given \b %WAString and extracts data from the std::istream into it. The extractions
     * ends with either the end of the std::istream or when reading a newline character.
     * @param  stream The istream object to extract data from.
     * @param  string The AString to receive data.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::basic_istream<wchar_t>& operator>>( std::basic_istream<wchar_t>& stream, WAString& string )
    {
        string << thirdparty::std::ISReadLineBase<wchar_t>( &stream, lang::CurrentData::Clear );
        return stream;
    }

    /** ********************************************************************************************
     * Clears the given \b %WAString and extracts data from the std::istream into it. The extractions
     * ends with either the end of the std::istream or when reading a newline character.
     * @param  stream The istream object to extract data from.
     * @param  string The AString to receive data.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    inline std::basic_istream<wchar_t>* operator>>( std::basic_istream<wchar_t>* stream, WAString& string )
    {
        ALIB_ASSERT_WARNING ( stream != nullptr, ASTR("Given std::IStream is nullptr") );

        if (stream != nullptr)
            string << thirdparty::std::ISReadLineBase<wchar_t>( stream, lang::CurrentData::Clear );
        return stream;
    }



    /**
     * Specialization of template struct \ref T_Apply for type
     * \ref ISReadLine.
     * @tparam TChar String character type.
     */
    template<typename TChar>
    struct T_Apply<thirdparty::std::ISReadLineBase<TChar>, TChar> : public std::true_type
    {
        /** ****************************************************************************************
         * Reads a line from a text file and appends the contents to \p{target}.
         * If the end of the input stream was reached, field
         * \alib{strings::thirdparty::std::ISReadLineBase,IsEOF} of parameter \p{reader} will be set
         * to \c true what indicates that a next read operation
         * would fail if it was performed.
         *
         * \note
         *   For setting field <em>IsEOF</em> the object will be casted to a non-constant reference.
         *   See \alib{strings,T_Apply} for an explanation why it is OK to
         *   do so.
         *
         * @param  target     The AString object to read into.
         * @param  reader     The object holding the \b std::istream and some parameters.
         * @return The number of characters appended to target.
         ******************************************************************************************/
        static  ALIB_API
        integer Apply( AStringBase<TChar>&                             target,
                       const thirdparty::std::ISReadLineBase<TChar>&   reader);
    };

    /** ********************************************************************************************
     * This template type may be specialized to suppress ambiguities for types \p{T} which
     * - have <c>std::operator<<(ostream, const T&)</c> defined, \b and
     * - are \alib{strings,T_Apply,applicable} to \alib strings.
     *
     * \note
     *    The ambiguity occurs due to the definition of <c>std::operator<<</c> for all applicable
     *    types.
     *
     * If a specialization of this template struct exists that inherits <c>std::true_type</c>,
     * the compiler will not choose the \alib implementation of the operator, which resolves the
     * ambiguity.
     *
     * \note Unlike this documentation states, this struct is defined in namespace
     *       #aworx::lib::strings.
     *
     * \see
     *   Specialization might be done with macro
     *   \ref ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR.
     *
     * @tparam T The applicable type to suppress
     **********************************************************************************************/
    template<typename T>  struct T_SuppressStdOstreamOperator  : std::false_type {};

    // We are faking all template specializations of namespace strings for doxygen into namespace
    // aworx::lib::strings::thirdparty::std to keep the documentation of namespace string clean!
    #if !defined(DOX_PARSER)
    }} // namespace aworx[::lib::strings]
    #endif

    #define ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(TYPE)                                       \
        namespace aworx { namespace lib { namespace strings {                                      \
        template<> struct T_SuppressStdOstreamOperator<TYPE>  : std::true_type {};}}}

    /** ********************************************************************************************
     * Copies the contents of the given \alib{strings,T_Apply,applicable type}
     * the \c std::ostream given as reference.
     *
     * \note Unlike this documentation states, this operator is defined in namespace #aworx.
     *
     * @param  stream     The \c std::ostream object to write the given  String into.
     * @param  applicable The object whose contents is to be written into the given \p{stream}.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    template<typename TApplicable,
             typename  TEnableIf= typename  std::enable_if<    aworx::lib::strings::T_Apply<TApplicable,char>::value
                                                           && !aworx::lib::strings::T_SuppressStdOstreamOperator<TApplicable>::value
                                                          >::type>

    std::ostream& operator<<( std::ostream& stream, const TApplicable& applicable )
    {
        NString256 buf;
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf, ReplaceExternalBuffer );

        if ( buf._(applicable).IsNotEmpty() )
            stream.write( buf.Buffer(), buf.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \alib{strings,T_Apply,applicable type}
     * the \c std::ostream given as pointer.
     *
     * \note Unlike this documentation states, this operator is defined in namespace #aworx.
     *
     * @param  stream     The \c std::ostream object to write the given  String into.
     * @param  applicable The object whose contents is to be written into the given \p{stream}.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    template<typename TApplicable,
             typename  TEnableIf= typename  std::enable_if<    aworx::lib::strings::T_Apply<TApplicable,char>::value
                                                           && !aworx::lib::strings::T_SuppressStdOstreamOperator<TApplicable>::value
                                                          >::type>

    std::ostream* operator<<( std::ostream* stream, const TApplicable& applicable )
    {
        if (stream != nullptr)
            aworx::operator<<( * stream, applicable );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \alib{strings,T_Apply,applicable type}
     * the \c std::ostream given as reference.
     *
     * \note Unlike this documentation states, this operator is defined in namespace #aworx.
     *
     * @param  stream     The \c std::ostream object to write the given  String into.
     * @param  applicable The object whose contents is to be written into the given \p{stream}.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    template<typename TApplicable,
             typename  TEnableIf= typename  std::enable_if<    aworx::lib::strings::T_Apply<TApplicable,wchar_t>::value
                                                           && !aworx::lib::strings::T_SuppressStdOstreamOperator<TApplicable>::value
                                                          >::type>

    std::wostream& operator<<( std::wostream& stream, const TApplicable& applicable )
    {
        WString256 buf;
        ALIB_WARN_ONCE_PER_INSTANCE_DISABLE( buf, ReplaceExternalBuffer );

        if ( buf._(applicable).IsNotEmpty() )
            stream.write( buf.Buffer(), buf.Length() );
        return stream;
    }

    /** ********************************************************************************************
     * Copies the contents of the given \alib{strings,T_Apply,applicable type}
     * the \c std::ostream given as pointer.
     *
     * \note Unlike this documentation states, this operator is defined in namespace #aworx.
     *
     * @param  stream     The \c std::ostream object to write the given  String into.
     * @param  applicable The object whose contents is to be written into the given \p{stream}.
     * @returns The ostream to allow concatenated operations.
     **********************************************************************************************/
    template<typename TApplicable,
             typename  TEnableIf= typename  std::enable_if<    aworx::lib::strings::T_Apply<TApplicable,wchar_t>::value
                                                           && !aworx::lib::strings::T_SuppressStdOstreamOperator<TApplicable>::value
                                                          >::type>

    std::wostream* operator<<( std::wostream* stream, const TApplicable& applicable )
    {
        if (stream != nullptr)
            aworx::operator<<( * stream, applicable );
        return stream;
    }


// We are faking all template specializations of namespace strings for doxygen into namespace
// strings::apply to keep the documentation of namespace string clean!
#if defined(DOX_PARSER)
}}}} // for doxygen this is now aworx[::lib::strings::thirdparty::std]
#endif

/// Type alias in namespace #aworx.
using  ISReadLineN         =     aworx::lib::strings::thirdparty::std::ISReadLineBase<char     >;

/// Type alias in namespace #aworx.
using  ISReadLineW         =     aworx::lib::strings::thirdparty::std::ISReadLineBase<wchar_t  >;

/// Type alias in namespace #aworx.
using  ISReadLine          =     aworx::lib::strings::thirdparty::std::ISReadLineBase<character>;

} // namespace [aworx]


ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(std::string)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(std::wstring)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(std::u16string)
ALIB_STRINGS_SUPPRESS_STD_OSTREAM_OPERATOR(std::u32string)

#endif // ALIB_MODULE_STRINGS


#endif // HPP_ALIB_COMPATIBILITY_STD_IOSTREAM

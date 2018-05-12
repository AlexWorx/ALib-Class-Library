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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_STRINGS_UTIL_STRING_IO)
    #error "Header already included"
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
#   include "alib/compatibility/std_iostream.hpp"
#endif

// then, set include guard
#ifndef HPP_ALIB_STRINGS_UTIL_STRING_IO
//! @cond NO_DOX
#define HPP_ALIB_STRINGS_UTIL_STRING_IO 1
//! @endcond

namespace aworx { namespace lib { namespace strings { namespace util  {


/** ************************************************************************************************
 * This class is a simple helper class that converts strings of the
 * \ref alib_strings_templated "default string type" defined with the \alib compilation
 * flags to narrow strings as expected by \c std::ostream objects.
 *
 * The basic goal of this class is to avoid preprocessor directives for code selection when
 * the default string type of \alib is a wide string.
 *
 * For example, to write string data into a file, the following approach is advised:
 *
 * 1. Create an output stream of (derived) type \c std::ostream.
 * 2. Create an object of this class and pass the output stream via #SetStream.
 * 3. Write string data stored in objects of platform dependent type alias \ref aworx::AString
 *    by passing those to method #Write.
 *
 * Within step 3, the compiler chooses the right overloaded version of method #Write. Hence the
 * potentially needed conversion of the string data is done transparently.
 *
 * \see Class #StringReader.
 **************************************************************************************************/
class StringWriter
{
    protected:
        /// The string buffer used for conversion.
        NAString                converter;

    public:
        /// The output stream as provided with #SetStream. Will be set to the \c std::cout,
        /// respectively \c std::wcout in the constructor.
        std::ostream*           ostream;

    public:
        /** ****************************************************************************************
         * Constructor.
         * Invokes #SetStream passing \c std::cout.
         ******************************************************************************************/
        inline                  StringWriter()
        : ostream( &std::cout )
        {}


        /** ****************************************************************************************
         * Sets the output stream.
         *
         * @param os  Pointer to the output stream to write to.
         ******************************************************************************************/
        inline   void           SetStream( std::ostream * os)             { ostream= os; }

        /** ****************************************************************************************
         * Returns the output stream previously set with #SetStream.
         *
         * @return  The output stream set with #SetStream.
         ******************************************************************************************/
        inline   std::ostream*  GetStream()                        { return ostream; }

        /** ****************************************************************************************
         * Write the given narrow string to the stream.
         * \note
         *  The return value is the length of the given string if it was converted to wide string,
         *  even in the case that it is not needed to be converted.
         *  This is useful to determine the real output width. Of-course, in many locales, this is
         *  still not the real output width, because even uni-code characters are not guaranteed
         *  to represent exactly one printable character. But still, this value is already a
         *  much better approximation than the length of the given narrow string.
         *
         * @param src  The string to write.
         * @return Returns the length of the given string as wide string.
         ******************************************************************************************/
        inline integer      Write( const NString& src )
        {
            ostream->write( src.Buffer(), src.Length() );
            return src.WStringLength();
        }

        /** ****************************************************************************************
         * Write the given wide string to the stream.
         * @param src  The string to write.
         * @return Returns the length of the given wide string.
         ******************************************************************************************/
        inline integer      Write( const WString& src )
        {
            converter.Clear() << src;
            ostream->write( converter.Buffer(), converter.Length() );
            return src.Length();
        }
};

/** ************************************************************************************************
 * This class is a helper class that converts narrow string data read from an object of (a derived)
 * type \c std::istream to the \ref alib_strings_templated "default string type" defined
 * with the \alib compilation flags.
 *
 * \see Further explanations are given with sibling class #StringWriter.
 **************************************************************************************************/
class StringReader
{
    protected:
        /// The string buffer used for conversion.
        NAString         converter;

        /// The input stream as provided with #SetStream. Will be set to the \c std::cin,
        /// respectively \c std::win in the constructor.
        thirdparty::std::ISReadLineBase<nchar>   readOp;


    public:
        /** ****************************************************************************************
         * Constructor.Invokes #SetStream passing \c std::cin.
         ******************************************************************************************/
        inline              StringReader()
        : readOp( &std::cin )
        {}


        /** ****************************************************************************************
         * Sets the input stream.
         *
         * @param is  Pointer to the input stream to read from to.
         ******************************************************************************************/
        inline
        void                SetStream( std::istream* is )                     { readOp.IStream= is;}

        /** ****************************************************************************************
         * Returns the input stream previously set with #SetStream.
         *
         * @return  The input stream set with #SetStream.
         ******************************************************************************************/
        inline std::istream*
                            GetStream()                                   { return readOp.IStream; }

        /** ****************************************************************************************
         * Returns \c true if the input stream signaled its end, \c false otherwise.
         *
         * @return \c true if the input stream is known to be at its end, \c false otherwise.
         ******************************************************************************************/
        inline bool         IsEOF()                                         { return readOp.IsEOF; }


        /** ****************************************************************************************
         * Reads one line of text from the input stream into a narrow string.
         *
         * @param target  The storage buffer for the string to read. This string will be cleared
         *                independent of the availability of input data.
         ******************************************************************************************/
        inline void         Read( NAString& target )
        {
            target.Clear();
            target << readOp;
        }

        /** ****************************************************************************************
         * Reads one line of text from the internal input stream into a wide string.
         *
         * @param target  The storage buffer for the string to read. This string will be cleared
         *                independent of the availability of input data.
         ******************************************************************************************/
        inline void         Read( WAString& target )
        {
            target.Clear();
            converter.Clear()  << readOp;
            target << converter;
        }
};

}}} // namespace aworx[::lib::strings::util]

/// Type alias in namespace #aworx.
using     StringWriter   =   aworx::lib::strings::util::StringWriter;

/// Type alias in namespace #aworx.
using     StringReader   =   aworx::lib::strings::util::StringReader;

} // namespace aworx

#endif // HPP_ALIB_STRINGS_UTIL_STRING_IO

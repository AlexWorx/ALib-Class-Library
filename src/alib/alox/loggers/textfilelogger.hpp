// #################################################################################################
//  aworx::lib::lox::loggers - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// include ALox main header first...
#if !defined (HPP_ALIB_ALOX)
    #include "alib/alox/alox.hpp"
#endif

// then, set include guard
#ifndef HPP_ALOX_TEXT_FILE_LOGGER
#define HPP_ALOX_TEXT_FILE_LOGGER 1


// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALOX_CORE_TEXTLOGGER_PLAINTEXTLOGGER)
    #include "alib/alox/core/textlogger/plaintextlogger.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_STRING_IO)
    #include "alib/strings/util/stringio.hpp"
#endif


namespace aworx { namespace lib { namespace lox { namespace loggers {

/** ************************************************************************************************
 * This is a very simple file logger for textual log outputs. The file name string provided
 * in the constructor is not verified.
 * The fileName may be changed by simply setting the public member #FileName.
 **************************************************************************************************/
class TextFileLogger : public aworx::lib::lox::core::textlogger::PlainTextLogger
{
    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:

        /// Encapsulates the text file stream in a system dependent way.
        StringWriter            writer;

        /// Flag to prevent file open/close operations when multi line text logging is performed.
        bool                    currentlyInMultiLineOp                                      = false;


    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /// The path and fileName to the log file.
        aworx::AString          FileName;

        /// Flag that indicates if there was an error opening he file
        bool                    hasIoError                                                   =false;


    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Creates a TextFileLogger.
         * @param fileName    The filename (potentially including a path) of the output log file.
         * @param loggerName  The name of the \e Logger. Defaults to "TEXTFILE".
         ******************************************************************************************/
        inline
        explicit            TextFileLogger( const aworx::String& fileName,
                                            const aworx::NString& loggerName    =nullptr )
        : PlainTextLogger( loggerName, "TEXTFILE", false )
        {
            FileName << fileName;
        }


        /** ****************************************************************************************
         * Destructs a TextFileLogger
         ******************************************************************************************/
        inline
        virtual            ~TextFileLogger()
        {
             ALIB_ASSERT_ERROR( writer.GetStream() == nullptr, ASTR("ostream not deleted") )
        }

    // #############################################################################################
    // Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Opens the file.
         ******************************************************************************************/
        ALIB_API
        void            openFile();

        /** ****************************************************************************************
         * Closes the file.
         ******************************************************************************************/
        ALIB_API
        void            closeFile();

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Starts/ends log line. Appends a new-line character sequence to previously logged lines.
         *
         * @param phase  Indicates the beginning or end of a log line.
         * @return Always returns true.
         ******************************************************************************************/
        ALIB_API
        virtual bool    notifyLogOp( lib::lang::Phase phase );

        /** ****************************************************************************************
         * Writes the given region of the given string to the console.
         *
         * @param buffer   The string to write a portion of.
         * @param start    The start of the portion in \p{buffer} to write out.
         * @param length   The length of the portion in \p{buffer} to write out.
         * @return The number of characters written, -1 on error.
         ******************************************************************************************/
        ALIB_API
        virtual integer logSubstring( const String& buffer, integer start, integer length );


        /** ****************************************************************************************
         *  Empty implementation, not needed for this class
         * @param phase  Indicates the beginning or end of a multi-line operation.
         ******************************************************************************************/
        ALIB_API
        virtual void    notifyMultiLineOp (lib::lang::Phase phase );

}; // class TextFileLogger


}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     TextFileLogger=           aworx::lib::lox::loggers::TextFileLogger;

}  // namespace aworx

#endif // HPP_ALOX_TEXT_FILE_LOGGER


/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX_TEXT_FILE_LOGGER
#define HPP_ALOX_TEXT_FILE_LOGGER 1

#if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_PLAINTEXTLOGGER)
    #include "alib/alox/detail/textlogger/plaintextlogger.hpp"
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
    #include "alib/compatibility/std_strings_iostream.hpp"
#endif


namespace aworx { namespace lib { namespace lox { namespace loggers {

/** ************************************************************************************************
 * This is a very simple file logger for textual log outputs. The file name string provided
 * in the constructor is not verified.
 * The fileName may be changed by simply setting the public member #FileName.
 **************************************************************************************************/
class TextFileLogger : public aworx::lib::lox::detail::textlogger::PlainTextLogger
{
    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:

        /** Encapsulates the text file stream in a system dependent way. */
        StringWriter            writer;

        /** Flag to prevent file open/close operations when multi line text logging is performed. */
        bool                    currentlyInMultiLineOp                                      = false;


    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /** The path and fileName to the log file. */
        aworx::AString          FileName;

        /** Flag that indicates if there was an error opening he file. */
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
        explicit            TextFileLogger( const aworx::String& fileName,
                                            const aworx::NString& loggerName    =nullptr )
        : PlainTextLogger( loggerName, "TEXTFILE", false )
        {
            FileName << fileName;

            // clear default std::cout stream to prevent assertion in destructor in case this
            // logger was not not used.
            ALIB_DBG( writer.SetStream( nullptr ) );
        }


        /** ****************************************************************************************
         * Destructs a TextFileLogger
         ******************************************************************************************/
        virtual            ~TextFileLogger()
        {
             ALIB_ASSERT_ERROR( writer.GetStream() == nullptr, "ostream not deleted" )
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
        virtual bool    notifyLogOp( Phase phase );

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
        virtual void    notifyMultiLineOp ( Phase phase );

}; // class TextFileLogger


}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     TextFileLogger=           lib::lox::loggers::TextFileLogger;

}  // namespace [aworx]

#endif // HPP_ALOX_TEXT_FILE_LOGGER


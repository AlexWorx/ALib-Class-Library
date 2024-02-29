/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
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

#if !defined(HPP_ALIB_CAMP_ERRORS)
#   include "alib/lang/system/systemerrors.hpp"
#endif

namespace alib {  namespace lox { namespace loggers {

/** ************************************************************************************************
 * This is a very simple file logger for textual log outputs. The file name string provided
 * in the constructor is not verified.
 * The fileName may be changed by simply setting the public member #FileName.
 *
 * In alignment with \alox principles (\alox does not throw!), the logger does not throw i/o
 * exceptions. In debug-compilations, \alib warnings are issued.
 * Recent system io/ errors are stored in public field# LastSystemError.
 * The constructor tries to open/close the file with write permissions.
 * For many applications, it should be enough to check this field after construction and
 * take according actions on failure. Otherwise, errors may periodically be checked by an
 * application (e.g. no space left on device, etc.).
 **************************************************************************************************/
class TextFileLogger : public alib::lox::detail::textlogger::PlainTextLogger
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
        alib::AString          FileName;

        /** Errors that usually indicate i/o problems.  With construction of the logger,
         *  the file is tried to be opened and closed, which might indicate major problems
         *  (permissions, path, etc) with this public field very early.                          */
        SystemErrors           LastSystemError                                 = SystemErrors::None;


    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Creates a TextFileLogger.
         * @param fileName    The filename (potentially including a path) of the output log file.
         * @param loggerName  The name of the \e Logger. Defaults to "TEXTFILE".
         ******************************************************************************************/
        ALIB_API
        explicit            TextFileLogger( const alib::String& fileName,
                                            const alib::NString& loggerName    =nullptr );

        /** ****************************************************************************************
         * Destructs a TextFileLogger
         ******************************************************************************************/
        virtual            ~TextFileLogger()                                            override  {}

    // #############################################################################################
    // Protected methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Opens the file.
         ******************************************************************************************/
        ALIB_API
        void                openFile();

        /** ****************************************************************************************
         * Closes the file.
         ******************************************************************************************/
        ALIB_API
        void                closeFile();

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
        virtual bool        notifyLogOp( lang::Phase phase )                               override;

        /** ****************************************************************************************
         * Writes the given region of the given string to the console.
         *
         * @param buffer   The string to write a portion of.
         * @param start    The start of the portion in \p{buffer} to write out.
         * @param length   The length of the portion in \p{buffer} to write out.
         * @return The number of characters written, -1 on error.
         ******************************************************************************************/
        ALIB_API
        virtual integer     logSubstring( const     String& buffer,
                                          integer   start,           integer length )      override;


        /** ****************************************************************************************
         *  Empty implementation, not needed for this class
         * @param phase  Indicates the beginning or end of a multi-line operation.
         ******************************************************************************************/
        ALIB_API
        virtual void        notifyMultiLineOp ( lang::Phase phase )                        override;

}; // class TextFileLogger


}} // namespace alib[::lox::loggers]

/// Type alias in namespace \b alib.
using     TextFileLogger=           lox::loggers::TextFileLogger;

} // namespace [alib]

#endif // HPP_ALOX_TEXT_FILE_LOGGER

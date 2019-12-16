/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX_CONSOLE_LOGGER
#define HPP_ALOX_CONSOLE_LOGGER 1

#if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_PLAINTEXTLOGGER)
    #include "alib/alox/detail/textlogger/plaintextlogger.hpp"
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
    #include "alib/compatibility/std_strings_iostream.hpp"
#endif


namespace aworx { namespace lib { namespace lox {
/** ************************************************************************************************
 * This is the C++ namespace for the implementation of <em>logger classes</em> that are provided
 * by default with <b>%ALox Logging Library</b>.
 *
 * Developed by A-Worx GmbH and published under Boost Software License.
 **************************************************************************************************/
namespace loggers {

/** ************************************************************************************************
 * A logger that logs all messages to the standard output <em>cout</em>.
 * The name of the \e Logger defaults to "CONSOLE".
 *
 * \alox text logger escape sequences (see class \ref aworx::lib::lox::ESC "ESC")
 * are removed and ignored.
 **************************************************************************************************/
class ConsoleLogger : public detail::textlogger::PlainTextLogger
{
    protected:
        /** The encapsulated output stream. */
        StringWriter      writer;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Creates a ConsoleLogger.
         * @param name (Optional) The name of the \e Logger, defaults to "CONSOLE".
         ******************************************************************************************/
        explicit            ConsoleLogger( const NString& name= nullptr )
        : PlainTextLogger( name, "CONSOLE", true )
        {}


        /** ****************************************************************************************
         * Destructs a ConsoleLogger
         ******************************************************************************************/
        virtual            ~ConsoleLogger() {}

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
        virtual bool        notifyLogOp( Phase phase );

        /** ****************************************************************************************
         * Writes the given region of the given string to the console.
         *
         * @param buffer   The string to write a portion of.
         * @param start    The start of the portion in \p{buffer} to write out.
         * @param length   The length of the portion in \p{buffer} to write out.
         * @return The number of characters written, -1 on error.
         ******************************************************************************************/
        ALIB_API
        virtual integer     logSubstring( const String& buffer, integer start, integer length );


        /** ****************************************************************************************
         *  Empty implementation, not needed for this class
         ******************************************************************************************/
        virtual void        notifyMultiLineOp( Phase )    {}

}; // class ConsoleLogger


}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     ConsoleLogger=           lib::lox::loggers::ConsoleLogger;

}  // namespace [aworx]

#endif // HPP_ALOX_CONSOLE_LOGGER


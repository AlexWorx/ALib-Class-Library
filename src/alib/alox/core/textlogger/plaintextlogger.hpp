// #################################################################################################
//  aworx::lib::lox::core - ALox Logging Library
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
#ifndef HPP_ALOX_CORE_TEXTLOGGER_PLAINTEXTLOGGER
#define HPP_ALOX_CORE_TEXTLOGGER_PLAINTEXTLOGGER 1


// #################################################################################################
// includes
// #################################################################################################

#if !defined (HPP_ALOX_CORE_TEXTLOGGER_TEXTLOGGER)
    #include "alib/alox/core/textlogger/textlogger.hpp"
#endif

// #################################################################################################
// forward declarations
// #################################################################################################

namespace aworx { namespace lib { namespace lox { namespace core { namespace textlogger{

/** ************************************************************************************************
 * A text logger that either removes or ignores (just writes through) \alox ESC sequences.
 * Implements abstract method #logText and introduces two new abstract methods
 * #logSubstring and #notifyLogOp.
 **************************************************************************************************/
class PlainTextLogger : public TextLogger
{
    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**
         * If this field is set to \c true (the default), all \ref aworx::lib::lox::ESC "ESC" color and
         * style codes get removed when logging into this Logger. \c ESC::TAB is processed.
         *
         * It might be useful, to set this to false, e.g. in the case, the contents of the
         * Logger is (later) written into a different logger (e.g. as a multi line message)
         * which is capable of interpreting the escape sequences of class ESC.
         */
        bool                        PruneESCSequences                                        = true;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Creates a PlainTextLogger
         * @param name     The name of the \e Logger. If empty, it defaults to the type name.
         * @param typeName The type of the \e Logger.
         * @param usesStdStreams  Denotes whether this logger writes to the
         *                        <em>standard output streams</em>.
         ******************************************************************************************/
        inline
        explicit  PlainTextLogger( const NString& name, const NString& typeName, bool usesStdStreams )
                  : TextLogger( name, typeName, usesStdStreams )
        {}

        /** ****************************************************************************************
         * Destructs a MemoryLogger
         ******************************************************************************************/
        virtual  ~PlainTextLogger()
        {}

    // #############################################################################################
    // Abstract methods introduced
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Abstract method to be implemented by descendants. This method is called when a new
         * log message is started. It is called exactly once before a series of logSubstring()
         * calls and exactly once after such series. If either the start or one of the calls
         * to #logSubstring returns \c false, the second invocation that would indicate the end of
         * a log message is omitted.
         *
         * Implementing this method allows the acquisition of system resources
         * (e.g. opening a file).
         *
         * @param phase  Indicates the beginning or end of a log line.
         * @return If \c false is returned, the log line is aborted without an invocation of
         *         \ref notifyLogOp "notifyLogOp(Phase::End)".
         ******************************************************************************************/
        virtual bool    notifyLogOp( lib::lang::Phase phase )                                   = 0;

        /** ****************************************************************************************
         * Abstract method to be implemented by descendants. Has to write the given region of
         * the given string to the destination.
         * Has to return the number of characters written, which might be different from the
         * region length requested, e.g. in case of character set conversions.
         *
         * If an error occurs, \c -1 can be returned to indicate that the current log should be
         * aborted. In this case, no invocation of \ref notifyLogOp "notifyLogOp(Phase::End)"
         * will be performed.
         *
         * @param buffer   The string to write a portion of.
         * @param start    The start of the portion in \p{buffer} to write out.
         * @param length   The length of the portion in \p{buffer} to write out.
         * @return The number of characters written, -1 on error.
         ******************************************************************************************/
        virtual integer logSubstring( const String& buffer, integer start, integer length )     = 0;


    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * The implementation of the abstract method of parent class TextLogger.
         * Loops over the log text, removes or ignores ESC sequences (all but ESC.TAB) and invokes
         * abstract methods of descendants as follows:
         * - \ref notifyLogOp "notifyLogOp(true)"
         * -   #logSubstring()
         * -   ...
         * - \ref notifyLogOp "notifyLogOp(Phase::End)"
         *
         * @param domain        The <em>Log Domain</em>.
         * @param verbosity     The verbosity. This has been checked to be active already on this
         *                      stage and is provided to be able to be logged out only.
         * @param msg           The log message.
         * @param scope         Information about the scope of the <em>Log Statement</em>..
         * @param lineNumber    The line number of a multi-line message, starting with 0. For
         *                      single line messages this is -1.
         ******************************************************************************************/
        ALIB_API
        virtual void    logText( core::Domain&     domain,    Verbosity verbosity,
                                 AString&          msg,
                                 core::ScopeInfo&  scope,     int       lineNumber );
}; // class PlainTextLogger

}}}}}// namespace [aworx::lib::lox::core::textlogger]

#endif // HPP_ALOX_CORE_TEXTLOGGER_PLAINTEXTLOGGER

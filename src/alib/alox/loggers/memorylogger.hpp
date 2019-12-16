/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX_MONOMEM_LOGGER
#define HPP_ALOX_MONOMEM_LOGGER 1

#if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_PLAINTEXTLOGGER)
    #include "alib/alox/detail/textlogger/plaintextlogger.hpp"
#endif

namespace aworx { namespace lib { namespace lox { namespace loggers {

/** ************************************************************************************************
 *  A logger that logs all messages to an in-memory buffer of type AString. The name of the \e Logger
 *  defaults to "MONOMEM".
 **************************************************************************************************/
class MemoryLogger : public aworx::lib::lox::detail::textlogger::PlainTextLogger
{
    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**
         * The logging Buffer. This can be accessed publicly and hence used as preferred.
         * Especially, the whole log can easily be cleared using
         * \alib{strings,TAString::Reset,AString::Reset}.
         * In multi-threaded environments, \c Lox interface's mutex should be acquired
         * before accessing this buffer. The initial size of the buffer is 8kb.
         */
        AString                     MemoryLog;

        /**
         * If this field is set to \c true (which is the default), the effective length of the
         * messages when converted to wide character strings are taken into account.
         *
         * Switching this off increases the overall logging performance (especially when logging
         * into memory) significantly.
         */
        bool                        UseWStringLengthForTabAdjustments                        = true;


    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Creates a MemoryLogger with the given name.
         * @param name              (Optional) The name of the \e Logger. Defaults to "MONOMEM".
         * @param pruneESCSequences (Optional) Sets the member \ref PruneESCSequences.
         *                          Defaults to \c true.
         * @param useWStringLengthForTabAdjustments (Optional) Sets the member
         *                          \ref UseWStringLengthForTabAdjustments.
         *                          Defaults to \c true.
         ******************************************************************************************/
        explicit        MemoryLogger( const NString& name                   = nullptr,
                                      bool pruneESCSequences                = true,
                                      bool useWStringLengthForTabAdjustments= true )
                        : PlainTextLogger( name, "MONOMEM", false )
                        {
                            MemoryLog.SetBuffer( 8092 );
                            PruneESCSequences                = pruneESCSequences;
                            UseWStringLengthForTabAdjustments= useWStringLengthForTabAdjustments;
                        }


        /** ****************************************************************************************
         * Destructs a MemoryLogger
         ******************************************************************************************/
        virtual        ~MemoryLogger() {}

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Start a new log line. Appends a new-line character sequence to previously logged lines.
         *
         * @param phase  Indicates the beginning or end of a log line.
         * @return Always returns true.
         ******************************************************************************************/
        virtual bool notifyLogOp(Phase phase)
        {
            // append new line if buffer has already lines stored
            if ( phase == Phase::Begin && MemoryLog.IsNotEmpty() )
                MemoryLog.NewLine();
            return true;
        }

        /** ****************************************************************************************
         * Write the given region of the given AString to the destination buffer.
         *
         * @param buffer   The string to write a portion of.
         * @param start    The start of the portion in \p{buffer} to write out.
         * @param length   The length of the portion in \p{buffer} to write out.
         * @return The number of characters written, -1 on error.
         ******************************************************************************************/
        virtual integer logSubstring( const String& buffer, integer start, integer length )
        {
            MemoryLog._<false>( buffer, start, length );
            return UseWStringLengthForTabAdjustments
                ? buffer.Substring<false>( start, length ).WStringLength()
                : length;
        }

        /** ****************************************************************************************
         *  Empty implementation, not needed for this class
         ******************************************************************************************/
        virtual    void notifyMultiLineOp( Phase )     {}

}; // class MemoryLogger

}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     MemoryLogger=           lib::lox::loggers::MemoryLogger;

}  // namespace [aworx]

#endif // HPP_ALOX_MONOMEM_LOGGER


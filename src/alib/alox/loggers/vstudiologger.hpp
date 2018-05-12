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
#ifndef HPP_ALOX_VSTUDIO_LOGGER
#define HPP_ALOX_VSTUDIO_LOGGER 1

#if defined(_WIN32) && ALIB_DEBUG


// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALOX_CORE_TEXTLOGGER_PLAINTEXTLOGGER)
    #include "alib/alox/core/textlogger/plaintextlogger.hpp"
#endif


namespace aworx { namespace lib { namespace lox { namespace loggers {
/** ************************************************************************************************
 *  A logger that logs to the Visual Studio output pane using <em>OutputDebugString()</em>.
 *  The name of the \e Logger defaults to "VSTUDIO_CONSOLE".
 *
 *  \alox text logger escape sequences (see class \ref aworx::lib::lox::ESC "ESC")
 *  are removed and ignored.
 *
 *  \note For the ease of use, class
 *  \ref aworx::lib::lox::Log "Log" implements a method
 *  \ref aworx::lib::lox::Log::AddDebugLogger "Log::AddDebugLogger"
 *  that tries to create the right Logger type for standard debug logging.
 *  If a Visual Studio debug session is detected, this logger is added. If in addition, a windows
 *  console application is detected, then a second logger of type WindowsConsoleLogger is added.
 **************************************************************************************************/
class VStudioLogger : public aworx::lib::lox::core::textlogger::PlainTextLogger
{
    // #############################################################################################
    // Internal members
    // #############################################################################################
    protected:
        /**
         * A character buffer used for the creation of zero-terminated output strings
         * required by the VStudio logger interface.
         *
         * If \ref ALIB_NARROW_STRINGS is set, this string also acts as a converter to wide
         * characters.
         */
        AString             outputString;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Creates a VStudioLogger.
         * @param name   (Optional) The name of the \e Logger, defaults to "VSTUDIO_CONSOLE".
         ******************************************************************************************/
        ALIB_API
        explicit            VStudioLogger( const NString& name= nullptr );

        /** ****************************************************************************************
         * Destructs a VStudioLogger
         ******************************************************************************************/
        ALIB_API
        virtual            ~VStudioLogger();

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Start a new log line. Appends a new-line character sequence to previously logged lines.
         *
         * @param phase  Indicates the beginning or end of a log operation.
         * @return Always returns true.
         ******************************************************************************************/
        ALIB_API
        virtual bool notifyLogOp(lib::lang::Phase phase);

        /** ****************************************************************************************
         * Write the given region of the given AString to the destination buffer.
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
         ******************************************************************************************/
        ALIB_API
        virtual void notifyMultiLineOp ( lib::lang::Phase )    {  }


}; // class VStudioLogger


}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     VStudioLogger=           aworx::lib::lox::loggers::VStudioLogger;

}  // namespace aworx


#endif // ALIB_IDE_VSTUDIO && ALIB_DEBUG

#endif // HPP_ALOX_VSTUDIO_LOGGER


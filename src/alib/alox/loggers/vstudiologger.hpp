//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LOX_VSTUDIO_LOGGER
#define HPP_ALIB_LOX_VSTUDIO_LOGGER 1
#pragma once
#if defined(_MSC_VER) && ALIB_DEBUG

#include "alib/alox/textlogger/plaintextlogger.hpp"


namespace alib {  namespace lox { namespace loggers {
//==================================================================================================
///  A logger that logs to the Visual Studio output pane using <em>OutputDebugString()</em>.
///  The name of the \e Logger defaults to "VSTUDIO_CONSOLE".
///
///  \alox text logger escape sequences (see class \ref alib::lox::ESC "ESC")
///  are removed and ignored.
///
///  \note For the ease of use, class
///  \ref alib::lox::Log "Log" implements a method
///  \ref alib::lox::Log::AddDebugLogger "Log::AddDebugLogger"
///  that tries to create the right Logger type for standard debug logging.
///  If a Visual Studio debug session is detected, this logger is added. If in addition, a windows
///  console application is detected, then a second logger of type WindowsConsoleLogger is added.
//==================================================================================================
class VStudioLogger : public alib::lox::textlogger::PlainTextLogger
{
    // #############################################################################################
    // Internal members
    // #############################################################################################
    protected:
        /// A character buffer used for the creation of zero-terminated output strings
        /// required by the VStudio logger interface.
        ///
        /// If \ref ALIB_CHARACTERS_WIDE is set, this string also acts as a converter to wide
        /// characters.
        AString             outputString;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        //==========================================================================================
        ///  Creates a VStudioLogger.
        /// @param name   (Optional) The name of the \e Logger, defaults to "VSTUDIO_CONSOLE".
        //==========================================================================================
        ALIB_API
        explicit            VStudioLogger( const NString& name= nullptr );

        //==========================================================================================
        /// Destructs a VStudioLogger
        //==========================================================================================
        ALIB_API
        virtual            ~VStudioLogger();

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /// Invokes parent method. Then performs some VStudio-related adoptions.
        ///
        /// @param lox     The \b %Lox to acknowledge insertion or removal
        /// @param op      The operation. Either \b ContainerOp::Insert or \b ContainerOp::Remove.
        ALIB_API
        virtual void AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op )          override;

        //==========================================================================================
        /// Start a new log line. Appends a new-line character sequence to previously logged lines.
        ///
        /// @param phase  Indicates the beginning or end of a log operation.
        /// @return Always returns true.
        //==========================================================================================
        ALIB_API
        virtual bool notifyLogOp(lang::Phase phase);

        //==========================================================================================
        /// Write the given region of the given AString to the destination buffer.
        ///
        /// @param buffer   The string to write a portion of.
        /// @param start    The start of the portion in \p{buffer} to write out.
        /// @param length   The length of the portion in \p{buffer} to write out.
        /// @return The number of characters written, -1 on error.
        //==========================================================================================
        ALIB_API
        virtual integer logSubstring( const String& buffer, integer start, integer length );


        //==========================================================================================
        ///  Empty implementation, not needed for this class
        //==========================================================================================
        ALIB_API
        virtual void notifyMultiLineOp (lang::Phase )    {  }


}; // class VStudioLogger


}} // namespace alib[::lox::loggers]

/// Type alias in namespace \b alib.
using     VStudioLogger=           lox::loggers::VStudioLogger;

}  // namespace [alib]


#endif // defined(_MSC_VER) && ALIB_DEBUG

#endif // HPP_ALIB_LOX_VSTUDIO_LOGGER


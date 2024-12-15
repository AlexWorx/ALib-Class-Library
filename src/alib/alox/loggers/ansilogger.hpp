//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LOX_ANSI_LOGGER
#define HPP_ALIB_LOX_ANSI_LOGGER 1
#pragma once
#include "alib/alox/textlogger/textlogger.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"


namespace alib {  namespace lox { namespace loggers {

//==================================================================================================
/// A logger that logs all messages to the <em>std::basic_ostream</em> instance provided in the
/// constructor. The name of the \e Logger defaults to "ANSI_LOGGER".
///
/// ALox text logger escape sequences (see class \ref alib::lox::ESC "ESC")
/// are translated to ANSI escape sequences.
/// Support for ANSI escape sequences (also referred to as <em>VT100 terminal emulation</em>)
/// is available on most unix terminal windows. Besides text colors, bold and italics font style
/// can be set.
/// ANSI escape sequences are also available in various IDE output windows.
///
/// Foreground and background colors can be set to be either light/dark or dark/light. This improves
/// the readability of log output a lot and even allows reading if foreground and background colors
/// are the same (they then still differ). However, the right setting for this is dependent on
/// the color scheme of the final output device (window). To manipulate the right setting, see field
/// #CFP and also configuration variable \ref alxcvALOX_CONSOLE_LIGHT_COLORS.
///
/// In the method #AcknowledgeLox, the default meta info string and color settings are changed
/// to fit this logger.
///
/// \note
///   Instead of using ANSI sequences in the format strings directly, which would lower the
///   run-time cost a little, ESC sequences are used because this way the light/dark color
///   selection, which depends on the field #CFP is performed correctly.
///
/// There is not 100% match between the ANSI sequences and the definitions in
/// \ref alib::lox::ESC "ESC".
/// For example ESC does not provide all ANSI colors and no blinking. On the other hand,
/// ANSI does not allow to reset the style without resetting the colors.
/// Of course, it is no problem to log other ANSI codes directly into an \b %AnsiLogger.
/// In this case, other Loggers that might be attached to the same Lox and that do not
/// support ANSI must be equipped with corresponding replacement information.
/// In other words: To support the same log output into different loggers, it is
/// recommended to use \ref alib::lox::ESC "ESC"  sequences instead of
/// directly using ANSI codes.
///
/// The ANSI codes used by this class are exposed through a list of fields.
/// They might be useful for manipulating the attributes of the various inherited format
/// variables, which of course might contain native ANSI sequences.
/// (In contrast to the log messages themselves, this meta-information is specific to a logger
/// instance and this way it does not need to be replaced in other loggers).
//==================================================================================================
class AnsiLogger : public alib::lox::textlogger::TextLogger
{
    // #############################################################################################
    // ANSI Escape Code definitions
    // #############################################################################################
    public:
    static constexpr character ANSI_RED             [6]  { A_CHAR("\033[31m")      }; ///< Select red as foreground color
    static constexpr character ANSI_GREEN           [6]  { A_CHAR("\033[32m")      }; ///< Select green as foreground color
    static constexpr character ANSI_YELLOW          [6]  { A_CHAR("\033[33m")      }; ///< Select yellow as foreground color
    static constexpr character ANSI_BLUE            [6]  { A_CHAR("\033[34m")      }; ///< Select blue as foreground color
    static constexpr character ANSI_MAGENTA         [6]  { A_CHAR("\033[35m")      }; ///< Select magenta as foreground color
    static constexpr character ANSI_CYAN            [6]  { A_CHAR("\033[36m")      }; ///< Select cyan as foreground color
    static constexpr character ANSI_BLACK           [6]  { A_CHAR("\033[30m")      }; ///< Select black as foreground color
    static constexpr character ANSI_WHITE           [11] { A_CHAR("\033[38;5;15m") }; ///< Select white as foreground color
    static constexpr character ANSI_GRAY            [12] { A_CHAR("\033[38;5;240m")}; ///< Select gray as foreground color
    static constexpr character ANSI_STD_COL         [6]  { A_CHAR("\033[39m")      }; ///< Select standard foreground color

    static constexpr character ANSI_BG_RED          [6]  { A_CHAR("\033[41m")      }; ///< Select red as background color
    static constexpr character ANSI_BG_GREEN        [6]  { A_CHAR("\033[42m")      }; ///< Select green as background color
    static constexpr character ANSI_BG_YELLOW       [6]  { A_CHAR("\033[43m")      }; ///< Select yellow as background color
    static constexpr character ANSI_BG_BLUE         [6]  { A_CHAR("\033[44m")      }; ///< Select blue as background color
    static constexpr character ANSI_BG_MAGENTA      [6]  { A_CHAR("\033[45m")      }; ///< Select magenta as background color
    static constexpr character ANSI_BG_CYAN         [6]  { A_CHAR("\033[46m")      }; ///< Select cyan as background color
    static constexpr character ANSI_BG_BLACK        [6]  { A_CHAR("\033[40m")      }; ///< Select black as background color
    static constexpr character ANSI_BG_WHITE        [11] { A_CHAR("\033[48;5;15m") }; ///< Select white as background color
    static constexpr character ANSI_BG_GRAY         [12] { A_CHAR("\033[48;5;240m")}; ///< Select gray as background color
    static constexpr character ANSI_BG_STD_COL      [6]  { A_CHAR("\033[49m")      }; ///< Select standard background color

    static constexpr character ANSI_LIGHT_RED       [11] { A_CHAR("\033[38;5;09m") }; ///< Select light red as foreground color
    static constexpr character ANSI_LIGHT_GREEN     [11] { A_CHAR("\033[38;5;10m") }; ///< Select light green as foreground color
    static constexpr character ANSI_LIGHT_YELLOW    [11] { A_CHAR("\033[38;5;11m") }; ///< Select light yellow as foreground color
    static constexpr character ANSI_LIGHT_BLUE      [11] { A_CHAR("\033[38;5;12m") }; ///< Select light blue as foreground color
    static constexpr character ANSI_LIGHT_MAGENTA   [11] { A_CHAR("\033[38;5;13m") }; ///< Select light magenta as foreground color
    static constexpr character ANSI_LIGHT_CYAN      [11] { A_CHAR("\033[38;5;14m") }; ///< Select light cyan as foreground color
    static constexpr character ANSI_LIGHT_GRAY      [12] { A_CHAR("\033[38;5;250m")}; ///< Select light gray as foreground color
    static constexpr character ANSI_LIGHT_STD_COL   [6]  { A_CHAR("\033[39m")      }; ///< Select standard foreground color

    static constexpr character ANSI_BG_LIGHT_RED    [11] { A_CHAR("\033[48;5;09m") }; ///< Select light red as background color
    static constexpr character ANSI_BG_LIGHT_GREEN  [11] { A_CHAR("\033[48;5;10m") }; ///< Select light green as background color
    static constexpr character ANSI_BG_LIGHT_YELLOW [11] { A_CHAR("\033[48;5;11m") }; ///< Select light yellow as background color
    static constexpr character ANSI_BG_LIGHT_BLUE   [11] { A_CHAR("\033[48;5;12m") }; ///< Select light blue as background color
    static constexpr character ANSI_BG_LIGHT_MAGENTA[11] { A_CHAR("\033[48;5;13m") }; ///< Select light magenta as background color
    static constexpr character ANSI_BG_LIGHT_CYAN   [11] { A_CHAR("\033[48;5;14m") }; ///< Select light cyan as background color
    static constexpr character ANSI_BG_LIGHT_GRAY   [12] { A_CHAR("\033[48;5;250m")}; ///< Select light gray as background color
    static constexpr character ANSI_BG_LIGHT_STD_COL[6]  { A_CHAR("\033[49m")      }; ///< Select standard background color

    static constexpr character ANSI_BOLD            [5]  { A_CHAR("\033[1m")       }; ///< Select bold font style
    static constexpr character ANSI_ITALICS         [5]  { A_CHAR("\033[3m")       }; ///< Select italics font style
    static constexpr character ANSI_STD_STYLE       [5]  { A_CHAR("\033[0m")       }; ///< Select standard font style
    static constexpr character ANSI_RESET           [5]  { A_CHAR("\033[0m")       }; ///< Reset colors and font style



    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// Encapsulates \c std::cout, \c std::wcout or the output stream provided in the
        /// corresponding constructor.
        StringWriter      writer;

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:

        /// As of today, this parameter struct has only one value of type
        /// \alib{lox::textlogger::ColorfulLoggerParameters;LightColorUsage}. With this logger,
        /// this flag is used as follows.
        ///
        /// Foreground and background colors chosen by this class might differ in their intensity.
        /// This increases the overall readability by increasing the contrast.
        /// If the background color of a console window is dark, then the background colors of
        /// colored log output should be darker colors than the foreground colors - and vice versa.
        ///
        /// Depending on the setting of this field, \alox
        /// \ref alib::lox::ESC "escape codes" for colors are translated to normal ANSI colors or
        /// lighter ones:
        /// - If this field evaluates to
        ///   \alib{lox::textlogger::ColorfulLoggerParameters;LightColorUsage;Never}, light colors
        ///   are never used.
        /// - If this field is
        ///   \alib{lox::textlogger::ColorfulLoggerParameters;LightColorUsage;Foreground},
        ///   colors will be light colors and background colors dark ones. This is the default.
        /// - With
        ///   \alib{lox::textlogger::ColorfulLoggerParameters;LightColorUsage;Background},
        ///   background colors will be light colors and foreground colors dark ones.
        ///
        /// The configuration variable \ref alxcvALOX_CONSOLE_LIGHT_COLORS allows externally
        /// modifying this flag. It is read once within the constructor.
        textlogger::ColorfulLoggerParameters   CFP;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        //==========================================================================================
        /// Creates an AnsiLogger using \c std::cout respectively \c std::wcout.
        /// @param name           The name of the \e Logger, defaults to what is provided with
        ///                       parameter \p{typeName}.
        /// @param typeName       The type of the \e Logger, defaults to "ANSI".
        //==========================================================================================
        ALIB_API explicit
        AnsiLogger( const NString& name= nullptr, const NString& typeName= "ANSI" )
        : TextLogger( name, typeName, true )                                                      {}

        //==========================================================================================
        /// Creates an AnsiLogger using the given output stream.
        /// \note If \c std::cout respectively \c std::wcout must not be provided here. Instead, the
        ///       alternative constructor omitting parameter \p{pOStream} is to be used.
        /// @param pOStream       The output stream to write into.
        /// @param name           The name of the \e Logger, defaults to what is provided with
        ///                       parameter \p{typeName}.
        /// @param typeName       The type of the \e Logger, defaults to "ANSI".
        //==========================================================================================
        ALIB_API explicit
        AnsiLogger( std::ostream*  pOStream,
                    const NString& name     = nullptr,
                    const NString& typeName = "ANSI"     )
        : TextLogger( name, typeName, false )                      { writer.SetStream( pOStream ); }


        //==========================================================================================
        /// Destructs an AnsiLogger
        //==========================================================================================
        ALIB_API
        virtual        ~AnsiLogger()                                                       override;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        //==========================================================================================
        /// Implementation of the abstract method of parent class TextLogger.
        /// Logs messages to the basic output stream provided in the constructor. Replaces
        /// \alox ESC escape sequences with ANSI escape sequences.
        ///
        /// @param domain      The <em>Log Domain</em>.
        /// @param verbosity   The verbosity. This has been checked to be active already on this
        ///                    stage and is provided to be able to be logged out only.
        /// @param msg         The log message.
        /// @param scope       Information about the scope of the <em>Log Statement</em>..
        /// @param lineNumber  The line number of a multi-line message, starting with 0. For
        ///                    single line messages this is -1.
        //==========================================================================================
        ALIB_API
        virtual void logText(  detail::Domain&      domain,
                               Verbosity            verbosity,
                               AString&             msg,
                               detail::ScopeInfo&   scope,
                               int                  lineNumber)                            override;

        //==========================================================================================
        /// Empty implementation.
        //==========================================================================================
        virtual void notifyMultiLineOp ( lang::Phase )                                  override  {}


        //==========================================================================================
        /// Invokes parent method. Then reads and interprets configuration variable
        /// \ref alxcvALOX_CONSOLE_LIGHT_COLORS. Finally it is checked if variable
        /// \ref alxcvALOX_LOGGERNAME_FORMAT has priority of default values, and if yes, such
        /// values are overwritten to include ANSI color codes. These color codes are retrieved from
        /// resources named <c>"Var_D21A"</c>.
        ///
        /// @param lox     The \b %Lox to acknowledge insertion or removal
        /// @param op      The operation. Either \b ContainerOp::Insert or \b ContainerOp::Remove.
        //==========================================================================================
        ALIB_API
        virtual void AcknowledgeLox( detail::LoxImpl* lox, lang::ContainerOp op )          override;


}; // class AnsiLogger

//==================================================================================================
/// A #AnsiLogger that logs all messages to the standard output <em>cout</em>.
/// The name of the \e Logger defaults to "ANSI_CONSOLE".
///
/// Provides 'cout' to the constructor of its parent class %AnsiLogger.
/// See class #AnsiLogger for more information on ANSI escape sequences and their use.
///
/// \note To avoid misunderstandings: This class cannot enable the output console (which receives
///       \alox log data) to support ANSI Escape Codes. The opposite is right: this class should be
///       used only if the console supports ANSI Escape Codes.
//==================================================================================================
class AnsiConsoleLogger : public AnsiLogger
{
    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        //==========================================================================================
        /// Creates an AnsiConsoleLogger.
        /// @param name     (Optional) The name of the \e Logger, defaults to "CONSOLE".
        //==========================================================================================
        ALIB_API
        explicit            AnsiConsoleLogger( const NString& name= nullptr  );

        //==========================================================================================
        /// Destructs an AnsiConsoleLogger
        //==========================================================================================
        ALIB_API
        virtual            ~AnsiConsoleLogger()                                            override;
}; // class AnsiConsoleLogger


}} // namespace alib[::lox::loggers]

/// Type alias in namespace \b alib.
using     AnsiLogger=           lox::loggers::AnsiLogger;

/// Type alias in namespace \b alib.
using     AnsiConsoleLogger=    lox::loggers::AnsiConsoleLogger;

}  // namespace [alib]

#endif // HPP_ALIB_LOX_ANSI_LOGGER


﻿/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/

#ifndef HPP_ALOX_ANSI_LOGGER
#define HPP_ALOX_ANSI_LOGGER 1

#if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_TEXTLOGGER)
    #include "alib/alox/detail/textlogger/textlogger.hpp"
#endif


#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
    #include "alib/compatibility/std_strings_iostream.hpp"
#endif


namespace aworx { namespace lib { namespace lox { namespace loggers {

/** ************************************************************************************************
 * A logger that logs all messages to the <em>std::basic_ostream</em> instance provided in the
 * constructor. The name of the \e Logger defaults to "ANSI_LOGGER".
 *
 * ALox text logger escape sequences (see class \ref aworx::lib::lox::ESC "ESC")
 * are translated to ANSI escape sequences.
 * Support for ANSI escape sequences (also referred to as <em>VT100 terminal emulation</em>)
 * is available on most unix terminal windows. Besides text colors, bold and italics font style
 * can be set.
 * ANSI escape sequences are also available in various IDE output windows.
 *
 * Foreground and background colors can be set to be either light/dark or dark/light. This improves
 * the readability of log output a lot and even allows to read if foreground and background colors
 * are the same (they then still differ). However, the right setting for this is dependent on
 * the color scheme of the final output device (window). To manipulate the right setting, see field
 * #UseLightColors and also configuration variable
 * \https{ALOX_CONSOLE_LIGHT_COLORS,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}.
 *
 * In the constructor, a default format string and some other definitions in member
 * \ref MetaInfo get set to include color settings.
 * Of-course, these publicly accessible format attributes can be customized after creation.
 *
 * \note Instead of using ANSI sequences in the format strings directly, which would lower the
 * run-time cost a little, ESC sequences are used because this way the light/dark color
 * selection, which depends on the field #UseLightColors is performed correctly.
 *
 * There is not 100% match between the ANSI sequences and the definitions in
 * \ref aworx::lib::lox::ESC "ESC".
 * For example ESC does not provide all ANSI colors and no blinking. On the other hand,
 * ANSI does not allow to reset the style without resetting the colors.
 * Of-course, it is no problem to log other ANSI codes directly into an \b %AnsiLogger.
 * In this case, other Loggers that might be attached to the same Lox and that do not
 * support ANSI must be equipped with corresponding replacement information.
 * In other words: To support the same log output into different loggers, it is
 * recommended to use \ref aworx::lib::lox::ESC "ESC"  sequences instead of
 * directly using ANSI codes.
 *
 * The ANSI codes used by this class are exposed through a list of fields.
 * They might be useful for manipulating the attributes of the \ref MetaInfo
 * member, which of-course might contain native ANSI sequences.
 * (In contrast to the log messages themselves, this meta information is specific to a logger
 * instance and this way it does not need to be replaced in other loggers).
 **************************************************************************************************/
class AnsiLogger : public aworx::lib::lox::detail::textlogger::TextLogger
{
    // #############################################################################################
    // ANSI Escape Code definitions
    // #############################################################################################
    public:
    #if defined(_MSC_VER)
    // MSC  (as of 12/2015):
    // C4579: in-class initialization for type 'const aworx::character[11]'
    // is not yet implemented; static member will remain uninitialized at run-time but
    // use in constant-expressions is supported
    ALIB_API static     character  ANSI_RED              [6] ; ///< Select red as foreground color
    ALIB_API static     character  ANSI_GREEN            [6] ; ///< Select green as foreground color
    ALIB_API static     character  ANSI_YELLOW           [6] ; ///< Select yellow as foreground color
    ALIB_API static     character  ANSI_BLUE             [6] ; ///< Select blue as foreground color
    ALIB_API static     character  ANSI_MAGENTA          [6] ; ///< Select magenta as foreground color
    ALIB_API static     character  ANSI_CYAN             [6] ; ///< Select cyan as foreground color
    ALIB_API static     character  ANSI_BLACK            [6] ; ///< Select black as foreground color
    ALIB_API static     character  ANSI_WHITE            [11]; ///< Select white as foreground color
    ALIB_API static     character  ANSI_GRAY             [12]; ///< Select gray as foreground color
    ALIB_API static     character  ANSI_STD_COL          [6] ; ///< Select standard foreground color

    ALIB_API static     character  ANSI_BG_RED           [6] ; ///< Select red as background color
    ALIB_API static     character  ANSI_BG_GREEN         [6] ; ///< Select green as background color
    ALIB_API static     character  ANSI_BG_YELLOW        [6] ; ///< Select yellow as background color
    ALIB_API static     character  ANSI_BG_BLUE          [6] ; ///< Select blue as background color
    ALIB_API static     character  ANSI_BG_MAGENTA       [6] ; ///< Select magenta as background color
    ALIB_API static     character  ANSI_BG_CYAN          [6] ; ///< Select cyan as background color
    ALIB_API static     character  ANSI_BG_BLACK         [6] ; ///< Select black as background color
    ALIB_API static     character  ANSI_BG_WHITE         [11]; ///< Select white as background color
    ALIB_API static     character  ANSI_BG_GRAY          [12]; ///< Select gray as background color
    ALIB_API static     character  ANSI_BG_STD_COL       [6] ; ///< Select standard background color

    ALIB_API static     character  ANSI_LIGHT_RED        [11]; ///< Select light red as foreground color
    ALIB_API static     character  ANSI_LIGHT_GREEN      [11]; ///< Select light green as foreground color
    ALIB_API static     character  ANSI_LIGHT_YELLOW     [11]; ///< Select light yellow as foreground color
    ALIB_API static     character  ANSI_LIGHT_BLUE       [11]; ///< Select light blue as foreground color
    ALIB_API static     character  ANSI_LIGHT_MAGENTA    [11]; ///< Select light magenta as foreground color
    ALIB_API static     character  ANSI_LIGHT_CYAN       [11]; ///< Select light cyan as foreground color
    ALIB_API static     character  ANSI_LIGHT_GRAY       [12]; ///< Select light gray as foreground color
    ALIB_API static     character  ANSI_LIGHT_STD_COL    [6] ; ///< Select standard foreground color

    ALIB_API static     character  ANSI_BG_LIGHT_RED     [11]; ///< Select light red as background color
    ALIB_API static     character  ANSI_BG_LIGHT_GREEN   [11]; ///< Select light green as background color
    ALIB_API static     character  ANSI_BG_LIGHT_YELLOW  [11]; ///< Select light yellow as background color
    ALIB_API static     character  ANSI_BG_LIGHT_BLUE    [11]; ///< Select light blue as background color
    ALIB_API static     character  ANSI_BG_LIGHT_MAGENTA [11]; ///< Select light magenta as background color
    ALIB_API static     character  ANSI_BG_LIGHT_CYAN    [11]; ///< Select light cyan as background color
    ALIB_API static     character  ANSI_BG_LIGHT_GRAY    [12]; ///< Select light gray as background color
    ALIB_API static     character  ANSI_BG_LIGHT_STD_COL [6] ; ///< Select standard background color

    ALIB_API static     character  ANSI_BOLD             [5] ; ///< Select bold font style
    ALIB_API static     character  ANSI_ITALICS          [5] ; ///< Select italics font style
    ALIB_API static     character  ANSI_STD_STYLE        [5] ; ///< Select standard font style
    ALIB_API static     character  ANSI_RESET            [5] ; ///< Reset colors and font style

    #else

    static constexpr    character  ANSI_RED              [6]   { A_CHAR("\033[31m")       };
    static constexpr    character  ANSI_GREEN            [6]   { A_CHAR("\033[32m")       };
    static constexpr    character  ANSI_YELLOW           [6]   { A_CHAR("\033[33m")       };
    static constexpr    character  ANSI_BLUE             [6]   { A_CHAR("\033[34m")       };
    static constexpr    character  ANSI_MAGENTA          [6]   { A_CHAR("\033[35m")       };
    static constexpr    character  ANSI_CYAN             [6]   { A_CHAR("\033[36m")       };
    static constexpr    character  ANSI_BLACK            [6]   { A_CHAR("\033[30m")       };
    static constexpr    character  ANSI_WHITE            [11]  { A_CHAR("\033[38;5;15m")  };
    static constexpr    character  ANSI_GRAY             [12]  { A_CHAR("\033[38;5;240m") };
    static constexpr    character  ANSI_STD_COL          [6]   { A_CHAR("\033[39m")       };

    static constexpr    character  ANSI_BG_RED           [6]   { A_CHAR("\033[41m")       };
    static constexpr    character  ANSI_BG_GREEN         [6]   { A_CHAR("\033[42m")       };
    static constexpr    character  ANSI_BG_YELLOW        [6]   { A_CHAR("\033[43m")       };
    static constexpr    character  ANSI_BG_BLUE          [6]   { A_CHAR("\033[44m")       };
    static constexpr    character  ANSI_BG_MAGENTA       [6]   { A_CHAR("\033[45m")       };
    static constexpr    character  ANSI_BG_CYAN          [6]   { A_CHAR("\033[46m")       };
    static constexpr    character  ANSI_BG_BLACK         [6]   { A_CHAR("\033[40m")       };
    static constexpr    character  ANSI_BG_WHITE         [11]  { A_CHAR("\033[48;5;15m")  };
    static constexpr    character  ANSI_BG_GRAY          [12]  { A_CHAR("\033[48;5;240m") };
    static constexpr    character  ANSI_BG_STD_COL       [6]   { A_CHAR("\033[49m")       };

    static constexpr    character  ANSI_LIGHT_RED        [11]  { A_CHAR("\033[38;5;09m")  };
    static constexpr    character  ANSI_LIGHT_GREEN      [11]  { A_CHAR("\033[38;5;10m")  };
    static constexpr    character  ANSI_LIGHT_YELLOW     [11]  { A_CHAR("\033[38;5;11m")  };
    static constexpr    character  ANSI_LIGHT_BLUE       [11]  { A_CHAR("\033[38;5;12m")  };
    static constexpr    character  ANSI_LIGHT_MAGENTA    [11]  { A_CHAR("\033[38;5;13m")  };
    static constexpr    character  ANSI_LIGHT_CYAN       [11]  { A_CHAR("\033[38;5;14m")  };
    static constexpr    character  ANSI_LIGHT_GRAY       [12]  { A_CHAR("\033[38;5;250m") };
    static constexpr    character  ANSI_LIGHT_STD_COL    [6]   { A_CHAR("\033[39m")       };

    static constexpr    character  ANSI_BG_LIGHT_RED     [11]  { A_CHAR("\033[48;5;09m")  };
    static constexpr    character  ANSI_BG_LIGHT_GREEN   [11]  { A_CHAR("\033[48;5;10m")  };
    static constexpr    character  ANSI_BG_LIGHT_YELLOW  [11]  { A_CHAR("\033[48;5;11m")  };
    static constexpr    character  ANSI_BG_LIGHT_BLUE    [11]  { A_CHAR("\033[48;5;12m")  };
    static constexpr    character  ANSI_BG_LIGHT_MAGENTA [11]  { A_CHAR("\033[48;5;13m")  };
    static constexpr    character  ANSI_BG_LIGHT_CYAN    [11]  { A_CHAR("\033[48;5;14m")  };
    static constexpr    character  ANSI_BG_LIGHT_GRAY    [12]  { A_CHAR("\033[48;5;250m") };
    static constexpr    character  ANSI_BG_LIGHT_STD_COL [6]   { A_CHAR("\033[49m")       };

    static constexpr    character  ANSI_BOLD             [5]   { A_CHAR("\033[1m")        };
    static constexpr    character  ANSI_ITALICS          [5]   { A_CHAR("\033[3m")        };
    static constexpr    character  ANSI_STD_STYLE        [5]   { A_CHAR("\033[0m")        };
    static constexpr    character  ANSI_RESET            [5]   { A_CHAR("\033[0m")        };
    #endif



    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /** Encapsulates \c std::cout, \c std::wcout or the output stream provided in the
            corresponding constructor.                                                    */
        StringWriter      writer;

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:

        /**
         * Foreground and background colors chosen by this class might differ in their intensity.
         * This increases the overall readability by increasing the contrast.
         * If the background color of a console window is dark, then the background colors of
         * colored log output should be darker colors than the foreground colors - and vice versa.
         *
         * Depending on the setting of this field, \alox
         * \ref aworx::lib::lox::ESC "escape codes" for colors are translated to normal ANSI colors or
         * lighter ones:
         * - If this field is \ref LightColorUsage "LightColorUsage::Never", light colors are
         *   never used.
         * - If this field is \ref LightColorUsage "LightColorUsage::ForegroundLight", foreground
         *   colors will be light colors and background colors dark ones. This is the default.
         * - If \ref LightColorUsage "LightColorUsage::ForegroundDark", background colors will be
         *   light colors and foreground colors dark ones.
         *
         * The configuration variable
         * \https{ALOX_CONSOLE_LIGHT_COLORS,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}
         * allows to externally modify this flag. It is read once within the constructor .
         */
        LightColorUsage         UseLightColors;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Creates an AnsiLogger using \c std::cout respectively \c std::wcout.
         * @param name           The name of the \e Logger, defaults to what is provided with
         *                       parameter \p{typeName}.
         * @param typeName       The type of the \e Logger, defaults to "ANSI".
         ******************************************************************************************/
        ALIB_API
        explicit        AnsiLogger( const NString& name= nullptr, const NString& typeName= "ANSI" );

        /** ****************************************************************************************
         * Creates an AnsiLogger using the given output stream.
         * \note If \c std::cout respectively \c std::wcout must not be provided here. Instead, the
         *       alternative constructor omitting parameter \p{pOStream} is to be used.
         * @param pOStream       The output stream to write into.
         * @param name           The name of the \e Logger, defaults to what is provided with
         *                       parameter \p{typeName}.
         * @param typeName       The type of the \e Logger, defaults to "ANSI".
         ******************************************************************************************/
        ALIB_API
        explicit        AnsiLogger( std::ostream* pOStream,
                                    const NString& name= nullptr, const NString& typeName= "ANSI" );

        /** ****************************************************************************************
         * Destructs an AnsiLogger
         ******************************************************************************************/
        ALIB_API
        virtual        ~AnsiLogger();

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:

    // #############################################################################################
    // Abstract method implementations
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of the abstract method of parent class TextLogger.
         * Logs messages to the basic output stream provided in the constructor. Replaces
         * \alox ESC escape sequences with ANSI escape sequences.
         *
         * @param domain      The <em>Log Domain</em>.
         * @param verbosity   The verbosity. This has been checked to be active already on this
         *                    stage and is provided to be able to be logged out only.
         * @param msg         The log message.
         * @param scope       Information about the scope of the <em>Log Statement</em>..
         * @param lineNumber  The line number of a multi-line message, starting with 0. For
         *                    single line messages this is -1.
         ******************************************************************************************/
        ALIB_API
        virtual void logText(  detail::Domain&       domain,
                               Verbosity           verbosity,
                               AString&            msg,
                               detail::ScopeInfo&    scope,
                               int                 lineNumber);

        /** ****************************************************************************************
         * Empty implementation.
         ******************************************************************************************/
        virtual void notifyMultiLineOp ( Phase ) {}


        /** ****************************************************************************************
         * The shared pared of the constructor.
         ******************************************************************************************/
        ALIB_API
        void construct();



}; // class AnsiLogger

/** ************************************************************************************************
 * A #AnsiLogger that logs all messages to the standard output <em>cout</em>.
 * The name of the \e Logger defaults to "ANSI_CONSOLE".
 *
 * Provides 'cout' to the constructor of its parent class %AnsiLogger.
 * See class #AnsiLogger for more information on ANSI escape sequences and their use.
 *
 * \note To avoid misunderstandings: This class can not enable the output console (which receives
 *       \alox log data) to support ANSI Escape Codes. The opposite is right: this class should be
 *       used only if the console supports ANSI Escape Codes.
 **************************************************************************************************/
class AnsiConsoleLogger : public AnsiLogger
{
    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Creates an AnsiConsoleLogger.
         * @param name     (Optional) The name of the \e Logger, defaults to "CONSOLE".
         ******************************************************************************************/
        ALIB_API
        explicit            AnsiConsoleLogger( const NString& name= nullptr  );

        /** ****************************************************************************************
         * Destructs an AnsiConsoleLogger
         ******************************************************************************************/
        ALIB_API
        virtual            ~AnsiConsoleLogger();
}; // class AnsiConsoleLogger


}}}// namespace aworx[::lib::lox::loggers]

/// Type alias in namespace #aworx.
using     AnsiLogger=           lib::lox::loggers::AnsiLogger;

/// Type alias in namespace #aworx.
using     AnsiConsoleLogger=    lib::lox::loggers::AnsiConsoleLogger;

}  // namespace [aworx]

#endif // HPP_ALOX_ANSI_LOGGER


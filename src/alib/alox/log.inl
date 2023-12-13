/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALOX_LOG
#define HPP_ALOX_LOG 1

#if !defined(HPP_ALIB_ALOX)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

namespace aworx { namespace lib { namespace lox {

// forward declarations
class Lox;
namespace detail
{
    class Logger;
    namespace textlogger
    {
        class TextLogger;
    }
}

class ALoxReportWriter;

// The debug lox singleton
#if ALOX_DBG_LOG && !defined(ALIB_DOX)

    extern ALIB_API Lox*     theDebugLox;  // will be created in ALox::bootstrap

#endif



/** ************************************************************************************************
 * Holds static objects used for standard debug logging and provides an interface to
 * create such objects. If compiler symbol #ALOX_DBG_LOG is set to \c 0, this class will be empty.
 *
 * \note:
 *   In C# and Java, this class is the interface class for debug logging which gets pruned,
 *   and mimics the functionality of class \ref aworx::lib::lox::Lox "Lox".
 *   In C++, there is no need to double functionality of \b Lox here.
 **************************************************************************************************/
class Log
{
    #if ALOX_DBG_LOG

    // #############################################################################################
    // Public fields
    // #############################################################################################

    public:
        /** The debug logger created by AddDebugLogger. */
        ALIB_API static detail::textlogger::TextLogger*   DebugLogger;

        /** An (additional) IDE specific logger, that might be created by AddDebugLogger. */
        ALIB_API static detail::textlogger::TextLogger*   IDELogger;

        /**  The \alib ReportWriter. This will be created and registered in method
         *   \ref aworx::lib::lox::Log::AddDebugLogger    "Log::AddDebugLogger" and removed and deleted in
         *   \ref aworx::lib::lox::Log::RemoveDebugLogger "Log::RemoveDebugLogger" in the case that
         * the original \alib \b ReportWriterStdIO is in place.
         */
        static ALoxReportWriter*                        DebugReportWriter;

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the default singleton of class \b %Lox used for debug logging.
         * @return The debug-logging Lox of \alox
         ******************************************************************************************/
        static ALIB_FORCE_INLINE
        Lox*     Get()
        {
            return theDebugLox;
        }


        /** ****************************************************************************************
         * This method creates an adequate/default debug logger.
         * It is used by macro \ref Log_AddDebugLogger also automatically invoked
         * when debug logging is used without the explicit creation and registration of
         * any other logger.
         *
         * Of-course, alternatively to using this method (resp. the macro), a suitable (set of)
         * debug logger(s) can be created manually. Also, before or after using this method
         * additional debug loggers may be created.
         *
         * In the current version of \alox (future changes are likely), this method does:
         * - use
         *   \ref aworx::lib::lox::Lox::CreateConsoleLogger "Lox::CreateConsoleLogger"
         *   to create the best compatible console logger for the
         *   running platform
         * - this logger will be added to the debug Lox object with specifying
         *   \c Verbosity::Error for internal domains.
         * - If under windows, a Visual Studio debug session is running, adds a
         *   \ref aworx::lib::lox::loggers::VStudioLogger "VStudioLogger"
         *   in addition to the standard console logger. This can be suppressed using
         *   configuration variable
         *   \https{ALOX_NO_IDE_LOGGER,alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html}.
         *
         * The name of the \e Logger created is \c "DEBUG_LOGGER". It will be registered with
         * the standard \b %Lox used for debug-logging, by setting \e Verbosities
         * - Verbosity::Verbose for the root domain <c> '/'</c> and
         * - Verbosity::Warning for internal domains.
         *
         * An optionally created second, IDE-specific \e Logger will be named \c "IDE_LOGGER"
         * and will be registered with the standard \b %Lox used for debug-logging with the same
         * \e Verbosities as \c "DEBUG_LOGGER" is.
         *
         * Finally, this method also invokes #AddALibReportWriter.
         *
         * @param lox  The lox to add the debug logger(s) to.
         ******************************************************************************************/
        ALIB_API static void      AddDebugLogger( Lox* lox );

        /** ****************************************************************************************
         *  Removes the \e Logger(s) and which was (were) created by \ref AddDebugLogger.
         *  This method also invokes #RemoveALibReportWriter.
         *
         *  @param lox The lox to remove the debug logger(s) from.
         ******************************************************************************************/
        ALIB_API static void      RemoveDebugLogger( Lox* lox );

        /** ****************************************************************************************
         * In the case that the original \alib \b ReportWriterStdIO is still in place,
         * \ref aworx::lib::results::Report::PushWriter "Report::PushWriter" is invoked to provide a
         * ReportWriter of type
         * \ref aworx::lib::lox::ALoxReportWriter "ALoxReportWriter".
         *
         * \note
         * This method is effective only with debug builds. Usually it is invoked indirectly by
         * using method #AddDebugLogger. Applications that do not use that method (e.g. because
         * they are using release logging exclusively), should invoke this method on bootstrap
         * providing their (release) lox.
         * In this case, the \e Verbosity of the internal domain used by class
         * \ref aworx::lib::lox::ALoxReportWriter "ALoxReportWriter" has to be set for the
         * the logger(s) in given \p{lox} in question.
         *
         * @param lox  The lox that the
         *             \ref aworx::lib::lox::ALoxReportWriter "ALoxReportWriter" created will be using.
         ******************************************************************************************/
        ALIB_API static void      AddALibReportWriter( Lox* lox );

        /** ****************************************************************************************
         * Removes the report writer created with #AddALibReportWriter.
         ******************************************************************************************/
        ALIB_API static void      RemoveALibReportWriter();

    #endif // ALOX_DBG_LOG

}; // LOG


}} // namespace aworx[::lib::lox]

/// Type alias in namespace #aworx.
using     Log=           lib::lox::Log;

}  // namespace [aworx]

#endif // HPP_ALOX_LOG

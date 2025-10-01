//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
/// 
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {

// forward declarations
class Lox;
#if !ALIB_C20_MODULES
namespace detail        { class Logger;    }
namespace textlogger    { class TextLogger; }
#endif

// The debug lox singleton
#if ALOX_DBG_LOG
    /// The debug-lox instance. This object will be created in method
    /// \alib{lox;ALoxCamp::Bootstrap} and deleted with \alib{lox;ALoxCamp::Shutdown}.
    extern ALIB_DLL Lox*     DEBUG_LOX;
#endif
#if ALIB_DEBUG
    /// The domain-prefix used with plug-in function \alib{lox;ALoxAssertionPlugin}.
    /// The given assertion domain will be appended to this name, separated by the domain
    /// separation character <c>'/'</c>.<br>
    /// Defaults to <c>"ALIB"</c>.
    ALIB_DLL extern std::string_view  const ALOX_ASSERTION_PLUGIN_DOMAIN_PREFIX;
#endif

//==================================================================================================
/// Holds static objects used for standard debug logging and provides an interface to
/// create such objects. If the compiler-symbol #ALOX_DBG_LOG is set to \c 0, this class will be empty.
//==================================================================================================
class Log
{
  public:
  #if ALOX_DBG_LOG
    /// The debug logger created by AddDebugLogger.
    ALIB_DLL static textlogger::TextLogger*   DebugLogger;

    /// An (additional) IDE specific logger, that might be created by AddDebugLogger.
    ALIB_DLL static textlogger::TextLogger*   IDELogger;

    /// Returns the default singleton of class \b %Lox used for debug logging.
    /// @return The debug-logging Lox of \alox
    static Lox*     Get()                                                    { return DEBUG_LOX; }

    /// This method creates an adequate/default debug logger.
    /// It is used by macro \ref Log_AddDebugLogger also automatically invoked
    /// when debug logging is used without the explicit creation and registration of
    /// any other logger.
    ///
    /// Of course, alternatively to using this method (resp. the macro), a suitable (set of)
    /// debug logger(s) can be created manually. Also, before or after using this method
    /// additional debug loggers may be created.
    ///
    /// In the current version of \alox (future changes are likely), this method does:
    /// - use
    ///   \ref alib::lox::Lox::CreateConsoleLogger "Lox::CreateConsoleLogger"
    ///   to create the best compatible console logger for the
    ///   running platform
    /// - this logger will be added to the debug Lox object with specifying
    ///   \c Verbosity::Error for internal domains.
    /// - If under windows, a Visual Studio debug session is running, adds a
    ///   \ref alib::lox::loggers::VStudioLogger "VStudioLogger"
    ///   in addition to the standard console logger. This can be suppressed using
    ///   configuration variable \ref alxcvALOX_NO_IDE_LOGGER.
    ///
    /// The name of the \e Logger created is \c "DEBUG_LOGGER". It will be registered with
    /// the standard \b %Lox used for debug-logging, by setting \e Verbosities
    /// - Verbosity::Verbose for the root domain <c> '/'</c> and
    /// - Verbosity::Warning for internal domains.
    ///
    /// An optionally created second, IDE-specific \e Logger will be named \c "IDE_LOGGER"
    /// and will be registered with the standard \b %Lox used for debug-logging with the same
    /// \e Verbosities as \c "DEBUG_LOGGER" is.
    ///
    /// Finally, this method also invokes #SetALibAssertionPlugin.
    ///
    /// @param lox  The lox to add the debug logger(s) to.
    ALIB_DLL static void      AddDebugLogger( Lox* lox );

    ///  Removes the \e Logger(s) and which was (were) created by \ref AddDebugLogger.
    ///  This method also invokes #SetALibAssertionPlugin, passing \c nullptr to unregister
    ///  the plug-in.
    ///  @param lox The lox to remove the debug logger(s) from.
    ALIB_DLL static void      RemoveDebugLogger( Lox* lox );

    #if ALIB_DEBUG
        /// Sets the global pointer \alib{assert::PLUGIN} to function
        /// \alib{lox;ALoxAssertionPlugin} which then redirects the output of
        /// \ref #alib::assert::Raise "ALib assertions" to the given \alib{lox;Lox}.
        ///
        /// \note
        ///   This method is effective only with debug-builds. Usually it is invoked indirectly by
        ///   using the method #AddDebugLogger.
        ///   Applications that do not use that method (e.g., because they are using release
        ///   logging exclusively) should invoke this method on bootstrap providing their
        ///   (release) lox.
        ///   In this case, the \e Verbosity of the internal domain used by function
        ///   \alib{lox;ALoxAssertionPlugin} has to be set for the logger(s) in the given
        ///   \p{lox} in question.
        ///
        /// @param lox  The lox that the \alib{lox;ALoxAssertionPlugin} will be using.
        ///             If \c nullptr is given, the plug-in will be removed.
        ALIB_DLL static void      SetALibAssertionPlugin( Lox* lox );
    #else
                 static void      SetALibAssertionPlugin( Lox*     )                              {}
    #endif

  #endif // ALOX_DBG_LOG
}; // class Log

} // namespace alib[::lox]

/// Type alias in namespace \b alib.
using     Log=           lox::Log;

}  // namespace [alib]



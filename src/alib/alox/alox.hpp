/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ALOX
#define HPP_ALIB_ALOX 1

#if !defined(HPP_ALIB_COMPILERS) && !defined(ALIB_DOX)
#   include "alib/lib/compilers.hpp"
#endif

ALIB_ASSERT_MODULE(ALOX)

#if !defined (HPP_ALIB_CONFIG_PRIORITIES)
    #include "alib/config/priorities.hpp"
#endif


// #################################################################################################
// Compilation Flags
// #################################################################################################

#if !defined(ALOX_DBG_LOG)
#   define ALOX_DBG_LOG    ALIB_DEBUG
#endif
#if !defined(ALOX_DBG_LOG_CI)
#   define ALOX_DBG_LOG_CI ALIB_DEBUG
#endif

#if !defined(ALOX_REL_LOG)
#   define ALOX_REL_LOG    1
#endif
#if !defined(ALOX_REL_LOG_CI)
#   define ALOX_REL_LOG_CI 0
#endif

namespace aworx { namespace lib { namespace lox {

class Lox;

/** ************************************************************************************************
 * This enum is used in \alox to control the "verbosity" or "verboseness" of the log output.
 * The values herein - apart from special value 'Off' - are sorted in the following order
 * - Verbose (highest level)
 * - Info
 * - Warning
 * - Error (lowest level).
 *
 * A value of this set is provided to \alox in two different ways:
 * First, all methods of class \ref aworx::lib::lox::Lox "Lox" that execute a log operation
 * assign a value of this enum to the <em>Log Statement</em>. Secondly, methods
 * \ref aworx::lib::lox::Lox::SetVerbosity "Lox::SetVerbosity", are defining the 'accepted'
 * <em>minimal Verbosity</em> for a pair of <em>\<Logger/%Log Domain\></em>.
 *
 * \alox, when executing a statement, checks both values against each other.
 * A <em>Log Statement</em> is executed, when the <em>\<Logger/%Log Domain\></em> setting is set
 * to the same or a 'higher level'.  For example if a <em>\<Logger/%Log Domain\></em> setting is
 * \b %Warning, then <em>Log Statements</em> with associated \e %Verbosity \b %Warning and
 * \b %Error are executed and those with \b %Info and \b %Verbose are suppressed.
 *
 * If special value \b %Off is used with \alib{lox,Lox::SetVerbosity}, all logging is switched off
 * for this pair of <em>\<Logger/%Log Domain\></em>.
 *
 * Some of the <em>Log Statements</em> accept the parameter directly (e.g.
 * \ref aworx::lib::lox::Lox::Entry "Lox::Entry",
 * \ref aworx::lib::lox::Lox::Once  "Lox::Once" and
 * \ref aworx::lib::lox::Lox::If "Lox::If"), others inherently use the right value as their method
 * name suggests (e.g.
 * \ref aworx::lib::lox::Lox::Error      "Lox::Error",
 * \ref aworx::lib::lox::Lox::Warning    "Lox::Warning",
 * \ref aworx::lib::lox::Lox::Info       "Lox::Info",
 * \ref aworx::lib::lox::Lox::Verbose    "Lox::Verbose" and
 * \ref aworx::lib::lox::Lox::Assert     "Lox::Assert"). The latter group of methods do not support
 * parameter \b %Off.
 *
 * If special value \b %Off is used with those <em>Log Statements</em>, that allow to specify the
 * \e %Verbosity as a parameter, the <em>Log Statement</em> is never executed This is useful if the
 * parameter is determined at run-time, depending on the state of an application.
 **************************************************************************************************/
enum class Verbosity
{
    /**
     * The 'highest' level of \e %Verbosity.
     * Statements with this value associated are logged only if a <em>%Log Domain</em> is set to
     * \b %Verbose as well.
     */
    Verbose,

    /**
     * The standard \e Verbosity for normal log output statements.
     * Logged if a <em>%Log Domain</em> is set to \b %Info or \b %Verbose.
     */
    Info,

    /**
     * A \e Verbosity for warning messages, hence things that might lead to errors or are not
     * welcome for other reasons, but maybe are not errors.<br>
     * Logged if a <em>%Log Domain</em> is set to \b %Warning, \b %Info or \b %Verbose.
     */
    Warning,

    /**
     * A \e Verbosity for error messages.
     * It is suppressed only if a <em>%Log Domain</em>'s setting is \b %Off.
     */
    Error,

    /**
     * Statements with this value associated are never logged (useful if \e %Verbosity is
     * evaluated at run-time). <em>%Log Domains</em> with this setting do not execute any
     * <em>Log Statement</em>.
     */
    Off
};


/** ************************************************************************************************
 * These are definitions which are used as a parameter to certain \alox methods to denote
 * the \e Scope of a setting. \e Scopes are dependent of the programming language
 * and hence differ slightly from each other in the different versions of \alox.
 *
 * This enumeration is an \alib{enums,T_EnumIsArithmetical,ALib arithmetical enum}. However,
 * the addition of values is only allowed with the last element, \b Path. By adding integer
 * values, the Nth parent directory of a source file's location are addressed. As an example,
 * invocations like this are used to select the source directory two levels above the source
 * code file for a prefix scope:
 *
 *      lox->SetPrefix( "#> ", Scope::Path + 2 );
 *
 * \note
 *   \alox for C++ implements scope mechanisms using scope information generated by the
 *   preprocessor.
 *   By default, debug logging supports such 'caller information', while release logging
 *   does not.<br>
 *   Therefore, in release-logging, the use of \e Scopes 'Path', 'Filename' and
 *   'Method' will just default to an empty scope and therefore the all reflect the same,
 *   shared scope, which is not very helpful. Therefore, for standard release logging,
 *   the use of the scope mechanisms should be be avoided, unless scope information is
 *   explicitly enabled.<br>
 *   For more information on how to change the defaults, see documentation of preprocessor
 *   symbols \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI.
 *
 *   For more information on \e Scopes consult the
 *   \https{ALox user manual,alexworx.github.io/ALox-Logging-Library/manual.html}.
 **************************************************************************************************/
enum class Scope
{
    /** Denotes the global (singleton) scope. */
    Global,

    /**
     * Denotes the actual thread as the scope. When used with <em>Scope Domains</em>,
     * 'inner' scopes can be defined optionally by multiple definitions.
     */
    ThreadOuter,

    /** Denotes the actual source file as the scope. */
    Filename,

    /** Denotes the actual method as the scope. */
    Method,

    /** Denotes the actual thread as the scope. When used with <em>Scope Domains</em>,
     *   'inner' scopes can be defined optionally by multiple definitions.              */
    ThreadInner,

    /**
     * Denotes the actual source path as the scope. By adding positive integral values
     * to this element (the enum type is an \alib{enums,T_EnumIsArithmetical,ALib arithmetical enum}),
     * 'outer' \e Scopes of this scope level itself can be defined using parent directories
     * of the path.
     */
    Path
};

/** ************************************************************************************************
 * This class defines "escape sequences" that influence the formatting of log output.
 * Specific implementations of class
 * \ref aworx::lib::lox::detail::Logger "Logger"
 * have to convert or interpret this classes definitions of escape sequences
 * when processing log data. If no formatting of the output is supported by a specific Logger
 * implementation, such logger should filter and discard escape sequences defined here.
 *
 * The sequences are similar to ANSI Escape sequences and logger classes that
 * log to 'VT100' compatible terminals will simply convert them.
 *
 * The name of the class was intentionally chosen to be short, because the escape codes
 * defined with this class will be concatenated to log strings like that:
 *
 * \snippet "ut_dox_manual.cpp"     DOX_ALOX_ESC
 *
 * \note
 *   With the introduction of own, \alox specific escape codes, software that uses ALox becomes
 *   independent from any underlying, platform-specific sequences. For example, \alox is not relying
 *   on ANSI color codes, which are not supported by colorful Windows consoles. Instead, on each
 *   platform, dedicated Loggers will perform the translation of \alox codes to platform-specific
 *   ones.
**************************************************************************************************/
class ESC
{
    public:
    #if defined(_MSC_VER)
    // MSC  (as of 12/2015):
    // C4579: in-class initialization for type 'const aworx::character[11]'
    // is not yet implemented; static member will remain uninitialized at run-time but
    // use in constant-expressions is supported

    ALIB_API static     character  RED        [4]            ; ///< Select red color for foreground.
    ALIB_API static     character  GREEN      [4]            ; ///< Select green color for foreground.
    ALIB_API static     character  YELLOW     [4]            ; ///< Select yellow color for foreground.
    ALIB_API static     character  BLUE       [4]            ; ///< Select blue color for foreground.
    ALIB_API static     character  MAGENTA    [4]            ; ///< Select magenta color for foreground.
    ALIB_API static     character  CYAN       [4]            ; ///< Select cyan color for foreground.
    ALIB_API static     character  BLACK      [4]            ; ///< Select black color for foreground.
    ALIB_API static     character  WHITE      [4]            ; ///< Select white color for foreground.
    ALIB_API static     character  GRAY       [4]            ; ///< Select gray color for foreground.
    ALIB_API static     character  FG_RESET   [4]            ; ///< Select std color for foreground.

    ALIB_API static     character  BG_RED     [4]            ; ///< Select red color for background.
    ALIB_API static     character  BG_GREEN   [4]            ; ///< Select green color for background.
    ALIB_API static     character  BG_YELLOW  [4]            ; ///< Select yellow color for background.
    ALIB_API static     character  BG_BLUE    [4]            ; ///< Select blue color for background.
    ALIB_API static     character  BG_MAGENTA [4]            ; ///< Select blue color for background.
    ALIB_API static     character  BG_CYAN    [4]            ; ///< Select blue color for background.
    ALIB_API static     character  BG_BLACK   [4]            ; ///< Select red color for background.
    ALIB_API static     character  BG_WHITE   [4]            ; ///< Select blue color for background.
    ALIB_API static     character  BG_GRAY    [4]            ; ///< Select gray color for background.
    ALIB_API static     character  BG_RESET   [4]            ; ///< Select std color for background.

    ALIB_API static     character  BOLD       [4]            ; ///< Select bold font style.
    ALIB_API static     character  ITALICS    [4]            ; ///< Select italics font style.
    ALIB_API static     character  STYLE_RESET[4]            ; ///< Select standard font style.
    ALIB_API static     character  RESET      [4]            ; ///< Reset color and style.

    ALIB_API static     character  URL_START  [4]            ; ///< Mark the start of an URL.
    ALIB_API static     character  URL_END    [4]            ; ///< Mark the end of an URL.
    ALIB_API static     character  TAB        [4]            ; ///< Go to next tab. Usually, text loggers will increase the tab position automatically.

    ALIB_API static     character  EOMETA     [4]            ; ///< End of meta information in log string

    #else
    static constexpr    character  RED        [4]{ A_CHAR("\033c0") }; ///< Select red color for foreground.
    static constexpr    character  GREEN      [4]{ A_CHAR("\033c1") }; ///< Select green color for foreground.
    static constexpr    character  YELLOW     [4]{ A_CHAR("\033c2") }; ///< Select yellow color for foreground.
    static constexpr    character  BLUE       [4]{ A_CHAR("\033c3") }; ///< Select blue color for foreground.
    static constexpr    character  MAGENTA    [4]{ A_CHAR("\033c4") }; ///< Select magenta color for foreground.
    static constexpr    character  CYAN       [4]{ A_CHAR("\033c5") }; ///< Select cyan color for foreground.
    static constexpr    character  BLACK      [4]{ A_CHAR("\033c6") }; ///< Select black color for foreground.
    static constexpr    character  WHITE      [4]{ A_CHAR("\033c7") }; ///< Select white color for foreground.
    static constexpr    character  GRAY       [4]{ A_CHAR("\033c8") }; ///< Select gray color for foreground.
    static constexpr    character  FG_RESET   [4]{ A_CHAR("\033c9") }; ///< Select std color for foreground.

    static constexpr    character  BG_RED     [4]{ A_CHAR("\033C0") }; ///< Select red color for background.
    static constexpr    character  BG_GREEN   [4]{ A_CHAR("\033C1") }; ///< Select green color for background.
    static constexpr    character  BG_YELLOW  [4]{ A_CHAR("\033C2") }; ///< Select yellow color for background.
    static constexpr    character  BG_BLUE    [4]{ A_CHAR("\033C3") }; ///< Select blue color for background.
    static constexpr    character  BG_MAGENTA [4]{ A_CHAR("\033C4") }; ///< Select blue color for background.
    static constexpr    character  BG_CYAN    [4]{ A_CHAR("\033C5") }; ///< Select blue color for background.
    static constexpr    character  BG_BLACK   [4]{ A_CHAR("\033C6") }; ///< Select red color for background.
    static constexpr    character  BG_WHITE   [4]{ A_CHAR("\033C7") }; ///< Select blue color for background.
    static constexpr    character  BG_GRAY    [4]{ A_CHAR("\033C8") }; ///< Select gray color for background.
    static constexpr    character  BG_RESET   [4]{ A_CHAR("\033C9") }; ///< Select std color for background.

    static constexpr    character  BOLD       [4]{ A_CHAR("\033sB") }; ///< Select bold font style.
    static constexpr    character  ITALICS    [4]{ A_CHAR("\033sI") }; ///< Select italics font style.
    static constexpr    character  STYLE_RESET[4]{ A_CHAR("\033sr") }; ///< Select standard font style.
    static constexpr    character  RESET      [4]{ A_CHAR("\033sa") }; ///< Reset color and style.

    static constexpr    character  URL_START  [4]{ A_CHAR("\033lS") }; ///< Mark the start of an URL.
    static constexpr    character  URL_END    [4]{ A_CHAR("\033lE") }; ///< Mark the end of an URL.
    static constexpr    character  TAB        [4]{ A_CHAR("\033t0") }; ///< Go to next tab. Usually, text loggers will increase the tab position automatically.

    static constexpr    character  EOMETA     [4]{ A_CHAR("\033A0") }; ///< End of meta information in log string

    #endif

    /** ********************************************************************************************
     * Replaces ESC codes in a string reversely to "ESC::XXX".
     * @param target   The string to replace in.
     * @param startIdx The index to start searching for ESC codes.
     **********************************************************************************************/
    ALIB_API
    static void ReplaceToReadable( AString& target, integer startIdx );
}; // class ESC

}} // namespace aworx[::lib::lox]


/// Type alias in namespace #aworx.
using   Verbosity=        lib::lox::Verbosity;

/// Type alias in namespace #aworx.
using   Scope=            lib::lox::Scope;

/// Type alias in namespace #aworx.
using   ESC=              lib::lox::ESC;

} // namespace [aworx]


ALIB_BOXING_VTABLE_DECLARE( aworx::lib::lox::Verbosity     , vt_lox_verbosity )
ALIB_BOXING_VTABLE_DECLARE( aworx::lib::lox::Scope         , vt_lox_scope     )
ALIB_BOXING_VTABLE_DECLARE( std::pair<aworx::lib::lox::Verbosity
                            ALIB_COMMA aworx::lib::config::Priorities> , vt_lox_pair_verby_prio )


ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::lox::Verbosity, aworx::lib::enums::ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD( aworx::lib::lox::Scope    , aworx::lib::enums::ERSerializable )
ALIB_ENUMS_MAKE_ARITHMETICAL( aworx::lib::lox::Scope     )



// #################################################################################################
// include Log and Lox classes
// #################################################################################################
#include "alib/alox/macros.inl"

#if (ALIB_DEBUG && ALOX_DBG_LOG) || ALOX_REL_LOG
#   include "alib/alox/log.inl"
#   include "alib/alox/lox.inl"
#endif

#endif // HPP_ALIB_ALOX


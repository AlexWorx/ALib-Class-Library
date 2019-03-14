// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_ALOX
#define HPP_ALIB_ALOX 1

#if !defined(HPP_ALOX_PREDEF)
#   include "alib/alox/alox_predef.hpp"
#endif

ALIB_ASSERT_MODULE(ALOX)

#if !defined (HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif

#if !defined(HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
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
 *   For more information on how to change the defaults, see \ref ALOX_DBG_LOG_CI and
 *   \ref ALOX_REL_LOG_CI in section \ref GrpALibCodeSelectorSymbols.
 *   For more information on \e Scopes consult the [ALox user manual](https://alexworx.github.io/ALox-Logging-Library/manual.html).
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
     * Denotes the actual source path as the scope. By adding positive integer values
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
};

/** ************************************************************************************************
 * This is the module class for \alox logging library residing in namespace \ref aworx::lib::lox.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::lib::ALOX.
 **************************************************************************************************/
class ALox : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        ALox();

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * This is the path for logging to the internal domain. By manipulating this
         *  <em>%Log %Domain's %Verbosity</em>, the verbosity of \alox itself can be controlled.
         * For example, with \e Verbosity.INFO, the 'on the fly' creation of <em>Log Domains</em>
         * are logged, which can be helpful to determine the <em>Lo                                                                 bg Domains</em> that are
         * created by libraries and larger projects.
         *
         * The following sub-domains are used by \alox:
         *
         *   Sub-Domain | Description
         *   - - - - - -| - - - - - - - - - - - - - - - - - - -
         *   LGR        | Used when \e %Loggers are registered, retrieved or removed from a \b %Lox and when the \e Verbosity of a <em>Log Domain</em> for a \e %Logger is changed.<br>In addition used with method \b %Lox::SetStartTime.
         *   DMN        | Used when <em>Log Domains</em> are registered (on first use), when <em>Scope Domains</em> are set or removed and when <em>Domain Substitution Rules</em> are set.
         *   PFX        | Used when <em>Prefix Logables</em> are set or removed.
         *   THR        | Used with method \b %Lox::MapThreadName.
         *   LGD        | Used with storing and retrieving <em>Log Data</em> objects.
         *
         * In addition, class \alox{ALoxReportWriter} logs into sub-domain <c>'REPORT'</c>.
         *
         * \note For internal logging an separated <em>domain tree</em> is used. This means, that
         *       setting the root domain of a \b %Lox to a certain \e Verbosity does \e not affect
         *       the internal domains. In other words, the \e Verbosity of the internal domain
         *       (or one of its sub-domains) has to be set explicitly.
         */
        static constexpr  NString  InternalDomains                                         { "$/" };

        #if defined(_WIN32)
        /** Attributes of corresponding
         *  [Configuration variable](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html)
         *  used by class \alox{loggers,WindowsConsoleLogger}.
         */
        ALIB_API static  lib::config::VariableDecl CODEPAGE;
        #endif

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        #if ALOX_DBG_LOG
            /** The Lox singleton for debug logging. Created on request. */
            Lox*                            theLog                                        = nullptr;

        #endif

        /** The Lox objects registered with us. */
        std::vector<Lox*>                   loxes;


    // #############################################################################################
    // Lox management
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the default singleton of class \b %Lox used for debug logging
         * @return The debug-logging Lox of \alox
         ******************************************************************************************/
        static ALIB_API
        Lox*     Log();

        /** ****************************************************************************************
         * Returns a \b %Lox with the given name.
         * A \b %Lox is only found if it was created and registered with \alox using #Register.
         * If not found, and parameter \p{create} is \c true (the default), a new \b Lox is created,
         * registered and returned.
         *
         * @param name      The name of the \b %Lox to search and optionally to create.
         *                  Comparison is case insensitive.
         * @param create    Denotes whether a \b %Lox that was not found is created.
         *                  Optional and defaults to \b %Create::Never.
         * @return The \b Lox found, \c nullptr in case of failure.
         ******************************************************************************************/
        ALIB_API
        Lox*     Get( const NString& name, Create create= Create::Never );

        /** ****************************************************************************************
         * Registers or un-registers a \b %Lox object statically with \alox.
         * Once registered,  any code entity of the same process is enabled to retrieve
         * the \b %Lox using #Get.<br>
         * No two objects with the same name must be registered. If this is done, the latter
         * will not be registered and not be found by #Get. In debug-compilations, an \alib
         * error report is written (by default raises 'assert') if a name is registered twice.<br>
         * Note that name comparison is performed case <b>in</b>-sensitive.
         *
         * If debug-logging is enabled (depends on compilation symbols) and used, the singleton
         * of type \% Lox provided for debug-logging is registered. This uses the name \c "Log"
         *
         * Registration is \e not mandatory but done by default by the constructor of class \b %Lox.
         * Therefore, to keep a \b Lox private, an optional parameter is available.
         *
         * @param lox       The \b %Lox to register.
         * @param operation If \b %ContainerOp::Remove, the given \p{Lox} is deregistered.
         *                  Defaults to \b %ContainerOp::Insert.
         ******************************************************************************************/
        ALIB_API
        void     Register( Lox* lox, ContainerOp operation );

        /** ****************************************************************************************
         * Resets this object. Concretely the following steps are performed:
         * - All \b %Lox objects are removed.
         * - If the debug lox singleton exists, it is deleted.
         *
         * \attention
         *   This method was introduced to support resetting \alox in the unit tests.
         *   In real applications, this method should NOT be used.
         *   Side effects might appear using this method and it is not tested otherwise than
         *   used in tests!
         ******************************************************************************************/
        ALIB_API
        void     Reset();

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \alib{Module::init}.
         *
         * In phase \alib{Module::InitLevels::Final} performs:
         * - Adds box-function \alib{boxing,FAppend::Appendable} for type
         *   \alox{detail,Logger}.
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t** wargv )     override;

        /** ****************************************************************************************
         * Implementation of \alib{Module::terminationCleanUp}.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

};// class ALox

} // namespace aworx::lib[::lox]
/** The singleton of module-class \alib{lox,ALox}. */
extern ALIB_API lox::ALox ALOX;
} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using   ALox=             aworx::lib::lox::ALox;

/// Type alias in namespace #aworx.
using   Verbosity=        aworx::lib::lox::Verbosity;

/// Type alias in namespace #aworx.
using   Scope=            aworx::lib::lox::Scope;

/// Type alias in namespace #aworx.
using   ESC=              aworx::lib::lox::ESC;



}  // namespace [aworx]

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::lox::Verbosity     , vt_lox_verbosity )
ALIB_BOXING_VTABLE_DECLARE( aworx::lib::lox::Scope         , vt_lox_scope     )
ALIB_BOXING_VTABLE_DECLARE( std::pair<aworx::lib::lox::Verbosity
                           ALIB_COMMA aworx::lib::config::Priorities> , vt_lox_pair_verby_prio )

ALIB_ENUM_PARSABLE( aworx::lib::lox::Verbosity )
ALIB_RESOURCED_IN_MODULE(aworx::lib::lox::Verbosity, aworx::lib::ALOX, "Verbosity" )
ALIB_ENUM_PARSABLE( aworx::lib::lox::Scope     )
ALIB_RESOURCED_IN_MODULE(aworx::lib::lox::Scope    , aworx::lib::ALOX, "Scope" )

ALIB_ENUM_IS_ARITHMETICAL( aworx::lib::lox::Scope     )


// #################################################################################################
// T_Append<Scope> / T_Append<Verbosity>
// #################################################################################################
//! @cond NO_DOX
namespace aworx { namespace lib { namespace strings
{
    template<> struct       T_Append<Scope,nchar>
    {
        inline void operator()( TAString<nchar>& target, const Scope src )
        {
            Scope scope= src;
            int pathLevel= EnumValue( scope - Scope::Path );
            if(pathLevel > 0 )
                scope= Scope::Path;

            auto& enumMetaData= EnumMetaData<Scope>::GetSingleton();
            enumMetaData.CheckLoad();
            target << "Scope::" << std::get<1>(*enumMetaData.Get( scope ) );

            if( pathLevel > 0 )
                target << '+' << pathLevel;
        }
    };

    template<> struct       T_Append<std::pair<Verbosity, Priorities>,nchar>
    {
        inline void operator()( TAString<nchar>& target, const std::pair<Verbosity, Priorities>& src )
        {
            target._( NFormat::Field( src.first, 7, Alignment::Left) );
            target._( '(' )._( src.second );
            target.InsertAt( ")", target.LastIndexOfAny<Inclusion::Exclude>( NDefaultWhitespaces() )  + 1 );
        }
    };
}}}
//! @endcond


// #################################################################################################
// include Log and Lox classes
// #################################################################################################
#define HPP_ALIB_LOX_PROPPERINCLUDE
#   include "alib/alox/log.inl"
#   include "alib/alox/lox.inl"
#   include "alib/alox/macros.inl"
#undef HPP_ALIB_LOX_PROPPERINCLUDE

#endif // HPP_ALIB_ALOX


// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

// needed for Doxygen include
/// @file

#ifndef HPP_ALIB_ALOX
#define HPP_ALIB_ALOX 1


// #################################################################################################
// include ALIB
// #################################################################################################
#if !defined(HPP_ALIB)
#   include "alib/alib.hpp"
#endif

// #################################################################################################
// Now we first process external conditional compilation symbols
// and then we define our prepro logging macros accordingly.
// #################################################################################################

#if defined(HPP_ALOX_SYMBOLS)
    #error "internals/alox_symbols.hpp must not be included manually. Include alox.hpp instead"
#endif
#include "internals/alox_symbols.hpp"

#if defined(HPP_ALOX_MACROS)
    #error "internals/alox_macros must not be included manually. Include alox.hpp instead"
#endif
#include "internals/alox_macros.hpp"

// #### other includes ####
#if !defined(HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif

namespace aworx { namespace lib { namespace lox {

class Lox;

/** ************************************************************************************************
 * This is the library class for \alox logging library residing in namespace \ref aworx::lib::lox.
 *
 * As with every library implementation of \alibmods, a static singleton of this class exists with
 * #aworx::lib::ALOX. Furthermore, the initialization of this singleton is automatically performed
 * with the initialization of singleton #aworx::lib::ALIB.
 **************************************************************************************************/
class ALox : public lib::lang::Library
{
    protected:
        /// A lock for Lox creation and registration.
        ThreadLockNR              loxManagement;


    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /**
         * This is the path for logging to the internal domain. By manipulating this
         *  <em>%Log %Domain's %Verbosity</em>, the verbosity of \alox itself can be controlled.
         * For example, with \e Verbosity.INFO, the 'on the fly' creation of <em>Log Domains</em>
         * are logged, which can be helpful to determine the <em>Log Domains</em> that are
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
         * In addition, class
         * \ref aworx::lib::lox::ALoxReportWriter "ALoxReportWriter" logs into sub-domain
         * <c>'REPORT'</c>.
         *
         * \note For internal logging an separated <em>domain tree</em> is used. This means, that
         *       setting the root domain of a \b %Lox to a certain \e Verbosity does \e not affect
         *       the internal domains. In other words, the \e Verbosity of the internal domain
         *       (or one of its sub-domains) has to be set explicitly.
         */
        #if defined(_MSC_VER)
            // MSC  (as of 12/2015):
            // C4579: in-class initialization for type 'const aworx::SLiteral<2>'
            // is not yet implemented; static member will remain uninitialized at runtime but
            // use in constant-expressions is supported

            ALIB_API static   NSLiteral<2>  InternalDomains;
        #else
            static constexpr NSLiteral<2>   InternalDomains                                { "$/" };
        #endif

        #if defined(_WIN32)
        ALIB_API static  lib::config::VariableDecl CODEPAGE;      ///< Attributes of corresponding [configuration variable](../group__GrpALoxConfigVars.html) used by class \ref aworx::lib::lox::loggers::WindowsConsoleLogger "WindowsConsoleLogger".
        #endif

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        #if ALOX_DBG_LOG
            /// The Lox singleton for debug logging. Created on request.
            Lox*                            theLog                                        = nullptr;

        #endif

        /// The Lox objects registered with us.
        std::vector<Lox*>                   loxes;

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        ALIB_API                            ALox();

    // #############################################################################################
    // Lox management
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns a singleton of class \b %Lox used for debug logging
         * @return The debug-logging Lox of \alox
         ******************************************************************************************/
        ALIB_API
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
    // Internals4
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \alib{lang,Library::init}.
         *
         * In phase \alib{lang,Library::Phases::final} performs:
         * - Adds boxing interface \alib{strings::boxing,IApply_TApplicable} for type
         *   \alox{core,Logger}.
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Implementation of \alib{lang,Library::terminationCleanUp}.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

};// class ALox


}
/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::lox.
 **************************************************************************************************/
ALIB_API
extern lox::ALox   ALOX;
namespace lox {

/** ************************************************************************************************
 * Configuration variables uses by ALox
 **************************************************************************************************/
enum class Variables
{
    NO_IDE_LOGGER        = 1,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by \alox{Log::AddDebugLogger}.
    CONSOLE_TYPE         = 2,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by \alox{Lox::CreateConsoleLogger}.
    VERBOSITY            = 3,  ///< Configuration variable [ALOX_LOXNAME_LOGGERNAME_VERBOSITY](../group__GrpALoxConfigVars.html).
    SPTR_GLOBAL          = 4,  ///< Configuration variable [ALOX_GLOBAL_SOURCE_PATH_TRIM_RULES](../group__GrpALoxConfigVars.html).
    SPTR_LOX             = 5,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    DOMAIN_SUBSTITUTION  = 6,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    PREFIXES             = 7,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    DUMP_STATE_ON_EXIT   = 8,  ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{Lox}.
    AUTO_SIZES           = 20, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    FORMAT               = 21, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    FORMAT_DATE_TIME     = 22, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    FORMAT_MULTILINE     = 23, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    FORMAT_TIME_DIFF     = 24, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    MAX_ELAPSED_TIME     = 25, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    REPLACEMENTS         = 26, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{core::textlogger,TextLogger}.
    CONSOLE_LIGHT_COLORS = 27, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by colorful specializations of class \alox{core::textlogger,TextLogger}.

    #if defined(_WIN32)
      CODEPAGE           = 28, ///< [Configuration variable](../group__GrpALoxConfigVars.html) used by class \alox{loggers,WindowsConsoleLogger}.
    #endif
};




/** ************************************************************************************************
 * The \b %ReportWriter for \alib when using \alox. An instance of this class is created
 * in method \ref aworx::lib::lox::Log::AddDebugLogger "Log::AddDebugLogger"
 * and registered with \alib.<br>
 * Uses internal domain <c>'$/REPORT'</c> for logging.
 **************************************************************************************************/
class    ALoxReportWriter : public aworx::lib::lang::ReportWriter
{
    protected:
        /// The \b Lox to report to
        Lox*                lox;

        /// The internal domain to report into
        static NString16    reportDomain;

    public:
    /** ********************************************************************************************
     * Constructs an \b %AloxReportWriter.
     * @param lox    The \b Lox to report to.
     **********************************************************************************************/
     ALIB_API
     ALoxReportWriter ( Lox* lox );

    /** ****************************************************************************************
     * Virtual destructor
     ******************************************************************************************/
    virtual ~ALoxReportWriter()              {}

    /** ********************************************************************************************
     * Notify activation/deactivation
     **********************************************************************************************/
    virtual void NotifyActivation  ( Phase ) { }


    /** ********************************************************************************************
     * Write \alib reports using \alox.
     * @param msg The message to log.
     **********************************************************************************************/
     virtual void Report  ( const lib::lang::Report::Message& msg );

    /** ********************************************************************************************
     * Returns the domain used to write reports.
     * @return The report log domain.
     **********************************************************************************************/
     ALIB_API
     static NString& LogDomain();
};// class ALoxReportWriter

}} // namespace aworx[::lib::lox]

/// Type alias in namespace #aworx.
using     ALox=                 aworx::lib::lox::ALox;

/// Type alias in namespace #aworx.
using     ALoxReportWriter=     aworx::lib::lox::ALoxReportWriter;

}  // namespace aworx


ALIB_CONFIG_VARIABLES( aworx::lib::lox::Variables, aworx::lib::ALOX, ASTR("Var") )

// #################################################################################################
// include Log and Lox classes
// #################################################################################################
#include "aloxtypesandenums.hpp"
#include "log.hpp"
#include "lox.hpp"


#endif // HPP_ALIB_ALOX

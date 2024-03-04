/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ALOXMODULE
#define HPP_ALIB_ALOXMODULE 1

#if !defined (HPP_ALIB_ALOX)
#   include "alib/alox/alox.hpp"
#endif

ALIB_ASSERT_MODULE(ALOX)

#if !defined (HPP_ALOX_DETAIL_TEXTLOGGER_TEXTLOGGER)
    #include "alib/alox/detail/textlogger/textlogger.hpp"
#endif

#if !defined (HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif

#if !defined(HPP_ALIB_CONFIG_CONFIG)
#   include "alib/config/config.hpp"
#endif

#if !defined(HPP_ALIB_CONFIG_CONFIGURATION)
#   include "alib/config/configuration.hpp"
#endif

namespace alib {  namespace lox {

/** ************************************************************************************************
 * Configuration variables uses by ALox.
 * \note
 *  As required by module \alib_config, this enumerations is equipped with
 *  \ref alib_enums_records "ALib Enum Records" of type \alib{config,VariableDecl}  and resourced
 *  with this module \alib_alox_nl.<br>
 *  Hence, all variable categories, names, default values and such can be modified
 *  by modifying the resource data of the singleton of class \alib{lang::basecamp,BaseCamp}.
 **************************************************************************************************/
enum class Variables
{
    /** Denotes configuration variable \ref alxcvALOX_NO_IDE_LOGGER used by \alox{Log::AddDebugLogger}. */
    NO_IDE_LOGGER        = 1,

    /** Denotes configuration variable \ref alxcvALOX_CONSOLE_TYPE used by \alox{Lox::CreateConsoleLogger}. */
    CONSOLE_TYPE         = 2,

    /** Denotes configuration variable \ref alxcvALOX_LOXNAME_LOGGERNAME_VERBOSITY. */
    VERBOSITY            = 3,

    /** Denotes configuration variable \ref alxcvALOX_GLOBAL_SOURCE_PATH_TRIM_RULES. */
    SPTR_GLOBAL          = 4,

    /** Denotes configuration variable \ref alxcvALOX_LOXNAME_SOURCE_PATH_TRIM_RULES used by class \alox{Lox}. */
    SPTR_LOX             = 5,

    /** Denotes configuration variable \ref alxcvALOX_LOXNAME_DOMAIN_SUBSTITUTION used by class \alox{Lox}. */
    DOMAIN_SUBSTITUTION  = 6,

    /** Denotes configuration variable \ref alxcvALOX_LOXNAME_PREFIXES used by class \alox{Lox}. */
    PREFIXES             = 7,

    /** Denotes configuration variable \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT used by class \alox{Lox}. */
    DUMP_STATE_ON_EXIT   = 8,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_AUTO_SIZES used by class \alox{detail::textlogger,TextLogger}. */
    AUTO_SIZES           = 20,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT used by class \alox{detail::textlogger,TextLogger}. */
    FORMAT               = 21,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_DATE_TIME used by class \alox{detail::textlogger,TextLogger}. */
    FORMAT_DATE_TIME     = 22,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_MULTILINE used by class \alox{detail::textlogger,TextLogger}. */
    FORMAT_MULTILINE     = 23,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_FORMAT_TIME_DIFF used by class \alox{detail::textlogger,TextLogger}. */
    FORMAT_TIME_DIFF     = 24,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_MAX_ELAPSED_TIME used by class \alox{detail::textlogger,TextLogger}. */
    MAX_ELAPSED_TIME     = 25,

    /** Denotes configuration variable \ref alxcvALOX_LOGGERNAME_REPLACEMENTS used by class \alox{detail::textlogger,TextLogger}. */
    REPLACEMENTS         = 26,

    /** Denotes configuration variable \ref alxcvALOX_CONSOLE_LIGHT_COLORS used by colorful specializations of class \alox{detail::textlogger,TextLogger}. */
    CONSOLE_LIGHT_COLORS = 27,

    #if defined(_WIN32)
        /** Denotes configuration variable \ref alxcvALOX_CODE_PAGE used by class \alox{loggers,WindowsConsoleLogger}. */
        CODEPAGE           = 28,
    #endif
};

/** ************************************************************************************************
 * This is the module class for \alox logging library residing in namespace \ref alib::lox.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref alib::ALOX.
 **************************************************************************************************/
class ALox : public lang::Camp
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_camp_modules_campmodule).
         ******************************************************************************************/
        ALox();

    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        #if defined(_WIN32)
            /** Attributes of corresponding configuration variable \ref alxcvALOX_CODE_PAGE
             *  used by class \alox{loggers,WindowsConsoleLogger}.              */
            ALIB_API static  config::VariableDecl CODEPAGE;
        #endif

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:

    // #############################################################################################
    // Lox management
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns a \b %Lox with the given name.
         * A \b %Lox is only found if it was created and registered with \alox using #Register.
         * If not found, and parameter \p{create} is \c true (the default), a new \b Lox is created,
         * registered and returned.
         *
         * @param name      The name of the \b %Lox to search and optionally to create.
         *                  Comparison is case insensitive.
         * @param create    Denotes whether a \b %Lox that was not found is created.
         *                  Optional and defaults to \b %CreateIfNotExists::No.
         * @return The \b Lox found, \c nullptr in case of failure.
         ******************************************************************************************/
        ALIB_API Lox*   Get( const NString&          name,
                             lang::CreateIfNotExists create= lang::CreateIfNotExists::No );

        /** ****************************************************************************************
         * Registers or un-registers a \b %Lox object statically with \alox.
         * Once registered,  any code entity of the same process is enabled to retrieve
         * the \b %Lox using #Get.<br>
         * No two objects with the same name must be registered. If this is done, the latter
         * will not be registered and not be found by #Get. In debug-compilations, an \alib
         * error report is written (by default raises 'assert') if a name is registered twice.<br>
         * Note that name comparison is performed case <b>in</b>-sensitive.
         *
         * If debug-logging is enabled (depends on optional compiler symbols) and used, the
         * singleton of type \% Lox provided for debug-logging is registered. This uses the name
         * \c "Log".
         *
         * Registration is \e not mandatory but done by default by the constructor of class \b %Lox.
         * Therefore, to keep a \b Lox private, an optional parameter is available.
         *
         * @param lox       The \b %Lox to register.
         * @param operation If \b %ContainerOp::Remove, the given \p{Lox} is deregistered.
         *                  Defaults to \b %ContainerOp::Insert.
         ******************************************************************************************/
        ALIB_API void   Register( Lox* lox, lang::ContainerOp operation );

        /** ****************************************************************************************
         * Resets this object. Concretely the following steps are performed:
         * - If the debug lox singleton exists, it is deleted.
         * - It is asserted that no other lox object is registered
         * - The ALOX section of the configuration plugins are deleted
         * - It is asserted that not more than 4 plugins are in the configuration
         *
         * \attention
         *   This method was introduced to support resetting \alox in the unit tests.
         *   In real applications, this method should NOT be used.
         *   Side effects might appear using this method and it is not tested otherwise than
         *   used in tests!
         ******************************************************************************************/
        ALIB_API void   Reset();

    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of \alib{lang,Camp::bootstrap}.
         *
         * In phase \alib{BootstrapPhases::Final} performs:
         * - Adds box-function \alib{boxing,FAppend::Appendable} for type
         *   \alox{detail,Logger}.
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        /** ****************************************************************************************
         * Terminates this module.
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                   override;

};// class ALox

} // namespace alib[::lox]

/** The singleton instance of \alibcamp class \alib{lox,ALox}. */
extern ALIB_API lox::ALox ALOX;


/// Type alias in namespace \b alib.
using   ALox=             lox::ALox;

}  // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD( alib::lox::Variables, alib::config::VariableDecl )
ALIB_RESOURCED_IN_MODULE( alib::lox::Variables, alib::ALOX, "Var" )

// #################################################################################################
// T_Append<Scope> / T_Append<Verbosity>
// #################################################################################################
#if !defined(ALIB_DOX)
namespace alib::strings {

template<> struct       T_Append<Scope,nchar>
{
    ALIB_API
    void operator()( TAString<nchar>& target, const Scope src );
};

template<> struct       T_Append<std::pair<Verbosity, Priorities>,nchar>
{
    ALIB_API
    void operator()( TAString<nchar>& target, const std::pair<Verbosity, Priorities>& src );
};

} // namespace [alib::strings]
#endif // !defined(ALIB_DOX)

#endif // HPP_ALIB_ALOXMODULE

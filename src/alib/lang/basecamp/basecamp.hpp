/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_LANG_BASECAMP
#define HPP_ALIB_LANG_BASECAMP 1

#if !defined(HPP_ALIB_CAMP_ERRORS)
#   include "alib/lang/system/systemerrors.hpp"
#endif

ALIB_ASSERT_MODULE(CAMP)

#if !defined(HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif

#if ALIB_CONFIGURATION && !defined(HPP_ALIB_CONFIG_VARIABLEDECL)
#   include "alib/config/variabledecl.hpp"
#endif


namespace alib { namespace lang {

namespace system{ class CalendarDateTime; }

/** ************************************************************************************************
 * This is the reference documentation of module \alib_basecamp_nl of the \aliblong.<br>
 * Besides the few entities found and documented here (namely in namespace
 * <em>%alib::lang::basecamp</em>), this module <b>"injects"</b> the following entities to different
 * places:
 *
 * Into namespace <b>%alib</b>:
 * - enum class \ref alib::BootstrapPhases
 * - enum class \ref alib::ShutdownPhases
 * - overload function \ref alib::Bootstrap( BootstrapPhases\, lang::Camp*\, int\, int\, uint64_t );
 * - overload function \ref alib::Shutdown(  ShutdownPhases \, lang::Camp*);
 *
 * Into namespace <b>%alib::lang</b>
 * - class \ref alib::lang::Camp
 * - class \ref alib::lang::Exception
 * - class \ref alib::lang::Report
 * - class \ref alib::lang::ReportWriter
 *
 * - namespace <b>%alib::lang::basecamp</b> (this one)
 * - namespace \ref alib::lang::format
 * - namespace \ref alib::lang::resources
 * - namespace \ref alib::lang::system
 *
 *
 *
 * \note
 *   This constitutes an explicit exclamation from the general \alib rules: All other
 *   \alibmods reside in their own, dedicated single namespace (of-course optionally with sub-namespaces)
 *   within namespace \ref alib. The justification and explanation for this is given with
 *   chapter \ref alib_manual_camp_modules "3. ALib Camps and special Module BootCamp" of the
 *   Programmer's Manual of \alib.
 *   
 * @see
 * Please also consult the \ref alib_mod_basecamp "Programmer's Manual" of this module.
 **************************************************************************************************/
namespace basecamp {


// #################################################################################################
// Variables
// #################################################################################################
#if ALIB_CONFIGURATION
    /**
     * Configuration variables of \alib.
     * \note
     *  As required by module \alib_config, this enumerations is equipped with
     *  \ref alib_enums_records "ALib Enum Records" of type \alib{config,VariableDecl}
     *  and resourced with this module.<br>
     *  Hence, all variable categories, names, default values and such can be modified
     *  by modifying the resource data of the singleton of class \alib{lang::basecamp,BaseCamp}.
     */
    enum class Variables
    {
        LOCALE               = 1   , ///< See \ref alibcvALIB_LOCALE.
        WAIT_FOR_KEY_PRESS   = 2   , ///< See \ref alibcvALIB_WAIT_FOR_KEY_PRESS.
        HAS_CONSOLE_WINDOW   = 3   , ///< See \ref alibcvALIB_HAS_CONSOLE_WINDOW.
    };
#endif // ALIB_CONFIGURATION



/** ************************************************************************************************
 * This is a strict singleton class representing module \alib_basecamp.
 * The only instance is found with namespace variable \ref alib::BASECAMP.
 *
 * @see
 *  For detailed descriptions, please consult
 *  - Chapter \ref alib_manual_camp_modules "3. ALib Camps and special Module BootCamp", as well as
 *  - this camps' \ref alib_mod_basecamp "Programmer's Manual".
 *  
 * ## Friends ##
 * class \alib{lang::system,CalendarDateTime}
 **************************************************************************************************/
class BaseCamp : public lang::Camp
{
    #if !defined(ALIB_DOX)
        friend class alib::lang::system::CalendarDateTime;
    #endif

    public:
        /**
         * If \c true, within #Shutdown, it is waited for a key press in the console
         * window.<br>
         * By default, this flag is enabled when debugging a console application under Visual
         * Studio.<br>
         * This default behavior can be overruled by setting configuration variable
         * \ref alibcvALIB_WAIT_FOR_KEY_PRESS.<br>
         * In addition, this public flag may be modified at run-time (after method #Bootstrap was
         * invoked).
         */
        bool                    WaitForKeyPressOnTermination                                 =false;

        /**
         * This flag indicates if the current process has console output window attached.
         * While this is not detectable in Unix like systems (or just with huge efforts), in
         * windows it is. The determination is performed in #bootstrap and done as follows
         *  - If configuration variable \b ALIB_HAS_CONSOLE_WINDOW
         *    is set, its value is returned.
         *  - Otherwise, on Unix like OS\c true is returned.
         *  - On Windows OS it is determined if a console window is attached to the process
         *    using system call \e GetConsoleWindow() and compare its result to \e NULL.
         *
         * In addition, this public flag may be modified at run-time (after method #Bootstrap was
         * invoked).
         *
         * \note
         *   The evaluation of configuration variable \b ALIB_HAS_CONSOLE_WINDOW is done
         *   only if module \alib_config is included in the \alibdist.
         */
        bool                    HasConsoleWindow;

        /**
         * This string is set during bootstrapping in the case that a locale information was
         * found (via standard environment variables or \alib variable \alib{lang::basecamp::Variables,LOCALE})
         * and successfully set.<br>
         * Otherwise this string is \e nulled.
         */
        String                  LocaleFound;

         /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_camp_modules_campmodule).
         ******************************************************************************************/
        ALIB_API
        BaseCamp();

    protected:
        /** ****************************************************************************************
         * Implementation of \alib{lang,Camp::bootstrap}.
         *
         * In phase \alib{BootstrapPhases::Final} performs:
         * - In glibc versions of \alib (GNU/unix) probably invoke glibc method
         *   <em>setlocale()</em>, depending on the setting of the environment variables
         *   <em>LANG</em> and <em>LANGUAGE</em> and depending on \alib configuration variable
         *   \ref alibcvALIB_LOCALE.
         *
         *   In addition, invokes
         *   \alib{strings,TNumberFormat::SetFromLocale,NumberFormat::SetFromLocale} on static singleton
         *   \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * - Config variable \ref WAIT_FOR_KEY_PRESS is read and field
         *   #WaitForKeyPressOnTermination set accordingly.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        ALIB_API
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        /** ****************************************************************************************
         * Terminates this module. (Nothing to do.)
         ******************************************************************************************/
        ALIB_API
        virtual void    shutdown( ShutdownPhases )                                         override;

    // #############################################################################################
    // Environment definition/detection
    // #############################################################################################
    public:
        /** ************************************************************************************
         * If \c true, the current process is under the hood of a debugger.
         *
         * @return \c true if the application is run in a debugger.
         **************************************************************************************/
        ALIB_API
        bool        IsDebuggerPresent();


}; // class BaseCamp

}} // namespace alib[::lang::basecamp]

/** The singleton instance of \alibcamp class \alib{lang::basecamp,BaseCamp}. */
extern ALIB_API lang::basecamp::BaseCamp BASECAMP;

} // namespace [alib]

ALIB_BOXING_VTABLE_DECLARE(   alib::lang::system::SystemExceptions, vt_system_exceptions    )
ALIB_ENUMS_ASSIGN_RECORD(     alib::lang::system::SystemExceptions, alib::lang::ERException )
ALIB_RESOURCED_IN_MODULE(     alib::lang::system::SystemExceptions, alib::BASECAMP, "E"     )

ALIB_BOXING_VTABLE_DECLARE(   alib::lang::system::SystemErrors , vt_system_systemerrors     )
ALIB_ENUMS_ASSIGN_RECORD(     alib::lang::system::SystemErrors , alib::lang::ERException    )
ALIB_RESOURCED_IN_MODULE(     alib::lang::system::SystemErrors , alib::BASECAMP, "SE"       )

#if ALIB_CONFIGURATION
    ALIB_ENUMS_ASSIGN_RECORD( alib::lang::basecamp::Variables, alib::config::VariableDecl )
    ALIB_RESOURCED_IN_MODULE( alib::lang::basecamp::Variables, alib::BASECAMP, "Var" )
#endif



#endif // HPP_ALIB_LANG_BASECAMP

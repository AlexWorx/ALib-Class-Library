/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_CONFIG
#define HPP_ALIB_CONFIG_CONFIG 1

#if !defined (HPP_ALIB_CONFIG_PRIORITIES)
    #include "alib/config/priorities.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ARITHMETICAL)
    #include "alib/enums/arithmetical.hpp"
#endif


namespace aworx { namespace lib { namespace config {


/** ************************************************************************************************
 * Exception codes of namespace #aworx::lib::config.
 **************************************************************************************************/
enum class Exceptions
{
    /** File not found when reading. */
    ErrorOpeningFile= 1,

    /** An error occurred writing the file . */
    ErrorWritingFile= 2,
};


/** ************************************************************************************************
 * The module class.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::lib::CONFIG.
 **************************************************************************************************/
class Config : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        Config();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param phase  The initialization phase to perform.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase,
                                   int argc, const char** argv, const wchar_t** wargv )    override;

        /** ****************************************************************************************
         * Terminates this module. (Nothing to do.)
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                 override
        { (void) phase; }

}; // class Config

} // namespace aworx::lib[::config]

/** The singleton of module-class \alib{config,Config}. */
extern ALIB_API config::Config CONFIG;

}} // namespace [aworx::lib]


namespace aworx { namespace lib { namespace config {



}}} // namespace [aworx::lib::config]


ALIB_BOXING_VTABLE_DECLARE( aworx::lib::config::Exceptions, vt_config_exceptions )
ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::config::Exceptions, aworx::lib::results::ERException  )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::config::Exceptions, aworx::lib::CONFIG,  "E" )

#endif // HPP_ALIB_CONFIG_CONFIG

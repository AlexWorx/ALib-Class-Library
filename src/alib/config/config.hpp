/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_CONFIG
#define HPP_ALIB_CONFIG_CONFIG 1

#if !defined (HPP_ALIB_CONFIG_PRIORITIES)
    #include "alib/config/priorities.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined(HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif

#if !defined (HPP_ALIB_CAMP_MESSAGE_EXCEPTION)
    #include "alib/lang/message/exception.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ARITHMETICAL)
    #include "alib/enums/arithmetical.hpp"
#endif


namespace alib {  namespace config {


/** ************************************************************************************************
 * Exception codes of namespace #alib::config.
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
 * \ref  alib::CONFIG.
 **************************************************************************************************/
class Config : public lang::Camp
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_camp_modules_campmodule).
         ******************************************************************************************/
        Config();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        /** ****************************************************************************************
         * Terminates this module. (Nothing to do.)
         * @param phase  The shutdown phase to perform.
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases phase )                                 override
        { (void) phase; }

}; // class Config

} // namespace alib[::config]


/** The singleton instance of \alibcamp class \alib{config,Config}. */
extern ALIB_API config::Config CONFIG;

} // namespace [alib]



ALIB_BOXING_VTABLE_DECLARE( alib::config::Exceptions, vt_config_exceptions )
ALIB_ENUMS_ASSIGN_RECORD(   alib::config::Exceptions, alib::lang::ERException  )
ALIB_RESOURCED_IN_MODULE(   alib::config::Exceptions, alib::CONFIG,  "E" )

#endif // HPP_ALIB_CONFIG_CONFIG

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SYSTEM_SYSTEM
#define HPP_ALIB_SYSTEM_SYSTEM 1

#if !defined(HPP_ALIB_SYSTEM_ERRORS)
#   include "alib/system/systemerrors.hpp"
#endif

ALIB_ASSERT_MODULE(SYSTEM)

#if !defined(HPP_ALIB_FS_MODULES_MODULE)
#   include "alib/lib/fs_modules/module.hpp"
#endif

namespace aworx { namespace lib {

/** ************************************************************************************************
 * Types found in this namespace comprise module \alib_system_nl. This module collects types
 * and functions that are interfacing with the operating system, hardware drivers etc.
 *
 * \note
 *   As of today, the term "collects" unfortunately expresses very well that this module and
 *   namespace is not developed with a plan and road map. Instead, types are added when other
 *   modules / software need them. While this hopefully changes in the future, it has to be
 *   stated that the functionality found in this module is rather limited.
 **************************************************************************************************/
namespace system {

/** ************************************************************************************************
 * This is a strict singleton class representing module \alib_system.
 *
 * The only instance is found with namespace variable \ref aworx::lib::SYSTEM.
 *
 * ## Friends ##
 * class \alib{system,CalendarDateTime}
 **************************************************************************************************/
class System : public Module
{
    #if !defined(ALIB_DOX)
        friend class CalendarDateTime;
    #endif

    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_module_singletons).
         ******************************************************************************************/
        System();

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

}; // class System



/** ************************************************************************************************
 * Implementation of \alib{text,FFormat} for boxable type \alib{time,DateTime}.<br>
 * Writes the content of \p{box} (which is of type \b %DateTime) to the given \b %AString
 * object \p{target} using a local instance of class \alib{system,CalendarDateTime} and its method
 * \alib{system,CalendarDateTime::Format}.
 *
 * If parameter \p{formatSpec} is empty, a default format string defined by string resource
 * of key \b "DFMT" is used.
 *
 * \note
 *   This interface implementation is only available if modules \alib_system and \alib_time
 *   are included in the library distribution.
 *
 * @param self       The box that the function was invoked on.
 * @param formatSpec The specification of the format.
 * @param target     The AString object receiving the formatted string.
 **************************************************************************************************/
ALIB_API void
FFormat_DateTime( const Box & self, const String & formatSpec, AString & target );


} // namespace aworx::lib[::system]

/** The singleton of module-class \alib{system,System}. */
extern ALIB_API system::System SYSTEM;

}} // namespace [aworx::lib]

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::system::Exceptions   , vt_system_exceptions   )
ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::system::Exceptions   , aworx::lib::results::ERException   )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::system::Exceptions   , aworx::lib::SYSTEM, "E"   )

ALIB_BOXING_VTABLE_DECLARE( aworx::lib::system::SystemErrors , vt_system_systemerrors )
ALIB_ENUMS_ASSIGN_RECORD(   aworx::lib::system::SystemErrors , aworx::lib::results::ERException   )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::system::SystemErrors , aworx::lib::SYSTEM, "SE" )


#endif // HPP_ALIB_SYSTEM_SYSTEM

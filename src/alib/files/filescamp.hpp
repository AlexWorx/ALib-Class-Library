/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_basecamp of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FILES_CAMP
#define HPP_ALIB_FILES_CAMP 1

#if !defined(HPP_ALIB_LANG_CAMP)
#   include "alib/lang/basecamp/camp.hpp"
#endif


namespace alib {

/** ************************************************************************************************
 * This is the reference documentation of module \alib_files_nl of the \aliblong.<br>
 * @see
 *  A user manual with tutorial-style sample code is found in the
 *  \ref alib_mod_files "Programmer's Manual" of this module.
 **************************************************************************************************/
namespace files {

/** ************************************************************************************************
 * This is a strict singleton class representing module \alib_files.
 *
 * The only instance is found with namespace variable \ref alib::files.
 **************************************************************************************************/
class Files : public lang::Camp
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_camp_modules_campmodule).
         ******************************************************************************************/
        ALIB_API
        Files();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        ALIB_API
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        /** ****************************************************************************************
         * Terminates this module. (Nothing to do.)
         ******************************************************************************************/
        virtual void    shutdown( ShutdownPhases )                                       override {}

}; // class Files

} // namespace alib[::files]

/** The singleton instance of \alibcamp class \alib{files,Files}. */
extern ALIB_API files::Files FILES;

} // namespace [alib]

#endif // HPP_ALIB_FILES_CAMP

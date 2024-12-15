//==================================================================================================
/// \file
/// This header file is part of module \alib_basecamp of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_FILES_CAMP
#define HPP_ALIB_FILES_CAMP 1
#pragma once
#include "alib/lang/basecamp/camp.hpp"

ALIB_ASSERT_MODULE(FILES)


namespace alib {

//==================================================================================================
/// This is the reference documentation of module \alib_files_nl of the \aliblong.<br>
/// @see
///  A user manual with tutorial-style sample code is found in the
///  \ref alib_mod_files "Programmer's Manual" of this module.
//==================================================================================================
namespace files {

//==================================================================================================
/// This is a strict singleton class representing module \alib_files.
///
/// The only instance is found with namespace variable \ref alib::files.
//==================================================================================================
class FilesCamp : public lang::Camp
{
    public:
        //==========================================================================================
        /// Constructor.<br>
        /// While this is public, it must not be invoked as this is a strict singleton type.
        /// (See notes in \ref alib_manual_camp_modules_campmodule).
        //==========================================================================================
        ALIB_API
        FilesCamp();

    protected:
        //==========================================================================================
        /// Initializes this camp.
        ///
        /// @param phase  The initialization phase to perform.
        //==========================================================================================
        ALIB_API
        virtual void    bootstrap( BootstrapPhases phase )                                 override;

        //==========================================================================================
        /// Terminates this camp. (Nothing to do.)
        //==========================================================================================
        virtual void    shutdown( ShutdownPhases )                                       override {}

}; // class FilesCamp

} // namespace alib[::files]

/// The singleton instance of \alibcamp class \alib{files;FilesCamp}.
extern ALIB_API files::FilesCamp FILES;

} // namespace [alib]

#endif // HPP_ALIB_FILES_CAMP


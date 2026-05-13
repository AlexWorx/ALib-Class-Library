// #################################################################################################
//  ALib C++ Framework
//  App Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#ifndef HPP_ALIB_SAMPLES_APP
#define HPP_ALIB_SAMPLES_APP
#pragma once
#include "ALib.Lang.H"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

#include "ALib.App.H" // Module ALib App

// Our custom app
DOX_MARKER( [DOX_APP_SAMPLE_CLASS])
class Sample : public alib::app::App {

    enum class DateCommands {
        Now                 =   1,   ///< Returns the current date.
        File                =   2,   ///< returns the modification date of a file or directory.
    };

    enum class DateOptions {
        Format              =   0,   ///< Overwrite the default format string.
    };

    enum class DateParameters {
        Filename            =   0,   ///< Used with command \e file to denote the file.
    };

    enum class DateExitCodes {
        ErrMissingFilename  = 101, ///< Command "file" given without a filename.
        ErrFileNotFound     = 102, ///< File given with command "file" not found.
        ErrPermissionDenied = 103, ///< File given with command "file" not found.
    };

    alib::String64    format= "yyyy-MM-dd HH:mm:ss";

    void    onBsCLIDefine()                           override;
    void    onBsPrepareConfig()                       override;
    bool    processCLICmd(alib::app::Command* cmd )   override;

    void bulkloadResources();
};
DOX_MARKER( [DOX_APP_SAMPLE_CLASS])

// assigning ALib enum records
ALIB_CAMP_ENUM( Sample::DateCommands   , alib::app::ERCommandDecl  , alib::APP, "DateC"  )
ALIB_CAMP_ENUM( Sample::DateOptions    , alib::app::EROptionDecl   , alib::APP, "DateO"  )
ALIB_CAMP_ENUM( Sample::DateParameters , alib::app::ERParameterDecl, alib::APP, "DateP"  )
ALIB_CAMP_ENUM( Sample::DateExitCodes  , alib::app::ERExitCodeDecl , alib::APP, "DateE"  )

#endif // !DOXYGEN
#endif // HPP_ALIB_SAMPLES_APP

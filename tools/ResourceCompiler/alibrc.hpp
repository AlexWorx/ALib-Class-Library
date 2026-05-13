// #################################################################################################
//  ALib C++ Framework
//  App ALibRC
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_TOOLS_RC
#define HPP_ALIB_TOOLS_RC
#pragma once
#include "ALib.Lang.H"

#if !DOXYGEN // otherwise this tool would be seen in the ALib dox

#include "ALib.App.H"
#include "ALib.Camp.ResourceCompiler.H"


// This tool has no own commands and no parameter types, just options and exit codes.
//enum class RCCommands {};
//enum class RCParameters {};

/// Options registered with \alib_cli.
enum class RCOptions {
    Validate          =   1,   ///< Validate the resource file.
};

/// Exit codes registered with \alib_cli.
enum class RCExitCodes {
    ErrMissingRCFilename   = 101, ///< No input resource file name given.
    ErrRCFileNotFound      = 102, ///< Given resource file not found.
    ErrRCFileNotAccessible = 103, ///< Resource file is not readable.
    ErrMissingCPPFilename  = 104, ///< No C++ source file to patch given.
    ErrCPPFileNotFound     = 105, ///< Given C++ resource file not found.
    ErrCPPFileNotAccessible= 106, ///< C++ resource file is not writable.
    ErrInResources         = 107, ///< Resource file contains errors.
};

/// The resource compiler application.
struct ALibRCApp : public alib::app::App {
    /// The result list of loading the resource file.
    alib::camp::ResourceList* ResFileList = nullptr;
    
    /// The error list of the resource file.
    alib::camp::RCErrorList*  ResFileErrors= nullptr;

    /// Overrides #"App::onBsCLIDefine".
    void    onBsCLIDefine()                           override;

    /// Overrides #"App::onBsPrepareConfig".
    void    onBsPrepareConfig()                       override;

    /// Overrides #"App::processCLICmd".
    bool    processCLICmd(alib::app::Command* cmd )   override;

    /// Constructor.
    ALibRCApp()
    : App{}  {
        flags +=   Flags::CreateReleaseLox
                 + Flags::UseReleaseLoggerForDebugLogging;
    }

    /// This method is our own resource bulk-loader that contains the snippet that is replaced
    /// when compiling resources. (Using class #"DevtimeResourceCompiler".)
    void    bulkloadResources();

    /// App behavior around function #"LoadResourceFile;3".
    void    loadResourceFile(const alib::String& rcFileName );

    /// App behavior around function #"PatchCPPFile;3".
    void    patchCPPFile    (const alib::String& cppFileName );
};

// assigning ALib enum records
ALIB_CAMP_ENUM( RCOptions    , alib::app::EROptionDecl  , alib::APP, "ARCO"  )
ALIB_CAMP_ENUM( RCExitCodes  , alib::app::ERExitCodeDecl, alib::APP, "ARCE"  )

#endif // !DOXYGEN
#endif // HPP_ALIB_TOOLS_RC

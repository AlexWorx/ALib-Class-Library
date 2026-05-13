// #################################################################################################
//  ALib C++ Framework
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#include "ALib.Lang.H"

// Include necessary ALib Camp headers
#include "ALib.Camp.Base.H"       // Include customized module bootstrapping
#include "ALib.App.H"             // Include ALib App module

#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS])
enum class Commands {
    Now                 =   1,   ///< Returns the current date.
    File                =   2,   ///< returns the modification date of a file or directory.
    Help                =  99,   ///< Prints a help text.
};

enum class Options {
    Format              =   0,   ///< Overwrite the default format string.
    Help                =  99,   ///< Show help text. (We allow this as option as well a command)
};

enum class Parameters {
    Filename            =   0,   ///< Used with command \e file to denote the file.
    Topic               =   1,   ///< Used with command \e help to optionally denote a help topic.
};

enum class ExitCodes {
    OK                  =   0, ///< Success.
    ErrUnknownCommand   = 100, ///< Unknown command given.
    ErrUnknownOption    = 101, ///< Unknown option given.
    ErrMissingFilename  = 102, ///< Command "file" given without a filename.
    ErrUnknownHelpTopic = 103, ///< Command or option "help" given without an unknown subtopic.
    ErrInternalError    = 255, ///< Unspecified internal error.
                               ///  (this demo might be incomplete :-)
};
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN])
// assigning ALib enum records
ALIB_ENUMS_ASSIGN_RECORD( Commands        , alib::app::ERCommandDecl        )
ALIB_ENUMS_ASSIGN_RECORD( Options         , alib::app::EROptionDecl         )
ALIB_ENUMS_ASSIGN_RECORD( Parameters      , alib::app::ERParameterDecl      )
ALIB_ENUMS_ASSIGN_RECORD( ExitCodes       , alib::app::ERExitCodeDecl       )
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN])

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_DECL])
class SampleCamp  : public alib::camp::Camp {
  public:
    //  Constructor. Passes version number and resource name to the module class
    SampleCamp() : Camp("DATEMOD")  {}

  protected:
    // Initialization of the module.
    virtual void    Bootstrap()                             override;

    // Terminate this module. (Nothing to do.)
    virtual void    Shutdown( alib::ShutdownPhases phase )  override;
}; // class SampleCamp
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_DECL])

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])
// The module singleton object
extern SampleCamp SAMPLE_CAMP;
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2])
// Specifying our custom module to hold resources of our enum records
ALIB_RESOURCED_IN_CAMP( Commands  , SAMPLE_CAMP, "Commands"   )
ALIB_RESOURCED_IN_CAMP( Parameters, SAMPLE_CAMP, "Parameters" )
ALIB_RESOURCED_IN_CAMP( Options   , SAMPLE_CAMP, "Options"    )
ALIB_RESOURCED_IN_CAMP( ExitCodes , SAMPLE_CAMP, "ExitCodes"  )
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2])

#endif // !DOXYGEN

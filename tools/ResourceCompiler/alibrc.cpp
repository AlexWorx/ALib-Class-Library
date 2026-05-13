// #################################################################################################
//  ALib C++ Framework
//  App ALibRC
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file

#include "alibrc.hpp"

// Include necessary ALib headers
#include "ALib.Strings.StdIOStream.H" 
#include "ALib.Variables.Plugins.H"   
#include "ALib.Bootstrap.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.Camp.H"

#include <filesystem>                 
#include <chrono>                     

// namespaces to use locally
using namespace alib;
using namespace alib::camp;

void ALibRCApp::onBsPrepareConfig() {
    // call parent's implementation. This bulk-loads the resources into the camps.
    App::onBsPrepareConfig();

// DOX_MARKER([DOX_ALIB_SAMPLE_DEVTIMERC_INVOCATION])
    // with debug-builds, invoke the resource compiler and execute 'bulkloadResources()' only if
    // it returned 'false'.
    #if ALIB_DEBUG
    camp::DevtimeResourceCompiler dtrc;
    if(!dtrc.Do( "alibrc.alibrc", __FILE__,
                 APP, APP.ResourceCategory,  true, // allow replacements
                 __FILE__ )
        )
    #endif
        bulkloadResources();
// DOX_MARKER([DOX_ALIB_SAMPLE_DEVTIMERC_INVOCATION])
    enumrecords::bootstrap::Bootstrap<RCOptions   >();
    enumrecords::bootstrap::Bootstrap<RCExitCodes >();

    // Read copyright string from resources and format to current version and year
    Paragraphs buffer;
    buffer.LineWidth= 70;
    {ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
    buffer.AddMarked( APP.GetResource( "AppInfo" ),
                      alib::VERSION,
                      alib::REVISION,
                      CalendarDateTime(DateTime()).Year      );
    }
    cli.AppInfo.Allocate(cli.GetAllocator(), buffer.Buffer);
}

void ALibRCApp::onBsCLIDefine() {
    App::onBsCLIDefine();

    // undefine unused enums inherited by Cliapp base class
    cli.UndefineCommand(  App::Commands ::Info   );
    cli.UndefineOption(   App::Options  ::Config );
    cli.UndefineExitCode( App::ExitCodes::ErrUnknownCommand );

    cli.DefineOptions  <RCOptions   >();
    cli.DefineExitCodes<RCExitCodes>();
}

bool ALibRCApp::processCLICmd(alib::app::Command* cmd ) {
    if ( App::processCLICmd(cmd) )
        return true;

    // No command given: normal compilation
    if ( cmd == nullptr ) {
        // get rcFileName
        String rcFileName= cli.PopArg();
        if ( rcFileName.IsEmpty() ) {
            cErr->Add(APP.GetResource("MSNGRCFILE"));
            cOut->Add( APP.GetResource( "HlpHdlUsage" ) )
            .PushIndent( 2 )
                .Add( APP.GetResource( "HlpUsage"    ) )
            .PopIndent();

            CLIUtil::GetHelp( cli, "USAGE", *cOut );
            machine.SetExitCode(RCExitCodes::ErrMissingCPPFilename);
            throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
        }

        // load resources. On errors, this exits!
        loadResourceFile(rcFileName);

        // get cppFileName
        String cppFileName= cli.PopArg();
        if ( cppFileName.IsEmpty() ) {
            cErr->Add(APP.GetResource("MSNGCPPFILE"));
            cOut->Add( APP.GetResource( "HlpHdlUsage" ) )
            .PushIndent( 2 )
                .Add( APP.GetResource( "HlpUsage"    ) )
            .PopIndent();

            CLIUtil::GetHelp( cli, "USAGE", *cOut );
            machine.SetExitCode(RCExitCodes::ErrMissingCPPFilename);
            throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
        }

        patchCPPFile(cppFileName);
        return true;
    }

    return false;
}

void ALibRCApp::patchCPPFile(const String&  cppFileName ) {
    Path cppFilePath= Path(cppFileName);
    if( !cppFilePath.IsAbsolute() ) {
        cppFilePath.Change(SystemFolders::Current);
        cppFilePath._<NC>(DIRECTORY_SEPARATOR)._<NC>(cppFileName);
    }
    cppFilePath.MakeCanonical();

    FileStatus rcFileStat (cppFilePath);
    String errMsg= nullptr;
    if( rcFileStat.ScanState() != FileStatus::ScanStates::STATS ) {
        if( rcFileStat.ScanState()==FileStatus::ScanStates::NOT_EXISTENT )
            ; // handled later in PatchCPPFile
        else if ( rcFileStat.ScanState()==FileStatus::ScanStates::NO_ACCESS)
            errMsg= APP.GetResource( "NOTACCESSIBLE" );
        else
            errMsg= APP.GetResource( "UKNFILE" );
    }
    else if( rcFileStat.IsDirectory() )
        errMsg= APP.GetResource( "ISDIR" );

    if(errMsg.IsNotNull()) {
        cErr->Add(errMsg, cppFilePath);
        machine.SetExitCode(RCExitCodes::ErrRCFileNotFound);
        throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
    }

    PatchCPPFile(cppFilePath, *ResFileList, *ResFileErrors, dryrun);
    if( !ResFileErrors->empty() ) {
        ListErrors(*ResFileErrors, *cErr, cppFilePath );
        machine.SetExitCode(RCExitCodes::ErrInResources);
        throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
    }
}

void ALibRCApp::loadResourceFile(const String&  rcFileName ) {
    Path rcFilePath= Path(rcFileName);
    if( !rcFilePath.IsAbsolute() ) {
        rcFilePath.Change(SystemFolders::Current);
        rcFilePath._<NC>(DIRECTORY_SEPARATOR)._<NC>(rcFileName);
    }
    rcFilePath.MakeCanonical();

    FileStatus rcFileStat (rcFilePath);
    String errMsg= nullptr;
    if( rcFileStat.ScanState() != FileStatus::ScanStates::STATS ) {
        if( rcFileStat.ScanState()==FileStatus::ScanStates::NOT_EXISTENT )
            ; // handled later in LoadResourceFile
        else if ( rcFileStat.ScanState()==FileStatus::ScanStates::NO_ACCESS)
            errMsg= APP.GetResource( "NOTACCESSIBLE" );
        else
            errMsg= APP.GetResource( "UKNFILE" );
    }
    else if( rcFileStat.IsDirectory() )
        errMsg= APP.GetResource( "ISDIR" );

    if(errMsg.IsNotNull()) {
        cErr->Add(errMsg, rcFilePath);
        machine.SetExitCode(RCExitCodes::ErrRCFileNotFound);
        throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
    }

    // load resource file
    ResFileList   = cli.GetAllocator()().New<ResourceList>(cli.GetAllocator());
    ResFileErrors = cli.GetAllocator()().New<RCErrorList>(cli.GetAllocator());
    LoadResourceFile(rcFilePath, *ResFileList, *ResFileErrors);
    if( !ResFileErrors->empty() ) {
        ListErrors(*ResFileErrors, *cErr, rcFilePath);
        machine.SetExitCode(RCExitCodes::ErrInResources);
        throw Exception(ALIB_CALLER_NULLED, App::Exceptions::ControlledEarlyExit);
    }
}
void ALibRCApp::bulkloadResources() {

    APP.GetResourcePool().BootstrapBulk( APP.ResourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "AppInfo"            , A_CHAR("@HL-ALib Resource Compiler V. {}.{}\n")
                           A_CHAR("(c) 2023-{} AWorx GmbH. Published under MIT License (Open Source).\n")
                           A_CHAR("For more information, see: https://alib.dev/alib_mod_resources.html\n")
                           A_CHAR("@HL-"),
    "MSNGRCFILE"         , A_CHAR("No resource file name given."),
    "MSNGCPPFILE"        , A_CHAR("No C++ file name given."),
    "NOTACCESSIBLE"      , A_CHAR("File {!Q} is not accessible."),
    "UKNFILE"            , A_CHAR("Unknown error with file {!Q}."),
    "ISDIR"              , A_CHAR("Given path {!Q} is a directory instead of a file."),
    "DryRunStart"        , A_CHAR("ALibRC dry-run mode."),
    "DryRunEnd"          , A_CHAR("ALibRC dry-run done."),
    "ARCO<"              , A_CHAR("ALibRC::Options::"),
    "ARCO"               , A_CHAR("1,validate,1,v,=,0,"),
    "ARCE<"              , A_CHAR("ALibRC::ExitCodes::"),
    "ARCE"               , A_CHAR("101,ErrMissingRCFilename,102,ErrRCFileNotFound,103,ErrRCFileNotAccessible,104,ErrMissingCPPFilename,105,ErrCPPFileNotFound,106,ErrCPPFileNotAccessible,107,ErrInResources"),
    "TExit101"           , A_CHAR("No input resource file name given."),
    "TExit102"           , A_CHAR("Given resource file not found."),
    "TExit103"           , A_CHAR("Resource file is not readable."),
    "TExit104"           , A_CHAR("No C++ source file to patch given."),
    "TExit105"           , A_CHAR("Given C++ resource file not found."),
    "TExit106"           , A_CHAR("C++ file is not writable."),
    "TExit107"           , A_CHAR("The resource file is erroneous."),
    "HlpCLIAppName"      , A_CHAR("ALibRC"),
    "HlpUsage"           , A_CHAR("alibrc resourcefile cppfile"),
    "HlpGeneral"         , A_CHAR("\nABOUT ALibRC\n")
                           A_CHAR("@>>This is a tool provided by the C++ Framework ALib that compiles external resources and inserts a corresponding code snippet into a C++ source file. The resources are specified in a resource file, while the C++ source file must exist and contain special markers for the insertion position.\n")
                           A_CHAR("All details are given in the documentation of the ALib C++ Framework at:\n")
                           A_CHAR("  https://alib.dev/ \n")
                           A_CHAR("\n")
                           A_CHAR("@<<"),
    "THlpCmdSht_validate", A_CHAR("Parses the given resource file and checks for errors."),
    "THlpCmdLng_validate", A_CHAR("No C++ code is generated and the cpp-file (if given) is not modified."),
    "TOptUsg_validate"   , A_CHAR("--validate resourcefile [cppfile]"),
    "TOptHlp_validate"   , A_CHAR("If this option is given (anywhere in the command line), the resource file is only parsed and no C++ code is generated. The cpp-file parameter is ignored and can be omitted."),
    "TOptUsg_format"     , A_CHAR("--format[=]\"placeholders\""),
    "TOptHlp_format"     , A_CHAR("Sets the output format. The format specification is given with the documentation of the ALib method CalendarDateTime::Format, found here:\n")
                           A_CHAR("https://alib.dev/classalib_1_1strings_1_1util_1_1CalendarDateTime.html"),
    "THlpParSht_rc-file" , A_CHAR("The '*.alibrc' input file."),
    "THlpParLng_rc-file" , A_CHAR("Denotes the input file, usually of extension '*.alibrc'. This is a mandatory parameter to give."),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );
}


int main( int argc, const char** argv) {
    return ALibRCApp().Main(argc, argv);
}


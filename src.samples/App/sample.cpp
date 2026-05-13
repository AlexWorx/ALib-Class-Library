// #################################################################################################
//  ALib C++ Framework
//  App Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#include "ALib.Lang.H"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

#include "sample.hpp"

// Include necessary ALib headers
#include "ALib.Strings.StdIOStream.H"    // Support to write ALib strings and boxes to cout
#include "ALib.Strings.Calendar.H"       // Module ALib Strings, calendar formatting
#include "ALib.Variables.Plugins.H"      // Module ALib Variables, environment and cli-plugins
#include "ALib.Bootstrap.H"              // Bootstrapping ALib.
#include "ALib.Camp.ResourceCompiler.H"  // The resource compiler

#include <filesystem>                    // C++ filesystem
#include <chrono>                        // C++ chrono

// namespaces to use locally
using namespace alib;

void Sample::onBsCLIDefine() {
    App::onBsCLIDefine();

    cli.DefineExitCodes <DateExitCodes >();
    cli.DefineParameters<DateParameters>();
    cli.DefineCommands  <DateCommands  >();
    cli.DefineOptions   <DateOptions   >();
}

// DOX_MARKER([DOX_ALIB_SAMPLE_RESOURCE_COMPILER])
void Sample::onBsPrepareConfig() {
    // call parent's implementation. This bulk-loads the resources into the camps.
    App::onBsPrepareConfig();

    // now, we just add the resources that parent App did not define for us.
    camp::DevtimeResourceCompiler rc;
    if(!rc.Do( "sample.alibrc", __FILE__, APP, APP.ResourceCategory, true,
                        //       __FILE__ )
                                nullptr )
        )
        bulkloadResources();

    enumrecords::bootstrap::Bootstrap<DateCommands  >();
    enumrecords::bootstrap::Bootstrap<DateParameters>();
    enumrecords::bootstrap::Bootstrap<DateOptions   >();
    enumrecords::bootstrap::Bootstrap<DateExitCodes >();

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
// DOX_MARKER([DOX_ALIB_SAMPLE_RESOURCE_COMPILER])

bool Sample::processCLICmd(alib::app::Command* cmd ) {
    if ( App::processCLICmd(cmd) )
        return true;

    if ( cmd == nullptr ) {
        String128 dateWritten;
        CalendarDateTime(DateTime()).Format( format, dateWritten );
        cOut->Add(dateWritten);
        return true;
    }

    // Note: Alternatively, we could do:
    // auto actCmdCode= cmd->Declaration->Element();
    //
    // if ( actCmdCode == DateCommands::Now )  {
    //     ...
    // }
    //
    // else if ( actCmdCode == DateCommands::File )  {
    // {
    //     ...
    //
    // But the following version lets the compiler assure us that
    // all custom commands are handled:

    ALIB_ALLOW_SWITCH_WITHOUT_DEFAULT
    if ( cmd->Declaration->Element().IsEnumType<DateCommands>()) {
        switch (cmd->Declaration->Element().Get<DateCommands>()) {
            case DateCommands::Now: {
                String128 dateWritten;
                CalendarDateTime(DateTime()).Format( format, dateWritten);
                cOut->Add(dateWritten);
                return true;
            }
            case DateCommands::File: {
                // check if filename was given as parameter
                if(cmd->ParametersMandatory.size() < 1)
                {
                    cErr->Add(APP.GetResource("MSNGFNAME"));
                    ALIB_ASSERT_RESULT_EQUALS(
                        CLIUtil::GetHelp( cli, cmd->Declaration->Identifier(), *cOut )
                    ,true)
                    machine.SetExitCode(DateExitCodes::ErrMissingFilename);
                    return true;
                }

                // get file (or directory) modification date
                String4K name( cmd->ParametersMandatory.front()->Args.front() );
                std::filesystem::path stdpath( name.Terminate() );
                DateTime dt;

                // While class App catches exceptions and provides explicit virtual method
                // App::exceptionToExitCode() to be overridden, we use the non-throwing version
                // of std::filesystem::last_write_time. Here is why:
                // - We have the filename here at hand. No need to pass it over
                // - while exceptionToExitCode is nice in respect to having all exceptions in
                //   one place, it is also nice to handle an error where it occurs.
                // So, maybe a matter of taste. And finally we mix it: if we do not handle the
                // error code here, we then throw!
                std::error_code errorCode;
                auto timeValue= std::filesystem::last_write_time( stdpath, errorCode );
                if ( errorCode.value() != 0 ) {
                    // not found
                    if ( errorCode.value() == int(std::errc::no_such_file_or_directory) ) {
                        cErr->Add( APP.GetResource("FNOTFND"), name);
                        machine.SetExitCode( DateExitCodes::ErrFileNotFound );
                        return true;
                    }

                    // permission denied
                    if ( errorCode.value() == int(std::errc::permission_denied) ) {
                        cErr->Add( APP.GetResource("FNOACC"), name);
                        machine.SetExitCode( DateExitCodes::ErrPermissionDenied );
                        return true;
                    }

                    // other errors
                    throw exceptions::CreateExceptionFromSystemError(ALIB_CALLER_NULLED, errorCode);
                }
                dt.Import( std::chrono::clock_cast<std::chrono::system_clock>(timeValue) );
                String128 dateWritten;
                CalendarDateTime(dt).Format( format, dateWritten );
                cOut->Add(dateWritten);

                return true;
            }
        }
    }

    ALIB_POP_ALLOWANCE

    return false;
}

void Sample::bulkloadResources() {

    APP.GetResourcePool().BootstrapBulk( APP.ResourceCategory,

    // ALIB-RESOURCE-COMPILER-REPLACEMENT-START
    "HlpCLIAppName"      , A_CHAR("sample"),
    "HlpUsage"           , A_CHAR("sample [format=\"FORMATSPEC\" [now]|[file FILENAME]"),
    "AppInfo"            , A_CHAR("@HL-ALib Resource Compiler V. {}.{}\n")
                           A_CHAR("(c) 2023-{} AWorx GmbH. Published under MIT License (Open Source).\n")
                           A_CHAR("For more information, see: https://alib.dev/alib_mod_resources.html\n")
                           A_CHAR("@HL-"),
    "HlpGeneral"         , A_CHAR("\nABOUT sample\n")
                           A_CHAR("@>> This is a sample application provided with C++ library 'ALib' to demonstrate the use of its module \"ALib App\".\" \n")
                           A_CHAR("@<<"),
    "DateC<"             , A_CHAR("datesample::Commands::"),
    "DateC"              , A_CHAR("1,now,1,,2,file,1,filename"),
    "THlpCmdSht_now"     , A_CHAR("Reports the actual date/time"),
    "THlpCmdLng_now"     , A_CHAR(""),
    "Reports"            , A_CHAR("the actual date/time. May be omitted, as this is the default if no command is given."),
    "THlpCmdSht_file"    , A_CHAR("Returns the date/time of a file."),
    "THlpCmdLng_file"    , A_CHAR("Returns the last modification date/time of a file."),
    "DateO<"             , A_CHAR("datesample::Options::"),
    "DateO"              , A_CHAR("0,format,1,f,=,1,"),
    "TOptUsg_format"     , A_CHAR("--format[=]\"placeholders\""),
    "TOptHlp_format"     , A_CHAR("Sets the output format. The format specification is given with the documentation of the ALib method CalendarDateTime::Format, found here:\n")
                           A_CHAR("https://alib.dev/classalib_1_1strings_1_1util_1_1CalendarDateTime.html"),
    "DateP<"             , A_CHAR("datesample::Parameters::"),
    "DateP"              , A_CHAR("0,FILENAME,1,,=,,-1,0"),
    "THlpParSht_FILENAME", A_CHAR("Mandatory parameter of command \"file\"."),
    "THlpParLng_FILENAME", A_CHAR("Denotes the file that is used for retrieving the modification date. This parameter is mandatory to be given with command 'file' and has to be appended to this command, separated by '='\""),
    "DateE<"             , A_CHAR("datesample::"),
    "DateE"              , A_CHAR("101,ErrMissingFilename,-1,102,ErrFileNotFound,-1,103,ErrPermissionDenied,-1"),
    "TExit101"           , A_CHAR("Command 'file' given without a filename argument."),
    "TExit102"           , A_CHAR("File not found."),
    "TExit103"           , A_CHAR("Permission denied."),
    "FNOTFND"            , A_CHAR("The file {!Q} specified with command 'file' was not found."),
    "FNOACC"             , A_CHAR("Access denied to file {!Q} specified with command 'file'."),
    "MSNGFNAME"          , A_CHAR("Error: no filename given with command 'file'."),
    // ALIB-RESOURCE-COMPILER-REPLACEMENT-END

    // end of BootstrapBulk()
    nullptr );

}


#endif // !DOXYGEN

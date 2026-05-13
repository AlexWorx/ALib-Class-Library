// #################################################################################################
//  ALib C++ Framework
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#include "ALib.Lang.H"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

#include "samplecamp.hpp"

// Include necessary ALib headers
#include "ALib.Strings.StdIOStream.H"           // Support to write ALib strings and boxes to cout
#include "ALib.Boxing.StdFunctors.H"            // Allow class boxing::Enum be key of Hashtables
#include "ALib.Strings.Calendar.H"              // ALib calendar formatting
#include "ALib.Bootstrap.H"                     // Bootstrapping ALib

#include <filesystem>                           // C++ filesystem
#include <chrono>                               // C++ chrono

// namespaces to use locally
using namespace alib;

DOX_MARKER( [DOX_EXPR_TUT_CLI_FWDDECL])
// forward declaration
ExitCodes processCLI( CommandLine& cli );
DOX_MARKER( [DOX_EXPR_TUT_CLI_FWDDECL])

// switch the macro ALIB_CALLER to not use "this" (because we have a global function here)
#include "ALib.Lang.CIFunctions.H"

DOX_MARKER( [DOX_EXPR_TUT_CLI_MAIN])
int main( int argc, const char **argv )
{
    alib::ARG_C  = argc;
    alib::ARG_VN = argv;

    // 1. Add our custom module to the list of modules
    alib::BootstrapAddDefaultCamps();
    alib::CAMPS.push_back( &SAMPLE_CAMP );

    // 2. Initialize all modules
    alib::Bootstrap();

    // 3. now we start catching exceptions
    Enum result= ExitCodes::ErrInternalError;
    try
    {
        // 4. Create the central command-line interface object app and perform
        //    mandatory initializations.
        CommandLine cli;
        {
            // Read copyright string from resources and format to current version and year
            Paragraphs buffer;
            buffer.LineWidth= 70;
            {ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
            buffer.AddMarked( SAMPLE_CAMP.GetResource( "AppInfo" ),
                              alib::VERSION,
                              alib::REVISION,
                              CalendarDateTime(DateTime()).Year      );
            }
            cli.AppInfo.Allocate(cli.GetAllocator(), buffer.Buffer);

            // Initialize the CLI with the module to fetch the resources from.
            cli.Init( &SAMPLE_CAMP );

            // Read enum records from resources and build up corresponding object lists.
            cli.DefineParameters<enum Parameters>();
            cli.DefineCommands  <enum Commands  >();
            cli.DefineOptions   <enum Options   >();
            cli.DefineExitCodes <enum ExitCodes >();

            // Read options from the command-line
            cli.ReadOptions();
        }

        // 5. check for unprocess options (not allowed with this demo. Other application might pass
        //    those to other libraries or parts of the software, which provide their own option
        //    processing.
        if( cli.OptionArgsIgnored.size() )
        {
            result= ExitCodes::ErrUnknownOption;
            std::cerr << "Error: Unknown option given \""
                      << cli.OptionArgsIgnored.front()
                      << "\"" << std::endl;
            goto END;
        }

        // 6. Now, the truly custom part: Process commands and options
        result= processCLI( cli );
    }

    // fetch exceptions and assign a corresponding exit-code (error code)
    catch( Exception& e)
    {ALIB_LOCK_RECURSIVE_WITH(Formatter::DEFAULT_LOCK)
        std::cerr << e.Format() << std::endl; // print out human-readable exception information
        result= e.Back().Type;                // For this demo, just return the internal exception
                                              // number as "exit-code".
    }
    catch(std::runtime_error& e)
    {
        result= ExitCodes::ErrInternalError;
        std::cerr << "A runtime error occurred: " << e.what()<< std::endl;
    }

    // 7. That's it.
    END:
    alib::Shutdown();
    return int(result.Integral());
}
DOX_MARKER( [DOX_EXPR_TUT_CLI_MAIN])



DOX_MARKER( [DOX_EXPR_TUT_CLI_PROCESS])
ExitCodes processCLI( CommandLine& cli )
{
    String64    format;     // The date output format
    Paragraphs  helpText;   // A buffer for help texts
    DateTime    dt;         // The timestamp to output

    format << "yyyy-MM-dd HH:mm:ss";

    //------- check for option 'format' -------
    app::Option* option= cli.GetOption( Options::Format);
    if( option )
    {
        format.Reset( option->Args.front() );
    }

    //------- check for option 'help' -------
    option= cli.GetOption( Options::Help);
    if( option )
    {
        if( !CLIUtil::GetHelp( cli, option, helpText ) )
        {
            std::cerr << "Error: Unknown help Topic \""
                      << (option->Args.size() > 0 ? option->Args.front() : String() )
                      << "\"" << std::endl
                      << "Usage Information follows: " << std::endl << std::endl;
            option->Args.Clear();
            helpText.Clear();
            CLIUtil::GetHelp( cli, option, helpText );
        }
        std::cout << helpText.Buffer << std::endl;
        return ExitCodes::OK;
    }

    //------- No command recognized? This is allowed, assuming now -------
    cli.ReadNextCommands();
    if( cli.CommandsParsed.size() == 0 )
    {
        // Still a command was given? This is not allowed
        if( cli.ArgsLeft.size() > 0 )
        {
            std::cerr << "Error: Unknown command given \""
                      << cli.ArgStrings.at(std::size_t(*cli.ArgsLeft.begin()))
                      << "\"" << std::endl;
            return ExitCodes::ErrUnknownCommand;       
        }

        // No command, results in command "now"
        CalendarDateTime  calendar= CalendarDateTime(dt);
        AString printBuffer;
        calendar.Format( format, printBuffer, lang::CurrentData::Keep );
        std::cout << printBuffer << std::endl;

        return ExitCodes::OK;
    }

    //------- Command loop -------
    // Note: Making a loop here is optional. We do it to allow multiple commands
    //       with one invocation of the application.
    app::Command* actCmd;
    while ( (actCmd= cli.NextCommand()) != nullptr )
    {
        auto actCmdCode= actCmd->Declaration->Element();

        if ( actCmdCode == Commands::Now      )
        {
            dt= DateTime();
        }

        else if ( actCmdCode == Commands::File     )
        {
            // check if filename was given as paraemter
            if(actCmd->ParametersMandatory.size() < 1)
            {
                std::cerr << "Error: no filename given with command 'file'" << std::endl;
                std::cerr << "Usage: " << CLIUtil::GetCommandUsageFormat(cli, *actCmd->Declaration )
                          << std::endl;
                return ExitCodes::ErrMissingFilename;
            }

            // get file (or directory) modification date
            String4K name( actCmd->ParametersMandatory.front()->Args.front() );
            std::filesystem::path path( name.Terminate() );
            dt.Import( std::chrono::clock_cast<std::chrono::system_clock>(
                          std::filesystem::last_write_time( path )        ) )   ;
        }

        else if ( actCmdCode == Commands::Help ) {
            if( !CLIUtil::GetHelp( cli, actCmd, helpText ) ) {
                std::cerr << "Error: Unknown help topic" << std::endl;
                std::cerr << "Usage: " << CLIUtil::GetCommandUsageFormat(cli, *actCmd->Declaration )
                          << std::endl;
                return ExitCodes::ErrUnknownHelpTopic;
            }
            std::cout << helpText.Buffer << std::endl;
            continue;
        }

        // execute printing of commands "now" and "file"
        CalendarDateTime  calendar= CalendarDateTime(dt);
        AString printBuffer;
        calendar.Format( format, printBuffer, lang::CurrentData::Keep );
        std::cout << printBuffer << std::endl;
    }
    return ExitCodes::OK;
}
DOX_MARKER( [DOX_EXPR_TUT_CLI_PROCESS])



#endif // !DOXYGEN

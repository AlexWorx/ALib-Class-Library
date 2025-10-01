// #################################################################################################
//  ALib C++ Library
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "ALib.Lang.H"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

DOX_MARKER( [DOX_EXPR_TUT_CLI_INCLUDES])
// Include necessary ALib CLI headers
#include "ALib.Strings.StdIOStream.H"           // Support to write ALib strings and boxes to cout
#include "ALib.Boxing.StdFunctors.H"            // Allow class boxing::Enum be key of Hashtables
#include "ALib.CLI.H"
#include "ALib.Camp.Base.H"
#include "ALib.Camp.H"                          // Support customized module bootstrapping
#include "ALib.EnumRecords.Bootstrap.H"         // Support for bootstrapping resourced enum records
#include "ALib.Lang.H"                          // ALib date/time types
#include "ALib.Strings.Calendar.H"              // ALib calendar formatting
#include "ALib.Bootstrap.H"                     // Bootstrapping ALib
#include <filesystem>                           // C++ filesystem
#include <chrono>                               // C++ chrono

#include "ALib.Strings.StdFunctors.H"
// namespaces to use locally
using namespace alib;
using namespace alib::cli;
DOX_MARKER( [DOX_EXPR_TUT_CLI_INCLUDES])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS])
// #################################################################################################
// Enumerations of Commands, Parameters, Options and ExitCodes of the CLI application
// #################################################################################################
enum class Commands
{
    Now                 =   1,   ///< Returns the current date.
    File                =   2,   ///< returns the modification date of a file or directory.
    Help                =  99,   ///< Prints a help text.
};

enum class Options
{
    Format              =   0,   ///< Overwrite the default format string.
    Help                =  99,   ///< Show help text. (We allow this as option as well a command)
};

enum class Parameters
{
    Filename            =   0,   ///< Used with command \e file to denote the file.
    Topic               =   1,   ///< Used with command \e help to optionally denote a help topic.
};

enum class ExitCodes
{
    OK                  =   0, ///< Success.
    ErrUnknownCommand   = 100, ///< Unkown command given.
    ErrUnknownOption    = 101, ///< Unkown option given.
    ErrMissingFilename  = 102, ///< Command "file" given without a filename.
    ErrUnknownHelpTopic = 103, ///< Command or option "help" given without an unknown subtopic.
    ErrInternalError    = 255, ///< Unspecified internal error.
                               ///  (this demo might be incomplete :-)
};
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN])
// assigning ALib enum records
ALIB_ENUMS_ASSIGN_RECORD( Commands        , alib::cli::ERCommandDecl        )
ALIB_ENUMS_ASSIGN_RECORD( Options         , alib::cli::EROptionDecl         )
ALIB_ENUMS_ASSIGN_RECORD( Parameters      , alib::cli::ERParameterDecl      )
ALIB_ENUMS_ASSIGN_RECORD( ExitCodes       , alib::cli::ERExitCodeDecl       )
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN])

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP])
// #################################################################################################
// The custom ALib module, needed to define externalized resources.
// #################################################################################################
class SampleCamp  : public camp::Camp
{
  public:
    //  Constructor. Passes version number and resource name to the module class
    SampleCamp()
    : Camp( "DATEMOD" )
    {}

  protected:
    // Initialization of the module.
    virtual void    Bootstrap()                                                             override
    {
        if( GetBootstrapState() == BootstrapPhases::PrepareResources) {
            // Add bulk !
            resourcePool->BootstrapBulk( ResourceCategory,
            #define EOS ,

            // ################################### Single Strings ######################################
            "AppInfo", A_CHAR( "@HL-"
                               "Command line tool 'date'. V. {}.{} (in fact a sample application only)\n"
                               "(c) 2023-{} AWorx GmbH. Published under MIT License (Open Source).\n"
                               "For more information, see: https://alib.dev\n"
                               "@HL-"),

            // ######################################## Commands ######################################
            "Commands", A_CHAR(
                 //enum       ident          minread     Params
                   "1,"       "now"           ",1"        ","               ","
                   "2,"       "file"          ",1"        ",filename"       ","
                  "99,"       "help"          ",1"        ",topic"                  ) EOS

            "Commands<",       A_CHAR("datesample::Commands::"),

            "THlpCmdSht_now",  A_CHAR("Reports the actual date/time"),
            "THlpCmdLng_now",  A_CHAR("Reports the actual date/time. May be omitted, as this is the\n"
                                      "default if no command is given.")                         EOS

            "THlpCmdSht_file", A_CHAR("Returns the date/time of a file. "),
            "THlpCmdLng_file", A_CHAR("Returns the last modification date/time of a file.")      EOS

            "THlpCmdSht_help", A_CHAR("Displays usage information. "),
            "THlpCmdLng_help", A_CHAR("Displays usage information. Can also be given as an "
                                      "option '--help'.")                                        EOS


            // ######################################## Options ######################################
            "Options", A_CHAR(
            //enum  ident        minread    identChar   in-arg-separ.   args to consume    ShortcutTo
             "0,"  "format"      ",1,"        "f,"        "="             ",1,"             ","
            "99,"  "help"        ",1,"        "h,"        "="             ",0,"           ) EOS

            "Options<",       A_CHAR("datesample::Options::"),

            "TOptUsg_format",  A_CHAR("--format[=]\"placholders\""),
            "TOptHlp_format",  A_CHAR("Sets the output format. The format specification is given with\n"
                                      "documentation of ALib method CalendarDateTime::Format, found here:\n"
                                      "https://alib.dev/classalib_1_1strings_1_1util_1_1CalendarDateTime.html" ) ,
            "TOptUsg_help"  ,  A_CHAR("--help[[=]TOPIC]"),
            "TOptHlp_help"  ,  A_CHAR("Displays usage information.")
            EOS

            // ######################################## Parameters ######################################
            "Parameters",  A_CHAR(
            //enum  name           minIdentLen   identifier          in-arg-sep   delim   args to consume  isOptional
            //                                   (if empty -> mandatory!)
            "0,"  "FILENAME"         ",1,"        ""                 ","          "="  ","  ",-1"          ",0"     ","
            "1,"  "TOPIC"            ",1,"        ""                 ","          "="  ","  ",-1"          ",1"   ) EOS

            "Parameters<",           A_CHAR("datesample::Parameters::"),
            "THlpParSht_FILENAME",   A_CHAR("Mandatory parameter of command 'file."),
            "THlpParLng_FILENAME",   A_CHAR("Denotes the file that is used for retrieving the modification date.\n"
                                            "This parameter is mandatory to command file and has to be appended\n"
                                            "to this command, separated by '='"),
            "THlpParSht_TOPIC"   ,   A_CHAR("Optional parameter of command (or option) 'help'."),
            "THlpParLng_TOPIC"   ,   A_CHAR("Denotes a specific toopic that the help command should be verbose about.")
            EOS

            // ######################################## ExitCodes ######################################
            "ExitCodes",  A_CHAR(
            //enum  name                             assoc. cli exception
              "0,"  "OK"                               ",-1"              ","
            "100,"  "ErrUnknownCommand"                ",-1"              ","
            "101,"  "ErrUnknownOption"                 ",-1"              ","
            "102,"  "ErrMissingFilename"               ",-1"              ","
            "103,"  "ErrUnknownHelpTopic"              ",-1"              ","
            "255,"  "ErrInternalError"                 ",-1"            ) EOS

            "ExitCodes<",  A_CHAR("datesample::"),

             "TExit0"   ,  A_CHAR("Success (no error).")
            ,"TExit100" ,  A_CHAR("An unknown command was given. Valid commands are 'now' and 'file'")
            ,"TExit101" ,  A_CHAR("An unknown option was given. The only valid option is '--format='FORMATSPEC'.")
            ,"TExit102" ,  A_CHAR("Command 'file' given without a filename argument.")
            ,"TExit103" ,  A_CHAR("Command or option 'help' given without an unknown subtopic.")
            ,"TExit255" ,  A_CHAR("Unspecified internal error.")
            EOS

            // ################################### Help Texts ######################################
            "HlpCLIAppName",  A_CHAR("date"),
            "HlpUsage"     ,  A_CHAR("date [format=\"FORMATSPEC\" [now]|[file FILENAME]"),
            "HlpHdlOpts"   ,  A_CHAR("OPTIONS:"                ),
            "HlpHdlCmds"   ,  A_CHAR("COMMANDS:"               ),
            "HlpHdlExtCds" ,  A_CHAR("EXIT CODES:"             ),
            "HlpHdlUsage"  ,  A_CHAR("USAGE:"                  ),
            "HlpHdlDscr"   ,  A_CHAR( "DESCRIPTION:"           ),
            "HlpHdlPDscr"  ,  A_CHAR("PARAMETER DESCRIPTION:"  ),
            "HlpHdlTopic"  ,  A_CHAR("Help on {} {!Q<>}:\n"    ),

            "HlpGeneral", A_CHAR(
            "\nABOUT date\n"
            "@>>"
            "This is a sample application provided with C++ library 'ALib'\n"
            "to demonstrate the use of its module \"ALib CLI\"."
            "\n@<<\n" )
            EOS

            // end of AddBulk()
            nullptr );
        }

        else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )
        {
            enumrecords::bootstrap::Bootstrap<Commands       >();
            enumrecords::bootstrap::Bootstrap<Parameters     >();
            enumrecords::bootstrap::Bootstrap<Options        >();
            enumrecords::bootstrap::Bootstrap<ExitCodes      >();
        }
    }

    // Termination this module. (Nothing to do.)
    virtual void    Shutdown( ShutdownPhases phase )                                        override
    { (void) phase;  }

}; // class SampleCamp
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP])

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])
// The module singleton object
extern SampleCamp SAMPLE_CAMP;
SampleCamp SAMPLE_CAMP;
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2])
// Specifying our custom module to hold resources of our enum records
ALIB_RESOURCED_IN_MODULE( Commands  , SAMPLE_CAMP, "Commands"   )
ALIB_RESOURCED_IN_MODULE( Parameters, SAMPLE_CAMP, "Parameters" )
ALIB_RESOURCED_IN_MODULE( Options   , SAMPLE_CAMP, "Options"    )
ALIB_RESOURCED_IN_MODULE( ExitCodes , SAMPLE_CAMP, "ExitCodes"  )
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_ASSIGN2])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_FWDDECL])
// forward declaration
ExitCodes processCLI( CommandLine& cli );
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_FWDDECL])

DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_MAIN])
// #################################################################################################
// The main() function of the CLI application
// #################################################################################################

// switch the macro ALIB_CALLER to not use "this" (because we have a global function here)
#include "ALib.Lang.CIFunctions.H"

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
        // 4. Create the central command line interface object app and perform
        //    mandatory initializations.
        CommandLine cli;
        {
            // Read copyright string from resources and format to current version and year
            Paragraphs buffer;
            buffer.LineWidth= 70;
            {ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
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

            // Read options from the command line
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

    // fetch exceptions and assign a corresponding exit code (error code)
    catch( Exception& e)
    {ALIB_LOCK_RECURSIVE_WITH(Formatter::DefaultLock)
        std::cerr << e.Format() << std::endl; // print out human-readable exception information
        result= e.Back().Type;                // For this demo, just return the internal exception
                                              // number as "exit code".
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
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_MAIN])


DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_PROCESS])
// #################################################################################################
// The custom function to process CLI params
// #################################################################################################
ExitCodes processCLI( CommandLine& cli )
{
    AString     format;     // The date output format
    Paragraphs  helpText;   // A buffer for help texts
    DateTime    dt;         // The timestamp to output

    format << "yyyy-MM-dd HH:mm:ss";

    //------- check for option 'format' -------
    Option* option= cli.GetOption( Options::Format);
    if( option )
    {
        format.Reset( option->Args.front() );
    }

    //------- check for option 'help' -------
    option= cli.GetOption( Options::Help);
    if( option )
    {
        if( !CLIUtil::GetHelp( cli, nullptr, option, helpText ) )
        {
            std::cerr << "Error: Unknown help Topic \""
                      << (option->Args.size() > 0 ? option->Args.front() : String() )
                      << "\"" << std::endl
                      << "Usage Information follows: " << std::endl << std::endl;
            option->Args.Clear();
            helpText.Clear();
            CLIUtil::GetHelp( cli, nullptr, option, helpText );
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
        calendar.Format( format, printBuffer, lang::CurrentData::Clear );
        std::cout << printBuffer << std::endl;

        return ExitCodes::OK;
    }

    //------- Command loop -------
    // Note: Making a loop here is optional. We do it to allow multiple commands
    //       with one invokation of the application.
    Command* actCmd;
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

        else if ( actCmdCode == Commands::Help     )
        {
            if( !CLIUtil::GetHelp( cli, actCmd, nullptr, helpText ) )
            {
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
        calendar.Format( format, printBuffer, lang::CurrentData::Clear );
        std::cout << printBuffer << std::endl;
    }
    return ExitCodes::OK;
}
DOX_MARKER( [DOX_EXPR_TUT_CLI_ENUMS_PROCESS])

#endif // !DOXYGEN

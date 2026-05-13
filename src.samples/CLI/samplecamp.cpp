// #################################################################################################
//  ALib C++ Framework
//  Configuration Sample
//
//  Copyright 2025 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
/// \file
#include "samplecamp.hpp"
#if !DOXYGEN // otherwise this sample would be seen in the ALib dox

// namespaces to use locally
using namespace alib;
using namespace alib::app;

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_IMPL])
void SampleCamp::Bootstrap() {
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
"Options<",       A_CHAR("datesample::Options::"),
"Options", A_CHAR(
//enum  ident        minread    identChar   in-arg-separ.   args to consume    ShortcutTo
 "0,"  "format"      ",1,"        "f,"        "="             ",1,"             ","
"99,"  "help"        ",1,"        "h,"        "="             ",0,"           ) EOS

"TOptUsg_format",  A_CHAR("--format[=]\"placeholders\""),
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
"THlpParLng_TOPIC"   ,   A_CHAR("Denotes a specific topic that the help command should be verbose about.")
EOS

// ######################################## ExitCodes ######################################
"ExitCodes",  A_CHAR(
//enum  name
  "0,"  "OK"                     ","
"100,"  "ErrUnknownCommand"      ","
"101,"  "ErrUnknownOption"       ","
"102,"  "ErrMissingFilename"     ","
"103,"  "ErrUnknownHelpTopic"    ","
"255,"  "ErrInternalError"     ) EOS

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
"HlpHdlExtCds" ,  A_CHAR("EXIT-CODES:"             ),
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

        // end of BootstrapBulk()
        nullptr );
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig ) {
        enumrecords::bootstrap::Bootstrap<Commands       >();
        enumrecords::bootstrap::Bootstrap<Parameters     >();
        enumrecords::bootstrap::Bootstrap<Options        >();
        enumrecords::bootstrap::Bootstrap<ExitCodes      >();
    }
}

// Terminate this module. (Nothing to do.)
void    SampleCamp::Shutdown( ShutdownPhases ) {}
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_IMPL])

DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])
// The module' singleton object
SampleCamp SAMPLE_CAMP;
DOX_MARKER( [DOX_EXPR_TUT_CLI_CUSTOM_CAMP_SINGLETON])

#endif // !DOXYGEN

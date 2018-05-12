// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/alox.hpp"
#include "alib/cli/cliapp.hpp"

#if !defined (HPP_ALIB_LANG_RESOURCE_TUPLE_LOADER)
#  include "alib/lang/resourcedtupleloader.hpp"
#endif


using namespace std;

namespace aworx { namespace lib {

// The singleton of the library class
cli::CLILib CLI;


/** ************************************************************************************************
 * # %ALib %CLI - Programmer's Manual #
 *
 * \attention
 *   Unfortunately, the programmer's manual is not written yet, apart from the few paragraphs
 *   found below. In fact, this whole \alibmod is more or less in a beta state.<br>
 *   You might want to wait for a next library release to see this fixed!
 *
 *
 * This namespace comprises <b>%ALib Module CLI</b>, which provides a foundation of classes
 * to create command line interface (CLI) programs.
 * Such processing is not only useful with "pure CLI commands" but also with
 * other software that should be enabled to receive parameter options, may it be daemons or
 * GUI applications.
 *
 * The processing of command line parameters is very much dependent on the task that is to
 * performed and the parameter and option "syntax" that needs to be supported.
 * Therefore, by definition, tool classes like this can never support each and every use case and
 * any unforeseen tweak a user want to implement. (At least we were not able to implement it that
 * way.)
 *
 * Therefore, classes found in this namespace are:
 * - Typical \alib Bauhaus code style, e.g, fields are mostly public and the interface methods allow
 *   to do modifications of data between the several steps of their invocation.
 *
 * - Incomplete and even partly inconsistent in respect to what can be defined as commands,
 *   parameters, etc. and what action can be "automatically" done with the information
 *   provided.
 *
\~Comment ####################################################################################### \~
 * ## %ALib %Enum Meta Data ##
 * The classes found in this namespace make a lot of use of
 * \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } stored in \alib{lang,Resources,ALib Resources}.
 * Therefore, it is a good advice to get familiar with both \alib features before using this class.
 *
\~Comment ####################################################################################### \~
 * ## Defining %Commands, %Parameters, %Options and %ExitCodes ##
 * Objects of the type of the structs
 * \alib{cli,CommandDecl},
 * \alib{cli,ParameterDecl},
 * \alib{cli,OptionDecl} and
 * \alib{cli,ExitCodeDecl}, are all defined using \alib{lang,T_EnumMetaDataDecl,ALib enum meta data }.
 * The type definiton of the meta data for these object types has to strictly follow
 * a fixed scheme. The types are defined with macro \ref ALIB_ENUM_SPECIFICATION_DECL by
 * library \alibmod_nolink_cli.
 *
 * To associate a custom enum type with one of these object types, macros are provided, which
 * interally use \alib macro \ref ALIB_ENUM_SPECIFICATION. The following table summarizes
 * this:
 *
 * |Object Type           | %Enum Meta Data Type| Macro To Use
 * |----------------------|---------------------|----------------------------------------------------
 * |\alib{cli,CommandDecl}    | See \ref CommandDecl    | \ref ALIB_CLI_COMMANDS   "ALIB_CLI_COMMANDS( TEnum, ResourceLibrary, ResourceName )"
 * |\alib{cli,ParameterDecl}  | See \ref ParameterDecl  | \ref ALIB_CLI_PARAMETERS "ALIB_CLI_PARAMETERS( TEnum, ResourceLibrary, ResourceName )"
 * |\alib{cli,OptionDecl}     | See \ref OptionDecl     | \ref ALIB_CLI_OPTIONS    "ALIB_CLI_OPTIONS( TEnum, ResourceLibrary, ResourceName )"
 * |\alib{cli,ExitCodeDecl}   | See \ref ExitCodeDecl   | \ref ALIB_CLI_EXIT_CODES "ALIB_CLI_EXIT_CODES( TEnum, ResourceLibrary, ResourceName )"
 *
 * associated with custom enum types th
 *
 *
\~Comment ####################################################################################### \~
 * ## Allowing Other Parts Of The Software To Access CLI Arguments ##
 * If other parts of a software (for example a library used) should have access to the command
 * line and "consume" those parameters and options that are addressing them, this is possible to do
 * using this class.
 *
 * \note
 *   An important sample, occurring with the use of other <b>%ALib Modules</b> already, are
 *   \alib{config,Variable,configuration variables}, which may be defined using CLI parameters
 *   the same as by using environment variables, configuration files or other data sources.
 *
 * Because this class uses its own copy of the argument list and exposes the original ones
 * (with fields
 * \alib{cli::CLIApp,ArgcOriginal},
 * \alib{cli::CLIApp,ArgNOriginal} and
 * \alib{cli::CLIApp,ArgWOriginal})  * other parts of the software have
 * still have full access to the CLI. In addition, this class exposes field
 * \alib{cli::CLIApp,OptionArgsIgnored},
 * that hold all CLI arguments that start with a hyphen <c>'-'</c> and are not recognized
 * by \alib{cli,OptionDecl}. A software might use this shortened list to look for options.
 *
 * The other way round, arguments may be removed from the internal list held by this class, prior
 * to have this class processing the arguments. Note that this is a must for  arguments that do not
 * start with a hyphen and are  addressing other software parts, because such arguments would
 * lead to "unknown command" errors.
 * Such remove should be done after the invocations of the various \b %Define methods and prior
 * to invoking \alib{cli,CLIApp::ReadOptions}.
 *
 *
 *
 * # Reference Documentation
 **************************************************************************************************/
namespace cli {


// ##########################################################################################
// ### Library class implementation
// ##########################################################################################

CLILib::CLILib()
: lib::lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("CLI") )
{
}

#define EOS ,
void CLILib::init( Phases phase )
{
    if( phase == Phases::resourceset )
    {
        lib::ALIB.CheckCompatibility( ALIB_VERSION, ALIB_COMPILATION_FLAGS );

        ALIB_BOXING_DEFINE_IAPPLY_FOR_APPLICABLE_TYPE( cli::Exceptions );


        Res->AddBulk( ResourceCategory.ToCString(),

            //--------- Exceptions ------
            "ExceptionsPrefix" , "cli::" ,
            "ExceptionsPostfix", ""         ,
            "Exceptions",
                // general exceptions
                "1,NoCommandGiven"                          ",Exc1"  ","
                "2,UnknownCommand"                          ",Exc2"  ","

                // option parsing exceptions
                "10,ParsingOptions"                         ",Exc10" ","
                "11,MissingOptionValue"                     ",Exc11" ","
                "12,IllegalOptionNameContinuation"          ",Exc12" ","

                // command parsing exceptions
                "20,ParsingCommand"                        ",Exc20"  ","
                "21,MissingParameterValue"                 ",Exc22"  EOS

            // messages of general exceptions
            "Exc1",  "No command given."                                                           ,
            "Exc2",  "Unknown command found with with CLI argument[{}]={!Q}"                       ,
            "Exc10", "Error parsing option. Usage:\\n"
                     "{}"                                                                          ,
            "Exc20", "Error parsing command.\\n"
                     "Usage: {}\\n"
                     "{}"                                                                          ,


            // messages of internal exceptions
            "Exc11", "Missing argument(s) of option <{}> found with CLI argument[{}]={!Q}.\\n"
                     "Expected {}, given {}."                                                      ,

            "Exc12", "Option identifier <{}> found at start of CLI argument[{}]={!Q},\\n"
                     "while CLI argument string continues with non-argument identifier characters.",

            "Exc21", "Missing argument(s) of parameter <{}> found with CLI argument[{}]={!Q}.\\n"
                     "Quantity expected {}, quantity given {}."                                    ,


            //--------- DryRunModes ------
            "DryRunModesPrefix" , "DryRunModes::" ,
            "DryRunModesPostfix", ""         ,
            "DryRunModes",
                "0,Off"                    ",1,"
                "2,CLIDeclarations"         ",4,"
                "1,CLIArgs"                ",1,"
                "3,Application"            ",1"      EOS

       // end of AddBulk()
        nullptr );


    }
    else if( phase == Phases::resourceload )
    {

    }
    else if( phase == Phases::final )
    {
    }
}


void CLILib::terminationCleanUp()
{
}


}}}// namespace aworx::lib::cli


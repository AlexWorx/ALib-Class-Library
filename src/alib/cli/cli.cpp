// #################################################################################################
//  ALib - A-Worx Utility Module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_CLI_CLI)
#   include "alib/cli/cliapp.hpp"
#endif

#if !defined (HPP_ALIB_LIB_ALIBMODULES)
#    include "alib/lib/alibmodules.hpp"
#endif

#if !defined (HPP_ALIB_RESULTS_RESULTS)
#    include "alib/results/results.hpp"
#endif


ALIB_BOXING_VTABLE_DEFINE( aworx::lib::cli::Exceptions, vt_cli_exceptions )

namespace aworx { namespace lib {

cli::Cli CLI;

/** ************************************************************************************************
 * \attention
 *   Unfortunately, this \alibmod is not completed and is more or less in an alpha version state.
 *   You might want to wait for a next library release to see this fixed!<br>
 *   The same is true for this reference documentation. It was never reviewed and might be
 *   completely wrong and outdated for many entities!<br>
 *   Finally, there is really only a start of a \ref alib_mod_cli "Programmer's Manual"
 *   available.
 **************************************************************************************************/
namespace cli {


// ##########################################################################################
// ### Module class implementation
// ##########################################################################################

Cli::Cli()
: Module( ALIB_VERSION, ALIB_REVISION, "ALIB_CLI" )
{
    ALIB_ASSERT_ERROR( this == &CLI,
        "Instances of class Cli must not be created. Use singleton aworx::lib::CLI" )

    Dependencies.emplace_back( &lib::RESULTS );
}

#define EOS ,
void Cli::init( InitLevels level, int, const char**, const wchar_t** )
{
    if( level == InitLevels::PrepareResources )
    {
        ALIB.CheckDistribution();

        ALIB_BOXING_VTABLE_REGISTER( vt_cli_exceptions )
        ALIB_BOXING_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( cli::Exceptions );


        Resources->AddBulk( ResourceCategory,

            //--------- Exceptions ------
            "ExceptionsPrefix" , A_CHAR( "cli::" ) ,
            "ExceptionsPostfix", A_CHAR( ""      ) ,
            "Exceptions",
                    // general exceptions
             A_CHAR("1,NoCommandGiven"                          ",Exc1"  ","
                    "2,UnknownCommand"                          ",Exc2"  ","

                    // option parsing exceptions
                    "10,ParsingOptions"                         ",Exc10" ","
                    "11,MissingOptionValue"                     ",Exc11" ","
                    "12,IllegalOptionNameContinuation"          ",Exc12" ","

                    // command parsing exceptions
                    "20,ParsingCommand"                        ",Exc20"  ","
                    "21,MissingParameterValue"                 ",Exc21"      )EOS

            // messages of general exceptions
            "Exc1",   A_CHAR( "No command given."                                     )                    ,
            "Exc2",   A_CHAR( "Unknown command found with with CLI argument[{}]={!Q}" )                    ,
            "Exc10",  A_CHAR( "Error parsing option. Usage:\\n"
                            "{}"                                                    )                      ,
            "Exc20",  A_CHAR( "Error parsing command.\\n"
                            "Usage: {}\\n"
                            "{}"                                                    )                      ,


            // messages of internal exceptions
            "Exc11",  A_CHAR( "Missing argument(s) of option <{}> found with CLI argument[{}]={!Q}.\\n"
                            "Expected {}, given {}."                                                   )  ,

            "Exc12",  A_CHAR( "Option identifier <{}> found at start of CLI argument[{}]={!Q},\\n"
                            "while CLI argument string continues with non-argument identifier characters." ),

            "Exc21",  A_CHAR( "Missing argument(s) of parameter <{}> found with CLI argument[{}]={!Q}.\\n"
                            "Quantity expected {}, quantity given {}."                                     ),


            //--------- DryRunModes ------
            "DryRunModesPrefix" ,  A_CHAR( "DryRunModes::" ),
            "DryRunModesPostfix",  A_CHAR( ""              ),
            "DryRunModes",
              A_CHAR( "0,Off"                    ",1,"
                    "2,CLIDeclarations"        ",4,"
                    "1,CLIArgs"                ",1,"
                    "3,Application"            ",1"      ) EOS

       // end of AddBulk()
        nullptr );


    }
}


void Cli::terminationCleanUp()
{
}


}}}// namespace aworx::lib::cli


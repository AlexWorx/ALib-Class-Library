// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/commandline.hpp"
#   endif

#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined (HPP_ALIB_LANG_BASECAMP)
#      include "alib/lang/basecamp/basecamp.hpp"
#   endif
#endif // !defined(ALIB_DOX)


ALIB_BOXING_VTABLE_DEFINE( alib::cli::Exceptions, vt_cli_exceptions )

namespace alib {

cli::Cli CLI;

/** ************************************************************************************************
 * This is the reference documentation of sub-namespace \c cli of the \aliblink, which
 * holds types of library module \alib_cli_nl.
 *
 * Besides this reference documentation, further documentation and source code samples for this
 * module is provided with \ref alib_mod_cli "ALib Module CLI - Programmer's Manual".
 **************************************************************************************************/
namespace cli {

Cli::Cli()
: Camp( "CLI" )
{
    ALIB_ASSERT_ERROR( this == &CLI, "CLI",
        "Instances of class Cli must not be created. Use singleton alib::CLI" )
}

#define EOS ,
void Cli::bootstrap( BootstrapPhases phase )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB_BOXING_BOOTSTRAP_VTABLE_DBG_REGISTER( vt_cli_exceptions )
        ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( cli::Exceptions )


#if !ALIB_RESOURCES_OMIT_DEFAULTS
        resourcePool->BootstrapBulk( ResourceCategory,

            //--------- Exceptions ------
            "E<" ,   A_CHAR( "cli::" ),

            // general exceptions
            "E0",    A_CHAR( "1,NoCommandGiven"                      ",ED1"   ),
            "E1",    A_CHAR( "2,UnknownCommand"                      ",ED2"   ),

            // option parsing exceptions
            "E2",    A_CHAR( "10,ParsingOptions"                     ",ED10"  ),
            "E3",    A_CHAR( "11,MissingOptionValue"                 ",ED11"  ),
            "E4",    A_CHAR( "12,IllegalOptionNameContinuation"      ",ED12"  ),

            // command parsing exceptions
            "E5",    A_CHAR( "20,ParsingCommand"                     ",ED20"  ),
            "E6",    A_CHAR( "21,MissingParameterValue"              ",ED21"  ) EOS

            // messages of general exceptions
            "ED1",   A_CHAR( "No command given."                                       ),
            "ED2",   A_CHAR( "Unknown command found with with CLI argument{!Q[]}={!Q}" ),
            "ED10",  A_CHAR( "Error parsing option. Usage:\n"
                             "{}"                                                      ),
            "ED20",  A_CHAR( "Error parsing command.\n"
                             "Usage: {}\n"
                             "{}"                                                      ),


            // messages of internal exceptions
            "ED11",  A_CHAR( "Missing argument(s) of option {!Q<>} found with CLI argument{!Q[]}={!Q}.\n"
                             "Expected {}, given {}."                                                       ),

            "ED12",  A_CHAR( "Option identifier {!Q<>} found at start of CLI argument{!Q[]}={!Q},\n"
                             "while CLI argument string continues with non-argument identifier characters." ),

            "ED21",  A_CHAR( "Missing argument(s) of parameter {!Q<>} found with CLI argument{!Q[]}={!Q}.\n"
                             "Quantity expected {}, quantity given {}."                                     ) EOS


            //--------- DryRunModes ------
            "DRM<",  A_CHAR( "DryRunModes::"                       ),
DOX_MARKER([DOX_ALIB_CLI_DRYRUN_RESOURCES])
"DRM" ,  A_CHAR( "0"  ","   "Off"              ","      "1"  ","
                 "2"  ","   "CLIDeclarations"  ","      "4"  ","
                 "1"  ","   "CLIArgs"          ","      "1"  ","
                 "3"  ","   "Application"      ","      "1"       ),
DOX_MARKER([DOX_ALIB_CLI_DRYRUN_RESOURCES])
       // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_RESOURCES_OMIT_DEFAULTS

    }

    else if( phase == BootstrapPhases::PrepareConfig )
    {
        EnumRecords<Exceptions >::Bootstrap();
        EnumRecords<DryRunModes>::Bootstrap( *this, "DRM" );
    }
}


}} // namespace alib::cli

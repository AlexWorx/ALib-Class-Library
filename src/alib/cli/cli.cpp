// #################################################################################################
//  ALib - A-Worx Utility Module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#   if !defined (HPP_ALIB_CLI_CLI)
#      include "alib/cli/cliapp.hpp"
#   endif

#   if !defined(HPP_ALIB_ENUMS_SERIALIZATION)
#      include "alib/enums/serialization.hpp"
#   endif
#   if !defined (HPP_ALIB_ENUMS_RECORDBOOTSTRAP)
#      include "alib/enums/recordbootstrap.hpp"
#   endif
#   if !defined (HPP_ALIB_RESULTS_RESULTS)
#      include "alib/results/results.hpp"
#   endif
#   if !defined (HPP_ALIB_FS_MODULES_DISTRIBUTION)
#      include "alib/lib/fs_modules/distribution.hpp"
#   endif
#endif // !defined(ALIB_DOX)


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
: Module( ALIB_VERSION, ALIB_REVISION, "CLI" )
{
    ALIB_ASSERT_ERROR( this == &CLI,
        "Instances of class Cli must not be created. Use singleton aworx::lib::CLI" )
}

#define EOS ,
void Cli::bootstrap( BootstrapPhases phase, int, const char**, const wchar_t** )
{
    if( phase == BootstrapPhases::PrepareResources )
    {
        ALIB.CheckDistribution();

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
            "ED10",  A_CHAR( "Error parsing option. Usage:\\n"
                             "{}"                                                      ),
            "ED20",  A_CHAR( "Error parsing command.\\n"
                             "Usage: {}\\n"
                             "{}"                                                      ),


            // messages of internal exceptions
            "ED11",  A_CHAR( "Missing argument(s) of option {!Q<>} found with CLI argument{!Q[]}={!Q}.\\n"
                             "Expected {}, given {}."                                                       ),

            "ED12",  A_CHAR( "Option identifier {!Q<>} found at start of CLI argument{!Q[]}={!Q},\\n"
                             "while CLI argument string continues with non-argument identifier characters." ),

            "ED21",  A_CHAR( "Missing argument(s) of parameter {!Q<>} found with CLI argument{!Q[]}={!Q}.\\n"
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


}}}// namespace aworx::lib::cli


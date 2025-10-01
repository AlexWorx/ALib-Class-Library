// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/resources/resources.prepro.hpp"
#include "alib/camp/camp.prepro.hpp"
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.CLI;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
#  if ALIB_BOXING
    import   ALib.Boxing;
#  endif
#  if ALIB_CAMP
    import   ALib.Camp;
#  endif
#else
#   include "ALib.Strings.H"
#   include "ALib.Boxing.H"
#   include "ALib.Camp.H"
#   include "ALib.CLI.H"
#endif
#   include "ALib.Characters.Functions.H"
// ======================================   Implementation   =======================================
ALIB_BOXING_VTABLE_DEFINE( alib::cli::Exceptions, vt_cli_exceptions )

namespace alib {

cli::CliCamp CLI;

//==================================================================================================
/// This is the reference documentation of sub-namespace \c cli of the \aliblink, which
/// holds types of library module \alib_cli_nl.
///
/// Besides this reference documentation, further documentation and source code samples for this
/// module is provided with \ref alib_mod_cli "ALib Module CLI - Programmer's Manual".
//==================================================================================================
namespace cli {

CliCamp::CliCamp()
: Camp( "CLI" )
{
    #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
      ALIB_ASSERT_ERROR( this == &CLI, "CLI",
          "Instances of class Cli must not be created. Use singleton alib::CLI" )
    #endif
}

#define EOS ,
void CliCamp::Bootstrap()
{
    if( GetBootstrapState() == BootstrapPhases::PrepareResources )
    {

#if !ALIB_CAMP_OMIT_DEFAULT_RESOURCES
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
            "ED2",   A_CHAR( "Unknown command found with CLI argument{!Q[]}={!Q}" ),
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
DOX_MARKER([DOX_CLI_DRYRUN_RESOURCES])
"DRM" ,  A_CHAR( "0"  ","   "Off"              ","      "1"  ","
                 "2"  ","   "CLIDeclarations"  ","      "4"  ","
                 "1"  ","   "CLIArgs"          ","      "1"  ","
                 "3"  ","   "Application"      ","      "1"       ),
DOX_MARKER([DOX_CLI_DRYRUN_RESOURCES])
       // end of BootstrapBulk()
        nullptr );
#endif // !ALIB_CAMP_OMIT_DEFAULT_RESOURCES

        // parse enum records
        enumrecords::bootstrap::Bootstrap<Exceptions >();
        enumrecords::bootstrap::Bootstrap<DryRunModes>( *this, "DRM" );
    }

    else if( GetBootstrapState() == BootstrapPhases::PrepareConfig )
    {}
    else if( GetBootstrapState() == BootstrapPhases::Final )
    {}

}


}} // namespace alib::cli


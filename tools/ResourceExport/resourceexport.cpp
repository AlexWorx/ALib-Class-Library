// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
//  Notes:
//   This tool exports ALib resources for externalization.
//   See section "3.5.3 Exporting Resources For Externalization" of the Programmer's Manual
//   of ALib for more information.
//
//   Starting with version 1903, the output of this tool for each version is found in folder
//   <b>./docs/pages/resource-exports/</b> of the ALib base directory.
// #################################################################################################

// DOX_MARKER([DOX_ALIB_RESOURCES_EXPORT])
#include "alib/lang/basecamp/basecamp.hpp"
#include "alib/lang/basecamp/bootstrap.hpp"
#include "alib/lang/resources/configresourcepool.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"

using namespace alib;
using namespace std;
int main( int argc, const char *argv[] )
{
    // create and set resource pool that uses a configuration file
    lang::resources::ConfigResourcePool pool;
    alib::BootstrapAddDefaultCamps();
    alib::Camps.Back()->BootstrapSetResourcePool( &pool );

    // bootstrap alib
    alib::ArgC = argc;
    alib::ArgVN= argv;
    alib::Bootstrap();

    // we externalize the string value, e.g. replacing "\" by "\\" and this way "\n" by "\\n"
    // This might not be wanted for custom exports, but works well for ALib INI-files.
    config::XTernalizer  externalizer;
    AString              externalizedValue;

    // loop over sections of the plugin
    auto& sections= pool.Config.GetPluginTypeSafe<InMemoryPlugin>(
                                           Priorities::DefaultValues )
                    ->Sections();

    for( auto& section : sections )
    {
        // no entries in section? (happens only for first, empty category)
        if( section.Entries().IsEmpty() )
            continue;

        // write category
        cout << endl << '[' << section.Name() << ']' << endl;

        // loop over resources in actual category
        for( auto& entry : section.Entries() )
        {
            // externalize and write
            externalizer.ExternalizeValue( entry.Value, externalizedValue.Reset()  , 0 );
            cout << entry.Name() << '=' << externalizedValue  << endl;
        }
    }

    // shutdown alib
    alib::Shutdown();
    return 0;
}
// DOX_MARKER([DOX_ALIB_RESOURCES_EXPORT])

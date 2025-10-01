// #################################################################################################
//  ALib C++ Library - Resource Export Tool
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
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

// DOX_MARKER([DOX_RESOURCES_EXPORT])
#include "ALib.Bootstrap.H"
#include "ALib.Strings.StdIOStream.H"

#include "ALib.Variables.H"
#include "ALib.Variables.ResourcePool.H"

using namespace alib;
using namespace std;
int main( int argc, const char *argv[] )
{
    // before bootstrapping, we initialize the global allocator "manually" to be able to use
    // it with the SharedPtr below.
    new (&alib::monomem::GLOBAL_ALLOCATOR) MonoAllocator(ALIB_DBG("GlobalAllocator", ) 100, 150);

    // create and set resource pool that uses a configuration file
    camp::Camp::SPResourcePool spPool;
    spPool.InsertDerived<ConfigResourcePool>(monomem::GLOBAL_ALLOCATOR);
    auto& pool= static_cast<ConfigResourcePool&>( *spPool );

    alib::BootstrapAddDefaultCamps();
    alib::CAMPS.back()->BootstrapSetResourcePool( spPool );

    // bootstrap alib
    alib::ARG_C = argc;
    alib::ARG_VN= argv;
    alib::Bootstrap();

    // we externalize the string value, e.g., replacing "\" by "\\" and this way "\n" by "\\n"
    // This might not be wanted for custom exports but works well for ALib INI-files.
    StringEscaperStandard externalizer;
    AString               externalizedValue;

    // loop over "sections", which is the first level of nodes
    auto section= pool->Root();
    section.GoToFirstChild();
    while( section.IsValid() )
    {
        // no entries in section? (happens only for the first, empty category)
        if( section.CountChildren() == 0 )
            continue;

        // write category
        cout << endl << '[' << section.Name() << ']' << endl;

        // loop over resources in actual category
        auto entry= section.FirstChild();
        while( entry.IsValid() )
        {
            // externalize and write
            externalizer.Escape( Variable(entry).GetString(), externalizedValue.Reset(), A_CHAR("") );
            cout << entry.Name() << '=' << externalizedValue  << endl;
            entry.GoToNextSibling();
        }
        section.GoToNextSibling();
    }

    // shutdown alib
    alib::Shutdown();
    return 0;
}
// DOX_MARKER([DOX_RESOURCES_EXPORT])

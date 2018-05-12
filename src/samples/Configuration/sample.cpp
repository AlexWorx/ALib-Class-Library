// #################################################################################################
//  ALib - A-Worx Utility Library
//  Configuration Sample
//
//  Copyright 2018 A-Worx GmbH, Germany
//  Published under Boost Software License (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/config/configuration.hpp"
#include "alib/config/inifile.hpp"
#include "alib/system/directory.hpp"
#include "alib/compatibility/std_iostream.hpp"

#include <fstream>

using namespace aworx;

int main(  int argc, char *argv[]  )
{
    //
    // Initialize ALib and pass command line arguments to the default configuration instance
    //
    lib::ALIB.Init( argc, argv );

    //
    // first, lets write a simple INI-file
    //
    AString fileName;
    {
        const char* iniFileContents=
        "##########################################################################"  "\n"
        "## config sample INI-file"                                                   "\n"
        "##########################################################################"  "\n"
        ""                                                                            "\n"
        "// This is a variable comment"                                               "\n"
        "Test=     Test Variable Content"                                             "\n"
        ""                                                                            "\n"
        "[My_Section]"                                                                "\n"
        "SectionVar = 5"                                                              "\n"
        "PI=3.14159"                                                                  "\n"
        ;

        Directory::CurrentDirectory( fileName );
        fileName <<  "/sample.ini";

        std::ofstream iniFile;
        iniFile.open ( fileName.ToCString() );
        iniFile << iniFileContents;
        iniFile.close();
    }


    //
    // Open the INI File
    //
    IniFile iniFile( fileName );

    //
    // Now we read some values
    //
    std::cout << "Reading Variables from INI-file: " << std::endl;

    Variable var;

    iniFile.Load( var.Declare( "",           "Test"        ) );
    std::cout << "  Test:        " << *var.GetString() << std::endl;

    iniFile.Load( var.Declare( "MY_SECTION", "SectionVar"  ) );
    std::cout << "  SectionVar:  " << *var.GetString() << std::endl;

    iniFile.Load( var.Declare( "MY_SECTION", "PI"          ) );
    std::cout << "  PI:          " << *var.GetString() << std::endl;
    std::cout << "  PI as float: " <<  var.GetFloat()  << std::endl;

    //
    // Now we attach the INI-File to the configuration and read the same variables once more
    //
    lib::ALIB.Config->InsertPlugin( &iniFile, Priorities::Standard );

    std::cout << std::endl;
    std::cout << "Reading Variables from Configuration: " << std::endl;
    std::cout << "(If you specify command line args --TEST=XYZ or --MY_SECTION_SECTIONVAR=42, " <<std::endl;
    std::cout << "then these will overwrite the values from the INI-file)" <<std::endl;


    lib::ALIB.Config->Load( var.Declare( "",           "Test"        ) );
    std::cout << "  Test:        " << *var.GetString() << std::endl;

    lib::ALIB.Config->Load( var.Declare( "MY_SECTION", "SectionVar"  ) );
    std::cout << "  SectionVar:  " << *var.GetString() << std::endl;

    lib::ALIB.Config->Load( var.Declare( "MY_SECTION", "PI"          ) );
    std::cout << "  PI:          " << *var.GetString() << std::endl;
    std::cout << "  PI as float: " <<  var.GetFloat()  << std::endl;


    //
    // Finally, add a new variable. This will go to an internal in-memory plug-in used for default values
    //
    var.Declare( "New_Section",  "programatically", 0, "This variable was written by the test program"   );
    var.Add("written");
    lib::ALIB.Config->Store( var );

    //
    // Copy programatically set default values to the INI-file
    //
    lib::ALIB.Config->FetchFromDefault( iniFile );
    iniFile.WriteFile();

    return 0;
}

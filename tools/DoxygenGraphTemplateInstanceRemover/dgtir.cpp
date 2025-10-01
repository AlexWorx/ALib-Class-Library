// #################################################################################################
//  ALib C++ Library Tools - DoxygenGraphTemplateInstanceRemover
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
//
// Notes:
// - This tool replaces simple, pure 'integer' template instantiation nodes in doxygen
//   dot files by their parameterized template class counterparts.
//   Furthermore inheritance graphs that contain only one template type node are deleted (the node
//   is deleted from the file) when the template parameter is not the default one.
//   Finally, all namespace names and newline tokens are removed from the node names
//   (occurs only in template parameters).
// - Since Feb. 24, the above is not true anymore. The single template classes are not
//   removed anymore. It became too complicated. The type hierarchy is anyhow quite useless
//   and will not very often be opened by users. The "per class" diagrams are much more
//   useful to have and are placed in every class now.
// - Since March 24, a new fix has been implemented.
//
// - This is a quick and dirty written piece of code.
//   Maybe doxygen will support an option to remove instantiated classes some day.
// #################################################################################################

// to preserve the right order, we are not includable directly from outside.
#include "ALib.ALox.H"
#include "ALib.Bootstrap.H"

using namespace alib;
using namespace std;



extern int main2402();
extern int postProcessHTMLFiles(const String& srcDir);

int main(int argc, char *argv[])
{
    // init ALib
    alib::ARG_C = argc;
    alib::ARG_VN= const_cast<const char**>(argv);
    alib::Bootstrap();


    if( argc == 3 && CString(argv[1]) == "postprocess")
    {
        auto result=  postProcessHTMLFiles( CString(argv[2]));
        alib::Shutdown();
        return result;
    }

    auto result= main2402();

    alib::Shutdown();
    return result;
}

// #################################################################################################
//  Documentation - ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

/**
\page alib_tools      Tools for ALib

\tableofcontents

\note We are happy to receive user input that helps coding with \alib with different IDEs,
      debuggers and other software development tools.


\I{################################################################################################}
# 1. Pretty Printers for GNU Debugger (gdb) ## {#alibtools_debug_helpers_gdb}

GNU debugger supports python scripts that allow the display of complex data types with so called
\https{pretty printers,sourceware.org/gdb/onlinedocs/gdb/Pretty-Printing-API.html#Pretty-Printing-API}.
\alib provides such script for some of its most important classes, for example, all string types,
class \alib{files;File}, class \alib{config;Variable} and several more.

The script is found in the ALox source folder under

        tools/ideplugins/gdb/alibpp.py

We think the biggest problem of the concept is that the contract between the debugger and
the printers is not duly specified. This means, it is not clear, what the printers should deliver
and which format to use. Now, different IDEs seem to interpret the output of such printers 
differently, and the development of the script was a pain... and with new types it is never finished.
Nevertheless, they are very helpful, of course and work well in 
\https{JetBrains CLion,www.jetbrains.com/clion}

To activate the script, under GNU/Linux you need to add the following lines to your
<c>.gdbinit</c>:

    # ALox pretty printers
    python print( ".gdbinit: Inserting ALib/ALox Pretty Printers" )
    python sys.path.insert(0, "/home/dev/A-Worx/ALib/tools/ideplugins/gdb" )
    python import alibpp

Be sure to adopt the path to the location of your project directory.


\I{################################################################################################}
# 2. Doxygen Dot-File Fixer # {#alibtools_DoxygenGraphTemplateInstanceRemover}
A small tool for patching the diagrams of the \https{Doxygen,https://www.doxygen.nl}
documentation of \alib.
The cmake project is found in the ALox source folder under:

    tools/DoxygenGraphTemplateInstanceRemover

It consists only of a single \e main.cpp file and the CMake target is configured to write
the release version into the

    docs/

folder. There, when available, the tool is used by doxygen as a replacement to the
\https{Graphviz dot tool,graphviz.org}. The tool patches the dot file that is to be processed
and then invokes the original software.<br>
This tool is only needed, if the HTML documentation (which you are currently reading) is to
be 'compiled' from scratch.

<br><br><br><br><br><br> */

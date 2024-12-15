// #################################################################################################
//  AWorx ALib Unit Tests
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_FILES && ALIB_DEBUG

DOX_MARKER( [DOX_FILES_HEADER])
#include "alib/files/ftree.hpp"
#include "alib/files/fscanner.hpp"
using namespace alib;
using namespace std;
DOX_MARKER( [DOX_FILES_HEADER])
DOX_MARKER( [DOX_FILES_HEADER2])
#include "alib/files/fileexpressions.hpp"
DOX_MARKER( [DOX_FILES_HEADER2])
#include <iostream>
#include <sstream>
#include "alib/compatibility/std_strings.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"

#define TESTCLASSNAME       UT_Dox_Files
#include "unittests/aworx_unittests.hpp"

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( disable:4100 ) // unreferenced formal parameter

#endif

//--------------------------------------------------------------------------------------------------
//--- DOCUMENTATION SAMPLES
//--------------------------------------------------------------------------------------------------

namespace std
{
    extern std::stringstream testOSFiles; // declaration (needed when clang warnings are on)
           std::stringstream testOSFiles;
}
#define cout testOSFiles
using namespace std;

// ###################################   Test Class   #############################################

namespace ut_aworx {

UT_CLASS


    UT_METHOD( filesMain )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")

DOX_MARKER( [DOX_FILES_OBJECTS])
// Beginning of main(). Initialize ALIB once
alib::Bootstrap();

// The file tree to collect the results (using the shared, self-contained version).
SharedFTree                 fileTree(10);

// The scan parameters. Apart from the start directory, we leave everything on defaults.
ScanParameters              scanParameters(ALIB_BASE_DIR  A_PATH("/src/alib/files") );

// A vector to collect the result path(s).
std::vector<ResultsPaths>   resultPaths;
DOX_MARKER( [DOX_FILES_OBJECTS])

DOX_MARKER( [DOX_FILES_SCAN])
// Scan files (If multithreaded, this uses the SharedLock from the SharedFTree
{
    files::ScanFiles( fileTree, scanParameters, resultPaths);
}
    
// Dump files to the console. Lock in shared mode (read only)
AString dumpBuffer;
{ALIB_LOCK_SHARED_WITH(fileTree)
    DbgDump(dumpBuffer, *fileTree);
}
cout << dumpBuffer << endl;
DOX_MARKER( [DOX_FILES_SCAN])
        ut.WriteResultFile( "DOX_FILES_MAIN.txt", testOSFiles.str());
        testOSFiles.str("");

DOX_MARKER( [DOX_FILES_RESULTS])
cout << "Number of result paths: " << resultPaths.size() << endl;
cout << "          real path[0]: " << resultPaths.front().RealPath << endl;
DOX_MARKER( [DOX_FILES_RESULTS])

        ut.WriteResultFile( "DOX_FILES_RESULTS.txt", testOSFiles.str());
        testOSFiles.str("");

DOX_MARKER( [DOX_FILES_LOCK])
// Type alib::SharedFTree is an alias for alib::files::TSharedFTree<SharedLock>
SharedFTree  myFiles(10);
{
    // lock the FTree for this compound
    ALIB_LOCK_WITH(myFiles)

    //...
    //...
    //...
}// <- here, the lock is automatically released
DOX_MARKER( [DOX_FILES_LOCK])

}

    UT_METHOD( filesFexFilter )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")
    try
    {
DOX_MARKER( [DOX_FILES_SCAN_FEX1])
// Beginning of main(). Initialize ALIB once
alib::Bootstrap();

// objects needed for scanning files (using an automatic shared value)
SharedFTree                 fileTree(10); // 10 kb initial monotonic memory
ScanParameters              scanParameters(ALIB_BASE_DIR  A_PATH("/src/alib/files") );
std::vector<ResultsPaths>   resultPaths;

// create the expression compiler and let it compile a filter
FileExpressions fex;
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("name = \"ftree.hpp\""));

// Scan files
files::ScanFiles( fileTree, scanParameters, resultPaths);
DOX_MARKER( [DOX_FILES_SCAN_FEX1])

DOX_MARKER( [DOX_FILES_SCAN_FEX11])
fileTree.DbgCriticalSections(lang::Switch::Off);
DOX_MARKER( [DOX_FILES_SCAN_FEX11])

// Dump files to the console
AString dumpBuffer; DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILES_SCAN_FEX1.txt", testOSFiles.str(), "");


DOX_MARKER( [DOX_FILES_SCAN_FEX2])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("size > kilobytes(10)"));
DOX_MARKER( [DOX_FILES_SCAN_FEX2])
testOSFiles.str(""); fileTree.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
files::ScanFiles( fileTree, scanParameters, resultPaths );
dumpBuffer.Reset(); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILES_SCAN_FEX2.txt", testOSFiles.str(), "");

DOX_MARKER( [DOX_FILES_SCAN_FEX3])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("GetHour(date) % 2 == 0"));
DOX_MARKER( [DOX_FILES_SCAN_FEX3])
testOSFiles.str(""); fileTree.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
files::ScanFiles( fileTree, scanParameters, resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILES_SCAN_FEX3.txt", testOSFiles.str(), "");

DOX_MARKER( [DOX_FILES_SCAN_FEX4])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("IndexOf(name,\"camp\")>0"));
DOX_MARKER( [DOX_FILES_SCAN_FEX4])
testOSFiles.str(""); fileTree.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
files::ScanFiles( fileTree, scanParameters, resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILES_SCAN_FEX4.txt", testOSFiles.str(),"");

DOX_MARKER( [DOX_FILES_SCAN_FEX5])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("EndsWith(Path,\"files\")"));
DOX_MARKER( [DOX_FILES_SCAN_FEX5])
testOSFiles.str(""); fileTree.Reset();  fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
files::ScanFiles( fileTree, scanParameters, resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILES_SCAN_FEX5.txt", testOSFiles.str(), "");


    }
    catch (lang::Exception& e)
    {
        LogTools::Exception(LOG_LOX, e);
    }
    catch (std::exception& e)
    {
        Log_Error("std::exception caught: ", e.what() )
    }

    }
#include "unittests/aworx_unittests_end.hpp"

} //namespace


#endif //  ALIB_UT_DOCS && ALIB_DEBUG && ALIB_DEBUG

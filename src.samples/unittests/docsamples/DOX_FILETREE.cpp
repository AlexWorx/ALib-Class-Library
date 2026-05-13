// #################################################################################################
//  AWorx ALib Unit Tests
//  Copyright 2013-2026 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_FILETREE && ALIB_DEBUG

#include <iostream>
#include <sstream>

#include "ALib.Strings.H"
#include "ALib.Strings.StdIOStream.H"
DOX_MARKER( [DOX_FILETREE_HEADER])
#include "ALib.FileTree.H"

using namespace alib;
using namespace std;
DOX_MARKER( [DOX_FILETREE_HEADER])
DOX_MARKER( [DOX_FILETREE_HEADER2])
#include "ALib.FileTree.Expressions.H"
DOX_MARKER( [DOX_FILETREE_HEADER2])
#include "ALib.Exceptions.H"
#include "ALib.Bootstrap.H"

#define TESTCLASSNAME       UT_Dox_Files
#include "aworx_unittests.hpp"

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

DOX_MARKER( [DOX_FILETREE_OBJECTS])
// Beginning of main(). Initialize ALIB once
alib::Bootstrap();

// The file tree to collect the results (using the shared, self-contained version).
SharedFTree                 fileTree(10);

// The scan parameters. Apart from the start directory, we leave everything on defaults.
ScanParameters              scanParameters(ALIB_BASE_DIR  A_PATH("/src/alib/filetree") );

// A vector to collect the result path(s).
filetree::CanonicalPathList    resultPaths;
DOX_MARKER( [DOX_FILETREE_OBJECTS])

DOX_MARKER( [DOX_FILETREE_SCAN])
// Scan files. For this a full lock is needed.
{ALIB_LOCK_WITH(fileTree)
    filetree::ScanFiles(*fileTree, scanParameters, &resultPaths);
}
    
// Dump files to the console. Lock in shared mode (read only)
AString dumpBuffer;
{ALIB_LOCK_SHARED_WITH(fileTree)
    filetree::DbgDump(dumpBuffer, *fileTree);
}
cout << dumpBuffer << endl;
DOX_MARKER([DOX_FILETREE_SCAN])
        ut.WriteResultFile( "DOX_FILETREE_MAIN.txt", testOSFiles.str());
        testOSFiles.str("");

DOX_MARKER([DOX_FILETREE_RESULTS])
cout << "Number of result paths: " << resultPaths.size() << endl;
    Path realPath;
    Path symbolicPath;
    {ALIB_LOCK_SHARED_WITH(fileTree)
        resultPaths.front().AssembleRealPath(    realPath    , lang::Inclusion::Include);
        resultPaths.front().AssembleSymbolicPath(symbolicPath, lang::Inclusion::Include);
    }
    cout << "           real path[0]: " << realPath     << endl;
    cout << "       symbolic path[0]: " << symbolicPath << endl;
DOX_MARKER([DOX_FILETREE_RESULTS])

        ut.WriteResultFile( "DOX_FILETREE_RESULTS.txt", testOSFiles.str());
        testOSFiles.str("");

DOX_MARKER( [DOX_FILETREE_LOCK])
// Type alib::SharedFTree is an alias for alib::filetree::TSharedFTree<SharedLock>
SharedFTree  myFiles(10);
{
    // lock the FTree for this compound
    ALIB_LOCK_WITH(myFiles)

    //...
    //...
    //...
}// <- here, the lock is automatically released
DOX_MARKER( [DOX_FILETREE_LOCK])

}

    UT_METHOD( filesFexFilter )
    {
        UT_INIT()
        UT_PRINT("*** Documentation Sample +**")
    try
    {
DOX_MARKER( [DOX_FILETREE_SCAN_FEX1])
// Beginning of main(). Initialize ALIB once
alib::Bootstrap();

// objects needed for scanning files (using an automatic shared value)
SharedFTree                 fileTree(10); // 10 kb initial monotonic memory
fileTree.DbgCriticalSections(lang::Switch::Off); // we access this tree only from one thread
ScanParameters              scanParameters(ALIB_BASE_DIR  A_PATH("/src/alib/filetree") );
CanonicalPathList           resultPaths;

// create the expression compiler and let it compile a filter
FileExpressions fex;
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("name = \"files.mpp\""));

// Scan files
filetree::ScanFiles( *fileTree, scanParameters, &resultPaths);
DOX_MARKER( [DOX_FILETREE_SCAN_FEX1])

DOX_MARKER( [DOX_FILETREE_SCAN_FEX11])
fileTree.DbgCriticalSections(lang::Switch::Off);
DOX_MARKER( [DOX_FILETREE_SCAN_FEX11])

// Dump files to the console
AString dumpBuffer; DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILETREE_SCAN_FEX1.txt", testOSFiles.str(), "");


DOX_MARKER( [DOX_FILETREE_SCAN_FEX2])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("size > kilobytes(10)"));
DOX_MARKER( [DOX_FILETREE_SCAN_FEX2])
testOSFiles.str("");
        fileTree.Reset();
        fileTree.DbgCriticalSections(lang::Switch::Off);
        resultPaths.clear();
filetree::ScanFiles( *fileTree, scanParameters, &resultPaths );
dumpBuffer.Reset(); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILETREE_SCAN_FEX2.txt", testOSFiles.str(), "");

DOX_MARKER( [DOX_FILETREE_SCAN_FEX3])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("GetHour(date) % 2 == 0"));
DOX_MARKER( [DOX_FILETREE_SCAN_FEX3])
testOSFiles.str(""); fileTree.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
filetree::ScanFiles( *fileTree, scanParameters, &resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILETREE_SCAN_FEX3.txt", testOSFiles.str(), "");

DOX_MARKER( [DOX_FILETREE_SCAN_FEX4])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("IndexOf(name,\"camp\")>0"));
DOX_MARKER( [DOX_FILETREE_SCAN_FEX4])
testOSFiles.str(""); fileTree.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
filetree::ScanFiles( *fileTree, scanParameters, &resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILETREE_SCAN_FEX4.txt", testOSFiles.str(),"");

DOX_MARKER( [DOX_FILETREE_SCAN_FEX5])
scanParameters.FileFilter= fex.CreateFilter(A_CHAR("EndsWith(Path,\"files\")"));
DOX_MARKER( [DOX_FILETREE_SCAN_FEX5])
testOSFiles.str(""); fileTree.Reset();  fileTree.DbgCriticalSections(lang::Switch::Off); resultPaths.clear();
filetree::ScanFiles( *fileTree, scanParameters, &resultPaths);
dumpBuffer.Reset(); fileTree.DbgCriticalSections(lang::Switch::Off); DbgDump(dumpBuffer, *fileTree); cout << dumpBuffer << endl;
ut.WriteResultFile( "DOX_FILETREE_SCAN_FEX5.txt", testOSFiles.str(), "");


    }
    catch (exceptions::Exception& e)
    {
        LogTools::Exception(LOG_LOX, e);
    }
    catch (std::exception& e)
    {
        Log_Error("std::exception caught: ", e.what() )
    }

    }
#include "aworx_unittests_end.hpp"

} //namespace


#endif //  ALIB_UT_DOCS && ALIB_DEBUG && ALIB_DEBUG

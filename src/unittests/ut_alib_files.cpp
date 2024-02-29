// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"

#if ALIB_UT_FILES

#include "alib/alox.hpp"
#include "alib/files/fileexpressions.hpp"
#include "alib/files/fscanner.hpp"


#define TESTCLASSNAME       CPP_ALib_Files

#include "unittests/aworx_unittests.hpp"
#include <numeric>

using namespace std;
using namespace alib;

namespace ut_aworx {

namespace {
files::PathString scanPath;
ScanParameters sp(A_CHAR(""), ScanParameters::SymbolicLinks::RESOLVE_BUT_DONT_FOLLOW );
MonoAllocator ftreeMA(4096);
FTree ftree(&ftreeMA);
std::vector<ResultsPaths> resultPaths;
AString dumpBuf;
bool tstDoDump = false;
FileExpressions* fex= nullptr;
bool usePostRecursionDirFilter= false;

void testFScan( AWorxUnitTesting& ut,
                const String& dirFilter, const String& fileFilter,
                int expDirs, int expFiles )
{
    UT_PRINT( "Test scan {!Q}: DirFilter=<{}>({}), FileFilter=<{}>, RemEmpty={}",
              sp.StartPath,
              dirFilter, usePostRecursionDirFilter ? "PRE" : "POST",
              fileFilter,
              sp.RemoveEmptyDirectories )
    // scan files
    resultPaths.clear();
    ftree.Root().Delete();
    try
    {
        if(fileFilter.IsEmpty()) sp.FileFilter= nullptr;
        else                     sp.FileFilter= fex->CreateFilter(fileFilter);

        if( usePostRecursionDirFilter )
        {
            sp.DirectoryFilterPreRecursion= nullptr;
            if(dirFilter.IsEmpty() ) sp.DirectoryFilterPostRecursion= nullptr;
            else                     sp.DirectoryFilterPostRecursion= fex->CreateFilter(dirFilter);
        }
        else
        {
            sp.DirectoryFilterPostRecursion= nullptr;
            if(dirFilter.IsEmpty() ) sp.DirectoryFilterPreRecursion= nullptr;
            else                     sp.DirectoryFilterPreRecursion= fex->CreateFilter(dirFilter);
        }

        files::ScanFiles( ftree, sp, resultPaths);
    }
    catch(Exception& e)
    {
        (void) e;
        UT_PRINT( "An non-expected exception occurred while parsing filter expression:")
        Log_Exception( e )
    }


    // dump (before ut checking)
    #if ALIB_DEBUG
    if( tstDoDump )
    {
        int cnt= 0;
        for (auto& r : resultPaths)
        {
            UT_PRINT("Result {}/{}: {!Q} Q={}", ++cnt, resultPaths.size(), r.RealPath, r.Node.Value().Quality() )
        }

        EnumBitSet<FInfo::Types> included;
        included.Set();
        dumpBuf.Reset();
        DbgDump(dumpBuf, ftree, included );

        UT_PRINT(dumpBuf)
    }
    #else
    (void) tstDoDump;
    #endif

    // ut checks
    if( resultPaths.size())
    {
        FInfo&                v   = resultPaths.front().Node.Value();
        FInfo::DirectorySums& sums= v.Sums();
        if( expDirs  >= 0 ) UT_EQ( uint32_t(expDirs ), sums.CountDirectories()    )
        if( expFiles >= 0 ) UT_EQ( uint32_t(expFiles), sums.CountNonDirectories() )
    }
    else
    {
        if( expDirs  >= 0 ) UT_EQ( uint32_t(expDirs ), uint32_t(0) )
        if( expFiles >= 0 ) UT_EQ( uint32_t(expFiles), uint32_t(0) )
    }

}
}// anonymous namespace



UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Basics
//--------------------------------------------------------------------------------------------------
UT_METHOD(Basics)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Files::Basics ###") 
    fex= new FileExpressions();

    //------------- Test basic filters with scanner functions ----------------
    // tests in "/src/alib/expressions"
    sp.StartPath.Reset(ALIB_BASE_DIR) << DirectorySeparator << "src" 
		                              << DirectorySeparator << "alib"
									  << DirectorySeparator << "expressions";
//tstDoDump= true;
   testFScan(ut, nullptr                 , nullptr                             , 3 , 36 );
   testFScan(ut, nullptr                 , A_CHAR("IsDirectory")               , 3 ,  0 );
   testFScan(ut, nullptr                 , A_CHAR("name = \"expression.hpp\"") , 3 ,  1 );
   testFScan(ut, nullptr                 , A_CHAR("name * \"*.hpp\"")          , 3 , 19 );
   testFScan(ut, nullptr                 , A_CHAR("name * \"e*.hpp\"")         , 3 ,  4 );
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );

   sp.RemoveEmptyDirectories= true;
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

   // use pre recursion dir filter
   usePostRecursionDirFilter= false;
   sp.RemoveEmptyDirectories= false;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 3 , 14 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 3 , 11 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.hpp\"")          , 3 ,  6 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 2 , 14 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 1 , 11 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.hpp\"")          , 0 ,  6 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

   // use post recursion dir filter
   usePostRecursionDirFilter= true;
   sp.RemoveEmptyDirectories= false;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 3 , 14 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 3 , 11 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.hpp\"")          , 3 ,  6 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 2 , 14 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.hpp\"")          , 1 , 11 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.hpp\"")          , 0 ,  6 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

    //------------- Test all basic expression functions ----------------
   usePostRecursionDirFilter= false;
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, nullptr, A_CHAR("size > 40 * 1024")                             , 2,   3 );
   testFScan(ut, nullptr, A_CHAR("time > DateTime(2020 , 1, 1) &&  time < today + days(1)"), 3,  36 );
   testFScan(ut, nullptr, A_CHAR("time > today + days(1)")                                 , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("mtime > DateTime(2020, 1, 1) && mtime < today+ days(1)") , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("mtime > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("md    > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("mtime > DateTime(2020, 1, 1) && mtime < today+ days(1)") , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("mtime > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("md    > today + days(1)")                                , 0,   0 );
// tstDoDump= true;
   testFScan(ut, nullptr, A_CHAR("atime > DateTime(2020, 1, 1) && atime < today+ days(1)") , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("atime > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("ad    > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("type == Directory")                                      , 0,   0 );
   testFScan(ut, A_CHAR("type == Directory")                                      ,nullptr , 3,  36 );
   testFScan(ut, A_CHAR("type != Directory")                                      ,nullptr , 0,  10 );
   testFScan(ut, nullptr, A_CHAR("type == Regular")                                        , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("type == Socket")                                         , 0,   0 );
   testFScan(ut, A_CHAR("type != Directory"), A_CHAR("type == Regular")                    , 0,  10 );
   testFScan(ut, A_CHAR("type == Directory"), A_CHAR("type == Regular")                    , 3,  36 );

   testFScan(ut, nullptr, A_CHAR("owner == userID ")                                       , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("owner != userID ")                                       , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("group == groupID")                                       , 3,  36 );
   testFScan(ut, nullptr, A_CHAR("group != groupID")                                       , 0,   0 );

   testFScan(ut, nullptr, A_CHAR("EndsWith(Path, \"detail\")") , 1,  10 );
    //------------- end of FileExpression unit tests ----------------
    delete fex;

    UT_PRINT("...done") 

}

#include "unittests/aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_TIME

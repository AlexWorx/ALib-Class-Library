// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"

#if ALIB_UT_FILES
#include "ALib.EnumOps.H"
#include "ALib.Monomem.H"
#include "ALib.Strings.Calendar.H"
#include "ALib.Containers.StringTreeIterator.H"
#include "ALib.Exceptions.H"
#include "ALib.Format.H"
#include "ALib.Files.H"
#include "ALib.Files.Expressions.H"
#include "ALib.ALox.H"


#define TESTCLASSNAME       UT_Files

#include "aworx_unittests.hpp"
#include <numeric>

using namespace std;
using namespace alib;
using namespace alib::system;

namespace ut_aworx {

namespace {
Path scanPath;
ScanParameters sp(A_PATH(""), ScanParameters::SymbolicLinks::RESOLVE_BUT_DONT_FOLLOW );

SharedFTree ftree(10);
std::vector<ResultsPaths> resultPaths;
AString dumpBuf;
bool tstDoDump = false;
FileExpressions* fex= nullptr;
bool usePostRecursionDirFilter= false;

struct UTFTreeListener : files::FTreeListener {
    int cntDirs = 0;
    int cntFiles= 0;

    virtual void    Reset() { cntDirs= 0; cntFiles= 0; }
    virtual void    Notify( File& file, Event event ) override {
        if ( file->Quality() == FInfo::Qualities::MAX_DEPTH_REACHED )
            return;
        int addend=  event == Event::CreateNode
                     ? 1 : -1;
        if (file->IsDirectory()) cntDirs += addend;
        else                     cntFiles+= addend;
    }
};

struct UTFTreeListener2 : files::FTreeListener {
    int cntDirs = 0;
    int cntFiles= 0;

    virtual void    Reset() { cntDirs= 0; cntFiles= 0; }
    virtual void    Notify( File& file, Event event ) override {
        if ( file->Quality() == FInfo::Qualities::MAX_DEPTH_REACHED )
            return;
        int addend=  event == Event::CreateNode
                     ? 1 : -1;
        if (file->IsDirectory()) cntDirs += addend;
        else                     cntFiles+= addend;
    }
};
UTFTreeListener firstListener;
UTFTreeListener2 secondListener;

#include "ALib.Lang.CIFunctions.H"
#if !defined(_WIN32)
void testFScanListener( AWorxUnitTesting& ut,
                int listener1Dirs, int listener1Files,
                int listener2Dirs, int listener2Files )
{
    UT_PRINT( "Test scan & listen {!Q}",  sp.StartPath )

    // scan files
    resultPaths.clear();
    firstListener.Reset();
    secondListener.Reset();
    Path rootPath; rootPath << DIRECTORY_SEPARATOR;

    { ALIB_LOCK_WITH(ftree)
        ftree->MonitorPathPrefix(lang::ContainerOp::Insert, &firstListener,
                                 files::FTreeListener::Event::CreateNode, rootPath);  }


    ftree.DbgCriticalSections(lang::Switch::Off);
    files::ScanFiles( ftree, sp, resultPaths);


    // dump (before ut checking)
    #if ALIB_DEBUG
    if( tstDoDump )
    {
        int cnt= 0;
        for (auto& r : resultPaths)
        {
            UT_PRINT("Result {}/{}: {!Q} Q={}", ++cnt, resultPaths.size(), r.RealPath, r.Node->Quality() )
        }

        EnumBitSet<FInfo::Types> included;
        included.Set();
        dumpBuf.Reset();
        DbgDump(dumpBuf, *ftree, included );

        UT_PRINT(dumpBuf)
    }
    #else
    (void) tstDoDump;
    #endif

    // ut checks
    if( resultPaths.size())
    {
        UT_EQ( listener1Dirs  , firstListener.cntDirs -1   ) // we subtract one directory
        UT_EQ( listener1Files , firstListener.cntFiles    )

        UT_EQ( listener2Dirs  , secondListener.cntDirs  )
        UT_EQ( listener2Files , secondListener.cntFiles )

    }
}
#endif
void testFScan( AWorxUnitTesting& ut,
                const String& dirFilter, const String& fileFilter,
                int expDirs, int expFiles, bool resetTree= true)
{
    UT_PRINT( "Test scan {!Q}: DirFilter=<{}>({}), FileFilter=<{}>, RemoveEmptyDirectories={}",
              sp.StartPath,
              dirFilter, usePostRecursionDirFilter ? "PRE" : "POST",
              fileFilter,
              sp.RemoveEmptyDirectories )
    // scan files
    resultPaths.clear();
    if( resetTree )
        ftree.Reset();

    ftree.DbgCriticalSections(lang::Switch::Off);
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
        UT_TRUE(false)
    }


    // dump (before ut checking)
    #if ALIB_DEBUG
    if( tstDoDump )
    {
        int cnt= 0;
        for (auto& r : resultPaths)
        {
            UT_PRINT("Result {}/{}: {!Q} Q={}", ++cnt, resultPaths.size(), r.RealPath, r.Node->Quality() )
        }

        EnumBitSet<FInfo::Types> included;
        included.Set();
        dumpBuf.Reset();
        DbgDump(dumpBuf, *ftree, included );

        UT_PRINT(dumpBuf)
    }
    #else
    (void) tstDoDump;
    #endif

    // ut checks
    if( resultPaths.size())
    {
        FInfo&                v   = *resultPaths.front().Node;
        FInfo::DirectorySums& sums= v.Sums();
        if( expDirs  >= 0 )  UT_EQ( uint32_t(expDirs ), sums.CountDirectories()   )
        if( expFiles >= 0 )  UT_EQ( uint32_t(expFiles), sums.CountNonDirectories())
    }
}
#include "ALib.Lang.CIMethods.H"

struct CustomDataTrivial
{
    // int space[2048];
    int value = 0;
    static int cnt;
    CustomDataTrivial(int pValue=0)  {++cnt; value= pValue; }
    ~CustomDataTrivial() {--cnt;}
};
int CustomDataTrivial::cnt= 0;

struct CustomDataDynamic
{
    int *value;
    static int cnt;
    CustomDataDynamic(int pValue)  {++cnt; value= new int; *value= pValue; }
    ~CustomDataDynamic() {--cnt; delete value; }
};
int CustomDataDynamic::cnt= 0;

}// anonymous namespace



UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- class File
//--------------------------------------------------------------------------------------------------
UT_METHOD(FileAndFTree)
{
    UT_INIT()

    //------------------------  Check FTree Acquisition   ----------------------------
    #if !ALIB_SINGLE_THREADED
    {
        SharedFTree ft(16);
        ALIB_LOCK_WITH(ft)
        SharedFTree ft2;
        // ALIB_LOCK_WITH(ft2) // would crash, empty instance
    }
    #endif

    // scan some files
    sp.StartPath.Reset(ALIB_BASE_DIR) << DIRECTORY_SEPARATOR << "src"
		                              << DIRECTORY_SEPARATOR << "alib"
									  << DIRECTORY_SEPARATOR << "expressions";

    testFScan(ut, nullptr                 , nullptr                             , 3 , 40 );

    UT_EQ(1u, resultPaths.size())

    String256 fmt;
    String256 exp;

    // directory "expressions":
    File file= File(resultPaths.begin()->Node);
    UT_EQ( FInfo::Types::DIRECTORY, file->Type() )  UT_TRUE( file->IsDirectory() )
    UT_EQ( A_PATH("expressions")  , file.Name() )
    UT_EQ( A_PATH("expressions")  , file.Stem() )
    UT_EQ( A_PATH(""           )  , file.Extension() )
    file.Format( A_CHAR("na ns ne NA NS NE"), fmt, lang::CurrentData::Clear );
    UT_EQ( A_CHAR("expressions expressions  EXPRESSIONS EXPRESSIONS "), fmt)


    // file "expression.hpp":
    UT_TRUE( file.AsCursor().GoToChild(A_PATH("expression.inl")) )
    UT_EQ( FInfo::Types::REGULAR, file->Type() )  UT_TRUE( !file->IsDirectory() )
    UT_EQ( A_PATH("expression.inl") , file.Name() )
    UT_EQ( A_PATH("expression"    ) , file.Stem() )
    UT_EQ( A_PATH("inl"           ) , file.Extension() )
    file.Format( A_CHAR("na ns ne NA NS NE"), fmt, lang::CurrentData::Clear );
    UT_EQ( A_CHAR("expression.inl expression inl EXPRESSION.INL EXPRESSION INL"), fmt)

    file.Format( A_CHAR( "a"), fmt, lang::CurrentData::Clear );   
    #if !defined(_WIN32)
        UT_EQ(A_CHAR( "rw-r--r--"), fmt)
    #endif
    file.Format( A_CHAR("ta"), fmt, lang::CurrentData::Clear );   
    #if !defined(_WIN32)
        UT_EQ(A_CHAR("-rw-r--r--"), fmt)
    #endif

    // use default formatter
    {ALIB_LOCK_RECURSIVE_WITH(format::Formatter::DefaultLock)
        String256 target;
        format::Formatter::Default->Format(target, "{:ta h on gn s dm nal}", file);
        UT_TRUE(target.EndsWith(A_CHAR("expression.inl")))
        String256 target2;
        format::Formatter::Default->Format(target2, "{}", file);
        UT_EQ(target, target2)
    }

    ftree->GetNumberFormat().DecMinimumFieldWidth    = 8;
    ftree->GetNumberFormat().IntegralPartMinimumWidth= 9;
    ftree->GetNumberFormat().FractionalPartWidth     = 3;
    ftree->GetNumberFormat().Flags|= NumberFormatFlags::ReplaceLeadingZerosWithSpaces;
    ftree->GetNumberFormat().Flags|= NumberFormatFlags::WriteGroupChars;

    auto cdc= lang::CurrentData::Clear;
    file.Format(A_CHAR("'Size: 's"                             ),fmt,cdc);   UT_EQ(A_CHAR("Size:        13.678KiB"       ), fmt )
    file.Format(A_CHAR("'Size: 's(KiB)"                        ),fmt,cdc);   UT_EQ(A_CHAR("Size:        13.678"          ), fmt )
    file.Format(A_CHAR("'Size: 's(B)"                          ),fmt,cdc);   UT_EQ(A_CHAR("Size:   14,006"               ), fmt )
    file.Format(A_CHAR("'Size: 's(B){15,c}"                    ),fmt,cdc);   UT_EQ(A_CHAR("Size:      14,006    "        ), fmt )
    file.Format(A_CHAR("'Size: 's(iec)"                        ),fmt,cdc);   UT_EQ(A_CHAR("Size:        13.678KiB"       ), fmt)
    file.Format(A_CHAR("'Size: 's(SI)"                         ),fmt,cdc);   UT_EQ(A_CHAR("Size:        14.006kB"        ), fmt)
    file.Format(A_CHAR("'Size: 's(mb)"                         ),fmt,cdc);   UT_EQ(A_CHAR("Size:         0.014"          ), fmt)
    file.Format(A_CHAR("'Size: 's(mib)"                        ),fmt,cdc);   UT_EQ(A_CHAR("Size:         0.013"          ), fmt)

    file.Format(A_CHAR("'Stem: 'ns"                            ),fmt,cdc);   UT_EQ(A_CHAR("Stem: expression"             ), fmt )
    file.Format(A_CHAR("'Name: 'na"                            ),fmt,cdc);   UT_EQ(A_CHAR("Name: expression.inl"         ), fmt )
    file.Format(A_CHAR("'Ext:  'ne"                            ),fmt,cdc);   UT_EQ(A_CHAR("Ext:  inl"                    ), fmt )
    #if !defined(_WIN32)
        file.Format(A_CHAR("'Path: 'np"                        ),fmt,cdc);   UT_TRUE(fmt.EndsWith(A_CHAR("src/alib/expressions")))
    #else
        file.Format(A_CHAR("'Path: 'np"                        ),fmt,cdc);   UT_TRUE(fmt.EndsWith(A_CHAR("src\\alib\\expressions")))
    #endif
    
    file.Format(A_CHAR("'Path: 'np"                            ),fmt,cdc);
    #if !defined(_WIN32)
        UT_TRUE(fmt.EndsWith(A_CHAR("src/alib/expressions")))
    #else
        UT_TRUE(fmt.EndsWith(A_CHAR("src\\alib\\expressions")))
    #endif
    file.Format(A_CHAR("'Path: 'np"                            ),fmt,cdc);
    #if !defined(_WIN32)
        UT_TRUE(fmt.EndsWith(A_CHAR("src/alib/expressions")))
    #else
        UT_TRUE(fmt.EndsWith(A_CHAR("src\\alib\\expressions")))
    #endif
    file.Format(A_CHAR("'Type: 't"                             ),fmt,cdc);   UT_EQ(A_CHAR("Type: -"                            ), fmt )
    file.Format(A_CHAR("'Type: 'tt"                            ),fmt,cdc);   UT_EQ(A_CHAR("Type: rf"                           ), fmt )
    file.Format(A_CHAR("'Type: 'ttt"                           ),fmt,cdc);   UT_EQ(A_CHAR("Type: reg"                          ), fmt )
    file.Format(A_CHAR("'Type: 'tttt"                          ),fmt,cdc);   UT_EQ(A_CHAR("Type: Regular"                      ), fmt )
    file.Format(A_CHAR("'Type: 'T"                             ),fmt,cdc);   UT_EQ(A_CHAR("Type: -"                            ), fmt )
    file.Format(A_CHAR("'Type: 'TT"                            ),fmt,cdc);   UT_EQ(A_CHAR("Type: RF"                           ), fmt )
    file.Format(A_CHAR("'Type: 'TTT"                           ),fmt,cdc);   UT_EQ(A_CHAR("Type: REG"                          ), fmt )
    file.Format(A_CHAR("'Type: 'TTTT"                          ),fmt,cdc);   UT_EQ(A_CHAR("Type: REGULAR"                      ), fmt )
    file.Format(A_CHAR("'Type: 'TTtt"                          ),fmt,cdc);   UT_EQ(A_CHAR("Type: RFrf"                         ), fmt )
    file.Format(A_CHAR("'MDate: 'dm{yyyy MMM dd - HH:mm}"      ),fmt,cdc);
       exp.Reset("MDate: "); CalendarDateTime(file->MDate()).Format(A_CHAR("yyyy MMM dd - HH:mm"), exp); UT_EQ(exp, fmt )
    file.Format(A_CHAR("'BDate: 'db{yyyy MMM dd - HH:mm}"      ),fmt,cdc);
       exp.Reset("BDate: "); CalendarDateTime(file->BDate()).Format(A_CHAR("yyyy MMM dd - HH:mm"), exp); UT_EQ(exp, fmt )
    file.Format(A_CHAR("'CDate: 'dc{yyyy MMM dd - HH:mm}"      ),fmt,cdc);
       exp.Reset("CDate: "); CalendarDateTime(file->CDate()).Format(A_CHAR("yyyy MMM dd - HH:mm"), exp); UT_EQ(exp, fmt )
    file.Format(A_CHAR("'ADate: 'da{yyyy MMM dd - HH:mm}"      ),fmt,cdc);
       exp.Reset("ADate: "); CalendarDateTime(file->ADate()).Format(A_CHAR("yyyy MMM dd - HH:mm"), exp); UT_EQ(exp, fmt )
    file.Format(A_CHAR("'ADate: 'da{yyyy MMM dd - HH:mm}{30,c}"),fmt,cdc);
       exp.Reset("ADate:      "); CalendarDateTime(file->ADate()).Format(A_CHAR("yyyy MMM dd - HH:mm"), exp); exp << "      "; UT_EQ(exp, fmt )


    // too platform-specific to test. On problems, test visually.
    file.Format(A_CHAR("'Owner: '>on<"                         ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>on{15}<"                     ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>on{15,c}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>on{15,l}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gn<"                         ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gn{15}<"                     ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gn{15,c}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gn{15,l}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>oi}<"                        ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>oi{15}<"                     ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>oi{15,c}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Owner: '>oi{15,l}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gi<"                         ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gi{15}<"                     ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gi{15,c}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Group: '>gi{15,l}<"                   ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'#hard links: 'l"                      ),fmt,cdc);   UT_PRINT( fmt )
    file.Format(A_CHAR("'Quality: 'q"                          ),fmt,cdc);   UT_EQ(A_CHAR("Quality: STATS"),fmt )
    file.Format(A_CHAR("'Quality: 'qqq"                        ),fmt,cdc);   UT_EQ(A_CHAR("Quality: STA"  ),fmt )
    file.Format(A_CHAR("'ls -l format: 'ta h on gn s dm nal"  ),fmt,cdc);   UT_PRINT( fmt )

    // Create some special file entries in the tree (not existing on disk) to test basic functions.
    auto file2= file;
    file2.AsCursor().GoToRoot().CreateChild(A_PATH("test"));
    UT_TRUE( file2.AsCursor().GoToChild(A_PATH("test")) )
    file2.AsCursor().CreateChild(A_PATH(".profile"));
    UT_TRUE( file2.AsCursor().GoToChild(A_PATH(".profile") ))
    UT_EQ( A_PATH(".profile") , file2.Name() ) UT_EQ( A_PATH(".profile") , file2.Stem() )   UT_EQ( A_PATH("")  , file2.Extension() )
    file2.Format( A_CHAR("np na ns ne NP NA NS NE"),fmt,cdc );
    {
        String64 expp("/test .profile .profile  /TEST .PROFILE .PROFILE ");
        expp.SearchAndReplace('/', DIRECTORY_SEPARATOR);
        UT_EQ( expp, fmt)
    }

    file2.AsCursor().GoToParent().CreateChild(A_PATH("stem."));
    UT_TRUE( file2.AsCursor().GoToChild(A_PATH("stem.")) )
    UT_EQ( A_PATH("stem.") , file2.Name() ) UT_EQ( A_PATH("stem")        , file2.Stem() )   UT_EQ( A_PATH("")  , file2.Extension() )
    file2.Format( A_CHAR("np na ns ne NP NA NS NE"),fmt,cdc );
    {
        String64 expp("/test stem. stem  /TEST STEM. STEM ");
        expp.SearchAndReplace('/', DIRECTORY_SEPARATOR);
        UT_EQ( expp, fmt)
    }

    file2.AsCursor().GoToParent().CreateChild(A_PATH("stem.ext"));
    UT_TRUE( file2.AsCursor().GoToChild(A_PATH("stem.ext")) )
    UT_EQ( A_PATH("stem.ext") , file2.Name() ) UT_EQ( A_PATH("stem")     , file2.Stem() )   UT_EQ( A_PATH("ext"), file2.Extension() )
    file2.Format( A_CHAR("np na ns ne NP NA NS NE"),fmt,cdc );
    {
        String64 expp("/test stem.ext stem ext /TEST STEM.EXT STEM EXT");
        expp.SearchAndReplace('/', DIRECTORY_SEPARATOR);
        UT_EQ( expp, fmt)
    }
    Path path;
    file2.AsCursor().AssemblePath(path);
    {
        Path expp("/test/stem.ext");
        expp.SearchAndReplace('/', DIRECTORY_SEPARATOR);
        UT_EQ( expp, path)
    }

    // ------------------- Custom data -------------------
    UT_TRUE(file .AttachCustomData<CustomDataTrivial>().cnt==1)
    UT_TRUE(file2.AttachCustomData<CustomDataTrivial>().cnt==2)
    // file.CustomData<String>();                       // -> run-time error

    //ftree.SetCustomDataType<CustomDataDynamic>();     // -> run-time error
    //ftree.DeleteAllCustomData<CustomDataDynamic>();   // -> run-time error wrong type
    ftree->DeleteAllCustomData<CustomDataTrivial>();

    //UT_TRUE(file .CustomData<CustomDataTrivial>().cnt==1) // -> run-time error wrong type
    auto& custom1= file.AttachCustomData<CustomDataDynamic>(10);
    UT_TRUE( custom1.cnt   == 1  )
    UT_TRUE(*custom1.value == 10 )

    //UT_TRUE(file2.CustomData<CustomDataTrivial>().cnt==2) // -> run-time error wrong type
    auto& custom2= file2.AttachCustomData<CustomDataDynamic>(11);
    UT_TRUE( custom2.cnt   == 2  )
    UT_TRUE(*custom2.value == 11 )

    // remove both values and add again. Their custom objects should appear in reverse order
    // due to pool allocation
    file .DeleteCustomData<CustomDataDynamic>();
    file2.DeleteCustomData<CustomDataDynamic>();
    auto& result21= file .AttachCustomData<CustomDataDynamic>(20);
    auto& result22= file2.AttachCustomData<CustomDataDynamic>(21);
    UT_EQ(&custom2, &result21)
    UT_EQ(&custom1, &result22)
    ftree->DeleteAllCustomData<CustomDataDynamic>();   // If removed, two mem leaks occur with the dynamic objects

    // Now we do the following
    // - scan a larger tree (ALIB_HOME_DIR)
    // - equip all files with a custom object
    // - measure pool allocator
    // - delete all objects
    // - repeat it
    // - compare pool allocator statistics
    {
        // scan all ALib
        sp.StartPath.Reset(ALIB_BASE_DIR);

        int     qtyFiles= 0;

        for (int pass = 0; pass < 2; ++pass)
        {
            UT_PRINT( "------ Pass {} ----- ",  pass )
            // we "manually" reset the tree by deleting all files. If we would reset, then
            // simply the mono-allocator was reset and this test would succeed, even if
            // recycling/poolallocation failed.
            ftree->DeleteAllCustomData<CustomDataTrivial>();
            ftree->Root().AsCursor().DeleteChildren();
            testFScan(ut, nullptr , nullptr   , -1,-1 , false ); // false= no reset

            StringTreeIterator<FTree> stit;
            stit.SetPathGeneration(lang::Switch::Off);

            // loop over all nodes and dump
            int cntFiles= 0;
            stit.Initialize( ftree->Root().AsCursor(), lang::Inclusion::Exclude );
            while( stit.IsValid())
            {
                File f( stit.Node() );
                f.AttachCustomData<CustomDataTrivial>();
                stit.Next();
                ++cntFiles;
            }

            monomem::Statistics        stats;
            ftree.GetAllocator().GetStatistics(stats);

            UT_PRINT( "AllocSize : ",  stats.AllocSize  )
            UT_PRINT( "HeapSize  : ",  stats.HeapSize   )
            UT_PRINT( "QtyBuffers: ",  stats.QtyBuffers )

            if( pass == 0 )
            {
                UT_PRINT( "Number of files scanned: ", cntFiles )
                qtyFiles     = cntFiles;
                #if ALIB_DEBUG_MEMORY
                    ftree.GetAllocator().GetStatistics(stats);
                #endif
            }
            else
            {
                UT_PRINT( "Number of files scanned: ", cntFiles )
                UT_EQ( qtyFiles     , cntFiles)
                #if ALIB_DEBUG_MEMORY
                    monomem::Statistics        statsNow;
                    ftree.GetAllocator().GetStatistics(statsNow);
                    UT_EQ( stats.AllocSize     ,  statsNow.AllocSize  )
                    UT_EQ( stats.HeapSize      ,  statsNow.HeapSize   )
                    UT_EQ( stats.QtyBuffers    ,  statsNow.QtyBuffers )
                #endif
            }
        } // for (int pass...

        ftree->DeleteAllCustomData<CustomDataTrivial>();
    }

    // test owner name and artificial fs
    #if ALIB_FILES_SCANNER_IMPL == ALIB_FILES_SCANNER_POSIX
    {
        sp.StartPath.Reset() << A_PATH("/proc");
        resultPaths.clear();
        files::ScanFiles( ftree, sp, resultPaths);
        if (resultPaths.size() > 0 && resultPaths.front().RealPath.Equals(A_PATH("/proc"))) {
            UT_EQ(A_PATH(""), file.AsCursor().GoTo(A_PATH("/proc")) )
            UT_TRUE( file->IsArtificialFS() )
            UT_EQ( "root", file.GetOwnerName() )
        }
    }
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- Scanning
//--------------------------------------------------------------------------------------------------
UT_METHOD(Scanning)
{
    UT_INIT()

    UT_PRINT("") UT_PRINT( "### Files::Basics ###") 
    fex= new FileExpressions();
    //------------- Test basic filters with scanner functions ----------------
    // tests in "/src/alib/expressions"
    sp.StartPath.Reset(ALIB_BASE_DIR) << DIRECTORY_SEPARATOR << "src"
		                              << DIRECTORY_SEPARATOR << "alib"
									  << DIRECTORY_SEPARATOR << "expressions";

   //==================================== Listener tests ===============================
//tstDoDump= true;
// we can not rely on the path prefix in WinOS, due to drive names, etc.
// This test would fail on many setups
#if !defined(_WIN32)
   Path baseDir; baseDir << ALIB_BASE_DIR;
   baseDir.MakeReal();

   
   { ALIB_LOCK_WITH(ftree) ftree.Reset(); ftree->MonitorFilesByName( lang::ContainerOp::Insert,
       &secondListener, files::FTreeListener::Event::CreateNode, A_PATH("expression.inl")); }
   testFScanListener(ut, 3, 40, 0, 1 );

   { ALIB_LOCK_WITH(ftree) ftree.Reset(); ftree->MonitorPathPrefix( lang::ContainerOp::Insert,
       &secondListener, files::FTreeListener::Event::CreateNode, baseDir ) ; }
   testFScanListener(ut, 3, 40, 4, 40 );

   { ALIB_LOCK_WITH(ftree) ftree.Reset(); ftree->MonitorPathSubstring( lang::ContainerOp::Insert,
       &secondListener, files::FTreeListener::Event::CreateNode, A_PATH("xpressio")); }
   testFScanListener(ut, 3, 40, 3, 40 );

   { ALIB_LOCK_WITH(ftree) ftree.Reset(); ftree->MonitorPathSubstring( lang::ContainerOp::Insert,
       &secondListener, files::FTreeListener::Event::CreateNode, A_PATH("detail")); }
   testFScanListener(ut, 3, 40, 0, 9 );
#endif

   //==================================== filter tests ===============================
//tstDoDump= true;
   testFScan(ut, nullptr                 , nullptr                             , 3 , 40 );
   testFScan(ut, nullptr                 , A_CHAR("IsDirectory")               , 3 ,  0 );
   testFScan(ut, nullptr                 , A_CHAR("name = \"expression.inl\"") , 3 ,  1 );
   testFScan(ut, nullptr                 , A_CHAR("name * \"*.inl\"")          , 3 , 20 );
   testFScan(ut, nullptr                 , A_CHAR("name * \"e*.inl\"")         , 3 ,  4 );
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );

   sp.RemoveEmptyDirectories= true;
   testFScan(ut, nullptr                 , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

   // use pre recursion dir filter
   usePostRecursionDirFilter= false;
   sp.RemoveEmptyDirectories= false;


   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 3 , 15 );
//   ftree.Reset(); { ALIB_LOCK_WITH(ftree) ftree->MonitorPathSubstring(lang::ContainerOp::Insert, &secondListener, files::FTreeListener::Event::CreateNode, "detail"); }
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 3 , 12 );//, false, 0,5);
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.inl\"")          , 3 ,  7 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 2 , 15 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 1 , 12 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.inl\"")          , 0 ,  7 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );
   testFScan(ut, nullptr                         , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

   // use post recursion dir filter
   usePostRecursionDirFilter= true;
   sp.RemoveEmptyDirectories= false;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 3 , 15 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 3 , 12 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.inl\"")          , 3 ,  7 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 3 ,  0 );
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 2 , 15 );
   testFScan(ut, A_CHAR("name==\"detail\"")      , A_CHAR("name * \"*.inl\"")          , 1 , 12 );
   testFScan(ut, A_CHAR("name==\"notexisting\"") , A_CHAR("name * \"*.inl\"")          , 0 ,  7 );
   testFScan(ut, A_CHAR("name!=\"detail\"")      , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );
   testFScan(ut, nullptr                         , A_CHAR("name == \"notexisting\"")   , 0 ,  0 );

    //------------- Test all basic expression functions ----------------
   usePostRecursionDirFilter= false;
   sp.RemoveEmptyDirectories= true;
   testFScan(ut, nullptr, A_CHAR("size > 40 * 1024")                             , 2,   3 );
   testFScan(ut, nullptr, A_CHAR("date > DateTime(2020 , 1, 1) &&  date < today + days(1)"), 3,  40 );
   testFScan(ut, nullptr, A_CHAR("date > today + days(1)")                                 , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("mdate > DateTime(2020, 1, 1) && mdate < today+ days(1)") , 3,  40 );
   testFScan(ut, nullptr, A_CHAR("mdate > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("md    > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("mdate > DateTime(2020, 1, 1) && mdate < today+ days(1)") , 3,  40 );
   testFScan(ut, nullptr, A_CHAR("mdate > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("md    > today + days(1)")                                , 0,   0 );
 //tstDoDump= true;
   testFScan(ut, nullptr, A_CHAR("adate > DateTime(2020, 1, 1) && adate < today+ days(1)") , 3,  40 );
   testFScan(ut, nullptr, A_CHAR("adate > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("ad    > today + days(1)")                                , 0,   0 );
   testFScan(ut, nullptr, A_CHAR("type == Directory")                                      , 0,   0 );
   testFScan(ut, A_CHAR("type == Directory")                                      ,nullptr , 3,  40 );
   testFScan(ut, A_CHAR("type != Directory")                                      ,nullptr , 0,  15 );
   testFScan(ut, nullptr, A_CHAR("type == Regular")                                        , 3,  40 );
   testFScan(ut, nullptr, A_CHAR("type == Socket")                                         , 0,   0 );
   testFScan(ut, A_CHAR("type != Directory"), A_CHAR("type == Regular")                    , 0,  15 );
   testFScan(ut, A_CHAR("type == Directory"), A_CHAR("type == Regular")                    , 3,  40 );

   #if ALIB_FILES_SCANNER_IMPL == ALIB_FILES_SCANNER_POSIX
     testFScan(ut, nullptr, A_CHAR("owner == userID ")                                     , 3,  40 );
     testFScan(ut, nullptr, A_CHAR("owner != userID ")                                     , 0,   0 );
     testFScan(ut, nullptr, A_CHAR("group == groupID")                                     , 3,  40 );
     testFScan(ut, nullptr, A_CHAR("group != groupID")                                     , 0,   0 );
   #endif

    testFScan(ut, nullptr, A_CHAR("EndsWith(Path, \"detail\")") , 1,  9 );

    //------------- end of FileExpression unit tests ----------------
    delete fex;
    UT_PRINT("...done")
}

#include "aworx_unittests_end.hpp"

} //namespace [ut_aworx]

#endif // ALIB_UT_TIME

// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2023 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#include "alib/lib/fs_commonenums/commonenumdefs_aliased.hpp"
#if ALIB_UT_MONOMEM

#include "alib/alox.hpp"

#if ALIB_CONFIGURATION && !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STRINGTREE)
    #include "alib/monomem/stringtree.hpp"
#endif
#if ALIB_SYSTEM && !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif
#if !defined(HPP_ALIB_TEXT_FORMATTER)
#   include "alib/text/formatter.hpp"
#endif

#include "alib/strings/util/spaces.hpp"

#define TESTCLASSNAME       CPP_ALib_Monomem_StringTree
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace aworx;

namespace ut_aworx {

namespace ut_stringtree { namespace {

//--------------------------------------------------------------------------------------------------
//--- StringTree_NodePtr
//--------------------------------------------------------------------------------------------------

using AStringST= StringTree<AString,StringTreeNamesStatic<character>>;

bool valueSorter(const AStringST::NodePtr& lhs, const AStringST::NodePtr& rhs)
{
    return lhs.Value().CompareTo<true, Case::Ignore>(rhs.Value()) < 0 ;
}


using MyTree= StringTree<const char*>;
MyTree::RecursiveIterator testIt;

void testIteration( AWorxUnitTesting& ut,
                    MyTree::NodePtr& nodePtr,
                    int qtyChilds,
                    unsigned int recursionDepth, int qtyChildsRecursive,
                    bool debugOutput   =false                        )
{
    String128 path;
    // test recursive walk
    if( debugOutput )
    {
        UT_PRINT( "testIteration() debug output: recursive walk for nodePtr: ",
                  nodePtr.AssemblePath(path)                                    )
    }

    testIt.SetPathGeneration( Switch::On );
    testIt.Initialize( nodePtr, recursionDepth );
    int cnt= 0;
    while ( testIt.IsValid() )
    {
        if( debugOutput )
            UT_PRINT( "{:02} Depth={} QtyChildren={} N={:<3} Value={:<8}  Path: {}" , cnt,
                      testIt.CurrentDepth(), testIt.Node().CountChildren(), testIt.Node().Name(),
                      testIt.Node().Value(), testIt.CurrentPath() )
        ++cnt;
        testIt.Next();
    }
    UT_EQ( qtyChildsRecursive, cnt)

    // check hash table size against counted nodes
    #if ALIB_DEBUG
        if( nodePtr.IsRoot() )
        {
            UT_EQ( cnt,  nodePtr.Tree().Size() )
        }
    #endif

    // test non-recursive walk
    if( debugOutput )
        UT_PRINT( "testIteration() debug output: range loop")
    cnt= 0;
    for( auto it=nodePtr.FirstChild(); it.IsValid() ; it.GoToNextSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), it.Value() )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)


    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk")
    cnt= 0;
    for( auto it=nodePtr.FirstChild(); it.IsValid() ; it.GoToNextSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), it.Value() )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)

    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk backward")
    cnt= 0;
    for( auto it=nodePtr.LastChild(); it.IsValid() ; it.GoToPreviousSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), it.Value() )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)
}

struct DynInt
{
    int* value;

//    DynInt()       { value= new int(0); }
    DynInt( int v) { value= new int(v); }
    ~DynInt()      { if(value) { delete value; value= nullptr; }     }

    DynInt( const DynInt&  copy) { value= new int(*copy.value); }
//    DynInt(       DynInt&& move) { value= move.value; move.value= nullptr; }

    DynInt& operator=( const DynInt&  copy) { *value= *copy.value; return *this; }
//    DynInt& operator=(       DynInt&& move) { *value= *move.value; delete move.value; move.value= nullptr; return *this; }
//
//    bool operator==(const int&    other ) { return *value ==  other; }
//    bool operator==(const DynInt& other ) { return *value == *other.value; }
};

} // anonymous namespace
} // namespace ut_stringtree

using namespace ut_stringtree;

UT_CLASS

UT_METHOD(StringTree)
{
    UT_INIT()
    MonoAllocator ma(4*1024);
    {
        aworx::StringTree<ut_stringtree::DynInt, StringTreeNamesDynamic<wchar>> tree( &ma, A_WCHAR( '/'), 0);

        auto ptr= tree.Root();                                       UT_EQ(  0,  *ptr.Value().value) UT_EQ(  0,  tree.RecyclablesCount() )
                                                                     UT_EQ(  0,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.CreateChild(A_WCHAR("C11"), 1);                          UT_EQ(  1,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.DeleteChildren();                                        UT_EQ(  0,  tree.Size() )       UT_EQ(  1,  tree.RecyclablesCount() )
        ptr.CreateChild(A_WCHAR("C11"), 1);                          UT_EQ(  1,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lib::ValueReference::Absolute);   UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lib::ValueReference::Absolute);   UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(1, lib::ValueReference::Relative);   UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(2, lib::ValueReference::Relative);   UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lib::ValueReference::Relative);   UT_EQ(  1,  tree.Size() )       UT_EQ(  3,  tree.RecyclablesCount() )

        auto result=
        ptr.CreatePathIfNotExistent( A_WCHAR("C12/C21"), 2);    UT_EQ(  2,  *result.first.Value().value) UT_EQ(  2,  result.second  )
                                                                UT_EQ(  3,  tree.Size() )       UT_EQ(  1,  tree.RecyclablesCount() )
        ptr= tree.Root();
        ptr.CreateChild(A_WCHAR("C14"), 4);                     UT_EQ(  4,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.DeleteChild(A_WCHAR("C12"));                        UT_EQ(  2,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.Clear();                                           UT_EQ(  0,  *ptr.Value().value)
                                                                UT_EQ(  0,  tree.Size() )       UT_EQ(  4,  tree.RecyclablesCount() )
        tree.Reset(99);                                         UT_EQ( 99,  *ptr.Value().value)
                                                                UT_EQ(  0,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
    }

    // StringTree shared
    ma.Reset();
    {
        using ST= aworx::StringTree<int, StringTreeNamesStatic<char>, Recycling::Shared>;
        ST::TSharedRecycler sharedRecycler;
        ST  tree1( &ma, sharedRecycler , '/' );
        ST  tree2( &ma, sharedRecycler , '/' );

                                                UT_EQ( 0, tree1.RecyclablesCount() )
                                                UT_EQ( 0, tree2.RecyclablesCount() )

        tree1.Root().CreateChild( "Child1", 1); UT_EQ( 0, tree1.RecyclablesCount() )
                                                UT_EQ( 0, tree2.RecyclablesCount() )

        tree2.Root().CreateChild( "Child1", 1); UT_EQ( 0, tree1.RecyclablesCount() )
                                                UT_EQ( 0, tree2.RecyclablesCount() )

        tree1.Root().DeleteChild( "Child1" );   UT_EQ( 1, tree1.RecyclablesCount() )
                                                UT_EQ( 1, tree2.RecyclablesCount() )

        tree2.Root().DeleteChild( "Child1" );   UT_EQ( 2, tree1.RecyclablesCount() )
                                                UT_EQ( 2, tree2.RecyclablesCount() )
    }

    // List no recycling
    ma.Reset();
    {
        aworx::StringTree<int, StringTreeNamesStatic<char>, Recycling::None> tree(&ma, '/');

        // not allowed:  UT_EQ( 0, tree.RecyclablesCount() )
        // not allowed:  tree.ReserveRecyclables(10);

        tree.Root().CreateChild( "Child1", 1);
        tree.Root().DeleteChild( "Child1" );
        tree.Root().CreateChild( "Child1", 1);
        tree.Root().DeleteChild( "Child1" );
    }

}

UT_METHOD(StringTree_NodePtr)
{
    UT_INIT()
    MonoAllocator ma(4*1024);

    cout << "\nNodePtr():" << endl;
    cout << "Build string tree without using nodePtr navigation" << endl;
    MyTree pm(nullptr, '/');
    pm.SetAllocatorPostConstruction(&ma);


        auto nodePtr=   pm.Root();       nodePtr.Value()= "root";
           UT_TRUE( nodePtr.GoToCreateChildIfNotExistent( A_CHAR("a")    ) ) nodePtr.Value()= "a--"    ;
           UT_TRUE( nodePtr.GoToCreateChildIfNotExistent( A_CHAR("A")    ) ) nodePtr.Value()= "aA-"    ;
           UT_TRUE( nodePtr.GoToCreateChildIfNotExistent( A_CHAR("1")    ) ) nodePtr.Value()= "aA1"    ;
    nodePtr.GoToParent(); UT_TRUE(  nodePtr.IsValid() ) UT_TRUE( nodePtr.GoToCreateChildIfNotExistent( A_CHAR("b")    ) ) nodePtr.Value()= "aAb"    ;
    nodePtr.GoToParent(); UT_TRUE(  nodePtr.IsValid() ) UT_TRUE( nodePtr.GoToCreateChildIfNotExistent( A_CHAR("c")    ) ) nodePtr.Value()= "aAc"    ;
    nodePtr= pm.Root(); nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B"   )  ); nodePtr.Value()= "aB-"  ;
    nodePtr= pm.Root(); nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/1" )  ); nodePtr.Value()= "aB1"  ;
    nodePtr= pm.Root(); nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/2" )  ); nodePtr.Value()= "aB2"  ;
    nodePtr= pm.Root(); nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/3" )  ); nodePtr.Value()= "aB3"  ;
                        nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C"   )  ); nodePtr.Value()= "aC-"  ;
                        nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/1" )  ); nodePtr.Value()= "aC1"  ;
                        nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/2" )  ); nodePtr.Value()= "aC2"  ;
                        nodePtr.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/3" )  ); nodePtr.Value()= "aC3"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b"     )  ).first.Value()= "b--"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/A"   )  ).first.Value()= "bA-"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/A/1" )  ).first.Value()= "bA1"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/A/2" )  ).first.Value()= "bA2"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/A/3" )  ).first.Value()= "bA3"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/B"   )  ).first.Value()= "bB-"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/B/1" )  ).first.Value()= "bB1"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/B/2" )  ).first.Value()= "bB2"  ;
                        nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/B/3" )  ).first.Value()= "bB3"  ;
             (nodePtr=  nodePtr    .CreatePathIfNotExistent ( A_CHAR("/b/C"   )  ).first).Value()= "bC-"  ;
                        auto nodePtr2 =  nodePtr.CreateChild       ( A_CHAR("1" )  ); nodePtr2.Value()="bC1"  ;
                             nodePtr2 =  nodePtr.CreateChild<false>( A_CHAR("2" )  ); nodePtr2.Value()="bC2"  ;
                             nodePtr2 =  nodePtr.CreateChild       ( A_CHAR("3" )  ); nodePtr2.Value()="bC3"  ;
                             nodePtr2 =  nodePtr.CreateChild       ( A_CHAR("3" )  ); UT_TRUE( nodePtr2.IsInvalid() )

    cout << "Check construction (sizes of some branches)" << endl;
              nodePtr= pm.Root();                                            UT_EQ( 0, nodePtr.Depth() ) testIteration( ut, nodePtr, 2, 100, 26, true );
    UT_TRUE( (nodePtr= pm.Root()).GoToChild(A_CHAR("a")  )              )    UT_EQ( 1, nodePtr.Depth() ) testIteration( ut, nodePtr, 3, 100, 12 );
              nodePtr=        nodePtr.Child(A_CHAR("B"));                    UT_EQ( 2, nodePtr.Depth() ) testIteration( ut, nodePtr, 3, 100,  3 );
    UT_TRUE( (nodePtr= pm.Root()).GoToTraversedPath (A_CHAR("a/B/3"))  .IsEmpty())    UT_EQ( 3, nodePtr.Depth() ) testIteration( ut, nodePtr, 0, 100,  0 );
    UT_FALSE((nodePtr= pm.Root()).GoToTraversedPath (A_CHAR("a/B/3/e")).IsEmpty())    UT_EQ( 3, nodePtr.Depth() ) testIteration( ut, nodePtr, 0, 100,  0 );
    UT_FALSE((nodePtr= pm.Root()).GoToTraversedPath (A_CHAR("ab/ce"))  .IsEmpty())    UT_EQ( 0, nodePtr.Depth() ) testIteration( ut, nodePtr, 2, 100, 26 );

    UT_PRINT( "NodePtr navigation" )
    AString path;
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("a"    ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root();                                                                        UT_EQ( A_CHAR(""     ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("a"    ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_FALSE( nodePtr.GoToTraversedPath(A_CHAR("XYZ")         ).IsEmpty()) UT_EQ( A_CHAR(""     ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("b")           ).IsEmpty()) UT_EQ( A_CHAR("b"    ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_FALSE( nodePtr.GoToCreateChildIfNotExistent(A_CHAR("a")) )          UT_EQ( A_CHAR("a"    ), nodePtr.AssemblePath(path) )

    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a/B/./1")     ).IsEmpty()) UT_EQ( A_CHAR("a/B/1"), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a/B/1/..")    ).IsEmpty()) UT_EQ( A_CHAR("a/B"  ), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a/B/1/../1")  ).IsEmpty()) UT_EQ( A_CHAR("a/B/1"), nodePtr.AssemblePath(path) )
    nodePtr= pm.Root(); UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("a/B/1")       ).IsEmpty()) UT_EQ( NString("aB1" ), nodePtr.Value() )
                        UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("../2")        ).IsEmpty()) UT_EQ( NString("aB2" ), nodePtr.Value() )
                        UT_FALSE( nodePtr.GoToTraversedPath(A_CHAR("b")           ).IsEmpty()) UT_EQ( NString("aB2" ), nodePtr.Value() )
                        UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("/b")          ).IsEmpty()) UT_EQ( NString("b--" ), nodePtr.Value() )
                        UT_TRUE ( nodePtr.GoToTraversedPath(A_CHAR("./C")         ).IsEmpty()) UT_EQ( NString("bC-" ), nodePtr.Value() )


    UT_PRINT( "Up" )
    UT_TRUE( (nodePtr= pm.Root()).GoToTraversedPath( A_CHAR("a/B/3") ).IsEmpty() )
                             UT_EQ( A_CHAR("3"),  nodePtr.Name() )
                             UT_FALSE( nodePtr.IsInvalid() )
                             UT_FALSE( nodePtr.IsRoot() )
    nodePtr.GoToParent();    UT_EQ( A_CHAR("B"),  nodePtr.Name() ) UT_FALSE( nodePtr.IsInvalid() ) UT_FALSE( nodePtr.IsRoot() )
    nodePtr.GoToParent();    UT_EQ( A_CHAR("a"),  nodePtr.Name() ) UT_FALSE( nodePtr.IsInvalid() ) UT_FALSE( nodePtr.IsRoot() )
    nodePtr.GoToParent();    UT_TRUE( nodePtr.Name().IsEmpty()   ) UT_FALSE( nodePtr.IsInvalid() ) UT_TRUE(  nodePtr.IsRoot() )
    nodePtr.GoToParent();    UT_TRUE( nodePtr.IsInvalid() )

    UT_PRINT( "Siblings" )
    UT_TRUE( (nodePtr= pm.Root()).GoToTraversedPath( A_CHAR("a/C") ).IsEmpty() )
                                                UT_EQ( A_CHAR("C"),  nodePtr.Name() )
    auto it= nodePtr.FirstChild();              UT_EQ( A_CHAR("1"),  it.Name() )
                                                UT_TRUE( it.NextSibling().IsValid() )
                                                UT_TRUE( it.PreviousSibling().IsInvalid() )
    UT_FALSE(it.GoToPreviousSibling())          UT_TRUE( it.IsInvalid() )
    it= nodePtr.LastChild();                    UT_EQ( A_CHAR("3"),  it.Name() )
                                                UT_TRUE( it.PreviousSibling().IsValid() )
                                                UT_TRUE( it.NextSibling().IsInvalid() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= nodePtr.FirstChild();                   UT_EQ( A_CHAR("1"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("2"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= nodePtr;                                UT_EQ( A_CHAR("C"),  nodePtr.Name() )
    UT_TRUE(it.GoToFirstChild())                UT_EQ( A_CHAR("1"),  it.Name() )
    it= nodePtr;                                UT_EQ( A_CHAR("C"),  nodePtr.Name() )
    UT_TRUE(it.GoToLastChild())                 UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )


    UT_PRINT( "GetPath" )
    path.SetNull();
    nodePtr= pm.Root(); nodePtr.GoToParent();        UT_TRUE ( nodePtr.IsInvalid() )
    path.Reset("");
    nodePtr= pm.Root();                                       UT_EQ   ( A_CHAR("")        , nodePtr.AssemblePath( path ))
    nodePtr= pm.Root(); nodePtr.GoToTraversedPath(A_CHAR("a")      );  UT_EQ   ( A_CHAR("a")       , nodePtr.AssemblePath( path ))
    nodePtr= pm.Root(); nodePtr.GoToTraversedPath(A_CHAR("a/b")    );  UT_EQ   ( A_CHAR("a")       , nodePtr.AssemblePath( path ))
    nodePtr= pm.Root(); nodePtr.GoToTraversedPath(A_CHAR("a/B")    );  UT_EQ   ( A_CHAR("a/B")     , nodePtr.AssemblePath( path ))
    nodePtr= pm.Root(); nodePtr.GoToTraversedPath(A_CHAR("a/B/1")  );  UT_EQ   ( A_CHAR("a/B/1")   , nodePtr.AssemblePath( path ))

    UT_PRINT( "Erase nodes" )
    nodePtr= pm.Root();                                     testIteration( ut, nodePtr,  2,  99,   26 );
    nodePtr= nodePtr.TraversePath( A_CHAR("a/B")  ).first;  UT_EQ( A_CHAR("B") , nodePtr.Name() )
                                                            testIteration( ut, nodePtr,  3,  99,   3, true );
    it= nodePtr.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                   UT_EQ( A_CHAR("2") , it.Name() )


                                                            UT_EQ( uinteger(3) , nodePtr.CountChildren() )
    nodePtr.DeleteChild( it );                              UT_EQ( uinteger(2) , nodePtr.CountChildren() )
    it= nodePtr.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                   UT_EQ( A_CHAR("3") , it.Name() )
    it.GoToNextSibling();                                   UT_TRUE( it.IsInvalid() )

    UT_TRUE(nodePtr.GoToTraversedPath( A_CHAR("/a/B/3") ).IsEmpty()) UT_EQ( A_CHAR("3") , nodePtr.Name() )
                                                            UT_EQ( uinteger(0) , nodePtr.CountChildren() )
                                                            UT_FALSE( nodePtr.IsRoot() )
    nodePtr.DeleteChildren();                               UT_EQ( A_CHAR("3") , nodePtr.Name() )
                                                            UT_EQ( uinteger(0) , nodePtr.CountChildren() )
                                                            UT_FALSE( nodePtr.IsRoot() )

    nodePtr.Delete();
                                                            UT_EQ( A_CHAR("B") , nodePtr.Name() )
                                                            UT_EQ( uinteger(1) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  1,   100,    1 );
                                                            UT_FALSE( nodePtr.IsRoot() )
    nodePtr.Delete();                                       UT_EQ( A_CHAR("a") , nodePtr.Name() )
                                                            UT_EQ( uinteger(2) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  2,   100,    8 );
                                                            UT_FALSE( nodePtr.IsRoot() )
    nodePtr.GoToParent(); nodePtr.DeleteChild(A_CHAR("a")); UT_TRUE( nodePtr.Name().IsEmpty() )
                                                            UT_EQ( uinteger(1) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  1,   100,    13 );
                                                            UT_TRUE ( nodePtr.IsRoot() )
    nodePtr.DeleteChildren();                               UT_TRUE( nodePtr.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  0,   100,    0 );
                                                            UT_TRUE ( nodePtr.IsRoot() )
    nodePtr.Delete();                                       UT_TRUE( nodePtr.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  0,   100,    0 );
                                                            UT_TRUE ( nodePtr.IsRoot() )
    nodePtr.DeleteChildren();                               UT_TRUE( nodePtr.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , nodePtr.CountChildren() )
                                                            testIteration( ut, nodePtr,  0,   100,    0 );
                                                            UT_TRUE ( nodePtr.IsRoot() )

    // test delete methods of iterator
    nodePtr= pm.Root();
    UT_EQ( 2, nodePtr.CreatePathIfNotExistent( A_CHAR("/a/1" )  ).second )
    UT_EQ( 1, nodePtr.CreatePathIfNotExistent( A_CHAR("/a/2" )  ).second )
    UT_EQ( 1, nodePtr.CreatePathIfNotExistent( A_CHAR("/a/3" )  ).second )
    UT_EQ( 1, nodePtr.CreatePathIfNotExistent( A_CHAR("/b"   )  ).second )
    UT_EQ( 1, nodePtr.CreatePathIfNotExistent( A_CHAR("/c"   )  ).second )
    nodePtr= pm.Root();
    it= nodePtr.FirstChild();                 UT_EQ( A_CHAR("a") , it.Name() )
                                              UT_EQ( uinteger(3) , it.CountChildren() )
    UT_TRUE( it.DeleteChild(A_CHAR("2")) )    UT_EQ( uinteger(2) , it.CountChildren() )
    UT_TRUE( it.GoToFirstChild()         )    UT_EQ( A_CHAR("1") , it.Name() )
    UT_TRUE( it.Delete()                 )    UT_EQ( A_CHAR("a") , it.Name() )
    UT_EQ(uinteger(1), it.DeleteChildren())   UT_EQ( uinteger(0) , it.CountChildren() )
    it.Delete();                              UT_TRUE( it.IsRoot() )
    UT_TRUE( it.GoToFirstChild()       )      UT_EQ( A_CHAR("b") , it.Name() )
    it.Delete();                              UT_TRUE( it.IsRoot() )
    UT_TRUE( it.GoToFirstChild()       )      UT_EQ( A_CHAR("c") , it.Name() )
    it.Delete();                              UT_TRUE( it.IsRoot() )
    UT_FALSE( it.GoToFirstChild()      )      UT_TRUE( it.IsInvalid() )
}

//--------------------------------------------------------------------------------------------------
//--- StringTree_RecursiveIterator
//--------------------------------------------------------------------------------------------------
template <typename TStartValue>
int doIterations( AWorxUnitTesting& ut,
                  aworx::StringTree<AString, StringTreeNamesStatic<character>>::RecursiveIterator& iterator,
                  TStartValue& startValue,
                  unsigned int recursionDepth  )
{
    String512 buf;

    int cnt= 0;
    iterator.Initialize( startValue, recursionDepth );
    if( !iterator.IsValid() )
    {
        UT_PRINT( "Invalid iterator after initialization (e.g. no children in given node). No iterations performed.")
        return 0;
    }

    UT_PRINT( "Iterator test. Iteration start path: {!Q}, depth: {}", iterator.Node().AssemblePath( buf), iterator.RequestedDepth() )
    for ( ; iterator.IsValid() ; iterator.Next() )
    {
        const String& path= iterator.CurrentPath();
        UT_TRUE(    ( iterator.CurrentDepth() == 0 && path.IsEmpty() )
                 || ( iterator.CurrentDepth() > 0 && path.EndsWith( iterator.Node().Parent().Name())
                      && (    ( iterator.CurrentDepth() == 1 && path.Length() == iterator.Node().Parent().Name().Length() )
                                                      || path[path.Length() - iterator.Node().Parent().Name().Length() - 1] == '/' )           )
               )
        UT_PRINT( buf,
                    "Depth: {:2}  "
                  "  Node: {!Q:>7} "
                  "  Value: {!Q:>11} "
                  "  Children: {:2} "
                  "  Path to node: {:<30} " ,
                   iterator.CurrentDepth()        ,
                   iterator.Node().Name()         ,
                   iterator.Node().Value()        ,
                   iterator.Node().CountChildren(),
                   path
                 )
        ++cnt;
    }

    return cnt;
}

UT_METHOD(StringTree_RecursiveIterator)
{
    UT_INIT()

    MonoAllocator ba(4*1024);
    AStringST tree(&ba, '/');
    auto nodePtr=  tree.Root();                      nodePtr.Value().Reset( "root" );

    nodePtr.CreatePathIfNotExistent( A_CHAR( ""               )).first.Value().Reset( "root"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer"          )).first.Value().Reset( "aDir"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first.Value().Reset( "inner"      );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/xinn1"    )).first.Value().Reset( "inn1"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/inn2"     )).first.Value().Reset( "inn2"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/Inn3"     )).first.Value().Reset( "xinn3"      );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/inn4"     )).first.Value().Reset( "inn4"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/inn5"     )).first.Value().Reset( "inn5"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first.Value().Reset( "Overwritten");
    nodePtr.CreatePathIfNotExistent( A_CHAR( "dir2"           )).first.Value().Reset( "dir2"       );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-a"   )).first.Value().Reset( "subd2-a"    );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-b"   )).first.Value().Reset( "subd2-b"    );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/abc" )).first.Value().Reset( "sort2"      );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/def" )).first.Value().Reset( "sort3"      );
    nodePtr.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/ght" )).first.Value().Reset( "sort1"      );

    AStringST::RecursiveIterator recursiveIt;
    recursiveIt.SetPathGeneration( Switch::On );

    // check nodePtr/iterator creation
    recursiveIt.Initialize( tree );
    UT_TRUE( recursiveIt.Node().Name() == tree.Root().FirstChild().Name()  )


    UT_PRINT(NewLine(), "--- non recursive ---" )
    UT_EQ( 2, doIterations( ut, recursiveIt, tree, 0 ) )

    UT_PRINT(NewLine(), "--- non recursive ---" )
    decltype(tree)::NodePtr start=   tree.Root();
    UT_TRUE( start.GoToTraversedPath(A_CHAR("outer/Inn3")).IsEmpty() )

    UT_EQ( 3, doIterations( ut, recursiveIt, start, 0 ) )



    UT_PRINT(NewLine(), "--- non recursive decending---" )
    recursiveIt.SetSorting( SortOrder::Descending, Case::Sensitive );
    UT_EQ( 3, doIterations( ut, recursiveIt, start, 0 ) )


    UT_PRINT(NewLine(), "--- ascending ---" )
    recursiveIt.SetSorting( SortOrder::Ascending, Case::Sensitive );
    UT_EQ( 13, doIterations( ut, recursiveIt, tree, 99 ) )

    UT_PRINT(NewLine(), "--- descending ---" )
    recursiveIt.SetSorting( SortOrder::Descending, Case::Sensitive );
    UT_EQ( 13, doIterations( ut, recursiveIt, tree, ( std::numeric_limits<int>::max )() ) )

    UT_PRINT(NewLine(), "--- value ---" )
    recursiveIt.SetSorting( valueSorter );
    UT_EQ( 13, doIterations( ut, recursiveIt, tree, ( std::numeric_limits<int>::max )() ) )

    UT_PRINT(NewLine(), "--- value ---" )
    recursiveIt.SetSorting( valueSorter );
    start=   tree.Root();
    UT_TRUE( start.GoToTraversedPath(A_CHAR("outer/Inn3")).IsEmpty() )
    UT_EQ( 3, doIterations( ut, recursiveIt, start, ( std::numeric_limits<int>::max )() ) )

    UT_EQ( 3, doIterations( ut, recursiveIt, start, 1 ) )


    UT_PRINT(NewLine(), "--- value ---" )
    UT_TRUE( start.GoToTraversedPath(A_CHAR("abc")).IsEmpty() )
    UT_EQ( 0, doIterations( ut, recursiveIt, start, ( std::numeric_limits<int>::max )() ) )

    UT_EQ( 0, doIterations( ut, recursiveIt, start, 4 ) )

    //---------- test skipping ---------------------
    UT_PRINT(NewLine(), "------- Test skipping ---" )
    recursiveIt.SetSorting( Switch::Off );

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("dir2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("dir2"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("subd2-a") , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("subd2-b") , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("inner"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_EQ( A_CHAR("dir2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("inner"  ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("xinn1"  ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("inn2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_EQ( A_CHAR("dir2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("inner"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("xinn1"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("inn2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("Inn3"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("abc"    ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("def"    ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_EQ( A_CHAR("inn4"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("inn5"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("dir2"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextParentSibling();   UT_TRUE( !recursiveIt.IsValid() )


    // test iterator with no children
    nodePtr= tree.Root();
    nodePtr.GoToTraversedPath( A_CHAR("dir2/subd2-a"));UT_EQ( uinteger(0), nodePtr.CountChildren() )
    recursiveIt.Initialize( nodePtr );    UT_TRUE( !recursiveIt.IsValid() )

    // test copying constructor and copying
    recursiveIt.Initialize( tree, 1 );                             UT_EQ( A_CHAR("outer"), recursiveIt.Node().Name() )
    AStringST::RecursiveIterator recursiveIt2(recursiveIt);        UT_EQ( A_CHAR("outer"), recursiveIt2.Node().Name() )
    recursiveIt.Next();                                            UT_EQ( A_CHAR("inner"), recursiveIt.Node().Name() )
                                                                   UT_EQ( A_CHAR("outer"), recursiveIt2.Node().Name() )
    recursiveIt2= recursiveIt;
    while( recursiveIt.IsValid() )
    {
        UT_TRUE( recursiveIt2.IsValid() )
        UT_EQ( recursiveIt.Node().Name()  , recursiveIt2.Node().Name() )
        UT_EQ( recursiveIt.Next(), recursiveIt2.Next() )
    }
    UT_TRUE( recursiveIt2.IsInvalid() )

    //---------- test sorting ---------------------
    UT_PRINT(NewLine(), "------- Test sorting ---" )
    recursiveIt.SetSorting( SortOrder::Ascending  );
    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("dir2"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("outer" ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.SetSorting( SortOrder::Descending  );
    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.SetSorting( SortOrder::Ascending, Case::Ignore  );
    recursiveIt.Next();                UT_EQ( A_CHAR("inn2"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("Inn3"  ) , recursiveIt.Node().Name() )
    recursiveIt.SetSorting( valueSorter  );
    recursiveIt.Next();                UT_EQ( A_CHAR("ght"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("abc"   ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                UT_EQ( A_CHAR("def"   ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("inn4"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("inn5"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("inner" ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("xinn1" ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("dir2"  ) , recursiveIt.Node().Name() )


    //---------- test deletion ---------------------
    UT_PRINT(NewLine(), "------- Test deletion ---" )
    recursiveIt.SetSorting( valueSorter  );
    nodePtr= tree.Root();
    nodePtr.GoToTraversedPath( A_CHAR("outer/Inn3") );
    recursiveIt.Initialize( nodePtr );                UT_EQ( A_CHAR("ght"      ) , recursiveIt.Node().Name() )
    recursiveIt.DeleteNode();                         UT_EQ( A_CHAR("abc"      ) , recursiveIt.Node().Name() )
                                                      UT_EQ( uinteger(2) , recursiveIt.Node().Parent().CountChildren() )
    recursiveIt.DeleteNode();                         UT_EQ( A_CHAR("def"      ) , recursiveIt.Node().Name() )
                                                      UT_EQ( uinteger(1) , recursiveIt.Node().Parent().CountChildren() )
    recursiveIt.DeleteNode();                         UT_TRUE( !recursiveIt.IsValid() )
    recursiveIt.SetSorting( Switch::Off  );
    recursiveIt.Initialize( tree );                   UT_EQ( A_CHAR("outer"    ) , recursiveIt.Node().Name() )
                                                      UT_EQ( uinteger(6) , recursiveIt.Node().CountChildren() )
    recursiveIt.Node().DeleteChild( A_CHAR("xinn1")); UT_EQ( uinteger( 5), recursiveIt.Node().CountChildren() )
    recursiveIt.Node().DeleteChild( A_CHAR("Inn3" )); UT_EQ( uinteger( 4), recursiveIt.Node().CountChildren() )
    recursiveIt.Next();                               UT_EQ( A_CHAR("inner"    ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                               UT_EQ( A_CHAR("inn2"     ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                               UT_EQ( A_CHAR("inn4"     ) , recursiveIt.Node().Name() )
    recursiveIt.Next();                               UT_EQ( A_CHAR("inn5"     ) , recursiveIt.Node().Name() )

    recursiveIt.Initialize( tree );                   UT_EQ( A_CHAR("outer"    ) , recursiveIt.Node().Name() )
    recursiveIt.DeleteNode();                         UT_EQ( A_CHAR("dir2"     ) , recursiveIt.Node().Name() )
    recursiveIt.DeleteNode();                         UT_TRUE( !recursiveIt.IsValid() )
    recursiveIt.Initialize( tree );                   UT_TRUE( !recursiveIt.IsValid() )
}

#include "unittests/aworx_unittests_end.hpp"

} // namespace ut_aworx


#endif // ALIB_UT_MONOMEM




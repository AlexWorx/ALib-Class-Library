// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_MONOMEM

#include "alib/alox.hpp"

#if ALIB_CONFIGURATION && !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STRINGTREE)
    #include "alib/monomem/stringtree.hpp"
#endif
#if ALIB_CAMP && !defined (HPP_ALIB_CAMP_DIRECTORY)
    #include "alib/lang/system/directory.hpp"
#endif
#if !defined(HPP_ALIB_LANG_FORMAT_FORMATTER)
#   include "alib/lang/format/formatter.hpp"
#endif

#include "alib/strings/util/spaces.hpp"

#define TESTCLASSNAME       CPP_ALib_Monomem_StringTree
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace alib;

namespace ut_aworx {

namespace ut_stringtree { namespace {

//--------------------------------------------------------------------------------------------------
//--- StringTree_Cursor
//--------------------------------------------------------------------------------------------------

using AStringST= StringTree<NAString,StringTreeNamesStatic<character>>;

bool valueSorter(const AStringST::Cursor& lhs, const AStringST::Cursor& rhs)
{
    return lhs.Value().CompareTo<true, lang::Case::Ignore>(rhs.Value()) < 0 ;
}


using MyTree= StringTree<const char*>;
MyTree::RecursiveIterator testIt;

void testIteration( AWorxUnitTesting& ut,
                    MyTree::Cursor& cursor,
                    int qtyChilds,
                    unsigned int recursionDepth, int qtyChildsRecursive,
                    bool debugOutput   =false                        )
{
    String128 path;
    // test recursive walk
    if( debugOutput )
    {
        UT_PRINT( "testIteration() debug output: recursive walk for cursor: ",
                  cursor.AssemblePath(path)                                    )
    }

    testIt.SetPathGeneration( lang::Switch::On );
    testIt.Initialize( cursor, recursionDepth );
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
        if( cursor.IsRoot() )
        {
            UT_EQ( cnt,  cursor.Tree().Size() )
        }
    #endif

    // test non-recursive walk
    if( debugOutput )
        UT_PRINT( "testIteration() debug output: range loop")
    cnt= 0;
    for( auto it=cursor.FirstChild(); it.IsValid() ; it.GoToNextSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), it.Value() )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)


    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk")
    cnt= 0;
    for( auto it=cursor.FirstChild(); it.IsValid() ; it.GoToNextSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), it.Value() )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)

    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk backward")
    cnt= 0;
    for( auto it=cursor.LastChild(); it.IsValid() ; it.GoToPreviousSibling() )
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
        alib::StringTree<ut_stringtree::DynInt, StringTreeNamesDynamic<wchar>> tree( &ma, A_WCHAR( '/'));

        auto ptr= tree.Root();                                                                       UT_EQ(  0,  tree.RecyclablesCount() )
                                                                     UT_EQ(  0,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.CreateChild(A_WCHAR("C11"), 1);                          UT_EQ(  1,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.DeleteChildren();                                        UT_EQ(  0,  tree.Size() )       UT_EQ(  1,  tree.RecyclablesCount() )
        ptr.CreateChild(A_WCHAR("C11"), 1);                          UT_EQ(  1,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lang::ValueReference::Absolute);  UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lang::ValueReference::Absolute);  UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(1, lang::ValueReference::Relative);  UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(2, lang::ValueReference::Relative);  UT_EQ(  1,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.ReserveRecyclables(3, lang::ValueReference::Relative);  UT_EQ(  1,  tree.Size() )       UT_EQ(  3,  tree.RecyclablesCount() )

        auto result=
        ptr.CreatePathIfNotExistent( A_WCHAR("C12/C21"), 2);    UT_EQ(  2,  *result.first.Value().value) UT_EQ(  2,  result.second  )
                                                                UT_EQ(  3,  tree.Size() )       UT_EQ(  1,  tree.RecyclablesCount() )
        ptr= tree.Root();
        ptr.CreateChild(A_WCHAR("C14"), 4);                     UT_EQ(  4,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
        ptr.DeleteChild(A_WCHAR("C12"));                        UT_EQ(  2,  tree.Size() )       UT_EQ(  2,  tree.RecyclablesCount() )
        tree.Clear();                                           UT_EQ(  0,  tree.Size() )       UT_EQ(  4,  tree.RecyclablesCount() )
        tree.Reset();                                           UT_EQ(  0,  tree.Size() )       UT_EQ(  0,  tree.RecyclablesCount() )
    }

    // StringTree shared
    ma.Reset();
    {
        using ST= alib::StringTree<int, StringTreeNamesStatic<char>, Recycling::Shared>;
        ST::TSharedRecycler sharedRecycler;
        ST  tree1( &ma, '/', sharedRecycler );
        ST  tree2( &ma, '/', sharedRecycler );

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
        alib::StringTree<int, StringTreeNamesStatic<char>, Recycling::None> tree(&ma, '/');

        // not allowed:  UT_EQ( 0, tree.RecyclablesCount() )
        // not allowed:  tree.ReserveRecyclables(10);

        tree.Root().CreateChild( "Child1", 1);
        tree.Root().DeleteChild( "Child1" );
        tree.Root().CreateChild( "Child1", 1);
        tree.Root().DeleteChild( "Child1" );
    }

}

UT_METHOD(StringTree_Cursor)
{
    UT_INIT()
    MonoAllocator ma(4*1024);

    cout << "\nCursor():" << endl;
    cout << "Build string tree without using cursor navigation" << endl;
    MyTree pm(nullptr, '/');
    pm.SetAllocatorPostConstruction(&ma);


        auto cursor=   pm.Root();
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("a")    ) ) cursor.Value()= "a--"    ;
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("A")    ) ) cursor.Value()= "aA-"    ;
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("1")    ) ) cursor.Value()= "aA1"    ;
    cursor.GoToParent(); UT_TRUE(  cursor.IsValid() ) UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("b")    ) ) cursor.Value()= "aAb"    ;
    cursor.GoToParent(); UT_TRUE(  cursor.IsValid() ) UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("c")    ) ) cursor.Value()= "aAc"    ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B"   )  ); cursor.Value()= "aB-"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/1" )  ); cursor.Value()= "aB1"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/2" )  ); cursor.Value()= "aB2"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent ( A_CHAR( "a/B/3" )  ); cursor.Value()= "aB3"  ;
                        cursor.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C"   )  ); cursor.Value()= "aC-"  ;
                        cursor.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/1" )  ); cursor.Value()= "aC1"  ;
                        cursor.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/2" )  ); cursor.Value()= "aC2"  ;
                        cursor.GoToCreatedPathIfNotExistent ( A_CHAR("/a/C/3" )  ); cursor.Value()= "aC3"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b"     )  ).first.Value()= "b--"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A"   )  ).first.Value()= "bA-"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/1" )  ).first.Value()= "bA1"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/2" )  ).first.Value()= "bA2"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/3" )  ).first.Value()= "bA3"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B"   )  ).first.Value()= "bB-"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/1" )  ).first.Value()= "bB1"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/2" )  ).first.Value()= "bB2"  ;
                        cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/3" )  ).first.Value()= "bB3"  ;
             (cursor=  cursor    .CreatePathIfNotExistent ( A_CHAR("/b/C"   )  ).first).Value()= "bC-"  ;
                        auto cursor2 =  cursor.CreateChild       ( A_CHAR("1" )  ); cursor2.Value()="bC1"  ;
                             cursor2 =  cursor.CreateChild<false>( A_CHAR("2" )  ); cursor2.Value()="bC2"  ;
                             cursor2 =  cursor.CreateChild       ( A_CHAR("3" )  ); cursor2.Value()="bC3"  ;
                             cursor2 =  cursor.CreateChild       ( A_CHAR("3" )  ); UT_TRUE( cursor2.IsInvalid() )

    cout << "Check construction (sizes of some branches)" << endl;
              cursor= pm.Root();                                            UT_EQ( 0, cursor.Depth() ) testIteration( ut, cursor, 2, 100, 26, true );
    UT_TRUE( (cursor= pm.Root()).GoToChild(A_CHAR("a")  )              )    UT_EQ( 1, cursor.Depth() ) testIteration( ut, cursor, 3, 100, 12 );
              cursor=        cursor.Child(A_CHAR("B"));                     UT_EQ( 2, cursor.Depth() ) testIteration( ut, cursor, 3, 100,  3 );
    UT_TRUE( (cursor= pm.Root()).GoToTraversedPath (A_CHAR("a/B/3"))  .IsEmpty())    UT_EQ( 3, cursor.Depth() ) testIteration( ut, cursor, 0, 100,  0 );
    UT_FALSE((cursor= pm.Root()).GoToTraversedPath (A_CHAR("a/B/3/e")).IsEmpty())    UT_EQ( 3, cursor.Depth() ) testIteration( ut, cursor, 0, 100,  0 );
    UT_FALSE((cursor= pm.Root()).GoToTraversedPath (A_CHAR("ab/ce"))  .IsEmpty())    UT_EQ( 0, cursor.Depth() ) testIteration( ut, cursor, 2, 100, 26 );

    UT_PRINT( "Cursor navigation" )
    AString path;
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )
    cursor= pm.Root();                                                                       UT_EQ( A_CHAR("/"     ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_FALSE( cursor.GoToTraversedPath(A_CHAR("XYZ")         ).IsEmpty()) UT_EQ( A_CHAR("/"     ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("b")           ).IsEmpty()) UT_EQ( A_CHAR("/b"    ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_FALSE( cursor.GoToCreateChildIfNotExistent(A_CHAR("a")) )          UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )

    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a/B/./1")     ).IsEmpty()) UT_EQ( A_CHAR("/a/B/1"), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a/B/1/..")    ).IsEmpty()) UT_EQ( A_CHAR("/a/B"  ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a/B/1/../1")  ).IsEmpty()) UT_EQ( A_CHAR("/a/B/1"), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("a/B/1")       ).IsEmpty()) UT_EQ( NString("aB1" ), cursor.Value() )
                        UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("../2")       ).IsEmpty()) UT_EQ( NString("aB2" ), cursor.Value() )
                        UT_FALSE( cursor.GoToTraversedPath(A_CHAR("b")          ).IsEmpty()) UT_EQ( NString("aB2" ), cursor.Value() )
                        UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("/b")         ).IsEmpty()) UT_EQ( NString("b--" ), cursor.Value() )
                        UT_TRUE ( cursor.GoToTraversedPath(A_CHAR("./C")        ).IsEmpty()) UT_EQ( NString("bC-" ), cursor.Value() )


    UT_PRINT( "Up" )
    UT_TRUE( (cursor= pm.Root()).GoToTraversedPath( A_CHAR("a/B/3") ).IsEmpty() )
                             UT_EQ( A_CHAR("3"),  cursor.Name() )
                             UT_FALSE( cursor.IsInvalid() )
                             UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_EQ( A_CHAR("B"),  cursor.Name() ) UT_FALSE( cursor.IsInvalid() ) UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_EQ( A_CHAR("a"),  cursor.Name() ) UT_FALSE( cursor.IsInvalid() ) UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_TRUE( cursor.Name().IsEmpty()   ) UT_FALSE( cursor.IsInvalid() ) UT_TRUE(  cursor.IsRoot() )
    cursor.GoToParent();    UT_TRUE( cursor.IsInvalid() )

    UT_PRINT( "Siblings" )
    UT_TRUE( (cursor= pm.Root()).GoToTraversedPath( A_CHAR("a/C") ).IsEmpty() )
                                                UT_EQ( A_CHAR("C"),  cursor.Name() )
    auto it= cursor.FirstChild();              UT_EQ( A_CHAR("1"),  it.Name() )
                                                UT_TRUE( it.NextSibling().IsValid() )
                                                UT_TRUE( it.PreviousSibling().IsInvalid() )
    UT_FALSE(it.GoToPreviousSibling())          UT_TRUE( it.IsInvalid() )
    it= cursor.LastChild();                    UT_EQ( A_CHAR("3"),  it.Name() )
                                                UT_TRUE( it.PreviousSibling().IsValid() )
                                                UT_TRUE( it.NextSibling().IsInvalid() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= cursor.FirstChild();                   UT_EQ( A_CHAR("1"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("2"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= cursor;                                UT_EQ( A_CHAR("C"),  cursor.Name() )
    UT_TRUE(it.GoToFirstChild())                UT_EQ( A_CHAR("1"),  it.Name() )
    it= cursor;                                UT_EQ( A_CHAR("C"),  cursor.Name() )
    UT_TRUE(it.GoToLastChild())                 UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )


    UT_PRINT( "GetPath" )
    path.SetNull();
    cursor= pm.Root(); cursor.GoToParent();        UT_TRUE ( cursor.IsInvalid() )
    path.Reset();
    cursor= pm.Root();                                               UT_EQ   ( A_CHAR("/")        , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoToTraversedPath(A_CHAR("a")      );  UT_EQ   ( A_CHAR("/a")       , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoToTraversedPath(A_CHAR("a/b")    );  UT_EQ   ( A_CHAR("/a")       , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoToTraversedPath(A_CHAR("a/B")    );  UT_EQ   ( A_CHAR("/a/B")     , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoToTraversedPath(A_CHAR("a/B/1")  );  UT_EQ   ( A_CHAR("/a/B/1")   , cursor.AssemblePath( path ))
                                                                     UT_EQ   ( A_CHAR("a/B/1")    , cursor.AssemblePath( path, pm.Root() ))
                                                                     UT_EQ   ( A_CHAR("1")        , cursor.AssemblePath( path, cursor.Parent() ) )
                                                                     UT_EQ   ( A_CHAR("B/1")        , cursor.AssemblePath( path, cursor.Parent().Parent() ) )

    UT_PRINT( "Erase nodes" )
    cursor= pm.Root();                                     testIteration( ut, cursor,  2,  99,   26 );
    cursor= cursor.TraversePath( A_CHAR("a/B")  ).first;  UT_EQ( A_CHAR("B") , cursor.Name() )
                                                           testIteration( ut, cursor,  3,  99,   3, true );
    it= cursor.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                  UT_EQ( A_CHAR("2") , it.Name() )


                                                           UT_EQ( uinteger(3) , cursor.CountChildren() )
    cursor.DeleteChild( it );                              UT_EQ( uinteger(2) , cursor.CountChildren() )
    it= cursor.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                  UT_EQ( A_CHAR("3") , it.Name() )
    it.GoToNextSibling();                                  UT_TRUE( it.IsInvalid() )

    UT_TRUE(cursor.GoToTraversedPath( A_CHAR("/a/B/3") ).IsEmpty()) UT_EQ( A_CHAR("3") , cursor.Name() )
                                                           UT_EQ( uinteger(0) , cursor.CountChildren() )
                                                           UT_FALSE( cursor.IsRoot() )
    cursor.DeleteChildren();                               UT_EQ( A_CHAR("3") , cursor.Name() )
                                                           UT_EQ( uinteger(0) , cursor.CountChildren() )
                                                           UT_FALSE( cursor.IsRoot() )

    cursor.Delete();
                                                           UT_EQ( A_CHAR("B") , cursor.Name() )
                                                           UT_EQ( uinteger(1) , cursor.CountChildren() )
                                                           testIteration( ut, cursor,  1,   100,    1 );
                                                           UT_FALSE( cursor.IsRoot() )
    cursor.Delete();                                       UT_EQ( A_CHAR("a") , cursor.Name() )
                                                           UT_EQ( uinteger(2) , cursor.CountChildren() )
                                                           testIteration( ut, cursor,  2,   100,    8 );
                                                           UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent(); cursor.DeleteChild(A_CHAR("a"));  UT_TRUE( cursor.Name().IsEmpty() )
                                                           UT_EQ( uinteger(1) , cursor.CountChildren() )
                                                           testIteration( ut, cursor,  1,   100,    13 );
                                                           UT_TRUE ( cursor.IsRoot() )
    cursor.DeleteChildren();                               UT_TRUE( cursor.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , cursor.CountChildren() )
                                                            testIteration( ut, cursor,  0,   100,    0 );
                                                            UT_TRUE ( cursor.IsRoot() )
    cursor.Delete();                                       UT_TRUE( cursor.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , cursor.CountChildren() )
                                                            testIteration( ut, cursor,  0,   100,    0 );
                                                            UT_TRUE ( cursor.IsRoot() )
    cursor.DeleteChildren();                               UT_TRUE( cursor.Name().IsEmpty() )
                                                            UT_EQ( uinteger(0) , cursor.CountChildren() )
                                                            testIteration( ut, cursor,  0,   100,    0 );
                                                            UT_TRUE ( cursor.IsRoot() )

    // test delete methods of iterator
    cursor= pm.Root();
    UT_EQ( 2, cursor.CreatePathIfNotExistent( A_CHAR("/a/1" )  ).second )
    UT_EQ( 1, cursor.CreatePathIfNotExistent( A_CHAR("/a/2" )  ).second )
    UT_EQ( 1, cursor.CreatePathIfNotExistent( A_CHAR("/a/3" )  ).second )
    UT_EQ( 1, cursor.CreatePathIfNotExistent( A_CHAR("/b"   )  ).second )
    UT_EQ( 1, cursor.CreatePathIfNotExistent( A_CHAR("/c"   )  ).second )
    cursor= pm.Root();
    it= cursor.FirstChild();                 UT_EQ( A_CHAR("a") , it.Name() )
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
                  alib::StringTree<NAString, StringTreeNamesStatic<character>>::RecursiveIterator& iterator,
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
    auto cursor=  tree.Root();

    cursor.CreatePathIfNotExistent( A_CHAR( ""               ));
    cursor.CreatePathIfNotExistent( A_CHAR( "outer"          )).first.Value().Reset( "aDir"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first.Value().Reset( "inner"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/xinn1"    )).first.Value().Reset( "inn1"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn2"     )).first.Value().Reset( "inn2"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3"     )).first.Value().Reset( "xinn3"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn4"     )).first.Value().Reset( "inn4"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn5"     )).first.Value().Reset( "inn5"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first.Value().Reset( "Overwritten");
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2"           )).first.Value().Reset( "dir2"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-a"   )).first.Value().Reset( "subd2-a"    );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-b"   )).first.Value().Reset( "subd2-b"    );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/abc" )).first.Value().Reset( "sort2"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/def" )).first.Value().Reset( "sort3"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/ght" )).first.Value().Reset( "sort1"      );

    AStringST::RecursiveIterator recursiveIt;
    recursiveIt.SetPathGeneration( lang::Switch::On );

    // check cursor/iterator creation
    recursiveIt.Initialize( tree );
    UT_TRUE( recursiveIt.Node().Name() == tree.Root().FirstChild().Name()  )


    UT_PRINT(NewLine(), "--- non recursive ---" )
    int qtyIt;
    qtyIt = doIterations(ut, recursiveIt, tree, 0);  UT_EQ(2, qtyIt)

    UT_PRINT(NewLine(), "--- non recursive ---" )
    decltype(tree)::Cursor start=   tree.Root();
    UT_TRUE( start.GoToTraversedPath(A_CHAR("outer/Inn3")).IsEmpty() )

    qtyIt = doIterations( ut, recursiveIt, start, 0 ); UT_EQ( 3, qtyIt)



    UT_PRINT(NewLine(), "--- non recursive decending---" )
    recursiveIt.SetSorting( lang::SortOrder::Descending, lang::Case::Sensitive );
    qtyIt = doIterations(ut, recursiveIt, start, 0); UT_EQ(3, qtyIt)


    UT_PRINT(NewLine(), "--- ascending ---" )
    recursiveIt.SetSorting( lang::SortOrder::Ascending, lang::Case::Sensitive );
    qtyIt = doIterations(ut, recursiveIt, tree, 99);  UT_EQ(13, qtyIt)

    UT_PRINT(NewLine(), "--- descending ---" )
    recursiveIt.SetSorting( lang::SortOrder::Descending, lang::Case::Sensitive );
    qtyIt = doIterations(ut, recursiveIt, tree, (std::numeric_limits<int>::max)());  UT_EQ(13, qtyIt)

    UT_PRINT(NewLine(), "--- value ---" )
    recursiveIt.SetSorting( valueSorter );
    qtyIt = doIterations(ut, recursiveIt, tree, (std::numeric_limits<int>::max)());  UT_EQ(13, qtyIt)

    UT_PRINT(NewLine(), "--- value ---" )
    recursiveIt.SetSorting( valueSorter );
    start=   tree.Root();
    UT_TRUE( start.GoToTraversedPath(A_CHAR("outer/Inn3")).IsEmpty() )
    qtyIt = doIterations( ut, recursiveIt, start, ( std::numeric_limits<int>::max )() ); UT_EQ( 3, qtyIt)

    qtyIt = doIterations(ut, recursiveIt, start, 1); UT_EQ( 3, qtyIt )


    UT_PRINT(NewLine(), "--- value ---" )
    UT_TRUE( start.GoToTraversedPath(A_CHAR("abc")).IsEmpty() )
    qtyIt = doIterations( ut, recursiveIt, start, ( std::numeric_limits<int>::max )() ); UT_EQ( 0, qtyIt)

    qtyIt = doIterations( ut, recursiveIt, start, 4 ); UT_EQ( 0, qtyIt)

    //---------- test skipping ---------------------
    UT_PRINT(NewLine(), "------- Test skipping ---" )
    recursiveIt.SetSorting( lang::Switch::Off );

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
    cursor= tree.Root();
    cursor.GoToTraversedPath( A_CHAR("dir2/subd2-a"));UT_EQ( uinteger(0), cursor.CountChildren() )
    recursiveIt.Initialize( cursor );    UT_TRUE( !recursiveIt.IsValid() )

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
    recursiveIt.SetSorting( lang::SortOrder::Ascending  );
    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("dir2"  ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_EQ( A_CHAR("outer" ) , recursiveIt.Node().Name() )
    recursiveIt.NextSibling();         UT_TRUE( !recursiveIt.IsValid() )

    recursiveIt.SetSorting( lang::SortOrder::Descending  );
    recursiveIt.Initialize( tree );    UT_EQ( A_CHAR("outer"  ) , recursiveIt.Node().Name() )
    recursiveIt.SetSorting( lang::SortOrder::Ascending, lang::Case::Ignore  );
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
    cursor= tree.Root();
    cursor.GoToTraversedPath( A_CHAR("outer/Inn3") );
    recursiveIt.Initialize( cursor );                UT_EQ( A_CHAR("ght"      ) , recursiveIt.Node().Name() )
    recursiveIt.DeleteNode();                         UT_EQ( A_CHAR("abc"      ) , recursiveIt.Node().Name() )
                                                      UT_EQ( uinteger(2) , recursiveIt.Node().Parent().CountChildren() )
    recursiveIt.DeleteNode();                         UT_EQ( A_CHAR("def"      ) , recursiveIt.Node().Name() )
                                                      UT_EQ( uinteger(1) , recursiveIt.Node().Parent().CountChildren() )
    recursiveIt.DeleteNode();                         UT_TRUE( !recursiveIt.IsValid() )
    recursiveIt.SetSorting( lang::Switch::Off  );
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

UT_METHOD(StringTree_RecIter_Const)
{
    // it mainly is about: does it compile? Because of the templated Cursor/RecursiveIterator
    // types.
    UT_INIT()

    MonoAllocator ba(4*1024);
    AStringST tree(&ba, '/');
    auto cursor=  tree.Root();

    cursor.CreatePathIfNotExistent( A_CHAR( ""               ));
    cursor.CreatePathIfNotExistent( A_CHAR( "outer"          )).first.Value().Reset( "aDir"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first.Value().Reset( "inner" );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn1"     )).first.Value().Reset( "inn1"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn2"     )).first.Value().Reset( "inn2"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2"           )).first.Value().Reset( "dir2"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/inn21"     )).first.Value().Reset( "inn21" );

    const  AStringST& ctree= tree;

    auto node= ctree.Root();
    node.GoToFirstChild();      UT_EQ( "aDir" , node.Value() )
    node.GoToNextSibling();     UT_EQ( "dir2" , node.Value() )
    node.GoToFirstChild();      UT_EQ( "inn21", node.Value() )
    node.GoToParent();          UT_EQ( "dir2" , node.Value() )
    node.GoToPreviousSibling(); UT_EQ( "aDir" , node.Value() )
    node.GoToLastChild();       UT_EQ( "inn2" , node.Value() )


    AStringST::ConstRecursiveIterator rit;
    rit.SetPathGeneration(lang::Switch::On);
    rit.Initialize( ctree );  UT_TRUE (   rit.IsValid())    UT_EQ( "aDir"  , rit.Node().Value() )
    rit.Next();               UT_TRUE (   rit.IsValid())    UT_EQ( "inner" , rit.Node().Value() )
    rit.Next();               UT_TRUE (   rit.IsValid())    UT_EQ( "inn1"  , rit.Node().Value() )
    rit.Next();               UT_TRUE (   rit.IsValid())    UT_EQ( "inn2"  , rit.Node().Value() )

    AStringST::ConstCursor node2= rit.Node();                  UT_EQ( "inn2", node2     .Value() )
    node2.GoToPreviousSibling();  UT_TRUE ( node2.IsValid())    UT_EQ( "inn1", node2     .Value() )
    rit.NextParentSibling();      UT_TRUE (   rit.IsValid())    UT_EQ( "dir2", rit.Node().Value() )
    rit.NextParentSibling();      UT_FALSE(   rit.IsValid())
}


#include "unittests/aworx_unittests_end.hpp"

} // namespace ut_aworx


#endif // ALIB_UT_MONOMEM

// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_MONOMEM

#include "ALib.ALox.H"

#include "ALib.Variables.IniFile.H"
#include "ALib.Containers.StringTree.H"
#include "ALib.Containers.StringTreeIterator.H"
#include "ALib.Monomem.H"
#include "ALib.System.H"
#include "ALib.Format.H"

#define TESTCLASSNAME       UT_ContMono_StringTree

#include "aworx_unittests.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace alib;

namespace ut_aworx {

namespace ut_stringtree { namespace {

#include "ALib.Lang.CIFunctions.H"

//--------------------------------------------------------------------------------------------------
//--- StringTree_Cursor
//--------------------------------------------------------------------------------------------------

using AStringST= StringTree<MonoAllocator, NAString,StringTreeNamesStatic<character>>;

struct ValueSorter : StringTreeIterator<AStringST>::Sorter
{
    bool Compare (const AStringST::ConstCursor& lhs,
                  const AStringST::ConstCursor& rhs) override {
        return lhs->CompareTo<CHK, lang::Case::Ignore>(*rhs) < 0 ;
    }
};


using MyTree= StringTree<MonoAllocator, const char*>;
StringTreeIterator<MyTree> testIt;

void testIteration( AWorxUnitTesting& ut,
                    MyTree::Cursor& cursor,
                    int qtyChilds,
                    unsigned recursionDepth, int qtyChildsRecursive,
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
    testIt.SetMaxDepth( recursionDepth );
    testIt.Initialize( cursor, lang::Inclusion::Exclude );
    int cnt= 0;
    while ( testIt.IsValid() )
    {
        if( debugOutput )
            UT_PRINT( "{:02} Depth={} QtyChildren={} N={:<3} Value={:<8}  Path: {}" , cnt,
                      testIt.CurrentDepth(), testIt.Node().CountChildren(), testIt.Node().Name(),
                      *testIt.Node(), testIt.Path() )
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
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), *it )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)


    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk")
    cnt= 0;
    for( auto it=cursor.FirstChild(); it.IsValid() ; it.GoToNextSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), *it )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)

    if( debugOutput )
        UT_PRINT( "testIteration() debug output: non-recursive walk backward")
    cnt= 0;
    for( auto it=cursor.LastChild(); it.IsValid() ; it.GoToPreviousSibling() )
    {
        if( debugOutput )
            UT_PRINT( "N={:<8} Value={:<8}", it.Name(), *it )
        ++cnt;
    }
    UT_EQ( qtyChilds, cnt)
}
#include "ALib.Lang.CIMethods.H"

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
    MonoAllocator ma(ALIB_DBG("UTStringTree",) 4);
    {
        alib::StringTree<MonoAllocator, ut_stringtree::DynInt, StringTreeNamesDynamic<wchar>> tree( ma, A_WCHAR( '/'));

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
        ptr.CreatePathIfNotExistent( A_WCHAR("C12/C21"), 2);    UT_EQ(  2,  *result.first->value) UT_EQ(  2,  result.second  )
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
        using ST= alib::StringTree<MonoAllocator, int, StringTreeNamesStatic<char>, Recycling::Shared>;
        ST::SharedRecyclerType sharedRecycler(ma);
        ST  tree1( '/', sharedRecycler );
        ST  tree2( '/', sharedRecycler );

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
        alib::StringTree<MonoAllocator,int, StringTreeNamesStatic<char>, Recycling::None> tree(ma, '/');

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
    MonoAllocator ma(ALIB_DBG("UTStringTreeCursor",) 4);

    cout << "\nCursor():" << endl;
    cout << "Build string tree without using cursor navigation" << endl;
    MyTree pm( ma, '/');


        auto cursor=   pm.Root();
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("a")    ) ) *cursor= "a--"    ;
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("A")    ) ) *cursor= "aA-"    ;
           UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("1")    ) ) *cursor= "aA1"    ;
    cursor.GoToParent(); UT_TRUE(  cursor.IsValid() ) UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("b")    ) ) *cursor= "aAb"    ;
    cursor.GoToParent(); UT_TRUE(  cursor.IsValid() ) UT_TRUE( cursor.GoToCreateChildIfNotExistent( A_CHAR("c")    ) ) *cursor= "aAc"    ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent( A_CHAR( "a/B"   )  ); *cursor= "aB-"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent( A_CHAR( "a/B/1" )  ); *cursor= "aB1"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent( A_CHAR( "a/B/2" )  ); *cursor= "aB2"  ;
    cursor= pm.Root(); cursor.GoToCreatedPathIfNotExistent( A_CHAR( "a/B/3" )  ); *cursor= "aB3"  ;
                       cursor.GoToCreatedPathIfNotExistent( A_CHAR("/a/C"   )  ); *cursor= "aC-"  ;
                       cursor.GoToCreatedPathIfNotExistent( A_CHAR("/a/C/1" )  ); *cursor= "aC1"  ;
                       cursor.GoToCreatedPathIfNotExistent( A_CHAR("/a/C/2" )  ); *cursor= "aC2"  ;
                       cursor.GoToCreatedPathIfNotExistent( A_CHAR("/a/C/3" )  ); *cursor= "aC3"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b"     )  ).first= "b--"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A"   )  ).first= "bA-"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/1" )  ).first= "bA1"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/2" )  ).first= "bA2"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/A/3" )  ).first= "bA3"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B"   )  ).first= "bB-"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/1" )  ).first= "bB1"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/2" )  ).first= "bB2"  ;
                      *cursor    .CreatePathIfNotExistent ( A_CHAR("/b/B/3" )  ).first= "bB3"  ;
             (cursor=  cursor    .CreatePathIfNotExistent ( A_CHAR("/b/C"   )  ).first).Value()= "bC-"  ;
                 auto cursor2 =  cursor.CreateChild       ( A_CHAR("1"      )  ); *cursor2="bC1"  ;
                      cursor2 =  cursor.CreateChild<NC>   ( A_CHAR("2"      )  ); *cursor2="bC2"  ;
                      cursor2 =  cursor.CreateChild       ( A_CHAR("3"      )  ); *cursor2="bC3"  ;
                      cursor2 =  cursor.CreateChild       ( A_CHAR("3"      )  ); UT_TRUE( cursor2.IsInvalid() )

    cout << "Check construction (sizes of some branches)" << endl;
              cursor= pm.Root();                                            UT_EQ( 0, cursor.Depth() ) testIteration( ut, cursor, 2, 100, 26, true );
    UT_TRUE( (cursor= pm.Root()).GoToChild(A_CHAR("a")  )              )    UT_EQ( 1, cursor.Depth() ) testIteration( ut, cursor, 3, 100, 12 );
              cursor=        cursor.Child(A_CHAR("B"));                     UT_EQ( 2, cursor.Depth() ) testIteration( ut, cursor, 3, 100,  3 );
    UT_TRUE( (cursor= pm.Root()).GoTo (A_CHAR("a/B/3"))  .IsEmpty())    UT_EQ( 3, cursor.Depth() ) testIteration( ut, cursor, 0, 100,  0 );
    UT_FALSE((cursor= pm.Root()).GoTo (A_CHAR("a/B/3/e")).IsEmpty())    UT_EQ( 3, cursor.Depth() ) testIteration( ut, cursor, 0, 100,  0 );
    UT_FALSE((cursor= pm.Root()).GoTo (A_CHAR("ab/ce"))  .IsEmpty())    UT_EQ( 0, cursor.Depth() ) testIteration( ut, cursor, 2, 100, 26 );

    UT_PRINT( "Cursor navigation" )
    AString path;
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )
    cursor= pm.Root();                                                                       UT_EQ( A_CHAR("/"     ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a")           ).IsEmpty()) UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_FALSE( cursor.GoTo(A_CHAR("XYZ")         ).IsEmpty()) UT_EQ( A_CHAR("/"     ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("b")           ).IsEmpty()) UT_EQ( A_CHAR("/b"    ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_FALSE( cursor.GoToCreateChildIfNotExistent(A_CHAR("a")) )          UT_EQ( A_CHAR("/a"    ), cursor.AssemblePath(path) )

    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a/B/./1")     ).IsEmpty()) UT_EQ( A_CHAR("/a/B/1"), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a/B/1/..")    ).IsEmpty()) UT_EQ( A_CHAR("/a/B"  ), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a/B/1/../1")  ).IsEmpty()) UT_EQ( A_CHAR("/a/B/1"), cursor.AssemblePath(path) )
    cursor= pm.Root(); UT_TRUE ( cursor.GoTo(A_CHAR("a/B/1")       ).IsEmpty()) UT_EQ( NString("aB1" ) , *cursor )
                       UT_TRUE ( cursor.GoTo(A_CHAR("../2")        ).IsEmpty()) UT_EQ( NString("aB2" ) , *cursor )
                       UT_FALSE( cursor.GoTo(A_CHAR("b")           ).IsEmpty()) UT_EQ( NString("aB2" ) , *cursor )
                       UT_TRUE ( cursor.GoTo(A_CHAR("/b")          ).IsEmpty()) UT_EQ( NString("b--" ) , *cursor )
                       UT_TRUE ( cursor.GoTo(A_CHAR("./C")         ).IsEmpty()) UT_EQ( NString("bC-" ) , *cursor )


    UT_PRINT( "Up" )
    UT_TRUE( (cursor= pm.Root()).GoTo( A_CHAR("a/B/3") ).IsEmpty() )
                             UT_EQ( A_CHAR("3"),  cursor.Name() )
                             UT_FALSE( cursor.IsInvalid() )
                             UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_EQ( A_CHAR("B"),  cursor.Name() ) UT_FALSE( cursor.IsInvalid() ) UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_EQ( A_CHAR("a"),  cursor.Name() ) UT_FALSE( cursor.IsInvalid() ) UT_FALSE( cursor.IsRoot() )
    cursor.GoToParent();    UT_TRUE( cursor.Name().IsEmpty()   ) UT_FALSE( cursor.IsInvalid() ) UT_TRUE(  cursor.IsRoot() )
    cursor.GoToParent();    UT_TRUE( cursor.IsInvalid() )

    UT_PRINT( "Siblings" )
    UT_TRUE( (cursor= pm.Root()).GoTo( A_CHAR("a/C") ).IsEmpty() )
                                                UT_EQ( A_CHAR("C"),  cursor.Name() )
    auto it= cursor.FirstChild();               UT_EQ( A_CHAR("1"),  it.Name() )
                                                UT_TRUE( it.NextSibling().IsValid() )
                                                UT_TRUE( it.PreviousSibling().IsInvalid() )
    UT_FALSE(it.GoToPreviousSibling())          UT_TRUE( it.IsInvalid() )
    it= cursor.LastChild();                     UT_EQ( A_CHAR("3"),  it.Name() )
                                                UT_TRUE( it.PreviousSibling().IsValid() )
                                                UT_TRUE( it.NextSibling().IsInvalid() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= cursor.FirstChild();                    UT_EQ( A_CHAR("1"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("2"),  it.Name() )
    UT_TRUE(it.GoToNextSibling())               UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )
    it= cursor;                                 UT_EQ( A_CHAR("C"),  cursor.Name() )
    UT_TRUE(it.GoToFirstChild())                UT_EQ( A_CHAR("1"),  it.Name() )
    it= cursor;                                 UT_EQ( A_CHAR("C"),  cursor.Name() )
    UT_TRUE(it.GoToLastChild())                 UT_EQ( A_CHAR("3"),  it.Name() )
    UT_FALSE(it.GoToNextSibling())              UT_TRUE( it.IsInvalid() )

    UT_PRINT( "Depth and Distance" )
    cursor= pm.Root();              UT_EQ(  0, cursor.Depth() )
                                    UT_EQ(  0, cursor.Distance( pm.Root() ) )
    it= cursor;                     UT_EQ(  0, cursor.Distance( it        ) )
    cursor.GoToChild(A_CHAR("a"));  UT_EQ(  1, cursor.Depth() )
                                    UT_EQ(  1, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    cursor.GoToChild(A_CHAR("C"));  UT_EQ(  2, cursor.Depth() )
                                    UT_EQ(  2, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    it.GoToChild(A_CHAR("b"));      UT_EQ( -1, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    it= pm.Root();                  UT_EQ(  2, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    it.GoToChild(A_CHAR("a"));      UT_EQ(  1, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    it.GoToChild(A_CHAR("C"));      UT_EQ(  0, cursor.Distance( it        ) )
                                    UT_EQ(  0, it    .Distance( cursor    ) )
    it.GoToParent();                UT_EQ(  1, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )
    it.GoToChild(A_CHAR("B"));      UT_EQ( -1, cursor.Distance( it        ) )
                                    UT_EQ( -1, it    .Distance( cursor    ) )

    UT_PRINT( "GetPath" )
    path.SetNull();
    cursor= pm.Root(); cursor.GoToParent();        UT_TRUE ( cursor.IsInvalid() )
    path.Reset();
    cursor= pm.Root();                                               UT_EQ   ( A_CHAR("/")        , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoTo(A_CHAR("a")      );  UT_EQ   ( A_CHAR("/a")       , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoTo(A_CHAR("a/b")    );  UT_EQ   ( A_CHAR("/a")       , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoTo(A_CHAR("a/B")    );  UT_EQ   ( A_CHAR("/a/B")     , cursor.AssemblePath( path ))
    cursor= pm.Root(); cursor.GoTo(A_CHAR("a/B/1")  );  UT_EQ   ( A_CHAR("/a/B/1")   , cursor.AssemblePath( path ))
                                                                     UT_EQ   ( A_CHAR("a/B/1")    , cursor.AssemblePath( path, pm.Root() ))
                                                                     UT_EQ   ( A_CHAR("1")        , cursor.AssemblePath( path, cursor.Parent() ) )
                                                                     UT_EQ   ( A_CHAR("B/1")        , cursor.AssemblePath( path, cursor.Parent().Parent() ) )

    UT_PRINT( "Erase nodes" )
    cursor= pm.Root();                                     testIteration( ut, cursor,  2,  99,   26 );
    cursor= cursor( A_CHAR("a/B")  ).first;                UT_EQ( A_CHAR("B") , cursor.Name() )
                                                           testIteration( ut, cursor,  3,  99,   3, true );
    it= cursor.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                  UT_EQ( A_CHAR("2") , it.Name() )


                                                           UT_EQ( uinteger(3) , cursor.CountChildren() )
    cursor.DeleteChild( it );                              UT_EQ( uinteger(2) , cursor.CountChildren() )
    it= cursor.FirstChild();                               UT_EQ( A_CHAR("1") , it.Name() )
    it.GoToNextSibling();                                  UT_EQ( A_CHAR("3") , it.Name() )
    it.GoToNextSibling();                                  UT_TRUE( it.IsInvalid() )

    UT_TRUE(cursor.GoTo( A_CHAR("/a/B/3") ).IsEmpty()) UT_EQ( A_CHAR("3") , cursor.Name() )
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
//--- StringTreeIterator
//--------------------------------------------------------------------------------------------------
#include "ALib.Lang.CIFunctions.H"

template <typename TCursor>
int doIterations( AWorxUnitTesting&              ut,
                  StringTreeIterator<AStringST>& iterator,
                  TCursor                        startNode,
                  bool                           includeStartNode,
                  unsigned                       recursionDepth     )
{
    String512 startPath;

    int cnt= 0;
    startNode.AssemblePath(startPath);
    iterator.SetMaxDepth( recursionDepth );
    iterator.Initialize( startNode, includeStartNode ? lang::Inclusion::Include : lang::Inclusion::Exclude );
    if( !iterator.IsValid() )
    {
        UT_PRINT( "Invalid iterator after initialization (e.g., no children in given node). No iterations performed.")
        return 0;
    }

    UT_PRINT( "\nIterator test. Iteration start path: {!Q}, include startNode= {}, depth: {}",
              startPath, includeStartNode, iterator.MaxDepth() )

    for ( ; iterator.IsValid() ; iterator.Next() )
    {
        String512 filePath;
        iterator.Node().AssemblePath(filePath);
        const String  nodeName= iterator.Node().Name();
        UT_PRINT( "Depth: {!ATab:2}  "
                  "  Node: {!ATab!Q} "
                  "  Value: {!ATab!Q} "
                  "  Children: {!ATab:2} "
                  "  path: {!ATab!Q} "
                  "  (true path): {!ATab!Q} " ,
                   iterator.CurrentDepth()    ,
                   nodeName                   ,
                   iterator.Node().Value()    ,
                   iterator.Node().CountChildren(),
                   iterator.Path(), filePath
                 )

        UT_EQ(filePath, iterator.Path() )

        ++cnt;
    }

    return cnt;
}
#include "ALib.Lang.CIMethods.H"

UT_METHOD(StringTreeIterator)
{
    UT_INIT()

    MonoAllocator ma(ALIB_DBG("UTStringRO",)4);
    AStringST tree(ma, '/');
    tree.ConstructRootValue("ROOT");
    auto cursor=  tree.Root();

    cursor.CreatePathIfNotExistent( A_CHAR( ""               ));
    cursor.CreatePathIfNotExistent( A_CHAR( "outer"          )).first->Reset( "aDir"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first->Reset( "inner"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/xinn1"    )).first->Reset( "inn1"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn2"     )).first->Reset( "inn2"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3"     )).first->Reset( "xinn3"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn4"     )).first->Reset( "inn4"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn5"     )).first->Reset( "inn5"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first->Reset( "Overwritten");
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2"           )).first->Reset( "dir2"       );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-a"   )).first->Reset( "subd2-a"    );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/subd2-b"   )).first->Reset( "subd2-b"    );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/abc" )).first->Reset( "sort2"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/def" )).first->Reset( "sort3"      );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/Inn3/ght" )).first->Reset( "sort1"      );

    StringTreeIterator<AStringST> stit;
    stit.SetPathGeneration( lang::Switch::On );

    // check cursor/iterator creation
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude );
    UT_TRUE( stit.Node().Name() == tree.Root().FirstChild().Name()  )


    UT_PRINT(NEW_LINE, "--- non recursive ---" )
    int qtyIt;
    qtyIt = doIterations(ut, stit, tree.Root(), false, 0);  UT_EQ(2, qtyIt)
    qtyIt = doIterations(ut, stit, tree.Root(), true , 0);  UT_EQ(1, qtyIt)
    qtyIt = doIterations(ut, stit, tree.Root(), true , 1);  UT_EQ(3, qtyIt)

    UT_PRINT(NEW_LINE, "--- non recursive ---" )
    decltype(tree)::Cursor start=   tree.Root();
    UT_TRUE( start.GoTo(A_CHAR("outer/Inn3")).IsEmpty() )

    qtyIt = doIterations( ut, stit, start, false, 0 ); UT_EQ( 3, qtyIt)
    qtyIt = doIterations( ut, stit, start, true , 0 ); UT_EQ( 1, qtyIt)
    qtyIt = doIterations( ut, stit, start, true , 1 ); UT_EQ( 4, qtyIt)



    UT_PRINT(NEW_LINE, "--- non recursive decending---" )
    decltype(stit)::NameSorter sorter;
    sorter.Descending   = true;
    sorter.CaseSensitive= true;
    stit.SetSorting( &sorter );
    qtyIt = doIterations(ut, stit, start, false, 0); UT_EQ(3, qtyIt)
    qtyIt = doIterations(ut, stit, start, true , 0); UT_EQ(1, qtyIt)
    qtyIt = doIterations(ut, stit, start, true , 1); UT_EQ(4, qtyIt)


    UT_PRINT(NEW_LINE, "--- ascending ---" )
    sorter.Descending   = false;
    sorter.CaseSensitive= true;
    qtyIt = doIterations(ut, stit, tree.Root(), false, 99);  UT_EQ(13, qtyIt)
    qtyIt = doIterations(ut, stit, tree.Root(), true , 99);  UT_EQ(14, qtyIt)

    UT_PRINT(NEW_LINE, "--- descending ---" )
    sorter.Descending   = true;
    sorter.CaseSensitive= true;
    qtyIt = doIterations(ut, stit, tree.Root(), false, (std::numeric_limits<int>::max)());  UT_EQ(13, qtyIt)
    qtyIt = doIterations(ut, stit, tree.Root(), true , (std::numeric_limits<int>::max)());  UT_EQ(14, qtyIt)

    UT_PRINT(NEW_LINE, "--- value ---" )
    ValueSorter  vs;
    stit.SetSorting( &vs );
    qtyIt = doIterations(ut, stit, tree.Root(), false, (std::numeric_limits<int>::max)());  UT_EQ(13, qtyIt)
    qtyIt = doIterations(ut, stit, tree.Root(), true , (std::numeric_limits<int>::max)());  UT_EQ(14, qtyIt)

    UT_PRINT(NEW_LINE, "--- value ---" )
    stit.SetSorting( &vs );
    start=   tree.Root();
    UT_TRUE( start.GoTo(A_CHAR("outer/Inn3")).IsEmpty() )
    qtyIt = doIterations( ut, stit, start, false, ( std::numeric_limits<int>::max )() ); UT_EQ( 3, qtyIt)
    qtyIt = doIterations( ut, stit, start, true , ( std::numeric_limits<int>::max )() ); UT_EQ( 4, qtyIt)

    qtyIt = doIterations(ut, stit, start, false, 1); UT_EQ( 3, qtyIt )
    qtyIt = doIterations(ut, stit, start, true , 1); UT_EQ( 4, qtyIt )


    UT_PRINT(NEW_LINE, "--- value ---" )
    UT_TRUE( start.GoTo(A_CHAR("abc")).IsEmpty() )
    qtyIt = doIterations( ut, stit, start, false, ( std::numeric_limits<int>::max )() ); UT_EQ( 0, qtyIt)
    qtyIt = doIterations( ut, stit, start, false, 4                                   ); UT_EQ( 0, qtyIt)
    qtyIt = doIterations( ut, stit, start, true , ( std::numeric_limits<int>::max )() ); UT_EQ( 1, qtyIt)
    qtyIt = doIterations( ut, stit, start, true , 4                                   ); UT_EQ( 1, qtyIt)

    //---------- test skipping ---------------------
    UT_PRINT(NEW_LINE, "------- Test skipping ---" )
    stit.SetSorting( nullptr );

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("dir2"   ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_TRUE( !stit.IsValid() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("dir2"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("subd2-a") , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("subd2-b") , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_TRUE( !stit.IsValid() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_TRUE( !stit.IsValid() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inner"  ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_EQ( A_CHAR("dir2"   ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_TRUE( !stit.IsValid() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inner"  ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("xinn1"  ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inn2"   ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_EQ( A_CHAR("dir2"   ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_TRUE( !stit.IsValid() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"  ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inner"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("xinn1"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("inn2"   ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("Inn3"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("abc"    ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("def"    ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_EQ( A_CHAR("inn4"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inn5"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("dir2"   ) , stit.Node().Name() )
    stit.NextParentSibling();                                 UT_TRUE( !stit.IsValid() )

    // test iterator with no children
    cursor= tree.Root();
    cursor.GoTo( A_CHAR("dir2/subd2-a"));                     UT_EQ( uinteger(0), cursor.CountChildren() )
    stit.Initialize( cursor, lang::Inclusion::Exclude );      UT_TRUE( !stit.IsValid() )

    // test copying constructor and copying
    stit.SetMaxDepth( 1 );
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"), stit.Node().Name() )
    StringTreeIterator<AStringST> recursiveIt2(stit);         UT_EQ( A_CHAR("outer"), recursiveIt2.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inner"), stit.Node().Name() )
                                                              UT_EQ( A_CHAR("outer"), recursiveIt2.Node().Name() )
    recursiveIt2= stit;
    while( stit.IsValid() )
    {
        UT_TRUE( recursiveIt2.IsValid() )
        UT_EQ( stit.Node().Name()  , recursiveIt2.Node().Name() )
        UT_EQ( stit.Next(), recursiveIt2.Next() )
    }
    UT_TRUE( recursiveIt2.IsInvalid() )

    //---------- test sorting ---------------------
    UT_PRINT(NEW_LINE, "------- Test sorting ---" )
    sorter.Descending= false;
    stit.SetSorting( &sorter  );
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("dir2"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("outer" ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_TRUE( !stit.IsValid() )


    sorter.Descending   = true;
    sorter.CaseSensitive= false;
    stit.SetSorting( &sorter  );
    stit.SetMaxDepth();
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer" ) , stit.Node().Name() )
    sorter.Descending   = false;
    sorter.CaseSensitive= false;
    stit.Next();                                              UT_EQ( A_CHAR("inn2"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("Inn3"  ) , stit.Node().Name() )
    stit.SetSorting( &vs  );
    stit.Next();                                              UT_EQ( A_CHAR("ght"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("abc"   ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("def"   ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("inn4"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("inn5"  ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("inner" ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("xinn1" ) , stit.Node().Name() )
    stit.NextSibling();                                       UT_EQ( A_CHAR("dir2"  ) , stit.Node().Name() )


    //---------- test deletion ---------------------
    UT_PRINT(NEW_LINE, "------- Test deletion ---" )
    stit.SetSorting( &vs  );
    cursor= tree.Root();
    cursor.GoTo( A_CHAR("outer/Inn3") );
    stit.Initialize( cursor, lang::Inclusion::Exclude );      UT_EQ( A_CHAR("ght"      ) , stit.Node().Name() )
    stit.DeleteNode();                                        UT_EQ( A_CHAR("abc"      ) , stit.Node().Name() )
                                                              UT_EQ( uinteger(2) , stit.Node().Parent().CountChildren() )
    stit.DeleteNode();                                        UT_EQ( A_CHAR("def"      ) , stit.Node().Name() )
                                                              UT_EQ( uinteger(1) , stit.Node().Parent().CountChildren() )
    stit.DeleteNode();                                        UT_TRUE( !stit.IsValid() )
    stit.SetSorting( nullptr  );
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"    ) , stit.Node().Name() )
                                                              UT_EQ( uinteger(6) , stit.Node().CountChildren() )
    stit.Node().DeleteChild( A_CHAR("xinn1"));                UT_EQ( uinteger( 5), stit.Node().CountChildren() )
    stit.Node().DeleteChild( A_CHAR("Inn3" ));                UT_EQ( uinteger( 4), stit.Node().CountChildren() )
    stit.Next();                                              UT_EQ( A_CHAR("inner"    ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inn2"     ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inn4"     ) , stit.Node().Name() )
    stit.Next();                                              UT_EQ( A_CHAR("inn5"     ) , stit.Node().Name() )

    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_EQ( A_CHAR("outer"    ) , stit.Node().Name() )
    stit.DeleteNode();                                        UT_EQ( A_CHAR("dir2"     ) , stit.Node().Name() )
    stit.DeleteNode();                                        UT_TRUE( !stit.IsValid() )
    stit.Initialize( tree.Root(), lang::Inclusion::Exclude ); UT_TRUE( !stit.IsValid() )
}

UT_METHOD(StringTree_RecIter_Const)
{
    // it mainly is about: does it compile? Because of the templated Cursor/StringTreeIterator
    // types.
    UT_INIT()

    MonoAllocator ma(ALIB_DBG("UTStringTreeRecItC",)4);
    AStringST tree(ma, '/');
    auto cursor=  tree.Root();

    cursor.CreatePathIfNotExistent( A_CHAR( ""               ));
    cursor.CreatePathIfNotExistent( A_CHAR( "outer"          )).first->Reset( "aDir"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inner"    )).first->Reset( "inner" );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn1"     )).first->Reset( "inn1"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "outer/inn2"     )).first->Reset( "inn2"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2"           )).first->Reset( "dir2"  );
    cursor.CreatePathIfNotExistent( A_CHAR( "dir2/inn21"     )).first->Reset( "inn21" );

    const  AStringST& ctree= tree;

    auto node= ctree.Root();
    node.GoToFirstChild();      UT_EQ( "aDir" , NString(*node) )
    node.GoToNextSibling();     UT_EQ( "dir2" , NString(*node) )
    node.GoToFirstChild();      UT_EQ( "inn21", NString(*node) )
    node.GoToParent();          UT_EQ( "dir2" , NString(*node) )
    node.GoToPreviousSibling(); UT_EQ( "aDir" , NString(*node) )
    node.GoToLastChild();       UT_EQ( "inn2" , NString(*node) )


    StringTreeIterator<const AStringST> stit;
    stit.SetPathGeneration(lang::Switch::On);
    stit.Initialize( ctree.Root(), lang::Inclusion::Exclude );  UT_TRUE (   stit.IsValid())   UT_EQ( "aDir"  , NString(*stit.Node()) )
    stit.Next();                                                UT_TRUE (   stit.IsValid())   UT_EQ( "inner" , NString(*stit.Node()) )
    stit.Next();                                                UT_TRUE (   stit.IsValid())   UT_EQ( "inn1"  , NString(*stit.Node()) )
    stit.Next();                                                UT_TRUE (   stit.IsValid())   UT_EQ( "inn2"  , NString(*stit.Node()) )

    AStringST::ConstCursor node2= stit.Node();                  UT_EQ( "inn2"  , NString(*node2     ) )
    node2.GoToPreviousSibling();                                UT_TRUE ( node2.IsValid())    UT_EQ( "inn1"  , NString(*node2     ) )
    stit.NextParentSibling();                                   UT_TRUE (   stit.IsValid())   UT_EQ( "dir2"  , NString(*stit.Node()) )
    stit.NextParentSibling();                                   UT_FALSE(   stit.IsValid())
}


#include "aworx_unittests_end.hpp"

} // namespace ut_aworx


#endif // ALIB_UT_MONOMEM

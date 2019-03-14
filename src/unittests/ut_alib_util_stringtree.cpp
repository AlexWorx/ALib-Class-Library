// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if !defined(ALIB_UT_SELECT) || defined(ALIB_UT_CORE)

#include "alib/alox.hpp"

#if !defined (HPP_ALIB_CONFIG_INI_FILE)
    #include "alib/config/inifile.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif

#include "alib/strings/util/spaces.hpp"


#define TESTCLASSNAME       CPP_ALib_util_StringTree
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <fstream>

using namespace std;
using namespace aworx;

namespace ut_aworx {

//--------------------------------------------------------------------------------------------------
//--- StringTree_Cursor
//--------------------------------------------------------------------------------------------------
bool valueSorter(const AString& lhs, const AString& rhs)
{
    return lhs.CompareTo<true, Case::Ignore>(rhs) < 0 ;
}


UT_CLASS()


template <typename TIterator>
int IteratorCount( TIterator copyOfIt, const TIterator& end, bool debugOutput=false )
{
    if( debugOutput )
        cout << "Dbg output: " << endl;
    int cnt= 0;
    while ( copyOfIt != end )
    {
        if( debugOutput )
            cout << cnt << " D=" << copyOfIt.Depth() << " N= \""<< copyOfIt.Name() <<"\"  V=" << *copyOfIt
                 <<   "  #children=  "<< copyOfIt.Size() <<endl;
        ++cnt;
        ++copyOfIt;
    }
    return cnt;
}

UT_METHOD(StringTree_Cursor)
{
    UT_INIT();

    cout << "\nCursor():" << endl;
    cout << "Build string tree without using cursor navigation" << endl;
    StringTree<const char*> pm;
    auto cursor=   pm.Root();       *cursor= "root";

    cursor= pm.Root();                                    *cursor=  "root" ;
           UT_TRUE( cursor.MoveToChildCreate( A_CHAR("a")    ) ); *cursor= "a--"    ;
           UT_TRUE( cursor.MoveToChildCreate( A_CHAR("A")    ) ); *cursor= "aA-"    ;
           UT_TRUE( cursor.MoveToChildCreate( A_CHAR("1")    ) ); *cursor= "aA1"    ;
    UT_TRUE(  cursor.MoveToParent() ); UT_TRUE( cursor.MoveToChildCreate( A_CHAR("b")    ) ); *cursor= "aab"    ;
    UT_TRUE(  cursor.MoveToParent() ); UT_TRUE( cursor.MoveToChildCreate( A_CHAR("c")    ) ); *cursor= "aac"    ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/B"   )  ); *cursor= "aB-"   ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/B/1" )  ); *cursor= "aB1"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/B/2" )  ); *cursor= "aB2"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/B/3" )  ); *cursor= "aB3"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/C"   )  ); *cursor= "aC-"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/C/1" )  ); *cursor= "aC1"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/C/2" )  ); *cursor= "aC2"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("a/C/3" )  ); *cursor= "aC3"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b"     )  ); *cursor= "b--"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/A"   )  ); *cursor= "bA-"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/A/1" )  ); *cursor= "bA1"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/A/2" )  ); *cursor= "bA2"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/A/3" )  ); *cursor= "bA3"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/B"   )  ); *cursor= "bB-"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/B/1" )  ); *cursor= "bB1"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/B/2" )  ); *cursor= "bB2"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/B/3" )  ); *cursor= "bB3"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/C"   )  ); *cursor= "bC-"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/C/1" )  ); *cursor= "bC1"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/C/2" )  ); *cursor= "bC2"  ;
    cursor= pm.Root(); cursor.MoveToAndCreateNonExistingPart ( A_CHAR("b/C/3" )  ); *cursor= "bC3"  ;

    cout << "Check construction (sizes of some branches)" << endl;
    auto iter= pm.begin();                                     UT_EQ( 27, IteratorCount( iter, pm.end() ) );
//    iter.SetSorted( SortOrder::Ascending, Case::Sensitive ); UT_TRUE(  iter == pm.begin() );

              cursor= pm.Root();                            cursor.SetIterator( iter, 0 ); UT_EQ( 0, cursor.Depth() ); UT_EQ( 27, IteratorCount( iter, pm.end() ) );
    UT_TRUE( (cursor= pm.Root()).MoveToChild(A_CHAR("a")  )    );   cursor.SetIterator( iter, 0 ); UT_EQ( 13, IteratorCount( iter, pm.end() ) );
    UT_TRUE( (cursor= pm.Root()).MoveTo     (A_CHAR("a/B"))    );   cursor.SetIterator( iter, 0 ); UT_EQ(  4, IteratorCount( iter, pm.end() ) );
    UT_TRUE( (cursor= pm.Root()).MoveTo     (A_CHAR("a/B/3"))  );   cursor.SetIterator( iter, 0 ); UT_EQ(  1, IteratorCount( iter, pm.end() ) );
    UT_FALSE((cursor= pm.Root()).MoveTo     (A_CHAR("a/B/3/e")));   cursor.SetIterator( iter, 0 ); UT_EQ( 27, IteratorCount( iter, pm.end() ) ); UT_TRUE(  iter == pm.begin() );
    UT_FALSE((cursor= pm.Root()).MoveTo     (A_CHAR("ab/ce"))  );   cursor.SetIterator( iter, 0 ); UT_EQ( 27, IteratorCount( iter, pm.end() ) ); UT_TRUE(  iter == pm.begin() );


    UT_PRINT( "Cursor navigation" );

    cursor= pm.Root(); UT_TRUE(  cursor.MoveTo(A_CHAR("a")           )); UT_EQ( 13, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );
    cursor= pm.Root();                                                 UT_EQ(  2, IteratorCount(cursor.SetIterator(iter, 1 ), pm.end() ) );
    cursor= pm.Root(); UT_TRUE(  cursor.MoveTo(A_CHAR("a")           )); UT_EQ(  3, IteratorCount(cursor.SetIterator(iter, 1 ), pm.end() ) );
    cursor= pm.Root(); UT_FALSE( cursor.MoveTo(A_CHAR("XYZ")         )); UT_EQ( 27, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );
    cursor= pm.Root(); UT_TRUE ( cursor.MoveTo(A_CHAR("b")           )); UT_EQ( 13, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );
    cursor= pm.Root(); UT_FALSE( cursor.MoveToChildCreate(A_CHAR("a"))); UT_EQ( 13, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );

    cursor= pm.Root(); UT_TRUE ( cursor.MoveTo(A_CHAR("a/B/./1")     )); UT_EQ(  1, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );
    cursor= pm.Root(); UT_TRUE ( cursor.MoveTo(A_CHAR("a/B/1/../1")  )); UT_EQ(  1, IteratorCount(cursor.SetIterator(iter, 0 ), pm.end() ) );

    cursor= pm.Root(); UT_TRUE ( cursor.MoveTo(A_CHAR("a/B/1")  )); UT_EQ(  "aB1", *cursor );
                       UT_TRUE ( cursor.MoveTo(A_CHAR("../2")   )); UT_EQ(  "aB2", *cursor );
                       UT_FALSE( cursor.MoveTo(A_CHAR("b")      )); UT_EQ(  "aB2", *cursor );
                       UT_TRUE ( cursor.MoveTo(A_CHAR("/b")     )); UT_EQ(  "b--", *cursor );
                       UT_TRUE ( cursor.MoveTo(A_CHAR("./C")    )); UT_EQ(  "bC-", *cursor );


    UT_PRINT( "Up" );
    UT_TRUE( (cursor= pm()).MoveTo( A_CHAR("a/B/3") ) );
                                                UT_EQ( A_CHAR("3"),        cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_FALSE( cursor.IsRoot() );
    cursor.MoveToParent();                      UT_EQ( A_CHAR("B")       , cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_FALSE( cursor.IsRoot() );
    cursor.MoveToParent<false>();               UT_EQ( A_CHAR("a")       , cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_FALSE( cursor.IsRoot() );
    cursor.MoveToParent<true>();                UT_EQ( NullString(), cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_TRUE(  cursor.IsRoot() );
    cursor.MoveToParent<true>();                UT_EQ( NullString(), cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_TRUE(  cursor.IsRoot() );
    cursor.MoveToParent();                      UT_EQ( NullString(), cursor.SearchName() ); UT_FALSE( cursor.IsInvalid() ); UT_TRUE(  cursor.IsRoot() );
    cursor.MoveToParent<false>();  UT_TRUE ( cursor.IsInvalid() );

    UT_PRINT( "GetPath" );
    AString path;
    cursor= pm(); cursor.MoveToParentUnchecked(); cursor.SearchPath( path );  UT_TRUE ( path.IsNull() );
    cursor= pm();                                 cursor.SearchPath( path );  UT_FALSE( path.IsNull() ); UT_TRUE( path.IsEmpty() );
    cursor= pm(); cursor.MoveTo(A_CHAR("a")      );       cursor.SearchPath( path );  UT_EQ   ( A_CHAR("a")        , path);
    cursor= pm(); cursor.MoveTo(A_CHAR("a/b")    );       cursor.SearchPath( path );  UT_EQ   ( A_CHAR("")         , path);
    cursor= pm(); cursor.MoveTo(A_CHAR("a/B")    );       cursor.SearchPath( path );  UT_EQ   ( A_CHAR("a/B")      , path);
    cursor= pm(); cursor.MoveTo(A_CHAR("a/B/1")  );       cursor.SearchPath( path );  UT_EQ   ( A_CHAR("a/B/1")    , path);

    UT_PRINT( "Iterators"  );
    {
         cursor= pm.Root(); auto it= cursor.begin( 0 );  UT_EQ( 27, IteratorCount( it, pm.end() ) );

         cursor= pm.Root();                  cursor.SetIterator(it, 0); UT_EQ( 27, IteratorCount( it, pm.end() ) );
         cursor= pm.Root();                  cursor.SetIterator(it,99); UT_EQ( 26, IteratorCount( it, pm.end() ) );
         cursor= pm.Root();                  cursor.SetIterator(it, 3); UT_EQ( 26, IteratorCount( it, pm.end() ) );
         cursor= pm.Root();                  cursor.SetIterator(it, 2); UT_EQ(  8, IteratorCount( it, pm.end() ) );
         cursor= pm.Root();                  cursor.SetIterator(it, 1); UT_EQ(  2, IteratorCount( it, pm.end() ) );

        (cursor= pm.Root()).MoveTo(A_CHAR("a")    ); cursor.SetIterator(it, 0); UT_EQ( 13, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a")    ); cursor.SetIterator(it, 2); UT_EQ( 12, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a")    ); cursor.SetIterator(it, 1); UT_EQ(  3, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a/A")  ); cursor.SetIterator(it, 0); UT_EQ(  4, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a/A")  ); cursor.SetIterator(it, 1); UT_EQ(  3, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a/A/1")); cursor.SetIterator(it, 0); UT_EQ(  1, IteratorCount( it, pm.end() ) );
        (cursor= pm.Root()).MoveTo(A_CHAR("a/A/1")); cursor.SetIterator(it, 1); UT_EQ(  0, IteratorCount( it, pm.end() ) );

        // quick test for range it on cursor
        {
            (cursor= pm.Root()).MoveTo(A_CHAR("a"));
            int cnt= 0;
            for( auto& rangeIt : cursor )
            {
                (void) rangeIt;
                cnt++;
            }
            UT_EQ(  3, cnt);
            cnt= 0;
            std::for_each( cursor.begin( 5 ), cursor.end(), [&cnt]( const char*& x ) {(void)x; cnt++; } );
            UT_EQ(  12, cnt);
        }

    }


    UT_PRINT( "Erase nodes" );
    iter= pm.begin(); UT_EQ(27, IteratorCount(iter, pm.end() ) );
    cursor= decltype(pm)::Cursor( iter );
    UT_TRUE(cursor.MoveTo( A_CHAR("a/B/3")  ));               UT_EQ( A_CHAR("3")       , cursor.SearchName() ); UT_EQ( 0, cursor.Size() ); UT_EQ( 1, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_FALSE( cursor.IsRoot() );
    cursor.DeleteChildren();                          UT_EQ( A_CHAR("3")       , cursor.SearchName() ); UT_EQ( 0, cursor.Size() ); UT_EQ( 1, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_FALSE( cursor.IsRoot() );
    cursor.SearchNodeNameAndDeleteNode();             UT_EQ( A_CHAR("B")       , cursor.SearchName() ); UT_EQ( 2, cursor.Size() ); UT_EQ( 3, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_FALSE( cursor.IsRoot() );
    cursor.SearchNodeNameAndDeleteNode();             UT_EQ( A_CHAR("a")       , cursor.SearchName() ); UT_EQ( 2, cursor.Size() ); UT_EQ( 9, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_FALSE( cursor.IsRoot() );
    cursor.MoveToParent(); cursor.DeleteChild(A_CHAR("a"));   UT_EQ( NullString(), cursor.SearchName() ); UT_EQ( 1, cursor.Size() ); UT_EQ(14, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_TRUE ( cursor.IsRoot() );
    cursor.DeleteChildren();                          UT_EQ( NullString(), cursor.SearchName() ); UT_EQ( 0, cursor.Size() ); UT_EQ( 1, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_TRUE ( cursor.IsRoot() );
    cursor.SearchNodeNameAndDeleteNode();             UT_EQ( NullString(), cursor.SearchName() ); UT_EQ( 0, cursor.Size() ); UT_EQ( 1, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_TRUE ( cursor.IsRoot() );
    cursor.DeleteChildren();                          UT_EQ( NullString(), cursor.SearchName() ); UT_EQ( 0, cursor.Size() ); UT_EQ( 1, IteratorCount(cursor.SetIterator(iter, 0), pm.end() ) ); UT_TRUE ( cursor.IsRoot() );
}


//--------------------------------------------------------------------------------------------------
//--- StringTree_Walker
//--------------------------------------------------------------------------------------------------
template <typename TStartValue>
int doWalk( StringTree<AString,aworx::lib::strings::util::StringTreeConstKeyAllocator<AString>>::Walker& walker, TStartValue startValue )
{
    String& spaces= Spaces::Get();

    int cnt= 0;
    for ( walker.SetStart( startValue ); walker.IsValid() ; walker.Next() )
    {
        //cout << walker.GetPath();
        cout << walker.Depth() << ": ";
        cout << spaces.Substring( 0, 2 * walker.Depth() );
        cout << walker.Name();
        cout << " \""<< *walker << "\"";
        cout << " <"<< walker.GetPathToNode() << ">";
        cout << " #"<< walker.Size() << ">";
        cout << endl;
        ++cnt;
    }

    return cnt;
}

UT_METHOD(StringTree_Walker)
{
    UT_INIT();

    StringTree<AString, aworx::lib::strings::util::StringTreeConstKeyAllocator<AString>> pm;
    auto cursor=  pm.Root();                      (*cursor).Reset( "root" );

    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR(""               )); (*cursor).Reset( "root"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer"          )); (*cursor).Reset( "aDir"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/inner"    )); (*cursor).Reset( "inner"      );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/xinn1"    )); (*cursor).Reset( "inn1"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/inn2"     )); (*cursor).Reset( "inn2"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/Inn3"     )); (*cursor).Reset( "xinn3"      );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/inn4"     )); (*cursor).Reset( "inn4"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/inn5"     )); (*cursor).Reset( "inn5"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/inner"    )); (*cursor).Reset( "Overwritten");
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("dir2"           )); (*cursor).Reset( "dir2"       );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("dir2/subd2-a"   )); (*cursor).Reset( "subd2-a"    );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("dir2/subd2-b"   )); (*cursor).Reset( "subd2-b"    );

    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/Inn3/abc" )); (*cursor).Reset( "abc"        );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/Inn3/def" )); (*cursor).Reset( "def"        );
    (cursor= pm()).MoveToAndCreateNonExistingPart( A_CHAR("outer/Inn3/ght" )); (*cursor).Reset( "ght"        );

    decltype(pm)::Walker walker( &pm );

    // check cursor/iterator creation
    UT_TRUE( walker.GetCursor() == pm.Root() );

    decltype(pm)::StdIterator it( &pm );
    walker.SetIterator( it );
    UT_TRUE( it == pm.begin() );


    walker.PathGeneration( Switch::On );

    UT_PRINT(NewLine(), "--- non recursive ---" );
    walker.SetRecursionDepth( 1 );
    UT_EQ(  2,  doWalk( walker, &pm ) );

    UT_PRINT(NewLine(), "--- non recursive ---" );
    decltype(pm)::Cursor start=   pm.Root();
    UT_TRUE( start.MoveTo(A_CHAR("outer/Inn3")) );
    UT_EQ(  3,  doWalk( walker, start ) );

    UT_PRINT(NewLine(), "--- non recursive decending---" );
    walker.SortByPathname( SortOrder::Descending, Case::Sensitive );
    UT_EQ(  3,  doWalk( walker, start ) );
    walker.SortByPathname( SortOrder::Ascending, Case::Sensitive );


    UT_PRINT(NewLine(), "--- ascending ---" );
    walker.SetRecursionDepth( 99 );
    UT_EQ( 13,  doWalk( walker, &pm ) );

    UT_PRINT(NewLine(), "--- descending ---" );
    walker.SetRecursionDepth( -1 );
    walker.SortByPathname( SortOrder::Descending, Case::Sensitive );
    UT_EQ( 14,  doWalk( walker, &pm ) );

    UT_PRINT(NewLine(), "--- value ---" );
    walker.SortByValue( valueSorter );
    UT_EQ( 14,  doWalk( walker, &pm ) );

    UT_PRINT(NewLine(), "--- value ---" );
    walker.SortByValue( valueSorter );
    start=   pm.Root();
    UT_TRUE( start.MoveTo(A_CHAR("outer/Inn3")) );
    UT_EQ(  4,  doWalk( walker, start ) );

    walker.SetRecursionDepth( 1 );
    UT_EQ(  3,  doWalk( walker, start ) );


    UT_PRINT(NewLine(), "--- value ---" );
    walker.SetRecursionDepth( -1 );
    UT_TRUE( start.MoveTo(A_CHAR("abc")) );
    UT_EQ(  1,  doWalk( walker, start ) );

    walker.SetRecursionDepth( 5 );
    UT_EQ(  0,  doWalk( walker, start ) );

}

UT_CLASS_END

}; //namespace

#endif // !defined(ALIB_UT_SELECT) || defined(ALIB_UT_CORE)




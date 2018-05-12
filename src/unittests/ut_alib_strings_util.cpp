// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"

#if !defined (HPP_ALIB_STRINGS_FORMAT_SIMPLETEXT)
    #include "alib/strings/format/simpletext.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER)
    #include "alib/strings/util/wildcardmatcher.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
    #include "alib/strings/util/subsearch.hpp"
#endif

#define TESTCLASSNAME       CPP_ALib_Strings_Util
#include "aworx_unittests.hpp"


#include <iostream>
#include <iomanip>


using namespace std;
using namespace aworx;

namespace ut_aworx {

UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- SimpleText
//--------------------------------------------------------------------------------------------------
UT_METHOD( SimpleTextTests )
{
    UT_INIT();

    AString exp("");

    {
        SimpleText st;
        st.PushIndent(2);

        st.Text._(); st.Add( ""                  ); exp._()                                                                            ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\n"                ); exp._() << NewLine                                                                 ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\n\n"              ); exp._() << NewLine << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\r\n\n"            ); exp._() << NewLine << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\r\n\r\n"          ); exp._() << NewLine << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\n\r\n"            ); exp._() << NewLine << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add(     "123\n"         ); exp._() << "  123" << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add(     "123\r\n"       ); exp._() << "  123" << NewLine                                                      ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add(     "123\n4567"     ); exp._() << "  123" << NewLine << "  4567"   << NewLine                             ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add(     "123\r\n4567"   ); exp._() << "  123" << NewLine << "  4567"   << NewLine                             ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add(   "\n123\n4567"     ); exp._() << NewLine << "  123"<<NewLine<<"  4567"  << NewLine                       ; UT_EQ( exp, st.Text );
        st.Text._(); st.Add( "\n\n123\n\n4567"   ); exp._() << NewLine << NewLine<< "  123" << NewLine << NewLine<< "  4567" << NewLine; UT_EQ( exp, st.Text );
    }


    {
        SimpleText st;

        st.LineWidth= 4;
        st.Add("1 22 333" );
        st.Add("--" );
        st.LineWidth= 3;
        st.Add("22 1" );
        st.Add("--" );
        st.Add("1 1" );

        exp._() << "1 22"       << NewLine
                << "333"        << NewLine
                << "--"         << NewLine
                << "22"         << NewLine
                << "1"          << NewLine
                << "--"         << NewLine
                << "1 1"        << NewLine;
        UT_EQ( exp, st.Text );
    }

    {
        SimpleText st;
        st.Add("123456789 123<--------" );
        st.LineWidth= 11;
        st.PushIndent(5);
        st.Add("1 22 333 4444 55555 666666 55555 4444 333 22 1 22 333 22 1" );

        exp._() << "123456789 123<--------"             << NewLine
                << "     1 22"                          << NewLine
                << "     333"                           << NewLine
                << "     4444"                          << NewLine
                << "     55555"                         << NewLine
                << "     666666"                        << NewLine
                << "     55555"                         << NewLine
                << "     4444"                          << NewLine
                << "     333 22"                        << NewLine
                << "     1 22"                          << NewLine
                << "     333 22"                        << NewLine
                << "     1"                             << NewLine
                ;
        UT_EQ( exp, st.Text );
    }

    {
        SimpleText st;
        st.Add("123456789 12345<-------" );
        st.LineWidth= 15;
        st.PushIndent(5);
        st.Add("1 22 333 4444 55555 666666 55555 4444 333 22 1 22 333 22 1" );

        exp._() << "123456789 12345<-------"            << NewLine
                << "     1 22 333"                      << NewLine
                << "     4444 55555"                    << NewLine
                << "     666666"                        << NewLine
                << "     55555 4444"                    << NewLine
                << "     333 22 1"                      << NewLine
                << "     22 333 22"                     << NewLine
                << "     1"                             << NewLine
                ;
        UT_EQ( exp, st.Text );
    //    UT_TRUE( st.Text.StartsWith(exp) );
    }

    // indent
    {
        SimpleText st;
        //st.LineWidth= 15;
        st.PushIndent(ASTR("  "));  st.Text._();  st.Add( ""             );  st.PopIndent();  exp._() << ASTR(""                      );    UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("XY"));  st.Text._();  st.Add( ""             );  st.PopIndent();  exp._() << ASTR(""                      );    UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("  "));  st.Text._();  st.Add( "\n"           );  st.PopIndent();  exp._() << NewLine;                           UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("XY"));  st.Text._();  st.Add( "\n"           );  st.PopIndent();  exp._() << ASTR("XY") << NewLine;             UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("  "));  st.Text._();  st.Add( "\n1"          );  st.PopIndent();  exp._() << NewLine <<  ASTR("  1")<< NewLine; UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("  "));  st.Text._();  st.Add( "\n1"          );  st.PopIndent();  exp._() << NewLine <<  ASTR("  1")<< NewLine; UT_EQ( exp , st.Text );
        st.PushIndent(ASTR("XY"));  st.Text._();  st.Add( "\n1\n\n\n2"   );  st.PopIndent();  exp._() << ASTR("XY") << NewLine << ASTR("XY1")<< NewLine<< ASTR("XY")<< NewLine<< ASTR("XY")<< NewLine<< ASTR("XY2")<< NewLine;
                                                                                                                                            UT_EQ( exp , st.Text );
//        st.Indent._()._("XY");  st.Clear();  st.Add("z" );  UT_EQ( "XYz\n"          , st.Text );
    }

    const String& lorem= ASTR("Lorem ipsum dolor sit amet, consetetur.\n"
                              "Sadipscing elitr, sed diam nonumy eirmod tempor.");
    {
        SimpleText st;

        // unchanged
        st.Add(lorem );
        exp._() <<   "Lorem ipsum dolor sit amet, consetetur."           << NewLine
                <<   "Sadipscing elitr, sed diam nonumy eirmod tempor."  << NewLine;
        UT_EQ( exp, st.Text );
        st.Clear();


        st.PushIndent(2);
        st.LineWidth= 22;
        st.Add(lorem );
        //            123456789 123456789 123456789
        exp._() <<   "  Lorem ipsum dolor"       << NewLine
                <<   "  sit amet,"               << NewLine
                <<   "  consetetur."             << NewLine
                <<   "  Sadipscing elitr,"       << NewLine
                <<   "  sed diam nonumy"         << NewLine
                <<   "  eirmod tempor."          << NewLine
                ;
        UT_EQ( exp, st.Text );

        st.LineWidth= 29;
        st.Text._();
        st.Add(lorem );
        //            123456789 123456789 123456789
        exp._() <<   "  Lorem ipsum dolor sit amet,"    << NewLine
                <<   "  consetetur."                    << NewLine
                <<   "  Sadipscing elitr, sed diam"     << NewLine
                <<   "  nonumy eirmod tempor."          << NewLine
                ;
        UT_EQ( exp, st.Text );

        st.LineWidth= 32;
        st.Text._();
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "  Lorem ipsum dolor sit amet,"    << NewLine
                <<   "  consetetur."                    << NewLine
                <<   "  Sadipscing elitr, sed diam"     << NewLine
                <<   "  nonumy eirmod tempor."          << NewLine
                ;
        UT_EQ( exp, st.Text );
        st.PopIndent();


        st.LineWidth= 32;
        st.PushIndent( ASTR("* "), ASTR("  ") );
        st.Text._();
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "* Lorem ipsum dolor sit amet,"    << NewLine
                <<   "  consetetur."                    << NewLine
                <<   "  Sadipscing elitr, sed diam"     << NewLine
                <<   "  nonumy eirmod tempor."          << NewLine
                ;
        UT_EQ( exp, st.Text );
    //    UT_TRUE( st.Text.StartsWith(exp) );
        st.PopIndent();

    }


    //-------- full justification ("Blocksatz") -------
    {
        SimpleText st;

        st.JustifyChar= ' ';
        st.LineWidth= 10;
        st.Clear();  st.Add( ASTR("1 0123456789")       );  exp._() << "1"         <<NewLine<<"0123456789"<<NewLine ; UT_EQ( exp , st.Text );
        st.Clear();  st.Add( ASTR("1 1 0123456789")     );  exp._() << "1        1"<<NewLine<<"0123456789"<<NewLine ; UT_EQ( exp , st.Text );
        st.Clear();  st.Add( ASTR("1 1 1 0123456789")   );  exp._() << "1   1    1"<<NewLine<<"0123456789"<<NewLine ; UT_EQ( exp , st.Text );
        st.Clear();  st.Add( ASTR("1 1 11 0123456789")  );  exp._() << "1   1   11"<<NewLine<<"0123456789"<<NewLine ; UT_EQ( exp , st.Text );
        st.Clear();  st.Add( ASTR("1 1 111 0123456789") );  exp._() << "1  1   111"<<NewLine<<"0123456789"<<NewLine ; UT_EQ( exp , st.Text );

        st.LineWidth= 32;
        st.PushIndent(2);
        st.JustifyChar= ' ';
        st.Text._();
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "  Lorem ipsum  dolor  sit  amet,"  << NewLine
                <<   "  consetetur."                     << NewLine
                <<   "  Sadipscing  elitr,  sed   diam"  << NewLine
                <<   "  nonumy eirmod tempor."           << NewLine
                ;
        UT_EQ( exp, st.Text );
    //    UT_TRUE( st.Text.StartsWith(exp) );


        UT_PRINT("'Visual' Tests (no checking rules, last time we saw it, it worked :-)" )
        st.IndentFirstLine= nullptr;
        st.JustifyChar= ' ';
        st.LineWidth= 32;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12|--------\n",  st.Text,"\n" );
        st.LineWidth= 33;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123|-------\n",  st.Text,"\n" );
        st.LineWidth= 34;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 1234|------\n",  st.Text,"\n" );
        st.LineWidth= 35;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12345|-----\n",  st.Text,"\n" );
        st.LineWidth= 36;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456|----\n",  st.Text,"\n" );
        st.LineWidth= 37;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 1234567|---\n",  st.Text,"\n" );
        st.LineWidth= 38;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12345678|--\n",  st.Text,"\n" );
        st.LineWidth= 39;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789|-\n",  st.Text,"\n" );
        st.LineWidth= 40;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789 |\n",  st.Text,"\n" );
        st.LineWidth= 41;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789 1|\n", st.Text,"\n" );
    }


}

//--------------------------------------------------------------------------------------------------
//--- SimpleTextMarked
//--------------------------------------------------------------------------------------------------
UT_METHOD( SimpleTextMarked )
{
    UT_INIT();

    AString exp("");

    {
        SimpleText st;
        st.Clear().AddMarked("@*>Bullet1@Pxxx@P\nBullet2\n@<*Normal text" );
        UT_PRINT( st.Text );
        exp._() <<   "* Bullet1"     << NewLine
                <<   "  xxx"         << NewLine
                <<   ""              << NewLine
                <<   "* Bullet2"     << NewLine
                <<   "Normal text"   << NewLine;
        UT_EQ( exp, st.Text );


        st.Clear().AddMarked("@*>Outer1\n@*>Inner\nInner2@Pyyy\n@<*Outer2@Pmore\n@<*simple" );
        UT_PRINT( st.Text );

        exp._() <<   "* Outer1"      << NewLine
                <<   "  - Inner"     << NewLine
                <<   "  - Inner2"    << NewLine
                <<   "    yyy"       << NewLine
                <<   "* Outer2"      << NewLine
                <<   "  more"        << NewLine
                <<   "simple"        << NewLine;
        UT_EQ( exp, st.Text );


        bool caught= false;
        try
        {
            st.Clear().AddMarked("This is a maker text with an unknown marker: @<>This is where the exception is raised." );
        }
        catch(Exception e)
        {
            caught= true;
            UT_EQ( Enum(aworx::lib::strings::format::Exceptions::UnknownMarker), e.Code() );
            SimpleText efmt;
            efmt.AddException( e );
            UT_PRINT( efmt.Text );
        }
        UT_TRUE( caught );



        caught= false;
        try
        {
            st.Clear().AddMarked("@*>Outer1\n@<*OK, level 0.\n@<*This is where\nthe exception\nis raised." );
        }
        catch(Exception e)
        {
            caught= true;
            UT_EQ( Enum(aworx::lib::strings::format::Exceptions::EndmarkerWithoutStart), e.Code() );
            SimpleText efmt;
            efmt.AddException( e );
            UT_PRINT( efmt.Text );
        }
        UT_TRUE( caught );



        caught= false;
        try
        {
            st.Clear().AddMarked("We must not unindent @<< prior to indent." );
        }
        catch(Exception e)
        {
            caught= true;
            UT_EQ( Enum(aworx::lib::strings::format::Exceptions::EndmarkerWithoutStart), e.Code() );
            SimpleText efmt;
            efmt.AddException( e );
            UT_PRINT( efmt.Text );
        }
        UT_TRUE( caught );

    }
}

//--------------------------------------------------------------------------------------------------
//--- WildcardMatcher
//--------------------------------------------------------------------------------------------------
WildcardMatcher wildcardMatcher;
void TestMatcher(AWorxUnitTesting& ut, const String& haystack, const String& pattern, bool result )
{
    wildcardMatcher.Compile(pattern);
    UT_EQ( result, wildcardMatcher.Match( haystack ) );
}

UT_METHOD( TestWildcardMatcher )
{
    UT_INIT();

    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.conf")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("ABC.conf")       , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.c*")         , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.c?*")        , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.c?")         , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.c??")        , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.?c??")       , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("abc.?o??")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.???")          , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.????")         , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.????")         , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.?*???")        , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.?**?*?*?")     , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*.?**??*?*?")    , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("?b?.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a?c.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("ab?.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("?b?.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("??c.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("???.*")          , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("???*")           , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("????????")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("?????????")      , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("??????????")     , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("ab???????")      , false   );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("ab??????")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("*bc.conf")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a*bc.conf")      , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a*c.conf")       , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a*c*")           , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a*o*")           , true    );
    TestMatcher( ut, ASTR("abc.conf"), ASTR("a*x*")           , false   );

    // quick test for case insenstive matching:
    WildcardMatcher wcm(ASTR("*bc.c*")  );
    UT_EQ( true , wcm.Match( ASTR("abc.conf") , Case::Ignore ) );
    UT_EQ( true , wcm.Match( ASTR("abC.conf") , Case::Ignore ) );
    UT_EQ( true , wcm.Match( ASTR("ABC.CONF") , Case::Ignore ) );
    UT_EQ( false, wcm.Match( ASTR("ABx.CONF") , Case::Ignore ) );
}

//--------------------------------------------------------------------------------------------------
//--- SubstringSearch
//--------------------------------------------------------------------------------------------------

UT_METHOD( TestSubstringSearch )
{

    UT_INIT();
    String haystack=  ASTR("Virgin of the Rocks is a title given to two paintings by Leonardo da Vinci");

    {
        SubstringSearch<Case::Sensitive> substringSearch( ASTR("of") );

        UT_EQ(  7, substringSearch.Search( haystack,  -1 ) );
        UT_EQ(  7, substringSearch.Search( haystack,   0 ) );
        UT_EQ(  7, substringSearch.Search( haystack,   1 ) );
        UT_EQ(  7, substringSearch.Search( haystack,   7 ) );
        UT_EQ( -1, substringSearch.Search( haystack,   8 ) );
        UT_EQ( -1, substringSearch.Search( haystack, 100 ) );
    }

    {
        SubstringSearch<Case::Sensitive> substringSearch( ASTR("Vi") );

        UT_EQ(  0, substringSearch.Search( haystack,  -1 ) );
        UT_EQ(  0, substringSearch.Search( haystack,   0 ) );
        UT_EQ( 69, substringSearch.Search( haystack,   1 ) );
        UT_EQ( 69, substringSearch.Search( haystack,  69 ) );
        UT_EQ( -1, substringSearch.Search( haystack,  70 ) );
        UT_EQ( -1, substringSearch.Search( haystack, 100 ) );
    }

    {
        SubstringSearch<Case::Sensitive> substringSearch( ASTR("VI") );

        UT_EQ( -1, substringSearch.Search( haystack ) );
    }

    {
        SubstringSearch<Case::Ignore   > substringSearch( ASTR("VI") );

        UT_EQ( -1, substringSearch.Search( haystack ) );
    }

    {
        SubstringSearch<Case::Ignore   > reused( ASTR("Rocks") );       UT_EQ( 14, reused.Search( haystack ) );
                                  reused.Compile( ASTR("is") );         UT_EQ( 20, reused.Search( haystack ) );
                                  reused.Compile( ASTR("title") );      UT_EQ( 25, reused.Search( haystack ) );
                                  reused.Compile( ASTR("paintings") );  UT_EQ( 44, reused.Search( haystack ) );
    }

}


UT_CLASS_END

} //namespace

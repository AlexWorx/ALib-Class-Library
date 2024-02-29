// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_STRINGS

#if defined(QT_CORE_LIB) // needed here for unity builds to be included before boxing (!)
#   include "alib/compatibility/qt_characters.hpp"
#endif

#include "alib/alox.hpp"

#if !defined (HPP_ALIB_LANG_FORMAT_PARAGRAPHS)
    #include "alib/lang/format/paragraphs.hpp"
#endif
#if !defined(HPP_ALIB_LANG_FORMAT_EXCEPTIONS)
#   include "alib/lang/format/fmtexceptions.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_WILDCARDMATCHER)
    #include "alib/strings/util/wildcardmatcher.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_SUBSEARCH)
    #include "alib/strings/util/subsearch.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_UTIL_TOKEN)
    #include "alib/strings/util/token.hpp"
#endif

#define TESTCLASSNAME       CPP_ALib_Strings_Util
#include "unittests/aworx_unittests.hpp"


#include <iostream>
#include <iomanip>


using namespace std;
using namespace alib;

namespace ut_aworx {

//--------------------------------------------------------------------------------------------------
//--- test function for class Token
//--------------------------------------------------------------------------------------------------
void testTokenDef(AWorxUnitTesting& ut, const NString& def, int8_t formatOrError, lang::Case sensitivity,
                  int8_t minLength1, int8_t minLength2= -1,
                  int8_t minLength3= -1, int8_t minLength4= -1, int8_t minLength5= -1,
                  int8_t minLength6= -1, int8_t minLength7= -1                                 )

{
    Token token;
    ALIB_STRINGS_FROM_NARROW( def, defCv, 1024)

    token.Define( defCv );

#if ALIB_DEBUG
    Token::DbgDefinitionError err= token.DbgGetError();
    if( err != Token::DbgDefinitionError::OK )
    {
        UT_EQ( formatOrError  ,  int8_t(err)   )
        return;
    }
#endif
    UT_EQ( formatOrError  , int8_t(token.GetFormat())    )
    UT_EQ( sensitivity    , token.Sensitivity()          )

    UT_EQ( minLength1     , token.GetMinLength(0)  )
    UT_EQ( minLength2     , token.GetMinLength(1)  ) if( minLength2 == -1 ) return;
    UT_EQ( minLength3     , token.GetMinLength(2)  ) if( minLength3 == -1 ) return;
    UT_EQ( minLength4     , token.GetMinLength(3)  ) if( minLength4 == -1 ) return;
    UT_EQ( minLength5     , token.GetMinLength(4)  ) if( minLength5 == -1 ) return;
    UT_EQ( minLength6     , token.GetMinLength(5)  ) if( minLength6 == -1 ) return;
    UT_EQ( minLength7     , token.GetMinLength(6)  ) if( minLength7 == -1 ) return;
}


UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Paragraphs
//--------------------------------------------------------------------------------------------------
UT_METHOD( TextTests )
{
    UT_INIT()

    AString exp("");

    {
        Paragraphs st;
        st.PushIndent(2);

        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( ""                  ); UT_EQ( exp._()                                                                                      , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\n"                ); UT_EQ( exp._() << NewLine()                                                                         , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\n\n"              ); UT_EQ( exp._() << NewLine() << NewLine()                                                            , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\r\n\n"            ); UT_EQ( exp._() << NewLine() << NewLine()                                                            , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\r\n\r\n"          ); UT_EQ( exp._() << NewLine() << NewLine()                                                            , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\n\r\n"            ); UT_EQ( exp._() << NewLine() << NewLine()                                                            , st.Buffer ) UT_EQ(0, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add(     "123\n"         ); UT_EQ( exp._() << "  123"   << NewLine()                                                            , st.Buffer ) UT_EQ(5, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add(     "123\r\n"       ); UT_EQ( exp._() << "  123"   << NewLine()                                                            , st.Buffer ) UT_EQ(5, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add(     "123\n4567"     ); UT_EQ( exp._() << "  123"   << NewLine() << "  4567"   << NewLine()                                 , st.Buffer ) UT_EQ(6, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add(     "123\r\n4567"   ); UT_EQ( exp._() << "  123"   << NewLine() << "  4567"   << NewLine()                                 , st.Buffer ) UT_EQ(6, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add(   "\n123\n4567"     ); UT_EQ( exp._() << NewLine() << "  123"<<NewLine()<<"  4567"  << NewLine()                           , st.Buffer ) UT_EQ(6, st.DetectedMaxLineWidth)
        st.DetectedMaxLineWidth= 0; st.Buffer._(); st.Add( "\n\n123\n\n4567"   ); UT_EQ( exp._() << NewLine() << NewLine()<< "  123" << NewLine() << NewLine()<< "  4567" << NewLine(), st.Buffer ) UT_EQ(6, st.DetectedMaxLineWidth)
    }


    {
        Paragraphs st;

        st.LineWidth= 4;
        st.Add("1 22 333" );
        st.Add("--" );
        st.LineWidth= 3;
        st.Add("22 1" );
        st.Add("--" );
        st.Add("1 1" );

        exp._() << "1 22"       << NewLine()
                << "333"        << NewLine()
                << "--"         << NewLine()
                << "22"         << NewLine()
                << "1"          << NewLine()
                << "--"         << NewLine()
                << "1 1"        << NewLine();
        UT_EQ( exp, st.Buffer )
    }

    {
        Paragraphs st;
        st.Add("123456789 123<--------" );
        st.LineWidth= 11;
        st.PushIndent(5);
        st.Add("1 22 333 4444 55555 666666 55555 4444 333 22 1 22 333 22 1" );

        exp._() << "123456789 123<--------"             << NewLine()
                << "     1 22"                          << NewLine()
                << "     333"                           << NewLine()
                << "     4444"                          << NewLine()
                << "     55555"                         << NewLine()
                << "     666666"                        << NewLine()
                << "     55555"                         << NewLine()
                << "     4444"                          << NewLine()
                << "     333 22"                        << NewLine()
                << "     1 22"                          << NewLine()
                << "     333 22"                        << NewLine()
                << "     1"                             << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
    }

    {
        Paragraphs st;
        st.Add("123456789 12345<-------" );
        st.LineWidth= 15;
        st.PushIndent(5);
        st.Add("1 22 333 4444 55555 666666 55555 4444 333 22 1 22 333 22 1" );

        exp._() << "123456789 12345<-------"            << NewLine()
                << "     1 22 333"                      << NewLine()
                << "     4444 55555"                    << NewLine()
                << "     666666"                        << NewLine()
                << "     55555 4444"                    << NewLine()
                << "     333 22 1"                      << NewLine()
                << "     22 333 22"                     << NewLine()
                << "     1"                             << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
    //    UT_TRUE( st.Paragraphs.StartsWith(exp) );
    }

    // indent
    {
        Paragraphs st;
        //st.LineWidth= 15;
        st.PushIndent(A_CHAR("  "));  st.Buffer._();  st.Add( ""             );  st.PopIndent();  exp._() << A_CHAR(""                      );        UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("XY"));  st.Buffer._();  st.Add( ""             );  st.PopIndent();  exp._() << A_CHAR(""                      );        UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("  "));  st.Buffer._();  st.Add( "\n"           );  st.PopIndent();  exp._() << NewLine();                               UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("XY"));  st.Buffer._();  st.Add( "\n"           );  st.PopIndent();  exp._() << A_CHAR("XY") << NewLine();               UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("  "));  st.Buffer._();  st.Add( "\n1"          );  st.PopIndent();  exp._() << NewLine() <<  A_CHAR("  1")<< NewLine(); UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("  "));  st.Buffer._();  st.Add( "\n1"          );  st.PopIndent();  exp._() << NewLine() <<  A_CHAR("  1")<< NewLine(); UT_EQ( exp , st.Buffer )
        st.PushIndent(A_CHAR("XY"));  st.Buffer._();  st.Add( "\n1\n\n\n2"   );  st.PopIndent();  exp._() << A_CHAR("XY") << NewLine() << A_CHAR("XY1")<< NewLine()<< A_CHAR("XY")<< NewLine()<< A_CHAR("XY")<< NewLine()<< A_CHAR("XY2")<< NewLine();
                                                                                                                                                      UT_EQ( exp , st.Buffer )
//        st.Indent.Reset("XY");  st.Clear();  st.Add("z" );  UT_EQ( "XYz\n"          , st.Paragraphs );
    }

    const String& lorem= A_CHAR("Lorem ipsum dolor sit amet, consetetur.\n"
                              "Sadipscing elitr, sed diam nonumy eirmod tempor.");
    {
        Paragraphs st;

        // unchanged
        st.Add(lorem );
        exp._() <<   "Lorem ipsum dolor sit amet, consetetur."           << NewLine()
                <<   "Sadipscing elitr, sed diam nonumy eirmod tempor."  << NewLine();
        UT_EQ( exp, st.Buffer )
        UT_EQ( 48, st.DetectedMaxLineWidth )

        st.Clear();
        st.PushIndent(2);
        st.LineWidth= 22;
        st.Add(lorem );
        //            123456789 123456789 123456789
        exp._() <<   "  Lorem ipsum dolor"       << NewLine()
                <<   "  sit amet,"               << NewLine()
                <<   "  consetetur."             << NewLine()
                <<   "  Sadipscing elitr,"       << NewLine()
                <<   "  sed diam nonumy"         << NewLine()
                <<   "  eirmod tempor."          << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
        UT_EQ( 19, st.DetectedMaxLineWidth )

        st.Clear();
        st.PushIndent(2);
        st.LineWidth= 29;
        st.Add(lorem );
        //            123456789 123456789 123456789
        exp._() <<   "  Lorem ipsum dolor sit amet,"    << NewLine()
                <<   "  consetetur."                    << NewLine()
                <<   "  Sadipscing elitr, sed diam"     << NewLine()
                <<   "  nonumy eirmod tempor."          << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
        UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )

        st.Clear();
        st.PushIndent(2);
        st.LineWidth= 32;
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "  Lorem ipsum dolor sit amet,"    << NewLine()
                <<   "  consetetur."                    << NewLine()
                <<   "  Sadipscing elitr, sed diam"     << NewLine()
                <<   "  nonumy eirmod tempor."          << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
        UT_EQ( 29, st.DetectedMaxLineWidth )
        st.PopIndent();

        st.Clear();
        st.LineWidth= 32;
        st.PushIndent( A_CHAR("* "), A_CHAR("  ") );
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "* Lorem ipsum dolor sit amet,"    << NewLine()
                <<   "  consetetur."                    << NewLine()
                <<   "  Sadipscing elitr, sed diam"     << NewLine()
                <<   "  nonumy eirmod tempor."          << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
        UT_EQ( 29, st.DetectedMaxLineWidth )
        st.PopIndent();

    }


    //-------- full justification ("Blocksatz") -------
    {
        Paragraphs st;

        st.JustifyChar= ' ';
        st.LineWidth= 10;
        st.Clear();  st.Add( A_CHAR("1 0123456789")       );  exp._() << "1"         <<NewLine()<<"0123456789"<<NewLine() ; UT_EQ( exp , st.Buffer )
        st.Clear();  st.Add( A_CHAR("1 1 0123456789")     );  exp._() << "1        1"<<NewLine()<<"0123456789"<<NewLine() ; UT_EQ( exp , st.Buffer )
        st.Clear();  st.Add( A_CHAR("1 1 1 0123456789")   );  exp._() << "1   1    1"<<NewLine()<<"0123456789"<<NewLine() ; UT_EQ( exp , st.Buffer )
        st.Clear();  st.Add( A_CHAR("1 1 11 0123456789")  );  exp._() << "1   1   11"<<NewLine()<<"0123456789"<<NewLine() ; UT_EQ( exp , st.Buffer )
        st.Clear();  st.Add( A_CHAR("1 1 111 0123456789") );  exp._() << "1  1   111"<<NewLine()<<"0123456789"<<NewLine() ; UT_EQ( exp , st.Buffer )

        st.LineWidth= 32;
        st.PushIndent(2);
        st.JustifyChar= ' ';
        st.Buffer._();
        st.Add(lorem );
        //            123456789 123456789 123456789 12
        exp._() <<   "  Lorem ipsum  dolor  sit  amet,"  << NewLine()
                <<   "  consetetur."                     << NewLine()
                <<   "  Sadipscing  elitr,  sed   diam"  << NewLine()
                <<   "  nonumy eirmod tempor."           << NewLine()
                ;
        UT_EQ( exp, st.Buffer )
    //    UT_TRUE( st.Buffer.StartsWith(exp) );


        UT_PRINT("'Visual' Tests (no checking rules, last time we saw it, it worked :-)" )
        st.IndentFirstLine= nullptr;
        st.JustifyChar= ' ';
        st.LineWidth= 32;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12|--------\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 33;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123|-------\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 34;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 1234|------\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 35;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12345|-----\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 36;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456|----\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 37;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 1234567|---\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 38;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 12345678|--\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 39;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789|-\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 40;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789 |\n",  st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
        st.LineWidth= 41;  st.Clear();   st.Add( lorem );  UT_PRINT("123456789 123456789 123456789 123456789 1|\n", st.Buffer,"\n" )  UT_EQ( st.LineWidth, st.DetectedMaxLineWidth )
    }


}

//--------------------------------------------------------------------------------------------------
//--- TextMarked
//--------------------------------------------------------------------------------------------------
UT_METHOD( TextMarked )
{
    UT_INIT()

    AString exp("");

    {
        Paragraphs st;
        st.Clear().AddMarked("@*>Bullet1@Pxxx@P\nBullet2\n@<*Normal text" );
        UT_PRINT( st.Buffer )
        exp._() <<   "* Bullet1"     << NewLine()
                <<   "  xxx"         << NewLine()
                <<   ""              << NewLine()
                <<   "* Bullet2"     << NewLine()
                <<   "Normal text"   << NewLine();
        UT_EQ( exp, st.Buffer )


        st.Clear().AddMarked("@*>Outer1\n@*>Inner\nInner2@Pyyy\n@<*Outer2@Pmore\n@<*simple" );
        UT_PRINT( st.Buffer )

        exp._() <<   "* Outer1"      << NewLine()
                <<   "  - Inner"     << NewLine()
                <<   "  - Inner2"    << NewLine()
                <<   "    yyy"       << NewLine()
                <<   "* Outer2"      << NewLine()
                <<   "  more"        << NewLine()
                <<   "simple"        << NewLine();
        UT_EQ( exp, st.Buffer )


        bool caught= false;
        try
        {
            st.Clear().AddMarked("This is a maker text with an unknown marker: @<>This is where the exception is raised." );
        }
        catch(Exception& e)
        {
            caught= true;
            UT_TRUE(  e.Type() == alib::lang::format::FMTExceptions::UnknownMarker )
            UT_PRINT( e.Format() )
        }
        UT_TRUE( caught )



        caught= false;
        try
        {
            st.Clear().AddMarked("@*>Outer1\n@<*OK, level 0.\n@<*This is where\nthe exception\nis raised." );
        }
        catch(Exception& e)
        {
            caught= true;
            UT_TRUE(  e.Type() == alib::lang::format::FMTExceptions::EndmarkerWithoutStart )
            UT_PRINT( e.Format() )
        }
        UT_TRUE( caught )



        caught= false;
        try
        {
            st.Clear().AddMarked("We must not unindent @<< prior to indent." );
        }
        catch(Exception& e)
        {
            caught= true;
            UT_TRUE(  e.Type() == alib::lang::format::FMTExceptions::EndmarkerWithoutStart )
            UT_PRINT( e.Format() )
        }
        UT_TRUE( caught )

    }
}

//--------------------------------------------------------------------------------------------------
//--- WildcardMatcher
//--------------------------------------------------------------------------------------------------
WildcardMatcher wildcardMatcher;
void TestMatcher(AWorxUnitTesting& ut, const String& haystack, const String& pattern, bool result )
{
    wildcardMatcher.Compile(pattern);
    UT_EQ( result, wildcardMatcher.Match( haystack ) )
}

UT_METHOD( TestWildcardMatcher )
{
    UT_INIT()

    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.conf")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("ABC.conf")       , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.c*")         , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.c?*")        , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.c?")         , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.c??")        , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.?c??")       , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("abc.?o??")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.???")          , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.????")         , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.????")         , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.?*???")        , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.?**?*?*?")     , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*.?**??*?*?")    , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("?b?.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a?c.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("ab?.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("?b?.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("??c.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("???.*")          , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("???*")           , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("????????")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("?????????")      , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("??????????")     , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("ab???????")      , false   );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("ab??????")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("*bc.conf")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a*bc.conf")      , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a*c.conf")       , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a*c*")           , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a*o*")           , true    );
    TestMatcher( ut, A_CHAR("abc.conf"), A_CHAR("a*x*")           , false   );

    // quick test for case insenstive matching:
    WildcardMatcher wcm(A_CHAR("*bc.c*")  );
    UT_EQ( true , wcm.Match( A_CHAR("abc.conf") , lang::Case::Ignore ) )
    UT_EQ( true , wcm.Match( A_CHAR("abC.conf") , lang::Case::Ignore ) )
    UT_EQ( true , wcm.Match( A_CHAR("ABC.CONF") , lang::Case::Ignore ) )
    UT_EQ( false, wcm.Match( A_CHAR("ABx.CONF") , lang::Case::Ignore ) )
}

//--------------------------------------------------------------------------------------------------
//--- SubstringSearch
//--------------------------------------------------------------------------------------------------
UT_METHOD( TestSubstringSearch )
{

    UT_INIT()
    String haystack=  A_CHAR("Virgin of the Rocks is a title given to two paintings by Leonardo da Vinci");

    {
        SubstringSearch<lang::Case::Sensitive> substringSearch( A_CHAR("of") );

        UT_EQ(  7, substringSearch.Search( haystack,  -1 ) )
        UT_EQ(  7, substringSearch.Search( haystack,   0 ) )
        UT_EQ(  7, substringSearch.Search( haystack,   1 ) )
        UT_EQ(  7, substringSearch.Search( haystack,   7 ) )
        UT_EQ( -1, substringSearch.Search( haystack,   8 ) )
        UT_EQ( -1, substringSearch.Search( haystack, 100 ) )
    }

    {
        SubstringSearch<lang::Case::Sensitive> substringSearch( A_CHAR("Vi") );

        UT_EQ(  0, substringSearch.Search( haystack,  -1 ) )
        UT_EQ(  0, substringSearch.Search( haystack,   0 ) )
        UT_EQ( 69, substringSearch.Search( haystack,   1 ) )
        UT_EQ( 69, substringSearch.Search( haystack,  69 ) )
        UT_EQ( -1, substringSearch.Search( haystack,  70 ) )
        UT_EQ( -1, substringSearch.Search( haystack, 100 ) )
    }

    {
        SubstringSearch<lang::Case::Sensitive> substringSearch( A_CHAR("VI") );

        UT_EQ( -1, substringSearch.Search( haystack ) )
    }

    {
        SubstringSearch<lang::Case::Ignore   > substringSearch( A_CHAR("VI") );

        UT_EQ( -1, substringSearch.Search( haystack ) )
    }

    {
        SubstringSearch<lang::Case::Ignore   > reused( A_CHAR("Rocks") );       UT_EQ( 14, reused.Search( haystack ) )
                                        reused.Compile( A_CHAR("is") );         UT_EQ( 20, reused.Search( haystack ) )
                                        reused.Compile( A_CHAR("title") );      UT_EQ( 25, reused.Search( haystack ) )
                                        reused.Compile( A_CHAR("paintings") );  UT_EQ( 44, reused.Search( haystack ) )
    }
}

//--------------------------------------------------------------------------------------------------
//--- TokenParse
//--------------------------------------------------------------------------------------------------
UT_METHOD( TokenParse )
{
    UT_INIT()


    #define FORM(val) int8_t(Token::Formats::val)
    #if ALIB_DEBUG
    #   define ERR( val) int8_t(Token::DbgDefinitionError::val)
    #endif

    //-------------------------- simple token construction tests ----------------------------------
    Token token;
    {
        token= Token( A_CHAR("Test") , lang::Case::Sensitive, 4 );
        UT_EQ( Token::Formats::Normal          , token.GetFormat()    )
        UT_EQ( lang::Case::Sensitive           , token.Sensitivity()  )
        UT_EQ( 4                               , token.GetMinLength(0))

#if ALIB_DEBUG
        token= Token( A_CHAR("Test") , lang::Case::Sensitive, 0 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("Test") , lang::Case::Sensitive, 5 );
        UT_EQ( ERR(MinLenExceedsSegmentLength)         , int8_t(token.DbgGetError())    )
#endif

        token= Token( A_CHAR("TestAbc") , lang::Case::Ignore , 4, 3 );
        UT_EQ( Token::Formats::CamelCase       , token.GetFormat()    )
        UT_EQ( lang::Case::Ignore              , token.Sensitivity()  )
        UT_EQ( 4                               , token.GetMinLength(0))
        UT_EQ( 3                               , token.GetMinLength(1))

#if ALIB_DEBUG
        token= Token( A_CHAR("Test") , lang::Case::Sensitive, 4, 3 );
        UT_EQ( ERR(NoCaseSchemeFound)         , int8_t(token.DbgGetError())    )

        token= Token( A_CHAR("TestAbc") , lang::Case::Sensitive, 0, 1 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("TestAbc") , lang::Case::Sensitive, 5, 3 );
        UT_EQ( ERR(MinLenExceedsSegmentLength)         , int8_t(token.DbgGetError())    )
#endif

        token= Token( A_CHAR("Test_Abc") , lang::Case::Ignore , 4, 3 );
        UT_EQ( Token::Formats::SnakeCase       , token.GetFormat()    )
        UT_EQ( lang::Case::Ignore              , token.Sensitivity()  )
        UT_EQ( 4                               , token.GetMinLength(0))
        UT_EQ( 3                               , token.GetMinLength(1))

#if ALIB_DEBUG
        token= Token( A_CHAR("Test_Abc") , lang::Case::Sensitive, 0, 1 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("Test_Abc") , lang::Case::Sensitive, 1, 0 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("Test_Abc") , lang::Case::Sensitive, 5, 3 );
        UT_EQ( ERR(MinLenExceedsSegmentLength)         , int8_t(token.DbgGetError())    )
#endif

        token= Token( A_CHAR("Test-Abc") , lang::Case::Ignore , 4, 3 );
        UT_EQ( Token::Formats::KebabCase       , token.GetFormat()    )
        UT_EQ(lang:: Case::Ignore                    , token.Sensitivity()  )
        UT_EQ( 4                               , token.GetMinLength(0))
        UT_EQ( 3                               , token.GetMinLength(1))

#if ALIB_DEBUG
        token= Token( A_CHAR("Test-Abc") , lang::Case::Sensitive, 0, 1 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("Test-Abc") , lang::Case::Sensitive, 1, 0 );
        UT_EQ( ERR(ZeroMinLengthAndNotLastCamelHump)   , int8_t(token.DbgGetError())    )
        token= Token( A_CHAR("Test-Abc") , lang::Case::Sensitive, 5, 3 );
        UT_EQ( ERR(MinLenExceedsSegmentLength)         , int8_t(token.DbgGetError())    )
#endif
    }

    //-------------------------- token definition tests ----------------------------------
#if ALIB_DEBUG
    testTokenDef(ut, ""                                      , ERR(EmptyName)                 ,lang:: Case::Sensitive , 0 );
#endif
    testTokenDef(ut, "Test"                                  , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
#if ALIB_DEBUG
    testTokenDef(ut, ""       ";i"                           , ERR(EmptyName)                 ,lang:: Case::Sensitive , 0 );
    testTokenDef(ut, ""       ";X"                           , ERR(EmptyName)                 ,lang:: Case::Sensitive , 0 );
#endif
    testTokenDef(ut, "Test"   ";i"                           , FORM(Normal)                   ,lang:: Case::Ignore    , 4 );
    testTokenDef(ut, "Test"   ";ign"                         , FORM(Normal)                   ,lang:: Case::Ignore    , 4 );
    testTokenDef(ut, "Test"   ";IGNORE"                      , FORM(Normal)                   ,lang:: Case::Ignore    , 4 );
    testTokenDef(ut, "Test"   ";s"                           , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
    testTokenDef(ut, "Test"   ";sens"                        , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
    testTokenDef(ut, "Test"   ";SEnsiTIVE"                   , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
#if ALIB_DEBUG
    testTokenDef(ut, "Test"   ";nonsense"                    , ERR(ErrorReadingSensitivity)   ,lang:: Case::Sensitive , 4 );
#endif
#if ALIB_DEBUG
    testTokenDef(ut, "Test"   ";S;0"                         , ERR(ZeroMinLengthAndNotLastCamelHump),lang:: Case::Sensitive , 4 );
#endif
    testTokenDef(ut, "Test"   ";S;4"                         , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
    testTokenDef(ut, "Test"   ";S;1"                         , FORM(Normal)                   ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test"   ";S;4"                         , FORM(Normal)                   ,lang:: Case::Sensitive , 4 );
#if ALIB_DEBUG
    testTokenDef(ut, "Test"   ";S;5"                         , ERR(MinLenExceedsSegmentLength) ,lang:: Case::Sensitive , 0 );
#endif

    testTokenDef(ut, "Test"   ";S;3"                         , FORM(Normal)                   ,lang:: Case::Sensitive , 3 );
#if ALIB_DEBUG
    testTokenDef(ut, "Test"   ";S;1;2"                       , ERR(NoCaseSchemeFound)         ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "Test"   ";S;1;2;3"                     , ERR(NoCaseSchemeFound)         ,lang:: Case::Sensitive , 1,2,3 );
#endif


    testTokenDef(ut, "TeSt"            ";S;1;2"              , FORM(CamelCase)                ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "TeStTeStTeSt"    ";S;1;2;1;2;1;2"      , FORM(CamelCase)                ,lang:: Case::Sensitive , 1,2,1,2,1,2 );
#if ALIB_DEBUG
    testTokenDef(ut, "TeStTeStTeSt"    ";S;1;0;1;2;1;2;1"    , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2,1,2,1,2,1 );
    testTokenDef(ut, "TeStTeStTeSt"    ";S;1;2;1;2;1;2;1"    , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2,1,2,1,2,1 );
    testTokenDef(ut, "Test"            ";S;5"                , ERR(MinLenExceedsSegmentLength),lang:: Case::Sensitive , 0 );
    testTokenDef(ut, "Test"            ";S;1;2"              , ERR(NoCaseSchemeFound)         ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "TeSt"            ";S;1;2;1"            , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "TeSt"            ";S;3;2"              , ERR(MinLenExceedsSegmentLength),lang:: Case::Sensitive , 1,2 );

    testTokenDef(ut, "Te_st_te_st_te_St" ";S;1;2;1;2;1;2;1"  , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2,1,2,1,2,1 );
    testTokenDef(ut, "Te_st"            ";S;1;2;1"           , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "Te_st"            ";S;3;2"             , ERR(MinLenExceedsSegmentLength),lang:: Case::Sensitive , 1,2 );

    testTokenDef(ut, "Te-st-te-st-te-St" ";S;1;2;1;2;1;2;1"  , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2,1,2,1,2,1 );
    testTokenDef(ut, "Te-st"            ";S;1;2;1"           , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1,2 );
    testTokenDef(ut, "Te-st"            ";S;3;2"             , ERR(MinLenExceedsSegmentLength),lang:: Case::Sensitive , 1,2 );
#endif
    testTokenDef(ut, "TeSt"            ";S;2;1"              , FORM(CamelCase)                ,lang:: Case::Sensitive , 2,1 );
    testTokenDef(ut, "Te_st"           ";S;2;1"              , FORM(SnakeCase)                ,lang:: Case::Sensitive , 2,1 );
    testTokenDef(ut, "Te-st"           ";S;2;1"              , FORM(KebabCase)                ,lang:: Case::Sensitive , 2,1 );
    testTokenDef(ut, "TeSt"            ";S;2"                , FORM(Normal)                   ,lang:: Case::Sensitive , 2   );
    testTokenDef(ut, "Te_st"           ";S;2"                , FORM(Normal)                   ,lang:: Case::Sensitive , 2   );
    testTokenDef(ut, "Te-st"           ";S;2"                , FORM(Normal)                   ,lang:: Case::Sensitive , 2   );

    testTokenDef(ut, "TeStTeStTeStTe"  ";S;1;2;1;2;1;2;1"    , FORM(CamelCase)                ,lang:: Case::Sensitive , 1,2,1,2,1,2,1 );
#if ALIB_DEBUG
    testTokenDef(ut, "TeStTeStTeSt"    ";S;1;2;3;4;5;6;7;8"  , ERR(TooManyMinLengthsGiven)    ,lang:: Case::Sensitive , 0 );
    testTokenDef(ut, "CamelHumpHump"   ";S;1;0;1"            , ERR(ZeroMinLengthAndNotLastCamelHump),lang:: Case::Sensitive , 4 );
#endif

    testTokenDef(ut, "TestCamel"       ";S;1;2"              , FORM(CamelCase)                ,lang:: Case::Sensitive , 1, 2 );
    testTokenDef(ut, "Test_Snake"      ";S;1;2"              , FORM(SnakeCase)                ,lang:: Case::Sensitive , 1, 2 );
    testTokenDef(ut, "Test-Kebab"      ";S;1;2"              , FORM(KebabCase)                ,lang:: Case::Sensitive , 1, 2 );
    testTokenDef(ut, "Test_Sna-ke"     ";S;1;2"              , FORM(SnakeCase)                ,lang:: Case::Sensitive , 1, 2 );
    testTokenDef(ut, "Test-Sna_ke"     ";S;1;2"              , FORM(SnakeCase)                ,lang:: Case::Sensitive , 1, 2 );

    testTokenDef(ut, "TestCamel"       ";Ign;1;2"            , FORM(CamelCase)                ,lang:: Case::Ignore    , 1, 2 );
    testTokenDef(ut, "Test_Snake"      ";Ign;1;2"            , FORM(SnakeCase)                ,lang:: Case::Ignore    , 1, 2 );
    testTokenDef(ut, "Test-Kebab"      ";Ign;1;2"            , FORM(KebabCase)                ,lang:: Case::Ignore    , 1, 2 );
    testTokenDef(ut, "Test_Sna-ke"     ";Ign;1;2"            , FORM(SnakeCase)                ,lang:: Case::Ignore    , 1, 2 );
    testTokenDef(ut, "Test-Sna_ke"     ";Ign;1;2"            , FORM(SnakeCase)                ,lang:: Case::Ignore    , 1, 2 );

#if ALIB_DEBUG
    testTokenDef(ut, "TestCamelCase"   ";S;1;2;3;4"          , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test_Snak_Casee" ";S;1;2;3;4"          , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test-Kebab-Case" ";S;1;2;3;4"          , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );

    testTokenDef(ut, "TestCamelCase"   ";S;1;2"              , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test_Snake_Case" ";S;1;2"              , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test-Kebab-Case" ";S;1;2"              , ERR(InconsistentMinLengths)    ,lang:: Case::Sensitive , 1 );

    testTokenDef(ut, "TestCamelCase"   ";S;1;"               , ERR(ErrorReadingMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test_Snake_Case" ";S;1;"               , ERR(ErrorReadingMinLengths)    ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Test-Kebab-Case" ";S;1;"               , ERR(ErrorReadingMinLengths)    ,lang:: Case::Sensitive , 1 );
#endif
    testTokenDef(ut, "NotCamelCase"    ";S;1"                , FORM(Normal)                   ,lang:: Case::Sensitive , 1 );
    testTokenDef(ut, "Not_Snake_Case"  ";I;1"                , FORM(Normal)                   ,lang:: Case::Ignore    , 1 );
    testTokenDef(ut, "Not-Kebab-Case"  ";S;1"                , FORM(Normal)                   ,lang:: Case::Sensitive , 1 );
}


//--------------------------------------------------------------------------------------------------
//--- TokenMatch
//--------------------------------------------------------------------------------------------------
UT_METHOD( TokenMatch )
{
    UT_INIT()
    Token tok;

    // no special format
    tok.Define( A_CHAR("T;S;1"           ));
    UT_FALSE(  tok.Match(A_CHAR("Test"   )) )
    UT_TRUE (  tok.Match(A_CHAR("T"      )) )
    UT_FALSE(  tok.Match(A_CHAR("TE"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Te"     )) )
    UT_FALSE(  tok.Match(A_CHAR("t"      )) )

    tok.Define( A_CHAR("T;i;1"           ));
    UT_FALSE(  tok.Match(A_CHAR("Test"   )) )
    UT_TRUE (  tok.Match(A_CHAR("T"      )) )
    UT_FALSE(  tok.Match(A_CHAR("TE"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Te"     )) )
    UT_TRUE (  tok.Match(A_CHAR("t"      )) )

    tok.Define( A_CHAR("Te;S;1"          ));
    UT_FALSE(  tok.Match(A_CHAR("Test"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Te"     )) )
    UT_FALSE(  tok.Match(A_CHAR("TE"     )) )
    UT_FALSE(  tok.Match(A_CHAR("TES"    )) )
    UT_FALSE(  tok.Match(A_CHAR("TeS"    )) )

    tok.Define( A_CHAR("Te;i;1"          ));
    UT_FALSE(  tok.Match(A_CHAR("Test"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Te"     )) )
    UT_TRUE (  tok.Match(A_CHAR("TE"     )) )
    UT_FALSE(  tok.Match(A_CHAR("TES"    )) )
    UT_FALSE(  tok.Match(A_CHAR("TeS"    )) )


    tok.Define( A_CHAR("Test;S;1"        ));
    UT_TRUE (  tok.Match(A_CHAR("Test"   )) )
    UT_FALSE(  tok.Match(A_CHAR("TEST"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Tes"    )) )
    UT_TRUE (  tok.Match(A_CHAR("Te"     )) )
    UT_TRUE (  tok.Match(A_CHAR("T"      )) )
    UT_FALSE(  tok.Match(A_CHAR("t"      )) )
    UT_FALSE(  tok.Match(A_CHAR("Testx"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Tesx"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Tex"    )) )
    UT_FALSE(  tok.Match(A_CHAR("Tx"     )) )
    UT_FALSE(  tok.Match(A_CHAR("x"      )) )

    tok.Define( A_CHAR("Test;I;1"        ));
    UT_TRUE (  tok.Match(A_CHAR("Test"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Tes"    )) )
    UT_TRUE (  tok.Match(A_CHAR("Te"     )) )
    UT_TRUE (  tok.Match(A_CHAR("t"      )) )
    UT_FALSE(  tok.Match(A_CHAR("Testx"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Tesx"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Tex"    )) )
    UT_FALSE(  tok.Match(A_CHAR("Tx"     )) )
    UT_FALSE(  tok.Match(A_CHAR("x"      )) )

    // camel case
    tok.Define( A_CHAR("MiSeS;I;1;1;0"   ));
    UT_TRUE (  tok.Match(A_CHAR("MSs"    )) )
    UT_TRUE (  tok.Match(A_CHAR("MSe"    )) )
    UT_TRUE (  tok.Match(A_CHAR("MS"     )) )

    tok.Define( A_CHAR("AbX;I;1;0"       ));
    UT_TRUE (  tok.Match(A_CHAR("AbX"    )) )
    UT_TRUE (  tok.Match(A_CHAR("Ab"     )) )
    UT_TRUE (  tok.Match(A_CHAR("A"      )) )
    UT_TRUE (  tok.Match(A_CHAR("AX"     )) )
    UT_FALSE(  tok.Match(A_CHAR("AcX"    )) )

    tok.Define( A_CHAR("AxBC;I;1;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("ABC"    )) )
    UT_FALSE(  tok.Match(A_CHAR("AC"     )) )

    tok.Define( A_CHAR("UtcToday;I;1;1"  ));
    UT_TRUE (  tok.Match(A_CHAR("UT"     )) )
    UT_TRUE (  tok.Match(A_CHAR("UTo"    )) )
    UT_TRUE (  tok.Match(A_CHAR("UtTo"   )) )
    UT_TRUE (  tok.Match(A_CHAR("UtT"    )) )
    UT_TRUE (  tok.Match(A_CHAR("UtcT"   )) )

    tok.Define( A_CHAR("AaAa;I;1;1"      ));
    UT_TRUE (  tok.Match(A_CHAR("AaAa"   )) )
    UT_FALSE(  tok.Match(A_CHAR("a"      )) )
    UT_TRUE (  tok.Match(A_CHAR("aa"     )) )
    UT_TRUE (  tok.Match(A_CHAR("aaa"    )) )
    UT_TRUE (  tok.Match(A_CHAR("aaaa"   )) )
    UT_FALSE(  tok.Match(A_CHAR("aaaaa"  )) )
    UT_FALSE(  tok.Match(A_CHAR("aab"    )) )
    UT_FALSE(  tok.Match(A_CHAR("aba"    )) )
    UT_FALSE(  tok.Match(A_CHAR("baa"    )) )

    tok.Define( A_CHAR("AaBbCc;I;1;1;1"  ));
    UT_FALSE (  tok.Match(A_CHAR("AB"    )) )

    tok.Define( A_CHAR("aBC;I;1;1;1"     ));
    UT_TRUE  (  tok.Match(A_CHAR("ABC"   )) )
    UT_FALSE (  tok.Match(A_CHAR("AB"    )) )

    tok.Define( A_CHAR("AbcXyz;s;3;3"    ));
    UT_TRUE (  tok.Match(A_CHAR("AbcXyz" )) )
    UT_FALSE(  tok.Match(A_CHAR("abcXyz" )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCXYZ" )) )

    tok.Define( A_CHAR("abcXyz;s;3;3"    ));
    UT_TRUE (  tok.Match(A_CHAR("abcXyz" )) )
    UT_FALSE(  tok.Match(A_CHAR("AbcXyz" )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCXYZ" )) )

    tok.Define( A_CHAR("AbcXyz;I;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("AbcXyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("ABCXYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("AX"     )) )
    UT_TRUE (  tok.Match(A_CHAR("AbXy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("AbXyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("AbcXy"  )) )

    UT_TRUE (  tok.Match(A_CHAR("abcxyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("abCxYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("ax"     )) )
    UT_TRUE (  tok.Match(A_CHAR("aBxy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("abxyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("abcxY"  )) )

    UT_FALSE(  tok.Match(A_CHAR("AbcdXy" )) )
    UT_FALSE(  tok.Match(A_CHAR("AdX"    )) )
    UT_FALSE(  tok.Match(A_CHAR("xAX"    )) )
    UT_FALSE(  tok.Match(A_CHAR("AXx"    )) )

    tok.Define( A_CHAR("AbcdXyz;I;4;3"   ));
    UT_TRUE (  tok.Match(A_CHAR("AbcdXyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCDXYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcdxyz")) )
    UT_FALSE(  tok.Match(A_CHAR("AX"     )) )
    UT_FALSE(  tok.Match(A_CHAR("AbXy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("AbXyz"  )) )
    UT_FALSE(  tok.Match(A_CHAR("AbcXy"  )) )
    UT_FALSE(  tok.Match(A_CHAR("AbcdXy" )) )

    tok.Define( A_CHAR("AbcdXyz;I;3;2"   ));
    UT_TRUE (  tok.Match(A_CHAR("AbcdXyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCDXYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcdxyz")) )
    UT_FALSE(  tok.Match(A_CHAR("AX"     )) )
    UT_FALSE(  tok.Match(A_CHAR("AbXy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("AbXyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("AbcXy"  )) )
    UT_TRUE (  tok.Match(A_CHAR("AbcdXy" )) )

       // Camel rollbacks
    tok.Define( A_CHAR("SystemTemperature;I;1;1") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;2;1") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;3;1") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;4;1") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;2;2") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;3;2") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;4;2") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;2;3") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;3;3") );     UT_TRUE (  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;4;3") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;2;4") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;3;4") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;4;4") );     UT_FALSE(  tok.Match( A_CHAR("system" ) ))
    tok.Define( A_CHAR("SystemTemperature;I;2;4") );     UT_TRUE (  tok.Match( A_CHAR("systemp") ))
    tok.Define( A_CHAR("SystemTemperature;I;3;4") );     UT_TRUE (  tok.Match( A_CHAR("systemp") ))
    tok.Define( A_CHAR("SystemTemperature;I;4;4") );     UT_FALSE(  tok.Match( A_CHAR("systemp") ))

    tok.Define( A_CHAR("AbcdeCx;I;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("abc"     )) )

    tok.Define( A_CHAR("AbBcC;I;1;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("abc"     )) )


    // snake case
    tok.Define( A_CHAR("Abc_Xyz;I;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("Abc_Xyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("ABC_XYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("A_X"     )) )
    UT_TRUE (  tok.Match(A_CHAR("Ab_Xy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Ab_Xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abc_Xy"  )) )

    UT_TRUE (  tok.Match(A_CHAR("abc_xyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("abC_xYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("a_x"     )) )
    UT_TRUE (  tok.Match(A_CHAR("aB_xy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("ab_xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("abc_xY"  )) )

    UT_FALSE(  tok.Match(A_CHAR("Abcd_Xy" )) )
    UT_FALSE(  tok.Match(A_CHAR("Ad_X"    )) )
    UT_FALSE(  tok.Match(A_CHAR("xA_X"    )) )
    UT_FALSE(  tok.Match(A_CHAR("A_Xx"    )) )

    tok.Define( A_CHAR("abcd_xyz;I;4;3"   ));
    UT_TRUE (  tok.Match(A_CHAR("Abcd_Xyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCD_XYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcd_xyz")) )
    UT_FALSE(  tok.Match(A_CHAR("A_X"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab_Xy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab_Xyz"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Abc_Xy"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Abcd_Xy" )) )

    tok.Define( A_CHAR("Abcd_Xyz;I;3;2"   ));
    UT_TRUE (  tok.Match(A_CHAR("Abcd_Xyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCD_XYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcd_xyz")) )
    UT_FALSE(  tok.Match(A_CHAR("A_X"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab_Xy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab_Xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abc_Xy"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abcd_Xy" )) )

    tok.Define( A_CHAR("AxBC;I;1;1;1"     ));
    UT_TRUE (  tok.Match(A_CHAR("ABC"     )) )
    UT_FALSE(  tok.Match(A_CHAR("AC"      )) )

    // kebab case
    tok.Define( A_CHAR("Abc-Xyz;I;1;1"    ));
    UT_TRUE (  tok.Match(A_CHAR("Abc-Xyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("ABC-XYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("A-X"     )) )
    UT_TRUE (  tok.Match(A_CHAR("Ab-Xy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("Ab-Xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abc-Xy"  )) )

    UT_TRUE (  tok.Match(A_CHAR("abc-xyz" )) )
    UT_TRUE (  tok.Match(A_CHAR("abC-xYZ" )) )
    UT_TRUE (  tok.Match(A_CHAR("a-x"     )) )
    UT_TRUE (  tok.Match(A_CHAR("aB-xy"   )) )
    UT_TRUE (  tok.Match(A_CHAR("ab-xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("abc-xY"  )) )

    UT_FALSE(  tok.Match(A_CHAR("Abcd-Xy" )) )
    UT_FALSE(  tok.Match(A_CHAR("Ad-X"    )) )
    UT_FALSE(  tok.Match(A_CHAR("xA-X"    )) )
    UT_FALSE(  tok.Match(A_CHAR("A-Xx"    )) )

    tok.Define( A_CHAR("abcd-xyz;I;4;3"   ));
    UT_TRUE (  tok.Match(A_CHAR("Abcd-Xyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCD-XYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcd-xyz")) )
    UT_FALSE(  tok.Match(A_CHAR("A-X"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab-Xy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab-Xyz"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Abc-Xy"  )) )
    UT_FALSE(  tok.Match(A_CHAR("Abcd-Xy" )) )

    tok.Define( A_CHAR("Abcd-Xyz;I;3;2"   ));
    UT_TRUE (  tok.Match(A_CHAR("Abcd-Xyz")) )
    UT_TRUE (  tok.Match(A_CHAR("ABCD-XYZ")) )
    UT_TRUE (  tok.Match(A_CHAR("abcd-xyz")) )
    UT_FALSE(  tok.Match(A_CHAR("A-X"     )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab-Xy"   )) )
    UT_FALSE(  tok.Match(A_CHAR("Ab-Xyz"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abc-Xy"  )) )
    UT_TRUE (  tok.Match(A_CHAR("Abcd-Xy" )) )

    // 7 humps
    tok.Define( A_CHAR("S1S2S3S4S5S6S7;s;1;1;1;1;1;1;1"            ));
    UT_TRUE (  tok.Match(A_CHAR("S1S2S3S4S5S6S7"                   )) )
    UT_TRUE (  tok.Match(A_CHAR("SSSSSSS"                          )) )
    UT_FALSE(  tok.Match(A_CHAR("S4SSSSSS"                         )) )

    tok.Define( A_CHAR("S1_S2_S3_S4_S5_S6_S7;s;1;1;1;1;1;1;1"      ));
    UT_TRUE (  tok.Match(A_CHAR("S1_S2_S3_S4_S5_S6_S7"             )) )
    UT_TRUE (  tok.Match(A_CHAR("S_S_S_S_S_S_S"                    )) )
    UT_FALSE(  tok.Match(A_CHAR("S4_S_S_S_S_S_S"                   )) )

    tok.Define( A_CHAR("S1-S2-S3-S4-S5-S6-S7;s;1;1;1;1;1;1;1"      ));
    UT_TRUE (  tok.Match(A_CHAR("S1-S2-S3-S4-S5-S6-S7"             )) )
    UT_TRUE (  tok.Match(A_CHAR("S-S-S-S-S-S-S"                    )) )
    UT_FALSE(  tok.Match(A_CHAR("S4-S-S-S-S-S-S"                   )) )

    // more than 7 humps
    tok.Define( A_CHAR("A1B2C3D4E5F6G7H8;s;1;1;1;1;1;1;1"          ));
    UT_TRUE (  tok.Match(A_CHAR("A1B2C3D4E5F6G7H8"                 )) )
    UT_TRUE (  tok.Match(A_CHAR("ABCDEFGH8"                        )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGH"                         )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFG"                          )) )

    tok.Define( A_CHAR("A1B2C3D4E5F6G7X8Y9;s;1;1;1;1;1;1;1"        ));
    UT_FALSE(  tok.Match(A_CHAR("A1B2C3D4E5F6G7X8Y99"              )) )
    UT_TRUE (  tok.Match(A_CHAR("A1B2C3D4E5F6G7X8Y9"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A1B2C3D4E5F6G7X8"                 )) )
    UT_FALSE(  tok.Match(A_CHAR("A1B2C3D4E5F6G7"                   )) )
    UT_TRUE (  tok.Match(A_CHAR("ABCDEFGX8Y9"                      )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGX8Y"                       )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGXY9"                       )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGXY"                        )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGXYS"                       )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGXYz"                       )) )
    UT_FALSE(  tok.Match(A_CHAR("ABCDEFGX"                         )) )

    tok.Define( A_CHAR("A1_B2_C3_D4_E5_F6_G7_H8;s;1;1;1;1;1;1;1"   ));
    UT_TRUE (  tok.Match(A_CHAR("A1_B2_C3_D4_E5_F6_G7_H8"          )) )
    UT_TRUE (  tok.Match(A_CHAR("A_B_C_D_E_F_G_H8"                 )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_H"                  )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G"                    )) )

    tok.Define( A_CHAR("A1_B2_C3_D4_E5_F6_G7_X8_Y9;s;1;1;1;1;1;1;1"));
    UT_FALSE(  tok.Match(A_CHAR("A1_B2_C3_D4_E5_F6_G7_X8_Y99"      )) )
    UT_TRUE (  tok.Match(A_CHAR("A1_B2_C3_D4_E5_F6_G7_X8_Y9"       )) )
    UT_FALSE(  tok.Match(A_CHAR("A1_B2_C3_D4_E5_F6_G7_X8"          )) )
    UT_FALSE(  tok.Match(A_CHAR("A1_B2_C3_D4_E5_F6_G7"             )) )
    UT_TRUE (  tok.Match(A_CHAR("A_B_C_D_E_F_G_X8_Y9"              )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X8_Y"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X_Y9"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X_Y"                )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X_Y_S"              )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X_Yz"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A_B_C_D_E_F_G_X"                  )) )

    tok.Define( A_CHAR("A1-B2-C3-D4-E5-F6-G7-H8;s;1;1;1;1;1;1;1"   ));
    UT_TRUE (  tok.Match(A_CHAR("A1-B2-C3-D4-E5-F6-G7-H8"          )) )
    UT_TRUE (  tok.Match(A_CHAR("A-B-C-D-E-F-G-H8"                 )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-H"                  )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G"                    )) )

    tok.Define( A_CHAR("A1-B2-C3-D4-E5-F6-G7-X8-Y9;s;1;1;1;1;1;1;1"));
    UT_FALSE(  tok.Match(A_CHAR("A1-B2-C3-D4-E5-F6-G7-X8-Y99"      )) )
    UT_TRUE (  tok.Match(A_CHAR("A1-B2-C3-D4-E5-F6-G7-X8-Y9"       )) )
    UT_FALSE(  tok.Match(A_CHAR("A1-B2-C3-D4-E5-F6-G7-X8"          )) )
    UT_FALSE(  tok.Match(A_CHAR("A1-B2-C3-D4-E5-F6-G7"             )) )
    UT_TRUE (  tok.Match(A_CHAR("A-B-C-D-E-F-G-X8-Y9"              )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X8-Y"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X-Y9"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X-Y"                )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X-Y-S"              )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X-Yz"               )) )
    UT_FALSE(  tok.Match(A_CHAR("A-B-C-D-E-F-G-X"                  )) )
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_STRINGS

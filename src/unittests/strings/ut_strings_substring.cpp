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
#include "alib/compatibility/std_strings.hpp"
#include "alib/strings/util/tokenizer.hpp"
#include "alib/strings/numberformat.hpp"

#include <iostream>
#include <sstream>

#define TESTCLASSNAME       UT_Strings_Substring
#include "unittests/aworx_unittests.hpp"

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

using namespace std;
using namespace alib;



namespace ut_aworx {


UT_CLASS

//--------------------------------------------------------------------------------------------------
//--- Test Substring
//--------------------------------------------------------------------------------------------------
void substringConstruction( const character* inputString, AString& res, bool trim )
{
    Substring subs( inputString );
    if (trim)
        subs.Trim();
    res.Reset(subs);
}

UT_METHOD( Constructor )
{
    UT_INIT()

    AString as;
    AString res;

    substringConstruction( A_CHAR("a")        , res, false ); UT_EQ( A_CHAR("a"),        res )
    substringConstruction( A_CHAR(" a")       , res, false ); UT_EQ( A_CHAR(" a"),       res )
    substringConstruction( A_CHAR("a ")       , res, false ); UT_EQ( A_CHAR("a "),       res )
    substringConstruction( A_CHAR("a b")      , res, false ); UT_EQ( A_CHAR("a b"),      res )
    substringConstruction( A_CHAR(" a b")     , res, false ); UT_EQ( A_CHAR(" a b"),     res )
    substringConstruction( A_CHAR("a b ")     , res, false ); UT_EQ( A_CHAR("a b "),     res )

    substringConstruction( A_CHAR("a")        , res, true  ); UT_EQ( A_CHAR("a"),        res )
    substringConstruction( A_CHAR(" a")       , res, true  ); UT_EQ( A_CHAR("a"),        res )
    substringConstruction( A_CHAR("a ")       , res, true  ); UT_EQ( A_CHAR("a"),        res )
    substringConstruction( A_CHAR("a b")      , res, true  ); UT_EQ( A_CHAR("a b"),      res )
    substringConstruction( A_CHAR(" a b")     , res, true  ); UT_EQ( A_CHAR("a b"),      res )
    substringConstruction( A_CHAR("a b ")     , res, true  ); UT_EQ( A_CHAR("a b"),      res )

    // changing whitespaces
    {
        {
            as.Reset( A_CHAR("xy xz abc xy") );
            Substring subs;
            (subs= as).Trim( A_CHAR("xy "));
            res.Reset( subs );
            UT_EQ( A_CHAR("z abc"), res )
        }
        {
            Substring subs( A_CHAR("xy xz abc xy") );
            res.Reset( subs.TrimStart( A_CHAR("xy ") )
                           .TrimEnd  ( A_CHAR("xy ") ) );
            UT_EQ( A_CHAR("z abc"), res )
        }
    }

    // test other constructors
    {
        {
            Substring null( nullptr);
            UT_EQ( true,      null.IsEmpty() )
            UT_EQ( true,      null.IsNull()  )
        }

        {
            Substring subs( A_CHAR(" const char* ") );
            res.Reset( subs.Trim() );
            UT_EQ( A_CHAR("const char*"), res )
        }
        {
            Substring subs= String(A_CHAR("const char* ")).Substring(0, 5);
            res.Reset( subs.Trim() );
            UT_EQ( A_CHAR("const"), res )
        }

        {
            String64 astr( A_CHAR(" astring "));
            Substring subs( astr);
            res.Reset( subs.Trim() );
            UT_EQ( A_CHAR("astring"), res )

            Substring subs2= astr.Substring( 2, 3 );
            res.Reset( subs2.Trim() );
            UT_EQ( A_CHAR("str"), res )

            Substring subs3= astr.Substring( 20, 3 );
            res.Reset( subs3.Trim() );
            UT_TRUE(  subs3.IsEmpty() )
            UT_TRUE( !subs3.IsNull()  )
        }
    }
}

UT_METHOD( CharAt)
{
    UT_INIT()
    // empty substring
    {
        Substring subs(nullptr);
        UT_EQ(  '\0',      subs.CharAtStart() )
        UT_EQ(  '\0',      subs.CharAtEnd() )
    }

    // Consume/PopLast
    {
        Substring subs(A_CHAR("abcde"));
        UT_EQ('a',      subs.CharAtStart()           )
        UT_EQ('e',      subs.CharAtEnd ()            )
        UT_EQ('a',      subs.CharAtStart<NC>()    )
        UT_EQ('e',      subs.CharAtEnd<NC>()     )
        UT_EQ('a',      subs.ConsumeChar()               )
        UT_EQ('e',      subs.ConsumeCharFromEnd()        )
        UT_EQ('d',      subs.ConsumeCharFromEnd()        )
        UT_EQ('b',      subs.ConsumeChar()               )
        UT_EQ('c',      subs.ConsumeChar()               )
        UT_EQ('\0',     subs.ConsumeChar()               )
        UT_EQ('\0',     subs.ConsumeChar()               )
        UT_EQ('\0',     subs.ConsumeCharFromEnd()        )
        UT_EQ('\0',     subs.ConsumeCharFromEnd()        )
    }

    // Delete first
    {
        Substring subs(A_CHAR("1234567890"));
        subs.ConsumeChars<NC>        (2) ;        UT_TRUE( subs == A_CHAR("34567890")  )
        subs.ConsumeCharsFromEnd<NC>(3) ;        UT_TRUE( subs == A_CHAR("34567")     )
        subs.ConsumeChars               (2) ;        UT_TRUE( subs == A_CHAR("567")    )
        subs.ConsumeCharsFromEnd        (3) ;        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeChars<NC>        (0) ;        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeCharsFromEnd<NC>(0) ;        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeChars               (0) ;        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeCharsFromEnd        (0) ;        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeChars               (-2);        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeCharsFromEnd        (-2);        UT_TRUE( subs == A_CHAR("")        )
        subs.ConsumeChars               (20);   UT_EQ  ( 0, subs.Length() )
        subs= A_CHAR("1234567890");
        subs.ConsumeCharsFromEnd        (20);   UT_EQ  ( 0, subs.Length() )
    }

}

UT_METHOD( ConsumeNumbers )
{
    UT_INIT()

    // ConsumeDecimal()
    {
        Substring subs(nullptr);
        int result;
                                     UT_EQ( false,  subs.ConsumeInt( result      ) )   UT_EQ(       0,  result )
        subs= A_CHAR("")           ; UT_EQ( false,  subs.ConsumeInt( result      ) )   UT_EQ(       0,  result )
        subs= A_CHAR("  ABC")      ; UT_EQ( false,  subs.ConsumeInt( result      ) )   UT_EQ(       0,  result )
        subs= A_CHAR("  12345")    ; UT_EQ( true ,  subs.ConsumeInt( result      ) )   UT_EQ(   12345,  result )
        subs= A_CHAR("  12 45")    ; UT_EQ( true ,  subs.ConsumeInt( result      ) )   UT_EQ(      12,  result )
                                     UT_EQ( true ,  subs.ConsumeInt( result      ) )   UT_EQ(      45,  result )

        subs= A_CHAR(" 42 ; 7 ; 6 ");UT_EQ( true ,  subs.ConsumeInt( result      ) )   UT_EQ(      42,  result )
                                     UT_EQ( false,  subs.ConsumeInt( result      ) )   UT_EQ(       0,  result )
                                     UT_EQ( false,  subs.ConsumeInt( result      ) )   UT_EQ(       0,  result )
        NumberFormat nf;    nf.Whitespaces= A_CHAR(" ;");
        subs= A_CHAR(" 42 ; 7 ; 6 ");UT_EQ( true ,  subs.ConsumeInt( result, &nf ) )   UT_EQ(      42,  result )
                                     UT_EQ( true ,  subs.ConsumeInt( result, &nf ) )   UT_EQ(       7,  result )
                                     UT_EQ( true ,  subs.ConsumeInt( result, &nf ) )   UT_EQ(       6,  result )
                                     UT_EQ( false,  subs.ConsumeInt( result, &nf ) )   UT_EQ(       0,  result )
                                     UT_EQ( false,  subs.ConsumeInt( result, &nf ) )   UT_EQ(       0,  result )
    }

    // ConsumeFloat()
    {
        Substring subs;
        double result;
                                     UT_EQ( false,  subs.ConsumeFloat  ( result      ) )   UT_EQ(      0.,  result )
        subs= A_CHAR("")           ; UT_EQ( false,  subs.ConsumeFloat  ( result      ) )   UT_EQ(      0.,  result )
        subs= A_CHAR("  ABC")      ; UT_EQ( false,  subs.ConsumeFloat  ( result      ) )   UT_EQ(      0.,  result )
        subs= A_CHAR("  12345")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) )   UT_EQ(  12345.,  result )
        subs= A_CHAR(" 12.45 ")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) )   UT_EQ(  12.45,   result )
        subs= A_CHAR("  12 45")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) )   UT_EQ(     12.,  result )
                                     UT_EQ( true ,  subs.ConsumeFloat  ( result      ) )   UT_EQ(     45.,  result )

        NumberFormat nf;    nf.Whitespaces= A_CHAR(" ;");
        subs= A_CHAR(" 42.3 ; 0.7 ; 6 ");
                                     UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) )   UT_EQ(    42.3,  result )
                                     UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) )   UT_EQ(     0.7,  result )
                                     UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) )   UT_EQ(      6.,  result )
                                     UT_EQ( false,  subs.ConsumeFloat  ( result, &nf ) )   UT_EQ(      0.,  result )
                                     UT_EQ( false,  subs.ConsumeFloat  ( result, &nf ) )   UT_EQ(      0.,  result )
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test Consume
//--------------------------------------------------------------------------------------------------
UT_METHOD( Consume )
{
    UT_INIT()
    // null substring
    {
        Substring s(nullptr);
        Substring r(A_CHAR("oldval"));
        UT_EQ(  '\0',      s.ConsumeChar         ( )         )
        UT_EQ(  0,         s.ConsumeChars        (  0  , &r) ) UT_TRUE(r.IsNull())
        UT_EQ(  0,         s.ConsumeChars        (  5  , &r) ) UT_TRUE(r.IsNull())
        UT_EQ(  false,     s.ConsumeChar         ( 'a' )     )
        UT_EQ(  false,     s.ConsumeString       ( A_CHAR("word") )  )
        UT_EQ(  '\0',      s.ConsumeCharFromEnd  ( )         )
        UT_EQ(  0,         s.ConsumeCharsFromEnd (  0  )     )
        UT_EQ(  0,         s.ConsumeCharsFromEnd (  5  )     )
        UT_EQ(  false,     s.ConsumeCharFromEnd  ( 'a' )     )
        UT_EQ(  false,     s.ConsumeStringFromEnd( A_CHAR("word") )  )
    }

    // empty substring
    {
        Substring s(EMPTY_STRING);
        Substring r(A_CHAR("oldval"));
        UT_EQ(  '\0',  s. ConsumeChar         ( )         )
        UT_EQ(  0,     s. ConsumeChars        (  0   ,&r) ) UT_TRUE( r.IsNotNull()) UT_TRUE(r.IsEmpty())
        UT_EQ(  0,     s. ConsumeChars        (  5   ,&r) ) UT_TRUE( r.IsNotNull()) UT_TRUE(r.IsEmpty())
        UT_EQ(  false, s. ConsumeChar         ( 'a' )     )
        UT_EQ(  false, s. ConsumeString       ( A_CHAR("word") )  )
        UT_EQ(  '\0',  s. ConsumeCharFromEnd  ( )         )
        UT_EQ(  0,     s. ConsumeCharsFromEnd (  0  )     )
        UT_EQ(  0,     s. ConsumeCharsFromEnd (  5  )     )
        UT_EQ(  false, s. ConsumeCharFromEnd  ( 'a' )     )
        UT_EQ(  false,  s.ConsumeStringFromEnd( A_CHAR("word") )  )
    }

    // substring of length 1
    {
        Substring s;
        Substring r(A_CHAR("oldval"));
        s= A_CHAR("a"); UT_EQ ( 'a',   s. ConsumeChar          (         ) )           UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 1,     s. ConsumeChars         ( 0       ) )           UT_EQ ( 1,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 0,     s. ConsumeChars         ( 1  , &r ) )           UT_EQ ( 0,        s. Length ( ) )  UT_TRUE ( r.Equals( A_CHAR("a") ) )
        s= A_CHAR("a"); UT_EQ ( 0,     s. ConsumeChars         ( 5  , &r ) )           UT_EQ ( 0,        s. Length ( ) )  UT_TRUE ( r.Equals( A_CHAR("a") ) )
        s= A_CHAR("a"); UT_EQ ( true,  s. ConsumeChar          ( 'a'     ) )           UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( false, s. ConsumeChar          ( 'b'     ) )           UT_EQ ( 1,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( false, s. ConsumeString        ( A_CHAR("word")  ) )   UT_EQ ( 1,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 'a',   s. ConsumeCharFromEnd   ( ) )                   UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 1,     s. ConsumeCharsFromEnd  ( 0       ) )           UT_EQ ( 1,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 0,     s. ConsumeCharsFromEnd  ( 1       ) )           UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( 0,     s. ConsumeCharsFromEnd  ( 5       ) )           UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( true,  s. ConsumeCharFromEnd   ( 'a'     ) )           UT_EQ ( 0,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( false, s. ConsumeCharFromEnd   ( 'b'     ) )           UT_EQ ( 1,        s. Length ( ) )
        s= A_CHAR("a"); UT_EQ ( false, s. ConsumeStringFromEnd ( A_CHAR("word")  ) )   UT_EQ ( 1, s. Length        ( ) )
    }

    // substring of length 2
    {
        Substring s;
        Substring r(A_CHAR("oldval"));

        s= A_CHAR("ab"); UT_EQ( 'a',   s.ConsumeChar        ( )         ) UT_EQ( 1,s.Length() )
                         UT_EQ( 'b',   s.ConsumeChar        ( )         ) UT_EQ( 0,s.Length() )
        s= A_CHAR("ab"); UT_EQ( 'b',   s.ConsumeCharFromEnd ( )         ) UT_EQ( 1,s.Length() )
                         UT_EQ( 'a',   s.ConsumeCharFromEnd ( )         ) UT_EQ( 0,s.Length() )

        s= A_CHAR("ab"); UT_EQ(  2,    s.ConsumeChars       (  0  ,&r)  ) UT_EQ( 2,s.Length() ) UT_TRUE(r.IsNotNull())  UT_TRUE(r.IsEmpty())
        s= A_CHAR("ab"); UT_EQ(  1,    s.ConsumeChars       (  1  ,&r)  ) UT_EQ( 1,s.Length() ) UT_TRUE(r.Equals(A_CHAR("a") ))
        s= A_CHAR("ab"); UT_EQ(  0,    s.ConsumeChars       (  2  ,&r)  ) UT_EQ( 0,s.Length() ) UT_TRUE(r.Equals(A_CHAR("ab")))
        s= A_CHAR("ab"); UT_EQ(  0,    s.ConsumeChars       (  3  ,&r)  ) UT_EQ( 0,s.Length() ) UT_TRUE(r.Equals(A_CHAR("ab")))
        s= A_CHAR("ab"); UT_EQ(  2,    s.ConsumeCharsFromEnd(  0  ,&r)  ) UT_EQ( 2,s.Length() ) UT_TRUE(r.IsNotNull())  UT_TRUE(r.IsEmpty())
        s= A_CHAR("ab"); UT_EQ(  1,    s.ConsumeCharsFromEnd(  1  ,&r)  ) UT_EQ( 1,s.Length() ) UT_TRUE(r.Equals(A_CHAR("b") ))
        s= A_CHAR("ab"); UT_EQ(  0,    s.ConsumeCharsFromEnd(  2  ,&r)  ) UT_EQ( 0,s.Length() ) UT_TRUE(r.Equals(A_CHAR("ab")))
        s= A_CHAR("ab"); UT_EQ(  0,    s.ConsumeCharsFromEnd(  3  ,&r)  ) UT_EQ( 0,s.Length() ) UT_TRUE(r.Equals(A_CHAR("ab")))

        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeChar       ( 'b' )     )  UT_EQ( 2,s.Length() )
                         UT_EQ(  true,      s.ConsumeChar       ( 'a' )     )  UT_EQ( 1,s.Length() )
                         UT_EQ(  true,      s.ConsumeChar       ( 'b' )     )  UT_EQ( 0,s.Length() )
                         UT_EQ(  false,     s.ConsumeChar       ( 'a' )     )  UT_EQ( 0,s.Length() )
                         UT_EQ(  false,     s.ConsumeChar       ( 'b' )     )  UT_EQ( 0,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeCharFromEnd( 'a' )     )  UT_EQ( 2,s.Length() )
                         UT_EQ(  true,      s.ConsumeCharFromEnd( 'b' )     )  UT_EQ( 1,s.Length() )
                         UT_EQ(  true,      s.ConsumeCharFromEnd( 'a' )     )  UT_EQ( 0,s.Length() )
                         UT_EQ(  false,     s.ConsumeCharFromEnd( 'b' )     )  UT_EQ( 0,s.Length() )
                         UT_EQ(  false,     s.ConsumeCharFromEnd( 'a' )     )  UT_EQ( 0,s.Length() )

        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeString       ( A_CHAR("word") )  )  UT_EQ( 2,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeString       ( A_CHAR("AB")   )  )  UT_EQ( 2,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  true,      s.ConsumeString       ( A_CHAR("ab")   )  )  UT_EQ( 0,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeStringFromEnd( A_CHAR("word") )  )  UT_EQ( 2,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  false,     s.ConsumeStringFromEnd( A_CHAR("AB")   )  )  UT_EQ( 2,s.Length() )
        s= A_CHAR("ab"); UT_EQ(  true,      s.ConsumeStringFromEnd( A_CHAR("ab")   )  )  UT_EQ( 0,s.Length() )
    }

    // 3 words
    {
        Substring s(A_CHAR("word1 word2 word3"));

        UT_EQ(  'w',       s.ConsumeChar   ( )         )
        UT_EQ(  'o',       s.ConsumeChar   ( )         )
        UT_EQ(  'r',       s.ConsumeChar   ( )         )
        UT_EQ(  'd',       s.ConsumeChar   ( )         )
        UT_EQ(  '1',       s.ConsumeChar   ( )         )

        UT_EQ(  false    , s.ConsumeChar                                       ('w'  ) )
        UT_EQ(  true     , s.ConsumeChar  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>   ('w'  ) )
        UT_EQ(  true     , s.ConsumeChar  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>   ('o'  ) )
        UT_EQ(  false    , s.ConsumeChar  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>   ('o'  ) )
        UT_EQ(  true     , s.ConsumeChar  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>   ('r'  ) )
        UT_EQ(  false    , s.ConsumeString<lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>   (A_CHAR("D2") ) )
        UT_EQ(  false    , s.ConsumeString                                                             (A_CHAR("D2") ) )
        UT_EQ(  true     , s.ConsumeString                                                             (A_CHAR("d2") ) )

        UT_EQ(  2   , s.ConsumeChars  ( 4 )   )
        UT_EQ(  A_CHAR("d3")     , s )

        s= A_CHAR("word1 word2 word3");

        UT_EQ(  '3',       s.ConsumeCharFromEnd( )         )
        UT_EQ(  'd',       s.ConsumeCharFromEnd( )         )
        UT_EQ(  'r',       s.ConsumeCharFromEnd( )         )
        UT_EQ(  'o',       s.ConsumeCharFromEnd( )         )
        UT_EQ(  'w',       s.ConsumeCharFromEnd( )         )

        UT_EQ(  false    , s.ConsumeCharFromEnd                                    ('2')   )
        UT_EQ(  true     , s.ConsumeCharFromEnd  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>('2')  )
        UT_EQ(  true     , s.ConsumeCharFromEnd  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>('d')  )
        UT_EQ(  false    , s.ConsumeCharFromEnd  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>('d')  )
        UT_EQ(  true     , s.ConsumeCharFromEnd  <lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>('r')  )
        UT_EQ(  false    , s.ConsumeStringFromEnd<lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>(A_CHAR("WO")) )
        UT_EQ(  false    , s.ConsumeStringFromEnd<lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>(A_CHAR("WO")) )
        UT_EQ(  true     , s.ConsumeStringFromEnd<lang::Case::Sensitive ALIB_COMMA lang::Whitespaces::Trim>(A_CHAR("wo")) )

        UT_EQ(  2   , s.ConsumeCharsFromEnd( 4 )   )
        UT_EQ(  A_CHAR("wo")     , s )
    }

    // Tokens
    {
        Substring s(A_CHAR("word1 word2 word3"));
        UT_EQ( A_CHAR("word1"), s.ConsumeToken(' ') )
        UT_EQ( A_CHAR("word2 word3"), s )
        UT_EQ( A_CHAR("word2"), s.ConsumeToken(' ') )    UT_EQ( A_CHAR("word3")      , s )
        UT_EQ( A_CHAR("word3"), s.ConsumeToken(' ') )    UT_EQ( 0, s.Length() )
        UT_EQ( EMPTY_STRING   , s.ConsumeToken(' ') ) UT_EQ( 0, s.Length() )
        UT_EQ( EMPTY_STRING   , s.ConsumeToken(' ') ) UT_EQ( 0, s.Length() )
    }


    {
        Substring subs(A_CHAR("1234567890"));
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("abc")) )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("abc"), 2) )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("abc"), 5) )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("125"), 5) )       UT_EQ( A_CHAR("1234567890"), subs )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("125"), 3) )       UT_EQ( A_CHAR("1234567890"), subs )
        UT_EQ( 2, subs.ConsumePartOf(A_CHAR("125"), 2) )       UT_EQ(   A_CHAR("34567890"), subs )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("125"))    )       UT_EQ(   A_CHAR("34567890"), subs )
        UT_EQ( 2, subs.ConsumePartOf(A_CHAR("34") )    )       UT_EQ(     A_CHAR("567890"), subs )
        UT_EQ( 1, subs.ConsumePartOf(A_CHAR("59") )    )       UT_EQ(      A_CHAR("67890"), subs )
        UT_EQ( 3, subs.ConsumePartOf(A_CHAR("678"), 2 ))       UT_EQ(         A_CHAR("90"), subs )
        UT_EQ( 2, subs.ConsumePartOf(A_CHAR("90")     ))       UT_EQ(           A_CHAR(""), subs )
        UT_EQ( 0, subs.ConsumePartOf(A_CHAR("90")     ))       UT_EQ(           A_CHAR(""), subs )
        UT_EQ( 0, subs.ConsumePartOf(EMPTY_STRING     ))       UT_EQ(           A_CHAR(""), subs )
    }

    {
        Substring subs(A_CHAR("abcdef"));
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Sensitive>(A_CHAR("ABC"), 1)  )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Sensitive>(A_CHAR("aBC"), 2)  )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("aBC"), 5)  )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("aBX"), 3)  )   UT_EQ( A_CHAR("abcdef"), subs )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Sensitive>(A_CHAR("aBX"), 2)  )   UT_EQ( A_CHAR("abcdef"), subs )
        UT_EQ( 2, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("aBX"), 2)  )   UT_EQ(   A_CHAR("cdef"), subs )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("CXX"), 2)  )   UT_EQ(   A_CHAR("cdef"), subs )
        UT_EQ( 1, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("CXX"), 1)  )   UT_EQ(    A_CHAR("def"), subs )
        UT_EQ( 3, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("def"), 2)  )   UT_EQ(       A_CHAR(""), subs )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR("def"), 2)  )   UT_EQ(       A_CHAR(""), subs )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR(""),    2)  )   UT_EQ(       A_CHAR(""), subs )
        UT_EQ( 0, subs.ConsumePartOf<lang::Case::Ignore   >(A_CHAR(""),    0)  )   UT_EQ(       A_CHAR(""), subs )
    }
}


//--------------------------------------------------------------------------------------------------
//--- Tokenizer
//--------------------------------------------------------------------------------------------------
void tokenizerTest( const alib::character* inputString, AString& res, alib::character delim, alib::character newDelim,
                    lang::Whitespaces trim, int inpStart= -1, int inpEnd= -1  )
{
    Substring inp( inputString );
    if ( inpStart < 0 )  inpStart= 0;
    if ( inpEnd   < 0 )  inpEnd=   static_cast<int>(inp.Length() - 1);
    inp = inp.Substring( inpStart, inpEnd-inpStart +1 );

    res.Reset();

    Tokenizer tknzr( inp, delim );

    while( tknzr.HasNext() )
    {
        res._( tknzr.Next(trim) );
        res._( newDelim );
    }

}

UT_METHOD( Tokenize )
{
    UT_INIT()

    AString as;
    AString res;

    // tokenizing empty string
    as.Reset( A_CHAR("") );
    res.Reset();
    {
        Tokenizer tknzr( as, ',' );  UT_EQ( true,  tknzr.HasNext() )
        res._( tknzr.Next() );                UT_EQ( A_CHAR(""), res )
    }

    // tokenizing no delim
    as.Reset( A_CHAR("abc") );
    res.Reset();
    {
        Tokenizer tknzr( as, ',' );           UT_EQ( true,  tknzr.HasNext() )
        res._( tknzr.Next() );                UT_EQ( A_CHAR("abc"), res )
    }

    // tokenizing
    {
        tokenizerTest( A_CHAR("abc"),                   res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("abc@")                , res )
        tokenizerTest( A_CHAR("a,bc"),                  res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("a@bc@")               , res )
        tokenizerTest( A_CHAR("a,bc"),                  res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("a@bc@")               , res )
        tokenizerTest( A_CHAR(","),                     res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("@@")                  , res )
        tokenizerTest( A_CHAR(",,"),                    res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("@@@")                 , res )
        tokenizerTest( A_CHAR("a,b,c,,"),               res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("a@b@c@@@")            , res )
        tokenizerTest( A_CHAR("a,b,c"),                 res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("a@b@c@")              , res )
        tokenizerTest( A_CHAR(",a,b,c"),                res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("@a@b@c@")             , res )
        tokenizerTest( A_CHAR("123567"),                res, ',', '@',   lang::Whitespaces::Trim,   2,   2 );   UT_EQ( A_CHAR("3@")                  , res )
        tokenizerTest( A_CHAR("123567"),                res, ',', '@',   lang::Whitespaces::Trim,   2,   1 );   UT_EQ( A_CHAR("@")                   , res )
        tokenizerTest( A_CHAR("  abc , def , ghe "),    res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("abc@def@ghe@")        , res )
        tokenizerTest( A_CHAR("abc , def,ghe,"),        res, ',', '@',   lang::Whitespaces::Trim,  -1,  -1 );   UT_EQ( A_CHAR("abc@def@ghe@@")       , res )
        tokenizerTest( A_CHAR("  abc , def , ghe "),    res, ',', '@',   lang::Whitespaces::Keep,  -1,  -1 );   UT_EQ( A_CHAR("  abc @ def @ ghe @") , res )
        tokenizerTest( A_CHAR("abc , def,ghe,"),        res, ',', '@',   lang::Whitespaces::Keep,  -1,  -1 );   UT_EQ( A_CHAR("abc @ def@ghe@@")     , res )
    }

    // tokenizing with different delimiters
    {
        as.Reset( A_CHAR("1,5;3@4") );
        Tokenizer tknzr(as, ',');
        res.Reset( tknzr.Next(                        ) );  UT_EQ( A_CHAR("1"),   res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next( lang::Whitespaces::Trim, ';' ) );  UT_EQ( A_CHAR("5"),   res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next( lang::Whitespaces::Trim, '@' ) );  UT_EQ( A_CHAR("3"),   res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next( lang::Whitespaces::Trim, '-' ) );  UT_EQ( A_CHAR("4"),   res )  UT_EQ( false, tknzr.HasNext() )
    }

    // tokenizing with different delimiters
    {
        as.Reset( A_CHAR("abc, 5;\t3;;; 4  ") );
        Tokenizer tknzr(as,',');
        res.Reset( tknzr.Next()                               );  UT_EQ( A_CHAR("abc"), res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next( lang::Whitespaces::Trim, ';' ) );  UT_EQ( A_CHAR("5"),   res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next()                               );  UT_EQ( A_CHAR("3"),   res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.Next()                               );  UT_EQ( A_CHAR(""),    res )  UT_EQ( true,  tknzr.HasNext() )
        res.Reset( tknzr.GetRest()                            );  UT_EQ( A_CHAR("; 4"), res )  UT_EQ( false, tknzr.HasNext() )
    }

    // sub-tokens
    {
        as.Reset( A_CHAR("1,2;3 , 4;5,;,") );
        Tokenizer tknzr(as, ';');

        Tokenizer tknzr2( tknzr.Next(), ',');
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR("1"), res )  UT_TRUE(  tknzr2.HasNext() )
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR("2"), res )  UT_TRUE( !tknzr2.HasNext() )
        UT_TRUE( tknzr.HasNext() )

        tknzr2.Set( tknzr.Next(), ',');
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR("3"), res )  UT_TRUE(  tknzr2.HasNext() )
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR("4"), res )  UT_TRUE( !tknzr2.HasNext() )
        UT_TRUE( tknzr.HasNext() )

        tknzr2.Set( tknzr.Next(), ',');
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR("5"), res )  UT_TRUE(  tknzr2.HasNext() )
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR(""),  res )  UT_TRUE( !tknzr2.HasNext() )
        UT_TRUE( tknzr.HasNext() )

        tknzr2.Set( tknzr.Next(), ',');
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR(""), res )   UT_TRUE(  tknzr2.HasNext() )
        res.Reset( tknzr2.Next() ); UT_EQ( A_CHAR(""), res )   UT_TRUE( !tknzr2.HasNext() )
        UT_TRUE( !tknzr.HasNext() )
    }
}


#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif // ALIB_UT_STRINGS

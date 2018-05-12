// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"



#define TESTCLASSNAME       CPP_ALib_Strings_Substring
#include "aworx_unittests.hpp"
#include "alib/compatibility/std_string.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

// Windows.h might bring in max/min macros
#if defined( max )
    #undef max
    #undef min
#endif

using namespace std;
using namespace aworx;



namespace ut_aworx {


UT_CLASS()

//--------------------------------------------------------------------------------------------------
//--- Test Substring
//--------------------------------------------------------------------------------------------------
void substringConstruction( const character* inputString, AString& res, bool trim )
{
    Substring subs( inputString );
    if (trim)
        subs.Trim();
    res.Clear()._(subs);
}

UT_METHOD( Constructor )
{
    UT_INIT();

    AString as;
    AString res;

    substringConstruction( ASTR("a")        , res, false ); UT_EQ( ASTR("a"),        res );
    substringConstruction( ASTR(" a")       , res, false ); UT_EQ( ASTR(" a"),       res );
    substringConstruction( ASTR("a ")       , res, false ); UT_EQ( ASTR("a "),       res );
    substringConstruction( ASTR("a b")      , res, false ); UT_EQ( ASTR("a b"),      res );
    substringConstruction( ASTR(" a b")     , res, false ); UT_EQ( ASTR(" a b"),     res );
    substringConstruction( ASTR("a b ")     , res, false ); UT_EQ( ASTR("a b "),     res );

    substringConstruction( ASTR("a")        , res, true  ); UT_EQ( ASTR("a"),        res );
    substringConstruction( ASTR(" a")       , res, true  ); UT_EQ( ASTR("a"),        res );
    substringConstruction( ASTR("a ")       , res, true  ); UT_EQ( ASTR("a"),        res );
    substringConstruction( ASTR("a b")      , res, true  ); UT_EQ( ASTR("a b"),      res );
    substringConstruction( ASTR(" a b")     , res, true  ); UT_EQ( ASTR("a b"),      res );
    substringConstruction( ASTR("a b ")     , res, true  ); UT_EQ( ASTR("a b"),      res );

    // changing whitespaces
    {
        {
            as= ASTR("xy xz abc xy");
            Substring subs;
            (subs= as).Trim( ASTR("xy "));
            res= subs;
            UT_EQ( ASTR("z abc"), res );
        }
        {
            Substring subs( ASTR("xy xz abc xy") );
            res= subs.TrimStart( ASTR("xy ") )
                     .TrimEnd( ASTR("xy ") );
            UT_EQ( ASTR("z abc"), res );
        }
    }

    // test other constructors
    {
        {
            Substring null;
            UT_EQ( true,      null.IsEmpty() );
            UT_EQ( true,      null.IsNull()  );
        }

        {
            Substring subs( ASTR(" const char* ") );
            res= subs.Trim();
            UT_EQ( ASTR("const char*"), res );
        }
        {
            Substring subs= String(ASTR("const char* ")).Substring(0, 5);
            res= subs.Trim();
            UT_EQ( ASTR("const"), res );
        }

        {
            String64 astr( ASTR(" astring "));
            Substring subs( astr);
            res= subs.Trim();
            UT_EQ( ASTR("astring"), res );

            Substring subs2= astr.Substring( 2, 3 );
            res= subs2.Trim();
            UT_EQ( ASTR("str"), res );

            Substring subs3= astr.Substring( 20, 3 );
            res= subs3.Trim();
            UT_TRUE(  subs3.IsEmpty() );
            UT_TRUE( !subs3.IsNull()  );
        }
    }
}

UT_METHOD( BinaryOps )
{
    UT_INIT();


    AString as;
    Substring subs( ASTR(" word   ") );
    as << subs.Trim() << '@' << subs;

    UT_EQ( ASTR("word@word"),          as );
    as << '@';
    as << subs;
    UT_EQ( ASTR("word@word@word"),     as );

    std::basic_stringstream<aworx::character> sstr;
    sstr << subs;
    UT_EQ( ASTR("word"),        sstr.str().c_str() );

}

UT_METHOD( CharAt)
{
    UT_INIT();
    // empty substring
    {
        Substring subs;
        UT_EQ(  '\0',      subs.CharAtStart() );
        UT_EQ(  '\0',      subs.CharAtEnd() );
    }

    // Consume/PopLast
    {
        Substring subs(ASTR("abcde"));
        UT_EQ('a',      subs.CharAtStart()           );
        UT_EQ('e',      subs.CharAtEnd ()            );
        UT_EQ('a',      subs.CharAtStart<false>()    );
        UT_EQ('e',      subs.CharAtEnd <false>()     );
        UT_EQ('a',      subs.ConsumeChar()               );
        UT_EQ('e',      subs.ConsumeCharFromEnd()        );
        UT_EQ('d',      subs.ConsumeCharFromEnd()        );
        UT_EQ('b',      subs.ConsumeChar()               );
        UT_EQ('c',      subs.ConsumeChar()               );
        UT_EQ('\0',     subs.ConsumeChar()               );
        UT_EQ('\0',     subs.ConsumeChar()               );
        UT_EQ('\0',     subs.ConsumeCharFromEnd()        );
        UT_EQ('\0',     subs.ConsumeCharFromEnd()        );
    }

    // Delete first
    {
        Substring subs(ASTR("1234567890"));
        subs.ConsumeChars<false>        (2) ;        UT_TRUE( subs == ASTR("34567890")  );
        subs.ConsumeCharsFromEnd <false>(3) ;        UT_TRUE( subs == ASTR("34567")     );
        subs.ConsumeChars               (2) ;        UT_TRUE( subs == ASTR("567")    );
        subs.ConsumeCharsFromEnd        (3) ;        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeChars<false>        (0) ;        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeCharsFromEnd <false>(0) ;        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeChars               (0) ;        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeCharsFromEnd        (0) ;        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeChars               (-2);        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeCharsFromEnd        (-2);        UT_TRUE( subs == ASTR("")        );
        subs.ConsumeChars               (20);   UT_EQ  ( 0, subs.Length() );
        subs= ASTR("1234567890");
        subs.ConsumeCharsFromEnd        (20);   UT_EQ  ( 0, subs.Length() );
    }

}

UT_METHOD( ConsumeNumbers )
{
    UT_INIT();

    // ConsumeDecimal()
    {
        Substring subs;
        int result;
                                   UT_EQ( false,  subs.ConsumeInt( result      ) );   UT_EQ(       0,  result );
        subs= ASTR("")           ; UT_EQ( false,  subs.ConsumeInt( result      ) );   UT_EQ(       0,  result );
        subs= ASTR("  ABC")      ; UT_EQ( false,  subs.ConsumeInt( result      ) );   UT_EQ(       0,  result );
        subs= ASTR("  12345")    ; UT_EQ( true ,  subs.ConsumeInt( result      ) );   UT_EQ(   12345,  result );
        subs= ASTR("  12 45")    ; UT_EQ( true ,  subs.ConsumeInt( result      ) );   UT_EQ(      12,  result );
                                   UT_EQ( true ,  subs.ConsumeInt( result      ) );   UT_EQ(      45,  result );

        subs= ASTR(" 42 ; 7 ; 6 ");UT_EQ( true ,  subs.ConsumeInt( result      ) );   UT_EQ(      42,  result );
                                   UT_EQ( false,  subs.ConsumeInt( result      ) );   UT_EQ(       0,  result );
                                   UT_EQ( false,  subs.ConsumeInt( result      ) );   UT_EQ(       0,  result );
        NumberFormat nf;    nf.Whitespaces= ASTR(" ;");
        subs= ASTR(" 42 ; 7 ; 6 ");UT_EQ( true ,  subs.ConsumeInt( result, &nf ) );   UT_EQ(      42,  result );
                                   UT_EQ( true ,  subs.ConsumeInt( result, &nf ) );   UT_EQ(       7,  result );
                                   UT_EQ( true ,  subs.ConsumeInt( result, &nf ) );   UT_EQ(       6,  result );
                                   UT_EQ( false,  subs.ConsumeInt( result, &nf ) );   UT_EQ(       0,  result );
                                   UT_EQ( false,  subs.ConsumeInt( result, &nf ) );   UT_EQ(       0,  result );
    }

    // ConsumeFloat()
    {
        Substring subs;
        double result;
                                   UT_EQ( false,  subs.ConsumeFloat  ( result      ) );   UT_EQ(      0.,  result );
        subs= ASTR("")           ; UT_EQ( false,  subs.ConsumeFloat  ( result      ) );   UT_EQ(      0.,  result );
        subs= ASTR("  ABC")      ; UT_EQ( false,  subs.ConsumeFloat  ( result      ) );   UT_EQ(      0.,  result );
        subs= ASTR("  12345")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) );   UT_EQ(  12345.,  result );
        subs= ASTR(" 12.45 ")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) );   UT_EQ(  12.45,   result );
        subs= ASTR("  12 45")    ; UT_EQ( true ,  subs.ConsumeFloat  ( result      ) );   UT_EQ(     12.,  result );
                                   UT_EQ( true ,  subs.ConsumeFloat  ( result      ) );   UT_EQ(     45.,  result );

        NumberFormat nf;    nf.Whitespaces= ASTR(" ;");
        subs= ASTR(" 42.3 ; 0.7 ; 6 ") ; UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) );   UT_EQ(    42.3,  result );
                                   UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) );   UT_EQ(     0.7,  result );
                                   UT_EQ( true ,  subs.ConsumeFloat  ( result, &nf ) );   UT_EQ(      6.,  result );
                                   UT_EQ( false,  subs.ConsumeFloat  ( result, &nf ) );   UT_EQ(      0.,  result );
                                   UT_EQ( false,  subs.ConsumeFloat  ( result, &nf ) );   UT_EQ(      0.,  result );
    }
}

//--------------------------------------------------------------------------------------------------
//--- Test Consume
//--------------------------------------------------------------------------------------------------
UT_METHOD( Consume )
{
    UT_INIT();
    // null substring
    {
        Substring s;
        Substring r(ASTR("oldval"));
        UT_EQ(  '\0',      s.ConsumeChar         ( )         );
        UT_EQ(  0,         s.ConsumeChars        (  0  , &r) ); UT_TRUE(r.IsNull());
        UT_EQ(  0,         s.ConsumeChars        (  5  , &r) ); UT_TRUE(r.IsNull());
        UT_EQ(  false,     s.ConsumeChar         ( 'a' )     );
        UT_EQ(  false,     s.ConsumeString       ( ASTR("word") )  );
        UT_EQ(  '\0',      s.ConsumeCharFromEnd  ( )         );
        UT_EQ(  0,         s.ConsumeCharsFromEnd (  0  )     );
        UT_EQ(  0,         s.ConsumeCharsFromEnd (  5  )     );
        UT_EQ(  false,     s.ConsumeCharFromEnd  ( 'a' )     );
        UT_EQ(  false,     s.ConsumeStringFromEnd( ASTR("word") )  );
    }

    // empty substring
    {
        Substring s(EmptyString);
        Substring r(ASTR("oldval"));
        UT_EQ(  '\0',  s. ConsumeChar         ( )         );
        UT_EQ(  0,     s. ConsumeChars        (  0   ,&r) ); UT_TRUE( r.IsNotNull()); UT_TRUE(r.IsEmpty());
        UT_EQ(  0,     s. ConsumeChars        (  5   ,&r) ); UT_TRUE( r.IsNotNull()); UT_TRUE(r.IsEmpty());
        UT_EQ(  false, s. ConsumeChar         ( 'a' )     );
        UT_EQ(  false, s. ConsumeString       ( ASTR("word") )  );
        UT_EQ(  '\0',  s. ConsumeCharFromEnd  ( )         );
        UT_EQ(  0,     s. ConsumeCharsFromEnd (  0  )     );
        UT_EQ(  0,     s. ConsumeCharsFromEnd (  5  )     );
        UT_EQ(  false, s. ConsumeCharFromEnd  ( 'a' )     );
        UT_EQ(  false,  s.ConsumeStringFromEnd( ASTR("word") )  );
    }

    // substring of length 1
    {
        Substring s;
        Substring r(ASTR("oldval"));
        s= ASTR("a"); UT_EQ ( 'a',   s. ConsumeChar          (         ) ) ; UT_EQ ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 1,     s. ConsumeChars         ( 0       ) ) ; UT_EQ ( 1,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 0,     s. ConsumeChars         ( 1  , &r ) ) ; UT_EQ ( 0,        s. Length ( ) ) ; UT_TRUE ( r.Equals ( ASTR("a") ) ) ;
        s= ASTR("a"); UT_EQ ( 0,     s. ConsumeChars         ( 5  , &r ) ) ; UT_EQ ( 0,        s. Length ( ) ) ; UT_TRUE ( r.Equals ( ASTR("a") ) ) ;
        s= ASTR("a"); UT_EQ ( true,  s. ConsumeChar          ( 'a'     ) ) ; UT_EQ ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( false, s. ConsumeChar          ( 'b'     ) ) ; UT_EQ ( 1,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( false, s. ConsumeString        ( ASTR("word")  ) ) ; UT_EQ ( 1,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 'a',   s. ConsumeCharFromEnd   ( ) ) ; UT_EQ         ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 1,     s. ConsumeCharsFromEnd  ( 0       ) ) ; UT_EQ ( 1,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 0,     s. ConsumeCharsFromEnd  ( 1       ) ) ; UT_EQ ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( 0,     s. ConsumeCharsFromEnd  ( 5       ) ) ; UT_EQ ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( true,  s. ConsumeCharFromEnd   ( 'a'     ) ) ; UT_EQ ( 0,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( false, s. ConsumeCharFromEnd   ( 'b'     ) ) ; UT_EQ ( 1,        s. Length ( ) ) ;
        s= ASTR("a"); UT_EQ ( false, s. ConsumeStringFromEnd ( ASTR("word")  ) ) ; UT_EQ ( 1, s. Length        ( ) ) ;
    }

    // substring of length 2
    {
        Substring s;
        Substring r(ASTR("oldval"));

        s= ASTR("ab"); UT_EQ( 'a',   s.ConsumeChar        ( )         ); UT_EQ( 1,s.Length() );
                 UT_EQ( 'b',   s.ConsumeChar        ( )         ); UT_EQ( 0,s.Length() );
        s= ASTR("ab"); UT_EQ( 'b',   s.ConsumeCharFromEnd ( )         ); UT_EQ( 1,s.Length() );
                 UT_EQ( 'a',   s.ConsumeCharFromEnd ( )         ); UT_EQ( 0,s.Length() );

        s= ASTR("ab"); UT_EQ(  2,    s.ConsumeChars       (  0  ,&r)  ); UT_EQ( 2,s.Length() ); UT_TRUE(r.IsNotNull()); UT_TRUE(r.IsEmpty());
        s= ASTR("ab"); UT_EQ(  1,    s.ConsumeChars       (  1  ,&r)  ); UT_EQ( 1,s.Length() ); UT_TRUE(r.Equals(ASTR("a") ));
        s= ASTR("ab"); UT_EQ(  0,    s.ConsumeChars       (  2  ,&r)  ); UT_EQ( 0,s.Length() ); UT_TRUE(r.Equals(ASTR("ab")));
        s= ASTR("ab"); UT_EQ(  0,    s.ConsumeChars       (  3  ,&r)  ); UT_EQ( 0,s.Length() ); UT_TRUE(r.Equals(ASTR("ab")));
        s= ASTR("ab"); UT_EQ(  2,    s.ConsumeCharsFromEnd(  0  ,&r)  ); UT_EQ( 2,s.Length() ); UT_TRUE(r.IsNotNull()); UT_TRUE(r.IsEmpty());
        s= ASTR("ab"); UT_EQ(  1,    s.ConsumeCharsFromEnd(  1  ,&r)  ); UT_EQ( 1,s.Length() ); UT_TRUE(r.Equals(ASTR("b") ));
        s= ASTR("ab"); UT_EQ(  0,    s.ConsumeCharsFromEnd(  2  ,&r)  ); UT_EQ( 0,s.Length() ); UT_TRUE(r.Equals(ASTR("ab")));
        s= ASTR("ab"); UT_EQ(  0,    s.ConsumeCharsFromEnd(  3  ,&r)  ); UT_EQ( 0,s.Length() ); UT_TRUE(r.Equals(ASTR("ab")));

        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeChar       ( 'b' )     ); UT_EQ( 2,s.Length() );
                 UT_EQ(  true,      s.ConsumeChar       ( 'a' )     ); UT_EQ( 1,s.Length() );
                 UT_EQ(  true,      s.ConsumeChar       ( 'b' )     ); UT_EQ( 0,s.Length() );
                 UT_EQ(  false,     s.ConsumeChar       ( 'a' )     ); UT_EQ( 0,s.Length() );
                 UT_EQ(  false,     s.ConsumeChar       ( 'b' )     ); UT_EQ( 0,s.Length() );
        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeCharFromEnd( 'a' )     ); UT_EQ( 2,s.Length() );
                 UT_EQ(  true,      s.ConsumeCharFromEnd( 'b' )     ); UT_EQ( 1,s.Length() );
                 UT_EQ(  true,      s.ConsumeCharFromEnd( 'a' )     ); UT_EQ( 0,s.Length() );
                 UT_EQ(  false,     s.ConsumeCharFromEnd( 'b' )     ); UT_EQ( 0,s.Length() );
                 UT_EQ(  false,     s.ConsumeCharFromEnd( 'a' )     ); UT_EQ( 0,s.Length() );

        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeString     ( ASTR("word") )  ); UT_EQ( 2,s.Length() );
        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeString     ( ASTR("AB")   )  ); UT_EQ( 2,s.Length() );
        s= ASTR("ab"); UT_EQ(  true,      s.ConsumeString     ( ASTR("ab")   )  ); UT_EQ( 0,s.Length() );
        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeStringFromEnd     ( ASTR("word") )  ); UT_EQ( 2,s.Length() );
        s= ASTR("ab"); UT_EQ(  false,     s.ConsumeStringFromEnd     ( ASTR("AB")   )  ); UT_EQ( 2,s.Length() );
        s= ASTR("ab"); UT_EQ(  true,      s.ConsumeStringFromEnd     ( ASTR("ab")   )  ); UT_EQ( 0,s.Length() );
    }

    // 3 words
    {
        Substring s(ASTR("word1 word2 word3"));

        UT_EQ(  'w',       s.ConsumeChar   ( )         );
        UT_EQ(  'o',       s.ConsumeChar   ( )         );
        UT_EQ(  'r',       s.ConsumeChar   ( )         );
        UT_EQ(  'd',       s.ConsumeChar   ( )         );
        UT_EQ(  '1',       s.ConsumeChar   ( )         );

        UT_EQ(  false    , s.ConsumeChar                                       ('w'  ) );
        UT_EQ(  true     , s.ConsumeChar  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>   ('w'  ) );
        UT_EQ(  true     , s.ConsumeChar  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>   ('o'  ) );
        UT_EQ(  false    , s.ConsumeChar  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>   ('o'  ) );
        UT_EQ(  true     , s.ConsumeChar  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>   ('r'  ) );
        UT_EQ(  false    , s.ConsumeString<Case::Sensitive ALIB_COMMA Whitespaces::Trim>   (ASTR("D2") ) );
        UT_EQ(  false    , s.ConsumeString                                                 (ASTR("D2") ) );
        UT_EQ(  true     , s.ConsumeString                                                 (ASTR("d2") ) );

        UT_EQ(  2   , s.ConsumeChars  ( 4 )   );
        UT_EQ(  ASTR("d3")     , s );

        s= ASTR("word1 word2 word3");

        UT_EQ(  '3',       s.ConsumeCharFromEnd( )         );
        UT_EQ(  'd',       s.ConsumeCharFromEnd( )         );
        UT_EQ(  'r',       s.ConsumeCharFromEnd( )         );
        UT_EQ(  'o',       s.ConsumeCharFromEnd( )         );
        UT_EQ(  'w',       s.ConsumeCharFromEnd( )         );

        UT_EQ(  false    , s.ConsumeCharFromEnd                                    ('2')   );
        UT_EQ(  true     , s.ConsumeCharFromEnd  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>('2')  );
        UT_EQ(  true     , s.ConsumeCharFromEnd  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>('d')  );
        UT_EQ(  false    , s.ConsumeCharFromEnd  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>('d')  );
        UT_EQ(  true     , s.ConsumeCharFromEnd  <Case::Sensitive ALIB_COMMA Whitespaces::Trim>('r')  );
        UT_EQ(  false    , s.ConsumeStringFromEnd<Case::Sensitive ALIB_COMMA Whitespaces::Trim>(ASTR("WO")) );
        UT_EQ(  false    , s.ConsumeStringFromEnd<Case::Sensitive ALIB_COMMA Whitespaces::Trim>(ASTR("WO")) );
        UT_EQ(  true     , s.ConsumeStringFromEnd<Case::Sensitive ALIB_COMMA Whitespaces::Trim>(ASTR("wo")) );

        UT_EQ(  2   , s.ConsumeCharsFromEnd( 4 )   );
        UT_EQ(  ASTR("wo")     , s );
    }

    // Tokens
    {
        Substring s(ASTR("word1 word2 word3"));
        UT_EQ( ASTR("word1"), s.ConsumeToken(' ') );
UT_EQ( ASTR("word2 word3"), s );
        UT_EQ( ASTR("word2"), s.ConsumeToken(' ') );    UT_EQ( ASTR("word3")      , s );
        UT_EQ( ASTR("word3"), s.ConsumeToken(' ') );    UT_EQ( 0, s.Length() );
        UT_EQ( EmptyString     , s.ConsumeToken(' ') );    UT_EQ( 0, s.Length() );
        UT_EQ( EmptyString     , s.ConsumeToken(' ') );    UT_EQ( 0, s.Length() );
    }


    {
        Substring subs(ASTR("1234567890"));
        UT_EQ( 0, subs.ConsumePartOf(ASTR("abc")) );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("abc"), 2) );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("abc"), 5) );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("125"), 5) );       UT_EQ( ASTR("1234567890"), subs );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("125"), 3) );       UT_EQ( ASTR("1234567890"), subs );
        UT_EQ( 2, subs.ConsumePartOf(ASTR("125"), 2) );       UT_EQ(   ASTR("34567890"), subs );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("125"))    );       UT_EQ(   ASTR("34567890"), subs );
        UT_EQ( 2, subs.ConsumePartOf(ASTR("34") )    );       UT_EQ(     ASTR("567890"), subs );
        UT_EQ( 1, subs.ConsumePartOf(ASTR("59") )    );       UT_EQ(      ASTR("67890"), subs );
        UT_EQ( 3, subs.ConsumePartOf(ASTR("678"), 2 ));       UT_EQ(         ASTR("90"), subs );
        UT_EQ( 2, subs.ConsumePartOf(ASTR("90")     ));       UT_EQ(           ASTR(""), subs );
        UT_EQ( 0, subs.ConsumePartOf(ASTR("90")     ));       UT_EQ(           ASTR(""), subs );
        UT_EQ( 0, subs.ConsumePartOf(EmptyString));           UT_EQ(           ASTR(""), subs );
    }

    {
        Substring subs(ASTR("abcdef"));
        UT_EQ( 0, subs.ConsumePartOf<Case::Sensitive>(ASTR("ABC"), 1)  );
        UT_EQ( 0, subs.ConsumePartOf<Case::Sensitive>(ASTR("aBC"), 2)  );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR("aBC"), 5)  );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR("aBX"), 3)  );       UT_EQ( ASTR("abcdef"), subs );
        UT_EQ( 0, subs.ConsumePartOf<Case::Sensitive>(ASTR("aBX"), 2)  );       UT_EQ( ASTR("abcdef"), subs );
        UT_EQ( 2, subs.ConsumePartOf<Case::Ignore   >(ASTR("aBX"), 2)  );       UT_EQ(   ASTR("cdef"), subs );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR("CXX"), 2)  );       UT_EQ(   ASTR("cdef"), subs );
        UT_EQ( 1, subs.ConsumePartOf<Case::Ignore   >(ASTR("CXX"), 1)  );       UT_EQ(    ASTR("def"), subs );
        UT_EQ( 3, subs.ConsumePartOf<Case::Ignore   >(ASTR("def"), 2)  );       UT_EQ(       ASTR(""), subs );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR("def"), 2)  );       UT_EQ(       ASTR(""), subs );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR(""),    2)  );       UT_EQ(       ASTR(""), subs );
        UT_EQ( 0, subs.ConsumePartOf<Case::Ignore   >(ASTR(""),    0)  );       UT_EQ(       ASTR(""), subs );
    }


}


//--------------------------------------------------------------------------------------------------
//--- Tokenizer
//--------------------------------------------------------------------------------------------------
void tokenizerTest( const aworx::character* inputString, AString& res, aworx::character delim, aworx::character newDelim,
                    Whitespaces trim, int inpStart= -1, int inpEnd= -1  )
{
    Substring inp( inputString );
    if ( inpStart < 0 )  inpStart= 0;
    if ( inpEnd   < 0 )  inpEnd=   static_cast<int>(inp.Length() - 1);
    inp = inp.Substring( inpStart, inpEnd-inpStart +1 );

    res.Clear();

    Tokenizer tknzr( inp, delim );

    while( tknzr.HasNext() )
    {
        res._( tknzr.Next(trim) );
        res._( newDelim );
    }

}

UT_METHOD( Tokenize )
{
    UT_INIT();

    AString as;
    AString res;

    // tokenizing empty string
    as.Clear()._( ASTR("") );
    res.Clear();
    {
        Tokenizer tknzr( as, ',' );  UT_EQ( true,  tknzr.HasNext() );
        res._( tknzr.Next() );                UT_EQ( ASTR(""), res );
    }

    // tokenizing no delim
    as.Clear()._( ASTR("abc") );
    res.Clear();
    {
        Tokenizer tknzr( as, ',' );           UT_EQ( true,  tknzr.HasNext() );
        res._( tknzr.Next() );                UT_EQ( ASTR("abc"), res );
    }

    // tokenizing
    {
        tokenizerTest( ASTR("abc"),                   res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("abc@")                , res );
        tokenizerTest( ASTR("a,bc"),                  res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("a@bc@")               , res );
        tokenizerTest( ASTR("a,bc"),                  res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("a@bc@")               , res );
        tokenizerTest( ASTR(","),                     res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("@@")                  , res );
        tokenizerTest( ASTR(",,"),                    res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("@@@")                 , res );
        tokenizerTest( ASTR("a,b,c,,"),               res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("a@b@c@@@")            , res );
        tokenizerTest( ASTR("a,b,c"),                 res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("a@b@c@")              , res );
        tokenizerTest( ASTR(",a,b,c"),                res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("@a@b@c@")             , res );
        tokenizerTest( ASTR("123567"),                res, ',', '@',   Whitespaces::Trim,   2,   2 );   UT_EQ( ASTR("3@")                  , res );
        tokenizerTest( ASTR("123567"),                res, ',', '@',   Whitespaces::Trim,   2,   1 );   UT_EQ( ASTR("@")                   , res );
        tokenizerTest( ASTR("  abc , def , ghe "),    res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("abc@def@ghe@")        , res );
        tokenizerTest( ASTR("abc , def,ghe,"),        res, ',', '@',   Whitespaces::Trim,  -1,  -1 );   UT_EQ( ASTR("abc@def@ghe@@")       , res );
        tokenizerTest( ASTR("  abc , def , ghe "),    res, ',', '@',   Whitespaces::Keep,  -1,  -1 );   UT_EQ( ASTR("  abc @ def @ ghe @") , res );
        tokenizerTest( ASTR("abc , def,ghe,"),        res, ',', '@',   Whitespaces::Keep,  -1,  -1 );   UT_EQ( ASTR("abc @ def@ghe@@")     , res );
    }

    // tokenizing with different delimiters
    {
        as.Clear()._( ASTR("1,5;3@4") );
        Tokenizer tknzr(as, ',');
        res= tknzr.Next();                            UT_EQ( ASTR("1"),   res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next( Whitespaces::Trim, ';' );    UT_EQ( ASTR("5"),   res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next( Whitespaces::Trim, '@' );    UT_EQ( ASTR("3"),   res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next( Whitespaces::Trim, '-' );    UT_EQ( ASTR("4"),   res );  UT_EQ( false, tknzr.HasNext() );
    }

    // tokenizing with different delimiters
    {
        as.Clear()._( ASTR("abc, 5;\t3;;; 4  ") );
        Tokenizer tknzr(as,',');
        res= tknzr.Next();                          UT_EQ( ASTR("abc"), res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next( Whitespaces::Trim, ';' );  UT_EQ( ASTR("5"),   res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next();                          UT_EQ( ASTR("3"),   res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.Next();                          UT_EQ( ASTR(""),    res );  UT_EQ( true,  tknzr.HasNext() );
        res= tknzr.GetRest();                       UT_EQ( ASTR("; 4"), res );  UT_EQ( false, tknzr.HasNext() );
    }

    // sub-tokens
    {
        as.Clear()._( ASTR("1,2;3 , 4;5,;,") );
        Tokenizer tknzr(as, ';');

        Tokenizer tknzr2( tknzr.Next(), ',');
        res= tknzr2.Next(); UT_EQ( ASTR("1"), res );  UT_TRUE(  tknzr2.HasNext() );
        res= tknzr2.Next(); UT_EQ( ASTR("2"), res );  UT_TRUE( !tknzr2.HasNext() );
        UT_TRUE( tknzr.HasNext() );

        tknzr2.Set( tknzr.Next(), ',');
        res= tknzr2.Next(); UT_EQ( ASTR("3"), res );  UT_TRUE(  tknzr2.HasNext() );
        res= tknzr2.Next(); UT_EQ( ASTR("4"), res );  UT_TRUE( !tknzr2.HasNext() );
        UT_TRUE( tknzr.HasNext() );

        tknzr2.Set( tknzr.Next(), ',');
        res= tknzr2.Next(); UT_EQ( ASTR("5"), res );  UT_TRUE(  tknzr2.HasNext() );
        res= tknzr2.Next(); UT_EQ( ASTR(""),  res );  UT_TRUE( !tknzr2.HasNext() );
        UT_TRUE( tknzr.HasNext() );

        tknzr2.Set( tknzr.Next(), ',');
        res= tknzr2.Next(); UT_EQ( ASTR(""), res );  UT_TRUE(  tknzr2.HasNext() );
        res= tknzr2.Next(); UT_EQ( ASTR(""), res );  UT_TRUE( !tknzr2.HasNext() );
        UT_TRUE( !tknzr.HasNext() );
    }
}


UT_CLASS_END

}; //namespace

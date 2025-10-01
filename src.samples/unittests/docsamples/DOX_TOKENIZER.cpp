// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_STRINGS

#include "ALib.ALox.H"

#include "ALib.Strings.Tokenizer.H"
#include "ALib.Strings.StdIOStream.H"
#include "ALib.Compatibility.StdStrings.H"

#define TESTCLASSNAME       UT_Dox_Tokenizer
#include "aworx_unittests.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace alib;

namespace ut_aworx {

UT_CLASS

//##################################################################################################
// SAMPLE code of class documentation
//##################################################################################################

void documentationSampleTokenizer(AWorxUnitTesting& ut)
{
    std::stringstream testOutputStream;
    #define cout testOutputStream

DOX_MARKER( [DOX_TOKENIZER])
    // data string to tokenize
    String data=  A_CHAR("test;  abc ; 1,2 , 3 ; xyz ; including;separator");

    // create tokenizer on data with ';' as delimiter
    Tokenizer tknzr( data, ';' );

    // read tokens
    cout << tknzr.Next() << endl; // will print "test"
    cout << tknzr.Next() << endl; // will print "abc"
    cout << tknzr.Next() << endl; // will print "1,2 , 3"

    // tokenize actual (third) token (nested tokenizer)
    Tokenizer subTknzr( tknzr.Actual,  ',');
    cout << subTknzr.Next();

    while( subTknzr.HasNext() )
        cout << '~' << subTknzr.Next();

    cout << endl;

    // continue with the main tokenizer
    cout << tknzr.Next() << endl; // will print "xyz"

    // grab the rest, as we know that the last token might include our separator character
    cout << tknzr.GetRest()      << endl; // will print "including;separator"
DOX_MARKER( [DOX_TOKENIZER])

    #undef cout
    ut.WriteResultFile( "DOX_TOKENIZER.txt", testOutputStream.str() ,"" );
}


UT_METHOD( DOX_TOKEN )
{
    UT_INIT()

    UT_PRINT("*** Documentation Sample +**")
    documentationSampleTokenizer( ut );
}

#include "aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_STRINGS

// #################################################################################################
//  aworx - Unit Tests
//  Documentation sample for ALib Expressions: Calculator
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alox.hpp"
#include "../aworx_unittests.hpp"


#define TESTCLASSNAME       CPP_ALib_Dox_Expr_Calculator


// Preparations to fake std::cout, main(), etc.
#include <sstream>
#include <iostream>
#include <iomanip>

namespace std { namespace {   basic_stringstream<aworx::character> testOutputStream;    } }
#define cout testOutputStream
#define main dox_calculator_sample


// #################################################################################################
// #### Tutorial code
// #################################################################################################
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable:4100 ) // unreferenced formal parameter
#endif

//! [DOX_ALIB_EXPR_TUT_CALC_MAIN]
#include "alib/alib.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/compatibility/std_string.hpp"
#include "alib/strings/format/simpletext.hpp"

using namespace std;
using namespace aworx;

int main( int argc, character *argv[] )
{
    // 1. Create the expression compiler
    //    Add all built-in stuff: number arithmetics, strings, time/date, etc.
    Compiler compiler;
    compiler.SetupDefaults();

    // 2. Compile. Catch exceptions please, never trust your user
    SPExpression expression;
    try
    {
        expression= compiler.Compile( argv[1] );
    }
    catch (Exception& e)
    {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << SimpleText().AddException(e).Text                                 << endl;

        return e.Code().Value();
    }

    // 2. We need an evaluation "scope"
    //    (later we will use a custom type here, that allows custom identifiers, functions and
    //    operators to access application data)
    lib::expressions::Scope scope(compiler.CfgFormatter);

    // 4. Evaluate the expression
    //    (We must not fear exceptions here, as the compiler did all type checking, and resolved
    //    everything to a duly checked internal "program" running on a virtual machine.)
    Box result= expression->Evaluate( scope );

    // 5. Write result
    cout << "Input:      " << expression->GetOriginalString()   << endl;
    cout << "Normalized: " << expression->GetNormalizedString() << endl;
    cout << "Result:     " << result                            << endl;
    return 0;
}
//! [DOX_ALIB_EXPR_TUT_CALC_MAIN]

ALIB_WARNINGS_RESTORE
#undef main

#include "alib/expressions/detail/program.hpp"

// #################################################################################################
// #### WriteOrigNormalizedAndOptimized()
// #################################################################################################
namespace{
void WriteOrigNormalizedAndOptimized(const String& expressionString)
{
    Compiler compiler;
    compiler.SetupDefaults();
    SPExpression expression;
    try
    {
        expression= compiler.Compile( expressionString );
    }
    catch (Exception& e)
    {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << SimpleText().AddException(e).Text                                 << endl;
        return;
    }

    cout << "Input:          " << expression->GetOriginalString()    << endl;
    cout << "Normalized:     " << expression->GetNormalizedString()  << endl;
    cout << "Optimized:      " << expression->GetOptimizedString()   << endl;
    cout << "Program Length: " << expression->GetProgram()->Length() << endl;
}}

// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

namespace ut_aworx {

UT_CLASS()


void invokeCalculator( AWorxUnitTesting& ut, const character* expression, int fileNo )
{
    const character* argc[]= {ASTR(""), expression };
    dox_calculator_sample( 1, reinterpret_cast<character**>(&argc) );
    ut.WriteResultFile( NString128("DOX_ALIB_EXPR_TUT_CALC_MAIN-") << fileNo<< ".txt",
                        testOutputStream.str() );
    testOutputStream.str(ASTR(""));
}
void invokeNormalizedAndOptimized( AWorxUnitTesting& ut, const character* expression, int fileNo )
{
    WriteOrigNormalizedAndOptimized( expression );
    ut.WriteResultFile( NString128("DOX_ALIB_EXPRESSIONS_TUT_WONO-") << fileNo<< ".txt",
                        testOutputStream.str() );
    testOutputStream.str(ASTR(""));
}


UT_METHOD( Calculator )
{
    UT_INIT();

    try{

    invokeCalculator( ut, ASTR("1 + 2 * 3")                                      , 1 );
    invokeCalculator( ut, ASTR("1 * 2 + 3")                                      , 2 );
    invokeCalculator( ut, ASTR("true && false == true < false")                  , 3 );
    invokeCalculator( ut, ASTR("asin(1.0) * 2.0")                                , 4 );
    invokeCalculator( ut, ASTR("tolo(\"Hello \") + toup(\"World\")")             , 5 );
    invokeCalculator( ut, ASTR("Format( \"Today is: {:yyyy/MM/dd}\", today )")   , 6 );


    // samples used with boxing interface IToLiteral
    invokeNormalizedAndOptimized( ut, ASTR("Milliseconds(1)")                          , 100 );
    invokeNormalizedAndOptimized( ut, ASTR("Milliseconds(1) * 1000")                   , 101 );
    invokeNormalizedAndOptimized( ut, ASTR("Minutes(18) + Seconds(23)")                , 102 );


    }
    catch(Exception e)
    {
        UT_PRINT( "An ALib exception occurred during test run." );
        UT_PRINT( e );
        UT_PRINT( "Stopping..." );
        UT_TRUE(false);
    }
}



UT_CLASS_END

} //namespace


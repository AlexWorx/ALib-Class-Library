// #################################################################################################
//  aworx - Unit Tests
//  Documentation sample for ALib Expressions: Calculator
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS


// Preparations to fake std::cout, main(), etc.
#include <sstream>
#include <iostream>
#include <iomanip>

namespace std { namespace {   basic_stringstream<char> testOutputStreamEC;    } }
#define cout testOutputStreamEC
#define main fakemain


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
// Using the expression compiler in this code entity, as well as the evaluation of expressions.
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/scope.hpp"

// Get support for writing expression result values to std::cout
#include "alib/compatibility/std_strings_iostream.hpp"

// ALib Exception class
#include "alib/results/exception.hpp"

// ALib module initialization (has to be done in main())
#include "alib/distribution.hpp"

// std::cout
#include <iostream>

using namespace std;
using namespace aworx;

//----- The Command Line Calculator Program -----
int main( int argc, const char **argv )
{
    // 0. Initialize ALib (this has to be done once at bootstrap with any software using ALib)
    aworx::ALIB.Bootstrap(argc, argv);
    aworx::ALIB.CheckDistribution();

    // 1. Create a defaulted expression compiler. This adds all built-in stuff, like number
    //    arithmetics, strings, time/date, etc.
    Compiler compiler;
    compiler.SetupDefaults();

    // 2. Compile. Catch exceptions (must not trust user input)
    SPExpression expression;
    try
    {
        ALIB_STRINGS_FROM_NARROW( argv[1], expressionString, 256 )

        expression= compiler.Compile( expressionString );
    }
    catch (Exception& e)
    {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << e  << endl;
        return static_cast<int>( e.Type().Integral() );
    }

    // 3. We need an evaluation "scope"
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

    // 6. Terminate library
    aworx::ALIB.Shutdown();

    return 0;
}
//! [DOX_ALIB_EXPR_TUT_CALC_MAIN]

// this is like the sample above. Unfortunately, we can not use the faked "main" function, because
// errors due to double initialization would occur.
int dox_calculator_sample( int argc, const char **argv )
{
    Compiler compiler;
    compiler.SetupDefaults();
    SPExpression expression;
    try
    {
        ALIB_STRINGS_FROM_NARROW( argv[1], expressionString, 256 )
        expression= compiler.Compile( expressionString );
    }
    catch (Exception& e)
    {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << e << endl;
        return static_cast<int>( e.Type().Integral() );
    }
    lib::expressions::Scope scope(compiler.CfgFormatter);
    Box result= expression->Evaluate( scope );
    cout << "Input:      " << expression->GetOriginalString()   << endl;
    cout << "Normalized: " << expression->GetNormalizedString() << endl;
    cout << "Result:     " << result                            << endl;
    return 0;
}

ALIB_WARNINGS_RESTORE
#undef main

#include "alib/expressions/detail/program.hpp"
#include "alib/compatibility/std_strings_iostream.hpp"
#include "alib/compatibility/std_characters.hpp"


#include "alib/alox.hpp"
#include "unittests/aworx_unittests.hpp"


#define TESTCLASSNAME       CPP_ALib_Dox_Expr_Calculator


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
             << e << endl;
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


void invokeCalculator( AWorxUnitTesting& ut, const nchar* expression, int fileNo )
{
    const nchar* argc[]= { "", expression };
    dox_calculator_sample( 1, reinterpret_cast<const nchar**>( &argc ) );
    ut.WriteResultFile( NString128("DOX_ALIB_EXPR_TUT_CALC_MAIN-") << fileNo<< ".txt",
                        testOutputStreamEC.str() );
    testOutputStreamEC.str( "");
}
void invokeNormalizedAndOptimized( AWorxUnitTesting& ut, const character* expression, int fileNo )
{
    WriteOrigNormalizedAndOptimized( expression );
    ut.WriteResultFile( NString128("DOX_ALIB_EXPRESSIONS_TUT_WONO-") << fileNo<< ".txt",
                        testOutputStreamEC.str() );
    testOutputStreamEC.str("");
}


UT_METHOD( Calculator )
{
    UT_INIT()

    try{

    invokeCalculator( ut, "1 + 2 * 3"                                      , 1 );
    invokeCalculator( ut, "1 * 2 + 3"                                      , 2 );
    invokeCalculator( ut, "true && false == true < false"                  , 3 );
    invokeCalculator( ut, "asin(1.0) * 2.0"                                , 4 );
    invokeCalculator( ut, "tolo(\"Hello \") + toup(\"World\")"             , 5 );
    invokeCalculator( ut, "Format( \"Today is: {:yyyy/MM/dd}\", today )"   , 6 );


    // samples used with box-function FToLiteral
    invokeNormalizedAndOptimized( ut, A_CHAR("Milliseconds(1)")                          , 100 );
    invokeNormalizedAndOptimized( ut, A_CHAR("Milliseconds(1) * 1000")                   , 101 );
    invokeNormalizedAndOptimized( ut, A_CHAR("Minutes(18) + Seconds(23)")                , 102 );


    }
    catch(Exception& e)
    {
        UT_PRINT( "An ALib exception occurred during test run." )
        UT_PRINT( e )
        UT_PRINT( "Stopping..." )
        UT_TRUE(false)
    }
}

#include "unittests/aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS

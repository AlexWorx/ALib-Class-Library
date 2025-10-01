// #################################################################################################
//  AWorx ALib Unit Tests
//  Documentation sample for ALib Expressions: Calculator
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#include "alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_EXPRESSIONS

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
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#elif defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable:4100 ) // unreferenced formal parameter
#endif

DOX_MARKER( [DOX_EXPR_TUT_CALC_MAIN])
// ALib basics
#include "ALib.Strings.H"
#include "ALib.Boxing.H"

// Using the expression compiler in this code entity, as well as the evaluation of expressions.
#include "ALib.Expressions.H"

// Get support for writing expression result values to std::cout
#include "ALib.Strings.StdIOStream.H"

// ALib Exception class
#include "ALib.Exceptions.H"

// ALib module initialization (has to be done in main())
#include "ALib.Bootstrap.H"

// std::cout
#include <iostream>

using namespace std;
using namespace alib;

//----- The Command Line Calculator Program -----
int main( int argc, const char **argv ) {
    // 0. Initialize ALib (this has to be done once at bootstrap with any software using ALib)
    alib::ARG_C=  argc;
    alib::ARG_VN= argv;
    alib::Bootstrap();

    // 1. Create a defaulted expression compiler. This adds all built-in stuff, like number
    //    arithmetics, strings, time/date, etc.
    Compiler compiler;
    compiler.SetupDefaults();

    // 2. Compile. Catch exceptions (must not trust user input)
    Expression expression;
    try {
        ALIB_STRINGS_FROM_NARROW( argv[1], expressionString, 256 )
        expression= compiler.Compile( expressionString );
    }
    catch (Exception& e) {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << e  << endl;
        return int( e.Type().Integral() );
    }

    // 3. We need an evaluation "scope"
    //    (later we will use a custom type here, that allows custom identifiers, functions and
    //    operators to access application data)
    expressions::Scope scope(compiler.CfgFormatter);

    // 4. Evaluate the expression
    //    (We must not fear exceptions here, as the compiler did all type checking, and resolved
    //    everything to a duly checked internal "program" running on a virtual machine.)
    Box result= expression->Evaluate( scope );

    // 5. Write result
    cout << "Input:      " << expression->GetOriginalString()   << endl;
    cout << "Normalized: " << expression->GetNormalizedString() << endl;
    cout << "Result:     " << result                            << endl;

    // 6. Terminate library
    alib::Shutdown();

    return 0;
}
DOX_MARKER( [DOX_EXPR_TUT_CALC_MAIN])

// this is like the sample above. Unfortunately, we cannot use the faked "main" function, because
// errors due to double initialization would occur.
int dox_calculator_sample( int argc, const char **argv )
{
    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;
    try {
        ALIB_STRINGS_FROM_NARROW( argv[1], expressionString, 256 )
        expression= compiler.Compile( expressionString );
    }
    catch (Exception& e) {
        cout << "An exception occurred compiling the expression. Details follow:" << endl
             << e << endl;
        return int( e.Type().Integral() );
    }
    expressions::Scope scope(compiler.CfgFormatter);
    Box result= expression->Evaluate( scope );
    cout << "Input:      " << expression->GetOriginalString()   << endl;
    cout << "Normalized: " << expression->GetNormalizedString() << endl;
    cout << "Result:     " << result                            << endl;
    return 0;
}

#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#elif defined(_MSC_VER)
    #pragma warning( pop )
#endif

#undef main

#include "ALib.Strings.StdIOStream.H"
#include "ALib.Compatibility.StdStrings.H"


#include "ALib.ALox.H"
#include "aworx_unittests.hpp"


#define TESTCLASSNAME       UT_Dox_Expr_Calculator


// #################################################################################################
// #### WriteOrigNormalizedAndOptimized()
// #################################################################################################
namespace{
void WriteOrigNormalizedAndOptimized(const String& expressionString)
{
    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;
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
    cout << "Program Length: " << expression->GetProgramLength()     << endl;
}}

// #################################################################################################
// #### Unit test executing tutorial code
// #################################################################################################

namespace ut_aworx {

UT_CLASS


void invokeCalculator( AWorxUnitTesting& ut, const nchar* expression, int fileNo )
{
    const nchar* argc[]= { "", expression };
    dox_calculator_sample( 1, reinterpret_cast<const nchar**>( &argc ) );
    ut.WriteResultFile( NString128("DOX_EXPR_TUT_CALC_MAIN-") << fileNo<< ".txt",
                        testOutputStreamEC.str()
                        , "" ); // no "OUTPUT", hence verbinclude has to be used
    testOutputStreamEC.str( "");
}
void invokeNormalizedAndOptimized( AWorxUnitTesting& ut, const character* expression, int fileNo )
{
    WriteOrigNormalizedAndOptimized( expression );
    ut.WriteResultFile( NString128("DOX_EXPRESSIONS_TUT_WONO-") << fileNo<< ".txt",
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

#include "aworx_unittests_end.hpp"

} //namespace

#endif //  ALIB_UT_DOCS && ALIB_EXPRESSIONS

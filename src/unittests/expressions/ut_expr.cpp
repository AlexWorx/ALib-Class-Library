// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"
#if ALIB_UT_DOCS && ALIB_UT_EXPRESSIONS

#include "alib/alox.hpp"
#if ALIB_ALOX
#   include "alib/alox/loggers/consolelogger.hpp"
#   include "alib/alox/logtools.hpp"
#endif
#include "alib/boxing/dbgboxing.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/plugins/arithmetics.hpp"
#include "alib/expressions/detail/program.hpp"
#include "alib/lang/format/fmtexceptions.hpp"
#if ALIB_THREADS
#   include "alib/threads/thread.hpp"
#endif

#include <math.h>

#define TESTCLASSNAME       UT_Expr
#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace alib;
using namespace alib;
using namespace alib::expressions;

#include "unittests/expressions/ut_expr_testfunc.hpp"

ALIB_WARNINGS_MACRO_NOT_USED_OFF

namespace ut_aworx {

// #################################################################################################
// ### Test Type & Scope
// #################################################################################################
//namespace {

DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_1])
struct MyType
{
    String   Name= A_CHAR("Joe");
    integer  Age=  42;
};
DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_1])


Box MyTypeSampleBox;

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-member-function"
#endif


//}
}
DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_2])
ALIB_STRINGS_APPENDABLE_TYPE_INLINE( ut_aworx::MyType , target << src.Name   << '/' << src.Age;  )
ALIB_STRINGS_APPENDABLE_TYPE_INLINE( ut_aworx::MyType*, target << src->Name  << '/' << src->Age; )
DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_2])

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif


namespace ut_aworx { namespace {

using Scope= alib::expressions::Scope;


class MyScope : public expressions::Scope
{
    public:
    MyScope(Compiler& compiler)
    : Scope(compiler.CfgFormatter)
    {}

    MyType   MyObject; // should be a pointer or reference in real life :-)
};

// #################################################################################################
// ### Test Compiler plugin
// #################################################################################################
Box name( Scope& scp, ArgIterator, ArgIterator )
{
    return dynamic_cast<MyScope&>(scp).MyObject.Name;
}

Box age ( Scope& scp, ArgIterator, ArgIterator )
{
    return dynamic_cast<MyScope&>(scp).MyObject.Age;
}

Box raw ( Scope& scp, ArgIterator, ArgIterator )
{
    return dynamic_cast<MyScope&>(scp).MyObject;
}


Box throwAlibException ( Scope& scp, ArgIterator, ArgIterator )
{
    String128 target;
    scp.Formatter->Format(target, A_CHAR("{"), 5 ); // throws!
    return target;
}

Box throwSTDException ( Scope& , ArgIterator begin, ArgIterator end)
{
    if((end - begin)==0) // always true
        throw std::runtime_error("test exception");
    return 42;
}


class MyFunctions : public plugins::Calculus
{
    public:
    MyFunctions( Compiler& compiler )
    : Calculus( "MyIdentifierPlugin", compiler, CompilePriorities::Custom + 1 )
    {
        MyTypeSampleBox= MyType();
        compiler.AddType( MyTypeSampleBox, "MyType" );

        Functions=
        {
            { {A_CHAR("name")     , lang::Case::Ignore, 4}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(name), &Types::String  , ETI },
            { {A_CHAR("age")      , lang::Case::Ignore, 3}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(age ), &Types::Integer , ETI },
            { {A_CHAR("rawobject"), lang::Case::Ignore, 3}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(raw ), &MyTypeSampleBox, ETI },
        };
    }
};


class CTExceptionThrowers : public plugins::Calculus
{
    public:
    CTExceptionThrowers( Compiler& compiler )
    : Calculus( "CTExceptionThrowers", compiler, CompilePriorities::Custom )
    {
        Functions=
        {
            { {A_CHAR("ThrowALib"),lang::Case::Ignore,9}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(throwAlibException ), &Types::String , CTI   },
            { {A_CHAR("ThrowStd") ,lang::Case::Ignore,8}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(throwSTDException  ), &Types::Integer, CTI   },
        };
    }
};

class ETExceptionThrowers : public plugins::Calculus
{
    public:
    ETExceptionThrowers( Compiler& compiler )
    : Calculus( "ETExceptionThrowers", compiler, CompilePriorities::Custom )
    {
        Functions=
        {
            { {A_CHAR("ThrowALib"),lang::Case::Ignore,9}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(throwAlibException ), &Types::String , ETI   },
            { {A_CHAR("ThrowStd") ,lang::Case::Ignore,8}, CALCULUS_SIGNATURE(nullptr), CALCULUS_CALLBACK(throwSTDException  ), &Types::Integer, ETI   },
        };
    }
};


// #################################################################################################
// ### MultiThreaded test
// #################################################################################################
#if ALIB_THREADS
ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION
struct EvaluationThread : public threads::Thread
{
    Expression        expr;
    Scope&             scope;
    Ticks::Duration   duration;
    int               Cnt       = 0;

    EvaluationThread(Expression e, Scope& s, const Ticks::Duration&            d) : expr(e), scope(s), duration(d) {}
    EvaluationThread(Expression e, Scope& s, const Ticks::Duration::TDuration& d) : expr(e), scope(s), duration(d) {}

    void Run() override
    {
        SetName(A_CHAR("EvalThread"));
        Ticks start;
        while (start.Age() < duration )
        {
            expr->Evaluate(scope);
            Cnt++;
        }
    }
};
ALIB_WARNINGS_RESTORE
#endif

} // anonymous namespace



// #################################################################################################
// ### Test Class
// #################################################################################################

UT_CLASS


// #################################################################################################
// ### ParseSpeed
// #################################################################################################
#if ALIB_TIME
UT_METHOD(ParseSpeed)
{
    UT_INIT()
    Compiler compiler;
    compiler.SetupDefaults();
    Expression expression;

    Ticks time;


    // 0 ms seconds (release)
    UT_PRINT("Expression A1:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  1+2+3+4+5+6+7+8+9+10+11+12+13+14+15+16+17+18+19+20 ) );
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )

    // 0 ms seconds (release)
    UT_PRINT("Expression A2:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  1+2*3+4*5+6*7+8*9+10*11+12*13+14*15+16*17+18*19+20 ) );
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )

    // 0 ms seconds (release)
    UT_PRINT("Expression A3:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  1&2+3*4&5+6*7&8+9*10&11+12*13&14+15*16&17+18*19&20 ) );
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )


    // 120 ms seconds (release)
    UT_PRINT("Expression B1:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  ((2*4)/(2 * ( 5 - (3 + 4 *( 9-4 *( 9-4 *( 9-4 *( 9-2 ) ) )) ) * (1 + 6 * (2 + 6 * (2 + 6 * (2 + 6 * (2 + 1)))) )))))   );
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )

    // 250 ms seconds (release)
    UT_PRINT("Expression B2:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  ((2*4)/(2 * ( 5 - (3 + 4 *( 9-4 *( 9-4 *( 9-4 *( 9-2 ) ) )) ) * ((2 + 6 * (2 + 6 * (2 + 1)) + 6 * (2 + 6 * (2 + 6 * (2 + 6 * (2 + 1)))) )))))  ) );
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )

    // 7,5 min, 760,250 ms seconds (release)
    UT_PRINT("Expression B3:")
    time= Ticks::Now();
    compiler.Compile( ALIB_STRINGIFY(  ((2*4)/(2 * ( 5 - (3 + 4 *( 9-4 *( 9-4 *( 9-4 *( 9-((2*4)/(2 * ( 5 - (3 + 4 *( 9-4 *( 9-4 *( 9-4 *( 9-2 ) ) )) ) * (1 + 6 * (2 + 6 * (2 + 6 * (2 + 6 * (2 + 1)))) ))))) ) ) )) ) * (1 + 6 * (2 + 6 * (2 + 6 * (2 + 6 * (2 + 1)))) )))));
    UT_PRINT("  Compile Time: {:,} ms", time.Age().InAbsoluteMilliseconds() )

}
#endif


// #################################################################################################
// ### ProgramListing
// #################################################################################################
UT_METHOD(ProgramListing)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();

    Expression expression;


    //#define NonAutomaticVisualTest
    #if !defined(NonAutomaticVisualTest)
        UT_PRINT("Test skipped, needs human check. To be done after changes of VM/Program code")
        UT_PRINT("Comment in the define two lines above to activate.")
    #else
        // Test VM listings (needs manual check 8-/)
        compiler.CfgCompilation+= Compilation::NoOptimization;
        PRINTPRGRM( !1 )
        PRINTPRGRM( 1 + 2  )
        PRINTPRGRM( 1 + (2 + 3)  )
        PRINTPRGRM( 1 + (true + 3)  )
        PRINTPRGRM( 1 +  2 * 3   )
        PRINTPRGRM( 1 + (2 + 3 + 4 + 5 + 6) + 7 + (8 + (9 + 10))   )
        PRINTPRGRM(         "Q"    ? "T" : "F" )
        PRINTPRGRM(       ( "Q"    ? "T" : "F" ) + "X"   )
        PRINTPRGRM( "A" + ( "Q"    ? "T" : "F" )         )
        PRINTPRGRM( "A" + ( "Q"    ? "T" : "F" ) + "X"   )
        PRINTPRGRM( "A" + (("Q"    ? "T" : "F" ) + "X" )  )
        PRINTPRGRM(  "Q"  ?  ("q"  ? "t" : "f")         : "F"  )
        PRINTPRGRM( ("Q"  ? (("q"  ? "t" : "f")  + "1") : "F" ) + "2"  )
        PRINTPRGRM( ("Q"  ? "T" :  ("q"  ? "t" : "f") )  + "X")
        PRINTPRGRM( ("Q"  ? "T" : (("q"  ? "t" : "f")   +  "x") ) +"X")
        PRINTPRGRM(  "Q"  ? ("q"  ? ("t" +"t") : ("f"+"f") ) : "F" )
        PRINTPRGRM(  "Q"  ? "T" : ("q"  ? ("t" +"t") : ("f"+"f") )  )
        PRINTPRGRM( ("Q"  ? ("T"+"T") : "F" ) + "X"   )
        PRINTPRGRM( "A" + ("Q"  ? ("T"+"T") : "F" ) + "X"   )


        UT_PRINT("")
        UT_PRINT("Human check needed:")
        UT_PRINT("  Check if listings are right. Especially the argument numbers in last column")
        UT_PRINT("  Testing stops with exit code 42.")
        UT_PRINT("")
        exit(42);
    #endif
}

// #################################################################################################
// ### Normalization
// #################################################################################################
UT_METHOD(TestNormalization)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);
    Expression expression;

    //Note: when calling  CCOMP_CONSTEXPR, the normalized expression will internally be
    //      re-parsed and double-checked


    compiler.CfgNormalization= Normalization(0);

    CCOMP_CONSTEXPR(  1 +  2  + 3  );
    CCOMP_CONSTEXPR( (1 +  2) + 3  );
    CCOMP_CONSTEXPR(  1 + (2  + 3) );

    CCOMP_CONSTEXPR(  1 -  2  - 3  );
    CCOMP_CONSTEXPR( (1 -  2) - 3  );
    CCOMP_CONSTEXPR(  1 - (2  - 3) );

    CCOMP_CONSTEXPR(  1 *  2  + 3  );
    CCOMP_CONSTEXPR( (1 *  2) + 3  );
    CCOMP_CONSTEXPR(  1 * (2  + 3) );

    CCOMP_CONSTEXPR(  1 +  2  * 3  );
    CCOMP_CONSTEXPR( (1 +  2) * 3  );
    CCOMP_CONSTEXPR(  1 + (2  * 3) );

    CCOMP_CONSTEXPR(  - (2  + 3 ) );
    CCOMP_CONSTEXPR(  - (true?  3 * 4  :  3 + 5  ) );
    CCOMP_CONSTEXPR(  - (true? (3 * 4) : (3 + 5) ) );
    CCOMP_CONSTEXPR(  - (true? (3 * 4) : (3 + 5) * 3 ) );


    EXPRNORM(1--1        ,1- -1       );
    EXPRNORM(-1          ,-1          );

    compiler.CfgNormalization+= Normalization::RemoveRedundantUnaryOpsOnNumberLiterals;
    EXPRNORM(+1          ,1           );
    EXPRNORM(--1         ,1           );

    compiler.CfgNormalization-= Normalization::RemoveRedundantUnaryOpsOnNumberLiterals;
    EXPRNORM(+1          ,+1          );
    EXPRNORM(--1         ,- -1        );


    EXPRNORM(! ! true    ,!!true       );
    EXPRNORM(- -true     ,- -true      );
    EXPRNORM(--true      ,- -true      );
    EXPRNORM(---1        ,- - -1       );
    EXPRNORM(- -1        ,- -1         );
    EXPRNORM(-1 - -1     ,-1- -1       );
    EXPRNORM(1--1        ,1- -1        );
    EXPRNORM(1++1        ,1+ +1        );

    EXPRNORM(1+ (1 + 2)  ,1+(1+2)        );
    EXPRNORM(1+ (+1 + 2) ,1+(+1+2)        );


    compiler.CfgNormalization+= Normalization::RemoveRedundantUnaryOpsOnNumberLiterals;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1>!!-1?1+2:!(~~3+~4)*5        );
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceBeforeQM;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1>!!-1 ?1+2:!(~~3+~4)*5       );
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceBeforeColon;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1>!!-1 ?1+2 :!(~~3+~4)*5      );
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceAfterQM;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1>!!-1 ? 1+2 :!(~~3+~4)*5     );
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceAfterColon;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1>!!-1 ? 1+2 : !(~~3+~4)*5    );

    compiler.CfgNormalization+= Normalization::UnaryOpSpace;
    EXPRNORM(--1                                ,1                                      );
    EXPRNORM(!!1                                ,!! 1                                   );
    EXPRNORM(!-1                                ,! -1                                   );
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1>!! -1 ? 1+2 : !(~~ 3+~ 4)*5        );
    compiler.CfgNormalization+= Normalization::UnaryOpSpaceIfUnaryFollows;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1>! ! -1 ? 1+2 : !(~ ~ 3+~ 4)*5      );
    compiler.CfgNormalization+= Normalization::UnaryOpInnerBracketSpace;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1>! ! -1 ? 1+2 : !( ~ ~ 3+~ 4 )*5    );
    compiler.CfgNormalization+= Normalization::UnaryOpSpaceIfBracketFollows;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1>! ! -1 ? 1+2 : ! ( ~ ~ 3+~ 4 )*5   );
    compiler.CfgNormalization+= Normalization::BinaryOpSpaces;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1 > ! ! -1 ? 1 + 2 : ! ( ~ ~ 3 + ~ 4 ) * 5        );
    compiler.CfgNormalization+= Normalization::RedundantBracketsBetweenTwoUnaryOps;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! 1 > ! ( ! -1 ) ? 1 + 2 : ! ( ~ ( ~ 3 ) + ~ 4 ) * 5);
    compiler.CfgNormalization+= Normalization::RedundantUnaryOpBrackets;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,! ( 1 ) > ! ( ! ( -1 ) ) ? 1 + 2 : ! ( ~ ( ~ ( 3 ) ) + ~ ( 4 ) ) * 5);


    compiler.CfgNormalization= Normalization::DEFAULT;
    EXPRNORM(!1 > !!-1 ? (1 + 2) : !(~~3+~4)*5  ,!1 > !!-1 ? 1 + 2 : !(~~3 + ~4) * 5        );


    compiler.CfgNormalization= Normalization(0);
    EXPRNORM(1 * (2+3) ,1*(2+3)        );
    compiler.CfgNormalization+= Normalization::InnerBracketSpace;
    EXPRNORM(1 * (2+3) ,1*( 2+3 )        );
    compiler.CfgNormalization+= Normalization::OuterBracketSpace;
    EXPRNORM(1 * (2+3) ,1* ( 2+3 ) );

    EXPRNORM(true&false==false|~false ,true&false==false|~false        );
    compiler.CfgNormalization+= Normalization::ReplaceAliasOperators;
    EXPRNORM(true&false==false|~false ,true&&false==false||!false      );

    MyFunctions myIdentifierPlugin(compiler);
    compiler.InsertPlugin( &myIdentifierPlugin );
    EXPRNORM(Name ,Name        );
    EXPRNORM(RAw   ,RAw         );
    compiler.CfgNormalization+= Normalization::ReplaceFunctionNames;
    EXPRNORM(Name ,name        );
    EXPRNORM(RAw   ,rawobject   );


    compiler.CfgNormalization= Normalization::BinaryOpSpaces;
    EXPRNORM(1 - 2 - 3                  ,1 - 2 - 3                     );
    EXPRNORM(1 - 2 * 3                  ,1 - 2 * 3                     );
    EXPRNORM(1 * 2 - 3                  ,1 * 2 - 3                     );
    EXPRNORM(1 * 2 * 3                  ,1 * 2 * 3                     );
    EXPRNORM(1 - 2 * 3 - 4 - 5 * 6 - 7 ,1 - 2 * 3 - 4 - 5 * 6 - 7     );
    EXPRNORM(true == false || true      ,true == false || true         );
    EXPRNORM(true || false == true      ,true || false == true         );
    EXPRNORM(1 - 2 - (3 - 4)            ,1 - 2 - (3 - 4)               );
    EXPRNORM(1 - 2 - (3 - 4)            ,1 - 2 - (3 - 4)               );

    CCOMP_CONSTEXPR(    1 - 2 - 3                 );
    CCOMP_CONSTEXPR(    1 - 2 * 3                 );
    CCOMP_CONSTEXPR(    1 * 2 - 3                 );
    CCOMP_CONSTEXPR(    1 * 2 * 3                 );
    CCOMP_CONSTEXPR(    1 - 2 * 3 - 4 - 5 * 6 - 7 );
    CCOMP_CONSTEXPR(    true == false || true     );
    CCOMP_CONSTEXPR(    true || false == true     );
    CCOMP_CONSTEXPR(    1 - 2 - (3 - 4)           );
    CCOMP_CONSTEXPR(    1 - 2 - (3 - 4)           );


    compiler.CfgNormalization+= Normalization::RedundantRhsBracketsIfRhsIsStrongerBinaryOp;
    EXPRNORM(1 - 2 - 3                        ,1 - 2 - 3                          );
    EXPRNORM(1 - 2 - 3 - 4                    ,1 - 2 - 3 - 4                      );
    EXPRNORM(1 - 2 * 3                        ,1 - (2 * 3)                        );
    EXPRNORM(1 * 2 - 3                        ,1 * 2 - 3                          );
    EXPRNORM(1 * 2 * 3                        ,1 * 2 * 3                          );
    EXPRNORM(1 - 2 * 3 - 4 - 5 * 6 - 7        ,1 - (2 * 3) - 4 - (5 * 6) - 7      );
    EXPRNORM(true == false || true            ,true == false || true              );
    EXPRNORM(true || false == true            ,true || (false == true)            );
    EXPRNORM(true == false && true            ,true == false && true              );
    EXPRNORM(true && false == true            ,true && (false == true)            );
    EXPRNORM(true && false == false && true   ,true && (false == false) && true   );
    EXPRNORM(true && false == false == true   ,true && (false == false == true)   );
    EXPRNORM(true && false == true < false    ,true && (false == (true < false))  );
    EXPRNORM(true && false == (false && true) ,true && (false == (false && true)) );

    CCOMP_CONSTEXPR(  1 - 2 - 3                      );
    CCOMP_CONSTEXPR(  1 - 2 - 3 - 4                  );
    CCOMP_CONSTEXPR(  1 - 2 * 3                      );
    CCOMP_CONSTEXPR(  1 * 2 - 3                      );
    CCOMP_CONSTEXPR(  1 * 2 * 3                      );
    CCOMP_CONSTEXPR(  1 - 2 * 3 - 4 - 5 * 6 - 7      );
    CCOMP_CONSTEXPR(  true == false || true          );
    CCOMP_CONSTEXPR(  true || false == true          );
    CCOMP_CONSTEXPR(  true == false && true          );
    CCOMP_CONSTEXPR(  true && false == true          );
    CCOMP_CONSTEXPR(  true && false == false && true );

#if defined(__GNUC__) && ! defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses")
#endif
    CCOMP_CONSTEXPR(  true && false == false == true );
    CCOMP_CONSTEXPR(  true && false == true < false  );

    #if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

    compiler.CfgNormalization-= Normalization::RedundantRhsBracketsIfRhsIsStrongerBinaryOp;
    compiler.CfgNormalization+= Normalization::RedundantBracketsIfLhsAndRhsAreBinaryOps;
    EXPRNORM(1 - 2 - 3                        ,1 - 2 - 3                         );
    EXPRNORM(1 - 2 - 3 - 4                    ,1 - 2 - 3 - 4                     );
    EXPRNORM(1 - 2 - (3 - 4)                  ,(1 - 2) - (3 - 4)                 );
    EXPRNORM(1 - 2 - 3 - (4 - 5)              ,(1 - 2 - 3) - (4 - 5)             );
    EXPRNORM(1 - 2 - (3 - 4) - 5              ,(1 - 2) - (3 - 4) - 5             );
    EXPRNORM(true == false || true            ,true == false || true             );
    EXPRNORM(true || false == true            ,true || false == true             );
    EXPRNORM(true == false && true            ,true == false && true             );
    EXPRNORM(true && false == true            ,true && false == true             );
    EXPRNORM(true && false == false && true   ,true && false == false && true    );
    EXPRNORM((true && false) == false && true ,(true && false) == false && true  );
    EXPRNORM(true == false && (false && true) ,(true == false) && (false && true));
    EXPRNORM(true && false == true < false   ,true && false == true < false      );



    compiler.CfgNormalization-= Normalization::RedundantBracketsIfLhsAndRhsAreBinaryOps;
    compiler.CfgNormalization+= Normalization::RedundantBinaryOpBrackets;
    EXPRNORM(1 - 2 - 3 - 4          ,((1 - 2) - 3) - 4              );
    EXPRNORM(1 - 2 - (3 - 4)        ,(1 - 2) - (3 - 4)              );
    EXPRNORM(1 - 2 - 3 - 4 - (5 - 6),(((1 - 2) - 3) - 4) - (5 - 6)  );


    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceBeforeQM;
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceAfterQM;
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceBeforeColon;
    compiler.CfgNormalization+= Normalization::ConditionalOpSpaceAfterColon;

    compiler.CfgNormalization-= Normalization::RedundantConditionalOpBrackets;
    EXPRNORM(1 ? 2 : 3
            ,1 ? 2 : 3                                     );

    EXPRNORM(1 ? (2 ? 3 : 4) : 5
            ,1 ? 2 ? 3 : 4 : 5                             );

    EXPRNORM(1 ? 2 : 3 ? 4 : 5
            ,1 ? 2 : 3 ? 4 : 5                             );

    EXPRNORM(0 ? 2 ? 3 ? 4 : 5 : 6 : 7
            ,0 ? 2 ? 3 ? 4 : 5 : 6 : 7                     );

    EXPRNORM(true ? 0 : 1.2 ? "a" : false ? 6 : 7.3
            ,true ? 0 : 1.2 ? "a" : false ? 6 : 7.3        );

    CCOMP_CONSTEXPR( true   ? (false  ? (false  ? (false  ? (false  ? 1   : 2.3)   : 3.3)   : 4.3)   : 5.3) : 6.3 );

    EXPRNORM(true ? (false  ? (false  ? (false  ? (false  ? 1   : 2.3)   : 3.3)   : 4.3)   : 5.3) : 6.3
            ,true ? false ? false ? false ? false ? 1 : 2.3 : 3.3 : 4.3 : 5.3 : 6.3  );

    compiler.CfgNormalization+= Normalization::RedundantConditionalOpBrackets;
    EXPRNORM(1 ? 2 : 3
            ,1 ? 2 : 3                                     );

    EXPRNORM(1 ? (2 ? 3 : 4) : 5
            ,1 ? (2 ? 3 : 4) : 5                           );

    EXPRNORM(1 ? 2 : 3 ? 4 : 5
            ,1 ? 2 : (3 ? 4 : 5)                           );

    EXPRNORM(0 ? 2 ? 3 ? 4 : 5 : 6 : 7
            ,0 ? (2 ? (3 ? 4 : 5) : 6) : 7                 );

    EXPRNORM(true ? 0 : 1.2 ? "a" : false ? 6 : 7.3
            ,true ? 0 : (1.2 ? "a" : (false ? 6 : 7.3))    );

    CCOMP_CONSTEXPR( true   ? (false  ? (false  ? (false  ? (false  ? 1   : 2.3)   : 3.3)   : 4.3)   : 5.3) : 6.3 );

    EXPRNORM(true ? (false ? (false ? (false ? (false ? 1 : 2.3) : 3.3) : 4.3) : 5.3) : 6.3
            ,true ? (false ? (false ? (false ? (false ? 1 : 2.3) : 3.3) : 4.3) : 5.3) : 6.3  );

    compiler.CfgNormalization-= Normalization::SubscriptSpaceBeforeBrackets;
    compiler.CfgNormalization-= Normalization::SubscriptInnerBracketSpace;
    EXPRNORM("Hello"[0]       ,"Hello"[0]);
    compiler.CfgNormalization+= Normalization::SubscriptSpaceBeforeBrackets;
    EXPRNORM("Hello"[0]       ,"Hello" [0]);
    compiler.CfgNormalization+= Normalization::SubscriptInnerBracketSpace;
    EXPRNORM("Hello"[0]       ,"Hello" [ 0 ]);
}


// #################################################################################################
// ### BoolOps
// #################################################################################################
UT_METHOD(BoolOps)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

    // declare some "identifiers", so that we can use them in C++ expressions the same as in ours
    constexpr bool on  = true;     (void) on  ;
    constexpr bool On  = true;     (void) On  ;
    constexpr bool ON  = true;     (void) ON  ;
    constexpr bool off = false;    (void) off ;
    constexpr bool Off = false;    (void) Off ;
    constexpr bool OFF = false;    (void) OFF ;
    constexpr bool yes = true;     (void) yes ;
    constexpr bool Yes = true;     (void) Yes ;
    constexpr bool YES = true;     (void) YES ;
    constexpr bool no  = false;    (void) no  ;
    constexpr bool No  = false;    (void) No  ;
    constexpr bool NO  = false;    (void) NO  ;

    // boolean
    CCOMP_CONSTEXPR( true  );
    CCOMP_CONSTEXPR( false );
    CCOMP_CONSTEXPR( Off );
    CCOMP_CONSTEXPR( OFF && Off );
    CCOMP_CONSTEXPR( ON  && On  );
    CCOMP_CONSTEXPR( off || on  );

    CCOMP_CONSTEXPR( ! !true  );
    CCOMP_CONSTEXPR( !! true  );
    CCOMP_CONSTEXPR( !!!true  );
    CCOMP_CONSTEXPR( !!true && false  );
    CCOMP_CONSTEXPR( !!!false         );
          CONSTEXPR(  !~true         ,  true    );
          CONSTEXPR( ~!false         ,  false   );
          CONSTEXPR( ~!~!~~!!~!true  ,  true    );
          CONSTEXPR( ~!~!~~!!~!false ,  false   );

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wparentheses"
#endif
    CCOMP_CONSTEXPR( true && false || false  );
    CCOMP_CONSTEXPR( false || true && false  );
    CCOMP_CONSTEXPR(  true &&  false ==  true  && false  );
    CCOMP_CONSTEXPR( (true &&  false)== (true  && false) );
    CCOMP_CONSTEXPR(  true && (false ==  true  && false) );
    CCOMP_CONSTEXPR( (true &&  false ==  true) && false  );
    CCOMP_CONSTEXPR(  true &&  false !=  true  && false  );
    CCOMP_CONSTEXPR( (true &&  false)!= (true  && false) );
    CCOMP_CONSTEXPR(  true && (false !=  true  && false) );
    CCOMP_CONSTEXPR( (true &&  false !=  true) && false  );

    CCOMP_CONSTEXPR( !!true && false  );

    CCOMP_CONSTEXPR( !1         );
    CCOMP_CONSTEXPR( !!1        );
    CCOMP_CONSTEXPR( !0         );
    CCOMP_CONSTEXPR( !!0        );
    CCOMP_CONSTEXPR(  1 && 2    );
    CCOMP_CONSTEXPR( 1.0 && 0.0 );

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wliteral-conversion"
#endif

    CCOMP_CONSTEXPR(  0  || 2.1 );

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

    // aliases
          CONSTEXPR( false &  false  , false );
          CONSTEXPR( false &   true  , false );
          CONSTEXPR(  true &  false  , false );
          CONSTEXPR(  true &   true  ,  true );
          CONSTEXPR( false |  false  , false );
          CONSTEXPR( false |   true  ,  true );
          CONSTEXPR(  true |  false  ,  true );
          CONSTEXPR(  true |   true  ,  true );

          CONSTEXPR( false &      0  , false );
          CONSTEXPR( false &      1  , false );
          CONSTEXPR(  true &      0  , false );
          CONSTEXPR(  true &      1  ,  true );
          CONSTEXPR(     0 |  false  , false );
          CONSTEXPR(     0 |   true  ,  true );
          CONSTEXPR(     1 |  false  ,  true );
          CONSTEXPR(     1 |   true  ,  true );

          CONSTEXPR( false &    0.0  , false );
          CONSTEXPR( false &    1.0  , false );
          CONSTEXPR(  true &    0.0  , false );
          CONSTEXPR(  true &    1.0  ,  true );
          CONSTEXPR(   0.0 |  false  , false );
          CONSTEXPR(   0.0 |   true  ,  true );
          CONSTEXPR(   1.0 |  false  ,  true );
          CONSTEXPR(   1.0 |   true  ,  true );

    // equal, not equal
    CCOMP_CONSTEXPR( false == false );
    CCOMP_CONSTEXPR( false ==  true );
    CCOMP_CONSTEXPR(  true == false );
    CCOMP_CONSTEXPR(  true ==  true );
    CCOMP_CONSTEXPR( false ==     0 );
    CCOMP_CONSTEXPR( false ==     1 );
    CCOMP_CONSTEXPR(  true ==     0 );
    CCOMP_CONSTEXPR(  true ==     1 );
    CCOMP_CONSTEXPR(     0 == false );
    CCOMP_CONSTEXPR(     0 ==  true );
    CCOMP_CONSTEXPR(     1 == false );
    CCOMP_CONSTEXPR(     1 ==  true );
    CCOMP_CONSTEXPR( false ==   0.0 );
    CCOMP_CONSTEXPR( false ==   1.0 );
    CCOMP_CONSTEXPR(  true ==   0.0 );
    CCOMP_CONSTEXPR(  true ==   1.0 );
    CCOMP_CONSTEXPR(   0.0 == false );
    CCOMP_CONSTEXPR(   0.0 ==  true );
    CCOMP_CONSTEXPR(   1.0 == false );
    CCOMP_CONSTEXPR(   1.0 ==  true );

    CCOMP_CONSTEXPR( false != false );
    CCOMP_CONSTEXPR( false !=  true );
    CCOMP_CONSTEXPR(  true != false );
    CCOMP_CONSTEXPR(  true !=  true );
    CCOMP_CONSTEXPR( false !=     0 );
    CCOMP_CONSTEXPR( false !=     1 );
    CCOMP_CONSTEXPR(  true !=     0 );
    CCOMP_CONSTEXPR(  true !=     1 );
    CCOMP_CONSTEXPR(     0 != false );
    CCOMP_CONSTEXPR(     0 !=  true );
    CCOMP_CONSTEXPR(     1 != false );
    CCOMP_CONSTEXPR(     1 !=  true );
    CCOMP_CONSTEXPR( false !=   0.0 );
    CCOMP_CONSTEXPR( false !=   1.0 );
    CCOMP_CONSTEXPR(  true !=   0.0 );
    CCOMP_CONSTEXPR(  true !=   1.0 );
    CCOMP_CONSTEXPR(   0.0 != false );
    CCOMP_CONSTEXPR(   0.0 !=  true );
    CCOMP_CONSTEXPR(   1.0 != false );
    CCOMP_CONSTEXPR(   1.0 !=  true );

    // aliases of equal
          CONSTEXPR( false =  false , false ==  false );
          CONSTEXPR( false =   true , false ==   true );
          CONSTEXPR(  true =  false ,  true ==  false );
          CONSTEXPR(  true =   true ,  true ==   true );
          CONSTEXPR( false =      0 , false ==      0 );
          CONSTEXPR( false =      1 , false ==      1 );
          CONSTEXPR(  true =      0 ,  true ==      0 );
          CONSTEXPR(  true =      1 ,  true ==      1 );
          CONSTEXPR(     0 =  false ,     0 ==  false );
          CONSTEXPR(     0 =   true ,     0 ==   true );
          CONSTEXPR(     1 =  false ,     1 ==  false );
          CONSTEXPR(     1 =   true ,     1 ==   true );
          CONSTEXPR( false =    0.0 , false ==    0.0 );
          CONSTEXPR( false =    1.0 , false ==    1.0 );
          CONSTEXPR(  true =    0.0 ,  true ==    0.0 );
          CONSTEXPR(  true =    1.0 ,  true ==    1.0 );
          CONSTEXPR(   0.0 =  false ,   0.0 ==  false );
          CONSTEXPR(   0.0 =   true ,   0.0 ==   true );
          CONSTEXPR(   1.0 =  false ,   1.0 ==  false );
          CONSTEXPR(   1.0 =   true ,   1.0 ==   true );
}

UT_METHOD(VerbalOps)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

    CONSTEXPR(  true or false           , true || false      );
    CONSTEXPR(  true and false          , true && false      );
    CONSTEXPR(  not false               , ! false            );
    CONSTEXPR(  not false and true      , ! false && true    );
    CONSTEXPR(     false and not true   ,   false && ! true  );
    CONSTEXPR(  not false and not true  , ! false && ! true  );
    CONSTEXPR(  not false or not true   , ! false || ! true  );

    CONSTEXPR(  NOT false              , true );
    CONSTEXPR(  not false OR not true  , true );
    CONSTEXPR(  Not false OR NOT true  , true );

    compiler.CfgNormalization= Normalization::COMPACT;
    CONSTEXPR(  not false, true );
    CONSTEXPR(  not false and not true, false );
    CONSTEXPR(  not false or not true, true );

    CONSTEXPR(  1  sm                2 , true  );
    CONSTEXPR(  1  sm                1 , false );
    CONSTEXPR(  1  smaller           2 , true  );
    CONSTEXPR(  1  smaller           1 , false );
    CONSTEXPR(  1  smeq              1 , true  );
    CONSTEXPR(  1  smeq              0 , false );
    CONSTEXPR(  1  smaller_or_equal  1 , true  );
    CONSTEXPR(  1  smaller_or_equal  0 , false );
    CONSTEXPR(  2  gt                1 , true  );
    CONSTEXPR(  1  gt                1 , false );
    CONSTEXPR(  2  greater           1 , true  );
    CONSTEXPR(  1  greater           1 , false );
    CONSTEXPR(  1  gteq              1 , true  );
    CONSTEXPR(  0  gteq              1 , false );
    CONSTEXPR(  1  greater_or_equal  1 , true  );
    CONSTEXPR(  0  greater_or_equal  1 , false );
    CONSTEXPR(  1  eq                1 , true  );
    CONSTEXPR(  1  eq                2 , false );
    CONSTEXPR(  1  equals            1 , true  );
    CONSTEXPR(  1  equals            2 , false );
    CONSTEXPR(  1  neq               2 , true  );
    CONSTEXPR(  1  neq               1 , false );
    CONSTEXPR(  1  not_equals        2 , true  );
    CONSTEXPR(  1  not_equals        1 , false );

    CONSTEXPR(  equals              , expressions::Exceptions::UnknownIdentifier );
    CONSTEXPR(  equals 1            , expressions::Exceptions::SyntaxErrorExpectation );
    CONSTEXPR(  equals equals       , expressions::Exceptions::SyntaxErrorExpectation );
    CONSTEXPR(  equals equals equals, expressions::Exceptions::UnknownIdentifier );

    compiler.CfgNormalization= Normalization::DEFAULT;

    Expression expression;
    try
    {
    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToUpperCase;
    EXPRNORM(not true and false       ,not true and false  );

    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToLowerCase;
    EXPRNORM(nOT true aND false       ,not true and false  );
    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToLowerCase;

    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToUpperCase;
    EXPRNORM(nOT true aND false       ,NOT true AND false  );
    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToUpperCase;

    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToDefinedLetterCase;
    EXPRNORM(not true and false       ,Not true And false  );
    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToDefinedLetterCase;

    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToSymbolic;
    EXPRNORM(!  true &&  false          ,!true && false    );
    EXPRNORM(not  true  and  false        ,!true && false  );
    compiler.CfgNormalization-= Normalization::BinaryOpSpaces;
    EXPRNORM(!  true &&  false          ,!true&&false      );
    EXPRNORM(not  true  and  false        ,!true&&false    );
    compiler.CfgNormalization+= Normalization::UnaryOpSpace;
    EXPRNORM(!  true &&  false          ,! true&&false     );
    EXPRNORM(not  true  and  false        ,! true&&false   );

    EXPRNORM(!!  true &&  false         ,!! true&&false    );
    EXPRNORM(not not  true  and  false  ,!! true&&false    );
    compiler.CfgNormalization+= Normalization::UnaryOpSpaceIfUnaryFollows;
    EXPRNORM(!!  true &&  false         ,! ! true&&false   );
    EXPRNORM(not not  true  and  false  ,! ! true&&false   );
    compiler.CfgNormalization-= Normalization::UnaryOpSpace;
    EXPRNORM(!!  true &&  false         ,! !true&&false    );
    EXPRNORM(not not  true  and  false  ,! !true&&false    );

    // which flag does supersede?
    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToDefinedLetterCase;
    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToUpperCase;
    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToLowerCase;
    compiler.CfgNormalization+= Normalization::ReplaceVerbalOperatorsToSymbolic;

    EXPRNORM(nOt true AnD true         ,!true&&true              );

    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToSymbolic;
    EXPRNORM(nOt true AnD true         ,not true and true        );

    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToLowerCase;
    EXPRNORM(nOt true AnD true         ,NOT true AND true        );

    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToUpperCase;
    EXPRNORM(nOt true AnD true         ,Not true And true        );

    compiler.CfgNormalization-= Normalization::ReplaceVerbalOperatorsToDefinedLetterCase;
    EXPRNORM(nOt true AnD true         ,nOt true AnD true        );
    }
    catch( Exception& e )
    {
        #define LOX_LOX ut.lox
        Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
        log_exception( ut, e );
        ALIB_ERROR( "UNITTESTS", "UT Failure" )
        #undef LOX_LOX
    }
}

// #################################################################################################
// ### ArithOps
// #################################################################################################
UT_METHOD(ArithOps)
{
    UT_INIT()

    #if defined(__clang__)
    #   pragma clang diagnostic push
    #   pragma clang diagnostic ignored "-Wfloat-equal"
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic push
    #   pragma GCC diagnostic ignored "-Wbool-compare"
    #endif


    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

    // conversions
         CONSTEXPR(  Bool(false)   , false    );
         CONSTEXPR(  Bool(true )   , true     );
         CONSTEXPR(  Bool(0)       , false    );
         CONSTEXPR(  Bool(1)       , true     );
         CONSTEXPR(  Bool(0.0)     , false    );
         CONSTEXPR(  Bool(1.3)     , true     );

         CONSTEXPR(  Int(false)    , 0        );
         CONSTEXPR(  Int(true )    , 1        );
         CONSTEXPR(  Int(5)        , 5        );
         CONSTEXPR(  Int(0.0)      , 0        );
         CONSTEXPR(  Int(1.3)      , 1        );


         CONSTEXPR(  Float(false)  , 0.0      );
         CONSTEXPR(  Float(true )  , 1.0      );
         CONSTEXPR(  Float(0)      , 0.0      );
         CONSTEXPR(  Float(1)      , 1.0      );
         CONSTEXPR(  Float(3.4)    , 3.4      );

    // arithmetical
    CCOMP_CONSTEXPR(  1        );
    CCOMP_CONSTEXPR(  2        );
    CCOMP_CONSTEXPR( -1        );
    CCOMP_CONSTEXPR( - -1      );
    CCOMP_CONSTEXPR( -1 + 2    );
    CCOMP_CONSTEXPR(  1 + 2    );
    CCOMP_CONSTEXPR(  1 - 2    );
    CCOMP_CONSTEXPR( 1 + 2 + 4 );
    CCOMP_CONSTEXPR( 1 + 2 + 4 );
    CCOMP_CONSTEXPR( 1 + 2 - 4 );
    CCOMP_CONSTEXPR(  1 +  2 -  4 *  5  );
    CCOMP_CONSTEXPR(  1 +  2 - (4 *  5) );
    CCOMP_CONSTEXPR(  1 + (2 -  4 *  5) );
    CCOMP_CONSTEXPR( (1 +  2 -  4) * 5  );

    CCOMP_CONSTEXPR(  27 %  2 +  8  / 3  );
    CCOMP_CONSTEXPR(  27 % (2 +  8) / 3  );
    CCOMP_CONSTEXPR(  27 % (2 +  8  / 3) );
    CCOMP_CONSTEXPR( (27 %  2 +  8) / 3  );

    CCOMP_CONSTEXPR( (( (25+93)*7 + 4 ) *2 +1 ) *3  );

    CCOMP_CONSTEXPR(   10 * 9        );
    CCOMP_CONSTEXPR(   10 / 3        );
    CCOMP_CONSTEXPR(   10 % 3        );
    CCOMP_CONSTEXPR(  -10 % 3        );
    CCOMP_CONSTEXPR(    2.3          );
    CCOMP_CONSTEXPR(   -2.3          );
    CCOMP_CONSTEXPR(  1.2 + 2.3      );
    CCOMP_CONSTEXPR(  1.2 - 2.3      );
    CCOMP_CONSTEXPR(  1.2 * 2.3      );
    CCOMP_CONSTEXPR(  5.2 / 1.7      );


    CCOMP_CONSTEXPR(  5.2 == 5.20001 );
    CCOMP_CONSTEXPR(  5.2 != 5.20001 );
    CCOMP_CONSTEXPR(  3.27232 == 3.27232 );

          CONSTEXPR(  1 = 1, true  ); // alias
          CONSTEXPR(  1 = 2, false ); // alias

    testExpression( ALIB_CALLER, ut, compiler, scope, A_CHAR("3.5 % 1.7")            , fmod(3.5, 1.7) , 1 );
    testExpression( ALIB_CALLER, ut, compiler, scope, A_CHAR("6.5 % 0.6")            , fmod(6.5, 0.6) , 1 );


    // shifting
    CCOMP_CONSTEXPR(  4 == 1  << 2   );
    CCOMP_CONSTEXPR( (4 == 1) << 2   );
    CCOMP_CONSTEXPR(  4 == (1 << 2)  );

    CCOMP_CONSTEXPR(  1  << 0   );
    CCOMP_CONSTEXPR(  1  << 1   );
    CCOMP_CONSTEXPR(  1  << 2   );
    CCOMP_CONSTEXPR(  1  >> 0   );
    CCOMP_CONSTEXPR(  2  >> 1   );
    CCOMP_CONSTEXPR(  4  >> 2   );
    CCOMP_CONSTEXPR(  4  >> 3   );
    CCOMP_CONSTEXPR(  4  >> 4   );
    CCOMP_CONSTEXPR(  1  << 16  );
    CCOMP_CONSTEXPR(  8  << 16  );
    CCOMP_CONSTEXPR(true << 3   );

    // comparison  bool and below
    CCOMP_CONSTEXPR(  false <  false   );
    CCOMP_CONSTEXPR(  false <   true   );
    CCOMP_CONSTEXPR(   true <  false   );
    CCOMP_CONSTEXPR(   true <   true   );
    CCOMP_CONSTEXPR(  false <= false   );
    CCOMP_CONSTEXPR(  false <=  true   );
    CCOMP_CONSTEXPR(   true <= false   );
    CCOMP_CONSTEXPR(   true <=  true   );
    CCOMP_CONSTEXPR(  false >  false   );
    CCOMP_CONSTEXPR(  false >   true   );
    CCOMP_CONSTEXPR(   true >  false   );
    CCOMP_CONSTEXPR(   true >   true   );
    CCOMP_CONSTEXPR(  false >= false   );
    CCOMP_CONSTEXPR(  false >=  true   );
    CCOMP_CONSTEXPR(   true >= false   );
    CCOMP_CONSTEXPR(   true >=  true   );

#if defined(_MSC_VER)
    #pragma warning( push )
    #pragma warning( disable : 4804 ) // unsafe use of type bool
    #pragma warning( disable : 4805 ) // unsafe mix of type bool and int
#endif

    // comparison  int and below
    CCOMP_CONSTEXPR(  false <  0       );
    CCOMP_CONSTEXPR(  false <= 0       );
    CCOMP_CONSTEXPR(   true <  0       );
    CCOMP_CONSTEXPR(   true <  1       );
    CCOMP_CONSTEXPR(   true <= 0       );
    CCOMP_CONSTEXPR(   true <= 1       );
    CCOMP_CONSTEXPR(   true <= 2       );
    CCOMP_CONSTEXPR(  false >  0       );
    CCOMP_CONSTEXPR(  false >= 0       );
    CCOMP_CONSTEXPR(   true >  0       );
    CCOMP_CONSTEXPR(   true >  1       );
    CCOMP_CONSTEXPR(   true >= 0       );
    CCOMP_CONSTEXPR(   true >= 1       );
    CCOMP_CONSTEXPR(   true >= 2       );
    CCOMP_CONSTEXPR(      0 <  false   );
    CCOMP_CONSTEXPR(      0 <= false   );
    CCOMP_CONSTEXPR(      0 <   true   );
    CCOMP_CONSTEXPR(      1 <   true   );
    CCOMP_CONSTEXPR(      0 <=  true   );
    CCOMP_CONSTEXPR(      1 <=  true   );
    CCOMP_CONSTEXPR(      2 <=  true   );
    CCOMP_CONSTEXPR(     0  >  false   );
    CCOMP_CONSTEXPR(     0  >= false   );
    CCOMP_CONSTEXPR(     0  >   true   );
    CCOMP_CONSTEXPR(     1  >   true   );
    CCOMP_CONSTEXPR(     0  >=  true   );
    CCOMP_CONSTEXPR(     1  >=  true   );
    CCOMP_CONSTEXPR(     2  >=  true   );

    CCOMP_CONSTEXPR(      0 <    0   );
    CCOMP_CONSTEXPR(      0 <=   0   );
    CCOMP_CONSTEXPR(      1 <    0   );
    CCOMP_CONSTEXPR(      1 <    1   );
    CCOMP_CONSTEXPR(      1 <=   0   );
    CCOMP_CONSTEXPR(      1 <=   1   );
    CCOMP_CONSTEXPR(      1 <=   2   );
    CCOMP_CONSTEXPR(      0 >    0   );
    CCOMP_CONSTEXPR(      0 >=   0   );
    CCOMP_CONSTEXPR(      1 >    0   );
    CCOMP_CONSTEXPR(      1 >    1   );
    CCOMP_CONSTEXPR(      1 >=   0   );
    CCOMP_CONSTEXPR(      1 >=   1   );
    CCOMP_CONSTEXPR(      1 >=   2   );

    // comparison  float and below
    CCOMP_CONSTEXPR(  false < 0.0      );
    CCOMP_CONSTEXPR(  false <=0.0      );
    CCOMP_CONSTEXPR(   true < 0.0      );
    CCOMP_CONSTEXPR(   true < 1.0      );
    CCOMP_CONSTEXPR(   true <=0.0      );
    CCOMP_CONSTEXPR(   true <=1.0      );
    CCOMP_CONSTEXPR(   true <= 2       );
    CCOMP_CONSTEXPR(  false > 0.0      );
    CCOMP_CONSTEXPR(  false >=0.0      );
    CCOMP_CONSTEXPR(   true > 0.0      );
    CCOMP_CONSTEXPR(   true > 1.0      );
    CCOMP_CONSTEXPR(   true >=0.0      );
    CCOMP_CONSTEXPR(   true >=1.0      );
    CCOMP_CONSTEXPR(   true >= 2       );
    CCOMP_CONSTEXPR(    0.0 <  false   );
    CCOMP_CONSTEXPR(    0.0 <= false   );
    CCOMP_CONSTEXPR(    0.0 <   true   );
    CCOMP_CONSTEXPR(    1.0 <   true   );
    CCOMP_CONSTEXPR(    0.0 <=  true   );
    CCOMP_CONSTEXPR(    1.0 <=  true   );
    CCOMP_CONSTEXPR(     2  <=  true   );
    CCOMP_CONSTEXPR(   0.0  >  false   );
    CCOMP_CONSTEXPR(   0.0  >= false   );
    CCOMP_CONSTEXPR(   0.0  >   true   );
    CCOMP_CONSTEXPR(   1.0  >   true   );
    CCOMP_CONSTEXPR(   0.0  >=  true   );
    CCOMP_CONSTEXPR(   1.0  >=  true   );
    CCOMP_CONSTEXPR(    2   >=  true   );
#if defined(_MSC_VER)
    #pragma warning( pop )
#endif

    CCOMP_CONSTEXPR(     0  <  0.0     );
    CCOMP_CONSTEXPR(     0  <= 0.0     );
    CCOMP_CONSTEXPR(     1  <  0.0     );
    CCOMP_CONSTEXPR(     1  <  1.0     );
    CCOMP_CONSTEXPR(     1  <= 0.0     );
    CCOMP_CONSTEXPR(     1  <= 1.0     );
    CCOMP_CONSTEXPR(     1  <=  2      );
    CCOMP_CONSTEXPR(     0  >  0.0     );
    CCOMP_CONSTEXPR(     0  >= 0.0     );
    CCOMP_CONSTEXPR(     1  >  0.0     );
    CCOMP_CONSTEXPR(     1  >  1.0     );
    CCOMP_CONSTEXPR(     1  >= 0.0     );
    CCOMP_CONSTEXPR(     1  >= 1.0     );
    CCOMP_CONSTEXPR(     1  >=  2      );
    CCOMP_CONSTEXPR(   0.0  <    0     );
    CCOMP_CONSTEXPR(   0.0  <=   0     );
    CCOMP_CONSTEXPR(   0.0  <    1     );
    CCOMP_CONSTEXPR(   1.0  <    1     );
    CCOMP_CONSTEXPR(   0.0  <=   1     );
    CCOMP_CONSTEXPR(   1.0  <=   1     );
    CCOMP_CONSTEXPR(    2   <=   1     );
    CCOMP_CONSTEXPR(  0.0   >    0     );
    CCOMP_CONSTEXPR(  0.0   >=   0     );
    CCOMP_CONSTEXPR(  0.0   >    1     );
    CCOMP_CONSTEXPR(  1.0   >    1     );
    CCOMP_CONSTEXPR(  0.0   >=   1     );
    CCOMP_CONSTEXPR(  1.0   >=   1     );
    CCOMP_CONSTEXPR(   2    >=   1     );

    CCOMP_CONSTEXPR(    0.0 <  0.0   );
    CCOMP_CONSTEXPR(    0.0 <= 0.0   );
    CCOMP_CONSTEXPR(    1.0 <  0.0   );
    CCOMP_CONSTEXPR(    1.0 <  1.0   );
    CCOMP_CONSTEXPR(    1.0 <= 0.0   );
    CCOMP_CONSTEXPR(    1.0 <= 1.0   );
    CCOMP_CONSTEXPR(    1.0 <= 2.0   );
    CCOMP_CONSTEXPR(    0.0 >  0.0   );
    CCOMP_CONSTEXPR(    0.0 >= 0.0   );
    CCOMP_CONSTEXPR(    1.0 >  0.0   );
    CCOMP_CONSTEXPR(    1.0 >  1.0   );
    CCOMP_CONSTEXPR(    1.0 >= 0.0   );
    CCOMP_CONSTEXPR(    1.0 >= 1.0   );
    CCOMP_CONSTEXPR(    1.0 >= 2.0   );

    // bitwise (just int)
    CCOMP_CONSTEXPR(     0  &  0   );
    CCOMP_CONSTEXPR(     0  ^  0   );
    CCOMP_CONSTEXPR(     0  |  0   );
    CCOMP_CONSTEXPR(     0  &  1   );
    CCOMP_CONSTEXPR(     0  ^  1   );
    CCOMP_CONSTEXPR(     0  |  1   );
    CCOMP_CONSTEXPR(     1  &  0   );
    CCOMP_CONSTEXPR(     1  ^  0   );
    CCOMP_CONSTEXPR(     1  |  0   );
    CCOMP_CONSTEXPR(    16  & 64   );
    CCOMP_CONSTEXPR(    16  ^ 64   );
    CCOMP_CONSTEXPR(    16  | 64   );
    CCOMP_CONSTEXPR(   255  & 16   );
    CCOMP_CONSTEXPR(   255  ^ 16   );
    CCOMP_CONSTEXPR(   255  | 16   );
    CCOMP_CONSTEXPR(     5  &  3   );
    CCOMP_CONSTEXPR(     5  ^  3   );
    CCOMP_CONSTEXPR(     5  |  3   );

    // bitwise not(just int)
    CCOMP_CONSTEXPR(    ~0         );
    CCOMP_CONSTEXPR(   ~~0         );
    CCOMP_CONSTEXPR(    ~1         );
    CCOMP_CONSTEXPR(   ~~1         );
    CCOMP_CONSTEXPR(   ~1 &  5     );
    CCOMP_CONSTEXPR(    1 &  ~5    );
    CCOMP_CONSTEXPR(   ~1 & ~5     );
    CCOMP_CONSTEXPR(   ~1 && false );

    #if defined(__clang__)
    #   pragma clang diagnostic pop
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic pop
    #endif
}

// #################################################################################################
// ### Exceptions
// #################################################################################################
UT_METHOD(Exceptions)
{
    UT_INIT()

    //  std configuration
    {
        Compiler compiler;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( toUpper(5)         , expressions::Exceptions::UnknownFunction );
        CONSTEXPR( Format(5, 3.14, 7) , expressions::Exceptions::UnknownFunction );

        CONSTEXPR( true(), true );
        CONSTEXPR( true[1]            , expressions::Exceptions::BinaryOperatorNotDefined );
        CONSTEXPR( true & true        , true   );
        CONSTEXPR( true = true        , true   );

        // test precedence of equal and assign
        compiler.CfgNormalization+= Normalization::RedundantBinaryOpBrackets;
        compiler.CfgNormalization-= Normalization::ReplaceAliasOperators;
        EXPRNORM(true = true == false,  (true = true) == false );
    }

    //  change some settings
    {
        Compiler compiler;
        compiler.CfgCompilation-= Compilation::AllowEmptyParenthesesForIdentifierFunctions;
        compiler.CfgCompilation-= Compilation::AllowSubscriptOperator;
        compiler.CfgCompilation-= Compilation::AllowBitwiseBooleanOperators;
        compiler.CfgCompilation-= Compilation::AliasEqualsOperatorWithAssignOperator;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);


        CONSTEXPR( true()             , expressions::Exceptions::IdentifierWithFunctionParentheses );
        CONSTEXPR( true[1]            , expressions::Exceptions::SyntaxError );
        CONSTEXPR( true & true        , expressions::Exceptions::BinaryOperatorNotDefined );
        CONSTEXPR( true = true        , expressions::Exceptions::BinaryOperatorNotDefined );
    }

    //  these tests try to produce memory leaks in the parser, hence are made for valgrind runs
    {
        Compiler compiler;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( true(1  2)                , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( true(1, 2 3)              , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( true(1, 2, 3 4)           , expressions::Exceptions::SyntaxErrorExpectation );

        CONSTEXPR( 1 2 ? 3 : 4               , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? 2 3 : 4               , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? 2 : 3 4               , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? ( 2 ? 3 4 : 5 ) : 6   , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? ( 2 ? 3 : 4 5 ) : 6   , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? 2 : ( 1 : 2 3 )       , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? 2 : ( 3 ? 4 5 : 6 )   , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1 ? 2 : ( 3 ? 4 : 5 6 )   , expressions::Exceptions::SyntaxErrorExpectation );

        CONSTEXPR( 1 *                       , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( 1  2   * 3                , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( (1 2 ) * 3                , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( (1 + 2 ) *                , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( (1 + 2 ) * 3 4            , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( (1 + 2 ) * (3 4)          , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( (1 + 2 ) * (3 +(4 5 ))    , expressions::Exceptions::SyntaxErrorExpectation );

        CONSTEXPR( true[1 + 2]               , expressions::Exceptions::BinaryOperatorNotDefined );
        CONSTEXPR( true[1   2]               , expressions::Exceptions::SyntaxErrorExpectation );

        // literals
        try { compiler.Compile( A_CHAR(R"("X" + "y )")  ); } catch(Exception& e)
        { UT_TRUE( e.Type() ==expressions::Exceptions::SyntaxErrorExpectation )   }
        try { compiler.Compile( A_CHAR(R"("abc)"     )  ); } catch(Exception& e)
        { UT_TRUE( e.Type() ==expressions::Exceptions::SyntaxErrorExpectation )   }

        CONSTEXPR( 1e5     , 1e5 );
        CONSTEXPR( 1E5     , 1E5 );
        CONSTEXPR( 1X5     , expressions::Exceptions::SyntaxErrorExpectation );

        CONSTEXPR( -   1X5 , expressions::Exceptions::SyntaxErrorExpectation );
        CONSTEXPR( - + 1X5 , expressions::Exceptions::SyntaxErrorExpectation );
    }

    // other "weird" input
    {
        Compiler compiler;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( [          , expressions::Exceptions::SyntaxErrorExpectation    );
        CONSTEXPR( ]          , expressions::Exceptions::SyntaxErrorExpectation    );
        CONSTEXPR( +          , expressions::Exceptions::SyntaxErrorExpectation    );
        CONSTEXPR( &          , expressions::Exceptions::UnknownUnaryOperatorSymbol);
        CONSTEXPR( ~          , expressions::Exceptions::SyntaxErrorExpectation    );
        CONSTEXPR( /          , expressions::Exceptions::UnknownUnaryOperatorSymbol);
        CONSTEXPR(  ^         , expressions::Exceptions::UnknownUnaryOperatorSymbol);
        EXPR_STR( A_CHAR(")" ), expressions::Exceptions::SyntaxErrorExpectation, 0 );
        EXPR_STR( A_CHAR("(" ), expressions::Exceptions::SyntaxErrorExpectation, 0 );
        EXPR_STR( A_CHAR("," ), expressions::Exceptions::SyntaxErrorExpectation, 0 );
        EXPR_STR( A_CHAR("\""), expressions::Exceptions::SyntaxErrorExpectation, 0 );
        CONSTEXPR(  .         , expressions::Exceptions::SyntaxError               );
        CONSTEXPR(  {         , expressions::Exceptions::SyntaxError);
        EXPR_STR (A_CHAR("$" ), expressions::Exceptions::SyntaxError           , 0 );
    }

    // parse localized numberformat
    {
        Expression expression;
        Compiler compiler;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);
        Scope ctScope(compiler.CfgFormatter);


DOX_MARKER( [DOX_EXPRESSIONS_LITERALS_DECIMALPOINTCHAR])
compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= ',';

expression= compiler.Compile( A_CHAR("1,5")  );
DOX_MARKER( [DOX_EXPRESSIONS_LITERALS_DECIMALPOINTCHAR])
        UT_EQ( 1.5, expression->Evaluate(scope).Unbox<double>()  )

        // This is a little complex: if it is a constant expression, changes
        // of the formatter of the compiler after compilation have no effect, while...
        expression= compiler.Compile( A_CHAR(R"( Format("{} {} {}", 1,5,2,5,3,5 )  )") );
        compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= '.';
        UT_EQ( A_CHAR("1,5 2,5 3,5"), expression->Evaluate(scope).Unbox<String>()  )

        // ...if its non-constant, is has.
        compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= ',';
        expression= compiler.Compile( A_CHAR(R"( Format("{} {} {} {}", 1,5,2,5,3,5 , random<0,0)  )"));
        compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= '.';
        UT_EQ( A_CHAR("1.5 2.5 3.5 false"), expression->Evaluate(scope).Unbox<String>()  )

        // parse with spaces
        compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= ',';
        expression= compiler.Compile( A_CHAR(R"( Format("{} {} {}", 1,5 , 2 , 3 )  )")  );
        compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= '.';
        UT_EQ( A_CHAR("1,5 2 3"), expression->Evaluate(scope).Unbox<String>()  )
    }

    // compile-time exceptions in plug-in
    {
        Compiler compiler;
        CTExceptionThrowers exceptionThrowers(compiler);
        compiler.SetupDefaults();
        compiler.InsertPlugin( &exceptionThrowers );
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( "Will throw" + ThrowALib , expressions::Exceptions::ExceptionInPlugin );
        CONSTEXPR( 42           + ThrowStd  , expressions::Exceptions::ExceptionInPlugin );

        compiler.CfgCompilation+= Compilation::PluginExceptionFallThrough;

        CONSTEXPR( "Will throw" + ThrowALib ,  lang::format::FMTExceptions::MissingClosingBracket );

        bool stdExceptionCaught= false;
        try
        {
            CONSTEXPR( 42 + ThrowStd        , expressions::Exceptions::ExceptionInCallback );
        }
        catch(std::exception& )
        {
            stdExceptionCaught= true;
        }
        UT_TRUE(stdExceptionCaught)
    }

    // run-time exceptions in callbacks
    {
        Compiler compiler;
        ETExceptionThrowers exceptionThrowers(compiler);
        compiler.SetupDefaults();
        compiler.InsertPlugin( &exceptionThrowers );
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( "Will throw" + ThrowALib , expressions::Exceptions::ExceptionInCallback );
        CONSTEXPR( 42           + ThrowStd  , expressions::Exceptions::ExceptionInCallback );

        compiler.CfgCompilation+= Compilation::CallbackExceptionFallThrough;

        CONSTEXPR( "Will throw" + ThrowALib ,  lang::format::FMTExceptions::MissingClosingBracket );

        bool stdExceptionCaught= false;
        try
        {
            CONSTEXPR( 42 + ThrowStd        , expressions::Exceptions::ExceptionInCallback );
        }
        catch(std::exception& )
        {
            stdExceptionCaught= true;
        }
        UT_TRUE(stdExceptionCaught)
    }

    // Formatter exception inside CBFormat
    {
        Compiler compiler;
        compiler.SetupDefaults();
        Scope scope(compiler.CfgFormatter);

        CONSTEXPR( Format("Hex: {:x}", 3.1415          ), expressions::Exceptions::ExceptionInPlugin   );
        CONSTEXPR( Format("Hex: {:x}", (rand * 3.1415) ), expressions::Exceptions::ExceptionInCallback );
    }
}

// #################################################################################################
// ### Conditional & Elvis
// #################################################################################################
UT_METHOD(Conditional)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

//--------------- all built-in combinations of T and F --------------
      CONSTEXPR( "Hello" , A_CHAR("Hello") );
      CONSTEXPR( ""      , A_CHAR("")      );


CCOMP_CONSTEXPR( true  ? false : true       );
CCOMP_CONSTEXPR( true  ? true  : false      );
CCOMP_CONSTEXPR( false ? false : true       );
CCOMP_CONSTEXPR( false ? true  : false      );

CCOMP_CONSTEXPR( true  ? false :     2      );
CCOMP_CONSTEXPR( true  ? true  :     2      );
CCOMP_CONSTEXPR( false ? false :     2      );
CCOMP_CONSTEXPR( false ? true  :     2      );

CCOMP_CONSTEXPR( true  ? false :   2.0      );
CCOMP_CONSTEXPR( true  ? true  :   2.0      );
CCOMP_CONSTEXPR( false ? false :   2.0      );
CCOMP_CONSTEXPR( false ? true  :   2.0      );



      CONSTEXPR( true  ? false : "str"    , A_CHAR("false"));
      CONSTEXPR( true  ? true  : "str"    , A_CHAR("true") );
      CONSTEXPR( false ? false : "str"    , A_CHAR("str")  );
      CONSTEXPR( false ? true  : "str"    , A_CHAR("str")  );

      CONSTEXPR( true  ?    1  :     2    ,      1 );
      CONSTEXPR( false ?    1  :     2    ,      2 );

      CONSTEXPR( true  ?    1  :   2.0    ,    1.0 );
      CONSTEXPR( false ?    1  :   2.0    ,    2.0 );

      CONSTEXPR( true  ?    1  :  "str"   ,    A_CHAR("1") );
      CONSTEXPR( false ?    1  :  "str"   ,  A_CHAR("str") );

      CONSTEXPR( true  ?  3.0  :     4.0  ,    3.0 );
      CONSTEXPR( false ?  3.0  :     4.0  ,    4.0 );

      CONSTEXPR( true  ?  3.0  :  "str"   ,  A_CHAR("3.0") );
      CONSTEXPR( false ?  3.0  :  "str"   ,  A_CHAR("str") );


CCOMP_CONSTEXPR( true  ?      2   : false    );
CCOMP_CONSTEXPR( true  ?      2   : true     );
CCOMP_CONSTEXPR( false ?      2   : false    );
CCOMP_CONSTEXPR( false ?      2   : true     );

CCOMP_CONSTEXPR( true  ?    2.0   : false    );
CCOMP_CONSTEXPR( true  ?    2.0   : true     );
CCOMP_CONSTEXPR( false ?    2.0   : false    );
CCOMP_CONSTEXPR( false ?    2.0   : true     );

      CONSTEXPR( false ?  "str" : false  , A_CHAR("false") );
      CONSTEXPR( false ?  "str" : true   , A_CHAR("true") );
      CONSTEXPR( true  ?  "str" : false  , A_CHAR("str")  );
      CONSTEXPR( true  ?  "str" : true   , A_CHAR("str")  );

      CONSTEXPR( false ?    2.0 :    1   ,    1.0 );
      CONSTEXPR( true  ?    2.0 :    1   ,    2.0 );

      CONSTEXPR( false ?   "str":    1   ,    A_CHAR("1") );
      CONSTEXPR( true  ?   "str":    1   ,  A_CHAR("str") );

      CONSTEXPR( false ?    4.0 :  3.0   ,    3.0 );
      CONSTEXPR( true  ?    4.0 :  3.0   ,    4.0 );

      CONSTEXPR( false ?   "str":  3.0   ,  A_CHAR("3.0") );
      CONSTEXPR( true  ?   "str":  3.0   ,  A_CHAR("str") );



//--------------- all built-in versions of Q --------------
      CONSTEXPR( true  ?   3.9  : "five" , A_CHAR("3.9")  );
      CONSTEXPR( false ?   3.9  : "five" , A_CHAR("five") );
      CONSTEXPR(     1 ?   3.9  : "five" , A_CHAR("3.9")  );
      CONSTEXPR(     2 ?  "six" : "five" , A_CHAR("six")  );
      CONSTEXPR(     0 ?   3.9  : "five" , A_CHAR("five") );
      CONSTEXPR(   1.0 ?   3.9  : "five" , A_CHAR("3.9")  );
      CONSTEXPR(   2.5 ?  "six" : "five" , A_CHAR("six")  );
      CONSTEXPR(   0.0 ?   3.9  : "five" , A_CHAR("five") );
      CONSTEXPR(   "x" ?   3.9  : "five" , A_CHAR("3.9")  );
      CONSTEXPR(   "xy"? "six"  : "five" , A_CHAR("six")  );
      CONSTEXPR(    "" ?   3.9  : "five" , A_CHAR("five") );

//exit(0);

//--------------- others --------------

      CONSTEXPR( true  ? 5 : "five"       , A_CHAR("5")     );
      CONSTEXPR( false ? 6 : "six"        , A_CHAR("six")   );
      CONSTEXPR( true  ? "Hello" : "World", A_CHAR("Hello") );
      CONSTEXPR( false ? "Hello" : "World", A_CHAR("World") );

      CONSTEXPR( true   ? 5       :   6  , 5    );
      CONSTEXPR( false  ? 5       :   6  , 6    );
      CONSTEXPR( !false ? 5 *7 /2 : 6+9  , 17   );
      CONSTEXPR( false  ? 5 *7 /2 : 6+9  , 15   );

CCOMP_CONSTEXPR( 1 ? 2 : 3 ? 4 : 5 );
CCOMP_CONSTEXPR(  -(  true ? 1 + 2 : 3 + 4  ));
CCOMP_CONSTEXPR(  -(  true ? 1 + 2 : 3 + 4  ));
      CONSTEXPR( (3 + 1) * 0   , 0 );
      CONSTEXPR( false ? "1" : 0  , A_CHAR("0"));
      CONSTEXPR( true  ? 0 : "1"  , A_CHAR("0"));
      CONSTEXPR( 0     ? "a" : false    , A_CHAR("false"));
      CONSTEXPR( true ? 0 : ( 1.2 ? "a" : (false ? 6 : 7.3)),  A_CHAR("0"));
      CONSTEXPR( true   ? (false  ? (false  ? (false  ? (false  ? 1   : 2.3)   : 3.3)   : 4.3)   : 5.3)   : 6.3 , 5.3   );
      CONSTEXPR( false  ? (false  ? (false  ? (false  ? (false  ? 1   : 2.3)   : 3.3)   : 4.3)   : 5.3)   : 6.3 , 6.3   );
      CONSTEXPR( true   ? (false  ? false  ? 1   : 2.3   : 3.3)   : 4.3 , 3.3   );


      CONSTEXPR( (true ? true : false )  & 1       , true  );
      CONSTEXPR( (true ? 1 : 2 )         & true    , true  );
      CONSTEXPR(  1     &  (true  ? true : false ) , true  );
      CONSTEXPR(  1     &  (false ? true : false ) , false );
      CONSTEXPR(  true  &  (true  ? 1 : 2 )        , true  );
      CONSTEXPR(  true  &  (false ? 1 : 2 )        , true  );

      CCOMP_CONSTEXPR( 1 > 5 ? 6 : 7 );

//--------------- Elvis  --------------

      CONSTEXPR( false    ?: false          , false    );
      CONSTEXPR( false    ?: true           , true     );
      CONSTEXPR( true     ?: false          , true     );
      CONSTEXPR( true     ?: true           , true     );
      CONSTEXPR(   0      ?: 7              , 7        );
      CONSTEXPR(   0.0    ?: 7.0            , 7.0      );
      CONSTEXPR(    5     ?: 6              , 5        );
      CONSTEXPR(   ""     ?: "Elvis"        , A_CHAR("Elvis")  );
      CONSTEXPR( "Johnny" ?: "Elvis"        , A_CHAR("Johnny") );

      CONSTEXPR( true  ? 7? :6 : 3          , 7);
      CONSTEXPR( true  ? 0? :6 : 3          , 6);
      CONSTEXPR( false ? 0? :6 : 3          , 3);

    // non-constant/partly constant
      CONSTEXPR( 1.0  ?: rand   , 1.0       );
     EXPRESSION( 0.0  ?: rand   , nullptr, 1);
     EXPRESSION( rand ?: 0.0    , nullptr, 3);
     EXPRESSION( rand ?: rand   , nullptr, 3);

    // including casts
     EXPRESSION( true  ?: rand      , 1.0    , 1);
     EXPRESSION( false ?: rand      , nullptr, 1);
     EXPRESSION( rand ?: false      , nullptr, 3);
     EXPRESSION( rand ?: (rand<0.5) , nullptr, 6);

      CONSTEXPR(   1  ?: rand   , 1.0       );
     EXPRESSION(   0  ?: rand   , nullptr, 1);
     EXPRESSION( rand ?:   0    , nullptr, 3);

      CONSTEXPR(  "X" ?: rand   , A_CHAR("X")       );
     EXPRESSION(  ""  ?: rand   , nullptr, 2);
     EXPRESSION( rand ?:  "X"   , nullptr, 4);
}

// #################################################################################################
// ### Strings
// #################################################################################################
UT_METHOD(Strings)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    MyScope scope(compiler);

    CONSTEXPR( ""     , A_CHAR(""     )   );
    CONSTEXPR( "Hello", A_CHAR("Hello")   );

    // unary !, +, -
    CONSTEXPR( !""                                 , true     );
    CONSTEXPR( !"" ? 1 : 2                         , 1        );
    CONSTEXPR( !"Hello"                            , false    );
    CONSTEXPR( !"Hello" ? 1 : 2                    , 2        );
    CONSTEXPR( !!"Hello"                           , true     );
    CONSTEXPR( !!"Hello" ? 1 : 2                   , 1        );

    CONSTEXPR( +"Hello"                            , A_CHAR("HELLO")  );
    CONSTEXPR( -"Hello"                            , A_CHAR("hello")  );

    // adding strings
    CONSTEXPR(  "Hello"  +  "World"                , A_CHAR("HelloWorld") );
    CONSTEXPR(  +"Hello" + -"World"                , A_CHAR("HELLOworld") );


    // adding bool and numbers
    CONSTEXPR(  "Bool: " + true                    , A_CHAR("Bool: true")  );
    CONSTEXPR(  "Bool: " + false                   , A_CHAR("Bool: false") );
    CONSTEXPR(  "Bool: " + (true  ? "OK" : "ERR")  , A_CHAR("Bool: OK")    );
    CONSTEXPR(  "Bool: " + (false ? "OK" : "ERR")  , A_CHAR("Bool: ERR")   );


    CONSTEXPR(  "Integer: " +  5000                , A_CHAR("Integer: 5000")  );
    CONSTEXPR(  "Integer: " + -5123                , A_CHAR("Integer: -5123") );

    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::WriteGroupChars;
    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::ReadGroupChars;
    compiler.CfgFormatter->DefaultNumberFormat.ThousandsGroupChar= ',';
    compiler.CfgFormatter->DefaultNumberFormat.PlusSign= '+';
    CONSTEXPR(  "Integer: " +  5000                , A_CHAR("Integer: +5,000"));

    compiler.CfgFormatter->DefaultNumberFormat.DecimalPointChar= '.';
    CONSTEXPR(  "Float: "   +  3.1415              , A_CHAR("Float: +3.1415") );
    CONSTEXPR(  "Float: "   + -3.1415              , A_CHAR("Float: -3.1415") );


    CONSTEXPR(  true                   + " <-Bool" , A_CHAR("true <-Bool")  );
    CONSTEXPR(  false                  + " <-Bool" , A_CHAR("false <-Bool") );
    CONSTEXPR(  (true  ? "OK" : "ERR") + " <-Bool" , A_CHAR("OK <-Bool")    );
    CONSTEXPR(  (false ? "OK" : "ERR") + " <-Bool" , A_CHAR("ERR <-Bool")   );


    CONSTEXPR(   -5123    + " <-Integer"           , A_CHAR("-5,123 <-Integer"));
    CONSTEXPR(    5000    + " <-Integer"           , A_CHAR("+5,000 <-Integer"));
    CONSTEXPR(    3.1415  + " <-Float"             , A_CHAR("+3.1415 <-Float") );
    CONSTEXPR(   -3.1415  + " <-Float"             , A_CHAR("-3.1415 <-Float") );

    // reset scope
    compiler.CfgFormatter->DefaultNumberFormat.SetComputational();

    // adding arbitrary boxes to strings
    MyFunctions myIdentifierPlugin(compiler);
    compiler.InsertPlugin( &myIdentifierPlugin );

    CONSTEXPR(   name             , A_CHAR("Joe")                );
   EXPRESSION(   "Name: " + name  , A_CHAR("Name: Joe")   ,3     );
   EXPRESSION(   "Name: " + +name , A_CHAR("Name: JOE")   ,4     );

    CONSTEXPR(   age              , 42                   );
   EXPRESSION(   "Age: "  + age   , A_CHAR("Age: 42")     ,3     );

   EXPRESSION(   age *   2   ,  84,  3 );
   EXPRESSION(   age *   1   ,  42,  1 );
   EXPRESSION(   age *   0   ,   0,  1 );
   EXPRESSION(     2 * age   ,  84,  3 );
   EXPRESSION(     1 * age   ,  42,  1 );
   EXPRESSION(     0 * age   ,   0,  1 );


DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_3])
ALIB_BOXING_BOOTSTRAP_REGISTER_FAPPEND_FOR_APPENDABLE_TYPE( ut_aworx::MyType* )
DOX_MARKER( [DOX_EXPRESSIONS_STRINGOPS_IAPPEND_3])

    CONSTEXPR(   raw              , scope.MyObject          );
   EXPRESSION( "MyObject: " + raw , A_CHAR("MyObject: Joe/42")  , 3 );
   EXPRESSION( raw + " <-MyObject", A_CHAR("Joe/42 <-MyObject") , 3 );

    // Comparison
    CONSTEXPR(  "ABC" ==  "ABC"       , true      );
    CONSTEXPR(  "ABC" !=  "ABC"       , false     );
    CONSTEXPR(  "ABC" ==  "AXC"       , false     );
    CONSTEXPR(  "ABC" !=  "AXC"       , true      );
    CONSTEXPR(  "ABC" ==  "abc"       , false     );
    CONSTEXPR(  "ABC" == +"abc"      , true      );
    CONSTEXPR( -"ABC" ==  "abc"      , true      );
    CONSTEXPR( !"ABC"                  , false     );
    CONSTEXPR( !"ABC" ==  ("ABC" ==""), true      );


    CONSTEXPR(  "ABC" <   "ABC"       , false     );
    CONSTEXPR(  "ABC" <=  "ABC"       , true      );
    CONSTEXPR(  "ABC" <   "CDE"       , true      );
    CONSTEXPR(  "CDE" <   "ABC"       , false     );
    CONSTEXPR(  "CDE" <=  "ABC"       , false     );

    CONSTEXPR(  "ABC" >   "ABC"       , false     );
    CONSTEXPR(  "ABC" >=  "ABC"       , true      );
    CONSTEXPR(  "CDE" >   "ABC"       , true      );
    CONSTEXPR(  "ABC" >   "CDE"       , false     );
    CONSTEXPR(  "ABC" >=  "CDE"       , false     );

    // wildcard
   EXPRESSION(  name  * "Joe"         , true   , 3  );
   EXPRESSION(  name  * "Joe" && false, false  , 1  );
    CONSTEXPR(  "ABC" * "?B*"         , true        );
#if ALIB_FEAT_BOOST_REGEX && (!ALIB_CHARACTERS_WIDE || ALIB_CHARACTERS_NATIVE_WCHAR)
    CONSTEXPR(  "ABC" % ".B."         , true        );
#endif

    CONSTEXPR(  "ABC" =  "ABC"        , true      );


    CONSTEXPR(  toupper("Hello")                 , A_CHAR("HELLO") );
    CONSTEXPR( -toupper("Hello")                 , A_CHAR("hello") );
    CONSTEXPR( -toupper(toupper("Hello"))        , A_CHAR("hello") );

}

// #################################################################################################
// ### Nested
// #################################################################################################
UT_METHOD(Nested)
{
    UT_INIT()


    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

    compiler.AddNamed(A_CHAR("nested"), A_CHAR("42") );


    // exception when using unary operator '*'
    CONSTEXPR( *"notdefined"        , expressions::Exceptions::NestedExpressionNotFoundCT );
    CONSTEXPR( *("notdefined")      , expressions::Exceptions::NestedExpressionNotFoundCT );
    CONSTEXPR( * 5                  , expressions::Exceptions::UnaryOperatorNotDefined    );
IF_ALIB_CAMP(
    CONSTEXPR( * (today + hours(5)) , expressions::Exceptions::UnaryOperatorNotDefined    );
    CONSTEXPR( * (today)            , expressions::Exceptions::NestedExpressionNotFoundCT );   )

    CONSTEXPR( * nested             , 42 );

IF_ALIB_CAMP(
    compiler.CfgCompilation-= Compilation::AllowIdentifiersForNestedExpressions;
    CONSTEXPR( * (today)            , expressions::Exceptions::UnaryOperatorNotDefined );
    compiler.CfgCompilation+= Compilation::AllowIdentifiersForNestedExpressions;               )

    // exception when using function "Expression()"
    CONSTEXPR( Expression("notdefined")                 , expressions::Exceptions::NestedExpressionNotFoundCT );
    CONSTEXPR( Expression( notdefined )                 , expressions::Exceptions::NestedExpressionNotFoundCT );
   EXPRESSION( Expression("notdefined", true)           , true , 3);
   EXPRESSION( Expression( notdefined , true)           , true , 3);
    CONSTEXPR( Expression("notdefined", true, throw)    , expressions::Exceptions::NestedExpressionNotFoundET );
    CONSTEXPR( Expression( notdefined , true, throw)    , expressions::Exceptions::NestedExpressionNotFoundET );
    CONSTEXPR( Expression                               , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression()                             , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression(5)                            , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression(5, throw)                     , expressions::Exceptions::UnknownIdentifier );
    CONSTEXPR( Expression(5, 5, throw)                  , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression(5, 5, throw, 4)               , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression(5, 5, 3, throw)               , expressions::Exceptions::NestedExpressionCallArgumentMismatch );


IF_ALIB_CAMP(
    CONSTEXPR( Expression(""+today)            , expressions::Exceptions::NamedExpressionNotConstant ); )

    CONSTEXPR( Expression("nested" )           , 42 );
    CONSTEXPR( Expression( nested  )           , 42 );


   EXPRESSION( Expression("nested" + (random < 0 ? "Never" : ""), Integer(true) )   , 42, 11 );
   EXPRESSION( Expression("nested" + (random < 0 ? "Never" : ""), "Stringtype"  )   , expressions::Exceptions::NestedExpressionResultTypeError, 11 );

    compiler.CfgCompilation-= Compilation::AllowIdentifiersForNestedExpressions;
    CONSTEXPR( Expression(notdefined)          , expressions::Exceptions::UnknownIdentifier );
    CONSTEXPR( Expression(notdefined, 1)       , expressions::Exceptions::UnknownIdentifier );
    compiler.CfgCompilation+= Compilation::AllowIdentifiersForNestedExpressions;

    // nested expressions with optional 3rd parameter
    CONSTEXPR( Expression(notdefined, 1  , 1)      , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    CONSTEXPR( Expression(notdefined, 1  , throw)  , expressions::Exceptions::NestedExpressionNotFoundET           );
   EXPRESSION( Expression(notdefined, 9  , true )  , expressions::Exceptions::NestedExpressionCallArgumentMismatch   , 3  );
   EXPRESSION( Expression(notdefined, "X" )        , A_CHAR("X") , 3  );
   EXPRESSION( Expression(notdefined, "X" + "Y")   , A_CHAR("XY") , 3  );
   EXPRESSION( Expression(notdefined, random < 0.0 )  , false , 5  );
   EXPRESSION( Expression(nested    , 43 , throw)  , 42   , 3  );
   EXPRESSION( Expression(nested    , 43        )  , 42   , 3  );

    compiler.CfgCompilation-= Compilation::AllowCompileTimeNestedExpressions;
    CONSTEXPR( *notdefined                         , expressions::Exceptions::UnaryOperatorNotDefined );
    CONSTEXPR( Expression(notdefined )             , expressions::Exceptions::NestedExpressionCallArgumentMismatch );
    compiler.CfgCompilation+= Compilation::AllowCompileTimeNestedExpressions;

    // test nested expression quotation normalization
    try
    {
        EXPRNORM( * "nested"  , *"nested"  );
        EXPRNORM( *  nested                       , *nested                              );
        EXPRNORM( Expres ("nested")               , Expression( "nested" )               );
        EXPRNORM( Expres ( nested)                , Expression( nested )                 );
        EXPRNORM( Expres ("nested", true)         , Expression( "nested", true )         );
        EXPRNORM( Expres ( nested   , true)       , Expression( nested, true )           );
        EXPRNORM( Expres ("nested",true,throw)    , Expression( "nested", true, throw )  );
        EXPRNORM( Expres ( nested   ,true,throw)  , Expression( nested, true, throw )    );
        compiler.CfgNormalization+=Normalization::QuoteUnaryNestedExpressionOperatorArgument;
        compiler.CfgNormalization-=Normalization::ReplaceFunctionNames;
        EXPRNORM( * "nested"                      , *"nested"                          );
        EXPRNORM( *  nested                       , *"nested"                          );
        EXPRNORM( Expres( "nested")               , Expres( "nested" )                 );
        EXPRNORM( Expres(  nested)                , Expres( "nested" )                 );
        EXPRNORM( Expres( "nested",true)          , Expres( "nested", true )           );
        EXPRNORM( Expres(  nested,true)           , Expres( "nested", true )           );
        EXPRNORM( Expres("nested",true,throw )    , Expres( "nested", true, throw )    );
        EXPRNORM( Expres(  nested,true,throw)     , Expres( "nested", true, throw )    );
    }
    catch( Exception& e )
    {
        UT_PRINT("Exception in Unit Test" )
        #define LOX_LOX ut.lox
        Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
        log_exception( ut, e );
        ALIB_ERROR( "UNITTESTS", "UT Failure" )
        #undef LOX_LOX
    }

    // removal of nested expression after compilation
    Expression expression= compiler.Compile( A_CHAR("*nested") );
    UT_TRUE ( expression->Evaluate(scope).Unbox<integer>() == 42 )
    UT_TRUE ( compiler.RemoveNamed(A_CHAR("nested")) )
    UT_FALSE( compiler.RemoveNamed(A_CHAR("nested")) )
    UT_TRUE ( expression->Evaluate(scope).Unbox<integer>() == 42 )
    expression= nullptr; // this calls expression destructor twice: this expression plus "nested".

    // Circular dependencies
    compiler.AddNamed( A_CHAR("first") , A_CHAR("Expression(first, 5)"));
   EXPRESSION( *first  , expressions::Exceptions::CircularNestedExpressions , 1  );
    compiler.AddNamed( A_CHAR("first") , A_CHAR("Expression(second, 5)"));
    compiler.AddNamed( A_CHAR("second"), A_CHAR("*first") );
   EXPRESSION( *first  , expressions::Exceptions::CircularNestedExpressions , 1  );

    compiler.AddNamed( A_CHAR("second"), A_CHAR("Expression(first, 6)") );
   EXPRESSION( *first  , expressions::Exceptions::CircularNestedExpressions , 1  );
}

// #################################################################################################
// ### NumberLiterals
// #################################################################################################
UT_METHOD(NumberLiterals)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();
    Scope scope(compiler.CfgFormatter);

    compiler.AddNamed(A_CHAR("nested"), A_CHAR("42") );


    // we use a function String which excepts variadic types to test parsing and writing back
    // numbers and test it only by checking the normalization.
    // For this we add space before and after the function parameters!
    compiler.CfgNormalization-= Normalization::ReplaceFunctionNames;
    compiler.CfgNormalization+= Normalization::FunctionSpaceBeforeComma;
    compiler.CfgNormalization+= Normalization::FunctionSpaceAfterComma;
    EXPRNORMNS(  str( 3.5 )        , "str( 3.5 )"       );
    EXPRNORMNS(  str( 3,5 )        , "str( 3 , 5 )"     );
    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::ReadGroupChars;
    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::WriteGroupChars;
    compiler.CfgFormatter->DefaultNumberFormat.Flags-= NumberFormatFlags::WriteGroupChars;
    compiler.CfgFormatter->DefaultNumberFormat.ThousandsGroupChar= ' ';
    EXPRNORMNS(  str( 3 500 )        , "str( 3500 )"         );
    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::WriteGroupChars;
    EXPRNORMNS(  str( 3 500 72 )     , "str( 350 072 )"      );
    EXPRNORMNS(  str( 3 500.72 )     , "str( 3 500.72 )"     );
    compiler.CfgFormatter->DefaultNumberFormat.Flags+= NumberFormatFlags::ForceScientific;
    EXPRNORMNS(  str( 3 500.72 )     , "str( 3.50072E+03 )"  );
    EXPRNORMNS(  str( 10.0     )     , "str( 1.0E+01 )"      );
    compiler.CfgFormatter->DefaultNumberFormat.Flags-= NumberFormatFlags::ForceScientific;
    EXPRNORMNS(  str( 10.0     )     , "str( 10.0 )"         );
    EXPRNORMNS(  str( 10.0E0   )     , "str( 10.0 )"         );
    compiler.CfgNormalization+= Normalization::KeepScientificFormat;
    EXPRNORMNS(  str( 10.0     )     , "str( 10.0 )"         );
    EXPRNORMNS(  str( 10.0E0   )     , "str( 1.0E+01 )"      );

    // integral values
    EXPRNORMNS(  10        , "10"      );
    EXPRNORMNS(  0x10      , "0x10"    );
    EXPRNORMNS(  0o721     , "0o721"   );
    EXPRNORMNS(  0b1011    , "0b1011"  );

    compiler.CfgNormalization+= Normalization::ForceBinary;
    EXPRNORMNS(  10        , "0b1010"  );
    compiler.CfgNormalization+= Normalization::ForceOctal;
    EXPRNORMNS(  10        , "0o12"    );
    compiler.CfgNormalization+= Normalization::ForceHexadecimal;
    EXPRNORMNS(  10        , "0xA"     );
    compiler.CfgFormatter->DefaultNumberFormat.HexFieldWidth= 6;
    EXPRNORMNS(  10        , "0x0'000A"     );
    compiler.CfgFormatter->DefaultNumberFormat.HexWordGroupChar= '*';
    EXPRNORMNS(  0x1*234A  , "0x1*234A"     );
}

// #################################################################################################
// ### MultiThreaded
// #################################################################################################
#if ALIB_THREADS
UT_METHOD(MultiThreaded)
{
    UT_INIT()

    Compiler compiler;
    compiler.SetupDefaults();

    Expression expr= compiler.Compile(A_CHAR("3 * 7"));

    Scope scope1(compiler.CfgFormatter);
    Scope scope2(compiler.CfgFormatter);
    UT_PRINT( "Result: ", expr->Evaluate(scope1) )


    EvaluationThread  eval1(expr, scope1, 1s);
    EvaluationThread  eval2(expr, scope2, 1s);
    eval1.Start();
    eval2.Start();


    while (     eval1.GetState() != EvaluationThread::State::Done
            &&  eval2.GetState() != EvaluationThread::State::Done )
    {
        Thread::Sleep(100ms);
        UT_PRINT( "Running: Evaluations: {}/ {}", eval1.Cnt, eval2.Cnt )
    }

    UT_PRINT( "Done:    Evaluations: {}/ {}  -> {} per eval", eval1.Cnt, eval2.Cnt,
              eval1.duration/ double(eval1.Cnt + eval2.Cnt) )
    eval1.Join();
    eval2.Join();
    UT_PRINT( "Threads joined" )
}
#endif // ALIB_THREADS



#include "unittests/aworx_unittests_end.hpp"

} //namespace

ALIB_WARNINGS_RESTORE

#endif // ALIB_UT_EXPRESSIONS

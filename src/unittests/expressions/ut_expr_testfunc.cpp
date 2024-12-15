// #################################################################################################
//  AWorx ALib Unit Tests
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"
#include "unittests/alib_test_selection.hpp"

#if ALIB_UT_EXPRESSIONS ||  ALIB_UT_DOCS

#include "alib/alox.hpp"
#if ALIB_ALOX
#   include "alib/alox/logtools.hpp"
#endif

#include "alib/boxing/dbgboxing.hpp"

#if ALIB_EXPRESSIONS
#   include "alib/expressions/compiler.hpp"
#endif
#include "alib/expressions/scope.hpp"
#include "alib/expressions/plugins/arithmetics.hpp"
#include "alib/expressions/detail/program.hpp"
#include "alib/expressions/detail/virtualmachine.hpp"

#if ALIB_TIME
#   include "alib/time/ticks.hpp"
#endif

#include <math.h>
#include <assert.h>

#include "unittests/aworx_unittests.hpp"


using namespace std;
using namespace alib;
using namespace alib;
#include "unittests/expressions/ut_expr_testfunc.hpp"

ALIB_WARNINGS_MACRO_NOT_USED_OFF

#define LOX_LOX ut.lox;

namespace ut_aworx {

// #################################################################################################
// ### Test method used by the test below, with two macros. One for expressions understood by
// ### C++, one for those that are not compatible.
// #################################################################################################
#if !ALIB_ALOX
extern    AString exceptOutput;
          AString exceptOutput;
#endif

#include "alib/lang/callerinfo_functions.hpp"
void log_exception( AWorxUnitTesting& ut, Exception& e )
{
    #if ALIB_ALOX
        LogTools::Exception( ut.lox, e, Verbosity::Info, nullptr, nullptr );
    #else
        (void) ut;
        e.Format( exceptOutput.Reset() );
        std::cout << exceptOutput << endl;
    #endif

}

void    testNormalizaton( const CallerInfo& ci,
                          AWorxUnitTesting&     ut,
                          Compiler&             compiler,
                          const String&         expressionString,
                          const String&         expected           )
{
    Expression   expression;

    //---------------------- Compile -------------------
    try
    {
        expression= compiler.Compile( expressionString );
    }
    catch( Exception& e )
    {
        ALIB_ASSERT( !expression )
        ut.Print (ci, alib::Verbosity::Info,
                  "------ Exception thrown when testing normalization -------\n"
                  "      Expression:  {{{}}}\n"
                  "Exception:",
                  expressionString      );
            log_exception( ut, e  );
            return;
    }

    if( !expression->GetNormalizedString().Equals( expected ) )
    {
        ut.Print (ci, alib::Verbosity::Info,
                  "------ Error in  normalization -------\n"
                  "   Expression:  {{{}}}\n"
                  "   Normalized:  {{{}}}\n"
                  "     Expected:  {{{}}}",
                  expressionString,
                  expression->GetNormalizedString(),
                  expected                            );
        assert(!ut.AssertOnFailure);
    }

}


Expression testExpression( const CallerInfo&    ci,
                           AWorxUnitTesting&    ut,
                           Compiler&            compiler,
                           expressions::Scope&  scope,
                           const String&        expressionString,
                           Box                  expected,
                           integer              programLength                               )
{
    Expression   expression;

    // fix integral and float types
    #if ALIB_FEAT_BOXING_BIJECTIVE_INTEGRALS
        if( !expected.IsType< integer>() && expected.IsSignedIntegral  () ) expected= expected.UnboxSignedIntegral();
        if( !expected.IsType<uinteger>() && expected.IsUnsignedIntegral() ) expected= expected.UnboxUnsignedIntegral();
    #endif

    #if ALIB_FEAT_BOXING_BIJECTIVE_FLOATS
        if( expected.IsType<float>() )          expected= expected.UnboxFloatingPoint();
    #endif

    //---------------------- Compile -------------------
    IF_ALIB_TIME( Ticks time; )
    try
    {
        expression= compiler.Compile( expressionString );
    }
    catch( Exception& e )
    {
        ALIB_ASSERT( !expression )
        if( expected == e.Type().CastToBox() )
        {
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            ut.Print (ci, alib::Verbosity::Info,
                  "Expression compilation threw exception as expected:\n"
                  "      Expression:  {{{}}}\n"
                  "Exception:",
                  expressionString      );
            log_exception( ut, e  );
            return expression;
        }

        if( expected.IsSameType(e.Type().CastToBox()) )
        {
            ut.Print (ci, alib::Verbosity::Info,
                  "--------- Error --------\n"
                  "Expression compilation threw a different exception than expected:\n"
                  "         Expression:  {{{}}}\n"
                  " Expected exception:  {}"     ,
                  expressionString               ,
                  expected.Unbox<expressions::Exceptions>()        );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        ut.Print (ci, alib::Verbosity::Info,
                  "--------- Error --------\n"
                  "Expression compilation threw UNEXPECTED exception:\n"
                  " Expression:  {{{}}}",
                  expressionString        );
        Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
        log_exception( ut, e  );
        assert(!ut.AssertOnFailure);
    }

    IF_ALIB_TIME( auto compileTime= time.Age(); )
    ALIB_ASSERT( expression )

    //---------------------- Evaluate -------------------
    Box result(0);
    IF_ALIB_TIME( time= Ticks::Now(); )
    try
    {
        result= expression->Evaluate( scope );
    }
    catch( Exception& e )
    {
        if( expected == e.Type().CastToBox() )
        {
            ut.Print (ci, alib::Verbosity::Info,
                  "Expression threw exception as expected:\n"
                  " Expression:  {{{}}}\n"
                  "  Normalized: {{{}}}",
                  expressionString,
                  expression->GetNormalizedString()             );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            return expression;
        }

        if( expected.IsSameType(e.Type().CastToBox()) )
        {
            ut.Print (ci, alib::Verbosity::Info,
                  "--------- Error --------\n"
                  "Expression threw different exception than expected:\n"
                  "         Expression:  {{{}}}\n"
                  "          Normalized: {{{}}}\n"
                  "  Expected exception: {}",
                  expressionString,
                  expression->GetNormalizedString(),
                  expected                                    );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        ut.Print (ci, alib::Verbosity::Info,
                  "--------- Error --------\n"
                  "Expression threw UNEXPECTED exception:\n"
                  " Expression:  {{{}}}\n"
                  "  Normalized: {{{}}}"  ,
                  expressionString,
                  expression->GetNormalizedString()             );
        Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
        log_exception( ut, e  );
        assert(!ut.AssertOnFailure);
    }

    IF_ALIB_TIME( auto evalTime= time.Age(); )

    //---------------- check result type -------------------
    if( !expected.IsType<void>() )
    {
        if( !expected.IsSameType(result) )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Error in expression result type:\n"
                      "             Expression: {{{}}}\n"
                      "             Normalized: {{{}}}\n"
                      "   Expected result type: {!Q<>} (value: {})\n"
                      "            Result type: {!Q<>}  (value: {})",
                      expressionString,
                      expression->GetNormalizedString(),
                      compiler.TypeName(expected), expected,
                      compiler.TypeName(result),   result
                    );
            assert(!ut.AssertOnFailure);
            return expression;
        }

        //---------------- check result value -------------------
        if( expected != result )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Error in expression result value:\n"
                      "          Expression: {{{}}}\n"
                      "          Normalized: {{{}}}\n"
                      "     Expected result: {}\n"
                      "              Result: {}"         ,
                      expressionString,
                      expression->GetNormalizedString(),
                      expected,
                      result  );
            assert(!ut.AssertOnFailure);
            return expression;
        }
    }

    //---------------- check parsing normalized -------------------
    IF_ALIB_TIME(
    Ticks::Duration recompileTime;
    Ticks::Duration recompiledEvalTime;
    time= Ticks::Now();                 )
    {
        Expression recompiled;
        try
        {
            recompiled= compiler.Compile( expression->GetNormalizedString() );
        }
        catch( Exception& e )
        {
            ALIB_ASSERT( !recompiled )
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Recompiling normalized (!!!) threw exception:\n"
                      "              Original: {{{}}}\n"
                      "  Failed normalization: {{{}}}",
                      expressionString,
                      expression->GetNormalizedString()          );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }
        IF_ALIB_TIME( recompileTime= time.Age(); )
        Box result2(0);

        IF_ALIB_TIME( time= Ticks::Now(); )
        try
        {
            result2= recompiled->Evaluate( scope );
        }
        catch( Exception& e )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Recompiled (!!!) Expression threw exception:\n"
                      "   Expression: {{{}}}\n"
                      "   Normalized: {{{}}}\n"
                      "    Optimized: {{{}}}\n"   ,
                      expressionString,
                      expression->GetNormalizedString(),
                      recompiled->GetNormalizedString()             );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }
        IF_ALIB_TIME( recompiledEvalTime= time.Age(); )

        //---------------- check recompiled result type -------------------
        if( !expected.IsType<void>() )
        {
            if( !expected.IsSameType(result2) )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result type of recompiled normalized expression:\n"
                          "            Expression: {{{}}}\n"
                          "            Normalized: {{{}}}\n"
                          "             Optimized: {{{}}}\n"
                          "  Expected result type: {}\n"
                          "           Result type: {}"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          recompiled->GetNormalizedString(),
                          compiler.TypeName(expected),
                          compiler.TypeName(result2)
                        );
                assert(!ut.AssertOnFailure);
                return expression;
            }

            //---------------- check recompiled result value -------------------
            if( expected != result2 )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result value of recompiled normalized expression:\n"
                          "       Expression: {{{}}}\n"
                          "       Normalized: {{{}}}\n"
                          "        Optimized: {{{}}}\n"
                          "  Expected result: {}\n"
                          "           Result: {}\n"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          recompiled->GetNormalizedString(),
                          expected,
                          result2  );
                assert(!ut.AssertOnFailure);
                return expression;
            }
        }
    }

    //---------------- check parsing optimized -------------------
    {
        Expression optimized;
        try
        {
            auto cfgComp= compiler.CfgCompilation;
            auto cfgNorm= compiler.CfgNormalization;
            compiler.CfgCompilation   = expressions::Compilation::DEFAULT;
            compiler.CfgNormalization = expressions::Normalization::DEFAULT;

                optimized= compiler.Compile( expression->GetOptimizedString() );

            compiler.CfgCompilation   = cfgComp;
            compiler.CfgNormalization = cfgNorm;

        }
        catch( Exception& e )
        {
            ALIB_ASSERT( !optimized )
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Recompiling OPTIMIZED normalized (!!!) threw exception:\n"
                      "                   Original: {{{}}}\n"
                      "                 Normalized: {{{}}}\n"
                      " Normalization of optimized: {{{}}}",
                      expressionString,
                      expression->GetNormalizedString(),
                      expression->GetOptimizedString()          );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        Box result2(0);
        try
        {
            result2= optimized->Evaluate( scope );
        }
        catch( Exception& e )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Recompiled (!!!) Expression threw exception:\n"
                      "  Expression: {{{}}}\n"
                      "  Normalized: {{{}}}\n"
                      "   Optimized: {{{}}}\n"   ,
                      expressionString,
                      expression->GetNormalizedString(),
                      optimized->GetNormalizedString()             );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        //---------------- check recompiled result type -------------------
        if( !expected.IsType<void>() )
        {
            if( !expected.IsSameType(result2) )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result type of recompiled normalized expression:\n"
                          "           Expression: {{{}}}\n"
                          "           Normalized: {{{}}}\n"
                          "            Optimized: {{{}}}\n"
                          " Expected result type: {}\n"
                          "          Result type: {}"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          optimized->GetNormalizedString(),
                          compiler.TypeName(expected),
                          compiler.TypeName(result2)
                        );
                assert(!ut.AssertOnFailure);
                return expression;
            }

            //---------------- check recompiled result value -------------------
            if( expected != result2 )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result value of recompiled normalized expression:\n"
                          "       Expression: {{{}}}\n"
                          "       Normalized: {{{}}}\n"
                          "        Optimized: {{{}}}\n"
                          "  Expected result: {}\n"
                          "           Result: {}\n"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          optimized->GetNormalizedString(),
                          expected,
                          result2  );
                assert(!ut.AssertOnFailure);
                return expression;
            }
        }

        //---------------- check program of optimized -------------------
        integer optimizedPLen= optimized->GetProgram()->Length();
        if( programLength > 0 && programLength != optimizedPLen )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Error in expression PROGRAM LENGTH of recompiling optimized expression:\n"
                      "       Expression: {{{}}}\n"
                      "       Normalized: {{{}}}\n"
                      "        Optimized: {{{}}}\n"
                      "  Expected length: {}\n"
                      "           length: {}"         ,
                      expressionString,
                      expression->GetNormalizedString(),
                      optimized->GetNormalizedString(),
                      programLength,
                      optimized->GetProgram()->Length()  );

            #if ALIB_DEBUG
                UT_PRINT("Progam listing:")
                UT_PRINT( expressions::detail::VirtualMachine::DbgList(
                           *dynamic_cast<expressions::detail::Program*>(optimized->GetProgram()) )  )
            #endif

            assert(!ut.AssertOnFailure);
        }
    }

    //---------------- check program length -------------------
    integer optimizedPLen= expression->GetProgram()->Length();
    if( programLength > 0 && programLength != optimizedPLen )
    {
        ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                  "Error in expression PROGRAM LENGTH:\n"
                  "      Expression: {{{}}}\n"
                  "      Normalized: {{{}}}\n"
                  " Expected length: {}\n"
                  "          length: {}"         ,
                  expressionString,
                  expression->GetNormalizedString(),
                  programLength,
                  expression->GetProgram()->Length()  );

        #if ALIB_DEBUG
            UT_PRINT("Progam listing:")
            UT_PRINT( expressions::detail::VirtualMachine::DbgList(
                       *dynamic_cast<expressions::detail::Program*>(expression->GetProgram()) )  )
        #endif

        assert(!ut.AssertOnFailure);
    }

    //---------------- NoOptimization -------------------
    integer nonOptimizedLen= 0;
    {
        // compile normalized without optimization
        compiler.CfgCompilation+= expressions::Compilation::NoOptimization;
        Expression expressionCompiledWitoutOptimizations;
        try
        {
            expressionCompiledWitoutOptimizations= compiler.Compile( expression->GetNormalizedString() );
        }
        catch( Exception& e )
        {
            compiler.CfgCompilation-= expressions::Compilation::NoOptimization;
            ALIB_ASSERT( !expressionCompiledWitoutOptimizations )
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Compiling with \"NoOptimization\" threw exception:\n"
                      "           Original: {{{}}}\n"
                      "  Normalized (used): {{{}}}",
                      expressionString,
                      expression->GetNormalizedString()          );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }
        compiler.CfgCompilation-= expressions::Compilation::NoOptimization;
        nonOptimizedLen= expressionCompiledWitoutOptimizations->GetProgram()->Length();


        Box result2(0);
        try
        {
            result2= expressionCompiledWitoutOptimizations->Evaluate( scope );
        }
        catch( Exception& e )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Evaluation of expression compiled with \"NoOptimization\"  threw exception:\n"
                      "                    Original: {{{}}}\n"
                      "           Normalized (used): {{{}}}",
                      "    Non-Optimized Normalized: {{{}}}\n"   ,
                      expressionString,
                      expression->GetNormalizedString(),
                      expressionCompiledWitoutOptimizations->GetNormalizedString()             );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        //---------------- check result type -------------------
        if( !expected.IsType<void>() )
        {
            if( !expected.IsSameType(result2) )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result type of expression compiled with \"NoOptimization\":\n"
                          "                            Expression: {{{}}}\n"
                          "                            Normalized: {{{}}}\n"
                          "  Non-Optimized Normalized (evaluated): {{{}}}\n"
                          "                  Expected result type: {}\n"
                          "                           Result type: {}"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          expressionCompiledWitoutOptimizations->GetNormalizedString(),
                          compiler.TypeName(expected),
                          compiler.TypeName(result2)
                        );
                assert(!ut.AssertOnFailure);
                return expression;
            }

            //---------------- check recompiled result value -------------------
            if( expected != result2 )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result value of expression compiled with \"NoOptimization\":\n"
                          "                            Expression: {{{}}}\n"
                          "                            Normalized: {{{}}}\n"
                          "  Non-Optimized Normalized (evaluated): {{{}}}\n"
                          "             Expected result: {}\n"
                          "                      Result: {}\n"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          expressionCompiledWitoutOptimizations->GetNormalizedString(),
                          expected,
                          result2  );
                assert(!ut.AssertOnFailure);
                return expression;
            }
        }

        // decompile non-optimized and compile once more. Note: this checks de-compilation on all
        // unit tests methods which are mostly constant. Hence, de-compilation would be only tested
        // with the use of non-const methods...and there are not too many built-in non-const
        // methods!
        Expression expressionCompiledWitoutOptimizationsDecompiled;
        try
        {
            expressionCompiledWitoutOptimizationsDecompiled= compiler.Compile( expressionCompiledWitoutOptimizations->GetOptimizedString() );
        }
        catch( Exception& e )
        {
            compiler.CfgCompilation-= expressions::Compilation::NoOptimization;
            ALIB_ASSERT( !expressionCompiledWitoutOptimizationsDecompiled )
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Compiling \"decompiled-non-optimized\" (!) threw exception:\n"
                      "                        Original: {{{}}}\n"
                      "                      Normalized: {{{}}}",
                      " Non-Optimized Decompiled (used): {{{}}}",
                      expressionString,
                      expression->GetNormalizedString()          );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        try
        {
            result2= expressionCompiledWitoutOptimizationsDecompiled->Evaluate( scope );
        }
        catch( Exception& e )
        {
            ut.Print (ci, alib::Verbosity::Info,
                      "--------- Error --------\n"
                      "Evaluation of \"decompiled-non-optimized\" (!) threw exception:\n"
                      "                               Original: {{{}}}\n"
                      "                  Normalized (original): {{{}}}",
                      "   Non-Optimized Decompiled (evaluated): {{{}}}\n"   ,
                      expressionString,
                      expression->GetNormalizedString(),
                      expressionCompiledWitoutOptimizationsDecompiled->GetNormalizedString()             );
            Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
            log_exception( ut, e  );
            assert(!ut.AssertOnFailure);
        }

        //---------------- check result type -------------------
        if( !expected.IsType<void>() )
        {
            if( !expected.IsSameType(result2) )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result type of \"decompiled-non-optimized\" (!):\n"
                          "                               Original: {{{}}}\n"
                          "                  Normalized (original): {{{}}}",
                          "   Non-Optimized Decompiled (evaluated): {{{}}}\n"   ,
                          "                   Expected result type: {}\n"
                          "                            Result type: {}"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          expressionCompiledWitoutOptimizationsDecompiled->GetNormalizedString(),
                          compiler.TypeName(expected),
                          compiler.TypeName(result2)
                        );
                assert(!ut.AssertOnFailure);
                return expression;
            }

            //---------------- check recompiled result value -------------------
            if( expected != result2 )
            {
                ut.Print (ci, alib::Verbosity::Info,
                          "--------- Error --------\n"
                          "Error in result type of \"decompiled-non-optimized\" (!):\n"
                          "                            Expression: {{{}}}\n"
                          "                            Normalized: {{{}}}\n"
                          "  Non-Optimized Decompiled (evaluated): {{{}}}\n"
                          "                       Expected result: {}\n"
                          "                                Result: {}\n"       ,
                          expressionString,
                          expression->GetNormalizedString(),
                          expressionCompiledWitoutOptimizationsDecompiled->GetNormalizedString(),
                          expected,
                          result2  );
                assert(!ut.AssertOnFailure);
                return expression;
            }
        }

    }

    //---------------- print success -------------------
    ut.Print (ci, alib::Verbosity::Info,
              "Expression: {}\n"
              "      Norm: {}\n"
              "       Opt: {}\n"
              "    result: {}"
              "       {!ATab} {:03}/{:03} ET: {:03}/{:03} Len: {:2>}/{:2>} #Opt: {}"  ,
              expressionString,
              expression->GetNormalizedString(),
              expression->GetOptimizedString(),
              result,
              "CT:",
              #if ALIB_TIME
                  compileTime,  recompileTime,
                  evalTime,     recompiledEvalTime,
              #else
                  -1, -1, -1, -1,
              #endif
              optimizedPLen, nonOptimizedLen,
              expression->GetProgram()->CountOptimizations()
            );
    return expression;
}


#if !ALIB_DEBUG
    #define      PRINTPRGRM(expr)
#else
    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );

Expression printProgram( const CallerInfo&  ci,
                         AWorxUnitTesting&  ut,
                         Compiler&          compiler,
                         const String&      expressionString   )
{
    Expression   expression;
#if !ALIB_ALOX
    (void) file;
    (void) line;
    (void) func;
    (void) ut;
    (void) compiler;
    (void) expressionString;
#else

    //---------------------- Compile -------------------
    try
    {
        expression= compiler.Compile( expressionString );
    }
    catch( Exception& e )
    {
        ut.Print (ci, alib::Verbosity::Info,
                  "Cant print program. Exception compiling expression:\n"
                  " Expression:  {{{}}}",
                  expressionString        );
        Lox_SetVerbosity( ut.utl, Verbosity::Verbose, "/" )
        log_exception( ut, e  );
        assert(!ut.AssertOnFailure);
    }
    ALIB_ASSERT( expression )

    ut.Print (ci, alib::Verbosity::Info, "Expression Listing:" );

    AString listing=  expressions::detail::VirtualMachine::DbgList(*expression->GetProgram() );

    auto        oldMultiLineMsgMode     = ut.utl->GetFormatMultiLine().Mode;
    String16    oldFmtMultiLinePrefix   = ut.utl->GetFormatMultiLine().Prefix;
    ut.utl->GetFormatMultiLine().Mode   = 4;
    ut.utl->GetFormatMultiLine().Prefix.Reset("");

    listing << NEW_LINE;
    ut.Print (ci, alib::Verbosity::Info, listing );
    ut.utl->GetFormatMultiLine().Mode  = oldMultiLineMsgMode;
    ut.utl->GetFormatMultiLine().Prefix.Reset(oldFmtMultiLinePrefix);
#endif

    return expression;
}
#endif

#include "unittests/aworx_unittests_end.hpp"

} //namespace

ALIB_WARNINGS_RESTORE

#endif // ALIB_UT_EXPRESSIONS

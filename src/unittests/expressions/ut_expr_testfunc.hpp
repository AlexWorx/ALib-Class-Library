/** ************************************************************************************************
 * \file
 * This header file is part of the unit tests of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_UT_ALIB_EXPR_TESTFUNC
#define HPP_UT_ALIB_EXPR_TESTFUNC
#pragma once
namespace ut_aworx {

// #################################################################################################
// ### Test method used by the test below, with two macros. One for expressions understood by
// ### C++, one for those that are not compatible.
// #################################################################################################
#define   CCOMP_CONSTEXPR(expr)               testExpression  ( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), expr  , 1 )
#define   CONSTEXPR(expr, result)             testExpression  ( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, 1 )
#define   EXPRESSION(expr, result,expProgLen) testExpression  ( ALIB_CALLER, ut, compiler, scope, ALIB_STRINGIFY(expr), result, expProgLen )
#define   EXPR_STR(expr, result,expProgLen)   testExpression  ( ALIB_CALLER, ut, compiler, scope,                expr , result, expProgLen )
#define   EXPRNORM(expr, norm)                testNormalizaton( ALIB_CALLER, ut, compiler,        ALIB_STRINGIFY(expr), ALIB_STRINGIFY(norm) )
#define   EXPRNORMNS(expr, norm)              testNormalizaton( ALIB_CALLER, ut, compiler,        ALIB_STRINGIFY(expr), A_CHAR(norm) )

extern
Expression testExpression(   const CallerInfo&     ci,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             expressions::Scope&   scope,
                             const String&         expressionString,
                             Box                   expected,
                             integer               programLength                               );

extern
void       testNormalizaton( const CallerInfo&     ci,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             const String&         expressionString,
                             const String&         expected           );

extern
void        log_exception ( AWorxUnitTesting& ut, Exception& e );


#if !ALIB_DEBUG
    #define      PRINTPRGRM(expr)
#else
    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );

extern
Expression printProgram(   const CallerInfo&  ci,
                           AWorxUnitTesting&  ut,
                           Compiler&          compiler,
                           const String&      expressionString         );
#endif


}

#endif //  HPP_UT_ALIB_EXPR_TESTFUNC

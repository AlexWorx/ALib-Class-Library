// #################################################################################################
//  aworx - Unit Tests
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_UT_ALIB_EXPR_TESTFUNC
#define HPP_UT_ALIB_EXPR_TESTFUNC

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
SPExpression testExpression( const NCString& file, int line, const NCString& func,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             expressions::Scope&   scope,
                             const String&         expressionString,
                             Box                   expected,
                             integer               programLength                               );

extern
void       testNormalizaton( const NCString& file, int line, const NCString& func,
                             AWorxUnitTesting&     ut,
                             Compiler&             compiler,
                             const String&         expressionString,
                             const String&         expected           );

#if !ALIB_DEBUG
    #define      PRINTPRGRM(expr)
#else
    #define      PRINTPRGRM(expr) printProgram( ALIB_CALLER, ut, compiler, ALIB_STRINGIFY(expr) );

extern
SPExpression printProgram( const NCString& file, int line, const NCString& func,
                           AWorxUnitTesting&  ut,
                           Compiler&          compiler,
                           const String&      expressionString         );
#endif


}

#endif //  HPP_UT_ALIB_EXPR_TESTFUNC

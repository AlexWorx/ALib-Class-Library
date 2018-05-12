// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#include "alib/expressions/expression.hpp"
#include "alib/expressions/compiler.hpp"
#include "alib/expressions/detail/virtualmachine.hpp"
#include "alib/expressions/detail/ast.hpp"
#include "alib/expressions/scope.hpp"
#include "alib/expressions/detail/program.hpp"


using namespace std;

namespace aworx { namespace lib { namespace expressions {


Expression::Expression( const String& sourceString )
: name          ( lib::EXPRESSIONS.Get(ASTR("ANON_EXPR_NAME")) )
, program       ( nullptr )
, originalString( sourceString )
{}

Expression::~Expression()
{
    if(program)     delete program;
}

aworx::Box  Expression::ResultType()
{
    ALIB_ASSERT_ERROR( program, "Internal error: Expression without program" )
        return program->ResultType();
}

aworx::Box  Expression::Evaluate( Scope& scope )
{
    ALIB_ASSERT_ERROR( program, "Internal error: Expression without program" )
    ALIB_DBG( Ticks startTime; );

        Box result= program->Run( scope );

    ALIB_DBG( DbgLastEvaluationTime= startTime.Age(); );

    return result;
}


String     Expression::GetOptimizedString()
{
    if( optimizedString.IsNull() )
        dynamic_cast<detail::Program*>( program )->compiler.getOptimizedExpressionString(this);
    return optimizedString;
}


}}} // namespace [aworx::lib::expressions]


#undef EOS

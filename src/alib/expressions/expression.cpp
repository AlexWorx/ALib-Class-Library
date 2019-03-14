// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSION)
#   include "alib/expressions/expression.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif

namespace aworx { namespace lib { namespace expressions {


Expression::Expression( const String& sourceString )
: name          ( EXPRESSIONS.GetResource("ANON_EXPR_NAME") )
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
    #if ALIB_MODULE_TIME && ALIB_DEBUG
        Ticks startTime;
    #endif

        Box result= program->Run( scope );

    #if ALIB_MODULE_TIME && ALIB_DEBUG
        DbgLastEvaluationTime= startTime.Age();
    #endif

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

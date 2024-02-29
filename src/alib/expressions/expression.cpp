// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSION)
#   include "alib/expressions/expression.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM)
#   include "alib/expressions/detail/program.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_SCOPE)
#   include "alib/expressions/scope.hpp"
#endif
#   if !defined (HPP_ALIB_LANG_CAMP_INLINES)
#      include "alib/lang/basecamp/camp_inlines.hpp"
#   endif
#endif // !defined(ALIB_DOX)


namespace alib {  namespace expressions {


Expression::Expression( const String& sourceString, Scope* pCTScope )
: ctScope       ( pCTScope)
, name          ( nullptr )
, program       ( nullptr )
, originalString( ctScope->Allocator.EmplaceString( sourceString ) )
{
    normalizedString.SetBuffer(sourceString.Length());
}

Expression::~Expression()
{
    if(program)
       delete program;
    delete ctScope;

}

String   Expression::Name()
{
    if( name.IsNull() )
        name= EXPRESSIONS.GetResource("ANON_EXPR_NAME");
    return name;
}

alib::Box  Expression::ResultType()
{
    ALIB_ASSERT_ERROR( program, "EXPR", "Internal error: Expression without program" )
        return program->ResultType();
}

alib::Box  Expression::Evaluate( Scope& scope )
{
    ALIB_ASSERT_ERROR( program, "EXPR","Internal error: Expression without program" )
    #if ALIB_TIME && ALIB_DEBUG
        Ticks startTime;
    #endif

        Box result= program->Run( scope );

    #if ALIB_TIME && ALIB_DEBUG
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


}} // namespace [alib::expressions]


#undef EOS

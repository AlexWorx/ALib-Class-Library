// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#include "alib/alib.hpp"
#include "alib/expressions/expressionslib.hpp"

#include "alib/expressions/compiler.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "parser_impl.hpp"

#if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
#   include "spirit.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace detail {

// static creation method
Parser* Parser::Create( Compiler& compiler )
{
#if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER
    if( EnumContains( compiler.CfgCompilation, Compilation::UseSpiritParser ) )
        return detail::CreateBoostSpiritParser(
                EnumContains( compiler.CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator ),
                EnumContains( compiler.CfgCompilation, Compilation::AllowSubscriptOperator                )
            );
    else
#endif
        return new detail::ParserImpl( compiler );
}


}}}} // namespace [aworx::lib::expressions::detail]






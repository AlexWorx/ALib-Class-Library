// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER)
#   include "alib/expressions/detail/parser.hpp"
#endif

#if !defined (HPP_ALIB_EXPRESSIONS_COMPILER)
#   include "alib/expressions/compiler.hpp"
#endif


#if !defined (HPP_ALIB_EXPRESSIONS_DETAIL_PARSER_IMPL)
#   include "alib/expressions/detail/parser_impl.hpp"
#endif

#if ALIB_FEAT_EXPRESSIONS_SPIRIT_PARSER && !defined (HPP_ALIB_EXPRESSIONS_DETAIL_SPIRIT)
#   include "alib/expressions/detail/spirit.hpp"
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






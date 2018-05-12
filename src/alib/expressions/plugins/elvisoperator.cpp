// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################


#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"

#include "elvisoperator.hpp"
#include "../scope.hpp"

//! @cond NO_DOX


namespace aworx { namespace lib { namespace expressions { namespace plugins {

ElvisOperator::ElvisOperator( Compiler& compiler )
: CompilerPlugin("ALib Elvis", compiler )
{}

namespace {

Box elvis( Scope&, ArgIterator  arg, ArgIterator )
{
    return (*arg).Invoke<IIsTrue, bool>() ? *arg : *(arg + 1);
}

} // anonymous namespace

bool ElvisOperator::TryCompilation( CIBinaryOp&   ciBinaryOp    )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // not Elvis operator  ASTR("A ?: B") ?
    if( ciBinaryOp.Operator != ASTR("?:") || !lhs.IsSameType(rhs) )
        return false;

    ALIB_DBG( ciBinaryOp.DbgCallBackName = "elvis";      )

    // constant A?
    if( ciBinaryOp.LhsIsConst )
    {
        if( lhs.Invoke<IIsTrue,bool>() )
            ciBinaryOp.TypeOrValue= lhs;
        else
        {
            // constant B?
            if( ciBinaryOp.RhsIsConst )
                ciBinaryOp.TypeOrValue= rhs;
            else
                ciBinaryOp.NonConstArgIsResult= true;
        }
        return true;
    }

    // non constant A
    ciBinaryOp.Callback   = elvis;
    ciBinaryOp.TypeOrValue= rhs;

    return true;
}

}}}} // namespace [aworx::lib::expressions::detail]

//! @endcond

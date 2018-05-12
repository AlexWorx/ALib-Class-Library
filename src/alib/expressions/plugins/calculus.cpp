// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "calculus.hpp"
#endif




namespace aworx { namespace lib { namespace expressions {  namespace plugins {

// #################################################################################################
// Identifiers operators
// #################################################################################################
bool Calculus::TryCompilation( CIFunction& ciFunction )
{
    String& name= ciFunction.Name;

    // search in constant identifiers
    if( ciFunction.QtyArgs() == 0 )
    {
        for( auto& entry : ConstantIdentifiers )
        {
            if ( MatchFunctionName( entry.Descriptor, name ) )
            {
                // check for wrong parentheses
                if(     ciFunction.NoParentheses
                    && !EnumContains(Parent.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                     entry.Descriptor.Name );

                if(    !ciFunction.NoParentheses
                    && !EnumContains(Parent.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
                    throw Exception( ALIB_CALLER_NULLED, Exceptions::IdentifierWithFunctionParentheses,
                                     entry.Descriptor.Name );


                // accept
                ciFunction.Name._()._( entry.Descriptor.Name );
                ciFunction.TypeOrValue= entry.Result;
                return true;
            }
        }
    }

    // search in functions
    for( auto& entry : Functions )
    {
        if( MatchFunctionName( entry.Descriptor, name ) )
        {
            // collect information about given and requested parameters
            size_t qtyGiven      =  ciFunction.QtyArgs();
            size_t qtyRequired   =  entry.ArgTypes ?  entry.ArgTypes->size() : 0;
            bool   isVariadic    =  false;
            if( qtyRequired > 0 && entry.ArgTypes->back().IsNull() )
            {
                isVariadic= true;
                qtyRequired--;
            }

            size_t qtyShared     = (std::min)( qtyGiven, qtyRequired );
            bool    sharedAreSameType = true;
            for( size_t i= 0;  i < qtyShared ; ++i )
                sharedAreSameType&=   ciFunction.Arg(i).IsSameType( (*entry.ArgTypes)[i] );

            // check if given parameter don't match
            if(    !sharedAreSameType
                || ( isVariadic ? qtyGiven <  qtyRequired
                                : qtyGiven != qtyRequired )         )
            {
                ciFunction.FunctionsWithNonMatchingArguments.emplace_back( entry.Descriptor.Name );
                if( entry.ArgTypes )
                    Parent.WriteFunctionSignature( entry.ArgTypes->begin(),
                                                   entry.ArgTypes->end  (),
                                                   ciFunction.FunctionsWithNonMatchingArguments.back() );
                // search next
                continue;
            }

            // check for wrong parentheses
            if(     ciFunction.NoParentheses
                &&  entry.ArgTypes != nullptr
                && !EnumContains(Parent.CfgCompilation, Compilation::AllowOmittingParenthesesOfParameterlessFunctions ) )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::MissingFunctionParentheses,
                                 entry.Descriptor.Name );

            if(    !ciFunction.NoParentheses
                &&  entry.ArgTypes == nullptr
                && !EnumContains(Parent.CfgCompilation, Compilation::AllowEmptyParenthesesForIdentifierFunctions ) )
                throw Exception( ALIB_CALLER_NULLED, Exceptions::IdentifierWithFunctionParentheses,
                                 entry.Descriptor.Name );



            // accept
            ciFunction.Name._()._( entry.Descriptor.Name );

            if( !entry.Callback )
            {
                ciFunction.TypeOrValue      = entry.ResultType;
    ALIB_DBG(   ciFunction.DbgCallBackName  = entry.DbgCallbackName; )
                return true;
            }

            // for constants, the callback might b invoked right away (optimizing cal out)
            if( ciFunction.AllArgsAreConst && entry.IsCTInvokable )
            {
                // calculate constant value
                ciFunction.TypeOrValue      = entry.Callback( ciFunction.CompileTimeScope,
                                                              ciFunction.ArgsBegin,
                                                              ciFunction.ArgsEnd           );
                ALIB_ASSERT_ERROR(ciFunction.TypeOrValue.IsSameType(entry.ResultType),
                                  "Type mismatch in definition of function {!Q} in plugin {!Q}.\\n"
                                  "                    Type specified: {}\\n"
                                  "Type returned by callback function: {}",
                                  entry.Descriptor.Name, CompilerPlugin::Name,
                                  CompilerPlugin::Parent.TypeName(entry.ResultType),
                                  CompilerPlugin::Parent.TypeName(ciFunction.TypeOrValue) );
    ALIB_DBG(   ciFunction.DbgCallBackName  = entry.DbgCallbackName;  )
                return true;
            }

            ciFunction.Callback             = entry.Callback;
            ciFunction.TypeOrValue          = entry.ResultType;
 ALIB_DBG(  ciFunction.DbgCallBackName      = entry.DbgCallbackName; )
            return true;
        }
    }

    return false;
}



// #################################################################################################
// Unary operators
// #################################################################################################
void Calculus::AddUnaryOp  ( const String&      op,
                             Type               argType,
                             CallbackDecl       callback,
                             #if ALIB_DEBUG
                                const char*     dbgCallbackName,
                             #endif
                             Type               resultType,
                             CTInvokable        cti              )
{
    auto& map=  UnaryOpMap[op];
    ALIB_ASSERT_ERROR( map.find( argType.GetTypeInfo<0>() ) == map.end(),
                       "Unary operator '{}' already defined for type <{}>.",
                       op, Parent.TypeName( argType )    );

    map[argType.GetTypeInfo<0>()]=  std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) );
}

void Calculus::AddUnaryOps( UnaryOpTableEntry* table, size_t length )
{

    #define OP         std::get<0>( *(table + i) )
    #define TYPE       std::get<1>( *(table + i) ).GetTypeInfo<0>()
    #define CBFUNC     std::get<2>( *(table + i) )
    #define RESULTTYPE std::get<ALIB_REL_DBG(3,4)>( *(table + i) )
    #define CTINVOKE   std::get<ALIB_REL_DBG(4,5)>( *(table + i) )
    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map=  UnaryOpMap[OP];
        ALIB_ASSERT_ERROR( map.find( TYPE ) == map.end(),
                           "Unary operator '{}' already defined for type <{}>.",
                           OP, Parent.TypeName( std::get<1>( *(table + i) ) )    );

        map[TYPE] =  std::make_tuple( CBFUNC,
                                      RESULTTYPE,
                                      CTINVOKE
                           ALIB_DBG(, std::get<3>(*(table + i)))            );

    }
    #undef OP
    #undef TYPE
    #undef CBFUNC
    #undef RESULTTYPE
    #undef CTINVOKE
}

void Calculus::AddUnaryOpAlias  ( const String& alias, Type argType, const String& op )
{
    auto& map=  UnaryOpAliases[alias];
    ALIB_ASSERT_ERROR( map.find( argType.GetTypeInfo<0>() ) == map.end(),
                       "Unary operator alias '{}' already defined for type <{}>.",
                       alias, Parent.TypeName( argType)    );
    map[argType.GetTypeInfo<0>()]= op;
}

void Calculus::AddUnaryOpAliases( UnaryOpAliasTableEntry* table, size_t length )
{
    if( length == 0 )
        return;

    #define ALIAS       std::get<0>( *(table + i) )
    #define ARG_TYPE    std::get<1>( *(table + i) ).GetTypeInfo<0>()
    #define OP          std::get<2>( *(table + i) )

    size_t i= 0;
    do
    {
        const String& alias= ALIAS;
        auto& map=  UnaryOpAliases[alias];
        do
        {
            ALIB_ASSERT_ERROR( map.find( ARG_TYPE ) == map.end(),
                               "Unary operator alias '{}' already defined for type <{}>.",
                               alias, Parent.TypeName( *(table + i) )    );
            map[ARG_TYPE]= OP ;
        }
        while( ++i < length && ALIAS == alias );
    }
    while( i < length );

    #undef OP
    #undef ARG_TYPE
    #undef ALIAS

}

bool Calculus::TryCompilation( CIUnaryOp&  ciUnaryOp )
{
    Box& arg= ciUnaryOp.CompileTimeScope.Stack[0];

    // search alias first
    {
        auto it1= UnaryOpAliases.find( ciUnaryOp.Operator );
        if( it1 != UnaryOpAliases.end() )
        {
            auto it2= it1->second.find( arg.GetTypeInfo<0>() );
            if( it2 != it1->second.end() )
            {
                ciUnaryOp.Operator= it2->second;
            }
        }
    }

    // search callback
    auto it1= UnaryOpMap.find( ciUnaryOp.Operator );
    if( it1 != UnaryOpMap.end() )
    {
        auto it2= it1->second.find( arg.GetTypeInfo<0>() );
        if( it2 != it1->second.end() )
        {
            // for constants, the callback might b invoked right away (optimizing cal out)
            if( ciUnaryOp.ArgIsConst && std::get<2>(it2->second) )
            {
                // calculate constant value
                ciUnaryOp.TypeOrValue=  std::get<0>(it2->second)( ciUnaryOp.CompileTimeScope,
                                                                  ciUnaryOp.ArgsBegin,
                                                                  ciUnaryOp.ArgsEnd           );
       ALIB_DBG(ciUnaryOp.DbgCallBackName= std::get<3>(it2->second);)
                ALIB_ASSERT_ERROR(ciUnaryOp.TypeOrValue.IsSameType(std::get<1>(it2->second)),
                                  "Type mismatch in definition of unary operator {!Q} in plugin {!Q}.\\n"
                                  "                    Type specified: {}\\n"
                                  "Type returned by callback function: {}",
                                  ciUnaryOp.Operator, CompilerPlugin::Name,
                                  CompilerPlugin::Parent.TypeName(std::get<1>(it2->second)),
                                  CompilerPlugin::Parent.TypeName(ciUnaryOp.TypeOrValue) );
                return true;
            }
            ciUnaryOp.Callback       = std::get<0>(it2->second);
            ciUnaryOp.TypeOrValue    = std::get<1>(it2->second);
   ALIB_DBG(ciUnaryOp.DbgCallBackName= std::get<3>(it2->second);)
            return true;
        }
    }

    // not found
    return false;
}

// #################################################################################################
// Binary operators
// #################################################################################################
void Calculus::AddBinaryOp     ( const String&      op,
                                 Type               lhsType,
                                 Type               rhsType,
                                 CallbackDecl       callback,
                                 #if ALIB_DEBUG
                                    const char*     dbgCallbackName,
                                 #endif
                                 Type               resultType,
                                 CTInvokable        cti                )
{
    auto& map= BinaryOpMap[op][lhsType.GetTypeInfo<0>()];
    ALIB_ASSERT_ERROR( map.find( rhsType.GetTypeInfo<0>() ) == map.end(),
                       "Binary operator '{}' already defined for types <{}> and <{}>.",
                       op, Parent.TypeName( lhsType ),
                           Parent.TypeName( rhsType )   );
    map[rhsType.GetTypeInfo<0>()]= std::make_tuple( callback, resultType, cti  ALIB_DBG(, dbgCallbackName) );

}

void Calculus::AddBinaryOps( BinaryOpTableEntry* table, size_t length )
{
    #define OP         std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).GetTypeInfo<0>()
    #define RHS_TYPE   std::get<2>( *(table + i) ).GetTypeInfo<0>()
    #define CBFUNC     std::get<3>( *(table + i) )
    #define RESULTTYPE std::get<ALIB_REL_DBG(4,5)>( *(table + i) )
    #define CTINVOKE   std::get<ALIB_REL_DBG(5,6)>( *(table + i) )

    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map= BinaryOpMap[OP][LHS_TYPE];
        ALIB_ASSERT_ERROR( map.find( RHS_TYPE ) == map.end(),
                           "Binary operator '{}' already defined for types <{}> and <{}>.",
                           OP, Parent.TypeName( std::get<1>( *(table+i) ) ),
                               Parent.TypeName( std::get<2>( *(table+i) ) )   );
        map[RHS_TYPE]= std::make_tuple( CBFUNC,
                                        RESULTTYPE,
                                        CTINVOKE
                             ALIB_DBG(, std::get<4>( *(table + i) )) );
    }

    #undef OP
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef CBFUNC
    #undef RESULTTYPE
    #undef CTINVOKE
}

void Calculus::AddBinaryOpAlias( const String& alias, Type lhs, Type rhs, const String& op )
{
    auto& map= BinaryOpAliases[alias][lhs.GetTypeInfo<0>()];
    ALIB_ASSERT_ERROR( map.find( rhs.GetTypeInfo<0>() ) == map.end(),
                       "Binary operator alias '{}' already defined for types <{}> and  <{}>.",
                       alias, Parent.TypeName( lhs ),
                              Parent.TypeName( rhs )                                              );
    map[rhs.GetTypeInfo<0>()]= op;
}

void Calculus::AddBinaryOpAliases( BinaryOpAliasTableEntry* table, size_t length )
{
    if( length == 0 )
        return;

    #define ALIAS      std::get<0>( *(table + i) )
    #define LHS_TYPE   std::get<1>( *(table + i) ).GetTypeInfo<0>()
    #define RHS_TYPE   std::get<2>( *(table + i) ).GetTypeInfo<0>()
    #define OP         std::get<3>( *(table + i) )

    size_t i= 0;
    do
    {
        const String& alias= ALIAS;
        auto& map1=  BinaryOpAliases[alias];
        do
        {
            auto& lhsType= LHS_TYPE;
            auto& map2=  map1[lhsType];
            do
            {
                ALIB_ASSERT_ERROR( map2.find( RHS_TYPE ) == map2.end(),
                                   "Binary operator alias '{}' already defined for types <{}> and  <{}>.",
                                   alias,   Parent.TypeName( std::get<1>( *(table + i) ) ),
                                            Parent.TypeName( std::get<2>( *(table + i) ) )                     );
                map2[RHS_TYPE]= OP;
            }
            while( ++i < length && LHS_TYPE == lhsType );
        }
        while( i < length && ALIAS == alias );
    }
    while( i < length );

    #undef ALIAS
    #undef LHS_TYPE
    #undef RHS_TYPE
    #undef OP
}

void Calculus::AddBinaryOpOptimizations( BinaryOpOptimizationsTableEntry* table, size_t length,
                                         bool lhsOrRhs )
{
    #define OP         std::get<0>( *(table + i) )
    #define TYPE       std::get<1>( *(table + i) ).GetTypeInfo<0>()
    #define CONSTVAL   std::get<2>( *(table + i) )
    #define RESULT     std::get<3>( *(table + i) )

    auto& map= !lhsOrRhs ? BinaryOpConstLHSOptimizations
                         : BinaryOpConstRHSOptimizations;

    for( size_t i= 0 ; i < length ; ++i )
    {
        auto& map2= map[OP][TYPE];
        ALIB_ASSERT_ERROR( map2.find( CONSTVAL ) == map2.end(),
                           "Optimization already defined for operator {}, {}-hand side type <{}> and {}-hand constant {}.",
                           OP,
                           ( lhsOrRhs ? "left" : "right"),
                           TYPE,
                           ( lhsOrRhs ? "right" : "left"),
                           CONSTVAL );
        map2[CONSTVAL]= RESULT;
    }

    #undef OP
    #undef TYPE
    #undef CONSTVAL
    #undef RESULT
}


bool Calculus::TryCompilation( CIBinaryOp& ciBinaryOp )
{
    Box& lhs= * ciBinaryOp.ArgsBegin;
    Box& rhs= *(ciBinaryOp.ArgsBegin + 1);

    // search alias first
    if(    ciBinaryOp.Operator == ASTR("=")
        && EnumContains( Parent.CfgCompilation, Compilation::AliasEqualsOperatorWithAssignOperator ) )
    {
        ciBinaryOp.Operator= ASTR("==");
    }
    else
    {
        auto it1= BinaryOpAliases.find( ciBinaryOp.Operator );
        if( it1 != BinaryOpAliases.end() )
        {
            auto it2= it1->second.find( lhs.GetTypeInfo<0>() );
            if( it2 != it1->second.end() )
            {
                auto it3= it2->second.find( rhs.GetTypeInfo<0>() );
                if( it3 != it2->second.end() )
                {
                    ciBinaryOp.Operator= it3->second;
                }
            }
        }
    }

    #define  CBFUNC        std::get<0>(it3->second)
    #define  RESULTTYPE    std::get<1>(it3->second)
    #define  CT_INVOKABLE  std::get<2>(it3->second)
    #if ALIB_DEBUG
    #define  DBG_CB_NAME   std::get<3>(it3->second)
    #endif
    // search callback
    auto it1= BinaryOpMap.find( ciBinaryOp.Operator );
    if( it1 != BinaryOpMap.end() )
    {
        auto it2= it1->second.find( lhs.GetTypeInfo<0>() );
        if( it2 != it1->second.end() )
        {
            auto it3= it2->second.find( rhs.GetTypeInfo<0>() );
            if( it3 != it2->second.end() )
            {
                // if both are constant, the callback might b invoked right away (optimizing the call out)
                if( ciBinaryOp.LhsIsConst && ciBinaryOp.RhsIsConst )
                {
                    if( CT_INVOKABLE )
                    {
                        // calculate constant value
                        ciBinaryOp.TypeOrValue=  CBFUNC  ( ciBinaryOp.CompileTimeScope,
                                                           ciBinaryOp.ArgsBegin,
                                                           ciBinaryOp.ArgsEnd           );
           ALIB_DBG(    ciBinaryOp.DbgCallBackName= DBG_CB_NAME;)
                        ALIB_ASSERT_ERROR(ciBinaryOp.TypeOrValue.IsSameType(RESULTTYPE),
                                          "Type mismatch in definition of binary operator {!Q} in plugin {!Q}.\\n"
                                          "                    Type specified: {}\\n"
                                          "Type returned by callback function: {}",
                                          ciBinaryOp.Operator, CompilerPlugin::Name,
                                          CompilerPlugin::Parent.TypeName(RESULTTYPE),
                                          CompilerPlugin::Parent.TypeName(ciBinaryOp.TypeOrValue) );
                        return true;
                    }
                }


                // if one is constant, we may find an entry in BinaryOpConsL/RHSOptimizations
                else if( ciBinaryOp.LhsIsConst || ciBinaryOp.RhsIsConst )
                {
                    auto& map=           ciBinaryOp.LhsIsConst ?  BinaryOpConstLHSOptimizations
                                                               :  BinaryOpConstRHSOptimizations;
                    auto& nonConstType= (ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin + 1 )
                                                               :  *(ciBinaryOp.ArgsBegin     ) ).GetTypeInfo();
                    auto& constValue=    ciBinaryOp.LhsIsConst ?  *(ciBinaryOp.ArgsBegin     )
                                                               :  *(ciBinaryOp.ArgsBegin + 1 );
                    auto& map2= map[ciBinaryOp.Operator][nonConstType];
                    auto entryIt= map2.find( constValue );
                    if( entryIt  != map2.end() )
                    {
                        // found! If it is a nulled box, this tells us, that the result is the other side
                        // (identity operation). Otherwise it is a constant.
                        if( entryIt->second.IsNull() )
                            ciBinaryOp.NonConstArgIsResult= true;
                        else
                            ciBinaryOp.TypeOrValue        = entryIt->second;
                        return true;
                    }

                }


                ciBinaryOp.Callback       = CBFUNC;
                ciBinaryOp.TypeOrValue    = RESULTTYPE;
       ALIB_DBG(ciBinaryOp.DbgCallBackName= DBG_CB_NAME; )
                return true;
            }
        }
    }

    return false;

    #undef CBFUNC
    #undef RESULTTYPE
    #undef CT_INVOKABLE
    #if ALIB_DEBUG
    #undef DBG_CB_NAME
    #endif
}


}}}} // namespace [aworx::lib::expressions::plugin]




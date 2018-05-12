// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################


#include "alib/alib.hpp"
#include "alib/strings/substring.hpp"

#include "autocast.hpp"
#include "scopestring.hpp"

// although this is not derived from calculus, we use its public type mechanics.
#include "../plugins/calculus.hpp"

// we are using method ToString
#include "../plugins/strings.hpp"

// ...and method ToBoolean
#include "../plugins/arithmetics.hpp"

//! @cond NO_DOX

#define ARG0           (*args)
#define BOL(box)       (box).Unbox<bool     >()
#define INT(box)       (box).Unbox<boxed_int>()
//#define FLT(box)       (box).Unbox<double   >()
#define FUNC( name,...) Box name( Scope& scope,                                                  \
                                  ArgIterator  args,                                            \
                                  ArgIterator  end    )                                         \
                                  { (void) scope; (void) args; (void) end; __VA_ARGS__ }


namespace aworx { namespace lib { namespace expressions { namespace plugins {

AutoCast::AutoCast( Compiler& compiler )
: CompilerPlugin( "ALib Auto Cast", compiler )
{}

namespace {

enum class SortedTypes
{
    UNKNOWN     =   0,
    Bool        =   1,
    Integer     =   2,
    Float       =   4,
    String      =   8,
};

SortedTypes   SortedType( Type type )
{
    if( type.IsType<bool     >() )      return SortedTypes::Bool;
    if( type.IsType<boxed_int>() )      return SortedTypes::Integer;
    if( type.IsType<double   >() )      return SortedTypes::Float;
    if( type.IsType<String   >() )      return SortedTypes::String;
    return SortedTypes::UNKNOWN;
}

// conversion integer to float.
FUNC( castI2F     , return static_cast<double>( INT(ARG0) );  )
FUNC( castB2F     , return static_cast<double>( BOL(ARG0) );  )
FUNC( castB2I     , return static_cast<int   >( BOL(ARG0));   )

} // anonymous namespace


// #################################################################################################
// ### AutoCast - TryCompilation
// #################################################################################################
bool AutoCast::TryCompilation( CIBinaryAutoCast& ciAutoCast )
{
    // convert the minor type to the major
    SortedTypes t1= SortedType( * ciAutoCast.ArgsBegin    );
    SortedTypes t2= SortedType( *(ciAutoCast.ArgsBegin+1) );
    if( t1 == t2 )
        return false;
    SortedTypes major;
    SortedTypes minor;
    if(  t1 > t2 )    {  major=  t1;  minor= t2;   }
    else              {  major=  t2;  minor= t1;   }

    // choose upgrade function
    CallbackDecl upgradeCast;
    String decompileFunctionCall;
    ALIB_DBG( const char*  upgradeCastDBG; )

    // we can cast every type to string
    if( major == SortedTypes::String )
    {
        upgradeCast             =  CBToString;
        decompileFunctionCall   =     ASTR("String");
ALIB_DBG(upgradeCastDBG         = "CBToString"; )
    }

    // integer to float?
    else if( major == SortedTypes::Float && minor == SortedTypes::Integer )
    {
        upgradeCast             =  castI2F;
        decompileFunctionCall   =  ASTR("Float");
ALIB_DBG(upgradeCastDBG         = "castI2F"; )

    }

    // bool to float?
    else if( major == SortedTypes::Float && minor == SortedTypes::Bool    )
    {
        upgradeCast             =  castB2F;
        decompileFunctionCall   =  ASTR("Float");
ALIB_DBG(upgradeCastDBG         = "castB2F"; )

    }

    // we can cast every type to integer via boolean
    else if( major == SortedTypes::Integer && minor == SortedTypes::Bool  )
    {
        upgradeCast             =  castB2I;
        decompileFunctionCall   =   ASTR("Integer");
ALIB_DBG(upgradeCastDBG         = "castB2I"; )
    }

    // we can cast every type to boolean as well
    else if( major == SortedTypes::Bool )
    {
        upgradeCast             =  ToBoolean;
        decompileFunctionCall   =   ASTR("Boolean");
ALIB_DBG(upgradeCastDBG         = "ToBoolean"; )
    }

    else
        return false;

    // set upgrade function to the right callback
    if(  t1 < t2 )
    {
        ciAutoCast.Callback                       = upgradeCast;
        ciAutoCast.TypeOrValue                    = *(ciAutoCast.ArgsBegin + 1);
        ciAutoCast.CastExpressionFunctionNameLhs  = decompileFunctionCall;
ALIB_DBG(ciAutoCast.DbgCallBackName= upgradeCastDBG; )
    }
    else
    {
        ciAutoCast.CallbackRhs                    = upgradeCast;
        ciAutoCast.TypeOrValueRhs                 = *(ciAutoCast.ArgsBegin    );
        ciAutoCast.CastExpressionFunctionNameRhs  = decompileFunctionCall;
ALIB_DBG(ciAutoCast.DbgCallBackNameRhs= upgradeCastDBG; )
    }

    // If constant values were given, we can we do it right away (compile time optimization)
         if( ciAutoCast.LhsIsConst && ciAutoCast.Callback != nullptr )
    {
        ciAutoCast.TypeOrValue   = ciAutoCast.Callback( ciAutoCast.CompileTimeScope,
                                                        ciAutoCast.ArgsBegin       ,
                                                        ciAutoCast.ArgsEnd    - 1  );
        ciAutoCast.Callback      = nullptr;
    }
    else if( ciAutoCast.RhsIsConst && ciAutoCast.CallbackRhs != nullptr )
    {
        ciAutoCast.TypeOrValueRhs= ciAutoCast.CallbackRhs( ciAutoCast.CompileTimeScope,
                                                           ciAutoCast.ArgsBegin + 1   ,
                                                           ciAutoCast.ArgsEnd         );
        ciAutoCast.CallbackRhs   = nullptr;
    }


    return true;
}



}}}} // namespace [aworx::lib::expressions::detail]

//! @endcond

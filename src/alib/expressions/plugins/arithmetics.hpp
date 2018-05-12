// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "../plugins/calculus.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace plugins {

/** ************************************************************************************************
 * This built-in \alib{expressions,CompilerPlugin} of \alibmod_nolink_expressions
 * primarily compiles unary and binary operators for permutations of types
 * \alib{expressions::Types,Boolean}, \alib{expressions::Types,Integer} and
 * \alib{expressions::Types,Float}.
 *
 * By default, this plug-in is \alib{expressions,Compiler::CfgBuiltInPlugins,automatically created}
 * and inserted into each instance of class \alib{expressions,Compiler} with the invocation of
 * \alib{expressions::Compiler,SetupDefaults}.
 *
 * <b>General Notes:</b><br>
 * - All identifier and function names are defined case insensitive.
 *
 * - All callback functions are defined compile-time invokable.
 *   This means that redundancies in expressions emerging from operations on constant numbers are
 *   optimized (pruned) by the compiler. For example, the two expressions:
 *          size > 81920
 *          size > 8 * 1024
 *
 *   are resulting in an identical expression program and thus have identical evaluation performance.
 *
 * - The following additional binary operator optimization are defined:
 *   - Addition and subtraction of \c 0.
 *   - Multiplication by \c 0 and \c 1.
 *   - Division by \c 1.
 *   - Modulo by \c 1.
 *   - Boolean or with \c true and \c false.
 *   - Boolean and with \c true and \c false.
 *
 * - Dependent on configuration flags of the given \b %Compiler, the following alias operators
 *   are defined:
 *   - With flag \alib{expressions::Compilation,AliasEqualsOperatorWithAssignOperator}:<br>
 *     Operator <c>'=='</c> is aliased with <c>'='</c>.
 *   - With flag \alib{expressions::Compilation,AllowBitwiseBooleanOperations}:<br>
 *     Operators <c>'&&'</c>) and <c>'||'</c> are aliased with operators <c>'&'</c> and <c>'|'</c>.
 *
 *
 * <b>Constant Identifiers:</b><br>
 * The following constant identifiers provide variants for boolean values. No abbreviation
 * is allowed.
 *
 * <br>
 * Return Type | Name      | Description
 * ------------|-----------|-------------
 * Boolean     |\b True    | Returns constant \c true.
 * Boolean     |\b False   | Returns constant \c false.
 * Boolean     |\b Yes     | Returns constant \c true.
 * Boolean     |\b No      | Returns constant \c false.
 * Boolean     |\b On      | Returns constant \c true.
 * Boolean     |\b Off     | Returns constant \c false.
 *
 * <br>
 * <b>Type Conversion Functions:</b><br>
 *
 * | Return Type | Name      |Min. Abbreviation| Signature| Description
 * |-------------|-----------|-----------------|--------- |------------
 * | Boolean     |\b Boolean |bool             | \<any\>  | Converts any type of boxed value by invoking boxing interface \alib{boxing,IIsTrue}.
 * | Integer     |\b Integer |int              | Boolean  | Converts \c true to \c 1, \c false to \c 0.
 * | Integer     |\b Integer |int              | Integer  | Does nothing (identity function).
 * | Integer     |\b Integer |int              | Float    | Returns the integral part of a floating point number.
 * | Float       |\b Float   |float            | Boolean  | Converts \c true to \c 1.0, \c false to \c 0.0.
 * | Float       |\b Float   |float            | Integer  | Converts an integral value to floating point.
 * | Float       |\b Float   |float            | Float    | Does nothing (identity function).
 *
 * <br>
 * <b>Functions:</b><br>
 *
 * | Return Type | Name      |Min. Abbreviation| Signature| Description
 * |-------------|-----------|-----------------|--------- |------------
 * | Integer     |\b Length  |len              | \<any array\>  | Returns the length of an array. \note Because built-in type \b %String is a boxed character array, this function can be used to determine the length of strings.
 *
 * <br>
 * <b>Unary Operators:</b><br>
 *
 * | Return Type    | Operator | Argument Type | Description|
 * |--------------|----------|----------|---------------------
 * | Integer      |<c>'+'</c> |Boolean | Converts the boolean value to integer.
 * | Integer      |<c>'+'</c> |Integer | Does nothing (identity operator).
 * | Float        |<c>'+'</c> |Float   | Does nothing (identity operator).
 * | Integer      |<c>'-'</c> |Boolean | Converts the boolean value to integer and negates it.
 * | Integer      |<c>'-'</c> |Integer | Negates an integral value.
 * | Float        |<c>'-'</c> |Float   | Negates a floating point value.
 * | Boolean      |<c>'!'</c> |Boolean | Boolean not operator.
 * | Boolean      |<c>'!'</c> |Integer | Returns the result of the comparison with <c>0</c>.
 * | Boolean      |<c>'!'</c> |Float   | Returns the result of the comparison with <c>0.0</c>.
 * | Integer      |<c>'~'</c> |Integer | Bitwise integral negation.
 *
 * <br>
 * <b>Binary Operators:</b><br>
 *
 * | Return Type | Lhs Type | Operator | Rhs Type | Description|
 * |-------------|----------|----------|----------|------------|
 * |Integer      | Boolean  | <c>'*'</c>    | Boolean | Operator <c>"*"</c>.
 * |Integer      | Boolean  | <c>'*'</c>    | Integer | Operator <c>"*"</c>.
 * |Float        | Boolean  | <c>'*'</c>    | Float   | Operator <c>"*"</c>.
 * |Integer      | Integer  | <c>'*'</c>    | Boolean | Operator <c>"*"</c>.
 * |Integer      | Integer  | <c>'*'</c>    | Integer | Operator <c>"*"</c>.
 * |Float        | Integer  | <c>'*'</c>    | Float   | Operator <c>"*"</c>.
 * |Float        | Float    | <c>'*'</c>    | Boolean | Operator <c>"*"</c>.
 * |Float        | Float    | <c>'*'</c>    | Integer | Operator <c>"*"</c>.
 * |Float        | Float    | <c>'*'</c>    | Float   | Operator <c>"*"</c>.
 * |Integer      | Boolean  | <c>'/'</c>    | Boolean | Operator <c>"/"</c>.
 * |Integer      | Boolean  | <c>'/'</c>    | Integer | Operator <c>"/"</c>.
 * |Float        | Boolean  | <c>'/'</c>    | Float   | Operator <c>"/"</c>.
 * |Integer      | Integer  | <c>'/'</c>    | Boolean | Operator <c>"/"</c>.
 * |Integer      | Integer  | <c>'/'</c>    | Integer | Operator <c>"/"</c>.
 * |Float        | Integer  | <c>'/'</c>    | Float   | Operator <c>"/"</c>.
 * |Float        | Float    | <c>'/'</c>    | Boolean | Operator <c>"/"</c>.
 * |Float        | Float    | <c>'/'</c>    | Integer | Operator <c>"/"</c>.
 * |Float        | Float    | <c>'/'</c>    | Float   | Operator <c>"/"</c>.
 * |Integer      | Boolean  | <c>'\%'</c>   | Boolean | Operator <c>"%"</c>.
 * |Integer      | Boolean  | <c>'\%'</c>   | Integer | Operator <c>"%"</c>.
 * |Float        | Boolean  | <c>'\%'</c>   | Float   | Operator <c>"%"</c>.
 * |Integer      | Integer  | <c>'\%'</c>   | Boolean | Operator <c>"%"</c>.
 * |Integer      | Integer  | <c>'\%'</c>   | Integer | Operator <c>"%"</c>.
 * |Float        | Integer  | <c>'\%'</c>   | Float   | Operator <c>"%"</c>.
 * |Float        | Float    | <c>'\%'</c>   | Boolean | Operator <c>"%"</c>.
 * |Float        | Float    | <c>'\%'</c>   | Integer | Operator <c>"%"</c>.
 * |Float        | Float    | <c>'\%'</c>   | Float   | Operator <c>"%"</c>.
 * |Integer      | Boolean  | <c>'+'</c>    | Boolean | Operator <c>"+"</c>.
 * |Integer      | Boolean  | <c>'+'</c>    | Integer | Operator <c>"+"</c>.
 * |Float        | Boolean  | <c>'+'</c>    | Float   | Operator <c>"+"</c>.
 * |Integer      | Integer  | <c>'+'</c>    | Boolean | Operator <c>"+"</c>.
 * |Integer      | Integer  | <c>'+'</c>    | Integer | Operator <c>"+"</c>.
 * |Float        | Integer  | <c>'+'</c>    | Float   | Operator <c>"+"</c>.
 * |Float        | Float    | <c>'+'</c>    | Boolean | Operator <c>"+"</c>.
 * |Float        | Float    | <c>'+'</c>    | Integer | Operator <c>"+"</c>.
 * |Float        | Float    | <c>'+'</c>    | Float   | Operator <c>"+"</c>.
 * |Integer      | Boolean  | <c>'-'</c>    | Boolean | Operator <c>"-"</c>.
 * |Integer      | Boolean  | <c>'-'</c>    | Integer | Operator <c>"-"</c>.
 * |Float        | Boolean  | <c>'-'</c>    | Float   | Operator <c>"-"</c>.
 * |Integer      | Integer  | <c>'-'</c>    | Boolean | Operator <c>"-"</c>.
 * |Integer      | Integer  | <c>'-'</c>    | Integer | Operator <c>"-"</c>.
 * |Float        | Integer  | <c>'-'</c>    | Float   | Operator <c>"-"</c>.
 * |Float        | Float    | <c>'-'</c>    | Boolean | Operator <c>"-"</c>.
 * |Float        | Float    | <c>'-'</c>    | Integer | Operator <c>"-"</c>.
 * |Float        | Float    | <c>'-'</c>    | Float   | Operator <c>"-"</c>.
 * |Integer      | Boolean  | <c>'<<'</c>   | Integer | Operator <c>"<<"</c>.
 * |Integer      | Integer  | <c>'<<'</c>   | Boolean | Operator <c>"<<"</c>.
 * |Integer      | Integer  | <c>'<<'</c>   | Integer | Operator <c>"<<"</c>.
 * |Integer      | Boolean  | <c>'>>'</c>   | Integer | Operator <c>">>"</c>.
 * |Integer      | Integer  | <c>'>>'</c>   | Boolean | Operator <c>">>"</c>.
 * |Integer      | Integer  | <c>'>>'</c>   | Integer | Operator <c>">>"</c>.
 * |Boolean      | Boolean  | <c>'<'</c>    | Boolean | Operator <c>"<"</c>.
 * |Boolean      | Boolean  | <c>'<'</c>    | Integer | Operator <c>"<"</c>.
 * |Boolean      | Boolean  | <c>'<'</c>    | Float   | Operator <c>"<"</c>.
 * |Boolean      | Integer  | <c>'<'</c>    | Boolean | Operator <c>"<"</c>.
 * |Boolean      | Integer  | <c>'<'</c>    | Integer | Operator <c>"<"</c>.
 * |Boolean      | Integer  | <c>'<'</c>    | Float   | Operator <c>"<"</c>.
 * |Boolean      | Float    | <c>'<'</c>    | Boolean | Operator <c>"<"</c>.
 * |Boolean      | Float    | <c>'<'</c>    | Integer | Operator <c>"<"</c>.
 * |Boolean      | Float    | <c>'<'</c>    | Float   | Operator <c>"<"</c>.
 * |Boolean      | Boolean  | <c>'<='</c>   | Boolean | Operator <c>"<="</c>.
 * |Boolean      | Boolean  | <c>'<='</c>   | Integer | Operator <c>"<="</c>.
 * |Boolean      | Boolean  | <c>'<='</c>   | Float   | Operator <c>"<="</c>.
 * |Boolean      | Integer  | <c>'<='</c>   | Boolean | Operator <c>"<="</c>.
 * |Boolean      | Integer  | <c>'<='</c>   | Integer | Operator <c>"<="</c>.
 * |Boolean      | Integer  | <c>'<='</c>   | Float   | Operator <c>"<="</c>.
 * |Boolean      | Float    | <c>'<='</c>   | Boolean | Operator <c>"<="</c>.
 * |Boolean      | Float    | <c>'<='</c>   | Integer | Operator <c>"<="</c>.
 * |Boolean      | Float    | <c>'<='</c>   | Float   | Operator <c>"<="</c>.
 * |Boolean      | Boolean  | <c>'>'</c>    | Boolean | Operator <c>">"</c>.
 * |Boolean      | Boolean  | <c>'>'</c>    | Integer | Operator <c>">"</c>.
 * |Boolean      | Boolean  | <c>'>'</c>    | Float   | Operator <c>">"</c>.
 * |Boolean      | Integer  | <c>'>'</c>    | Boolean | Operator <c>">"</c>.
 * |Boolean      | Integer  | <c>'>'</c>    | Integer | Operator <c>">"</c>.
 * |Boolean      | Integer  | <c>'>'</c>    | Float   | Operator <c>">"</c>.
 * |Boolean      | Float    | <c>'>'</c>    | Boolean | Operator <c>">"</c>.
 * |Boolean      | Float    | <c>'>'</c>    | Integer | Operator <c>">"</c>.
 * |Boolean      | Float    | <c>'>'</c>    | Float   | Operator <c>">"</c>.
 * |Boolean      | Boolean  | <c>'>='</c>   | Boolean | Operator <c>">="</c>.
 * |Boolean      | Boolean  | <c>'>='</c>   | Integer | Operator <c>">="</c>.
 * |Boolean      | Boolean  | <c>'>='</c>   | Float   | Operator <c>">="</c>.
 * |Boolean      | Integer  | <c>'>='</c>   | Boolean | Operator <c>">="</c>.
 * |Boolean      | Integer  | <c>'>='</c>   | Integer | Operator <c>">="</c>.
 * |Boolean      | Integer  | <c>'>='</c>   | Float   | Operator <c>">="</c>.
 * |Boolean      | Float    | <c>'>='</c>   | Boolean | Operator <c>">="</c>.
 * |Boolean      | Float    | <c>'>='</c>   | Integer | Operator <c>">="</c>.
 * |Boolean      | Float    | <c>'>='</c>   | Float   | Operator <c>">="</c>.
 * |Boolean      | Boolean  | <c>'=='</c>   | Boolean | Operator <c>"=="</c>.
 * |Boolean      | Boolean  | <c>'=='</c>   | Integer | Operator <c>"=="</c>.
 * |Boolean      | Boolean  | <c>'=='</c>   | Float   | Operator <c>"=="</c>.
 * |Boolean      | Integer  | <c>'=='</c>   | Boolean | Operator <c>"=="</c>.
 * |Boolean      | Integer  | <c>'=='</c>   | Integer | Operator <c>"=="</c>.
 * |Boolean      | Integer  | <c>'=='</c>   | Float   | Operator <c>"=="</c>.
 * |Boolean      | Float    | <c>'=='</c>   | Boolean | Operator <c>"=="</c>.
 * |Boolean      | Float    | <c>'=='</c>   | Integer | Operator <c>"=="</c>.
 * |Boolean      | Float    | <c>'=='</c>   | Float   | Operator <c>"=="</c>.
 * |Boolean      | Boolean  | <c>'!='</c>   | Boolean | Operator <c>"!="</c>.
 * |Boolean      | Boolean  | <c>'!='</c>   | Integer | Operator <c>"!="</c>.
 * |Boolean      | Boolean  | <c>'!='</c>   | Float   | Operator <c>"!="</c>.
 * |Boolean      | Integer  | <c>'!='</c>   | Boolean | Operator <c>"!="</c>.
 * |Boolean      | Integer  | <c>'!='</c>   | Integer | Operator <c>"!="</c>.
 * |Boolean      | Integer  | <c>'!='</c>   | Float   | Operator <c>"!="</c>.
 * |Boolean      | Float    | <c>'!='</c>   | Boolean | Operator <c>"!="</c>.
 * |Boolean      | Float    | <c>'!='</c>   | Integer | Operator <c>"!="</c>.
 * |Boolean      | Float    | <c>'!='</c>   | Float   | Operator <c>"!="</c>.
 * |Integer      | Integer  | <c>'&'</c>    | Integer | Operator <c>"&"</c>.
 * |Integer      | Integer  | <c>'^'</c>    | Integer | Operator <c>"^"</c>.
 * |Integer      | Integer  | <c>'\|'</c>   | Integer | Operator <c>"|"</c>.
 * |Boolean      | Boolean  | <c>'&&'</c>   | Boolean | Operator <c>"&&"</c>.
 * |Boolean      | Boolean  | <c>'&&'</c>   | Integer | Operator <c>"&&"</c>.
 * |Boolean      | Boolean  | <c>'&&'</c>   | Float   | Operator <c>"&&"</c>.
 * |Boolean      | Integer  | <c>'&&'</c>   | Boolean | Operator <c>"&&"</c>.
 * |Boolean      | Integer  | <c>'&&'</c>   | Integer | Operator <c>"&&"</c>.
 * |Boolean      | Integer  | <c>'&&'</c>   | Float   | Operator <c>"&&"</c>.
 * |Boolean      | Float    | <c>'&&'</c>   | Boolean | Operator <c>"&&"</c>.
 * |Boolean      | Float    | <c>'&&'</c>   | Integer | Operator <c>"&&"</c>.
 * |Boolean      | Float    | <c>'&&'</c>   | Float   | Operator <c>"&&"</c>.
 * |Boolean      | Boolean  | <c>'\|\|'</c> | Boolean | Operator <c>"||"</c>.
 * |Boolean      | Boolean  | <c>'\|\|'</c> | Integer | Operator <c>"||"</c>.
 * |Boolean      | Boolean  | <c>'\|\|'</c> | Float   | Operator <c>"||"</c>.
 * |Boolean      | Integer  | <c>'\|\|'</c> | Boolean | Operator <c>"||"</c>.
 * |Boolean      | Integer  | <c>'\|\|'</c> | Integer | Operator <c>"||"</c>.
 * |Boolean      | Integer  | <c>'\|\|'</c> | Float   | Operator <c>"||"</c>.
 * |Boolean      | Float    | <c>'\|\|'</c> | Boolean | Operator <c>"||"</c>.
 * |Boolean      | Float    | <c>'\|\|'</c> | Integer | Operator <c>"||"</c>.
 * |Boolean      | Float    | <c>'\|\|'</c> | Float   | Operator <c>"||"</c>.
 *
 **************************************************************************************************/
struct Arithmetics   : public plugins::Calculus
{
    /** ********************************************************************************************
     * Constructor. Creates the hash maps.
     * @param compiler The compiler we will get attached to.
     **********************************************************************************************/
     ALIB_API   Arithmetics( Compiler& compiler );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual    ~Arithmetics()                                                               override
    {}

    /** ********************************************************************************************
     * Invokes parent's method. On failure, tries to compile function <b>%Length(array)</b>.
     *
     * @param[in,out] ciFunction  The compilation information.
     * @return \c true if an entry was found. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIFunction& ciFunction )                               override;

};

/** ************************************************************************************************
 * This is the callback method for function \b %Boolean, which converts arbitrary
 * types to boolean values.
 * As an exclamation to the rule, this function is not defined in an anonymous namespace, but
 * exposed through the header of struct \alib{expressions,plugins::Arithmetics}.
 * The rationale for this is that the function is also used for auto-casting custom types to
 * boolean values, which is performed with compiler plug-in \alib{expressions,plugins::AutoCast}.
 *
 * The function is compile-time invokable and uses boxing interface \alib{boxing,IIsTrue} to
 * determine if a boxed value represents \c true or \c false.
 *
 * @param scope The scope.
 * @param args  The single argument.
 * @return The boxed boolean result.
 **************************************************************************************************/
ALIB_API
Box ToBoolean( Scope& scope, ArgIterator  args, ArgIterator );


}}}} // namespace [aworx::lib::expressions::detail]



#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "alib/expressions/plugins/calculus.hpp"
#endif


namespace alib {  namespace expressions { namespace plugins {

/** ************************************************************************************************
 * This built-in \alib{expressions,CompilerPlugin} of \alib_expressions_nl
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
     \verbatim
         size > 81920
         size > 8 * 1024
     \endverbatim
 *   are resulting in an identical expression program and thus the latter has no evaluation penalty.
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
 *   - With flag \alib{expressions::Compilation,AllowBitwiseBooleanOperators}:<br>
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
 * | Boolean     |\b Boolean |bool             | \<any\>  | Converts any type of boxed value by invoking box-function \alib{boxing,FIsTrue}.
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
 * | Integer      |<b>+</b> |Boolean | Converts the boolean value to integer.
 * | Integer      |<b>+</b> |Integer | Identity operator (has no effect).
 * | Float        |<b>+</b> |Float   | Identity operator (has no effect).
 * | Integer      |<b>-</b> |Boolean | Converts the boolean value to integer and negates it.
 * | Integer      |<b>-</b> |Integer | Negates an integral value.
 * | Float        |<b>-</b> |Float   | Negates a floating point value.
 * | Boolean      |<b>!</b> |Boolean | Boolean not operator.
 * | Boolean      |<b>!</b> |Integer | Returns the result of the comparison with <c>0</c>.
 * | Boolean      |<b>!</b> |Float   | Returns the result of the comparison with <c>0.0</c>.
 * | Integer      |<b>~</b> |Integer | Bitwise integral negation.
 *
 * <br>
 * <b>Binary Operators:</b><br>
 *
 * | Return Type | Lhs Type | Operator | Rhs Type | Description|
 * |-------------|----------|----------|----------|------------|
 * |Integer      | Boolean  | <b>*</b>    | Boolean | Multiplication with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Integer      | Boolean  | <b>*</b>    | Integer | Multiplication with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Float        | Boolean  | <b>*</b>    | Float   | Multiplication with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b>*</b>    | Boolean | Multiplication with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Integer      | Integer  | <b>*</b>    | Integer | Multiplication.
 * |Float        | Integer  | <b>*</b>    | Float   | Multiplication.
 * |Float        | Float    | <b>*</b>    | Boolean | Multiplication with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Float        | Float    | <b>*</b>    | Integer | Multiplication.
 * |Float        | Float    | <b>*</b>    | Float   | Multiplication.
 * |Integer      | Boolean  | <b>/</b>    | Integer | Division with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Float        | Boolean  | <b>/</b>    | Float   | Division with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Integer      | Integer  | <b>/</b>    | Integer | Division.
 * |Float        | Integer  | <b>/</b>    | Float   | Division.
 * |Float        | Float    | <b>/</b>    | Integer | Division.
 * |Float        | Float    | <b>/</b>    | Float   | Division.
 * |Integer      | Boolean  | <b>\%</b>   | Integer | Modulo with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Float        | Boolean  | <b>\%</b>   | Float   | Modulo with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Integer      | Integer  | <b>\%</b>   | Integer | Modulo.
 * |Float        | Integer  | <b>\%</b>   | Float   | Modulo.
 * |Float        | Float    | <b>\%</b>   | Integer | Modulo.
 * |Float        | Float    | <b>\%</b>   | Float   | Modulo.
 * |Integer      | Boolean  | <b>+</b>    | Boolean | Addition with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Integer      | Boolean  | <b>+</b>    | Integer | Addition with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Float        | Boolean  | <b>+</b>    | Float   | Addition with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b>+</b>    | Boolean | Addition with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Integer      | Integer  | <b>+</b>    | Integer | Addition.
 * |Float        | Integer  | <b>+</b>    | Float   | Addition.
 * |Float        | Float    | <b>+</b>    | Boolean | Addition with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Float        | Float    | <b>+</b>    | Integer | Addition.
 * |Float        | Float    | <b>+</b>    | Float   | Addition.
 * |Integer      | Boolean  | <b>-</b>    | Boolean | Subtraction with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Integer      | Boolean  | <b>-</b>    | Integer | Subtraction with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Float        | Boolean  | <b>-</b>    | Float   | Subtraction with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b>-</b>    | Boolean | Subtraction with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Integer      | Integer  | <b>-</b>    | Integer | Subtraction.
 * |Float        | Integer  | <b>-</b>    | Float   | Subtraction.
 * |Float        | Float    | <b>-</b>    | Boolean | Subtraction with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Float        | Float    | <b>-</b>    | Integer | Subtraction.
 * |Float        | Float    | <b>-</b>    | Float   | Subtraction.
 * |Integer      | Boolean  | <b><<</b>   | Integer | Bitwise shift left with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b><<</b>   | Boolean | Bitwise shift left with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b><<</b>   | Integer | Bitwise shift left.
 * |Integer      | Boolean  | <b>>></b>   | Integer | Bitwise shift right with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b>>></b>   | Boolean | Bitwise shift right with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Integer      | Integer  | <b>>></b>   | Integer | Bitwise shift right.
 * |Boolean      | Boolean  | <b><</b>    | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b><</b>    | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b><</b>    | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b><</b>    | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b><</b>    | Integer | Comparison operator.
 * |Boolean      | Integer  | <b><</b>    | Float   | Comparison operator.
 * |Boolean      | Float    | <b><</b>    | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b><</b>    | Integer | Comparison operator.
 * |Boolean      | Float    | <b><</b>    | Float   | Comparison operator.
 * |Boolean      | Boolean  | <b><=</b>   | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b><=</b>   | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b><=</b>   | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b><=</b>   | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b><=</b>   | Integer | Comparison operator.
 * |Boolean      | Integer  | <b><=</b>   | Float   | Comparison operator.
 * |Boolean      | Float    | <b><=</b>   | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b><=</b>   | Integer | Comparison operator.
 * |Boolean      | Float    | <b><=</b>   | Float   | Comparison operator.
 * |Boolean      | Boolean  | <b>></b>    | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>></b>    | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>></b>    | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b>></b>    | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b>></b>    | Integer | Comparison operator.
 * |Boolean      | Integer  | <b>></b>    | Float   | Comparison operator.
 * |Boolean      | Float    | <b>></b>    | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b>></b>    | Integer | Comparison operator.
 * |Boolean      | Float    | <b>></b>    | Float   | Comparison operator.
 * |Boolean      | Boolean  | <b>>=</b>   | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>>=</b>   | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>>=</b>   | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b>>=</b>   | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b>>=</b>   | Integer | Comparison operator.
 * |Boolean      | Integer  | <b>>=</b>   | Float   | Comparison operator.
 * |Boolean      | Float    | <b>>=</b>   | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b>>=</b>   | Integer | Comparison operator.
 * |Boolean      | Float    | <b>>=</b>   | Float   | Comparison operator.
 * |Boolean      | Boolean  | <b>==</b>   | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>==</b>   | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>==</b>   | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b>==</b>   | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b>==</b>   | Integer | Comparison operator.
 * |Boolean      | Integer  | <b>==</b>   | Float   | Comparison operator.
 * |Boolean      | Float    | <b>==</b>   | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b>==</b>   | Integer | Comparison operator.
 * |Boolean      | Float    | <b>==</b>   | Float   | Comparison operator.
 * |Boolean      | Boolean  | <b>!=</b>   | Boolean | Comparison operator with interpreting both boolean operands as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>!=</b>   | Integer | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Boolean  | <b>!=</b>   | Float   | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Integer  | <b>!=</b>   | Boolean | Comparison operator with interpreting the boolean operand as integral value \c 0 or \c 1.
 * |Boolean      | Integer  | <b>!=</b>   | Integer | Comparison operator.
 * |Boolean      | Integer  | <b>!=</b>   | Float   | Comparison operator.
 * |Boolean      | Float    | <b>!=</b>   | Boolean | Comparison operator with interpreting the boolean operand as floating point value \c 0.0 or \c 1.0.
 * |Boolean      | Float    | <b>!=</b>   | Integer | Comparison operator.
 * |Boolean      | Float    | <b>!=</b>   | Float   | Comparison operator.
 * |Integer      | Integer  | <b>&</b>    | Integer | Bitwise boolean "and" operator.
 * |Integer      | Integer  | <b>\|</b>   | Integer | Bitwise boolean "or" operator.
 * |Integer      | Integer  | <b>^</b>    | Integer | Bitwise boolean "xor" operator.
 * |Boolean      | Boolean  | <b>&&</b>   | Boolean | Logical boolean "and" operator.
 * |Boolean      | Boolean  | <b>&&</b>   | Integer | Logical boolean "and" operator. The integral operand is tested for a non-zero value.
 * |Boolean      | Boolean  | <b>&&</b>   | Float   | Logical boolean "and" operator. The floating point operand is tested for a non-zero value.
 * |Boolean      | Integer  | <b>&&</b>   | Boolean | Logical boolean "and" operator. The integral operand is tested for a non-zero value.
 * |Boolean      | Integer  | <b>&&</b>   | Integer | Logical boolean "and" operator. The integral operands are tested for a non-zero value.
 * |Boolean      | Integer  | <b>&&</b>   | Float   | Logical boolean "and" operator. The integral and floating point operands are tested for a non-zero value.
 * |Boolean      | Float    | <b>&&</b>   | Boolean | Logical boolean "and" operator. The floating point operand is tested for a non-zero value.
 * |Boolean      | Float    | <b>&&</b>   | Integer | Logical boolean "and" operator. The integral and floating point operands are tested for a non-zero value.
 * |Boolean      | Float    | <b>&&</b>   | Float   | Logical boolean "and" operator. The floating point operands are tested for a non-zero value.
 * |Boolean      | Boolean  | <b>\|\|</b> | Boolean | Logical boolean "or" operator.
 * |Boolean      | Boolean  | <b>\|\|</b> | Integer | Logical boolean "or" operator. The integral operand is tested for a non-zero value.
 * |Boolean      | Boolean  | <b>\|\|</b> | Float   | Logical boolean "or" operator. The floating point operand is tested for a non-zero value.
 * |Boolean      | Integer  | <b>\|\|</b> | Boolean | Logical boolean "or" operator. The integral operand is tested for a non-zero value.
 * |Boolean      | Integer  | <b>\|\|</b> | Integer | Logical boolean "or" operator. The integral operands are tested for a non-zero value.
 * |Boolean      | Integer  | <b>\|\|</b> | Float   | Logical boolean "or" operator. The integral and floating point operands are tested for a non-zero value.
 * |Boolean      | Float    | <b>\|\|</b> | Boolean | Logical boolean "or" operator. The floating point operand is tested for a non-zero value.
 * |Boolean      | Float    | <b>\|\|</b> | Integer | Logical boolean "or" operator. The integral and floating point operands are tested for a non-zero value.
 * |Boolean      | Float    | <b>\|\|</b> | Float   | Logical boolean "or" operator. The floating point operands are tested for a non-zero value.
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
 * The function is compile-time invokable and uses box-function \alib{boxing,FIsTrue} to
 * determine if a boxed value represents \c true or \c false.
 *
 * @param scope The scope.
 * @param args  The single argument.
 * @return The boxed boolean result.
 **************************************************************************************************/
ALIB_API
Box ToBoolean( Scope& scope, ArgIterator  args, ArgIterator );


}}} // namespace [alib::expressions::detail]



#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_ARITHMETICS

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_MATH
#define HPP_ALIB_EXPRESSIONS_PLUGINS_MATH

#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_CALCULUS
#   include "alib/expressions/plugins/calculus.hpp"
#endif


namespace aworx { namespace lib { namespace expressions { namespace plugins {

/** ************************************************************************************************
 * %Compiler plug-in that provides mathematical functions.
 *
 * All identifier and function names are defined case insensitive and can \e not be abbreviated with
 * the exclamation of
 * \b remainder (shortenable up to \b remain) and
 * \b random (shortenable up to \b rand).
 *
 * <b>Constants:</b><br>
 *
 * Type    | Name  | Description
 * --------|-------|-------------
 * Float   |\b PI  | The ratio of a circle's circumference to its diameter.
 * Float   |\b E   | Euler's number.
 *
 * <br>
 * <b>Functions:</b><br>
 *
 * Return Type| Name      |  Signature | Description |
 * -----------|-----------|------------|-------------|
 * Float      | \b abs        | Integer         | Computes the absolute value of an integer number.
 * Float      | \b abs        | Float           | Computes the absolute value of a floating point number.
 * Float      | \b ceil       | Float           | Returns the nearest integer not less than the given value.
 * Float      | \b floor      | Float           | Returns the nearest integer not greater than the given value.
 * Float      | \b trunc      | Float           | Returns the nearest integer not greater in magnitude than the given value.
 * Float      | \b round      | Float           | Returns the nearest integer, rounding away from zero in halfway cases.
 * Integer    | \b rint       | Float           | Returns the nearest integer, rounding away from zero in halfway cases.
 * Float      | \b max        | Integer,Integer | Returns the greater of two integer numbers.
 * Float      | \b max        | Float, Float    | Returns the greater of two floating point numbers.
 * Float      | \b min        | Integer,Integer | Returns the smaller of two integer numbers.
 * Float      | \b min        | Float, Float    | Returns the smaller of two floating point numbers.
 * Float      | \b remainder  | Float,Float     | Computes the remainder of the floating point division of the first argument by the second argument.
 * Float      | \b random     | Float           | Returns a random number in the range [0.0 ... 1.0[. Note uses \c std::rand. No seeding is performed. Invoke \c std::srand prior to using this function for the first time, if numbers should change between different runs of the software.
 * Float      | \b sin        | Float           | Computes sine.
 * Float      | \b asin       | Float           | Computes cosine.
 * Float      | \b cos        | Float           | Computes tangent.
 * Float      | \b acos       | Float           | Computes arc sine.
 * Float      | \b tan        | Float           | Computes arc cosine.
 * Float      | \b atan       | Float           | Computes arc tangent.
 * Float      | \b sinh       | Float           | Computes hyperbolic sine.
 * Float      | \b asinh      | Float           | Computes hyperbolic cosine.
 * Float      | \b cosh       | Float           | Computes hyperbolic tangent.
 * Float      | \b acosh      | Float           | Computes inverse hyperbolic sine.
 * Float      | \b tanh       | Float           | Computes inverse hyperbolic cosine.
 * Float      | \b atanh      | Float           | Computes inverse hyperbolic tangent.
 * Float      | \b exp        | Float           | Computes \e e raised to the given power.
 * Float      | \b exp2       | Float           | Computes \e 2.0 raised to the given power.
 * Float      | \b exp10      | Float           | Computes \e 10.0 raised to the given power.
 * Float      | \b log        | Float           | Computes natural base (\e e) logarithm.
 * Float      | \b log10      | Float           | Computes common base (\e 10) logarithm.
 * Float      | \b log2       | Float           | Computes base \e 2.0 logarithm.
 * Float      | \b pow        | Float, Float    | Raises a given number to the given power.
 * Float      | \b pow10      | Float           | Raises \e 10 to the given power.
 * Float      | \b sqrt       | Float           | Computes square root of a given number.
 * Float      | \b cbrt       | Float           | Computes cubic root of a given number.
 **************************************************************************************************/
struct Math   : public plugins::Calculus
{
    /** ********************************************************************************************
     * Constructor. Creates the hash maps.
     * @param compiler The compiler we will get attached to.
     **********************************************************************************************/
    ALIB_API    Math( Compiler& compiler );

    /** ********************************************************************************************
     * Virtual destructor
     **********************************************************************************************/
    virtual    ~Math()
    {}
};


}}}} // namespace [aworx::lib::expressions::detail]



#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_MATH

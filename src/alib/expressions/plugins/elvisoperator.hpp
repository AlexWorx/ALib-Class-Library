/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_ELVIS
#define HPP_ALIB_EXPRESSIONS_PLUGINS_ELVIS

#ifndef HPP_ALIB_EXPRESSIONS_COMPILERPLUGIN
#   include "alib/expressions/compilerplugin.hpp"
#endif


namespace alib {  namespace expressions { namespace plugins {

/** ************************************************************************************************
 * The binary variant of ternary operator <c>"Q ? T : F"</c> is created by leaving out
 * term \c 'T', which results in <c>"A ? : B"</c>. This operator is often called
 * <b>"Elvis Operator"</b> because <c>?:</c> reminds people to an emoticon of singer Elvis Presley.
 *
 * Usually the operator chooses \c 'A' in the case that \c 'A' can be interpreted as something like
 * not null, not \e nulled or \c true in any means. Otherwise it selects \c 'B'.
 * Due to the type-safe approach of \alib_expressions_nl, the type of \c 'A' and \c 'B'
 * must be equal, or at least an auto-cast has to exist that casts the types to a pair of
 * equal types.
 *
 * This built-in compiler plug-in covers the operator for virtually any type, including custom ones.
 * The implementation invokes box-function \alib{boxing,FIsTrue} on term \c 'A' and chooses
 * \c 'A' if the interface returned \c true, otherwise it chooses \c 'B'.
 *
 * Often, the elvis operator is used with strings. Because the default implementation of boxing
 * interface \alib{boxing,FIsTrue} returns \c true if the boxed value is an array type and the
 * array length is not zero, with strings given (which are arrays of characters) only empty strings
 * evaluate to \c false. This way the expression:
 *
 *          "" ?: "Elvis"
 *
 * rightfully evaluates to string "Elvis".
 *
 * With this default implementation, it should be very seldom needed to define a custom
 * Elvis-Operator for custom types.
 **************************************************************************************************/
struct ElvisOperator   :  public CompilerPlugin
{
    /** ********************************************************************************************
     * Constructor.
     * @param compiler The compiler we will get attached to.
     **********************************************************************************************/
    ALIB_API        ElvisOperator( Compiler& compiler );

    /** ********************************************************************************************
     * Virtual destructor.
     **********************************************************************************************/
    virtual        ~ElvisOperator()                                                         override
    {}

    /** ********************************************************************************************
     * Compiles binary elvis operator <c>A ?: B</c> if terms \c 'A' and \c 'B' share the same type.
     *
     * @param[in,out]  ciBinaryOp  The compilation info struct.
     * @return \c true if an entry was found. \c false otherwise.
     **********************************************************************************************/
    ALIB_API
    virtual bool    TryCompilation( CIBinaryOp& ciBinaryOp )                               override;

};

}}} // namespace [alib::expressions::detail]

#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_ELVIS

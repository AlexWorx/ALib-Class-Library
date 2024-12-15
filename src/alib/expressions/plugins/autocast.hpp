//==================================================================================================
/// \file
/// This header file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_EXPRESSIONS_PLUGINS_AUTOCAST
#define HPP_ALIB_EXPRESSIONS_PLUGINS_AUTOCAST
#pragma once
#include "alib/expressions/compilerplugin.hpp"


namespace alib {  namespace expressions { namespace plugins {

//==================================================================================================
/// This built-in compiler plug-in of \alib_expressions_nl, performs auto-casts only if two
/// different types are given. In this case, the following casts are performed:
///
/// 1. If one argument is of type \alib{expressions;Types::String}, the other is converted to
///    string. The expression function returned for (optional) de-compilation is \b "String()".
///
/// 2. If one argument is of type \alib{expressions;Types::Float} and the other of type
///    \alib{expressions;Types::Integer}, the integral value is cast to floating point.
///    The expression function returned for (optional) de-compilation is \b "Float()".
///
/// 3. If one argument is of type \alib{expressions;Types::Float} and the other of type
///    \alib{expressions;Types::Boolean}, the boolean value is cast to floating point.
///    The expression function returned for (optional) de-compilation is \b "Float()".
///
/// 4. If one argument is of type \alib{expressions;Types::Integer} and the other of type
///    \alib{expressions;Types::Boolean}, the boolean value is cast to integer.
///    The expression function returned for (optional) de-compilation is \b "Integer()".
///
/// 5. If one argument is of type \alib{expressions;Types::Boolean}, the other is converted to
///    boolean using an internal callback function that simply invokes box-function
///    \alib{boxing;FIsTrue}.
///    The expression function returned for (optional) de-compilation is \b "Boolean()".
//==================================================================================================
struct AutoCast  : public CompilerPlugin
{
    //==============================================================================================
    /// Constructor.
    /// @param compiler The compiler we will get attached to.
    //==============================================================================================
    ALIB_API        AutoCast( Compiler& compiler );

    //==============================================================================================
    /// Virtual destructor.
    //==============================================================================================
    virtual        ~AutoCast()                                                              override
    {}

    //==============================================================================================
    /// Offers auto-cast functions for built-in types.
    ///
    /// @param[out] ciAutoCast  The compilation result.
    /// @return \c true if an entry was found. \c false otherwise.
    //==============================================================================================
    ALIB_API
    virtual bool    TryCompilation( CIAutoCast& ciAutoCast )                               override;
};

}}} // namespace [alib::expressions::detail]

#endif // HPP_ALIB_EXPRESSIONS_PLUGINS_AUTOCAST


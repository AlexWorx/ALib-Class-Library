//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions { namespace plugins {

//==================================================================================================
/// This built-in compiler plug-in of \alib_expressions_nl, performs auto-casts only if two
/// different types are given. In this case, the following casts are performed:
///
/// 1. If one argument is of type #"Types::String;*", the other is converted to
///    string. The expression function returned for (optional) de-compilation is \b "String()".
///
/// 2. If one argument is of type #"Types::Float;*" and the other of type
///    #"Types::Integer;*", the integral value is cast to floating point.
///    The expression function returned for (optional) de-compilation is \b "Float()".
///
/// 3. If one argument is of type #"Types::Float;*" and the other of type
///    #"Types::Boolean;*", the boolean value is cast to floating point.
///    The expression function returned for (optional) de-compilation is \b "Float()".
///
/// 4. If one argument is of type #"Types::Integer;*" and the other of type
///    #"Types::Boolean;*", the boolean value is cast to integer.
///    The expression function returned for (optional) de-compilation is \b "Integer()".
///
/// 5. If one argument is of type #"Types::Boolean;*", the other is converted to
///    boolean using an internal callback function that simply invokes box-function
///    #"FIsTrue".
///    The expression function returned for (optional) de-compilation is \b "Boolean()".
//==================================================================================================
struct AutoCast  : public CompilerPlugin {
    /// Constructor.
    /// @param compiler The compiler we will get attached to.
    ALIB_DLL        AutoCast( Compiler& compiler );

    /// Virtual destructor.
    virtual        ~AutoCast()                                                           override {}

    /// Offers auto-cast functions for built-in types.
    ///
    /// @param[out] ciAutoCast  The compilation result.
    /// @return \c true if an entry was found. \c false otherwise.
    ALIB_DLL
    virtual bool    TryCompilation( CIAutoCast& ciAutoCast )                               override;
};

}}} // namespace [alib::expressions::detail]

//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::expressions   { class Compiler;
namespace detail {

//==================================================================================================
/// This detail class constitutes an abstract base class for expression parsers.
//==================================================================================================
struct Parser
{
    /// Virtual destructor.
    virtual    ~Parser()                                                                          {}

    /// Parses the given expression string.
    /// \note
    ///   The return value is hidden by using <c>void*</c>. This is to allow avoid flooding
    ///   of \c boost header includes files to the code entities using module \alib_expressions_nl.
    ///
    /// @param expressionString The string to parse.
    /// @param numberFormat     Used to parse literal numbers.
    /// @return  The abstract syntax tree representing the expression.
    virtual detail::ASTBase* Parse( const String&            expressionString,
                                    NumberFormat*            numberFormat                )          = 0;


    /// Static method to create a parser object.
    ///
    /// @param compiler The compiler that needs a parser.
    /// @return The parser.
    ALIB_DLL static
    Parser*     Create( Compiler& compiler );
};

}} // namespace [alib::expressions::detail]


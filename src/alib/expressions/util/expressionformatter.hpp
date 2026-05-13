//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace expressions::util {

//==================================================================================================
/// This class allows using \alib_expressions_nl within format strings used with
/// #"format::Formatter;ALib Formatters" and thus allows exposing customizable format
/// strings to end-users, which leverage an application's expression subsystem.
///
/// To understand the concept of this class, please read the documentation of class
/// #"PropertyFormatter" \b first, which is almost a 1:1 copy of this class but
/// uses simple "property callback functions" instead of expressions.
///
/// \note
///    This documentation rather documents the differences to #"%PropertyFormatter" instead of
///    repeating what is explained already in the sibling class.
///
/// This class can be used in the same way as #"PropertyFormatter". However,
/// instead of the provision of a table of type
/// #"PropertyFormatter::TCallbackTable;*", this class expects an expression
/// compiler in the constructor.
///
/// With that, simple expressions comprising just a single identifier term can be used in
/// exactly the same way as demonstrated with #"%PropertyFormatter". For example, format string:
///
///      "{name} is aged {age} and his/her hobby is {hobby}"
///
/// would compile three expressions (<em>"name"</em>, <em>"age"</em> and <em>"hobby"</em>) and
/// convert the format string to just
///
///      "{} is aged {} and his/her hobby is {}"
///
/// Then, with the invocation of method #".Format", an expression scope has to be provided, which
/// is used to evaluate each expression and add the result value to the list of format parameters.
///
/// To separate the given expressions strings from other formatting information provided in a
/// placeholder, a special separator character is used. This character is provided with construction
/// and defaults to the symbol \c '@'. A format string that uses
/// #"FormatterPythonStyle;python formatting syntax" might look like this:
///
///      "The surface is {width * height@:>5.2} sqm."
///
/// <p>
///\I{#############################################################################################}
/// # Reference Documentation #
/// @throws alib::format::FMTExceptions::UnknownPropertyInFormatString \I{CLANGDUMMY}
//==================================================================================================
class ExpressionFormatter {
  protected:
    /// The expression compiler (as given in the constructor).
    Compiler*                   compiler;

    /// The formatter to use (as given in the constructor).
    SPFormatter                 stdFormatter;

    /// The original format string. Used only for exception information.
    const AString               originalFormatString;

    /// The resulting format string passed to \alib formatters.
    AString                     formatStringStripped;

    /// The expression functions to receive the format data.
    std::vector<Expression>     expressions;

    /// The end-positions of the substrings in #"formatStringStripped". Starting with the second,
    /// each contains one placeholder.
    std::vector<integer>        formatSubstrings;
  public:

    /// Constructor. Processes the given format string and compiles embedded expressions, which
    /// are then used with invocations of method #".Format".
    ///
    /// @param formatString        The format string as described in the class documentation.
    /// @param compiler            The expression compiler.
    /// @param formatter           The formatter to use. Defaults to \c nullptr which selects
    ///                            #"Formatter::DEFAULT;default formatter".
    /// @param separatorChar       The character that is to be used to separate the expression
    ///                            from the placeholder format information in
    ///                            \p{customFormatString}.<br>
    ///                            Defaults to <c>'@'</c>.
    ///
    /// @throws alib::format::FMTExceptions::UnknownPropertyInFormatString. \I{CLANGDUMMY}
    ALIB_DLL
    ExpressionFormatter( const String&  formatString,
                         Compiler*      compiler,
                         SPFormatter    formatter            = nullptr,
                         character      separatorChar        = '@'          );

    /// Writes the formatted output of the expressions in accordance to the given \p{scope}
    /// object to given \p{target} string.
    ///
    /// @param target    The target string to write into.
    /// @param scope     The scope used to evaluate the expressions found in the format string.
    ALIB_DLL
    void    Format( AString& target, expressions::Scope&  scope );

}; // class ExpressionFormatter

} // namespace alib[::expressions::util]

/// Type alias in namespace #"%alib".
using ExpressionFormatter  =    expressions::util::ExpressionFormatter;

} // namespace [alib]

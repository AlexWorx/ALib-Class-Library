//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions { namespace detail {

//==================================================================================================
/// Implementation of the default parser of module \alib_expressions_nl.
///
/// This internal class is not too well documented. Nevertheless, it is designed to be able
/// to tweak its behavior slightly and in case of need, even derive and use a custom parser
/// class. For doing so, please consult the source code of this class.
/// A custom parser might be set to protected field \alib{expressions;Compiler::parser}by a derived
/// compiler type before compiling a first expression.
///
/// It is possible to define scannable custom unary and binary operators. Definitions of binary
/// operators include a "precedence value" that allows aligning them with the built-in types.
/// Also, built-in operators can be \em removed if wanted.
//==================================================================================================
class ParserImpl  : public Parser
{
  protected:
        /// Types of tokens.
        enum class Tokens : char
        {
            EOT             = '\0', ///< End of tokens. (No next token available.)
            SymbolicOp      = 'O' , ///< A symbolic operator. Can be unary or binary.
            AlphaUnOp       = 'U' , ///< An alphabetic unary operator.
            AlphaBinOp      = 'B' , ///< An alphabetic binary operator.

            LitString       = 'S' , ///< a string literal.
            LitInteger      = 'I' , ///< a integer literal.
            LitFloat        = 'F' , ///< a float literal.

            Identifier      = 'A' , ///< An identifier.

            BraceOpen       = '(' , ///< An opening brace.
            BraceClose      = ')' , ///< A closing brace.
            Comma           = ',' , ///< A comma.
                                    
            SubscriptOpen   = '[' , ///< An opening subscript brace.
            SubscriptClose  = ']' , ///< A closing subscript brace.
        };

    /// Memory for temporary allocations, like AST objects or literal strings with converted
    /// escape sequences. Provided by the compiler with the method #Parse.
    MonoAllocator&              compileTimeAllocator;

    /// The actual token type.
    Tokens                      token;

    /// The actual token type.
    ASTLiteral::NFHint          tokLiteralHint;

    /// Integer value of token (if applicable).
    integer                     tokInteger;

    /// Float value of token (if applicable).
    double                      tokFloat;

    /// String value of token (if applicable).
    String                      tokString;

    /// The position of the token in #expression.
    integer                     tokPosition;

    /// The compiler that this parser works for.
    Compiler&                   compiler;

    /// Used for scanning literals. Provided to this class with each parse request.
    NumberFormat*               numberFormat;


    /// The given expression to parse.
    String                      expression;

    /// The rest of #expression.
    Substring                   scanner;

    /// Lists single characters that get directly converted into tokens of corresponding type
    /// when found in the expression string. Tokens are <c>"()[],"</c>.
    BitSet<256>                 syntaxTokens;

    /// Lists single characters that got found in operator strings which have been registered
    /// with
    /// \alib{expressions;Compiler::AddUnaryOperator} and
    /// \alib{expressions;Compiler::AddBinaryOperator}.
    ///
    /// Used by the internal token scanner (lexer) and by default will become something like
    /// <c>"=+-*%/?:~!|&^!<>/%"</c>.
    /// when found in the expression string.
    BitSet<256>                 operatorChars;

    /// Hash set of unary operators. The key of the table is the operator string, which usually
    /// consists of one character, like <c>'-'</c> or <c>'!'</c>.
    ///
    /// This table is filled in the constructor of the class with the values stored in
    /// \alib{expressions;Compiler::UnaryOperators} and used for testing of existence.
    HashSet< MonoAllocator,
             String,
             alib::hash_string_ignore_case    <character>,
             alib::equal_to_string_ignore_case<character>  >  unaryOperators;

    /// Hash set of binary operators. The key of the table is the operator string, which usually
    /// consists of one to three characters, like <c>'+'</c> or <c>'<<='</c>.
    ///
    /// This table is filled in the constructor of the class with the values stored in
    /// \alib{expressions;Compiler::BinaryOperators} and used for testing of existence.
    HashSet< MonoAllocator,
             String,
             alib::hash_string_ignore_case    <character>,
             alib::equal_to_string_ignore_case<character>  >  binaryOperators;

    /// List of ASTs currently created in recursion.
    /// \note
    ///   This vector is created in the monotonic allocator and never even deleted, as all
    ///   inserted \b AST elements, exclusively allocate from the same temporary allocator.
    StdVectorMA<AST*>*                                        ASTs;

  //################################################################################################
  // Constructor/destructor, interface
  //################################################################################################
  public:
    /// Constructor.
    /// @param compiler   The compiler that this parser works for.
    /// @param allocator  A monotonic allocator for permanent allocations.
    ParserImpl( Compiler& compiler, MonoAllocator& allocator );

    /// Virtual destructor.
    virtual    ~ParserImpl()                                                             override {}

    /// Parses the given expression string.
    /// \note
    ///   The return value is hidden by using <c>void*</c>. This is to allow avoid flooding
    ///   of \c boost header includes files to the code entities using module \alib_expressions_nl.
    ///
    /// @param exprString The string to parse.
    /// @param nf         Used to scan number literals.
    /// @return The abstract syntax tree representing the expression.
    ALIB_DLL virtual
    detail::AST* Parse( const String&   exprString, NumberFormat* nf )                     override;


  protected:
  //################################################################################################
  // Lexer
  //################################################################################################
    /// This is the "scanner" or "lexer" method.
    void        NextToken();


  //################################################################################################
  // Parser
  //################################################################################################

    /// Tests if the actual token represents a known unary operator.
    /// @return Returns the binary operator symbol, respectively a \e nulled string on failure.
    ALIB_DLL
    String      getUnaryOp();

    /// Tests if the actual token represents a known binary operator.
    /// @return Returns the binary operator symbol, respectively a \e nulled string on failure.
    ALIB_DLL
    String      getBinaryOp();

    /// Internal method that optionally parses a conditional operator (<c>Q ? T : F</c> )
    ///
    /// @return T.
    AST*        parseConditional();

    /// Internal method that optionally parses a binary operator and levels (recursively)
    /// trees of such according to operator precedence and brackets given.
    /// @return T.
    AST*        parseBinary();

    /// Parses unary ops, literals, identifiers, functions and expressions surrounded by
    /// brackets.
    /// @return The abstract syntax tree node parsed.
    AST*        parseSimple();

    /// Invoked after an identifier or function was parsed. Tests for subscript
    /// operator, otherwise returns the given ast as is.
    /// @param  function The identifier or function parsed.
    /// @return Either the given node or a node of type \alib{expressions::detail;ASTBinaryOp}
    ///         with \c lhs set to \p{function}, \c rhs to the parsed subscript arg and
    ///         operator set to <c>'[]'</c>.
    AST*        parseSubscript( AST* function );



    /// Simple shortcut popping and returning last ast from the current list.
    /// @return Popped AST object.
    AST* pop() {
        AST* ast= ASTs->back();
        ASTs->pop_back();
        return ast;
    }

    /// Simple shortcut pushing an ast to current list and returning it.
    /// @param  ast The AST node to push.
    /// @return Popped AST object.
    AST* push( AST* ast )                                   { ASTs->emplace_back(ast); return ast; }

    /// Simple shortcut to the topmost AST.
    /// @return The topmost AST object.
    AST* top()                                                              { return ASTs->back(); }

    /// Simple shortcut replacing the topmost ast.
    /// @param  ast The new AST node to replace the existing one with.
    /// @return The given object.
    AST* replace( AST* ast )                                      { ASTs->back()= ast; return ast; }
};  // class ParserImpl


}}} // namespace [alib::expressions::detail]

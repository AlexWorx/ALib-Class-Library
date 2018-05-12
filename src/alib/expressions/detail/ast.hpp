// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_AST
#define HPP_ALIB_EXPRESSIONS_DETAIL_AST


namespace aworx { namespace lib { namespace expressions { namespace detail {

class Program;

/**
 * Base class for nodes of abstract syntax trees of module \alibmod_expressions.
 */
struct AST
{
    /// The type of node.
    enum class Types
    {
        Literal     ,
        Identifier  ,
        Function    ,
        UnaryOp     ,
        BinaryOp    ,
        TernaryOp   ,
    };

    /// Type of derived this AST node
    Types   NodeType;

    /// Position in original expression string.
    integer Position;

    /// Deleted default constructor.
    AST() = delete;

    /**
     * Constructor
     * @param type      The node type.
     * @param position  The index of this AST in the expression string.
     */
    AST( Types type, integer position )
    : NodeType(type)
    , Position(position)
    {}

    /// Virtual destructor.
    virtual ~AST()
    {}

    /**
     * Recursively compiles nested nodes and invokes one of the add-methods of program for itself.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                         = 0;

    /**
     * Recursively walks through the tree and performs optimizations, dependent on given flags.
     *
     * As of today, the only optimization performed in this AST itself is to combine nested unary
     * '+' and '-' operators on literals.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                        = 0;
};


/**
 * Abstract syntax tree node representing identifiers.
 */
struct ASTLiteral       : public AST
{
    /**
     * This is a list of flags that store information obbout the format of the literal given
     * in the expression string. This information is used with the generation of a normalized
     * version of the literal.
     */
    enum class NFHint
    {
        NONE         , ///< No hint.
        Scientific   , ///< Float was given in scientific format
        Hexadecimal  , ///< Integral value was given in hexadecimal format.
        Octal        , ///< Integral value was given in octal format.
        Binary       , ///< Integral value was given in binary format.
    };

    Box         Value;         ///< The value of the literal.
    NFHint      Format;        ///< The value of the literal.
    AString     stringValue;   ///< Used with string literals as string copy buffer.

    /**
     * Constructs a string literal.
     * @param value    The value of the literal. Will be copied to #stringValue, which then is
     *                 itself placed in #Value.
     * @param position The index of this AST in the expression string.
     */
    ASTLiteral( const String& value, integer position )
    : AST(Types::Literal, position)
    , Format(NFHint::NONE)
    {
        stringValue << value;
        Value= stringValue;
    }

    /**
     * Constructs a string literal from a movable \b %AString.
     * @param value    The value of the literal. Will be moved to #stringValue, which then is
     *                 itself placed in #Value.
     * @param position The index of this AST in the expression string.
     */
    ASTLiteral( AString&& value, integer position )
    : AST(Types::Literal, position)
    , Format(NFHint::NONE)
    {
        stringValue= value;
        Value= stringValue;
    }

    /**
     * Constructs an integer literal.
     * @param value     The value of the literal.
     * @param position  The index of this AST in the expression string.
     * @param hint      A hint about the format that expression string used to express the literal.
     */
    ASTLiteral( integer value, integer position, NFHint hint= NFHint::NONE )
    : AST(Types::Literal, position)
    , Format(hint)
    {
        Value= value;
    }

    /**
     * Constructs a floating point literal.
     * @param value    The value of the literal.
     * @param position The index of this AST in the expression string.
     * @param hint     Optional information about the number format that was detected while parsing.
     */
    ASTLiteral( double value, integer position, NFHint hint= NFHint::NONE )
    : AST(Types::Literal, position)
    , Format(hint)
    {
        Value= value;
    }

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};


/**
 * Abstract syntax tree node representing identifiers.
 */
struct ASTIdentifier    : public AST
{
    AString     Name;   ///< The name of the identifier as parsed from the expression string.

    /**
     * Constructor providing all fields.
     * @param name      The name of the identifier.
     * @param position  The index of this AST in the expression string.
     */
    explicit
    ASTIdentifier( const String& name, integer position )
    : AST(Types::Identifier, position)
    , Name(name)
    {}

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};

/**
 * Abstract syntax tree node representing a function call.
 */
struct ASTFunction      : public AST
{
    AString                            Name;      ///< Enum element denoting the operator.
    std::vector<AST*>                  Arguments; ///< The argument nodes.

    /**
     * Constructor providing name, but not arguments, yet.
     * @param name      The name of the function
     * @param position  The index of this AST in the expression string.
     */
    explicit
    ASTFunction(const String name, integer position )
    : AST(Types::Function, position)
    , Name(name)
    {}

    /**
     * Virtual destructor.
     */
    ALIB_API virtual
    ~ASTFunction()                                                                         override;

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};


/**
 * Abstract syntax tree node representing unary operators.
 */
struct ASTUnaryOp       : public AST
{
    String  Operator; ///< The operator symbol.
    AST*    Argument; ///< The argument node.

    /**
     * Constructor providing all fields.
     * @param op       The operator symbol.
     * @param argument The argument of the operator.
     * @param position The index of this AST in the expression string.
     */
    explicit
    ASTUnaryOp(const String& op, AST* argument, integer position )
    : AST(Types::UnaryOp, position)
    , Operator(op), Argument(argument)
    {}

    /**
     * Virtual destructor.
     */
    ALIB_API virtual
    ~ASTUnaryOp()                                                                          override;

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};

/**
 * Abstract syntax tree node representing binary operators.
 */
struct ASTBinaryOp      : public AST
{
    String  Operator;  ///< The operator symbol.
    AST*    Lhs;       ///< The left-hand-side expression node.
    AST*    Rhs;       ///< The right-hand-side expression node.
    /**
     * Constructor providing all fields.
     * @param lhs       Left-hand side node.
     * @param rhs       Right-hand side node.
     * @param op        The operator symbol.
     * @param position  The index of this AST in the expression string.
     */
    explicit
    ASTBinaryOp( const String& op, AST* lhs, AST* rhs, integer position )
    : AST(Types::BinaryOp, position)
    , Operator(op), Lhs(lhs), Rhs(rhs)
    {}

    /**
     * Virtual destructor.
     */
    ALIB_API virtual
    ~ASTBinaryOp()                                                                         override;

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};

/**
 * Abstract syntax tree node representing ternary operator <c>Q ? T : F</c>.
 */
struct ASTConditional     : public AST
{
    AST*         Q;             ///< The question.
    AST*         T;             ///< The true-result.
    AST*         F;             ///< The false-result.
    integer      ColonPosition; ///< The index of the colon in the expression string.

    /**
     * Constructor providing all fields.
     * @param q              The question.
     * @param t              The true-result.
     * @param f              The false-result.
     * @param position       The index of the question mark in the expression string.
     * @param colonPosition  The index of the colon in the expression string.
     */
    explicit
    ASTConditional(AST* q, AST* t, AST* f, integer position, integer colonPosition)
    : AST(Types::TernaryOp, position)
    , Q(q), T(t), F(f)
    , ColonPosition(colonPosition)
    {}

    /**
     * Virtual destructor.
     */
    ALIB_API virtual
    ~ASTConditional()                                                                      override;

    /**
     * Implements abstract method.
     * @param program         The program to be compiled.
     * @param[out] normalized The normalized string, built during recursive compilation of the AST.
     */
    virtual void Assemble( Program& program, AString & normalized )                    override;

    /**
     * Implements abstract method.
     * @param normalization The compiler flags denoting the normalization settings.
     * @return A potentially replaced AST or itself.
     */
    virtual AST* Optimize( Normalization normalization )                                   override;
};

}}}}; // namespace [aworx::lib::expressions::detail]


#endif // HPP_ALIB_EXPRESSIONS_DETAIL_AST

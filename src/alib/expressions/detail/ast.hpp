//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions { namespace detail {

/// Base class for nodes of abstract syntax trees of module \alib_expressions.
/// Note that AST objects (and their data) are allocated in a #"MonoAllocator" and
/// hence have empty destructors.
struct ASTBase {
    /// The type of node.
    enum class Types {
        Literal     , ///< A literal.
        Identifier  , ///< An identifier.
        Function    , ///< A function.
        UnaryOp     , ///< A unary operator.
        BinaryOp    , ///< A binary operator.
        TernaryOp   , ///< A ternary operator.
    };

    /// Type of derived this AST node
    Types   NodeType;

    /// Constructor
    /// @param type      The node type.
    ASTBase( Types type )
    : NodeType(type)                                                                              {}
};

}}} // namespace [alib::expressions::detail]

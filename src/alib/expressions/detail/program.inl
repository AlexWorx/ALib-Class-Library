//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions {

/// This is the detail namespace of #alib::expressions implementing the abstract syntax tree,
/// the expression parser, the expression program and the virtual machine to execute the program
/// with expression evaluation.
namespace detail {


//==================================================================================================
/// This class represents a program that is "run on" the \alib{expressions;detail::VirtualMachine}
/// to evaluate an expression.
///
/// ## Friends ##
/// class \alib{expressions;detail::VirtualMachine}
//==================================================================================================
class Program : public ProgramBase
{
  //################################################################################################
  // Public fields
  //################################################################################################
  public:
    /// The compiler that created this object.
    Compiler&                   compiler;

    /// The expression that this program evaluates.
    ExpressionVal&            expression;


  //################################################################################################
  // Internal fields
  //################################################################################################
  protected:
        /// Shortcut.
        using VM= VirtualMachine;

    /// The array of commands.
    VM::Command*                              commands;

    /// The number of commands.
    integer                                   commandsCount;

    /// List of compile-time identified nested expressions. Using the shared pointers, it is
    /// ensured that the expressions do not get deleted until this program is.
    StdVectorMA<Expression>                   ctNestedExpressions;

    /// Counter of the number of optimization made during program assembly.
    int                                       qtyOptimizations;

    /// Data needed during compilation. An instance of this object is allocated
    /// in the temporary compile-time monotonic allocator.
    struct CompileStorage
    {
        /// The intermediate program listing. Commands are collected here during
        /// compilation. Only when finalized, the result is copied into the vector that
        /// the outer class program inherits from, which uses the non-temporary monotonic
        /// allocator.
        StdVectorMA<VirtualMachine::Command*> Assembly;

        /// Used with compilation. Stores the positions of current result types while adding new
        /// commands.
        StdVectorMA<VM::PC>                   ResultStack;

        /// Compile-time information on conditional operator jump positions.
        struct ConditionalInfo
        {
            #if !DOXYGEN
            ConditionalInfo( VM::PC q, VM::PC t, int f)
            : QJumpPos  (q)
            , TJumpPos  (t)
            , ConstFlags(f)                                                                       {}
            #endif

            VM::PC QJumpPos;     ///< The position of the lhs result.
            VM::PC TJumpPos;     ///< The position of the jump command between T and F.
            int    ConstFlags;   ///< Two bits: Bit 1 determines whether Q was constant and
                                 ///< bit 0 stores the value of Q (if constant)
        };

        /// Stores the positions of current results while adding new commands.
        /// The third value is used for optimizing constant conditionals out.
        StdVectorMA<ConditionalInfo>          ConditionalStack;

        /// Needed during compilation. Collects information from plug-ins to create meaningful
        /// messages.
        ListMA<String>           FunctionsWithNonMatchingArguments;

        /// Constructor.<br>
        /// The given allocator is used exclusively during compilation.
        /// Its memory is cleared (respectively reset to a previous state) after the
        /// compilation completed. The program is created in this allocator. Only when
        /// compilation is finished (and after all optimizations have been performed)
        /// it is copied to the compile-time scope's allocator.<br>
        /// (Refers to object \alib{expressions;Compiler::allocator}.)
        ///
        /// @param compileTimeAllocator The allocator to use temporarily during compilation.
        CompileStorage( MonoAllocator& compileTimeAllocator )
        : Assembly                         (compileTimeAllocator)
        , ResultStack                      (compileTimeAllocator)
        , ConditionalStack                 (compileTimeAllocator)
        , FunctionsWithNonMatchingArguments(compileTimeAllocator) {
            Assembly        .reserve(30);
            ResultStack     .reserve(20);
            ConditionalStack.reserve(5);
        }

    };

    /// Set of data needed during compilation and deleted afterwards. Also, this field indicates
    /// that compilation is "suppressed", which is used when normalized optimized expression
    /// strings are generated from de-compiled programs.
    CompileStorage*                             compileStorage;


  //################################################################################################
  // Constructor/destructor
  //############################################### P ##############################################
  public:
    /// Constructor.
    ///
    /// Prepares the assembly if \p{compileTimeAlloc} is given. If it is \c nullptr, then no
    /// program is assembled. This option is used for creating normalized expression strings from
    /// de-compiled, optimized programs.
    ///
    /// @param pCompiler     Stored in field #compiler.
    /// @param pExpression   Stored in field #expression.
    /// @param ctAlloc       A temporary allocator valid until assembly of the program
    ///                      finished.<br>
    ///                      If \c nullptr, no assembly is performed in later invocations
    ///                      of assemble methods.
    ///                      This option is used for the generation of normalized, optimized
    ///                      expression strings. from reversely created ASTs.
    ALIB_DLL
    Program( Compiler& pCompiler, ExpressionVal& pExpression, MonoAllocator* ctAlloc );

    /// Destructor.
    ALIB_DLL        ~Program();

  //################################################################################################
  // Overrides
  //################################################################################################
    /// Returns the result type of the program.
    /// @return The program's result type.
    ALIB_DLL
    const Box&      ResultType()              const { return commands[commandsCount-1].ResultType; }


    /// Returns the number of \alib{expressions::detail;VirtualMachine::Command} that the
    /// program encompasses.
    /// @return The program's length.
    integer         Length()                                        const { return  commandsCount; }

    /// Returns the command at the given program counter \p{pc}.
    /// @param  pc  The program counter of the command to get.
    /// @return A reference to the requested command.
    VM::Command&    At( VM::PC pc )                                        { return  commands[pc]; }

    /// @return The number of optimizations or \c -1 if optimizations were not activated during
    ///         program assembly.
    int             CtdOptimizations()                            const { return qtyOptimizations; }

    /// Runs the program using the virtual machine.
    ///
    /// @param scope  The evaluation scope.
    /// @return The result value.
    Box             Run(Scope& scope)                  { return VirtualMachine::Run(*this, scope); }


  //################################################################################################
  // Assemble methods
  //################################################################################################

    /// Has to be invoked to finalizes the program after.
    /// No further invocations of assemble methods must be invoked after a call to this method.
    ALIB_DLL
    void        AssembleFinalize();


    /// Add a command that produces a constant value. Used with literals.
    /// @param value            The value to produce.
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression
    ///                         string.
    ALIB_DLL
    void        AssembleConstant( Box& value,
                                  integer idxInOriginal, integer idxInNormalized );

    /// Add a command that invokes a native function.
    /// @param functionName           The name of the function.
    ///                               Used to query the corresponding native C++ callback
    ///                               function from the compiler plug-ins.
    /// @param idxInOriginal          The index of the operator in the expression string.
    /// @param idxInNormalized        The index of the operator in the normalized expression
    ///                               string.
    /// @param isIdentifier           To be \c true, if no parentheses were given. In this case,
    ///                               was given as an 'identifier', what
    ///                               means that no brackets '()' had been added.
    /// @param qtyArgs                The number of function args. If negative, this indicates
    ///                               that the function name was given as an 'identifier', what
    ///                               means that no brackets '()' had been added.
    ALIB_DLL
    void        AssembleFunction( AString& functionName, bool isIdentifier, int qtyArgs,
                                  integer idxInOriginal, integer idxInNormalized );

    /// Add a command that invokes a native function that implements an unary operator.
    /// @param op               The operator to add a command for.
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression string.
    ALIB_DLL
    void        AssembleUnaryOp( String& op, integer idxInOriginal, integer idxInNormalized );

    /// Add a command that invokes a native function that implements a binary operator.
    /// @param op               The operator to add a command for.
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression string.
    ALIB_DLL
    void        AssembleBinaryOp( String& op, integer idxInOriginal, integer idxInNormalized );

    /// To be called after the AST for \c Q was assembled. Adds an "jump on false" statement,
    /// unless it is detected that \c Q is constant. In this case, only one of the subsequent
    /// sub-expressions is assembled.
    ///
    /// Has to be followed by assembly of \c T, followed by
    /// #AssembleCondFinalize_T and furthermore assembly of \c F, followed by
    /// #AssembleCondFinalize_F.
    ///
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression string.
    ALIB_DLL
    void        AssembleCondFinalize_Q( integer idxInOriginal, integer idxInNormalized );

    /// End of ternary \c T expression. Jumps to end of \c f.
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression string.
    ALIB_DLL
    void        AssembleCondFinalize_T( integer idxInOriginal, integer idxInNormalized );

    /// Finalizes a previously started conditional expression.
    /// @param idxInOriginal    The index of the operator in the expression string.
    /// @param idxInNormalized  The index of the operator in the normalized expression string.
    ALIB_DLL
    void        AssembleCondFinalize_F( integer idxInOriginal, integer idxInNormalized );

  //################################################################################################
  // Internals used during compilation
  //################################################################################################
  protected:
    /// Collects \p{qty} types from the result stack and stores them, respectively the constant
    /// values in field \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;af1f402e9cac81ef3ad0a982590344472;Scope::Stack;expressions::Scope::Stack}
    /// stack of field \alib{expressions;ExpressionVal::ctScope}.
    /// @param  qty The number of types to collect.
    /// @return \c true if all arguments collected were constants.
    ALIB_DLL
    bool                collectArgs( integer qty );

};

}}} // namespace [alib::expressions::detail]

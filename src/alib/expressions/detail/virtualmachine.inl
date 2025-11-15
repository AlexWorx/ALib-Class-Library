//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================

ALIB_EXPORT namespace alib::expressions::detail {

//==================================================================================================
/// This type implements a very simple stack machine that understands just four commands (!) plus
/// a fifth to execute sub-programs.
///
/// This class is part of module \alib_expressions_nl. It is not too well documented as it
/// resides in a \c detail namespace and is not to be used from the outside expression API.
//==================================================================================================
struct VirtualMachine : VirtualMachineBase
{
    /// Type definition for a program counter.
    using PC = integer;

    /// A command of the VM.
    class Command
    {
      public:
        /// Some meanings and masks of field #bits.
        enum Bits
        {
            CMD_MASK             = 0x07,  ///< Used to mask the command out of an opcode.
            FlagEOC              = 0x08,  ///< Flags a command to be the last of a conditional.
            CMD_MASK_WITH_EOC    = 0x0F,  ///< Used to mask the command out of an opcode,
                                          ///< including the EOC flag.
            TYPE_MASK            = 0xF0   ///< The mask for the \b ListingTypes.
        };

        /// The opcode type of VM commands.
        enum OpCodes
        {
            Constant    = 0,      ///< Pushes a constant to the stack.
            Function    = 1,      ///< Invokes a C++ callback function.
            Subroutine  = 2,      ///< Invokes another program.

            // leaving out one, so that jumps have bit 3 set.
            JumpIfFalse = 4,      ///< Jumps if top of the stack indicates \c false.
            Jump        = 5,      ///< Jumps.
        };

        /// Denotes the type of parsing de-compilation information attached to the command.
        enum ListingTypes
        {
            LiteralConstant     = 0x00,  ///< Command results from a literal constant.
            OptimizationConstant= 0x10,  ///< Command results from a constant resulting from an optimization.
            UnaryOp             = 0x20,  ///< Command results from an unary operator.
            BinaryOp            = 0x30,  ///< Command results from a binary operator.
            Identifier          = 0x40,  ///< Command results from a function call with no parenthesis given.
            FunctionCall        = 0x50,  ///< Command results from a function call.
            AutoCast            = 0x60,  ///< Command results from an automatically inserted cast.
            NestedExpression    = 0x70,  ///< Command results from a nested expression.
        };

        /// Denotes the two different jump types (used only as parameter, not stored.)
        enum class JumpType
        {
            Conditional,   ///< Denotes \b %JumpIfFalse.
            Unconditional, ///< Denotes \b %Jump.
        };


        #if ALIB_DEBUG
        /// Provides additional debug information for a command.
        /// Available only with debug-builds of the library.
        /// Used with \alib{expressions::detail;VirtualMachine::DbgList}
        struct DbgInformation
        {
            /// The plug-in that provided the callback or constant.
            CompilerPlugin* Plugin                                                         =nullptr;

            /// The native C++ name of the callback function.
            const char*     Callback;
        };
        #endif

        /// A union of different parameter types for the commands.
        union OperationParam
        {
            PC              Distance;      ///< A distance to jump
            CallbackDecl    Callback;      ///< A C++ callback function to invoke.
            Program*        NestedProgram; ///< The nested virtual machine program to invoke.

            /// Default constructor leaving this instance uninitialized.
            OperationParam()                                                                      {}

            /// Union constructor.
            /// @param distance Stored in #Distance.
            OperationParam( integer       distance ) : Distance(distance)                         {}

            /// Union constructor.
            /// @param callback  Stored in #Callback.
            OperationParam( CallbackDecl  callback ) : Callback(callback)                         {}

            /// Union constructor.
            /// @param program Stored in #NestedProgram.
            OperationParam ( Program*     program)   : NestedProgram(program)                     {}
        };

      protected:
        /// Operation code of this command.
        int16_t             bits;

        /// The number of function args.
        /// If negative, this indicates that the function name was given as an 'identifier',
        /// what means that no brackets '()' had been added. This piece of information (namely
        /// having a signed integer here which allows differentiating between \c 0 and a negative
        /// number of arguments) is used when decompiling a program back to an expression
        /// string. Hence, this piece of it logically belongs to #ListingTypes.
        uint16_t            qtyArgs;

      public:
        /// The parameter of the operation.
        OperationParam      Parameter;

        /// With every command, this box contains the result type.
        /// For constants, also the commands' value is contained.
        Box                 ResultType;

        /// This encodes both, the position in the original and in the normalized expression
        /// string that resulted in this command.
        /// Used for generation of exception information and debug listings.
        uinteger            ExpressionPositions;


        /// The operator symbol or function name used with de-compilation to recreate an
        /// expression string.
        String              DecompileSymbol;


        #if ALIB_DEBUG
        /// Operation code of this command. Available only with debug-builds.
        DbgInformation  DbgInfo;
        #endif

        /// Constructor creating a constant.
        /// @param value           The constant to add.
        /// @param isOptimization  Denotes if the constant results from an optimization or
        ///                        from a literal of the expression string.
        /// @param idxOriginal     Expression string index that imposed command.
        /// @param idxNormalized   Normalized expression string index that imposed command.
        Command( const Box& value, bool isOptimization,
                 integer idxOriginal, integer idxNormalized )
        : bits      (   int16_t(OpCodes::Constant)
                      | ( isOptimization ? int16_t(ListingTypes::OptimizationConstant)
                                         : int16_t(ListingTypes::LiteralConstant     ) ) )
        , ResultType(value)
        , ExpressionPositions( (uinteger(idxNormalized) << (bitsof(integer)/2) )
                              + uinteger(idxOriginal  )  )                                        {}


        /// Constructor creating a native function call exposed from an identifier, function
        /// or operator.
        /// @param callback          The callback function.
        /// @param isIdentifier      If no parentheses were given, this has to be \c true.
        ///                          Otherwise it is a function call.
        /// @param qtyFunctionArgs   The number of arguments that the callback function expects.
        /// @param resultType        The result type of the function.
        /// @param functionOrOp      The identifier, function or operator name/symbol.
        ///                          This is used for de-compilation of programs.
        /// @param isOperator        If \c true an unary operator was compiled, a binary operator
        ///                          otherwise.
        /// @param idxOriginal       Expression string index that imposed command.
        /// @param idxNormalized     Normalized expression string index that imposed command.
        Command( CallbackDecl   callback, bool isIdentifier, int qtyFunctionArgs,
                 const Box&     resultType,
                 const String&  functionOrOp, bool isOperator,
                 integer idxOriginal, integer idxNormalized                            )
        : bits                 (    int16_t(OpCodes::Function)
                                 | ( isOperator ? ( qtyFunctionArgs == 1 ? int16_t(ListingTypes::UnaryOp )
                                                                         : int16_t(ListingTypes::BinaryOp) )
                                                : ( isIdentifier         ? int16_t(ListingTypes::Identifier )
                                                                         : int16_t(ListingTypes::FunctionCall) ) ) )
        , qtyArgs            (uint16_t(qtyFunctionArgs)  )
        , Parameter          (callback         )
        , ResultType         (resultType       )
        , ExpressionPositions( (uinteger(idxNormalized) << (bitsof(integer)/2) )
                              + uinteger(idxOriginal  )  )
        , DecompileSymbol    (functionOrOp     )                                                  {}

        /// Constructor creating a subroutine call.
        /// @param program         The program to call.
        /// @param resultType      The result type of the program.
        /// @param functionOrOp    The function or operator that created the nested call.
        /// @param idxOriginal     Expression string index that imposed command.
        /// @param idxNormalized   Normalized expression string index that imposed command.
        ALIB_DLL
        Command( Program* program, const Box& resultType, const String& functionOrOp,
                 integer idxOriginal, integer idxNormalized          );

        /// Constructor creating a jump. Note, the address is usually not known yet, hence not
        /// provided.
        /// @param jumpType        The type of jump.
        /// @param idxOriginal     Expression string index that imposed command.
        /// @param idxNormalized   Normalized expression string index that imposed command.
        Command( integer idxOriginal, integer idxNormalized, JumpType jumpType )
        : bits      (int16_t(jumpType == JumpType::Conditional ? OpCodes::JumpIfFalse : OpCodes::Jump ))
        , Parameter (-1)
        , ResultType(nullptr)
        , ExpressionPositions( (uinteger(idxNormalized) << (bitsof(integer)/2) )
                              | uinteger(idxOriginal  )  )                                        {}


        /// Returns the opcode of this command.
        /// @return The masked command part of the opcode.
        constexpr OpCodes OpCode()                  const { return OpCodes(bits & Bits::CMD_MASK); }

        /// Returns the decompile type of this command.
        /// @return The masked command part of the opcode.
        constexpr ListingTypes TerminalType()   const {return ListingTypes(bits & Bits::TYPE_MASK);}

        /// Returns \c true if the command represents a constant value, but is not the end
        /// of a conditional jump.
        /// @return \c true if the command represents a constant value, \c false otherwise.
        bool IsConstant()                                                                      const
        { return   OpCodes(bits & Bits::CMD_MASK_WITH_EOC) == OpCodes::Constant; }

        /// Returns \c true if the command represents a conditional or unconditional jump.
        /// @return \c true if the command represents a jump, \c false otherwise.
        bool IsJump()                                              const { return (bits & 4) == 4; }

        /// Marks the command as the end of a conditional term.
        void SetEndOfConditionalFlag()                                                 { bits|= 8; }

        /// @return \c true if the function has arguments, \c false otherwise.
        bool HasArgs()                                                const { return qtyArgs <= 0; }

        /// @return \c true if the function has arguments, \c false otherwise.
        bool IsIdentifier()                                           const { return qtyArgs <= 0; }

        /// @return The number of arguments of a function call.
        int QtyArgs()                                                 const { return int(qtyArgs); }
    }; // inner struct Command

    /// Static method that runs an expression program.
    /// @param program  The program to run.
    /// @param scope    The scope to use.
    /// @return The result value of the expression program.
    ALIB_DLL static
    alib::Box  Run( Program& program, Scope& scope );

    /// Static method that decompiles a program into an abstract syntax tree.
    /// Used to generate optimized, normalized, parsable expression strings.
    /// @param program   The program to decompile.
    /// @param allocator Allocator for AST objects (and their data).
    /// @return The abstract syntax tree as a result of de-compilation.
    ALIB_DLL static
    AST*        Decompile( Program& program, MonoAllocator& allocator );


    #if ALIB_DEBUG
    /// Lists a virtual machine program.
    ///
    /// Note: This method is available only with debug-builds of the library.
    /// @param program  The program to list.
    /// @return The program listing.
    ALIB_DLL static
    AString DbgList( Program& program );

    #endif

    /// The implementation of #Run, which itself is just initialization code.
    /// @param program      The program to run.
    /// @param scope        The scope to use.
    ALIB_DLL static
    void  run( Program& program, Scope& scope );
    
}; // struct VirtualMachine

} // namespace [alib::expressions::detail]

ALIB_BOXING_VTABLE_DECLARE(  alib::expressions::detail::VirtualMachine::Command::OpCodes, vt_expressions_vmopcodes )
#if ALIB_DEBUG
    ALIB_ENUMS_ASSIGN_RECORD(alib::expressions::detail::VirtualMachine::Command::OpCodes, ERSerializable )
#endif

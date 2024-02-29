/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE
#define HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE

#if !defined (HPP_ALIB_EXPRESSIONS_EXPRESSION)
#   include "alib/expressions/expression.hpp"
#endif

#ifndef HPP_ALIB_ENUMS_UNDERLYING_INTEGRAL
#   include "alib/enums/underlyingintegral.hpp"
#endif



namespace alib {  namespace expressions {

struct CompilerPlugin;

namespace detail {

class   Program;
struct  AST;

/** ************************************************************************************************
 * This class implements a very simple stack machine that understands just four commands (!) plus
 * a fifth to execute sub-programs.
 *
 * This class is part of module \alib_expressions_nl. It is not too well documented as it
 * resides in a \c detail namespace and is not to be used from the outside expression API.
 **************************************************************************************************/
class VirtualMachine
{
    public:
        /// Type definition for a program counter.
        using PC = integer;

        /**
         * A command of the VM.
         */
        class Command
        {
            public:
                /**
                 * The opcode type of VM commands. This is defined as a
                 * \alib{enums,T_EnumIsBitwise,bitwise enum type} as also flags may be set in
                 * addition to defining the type of operation.
                 */
                enum class OpCodes
                {
                    Constant    = 1,              ///< Pushes a constant to the stack.
                    Function    = 2,              ///< Invokes a C++ callback function.
                    JumpIfFalse = 3,              ///< Jumps if top of the stack indicates \c false.
                    Jump        = 4,              ///< Jumps.
                    Subroutine  = 5,              ///< Invokes another program.

                    CmdMask     = (1 << 16) - 1,  ///< Used to mask the command out of an opcode.

                    FlagEOC     = (1 << 17),      ///< Flags a command to be the last of a conditional.
                };

                /**
                 * Denotes the two different jump types (used as constructor parameter only.)
                 */
                enum class JumpType
                {
                    Conditional,   ///< Denotes \b %JumpIfFalse.
                    Unconditional, ///< Denotes \b %Jump.
                };


                /**
                 * Denotes the type of decompilation information attached to the command.
                 */
                enum class DecompileInfoType
                {
                    LiteralConstant,       ///< Command results from a literal constant.
                    OptimizationConstant,  ///< Command results from a constant resulting from an optimization.
                    UnaryOp,               ///< Command results from an unary operator.
                    BinaryOp,              ///< Command results from a binary operator.
                    Function,              ///< Command results from a function call.
                    AutoCast,              ///< Command results from an automatically inserted cast.
                    Subroutine,            ///< Command results from a nested expression.
                };

                #if ALIB_DEBUG
                    /**
                     * Provides additional debug information for a command.
                     * Available only with debug builds of the module \alib_expressions_nl
                     * library. Used with \alib{expressions::detail,VirtualMachine::DbgList}
                     */
                    struct DbgInformation
                    {
                        /// The plug-in that provided the callback or constant.
                        CompilerPlugin* Plugin                                            = nullptr;

                        /// The native C++ name of the callback function.
                        const char*     Callback;
                    };
                #endif

            /**
             * A union of different parameter types for the commands.
             */
            union OperationParam
            {
                PC              Distance;      ///< A distance to jump
                CallbackDecl    Callback;      ///< A C++ callback function to invoke.
                Box             Value;         ///< A constant value to push to the stack.
                Program*        NestedProgram; ///< The nested virtual machine program to invoke.

                /**
                 * Union constructor.
                 * @param distance Stored in #Distance.
                 */
                OperationParam( integer          distance     )
                : Distance    (distance    )
                {}

                /**
                 * Union constructor.
                 * @param callback  Stored in #Callback.
                 */
                OperationParam( CallbackDecl  callback )
                : Callback(callback)
                {}

                /**
                 * Union constructor.
                 * @param value Stored in #Value.
                 */
                OperationParam( Box              value        )
                : Value       (value       )
                {}

                /**
                 * Union constructor.
                 * @param program Stored in #NestedProgram.
                 */
                OperationParam ( Program*     program      )
                : NestedProgram( program     )
                {}
            };

            protected:
                /** Operation code of this command. */
                OpCodes             opcode;

            public:
                /** The parameter of the operation. */
                OperationParam      Operation;

            /**
             * The number of function args.
             * If negative, this indicates that the function name was given as an 'identifier',
             * what means that no brackets '()' had been added. This piece of information (namely
             * having a signed integer here which allows to differentiate between \c 0 and a negative
             * number of arguments) is used when decompiling a program to back to an expression
             * string. Hence, this piece of it logically belongs to #DecompileType.
             */
            integer                 QtyFunctionArgs;

            /**
             * The result type of a command. This is only needed during compilation.
             * With debug builds of the library, it is furthermore used with
             * \alib{expressions::detail,VirtualMachine::DbgList}. Hence for simplicity and
             * shortness of the library code we keep it here, taking into account that after
             * compilation each command is equipped with a then unnecessary box.
             */
            Box                     ResultType;

            /**
             * This encodes both, the position in the original and normalized expression string
             * that resulted in this command. Used for generation of exception information and
             * and debug listings.
             */
            uinteger                ExpressionPositions;


            /** Type of expression term used with decompilation. */
            DecompileInfoType       DecompileType;

            /** The operator symbol or function name used with decompilation to recreate an
             *  expression string. */
            String                  DecompileSymbol;


            #if ALIB_DEBUG
                /** Operation code of this command. Available only with debug builds. */
                DbgInformation      DbgInfo;
            #endif

            /**
             * Constructor creating a constant.
             * @param value           The constant to add.
             * @param isOptimization  Denotes if the constant results from an optimization or
             *                        from a literal of the expression string.
             * @param idxOriginal     Expression string index that imposed command.
             * @param idxNormalized   Normalized expression string index that imposed command.
             */
            Command( const Box& value, bool isOptimization,
                     integer idxOriginal, integer idxNormalized )
            : opcode(OpCodes::Constant)
            , Operation (value)
            , ResultType(value)
            , ExpressionPositions( (static_cast<uinteger>(idxNormalized) << (bitsof(integer)/2) )
                                  + static_cast<uinteger>(idxOriginal  )  )
            , DecompileType( isOptimization ? DecompileInfoType::OptimizationConstant
                                            : DecompileInfoType::LiteralConstant       )
            {}


            /**
             * Constructor creating a native function call exposed from an identifier, function
             * or operator.
             * @param callback          The callback function.
             * @param qtyFunctionArgs   The number of arguments that the callback function expects.
             * @param resultType        The result type of the function.
             * @param functionOrOp      The identifier, function or operator name/symbol.
             *                          This is used for decompilation of programs.
             * @param isOperator        If \c true an unary operator was compiled, a binary operator
             *                          otherwise.
             * @param idxOriginal       Expression string index that imposed command.
             * @param idxNormalized     Normalized expression string index that imposed command.
             */
            Command( CallbackDecl callback, integer qtyFunctionArgs, const Box& resultType,
                     const String& functionOrOp, bool isOperator,
                     integer idxOriginal, integer idxNormalized                            )
            : opcode               (OpCodes::Function)
            , Operation            (callback         )
            , QtyFunctionArgs      (qtyFunctionArgs  )
            , ResultType           (resultType       )
            , ExpressionPositions( (static_cast<uinteger>(idxNormalized) << (bitsof(integer)/2) )
                                  + static_cast<uinteger>(idxOriginal  )  )
            , DecompileType        ( isOperator ? ( qtyFunctionArgs == 1 ? DecompileInfoType::UnaryOp
                                                                         : DecompileInfoType::BinaryOp )
                                                : DecompileInfoType::Function )
            , DecompileSymbol      (functionOrOp     )
            {}

            /**
             * Constructor creating a subroutine call.
             * @param program         The program to call.
             * @param resultType      The result type of the program.
             * @param functionOrOp    The function or operator that created the nested call.
             * @param idxOriginal     Expression string index that imposed command.
             * @param idxNormalized   Normalized expression string index that imposed command.
             */
            Command( Program* program, const Box& resultType, const String& functionOrOp,
                     integer idxOriginal, integer idxNormalized          );

            /**
             * Constructor creating a jump. Note, the address is usually not known yet, hence not
             * provided. With debug builds, this constructor asserts that it is used for jump
             * commands only.
             * @param jumpType        The type of jump.
             * @param idxOriginal     Expression string index that imposed command.
             * @param idxNormalized   Normalized expression string index that imposed command.
             */
            Command( JumpType jumpType, integer idxOriginal, integer idxNormalized )
            : opcode(jumpType == JumpType::Conditional ? OpCodes::JumpIfFalse : OpCodes::Jump )
            , Operation(-1)
            , ResultType(nullptr)
            , ExpressionPositions( (static_cast<uinteger>(idxNormalized) << (bitsof(integer)/2) )
                                  + static_cast<uinteger>(idxOriginal  )  )
            {}


            /**
             * Returns \c true if the command represents a constant value.
             * @return  \c true if the command represents a constant value, \c false otherwise.
             */
            bool IsConstant()                                                                  const
            {
                // this excludes OpCodes::Constant with FlagEOC set.
                return  opcode == OpCodes::Constant;
            }

            /**
             * Returns \c true if the command represents a jump.
             * @return  \c true if the command represents a jump, \c false otherwise.
             */
            bool IsConditionalJump()                                                           const
            {
                return     opcode == OpCodes::JumpIfFalse
                        || opcode == OpCodes::Jump         ;
            }

            /**
             * Marks the command as the end of an conditional term.
             */
            void SetEndOfConditionalFlag()
            {
                opcode = OpCodes( UnderlyingIntegral(opcode) | UnderlyingIntegral(OpCodes::FlagEOC) );
            }

            /**
             * Returns the masked command part of the opcode.
             * @return  The masked command part of the opcode.
             */
            OpCodes  Which()                                                                   const
            {
                return OpCodes( UnderlyingIntegral(opcode) & UnderlyingIntegral(OpCodes::CmdMask) );
            }
        }; // inner struct Command


    /** ********************************************************************************************
     * Static method that runs an expression program.
     * @param program  The program to run.
     * @param scope    The scope to use.
     * @return The result value of the expression program.
     **********************************************************************************************/
    ALIB_API static
    alib::Box  Run( Program& program, Scope& scope );

    /** ********************************************************************************************
     * Static method that decompiles a program into an abstract syntax tree.
     * Used to generate optimized, normalized, parsable expression strings.
     * @param program   The program to decompile.
     * @param allocator Allocator for AST objects (and their data).
     * @return The abstract syntax tree as a result of decompilation.
     **********************************************************************************************/
    ALIB_API static
    AST*        Decompile( Program& program, MonoAllocator& allocator );


    #if ALIB_DEBUG
        /** ****************************************************************************************
         * Lists a virtual machine program.
         *
         * Note: This method is available only with debug builds of the library.
         * @param program  The program to list.
         * @return The program listing.
         ******************************************************************************************/
        ALIB_API static
        AString DbgList( Program& program );

    #endif

    /** ********************************************************************************************
     * The implementation of #Run, which itself is just initialization code
     * @param program      The program to run.
     * @param scope        The scope to use.
     **********************************************************************************************/
    ALIB_API static
    void  run( Program& program, Scope& scope );


};

}}} // namespace [alib::expressions::detail]

ALIB_BOXING_VTABLE_DECLARE(       alib::expressions::detail::VirtualMachine::Command::OpCodes, vt_expressions_vmopcodes )
ALIB_ENUMS_MAKE_BITWISE(             alib::expressions::detail::VirtualMachine::Command::OpCodes)
#if ALIB_DEBUG
    ALIB_ENUMS_ASSIGN_RECORD(alib::expressions::detail::VirtualMachine::Command::OpCodes, ERSerializable )
#endif

#endif // HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE

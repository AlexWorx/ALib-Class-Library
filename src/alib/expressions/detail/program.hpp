// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM
#define HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM

#ifndef HPP_ALIB_EXPRESSIONS_DETAIL_VIRTUAL_MACHINE
#   include "virtualmachine.hpp"
#endif

namespace aworx { namespace lib { namespace expressions {

/**
 * This is the detail namespace of #aworx::lib::expressions. Among other things, in this namespace,
 * [boost::spirit](http://www.boost.org/doc/libs/1_66_0/libs/spirit/doc/html/index.html)
 * is used to implement the parsing of \alib{expressions::Expression} trees.
 *
 * Good care is taken to truly separate code that is using \c boost::spirit and that no
 * \alib public header files include this library, because compile times are increasing
 * tremendously due to the use of excessive TMP.
 */
namespace detail {


/** ************************************************************************************************
 * This class represents a program that is "run on" the \alib{expressions,detail::VirtualMachine}
 * to evaluate an expression.
 **************************************************************************************************/
class Program : protected std::vector<VirtualMachine::Command>
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /// The compiler that created this object.
        Compiler&                   compiler;


    // #############################################################################################
    // Internal fields
    // #############################################################################################
    protected:
        /// Shortcut.
        using VM= VirtualMachine;

        /// Machine has full access.
        friend class VirtualMachine;

        /// The expression that this program evaluates.
        Expression&                 expression;

        /**
         * Compile-time scope object. Used to allocate constant program object copies.
         * Also passed to the compiler plug-ins for their use.
         */
        Scope*                      ctScope;

        /**
         * List of compile-time identified nested expressions. Shared pointers are stored here
         * so that the expressions do not get deleted until this program is.
         */
        std::vector<SPExpression>   ctNestedExpressions;

        /// Counter of the number of optimization made during program assembly.
        int                         qtyOptimizations;

        /**
         * Data needed during compilation.
         */
        struct CompileStorage
        {
            /**
             * Constructor.
             * @param pPlugins Stored in field #plugins.
             */
            CompileStorage( Compiler::Plugins&  pPlugins )
            : plugins( pPlugins )
            {}


            /// The compiler plug-ins providing the native C++ implementations of callback functions.
            Compiler::Plugins&      plugins;

            /// Used with compilation. Stores the positions of current results types adding new commands.
            std::vector<VM::PC>     resultStack;

            /**
             * Used with compilation. Stores the positions of current results while adding new commands.
             * The third value is used for optimizing constant conditionals out.
             */
            std::stack<std::tuple<VM::PC,VM::PC,int>>   conditionalStack;

            /**
             * Needed during compilation. Collects information from plug-ins to create meaningful
             * exceptions.
             */
            std::vector<AString>    functionsWithNonMatchingArguments;
        };

        /**
         * Set of data needed during compilation and deleted afterwards. Also, this field indicates
         * that compilation is "suppressed", which is used when normalized optimized expression
         * strings are generated from de-compiled programs.
         */
        CompileStorage*             compileStorage;


    // #############################################################################################
    // Constructor/destructor
    // ##################################################P###########################################
    public:
        /** ****************************************************************************************
         * Constructor.
         *
         * Prepares the assembly if \p{pCTScope} is given. If it is \c nullptr, then no program is
         * assembled. This option is used for creating normalized expression strings from
         * de-compiled, optimized programs.
         * @param pCompiler    Stored in field #compiler.
         * @param pPlugins     Stored in field
         *                     \alib{expressions::detail::Program,CompileStorage::plugins}.
         * @param pExpression  Stored in field #expression.
         * @param pCTScope     Stored in field #ctScope.
         *                     If \c nullptr, no assembly is performed in subsequent invocations
         *                     of assemble methods.
         ******************************************************************************************/
        Program(Compiler&  pCompiler, Compiler::Plugins& pPlugins, Expression& pExpression, Scope* pCTScope)
        : compiler   ( pCompiler   )
        , expression ( pExpression )
        , ctScope    ( pCTScope    )
        , qtyOptimizations( EnumContains( compiler.CfgCompilation, Compilation::NoOptimization )
                            ? -1 : 0 )
        , compileStorage( pCTScope ? new CompileStorage(pPlugins) : nullptr )
        {}


        /** ****************************************************************************************
         * Destructor.
         ******************************************************************************************/
                        ~Program();

    // #############################################################################################
    // Overrides
    // #############################################################################################
        /** ****************************************************************************************
         * Returns the result type of the program.
         * @return The program's result type.
         ******************************************************************************************/
        const Box&      ResultType()                                                          const;

        /** ****************************************************************************************
         * Returns the number of \alib{expressions::detail,VirtualMachine::Command} that the
         * program encompasses.
         * @return The program's length.
         ******************************************************************************************/
        inline integer  Length()                                                               const
        {
            return  static_cast<integer>( size() );
        }

        /** ****************************************************************************************
         * Implementation of abstract interface method.
         *
         * @return The number of optimizations or \c -1 if optimizations were not activated during
         *         program assembly.
         ******************************************************************************************/
        inline int      CountOptimizations()                                                   const
        {
            return qtyOptimizations;
        }

        /** ****************************************************************************************
         * Runs the program using the virtual machine.
         *
         * @param scope  The evaluation scope.
         * @return The result value.
         ******************************************************************************************/
        inline Box      Run(Scope& scope)
        {
            return VirtualMachine::Run(*this, scope);
        }


    // #############################################################################################
    // Assemble methods
    // #############################################################################################

        /** ****************************************************************************************
         * Has to be invoked to finalizes the program after.
         * No further invocations of assemble methods must be invoked after a call to this methods.
         ******************************************************************************************/
        ALIB_API
        void        AssembleFinalize();


        /** ****************************************************************************************
         * Add a command that produces a constant value. Used with literals.
         * @param value            The value to produce.
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression
         *                         string.
         ******************************************************************************************/
        void        AssembleConstant( Box& value,
                                      integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * Add a command that invokes a native function.
         * @param functionName           The name of the function.
         *                               Used to query the corresponding native C++ callback
         *                               function from the compiler plug-ins.
         * @param idxInOriginal          The index of the operator in the expression string.
         * @param idxInNormalized        The index of the operator in the normalized expression
         *                               string.
         * @param qtyArgsOrNoParentheses The number of function args. If negative, this indicates
         *                               that the function name was given as an 'identifier', what
         *                               means that no brackets '()' had been added.
         ******************************************************************************************/
        ALIB_API
        void        AssembleFunction( AString& functionName, integer qtyArgsOrNoParentheses,
                                      integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * Add a command that invokes a native function that implements an unary operator.
         * @param op               The operator to to add a command for.
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression string.
         ******************************************************************************************/
        ALIB_API
        void        AssembleUnaryOp( String& op, integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * Add a command that invokes a native function that implements a binary operator.
         * @param op               The operator to to add a command for.
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression string.
         ******************************************************************************************/
        ALIB_API
        void        AssembleBinaryOp( String& op, integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * To be called after the AST for \c Q was assembled. Adds an "jump on false" statement,
         * unless it is detected that \c Q is constant. In this case, only one of the subsequent
         * sub-expressions is assembled.
         *
         * Has to be followed by assembly of \c T, followed by
         * #AssembleCondFinalize_T and furthermore assembly of \c F, followed by
         * #AssembleCondFinalize_F.
         *
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression string.
         ******************************************************************************************/
        ALIB_API
        void        AssembleCondFinalize_Q( integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * End of ternary \c T expression. Jumps to end of \c f.
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression string.
         ******************************************************************************************/
        ALIB_API
        void        AssembleCondFinalize_T( integer idxInOriginal, integer idxInNormalized );

        /** ****************************************************************************************
         * Finalizes a previously started conditional expression.
         * @param idxInOriginal    The index of the operator in the expression string.
         * @param idxInNormalized  The index of the operator in the normalized expression string.
         ******************************************************************************************/
        ALIB_API
        void        AssembleCondFinalize_F( integer idxInOriginal, integer idxInNormalized );


    // #############################################################################################
    // Internals
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Returns the command at the given program counter \p{pc}.
         * @param  pc  The program counter of the command to get.
         * @return A reference to the requested command.
         ******************************************************************************************/
        inline        VM::Command&  At( VM::PC pc )
        {
            return  (*this)[ static_cast<size_t>( pc ) ];
        }

        /** ****************************************************************************************
         * Returns the command at the given program counter \p{pc}.
         * @param  pc  The program counter of the command to get.
         * @return A reference to the requested command.
         ******************************************************************************************/
        inline const VM::Command&   AtConst( VM::PC pc )                                       const
        {
            return  (*this)[ static_cast<size_t>( pc ) ];
        }


        /** ****************************************************************************************
         * Collects \p{qty} types from the result stack and stores them, respectively the constant
         * values in the stack of #ctScope.
         * @param  qty The number of types to collect.
         * @return \c true if all arguments collected were constants.
         ******************************************************************************************/
        bool                        collectArgs( integer qty );
};

}}}}; // namespace [aworx::lib::expressions::detail]


#endif // HPP_ALIB_EXPRESSIONS_DETAIL_PROGRAM

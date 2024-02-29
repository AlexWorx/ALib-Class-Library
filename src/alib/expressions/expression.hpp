/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_expressions of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSION
#define HPP_ALIB_EXPRESSIONS_EXPRESSION

#ifndef HPP_ALIB_EXPRESSIONS_EXPRESSIONS
#   include "alib/expressions/expressions.hpp"
#endif

#if ALIB_TIME && ALIB_DEBUG && !defined (HPP_ALIB_TIME_TICKS)
#  include "alib/time/ticks.hpp"
#endif

#if !defined (_GLIBCXX_MEMORY) && !defined(_MEMORY_)
#   include <memory>
#endif

namespace alib {  namespace expressions {

// forwards
class  Compiler;
struct Scope;
namespace detail{ class Program; class VirtualMachine; }

/** ************************************************************************************************
 * This is a central class of library module \alib_expressions_nl representing compiled,
 * evaluatable expressions.
 *
 * The constructor is protected. Expressions are created using method
 * \alib{expressions,Compiler::Compile}. The result of this method is a shared pointer
 * \alib{expressions,SPExpression} which frees the user from the obligation to manage the life-cylce
 * of \alib expressions.
 *
 * For information about general use and features of this class consult the
 * \ref alib::expressions "ALib Expressions User Manual".
 *
 * ## Friends ##
 * class \alib{expressions,Compiler}
 * class \alib{expressions,detail::Program}
 **************************************************************************************************/
class Expression
{
    #if !defined(ALIB_DOX)
        // The compiler builds this this type which by design does not expose a constructor.
        friend class Compiler;

        // The program that evaluates us.
        friend class detail::Program;

        // The program that evaluates us.
        friend class detail::VirtualMachine;
    #endif

    protected:
        /**
         * Compile-time scope object. Used to allocate constant program object copies.
         * Also passed to the compiler plug-ins for their use.
         */
        Scope*              ctScope;

        /** The name of the expression (if named, otherwise resourced, usually "ANONYMOUS" ). */
        String              name;

        /** The compiled expression program. */
        detail::Program*    program;

        /** The original source string of the expression. */
        String              originalString;

        /** The normalized string as a result of compilation. */
        AString             normalizedString;

        /** The normalized string generated on request out of optimized expression program. */
        AString             optimizedString;

    public:
    #if ALIB_TIME && ALIB_DEBUG
        /**
         * Provides the time needed to parse the expression into an abstract syntax tree.
         *
         * Note: This field is available only with debug builds of the library.
         */
        Ticks::Duration     DbgParseTime;

        /**
         * Provides the time needed to parse the expression into an abstract syntax tree.
         *
         * Note: This field is available only with debug builds of the library.
         */
        Ticks::Duration     DbgAssemblyTime;

        /**
         * Provides the time needed for the last evaluation of the expression.
         *
         * Note: This field is available only with debug builds of the library.
         */
        Ticks::Duration     DbgLastEvaluationTime;

    #endif

    protected:
        /** ****************************************************************************************
         * Protected constructor to disallow creation.
         * Expressions are created using \alib{expressions,Compiler::Compile}.
         * @param sourceString The original string that was parsed.
         * @param pCTScope     The compile-time scope.
         ******************************************************************************************/
        ALIB_API            Expression( const String& sourceString, Scope* pCTScope );

    public:
        /** ****************************************************************************************
         * Destructor.
         ******************************************************************************************/
        ALIB_API           ~Expression();

        /** ****************************************************************************************
         * The name of the expression. A name is only available if the expression was created with
         * \alib{expressions,Compiler::AddNamed}. This might be 'automatically' done when nested
         * expressions get compiled and the compiler supports retrieval of expression strings by
         * name from some custom location (or built-in \alib configuration mechanics).
         *
         * Otherwise, the name is \b "ANONYMOUS", which is a resourced string of key
         * \c "ANON_EXPR_NAME".
         *
         * @return The expression's name.
         ******************************************************************************************/
        ALIB_API
        String     Name();

        /** ****************************************************************************************
         * Evaluates the expression by executing the compiled \p{program}.
         *
         * @return The result of this evaluation of this expression node.
         ******************************************************************************************/
        ALIB_API Box        ResultType();

        /** ****************************************************************************************
         * Evaluates the expression by executing the compiled \p{program}.
         *
         * With debug builds of this library, \alib assertions may be raised.
         * Usually this indicates that a native callback function returned a value of erroneous
         * type, which usually are caused by erroneous compiler plug-ins, respectively the native
         * callback functions that those provide.
         *
         * The assertion will most probably give detailed information.
         *
         * @param scope  The evaluation scope.
         * @return The result of this evaluation of this expression node.
         ******************************************************************************************/
        ALIB_API Box        Evaluate(Scope& scope);


        /** ****************************************************************************************
         * Returns the originally given expression string.
         *
         * @return The original expression string.
         ******************************************************************************************/
        String              GetOriginalString()     const
        {
            return originalString;
        }

        /** ****************************************************************************************
         * Returns a normalized version of the original expression string.
         *
         * The result of normalization can be tweaked with the flags in field configuration field
         * \alib{expressions,Compiler::CfgNormalization}. In any case, unnecessary (multiple)
         * whitespaces and brackets are removed. Consult the documentation of enumeration
         * \alib{expressions,Normalization} for details of the options.
         *
         * It is guaranteed that the normalized version of the expression string is parsable and
         * leads to the identical evaluation program as the original expression string.
         *
         * Software might choose to write back normalized expressions, for example into
         * configuration files.
         *
         * \note
         *   This method does not perform the normalization, but returns a normalized version of the
         *   parsed expression string, which was created with the compilation of the expression.
         *   A normalized string is always created.
         *
         * @return The normalized expression string.
         ******************************************************************************************/
        String              GetNormalizedString()    const
        {
            return normalizedString;
        }

        /** ****************************************************************************************
         * Returns a normalized expression string reflecting an optimized version of this
         * expression.
         * The number of optimizations performed during compilation of the expression can be
         * received by invoking
         * \alib{expressions,Program::CountOptimizations} on the program returned by
         * #GetProgram. If this is \c 0, then the expression string returned here matches the
         * normalized expression string received with #GetNormalizedString.
         *
         * \note
         *   On the first invocation, the string is generated once. For this, an abstract syntax
         *   tree is created by decompiling the optimized program. This in turn is assembled
         *   back to a program (by omitting the generation of commands and without invoking
         *   on compiler plug-ins, etc.) which generates the normalized expression string from the
         *   AST.
         *
         * @return The expression string requested.
         ******************************************************************************************/
        ALIB_API
        String              GetOptimizedString();


        /** ****************************************************************************************
         * Returns the program that evaluates the expression.
         *
         * @return The result of this evaluation of this expression node.
         ******************************************************************************************/
        detail::Program*    GetProgram()
        {
            return program;
        }
};


/**
 * As expressions are usually named and cached to enable nested expressions, but also shared
 * as root expressions, and often encapsulated in more or less volatile custom objects, this shared
 * pointer type is used to pass expression trees around. This manages their life-cycle
 * automatically.
 */
using SPExpression=    std::shared_ptr<Expression>;

} // namespace alib[::expressions]

/// Type alias in namespace \b alib.
using     SPExpression=    expressions::SPExpression;

} // namespace [alib]


namespace alib::strings {
#if defined(ALIB_DOX)
namespace APPENDABLES {
#endif
    /** ********************************************************************************************
     * Specialization of functor \alib{strings,T_Append} for type
     * \alib{expressions,Expression}.
     **********************************************************************************************/
    template<> struct T_Append<expressions::Expression,alib::character>
    {
        /**
         * Appends the result of \alib{expressions,Expression::GetNormalizedString} to the
         * \p{target}.
         *
         * @param target The \b AString that method \b Append was invoked on.
         * @param src    The expression to append.
         */
        void operator()( AString& target, const expressions::Expression& src )
        {
           target << src.GetNormalizedString();
        }
    };
#if defined(ALIB_DOX)
}
#endif
} // namespace [alib::strings]


#endif // HPP_ALIB_EXPRESSIONS_EXPRESSION

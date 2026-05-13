//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================

ALIB_EXPORT namespace alib {  namespace expressions {
namespace detail {
class  ProgramBase;
struct VirtualMachineBase;
}

// forwards
class  Compiler;
struct Scope;

//==================================================================================================
/// This is a central class of library module \alib_expressions_nl representing compiled,
/// evaluable expressions. Instances of this type are always embedded in the automatic pointer
/// #"Expression", which are received with the method #"Compiler::Compile;2*".
///
/// For information about general use and features of this class consult the
/// #"alib_mod_expressions;ALib Expressions User Manual".
///
/// ## Friends ##
/// class #"Compiler"
/// class #"detail::Program;*"
//==================================================================================================
class ExpressionVal {
    #if !DOXYGEN
        // The compiler that compiles us.
        friend class Compiler;

        // The program that evaluates us.
        friend class detail::ProgramBase;

        // The virtual machine our program runs on.
        friend struct detail::VirtualMachineBase;
    #endif

  protected:
    /// The allocator, provided with construction.
    /// This usually is the 'self-contained' instance of type #"Expression".
    /// This allocator is forwarded to the #"ctScope" and
    /// #"DbgLock;locked" after compilation.
    MonoAllocator&          allocator;

    /// Compile-time scope object. Used to allocate constant program object copies.
    /// Also passed to the compiler plug-ins during compilation to add pre-calculated
    /// data.
    Scope*                  ctScope;

    /// The name of the expression (if named, otherwise resourced, usually "ANONYMOUS" ).
    String                  name;

    /// The compiled expression program.
    detail::ProgramBase*    program;

    /// The original source string of the expression.
    String                  originalString;

    /// The normalized string as a result of compilation.
    AString                 normalizedString;

    /// The normalized string generated on request out of optimized expression program.
    AString                 optimizedString;

  public:
    #if ALIB_DEBUG
    /// Provides the time needed to parse the expression into an abstract syntax tree.
    ///
    /// Note: This field is available only with debug-builds of the library.
    Ticks::Duration     DbgParseTime;

    /// Provides the time needed to parse the expression into an abstract syntax tree.
    ///
    /// Note: This field is available only with debug-builds of the library.
    Ticks::Duration     DbgAssemblyTime;

    /// Provides the time needed for the last evaluation of the expression.
    ///
    /// Note: This field is available only with debug-builds of the library.
    Ticks::Duration     DbgLastEvaluationTime;
    #endif

    /// Constructor.
    /// Expressions are created using #"Compiler::Compile;*" and thus, this
    /// constructor is available for the compiler only.
    /// \note
    ///   The common way to assert accessibility would be to make this constructor protected
    ///   and make class #"%Compiler" a friend. This is not possible, as this type is
    ///   to be constructed by container type #"SharedVal". Therefore, an
    ///   unused parameter of a protected type has to be passed, which can be created only
    ///   by friend #"%Compiler".
    ///
    /// @param allocator    The allocator to use. Usually this is the self-contained allocator
    ///                     of type #"Expression"
    /// @param sourceString The original string that is to be compiled.
    /// @param pCTScope     The compile-time scope.
    ALIB_DLL            ExpressionVal( MonoAllocator&  allocator,
                                         const String& sourceString,
                                         Scope*        pCTScope      );

    /// Destructor.
    ALIB_DLL           ~ExpressionVal();

    /// The name of the expression. A name is only available if the expression was created with
    /// #"Compiler::AddNamed;*". This might be 'automatically' done when nested
    /// expressions get compiled and the compiler supports retrieval of expression strings by
    /// name from some custom location (or built-in \alib variable mechanics).
    ///
    /// Otherwise, the name is \b "ANONYMOUS", which is a resourced string of key
    /// \c "ANON_EXPR_NAME".
    ///
    /// @return The expression's name.
    ALIB_DLL
    String              Name();

    /// Evaluates the expression by executing the compiled \p{program}.
    ///
    /// @return The result of this evaluation of this expression node.
    ALIB_DLL Box        ResultType();

    /// Evaluates the expression by executing the compiled \p{program}.
    ///
    /// With debug-builds of this library, \alib_assertions may be raised.
    /// Usually this indicates that a native callback function returned a value of erroneous
    /// type, which usually are caused by erroneous compiler plug-ins, respectively the native
    /// callback functions that those provide.
    ///
    /// The assertion will most probably give detailed information.
    ///
    /// @param scope  The evaluation scope.
    /// @return The result of this evaluation of this expression node.
    ALIB_DLL Box        Evaluate(Scope& scope);


    /// Returns the originally given expression string.
    ///
    /// @return The original expression string.
    String              GetOriginalString()                         const { return originalString; }

    /// Returns a normalized version of the original expression string.
    ///
    /// The result of normalization can be tweaked with the flags in field configuration field
    /// #"Compiler::CfgNormalization;*". In any case, unnecessary (multiple)
    /// whitespaces and brackets are removed. Consult the documentation of enumeration
    /// #"Normalization" for details of the options.
    ///
    /// It is guaranteed that the normalized version of the expression string is parsable and
    /// leads to the identical evaluation program as the original expression string.
    ///
    /// Software might choose to write back normalized expressions, for example, into
    /// configuration files.
    ///
    /// \note
    ///   This method does not perform the normalization, but returns a normalized version of the
    ///   parsed expression string, which was created with the compilation of the expression.
    ///   A normalized string is always created.
    ///
    /// @return The normalized expression string.
    String              GetNormalizedString()                     const { return normalizedString; }

    /// Returns a normalized expression string reflecting an optimized version of this
    /// expression.
    /// The number of optimizations performed during compilation of the expression can be
    /// received by invoking
    /// #"Program::CtdOptimizations"
    /// on the program returned by #"GetProgram". If this is \c 0, then the expression string
    /// returned here matches the normalized expression string received with
    /// #"GetNormalizedString".
    ///
    /// \note
    ///   On the first invocation, the string is generated once. For this, an abstract syntax
    ///   tree is created by decompiling the optimized program. This in turn is assembled
    ///   back to a program (by omitting the generation of commands and without invoking
    ///   on compiler plug-ins, etc.) which generates the normalized expression string from the
    ///   AST.
    ///
    /// @return The expression string requested.
    ALIB_DLL
    String                  GetOptimizedString();


    /// Returns the program that evaluates the expression.
    /// @return The result of this evaluation of this expression node.
    detail::ProgramBase*    GetProgram()                                         { return program; }

    /// Returns the number of #"VirtualMachine::Command"s that the program encompasses.
    /// @return The program's length.
    ALIB_DLL integer        GetProgramLength();

    /// Tests whether the expression always results to the same value (and hence is independent from
    /// the expression scope. In this case, the end-user might be notified about that fact, because
    /// probably the expression is incorrect.<br>
    /// The implementation of this method checks, if
    /// - the expression-program has the length of \c 1, and
    /// - the single command is <b>CONSTANT</b>.
    ///
    /// \attention
    ///   As explained in the chapter #"alib_expressions_details_optimizations", the compiler is
    ///   not enabled to perform all optimizations, which might in theory be possible. Thus, the
    ///   rule for this method's result is: If \c true is returned it is sure that the expression
    ///   is constant, if \c false is returned, it might still be!
    /// @return The best possible guess about whether this expression is constant.
    ALIB_DLL bool           IsConstant();

    /// @return The number of optimizations or \c -1 if optimizations were not activated during
    ///         program assembly.
    ALIB_DLL int            CtdOptimizations();
};


/// Implements #"TSharedMonoVal" with type #"ExpressionVal".
/// Therefore, the relevant documentation is found with class #"ExpressionVal",
/// which can be accessed through this type using <c>operator->()</c>.
///
/// The result of combining both is an automatic pointer to a #"%ExpressionVal" that is
/// "self-contained" in the first buffer of a #"MonoAllocator" together with the
/// allocator itself.
/// The expression is deleted and all associated memory is freed when the last copy of the pointer
/// goes out of scope.
///
/// \note
///   With the documentation of type #"%TSharedMonoVal",
///   #"alib_contmono_smv_naming;two naming schemes" are suggested.
///   Here, the second scheme is used because the method #"Compiler::Compile;*"
///   will always return this encapsulated object. Expressions are shared by definition.
struct Expression : monomem::TSharedMonoVal<ExpressionVal, HeapAllocator, void> {
  protected:
    #if !DOXYGEN
      // The compiler builds this type which by design does not expose a constructor.
      friend class Compiler;
    #endif

    #if DOXYGEN
        /// Forbid this method by making it protected.
        /// @tparam  TArgs    The argument types used for re-constructing \p{T}.
        /// @param   args     The arguments to re-construct the instance of \p{T}.
        template<typename... TArgs>
        void    Reset( TArgs&&... args );
    #else
        using TSharedMonoVal::Reset;
    #endif

    /// Constructor.
    /// Calls the constructor of parent #"%TSharedMonoVal" and then invokes
    /// #"TSharedMonoVal::ConstructT;*" passing the mono allocator that the
    /// parent creates this instance in.
    /// @param initialBufferSizeInKB  The initial size of memory buffers.
    ///                               Passed to the allocator given with parent class
    ///                               #"TSharedMonoVal".<br>
    /// @param bufferGrowthInPercent  Optional growth factor in percent, applied to the buffer size
    ///                               with each next buffer allocation.
    Expression(  size_t        initialBufferSizeInKB, unsigned bufferGrowthInPercent  )
    : TSharedMonoVal(initialBufferSizeInKB, bufferGrowthInPercent )                               {}

  public:
    /// Constructs an empty instance, hence a cleared automatic pointer.
    Expression()                                                                           =default;

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c nullptr to values of this type,
    /// which clears the automatic pointer.
    Expression(std::nullptr_t)                                                           noexcept {}

}; // struct Expression


#if ALIB_DEBUG
/// Lists a virtual machine program.
///
/// Note: This method is available only with debug-builds of the library.
/// @param expression  The expression to list the virtual machine program for.
/// @return The program listing.
ALIB_DLL
AString  DbgList(Expression expression);
#endif

} // namespace alib[::expressions]

/// Type alias in namespace #"%alib".
using     Expression=    expressions::Expression;

} // namespace [alib]


namespace alib::strings {
#if DOXYGEN
namespace APPENDABLES {
#endif
//==================================================================================================
/// Specialization of functor #"AppendableTraits" for type
/// #"ExpressionVal".
//==================================================================================================
template<>
struct AppendableTraits<expressions::ExpressionVal,alib::character, lang::HeapAllocator> {
    /// Appends the result of #"ExpressionVal::GetNormalizedString;*" to the \p{target}.
    /// @param target The #"%AString" that method #"%Append(const TAppendable&)" was invoked on.
    /// @param src    The expression to append.
    void operator()( AString& target, const expressions::ExpressionVal& src )
    { target << src.GetNormalizedString(); }
};

//==================================================================================================
/// Specialization of functor #"AppendableTraits" for type
/// #"Expression".
//==================================================================================================
template<>
struct AppendableTraits<expressions::Expression,alib::character, lang::HeapAllocator> {
    /// Appends the result of #"ExpressionVal::GetNormalizedString;*" to the
    /// \p{target}.
    ///
    /// @param target The #"%AString" that method #"%Append(const TAppendable&)" was invoked on.
    /// @param src    The expression to append.
    void operator()( AString& target, const expressions::Expression& src )
    { target << src->GetNormalizedString(); }
};

#if DOXYGEN
}
#endif
} // namespace [alib::strings]

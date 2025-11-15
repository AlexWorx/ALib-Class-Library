//==================================================================================================
/// \file
/// This header-file is part of module \alib_expressions of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace expressions {

class Compiler;
class ExpressionVal;
namespace detail { struct VirtualMachineBase; }

//==================================================================================================
/// This class acts as a simple virtual container to store custom resources in
/// \alib{expressions;Scope} objects.
///
/// To do so, custom derived types would simply add a custom member object together with a virtual
/// destructor that ensures that the member object(s) get rightfully deleted.
///
/// Instances of this type have to be created using the scope's allocator and are to be stored in
/// container
/// \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a45b12fe152f8e6a90341987ef35c958e;Scope::NamedResources}.
//==================================================================================================
struct ScopeResource
{
    /// The virtual destructor needed for virtual types.
    virtual ~ScopeResource()                                                                      {}
};

//==================================================================================================
/// This type is used as the default class to provide access to program data when evaluating
/// \alib expressions.
/// Usually a derived type which contains references to necessary application data is passed to
/// the method \alib{expressions;ExpressionVal::Evaluate}.
/// Then, custom callback functions may cast instances of this type that they receive back to
/// the derived type and access such application-specific data.
///
/// Also, scope objects are used to store intermediate results as well as the final one,
/// in the case that such results are not of a simple type that can be boxed
/// \alib{boxing;Box} "by value".
///
/// For this, two different allocator objects are provided. One for compile-time results and
/// one for those needed at evaluation-time.
///
/// A scope object can be reused for evaluating the same expression several times.
/// Before the evaluation, the custom "scoped data" has to be set.
/// With each reuse, the method #Reset will be invoked internally.
/// Hence, if custom storage members are added in derived types, this method has
/// to be overwritten to a) invoke the original method and b) clean such custom types.
///
/// One singleton of this type, which is used to store compile-time data, is created with the
/// virtual method \alib{expressions;Compiler::createCompileTimeScope}.
/// If compile-time invokable custom callback methods use custom storage allocators, this method
/// has to be overridden to return the proper custom version of this class.
/// (Note, this is not needed for the evaluation-time instances, as this is created in the custom
/// code unit anyhow and passed to method \alib{expressions;ExpressionVal::Evaluate}).
///
/// \see
///   - For details on the use of scopes see the manual chapter \ref alib_expressions_scopes.
///   - This is an almost completely public struct.
///     The design rationale behind this is explained in the manual chapter
///     \ref alib_expressions_prereq_bauhaus
//==================================================================================================
struct Scope
{
    /// Members used by the virtual machine. This is constructed only with evaluation-time scopes.
    struct VMMembers
    {
        /// Constructor.
        /// @param allocator The allocator of the evaluation scope.b
        VMMembers(MonoAllocator& allocator)
        : CTScope(nullptr)
        , NestedExpressions(allocator)                                                            {}

        /// This is a pointer to the compile-time scope, primarily is used to access field
        /// \doxlinkproblem{structalib_1_1expressions_1_1Scope.html;a45b12fe152f8e6a90341987ef35c958e;Scope::NamedResources}.
        /// which is only created with compile time scopes.
        /// This concept allows creating resources at compile-time which can be used for evaluation.
        ///
        /// A sample use case is implemented with the built-in compiler plug-in
        /// \alib{expressions::plugins;Strings}. When wildcard or regex matching is performed on
        /// constant pattern strings, the matching class (which itself "compiles" the pattern once)
        /// is created once and reused during evaluation.
        Scope*                          CTScope;

        /// Stack of nested expressions called during evaluation. Used to detect cyclic expressions.
        StdVectorMA<ExpressionVal*>     NestedExpressions;
    };

    /// Evaluation-scope allocator. With compile-time scopes, this is allocator will not be
    /// initialized.
    MonoAllocator*                      EvalScopeAllocator;
    
    /// Monotonic allocator used to store temporary data and results.
    /// The allocated data within this object becomes cleared automatically by method #Reset,
    /// at the moment an expression is evaluated the next time (usually with different custom scope
    /// data).
    ///
    /// Note that this allocator is \b not cleared for the compile-time scope instance.
    MonoAllocator&                      Allocator;

    /// This is the argument stack used by class \alib{expressions;detail::VirtualMachine} when
    /// evaluating expressions.<br>
    /// With compilation, it used to "simulate" evaluation calls at compile-time.
    StdVectorMA<Box>*                   Stack;

    /// Used to convert numbers to strings and vice versa. In addition, expression function
    /// \b %Format of built-in compiler plugin \alib{expressions::plugins;Strings} uses this object
    /// to perform the formatting of arbitrary objects according to a given format string.
    ///
    /// Hence, to support customized format strings, a different formatter is to be passed here.
    /// Default format string conventions provided with \alib are
    /// \alib{format;FormatterPythonStyle;python style} and
    /// \alib{format;FormatterJavaStyle;java/printf-like style}.
    ///
    /// The default implementation of method \alib{expressions;Compiler::createCompileTimeScope}
    /// provides the field \alib{expressions;Compiler::CfgFormatter} with the constructor of the
    /// default compile-time scope.
    SPFormatter                         Formatter;

    /// A list of user-defined, named resources.
    /// Named resources may be allocated at compile-time and used at evaluation-time.<br>
    /// This pointer is only set with compile-time scopes.
    HashMap<MonoAllocator,
            NString, ScopeResource*>*   NamedResources;

    /// The members used for the virtual machine. Available only with evaluation-time instances.
    VMMembers*                          EvalScopeVMMembers;

    #if ALIB_DEBUG_CRITICAL_SECTIONS
    /// Debug-tool to detect usage of evaluation scope from within multiple threads
    /// (which is not allowed). It is set by the virtual machine when running programs.
    lang::DbgCriticalSections       DCS;
    #endif

    /// Constructor used with evaluation scopes. Creates a mono allocator.<br>
    /// Usually, for parameter \p{formatter} field \alib{expressions;Compiler::CfgFormatter} should
    /// be provided.
    ///
    /// @param formatter A reference to a \c std::shared_ptr holding a formatter.
    ///                  Usually \alib{expressions;Compiler::CfgFormatter}.
    ALIB_DLL            Scope( SPFormatter& formatter  );

    /// Constructor used with compile-time scopes. Receives the allocator from the expression
    /// instance.<br>
    /// Usually, for parameter \p{formatter} field \alib{expressions;Compiler::CfgFormatter} should
    /// be provided.
    ///
    /// @param allocator The allocator of the expression.
    /// @param formatter A reference to a \c std::shared_ptr holding a formatter.
    ///                  Usually \alib{expressions;Compiler::CfgFormatter}.
    ALIB_DLL            Scope( MonoAllocator& allocator, SPFormatter& formatter );

    /// Virtual destructor.
    ALIB_DLL virtual   ~Scope();

    /// Deleted copy constructor.
    Scope(const Scope&)                                                                     =delete;

    /// Deleted copy assignment.
    void operator=(const Scope&)                                                            =delete;

    /// Usually, this method is unnecessary to be checked.
    /// It is useful and provided to support more complicated management of resources, i.e.
    /// allocation of resources at compile-time which are later used for evaluation.
    ///
    /// @return \c true if this is a compile-time invocation, \c false otherwise.
    bool                IsCompileTime()                    { return EvalScopeVMMembers == nullptr; }

    /// Scope objects usually are reused, either for evaluating the same expression using
    /// different scoped data (attached to derived versions of this class), or for evaluating
    /// different expression.<br>
    /// Such a reuse is internally detected, and if so, this method is invoked.
    ///
    /// Instances of this class used as compilation scope, are not reset during the life-cycle
    /// of an expression.
    ///
    /// Derived versions of this class need to free allocations performed by callback functions.
    ALIB_DLL
    virtual void        Reset();

  protected:
    /// This method is called in the destructor, as well as in method #Reset.
    ALIB_DLL
    virtual void        freeResources();
}; // Scope

} // namespace alib[::expressions]

/// Type alias in namespace \b alib. Renamed to not collide with #alib::lox::Scope.
using     ExpressionScope=    expressions::Scope;

} // namespace [alib]

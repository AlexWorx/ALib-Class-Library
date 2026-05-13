//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib{ namespace filetree {

/// This class implements an #"alib_mod_expressions;ALib Expression Compiler Plugin"
/// and thus enables "runtime expressions" to work with nodes of #"FTree"
///
/// Unless multithreaded compilation of expressions is planned, an application should create one
/// singleton of this class.
///
/// Method #"CreateFilter" creates an object of inner type #"FileExpressions::Filter;*", which
/// implements the #"%FFilter" interface used with scan parameters of overloaded function
/// #"ScanFiles(FTree&)". This allows runtime evaluation of filter rules, for example, rules
/// coming from the command-line, from configuration files or from interactive end-user input
/// (hence from an UX/UI).
///
/// Of course, the expression compiler can also be used directly without using the inner
/// #"%Filter" class for evaluation, and expressions then do not need to return a boolean "yes/no"
/// result.
///
/// @see
///   For a quick tutorial, see the #"alib_mod_filetree;Programmer's Manual".
///
/// # Implemented Expression Features #
/// Compiler plug-in that provides expressions on filesystem entries, stored in nodes of
/// class #"FTree".
///
/// All identifier and function names are defined to be matched case-insensitive and can be
/// abbreviated along their <em>CamelHumps</em>. This means an identifier called
///
///     CamelHumpCounter
///
/// can be abbreviated to:
///
///     CamelHC
///     CHCounter
///     CamHuCo
///     CHC
///     chc
///     cHc
///
/// and so on.
///
/// Furthermore, the matchable tokens are not hard-coded but resourced with \alibcamp singleton
/// #"alib::FILETREE". With that, for example, language translations might be performed.
///
/// ## Types: ##
/// <br>
/// This plug-in introduces the following types to the expression compiler:
/// - #"FileStatus::Types;*"
/// - #"FileStatus::Permissions;*"
/// - #"FileStatus::TOwnerAndGroupID;*"
///
/// Each type is auto-cast to built-in expression type #"%Integer" to allow all common operators,
/// especially bitwise boolean operators.
///
/// ## Constants: ##
/// <br>
/// Type                            | Name             |Min. Abbreviation| Description
/// --------------------------------|------------------|-----------------|-------------
/// #"FileStatus::Permissions" |\b OwnerRead      | ore       | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b OwnerWrite     | ow        | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b OwnerExecute   | oe        | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b GroupRead      | gr        | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b GroupWrite     | gw        | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b GroupExecute   | ge        | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b OthersRead     | otr       | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b OthersWrite    | otw       | Used test result of function \p{Permission}.
/// #"FileStatus::Permissions" |\b OthersExecute  | ote       | Used test result of function \p{Permission}.
/// #"FileStatus::Types"       |\b Directory      | dir       | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b SymbolicLinkDir| sld       | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b Regular        | reg       | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b SymbolicLink   | sl        | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b Block          | block     | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b Character      | character | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b Fifo           | fifo      | Used to compare the result of function \p{Type}.
/// #"FileStatus::Types"       |\b Socket         | socket    | Used to compare the result of function \p{Type}.
///
///
/// <br>
/// ## Constant Constructor Functions: ##
/// <br>
/// Return Type| Name        |Min. Abbreviation| Signature | Description |
/// -----------|-------------|-----------------|-----------|-------------|
/// Integer    |\b KiloBytes | KB              | int       | Returns the given number multiplied with 2^10.
/// Integer    |\b MegaBytes | MB              | int       | Returns the given number multiplied with 2^20.
/// Integer    |\b GigaBytes | GB              | int       | Returns the given number multiplied with 2^30.
/// Integer    |\b TeraBytes | TB              | int       | Returns the given number multiplied with 2^40.
/// Integer    |\b PetaBytes | PB              | int       | Returns the given number multiplied with 2^50.
/// Integer    |\b ExaBytes  | EB              | int       | Returns the given number multiplied with 2^60.
///
/// <br>
/// ## Functions: ##
/// <br>
///
/// Return Type                        | Name             | Min. Abbreviation | Signature  | Description                                                                                                                           |
/// ----------------------------------- | -----------      | ------------------| -----------| -------------                                                                                                                         |
/// String                              | \b Name          | name              | ./.        | Returns the file name.
/// #"FileStatus::Types"            | \b Type          | ty                | ./.        | Returns the result of #"FileStatus::Type".
/// Boolean                             | \b IsDirectory   | isdir             | ./.        | Returns the result of #"FileStatus::IsDirectory".
/// Boolean                             | \b IsSymbolicLink| issl              | ./.        | Returns the result of #"FileStatus::IsSymbolicLink".
/// Integer                             | \b Size          | size              | ./.        | Returns the result of #"FileStatus::Size".
/// DateTime                            | \b Date          | timestamp         | ./.        | Returns the result of #"FileStatus::MDate".
/// DateTime                            | \b MDate         | md                | ./.        | Returns the result of #"FileStatus::MDate".
/// DateTime                            | \b BDate         | bd                | ./.        | Returns the result of #"FileStatus::BDate".
/// DateTime                            | \b CDate         | cd                | ./.        | Returns the result of #"FileStatus::CDate".
/// DateTime                            | \b ATime         | ad                | ./.        | Returns the result of #"FileStatus::ADate".
/// #"FileStatus::Permissions"      | \b Permissions   | perm              | ./.        | Returns the result of #"FileStatus::Permissions".
/// #"FileStatus::TOwnerAndGroupID" | \b Owner         | owner             | ./.        | Returns the result of #"FileStatus::Owner".
/// #"FileStatus::TOwnerAndGroupID" | \b Group         | group             | ./.        | Returns the result of #"FileStatus::Group".
/// #"FileStatus::TOwnerAndGroupID" | \b UserID        | uid               | ./.        | Returns the current user's ID (calls POSIX <b>getuid()</b>).
/// #"FileStatus::TOwnerAndGroupID" | \b GroupID       | gid               | ./.        | Returns the current user's group ID (calls POSIX <b>getgid()</b>).
///
/// \par Availability
///   This class is available only if the module \alib_expressions is included in the \alibbuild.
class FileExpressions {
  protected:
    /// The internal compiler plug-in which adds file-related functions to the expression
    /// compiler as documented with the outer class.
    struct Plugin : public expressions::plugins::Calculus {
        /// Constructor
        /// @param compiler The compiler that this plugin will be attached to.
        Plugin( Compiler& compiler );
    };

  public:
    class Filter;

    /// The expression scope used with file expressions. Contains a #"%FTree" cursor and
    /// the string of the path that the file or folder is located in.
    struct FexScope : expressions::Scope {
      #if !DOXYGEN
        //The filter class needs casting this class to its base.
        friend class Filter;
      #endif

        FTFile        Node; ///<A node of a #"FTree".
        PathString    ParentPath; ///<The absolute path to the parent directory.

        /// Constructor. Passes the formatter to the parent constructor.
        /// @param formatter A reference to a \c std::shared_ptr holding a formatter.
        ///                  Usually #"Compiler::CfgFormatter;*".
        FexScope( SPFormatter& formatter )
        : expressions::Scope(formatter)                                                           {}
    };

    /// Implementation of abstract class #"FFilter" which uses runtime
    /// compiled and evaluated expressions to determine the inclusion of files or directories.
    /// Instances of this type can be created with outer classes' method
    /// #"FileExpressions::CreateFilter;*" and, for example, be attached to fields
    /// #"ScanParameters::FileFilter",
    /// #"ScanParameters::DirectoryFilterPreRecursion" and
    /// #"ScanParameters::DirectoryFilterPostRecursion" of class
    /// #"%ScanParameters".
    ///
    /// \par Availability
    ///   This class is available only if the module \alib_expressions is included in the \alibbuild.
    class Filter  : public FFilter {
      protected:
        #if !DOXYGEN
          // The outer class is our friend.
          friend class FileExpressions;
        #endif

        FileExpressions&           fex;        ///< A reference to the instance that this filter
                                               ///< uses.
        FileExpressions::FexScope  scope;      ///< The scope used with this filter.
        Expression                 expression; ///< The compiled expression.


        /// Protected constructor. Compiles the given expression.
        /// To create an instance of this class, use
        /// #"FileExpressions::CreateFilter;*".
        /// @param pFex              The expression compiler to use.
        /// @param expressionString The expression string.
        ALIB_DLL
        Filter( FileExpressions& pFex, const String& expressionString );


      public:
        /// The filter method.
        /// @param node        The node in the file-tree to examine.
        /// @param parentPath  The absolute path to the parent directory that the file resides in.
        /// @return The result determines, whether the given file or directory has passed the filter
        ///        or not. Consequently, \c true means "passed" and \c false means
        ///        "filtered out".
        virtual bool Includes( const FTFile& node, const PathString& parentPath)          override {
            scope.Node       = node;
            scope.ParentPath = parentPath;
            return expression->Evaluate( scope ).Unbox<bool>();
        }
    }; // class Filter


    Compiler  compiler;    ///< The expression compiler.
    Plugin    plugin;      ///< The file expression Plugin


    /// Default constructor.
    ALIB_DLL
    FileExpressions();

    /// Creates a file filter using the expression compiler of this instance.
    /// @param expressionString The expression string to filter files and directories.
    /// @return A shared pointer to the dynamically allocated filter object.
    ALIB_DLL
    SPFileFilter    CreateFilter( const String& expressionString );

}; // class FileExpressions

} // namespace alib[::filetree]


/// Type alias in namespace #"%alib".
using     FileExpressions=   filetree::FileExpressions;

}  // namespace [alib]

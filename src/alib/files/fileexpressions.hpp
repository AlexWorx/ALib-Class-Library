//==================================================================================================
/// \file
/// This header file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_FILES_FILEEXPRESSIONS
#define HPP_ALIB_FILES_FILEEXPRESSIONS 1
#pragma once
#include "alib/files/ftree.hpp"
#include "alib/files/ffilter.hpp"

ALIB_ASSERT_MODULE(EXPRESSIONS)
#include "alib/expressions/plugins/calculus.hpp"

namespace alib{ namespace files {

/// This class implements an \ref alib_mod_expressions "ALib Expression Compiler Plugin"
/// and thus enables "run-time expressions" to work with nodes of \alib{files;FTree}
///
/// Unless multithreaded use is planned, an application should create one singleton of this class.
///
/// Method #CreateFilter creates an object of inner type  \alib{files;FileExpressions::Filter}, which
/// implements the \b FFilter interface  used with scan parameters of function
/// \alib{files;ScanFiles}. This allows run-time evaluation of filter rules, for example rules
/// coming from the command line, from configuration files or from interactive end-user input
/// (hence from an UX/UI).
///
/// Of course the expression compiler can also be used directly without using the inner
/// \b Filter class for evaluation and expressions do not need return a boolean "yes/no" result.
///
/// @see
///   For a quick tutorial, see the \ref alib_mod_files "Programmer's Manual".
///
/// # Implemented Expression Features #
/// %Compiler plug-in that provides expressions on filesystem entries, stored in nodes of
/// class \alib{files;FTree}.
///
/// All identifier and function names are defined case insensitive.
/// Furthermore, the tokens are not hard-coded but resourced with \alibcamp singleton
/// \ref alib::FILES. With that, for example language translations might be performed.
///
/// ## Types:  ##
/// <br>
/// This plug-in introduces the following types to the expression compiler:
/// - \alib{files;FInfo::Types}
/// - \alib{files;FInfo::Permissions}
/// - \alib{files;FInfo::TOwnerAndGroupID}
///
/// Each types are auto-cast to \b Integer to allow all common operators, especially bitwise
/// boolean operators.
///
/// ## Constants:  ##
/// <br>
/// Type                            | Name             |Min. Abbreviation| Description
/// --------------------------------|------------------|-----------------|-------------
/// \alib{files::FInfo;Permissions} |\b OwnerRead      | ore       | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b OwnerWrite     | ow        | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b OwnerExecute   | oe        | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b GroupRead      | gr        | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b GroupWrite     | gw        | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b GroupExecute   | ge        | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b OthersRead     | otr       | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b OthersWrite    | otw       | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Permissions} |\b OthersExecute  | ote       | Used test result of function \p{Permission}.
/// \alib{files::FInfo;Types}       |\b Directory      | dir       | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b SymbolicLinkDir| sld       | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b Regular        | reg       | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b SymbolicLink   | sl        | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b Block          | block     | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b Character      | character | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b Fifo           | fifo      | Used to compare the result of function \p{Type}.
/// \alib{files::FInfo;Types}       |\b Socket         | socket    | Used to compare the result of function \p{Type}.
///
///
/// <br>
/// ## Constant Constructor Functions:  ##
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
/// ## Functions:  ##
/// <br>
///
/// Return Type                        | Name             | Min. Abbreviation | Signature  | Description                                                                                                                           |
/// ----------------------------------- | -----------      | ------------------| -----------| -------------                                                                                                                         |
/// String                              | \b Name          | name              | ./.        | Returns the file name.
/// \alib{files::FInfo;Types}           | \b Type          | ty                | ./.        | Returns the result of \alib{files::FInfo;Type}.
/// Boolean                             | \b IsDirectory   | isdir             | ./.        | Returns the result of \alib{files::FInfo;IsDirectory}.
/// Boolean                             | \b IsSymbolicLink| issl              | ./.        | Returns the result of \alib{files::FInfo;IsSymbolicLink}.
/// Integer                             | \b Size          | size              | ./.        | Returns the result of \alib{files::FInfo;Size}.
/// DateTime                            | \b Date          | timestamp         | ./.        | Returns the result of \alib{files::FInfo;MDate}.
/// DateTime                            | \b MDate         | md                | ./.        | Returns the result of \alib{files::FInfo;MDate}.
/// DateTime                            | \b BDate         | bd                | ./.        | Returns the result of \alib{files::FInfo;BDate}.
/// DateTime                            | \b CDate         | cd                | ./.        | Returns the result of \alib{files::FInfo;CDate}.
/// DateTime                            | \b ATime         | ad                | ./.        | Returns the result of \alib{files::FInfo;ADate}.
/// \alib{files::FInfo;Permissions}     | \b Permissions   | perm              | ./.        | Returns the result of \alib{files::FInfo;Permissions}.
/// \alib{files::FInfo;TOwnerAndGroupID}| \b Owner         | owner             | ./.        | Returns the result of \alib{files::FInfo;Owner}.
/// \alib{files::FInfo;TOwnerAndGroupID}| \b Group         | group             | ./.        | Returns the result of \alib{files::FInfo;Group}.
/// \alib{files::FInfo;TOwnerAndGroupID}| \b UserID        | uid               | ./.        | Returns the current user's ID (calls posix <b>getuid()</b>).
/// \alib{files::FInfo;TOwnerAndGroupID}| \b GroupID       | gid               | ./.        | Returns the current user's group ID (calls posix <b>getgid()</b>).
///
class FileExpressions
{
  protected:
    /// The internal compiler plug-in which adds file-related functions to the expression
    /// compiler as documented with the outer class.
    struct Plugin : public expressions::plugins::Calculus
    {
        /// Constructor
        /// @param compiler The compiler that this plugin will be attached to.
        Plugin( Compiler& compiler );
    };

  public:
    /// The expression scope used with file expressions. Contains an \b FTree cursor and
    /// the string of the path the file or folder is located in.
    struct FexScope : public expressions::Scope
    {
        File                        Node;          ///< A node of a \alib{files;FTree}.
        lang::system::PathString    ParentPath;    ///< The absolute path to the parent directory.

        /// Constructor. Passes the formatter to the parent constructor.
        /// @param formatter A reference to a \c std::shared_ptr holding a formatter.
        ///                  Usually \alib{expressions;Compiler::CfgFormatter}.
        FexScope( SPFormatter& formatter )
        : expressions::Scope(formatter)                                                           {}
    };

    /// Implementation of abstract class \alib{files;FFilter} which uses run-time
    /// compiled and evaluated expressions to determine inclusion of files or directories.
    /// Instances of this type can be created with outer classes' method
    /// \alib{files;FileExpressions::CreateFilter} and for example be attached to fields
    /// \alib{files::ScanParameters;FileFilter},
    /// \alib{files::ScanParameters;DirectoryFilterPreRecursion} and
    /// \alib{files::ScanParameters;DirectoryFilterPostRecursion} of class
    /// \b ScanParameters.
    class Filter  : public FFilter
    {
      protected:
        friend class FileExpressions;            ///< The outer class is our friend.
        FileExpressions&           fex;          ///< The scope used with this filter.
        FileExpressions::FexScope  scope;        ///< The scope used with this filter.
        Expression                 expression;   ///< The compiled expression.


        /// Protected constructor. Compiles the given expression.
        /// To create an instance of this class, use
        /// \alib{files;FileExpressions::CreateFilter}.
        /// @param pFex              The expression compiler to use.
        /// @param expressionString The expression string.
        ALIB_API
        Filter( FileExpressions& pFex, const String& expressionString );


      public:
        /// The filter method.
        /// @param node        The node in the file-tree to examine.
        /// @param parentPath  The absolute path to the parent directory that the file resides in.
        /// @return The result determines, whether the given file or directory has passed the filter
        ///        or not. Consequently, \c true means "passed" and \c false means
        ///        "filtered out".
        virtual bool Includes( const File& node, const lang::system::PathString& parentPath) override
        {
            scope.Node       = node;
            scope.ParentPath = parentPath;
            return expression->Evaluate( scope ).Unbox<bool>();
        }
    }; // class Filter


    Compiler  compiler;    ///< The expression compiler.
    Plugin    plugin;      ///< The file expression Plugin


    /// Default constructor.
    ALIB_API
    FileExpressions();

    /// Creates a file filter using the expression compiler of this instance.
    /// @param expressionString The expression string to filter files and directories.
    /// @return A shared pointer to the dynamically allocated filter object.
    ALIB_API
    SPFileFilter    CreateFilter( const String& expressionString );

}; // class FileExpressions

} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     FileExpressions=   files::FileExpressions;

}  // namespace [alib]

#endif // HPP_ALIB_FILES_FILEEXPRESSIONS


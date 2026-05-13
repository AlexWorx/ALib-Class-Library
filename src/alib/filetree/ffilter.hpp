//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace filetree {

//==================================================================================================
/// This is a straight forward simple abstract class with just one interface method which has
/// to return \c true if an file is included (not filtered) or \c false if it is not included and
/// thus to be filtered out.<br>
/// Instead of the provision of an #"%FTValue" object, a cursor of an #"%FTree" is provided.
/// This allows more detailed inspection of parent directories, if needed.
/// In addition, the full path string (the real path, all symbolic links translated) is provided.
/// While this could be assembled from the given cursor \p{node}, the provision is made to allow
/// the highest execution performance, as the string is available during the scan process anyhow.
///
/// Instances of derived (non-abstract) implementations can optionally be attached to fields
/// #"ScanParameters::FileFilter",
/// #"ScanParameters::DirectoryFilterPreRecursion", and
/// #"ScanParameters::DirectoryFilterPostRecursion" of class
/// #"%ScanParameters" to filter files during the scan process.
///
/// Users of the API might also create code that uses this interface type for post-scan
/// filtering, but in this case may of course also rely on their own implementations /mechanics.
//==================================================================================================
struct FFilter {
    /// Destructor
    virtual ~FFilter()                                                                     =default;

    /// Abstract virtual method to evaluate the inclusion of the given #"%FTValue" object.
    /// @param file          The node in the file-tree to examine.
    /// @param parentPath    The absolute path to the parent directory that the file resides in.
    /// @return The result determines, whether the given file or directory has passed the filter or
    ///         not. Consequently, \c true means "passed" and \c false means "filtered out".
    virtual bool Includes( const FTFile& file, const PathString& parentPath )                    =0;

}; //class FFilter

/// A shared pointer to a filter.
using SPFileFilter= std::shared_ptr<FFilter>;

} // namespace alib[::filetree]


/// Type alias in namespace #"%alib".
using     FFilter =   filetree::FFilter;

/// Type alias in namespace #"%alib".
using SPFileFilter= filetree::SPFileFilter;

}  // namespace [alib]

//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace filetree {

#if ALIB_DEBUG
/// The format string used with verbose logging to domain <c>/ALIB/FILETREE/SCAN</c> by the namespace
/// function #"ScanFiles(FTree&)".<br>
/// Defaults to <c>" {:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq nf l}"</c>
extern String DBG_FILETREE_SCAN_VERBOSE_LOG_FORMAT;
#endif


/// Input parameters to function #"ScanFiles(FTree&)".
struct ScanParameters {
    /// Options for processing symbolic links.
    enum class SymbolicLinks {
        DONT_RESOLVE            = 0, ///< Demands \b not to resolve symbolic links in any way.
        RESOLVE_BUT_DONT_FOLLOW = 1, ///< Demands to read symbolic links, but not follow linked directories.
                                     ///< FTValue dates, sizes, and access rights are set according to
                                     ///< the link target.
        RECURSIVE               = 2, ///< Read symbolic links and in case they are targeting a
                                     ///< directory, recurse into, if this directory meets the
                                     ///< other constraints associated with the current scan.
    };

    /// Denotes 'infinite' recursion if set to field #"MaxDepth".
    static constexpr unsigned InfiniteRecursion = (std::numeric_limits<unsigned>::max)();

    /// The path to be scanned.
    Path            StartPath;

    /// Denotes how symbolic links are treated.
    SymbolicLinks   LinkTreatment                                         =SymbolicLinks::RECURSIVE;

    /// The maximum recursion depth. Defaults to #"InfiniteRecursion".
    unsigned        MaxDepth                                                     =InfiniteRecursion;

    /// If \c true, the default, scanning does not stop recursion on directories which represent
    /// a mounted filesystem. If \c false, the search is restricted to the device that #"StartPath"
    /// resides in.
    bool            CrossFileSystems                                                         = true;

    /// If \c false (the default), scanning aborts if \e 'artificial' filesystems are found.
    /// Artificial filesystems under GNU/Linux, are for example:
    /// <c>/proc</c>, <c>/dev</c>, <c>/run</c>, <c>/sys</c>, and <c>/temp</c>.
    bool            IncludeArtificialFS                                                     = false;

    /// If \c false, empty directories remain in the result tree. Otherwise, they are deleted
    /// and do not appear in the tree.
    bool            RemoveEmptyDirectories                                                  = false;

    /// If set (not containing \c nullptr), files are passed to this filter and removed if \c false
    /// is returned.<br>
    /// The term "files" here means all sorts of files except Directories.
    /// Directories are either real directories, or in case the field #"LinkTreatment" is set to
    /// #"SymbolicLinks::RECURSIVE", symbolic links that
    /// target a directory.
    ///
    /// \see Optional filters #"DirectoryFilterPreRecursion" and #"DirectoryFilterPostRecursion".
    SPFileFilter    FileFilter;

    /// If set (not containing \c nullptr), this filter is invoked \b after a recursive scan of
    /// a directory. If \c false is returned, the recursion is not performed, but the (empty)
    /// directory remains in the result list, if field #"RemoveEmptyDirectories" evaluates to
    /// \c false.<br>
    /// Note that in case field #"LinkTreatment" is set to
    /// #"SymbolicLinks::RECURSIVE", this filter
    /// is also applied to symbolic links, which are readable, not broken, and target a directory.
    ///
    /// \note
    ///   Directories (and symbolic links to directories) are first recursively scanned before this
    ///   filter is applied. On deletion, of course the whole scanned subtree is deleted.
    ///   This allows filtering directories, depending on information available only after
    ///   scanning, hence by the numbers retrieved with #"FTValue::Sums;*".
    ///   To increase performance and filter directories \e before their recursive scan,
    ///   alternative field #"DirectoryFilterPreRecursion" is to be used.
    ///
    /// \see Optional filters #"DirectoryFilterPreRecursion" and #"FileFilter".
    ///
    SPFileFilter    DirectoryFilterPostRecursion;

    /// Same as #".DirectoryFilterPostRecursion" but is used \b before a recursive scan of
    /// a directory. Consequently, this filter leads to much higher scan performance than the
    /// alternative version, because huge branches of the file system might be omitted during scan.
    /// However, the numbers retrieved with #"FTValue::Sums;*" will all indicate
    /// \c 0, because no information is retrieved.<br>
    /// If a directory is "pruned" due to this filter, the entry still occurs in the #"%FTree",
    /// unless field #".RemoveEmptyDirectories" evaluates to \c true.<br>
    ///
    /// \see Optional filters #".DirectoryFilterPostRecursion" and #".FileFilter".
    ///
    SPFileFilter    DirectoryFilterPreRecursion;

    /// Constructor accepting all features.
    /// @param startPath             Stored in field #".StartPath".
    /// @param linkTreatment         Stored in field #".LinkTreatment". Defaults to #"%SymbolicLinks::RECURSIVE".
    /// @param maxDepth              Stored in field #".MaxDepth". Defaults to +InfiniteRecursion.
    /// @param crossFileSystems      Stored in field #".CrossFileSystems". Defaults to \c true.
    /// @param includeArtificialFS   Stored in field #".IncludeArtificialFS". Defaults to \c false.
    ScanParameters( const PathString& startPath,
                    SymbolicLinks     linkTreatment      = SymbolicLinks::RECURSIVE,
                    unsigned          maxDepth           = InfiniteRecursion,
                    bool              crossFileSystems   = true,
                    bool              includeArtificialFS= false                        )
    : StartPath          (startPath          )
    , LinkTreatment      (linkTreatment      )
    , MaxDepth           (maxDepth           )
    , CrossFileSystems   (crossFileSystems   )
    , IncludeArtificialFS(includeArtificialFS)                                                    {}

}; // struct ScanParameters

/// A simple vector containing nodes of an #"FTree". Such nodes are collected during calls
/// of the function #"ScanFiles". One call (aka during the recursive scan of one path) can result
/// in more than one entry in this list, because with resolving symbolic links new isolated
/// siblings can occur.<br>
/// The single new method of this type is #".Add", which checks if the given new start-path
/// is superseding others or is superseded itself by an existing path. In that case the
/// superseded path is deleted.
///
/// Despite the little effort that \alib takes with the provision of these mechanics, often the
/// analysis of, or a loop through this path list is not necessary. This is because most
/// using code would just scan one or more paths and then #"StringTreeIterator;loop through" just
/// all resulting directory and file nodes that have been inserted into the tree.
/// Consequently, the function #"ScanFiles" accepts an instance of this class only optionally.
struct CanonicalPathList : std::vector<FTFile> {
    /// Adds the given node to the list, in the case it is not superseded by an already
    /// collected node. Vice versa, existing nodes that are superseded by the given one are removed.
    /// @param node      The node to add.
    ALIB_DLL
    void Add(FTFile node);
};


/// ### General Information ###
/// Scans the filesystem according to the given #"%ScanParameters" and adds #"FTValue"
/// entries to the given #"FTree".
///
/// ### ALib FTree Data Contract ###
/// This function has a contract with the class #"FTree" that is used to store the scan results.
/// This contract states that any file or directory found during a scan is always stored using
/// the <em>"Real Path"</em> of the entry. This means that any symbolic link is resolved.
/// The consequences are:
/// - Files and directories which represent a symbolic link are always "leaf nodes".
///   (They never contain child nodes.). However, their symlink target path is attached twice
///   to the entry:
///   1. The original link information given, which often uses relative path addressing.
///   2. The absolute, <em>"Real Path"</em> of the target, which has a corresponding result entry
///      in the given #"%FTree".
/// - If a using software wants to use symbolic paths, for example, to present them to the end
///   user, such paths have to be assembled by the user's code in own responsibility.
///   All information for doing this is provided in the resulting tree object
/// - If symbolic path reconstruction is needed, nodes that participate in symbolic backlinks
///   must stay alive. Therefore, deleting nodes after scanning (or enabling scan options that
///   delete nodes during scanning) is not compatible with this feature.
/// - Doubly linked target files and directories are never a problem for this scanner. Each
///   file is scanned only once. This especially prevents all sorts of problems that would otherwise
///   occur with cyclic symbolic links.
/// - Due to this, even the given start path of a search might not be found as a result
///   in the given #"%FTree", because also start paths are converted to a <em>Real Path</em>.
/// - The scan result may contain more than one resulting path. This happens if a symbolic link
///   targets a file or directory not recursively included in the start path.
///   The resulting <em>"Real Path"</em> of the given start path is, however, always the first
///   result added.
///
/// The latter is reflected with (optional) parameter \p{resultPaths} of this function, which is
/// of type #"CanonicalPathList".
///
/// \note
///   Because the class #"FTree" is based on class #"StringTree", using code
///   is enabled to break this contract by adding entries below symbolic links.
///   Other entities of this \alibmod_nl will not break this contract.
///
/// ### Rescanning of Entries ###
/// Existing entries in the given \p{tree} are not overwritten. They might be scanned with "higher"
/// #"FileStatus::ScanStates;*" values, depending on given \p{parameters} and how they had been
/// scanned before. If the same "level" of scanning is provided, existing entries will not be
/// scanned again. If a rescan of a certain path is wanted, then the target entry of that path has
/// to be deleted before invoking this function. Due to the implementation of class FTree, repeated
/// delete and scan operations will not cause any heap-memory allocations or deallocations.
///
/// ### platform-dependent Code Selection ###
/// File scanning is a platform-dependent task and hence \alib uses one of two different
/// implementations:
/// 1. A POSIX version for compatible OSes,
/// 2. A version that relies on <c>C++ std::filesystem</c>.
///
/// The fallback version using <c>std::filesystem</c> has the following restrictions:
/// - The only time attribute available is the #"FileStatus::MDate;modification time" of
///   an entry. The fields #"FileStatus::BDate", #"FileStatus::CDate", and #"FileStatus::ADate" are always set
///   to the same as the modification time, even on filesystems that support the other values.
/// - The file time of symbolic links is \b always that of the target file. The C++ standard has
///   no possibility to access the link's time itself.
/// - The file time of broken symbolic links is set to the current time (time of scanning).
/// - The size that directories occupy on a disk cannot be determined.
///   Directory entries always report size <c>0</c>.
/// - The target of a symbolic link which points to a non-accessible directory, cannot be resolved
///   to a "real" (aka canonical) path, even if all other path components before were accessible.
///   (This is true for the implementation of the standard library under GNU/Linux and Clang
///   compiler at the time of writing this, 2024/02.)
/// - The flag #"ScanParameters::CrossFileSystems;*" is ignored. Crossing Filesystems cannot
///   be detected using purely the standard library.<br>
/// - A files' owner and owning group is not determined. Instead, #"FileStatus::UnknownID;*"
///   is set for both.
/// - The scanning process is half as fast as in the Posix version. The reason for this is probably
///   the internal allocation and deallocation of many quite volatile string objects in the C++
///   standard library.
///   Well, but it is still fast though!
///
/// \note As for today, using this module under WindowsOS, will fall back to the
///       <em>C++ std::filesystem</em> version. It may be that a future version will provide a
///       native implementation of this target system. Volunteers from the community are welcome to
///       contribute.
///
/// @param      tree           The tree to fill.
/// @param      parameters     The input parameters to determine the scan process.
/// @param[out] resultPaths    An optional container to store the result paths of a scan.
///                            If \c nullptr is given, the result paths are not collected. See the
///                            #"CanonicalPathList;types documentation" for further information.
/// @param[out] remainingStart An optional path string. If given, on failure, it will receive the
///                            remainder of the path given with #"ScanParameters::StartPath;2"
///                            starting with the first directory or file that could not be resolved
///                            or accessed.
///
/// @return The scan state code of the tree node of the first resulting path, hence of the node
///         referred to by the given #"ScanParameters::StartPath;2".<br>
///         On error, i.e. if the start path was invalid, not accessible, a broken link, a circular
///         link, or other failures, #"ScanStates::NOT_EXISTENT" is returned.
ALIB_DLL
FTValue::ScanStates  ScanFiles( FTree&               tree,
                                ScanParameters&      parameters,
                                CanonicalPathList*   resultPaths         = nullptr,
                                Path*                remainingStart      = nullptr   );

/// Classification of path root formats that cannot be directly scanned
enum class PathRootKind {
    Errorneous,     ///< Errorneous path format.
    Relative,       ///< Relative path (no special root)
    AbsoluteRoot,   ///< Unix-style absolute path starting with /
    DriveLetter,    ///< Windows drive letter (C:, D:, etc)
    UNC,            ///< Universal Naming Convention (\\\\server\\share)
    URL,            ///< URL scheme (http://, ftp://, file://, etc)
    Device,         ///< Windows device path (\\\\.\\...)
};

/// Result information from MakeCanonical
struct CanonicalResult {
    PathRootKind              RootKind;     ///< What kind of path root was detected
    FileStatus::ScanStates    ScanState;    ///< Result of canonicalization/scanning
};

/// Analyses the given \p{sourcePath} and converts it to its canonical version.
/// This is similar to what the POSIX function <c>realpath()</c> and C++
/// <c>std::filesystem::canonical</c> do.<br>
/// This version, in addition, creates corresponding nodes in the #"FTree" (passed indirectly with
/// the parameter \p{node}). Besides removing <c>"."</c> and <c>".."</c> entries, symbolic links
/// are not only resolved, but the nodes they are targeting receive information about the link
/// that targeted them. This information is set with the method #"FTFile::SetSymbolicParent(FTFile)".
/// With that, the path of directories or files that are children of such a targeted node, can
/// re-establish the file-path as originally specified. This is done with the method
/// #"FTFile::AssembleSymbolicPath".
///
/// ## Path Root Detection and Tree Representation
/// The method detects and handles various path root formats, creating appropriate tree nodes:
///
/// - <b>Relative paths</b> (#".PathRootKind::Relative"):<br>
///   No special root. The path is resolved relative to the provided \p{node}.<br>
///   Example: <c>"foo/bar"</c>
///
/// - <b>Absolute paths</b> (#".PathRootKind::AbsoluteRoot"):<br>
///   Unix-style absolute paths starting with <c>'/'</c>.<br>
///   Tree node: positioned at tree root with path <c>'/'</c>.<br>
///   Example: <c>"/usr/local/bin"</c> → root node
///
/// - <b>URL schemes</b> (#".PathRootKind::URL"):<br>
///   Format: <c>scheme://</c> where scheme is alphanumeric with <c>+</c>, <c>-</c>, or <c>.</c>.<br>
///   Tree node: child of root named with the scheme (e.g., <c>http</c>, <c>ftp</c>).<br>
///   The node is marked as type #"Types::SOCKET" with state
///   #"ScanStates::NOT_EXISTENT" (unscannable virtual node).<br>
///   Examples: <c>http://example.com</c> → node <c>http</c>,
///             <c>file://path</c> → node <c>file</c>
///
/// - <b>Windows drive letters</b> (#".PathRootKind::DriveLetter"):<br>
///   Format: <c>C:</c>, <c>C:\\</c>, or <c>C:/</c>.<br>
///   Tree node: child of root named with the drive letter (e.g., <c>C:</c>).<br>
///   Examples: <c>C:\\Windows</c> → node <c>"C:"</c>, <c>"D:/data"</c> → node <c>"D:"</c><br>
///   (Windows only.)
///
/// - <b>UNC paths</b> (#".PathRootKind::UNC"):<br>
///   Universal Naming Convention for network shares: <c>\\\\</c> or <c>//</c>.<br>
///   Tree node: positioned at the root with path <c>//</c>.<br>
///   Examples: <c>\\\\server\\share</c> or <c>//server/share</c> → root node with <c>//</c>.<br>
///   (Windows only.)
///
/// - <b>Windows device paths</b> (#".PathRootKind::Device"):<br>
///   Format: <c>\\\\.\\...</c> or <c>//./...</c>.<br>
///   Tree node: child of root with synthesized name starting with <c>DEV</c>.<br>
///   The remaining path is encoded with colons replacing separators.<br>
///   The node is marked as type #"Types::SOCKET" with state
///   #"ScanStates::NOT_EXISTENT" (unscannable virtual node).<br>
///   Examples: <c>\\\\.\\C:</c> → node <c>DEV:C:</c>,
///             <c>\\\\.\\UNC\\server</c> → treated as UNC after normalization.<br>
///   (Windows only.)
///
/// \note
///   The detected #".PathRootKind" is returned in the result and should be preserved by callers
///   if they need to distinguish between root types later, as the node name alone is not always
///   sufficient to unambiguously identify the root kind (e.g., a drive letter <c>C:</c> vs.
///   a scheme <c>c:</c> from URL <c>c://...</c>).
///
/// \note
///   This function is used by the function #"ScanFiles" to resolve the start path and symbolic
///   link targets.
///
/// @param[in,out] sourcePath  The path to scan. This might contain <c>"."</c> and <c>".."</c>
///                            directories, as well as symbolic links.
///                            When the method exits successfully, this path is empty.
///                            Otherwise, this path-string contains the remaining
///                            path, starting with the name of the file or directory that could
///                            not be found, accessed, or otherwise be resolved.
/// @param[in,out] node        The starting node. In case the parameter \p{sourcePath} contains
///                            a root specification (absolute path, URL, etc.), this node is moved
///                            to the appropriate position in the tree (often the root).<br>
///                            When the method exits successfully, this cursor targets the
///                            file that the source path resolved to.<br>
///                            In case of failure, this cursor becomes
///                            #"TCursor::IsInvalid;invalid".
/// @param[in,out] pathToNode  This path has to point to the given \p{node} when the method is
///                            called. When the method returns, it contains the canonical path
///                            to the then repositioned \p{node}.
/// @param[in,out] resultPaths Optional pointer to a #".CanonicalPathList" that receives nodes
///                            for newly created paths during resolution.
/// @return A #"CanonicalResult" containing:
///         - #"CanonicalResult::RootKind": The detected path root format
///         - #"CanonicalResult::ScanState": The scan state indicating success or failure:<br>
///           #"ScanStates::NONE" (new node created),
///           #"ScanStates::DUPLICATE" (already existed),
///           #"ScanStates::STATS" or #"ScanStates::RESOLVED" (successfully scanned),
///           #"ScanStates::NOT_EXISTENT" (path not found), or
///           other error states.
CanonicalResult  MakeCanonical(  Path&               sourcePath,
                                 FTree::Cursor&      node,
                                 Path&               pathToNode,
                                 CanonicalPathList*  resultPaths= nullptr   );

} // namespace alib[::filetree]


/// Type alias in namespace #"%alib".
using     ScanParameters    =   filetree::ScanParameters;

/// Type alias in namespace #"%alib".
using     CanonicalPathList =   filetree::CanonicalPathList;

}  // namespace [alib]

//==================================================================================================
/// \file
/// This header-file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace files {

#if ALIB_DEBUG
/// The format string used with verbose logging to domain <c>/ALIB/FILES/SCAN</c> during
/// with namespace function \alib{files;ScanFiles}.<br>
/// Defaults to <c>" {:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq nf l}"</c>
extern String DBG_FILES_SCAN_VERBOSE_LOG_FORMAT;
#endif


/// Input parameters to function #ScanFiles.
struct ScanParameters
{
    /// Options for processing symbolic links.
    enum class SymbolicLinks
    {
        DONT_RESOLVE            = 0, ///< Demands \b not to resolve symbolic links in any way.
        RESOLVE_BUT_DONT_FOLLOW = 1, ///< Demands to read symbolic links, but not follow linked directories.
                                     ///< FInfo dates, sizes, and access rights are set according to
                                     ///< the link target.
        RECURSIVE               = 2, ///< Read symbolic links and in case they are targeting a
                                     ///< directory, recurse into, if this directory meets the
                                     ///< other constraints associated with the current scan.
    };

    /// Denotes 'infinite' recursion if set to field #MaxDepth.
    static constexpr unsigned InfiniteRecursion = (std::numeric_limits<unsigned>::max)();

    /// The path to be scanned.
    Path            StartPath;

    /// Denotes how symbolic links are treated.
    SymbolicLinks   LinkTreatment                                         =SymbolicLinks::RECURSIVE;

    /// The maximum recursion depth. Defaults to #InfiniteRecursion.
    unsigned        MaxDepth                                                     =InfiniteRecursion;

    /// If \c true, the default, scanning does not stop recursion on directories which represent
    /// a mounted filesystem. If \c false, the search is restricted to the device that #StartPath
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
    /// Directories are either real directories, or in case the field #LinkTreatment is set to
    /// \alib{files::ScanParameters;SymbolicLinks::RECURSIVE}, symbolic links that
    /// target a directory.
    ///
    /// \see Optional filters #DirectoryFilterPreRecursion and #DirectoryFilterPostRecursion.
    SPFileFilter    FileFilter;

    /// If set (not containing \c nullptr), this filter is invoked \b after a recursive scan of
    /// a directory. If \c false is returned, the recursion is not performed, but the (empty)
    /// directory remains in the result list, if field #RemoveEmptyDirectories evaluates to
    /// \c false.<br>
    /// Note that in case field #LinkTreatment is set to
    /// \alib{files::ScanParameters;SymbolicLinks::RECURSIVE}, this filter
    /// is also applied to symbolic links, which are readable, not broken, and target a directory.
    ///
    /// \note
    ///   Directories (and symbolic links to directories) are first recursively scanned before this
    ///   filter is applied. On deletion, of course the whole scanned subtree is deleted.
    ///   This allows filtering directories, depending on information available only after
    ///   scanning, hence by the numbers retrieved with \alib{files;FInfo::Sums}.
    ///   To increase performance and filter directories \e before their recursive scan,
    ///   alternative field #DirectoryFilterPreRecursion is to be used.
    ///
    /// \see Optional filters #DirectoryFilterPreRecursion and #FileFilter.
    ///
    SPFileFilter    DirectoryFilterPostRecursion;

    /// Same as #DirectoryFilterPostRecursion but is used \b before a recursive scan of
    /// a directory. Consequently, this filter leads to much higher scan performance than the
    /// alternative version, because huge branches of the file system might be omitted during scan.
    /// However, the numbers retrieved with \alib{files;FInfo::Sums} will all indicate
    /// \c 0, because no information is retrieved.<br>
    /// If a directory is "pruned" due to this filter, the entry still occurs in the \b %FTree,
    /// unless field #RemoveEmptyDirectories evaluates to \c true.<br>
    ///
    /// \see Optional filters #DirectoryFilterPostRecursion and #FileFilter.
    ///
    SPFileFilter    DirectoryFilterPreRecursion;

    /// Constructor accepting all features.
    /// @param startPath             Stored in field #StartPath.
    /// @param linkTreatment         Stored in field #LinkTreatment. Defaults to \b SymbolicLinks::RECURSIVE.
    /// @param maxDepth              Stored in field #MaxDepth. Defaults to +InfiniteRecursion.
    /// @param crossFileSystems      Stored in field #CrossFileSystems. Defaults to \c true.
    /// @param includeArtificialFS   Stored in field #IncludeArtificialFS. Defaults to \c false.
    ScanParameters( const system::PathString& startPath,
                    SymbolicLinks   linkTreatment      = SymbolicLinks::RECURSIVE,
                    unsigned        maxDepth           = InfiniteRecursion,
                    bool            crossFileSystems   = true,
                    bool            includeArtificialFS= false                        )
    : StartPath          (startPath          )
    , LinkTreatment      (linkTreatment      )
    , MaxDepth           (maxDepth           )
    , CrossFileSystems   (crossFileSystems   )
    , IncludeArtificialFS(includeArtificialFS)                                                    {}

}; // struct ScanParameters

/// A simple triple of a path string, a corresponding \b FTree node, and a boolean to indicate
/// whether the path existed already. This struct is used as an output parameter of function
/// #ScanFiles.
struct ResultsPaths
{
    system::Path RealPath;  ///< The 'real' absolute path to the node (no symbolic links included).
    FTree::Cursor      Node;      ///< The node in the tree representing this path.
    bool               Existed;   ///< If \c true, the complete path existed already in the target
                                  ///< \alib{files;FTree}. However, it might have existed as
                                  ///< another result path, hence not recursively scanned fashion and
                                  ///< now is rescanned with 'higher' scan quality.

    /// Constructor.
    /// @param realPath  Stored in field #RealPath.
    /// @param node      Stored in field #Node.
    /// @param existed   Stored in field #Existed.
    ResultsPaths(const system::PathString& realPath, FTree::Cursor node, bool existed )
    : RealPath(realPath)
    , Node    (node)
    , Existed (existed)                                                                           {}
};

#if DOXYGEN
/// ### General Information ###
/// Scans the filesystem according to the given \b ScanParameters and adds \alib{files;FInfo}
/// entries to the given \alib{files;FTree}.
///
/// ### ALib FTree Data Contract ###
/// This function has a contract with the class \alib{files;FTree} that is used to store the scan
/// results.
/// This contract states that any file or directory found during a scan is always stored using
/// the <em>"Real Path"</em> of the entry. This means that any symbolic link is resolved.
/// The consequences are:
/// - %Files and directories which represent a symbolic link are always "leaf nodes".
///   (They never contain child nodes.). However, their symlink target path is attached twice
///   to the entry:
///   1. The original link information given, which often uses relative path addressing.
///   2. The absolute, <em>"Real Path"</em> of the target, which has a corresponding result entry
///      in the given \b %FTree.
/// - If a using software wants to use symbolic paths, for example, to present them to the end
///   user, such paths have to be assembled by the user's code in own responsibility.
///   All information for doing this is provided in the resulting tree object
/// - Doubly linked target files and directories are never a problem for this scanner. Each
///   file is scanned only once. This especially prevents all sorts of problems that would otherwise
///   occur with cyclic symbolic links.
/// - Due to this, even the given start path of a search might not be found as a result
///   in the given \b %FTree, because also start paths are converted to a <em>Real Path</em>.
/// - The scan result may contain more than one resulting path. This happens, if a symbolic link
///   targets a file or directory that is not recursively included in the start path.
///   The resulting <em>"Real Path"</em> of the given start path is however always the first
///   result added.
///
/// The latter is reflected with parameter \p{resultPaths} of this function, which is defined
/// as a <c>std::vector</c>.
///
/// \note
///   As class \alib{files;FTree} is based on class \alib{containers;StringTree}, using code
///   is enabled to break this contract by adding entries below symbolic links.
///   Other entities of this \alibmod_nl will not break this contract.
///
/// ### Rescanning of Entries ###
/// Existing entries in the given \p{tree} are not overwritten. They might be scanned with "higher"
/// \alib{files;FInfo::Qualities} values, depending on given \p{parameters} and how they had been
/// scanned before. If the same "level" of scanning is provided,  existing entries will not be
/// scanned again. If a rescan of a certain path is wanted, then the target entry of that path has
/// to be deleted before invoking this function. Due to the implementation of class FTree, repeated
/// delete and scan operations will not cause any heap-memory activities (of course, as long as no
/// are detected which have been created meanwhile).
///
/// ### platform-dependent Code Selection ###
/// File scanning is a platform-dependent task and hence \b ALib uses one of two different
/// implementations:
/// 1. A posix version for posix compatible OSes,
/// 2. A version that relies on <c>C++ std::filesystem</c>.
///
/// The fallback version using <c>std::filesystem</c> has the following restrictions:
/// - The only time attribute available is the \alib{files;FInfo::MDate;modification time} of
///   an entry. Fields \alib{files::FInfo;BDate}, \alib{files::FInfo;CDate}, and
///   \alib{files::FInfo;ADate} are always set to the same as the
///   modification time, even on filesystems that support the other values.
/// - The file time of symbolic links is \b always that of the target file. The C++ standard has
///   no possibility to access the link's time itself.
/// - The file time of broken symbolic links is set to the current time (time of scanning).
/// - The size that directories occupy on disk cannot be determined.
///   Directory entries always report size <c>0</c>.
/// - The target of a symbolic link which points to a non-accessible directory, cannot be resolved
///   to a 'real' path, even if all other path components before are accessible.
///   (This is true for the implementation of the standard library under GNU/Linux and Clang compiler
///   at the time of writing this, 2024/02.)
/// - Flag \alib{files;ScanParameters::CrossFileSystems} is ignored. Crossing Filesystems cannot
///   be detected using purely the standard library.
/// - A files' owner and owning group is not determined. Instead, \alib{files;FInfo::UnknownID} is
///   set for both.
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
/// @param tree        The tree to fill.
/// @param parameters  The input parameters to determine the scan process.
/// @param resultPaths A container to add the resulting list of 'real' paths and corresponding start
///                    nodes found during the search. The first entry added by this function is
///                    always the 'real'-version of field \alib{files::ScanParameters;StartPath}
///                    of the given \p{params} struct. Further paths/nodes pairs are created when
///                    symbolic links are found and followed.
/// @param lock        Pointer to an (optional) \alib{threads;SharedLock}.
///                    The overloaded version of this function that accepts \alib{SharedFTree}
///                    sets this to the instance found in the shared tree.<br>
///                    This parameter is available (and to be passed) only if the module
///                    \alib_threads is included in the \alibbuild.
///
/// @return Scan quality code of the tree node of the first resulting path, hence of the node
///         referred to by \alib{files;ScanParameters::StartPath}.<br>
///         On error, i.e the start path was invalid, not accessible, a broken link, a circular
///         link, or other, then \alib{files::FInfo;Qualities;Qualities::NOT_EXISTENT} is returned.
///         etc.
ALIB_DLL
FInfo::Qualities  ScanFiles( FTree&                      tree,
                             ScanParameters&             parameters,
                             std::vector<ResultsPaths>&  resultPaths,
                             SharedLock*                 lock           );
#else
    ALIB_DLL
FInfo::Qualities  ScanFiles( FTree&                      tree,
                             ScanParameters&             parameters,
                             std::vector<ResultsPaths>&  resultPaths
          IF_ALIB_THREADS( , SharedLock*                lock)           );
#endif

/// Invokes #ScanFiles( FTree&, ScanParameters&, std::vector<ResultsPaths>&,SharedLock*)
/// passing the lock included in the given \alib{SharedFTree} as parameter \p{lock}.
///
/// @param tree        The shared tree to fill.
/// @param parameters  The input parameters to determine the scan process.
/// @param resultPaths The result paths.
/// @return Scan quality code of the first resulting path.
inline
FInfo::Qualities  ScanFiles( SharedFTree&                tree,
                             ScanParameters&             parameters,
                             std::vector<ResultsPaths>&  resultPaths )
{ return ScanFiles(*tree,parameters,resultPaths IF_ALIB_THREADS(, &tree.GetLock()) ); }

} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     ScanParameters    =   files::ScanParameters;

/// Type alias in namespace \b alib.
using     ResultsPaths      =   files::ResultsPaths;

}  // namespace [alib]

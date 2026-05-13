//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace filetree {

class   FTree;
class   FTFile;
template<typename TLock>
struct  TSharedFTree;

/// This namespace implements internals of namespace #"alib::filetree;2".
namespace detail {

struct FTreeNodeHandler;

/// A shortcut to the base class of the base class of class #"FTree".
using TTree= containers::detail::StringTreeBase< MonoAllocator,
                                                 FTValue,
                                                 FTreeNodeHandler,
                                                 Recycling::Private>;
                                              
/// Specialized #"alib_ns_containers_stringtree_referencedoc;TNodeHandler" for class
/// #"FTree" which recycles extended information objects of type
/// #"FTValue::EIDirectory;*", #"FTValue::EISymLinkFile;*", and
/// #"FTValue::EISymLinkDir;*" with node deletion.
///
/// In respect to the node name allocation, this type behaves like
/// default handler #"StringTreeNamesDynamic".
/// In debug compilations, statistics
/// variables #"DBG_STATS_STRINGTREE_NAMES" and
/// #"DBG_STATS_STRINGTREE_NAME_OVERFLOWS" are increased, just like the original
/// does.
struct FTreeNodeHandler {
    /// The character type that the #"%StringTree" uses for child name and path strings.
    /// This is taken from the C++ standard library.
    using CharacterType=  PathCharType;

    /// The string-type of a node's name. This is a simple static string, allocated with the
    /// pool allocator.
    using NameStringType= PathString;

    /// Copies the node's name to the local string.
    ///
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @param  tree  The instance of struct #"detail::StringTreeBase;*" that invokes
    ///               this method. Any member may be accessed, including
    ///               #"StringTreeBase;nodeTable" which contains the
    ///               #"MonoAllocator" that the tree uses for the allocation of nodes.
    static
    inline
    void InitializeNode( TTree::Node& node, TTree& tree );

    /// This implementation frees any dynamically allocated memory of the node's name and in
    /// addition recycles any extended information object attached to the #"FTValue"
    /// object.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @param  tree  The instance of struct #"detail::StringTreeBase;*" that invokes
    ///               this method. Any member may be accessed, including
    ///               #"StringTreeBase;nodeTable" which contains the
    ///               #"MonoAllocator" that the tree uses for the allocation of nodes.
    static
    inline
    void FreeNode( TTree::Node& node, TTree& tree );
    
    /// Implements #"FTree::AllocateExtendedInfo;*".
    /// @param node             The node add extended information to.
    /// @param symLinkDest      In case of symbolic link types, the symbolic link target.
    /// @param symLinkRealPath  In case of symbolic link types, the symbolic link target as real path.
    ALIB_DLL
    static
    void AllocateExtendedInfo( StringTree<MonoAllocator, FTValue, FTreeNodeHandler>::Cursor& node,
                               const PathString& symLinkDest,
                               const PathString& symLinkRealPath      );
};  // struct FTreeNodeHandler

} // namespace alib::filetree[::detail]


//==================================================================================================
/// Abstract virtual interface type to implement types observing changes in instances of class
/// #"FTree".
/// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
//==================================================================================================
struct FTreeListener {
    /// The type of change that imposes the notification of a listener.
    enum class Event {
        CreateNode,     ///< A file or directory entry was created.
        DeleteNode,     ///< A file or directory entry was deleted.
    };

    /// Virtual destructor.
    virtual ~FTreeListener()                                                                      {}

    /// The virtual notification method.
    /// @param file   The file or directory that was modified.
    /// @param event  The type of modification.
    virtual void    Notify( FTFile& file, Event event )                                          =0;

}; // struct FTreeListener


//==================================================================================================
/// This class builds on \alib type #"StringTree".
/// The contained elements (tree nodes) are of type#"FTValue" and represent entries in
/// filesystems.
/// Usually the tree is filled using the function #"ScanFiles(FTree&)".
///
/// ### StringTree Interface ###
/// Public base class #"StringTree" provides all interfaces necessary to create and
/// delete entries, iterate and recursively walk the file tree. Please consult its documentation
/// for further information.
///
/// \note As #"ScanFiles(FTree&);documented with function ScanFiles", entities of this
///       module \alib_filetree_nl exclusively store entries along their <em>"Real Path"</em>, hence
///       always resolving symbolic links. A user of this library may deviate from this
///       "data contract".
///
/// ### Class FTFile ###
/// The base classes' method #"StringTree::Root;*" is overloaded by this class
/// and returns an instance of class #"filetree::FTFile" instead of an instance of class
/// alib{containers;StringTree::Cursor}.
/// This class can be turned into a cursor using #"FTFile::AsCursor" and then used to
/// navigate through the tree. Then, the cursor can be cast back (or assigned) to a #"%FTFile"
/// instance.
///
/// ### Monotonic Behavior ###
/// The class fulfills #"alib_contmono_intro_strictweak;weak monotonic allocation requirements",
/// which is achieved by recycling not only the nodes (what base type #"%StringTree" does by default)
/// but also any extended node information. This is implemented with the <b>StringTree</b>
/// handler-type #"detail::FTreeNodeHandler;*" on the one hand, and on the other hand,
/// with the implementation of the method #".AllocateExtendedInfo", which has to be exclusively used
/// to attach information structs on #"%FTValue" elements.<br>
/// Furthermore, class #"%FTFile" (the nodes of the tree) provides method
/// #"FTFile::AttachCustomData", which likewise uses the internal pool-allocator.
///
/// In summary, this allows an indefinite sequence of file-scan and result filtering (deletions)
/// with using more memory than the highest resulting fill state in such sequence requires.
//==================================================================================================
class FTree : public StringTree<MonoAllocator, FTValue, detail::FTreeNodeHandler> {
    friend struct FTreeNodeHandler;
    
  public:
    /// An object pool used for recycling all sorts of allocated objects as well as the
    /// hashtable entries.
    /// It is #"alib_contmono_chaining;chained" to the allocator provided with construction.
    ///
    /// The pool may be used in accordance with the general rules imposed by camp \alib_monomem.
    /// If so, in multithreaded environments, this object has to be locked (in addition
    /// to all other custom locks when interfacing this type), when using this pool from custom
    /// code.
    PoolAllocator           Pool;
     
  protected:
    /// Type alias of this classes' base class.
    using base= StringTree<MonoAllocator, FTValue, detail::FTreeNodeHandler>;

    #if !DOXYGEN
    friend struct  detail::FTreeNodeHandler;                         ///< Friendship declaration.
    friend class   filetree::FTFile;                                      ///< Friendship declaration.
    #endif
    
    /// Formatting information used with #"FTFile::Format;format methods" of associated
    /// #"%FTFile" instances.
    NumberFormat              numberFormat;

    /// A caching owner and group resolver. Used with #"FTFile::Format;format methods"
    /// of associated #"%FTFile" instances.
    OwnerAndGroupResolver     ogResolver;

    /// Record used to manage registered listeners.
    struct ListenerRecord {
        FTreeListener*       listener;      ///< The listener to register or dispose.
        FTreeListener::Event event;         ///< The event to listen to.
        ConstCursorHandle    file;          ///< If given, the files to listen to.
        ConstCursorHandle    subTree;       ///< If given, the path of files to listen to.
        PathStringPA         fileName;      ///< If given, the file's name to listen to.
        PathStringPA         pathPrefix;    ///< If given, the start string of the file path
                                            ///< to monitor.
        PathStringPA         pathSubstring; ///< If given, the substring to match in the path
                                            ///< (including the file name) of files to monitor.
    };

    /// The list of registered listeners.
    ListMA<ListenerRecord>   listeners;

    /// Implements the various overloaded listener registration methods.
    /// @param listener         The listener to register or dispose.
    /// @param insertOrRemove   Denotes registration or disposal of a listener.
    /// @param event            The event to listen to.
    /// @param file             If given, the exported value of the file to listen to.
    /// @param subTree          If given, the exported value of the subtree of files to listen to.
    /// @param fileName         If given, the file's name to listen to.
    /// @param pathPrefix       If given, the start string of the file path to monitor.
    /// @param pathSubstring    If given, the substring to match in the path (including the file name)
    ///                         of the files to monitor.
    ALIB_DLL  void registerListener( FTreeListener*             listener,
                                     lang::ContainerOp          insertOrRemove,
                                     FTreeListener::Event       event,
                                     const FTFile*              file,
                                     const StringTree::Cursor*  subTree,
                                     const PathString&          fileName,
                                     const PathString&          pathPrefix,
                                     const PathString&          pathSubstring  );

    /// Notifies registered listeners on events.
    /// @param event    The event that occurred.
    /// @param file     The file.
    /// @param filePath The full path of the file. Might be nulled if not available, yet.
    ALIB_DLL  void notifyListeners(  FTreeListener::Event event,
                                     FTFile&              file,
                                     const PathString&    filePath );

  public:
    /// Constructor.
    /// @param allocator The allocator to use.
    ALIB_DLL
    FTree( MonoAllocator& allocator );

    /// Destructor.
    ALIB_DLL
    ~FTree();

    /// Sort of 'overloads' method #"StringTree::Root;*", which otherwise is accessible
    /// via <b>operator-></b> inherited by parent class #"TSharedMonoVal".
    /// In contrast to the inherited method, this version returns an instance of type #"%FTFile".
    /// @return A file-cursor pointing to the root node of this file tree.
    inline
    FTFile            Root();

    /// Allocates (or recycles) an appropriate information object fitting to the type of this entry.
    /// This method must only be applied to entries of types
    /// - #"Types::DIRECTORY",
    /// - #"Types::SYMBOLIC_LINK" or
    /// - #"Types::SYMBOLIC_LINK_DIR".
    /// In debug compilations, this is asserted. It is likewise asserted that the sybolic link
    /// information strings are empty in case the type is #"Types::DIRECTORY".
    ///
    /// @param node            The node add extended information to.
    /// @param symLinkDest     In case of symbolic link types, the symbolic link target.
    /// @param symLinkRealPath In case of symbolic link types, the symbolic link target as real
    ///                        path.
    void           AllocateExtendedInfo( Cursor& node, const PathString& symLinkDest,
                                                       const PathString& symLinkRealPath )
    { detail::FTreeNodeHandler::AllocateExtendedInfo( node, symLinkDest, symLinkRealPath); }

    /// Deletes all custom data objects attached to any #"%FTFile" in this tree.<br>
    /// Note that this method is only applicable if all custom data objects set in any node
    /// of this tree share the same type \p{TCustom}.
    /// With debug-compilations this is asserted.
    ///
    /// @see Method #"FTFile::AttachCustomData;*".
    /// @tparam TCustom The object type to optionally store in tree nodes.
    template<typename TCustom>
    void DeleteAllCustomData() {
        for( auto& node : nodeTable ) {
            if( node.data.custom ) {
                ALIB_ASSERT_ERROR( &typeid(TCustom) == node.data.dbgCustomType, "FILETREE",
                  "CustomData to delete does not match attached type.\n"
                  "Deletion has to be performed individually by this software.\n"
                  "This method must only be used if all tree nodes have the same custom data "
                  "attached\n"
                  "  Attached type: <{}>\n"
                  "     Given type: <{}>"   , &typeid(TCustom), node.data.dbgCustomType )

                static_cast<TCustom*>( node.data.custom ) -> ~TCustom();
                Pool.free( node.data.custom, sizeof(TCustom) );
                node.data.custom= nullptr;
      ALIB_DBG( node.data.dbgCustomType= nullptr; )
    }   }   }

    /// Recalculates the sums of the given node. This is \b not done recursively. The fix is needed
    /// when scanning an existent directory with potentially more greedy scan parameters.
    /// @param directory  The directory to re-calculate the sums for.
    ALIB_DLL
    static
    void            FixSums(Cursor directory);

    /// Retrieves formatting flags which are used with method #"FTFile::Format;*".
    /// @return Number formatting information for #"%FTFile" objects associated with this file tree.
    NumberFormat&   GetNumberFormat()                                       { return numberFormat; }

    /// Retrieves formatting flags which are used with method #"FTFile::Format;*".
    /// @return Number formatting information for #"%FTFile" objects associated with this file tree.
    const NumberFormat&   GetNumberFormat()                           const { return numberFormat; }

    /// Retrieves formatting flags which are used with method #"FTFile::Format;*".
    /// @return Number formatting information for #"%FTFile" objects associated with this file tree.
    const OwnerAndGroupResolver&   GetOGResolver()                      const { return ogResolver; }

  //===================================== Listener Registration ====================================


    /// Notifies registered listeners on events.
    /// @param event    The event that occurred.
    /// @param file     The file.
    /// @param filePath The full path of the file. Might be nulled if not available to the caller.
    ///                 In this case it is internally created.<br>
    ///                 Defaults to #"NULL_STRING".
    inline
    void Notify( FTreeListener::Event   event,
                 FTFile                 file ,
                 const PathString&      filePath= NULL_PATH  );

    /// @return \c true if listeners are registered with this file tree, \c false otherwise
    bool            HasListeners()                                  { return listeners.size() > 0; }

    /// Inserts or removes a listener to a specific file.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements #"%ContainerOp::Insert" or #"%ContainerOp::Remove"
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param file           The file to listen to.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    void MonitorDistinctFile( lang::ContainerOp       insertOrRemove,
                              FTreeListener*          listener,
                              FTreeListener::Event    event,
                              const FTFile&             file       ) {
        ALIB_ASSERT_WARNING( event != FTreeListener::Event::CreateNode, "VARIABLES",
             "Event::Creation will never be invoked with this listener-registration-type." )
        registerListener( listener, insertOrRemove, event, &file, nullptr,
                          NULL_PATH, NULL_PATH, NULL_PATH );
    }

    /// Inserts or removes a listener for all files that share the given \p{fileName}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements #"%ContainerOp::Insert" or #"%ContainerOp::Remove"
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param fileName       The name of one or more files to listen to.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    void MonitorFilesByName( lang::ContainerOp    insertOrRemove,
                             FTreeListener*       listener,
                             FTreeListener::Event event,
                             const PathString&    fileName ) {
        ALIB_ASSERT_ERROR( fileName.IsNotEmpty(), "VARIABLES", "Empty file name given." )
        registerListener( listener, insertOrRemove, event, nullptr, nullptr, fileName,
                          NULL_PATH, NULL_PATH );
    }


    /// Inserts or removes a listener for all files below the subtree specified by the
    /// given \p{cursor}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements #"%ContainerOp::Insert" or #"%ContainerOp::Remove"
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param cursor         The parent node in the tree of files to monitor.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    void MonitorPath(  lang::ContainerOp       insertOrRemove,
                       FTreeListener*          listener,
                       FTreeListener::Event    event,
                       const FTree::Cursor&    cursor            ) {
        registerListener( listener, insertOrRemove, event, nullptr, &cursor,
                          NULL_PATH, NULL_PATH, NULL_PATH );
    }

    /// Inserts or removes a listener for all files below the subtree specified by the
    /// given \p{startPath}.
    /// \attention Note that the parameter \p{pathPrefix} has to be a portion of a
    ///            #"Path::;real path".
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements #"%ContainerOp::Insert" or #"%ContainerOp::Remove"
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathPrefix     The path prefix of the subtree of files to monitor. Note that a
    ///                       missing leading separator character will be added.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    void MonitorPathPrefix( lang::ContainerOp     insertOrRemove,
                            FTreeListener*        listener,
                            FTreeListener::Event  event,
                            const PathString&     pathPrefix      ) {
        ALIB_ASSERT_ERROR( pathPrefix.IsNotEmpty(), "VARIABLES", "Empty path prefix given." )
        registerListener( listener, insertOrRemove, event, nullptr, nullptr,
                          NULL_PATH, pathPrefix, NULL_PATH );
    }

    /// Inserts or removes a listener for all files whose path (excluding the file name) contains
    /// the given \p{pathSubstring}.
    /// \attention Note that the parameter \p{pathSubstring} has to be a portion of a
    ///            #"Path::;real path".
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements #"%ContainerOp::Insert" or #"%ContainerOp::Remove"
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathSubstring  The substring to match in the path (including the file name)
    ///                       of the files to monitor.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    void MonitorPathSubstring( lang::ContainerOp    insertOrRemove,
                               FTreeListener*       listener,
                               FTreeListener::Event event,
                               const PathString&    pathSubstring  ) {
        ALIB_ASSERT_ERROR( pathSubstring.IsNotEmpty(), "VARIABLES", "Empty path substring given." )
        registerListener( listener, insertOrRemove, event, nullptr, nullptr,
                          NULL_PATH, NULL_PATH, pathSubstring );
    }

    /// Removes all registrations of the given listener.
    /// @param listener The listener to remove.
    /// @return The number of registrations that have been removed.
    /// @see Chapter #"alib_filetree_monitoring" of the Programmer's Manual of camp \alib_filetree_nl.
    ALIB_DLL
    int MonitorStop( FTreeListener*  listener );
}; // FTree


/// Utility type which implements #"TSharedMonoVal" with class #"FTree".
/// The result of combining both is an automatic pointer to a #"%FTree" that is "self-contained"
/// in the first buffer of a #"MonoAllocator" together with the allocator itself.
/// The tree is deleted and all associated memory is freed when the last copy of the pointer
/// goes out of scope.
///
/// Along with the #"%FTree", this shared object includes a #"SharedLock".
/// See chapter #"alib_contmono_smv_locking" of the Programmer's Manual of module \alib_monomem
/// for further information on how to protect the contents of this type against
/// thread-racing-conditions.
///
/// @tparam TLock The lock type passed to the template parameter of parent type
///               #"TSharedMonoVal" with the same name.<br>
///               With the inclusion of module \alib_threads in the \alibbuild, the type-alias
///               #"alib::SharedFTree;2" chooses type #"SharedLock".<br>
///               Otherwise, in case \alib is compiled without threading support, the alias chooses
///               <c>void</c>.<br>
///               If it is assured that no racing-conditions occur with shared instances in
///               multithreaded software, the using code may pass <c>void</c> here as well.
template<typename TLock>
struct TSharedFTree : monomem::TSharedMonoVal<FTree, HeapAllocator, TLock> {
    /// Exposed shortcut to the base type.
    using Base= monomem::TSharedMonoVal<FTree, HeapAllocator, TLock>;

    /// Constructs an empty instance, hence a cleared automatic pointer.
    TSharedFTree()                                                                         =default;

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c nullptr to values of this type,
    /// which clears the automatic pointer.
    TSharedFTree(std::nullptr_t)                                                         noexcept {}

    /// Constructor.
    /// Calls the constructor of parent #"%TSharedMonoVal" and then invokes
    /// #"TSharedMonoVal::ConstructT;*" passing the mono allocator that the
    /// parent creates this instance in.<br>
    /// Furthermore calls DbgCriticalSections to enable assertions to locked usage.
    /// @param initialBufferSizeInKB The initial size of memory buffers.
    ///                              Passed to the allocator given with parent class
    ///                              #"TSharedMonoVal".
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Passed to the allocator given with parent class
    ///                              #"TSharedMonoVal".
    ///                              Should be set to \c 200, to double the size with each
    ///                              allocation.
    ///                              Defaults to \c 200.
    TSharedFTree( size_t    initialBufferSizeInKB,
                  unsigned  bufferGrowthInPercent = 200  )
    : Base(initialBufferSizeInKB, bufferGrowthInPercent) {
        Base::ConstructT( Base::GetAllocator() );
        DbgCriticalSections(lang::Switch::On);
        ALIB_DBG(Base::GetAllocator().DbgName= "SharedFTree";)
    }

    /// Defaulted copy-assignment operator.
    /// @return A reference to <c>this</c>.
    TSharedFTree& operator=(const TSharedFTree&)                                           =default;

    /// Destructor.
    /// Calls #".DbgCriticalSections" to stop checking the integrated \p{TLock}.
    ~TSharedFTree()                                      { DbgCriticalSections(lang::Switch::Off); }

    #if DOXYGEN
    /// Enables or disables critical section checks between the contained \p{T} and the likewise
    /// contained \p{TLock}.<br>
    /// In case \p{TLock} equals <c>void</c> or if symbol #"ALIB_DEBUG_CRITICAL_SECTIONS" is not
    /// set, this method is empty (and its use is optimized out).
    /// @param onOff The switch.
    void DbgCriticalSections(lang::Switch onOff);
    #else
        template<typename TRequires= typename Base::LockType>
        requires( !std::same_as<TRequires, void> )
    void DbgCriticalSections(lang::Switch onOff) {
        #if ALIB_DEBUG_CRITICAL_SECTIONS
        if ( !Base::IsNulled() ) {
            if( onOff == lang::Switch::On ) {
                Base::Self().NodeTable().dcs                     .DCSLock= &Base::GetLock();
                Base::GetAllocator().DbgCriticalSectionsPH.Get()->DCSLock= &Base::GetLock();
                Base::Self().Pool                                .DCSLock= &Base::GetLock();
            }
            else {
                Base::Self().NodeTable().dcs                     .DCSLock= nullptr;
                Base::GetAllocator().DbgCriticalSectionsPH.Get()->DCSLock= nullptr;
                Base::Self().Pool                                .DCSLock= nullptr;
        }   }
        #else
            (void) onOff;
        #endif
    }

        template<typename TRequires= typename Base::LockType>
        requires std::same_as<TRequires, void>
    void DbgCriticalSections(lang::Switch)                                                        {}
    #endif

    /// Clears all scanned or otherwise inserted data and re-initializes this object to its
    /// constructor defaults and resets the #"%MonoAllocator" of the parent class.<br>
    ///
    /// All shared instances remain valid (while, of course, their content is likewise reset).
    void    Reset() {
        // just invoke parent's reset method passing the mono allocator to the constructor.
        DbgCriticalSections(lang::Switch::Off);
        Base::Reset(Base::GetAllocator());
        DbgCriticalSections(lang::Switch::On);
    }

}; // struct TSharedFTree

//==================================================================================================
//================================ Implementation of the node mainer ===============================
//==================================================================================================
#if !DOXYGEN
#include "ALib.Lang.CIFunctions.H"

void detail::FTreeNodeHandler::InitializeNode( TTree::Node& node, TTree& tree )
{ node.name.storage.Allocate( static_cast<FTree&>(tree).Pool, node.name.key ); }

void detail::FTreeNodeHandler::FreeNode( TTree::Node& node, TTree& tree ) {
    // delete node name
    auto& pool= static_cast<FTree&>(tree).Pool;

    if ( node.name.storage.Length() )
        pool.free( const_cast<TTree::CharacterType*>(node.name.storage.Buffer()),
                                              size_t(node.name.storage.Length()) * sizeof(TTree::CharacterType) );

    // recycle extended info structs
    FTValue& value= node.data;
    auto extendedInfo= value.GetExtendedInfo();
    if( extendedInfo == nullptr )
        return;

    if(  value.IsSymbolicLink() ) {
        // delete old values
        FTValue::EISymLinkFile& ei= *static_cast<FTValue::EISymLinkFile*>(extendedInfo);

        if( ei.RealTarget.Length() && ei.RealTarget.Buffer() != ei.Target.Buffer() )
            pool().Free( ei.RealTarget.Buffer(), ei.RealTarget.Length() + 1 );

        if(  ei.Target.Buffer() )
            pool().Free( ei.Target.Buffer(), ei.Target.Length() + 1 );

        if( value.Type() == FileStatus::Types::SYMBOLIC_LINK )
            pool().Delete( static_cast<FTValue::EISymLinkFile*>(extendedInfo) );
        else
            pool().Delete( static_cast<FTValue::EISymLinkDir*>(extendedInfo) );

        // clear to be able to check double use in debug mode
        ALIB_DBG( value.SetExtendedInfo(nullptr) );
        return;
    }

    ALIB_ASSERT_ERROR( value.Type() == FileStatus::Types::DIRECTORY,
      "FILETREE", "Given node is not a directory or symbolic link but still has extendedInfo set." )

    pool().Delete( static_cast<FTValue::EIDirectory*>(extendedInfo) );

    // clear to be able to check double use in debug mode
    ALIB_DBG( value.SetExtendedInfo(nullptr) );
}
#include "ALib.Lang.CIMethods.H"
#endif


//==================================================================================================
/// This class represents nodes in #"FTree" instances.
/// While class #"%FTree" is just a rather small wrapper around its base class
/// #"StringTree", this class #"%FTFile" is a wrapper around class
/// #"StringTree::Cursor;*".
/// With that, instances of this class are very lightweight and contain only two pointers: One
/// pointing to the #"%FTree" that an instance originates from, the second pointing to the
/// node in the tree. The node in the tree then contains a pointer to type #"FTValue" which is
/// an extension of the class #"FileStatus".
///
/// It is important to understand that with this architecture, this class has three interfaces.
/// 1. The direct interface as exposed with this class.
/// 2. The interface reached with <c>operator-></c> which indirects to the embedded #"%FTValue"
///    data.
/// 3. The interface of class #"^StringTree::Cursor", which is accessible with method #".AsCursor"
///
/// To get a thorough understanding of why this split exists and what purpose which of the
/// three interfaces serve, a basic understanding of container type class
/// #"StringTree" is very helpful.
/// A similar design principle is implemented with class #"Variable" of module
/// \alib_variables. A technical explanation to why base class <b>FTree::Cursor</b> is protected
/// there in the same fashion is #"Variable::AsCursor;given here".
///
/// @see
///   - For a quick tutorial about using \alib files, consult the tutorial-style
///     #"alib_mod_filetree;Programmer's Manual" of camp \alib_filetree_nl.
///   - For this class, a #"alibtools_debug_helpers_gdb;pretty printer" for the
///     GNU debugger is provided.
///   - Instances of this type are #"alib_strings_assembly_ttostring;appendable" to
///     class #"%AString". If done, the full path and file name is written to the target string.
//==================================================================================================
class FTFile : protected FTree::Cursor {
  public:
    /// The base cursor type of the internal #"%StringTree". This type is used to perform
    /// cursor operations on #"%FTree" instances.
    using Cursor= FTree::Cursor;

    /// The constant version of type #".Cursor".
    using ConstCursor= FTree::ConstCursor;


    /// Returns a \c reference to the file tree that this file resides in.
    /// @return The associated file tree instance.
    FTree&            GetFTree()                      const { return  static_cast<FTree&>(Tree()); }

    /// Defaulted default constructor.
    FTFile()                                                                               =default;

    /// Constructor taking a file tree. After construction, this file will point to the root
    /// node <c>"/"</c> of the tree.
    /// @param pTree The tree to associate this file instance with.
    FTFile( FTree& pTree )
    : Cursor( pTree.Root() )                                                                      {}

    /// Constructs an instance of this type from its base type.
    /// This constructor is for advanced use when direct operations with class #"%StringTree" and
    /// its cursor and iterator types are performed.
    /// @param cursor The #"%StringTree" cursor representing a file.
    FTFile( const Cursor& cursor )
    : Cursor(cursor)                                                                              {}

    /// Comparison operator.
    /// @param other  The object to compare ourselves to.
    /// @return \c    true if this and the given cursor are equal, \c false
    ///               otherwise.
    bool operator==(const FTFile &other)   const { return  node == other.node && tree == other.tree; }

    /// Comparison operator.
    /// @param other  The object to compare ourselves to.
    /// @return \c    false if this and the given file are equal, \c true
    ///               otherwise.
    bool operator!=(const FTFile &other)                       const { return !((*this) == other); }

    /// Sets this \b Cursor to point to the same file (node in the #"FTree") as the given
    /// \p{other}.
    /// @param other  The node to let this file instance point to.
    /// @return A reference to \c this.
    FTFile& operator=( const Cursor& other )           { Cursor::operator=( other ); return *this; }

    /// Provides \c const access to members of contained #"FTValue" record. Note that
    /// access to a mutable version of the type is available with method #".GetMutableFInfo".
    /// @return A non-writable pointer to the embedded #"%FTValue" data.
    const FTValue*    operator->()                            const { return Cursor::operator->(); }

    /// Provides \c access to members of contained #"FTValue" record. Note that
    /// \c const access is available with method #".operator->".<br>
    /// Changes to the values should be done with caution. Usually the values are only set when
    /// scanning files or using certain interface methods of this class.
    /// @return A \b writable pointer to the embedded #"%FTValue" data.
    FTValue&          GetMutableFInfo()                                          { return Value(); }

    /// This is an explicit <c>cast operator</c> to the protected base class.
    /// \note For details on the code design which makes this method necessary, consult the
    ///       documentation of the same concept found with method #"Variable::AsCursor;*".
    /// @return This instance cast 'down' to its protected base class.
    Cursor&         AsCursor()                               { return static_cast<Cursor&>(*this); }

    /// \c const version of the <c>cast operator</c> to the protected base class.
    /// @return This instance cast 'down' to its protected base class.
    const Cursor&   AsCursor()                   const { return static_cast<const Cursor&>(*this); }

    /// Sets the symbolic parent in case it is not already set, or \p{overwrite} is given.
    /// \attention The tree node represented by the given cursor handle has to stay valid as long
    ///            as symbolic path assembly is used with this node.
    /// @see Manual chapter #"alib_filetree_tut_scan_realpath".
    /// @param handle      The exported cursor to the symbolic link that targets this file.
    /// @param overwrite   Advises setting the new value even if a different symbolic parent is
    ///                    already set. Defaults to \c false.
    void            SetSymbolicParent(FTree::CursorHandle handle, bool overwrite= false) {
        FTValue& value= Value();
        if(handle.value && (!value.symParent || overwrite))
            value.symParent= handle.value;
    }

    /// Sets the symbolic parent in case it is not already set, or \p{overwrite} is given.
    /// \attention The tree node \p{symbolicParent} has to stay valid as long as symbolic path
    ///            assembly is used with this node.
    /// @param symbolicParent  A symbolic link that targets this file.
    /// @param overwrite       Advises setting the new value even if a different symbolic parent is
    ///                        already set. Defaults to \c false.
    /// @see Manual chapter #"alib_filetree_tut_scan_realpath".
    void            SetSymbolicParent(FTFile& symbolicParent, bool overwrite= false)
    { SetSymbolicParent(symbolicParent.Export(), overwrite); }

    /// Clears the symbolic parent.
    /// @see Manual chapter #"alib_filetree_tut_scan_realpath".
    ///                    already set. Defaults to \c false.
    void            ClearSymbolicParent()                                  { Value().symParent= 0; }

    /// Tests if this file has a symbolic parent.
    /// @see Manual chapter #"alib_filetree_tut_scan_realpath".
    /// @return \c true if a symbolic parent is set, \c false otherwise.
    bool     HasSymbolicParent()                                  { return Value().symParent != 0; }

    /// Returns the symbolic parent detected when scanning the file.
    /// @see Manual chapter #"alib_filetree_tut_scan_realpath".
    /// @return If present, the symbolic parent that resolved this file.
    ///         Otherwise the "real" parent in the #"FTree".
    FTFile     GetSymbolicParent()            {
        uinteger handleValue= Value().symParent;
        return   handleValue ? FTFile(GetFTree().ImportCursor(FTree::CursorHandle(handleValue)))
                             : FTFile(AsCursor().Parent());
    }

    /// \c const version of #".GetSymbolicParent".
    /// @return If present, the symbolic parent that resolved this file.
    ///         Otherwise the "real" parent in the #"FTree".
    const FTFile     GetSymbolicParent()                                                     const {
        uinteger handleValue= Value().symParent;
        return   handleValue ? FTFile(GetFTree().ImportCursor(FTree::CursorHandle(handleValue)))
                             : FTFile(AsCursor().Parent());
    }



    // Publishes the protected method TCursor::Name.
    using Cursor::Name;

    // Publishes the protected method TCursor::IsValid.
    using Cursor::IsValid;

    // Publishes the protected method TCursor::IsInvalid.
    using Cursor::IsInvalid;

    /// Re-implements #"TCursor::Parent" to return the parent-file.
    /// @return A #"%C;FTFile" pointing to the parent folder of the file or folder represented by this.
    FTFile  Parent()                                             const { return TCursor::Parent(); }

    /// Returns the substring from the beginning of #"TCursor::Name" up to (and not including) the
    /// last period <c>'.'</c> character which is not located at the start of the name.
    /// With that, edge cases are treated as follows:
    /// - A filename like "filename.ext.txt" -> "filename.ext"
    /// - A filename like ".profile" results to identity ".profile".
    /// @return The filename excluding the #".Extension".
    PathString      Stem()                                                                   const {
        PathString result= Name();
        auto       dotPos= result.LastIndexOf('.');
        return  dotPos < 2 ? result
                           : result.Substring( 0, dotPos );
    }

    /// Returns the file extension, which is the substring behind the last period <c>'.'</c>
    /// character which is not located at the start of the name.
    /// (A filename like ".profile" is not treated to have an extension).
    /// @return The extension found in the filename. An empty string if none is found.
    PathString      Extension()                                                              const {
        auto    dotPos= Name().LastIndexOf('.');
        return  dotPos < 2 ? EMPTY_PATH
                           : Name().Substring( dotPos + 1 );
    }

    /// Appends the file's #"alib_filetree_tut_scan_realpath;real path" to the given \p{target} string.
    /// The result excludes a trailing separation character.
    /// If this file represents the root folder of the file tree, nothing is written to \p{target}.
    /// @see Sibling method #".AssembleSymbolicPath".
    /// @param target          The string buffer to append the path to.
    /// @param includeFilename Denotes whether the filename should be included or if the path
    ///                        to the parent directory is requested.
    /// @return The given #"%AString" to allow concatenated operations.
    strings::TAString<PathCharType>&
    AssembleRealPath( strings::TAString<PathCharType>&  target,
                      lang::Inclusion                   includeFilename )                    const {
        if(includeFilename==lang::Inclusion::Exclude) {
            if( !AsCursor().IsRoot() )
                AsCursor().Parent().AssemblePath(target, lang::CurrentData::Keep);
        }
        else
            AsCursor().AssemblePath(target, lang::CurrentData::Keep);
        return target;
    }

    /// Returns the #"alib_filetree_tut_scan_realpath;symbolic path" to this file.
    /// The result excludes a trailing separation character.
    /// If this file represents the root folder of the file tree, nothing is written to \p{target}.
    /// @see Sibling method #".AssembleRealPath".
    /// @param target          The string buffer to append the path to.
    /// @param includeFilename Denotes whether the filename should be included or if the path
    ///                        to the parent directory is requested.
    /// @return The given #"%AString" to allow concatenated operations.
    ALIB_DLL
    strings::TAString<PathCharType>&
    AssembleSymbolicPath( strings::TAString<PathCharType>&  target,
                          lang::Inclusion                   includeFilename )                 const;

    /// Retrieves the file's owner's name.
    /// @return The name of the owner of the file.
    const NString  GetOwnerName() const { return GetFTree().GetOGResolver().GetOwnerName(Value()); }

    /// Retrieves the file's group name.
    /// @return The name of the group of the file.
    const NString  GetGroupName() const { return GetFTree().GetOGResolver().GetGroupName(Value()); }

    /// Tests if custom data is attached to this file.
    /// @see Methods #".AttachCustomData", #".GetCustomData", #".DeleteCustomData", and
    ///      #"FTree::DeleteAllCustomData;*".
    /// @return <c>true</c> if custom data is attached to this file, <c>false</c> otherwise.
    bool        HasCustomData()                          const { return Value().custom != nullptr; }

    /// Retrieves a custom data object.
    /// With debug-compilations it is asserted that #".HasCustomData" returns <c>true</c>
    /// and that \p{TCustom} is the same as set.
    /// @tparam TCustom The type of custom data requested. In case no data was previously attached,
    ///                 yet, the constructor of this type is called on the new allocated memory.
    /// @see Methods #".AttachCustomData", #".HasCustomData", #".DeleteCustomData", and
    ///      #"FTree::DeleteAllCustomData;*".
    /// @return The custom data record.
    template<typename TCustom>
    TCustom& GetCustomData() {
        ALIB_ASSERT_ERROR( Value().custom != nullptr, "FILETREE", "No custom data set." )
        ALIB_ASSERT_ERROR( &typeid(TCustom) == Value().dbgCustomType, "FILETREE",
         "Requested custom object type mismatch.\n"
         "  Attached type: <{}>\n"
         "     Given type: <{}>"   , &typeid(TCustom), Value().dbgCustomType )

        return *static_cast<TCustom*>( Value().custom );
    }

    /// Allocates a custom object attached to this file using the
    /// #"TPoolAllocator;PoolAllocator" of the #"%FTree".
    ///
    /// @see Methods #".GetCustomData", #".HasCustomData", #".DeleteCustomData", and
    ///      #"FTree::DeleteAllCustomData;*".
    /// @tparam TCustom The type of custom data associated to the #"%FTree" that this file belongs
    ///                 to.
    /// @tparam TArgs   Types of the variadic arguments \p{args} that construct \p{TCustom}.
    /// @param  args    Variadic arguments forwarded to the constructor of \p{TCustom}.
    /// @return The custom data record.
    template<typename TCustom, typename... TArgs>
    TCustom& AttachCustomData(TArgs&&... args) {
        ALIB_ASSERT_ERROR( Value().custom == nullptr, "FILETREE", "Custom data already set." )

        auto* custom= GetFTree().Pool().template New<TCustom>( std::forward<TArgs>(args)... );
        Value().custom= custom;
        ALIB_DBG(Value().dbgCustomType= &typeid(TCustom); )
        return *custom;
    }

    /// Destructs and deletes the custom data attached to this file.
    /// With debug-compilations it is asserted that #".HasCustomData" returns <c>true</c>
    /// and that \p{TCustom} is the same as set.
    /// @see Methods #".AttachCustomData", #".GetCustomData", #HasCustomData, and
    ///      #"FTree::DeleteAllCustomData;*".
    /// @tparam TCustom The object type to optionally store in tree nodes.
    template<typename TCustom>
    void DeleteCustomData() {
        lang::Destruct(GetCustomData<TCustom>());
        GetFTree().Pool.free( Value().custom, sizeof(TCustom) );
        Value().custom= nullptr;
    }

    /// Writes the permission flags to the given \p{target} string in the
    /// same format as GNU/Linux command <em>'ls -l'</em> does.
    /// @param target  The target string to write into.
    /// @return The given target to allow concatenated calls.
    AString&     FormatAccessRights(AString& target)                                          const;

    /// Writes formatted information on this file to the given string buffer \p{target}.
    /// Within the pattern string \p{format}, different symbols are interpreted as tokens.
    /// Spaces between tokens are written as given.
    /// Strings within the format text that should not be interpreted as tokens may be given
    /// in single quotes.
    /// Two consecutive single quotes will be replaced to one single quote.<br>
    ///
    /// Tokens are defined in lower case letters.
    /// If given with upper case letters, the generated string is converted to upper case letters.
    ///
    /// This method supports the following tokens:
    ///
    ///  <center>Token</center>  | <center>Description</center>
    ///  - - - - - - - - - - - - | - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ///   a             |Invokes #FormatAccessRights.
    ///   b             |In case this node has a #"alib_filetree_tut_scan_realpath;symbolic parent", prints " <- /absolute/real/path/to/symbolicparent".
    ///   dm{DATEFORMAT}|The #"FileStatus::MDate;modification date" of this file. This token is optionally followed by a "DATEFORMAT" string given in curly braces. For specification information, see #"CalendarDateTime::Format;*".
    ///   db{DATEFORMAT}|Same as 'dm', but uses the #"FileStatus::BDate;creation date" of this file.
    ///   dc{DATEFORMAT}|Same as 'dm', but uses the #"FileStatus::CDate;change date" of this file.
    ///   da{DATEFORMAT}|Same as 'dm', but uses the #"FileStatus::ADate;date of last access" to this file.
    ///   fx            |Prints <c>'m'</c> if #"FileStatus::IsCrossingFS" returns \c true, <c>'-'</c> otherwise.
    ///   fa            |Prints <c>'a'</c> if #"FileStatus::IsArtificialFS" returns \c true, <c>'-'</c> otherwise.
    ///   gi[{width[,alignment]}] |The ID of the user group of the file.
    ///   gn[{width[,alignment]}] |The name of the user group of the file.
    ///   h             |The #"QtyHardLinks;number of hard links" pointing to this file.
    ///   l             |In case of Symbolic links, prints " -> linktarget". If the linktarget is a relative path, then the absolute path is appended in round brackets.
    ///   na            |The name of the file.
    ///   ns            |The #"Stem;stem" of the file.
    ///   ne            |The #"Extension;extension" of the file.
    ///   np            |The #"alib_filetree_tut_scan_realpath;symbolic path" to the file, excluding the file name and a trailing separation character.
    ///   nf            |The #"alib_filetree_tut_scan_realpath;symbolic path" to the file, including the file name and excluding a trailing separation character.
    ///   nr            |The #"alib_filetree_tut_scan_realpath;real path" to the file, excluding the file name and a trailing separation character.
    ///   nx            |The #"alib_filetree_tut_scan_realpath;real path" to the file, including the file name and excluding a trailing separation character.
    ///   oi[{width[,alignment]}] |The ID of the owner of the file.
    ///   on[{width[,alignment]}] |The name of the owner of the file.
    ///   q             |The scan state printed as resourced with enum #"ScanStates".
    ///   rd            |Recursively counted subfolders.
    ///   rf            |Recursively counted files.
    ///   re            |Recursively counted access errors.
    ///   rb            |Recursively counted broken links.
    ///   qqq           |The scan #"ScanStates", encoded in three characters. The conversion is resourced with enum #"FileStatus::ScanStates3Letters".
    ///   s[{unit}]     |The size of the file. See the explanation below.
    ///   t             |The #"FileStatus::Types;type", encoded in a single character. The conversion is resourced with enum #"FileStatus::TypeNames1Letter".
    ///   tt            |The #"FileStatus::Types;type", encoded in two characters. The conversion is resourced with enum #"FileStatus::TypeNames2Letters".
    ///   ttt           |The #"FileStatus::Types;type", encoded in three characters. The conversion is resourced with enum #"FileStatus::TypeNames3Letters".
    ///   tttt          |The #"FileStatus::Types;type", as a full word. The conversion is resourced with enum #"FileStatus::Types".
    ///
    /// \par Fields and Alignment
    ///   Any of the tokens above may be followed by <b>{width[,Alignment]}</b>. In words:
    ///   a pair of curly braces that contains an integral value specifying a field width
    ///   and, optionally and separated by a comma, an #"lang::Alignment;alignment" specifier.
    ///   (The optional alignment specifier is #"enumrecords::Parse;parsed using enum records".)
    ///   If so, a corresponding field, using spaces as padding character, is printed.
    ///
    /// \par Printing sizes:
    ///   Token <c>"s[(unit)]"</c> is used to print file sizes.
    ///   The optional unit string in curly braces may have one the following values:
    ///   - <c>IEC</c>: Chooses #"ByteSizeIEC;IEC standard" with automatic
    ///     detection of an appropriate magnitude. The unit of the magnitude found (<c>"B"</c>,
    ///     <c>"KiB"</c>, <c>"MiB"</c>, <c>"GiB"</c>,...) is added to the output.
    ///     This is the default if the optional unit-partis omitted.
    ///   - <c>SI</c>: Chooses #"ByteSizeSI;SI standard" with automatic
    ///     detection of an appropriate magnitude. The unit of the magnitude found (<c>"B"</c>,
    ///     <c>"kB"</c>, <c>"MB"</c>, <c>"GB"</c>,...) is added to the output.
    ///   - One of the more than 20 possible entity names of either IEC or SI standard.
    ///     In this case, the unit is \b not included after the number, because this way it can be
    ///     optionally added to the format string by using a pair of single quotes <c>'</c>.
    /// \par
    ///   With the two automatic modes <c>IEC</c> and <c>SI</c>, the namespace function
    ///    #"FormatByteSize" is used.<br>
    ///
    /// \par
    ///   For formatting the file size numbers, this method retrieves formatting hints with
    ///   #"FTree::GetNumberFormat;*". With that, the details of the format can
    ///   be specified "per FTree". Manipulations of this object before invoking this method,
    ///   allows specifying output widths, group characters, decimal separation character, and so
    ///   forth.
    ///
    ///
    /// \par Printing owner and group:
    ///   For printing owner and group names, those have to be queried from the OS.
    ///   To increase performance, the resolver utility instance received with
    ///   #"FTree::GetOGResolver;*" is used. The use of this instance has to be protected
    ///   against racing conditions in multithreaded applications. This means if two threads
    ///   invoke this method on #"%FTFile" object that belong to the same #"%FTree", a locking
    ///   mechanism has to be used, to avoid undefined behavior. (For example, by using the class
    ///   #"threads::Lock".)
    ///
    /// \par Sample
    ///   As a sample, the following format string mimics the output of GNU/Linux console command
    ///   <em>ls -l</em>:
    ///
    ///            "ta h on gn s dm nal"
    ///
    /// @see This method is invoked by #"FFormat_File", which is an implementation of
    ///      box-function #"FFormat". With hat, objects of this type can be used
    ///      as arguments for #"FormatterPythonStyle".
    ///      The format specifier passed to this method has to be placed behind the colon
    ///      in the placeholder field, as in <c>"{:FORMATSPEC}"</c>.
    ///      If no format string is given in the placeholder, the string
    ///      <b>"ta h on gn s dm nal"</b> is used, which is resourced in the camp
    ///      #"FILETREE;2" under key <b>"FFMT"</b>.
    ///
    ///
    /// @param format       The format pattern string.
    /// @param target       A reference to an AString that gets the result of the format processing
    ///                     appended.
    /// @param targetData   If \c CurrentData::Keep (the default) the string is appended to \p{target}.
    ///                     if \c CurrentData::Clear, \p{target} is cleared.
    /// @param numberFormat The number format specification to use. Defaults to \c nullptr which
    ///                     chooses #"TNumberFormat::Computational;*".
    /// @returns \p{target} (for convenience).
    ALIB_DLL
    AString&    Format( Substring           format,
                        AString&            target,
                        lang::CurrentData   targetData   = lang::CurrentData::Keep,
                        NumberFormat*       numberFormat = nullptr                        )   const;

}; // class FTFile

//==================================================================================================
//============================= Implementation of inlines of class FTree ===========================
//==================================================================================================
FTFile  FTree::Root()                                             { return FTFile( base::Root() ); }

void    FTree::Notify( FTreeListener::Event event, FTFile file, const PathString& filePath )
{ if (HasListeners()) notifyListeners(event, file, filePath); }


//==================================================================================================
//==================================== Box-function FFormat_File ===================================
//==================================================================================================
/// This implementation of boxing function #"%FFormat" for objects of type #"%FTFile", simply
/// invokes the method #"FTFile::Format;*" and thus, using the format specification is given
/// with that method.
///
/// Note that the #"TNumberFormat;NumberFormat" instance used for formatting file sizes
/// and similar, does not use the instance given with parameter \p{nf}. Instead, the instance
/// retrieved with #"FTree::GetNumberFormat;*" is used. This feature enables to
/// determine the number format separately for file data output, independent of the settings the
/// formater uses.
///
/// If the parameter \p{formatSpec} is empty, the string <b>"ta h on gn s dm nal"</b> is used,
/// which is resourced in camp #"FILETREE" under the key <b>"FFMT"</b>.
///
/// @param box          The box containing the file object.
/// @param formatSpec   The format string.
/// @param nf           The number format specification to use.
/// @param target       The target string to write to.
void FFormat_File( const Box& box, const String& formatSpec, NumberFormat& nf, AString& target );

} // namespace alib[::filetree]

/// Type alias in namespace #"%alib".
using     FTree      =   filetree::FTree;

#if !ALIB_SINGLE_THREADED || DOXYGEN
DOX_MARKER([DOX_MANUAL_ALIASES_FTREE])
/// Type alias in namespace #"%alib".
using     SharedFTree=   filetree::TSharedFTree<SharedLock>;
DOX_MARKER([DOX_MANUAL_ALIASES_FTREE])
#else
using     SharedFTree=   filetree::TSharedFTree<void>;
#endif

/// Type alias in namespace #"%alib".
using     FTFile       =   filetree::FTFile;

} // namespace [alib]


//##################################################################################################
// struct AppendableTraits<Cursor>
//##################################################################################################

// Faking all template specializations of namespace strings for doxygen into namespace
// strings::APPENDABLES to keep the documentation of namespace string clean!
namespace alib::strings {
#if DOXYGEN
namespace APPENDABLES {
#endif

/// Specialization of functor #"AppendableTraits" for type #"filetree::FTFile".
template<typename TChar>
struct AppendableTraits<filetree::FTFile, TChar, lang::HeapAllocator> {
    /// Writes the file's complete path (including the filename) to the given AString.
    /// @param target   The #"%WAString" that #"%Append(const TAppendable&)" was invoked on.
    /// @param file     The file.
    void operator()( TAString<TChar, lang::HeapAllocator>& target, const filetree::FTFile& file )
    requires (sizeof(TChar) == sizeof(PathCharType))  {
        file.AssembleSymbolicPath( target, lang::Inclusion::Include );
    }

    #if !DOXYGEN
    void operator()( TAString<TChar, lang::HeapAllocator>& target, const filetree::FTFile& file )
    requires (sizeof(TChar) != sizeof(PathCharType))  {
        Path path;
        file.AssembleSymbolicPath( path, lang::Inclusion::Include );
        target << path;
    }
    #endif

};

#if DOXYGEN
} // namespace alib::strings[APPENDABLES]
#endif
} // namespace [alib::strings]

ALIB_BOXING_VTABLE_DECLARE(   alib::filetree::FTFile     , vt_files_cursor      )

//-------------------------------------------- Debug Dump ------------------------------------------
#if ALIB_DEBUG
ALIB_EXPORT namespace alib::filetree {

    /// The format string used with namespace function #"filetree::DbgDump".<br>
    /// Defaults to <c>"{:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nf l}\n"</c><br>
    /// This global variable is only available with debug-compilations.
    extern String DBG_DUMP_FORMAT;

/// Dumps the given branch of this object's tree.<br>
/// This function is only available with debug-compilations.
/// @param target        The target string buffer.
/// @param tree          The tree to dump.
/// @param includedTypes Optional filter for types. Defaults to 'all'.
/// @param startNode     The start node. If this is not #"TCursor::IsValid;valid",
///                      the root node is chosen. Defaults to an invalid cursor.
/// @param depth         The maximum depth of recursion. Defaults to unlimited depth.
/// @return The given \p{target} to allow concatenated operations.
ALIB_DLL
AString&     DbgDump( AString&                  target,
                      FTree&                    tree  ,
                      EnumBitSet<FileStatus::Types>  includedTypes= EnumBitSet<FileStatus::Types>(true),
                      FTree::Cursor             startNode    = FTree::Cursor(),
                      unsigned                  depth        = (std::numeric_limits<unsigned int>::max)()  );

} // namespace [alib::filetree]
#endif

#if DOXYGEN
namespace alib::filetree {
#endif
/// This is the namespace <c>::std</c>.
/// Only for this documentation it is placed inside <c>alib::filetree</c>.
namespace std {

/// Specialization of <c>std::hash</c> for the type #"^FTree::CursorHandle".
template <>
struct hash<alib::filetree::FTree::CursorHandle> {
    /// Functor operator returning the hash-value.
    /// @param cursorHandle The handle to get the hash for.
    /// @return Simply this handle's field "CursorHandle::value".
    size_t operator()(const alib::filetree::FTree::CursorHandle& cursorHandle)      const noexcept {
        return cursorHandle.value;
    }
};

/// Specialization of <c>std::hash</c> for the type #"^FTree::ConstCursorHandle".
template <>
struct hash<alib::filetree::FTree::ConstCursorHandle> {
    /// Functor operator returning the hash-value.
    /// @param cursorHandle The handle to get the hash for.
    /// @return Simply this handle's field "CursorHandle::value".
    size_t operator()(const alib::filetree::FTree::ConstCursorHandle& cursorHandle) const noexcept {
        return cursorHandle.value;
    }
};

} // namespace std
#if DOXYGEN
} namespace [alib::filetree]
#endif

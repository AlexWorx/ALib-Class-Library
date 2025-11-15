//==================================================================================================
/// \file
/// This header-file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace files {

class   FTree;
template<typename TLock>    struct  TSharedFTree;
class   File;

/// This namespace implements internals of namespace #alib::files.
namespace detail {

struct FTreeNodeHandler;

/// A shortcut to the base class of the base class of class \alib{files;FTree}.
using TTree= alib::containers::detail::StringTreeBase< MonoAllocator,
                                                       FInfo,
                                                       FTreeNodeHandler,
                                                       Recycling::Private>;
                                              
/// Specialized \ref alib_ns_containers_stringtree_referencedoc "TNodeHandler" for class
/// \alib{files;FTree} which recycles extended information objects of type
/// \alib{files;FInfo::EIDirectory}, \alib{files;FInfo::EISymLinkFile}, and
/// \alib{files;FInfo::EISymLinkDir} with node deletion.
///
/// In respect to the node name allocation, this type behaves like
/// default handler \alib{containers;StringTreeNamesDynamic}.
/// In debug compilations, statistics
/// variables \alib{containers;DBG_STATS_STRINGTREE_NAMES} and
/// \alib{containers;DBG_STATS_STRINGTREE_NAME_OVERFLOWS} are increased, just like the original
/// does.
struct FTreeNodeHandler
{
    /// The character type that the \b StringTree uses for child name and path strings.
    /// This is taken from the C++ standard library.
    using CharacterType  = system::PathCharType;

    /// The string-type of a node's name. This is a simple static string, allocated with the
    /// pool allocator.
    using NameStringType =  system::PathString;

    /// Copies the node's name to the local string.
    ///
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               \alib{MonoAllocator} that the tree uses for the allocation of nodes.
    static
    inline
    void InitializeNode( TTree::Node& node, TTree& tree );

    /// This implementation frees any dynamically allocated memory of the node's name and in
    /// addition recycles any extended information object attached to the \alib{files;FInfo}
    /// object.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               \alib{MonoAllocator} that the tree uses for the allocation of nodes.
    static
    inline
    void FreeNode( TTree::Node& node, TTree& tree );
    
    /// Implements \alib{files;FTree::AllocateExtendedInfo}.
    /// @param node             The node add extended information to.
    /// @param symLinkDest      In case of symbolic link types, the symbolic link target.
    /// @param symLinkRealPath  In case of symbolic link types, the symbolic link target as real path.
    ALIB_DLL
    static
    void AllocateExtendedInfo( StringTree<MonoAllocator, FInfo, detail::FTreeNodeHandler>::Cursor& node,
                               const system::PathString& symLinkDest,
                               const system::PathString& symLinkRealPath      );
};  // struct FTreeNodeHandler

} // namespace alib::files[::detail]


//==================================================================================================
/// Abstract virtual interface type to implement types observing changes in instances of class
/// \alib{files;FTree}.
/// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
//==================================================================================================
struct FTreeListener
{
    /// The type of change that imposes the notification of a listener.
    enum class Event
    {
        CreateNode,     ///< A file or directory entry was created.
        DeleteNode,     ///< A file or directory entry was deleted.
    };

    /// Virtual destructor.
    virtual ~FTreeListener()                                                                      {}

    /// The virtual notification method.
    /// @param file   The file or directory that was modified.
    /// @param event  The type of modification.
    virtual void    Notify( File& file, Event event )                                            =0;

}; // struct FTreeListener


//==================================================================================================
/// This class builds on \alib type \alib{containers;StringTree}.
/// The contained elements (tree nodes) are of type\alib{files;FInfo} and represent entries in
/// filesystems.
/// Usually the tree is filled using function \alib{files;ScanFiles}.
///
/// ### StringTree Interface ###
/// Public base class \alib{containers;StringTree} provides all interfaces necessary to create and
/// delete entries, iterate and recursively walk the file tree. Please consult its documentation
/// for further information.
///
/// \note As \alib{files;ScanFiles;documented with function ScanFiles}, entities of this
///       module \alib_files_nl exclusively store entries along their <em>"Real Path"</em>, hence
///       always resolving symbolic links. A user of this library may deviate from this
///       "data contract".
///
/// ### Class File ###
/// The base classes' method \alib{containers;StringTree::Root} is overloaded by this class
/// and returns an instance of class \alib{files;File} instead of an instance of class
/// alib{containers;StringTree::Cursor}.
/// This class can be turned into a cursor using \alib{files::File;AsCursor} and then used to
/// navigate through the tree. Then, the cursor can be cast back (or assigned) to a \b File
/// instance.
///
/// ### Monotonic Behavior ###
/// The class fulfills \ref alib_contmono_intro_strictweak "weak monotonic allocation requirements",
/// which is achieved by recycling not only the nodes (what base type \b StringTree does by default)
/// but also any extended node information. This is implemented with the <b>StringTree</b>
/// handler-type \alib{files;detail::FTreeNodeHandler} on the one hand, and on the other hand,
/// with the implementation of the method #AllocateExtendedInfo, which has to be exclusively used
/// to attach information structs on \b FInfo elements.<br>
/// Furthermore, class \b File (the nodes of the tree) provides method
/// \alib{files::File;AttachCustomData}, which likewise uses the internal pool-allocator.
///
/// In summary, this allows an indefinite sequence of file-scan and result filtering (deletions)
/// with using more memory than the highest resulting fill state in such sequence requires.
//==================================================================================================
class FTree : public StringTree<MonoAllocator, FInfo, detail::FTreeNodeHandler>
{
    friend struct FTreeNodeHandler;
    
  public:
    /// An object pool used for recycling all sorts of allocated objects as well as the
    /// hashtable entries.
    /// It is \ref alib_contmono_chaining "chained" to the allocator provided with construction.
    ///
    /// The pool may be used in accordance with the general rules imposed by camp \alib_monomem.
    /// If so, in multithreaded environments, this object has to be locked (in addition
    /// to all other custom locks when interfacing this type), when using this pool from custom
    /// code.
    PoolAllocator           Pool;
     
  protected:
    /// Type alias of this classes' base class.
    using base= StringTree<MonoAllocator, FInfo, detail::FTreeNodeHandler>;

    friend struct  detail::FTreeNodeHandler;                         ///< Friendship declaration.
    friend class   files::File;                                      ///< Friendship declaration.

    /// Formatting information used with \alib{files;File::Format;format methods} of associated
    /// \b File instances.
    NumberFormat            numberFormat;

    /// A caching owner and group resolver. Used with \alib{files;File::Format;format methods}
    /// of associated \b File instances.
    OwnerAndGroupResolver   ogResolver;

    /// Record used to manage registered listeners.
    struct ListenerRecord
    {
        FTreeListener*        listener;      ///< The listener to register or dispose.
        FTreeListener::Event  event;         ///< The event to listen to.
        ConstCursorHandle     file;          ///< If given, the files to listen to.
        ConstCursorHandle     subTree;       ///< If given, the path of files to listen to.
        system::PathStringPA  fileName;      ///< If given, the file's name to listen to.
        system::PathStringPA  pathPrefix;    ///< If given, the start string of the file path
                                             ///< to monitor.
        system::PathStringPA  pathSubstring; ///< If given, the substring to match in the path
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
    ALIB_DLL  void registerListener( FTreeListener*                     listener,
                                     lang::ContainerOp                  insertOrRemove,
                                     FTreeListener::Event               event,
                                     const File*                        file,
                                     const StringTree::Cursor*          subTree,
                                     const system::PathString&    fileName,
                                     const system::PathString&    pathPrefix,
                                     const system::PathString&    pathSubstring  );

    #if DOXYGEN
    /// Notifies registered listeners on events.
    /// @param event    The event that occurred.
    /// @param file     The file.
    /// @param filePath The full path of the file. Might be nulled if not available, yet.
    /// @param lock     Pointer to an (optional) \alib{threads;SharedLock}.<br>
    ///                 This parameter is available (and to be passed) only if the module
    ///                 \alib_threads is included in the \alibbuild.
    ALIB_DLL  void notifyListeners(  FTreeListener::Event               event,
                                     File&                              file,
                                     SharedLock*                        lock,
                                     const system::PathString&    filePath );
    #else
    ALIB_DLL  void notifyListeners(  FTreeListener::Event               event,
                                     File&                              file
                  IF_ALIB_THREADS( , SharedLock*                        lock) ,
                                     const system::PathString&    filePath  );
    #endif

  public:
    /// Constructor.
    /// @param allocator The allocator to use.
    ALIB_DLL
    FTree( MonoAllocator& allocator );

    /// Destructor.
    ALIB_DLL
    ~FTree();

    /// Sort of 'overloads' method \alib{monomem;StringTree::Root}, which otherwise is accessible
    /// via <b>operator-></b> inherited by parent class \alib{monomem;TSharedMonoVal}.
    /// In contrast to the inherited method, this version returns an instance of type \b File.
    /// @return A file-cursor pointing to the root node of this file tree.
    inline
    File            Root();

    /// Allocates (or recycles) an appropriate information object fitting to the type of this entry.
    /// This method must only be applied to entries of types
    /// - \alib{files::FInfo::Types;DIRECTORY},
    /// - \alib{files::FInfo::Types;SYMBOLIC_LINK} or
    /// - \alib{files::FInfo::Types;SYMBOLIC_LINK_DIR}.
    /// In debug compilations, this is asserted. It is likewise asserted that the sybolic link
    /// information strings are empty in case the type is \alib{files::FInfo::Types;DIRECTORY}.
    ///
    /// @param node            The node add extended information to.
    /// @param symLinkDest     In case of symbolic link types, the symbolic link target.
    /// @param symLinkRealPath In case of symbolic link types, the symbolic link target as real
    ///                        path.
    void           AllocateExtendedInfo( Cursor& node,
                                         const system::PathString& symLinkDest,
                                         const system::PathString& symLinkRealPath)
    { detail::FTreeNodeHandler::AllocateExtendedInfo( node, symLinkDest, symLinkRealPath); }

    /// Deletes all custom data objects attached to any \b File in this tree.<br>
    /// Note that this method is only applicable if all custom data objects set in any node
    /// of this tree share the same type \p{TCustom}.
    /// With debug-compilations this is asserted.
    ///
    /// @see Method \alib{files;File::AttachCustomData}.
    /// @tparam TCustom The object type to optionally store in tree nodes.
    template<typename TCustom>
    void DeleteAllCustomData() {
        for( auto& node : nodeTable ) {
            if( node.data.custom ) {
                ALIB_ASSERT_ERROR( &typeid(TCustom) == node.data.dbgCustomType, "FILES",
                  "CustomData to delete does not match attached type.\n"
                  "Deletion has to be performed individually by this software.\n"
                  "This method must only be used if all tree nodes have the same custom data "
                  "attached\n"
                  "  Attached type: <{}>\n"
                  "     Given type: <{}>"   , &typeid(TCustom), node.data.dbgCustomType )

                reinterpret_cast<TCustom*>( node.data.custom ) -> ~TCustom();
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

    /// Retrieves formatting flags which are used with method \alib{files;File::Format}.
    /// @return Number formatting information for \b File objects associated with this file tree.
    NumberFormat&   GetNumberFormat()                                       { return numberFormat; }

    /// Retrieves formatting flags which are used with method \alib{files;File::Format}.
    /// @return Number formatting information for \b File objects associated with this file tree.
    OwnerAndGroupResolver&   GetOGResolver()                                  { return ogResolver; }

  //===================================== Listener Registration ====================================


    #if DOXYGEN
    /// Notifies registered listeners on events.
    /// @param event    The event that occurred.
    /// @param file     The file.
    /// @param lock     Pointer to an (optional) \alib{threads;SharedLock}.
    ///                 Has to be given only if multithreaded access is performed.
    ///                 If the \p{filePath} is nulled, method
    ///                 \alib{threads::SharedLock;AcquireShared} will be called.<br>
    ///                 This parameter is available (and to be passed) only if the module
    ///                 \alib_threads is included in the \alibbuild.
    /// @param filePath The full path of the file. Might be nulled if not available to the caller.
    ///                 In this case it is internally created.<br>
    ///                 Defaults to \alib{NULL_STRING}.
    void Notify( FTreeListener::Event event,
                 File&                file ,
                 SharedLock*          lock ,
                 const String&        filePath= NULL_STRING  );
    #else
    void Notify( FTreeListener::Event             event,
                   File&                            file
IF_ALIB_THREADS( , SharedLock*                      lock) ,
                   const system::PathString&  filePath= system::NULL_PATH )
    { if (HasListeners()) notifyListeners(event, file IF_ALIB_THREADS(,lock), filePath); }
    #endif
    
    /// @return \c true if listeners are registered with this file tree, \c false otherwise
    bool            HasListeners()                                  { return listeners.size() > 0; }

    /// Inserts or removes a listener to a specific file.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param file           The file to listen to.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    void MonitorDistinctFile( lang::ContainerOp       insertOrRemove,
                              FTreeListener*          listener,
                              FTreeListener::Event    event,
                              const File&             file       ) {
        ALIB_ASSERT_WARNING( event != FTreeListener::Event::CreateNode, "VARIABLES",
             "Event::Creation will never be invoked with this listener-registration-type." )
        registerListener( listener,
                          insertOrRemove,
                          event,
                          &file, nullptr,
                          system::NULL_PATH,
                          system::NULL_PATH,
                          system::NULL_PATH );
    }

    /// Inserts or removes a listener for all files that share the given \p{fileName}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param fileName       The name of one or more files to listen to.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    void MonitorFilesByName( lang::ContainerOp                  insertOrRemove,
                             FTreeListener*                     listener,
                             FTreeListener::Event               event,
                             const system::PathString&    fileName ) {
        ALIB_ASSERT_ERROR( fileName.IsNotEmpty(), "VARIABLES", "Empty file name given." )
        registerListener( listener, insertOrRemove, event,
                          nullptr, nullptr, fileName,
                          system::NULL_PATH, system::NULL_PATH );
    }


    /// Inserts or removes a listener for all files below the subtree specified by the
    /// given \p{cursor}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param cursor         The parent node in the tree of files to monitor.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    void MonitorPath(  lang::ContainerOp       insertOrRemove,
                       FTreeListener*          listener,
                       FTreeListener::Event    event,
                       const FTree::Cursor&    cursor            ) {
        registerListener( listener,
                          insertOrRemove,
                          event,
                          nullptr, &cursor,
                          system::NULL_PATH,
                          system::NULL_PATH,
                          system::NULL_PATH );
    }



    /// Inserts or removes a listener for all files below the subtree specified by the
    /// given \p{startPath}.
    /// \attention Note that the parameter \p{pathPrefix} has to be a portion of a
    ///            \alib{system;Path::MakeReal;real path}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathPrefix     The path prefix of the subtree of files to monitor. Note that a
    ///                       missing leading separator character will be added.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    void MonitorPathPrefix( lang::ContainerOp                   insertOrRemove,
                            FTreeListener*                      listener,
                            FTreeListener::Event                event,
                            const system::PathString&     pathPrefix      ) {
        ALIB_ASSERT_ERROR( pathPrefix.IsNotEmpty(), "VARIABLES", "Empty path prefix given." )
        registerListener( listener,
                          insertOrRemove,
                          event,
                          nullptr, nullptr,
                          system::NULL_PATH, pathPrefix, system::NULL_PATH );
    }

    /// Inserts or removes a listener for all files whose path (excluding the file name) contains
    /// the given \p{pathSubstring}.
    /// \attention Note that the parameter \p{pathSubstring} has to be a portion of a
    ///            \alib{system;Path::MakeReal;real path}.
    /// @param insertOrRemove Denotes whether the listener should be inserted or removed.
    ///                       (Only enum elements \b ContainerOp::Insert or \b ContainerOp::Remove
    ///                       must be passed.)
    /// @param listener       The listener to register.
    /// @param event          The event to listen to.
    /// @param pathSubstring  The substring to match in the path (including the file name)
    ///                       of the files to monitor.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    void MonitorPathSubstring( lang::ContainerOp                insertOrRemove,
                               FTreeListener*                   listener,
                               FTreeListener::Event             event,
                               const system::PathString&  pathSubstring  ) {
        ALIB_ASSERT_ERROR( pathSubstring.IsNotEmpty(), "VARIABLES", "Empty path substring given." )
        registerListener( listener,
                          insertOrRemove,
                          event,
                          nullptr, nullptr,
                          system::NULL_PATH, system::NULL_PATH,
                          pathSubstring );
    }

    /// Removes all registrations of the given listener.
    /// @param listener The listener to remove.
    /// @return The number of registrations that have been removed.
    /// @see Chapter \ref alib_files_monitoring of the Programmer's Manual of camp \alib_files_nl.
    ALIB_DLL
    int MonitorStop( FTreeListener*  listener );
}; // FTree


/// Utility type which implements \alib{monomem;TSharedMonoVal} with class \alib{files;FTree}.
/// The result of combining both is an automatic pointer to a \b %FTree that is "self-contained"
/// in the first buffer of a \alib{MonoAllocator} together with the allocator itself.
/// The tree is deleted and all associated memory is freed when the last copy of the pointer
/// goes out of scope.
///
/// Along with the \b FTree, this shared object includes a \alib{threads;SharedLock}.
/// See chapter \ref alib_contmono_smv_locking of the Programmer's Manual of module \alib_monomem
/// for further information on how to protect the contents of this type against
/// thread-racing-conditions.
///
/// @tparam TLock The lock type passed to the template parameter of parent type
///               \alib{monomem;TSharedMonoVal} with the same name.<br>
///               With the inclusion of module \alib_threads in the \alibbuild, the type-alias
///               #alib::SharedFTree chooses type \alib{threads;SharedLock}.<br>
///               Otherwise, in case \alib is compiled without threading support, the alias chooses
///               <c>void</c>.<br>
///               If it is assured that no racing-conditions occur with shared instances in
///               multithreaded software, the using code may pass <c>void</c> here as well.
template<typename TLock>
struct TSharedFTree : monomem::TSharedMonoVal<FTree, HeapAllocator, TLock>
{
    /// Exposed shortcut to the base type.
    using Base= monomem::TSharedMonoVal<FTree, HeapAllocator, TLock>;

    /// Constructs an empty instance, hence a cleared automatic pointer.
    TSharedFTree()                                                                         =default;

    /// Constructs an empty instance from \c std::nullptr.
    /// This constructor is necessary to allow assignment of \c nullptr to values of this type,
    /// which clears the automatic pointer.
    TSharedFTree(std::nullptr_t)                                                         noexcept {}

    /// Constructor.
    /// Calls the constructor of parent \b TSharedMonoVal and then invokes
    /// \alib{monomem;TSharedMonoVal::ConstructT} passing the mono allocator that the
    /// parent creates this instance in.<br>
    /// Furthermore calls DbgCriticalSections to enable assertions to locked usage.
    /// @param initialBufferSizeInKB The initial size of memory buffers.
    ///                              Passed to the allocator given with parent class
    ///                              \alib{monomem;TSharedMonoVal}.
    /// @param bufferGrowthInPercent Optional growth factor in percent, applied to the buffer size
    ///                              with each next buffer allocation.
    ///                              Passed to the allocator given with parent class
    ///                              \alib{monomem;TSharedMonoVal}.
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
    /// Calls #DbgCriticalSections to stop checking the integrated \p{TLock}.
    ~TSharedFTree()                                      { DbgCriticalSections(lang::Switch::Off); }

    #if DOXYGEN
    /// Enables or disables critical section checks between the contained \p{T} and the likewise
    /// contained \p{TLock}.<br>
    /// In case \p{TLock} equals <c>void</c> or if symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is not
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
    /// constructor defaults and resets the \b MonoAllocator of the parent class.<br>
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
    FInfo& value= node.data;
    auto extendedInfo= value.GetExtendedInfo();
    if( extendedInfo == nullptr )
        return;

    if(  value.IsSymbolicLink() ) {
        // delete old values
        FInfo::EISymLinkFile& ei= *static_cast<FInfo::EISymLinkFile*>(extendedInfo);

        if(    ei.RealTarget.Buffer() != ei.Target.Buffer()
            && ei.RealTarget.Buffer() != nullptr             )
            pool().Free( ei.RealTarget.Buffer(), ei.RealTarget.Length() + 1 );

        if(    ei.Target.Buffer() != nullptr             )
            pool().Free( ei.Target.Buffer(), ei.Target.Length() + 1 );

        if( value.Type() == FInfo::Types::SYMBOLIC_LINK )
            pool().Delete( static_cast<FInfo::EISymLinkFile*>(extendedInfo) );
        else
            pool().Delete( static_cast<FInfo::EISymLinkDir*>(extendedInfo) );

        // clear to be able to check double use in debug mode
        ALIB_DBG( value.SetExtendedInfo(nullptr) );
        return;
    }

    ALIB_ASSERT_ERROR( value.Type() == FInfo::Types::DIRECTORY,
      "FILES", "Given node is not a directory or symbolic link but still has extendedInfo set." )

    pool().Delete( reinterpret_cast<FInfo::EIDirectory*>(extendedInfo) );

    // clear to be able to check double use in debug mode
    ALIB_DBG( value.SetExtendedInfo(nullptr) );
}
#include "ALib.Lang.CIMethods.H"
#endif


//==================================================================================================
/// This class represents nodes in \alib{files;FTree} instances.
/// While class \b FTree is just a rather small wrapper around its base class
/// \alib{containers;StringTree}, this class \b File is a wrapper around class
/// \alib{containers;StringTree::Cursor}.
/// With that, instances of this class are very lightweight and contain only two pointers: One
/// pointing to the \b %FTree that an instance originates from, the second pointing to the
/// node in the tree.
///
/// It is important to understand that this class has three interfaces.
/// 1. The direct interface as exposed with this class.
/// 2. Using <c>operator-></c> the attached data instance of type \alib{files;FInfo} is accessed.
/// 3. The method #AsCursor casts an instance to the (otherwise protected) type \b Cursor of the
///    underlying string tree.
///
/// To get a thorough understanding of why this split exists and what purpose which of the
/// three interfaces serve, a basic understanding of container type class
/// \alib{containers;StringTree} is very helpful.
/// A similar design principle is implemented with class \alib{variables;Variable} of module
/// \alib_variables. A technical explanation to why base class <b>FTree::Cursor</b> is protected
/// there in the same fashion is \alib{variables;Variable::AsCursor;given here}.
///
/// @see
///   - For a quick tutorial about using \alib files, consult the tutorial-style
///     \ref alib_mod_files "Programmer's Manual" of camp \alib_files_nl.
///   - For this class, a \ref alibtools_debug_helpers_gdb "pretty printer" for the
///     GNU debugger is provided.
///   - Instances of this type are \ref alib_strings_assembly_ttostring "appendable" to
///     class \b %AString. If done, the full path and file name is written to the target string.
//==================================================================================================
class File : protected FTree::Cursor
{
  public:
    /// The base cursor type of the internal \b StringTree. This type is used to perform
    /// cursor operations on \b FTree instances.
    using Cursor= FTree::Cursor;

    /// The constant version of type #Cursor.
    using ConstCursor= FTree::ConstCursor;


    /// Returns a \c reference to the file tree that this file resides in.
    /// @return The associated file tree instance.
    FTree&      GetFTree()                            const { return  static_cast<FTree&>(Tree()); }

    File()                                                                                 =default;

    /// Constructor taking a file tree. After construction, this file will point to the root
    /// node <c>"/"</c> of the tree.
    /// @param pTree The tree to associate this file instance with.
    File( FTree& pTree )
    : Cursor( pTree.Root() )                                                                      {}

    /// Constructs an instance of this type from its base type.
    /// This constructor is for advanced use when direct operations with class \b StringTree and
    /// its cursor and iterator types are performed.
    /// @param cursor The \b StringTree cursor representing a file.
    File( const Cursor& cursor )
    : Cursor(cursor)                                                                              {}

    /// Sets this \b Cursor to point to the same file (node in the \alib{files;FTree}) as the given
    /// \p{other}.
    /// @param other  The node to let this file instance point to.
    /// @return A reference to \c this.
    File& operator=( const Cursor& other )             { Cursor::operator=( other ); return *this; }

    /// Provides \c const access to members of contained \alib{files;FInfo} record. Note that
    /// access to a mutable version of the type is available with method #GetMutableFInfo.
    /// @return A non-writable pointer to the embedded \b FInfo data.
    const FInfo*    operator->()                              const { return Cursor::operator->(); }

    /// Provides \c access to members of contained \alib{files;FInfo} record. Note that
    /// \c const access is available with method #operator->.<br>
    /// Changes to the values should be done with caution. Usually the values are only set when
    /// scanning files or using certain interface methods of this class.
    /// @return A \b writable pointer to the embedded \b FInfo data.
    FInfo&          GetMutableFInfo()                                            { return Value(); }

    /// This is an explicit <c>cast operator</c> to the protected base class.
    /// \note For details on the code design which makes this method necessary, consult the
    ///       documentation of the same concept found with method \alib{variables;Variable::AsCursor}.
    /// @return This instance cast 'down' to its protected base class.
    Cursor& AsCursor()                                       { return static_cast<Cursor&>(*this); }

    /// \c const version of the <c>cast operator</c> to the protected base class.
    /// @return This instance cast 'down' to its protected base class.
    const Cursor&   AsCursor()                   const { return static_cast<const Cursor&>(*this); }


    ALIB_WARNINGS_IGNORE_DOCS
    /// Publish protected method
    /// \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a58dad13c22c77ff79290aa72770b06ce;StringTree::TCursor::Name}.
    /// @return A constant reference to the name of the represented node.
    using Cursor::Name;
    ALIB_WARNINGS_RESTORE

    /// Returns the substring from the beginning of \b %Name() up to (and not including) the last
    /// period <c>'.'</c> character which is not located at the start of the name.
    /// With that, edge cases are treated as follows:
    /// - A filename like "filename.ext.txt" -> "filename.ext"
    /// - A filename like ".profile" results to identity ".profile".
    /// @return The filename excluding the #Extension.
    system::PathString      Stem()                                                           const {
        system::PathString result= Name();
        auto dotPos= result.LastIndexOf('.');
        return  dotPos < 2 ? result
                           : result.Substring( 0, dotPos );
    }

    /// Returns the file extension, which is the substring behind the last period <c>'.'</c>
    /// character which is not located at the start of the name.
    /// (A filename like ".profile" is not treated to have an extension).
    /// @return The extension found in the filename. An empty string if none is found.
    system::PathString      Extension()                                                      const {
        auto dotPos= Name().LastIndexOf('.');
        return  dotPos < 2 ? system::EMPTY_PATH
                           : Name().Substring( dotPos + 1 );
    }

    /// Returns the path to this file. This excludes this files' name as well as a trailing
    /// separation character. If this file represents the root folder of the file tree,
    /// nothing is written to \p{target}.
    ///
    /// @see To receive the full path of this file, including this %files' name, use
    ///      #AsCursor and invoke
    ///      \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a095127b50447399e888b0d382e82014d;Cursor::AssemblePath}.

    /// @param target       The string buffer to append the path to.
    /// @param targetData   Denotes whether \p{target} should be cleared before
    ///                     appending the path. Defaults to \b CurrentData::Clear.
    /// @return The given \b AString to allow concatenated operations.
    Path&       AssemblePath( Path& target,
                              lang::CurrentData targetData= lang::CurrentData::Clear)        const {
        if( targetData == lang::CurrentData::Clear )
            target.Reset();
        if( !AsCursor().IsRoot() )
            AsCursor().Parent().AssemblePath(target, lang::CurrentData::Keep);
        return target;
    }

    /// Retrieves the file's owner's name.
    /// @return The name of the owner of the file.
    const NString&  GetOwnerName() const { return GetFTree().GetOGResolver().GetOwnerName(Value());}

    /// Retrieves the file's group name.
    /// @return The name of the group of the file.
    const NString&  GetGroupName() const { return GetFTree().GetOGResolver().GetGroupName(Value());}

    /// Tests if custom data is attached to this file.
    /// @see Methods #AttachCustomData, #GetCustomData, #DeleteCustomData, and
    ///      \alib{files;FTree::DeleteAllCustomData}.
    /// @return <c>true</c> if custom data is attached to this file, <c>false</c> otherwise.
    bool        HasCustomData()                          const { return Value().custom != nullptr; }

    /// Retrieves a custom data object.
    /// With debug-compilations it is asserted that #HasCustomData() returns <c>true</c>
    /// and that \p{TCustom} is the same as set.
    /// @tparam TCustom The type of custom data requested. In case no data was previously attached,
    ///                 yet, the constructor of this type is called on the new allocated memory.
    /// @see Methods #AttachCustomData, #HasCustomData, #DeleteCustomData, and
    ///      \alib{files;FTree::DeleteAllCustomData}.
    /// @return The custom data record.
    template<typename TCustom>
    TCustom& GetCustomData() {
        ALIB_ASSERT_ERROR( Value().custom != nullptr, "FILES", "No custom data set." )
        ALIB_ASSERT_ERROR( &typeid(TCustom) == Value().dbgCustomType, "FILES",
         "Requested custom object type mismatch.\n"
         "  Attached type: <{}>\n"
         "     Given type: <{}>"   , &typeid(TCustom), Value().dbgCustomType )

        return *reinterpret_cast<TCustom*>( Value().custom );
    }

    /// Allocates a custom object attached to this file using the
    /// \alib{monomem;TPoolAllocator;PoolAllocator} of the \b FTree.
    ///
    /// @see Methods #GetCustomData, #HasCustomData, #DeleteCustomData, and
    ///      \alib{files;FTree::DeleteAllCustomData}.
    /// @tparam TCustom The type of custom data associated to the \b FTree that this file belongs
    ///                 to.
    /// @tparam TArgs   Types of the variadic arguments \p{args} that construct \p{TCustom}.
    /// @param  args    Variadic arguments forwarded to the constructor of \p{TCustom}.
    /// @return The custom data record.
    template<typename TCustom, typename... TArgs>
    TCustom& AttachCustomData(TArgs&&... args) {
        ALIB_ASSERT_ERROR( Value().custom == nullptr, "FILES", "Custom data already set." )

        auto* custom= GetFTree().Pool().template New<TCustom>( std::forward<TArgs>(args)... );
        Value().custom= custom;
        ALIB_DBG(Value().dbgCustomType= &typeid(TCustom); )
        return *custom;
    }

    /// Destructs and deletes the custom data attached to this file.
    /// With debug-compilations it is asserted that #HasCustomData() returns <c>true</c>
    /// and that \p{TCustom} is the same as set.
    /// @see Methods #AttachCustomData, #GetCustomData, #HasCustomData, and
    ///      \alib{files;FTree::DeleteAllCustomData}.
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


    /// Writes formatted information on this file to given string buffer \p{target}.
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
    ///   a             |Invokes #FormatAccessRights.</TD> </TR>
    ///   dm{DATEFORMAT}|The \alib{files::FInfo;MDate;modification date} of this file. This token is optionally followed by a "DATEFORMAT" string given in curly braces. For specification information, see \alib{format;CalendarDateTime::Format}.</TD> </TR>
    ///   db{DATEFORMAT}|Same as 'dm', but uses the \alib{files::FInfo;BDate;creation date} of this file.</TD> </TR>
    ///   dc{DATEFORMAT}|Same as 'dm', but uses the \alib{files::FInfo;CDate;change date} of this file.</TD> </TR>
    ///   da{DATEFORMAT}|Same as 'dm', but uses the \alib{files::FInfo;ADate;date of last access} to this file.</TD> </TR>
    ///   fx            |Prints <c>'m'</c> if \alib{files::FInfo;IsCrossingFS} returns \c true, <c>'-'</c> otherwise.
    ///   fa            |Prints <c>'a'</c> if \alib{files::FInfo;IsArtificialFS} returns \c true, <c>'-'</c> otherwise.
    ///   gi[{width[,alignment]}] |The ID of the user group of the file.
    ///   gn[{width[,alignment]}] |The name of the user group of the file.
    ///   h             |The \alib{files::FInfo;QtyHardLinks;number of hard links} pointing to this file.</TD> </TR>
    ///   l             |In case of Symbolic links, prints " -> linktarget". if the linktarget is a relative path, then the absolute path is appended in round brackets.</TD> </TR>
    ///   na            |The name of the file.</TD> </TR>
    ///   ns            |The \alib{files::File;Stem;stem} of the file.</TD> </TR>
    ///   ne            |The \alib{files::File;Extension;extension} of the file.</TD> </TR>
    ///   np            |The \alib{files::File;Path;path} to the file, excluding the file name and a trailing separation character.</TD> </TR>
    ///   oi[{width[,alignment]}] |The ID of the owner of the file.
    ///   on[{width[,alignment]}] |The name of the owner of the file.
    ///   q             |The scan \alib{files::FInfo;Quality;quality} as resourced with enum \alib{files::FInfo;Qualities}.</TD> </TR>
    ///   rd            |Recursively counted subfolders.</TD> </TR>
    ///   rf            |Recursively counted files.</TD> </TR>
    ///   re            |Recursively counted access errors.</TD> </TR>
    ///   rb            |Recursively counted broken links.</TD> </TR>
    ///   qqq           |The scan \alib{files::FInfo;Quality;quality}, encoded in three characters. The conversion is resourced with enum \alib{files::FInfo;Qualities3Letters}.</TD> </TR>
    ///   s[{unit}]     |The size of the file. See the explanation below.</TD> </TR>
    ///   t             |The \alib{files::FInfo;Type;type}, encoded in a single character. The conversion is resourced with enum \alib{files::FInfo;TypeNames1Letter}.</TD> </TR>
    ///   tt            |The \alib{files::FInfo;Type;type}, encoded in two characters. The conversion is resourced with enum \alib{files::FInfo;TypeNames2Letters}.</TD> </TR>
    ///   ttt           |The \alib{files::FInfo;Type;type}, encoded in three characters. The conversion is resourced with enum \alib{files::FInfo;TypeNames3Letters}.</TD> </TR>
    ///   tttt          |The \alib{files::FInfo;Type;type}, as a full word. The conversion is resourced with enum \alib{files::FInfo;Types}.</TD> </TR>
    ///
    /// \par Fields and Alignment
    ///   Any of the tokens above may be followd by <b>{width[,Alignment]}</b>. In words:
    ///   a pair of curly braces that contains an integral value specifying a field width
    ///   and, optionally and seperated by a comma, an \alib{lang;Alignment,alignment} specifier.
    ///   If so, a corresponding field, using spaces as padding character, is printed.
    ///
    /// \par Printing sizes:
    ///   Token <c>"s[(unit)]"</c> is used to print file sizes.
    ///   The optional unit string in curly braces may have one the following values:
    ///   - <c>IEC</c>: Chooses \alib{format;ByteSizeIEC;IEC standard} with automatic
    ///     detection of an approiate magnitude. The unit of the magnitude found (<c>"B"</c>,
    ///     <c>"KiB"</c>, <c>"MiB"</c>, <c>"GiB"</c>,...) is added to the output.
    ///     This is the default, if the optional unit-partis omited.
    ///   - <c>SI</c>: Chooses \alib{format;ByteSizeSI;SI standard} with automatic
    ///     detection of an approiate magnitude. The unit of the magnitude found (<c>"B"</c>,
    ///     <c>"kB"</c>, <c>"MB"</c>, <c>"GB"</c>,...) is added to the output.
    ///   - One of the more than 20 possible entity names of either IEC or SI standard.
    ///     In this case, the unit is \b not included after the number, because this way it can be
    ///     optionally added to the format string by using a pair of single quotes <c>'</c>.
    /// \par
    ///   With the two automatic modes <c>IEC</c> and <c>SI</c>, namespace function
    ///   \alib{format::FormatByteSize} is used.<br>
    ///
    /// \par
    ///   For formatting the file size numbers, this method retrieves formatting hints with
    ///   \alib{files;FTree::GetNumberFormat}. With that, the details of the format can
    ///   be specified "per FTree". Manipulations of this object before invoking this methods,
    ///   allows specifying output widths, group characters, decimal separation character, and so
    ///   forth.
    ///
    ///
    /// \par Printing owner and group:
    ///   For printing owner and group names, those have to be queried from the OS.
    ///   To increase performance, the resolver utility instance received with
    ///   \alib{files;FTree::GetOGResolver} is used. The use of this instance has to be protected
    ///   against racing conditions in multithreaded applications. This means if two threads
    ///   invoke this method on \b %File object that belong to the same \b %FTree, a locking
    ///   mechanism has to be used, to avoid undefined behavior. (For example, by using class
    ///   \alib{threads;Lock}.)
    ///
    /// \par Sample
    ///   As a sample, the following format string mimics the output of GNU/Linux console command
    ///   <em>ls -l</em>:
    ///
    ///            "ta h on gn s dm nal"
    ///
    /// @see This method is invoked by \alib{files;FFormat_File}, which is an implementation of
    ///      box-function \alib{format;FFormat}. With hat, objects of this type can be used
    ///      as arguments for \alib{format;FormatterPythonStyle}.
    ///      The format specifier passed to this method has to be placed behind the colon
    ///      in the placeholder field, as in <c>"{:FORMATSPEC}"</c>.
    ///      If no format string is given in the placeholder, the string
    ///      <b>"ta h on gn s dm nal"</b> is used, which is resourced in camp
    ///      \alib{FILES} under key <b>"FFMT"</b>.
    ///
    ///
    /// @param format       The format pattern string.
    /// @param target       A reference to an AString that gets the result of the format processing
    ///                     appended.
    /// @param targetData   If \c CurrentData::Keep (the default) the string is appended to \p{target}.
    ///                     if \c CurrentData::Clear, \p{target} is cleared.
    /// @param numberFormat The number format specification to use. Defaults to \c nullptr which
    ///                     chooses \alib{format;NumberFormat::Computational}.
    /// @returns \p{target} (for convenience).
    ALIB_DLL
    AString&    Format( Substring           format,
                        AString&            target,
                        lang::CurrentData   targetData   = lang::CurrentData::Keep,
                        NumberFormat*       numberFormat = nullptr                        )   const;

}; // class File

//==================================================================================================
//============================= Implementation of inlines of class FTree ===========================
//==================================================================================================
File    FTree::Root()                                               { return File( base::Root() ); }

//==================================================================================================
//==================================== Box-function FFormat_File ===================================
//==================================================================================================
/// This implementation of boxing function \b FFormat for objects of type \b File, simply
/// invokes the method \alib{files;File::Format} and thus, using the format specification is given
/// with that method.
///
/// Note that the \alib{strings;TNumberFormat;NumberFormat} instance used for formatting file sizes
/// and similar, does not use the instance given with parameter \p{nf}. Instead, the instance
/// retrieved with \alib{files;FTree::GetNumberFormat} is used. This feature enables to
/// determine the number format separately for file data output, independent of the settings the
/// formater uses.
///
/// If the parameter \p{formatSpec} is empty, the string <b>"ta h on gn s dm nal"</b> is used,
/// which is resourced in camp \alib{FILES} under the key <b>"FFMT"</b>.
///
/// @param box          The box containing the file object.
/// @param formatSpec   The format string.
/// @param nf           The number format specification to use.
/// @param target       The target string to write to.
void FFormat_File( const Box& box, const String& formatSpec, NumberFormat& nf, AString& target );

} // namespace alib[::files]

/// Type alias in namespace \b alib.
using     FTree      =   files::FTree;

#if !ALIB_SINGLE_THREADED || DOXYGEN
DOX_MARKER([DOX_MANUAL_ALIASES_FTREE])
/// Type alias in namespace \b alib.
using     SharedFTree=   files::TSharedFTree<SharedLock>;
DOX_MARKER([DOX_MANUAL_ALIASES_FTREE])
#else
using     SharedFTree=   files::TSharedFTree<void>;
#endif

/// Type alias in namespace \b alib.
using     File       =   files::File;

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

/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{files;File}.
template<> struct       AppendableTraits<files::File, nchar, lang::HeapAllocator>
{
    /// Writes the file's complete path (including the filename) to the given AString.
    /// @param target  The \b NAString that \b Append was invoked on.
    /// @param file    The file.
    ALIB_DLL void operator()( TAString<nchar, lang::HeapAllocator>& target, const files::File& file );
};

/// Specialization of functor \alib{strings;AppendableTraits} for type \alib{files;File}.
template<> struct       AppendableTraits<files::File, wchar, lang::HeapAllocator>
{
    /// Writes the file's complete path (including the filename) to the given AString.
    /// @param target   The \b WAString that \b Append was invoked on.
    /// @param file     The file.
    ALIB_DLL void operator()( TAString<wchar, lang::HeapAllocator>& target, const files::File& file );
};

#if DOXYGEN
} // namespace alib::strings[APPENDABLES]
#endif
} // namespace [alib::strings]



ALIB_BOXING_VTABLE_DECLARE(   alib::files::File     , vt_files_cursor      )




//-------------------------------------------- Debug Dump ------------------------------------------
#if ALIB_DEBUG
ALIB_EXPORT namespace alib::files {

    /// The format string used with namespace function \alib{files;DbgDump}.<br>
    /// Defaults to <c>"{:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nf l}\n"</c><br>
    /// This global variable is only available with debug-compilations.
    extern String DBG_DUMP_FORMAT;

/// Dumps the given branch of this object's tree.<br>
/// This function is only available with debug-compilations.
/// @param target        The target string buffer.
/// @param tree          The tree to dump.
/// @param includedTypes Optional filter for types. Defaults to 'all'.
/// @param startNode     The start node. If this is not
///                      \doxlinkproblem{classalib_1_1monomem_1_1StringTree_1_1TCursor.html;ac532c4b500b1a85ea22217f2c65a70ed;a valid node;alib::monomem::StringTree::TCursor::IsValid},
///                      the root node is chosen. Defaults to an invalid cursor.
/// @param depth         The maximum depth of recursion. Defaults to unlimited depth.
/// @return The given \p{target} to allow concatenated operations.
ALIB_DLL
AString&     DbgDump( AString&                  target,
                      FTree&                    tree  ,
                      EnumBitSet<FInfo::Types>  includedTypes= EnumBitSet<FInfo::Types>(true),
                      FTree::Cursor             startNode    = FTree::Cursor(),
                      unsigned                  depth        = (std::numeric_limits<unsigned int>::max)()  );



} // namespace [alib::files]
#endif

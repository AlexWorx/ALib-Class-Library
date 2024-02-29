/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_files of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FILES_FTREE
#define HPP_ALIB_FILES_FTREE 1

#if !defined(HPP_ALIB_FILES_FINFO)
#   include "alib/files/finfo.hpp"
#endif

#if !defined(HPP_ALIB_MONOMEM_STRINGTREE)
#   include "alib/monomem/stringtree.hpp"
#endif

#if !defined (HPP_ALIB_CAMP_DIRECTORY)
#   include "alib/lang/system/directory.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ITERABLE_BITSET)
#   include "alib/enums/iterablebitset.hpp"
#endif


namespace alib { namespace files {

/** Local path string buffer helper type fixed to size 512. The flexible nature of \alib local
 *  strings allow dynamic allocation if exceeded.*/
using PathString= alib::strings::TLocalString<character, 512>;

/**
 * This namespace implements internals of namespace #alib::files.
 */
 namespace detail {

/**
 * Specialized  \ref alib_ns_monomem_stringtree_referencedoc "TNodeMaintainer" for class
 * \alib{files,FTree} which recycles extended information objects of type
 * \alib{files,FInfo::EIDirectory}, \alib{files,FInfo::EISymLinkFile} and
 * \alib{files,FInfo::EISymLinkDir} with node deletion.
 *
 * In respect to the node name allocation, this type behaves like
 * default maintainer \alib{monomem,StringTreeNamesDynamic}. In debug compilations, statistics
 * variables \alib{monomem,DbgStatsStringTreeNames} and \alib{monomem,DbgStatsStringTreeNameOverflows}
 * are increased, just like the original does.
 */
struct FTreeNodeMaintainer
{
    /** The character type that the \b StringTree uses for child name and path strings. */
    using CharacterType  = character;

    /** The string type of a node's name. Here, a buffered dynamic string */
    using NameStringType =  strings::TLocalString<character, 100>;

    /**
     * Copies the node's name to the local string.
     *
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that was just created. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     *               (Deduced by the compiler.)
     */
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        (void) tree;

        String key= node.name.key;                              // get current pointer
        new (&node.name.storage) NameStringType();              // placement new to re-establish local string
        node.name.storage.DbgDisableBufferReplacementWarning();
        ALIB_DBG( const auto* internalBuffer= node.name.storage.Buffer(); )
        node.name.storage.Append(key);                          // copy key to buf
        ALIB_DBG( ++monomem::DbgStatsStringTreeNames;
                  if( internalBuffer != node.name.storage.Buffer() )
                     ++monomem::DbgStatsStringTreeNameOverflows;                )
    }

    /**
     * This implementation frees any dynamically allocated memory of the node's name and in
     * addition recycles any extended information object attached to the \alib{files,FInfo}
     * object.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that is to be removed. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     *               (Deduced by the compiler.)
     */
    template<typename TTree>
    static
    inline
    void FreeNode( TTree& tree, typename TTree::Node& node );
};  // struct FTreeNodeMaintainer
} // namespace alib::files[::detail]

/** ************************************************************************************************
 * This class is a \alib{monomem,StringTree} containing \alib{files,FInfo} elements.
 * Usually the tree is filled using function \alib{files,ScanFiles}.
 *
 * ### StringTree Interface ###
 * Public base class \alib{monomem::StringTree} provides all interfaces necessary to create and
 * delete entries, iterate and recursively walk the file tree. Please consult its documentation
 * for further information.
 *
 * \note As \alib{files,ScanFiles,documented with function ScanFiles}, entities of this
 *       module \alib_files_nl exclusively store entries along their <em>"Real Path"</em>, hence
 *       always resolving symbolic links. A user of this library may deviate from this
 *       "data contract".
 *
 * ### Monotonic Behaviour ###
 * The class fulfills \ref alib_monomem_intro_strictweak "weak monotonic allocation requirements",
 * which is achieved by recycling not only the nodes (what base type \b StringTree does by default)
 * but also any extended node information. This is implemented with maintainer type
 * \alib{files,detail::FTreeNodeMaintainer} on the one hand, and on the other hand
 * with the implementation of method #AllocateExtendedInfo, which has to be exclusively used
 * to attach information structs on \b FInfo elements.
 *
 * In respect to the file names, hence the key types of base class \b StringTree, class
 * \alib{files,detail::FTreeNodeMaintainer} uses an internal buffer of size <c>100</c>.
 * This leads to quite some monotonic allocation overhead when file names are short, but
 * limits the percentage of exceeding file names quite well. Exceeding filenames have not been
 * further optimized, but are simply allocated on the heap (and of-course free with the deletion
 * of an entry).
 *
 * In summary, this allows an indefinite sequence of file-scan and result filtering (deletions)
 * with using more memory than the highest resulting fill state in such sequence requires.
 *
 **************************************************************************************************/
class FTree : public StringTree<FInfo, detail::FTreeNodeMaintainer>
{
  public:
    /** Type alias of this classes' base class. */
    using TTree= StringTree<FInfo, detail::FTreeNodeMaintainer>;

  protected:
    /** The node maintainer is friend, as it needs access to the recyclers of this class. */
    friend struct detail::FTreeNodeMaintainer;

    /** Internal struct which embeds extended \b FInfo data at the front, together with a recursive
     *  link used for recycling instances of the extended type.    */
    struct LinkedEIDir
    {
       FInfo::EIDirectory   data;             ///< The usable data receivable with the \b FInfo instance.
       LinkedEIDir*         next   = nullptr; ///< A recursive link to implement a recyling list.
    };

    /** Internal struct which embeds extended \b FInfo data at the front, together with a recursive
     *  link used for recycling instances of the extended type.    */
    struct LinkedEISL
    {
       FInfo::EISymLinkFile data;             ///< The usable data receivable with the \b FInfo instance.
       LinkedEISL*          next   = nullptr; ///< A recursive link to implement a recyling list.
    };

    /** Internal struct which embeds extended \b FInfo data at the front, together with a recursive
     *  link used for recycling instances of the extended type.    */
    struct LinkedEISLDir
    {
       FInfo::EISymLinkDir  data;             ///< The usable data receivable with the \b FInfo instance.
       LinkedEISLDir*       next   = nullptr; ///< A recursive link to implement a recyling list.
    };


    LinkedEIDir  *          recyclerEIDir  = nullptr; ///< Linked list hook for recycling information elements of disposed nodes.
    LinkedEISL   *          recyclerEISL   = nullptr; ///< Linked list hook for recycling information elements of disposed nodes.
    LinkedEISLDir*          recyclerEISlDir= nullptr; ///< Linked list hook for recycling information elements of disposed nodes.

  public:
    /**
     * Constructor
     * @param allocator The allocator to use. Passed to the
     *                  \alib{monomem::StringTree,StringTree,constructor} of base class
     *                  \alib{monomem,StringTree}.
     */
    ALIB_API
    FTree(monomem::MonoAllocator* allocator);

    /** Destructor */
    ~FTree()   { DeleteRootValue();  }

    /**
     * Allocates (or recycles) an appropriate information object fitting to the type of this entry.
     * This method must only be applied to entries of types
     * - \alib{files::FInfo::Types,DIRECTORY},
     * - \alib{files::FInfo::Types,SYMBOLIC_LINK} or
     * - \alib{files::FInfo::Types,SYMBOLIC_LINK_DIR}.
     * In debug compilations, this is asserted. It is likewise asserted that the sybolic link
     * information strings are empty in case the type is \alib{files::FInfo::Types,DIRECTORY}.
     *
     *  \param node             The node add extended information to.
     *  \param symLinkDest      In case of symbolic link types, the symbolic link target.
     *  \param symLinkRealPath  In case of symbolic link types, the symbolic link target as real path.
     */
    ALIB_API
    void         AllocateExtendedInfo(Cursor& node, String& symLinkDest, String& symLinkRealPath);

    /**
     * Returns the number of currently available recycled objects of derived type of
     * \alib{files::FInfo,ExtendedEntryInfo}.
     * This method is rather for testing or debugging.
     * @param type  Must be one of the types
     *              \alib{files::FInfo,Types::DIRECTORY},
     *              \alib{files::FInfo,Types::SYMBOLIC_LINK} or
     *              \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}.
     * @return The number of
     */
    ALIB_API
    integer      CountRecyclables( FInfo::Types type );

    /**
     * Recalculates the sums of the given node. This is \b not done recursively. The fix is needed
     * when scanning an existent directory with potentially more greedy scan parameters.
     * @param directoryNode  The node to re-calculate the sums for.
     */
    ALIB_API
    static
    void FixSums( Cursor directoryNode);

}; //class FTree

// --------------------- Implementation of the node mainer's FreeNode() method ---------------------
#if !defined(ALIB_DOX)
template<typename TTree>
inline
void detail::FTreeNodeMaintainer::FreeNode( TTree& tree, typename TTree::Node& node )
{
    // delete local string
    node.name.storage.~TLocalString();

    // recycle extended info structs
    FTree& fTree= static_cast<FTree&>(tree);
    FInfo& value= node.data;
    auto extendedInfo= value.GetExtendedInfo();
    if( extendedInfo == nullptr )
        return;

    if( value.Type() == FInfo::Types::DIRECTORY )
    {
        FTree::LinkedEIDir* recycle= reinterpret_cast<FTree::LinkedEIDir*>(extendedInfo );
        recycle->next= fTree.recyclerEIDir;
        fTree.recyclerEIDir= recycle;
    }
    else if( value.Type() == FInfo::Types::SYMBOLIC_LINK )
    {
        FTree::LinkedEISL* recycle= reinterpret_cast<FTree::LinkedEISL*>(extendedInfo );
        recycle->next= fTree.recyclerEISL;
        fTree.recyclerEISL= recycle;
    }
    else
    {
        ALIB_ASSERT_ERROR( value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR,
         "CAMP/FILES", "Given node is not a directory or symbolic link but still has extendedInfo set.")


        FTree::LinkedEISLDir* recycle= reinterpret_cast<FTree::LinkedEISLDir*>(extendedInfo);
        recycle->next= fTree.recyclerEISlDir;
        fTree.recyclerEISlDir= recycle;
    }

    // clear to be able to check double use in debug mode
    ALIB_DBG( value.SetExtendedInfo(nullptr) );
}
#endif


} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     FTree          =   files::FTree;
} // namespace [alib]

//---------------------------------------   Debug Dump   ---------------------------------------
#if ALIB_DEBUG
namespace alib::files {
    /**
     * Dumps the given branch of this object's tree.
     * @param target        The target string buffer.
     * @param tree          The tree to dump.
     * @param includedTypes Optional filter for types. Defaults to 'all'.
     * @param startNode     The start node. If this is not
     *                      \alib{monomem::StringTree::TCursor,IsValid(),a valid node}, the root
     *                      node is chosen. Defaults to an invalid cursor.
     * @param depth         The maxiumum depth of recursion. Defaults to unlimited depth.
     * @return The given \p{target} to allow concatenated operations.
     */
    ALIB_API
    AString&     DbgDump( AString&                  target,
                          FTree&                    tree  ,
                          EnumBitSet<FInfo::Types>  includedTypes= EnumBitSet<FInfo::Types>(true),
                          FTree::Cursor             startNode    = FTree::Cursor(),
                          unsigned int              depth        = (std::numeric_limits<unsigned int>::max)()  );



} // namespace [alib::files]
#endif



#endif // HPP_ALIB_FILES_FTREE

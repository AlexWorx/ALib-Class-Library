/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_STRINGTREE
#define HPP_ALIB_MONOMEM_STRINGTREE 1

#if !defined (HPP_ALIB_MONOMEM_MONOMEM)
#   include "alib/monomem/monomem.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)

#include "alib/monomem/detail/stringtreebase.inl"


namespace aworx { namespace lib { namespace monomem {


/**
 * This struct is the default type for template parameter
 * \ref alib_ns_monomem_stringtree_referencedoc "TNodeMaintainer" of class
 * \alib{monomem,StringTree}.
 *
 * Besides defining the character type as given with template parameter \p{TChar}, the node name
 * string type is exposed with #NameStringType. The string type depends on template parameter
 * \p{TLocalCapacity}:
 * - If this is \c 0, the type evaluates to a simple string with no internal storage.
 * - If this is greater than zero, the type evaluates to a \alib{strings,TLocalString,LocalString}
 *   of given capacity.
 *
 * This design allows to allocate a fixed-size string buffer with each node and only if a
 * node's name exceeds this capacity, a dynamic allocation for storing the node name is performed.
 * As a consequence, some overhead of wasted memory will occur, as this capacity is
 * allocated with every node, regardless of its name's length.
 *
 * Method #InitializeNode is invoked after insertion of a new element (aka "node")
 * into the container and #FreeNode is invoked prior to the destruction of a node.
 * When #InitializeNode is invoked, the custom object of template type \p{T} (of the \b StringTree)
 * is already default constructed and the name of the node
 * (field \alib{monomem,detail::StringTreeBase::Node::name}) is set to what was provided
 * as a child name or path string. (In the latter case, it is set to a substring of the
 * given path.). If template parameter \p{TLocalCapacity} is greater than 0, the
 * method remains empty, as the \b LocalString manages its memory automatically.
 * If \c 0 is given, the node name is replaced by a copy of the string which is dynamically
 * allocated.
 *
 * ## Custom Implementations ##
 * A custom implementation has to provide all four entities that this type provides in a
 * compatible fashion.
 *
 * The main purpose of the node maintainer types is to ensure that the name strings of inserted
 * nodes are duly allocated, copied and freed as needed:
 * When a new element is (or a whole path of new elements are) created, then the initial name
 * of the nodes are taken from the string passed to the corresponding interface method of class
 * \b StringTree (and inner types). The challenge is that these string's life-cycle might
 * be only short term. Therefore, right after the creation of an element, method #InitializeNode
 * is invoked, allowing to create a safe copy of the name.<br>
 * To free any allocated space, method #FreeNode is invoked.
 *
 * Besides this, custom implementation may tweak the given node node on their own discretion.
 *
 * \see
 *   Two other built-in implementations of this type to be used with \b StringTree instantiations
 *   are provided with this \alibmod:
 *   - \alib{monomem,StringTreeNamesStatic}.
 *   - \alib{monomem,StringTreeNamesMonoAlloc}.
 *
 *
 * @tparam TChar           The character type of the key strings. This type is used with any
 *                         interface method of \alib{monomem,StringTree} that accepts a node name
 *                         or path string.<br>
 *                         Defaults to type \alib{characters,character}.
 * @tparam TLocalCapacity  The capacity of the \alib{strings,TLocalString,LocalString} to place in
 *                         the <b>StringTree</b>'s node. If \c 0 is given, a normal
 *                         \alib{strings,TString,String} is used for the name, and the buffer is
 *                         copied to an dynamically allocated array.<br>
 *                         Defaults to \c 32.
 */
template<typename TChar= character, integer TLocalCapacity= 32>
struct StringTreeNamesDynamic
{
    /** The character type that the \b StringTree uses for child name and path strings. */
    using CharacterType  = TChar;

    /** The string type of a node's name. */
    using NameStringType = ATMP_IF_T_F ( (TLocalCapacity > 0),
                                         strings::TLocalString<TChar ALIB_COMMA TLocalCapacity>,
                                         strings::TString     <TChar>  );


    /**
     * This implementation copies the node's name to a dynamically allocated piece of heap memory.
     * \see
     *   See class description for further information.
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that was just created. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        (void) tree;

        // if not a local string buffer, then dynamically allocate and copy. Else, do nothing
        if ALIB_CONSTEXPR17 (TLocalCapacity <= 0)
        {
            CharacterType* buffer= new CharacterType[static_cast<size_t>(node.name.Length()) ];
            node.name.CopyTo( buffer );
            node.name= strings::TString<CharacterType>( buffer, node.name.Length() );
        }
    }

    /**
     * This implementation frees the dynamically allocated memory of the node's name.
     * \see
     *   See class description for further information.
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that is to be removed. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::Node& node )
    {
        (void) tree;
        if ALIB_CONSTEXPR17 (TLocalCapacity <= 0)
            delete[] node.name.Buffer();
    }
};

/**
 * Built-in implementation usable as template parameter
 * \ref alib_ns_monomem_stringtree_referencedoc "TNodeMaintainer" of class
 * \alib{monomem,StringTree}.
 *
 * This type does not allocate memory and does not copy the key string of a node. Therefore, this
 * type is very efficient to use in situations where exclusively "static" strings for child names
 * and paths are passed to the interface methods of class \b StringTree (and inner types) which lead
 * to the creation of new child nodes.<br>
 * The term "static" here means that the strings given are either static character data of a
 * compilation unit or by any other means their allocated memory and the contained data survive
 * the life-cycle of the corresponding \b StringTree.
 *
 * \see
 *   For more information see documentation of the built-in "default" version of this template
 *   parameter type, \alib{monomem,StringTreeNamesDynamic}.
 *
 * @tparam TChar  The character type of the key strings. This type is used with any
 *                interface method of \alib{monomem,StringTree} that accepts a node name or path
 *                string.
 */
template<typename TChar= character>
struct StringTreeNamesStatic
{
    /** The character type that the \b StringTree uses for child name and path strings. */
    using CharacterType  = TChar;

    /** The string type of a node's name. */
    using NameStringType = strings::TString<TChar>;

    /**
     * This implementation is empty.
     *
     * \see
     *   See description of this class and the "default implementation"
     *   \alib{monomem,StringTreeNamesDynamic}.
     *
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that was just created. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        (void) tree;
        (void) node;
    }

    /**
     * This implementation is empty.
     *
     * \see
     *   See description of this class and the "default implementation"
     *   \alib{monomem,StringTreeNamesDynamic}.
     *
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that is to be removed. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::Node& node )
    {
        (void) tree;
        (void) node;
    }

};

/**
 * Built-in implementation usable as template parameter
 * \ref alib_ns_monomem_stringtree_referencedoc "TNodeMaintainer" of class
 * \alib{monomem,StringTree}.
 *
 * This type copies the node's name into memory acquired with the monotonic allocator that the
 * \b StringTree uses.<br>
 *
 * \attention
 *   The use of this type is dangerous in respect to memory exhaustion. While class
 *   \b StringTree uses monotonic allocation in a
 *   \ref alib_ns_monomem_stringtree_allocations "very safe way", with the use of this
 *   type, repeated removals and insertions of tree nodes, increase the memory usage.<br>
 *   Consequently, the use of this type is restricted to cases that imply a limited
 *   number of insertions.
 *
 * \see
 *   For more information see documentation of the built-in "default" version of this template
 *   parameter type, \alib{monomem,StringTreeNamesDynamic}.
 *
 * @tparam TChar  The character type of the key strings. This type is used with any
 *                interface method of \alib{monomem,StringTree} that accepts a node name or path
 *                string.
 */
template<typename TChar= character>
struct StringTreeNamesMonoAlloc
{
    /** The character type that the \b StringTree uses for child name and path strings. */
    using CharacterType= TChar;

    /** The string type of a node's name. */
    using NameStringType = strings::TString<TChar>;

    /**
     * This implementation copies the node's name to a piece of memory allocated in the
     * \alib{monomem,MonoAllocator} found in field \alib{monomem::detail::StringTreeBase,nodeTable}
     * of the given \p{tree}.
     *
     * \see
     *   See description of this class and the "default implementation"
     *   \alib{monomem,StringTreeNamesDynamic}.
     *
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that was just created. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        node.name= tree.nodeTable.GetAllocator()->EmplaceString( node.name );
    }

    /**
     * This implementation does nothing.
     *
     * \see
     *   See description of this class and the "default implementation"
     *   \alib{monomem,StringTreeNamesDynamic}.
     *
     * @tparam TTree The type of the templated instantiation of struct
     *               \alib{monomem,detail::StringTreeBase} that this method is invoked by.
     * @param  tree  The instance of struct \alib{monomem,detail::StringTreeBase} that invokes
     *               this method. Any member may be accessed, including
     *               \alib{monomem::detail::StringTreeBase,nodeTable} which contains the
     *               \alib{monomem,MonoAllocator} that the tree uses for the allocation of nodes.
     * @param  node  The node that is to be removed. Allows access to the key and
     *               custom value data. While the parent and sibling nodes are likewise accessible,
     *               it is strictly forbidden to modify those.
     */
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::Node& node )
    {
        (void) tree;
        (void) node;
    }

};

/** ********************************************************************************************
 * # 1. Introduction # {#alib_ns_strings_util_stringtree_overview}
 * This container type implements a directed, non-circular graph (tree) with named nodes.
 *
 * The internal node type \alib{monomem,detail::StringTreeBase::Node} stores:
 * - A pointer to a parent \b %Node,
 * - pointers to the previous and next sibling nodes,
 * - pointers to the first and last child nodes,
 * - a name string, which has to be unique in respect to the names of sibling nodes, and
 * - finally a data field holding the node's custom value of template type \p{T}.
 *
 * The way from the root node to a descendent node, usually is called "path". The class incorporates
 * functionality to work with string representations of such paths where names of child nodes are
 * concatenated and separated by a special separation character.
 *
 * The search and creation of tree nodes by using aforementioned path strings, is very similar to
 * what is well known from addressing files and folders in file systems.
 * This class does not differentiate between 'folders' and 'files', hence between 'nodes' and
 * 'leafs'. Every node has the same data of type \p{T} attached and may or may not have child nodes.
 * If such differentiation - or other semantics - is wanted, this may well be modeled by custom
 * attributes provided in template type \p{T}.
 *
 * \I{#############################################################################################}
 * # 2. Inner Types # {#alib_ns_strings_util_stringtree_inner}
 * Two public inner types exist.
 * All operations on tree nodes like insertion, deletion, search and attribute access is performed
 * using objects of public type \alib{monomem,StringTree::NodePtr}. This is a lightweight
 * "handle" containing a pointer to the originating tree object and to a represented node.
 *
 * Besides this, class \alib{monomem,StringTree::RecursiveIterator} allows recursive
 * iterations with built-in or custom sort orders.
 *
 * Both types are explained in the following paragraphs.
 *
 * \I{#############################################################################################}
 * ## 2.1 Inner Class NodePtr ## {#alib_ns_strings_util_stringtree_nodeptr}
 *
 * The main interface into class \b %StringTree is given by public, inner type
 * \alib{monomem,StringTree::NodePtr}. Method #Root returns an object of that type that
 * initially refers to the root node of the tree.
 * With this, child names and composite "paths" can be used to move the pointer along existing nodes
 * of the tree or to create new child nodes or even a whole path of such child nodes at once.
 *
 * Class \b %NodePtr is very lightweight as it contains just two pointers, one to to the
 * \b %StringTree it originates from and one to the tree node currently represented.
 * Hence, objects of this type can be copied, assigned and passed around very efficiently.<br>
 * The currently represented node's templated custom data can be accessed with method
 * \alib{monomem::StringTree,NodePtr::Value}.
 *
 * The methods to traverse over the nodes of the tree are:
 * - \alib{monomem::StringTree,NodePtr::GoToRoot},
 * - \alib{monomem::StringTree,NodePtr::GoToParent}.
 * - \alib{monomem::StringTree,NodePtr::GoToChild},
 * - \alib{monomem::StringTree,NodePtr::GoToNextSibling},
 * - \alib{monomem::StringTree,NodePtr::GoToPreviousSibling},
 * - \alib{monomem::StringTree,NodePtr::GoToFirstChild},
 * - \alib{monomem::StringTree,NodePtr::GoToLastChild},
 * - \alib{monomem::StringTree,NodePtr::GoToChild},
 * - \alib{monomem::StringTree,NodePtr::GoToTraversedPath},
 *
 * For some of these methods an alternative version exists, which returns a corresponding copy
 * of the node pointer, while leaving the original object unchanged. These methods share
 * the same name excluding the prefix <b>GoTo</b>.
 *
 * For the creation of new child nodes or a complete path of such, methods
 * - \alib{monomem::StringTree,NodePtr::GoToCreateChildIfNotExistent} and ,
 * - \alib{monomem::StringTree,NodePtr::GoToCreatedPathIfNotExistent}
 * are provided.
 *
 * Next, four methods that perform node deletion exist:
 * - \alib{monomem::StringTree,NodePtr::DeleteChild} (two overloaded versions),
 * - \alib{monomem::StringTree,NodePtr::DeleteChildren} and
 * - \alib{monomem::StringTree,NodePtr::Delete}
 *
 * The already mentioned methods:
 * - \alib{monomem::StringTree::NodePtr,GoToParent},
 * - \alib{monomem::StringTree::NodePtr,GoToFirstChild},
 * - \alib{monomem::StringTree::NodePtr,GoToLastChild},
 * - \alib{monomem::StringTree::NodePtr,GoToNextSibling} and
 * - \alib{monomem::StringTree::NodePtr,GoToPreviousSibling}
 *
 * of class \b NodePtr can be used to iterate from a node upward to the root node or through the
 * list of children of a node. Each method may \e invalidate the object in the case that no
 * corresponding parent or sibling node exists.
 * Invalid node pointer objects can be (or rather have to be!) detected using method
 * \alib{monomem::StringTree,NodePtr::IsValid}.
 * Most of the class's methods must not be invoked on an invalidated object. Doing so is undefined
 * behavior and raises an \alib assertion in debug-builds.
 * Invalid \b %NodePtr objects can reset to a valid state using methods
 * - \alib{monomem::StringTree::NodePtr,GoToRoot} and
 * - \alib{monomem::StringTree::NodePtr,GoToTraversedPath} if absolute path addressing is used.
 *
 * Instances that have been default-constructed may only be set to a valid state by (copy-)
 * assigning a valid instance.
 *
 * \I{#############################################################################################}
 * ## 2.2. Inner Class RecursiveIterator ## {#alib_ns_strings_util_stringtree_iterator}
 * Class \alib{monomem,StringTree::RecursiveIterator} provides a configurable and controlled
 * way of iterating a branch of a tree. Some features of the class are:
 * - Iterators can be initialized to start from any node of the tree
 *   Iteration ends when all (recursive) child nodes of the start node have been visited.
 * - The iteration follows a "depth first search" approach: Prior to visiting a sibling node,
 *   all children of a node are visited.
 * - The recursion depth can be limited, including to depth \c 0, which iterates only the
 *   direct child nodes of the start node.
 * - Prior to entering a new depth-level during iteration, different sort orders can be set.
 *   The choices are:
 *   - No sorting (iterates in order of node insertion).
 *   - Built-in sorting by node (path) name, ascending/descending, case sensitive or ignoring case
 *   - user defined by path name, number of children or any other attribute of the node, of-course
 *     including a node's custom data values.
 *
 * Class \b RecursiveIterator is of rather heavy weight and sorted iteration needs to allocate
 * memory for sorting the child nodes for each depth level of a potential recursion.
 * Therefore it is recommended to reuse instances of the class with subsequent, similar iterations.
 * In addition this explains why this class does not follow the concept of <c>std::iterator_traits</c>,
 * which is designed to be used with lightweight iterator types.
 *
 * \I{#############################################################################################}
 * \anchor alib_ns_monomem_stringtree_allocations
 * # 3. Node Allocation And Hashing # {#alib_ns_strings_util_stringtree_hashing}
 * While each node maintains a doubly linked list of child nodes for iteration, this class stores
 * each inserted element in a \alib{monomem,HashTable} using the parent node and the element's
 * name as a unique key.
 * This is done to be able to search for a child with a given name in constant time.
 * This container does not perform any other memory allocations than those that this
 * \b HashTable does.
 *
 * With that, the implementation of this container type is able to leave all allocation and
 * <em>"recycling"</em> of node elements to this hash table, which is found in base class's field
 * \alib{monomem,detail::StringTreeBase::nodeTable}. As explained in the documentation of
 * \alib{monomem,HashTable}, its allocation mechanism can be made safe in respect to memory
 * exhaustion of the underlying \alib{monomem,MonoAllocator}, even if a virtually unlimited number of
 * insertions and removals of elements is performed. Such safeness depends on template
 * parameter \p{TRecycling} which is just passed to the definition of the internal hash table of
 * nodes.
 * \see
 *   For more information on this topic see also chapter \ref alib_monomem_intro_recycling
 *   of this \alibmod_nl Programmer's Manual.
 *
 * \I{#############################################################################################}
 * \anchor alib_ns_monomem_stringtree_namestrings
 * # 4. Node and Node Name String Allocation # {#alib_ns_strings_util_stringtree_allocations}
 *
 * The C++ version of this class allows user-defined allocation (and copying) of the node's name
 * character strings. For this, template parameter \p{TNodeMaintainer} is defined,
 * which defaults to built-tin struct \alib{monomem,StringTreeNamesDynamic}.
 * This default "node maintainer" defines the name type of nodes to \alib{strings,TLocalString,LocalString}
 * with a default capacity of \c 32 characters. This way, dynamic allocation is performed automatically
 * by this local string when a node is constructed.<br>
 * In the case that many "long" node names are expected, it might be efficient to set the capacity
 * to \c 0. In this case, type \b StringTreeNamesDynamic defines the string type to a normal
 * \b String and uses C++ keywords <c>new</c> and <c>delete[]</c> to allocate
 * and free character buffers for the name string.
 *
 * A second built-in and ready to use "node maintainer" type is given with
 * \alib{monomem,StringTreeNamesStatic}. This uses an unbuffered \b String and has empty
 * implementations of its static methods. This way no copies of the original string buffers that
 * are passed to the to interface methods of class \b NodePtr that create children are made.
 * This is useful (and very efficient!) if \b all child name and creation path strings provided
 * of class \b %NodePtr are permanently valid (in other words, their life-cycle spans over
 * the life-cycle of the nodes in a tree) and therefore do not need to be copied to dedicated
 * allocated memory. If this precondition is not assured, the \b StringTree produces undefined
 * behavior.
 *
 * Finally, string trees that during their lifetime just grow and where no nodes (or only a limited
 * number of nodes) are removed until the whole tree is disposed, may be instantiated using
 * the third built-in type \alib{monomem,StringTreeNamesMonoAlloc}.
 * With that, the concept of \ref alib_monomem_intro "monotonic allocation" that this container
 * type uses can be extended to the node name strings.
 * Type \b StringTreeNamesMonoAlloc grabs the allocator from the tree provided with method
 * \alib{monomem::StringTreeNamesMonoAlloc,InitializeNode} and just clones the given string
 * into this.
 *
 * \I{#############################################################################################}
 * \anchor alib_ns_monomem_stringtree_referencedoc
 * # Reference Documentation #
 *
 * @tparam T  The custom value type of elements stored in this container.
 *
 * @tparam TNodeMaintainer
 *         A template type that needs to provide an interface as documented with
 *         \alib{monomem,StringTreeNamesDynamic}, which is also the default type
 *         if not specified. For details see
 *         \ref alib_ns_strings_util_stringtree_allocations "corresponding section" of this
 *         class's documentation.
 *
 * @tparam TRecycling
 *         Denotes the type of recycling that is to be performed. Possible values are
 *         \alib{monomem,Recycling::Private} (the default),
 *         \alib{monomem,Recycling::Shared} or \alib{monomem,Recycling::None}.
 **************************************************************************************************/
template<typename T,
         typename TNodeMaintainer= StringTreeNamesDynamic<character>,
         typename TRecycling     = Recycling::Private             >
class StringTree  : protected detail::StringTreeBase<T, TNodeMaintainer, TRecycling>
{
#if !defined(ALIB_DOX)
    protected:
        friend class NodePtr;

        using basetree         =  detail::StringTreeBase<T,TNodeMaintainer,TRecycling>;
        using baseNodeKey      = typename basetree::NodeKey;
        using baseNode         = typename basetree::Node;
        using baseNodePtr      = typename basetree::NodePtrBase;
#endif

public:

    /** The character type of node names and paths strings. This is defined using character type
     *  definition \alib{monomem::StringTreeNamesDynamic,CharacterType} of template type
     *  \p{TNodeMaintainer}. */
    using CharacterType= typename TNodeMaintainer::CharacterType;

    /** The string type of node names and paths. This is defined using character type definition
     *  \alib{monomem::StringTreeNamesDynamic,CharacterType} of template type \p{TNodeMaintainer}. */
    using NameType     = typename strings::TString<CharacterType>;

    /** The sub-string type of paths. This is defined using character type definition
     *  \alib{monomem::StringTreeNamesDynamic,CharacterType} of template type \p{TNodeMaintainer}. */
    using SubstringType= typename strings::TSubstring<CharacterType>;

    /** This type definition may be used to define an externally managed shared recycler,
     *  which can be passed to the alterative constructor of this class when template
     *  parameter \p{TRecycling} equals \alib{monomem,Recycling::Shared}.
     *  \see
     *    Chapter \ref alib_monomem_containers_recycling_shared of the Programmer's Manual
     *    for this \alibmod.                                                          */
    using TSharedRecycler= typename basetree::TSharedRecycler;

    /** ********************************************************************************************
     * This public, inner class provides the main interface into outer class \b StringTree.
     * The class should be considered being similar to a simple pointer or to a lightweight
     * iterator type, which refers to a tree and a current node.<br>
     * The class's interface allows the access to a node's name and value and to to insert and
     * remove child nodes.
     *
     * Instances of this class can be received with methods
     * \alib{monomem,StringTree::Root} and
     * \alib{monomem,StringTree::RecursiveIterator::Node}.
     *
     * The default constructor creates an invalid object, which has to be initialized by
     * assigning a valid object prior to its first use.
     *
     * \see
     *   For more information on how this class is used, see paragraph
     *   \ref alib_ns_strings_util_stringtree_nodeptr "2.1 Inner Class NodePtr"
     *   of the description of class \b %StringTree.
     *
     * ## Friends ##
     * class \alib{monomem,StringTree}
     **********************************************************************************************/
    class NodePtr : protected baseNodePtr
    {
        #if !defined(ALIB_DOX)
            friend class StringTree;
        #endif

        //####  Protected methods (class NodePtr)  ############################################
        protected:

            /**
             * Internal constructor
             * @param pTree  The \b %StringTree we work on.
             * @param pNode  The node to refer to.
             */
            NodePtr( basetree* pTree, baseNode* pNode)                                  noexcept
            : baseNodePtr( pTree, pNode )
            {}

        //####  Constructor, comparison operators, etc     #####################################
        public:

            /**
             * Public constructor. Creates a node pointer which is not valid.
             * The only way to make a default-constructed instance valid is by (copy-) assigning
             * another instance.
             */
            NodePtr()                                                           noexcept  = default;

            /** Trivial default copy constructor. */
            NodePtr( const NodePtr&  )                                          noexcept  = default;

            /** Trivial default move constructor. */
            NodePtr(       NodePtr&& )                                          noexcept  = default;

            /** Trivial default copy assign operator.
              * @return A reference to \c this.        */
            NodePtr& operator=( const NodePtr&  )                               noexcept  = default;

            /** Trivial default move assign operator.
              * @return A reference to \c this.        */
            NodePtr& operator=(       NodePtr&& )                               noexcept  = default;

            /** Trivial default destructor. */
            ~NodePtr()                                                          noexcept  = default;


            /** Comparison operator.
             *  @param other  The object to compare ourselves to.
             *  @return \c true if this and given node pointer are equal, \c false otherwise. */
            bool operator==(const NodePtr &other)                                              const
            {
                return     baseNodePtr::node == other.node
                        && baseNodePtr::tree == other.tree;
            }

            /** Comparison operator.
             *  @param other  The object to compare ourselves to.
             *  @return \c false if this and given node pointer are equal, \c true otherwise. */
            bool operator!=(const NodePtr &other)                                              const
            {
                return !( (*this) == other );
            }

            /**
             * Determines if this is a valid object. Node pointers may become invalid with
             * transition methods like #GoToParent, #GoToFirstChild or GoToNextSibling.
             * An invalid object may be turned into a valid one by either
             * - assigning a valid object (copy assignment), or
             * - invoking method #GoToRoot, or
             * - invoking method #GoToTraversedPath using absolute path addressing.
             *
             * Note that the latter is not applicable to a default-constructed objects
             * (which are also invalid) as with such no \b %StringTree is assigned.
             *
             * @return \c true if this is a valid node pointer.
             *          If invalid, \c false is returned and this object must not be used.
             */
            bool        IsValid()                                                              const
            {
                return baseNodePtr::node != nullptr;
            }

            /**
             * Returns the opposite of #IsValid.
             *
             * @return \c true if this is an invalid node pointer that must not be used,
             *         \c false otherwise.
             */
            bool        IsInvalid()                                                            const
            {
                return !IsValid();
            }

        //####  Tree navigation inplace, returning status      #################################
            /**
             * Creates a node pointer value representing the same node like this pointer.
             * @return A clone of this node pointer.
             */
            NodePtr     Clone()                                                                const
            {
                return NodePtr( baseNodePtr::tree, baseNodePtr::node );
            }

            /**
             * Returns a node pointer to the root node of the tree.
             *
             * @return A node pointer representing the root node of the tree this pointer
             *         represents.
             */
            NodePtr     Root()                                                                 const
            {
                return NodePtr( baseNodePtr::tree, baseNodePtr::tree->root );
            }

            /**
             * Returns a node pointer to the root node of the tree.
             *
             * @return A node pointer representing the root node of the tree this pointer
             *         represents.
             */
            NodePtr     GoToRoot()
            {
                return baseNodePtr::node= baseNodePtr::tree->root;
            }

            /**
             * Creates a node pointer value representing the parent node of the
             * node represented by this object.
             *
             * If this object represents the root node of the tree, the returned node pointer
             * is invalid.
             *
             * @return A node pointer representing the parent node of the
             *         node represented by this node pointer.
             */
            NodePtr     Parent()                                                               const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return NodePtr( baseNodePtr::tree, baseNodePtr::node->parent);
            }

            /**
             * Moves this node pointer to its parent node.
             * If this is the root node, this object becomes invalid.
             *
             * @return *this to allow concatenated calls
             */
            NodePtr&    GoToParent()
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                baseNodePtr::node= baseNodePtr::node->parent;
                return (*this);
            }

            /**
             * Returns a node pointer value that represents the next sibling of the node
             * represented this node pointer.
             * If the node has no next sibling, an invalid node pointer is returned.
             *
             * @return A node pointer object representing the next sibling of the node
             *         represented by this object.
             */
            NodePtr     NextSibling()                                                          const
            {
                return NodePtr(baseNodePtr::tree, HasNextSibling() ? baseNodePtr::node->next() : nullptr );
            }

            /**
             * Moves this node pointer to the next sibling of the represented node.
             * If the node has no next sibling, this node pointer becomes invalid.
             * The latter is always true if this is the root node of the tree.
             *
             * @return \c true if this node pointer was moved,
             *         \c false if the represented node has no next sibling.
             */
            bool        GoToNextSibling()
            {
                if( HasNextSibling() )
                {
                    baseNodePtr::node= baseNodePtr::node->next();
                    return true;
                }
                baseNodePtr::node= nullptr;
                return false;
            }

            /**
             * Returns a node pointer value that represents the previous sibling of the node
             * represented this node pointer.
             * If the node has no previous sibling, an invalid node pointer is returned.
             *
             * @return A node pointer object representing the previous sibling of the node
             *         represented by this object.
             */
            NodePtr     PreviousSibling()                                                      const
            {
                return NodePtr(baseNodePtr::tree,
                               HasPreviousSibling() ? baseNodePtr::node->prev()
                                                    : nullptr );
            }

            /**
             * Moves this node pointer to the previous sibling of the represented node.
             * If the node has no previous sibling, this node pointer becomes invalid.
             * The latter is always true if this is the root node of the tree.
             *
             * @return \c true  if this node pointer was moved,
             *         \c false if the represented node has no previous sibling.
             */
            bool        GoToPreviousSibling()
            {
                if( HasPreviousSibling() )
                {
                    baseNodePtr::node= baseNodePtr::node->prev();
                    return true;
                }
                baseNodePtr::node= nullptr;
                return false;
            }

            /**
             * Returns a node pointer value that represents the first child of the node
             * represented.
             * If the represented node has no children, an invalid node pointer is returned.
             *
             * @return A node pointer representing the first child of the node represented
             *         by this node pointer.
             */
            NodePtr     FirstChild()                                                           const
            {
                return NodePtr( baseNodePtr::tree, HasChildren() ? baseNodePtr::node->children.first()
                                                                 : nullptr                             );
            }

            /**
             * Moves this node pointer to the first child of its represented node.
             * If the represented node has no children, this node pointer becomes invalid.
             *
             * @return \c true if the node pointer was moved, \c false if the represented node
             *         has no children.
             */
            bool        GoToFirstChild()
            {
                if( HasChildren() )
                {
                    baseNodePtr::node= baseNodePtr::node->children.first();
                    return true;
                }
                baseNodePtr::node= nullptr;
                return false;
            }

            /**
             * Returns a node pointer value that represents the last child of the node
             * represented.
             * If the represented node has no children, an invalid node pointer is returned.
             *
             * @return A node pointer representing the last child of the node represented
             *         by this node pointer.
             */
            NodePtr     LastChild()                                                            const
            {
                return NodePtr(baseNodePtr::tree, HasChildren() ? baseNodePtr::node->children.last() : nullptr);
            }

            /**
             * Moves this node pointer to the last child of its represented node.
             * If the represented node has no children, this node pointer becomes invalid.
             *
             * @return \c true if the node pointer was moved, \c false if the represented node
             *         has no children.
             */
            bool        GoToLastChild()
            {
                if( HasChildren() )
                {
                    baseNodePtr::node= baseNodePtr::node->children.last();
                    return true;
                }
                baseNodePtr::node= nullptr;
                return false;
            }

            /**
             * Searches a child with the given name and returns a node pointer to it.
             * If no child with this name exists, the returned node pointer is invalid
             *
             * The given \p{name} is not considered a path and is not checked for being <c>"."</c>
             * or <c>".."</c> or if it contains a separator character.
             * Children with such name can not exist and hence can't be found. However, in
             * debug builds, an \ref ALIB_WARNING is reported.
             *
             * @param  name   The name of the child to search.
             * @return A node pointer representing the last child of the node represented
             *         by this node pointer.
             */
            NodePtr     Child( const NameType& name )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                ALIB_DBG( baseNodePtr::tree->checkChildName( name ); ) // gives warning

                return NodePtr( baseNodePtr::tree, baseNodePtr::node->findChild( baseNodePtr::tree, name ) );
            }

            /**
             * Searches a child with the given name and moves this node pointer to it.
             * If no child with this name exists, the node pointer does not change and
             * \c false is returned.
             *
             * The given \p{name} is not considered a path and is not checked for being <c>"."</c>
             * or <c>".."</c> or if it contains a separator character.
             * Children with such name can not exist and hence can't be found. However, in
             * debug builds, an \ref ALIB_WARNING is reported.
             *
             * @param  name   The name of the child to search.
             * @return \c true if the child existed and this object changed, \c false
             *         otherwise.
             */
            bool        GoToChild( const NameType& name )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                ALIB_DBG( baseNodePtr::tree->checkChildName( name ); )

                baseNode* child= baseNodePtr::node->findChild( baseNodePtr::tree, name );
                if( child)
                {
                    baseNodePtr::node= child;
                    return true;
                }
                return false;
            }

            /**
             * Moves this node pointer to the child with given \p{name}.
             * If no child with this name exists, one will be created.
             *
             * If the given \p{childName} is invalid (equals to <c>"."</c> or
             * <c>".."</c> or contains the separation character), then still \c true is returned,
             * but this node pointer becomes invalid.
             * In addition, with debug builds, an \ref ALIB_WARNING is reported.
             *
             * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
             * @param  name   The name of the desired child.
             * @param  args   Variadic parameters to be forwarded to constructor of custom type
             *                \p{T} in the case a child is created.
             * @return A pair of a node pointer pointing to the child and a boolean that equals
             *         \c false if the child was found, and \c true if a child was created.
             *         If the given name was invalid, the returned node pointer will be invalid
             *         while the boolean still indicates "not found" (aka \c true).
             */
            template<typename... TArgs>
            std::pair<NodePtr, bool> CreateChildIfNotExistent( const NameType& name,
                                                               TArgs&&... args  )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )

                if( !baseNodePtr::tree->checkChildName( name ) )
                    return std::make_pair( NodePtr(baseNodePtr::tree, nullptr), true );

                auto result= baseNodePtr::node->findOrCreateChild( baseNodePtr::tree, name,
                                                            std::forward<TArgs>(args)... );

                return std::make_pair( NodePtr( baseNodePtr::tree, result.first ), result.second );
            }

            /**
             * Moves this node pointer to the child with given \p{name}.
             * If no child with this name exists, one will be created.
             *
             * If the given \p{childName} is invalid (equals to <c>"."</c> or
             * <c>".."</c> or contains the separation character), then still \c true is returned,
             * but this node pointer becomes invalid.
             * In addition, with debug builds, an \ref ALIB_WARNING is reported.
             *
             * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
             * @param  name   The name of the desired child.
             * @param  args   Variadic parameters to be forwarded to constructor of custom type
             *                \p{T} in the case a child is created.
             * @return \c false if the child was found, and \c true if one was created or the given
             *         child name was invalid.
             */
            template<typename... TArgs>
            bool        GoToCreateChildIfNotExistent( const NameType& name, TArgs&&... args  )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )

                if( !baseNodePtr::tree->checkChildName( name ) )
                {
                    baseNodePtr::node= nullptr;
                    return true;
                }

                auto result= baseNodePtr::node->findOrCreateChild( baseNodePtr::tree, name,
                                                            std::forward<TArgs>(args)... );

                baseNodePtr::node= result.first;
                return result.second;
            }

            /**
             * Follows the given \p{path} from the currently represented node to the target
             * node.
             *
             * The method supports absolute and relative path addressing: If \p{path} begins
             * with a separation character, then the transition starts with the root node of the
             * \b %StringTree. Furthermore, child name <c>"."</c> is ignored and just skipped
             * while a name of <c>".."</c> addresses the parent node during the transition.
             * Repeated separation characters are ignored.<br>
             * If, while processing the path string, the root node is found an the next
             * path element is "..", this element is ignored and processing continues.
             * As a sample, assuming that nodes \e /a and \e /b exist, the paths:
             *
             *      /a/../b
             * and
             *
             *      /a/../../b
             * both evaluate to
             *
             *      /b
             *
             * Relative paths must not be used on
             * \alib{monomem::StringTree,NodePtr::IsValid,invalid} node pointers. Doing so
             * is undefined behavior and raises an \alib assertion in debug-compilations.
             *
             * If a child along the path does not exist, the traversal is ended and the remaining
             * portion of the path is returned.
             *
             * \note
             *   If parameter \p{path} is a temporary object, the resulting \b Substring must
             *   not be used, as it refers to the given string's buffer. In any case, its
             *   length can still be compared to \c 0 to evaluate success of the traversal.
             *
             * @param path  The path to follow, starting with the node this pointer represents.
             *
             * @return A pair of a node pointer pointing to last child not of the existing portion
             *         of the given \p{path}, and a substring that contains the non-existing
             *         portion of a path, or is empty if the complete path existed.
             */
            std::pair<NodePtr, SubstringType> TraversePath( const NameType& path )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                SubstringType remainingPath(path);
                baseNode* grandChild= baseNodePtr::followPath( remainingPath );
                return std::make_pair( NodePtr(baseNodePtr::tree, grandChild), remainingPath );
            }

            /**
             * Same as #TraversePath, but moves this node pointer instead of returning one.
             *
             * @param path  The path to move this node pointer along.
             * @return The unconsumed portion of the path.
             *         An empty \b Substring if the path existed.
             */
            SubstringType GoToTraversedPath( const NameType& path )
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                SubstringType remainingPath(path);
                baseNodePtr::node= baseNodePtr::followPath( remainingPath );
                return remainingPath;
            }

            /**
             * Follows the given path and creates non-existing children along the way.
             *
             * Child names <c>"."</c> and <c>".."</c> are allowed and respected same
             * as documented with method #TraversePath.
             *
             * New child nodes are constructed by forwarding the given \p{args}. Existing children
             * remain untouched.
             *
             * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
             * @param  path   The path to move along.
             * @param  args   Variadic parameters to be forwarded to constructor of each node
             *                that is created.
             * @return A <c>std::pair</c> containing a resulting NodePtr and the number of nodes
             *         created.
             */
            template<typename... TArgs>
            std::pair<NodePtr, integer>  CreatePathIfNotExistent( const NameType& path,
                                                                  TArgs&&... args  )
            {
                ALIB_ASSERT_ERROR( IsValid() || path.CharAtStart() == baseNodePtr::tree->separator,
                             "MONOMEM/STRINGTREE",
                             "Invalid StringTree::NodePtr given with relative path addressing." )

                auto result= baseNodePtr::followPathCreate( path, std::forward<TArgs>(args)... );

                return std::make_pair( NodePtr(baseNodePtr::tree, result.first), result.second );
            }

            /**
             * Follows the given path and creates non-existing children along the way.
             *
             * Child names <c>"."</c> and <c>".."</c> are allowed and respected same
             * as in #TraversePath.
             *
             * New child nodes are constructed by forwarding the given \p{args}. Existing children
             * remain untouched.
             *
             * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
             * @param  path   The path to move along.
             * @param  args   Variadic parameters to be forwarded to constructor of each node
             *                that is created.
             * @return The number of nodes created.
             */
            template<typename... TArgs>
            integer                  GoToCreatedPathIfNotExistent( const NameType& path,
                                                                  TArgs&&... args        )
            {
                ALIB_ASSERT_ERROR( IsValid() || path.CharAtStart() == baseNodePtr::tree->separator,
                             "MONOMEM/STRINGTREE",
                             "Invalid StringTree::NodePtr given with relative path addressing." )

                auto result= baseNodePtr::followPathCreate( path, std::forward<TArgs>(args)... );
                baseNodePtr::node= result.first;
                return result.second;
            }

        //####  NodePtr Interface        #######################################################
            /**
             * Returns the name of the represented node.
             * Note that the concatenated names of recursive child nodes, separated by
             * \p{TSeparator} constitute a \e path.
             *
             * @return A constant reference to the name of the represented node.
             */
            const NameType& Name()                                                             const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->name;
            }

            /**
             * Returns the three that this pointer is created of.
             *
             * @return The tree that this object refers to.
             */
            StringTree&     Tree()
            {
                ALIB_ASSERT_ERROR( baseNodePtr::tree, "MONOMEM/STRINGTREE",
                                   "StringTree::NodePtr is not initialized." )
                return *static_cast<StringTree*>(baseNodePtr::tree);
            }

            /** Retrieves a reference to the templated value of type \p{T} stored
             *  in the represented node.
             * @return The current node's value.          */
            T&              Value()
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->data;
            }

            /** Retrieves a constant reference to the templated value of type \p{T} stored in
             *  the represented node.
             * @return The current node's value.          */
            const T&        Value()                                                            const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->data;
            }

            /**
             * Returns \c true if this node pointer represents the root node of the
             * \b %StringTree, \c false otherwise.
             * @return \c true if this object represents the root node, \c false otherwise.
             */
            bool            IsRoot()                                                           const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->isRoot();
            }

            /**
             * Determines the depth of the node represented by this object. This is done by
             * counting the iterations needed to reach the root node of the tree.
             * @return The distance from this node to the root node.
             */
            int             Depth()                                                            const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->depth();
            }

            /**
             * Returns \c true if the represented node has at least one direct child.
             * @return \c true if the current node has children, \c false otherwise.
             */
            bool            HasChildren()                                                      const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->qtyChildren != 0;
            }

            /**
             * Returns the number of direct children of the represented node.<br>
             * Note that this method runs in constant time.
             * @return The number of direct children of the represented node.
             */
            uinteger        CountChildren()                                                    const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return baseNodePtr::node->qtyChildren;
            }

            /**
             * Evaluates if the node represented by this object has a next sibling in its
             * parent's list of children.
             * @return \c true if a next sibling to this object's represented node exists,
             *         \c false otherwise.
             */
            bool            HasNextSibling()                                                   const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return     !IsRoot()
                        && !baseNodePtr::node->parent->children.isLast( baseNodePtr::node );
            }

            /**
             * Evaluates if the node represented by this object has a previous sibling in its
             * parent's list of children.
             * @return \c true if a previous sibling to this object's represented node exists,
             *         \c false otherwise.
             */
            bool            HasPreviousSibling()                                               const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return     !IsRoot()
                        && !baseNodePtr::node->parent->children.isFirst( baseNodePtr::node );
            }

            /**
             * Writes the path to the represented node (excluding the represented node's name)
             * to the given \b %AString.<br>
             * If this node represents the root node, then nothing is written.<br>
             * No separation character is prepended to the start and appended to the end of the
             * resulting path.
             *
             * Note that this method is rather inefficient, as the list of parent nodes have
             * to be inserted to the beginning of the string in an iteration loop, which implies
             * a string copy per parent node.
             * In many situations, it is recommended to search for other ways to keep track of
             * the current path of a node.
             * Class \alib{monomem,StringTree::RecursiveIterator} for example, optionally
             * allows to maintain a string representing the current path with every iteration.
             *
             * @param target        The target to append the path to.
             * @param targetData    Denotes whether \p{target} should be cleared prior to
             *                      appending the path. Defaults to \b CurrentData::Clear.
             * @return The given \b AString to allow concatenated operations on it.
             */
            AString&        AssemblePath( AString& target,
                                          CurrentData targetData= CurrentData::Clear )         const
            {
                if( targetData == CurrentData::Clear )
                    target.Reset();

                return baseNodePtr::node->assemblePath(target, baseNodePtr::tree->separator);
            }

        //####  NodePtr child creation     #####################################################
            /**
             * Creates and returns a child node. If a node already exists, nothing is done and
             * \c nullptr is returned as this is considered an error.
             *
             * If the child name is illegal (equal to <c>"."</c> or <c>".."</c> or contains a
             * separation character), an \alib warning is raised and an invalid node pointer
             * is returned.
             *
             * Template parameter \p{TCheck} may be used to suppress the search for an
             * existing child with the same name, as well as the check for correctness of the
             * given child name.
             * This tremendously improves the execution performance of this method.
             *
             * \attention Setting template parameter \p{TCheck} to \c false and inserting
             *            child nodes with the same name, sets a \b %StringTree to an
             *            undefined state.
             *
             * @tparam TCheck     If \c false, no check for an existing child with the same name is
             *                    performed.
             * @tparam TArgs      Types of variadic parameters given with parameter \p{args}.
             * @param  childName  The name of the child
             * @param  args       Variadic parameters to be forwarded to constructor of custom
             *                    type \p{T} of the child created.
             *
             * @return A new node pointer object representing the created child node.
             *         If the given \p{childName} was invalid or the child existed already,
             *         the returned object is invalid.
             */
            template<bool TCheck =true, typename... TArgs>
            NodePtr CreateChild( const NameType& childName, TArgs&&... args )                  const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                if ALIB_CONSTEXPR17 (TCheck)
                {
                    // check name
                    if( !baseNodePtr::tree->checkChildName( childName ) )
                    {
                        ALIB_WARNING( "STRINGTREE", "Illegal child name" )
                        return NodePtr( baseNodePtr::tree, nullptr );
                    }

                    // check existence
                    if(     baseNodePtr::node->qtyChildren > 0
                        &&  baseNodePtr::tree->nodeTable.Contains( baseNodeKey( baseNodePtr::node, childName) ))
                        return NodePtr( baseNodePtr::tree, nullptr );
                }

                baseNode* child= &baseNodePtr::tree->nodeTable.EmplaceUnique( baseNodePtr::node, childName,
                                                                              std::forward<TArgs>(args)... )
                                                   .Value();
                TNodeMaintainer::InitializeNode( *baseNodePtr::tree, *child );

                baseNodePtr::node->children.pushEnd( child );
                ++baseNodePtr::node->qtyChildren;

                return NodePtr( baseNodePtr::tree, child );
            }

        //####  NodePtr deletion         #######################################################
            /**
             * Searches and deletes the child named \p{childName} from the node that this object
             * refers to. This object itself is not changed.
             *
             * \see
             *   Overloaded version of this method that accepts a node pointer referring to
             *   the child in question.
             *
             *
             * @param   childName   The name of the desired child.
             * @return \c true if the child existed and was deleted, \c false otherwise.
             */
            bool DeleteChild( const NameType& childName )                                       const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                if( baseNodePtr::node->qtyChildren == 0 )
                    return false;

                auto handle= baseNodePtr::tree->nodeTable.Extract( baseNodeKey( baseNodePtr::node, childName) );
                if( handle.IsEmpty() )
                    return false;

                handle.Value().deleteChildren( baseNodePtr::tree );

                TNodeMaintainer::FreeNode( *baseNodePtr::tree, handle.Value() );
                handle.Value().remove();

                --baseNodePtr::node->qtyChildren;
                return true;
            }

            /**
             * Deletes the child represented by the given node pointer \p{child} from the
             * node that this node pointer refers to.
             * After the invocation the given \p{child} refers to its next sibling. If no
             * such sibling exists, \p{child} becomes invalid.
             * This node pointer itself is not changed.
             *
             * \note
             *   This method is handy to implement forward iterations through children of
             *   a parent node with the aim to delete certain child nodes.
             *   No corresponding version of this method exists that moves the given
             *   \p{child} pointer to its previous sibling. For reverse iterations, a
             *   \ref Clone "clone" of the \p{child} argument has to be passed.
             *   However, any overhead caused by such temporary object creation will be
             *   optimized out by modern C++ compilers.
             *
             * @param   child   Deletes the child represented by the given node.
             */
            void DeleteChild( NodePtr& child )                                                 const
            {
                ALIB_ASSERT_ERROR(       IsValid(), "MONOMEM/STRINGTREE",
                                   "Invalid StringTree::NodePtr." )
                ALIB_ASSERT_ERROR( child.IsValid(), "MONOMEM/STRINGTREE",
                                   "Invalid StringTree::NodePtr given for parameter 'child'." )
                baseNode* nodeToDelete= child.node;
                child.GoToNextSibling();
                baseNodePtr::node->deleteChild( baseNodePtr::tree, nodeToDelete );
            }

            /**
             * Deletes the children of the node that this node pointer refers to.
             * This object itself is not changed.
             *
             * @return The number of children that were deleted.
             */
            uinteger DeleteChildren()                                                          const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                return  baseNodePtr::node->deleteChildren( baseNodePtr::tree );
            }

            /**
             * Deletes the branch that this node pointer refers to from the tree.
             * If this node pointer does not represent the root node, then after the operation,
             * it refers to the parent of the current node.<br>
             * If the represented node is the root node, only the children are deleted and this
             * object remains representing the root node.
             *
             * \note
             *   If this method is invoked on an object returned by method
             *   \alib{monomem::StringTree,RecursiveIterator::Node}, the invoking iterator
             *   becomes invalid.<br>
             *   To avoid this, method \alib{monomem::StringTree,RecursiveIterator::DeleteNode} is
             *   to be used.
             *
             * @return
             *   The total number of nodes deleted. Can be zero, in case this object represents
             *   the root node and this node has no children.
             */
            uinteger Delete()
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE", "Invalid StringTree::NodePtr." )
                if( baseNodePtr::node->isRoot() )
                    return baseNodePtr::node->deleteChildren( baseNodePtr::tree );

                baseNode* child= baseNodePtr::node;
                baseNodePtr::node= baseNodePtr::node->parent;
                return baseNodePtr::node->deleteChild( baseNodePtr::tree, child );
            }
    };  // inner class NodePtr


    /** ********************************************************************************************
     * This public, inner class can be used to recursively iterate through the nodes of
     * outer class \b StringTree.<br>
     * The type does <b>not</b> apply to the concept of \c std::iterator_traits.
     * The rationale for this is the fact that mechanics for sorting the child nodes are
     * provided, which requires allocation of more resources than usual container iterators do.
     * Therefore, objects of this type are not supposed to be temporary and
     * created "on the fly", e.g. in range based loops introduced with C++ 11 .
     * Instead, instances should rather be created once and then re-used with subsequent
     * iterations.
     *
     * The sorting of child nodes is optional and can be changed prior to each recursion.
     * A built-in comparison function which works on node names (path names) allows to choose
     * ascending and descending order and to ignore or be sensitive about letter case.
     * Besides this, custom comparison functions that take a combination of arbitrary node
     * attributes, including a node's value of template type \p{T} can be established.
     * See overloaded methods #SetSorting for details on this topic.
     *
     * Objects of this type can be initialized, respectively reset to distinct start nodes by
     * providing objects of
     * - type \b %StringTree,
     * - type \b %StringTree::NodePtr or
     * - other objects of this type itself,
     *
     * to overloaded methods #Initialize.
     *
     * The maximum depth of recursion may be limited with optional parameter \p{depth}
     * found with each overloaded version of #Initialize.
     * During the iteration, the recursion can be individually selected per node visited.
     * This is done by using either of methods #Next or #NextSibling to proceed.
     * Furthermore, method #NextParentSibling allows to skip the rest of the current iteration
     * branch.<br>
     * The end of an iteration is detected with method #IsValid.
     *
     * The children of a node can be iterated in a sorted fashion. To enable and disable
     * sorting, various overloads of #SetSorting exist. These methods may be invoked any
     * time during the iteration. Whenever a recursion in iteration occurs, the most recent
     * settings of sorting are respected for the list of children of the node that is
     * processed with that recursion.
     *
     * Finally, the generation of a string representing the actual path to the current
     * iteration node, relative to the iteration's start node can be activated.
     * See method #SetPathGeneration for more information about this feature.
     *
     * \see
     *   For more information on how this class is used, see paragraph
     *   \ref alib_ns_strings_util_stringtree_iterator "2.2 Inner Class RecursiveIterator"
     *   of the description of class \b %StringTree.
     *
     * ## Friends ##
     * class \alib{monomem,StringTree}
     **********************************************************************************************/
    class RecursiveIterator
    {
        #if !defined(ALIB_DOX)
            friend class StringTree;
        #endif
        protected:

        //######################################################################################
        // Inner type RecursionData
        //######################################################################################
        /** Protected, internal struct used to store the data of recursive iterations. */
        struct RecursionData
        {
            /**
             * Combines a node hook and and a current vector index used to remember the
             * current child in unsorted, respectively sorted mode.
             */
            union
            {
                /** The current child of the current node in case of unsorted access
                 *  If this is pointing to the end of the child map, then
                 *  the actual node itself is selected by this RecursiveIterator. */
                baseNode*           unsorted;

                /** The current child index in case of sorted access.
                 *  A value of <c>static_cast<size_t>(-1)</c> indicates that
                 *  the actual node itself is selected. */
                size_t              sorted;

            } actChild;

            /** The child hook of the parent node, used with unsorted iteration. */
            typename basetree::NodeList*     childrenUnsorted;

            /** A pointer to a dynamically allocated vector of children used with sorting. */
            std::vector<baseNode*>  childrenSorted;

            /** Copied from
             * \alib{monomem::detail::StringTree,RecursiveIterator::nextCustomSorter}
             *  with every recursion step. */
            bool                  (*customSorter)(const NodePtr&, const NodePtr&);

            /** Copied from
             * \alib{monomem::detail::StringTree,RecursiveIterator::nextIsSorting}
             *  with every recursion step. */
            bool                    isSorting;

            /** Copied from
             * \alib{monomem::detail::StringTree,RecursiveIterator::nextSortingIsDescending}
             *  with every recursion step. */
            bool                    sortingIsDescending;

            /** Copied from
             * \alib{monomem::detail::StringTree,RecursiveIterator::nextSortingIsCaseSensitive}
             *  with every recursion step. */
            bool                    sortingIsCaseSensitive;


            /** Trivial default constructor. */
            RecursionData()                                             noexcept          = default;

            /** Trivial default copy constructor. */
            RecursionData( const RecursionData&  )                                        = default;

            /** Trivial default move constructor. */
            RecursionData(       RecursionData&& )                      noexcept          = default;

            /** Trivial default copy assign operator.
              * @return A reference to \c this.           */
            RecursionData& operator=( const RecursionData&  )                             = default;

            /** Trivial default move assign operator.
              * @return A reference to \c this.           */
            RecursionData& operator=(       RecursionData&& )           noexcept          = default;

            /** Trival default destructor  */
            ~RecursionData()                                            noexcept          = default;
        }; // inner struct RecursionData

        //######################################################################################
        //  RecursiveIterator members
        //######################################################################################

        /** The \b %StringTree this iterator belongs to. */
        StringTree*                 tree                                                  = nullptr;

        /** The pointer to the actual node. */
        baseNode*                   node;

        /**
         * A stack holding the recursive list of unsorted or sorted children and the
         * hook to the current child. Implemented as a vector in combination with member #actDepth,
         * to reuse allocated storage space during iteration and when this iterator is
         * re-used (freshly initialized).
         */
        std::vector<RecursionData>  stack;

        /**
         * The current depth of the iteration (and usage but not size of field #stack).
         * set to \c -1 to if iteration is finished, respectively this iterator was not
         * initialized.
         */
        size_t                      actDepth                             = static_cast<size_t >(-1);

        /** The path to the actual node (excluding the name of the actual node).
         *  If this object is \e nulled, no paths are generated. */
        AString                     actPath;

        /** The requested depth of iteration recursion. */
        unsigned int                recursionDepth      =(std::numeric_limits<unsigned int>::max)();

        /** A pointer to a user-defined comparison function for path names.  */
        bool                      (*nextCustomSorter)(const NodePtr&, const NodePtr&)     = nullptr;

        /** Denotes if the children are iterated in a sorting fashion or not. */
        bool                        nextIsSorting                                           = false;

        /** The sort order (used with built-in sorting by node name). */
        bool                        nextSortingIsDescending                                 = false;

        /** The case sensitivity of the sort (used with built-in sorting by node name). */
        bool                        nextSortingIsCaseSensitive                              = false;

        //##########################################################################################
        //  RecursiveIterator Constructor/Destructor
        //##########################################################################################
        public:
            /** Default constructor. */
            RecursiveIterator()                                                           = default;

            /** Trivial copy constructor. */
            RecursiveIterator( const RecursiveIterator&  )                                = default;

            /** Trivial default move constructor. */
            RecursiveIterator(       RecursiveIterator&& )                                = default;

            /** Trivial default copy assign operator.
             * @return A reference to \c this.           */
            RecursiveIterator& operator=( const RecursiveIterator&  )                     = default;

            /** Trivial default move assign operator.
             * @return A reference to \c this.           */
            RecursiveIterator& operator=(       RecursiveIterator&& )                     = default;


            /** Destructor. */
            ~RecursiveIterator()                                                          = default;

        //##########################################################################################
        //  Interface
        //##########################################################################################
            /**
             * With this method, the assembly of a string representing the path from the
             * node used to initialize this iterator to the actual node, is activated or
             * deactivated.<br>
             * If activated, the path to the current node can be received using overloaded
             * methods #CurrentPath and #FullPath.
             *
             * The invocation of the method invalidates this iterator. Note that the
             * path generation can be enabled and disabled with the optional parameter
             * of the overloaded #Initialize methods.
             *
             * @param pathGeneration Denotes whether the path should be generated or not.
             */
            void SetPathGeneration( Switch pathGeneration )
            {
                Invalidate();
                actPath.Reset( pathGeneration == Switch::On ? EmptyString()
                                                            : NullString()  );
            }

            /**
             * Resets this iterator to work with the given \b %StringTree.
             * Initializes recursive iteration to the tree's root node.
             * Optionally, a recursion depth can be set.
             *
             * @param pTree  The \b %StringTree to use.
             * @param depth  Sets the recursion depth.
             *               A depth of \c 0 iterates only the direct children of the root node.
             *               Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
             *               for "unlimited" recursion.
             */
            void Initialize( StringTree&   pTree,
                             unsigned int  depth= (std::numeric_limits<unsigned int>::max)() )
            {
                initialize( &pTree, &pTree.root, depth );
            }

            /**
             * Resets this iterator to the first child of the node that the given node pointer
             * object represents.
             * If the node pointer is invalid, the root node of the tree it represents is used.
             *
             * If the given node has no children, this iterator is marked invalid when this
             * method returns.
             *
             * Optional parameter \p{depth} allows to limit the recursion depth.
             *
             * @param nodePtr The node pointer to define the branch of the tree to iterate.
             * @param depth   Sets the recursion depth.
             *                A depth of \c 0 iterates only the direct children of the node
             *                represented by \p{nodePtr}.
             *                Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
             *                for "unlimited" recursion.
             */
            void Initialize( NodePtr       nodePtr,
                             unsigned int  depth= (std::numeric_limits<unsigned int>::max)()  )
            {
                initialize( static_cast<StringTree*>( nodePtr.tree ),
                            nodePtr.IsValid() ? nodePtr.node
                                              : &(nodePtr.tree->root),
                            depth                                                       );
            }

            /**
             * Resets this iterator to the first child of the node that the given other iterator
             * currently refers to. The given iterator has to be in a valid state.
             * Optionally, a recursion depth can be set.
             *
             * @param other  The iterator whose current node becomes the start node for this
             *               iterator.
             * @param depth  Sets the recursion depth.
             *               A depth of \c 0 iterates only the direct children of the node
             *               represented by \p{other}.
             *               Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
             *               for "unlimited" recursion.
             */
            void Initialize( const RecursiveIterator&   other,
                             unsigned int   depth= (std::numeric_limits<unsigned int>::max)() )
            {
                initialize( other.tree, other.node, depth );
            }

            /**
             * Invalidates this object. After invoking this method, this iterator can not be
             * used further, until one of the overloaded methods #Initialize is invoked.
             * After the invocation, method #IsValid will return \c false.
             */
            void Invalidate()
            {
                actDepth= static_cast<size_t >(-1);
            }

            /**
             * Determines if this is a valid. RecursiveIterator instances may become invalid
             * after invocations of one of the methods #Next, #NextSibling or #NextParentSibling
             * (at the end of the iteration) and become valid with the invocation of one of the
             * overloaded methods #Initialize.
             *
             * @return \c true if this is a valid iterator. If invalid, \c false is returned and
             *         the iterator must not be evaluated prior to being initialized.
             */
            bool IsValid()                                                                     const
            {
                return actDepth != static_cast<size_t >(-1);
            }

            /**
             * The negation of #IsValid.
             *
             * @return \c false if this is a valid iterator. If invalid, \c true is returned and
             *         the iterator must not be evaluated prior to being initialized.
             */
            bool IsInvalid()                                                                   const
            {
                return !IsValid();
            }


            /**
             * Allows to switch sorting on or off. If switched on, sorting is performed
             * by the node names in ascending order.
             *
             * This and the overloaded versions of this method may be invoked at any time,
             * even on invalid iterators and those that are not initialized.
             * All that the methods do is storing the given parameters for future use.
             * Such use happens whenever a recursive iteration over a list of child nodes is
             * started. In that moment the current configuration of sorting is applied to
             * the list of direct children.
             *
             * @param sorting  The switch value.
             */
            void SetSorting( Switch sorting )
            {
                if( sorting == Switch::Off )
                    nextIsSorting= false;
                else
                    SetSorting(SortOrder::Ascending, Case::Ignore );
            }

            /**
             * Sets the sorting of children by their path name, using the built-in comparison
             * methods, which in turn use method \alib{strings,TString::Equals,String::Equals}.
             *
             * \see
             *   For more details on sorting see method \ref SetSorting(Switch sorting).
             *
             *
             * @param order         The sort order.
             *                      Defaults to \b %SortOrder::Ascending.
             * @param sensitivity   The case sensitivity when comparing path names.
             *                      Defaults to \b %Case::Ignore.
             */
            void SetSorting( SortOrder order         = SortOrder::Ascending,
                             Case      sensitivity   = Case::Ignore            )
            {
                nextIsSorting             = true;
                nextCustomSorter          = nullptr;
                nextSortingIsDescending   =  ( order       == SortOrder::Descending );
                nextSortingIsCaseSensitive=  ( sensitivity ==      Case::Sensitive  );
            }

            /**
             * Sets the sorting of children by their template value, using the given
             * callback function.
             *
             * \see
             *   For more details on sorting see method \ref SetSorting(Switch sorting).
             *
             * @param customSorterFunction  A custom comparison method used for sorting the children
             *                      of nodes.
             */
            void SetSorting( bool (*customSorterFunction)(const NodePtr&, const NodePtr&) )
            {
                nextIsSorting     = true;
                nextCustomSorter  = customSorterFunction;
            }

            /**
             * Iterates to the first child of the current node. If no such child exists,
             * to the next sibling node. If also no sibling exists, iteration continues
             * with the next available node of a previous recursion level.
             *
             * @return \c true if a next node was found, \c false otherwise.
             *         If \c false is returned, this iterator is invalid after the call. */
            bool Next()
            {
                return next(0);
            }

            /**
             * Omits recursion on the current node's children, even if the current depth
             * is lower than #RequestedDepth.<br>
             *
             * If no sibling exists, iteration continues with the next available node of a
             * previous recursion level.
             *
             * @return \c true if a next node was found, \c false otherwise.
             *         If \c false is returned, this iterator is invalid after the call.
             */
            bool NextSibling()
            {
                return next(1);
            }

            /**
             * Skips the remaining siblings of the current recursion level and continues with
             * the next available sibling of a previous level.
             *
             * @return \c true if a next node was found, \c false otherwise.
             *         If \c false is returned, this iterator is invalid after the call.
             */
            bool NextParentSibling()
            {
                return next(2);
            }


            /**
             * Retrieves the current path of walking as a string representation.
             * The path returned is relative to the start node and does not contain a leading
             * separator character. Also, it does not contain the name of the current node,
             * which can be received by invoking method
             * \alib{monomem::StringTree::NodePtr,Name} on the node pointer returned by
             * method #Node.
             *
             * Note that this method can be used only if path generation was activated
             * prior to the current iteration. Activation is performed with method
             * #SetPathGeneration.
             *
             * @return The path of the current node.
             */
                const NameType&    CurrentPath()                                               const
            {
                ALIB_ASSERT_ERROR( actPath.IsNotNull(), "MONOMEM/STRINGTREE",
                                   "Path generation not activated" )
                return actPath;
            }

            /**
             * Writes the results of #CurrentPath and
             * \alib{monomem::StringTree::NodePtr,Name}, separated by the separator
             * character \p{TSeparator}.
             *
             * Note that this method can be used only if path generation was activated
             * prior to the current iteration. Activation is performed with method
             * #SetPathGeneration.
             *
             * @param target        The target to append the path to.
             * @param targetData    Denotes whether \p{target} should be cleared prior to
             *                      appending the path. Defaults to CurrentData::Clear.
             * @return The given string to allow concatenated operations
             */
            AString&    FullPath( AString&      target,
                                  CurrentData   targetData= CurrentData::Clear )               const
            {
                ALIB_ASSERT_ERROR( actPath.IsNotNull(), "MONOMEM/STRINGTREE",
                                   "Path generation not activated" )

                if( targetData == CurrentData::Clear )
                    target.Reset();

                if( actPath.IsNotEmpty() )
                    target << actPath  << tree->separator;

                return target << node->name;
            }


            /**
             * Returns the requested maximum depth of iteration, set with #Initialize.
             *
             * @see For the current iteration depth, use #CurrentDepth.
             *
             *
             * @return The distance of the current node and the node of the start of the
             *         iteration.
             */
            int RequestedDepth()                                                               const
            {
                return static_cast<int>( recursionDepth );
            }

            /**
             * Returns the depth of the current iteration. This is value is available to the
             * algorithm which means this method executes in constant time.
             *
             * To get the absolute depth of the current node, method
             * \alib{monomem,StringTree::NodePtr::Depth} may be used.
             *
             * @return The distance of the current node and the node of the start of the
             *         iteration.
             */
            int CurrentDepth()                                                                 const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE",
                                   "RecursiveIterator not initialized or exceeded (invalid)." )
                return static_cast<int>( actDepth );
            }

            /**
             * Returns the current node, encapsulated in a node pointer object.

             *
             * \note
             *   It is \b not allowed to use method
             *   \alib{monomem::StringTree,NodePtr::Delete} on the node returned by
             *   this method. As a replacement, use method #DeleteNode implemented in this
             *   class.<br>
             *   However, methods \alib{monomem::StringTree,NodePtr::DeleteChild}
             *   and \alib{monomem::StringTree,NodePtr::DeleteChildren} are allowed
             *   to be invoked and therefore have no replacement in this class.
             *
             * @return An instance of the public node interface pointing to the currently
             *         referenced tree node.
             */
            NodePtr Node()                                                                     const
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE",
                                   "RecursiveIterator not initialized or exceeded (invalid)." )
                return NodePtr( tree, node );
            }

        //####  node deletion (RecursiveIterator)  #############################################

            /**
             * Deletes the node that this iterator currently refers to from the tree.
             * After the operation, the iterator is moved forward to the next sibling
             * of the current node, respectively of the first sibling found in the
             * recursion stack.
             *
             * \note
             *   This method constitutes a legal alternative to method
             *   \alib{monomem::StringTree,NodePtr::Delete}, which is forbidden to be
             *   invoked on the node returned by method #Node as this would invalidate this
             *   iterator.<br>
             *
             *   Methods \alib{monomem::StringTree,NodePtr::DeleteChild}
             *   and \alib{monomem::StringTree,NodePtr::DeleteChildren} are allowed with this
             *   iterator type. Consequently, no replacement method for those is given with this
             *   class.
             *
             * @return The total number of nodes deleted.
             */
            uinteger DeleteNode()
            {
                ALIB_ASSERT_ERROR( IsValid(), "MONOMEM/STRINGTREE",
                                   "RecursiveIterator not initialized or exceeded (invalid)." )
                auto& nodeToDelete= *node;
                next( 1 ); // next sibling
                return nodeToDelete.parent->deleteChild( tree, &nodeToDelete );
            }

        //##########################################################################################
        // RecursiveIterator internal methods
        //##########################################################################################
        protected:
            /**
             * Resets this iterator to represent to the given node of the given tree.
             *
             * @param pTree   The tree to iterate on.
             * @param newnode The new node to start the iteration from.
             * @param depth   The requested recursion depth.
             */
            void initialize( StringTree* pTree, baseNode* newnode, unsigned int depth )
            {
                this->tree= pTree;
                if( actPath.IsNotNull() )
                    actPath.Reset();

                node= newnode;
                if( newnode->qtyChildren )
                {
                    recursionDepth= depth;
                    actDepth= static_cast<size_t >(-1);
                    recursion();
                }
                else
                    actDepth= static_cast<size_t>( -1 );
            }

            /**
             * Sets this iterator to point to the first child of the actual node.
             * If sorting is enabled, copies all children from the map to a vector and sorts
             * them there.
             */
            void  recursion()
            {
                ++actDepth;
                if( stack.size() == actDepth )
                    stack.emplace_back();

                auto& rd= stack[actDepth];
                rd.customSorter          = nextCustomSorter;
                rd.isSorting             = nextIsSorting;
                rd.sortingIsDescending   = nextSortingIsDescending;
                rd.sortingIsCaseSensitive= nextSortingIsCaseSensitive;

                // no sorting: set link to node's child hook
                if (!rd.isSorting)
                {
                    rd.childrenUnsorted= &node->children;
                    node= (rd.actChild.unsorted= rd.childrenUnsorted->first());
                    return;
                }

                // sorting: copy children to a sortable vector
                rd.childrenSorted.clear();
                rd.childrenSorted.reserve( static_cast<size_t>( node->qtyChildren ) );
                auto* copyIt= node->children.first();
                while( copyIt != &node->children.hook )
                {
                    rd.childrenSorted.emplace_back( copyIt );
                    copyIt= copyIt->next();
                }

                // sort
                if( rd.customSorter )
                {
                    std::sort( rd.childrenSorted.begin(), rd.childrenSorted.end(),
                               [this,&rd]( typename basetree::Node* lhs,
                                           typename basetree::Node* rhs  )
                               {
                                    return rd.customSorter( NodePtr(tree, lhs),
                                                            NodePtr(tree, rhs) );
                               }
                              );
                }
                else
                {
                    std::sort( rd.childrenSorted.begin(), rd.childrenSorted.end(),
                               [&rd]( baseNode* lhs, baseNode* rhs)
                               {
                                    int compResult=  rd.sortingIsCaseSensitive
                                       ? lhs->name.template CompareTo<true, Case::Sensitive>(rhs->name)
                                       : lhs->name.template CompareTo<true, Case::Ignore   >(rhs->name);
                                    return rd.sortingIsDescending ? compResult > 0
                                                                  : compResult < 0;
                               }
                              );
                }

                // set to first child
                rd.actChild.sorted= 0;
                node= rd.childrenSorted[0];
            }


            /**
             * Goes to the next node.
             * This method is used with the derived class's interface methods
             * \alib{monomem::StringTree::RecursiveIterator,Next},
             * \alib{monomem::StringTree::RecursiveIterator,NextSibling} and
             * \alib{monomem::StringTree::RecursiveIterator,NextParentSibling}, as well as with
             * \alib{monomem::StringTree::RecursiveIterator,DeleteNode}.
             *
             * @param skipMode   \c 0 iterates to the first child (if available),
             *                   \c 1 iterates to the next sibling (if available) and
             *                   \c 2 to the next available sibling of the parent, respectively the
             *                   current recursion stack.
             * @return \c true if this iterator is valid (a next node was found), \c false
             *         otherwise.                                                                 */
            bool next(int skipMode)
            {
                ALIB_ASSERT_ERROR( actDepth != static_cast<size_t >(-1), "MONOMEM/STRINGTREE",
                                   "Invalid iterator" )

                // recursion to first child of actual node?
                if( skipMode == 0
                    && static_cast<unsigned int>( actDepth ) < recursionDepth
                    && node->qtyChildren )
                {
                    if( actPath.IsNotNull() )
                    {
                        if( actPath.IsNotEmpty() )
                            actPath << tree->separator;
                        actPath << node->name;
                    }

                    // increase stack capacity
                    if( stack.size() == actDepth + 1 )
                        stack.emplace_back();

                    recursion();

                    return true;
                }

                for(;;)
                {
                    if( skipMode != 2 )
                    {
                        // next sibling
                        bool foundNextChild;
                        {
                            RecursionData& rd= stack[ actDepth ];
                            if( rd.isSorting )
                            {
                                ++rd.actChild.sorted;

                                if( (foundNextChild= rd.actChild.sorted < rd.childrenSorted.size())
                                    ==true )
                                    node= rd.childrenSorted[rd.actChild.sorted];
                            }
                            else
                            {
                                node      = (rd.actChild.unsorted= rd.actChild.unsorted->next());
                                foundNextChild= (node != &rd.childrenUnsorted->hook);
                            }
                        }

                        if( foundNextChild )
                            break;
                    }
                    skipMode= 0;

                    // climb down
                    if( actDepth > 0 )
                    {
                        --actDepth;

                        // remove separator from path
                        if( actPath.IsNotEmpty() )
                        {
                            character lastChar;
                            do
                            {
                                lastChar= actPath.CharAtEnd<false>();
                                actPath.DeleteEnd<false>( 1 );
                            }
                            while( lastChar != tree->separator && actPath.IsNotEmpty() );
                        }
                    }
                    else
                    {
                        actDepth=  static_cast<size_t >(-1);
                        ALIB_ASSERT( actPath.IsEmpty())
                        break;
                    }
                }

               return actDepth != static_cast<size_t >(-1);
            }

    };  // inner class "RecursiveIterator"


    // #############################################################################################
    // Constructor/destructor/Reset
    // #############################################################################################

    /** ********************************************************************************************
     * Constructor.
     * @tparam TArgs          Types of variadic parameters given with parameter \p{args}.
     * @param  allocator      The monotonic allocator to use.
     * @param  pathSeparator  The separation character used with path strings.
     * @param  args           Variadic parameters to be forwarded to constructor of custom type
     *                        \p{T} of this tree's root node.
     **********************************************************************************************/
    template<typename... TArgs>
    StringTree( MonoAllocator* allocator    ,
                CharacterType  pathSeparator, TArgs&&...       args        )
    : basetree( allocator, pathSeparator, std::forward<TArgs>( args)... )
    {}

    /** ********************************************************************************************
     * Constructor taking a shared recycler.
     * @tparam TArgs          Types of variadic parameters given with parameter \p{args}.
     * @param  allocator      The monotonic allocator to use.
     * @param  pRecycler      The shared recycler.
     * @param  pathSeparator  The separation character used with path strings.
     * @param  args           Variadic parameters to be forwarded to constructor of custom type
     *                        \p{T} of this tree's root node.
     **********************************************************************************************/
    template<typename... TArgs>
    StringTree( MonoAllocator* allocator    , TSharedRecycler& pRecycler,
                CharacterType  pathSeparator, TArgs&&...       args        )
    : basetree( allocator, pRecycler, pathSeparator, std::forward<TArgs>( args)... )
    {}


    /** ********************************************************************************************
     * Defaulted Destructor
     **********************************************************************************************/
    ~StringTree()
    {
        for( auto& node : basetree::nodeTable )
            TNodeMaintainer::FreeNode( *static_cast<basetree*>(this), node );
    }

    /** ********************************************************************************************
     * In some situations, the allocator object to use might not be available with construction
     * of an instance. This method may be used to attach an external allocator at a later
     * point in time - but prior to this instance's first use.
     *
     * @param pAllocator The allocator that was omitted in the constructor.
     **********************************************************************************************/
    void        SetAllocatorPostConstruction( MonoAllocator* pAllocator )
    {
        basetree::nodeTable.SetAllocatorPostConstruction( pAllocator );
    }


    /** ********************************************************************************************
     * Removes all elements from this container. The use of this method is more efficient than
     * deleting the children of the root node.
     *
     * Invokes \alib{monomem,HashTable::Clear} on field
     * \alib{monomem::detail::StringTreeBase,nodeTable}. As documented with that method, the
     * allocated nodes will be preserved for "recycling" with future insertions.
     *
     * The custom data of the root node is preserved.
     **********************************************************************************************/
    void        Clear()
    {
        for( auto& node : basetree::nodeTable )
            TNodeMaintainer::FreeNode( *static_cast<basetree*>(this), node );

        basetree::nodeTable.Clear();

        // re-initialize root node
        basetree::root.children.reset();
        basetree::root.qtyChildren= 0;
    }

    /** ********************************************************************************************
     * Clears all nodes and values. The use of this method is more efficient than deleting the
     * children of the root node.<br>
     * In addition, depending on template type \p{TNodeMaintainer}, it may also declare allocated
     * memory for future reuse.<br>
     * The latter is true for type \alib{monomem,StringTreeNamesMonoAlloc}.
     *
     * In case of \alib{monomem,Recycling::Private,private recycling} disposes all previously
     * allocated recyclable instances of internal element type.
     *
     * The custom data of the root node is deleted and default constructed using the given
     * arguments.
     * @tparam TArgs         Types of variadic parameters given with parameter \p{args}.
     * @param  args          Variadic parameters to be forwarded to constructor of custom type
     *                       \p{T} of this tree's root node.
     **********************************************************************************************/
    template<typename... TArgs>
    void        Reset( TArgs&&... args )
    {
        for( auto& node : basetree::nodeTable )
            TNodeMaintainer::FreeNode( *static_cast<basetree*>(this), node );

        basetree::nodeTable.Reset();

        // re-initialize root node
        basetree::root.children.reset();
        basetree::root.qtyChildren= 0;
        basetree::root.data.~T();
        new(&basetree::root.data)  T( std::forward<TArgs>( args)...);
    }

    /** ********************************************************************************************
     * Counts the number of currently allocated but unused (not contained) element nodes
     * that will be recycled with upcoming insertions.
     *
     * \note
     *   This method is provided for completeness and unit-testing. It should not be of
     *   relevance for common usage.
     *   Furthermore, this method is not available (aka does not compile) with instantiations
     *   that specify template parameter \p{TRecycling} as \alib{monomem,Recycling::None}.
     *
     * @return The number of removed and not yet recycled elements.
     **********************************************************************************************/
    integer     RecyclablesCount()                                                             const
    {
        return basetree::nodeTable.RecyclablesCount();
    }

    /** ********************************************************************************************
     * Returns the overall number of elements contained in this tree.
     *
     * \note
     *   This method performs in constant time.
     *
     * @return The number elements contained in this tree.
     **********************************************************************************************/
    integer     Size()                                                                         const
    {
        return basetree::nodeTable.Size();
    }

    /** ********************************************************************************************
     * Tests for emptyness.
     *
     * @return \c true if this tree is empty, \c false otherwise.
     **********************************************************************************************/
    bool        IsEmpty()                                                                      const
    {
        return basetree::nodeTable.Size() == 0;
    }

    /** ********************************************************************************************
     * Allocates the required space for the number of elements expected.
     *
     * \note
     *   If the definite number of stored elements that is never exceeded is known,
     *   a \alib{monomem,MonoAllocator::TakeSnapshot,snapshot} of the monotonic allocator could
     *   be taken after the invocation of this method and then used to reset the monotonic
     *   allocator with preserving the memory used by this container.
     *
     * @param expected  The expected number or increase of elements to be stored in the hash table.
     * @param reference Denotes whether \p{expected} is meant as an absolute size or an increase .
     **********************************************************************************************/
    void        ReserveRecyclables( integer expected, lib::ValueReference reference )
    {
        basetree::nodeTable.ReserveRecyclables( expected, reference );
    }

    /** ********************************************************************************************
     * Creates a node pointer instance representing the root node.
     * @return A node pointer pointing to the root node of this \b %StringTree.
     **********************************************************************************************/
    NodePtr     Root()
    {
        return NodePtr( this, &(basetree::root) );
    }

}; // StringTree



}}// namespace aworx::[lib::monomem]

/// Type alias in namespace #aworx.
template<typename TChar, integer TLocalCapacity= 32>
using  StringTreeNamesDynamic    = lib::monomem::StringTreeNamesDynamic<TChar, TLocalCapacity>;

/// Type alias in namespace #aworx.
template<typename TChar>
using  StringTreeNamesStatic     = lib::monomem::StringTreeNamesStatic<TChar>;

/// Type alias in namespace #aworx.
template<typename TChar>
using  StringTreeNamesMonoAlloc  = lib::monomem::StringTreeNamesMonoAlloc<TChar>;

/// Type alias in namespace #aworx.
template<typename T,
         typename TNodeMaintainer= StringTreeNamesDynamic<character>,
         typename TRecycling     = Recycling::Private               >
using  StringTree = lib::monomem::StringTree<T, TNodeMaintainer, TRecycling>;


} // namespace [aworx]


#endif // HPP_ALIB_MONOMEM_STRINGTREE

//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace containers {

#if ALIB_DEBUG
    /// Statistic variable increased by \alib{containers;StringTreeNamesDynamic} with every creation
    /// of a node. With process creation the variable is \c 0. A user may reset the variable to
    /// inspect percentages of name overflows during certain operations. The variable is not thread
    /// safe and used by any instance of class \b StringTree which uses node handler
    /// \b StringTreeNamesDynamic.
    /// @see  Sibling variable \alib{containers;DBG_STATS_STRINGTREE_NAME_OVERFLOWS}
    ALIB_DLL extern uinteger  DBG_STATS_STRINGTREE_NAMES;

    /// Statistic variable increased by \alib{containers;StringTreeNamesDynamic} with every creation
    /// of a node whose name exceeds the internal string buffer size.
    /// With process creation the variable is \c 0. A user may reset the variable to
    /// inspect percentages of name overflows during certain operations. The variable is not thread
    /// safe and used by any instance of class \b StringTree which uses node handler
    /// \b StringTreeNamesDynamic.
    /// @see  Sibling variable \alib{containers;DBG_STATS_STRINGTREE_NAME_OVERFLOWS}
    ALIB_DLL extern uinteger  DBG_STATS_STRINGTREE_NAME_OVERFLOWS;
#endif

/// This struct is the default type for template parameter
/// \ref alib_ns_containers_stringtree_referencedoc "TNodeHandler" of class
/// \alib{containers;StringTree}.
///
/// Besides defining the character type as given with template parameter \p{TChar}, the node name
/// string-type is exposed with #NameStringType. The string-type depends on the template parameter
/// \p{TLocalCapacity}:
/// - If this is \c 0, the type evaluates to a simple string with no internal storage.
/// - If this is greater than zero, the type evaluates to a \alib{strings;TLocalString;LocalString}
///   of given capacity.
///
/// This design allows allocating a fixed-size string buffer with each node, and only if a
/// node's name exceeds this capacity, a dynamic allocation for storing the node name is performed.
/// As a consequence, some overhead of wasted memory will occur, as this capacity is
/// allocated with every node, regardless of its name's length.
/// To investigate into the percentage of overflows to evaluate a reasonable value for template
/// parameter \p{TLocalCapacity}, simple global debug counters
/// \alib{containers;DBG_STATS_STRINGTREE_NAMES} and \alib{containers;DBG_STATS_STRINGTREE_NAME_OVERFLOWS}
/// can be used.
///
/// Method #InitializeNode is invoked after insertion of a new element (aka "node")
/// into the container and #FreeNode is invoked before the destruction of a node.
/// When #InitializeNode is invoked, the custom object of template type \p{T} (of the \b StringTree)
/// is already default constructed and the key of the node in union
/// (field \alib{containers;detail::StringTreeBase::NodeKey::name}) is set to what was provided
/// as a child name or path string. (In the latter case, it is set to a substring of the
/// given path.). If template parameter \p{TLocalCapacity} is greater than 0, the
/// method copies the key to field \e storage of the union (which is still accessible with the
/// base string-type of union-field \e key).
/// If \c 0 is given, the node name is replaced by a copy of the string which is dynamically
/// allocated.
///
/// ## Custom Implementations ##
/// A custom implementation has to provide all four entities that this type provides in a
/// compatible fashion.
///
/// The main purpose of the node handler types is to ensure that the name strings of inserted
/// nodes are duly allocated, copied and freed as needed:
/// When a new element is (or a whole path of new elements are) created, then the initial name
/// of the nodes are taken from the string passed to the corresponding interface method of class
/// \b StringTree (and inner types). The challenge is that these string's life-cycle might
/// be only short term. Therefore, right after the creation of an element, method #InitializeNode
/// is invoked, allowing to create a safe copy of the name.<br>
/// To free any allocated space, method #FreeNode is invoked.
///
/// Besides this, custom implementation may tweak the given node on their own discretion.
/// Especially a custom implementation may create and recycle other portions of the stored
/// objects, to establish \ref alib_contmono_intro_strictweak "weak monotonic allocation rules".
/// A sample of such more complex behavior is found with \alib type \alib{files;FTree}.
///
/// \see
///   Two other built-in implementations of this type to be used with \b StringTree instantiations
///   are provided with this \alibmod:
///   - \alib{containers;StringTreeNamesStatic}.
///   - \alib{containers;StringTreeNamesAlloc}.
/// \see
///   Further information can be found in chapter
///   \ref alib_ns_containers_stringtree_alloc_names "4. Node and Node Name String Allocation"
///   of the reference documentation of class \b StringTree.
///
/// @tparam TChar           The character type of the key strings. This type is used with any
///                         interface method of \alib{containers;StringTree} that accepts a node name
///                         or path string.<br>
///                         Defaults to type \alib{characters;character}.
/// @tparam TLocalCapacity  The capacity of the \alib{strings;TLocalString;LocalString} to place in
///                         the <b>StringTree</b>'s node. If \c 0 is given, a normal
///                         \alib{strings;TString;String} is used for the name, and the buffer is
///                         copied to an dynamically allocated array.<br>
///                         Defaults to \c 32.
template<typename TChar= character, integer TLocalCapacity= 32>
struct StringTreeNamesDynamic
{
    /// The character type that the \b StringTree uses for child name and path strings.
    using CharacterType  = TChar;

    /// The string-type of a node's name.
    using NameStringType = std::conditional_t<( TLocalCapacity > 0),
                                                strings::TLocalString<TChar, TLocalCapacity>,
                                                strings::TString     <TChar>                   >;


    /// This implementation copies the node's name to a dynamically allocated piece of heap memory.
    /// \see
    ///   See class description for further information.
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        (void) tree;

        // if not a local string buffer, then dynamically allocate and copy.
        if constexpr (TLocalCapacity <= 0)
        {
            CharacterType* buffer= new CharacterType[size_t(node.name.key.Length())];
            node.name.key.CopyTo( buffer );
            node.name.key= strings::TString<CharacterType>( buffer, node.name.key.Length() );
        }
        else
        {
            // create a local string which may allocate heap if name is too long
            strings::TString<TChar> key= node.name.key;                   // get current pointer
            new (&node.name.storage) NameStringType();                    // placement-new to re-establish local string
            node.name.storage.DbgDisableBufferReplacementWarning();       // Disable warnings
            ALIB_DBG( const TChar* internalBuffer= node.name.storage.Buffer(); ) // get internal local buffer
            node.name.storage.Append(key);                                       // copy key to buf
            ALIB_DBG( ++DBG_STATS_STRINGTREE_NAMES;
                      if( internalBuffer != node.name.storage.Buffer() )         // ++statistics if local buffer was too small
                        ++DBG_STATS_STRINGTREE_NAME_OVERFLOWS; )
        }
    }

    /// This implementation frees the dynamically allocated memory of the node's name.
    /// \see
    ///   See class description for further information.
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::Node& node )
    {
        (void) tree;
        if constexpr (TLocalCapacity <= 0)
            delete[] node.name.key.Buffer();
        else
            node.name.storage.~TLocalString();
    }

}; // StringTreeNamesDynamic

/// Built-in implementation usable as template parameter
/// \ref alib_ns_containers_stringtree_referencedoc "TNodeHandler" of class
/// \alib{containers;StringTree}.
///
/// This type does not allocate memory and does not copy the key string of a node. Therefore, this
/// type is very efficient to use in situations where exclusively "static" strings for child names
/// and paths are passed to the interface methods of class \b StringTree (and inner types) which lead
/// to the creation of new child nodes.<br>
/// The term "static" here means that the strings given are either static character data of a
/// compilation unit or by any other means their allocated memory and the contained data survive
/// the life-cycle of the corresponding \b StringTree.
///
/// \see
///   Two other built-in implementations of this type to be used with \b StringTree instantiations
///   are provided with this \alibmod:
///   - \alib{containers;StringTreeNamesDynamic}.
///   - \alib{containers;StringTreeNamesAlloc}.
/// \see
///   Further information can be found in chapter
///   \ref alib_ns_containers_stringtree_alloc_names "4. Node and Node Name String Allocation"
///   of the reference documentation of class \b StringTree.
///
/// @tparam TChar  The character type of the key strings. This type is used with any
///                interface method of \alib{containers;StringTree} that accepts a node name or path
///                string.
template<typename TChar= character>
struct StringTreeNamesStatic
{
    /// The character type that the \b StringTree uses for child name and path strings.
    using CharacterType  = TChar;

    /// The string-type of a node's name.
    using NameStringType = strings::TString<TChar>;

    /// This implementation is empty.
    ///
    /// \see
    ///   See description of this class and the "default implementation"
    ///   \alib{containers;StringTreeNamesDynamic}.
    ///
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        (void) tree;
        (void) node;
    }

    /// This implementation is empty.
    ///
    /// \see
    ///   See description of this class and the "default implementation"
    ///   \alib{containers;StringTreeNamesDynamic}.
    ///
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::Node& node )
    { (void) tree; (void) node; }
}; // StringTreeNamesStatic

/// Built-in implementation usable as template parameter
/// \ref alib_ns_containers_stringtree_referencedoc "TNodeHandler" of class
/// \alib{containers;StringTree}.
///
/// This type copies the node's name into memory acquired with the monotonic allocator that the
/// \b StringTree uses.<br>
///
/// \attention
///   The use of this type is dangerous in respect to memory exhaustion. While class
///   \b StringTree uses monotonic allocation in a
///   \ref alib_ns_containers_stringtree_hashing "very safe way", with the use of this
///   type, repeated removals and insertions of tree nodes, increase the memory usage.<br>
///   Consequently, the use of this type is restricted to cases that imply a limited
///   number of insertions.
///
/// \see
///   Two other built-in implementations of this type to be used with \b StringTree instantiations
///   are provided with this \alibmod:
///   - \alib{containers;StringTreeNamesStatic}.
///   - \alib{containers;StringTreeNamesDynamic}.
/// \see
///   Further information can be found in chapter
///   \ref alib_ns_containers_stringtree_alloc_names "4. Node and Node Name String Allocation"
///   of the reference documentation of class \b StringTree.
///
/// @tparam TChar  The character type of the key strings. This type is used with any
///                interface method of \alib{containers;StringTree} that accepts a node name or path
///                string.
template<typename TChar= character>
struct StringTreeNamesAlloc
{
    /// The character type that the \b StringTree uses for child name and path strings.
    using CharacterType= TChar;

    /// The string-type of a node's name.
    using NameStringType = strings::TString<TChar>;

    /// This implementation copies the node's name to a piece of memory allocated in the
    /// allocator found in field \alib{containers::detail::StringTreeBase;nodeTable}
    /// of the given \p{tree}.
    ///
    /// \see
    ///   See description of this class and the "default implementation"
    ///   \alib{containers;StringTreeNamesDynamic}.
    ///
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that was just created. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void InitializeNode( TTree& tree, typename TTree::Node&  node )
    {
        node.name.storage.Allocate( tree.nodeTable.GetAllocator(), node.name.key );
    }

    /// This implementation does nothing.
    ///
    /// \see
    ///   See description of this class and the "default implementation"
    ///   \alib{containers;StringTreeNamesDynamic}.
    ///
    /// @param  tree  The instance of struct \alib{containers;detail::StringTreeBase} that invokes
    ///               this method. Any member may be accessed, including
    ///               \alib{containers::detail::StringTreeBase;nodeTable} which contains the
    ///               allocator that the tree uses for the allocation of nodes.
    /// @param  node  The node that is to be removed. Allows access to the key and
    ///               custom value data. While the parent and sibling nodes are likewise accessible,
    ///               it is strictly forbidden to modify those.
    /// @tparam TTree The type of the templated instantiation of struct
    ///               \alib{containers;detail::StringTreeBase} that this method is invoked by.
    ///               (Deduced by the compiler.)
    template<typename TTree>
    static
    void FreeNode( TTree& tree, typename TTree::baseNode& node )       { (void) tree; (void) node; }
};

//==================================================================================================
/// # 1. Introduction # {#alib_ns_containers_stringtree_overview}
/// This container type implements a directed, non-circular graph (tree) with named nodes.
///
/// The internal node type \alib{containers;detail::StringTreeBase::Node} stores:
/// 1. A name string, which has to be unique in respect to the names of sibling nodes. (Just like
///    no two files in a folder may have the same name.)
/// 2. Five pointers to related nodes:
///   - the parent node
///   - the previous and next sibling nodes,
///   - the first and last child nodes,
/// 3. A data field holding the node's custom value of template type \p{T}.
///
/// The way from the root node to a descendent node, usually is called "path". The class incorporates
/// functionality to work with string representations of such paths where names of child nodes are
/// concatenated and separated by a special separation character.
///
/// The search and creation of tree nodes by using aforementioned path strings, is very similar to
/// what is well known from addressing files and folders in file systems.
/// This class does not differentiate between 'folders' and 'files', hence between 'nodes' and
/// 'leafs'. Every node has the same data of type \p{T} attached and may or may not have child nodes.
/// If such differentiation - or other semantics - is wanted, this may well be modeled by custom
/// attributes provided in template type \p{T}.
///
/// \I{#############################################################################################}
/// # 2. Inner Types # {#alib_ns_containers_stringtree_inner}
/// Two public inner types exist.
/// All operations on tree nodes like insertion, deletion, search and attribute access is performed
/// using objects of public type \alib{containers;StringTree::TCursor}. This is a lightweight,
/// iterator-like "handle" containing a pointer to the originating tree object and to a represented
/// node. The type provides various methods to travers the tree. It is templated over a boolean
/// value which determines if a const or mutable \b StringTree is given. Shortcuts for these
/// types are \alib{containers;StringTree::Cursor} and \alib{containers;StringTree::ConstCursor}.
///
/// Besides this, class \alib{containers;StringTree::RecursiveIterator} allows recursive
/// iterations with built-in or custom sort orders.
///
/// Both types are explained in the following paragraphs.
///
/// \I{#############################################################################################}
/// ## 2.1 Inner Class Cursor ## {#alib_ns_containers_stringtree_cursor}
///
/// The main interface into class \b %StringTree is given by public, inner type
/// \alib{containers;StringTree::Cursor}. Method #Root returns an object of that type that
/// initially refers to the root node of the tree.
/// With this, child names and composite "paths" can be used to move the pointer along existing nodes
/// of the tree or to create new child nodes or even a whole path of such child nodes at once.
///
/// Class \b %Cursor is very lightweight as it contains just two pointers, one to the
/// \b %StringTree it originates from and one to the tree node currently represented.
/// Hence, objects of this type can be copied, assigned and passed around very efficiently.<br>
/// The currently represented node's templated custom data can be accessed with method
/// #TCursor::Value.
///
/// The methods to traverse over the nodes of the tree are:
/// - #TCursor::GoToRoot
/// - #TCursor::GoToParent
/// - #TCursor::GoTo
/// - #TCursor::GoToNextSibling
/// - #TCursor::GoToPreviousSibling
/// - #TCursor::GoToChild
/// - #TCursor::GoToFirstChild
/// - #TCursor::GoToLastChild
///
/// With these methods, class \b StringTree::Cursor constitutes a sort of iterator idiom
/// idiom. For example to traverse all entries in the root folder, the following schematic would
/// be used:
///
///          myCursor=  myTree.GetRoot()
///          myCursor.GotoFirstChild()
///          While( myCursor.IsValid() )
///              DoSomething( myCursor.Value() )
///              myCursor.GotoNextSibling
///
/// For some of these methods an alternative version exists, which returns a corresponding copy
/// of the cursor, while leaving the original object unchanged. These methods share
/// the same name excluding the prefix <b>GoTo</b>.
///
/// For the creation of new child nodes or a complete path of such, methods
/// - #TCursor::GoToCreateChildIfNotExistent and ,
/// - #TCursor::GoToCreatedPathIfNotExistent
/// are provided.
///
/// Next, four methods that perform node deletion exist:
/// - #TCursor::DeleteChild (two overloaded versions),
/// - #TCursor::DeleteChildren and
/// - #TCursor::Delete
///
/// The already mentioned methods:
/// - #TCursor::GoToParent,
/// - #TCursor::GoToFirstChild,
/// - #TCursor::GoToLastChild,
/// - #TCursor::GoToNextSibling and
/// - #TCursor::GoToPreviousSibling
///
/// of class \b Cursor can be used to iterate from a node upward to the root node or through the
/// list of children of a node. Each method may \e invalidate the object in the case that no
/// corresponding parent or sibling node exists.
/// Invalid cursor objects can be (or rather have to be!) detected using method
/// #TCursor::IsValid.
/// Most of the class's methods must not be invoked on an invalidated object. Doing so is undefined
/// behavior and \ref alib_mod_assert "raises an ALib error" in debug-builds.
/// Invalid \b %Cursor objects can reset to a valid state using methods
/// - #TCursor::GoToRoot and
/// - #TCursor::GoTo
///
/// if absolute path addressing is used.
///
/// Instances that have been default-constructed may only be set to a valid state by (copy-)
/// assigning a valid instance.
///
/// \I{#############################################################################################}
/// ## 2.2. Inner Class RecursiveIterator ## {#alib_ns_containers_stringtree_iterator}
/// Class \alib{containers;StringTree::RecursiveIterator} provides a configurable and controlled
/// way of iterating a branch of a tree. Some features of the class are:
/// - Iterators can be initialized to start from any node of the tree
///   Iteration ends when all (recursive) child nodes of the start node have been visited.
/// - The iteration follows a "depth first search" approach: Before visiting a sibling node,
///   all children of a node are visited. However, it is not differentiated whether a sibling
///   has child nodes or not. In case all siblings with children are to be processed first,
///   a custom sorter has to be created which prefers those nodes that have children.
/// - The recursion depth can be limited, including to depth \c 0, which iterates only the
///   direct child nodes of the start node.
/// - Before entering a new depth-level during iteration, different sort orders can be set.
///   The choices are:
///   - No sorting (iterates in order of node insertion).
///   - Built-in sorting by node (path) name, ascending/descending, case-sensitive or ignoring case
///   - user-defined by path name, number of children or any other attribute of the node, of course
///     including a node's custom data values.
///
/// Class \b RecursiveIterator is of rather heavy weight and sorted iteration needs to allocate
/// memory for sorting the child nodes for each depth level of a potential recursion.
/// Therefore , it is recommended to reuse instances of the class with subsequent, similar iterations.
/// In addition this explains why this class does not follow the concept of
/// <c>std::iterator_traits</c>, which is designed to be used with lightweight iterator types.
///
/// \I{#############################################################################################}
/// # 3. Node Allocation And Hashing # {#alib_ns_containers_stringtree_hashing}
/// While each node maintains a doubly linked list of child nodes for iteration, this class stores
/// each inserted element in a \alib{containers;HashTable} using the parent node and the
/// element's name as a unique key.
/// This is done to be able to search for a child with a given name in constant time.
/// This container does not perform any other memory allocations than those that this
/// \b HashTable does.
///
/// With that, the implementation of this container type is able to leave all allocation and
/// <em>"recycling"</em> of node elements to this hash table, which is found in base class's field
/// \alib{containers;detail::StringTreeBase::nodeTable}. As explained in the documentation of
/// \alib{containers;HashTable}, its allocation mechanism can be made safe in respect to
/// memory exhaustion. This is true even if a \alib{MonoAllocator} is used and a virtually
/// unlimited number of insertions and removals of elements is performed.
/// Such safeness depends on template parameter \p{TRecycling} which is just passed to the
/// internal hash table's definition of nodes.
/// \see
///   For more information on this topic see also chapter \ref alib_contmono_intro_recycling
///   of this \alibmod_nl Programmer's Manual.
///
/// \I{#############################################################################################}
/// # 4. Node and Node Name String Allocation # {#alib_ns_containers_stringtree_alloc_names}
///
/// The C++ version of this class allows user-defined allocation (and copying) of the node's name
/// character strings. For this, a template parameter \p{TNodeHandler} is defined,
/// which defaults to built-in struct \alib{containers;StringTreeNamesDynamic}.
/// This default "node handler" defines the name type of nodes to \alib{strings;TLocalString;LocalString}
/// with a default capacity of \c 32 characters.
/// This way, this local string performs dynamic allocation automatically when a node is
/// constructed.<br>
/// In the case that many "long" node names are expected, it might be efficient to set the capacity
/// to \c 0. In this case, type \b StringTreeNamesDynamic defines the string-type to a normal
/// \b String and uses C++ keywords <c>new</c> and <c>delete[]</c> to allocate
/// and free character buffers for the name string.
///
/// A second built-in and ready-to-use "node handler" type is given with
/// \alib{containers;StringTreeNamesStatic}. This uses an unbuffered \b String and has empty
/// implementations of its static methods. This way no copies of the original string buffers that
/// are passed to the to interface methods of class \b Cursor that create children are made.
/// This is useful (and very efficient!) if \b all child name and creation path strings provided
/// of class \b %Cursor are permanently valid, or, in other words, their life-cycle spans over
/// the life-cycle of the nodes in a tree. Then, the names do not need to be copied to dedicated
/// allocated memory.
///
/// Finally, string trees that during their lifetime just grow and where no nodes (or only a limited
/// number of nodes) are removed until the whole tree is disposed may be instantiated using
/// the third built-in type \alib{containers;StringTreeNamesAlloc}.
/// With that, the concept of \ref alib_contmono_intro "monotonic allocation" that this container
/// type might use can be extended to the node name strings.
/// Type \b StringTreeNamesAlloc grabs the allocator from the tree provided with method
/// \alib{containers::StringTreeNamesAlloc;InitializeNode} and just clones the given
/// string into this.
///
/// \I{#############################################################################################}
/// # 5. Equipping the Root Node with Values # {#alib_ns_containers_stringtree_rootnodevalues}
/// It depends on the field of application, whether the root node should dispose over an instance
/// of custom type \p{T} or not.
/// For example a tree of depth \c 1, which could be implemented using type
/// <c>std::vector<T></c>, no stored type \p{T} can be be attached to the vector object itself, only
/// to its "children".<br>
/// Nevertheless, in many use cases, the root node naturally contains the same data as any other
/// node in the tree. Therefore, if this class would not support root node data, using
/// code would for example have to check a \alib{containers::StringTree;TCursor;Cursor} for pointing
/// to the root node and in this case get the custom data from somewhere else.<br>
/// On the other hand, if this class would "insist" in the provision of root node values, then already
/// with construction of the tree, arguments for the construction of the associated \p{T} object
/// would have to be provided - even if the root node value was never used.
/// The provision of initial "dummy" root data, is sometimes not even (easily) possible, and would
/// sometimes add the need to adjust the custom stored type \p{T} to fit into this class.
/// (In fact, with former versions of \alib, root-node initialization data was mandatory to provide
/// already with the construction of the tree.)
///
/// Therefore, this class makes the use of root node values optional. After construction of a
/// \b StringTree, methods \alib{containers::StringTree;ConstructRootValue} and
/// methods \alib{containers::StringTree;DestructRootValue} may be used to initialize and
/// destruct the optional root nodes' data.
///
/// To prevent memory leaks, in debug-compilations, the following \alib assertions and warnings are
/// raised:
/// - \alib{containers::StringTree;TCursor::Value;Cursor::Value} will raise an assertion if
///   called on the root node without having set a value.
/// - \alib{containers::StringTree;ConstructRootValue} will raise a warning if called twice
///   without a prior call to \alib{containers::StringTree;DestructRootValue}.
/// - \alib{containers::StringTree;DestructRootValue} will raise a warning if called without
///   a prior call to \alib{containers::StringTree;ConstructRootValue}.
/// - The destructor of this class will raise a warning if a root value was set but not deleted.
///
/// \I{#############################################################################################}
/// # Reference Documentation # {#alib_ns_containers_stringtree_referencedoc}
///
/// @tparam TAllocator   The allocator type to use, as prototyped with \alib{lang;Allocator}.
/// @tparam T            The custom type of elements stored in this container.
/// @tparam TNodeHandler
///         A template type that needs to provide an interface as documented with
///         \alib{containers;StringTreeNamesDynamic}, which is also the default type
///         if not specified. For details see
///         \ref alib_ns_containers_stringtree_alloc_names "corresponding section" of this
///         class's documentation.
///         The type is published as  \alib{containers;StringTree::HandlerType}.
/// @tparam TRecycling Denotes the type of recycling that is to be performed.
///         Possible values are
///         \alib{containers;Recycling;None},
///         \alib{containers;Recycling;Private} (the default), or
///         \alib{containers;Recycling;Shared}.
//==================================================================================================
template<typename TAllocator,
         typename T,
         typename TNodeHandler= StringTreeNamesDynamic<character>,
         Recycling     TRecycling  = Recycling::Private>
class StringTree : protected detail::StringTreeBase<TAllocator,T,TNodeHandler,TRecycling>
{
  #if !DOXYGEN
  protected:
        friend class Cursor;

        friend class ConstCursor;

        friend TNodeHandler;

        using basetree         = detail::StringTreeBase<TAllocator,T,TNodeHandler,TRecycling>;
        using baseNodeKey      = typename basetree::NodeKey;
        using baseNodeBase     = typename basetree::NodeBase;
        using baseNode         = typename basetree::Node;
        using baseCursor       = typename basetree::CursorBase;
        using baseConstCursor  = typename basetree::ConstCursorBase;
  #endif

  public:
    /// Type definition publishing template parameter  \p{TAllocator}.
    using AllocatorType     = TAllocator;

    /// The character type of node names and paths strings. This is defined using character type
    /// definition \alib{containers::StringTreeNamesDynamic;CharacterType} of template
    /// type \p{TNodeHandler}.
    using CharacterType     = typename TNodeHandler::CharacterType;

    /// The string-type of node names and paths. This is defined using character type definition
    /// \alib{containers::StringTreeNamesDynamic;CharacterType} of template type
    /// \p{TNodeHandler}.
    using NameType          = strings::TString<CharacterType>;

    /// The substring-type of paths. This is defined using character type definition
    /// \alib{containers::StringTreeNamesDynamic;CharacterType} of template type
    /// \p{TNodeHandler}.
    using SubstringType     = typename strings::TSubstring<CharacterType>;

    /// Type definition publishing template parameter \p{TNodeHandler}.
    using HandlerType       = TNodeHandler;

    /// This type definition may be used to define an externally managed shared recycler,
    /// which can be passed to the alternative constructor of this class when template
    /// parameter \p{TRecycling} equals \alib{containers;Recycling;Shared}.
    /// \see
    ///   Chapter \ref alib_contmono_containers_recycling_shared of the Programmer's Manual
    ///   for this \alibmod.
    using SharedRecyclerType=  typename basetree::SharedRecyclerType;


    /// A handle type used with methods #TCursor::Export and #ImportCursor.
    struct CursorHandle
    {
        uinteger value; ///< The encapsulated value.

        /// Comparison operator.
        /// @param other The cursor handle to compare this handle to.
        /// @return \c true if both handles refer to the same cursor or if both handles are
        ///         invalid. \c false otherwise.
        bool operator==(const CursorHandle& other) const noexcept { return value==other.value; }

        /// Checks if this is a valid handle.
        /// @return \c true if this handle is not nulled.
        bool IsValid()                                     const noexcept { return value != 0; }
    };

    /// A handle type used with methods #TCursor::Export and #ImportCursor.
    struct ConstCursorHandle
    {
        uinteger value; ///< The encapsulated value.

        /// Comparison operator.
        /// @param other The cursor handle to compare this handle to.
        /// @return \c true if both handles refer to the same cursor or if both handles are
        ///         invalid. \c false otherwise.
        bool operator==(const ConstCursorHandle& other)                           const noexcept
                                                                  { return value==other.value; }

        /// Comparison operator.
        /// @param other The cursor handle to compare this handle to.
        /// @return \c true if both handles refer to the same cursor or if both handles are
        ///         invalid. \c false otherwise.
        bool operator==(const CursorHandle& other)                                const noexcept
                                                                  { return value==other.value; }

        /// Checks if this is a valid handle.
        /// @return \c true if this handle is not nulled.
        bool IsValid()                                     const noexcept { return value != 0; }
    };

    //==========================================================================================
    /// This public, inner class provides the main interface into outer class \b StringTree.
    /// The class should be considered being similar to a simple pointer or to a lightweight
    /// iterator type, which refers to a tree and a current node.<br>
    /// The class's interface allows the access to a node's name and value and to insert and
    /// remove child nodes.
    ///
    /// Instances of this class can be received with methods #Root and #RecursiveIterator::Node.
    ///
    /// The default constructor creates an invalid object, which has to be initialized by
    /// assigning a valid object before its first use.
    ///
    /// \see
    ///   For more information on how this class is used, see paragraph
    ///   \ref alib_ns_containers_stringtree_cursor "2.1 Inner Class Cursor"
    ///   of the description of class \b %StringTree.
    ///
    /// @tparam TConst  If true, internal fields representing the StringTree and the current Node
    ///                 become \c const and methods which are not declared \c const become
    ///                 unavailable.
    ///
    /// ## Friends ##
    /// class \alib{containers;StringTree}
    //==========================================================================================
    template<bool TConst>
    class TCursor : protected basetree::template TCursorBase<TConst>
    {
        #if !DOXYGEN
            friend class StringTree;
        #endif

        #if !DOXYGEN
            #if ALIB_DEBUG_CRITICAL_SECTIONS
                #define  DCS       ALIB_DCS_WITH(cmCursor::tree->nodeTable.dcs)
                #define  DCSSHRD   ALIB_DCS_SHARED_WITH(cmCursor::tree->nodeTable.dcs)
            #else
                #define  DCS
                #define  DCSSHRD
            #endif
        #endif

      protected:
        /// Constant or mutable version of the base tree type, depending on template parameter
        /// \p{TConst}
        using cmTree    = std::conditional_t<!TConst, basetree, const basetree>;

        /// Constant or mutable version of the base node type, depending on template parameter
        /// \p{TConst}
        using cmNode    = std::conditional_t<!TConst, baseNode, const baseNode>;

        /// Constant or mutable version of the base cursor type, depending on template parameter
        /// \p{TConst}
        using cmCursor  = std::conditional_t<!TConst, baseCursor, baseConstCursor>;

        //####  Protected methods (class Cursor)  ##############################################

        /// Internal constructor
        /// @param pTree  The \b %StringTree we work on.
        /// @param pNode  The node to refer to.
        TCursor(cmTree *pTree, cmNode *pNode) noexcept
        : basetree::template TCursorBase<TConst>(pTree, pNode) {}

        /// Checks if this cursor is associated with a tree.
        /// Empty and optimized out with release-builds.
        void dbgCheckTree()                                                                const
        {
            ALIB_ASSERT_ERROR(cmCursor::tree != nullptr, "STRINGTREE",
              "Invalid StringTree::Cursor: No binding with a StringTree. "
              "(Probably default-constructed.)")
        }

        /// Checks if this cursor is associated with a tree and a valid node of the tree.
        /// Empty and optimized out with release-builds.
        void dbgCheckTreeAndNode()                                                         const
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( cmCursor::node != nullptr, "STRINGTREE",
             "Invalid StringTree::Cursor not representing a node of the assigned tree." )
        }

      //##########  Constructor, comparison operators, etc     #####################################
      public:
        /// Constant or mutable version of the outer string tree type, depending on template
        /// parameter \p{TConst}
        using TStringTree  = std::conditional_t<!TConst, StringTree, const StringTree>;


        /// Public constructor. Creates an invalid cursor.
        /// The only way to make a default-constructed instance valid is by
        /// (copy-) assigning another instance.
        TCursor() noexcept                                                            = default;

        /// Copy constructor.
        /// @param src The cursor to copy.
        TCursor( const TCursor& src)                                                    noexcept
        : TCursor{ src.tree, src.node }
        {}

        /// Move constructor.
        /// @param src The cursor to move.
        TCursor( TCursor&& src)                                                         noexcept
        : TCursor{ src.tree, src.node }
        {}

        /// Trivial default copy assign operator.
         /// @return A reference to \c this.
        TCursor &operator=(const TCursor &)                                 noexcept  = default;

        /// Trivial default move assign operator.
         /// @return A reference to \c this.
        TCursor &operator=(TCursor &&)                                      noexcept  = default;

        /// Trivial default destructor.
        ~TCursor()                                                          noexcept  = default;

        /// Conversion operator from <em>TCursor<TConst></em> to <em>TCursor<!TConst></em>.
        /// For const to mutable, this will fail as intended.
        /// @return A constant iterator, if this is a mutable. Otherwise compilation will
        ///         duly fail.
        operator TCursor<!TConst>() { return TCursor<!TConst>(cmCursor::tree, cmCursor::node); }

        /// Comparison operator.
        /// @param other  The object to compare ourselves to.
        /// @return \c    true if this and given cursor are equal, \c false
        ///               otherwise.
        bool operator==(const TCursor &other) const
        {
            return    cmCursor::node == other.node
                   && cmCursor::tree == other.tree;
        }

        /// Comparison operator.
        /// @param other  The object to compare ourselves to.
        /// @return \c    false if this and given cursor are equal, \c true
        ///               otherwise.
        bool operator!=(const TCursor &other) const
        {
            return !((*this) == other);
        }

        /// This method exports the address of the node in the \b StringTree.
        /// The second pointer needed to comprise a cursor determines the tree a node belongs to.
        /// Sometimes, it is necessary to store and restore a cursor, where the corresponding
        /// tree is known. With this method, in combination with method StringTree::ImportCursor,
        /// such storage with <c>sizeof(void*)</c>(instead of twice that size).
        ///
        /// \attention In fact this method and the corresponding constructor perform pointer
        ///            operations and keyword <c>reinterpret_cast</c>. Use with care.
        /// @return A value usable to reconstruct this cursor with the method
        ///         #StringTree::ImportCursor.
        CursorHandle        Export()         { return CursorHandle {uinteger(cmCursor::node)}; }

        /// Overloaded \c const version that returns a \c const handle, usable likewise only
        /// to re-construct a \c const cursor instance.
        ///
        /// @return A value usable to reconstruct this cursor with method
        ///         #StringTree::ImportCursor.
        ConstCursorHandle   Export() const{return ConstCursorHandle {uinteger(cmCursor::node)};}

        /// Determines if this is a valid object. Cursors may become invalid with
        /// transition methods like #GoToParent, #GoToFirstChild or GoToNextSibling.
        /// An invalid object may be turned into a valid one by either
        /// - assigning a valid object (copy assignment), or
        /// - invoking method #GoToRoot, or
        /// - invoking method #GoTo using absolute path addressing.
        ///
        /// Note that the latter is not applicable to a default-constructed objects
        /// (which are also invalid) as with such no \b %StringTree is assigned.
        ///
        /// @return \c true if this is a valid cursor.
        ///          If invalid, \c false is returned and this object must not be used.
        bool IsValid()                                                                     const
        {
            return cmCursor::node != nullptr;
        }

        /// Returns the opposite of #IsValid.
        ///
        /// @return \c true if this is an invalid cursor that must not be used,
        ///         \c false otherwise.
        bool IsInvalid()                                                                   const
        {
            return !IsValid();
        }

        //####  Tree navigation inplace, returning status      #########################
        /// Returns a cursor to the root node of the tree.
        /// @return A cursor representing the root node of the tree this pointer
        ///         represents.
        TCursor Root()                                                                     const
        {
            dbgCheckTree();
            return TCursor(cmCursor::tree, &cmCursor::tree->root.root);
        }

        /// Moves this cursor to the root node of the tree.
        /// @return A reference to this object
        TCursor& GoToRoot()
        {
            dbgCheckTree();
            cmCursor::node = &cmCursor::tree->root.root;
            return *this;
        }

        /// Creates a cursor value representing the parent node of the
        /// node represented by this object.
        ///
        /// If this object represents the root node of the tree, the returned cursor
        /// is invalid.
        ///
        /// @return A cursor pointing to the parent node of the node represented
        ///         by this cursor.
        TCursor Parent() const
        {
            dbgCheckTreeAndNode();
            return TCursor(cmCursor::tree, static_cast<cmNode*>(cmCursor::node->parent));
        }

        /// Moves this cursor to the parent of the current node.
        /// If this is the root node, this object becomes invalid.
        ///
        /// @return *this to allow concatenated calls
        TCursor& GoToParent()
        {DCSSHRD
            dbgCheckTreeAndNode();
            cmCursor::node = static_cast<cmNode*>(cmCursor::node->parent);
            return (*this);
        }

        /// Returns a cursor value that represents the next sibling of the node
        /// represented this cursor.
        /// If the node has no next sibling, an invalid cursor is returned.
        ///
        /// @return A cursor object representing the next sibling of the node
        ///         represented by this object.
        TCursor NextSibling() const
        {DCSSHRD
            return TCursor(cmCursor::tree, HasNextSibling() ? static_cast<cmNode*>(cmCursor::node->next())
                                                            : nullptr);
        }

        /// Moves this cursor to the next sibling of the represented node.
        /// If the node has no next sibling, this cursor becomes invalid.
        /// The latter is always true if this is the root node of the tree.
        ///
        /// @return \c true if this cursor was moved,
        ///         \c false if the represented node has no next sibling.
        bool GoToNextSibling()
        {DCSSHRD
            // go to node next and check for hook?
            if (HasNextSibling())
            {
                cmCursor::node = static_cast<cmNode*>(cmCursor::node->next());
                return true;
            }
            cmCursor::node = nullptr;
            return false;
        }

        /// Returns a cursor value that represents the previous sibling of the node
        /// represented this cursor.
        /// If the node has no previous sibling, an invalid cursor is returned.
        ///
        /// @return A cursor object representing the previous sibling of the node
        ///         represented by this object.
        TCursor PreviousSibling() const
        {DCSSHRD
            return TCursor(cmCursor::tree,  HasPreviousSibling() ? static_cast<cmNode*>(cmCursor::node->prev())
                                                                 : nullptr );
        }

        /// Moves this cursor to the previous sibling of the represented node.
        /// If the node has no previous sibling, this cursor becomes invalid.
        /// The latter is always true if this is the root node of the tree.
        ///
        /// @return \c true  if this cursor was moved,
        ///         \c false if the represented node has no previous sibling.
        bool        GoToPreviousSibling()
        {DCSSHRD
            if( HasPreviousSibling() )
            {
                cmCursor::node= static_cast<cmNode*>(cmCursor::node->prev());
                return true;
            }
            cmCursor::node= nullptr;
            return false;
        }

        /// Returns a cursor object that represents the first child of the node
        /// represented.
        /// If the represented node has no children, an invalid cursor is returned.
        ///
        /// @return A cursor representing the first child of the node this cursor points to.
        TCursor     FirstChild()                                                           const
        {DCSSHRD
            return TCursor( cmCursor::tree, HasChildren() ? static_cast<cmNode*>(cmCursor::node->children.first())
                                                          : nullptr                        );
        }

        /// Moves this cursor to the first child of its represented node.
        /// If the represented node has no children, this cursor becomes invalid.
        ///
        /// @return \c true if the cursor was moved, \c false if the represented node
        ///         has no children.
        bool        GoToFirstChild()
        {DCSSHRD
            if( HasChildren() )
            {
                cmCursor::node= static_cast<cmNode*>(cmCursor::node->children.first());
                return true;
            }
            cmCursor::node= nullptr;
            return false;
        }

        /// Returns a cursor value that represents the last child of the node
        /// represented.
        /// If the represented node has no children, an invalid cursor is returned.
        ///
        /// @return A cursor representing the last child of the node represented
        ///         by this cursor.
        TCursor     LastChild()                                                            const
        {DCSSHRD
            return TCursor( cmCursor::tree, HasChildren() ? static_cast<cmNode*>(cmCursor::node->children.last())
                                                          : nullptr );
        }

        /// Moves this cursor to the last child of its represented node.
        /// If the represented node has no children, this cursor becomes invalid.
        ///
        /// @return \c true if the cursor was moved, \c false if the represented node
        ///         has no children.
        bool        GoToLastChild()
        {DCSSHRD
            if( HasChildren() )
            {
                cmCursor::node= static_cast<cmNode*>(cmCursor::node->children.last());
                return true;
            }
            cmCursor::node= nullptr;
            return false;
        }

        /// Searches a child with the given name and returns a cursor to it.
        /// If no child with this name exists, the returned cursor is invalid
        ///
        /// The given \p{name} is not considered a path and is not checked for being <c>"."</c>
        /// or <c>".."</c> or if it contains a separator character.
        /// Children with such name cannot exist and hence can't be found. However, in
        /// debug-builds, a \ref alib_mod_assert "warning is raised".
        ///
        /// @param  name   The name of the child to search.
        /// @return A cursor representing the last child of the node represented
        ///         by this cursor.
        TCursor     Child( const NameType& name )                                          const
        {DCSSHRD
            dbgCheckTreeAndNode();
            ALIB_DBG( cmCursor::tree->checkChildName( name ); ) // gives warning
            return TCursor( cmCursor::tree,
                           static_cast<cmNode*>(cmCursor::node->findChild(cmCursor::tree, name)));
        }

        /// Searches a child with the given name and moves this cursor to it.
        /// If no child with this name exists, the cursor does not change and
        /// \c false is returned.
        ///
        /// The given \p{name} is not considered a path and is not checked for being <c>"."</c>
        /// or <c>".."</c> or if it contains a separator character.
        /// Children with such a name cannot exist and hence can't be found. However, in
        /// debug-builds, a \ref alib_mod_assert "warning is raised".
        ///
        /// @param  name   The name of the child to search.
        /// @return \c true if the child existed and this object changed, \c false
        ///            otherwise.
        bool        GoToChild( const NameType& name )
        {DCSSHRD
            dbgCheckTreeAndNode();
            ALIB_DBG( cmCursor::tree->checkChildName( name ); )

            cmNode* child= static_cast<cmNode*>(cmCursor::node->findChild( cmCursor::tree, name ));
            if(child)
            {
                cmCursor::node= child;
                return true;
            }
            return false;
        }

        /// Moves this cursor to the child with given \p{name}.
        /// If no child with this name exists, one will be created.
        ///
        /// If the given \p{childName} is invalid (equals to <c>"."</c> or
        /// <c>".."</c> or contains the separation character), then still \c true is returned,
        /// but this cursor becomes invalid.
        /// In addition, with debug-builds, a \ref alib_mod_assert "warning is raised".
        ///
        /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
        /// @param  name   The name of the desired child.
        /// @param  args   Variadic parameters to be forwarded to the constructor of custom type
        ///                \p{T} in the case a child is created.
        /// @return A pair of a cursor pointing to the child and a boolean that equals
        ///         \c false if the child was found, and \c true if a child was created.
        ///         If the given name was invalid, the returned cursor will be invalid
        ///         while the boolean still indicates "not found" (aka \c true).
        template<typename... TArgs>
        std::pair<TCursor, bool> CreateChildIfNotExistent( const NameType& name,
                                                           TArgs&&... args  )
        {DCS
            dbgCheckTreeAndNode();
            if( !cmCursor::tree->checkChildName( name ) )
                return std::make_pair( TCursor(cmCursor::tree, nullptr), true );

            auto result= cmCursor::node->findOrCreateChild( cmCursor::tree, name,
                                                            std::forward<TArgs>(args)... );

            return std::make_pair( TCursor( cmCursor::tree, result.first ), result.second );
        }

        /// Moves this cursor to the child with given \p{name}.
        /// If no child with this name exists, one will be created.
        ///
        /// If the given \p{childName} is invalid (equals to <c>"."</c> or
        /// <c>".."</c> or contains the separation character), then still \c true is returned,
        /// but this cursor becomes invalid.
        /// In addition, with debug-builds, a \ref alib_mod_assert "warning is raised".
        ///
        /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
        /// @param  name   The name of the desired child.
        /// @param  args   Variadic parameters to be forwarded to the constructor of custom type
        ///                \p{T} in the case a child is created.
        /// @return \c false if the child was found, and \c true if one was created or the given
        ///         child name was invalid.
        template<typename... TArgs>
        bool        GoToCreateChildIfNotExistent(const NameType& name, TArgs&&... args)
        {DCS
            dbgCheckTreeAndNode();
            if( !cmCursor::tree->checkChildName( name ) )
            {
                cmCursor::node= nullptr;
                return true;
            }

            auto result= cmCursor::node->findOrCreateChild( cmCursor::tree, name,
                                                            std::forward<TArgs>(args)... );

            cmCursor::node= static_cast<cmNode*>(result.first);
            return result.second;
        }

        /// Follows the given \p{path} from the currently represented node to the target
        /// node and returns a new cursor instance..
        ///
        /// The method supports absolute and relative path addressing: If \p{path} begins
        /// with a separation character, then the transition starts with the root node of the
        /// \b %StringTree. Furthermore, child name <c>"."</c> is ignored and just skipped
        /// while a name of <c>".."</c> addresses the parent node during the transition.
        /// Repeated separation characters are ignored.<br>
        /// If, while processing the path string, the root node is found an the next
        /// path element is "..", this element is ignored and processing continues.
        /// As a sample, assuming that nodes \e /a and \e /b exist, the paths:
        ///
        ///      /a/../b
        /// and
        ///
        ///      /a/../../b
        /// both evaluate to
        ///
        ///      /b
        ///
        /// Relative paths must not be used on \ref TCursor::IsValid "invalid" cursors. Doing so
        /// is undefined behavior and \ref alib_mod_assert "raises an ALib error" in
        /// debug-compilations.
        ///
        /// If a child along the path does not exist, the traversal is ended and the remaining
        /// portion of the path is returned.
        ///
        /// \note
        ///   If parameter \p{path} is a temporary object, the resulting \b Substring must
        ///   not be used, as it refers to the given string's buffer. In any case, its
        ///   length can still be compared to \c 0 to evaluate success of the traversal.
        ///
        /// @param path  The path to follow, starting with the node this pointer represents.
        ///
        /// @return A pair of a cursor pointing to last child not of the existing portion
        ///         of the given \p{path}, and a substring that contains the non-existing
        ///         portion of a path, or is empty if the complete path existed.
        std::pair<TCursor, SubstringType> operator()( const NameType& path )               const
        {DCSSHRD
            dbgCheckTreeAndNode();
            SubstringType remainingPath(path);
            cmNode* grandChild= cmCursor::followPath( remainingPath );
            return std::make_pair( TCursor(cmCursor::tree, grandChild), remainingPath );
        }

        /// Same as the
        /// \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a55db5acb390a2d38dd8e30c123cb6e4b;operator()(const NameType&)},
        /// but moves this cursor instead of returning a new one.
        ///
        /// @param path  The path to move this cursor along.
        /// @return The unconsumed portion of the path.
        ///         An empty \b Substring if the path existed.
        SubstringType GoTo( const NameType& path )
        {DCSSHRD
            dbgCheckTreeAndNode();
            SubstringType remainingPath(path);
            cmCursor::node= cmCursor::followPath( remainingPath );
            return remainingPath;
        }

        /// Follows the given path and creates non-existing children along the way.
        ///
        /// Child names <c>"."</c> and <c>".."</c> are allowed and respected the same as
        /// documented with the
        /// \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a55db5acb390a2d38dd8e30c123cb6e4b;operator()(const NameType&)}.
        /// New child nodes are constructed by forwarding the given \p{args}. Existing children
        /// remain untouched.
        ///
        /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
        /// @param  path   The path to move along.
        /// @param  args   Variadic parameters to be forwarded to the constructor of each node
        ///                that is created.
        /// @return A <c>std::pair</c> containing a resulting \b Cursor and the number of nodes
        ///         created.
        template<typename... TArgs>
        std::pair<TCursor, integer>  CreatePathIfNotExistent( const NameType&  path,
                                                              TArgs&&...       args  )
        {DCS
            dbgCheckTree();
            ALIB_ASSERT_ERROR( IsValid() || path.CharAtStart() == cmCursor::tree->separator,
                   "STRINGTREE", "Invalid StringTree::Cursor given with relative path addressing." )

            auto result= cmCursor::followPathCreate( path, std::forward<TArgs>(args)... );

            return std::make_pair( TCursor(cmCursor::tree, static_cast<cmNode*>(result.first)),
                                   result.second );
        }

        /// Follows the given path and creates non-existing children along the way.
        ///
        /// Child names <c>"."</c> and <c>".."</c> are allowed and respected same as in
        /// \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a55db5acb390a2d38dd8e30c123cb6e4b;operator()(const NameType&)}.
        ///
        /// New child nodes are constructed by forwarding the given \p{args}. Existing children
        /// remain untouched.
        ///
        /// @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
        /// @param  path   The path to move along.
        /// @param  args   Variadic parameters to be forwarded to the constructor of each node
        ///                that is created.
        /// @return The number of nodes created.
        template<typename... TArgs>
        integer                  GoToCreatedPathIfNotExistent( const NameType& path,
                                                               TArgs&&...      args      )
        {DCS
            dbgCheckTree();
            ALIB_ASSERT_ERROR( IsValid() || path.CharAtStart() == cmCursor::tree->separator,
                  "STRINGTREE", "Invalid StringTree::Cursor given with relative path addressing." )

            auto result= cmCursor::followPathCreate( path, std::forward<TArgs>(args)... );
            cmCursor::node= static_cast<cmNode*>(result.first);
            return result.second;
        }

        //#####  Cursor Interface        #######################################################
        /// Returns the name of the represented node.
        /// Note that the concatenated names of recursive child nodes, separated by
        /// \p{TSeparator} constitute a \e path.
        ///
        /// @return A constant reference to the name of the represented node.
        const NameType& Name()                                                             const
        {
            dbgCheckTreeAndNode();
            return cmCursor::node->name.key;
        }

        /// Returns the tree that this cursor belongs to.
        /// @tparam TParent  Optional template parameter, which casts the internal tree type
        ///                  to a derived type. This is for convenience, as otherwise the
        ///                  cast hast to be done by the caller which does not look too nice.
        /// @return The tree that this object refers to.
        template<typename TParent= StringTree>
        requires std::derived_from<TParent, StringTree>
        TParent& Tree()                                                                    const
        { dbgCheckTree(); return *static_cast<TParent*>(cmCursor::tree); }

        /// Retrieves a reference to the templated value of type \p{T} stored in the represented
        /// node.
        /// \note This method is only available if the template parameter \p{TConst} of this
        ///       type is \c false.
        /// @see Operators #operator->() and #operator*().
        /// @tparam TRequires Defaulted template parameter. Must not be specified.
        /// @return The current node's value.
        template<bool TRequires= !TConst>
        requires TRequires
        T&              Value()
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return static_cast<baseNode*>(cmCursor::node)->data;
        }

        /// Retrieves a constant reference to the templated value of type \p{T} stored in
        /// the represented node.
        /// @see Operators #operator->() and #operator*().
        /// @return The current node's value.
        const T&        Value()                                                            const
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return static_cast<const baseNode*>(cmCursor::node)->data;
        }

        /// Retrieves a pointer to the templated value of type \p{T} stored
        /// in the represented node.
        /// \note This method is only available if the template parameter \p{TConst} of this
        ///       type is \c false.
        /// @tparam TRequires Defaulted template parameter. Must not be specified.
        /// @return The current node's value.
        template<bool TRequires= !TConst>
        requires TRequires
        T*              operator->()
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return &static_cast<baseNode*>(cmCursor::node)->data;
        }

        /// Retrieves a constant pointer to the templated value of type \p{T} stored in
        /// the represented node.
        /// @return The current node's value.
        const T*        operator->()                                                       const
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return &static_cast<const baseNode*>(cmCursor::node)->data;
        }

        /// Retrieves a reference to the templated value of type \p{T} stored
        /// in the represented node.
        /// \note This method is only available if the template parameter \p{TConst} of this
        ///       type is \c false.
        /// @tparam TRequires Defaulted template parameter. Must not be specified.
        /// @return The current node's value.
        template<bool TRequires= !TConst>
        requires TRequires
        T&              operator*()
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return static_cast<baseNode*>(cmCursor::node)->data;
        }

        /// Retrieves a reference to the templated value of type \p{T} stored
        /// in the represented node.
        /// \note This operator is only available if template parameter \p{TConst} is false.
        /// @return The current node's value.
        const T&        operator*()                                                        const
        {
            dbgCheckTree();
            ALIB_ASSERT_ERROR( !IsRoot() || cmCursor::tree->dbgRootDataSet > 0, "STRINGTREE",
              "Root node has no value. Either this operation is unwanted or root node's value\n"
              "has to be explicitly set using SetRootNode(...)" )
            return static_cast<const baseNode*>(cmCursor::node)->data;
        }

        /// Returns \c true if this cursor represents the root node of the
        /// \b %StringTree, \c false otherwise.
        /// @return \c true if this object represents the root node, \c false otherwise.
        bool            IsRoot()                                                           const
        {
            dbgCheckTreeAndNode();
            return cmCursor::node->isRoot();
        }

        /// Determines the depth of the node represented by this object. This is done by
        /// counting the iterations needed to reach the root node of the tree.
        /// @return The distance from this node to the root node.
        int             Depth()                                                            const
        {DCSSHRD
            dbgCheckTreeAndNode();
            return cmCursor::node->depth();
        }

        /// Determines the distance between the node represented by this cursor to the
        /// node represented by given \p{other}. The distace is defined as follows:
        ///
        /// - \b 0 if other represents the same node.
        /// - \b 1 if other represents the parent of this node.
        /// - \b 2 if other represents the grand-parent of this node.
        /// - \b 3 ...
        /// - \b N if other represents the root node.
        ///
        /// @param other The node to test.
        /// @return The distance from this node to the root node. \c -1 is returned in case
        ///         \p{other} is not found in the path to this node.
        int             Distance( const TCursor<true>& other )                             const
        {DCSSHRD
            dbgCheckTreeAndNode();
            ALIB_ASSERT_ERROR( other.node, "STRINGTREE", "Invalid node given." )
            ALIB_ASSERT_ERROR( cmCursor::tree == other.tree, "STRINGTREE",
                               "Given node belongs to a different StringTree." )
            return cmCursor::node->distance( other.node );
        }

        /// Returns \c true if the represented node has at least one direct child.
        /// @return \c true if the current node has children, \c false otherwise.
        bool            HasChildren()                                                      const
        {DCSSHRD
            dbgCheckTreeAndNode();
            return cmCursor::node->qtyChildren != 0;
        }

        /// Returns the number of direct children of the represented node.<br>
        /// Note that this method runs in constant time.
        /// @return The number of direct children of the represented node.
        uinteger        CountChildren()                                                    const
        {DCSSHRD
            dbgCheckTreeAndNode();
            return cmCursor::node->qtyChildren;
        }

        /// Evaluates if the node represented by this object has a next sibling in its
        /// parent's list of children.
        /// @return \c true if a next sibling to this object's represented node exists,
        ///         \c false otherwise.
        bool            HasNextSibling()                                                   const
        {DCSSHRD
            dbgCheckTreeAndNode();
            return     !IsRoot()
                    && !cmCursor::node->parent->children.isLast( cmCursor::node );
        }

        /// Evaluates if the node represented by this object has a previous sibling in its
        /// parent's list of children.
        /// @return \c true if a previous sibling to this object's represented node exists,
        ///         \c false otherwise.
        bool            HasPreviousSibling()                                               const
        {DCSSHRD
            dbgCheckTreeAndNode();
            return     !IsRoot()
                    && !cmCursor::node->parent->children.isFirst( cmCursor::node );
        }

        /// Writes the absolute path to the represented node (including the represented node's
        /// name) to the given \b %AString.<br>
        /// If this node represents the root node, then nothing is written but a single
        /// separation character.
        ///
        /// While the implementation of this method is as efficient as possible (to avoid
        /// insertions at the beginning of the target string while moving to the destination/root
        /// node, internally a local node-stack is created first, which then is traversed
        /// top-down), still in many situations, it is recommended to search for other ways to
        /// keep track of the current path of a node and modify and re-use such path.
        /// For this, class \alib{containers;StringTree::RecursiveIterator}, optionally
        /// allows maintaining a string representing the current path with every iteration.
        ///
        /// \see
        ///  Overloaded version <b>AssemblePath(AString&,TCursor<TConst>&,lang::CurrentData)</b>,
        ///  which allows the creation a relative path from a parent node to this node.
        ///
        /// @param targetString  The string buffer to append the path to.
        /// @param targetData    Denotes whether \p{target} should be cleared before
        ///                      appending the path. Defaults to \b CurrentData::Clear.
        /// @return The given \b AString to allow concatenated operations.
        strings::TAString<typename cmTree::CharacterType, lang::HeapAllocator>&
        AssemblePath( strings::TAString<typename cmTree::CharacterType,
                                        lang::HeapAllocator            >&  targetString,
                      lang::CurrentData                                    targetData=
                                                              lang::CurrentData::Clear )   const
        {DCSSHRD
            if( targetData == lang::CurrentData::Clear )
                targetString.Reset();

            return cmCursor::node->assemblePath( targetString, cmCursor::node, nullptr,
                                                 cmCursor::tree->separator );
        }

        /// Same as #AssemblePath but accepts a parent node to stop at, instead of the root node.
        /// The path created is a relative path from the \p{parent} to the represented node,
        /// hence it does \b not include the parent' name and also does \b not start with the
        /// separation character. The latter is true even if the given \p targetParent represents
        /// the root node. In this case the path is a relative path from the root node \c '/'
        /// to the child.
        ///
        /// If the given \p{parent} is not found within the list of parent nodes, then
        /// an absolute path from the tree's root to the represented node is returned.
        ///
        ///
        /// @param targetString The string buffer to append the path to.
        /// @param parent       Denotes the parent node to start a relative path from.
        /// @param targetData   Denotes whether \p{target} should be cleared before
        ///                     appending the path. Defaults to \b CurrentData::Clear.
        /// @return The given \b AString to allow concatenated operations.
        strings::TAString<typename cmTree::CharacterType, lang::HeapAllocator>&
        AssemblePath( strings::TAString<typename cmTree::CharacterType,
                                        lang::HeapAllocator             >&  targetString,
                      const TCursor<true>&                                  parent,
                      lang::CurrentData                                     targetData
                                                            = lang::CurrentData::Clear )   const
        {DCSSHRD
            dbgCheckTreeAndNode();
            if( targetData == lang::CurrentData::Clear )
                targetString.Reset();
            return cmCursor::node->assemblePath( targetString, cmCursor::node, parent.node,
                                                 cmCursor::tree->separator );
        }

        //#####  Cursor child creation     #####################################################
        /// Creates and returns a child node. If a node already exists, nothing is done and
        /// \c nullptr is returned as this is considered an error.
        ///
        /// If the child name is illegal (equal to <c>"."</c> or <c>".."</c> or contains a
        /// separation character), an \alib warning is raised and an invalid cursor
        /// is returned.
        ///
        /// Template parameter \p{TCheck} may be used to suppress the search for an
        /// existing child with the same name, as well as the check for correctness of the
        /// given child name.
        /// This tremendously improves the execution performance of this method.
        ///
        /// \attention Setting template parameter \p{TCheck} to \alib{NC} and inserting
        ///            child nodes with the same name, sets a \b %StringTree to an
        ///            undefined state.
        ///
        /// @tparam TCheck     If \alib{NC}, no check for an existing child with the same name
        ///                    is performed.
        /// @tparam TArgs      Types of variadic parameters given with parameter \p{args}.
        /// @param  childName  The name of the child
        /// @param  args       Variadic parameters to be forwarded to the constructor of custom
        ///                    type \p{T} of the child created.
        ///
        /// @return A new cursor object representing the created child node.
        ///         If the given \p{childName} was invalid or the child existed already,
        ///         the returned object is invalid.
        template<typename TCheck =CHK, typename... TArgs>
        TCursor CreateChild( const NameType& childName, TArgs&&... args )                  const
        {DCS
            dbgCheckTreeAndNode();
            if constexpr ( TCheck::value )
            {
                // check name
                if( !baseCursor::tree->checkChildName( childName ) )
                {
                    ALIB_WARNING( "STRINGTREE", "Illegal child name \"{}\"", childName )
                    return Cursor( baseCursor::tree, nullptr );
                }

                // check existence
                if(     baseCursor::node->qtyChildren > 0
                    &&  baseCursor::tree->nodeTable.Contains( baseNodeKey( baseCursor::node, childName) ))
                    return Cursor( baseCursor::tree, nullptr );
            }

            baseNode* child= &baseCursor::tree->nodeTable.EmplaceUnique( baseCursor::node, childName,
                                                                           std::forward<TArgs>(args)... )
                                               .Value();
            TNodeHandler::InitializeNode( *baseCursor::tree, *child );

            baseCursor::node->children.pushEnd( child );
            ++baseCursor::node->qtyChildren;
            return TCursor( baseCursor::tree, child );
        }

        //#####  Cursor deletion         #######################################################
        /// Searches and deletes the child named \p{childName} from the node that this object
        /// refers to. This object itself is not changed.
        ///
        /// \see
        ///   Overloaded version of this method that accepts a cursor referring to
        ///   the child in question.
        ///
        ///
        /// @param   childName   The name of the desired child.
        /// @return \c true if the child existed and was deleted, \c false otherwise.
        bool DeleteChild( const NameType& childName )                                      const
        {DCS
            dbgCheckTreeAndNode();
            if( baseCursor::node->qtyChildren == 0 )
                return false;

            auto handle= baseCursor::tree->nodeTable.Extract( baseNodeKey( baseCursor::node, childName) );
            if( handle.IsEmpty() )
                return false;
            handle.Value().deleteChildren( baseCursor::tree );
            TNodeHandler::FreeNode( *baseCursor::tree, handle.Value() );
            handle.Value().remove();

            --baseCursor::node->qtyChildren;
            return true;
        }

        /// Deletes the child represented by the given cursor \p{child} from the node that this
        /// cursor refers to. After the invocation, the given \p{child} cursor refers to its
        /// next sibling. If no such sibling exists, \p{child} becomes invalid.
        /// This cursor itself is not changed.
        ///
        /// \note
        ///   This method is useful to implement forward iterations through children of a parent
        ///   node with the aim to delete certain child nodes. No corresponding version of this
        ///   method exists that moves the given \p{child} pointer to its previous sibling.
        ///   For reverse iterations, a copy of the \p{child} argument has to be passed.
        ///   However, any overhead caused by such temporary object creation will be optimized
        ///   out by common C++ compilers.
        ///
        /// @param   child   Deletes the child represented by the given node.
        void DeleteChild( TCursor& child )                                                 const
        {DCS
            dbgCheckTreeAndNode();
            cmNode* nodeToDelete= child.node;
            child.GoToNextSibling();
            baseCursor::node->deleteChild( baseCursor::tree, nodeToDelete );
        }

        /// Deletes the children of the node that this cursor refers to.
        /// This object itself is not changed.
        ///
        /// @return The number of children that were deleted.
        uinteger DeleteChildren()                                                          const
        {DCS
            dbgCheckTreeAndNode();
            return  baseCursor::node->deleteChildren( baseCursor::tree );
        }

        /// Deletes the branch that this cursor refers to from the tree.
        /// If this cursor does not represent the root node, then after the operation,
        /// it refers to the parent of the current node.<br>
        /// If the represented node is the root node, only the children are deleted and this
        /// object remains representing the root node. Note that in this case any explicitly
        /// set custom value of the root node is \b not deleted. For this, exclusively methods
        /// #ConstructRootValue and #DestructRootValue are to be used.
        ///
        /// \note
        ///   If this method is invoked on an object returned by method
        ///   #RecursiveIterator::Node, the invoking iterator becomes invalid.<br>
        ///   To avoid this, method #RecursiveIterator::DeleteNode is to be used.
        ///
        /// @return
        ///   The total number of nodes deleted. Can be zero, in case this object represents
        ///   the root node and this node has no children.
        uinteger Delete()
        {DCS
            dbgCheckTreeAndNode();
            if( baseCursor::node->isRoot() )
                return baseCursor::node->deleteChildren( baseCursor::tree );

            cmNode * child= baseCursor::node;
            baseCursor::node= static_cast<cmNode*>(baseCursor::node->parent);
            return baseCursor::node->deleteChild( baseCursor::tree, child );
        }
    };  // inner class TCursor

    /// The mutable version of type \b TCursor<TConst>.
    using Cursor       = TCursor<false>;

    /// The constant version of type \b TCursor<TConst>.
    using ConstCursor  = TCursor<true>;

  protected:
    /// Protected methods that allows derived types to create cursor instances from
    /// nodes received directly from the hashtable.
    /// @param node The base node.
    /// @return A valid cursor.
    Cursor  createCursor(baseNode& node)                         { return Cursor(this, &node); }

  public:

    //==============================================================================================
    /// This public, inner class can be used to recursively iterate through the nodes of
    /// outer class \b StringTree.<br>
    /// The type does <b>not</b> apply to the concept of \c std::iterator_traits.
    /// The rationale for this is the fact that mechanics for sorting the child nodes are
    /// provided, which requires allocation of more resources than usual container iterators do.
    /// Therefore, objects of this type are not supposed to be temporary and
    /// created "on the fly", e.g., in C++ range based loops.
    /// Instead, instances should rather be created once and then re-used with later iterations.
    ///
    /// The sorting of child nodes is optional and can be changed before each recursion.
    /// A built-in comparison function which works on node names (path names) allows choosing
    /// ascending and descending order and to ignore or be sensitive about the letter case.
    /// Besides this, custom comparison functions that take a combination of arbitrary node
    /// attributes, including a node's value of template type \p{T} can be established.
    /// See overloaded methods #SetSorting for details on this topic.
    ///
    /// Objects of this type can be initialized, respectively reset to distinct start nodes by
    /// providing objects of
    /// - type \b %StringTree,
    /// - type \b %StringTree::Cursor or
    /// - other objects of this type itself,
    ///
    /// to overloaded methods #Initialize.
    ///
    /// The maximum depth of recursion may be limited with optional parameter \p{depth}
    /// found with each overloaded version of #Initialize.
    /// During the iteration, the recursion can be individually selected per node visited.
    /// This is done by using either of the methods #Next or #NextSibling to proceed.
    /// Furthermore, method #NextParentSibling allows skipping the rest of the current iteration
    /// branch.<br>
    /// The end of an iteration is detected with the method #IsValid.
    ///
    /// The children of a node can be iterated in a sorted fashion. To enable and disable
    /// sorting, various overloads of #SetSorting exist. These methods may be invoked any
    /// time during the iteration. Whenever a recursion in iteration occurs, the most recent
    /// settings of sorting are respected for the list of children of the node that is
    /// processed with that recursion.
    ///
    /// Finally, the generation of a string representing the actual path to the current
    /// iteration node, relative to the iteration's start node can be activated.
    /// See method #SetPathGeneration for mor        e information about this feature.
    ///
    /// \see
    ///   For more information on how this class is used, see paragraph
    ///   \ref alib_ns_containers_stringtree_iterator "2.2 Inner Class RecursiveIterator"
    ///   of the description of class \b %StringTree.
    ///
    /// ## Friends ##
    /// class \alib{containers;StringTree}
    //==============================================================================================
    template<bool TConst>
    class TRecursiveIterator
    {
        #if !DOXYGEN
            #if ALIB_DEBUG_CRITICAL_SECTIONS
                #undef   DCS
                #undef   DCSSHRD
                #define  DCS       ALIB_DCS_WITH(tree->nodeTable.dcs)
                #define  DCSSHRD   ALIB_DCS_SHARED_WITH(tree->nodeTable.dcs)
            #endif
        #endif
      protected:
        /// Constant or mutable version of the base tree type, depending on template parameter
        /// \p{TConst}
        using cmTree    = std::conditional_t<!TConst, StringTree  , const StringTree>;

        /// Constant or mutable version of the base node type, depending on template parameter
        /// \p{TConst}
        using cmNodeBase= std::conditional_t<!TConst, baseNodeBase, const baseNodeBase>;

        /// Constant or mutable version of the base node type, depending on template parameter
        /// \p{TConst}
        using cmNode    = std::conditional_t<!TConst, baseNode    , const baseNode>;

        /// Constant or mutable version of the base Cursor type, depending on template parameter
        /// \p{TConst}
        using cmCursor = std::conditional_t<!TConst, Cursor     , ConstCursor>;

        //######################################################################################
        // Inner type RecursionData
        //######################################################################################
        /// Protected, internal struct used to store the data of recursive iterations.
        struct RecursionData
        {
            /// Combines a node hook and and a current vector index used to remember the
            /// current child in unsorted, respectively sorted mode.
            union
            {
                /// The current child of the current node in case of unsorted access
                /// If this is pointing to the end of the child map, then
                /// the actual node itself is selected by this RecursiveIterator.
                cmNodeBase*         unsorted;

                /// The current child index in case of sorted access.
                /// A value of <c>size_t(-1)</c> indicates that
                /// the actual node itself is selected.
                size_t              sorted;

            } actChild;

            #if DOXYGEN
                /// The child hook of the parent node, used with unsorted iteration.
                /// Note that this is declared \c const, in case template param \p{TConst} equals
                /// \c true.
                lang::BidiListHook<baseNodeBase>*      childrenUnsorted;
            #else
                 std::conditional_t<!TConst,
                                    lang::BidiListHook<baseNodeBase>*,
                                    const lang::BidiListHook<baseNodeBase>*>   childrenUnsorted;
            #endif

            /// A pointer to a dynamically allocated vector of children used with sorting.
            std::vector<cmNode*>    childrenSorted;

            /// Copied from \alib{containers::StringTree;TRecursiveIterator} with every
            /// recursion step.
            bool                   (*customSorter)(const cmCursor&, const cmCursor&);

            /// Copied from \alib{containers::StringTree;TRecursiveIterator} with every
            /// recursion step.
            bool                    isSorting;

            /// Copied from \alib{containers::StringTree;TRecursiveIterator} with every
            /// recursion step.
            bool                    sortingIsDescending;

            /// Copied from \alib{containers::StringTree;TRecursiveIterator} with every
            /// recursion step.
            bool                    sortingIsCaseSensitive;


            /// Trivial default constructor.
            RecursionData()                                             noexcept          = default;

            /// Trivial default copy constructor.
            RecursionData( const RecursionData&  )                                        = default;

            /// Trivial default move constructor.
            RecursionData(       RecursionData&& )                      noexcept          = default;

            /// Trivial default copy assign operator.
             /// @return A reference to \c this.
            RecursionData& operator=( const RecursionData&  )                             = default;

            /// Trivial default move assign operator.
             /// @return A reference to \c this.
            RecursionData& operator=(       RecursionData&& )           noexcept          = default;

            /// Trival default destructor
            ~RecursionData()                                            noexcept          = default;
        }; // inner struct RecursionData

      //#######  RecursiveIterator members     #####################################################
        /// The \b %StringTree this iterator belongs to.
        cmTree*                     tree                                                  = nullptr;

        /// The pointer to the actual node.
        cmNode*                     node;

        /// A stack holding the recursive list of unsorted or sorted children and the
        /// hook to the current child. Implemented as a vector in combination with member #actDepth,
        /// to reuse allocated storage space during iteration and when this iterator is
        /// re-used (freshly initialized).
        std::vector<RecursionData>  stack;

        /// The current depth of the iteration (and usage but not size of field #stack).
        /// set to \c -1 to if iteration is finished, respectively this iterator was not
        /// initialized.
        size_t                      actDepth                                           = size_t(-1);

        /// The path to the actual node (excluding the name of the actual node).
        /// If this object is \e nulled, no paths are generated.
        AString                     actPath;

        /// The requested depth of iteration recursion.
        unsigned int                recursionDepth      =(std::numeric_limits<unsigned int>::max)();

        /// A pointer to a user-defined comparison function.
        bool                        (*nextCustomSorter)(const cmCursor&, const cmCursor&) = nullptr;

        /// Denotes if the children are iterated in a sorting fashion or not.
        bool                        nextIsSorting                                           = false;

        /// The sort order (used with built-in sorting by node name).
        bool                        nextSortingIsDescending                                 = false;

        /// The case sensitivity of the sort (used with built-in sorting by node name).
        bool                        nextSortingIsCaseSensitive                              = false;

    // ######## RecursiveIterator Constructor/Destructor    ########################################
    public:
        /// Default constructor.
        TRecursiveIterator()                                                           = default;

        /// Trivial copy constructor.
        TRecursiveIterator( const TRecursiveIterator&  )                               = default;

        /// Trivial default move constructor.
        TRecursiveIterator(       TRecursiveIterator&& )                              = default;

        /// Trivial default copy assign operator.
        /// @return A reference to \c this.
        TRecursiveIterator& operator=( const TRecursiveIterator&  )                   = default;

        /// Trivial default move assign operator.
        /// @return A reference to \c this.
        TRecursiveIterator& operator=(       TRecursiveIterator&& )                   = default;


        /// Destructor.
        ~TRecursiveIterator()                                                         = default;

      // ###### RecursiveIterator Interface      ###################################################
        /// With this method, the assembly of a string representing the path from the
        /// node used to initialize this iterator to the actual node, is activated or
        /// deactivated.<br>
        /// If activated, the path to the current node can be received using overloaded
        /// methods #CurrentPath and #FullPath.
        ///
        /// The invocation of the method invalidates this iterator. Note that the
        /// path generation can be enabled and disabled with the optional parameter
        /// of the overloaded #Initialize methods.
        ///
        /// @param pathGeneration Denotes whether the path should be generated or not.
        void SetPathGeneration( lang::Switch pathGeneration )
        {
            Invalidate();
            actPath.Reset( pathGeneration == lang::Switch::On ? EMPTY_STRING
                                                              : NULL_STRING  );
        }

        /// Resets this iterator to work with the given \b %StringTree.
        /// Initializes recursive iteration to the tree's root node.
        /// Optionally, a recursion depth can be set.
        ///
        /// @param pTree  The \b %StringTree to use.
        /// @param depth  Sets the recursion depth.
        ///               A depth of \c 0 iterates only the direct children of the root node.
        ///               Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
        ///               for "unlimited" recursion.
        void Initialize( cmTree&       pTree,
                         unsigned int  depth= (std::numeric_limits<unsigned int>::max)() )
        {
            initialize( &pTree, &pTree.root.root, depth );
        }

        /// Resets this iterator to the first child of the node that the given cursor
        /// object represents.
        /// If the cursor is invalid, the root node of the tree it represents is used.
        ///
        /// If the given node has no children, this iterator is marked invalid when this
        /// method returns.
        ///
        /// Optional parameter \p{depth} allows limiting the recursion depth.
        ///
        /// @param cursor The cursor to define the branch of the tree to iterate.
        /// @param depth  Sets the recursion depth.
        ///               A depth of \c 0 iterates only the direct children of the node
        ///               represented by \p{cursor}.
        ///               Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
        ///               for "unlimited" recursion.
        void Initialize( cmCursor      cursor,
                         unsigned int  depth= (std::numeric_limits<unsigned int>::max)()  )
        {
            initialize( static_cast<cmTree*>( cursor.tree ),
                        cursor.IsValid() ?  cursor.node : &cursor.tree->root.root,
                        depth                                                         );
        }

        /// Resets this iterator to the first child of the node that the given other iterator
        /// currently refers to. The given iterator has to be in a valid state.
        /// Optionally, a recursion depth can be set.
        ///
        /// @param other  The iterator whose current node becomes the start node for this
        ///               iterator.
        /// @param depth  Sets the recursion depth.
        ///               A depth of \c 0 iterates only the direct children of the node
        ///               represented by \p{other}.
        ///               Defaults to <c>std::numeric_limits<unsigned int>::max()</c>
        ///               for "unlimited" recursion.
        void Initialize( const TRecursiveIterator&   other,
                         unsigned int   depth= (std::numeric_limits<unsigned int>::max)() )
        {
            initialize( other.tree, other.node, depth );
        }

        /// Invalidates this object. After invoking this method, this iterator cannot be
        /// used further, until one of the overloaded methods #Initialize is invoked.
        /// After the invocation, method #IsValid will return \c false.
        void Invalidate()
        {
            actDepth= size_t(-1);
        }

        /// Determines if this is a valid. \b RecursiveIterator instances may become invalid
        /// after invocations of one of the methods #Next, #NextSibling or #NextParentSibling
        /// (at the end of the iteration) and become valid with the invocation of one of the
        /// overloaded methods #Initialize.
        ///
        /// @return \c true if this is a valid iterator. If invalid, \c false is returned and
        ///         the iterator must not be evaluated before being initialized.
        bool IsValid()                                                                     const
        {
            return actDepth != size_t(-1);
        }

        /// The negation of #IsValid.
        ///
        /// @return \c false if this is a valid iterator. If invalid, \c true is returned and
        ///         the iterator must not be evaluated before being initialized.
        bool IsInvalid()                                                                   const
        {
            return !IsValid();
        }


        /// Allows switching sorting on or off. If switched on, sorting is performed
        /// by the node names in ascending order.
        ///
        /// This and the overloaded versions of this method may be invoked at any time,
        /// even on invalid iterators and those that are not initialized.
        /// All that the methods do is store the given parameters for future use.
        /// Such a use happens whenever a recursive iteration over a list of child nodes is
        /// started. At that moment the current configuration of sorting is applied to
        /// the list of direct children.
        ///
        /// @param sorting  The switch value.
        void SetSorting( lang::Switch sorting )
        {
            if( sorting == lang::Switch::Off )
                nextIsSorting= false;
            else
                SetSorting(lang::SortOrder::Ascending, lang::Case::Ignore );
        }

        /// Sets the sorting of children by their path name, using the built-in comparison
        /// methods, which in turn use method \alib{strings;TString::Equals;String::Equals}.
        ///
        /// \see
        ///   For more details on sorting see method \ref SetSorting(lang::Switch sorting).
        ///
        ///
        /// @param order         The sort order.
        ///                      Defaults to \b %SortOrder::Ascending.
        /// @param sensitivity   The case sensitivity when comparing path names.
        ///                      Defaults to \b %Case::Ignore.
        void SetSorting( lang::SortOrder order         = lang::SortOrder::Ascending,
                         lang::Case      sensitivity   = lang::Case::Ignore            )
        {
            nextIsSorting             = true;
            nextCustomSorter          = nullptr;
            nextSortingIsDescending   =  ( order       == lang::SortOrder::Descending );
            nextSortingIsCaseSensitive=  ( sensitivity == lang::     Case::Sensitive  );
        }

        /// Sets the sorting of children by their template value, using the given
        /// callback function.
        ///
        /// \see
        ///   For more details on sorting see method \ref SetSorting(lang::Switch sorting).
        ///
        /// @param customSorterFunction  A custom comparison method used for sorting the children
        ///                      of nodes.
        void SetSorting( bool (*customSorterFunction)(const Cursor&, const Cursor&) )
        {
            nextIsSorting     = true;
            nextCustomSorter  = customSorterFunction;
        }

        /// Iterates to the first child of the current node. If no such child exists,
        /// to the next sibling node. If also no sibling exists, iteration continues
        /// with the next available node of a previous recursion level.
        ///
        /// @return \c true if a next node was found, \c false otherwise.
        ///         If \c false is returned, this iterator is invalid after the call.
        bool Next()                                                   {DCSSHRD return next(0); }

        /// Omits recursion on the current node's children, even if the current depth
        /// is lower than #RequestedDepth.<br>
        ///
        /// If no sibling exists, iteration continues with the next available node of a
        /// previous recursion level.
        ///
        /// @return \c true if a next node was found, \c false otherwise.
        ///         If \c false is returned, this iterator is invalid after the call.
        bool NextSibling()                                            {DCSSHRD return next(1); }

        /// Skips the remaining siblings of the current recursion level and continues with
        /// the next available sibling of a previous level.
        ///
        /// @return \c true if a next node was found, \c false otherwise.
        ///         If \c false is returned, this iterator is invalid after the call.
        bool NextParentSibling()                                      {DCSSHRD return next(2); }


        /// Retrieves the current path of walking as a string representation.
        /// The path returned is relative to the start node and does not contain a leading
        /// separator character. Also, it does not contain the name of the current node,
        /// which can be received by invoking method #TCursor::Name on the cursor returned by
        /// method #Node.
        ///
        /// Note that this method can be used only if path generation was activated
        /// before the current iteration. Activation is performed with method
        /// #SetPathGeneration.
        ///
        /// @return The path of the current node.
        const NameType&    CurrentPath()                                                   const
        {
            ALIB_ASSERT_ERROR( actPath.IsNotNull(), "STRINGTREE", "Path generation not activated" )
            return actPath;
        }

        /// Writes the results of #CurrentPath and #TCursor::Name, separated by the separator
        /// character \p{TSeparator}.
        ///
        /// Note that this method can be used only if path generation was activated
        /// before the current iteration. Activation is performed with method
        /// #SetPathGeneration.
        ///
        /// @param target        The target to append the path to.
        /// @param targetData    Denotes whether \p{target} should be cleared before
        ///                      appending the path. Defaults to CurrentData::Clear.
        /// @return The given string to allow concatenated operations
        AString&    FullPath( AString&          target,
                              lang::CurrentData targetData= lang::CurrentData::Clear )     const
        {
            ALIB_ASSERT_ERROR( actPath.IsNotNull(), "STRINGTREE", "Path generation not activated" )

            if( targetData == lang::CurrentData::Clear )
                target.Reset();

            if( actPath.IsNotEmpty() )
                target << actPath  << tree->separator;

            return target << node->name.key;
        }


        /// Returns the requested maximum depth of iteration, set with #Initialize.
        ///
        /// @see For the current iteration depth, use #CurrentDepth.
        ///
        ///
        /// @return The distance of the current node and the node of the start of the
        ///         iteration.
        int RequestedDepth()                              const  { return int(recursionDepth); }

        /// Returns the depth of the current iteration. This is value is available to the
        /// algorithm which means this method executes in constant time.
        ///
        /// To get the absolute depth of the current node, method #TCursor::Depth may be used.
        ///
        /// @return The distance of the current node and the node of the start of the
        ///         iteration.
        int CurrentDepth()                                                                 const
        {
            ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                               "RecursiveIterator not initialized or exceeded (invalid)." )
            return int(actDepth);
        }

        /// Returns the current node, encapsulated in a cursor object.
        ///
        /// \note
        ///   It is \b not allowed to use method #TCursor::Delete on the node returned by
        ///   this method. As a replacement, use method #DeleteNode implemented in this
        ///   class.<br>
        ///   However, methods #TCursor::DeleteChild and #TCursor::DeleteChildren are allowed
        ///   to be invoked and therefore have no replacement in this class.
        ///
        /// @return An instance of the public node interface pointing to the currently
        ///         referenced tree node.
        cmCursor Node()                                                                    const
        {
            ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                               "RecursiveIterator not initialized or exceeded (invalid)." )
            return cmCursor( tree, node );
        }

      //######  node deletion (RecursiveIterator)  #############################################

        /// Deletes the node that this iterator currently refers to from the tree.
        /// After the operation, the iterator is moved forward to the next sibling
        /// of the current node, respectively of the first sibling found in the
        /// recursion stack.
        ///
        /// \note
        ///   This method constitutes a legal alternative to method #TCursor::Delete, which is
        ///   forbidden to be invoked on the node returned by method #Node as this would
        ///   invalidate this iterator.<br>
        ///
        ///   Methods #TCursor::DeleteChild and #TCursor::DeleteChildren are allowed with this
        ///   iterator type. Consequently, no replacement method for those is given with this
        ///   class.
        ///
        /// @return The total number of nodes deleted.
        uinteger DeleteNode()
        {DCS
            ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                               "RecursiveIterator not initialized or exceeded (invalid)." )
            auto& nodeToDelete= *node;
            next( 1 ); // next sibling
            return nodeToDelete.parent->deleteChild( tree, &nodeToDelete );
        }

      // ###### RecursiveIterator Internals      ###################################################
      protected:
        /// Resets this iterator to represent to the given node of the given tree.
        ///
        /// @param pTree   The tree to iterate on.
        /// @param newnode The new node to start the iteration from.
        /// @param depth   The requested recursion depth.
        void initialize( cmTree* pTree, cmNode* newnode, unsigned int depth )
        {
            this->tree= pTree;
            DCSSHRD
            if( actPath.IsNotNull() )
            {
                actPath.Reset();
                if( newnode->isRoot() )
                    actPath << tree->separator;
            }

            node= newnode;
            if( newnode->qtyChildren )
            {
                recursionDepth= depth;
                actDepth= size_t(-1);
                recursion();
            }
            else
                actDepth= size_t( -1 );
        }

        /// Sets this iterator to point to the first child of the actual node.
        /// If sorting is enabled, copies all children from the map to a vector and sorts
        /// them there.
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
                node= static_cast<cmNode*>(rd.actChild.unsorted= rd.childrenUnsorted->first());
                return;
            }

            // sorting: copy children to a sortable vector
            rd.childrenSorted.clear();
            rd.childrenSorted.reserve( size_t( node->qtyChildren ) );
            auto* copyIt= node->children.first();
            while( copyIt != &node->children.hook )
            {
                rd.childrenSorted.emplace_back( static_cast<cmNode*>(copyIt) );
                copyIt= copyIt->next();
            }

            // sort
            if( rd.customSorter )
            {
                std::sort( rd.childrenSorted.begin(), rd.childrenSorted.end(),
                           [this,&rd]( cmNode* lhs,
                                       cmNode* rhs  )
                           {
                                return rd.customSorter( cmCursor(tree, lhs),
                                                        cmCursor(tree, rhs) );
                           }
                          );
            }
            else
            {
                std::sort( rd.childrenSorted.begin(), rd.childrenSorted.end(),
                           [&rd]( cmNode* lhs, cmNode* rhs)
                           {
                                int compResult=  rd.sortingIsCaseSensitive
                                   ? lhs->name.key. template CompareTo<CHK, lang::Case::Sensitive>(rhs->name.key)
                                   : lhs->name.key. template CompareTo<CHK, lang::Case::Ignore   >(rhs->name.key);
                                return rd.sortingIsDescending ? compResult > 0
                                                              : compResult < 0;
                           }
                          );
            }

            // set to first child
            rd.actChild.sorted= 0;
            node= rd.childrenSorted[0];
        }


        /// Goes to the next node.
        /// This method is used with interface methods #Next, #NextSibling and
        /// #NextParentSibling, as well as with #DeleteNode}.
        ///
        /// @param skipMode   \c 0 iterates to the first child (if available),
        ///                   \c 1 iterates to the next sibling (if available) and
        ///                   \c 2 to the next available sibling of the parent, respectively the
        ///                      current recursion stack.
        /// @return \c true if this iterator is valid (a next node was found), \c false
        ///         otherwise.
        bool next(int skipMode)
        {
            ALIB_ASSERT_ERROR( actDepth != size_t(-1), "STRINGTREE", "Invalid iterator" )

            // recursion to first child of actual node?
            if( skipMode == 0
                && static_cast<unsigned int>( actDepth ) < recursionDepth
                && node->qtyChildren )
            {
                if( actPath.IsNotNull() )
                {
                    if(     actPath.IsNotEmpty()
                        && (actPath.Length() != 1 || actPath.CharAtStart() != tree->separator ) )
                        actPath << tree->separator;

                    actPath << node->name.key;
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
                            node      = static_cast<cmNode*>(rd.actChild.unsorted= rd.actChild.unsorted->next());
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
                            lastChar= actPath.CharAtEnd<NC>();
                            actPath.DeleteEnd<NC>( 1 );
                        }
                        while( lastChar != tree->separator && actPath.IsNotEmpty() );
                    }
                }
                else
                {
                    actDepth=  size_t(-1);
                    ALIB_ASSERT(    actPath.IsEmpty()
                                 || (actPath.Length() == 1 && actPath.CharAtStart() == tree->separator)
                                 , "STRINGTREE" )
                    break;
                }
            }

           return actDepth != size_t(-1);
        }
    };  // inner class "TRecursiveIterator"

    /// The mutable version of type \b TRecursiveIterator.
    using RecursiveIterator       = TRecursiveIterator<false>;

    /// The constant version of type \b TRecursiveIterator.
    using ConstRecursiveIterator  = TRecursiveIterator<true>;


    // #############################################################################################
    // StringTree Main
    // #############################################################################################
    #if !DOXYGEN
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            #undef   DCS
            #undef   DCSSHRD
            #define  DCS       ALIB_DCS_WITH(basetree::nodeTable.dcs)
            #define  DCSSHRD   ALIB_DCS_SHARED_WITH(basetree::nodeTable.dcs)
        #endif
    #endif

    //==============================================================================================
    /// Constructor.
    /// @param  allocator      The allocator instance to use.
    /// @param  pathSeparator  The separation character used with path strings.
    //==============================================================================================
    StringTree( AllocatorType& allocator, CharacterType pathSeparator )
    : basetree( allocator, pathSeparator )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS
         basetree::nodeTable.dcs.DCSName= "StringTree";
        #endif
    }

    //==============================================================================================
    /// Constructor taking a shared recycler.
    /// @param  pRecycler      The shared recycler.
    /// @param  pathSeparator  The separation character used with path strings.
    //==============================================================================================
    template<typename TSharedRecycler= SharedRecyclerType>
    requires(!std::same_as<TSharedRecycler, void>)
    StringTree( CharacterType pathSeparator, TSharedRecycler& pRecycler )
    : basetree( pRecycler, pathSeparator )
    {
        #if ALIB_DEBUG_CRITICAL_SECTIONS
         basetree::nodeTable.dcs.DCSName= "StringTree";
        #endif
    }

    //==============================================================================================
    /// Destructor.
    /// Raises a warning if \alib{containers::StringTree;ConstructRootValue;a root value was} but
    /// not deleted accordingly.
    //==============================================================================================
    ~StringTree()
    {
        for( auto& node : basetree::nodeTable )
            TNodeHandler::FreeNode( *this, *static_cast<baseNode*>(&node) );

        ALIB_ASSERT_WARNING( basetree::dbgRootDataSet == 0, "STRINGTREE",
          "Possible memory leak! The root node's value object was set but not deleted before\n"
          "destruction of this StringTree. To suppress this warning call DestructRootValue()\n"
          "before destruction. In case this is not necessary (because the stored type does not\n"
          "leak if not destructed), emplace it in macro ALIB_DBG() to remove the call in\n"
          "release-builds." )
    }

    //==============================================================================================
    /// Shortcut to
    /// \alib{containers;HashTable::GetAllocator;NodeTable().GetAllocator()}.
    /// @return The allocator that was provided in the constructor and stored in the internal
    ///         #NodeTable.
    //==============================================================================================
    AllocatorType&        GetAllocator()                                                    noexcept
    { return basetree::nodeTable.GetAllocator(); }

    //==============================================================================================
    /// Returns the path separator character that this string tree works with.
    /// @return The path separator character.
    //==============================================================================================
    constexpr CharacterType Separator()                                               const noexcept
    { return basetree::separator; }

    #if ALIB_DEBUG_CRITICAL_SECTIONS
    /// Sets the critical section name of this string tree. Empty and optimized out if compiler
    /// symbol \ref ALIB_DEBUG_CRITICAL_SECTIONS is not set.
    /// @param name The name to use with debug-assertions.
    void DbgSetDCSName(const char* name)            const { basetree::nodeTable.dcs.DCSName= name; }
    #else
    constexpr void DbgSetDCSName(const char*)                                               const {}
    #endif

    //==============================================================================================
    /// Depending on the use case, it might be appropriate to attach a value of template type \p{T}
    /// to the root node of the tree. If so, this can be done with this method. If not done, in debug
    /// compilations, method #TCursor::Value will
    /// raise an \alib assertion if called on the root node.
    ///
    /// Custom data that is explicitly attached to the root node with this method has to be
    /// deleted explicitly by calling #DestructRootValue before deletion of the tree.
    ///
    /// @tparam TArgs      Types of variadic parameters given with parameter \p{args}.
    /// @param  args       Variadic parameters to be forwarded to the constructor of custom
    ///                    type \p{T} of the child created.
    //==============================================================================================
    template<typename... TArgs>
    void    ConstructRootValue( TArgs&&...  args )
    {
        #if ALIB_DEBUG
           ALIB_ASSERT_WARNING( basetree::dbgRootDataSet != 1, "STRINGTREE",
             "Root node value is set without prior deletion. Possible memory leak (depending on\n "
             "allocation of template type T). This warning is only printed on the first overwrite.")
            ++basetree::dbgRootDataSet;
        #endif

        new (&basetree::root.root.data) T( std::forward<TArgs>(args)... );
    }

    //==============================================================================================
    /// Calls the destructor of the custom data object of type \p{T}, which may explicitly set using
    /// #ConstructRootValue.\n
    /// If not done, in debug-compilations, an \alib warning is raised in the destructor
    /// of this tree.
    //==============================================================================================
    void    DestructRootValue()
    {
        #if ALIB_DEBUG
            ALIB_ASSERT_ERROR( basetree::dbgRootDataSet != 0, "STRINGTREE",
              "Deletion of root node data without prior setting (or double deletion)." )
            --basetree::dbgRootDataSet;
        #endif
        basetree::root.root.data.~T();
    }

    //==============================================================================================
    /// Removes all elements from this container. The use of this method is more efficient than
    /// deleting the children of the root node.
    ///
    /// Invokes \alib{containers;HashTable::Clear} on field
    /// \alib{containers::detail::StringTreeBase;nodeTable}. As documented with that method, the
    /// allocated nodes will be preserved for "recycling" with future insertions.
    ///
    /// The custom data of the root node is preserved.
    //==============================================================================================
    void        Clear()
    {DCS
        // clear the nodes in the table, then the table itself
        for( auto& node : basetree::nodeTable )
            TNodeHandler::FreeNode( *this, *static_cast<baseNode*>(&node) );
        basetree::nodeTable.Clear();

        // re-initialize root node
        basetree::root.root.children.reset();
        basetree::root.root.qtyChildren= 0;
    }

    //==============================================================================================
    /// Clears all nodes and values. The use of this method is more efficient than deleting the
    /// children of the root node.<br>
    /// In addition, depending on template type \p{TNodeHandler}, it may also declare allocated
    /// memory for future reuse.<br>
    /// The latter is true for type \alib{containers;StringTreeNamesAlloc}.
    ///
    /// Invokes \alib{containers;HashTable::Reset} on field
    /// \alib{containers::detail::StringTreeBase;nodeTable}.
    /// As documented with that method, in the case of \alib{containers;Recycling;private recycling},
    /// all previously allocated recyclable instances of the internal element type are disposed.
    ///
    /// \note The value of the root-node, set with #ConstructRootValue is not deleted.
    //==============================================================================================
    void        Reset()
    {
        {DCS
            for( auto& node : basetree::nodeTable )
                TNodeHandler::FreeNode( *this, *static_cast<baseNode*>(&node) );
        }
        
        basetree::nodeTable.Reset();
        basetree::root.root.children.reset();
        basetree::root.root.qtyChildren= 0;
    }

    //==============================================================================================
    /// Counts the number of currently allocated but unused (not contained) element nodes
    /// that will be recycled with upcoming insertions.
    ///
    /// \note
    ///   This method is provided for completeness and unit-testing. It should not be of
    ///   relevance for common usage.
    ///   Furthermore, this method is not available (aka does not compile) with instantiations
    ///   that specify template parameter \p{TRecycling} as
    ///   \alib{containers;Recycling;None}.
    ///
    /// @return The number of removed and not yet recycled elements.
    //==============================================================================================
    integer     RecyclablesCount()                                                             const
    { return basetree::nodeTable.RecyclablesCount();  }

    //==============================================================================================
    /// Returns the overall number of elements contained in this tree.
    ///
    /// \note
    ///   This method performs in constant time.
    ///
    /// @return The number elements contained in this tree.
    //==============================================================================================
    integer     Size()                                                                         const
    { return basetree::nodeTable.Size();  }

    //==============================================================================================
    /// Tests for emptiness.
    ///
    /// @return \c true if this tree is empty, \c false otherwise.
    //==============================================================================================
    bool        IsEmpty()                                                                      const
    { return basetree::nodeTable.Size() == 0; }

    //==============================================================================================
    /// Invokes \alib{containers;HashTable::ReserveRecyclables} on the internal hashtable.
    ///
    /// @see Chapter \ref alib_contmono_containers_recycling_reserving of the Programmer's
    ///      Manual.
    ///
    /// @param qty       The expected resulting number (or increase) of elements to be stored in
    ///                  this container.
    /// @param reference Denotes whether \p{qty} is meant as an absolute size or an
    ///                  increase.
    //==============================================================================================
    void                ReserveRecyclables( integer qty, lang::ValueReference reference )
    { basetree::nodeTable.ReserveRecyclables( qty, reference );  }

    //==============================================================================================
    /// Returns the internal \alib{containers;HashTable} used for storing the tree nodes.
    /// This may be used to manipulate load factors, for direct iteration over all nodes, etc.<br>
    /// \note The returned object should be used with caution to keep the tree and its data
    ///       consistent.
    /// @return The internal node table.
    //==============================================================================================
    auto&               NodeTable()
    { return basetree::nodeTable; }

    //==============================================================================================
    /// Returns the internal \alib{containers;HashTable} used for storing the tree nodes.
    /// This may be used to manipulate load factors, for direct iteration over all nodes, etc.<br>
    /// \note The returned object should be used with caution to keep the tree and its data
    ///       consistent.
    /// @return The internal node table.
    //==============================================================================================
    const auto&         NodeTable()                                                            const
    { return basetree::nodeTable; }

    //==============================================================================================
    /// Creates a cursor instance representing the root node.
    /// @return A cursor pointing to the root node of this \b %StringTree.
    //==============================================================================================
    Cursor              Root()
    { return Cursor( this, &(basetree::root.root) ); }

    //==============================================================================================
    /// Creates a \c const cursor instance representing the root node.
    /// @return A read-only pointer, pointing to the root node of this \b %StringTree.
    //==============================================================================================
    const ConstCursor   Root()                                                                 const
    { return ConstCursor( this, &(basetree::root.root) ); }

    /// Imports a cursor previously exported with #TCursor::Export.
    /// @param handle The handle value.
    /// @return The imported cursor value.
    Cursor          ImportCursor( CursorHandle handle   )
    { return Cursor( this, reinterpret_cast<typename basetree::Node*>(handle.value) );  }

    /// Imports a cursor previously exported with #TCursor::Export.
    /// @param handle The handle value.
    /// @return The imported \c const cursor value.
    ConstCursor     ImportCursor( ConstCursorHandle handle)
    { return Cursor( this, reinterpret_cast<typename basetree::Node*>(handle.value) );  }

    #if !DOXYGEN
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            #undef   DCS
            #undef   DCSSHRD
        #endif
    #endif

}; // StringTree



} // namespace alib::[containers]

/// Type alias in namespace \b alib.
template<typename TChar, integer TLocalCapacity= 32>
using  StringTreeNamesDynamic    = containers::StringTreeNamesDynamic<TChar, TLocalCapacity>;

/// Type alias in namespace \b alib.
template<typename TChar>
using  StringTreeNamesStatic     = containers::StringTreeNamesStatic<TChar>;

/// Type alias in namespace \b alib.
template<typename TChar>
using  StringTreeNamesAlloc  = containers::StringTreeNamesAlloc<TChar>;

/// Type alias in namespace \b alib.
template<typename TAllocator,
         typename T,
         typename TNodeHandler   = StringTreeNamesDynamic<character>,
         Recycling     TRecycling     = Recycling::Private               >
using  StringTree = containers::StringTree<TAllocator, T, TNodeHandler, TRecycling>;


} // namespace [alib]

//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::containers::detail {

/// Base struct of \alib{containers;StringTree} providing internals.
/// \note
///   The separation of the internals of class \b StringTree to this type in namespace \b detail
///   has no benefit on compilation speed or other positive "technical" effect, nor is it a matter
///   of software design.<br>
///   A user of derived class \alib{containers;HashTable} finds all interface methods and types
///   in one place, which is not cluttered by the documentation of the internals found here.
///   Otherwise, the separation is exclusively supporting source code organization.
///
/// @see For a description of the template parameters and a general introduction to the topic,
///      se the reference documentation of the derived main class
///      \alib{containers;StringTree}.
template<typename TAllocator, typename  T, typename TNodeHandler, Recycling TRecycling>
struct StringTreeBase
{
  //################################################################################################
  // Inner types
  //################################################################################################
    struct NodeBase; // forward declaration
    struct Node;     // forward declaration

    /// Alias shortcut for a bidirectional list of \b Node elements.
    using NodeList          = lang::BidiListHook<NodeBase>;

    /// The character type of node names and paths strings. This is defined using character type
    /// definition \alib{containers::StringTreeNamesDynamic;CharacterType} of template type
    /// \p{TNodeHandler}.
    using CharacterType     = typename TNodeHandler::CharacterType;

    /// The string-type of node names and paths if provided externally for comparison.
    using NameType          = const strings::TString<CharacterType>;

    /// The string-type of node names and paths. This is defined by
    /// \alib{containers::StringTreeNamesDynamic;NameStringType} of template type \p{TNodeHandler}.
    using NameStorageType   = typename TNodeHandler::NameStringType;

    /// The substring-type of paths. This is defined using character type definition
    /// \alib{containers::StringTreeNamesDynamic;CharacterType} of template type \p{TNodeHandler}.
    using SubstringType     = typename strings::TSubstring<CharacterType>;

    /// The unique key to any element stored in this container.
    /// By being a (second) base type for of type \alib{containers::detail::StringTreeBase;Node}, any
    /// node includes this key.
    struct NodeKey
    {
        /// The parent node. A value of \c nullptr indicates that this is the root node of
        /// the tree, which is always existing.
        NodeBase*       parent;

        /// A union of base string and the derived (or same) final storage type. Only the node
        /// handler will finalize the name into the second field.
        union NodeNameUnion
        {
            /// Constructor taking a key string. @param n The name of the node.
            explicit NodeNameUnion(const NameType&   n) : key(n)                                  {}

            /// Copy constructor. @param n The union to copy.
            explicit NodeNameUnion(const NodeNameUnion& n) : key(n.key)                           {}

            /// Destructor.
            ~NodeNameUnion()                                                                      {}

            NameType        key;         ///< The name to compare when just keys are used.
            NameStorageType storage;     ///< The name when stored in the hashtable
        };

        /// A string object containing the pointer to this node's name.
        /// Node names constitute path strings and, together with the pointer to their parent, form
        /// the key of the hash set found with field \alib{containers::detail::StringTreeBase;nodeTable}.
        /// <br>
        /// Node names must not contain the separator character and must not equal to <c>"."</c> or
        /// <c>".."</c>.
        ///
        /// The name of the root node is nulled.
        NodeNameUnion name;

        /// Constructor
        /// @param pParent  Parent node to search a child for.
        /// @param pName    Child name to search
        NodeKey( NodeBase* pParent, const NameType& pName )
        : parent(pParent)
        , name( pName )                                                                           {}

        /// Hash functor for nodes hashed in field #nodeTable.
        struct Hash
        {
            /// Calculates a hash code for \b NodeKey objects.
            /// @param  key The key to hash.
            /// @return The hash code.
            std::size_t operator()(const NodeKey& key)                                       const {
                return   key.name.key.Hashcode()
                       + reinterpret_cast<std::size_t>(key.parent) * 29;
            }
        };

        /// Equality functor for nodes in field #nodeTable.
        struct EqualTo
        {
            /// Invokes \alib{strings;TString::Equals;String::Equals} on \p{lhs}, passing \p{rhs}
            /// and returns the result.
            /// @param lhs The first string object.
            /// @param rhs The second string object.
            /// @return The result of the string comparison.
            bool operator()(const NodeKey& lhs,
                            const NodeKey& rhs )                                               const
            {
                return     lhs.parent == rhs.parent
                        && lhs.name.key. template Equals<NC>( rhs.name.key );
            }
        };

        /// ValueDescriptor for hash table #nodeTable.
        struct ValueDescriptor :  containers::TSubsetKeyDescriptor<Node, NodeKey>
        {
            /// Casts given \p{src} down to its base class
            /// \alib{containers::detail::StringTreeBase;NodeKey}.
            /// @param src The node to receive the key-portion for.
            /// @return The key-portion of the node.
            NodeKey&       Key( NodeBase& src )       const { return static_cast<NodeKey&>( src ); }
        };
    };

    /// This is the base class of the internal node type \alib{containers::detail::StringTreeBase;Node}.
    /// This type implements functionality needed. Derived type \b Node then only adds the custom
    /// value \p{T}.
    ///
    /// Objects of this type cannot be received directly and all interface is available
    /// via public type \alib{containers;StringTree::Cursor} only, which holds a pointer to
    /// an object of this class.
    struct NodeBase : public lang::BidiNodeBase<NodeBase>,
                      public NodeKey
    {
        /// The number of children currently stored in this node.
        uinteger    qtyChildren;

        /// The hook to the doubly linked list of children.
        NodeList    children;

        /// Constructor.
        /// @param  pKey   The key portion of the node.
        NodeBase(const NodeKey& pKey)
        : NodeKey    ( pKey )
        , qtyChildren(0)                                                                          {}

        /// Constructor. Custom data is default-initialized.
        /// @param  pParent Parent node to search a child for.
        /// @param  pName   Child name to search
        NodeBase( NodeBase* pParent, const NameType& pName)
        : NodeKey    ( pParent, pName )
        , qtyChildren(0)                                                                          {}

        /// Returns \c true if this is the root node, \c false otherwise.
        /// @return \c true if this is the root node, \c false otherwise.
        bool isRoot()                                   const { return NodeKey::parent == nullptr; }

        /// Searches a child with a given name.
        /// The name is not checked for <c>.</c>, <c>..</c> or if separation characters.
        ///
        /// @param tree      The tree this node belongs to.
        /// @param childName The name of the child to search.
        /// @return The child or \c nullptr if not found.
        NodeBase*  findChild( StringTreeBase* tree, const NameType& childName ) {
            if( qtyChildren == 0  )
                return nullptr;

            // With a small number of children, the search is faster if we iterate, because
            // a) no hash value has to be calculated and
            // b) the string compare is fast, at least if string have different length or are
            //    different at the beginning.
            // A good value is probably five children. With bigger numbers, it soon becomes better
            // to calculate the hash value. While in the bucket also children of other nodes
            // are found, each entry of the hashtable is first compared against the full hash code.
            if( qtyChildren <= 5 ) {
                NodeBase* childIt= children.first();
                while( childIt != &children.hook ) {
                    if( childIt->name.key. template Equals<NC>( childName ) )
                        return childIt;
                    childIt= childIt->next();
                }

                return nullptr;
            }

            // search in hash table
            auto   childIt=   tree->nodeTable.Find( NodeKey( this, childName ) );
            return childIt != tree->nodeTable.end() ?  &childIt.Value()
                                                    :  nullptr;
        }

        /// Iterates over the parent nodes to the root node and returns this node's depth.
        /// @return The depth of this node.
        int depth()                                                                          const {
            int result= -1;
            const NodeBase* p= this;
            while( p != nullptr ) {
                ++result;
                p= p->parent;
            }
            return result;
        }

        /// Iterates over the parent nodes and searches given \p{other} in the path.
        /// @param other The node to calculate the distance to.
        /// @return The distance of \p{other} to this node.
        ///         \c 0 if the nodes are the same.
        ///         \c -1 if \p{other} was not found.
        int distance( const NodeBase* other )                                                const {
            int result= 0;
            const NodeBase* p= this;
            while( p != nullptr ) {
                if( p == other )
                    return result;
                ++result;
                p= p->parent;
            }
            return -1;
        }

        /// Searches a child with a given name, if not found, one is created.
        /// The name is not checked for <c>.</c>, <c>..</c> or if separation characters.
        ///
        /// @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
        /// @param  tree      The tree this node belongs to.
        /// @param  childName The name of the child to search.
        /// @param  args      Variadic parameters to be forwarded to the constructor of custom type
        ///                   \p{T} in the case a child is created.
        /// @return A pair containing an iterator referencing either the element found or the new
        ///         element added.
        ///         The bool component is \c true if the insertion took place and \c false nothing
        ///         was changed.
        template<typename... TArgs>
        std::pair<NodeBase*,bool>  findOrCreateChild( StringTreeBase*   tree,
                                                      const NameType&   childName,
                                                      TArgs&&...        args       ) {
            auto childCreation= tree->nodeTable.EmplaceIfNotExistent( NodeKey(this, childName),
                                                                      std::forward<TArgs>(args)...);
            NodeBase* child= &childCreation.first.Value();

            if( childCreation.second ) {
                TNodeHandler::InitializeNode( *static_cast<Node*>(child), *tree );
                children.pushEnd( child );
                ++qtyChildren;
            }

            return std::make_pair( child, childCreation.second );
        }

        /// Deletes a given child node.
        /// \note
        ///   If the given node is not a child of this node, the behavior is undefined.
        ///   With debug-builds, in this case an \alib_assertion is raised.
        ///
        /// @param tree   The tree this node belongs to.
        /// @param child  A pointer to a child of this node that is to be deleted.
        /// @return The total number of nodes deleted.
        uinteger  deleteChild( StringTreeBase* tree, NodeBase* child ) {
            ALIB_ASSERT_ERROR( NodeBase::qtyChildren   >  0,
                "STRINGTREE",  "This node has no children to remove")
            ALIB_ASSERT_ERROR( child->parent == this,
                "STRINGTREE", "The given node is not a child of this node.")

            --qtyChildren;
            child->remove(); // remove from linked list
            auto count= child->deleteChildren( tree );
            auto handle= tree->nodeTable.Extract( *child );
            ALIB_ASSERT( !handle.IsEmpty(), "STRINGTREE" )
            TNodeHandler::FreeNode( handle.Value(), *tree );

            return count + 1;
        }

        /// Deletes all child nodes.
        /// @param tree           The tree this node belongs to.
        /// @return The number of children that were deleted.
        uinteger deleteChildren( StringTreeBase* tree ) {
            if( children.isEmpty() )
                return 0;

            uinteger     count= qtyChildren;

            auto* child= children.first();
            while( child != &children.hook ) {
                count+=  child->deleteChildren( tree ); // recursion
                auto handle= tree->nodeTable.Extract( *child );    ALIB_ASSERT( !handle.IsEmpty(), "STRINGTREE")
                TNodeHandler::FreeNode( handle.Value(), *tree );
                child= child->next();
                --qtyChildren;
            }
            
            ALIB_ASSERT(qtyChildren == 0, "STRINGTREE")
            children.reset();
            return count;
        }

        /// Implementation of
        /// \alib{containers;StringTree<T,TNodeHandler,TRecycling>;TCursor::AssemblePath}.
        ///
        /// @param target        The target to append the path to.
        /// @param childNode     The (current) child node.
        /// @param maxParent     The last parent node to travel up to. The root node is designated
        ///                      by \c nullptr.
        /// @param separatorChar The separator character as defined with the template parameter of
        ///                      class \alib{containers;StringTree}.
        /// @return The given \b AString to allow concatenated operations on it.
        strings::TAString<CharacterType, lang::HeapAllocator>&
        assemblePath( strings::TAString<CharacterType,
                                        lang::HeapAllocator>&   target,
                      const NodeBase*                           childNode,
                      const NodeBase*                           maxParent,
                      CharacterType                             separatorChar )              const {
            static constexpr int STACK_SIZE= 32;
            const NodeBase*   nStack[STACK_SIZE];

            // build stack
            int               sp    =1;
            nStack[0]               = childNode;
            while( childNode->parent != maxParent ) {
                childNode= childNode->parent;
                if( childNode == nullptr)
                    break;

                // local stack full? -> let a recursive call do the rest
                if(sp == STACK_SIZE) {
                    assemblePath( target, childNode, maxParent, separatorChar );
                    break;
                }
                nStack[sp++]= childNode;
            }

            // unroll stack now from top to bottom
            while( --sp >= 0) {
                if( nStack[sp]->parent != nullptr ) {
                    if(     target.CharAtEnd() != separatorChar
                        &&  nStack[sp]->parent != maxParent      )
                        target << separatorChar;

                    target << nStack[sp]->name.key;
                }
                else
                    target << separatorChar;
            }

            return target;
        }
    }; // inner class NodeBase

    /// This is the "final" internal node type, just adds a field of template type \p{T}
    /// to its base class.
    ///
    /// Objects of this type cannot be received directly, and all interfaces are available
    /// via public type \alib{containers;StringTree::Cursor} only, which holds a pointer to
    /// an object of this class.
    struct Node : public NodeBase
    {
        /// The templated custom data object stored with each node.
        T           data;

        /// Deleted copy constructor.
        Node( const Node&  )                                                                =delete;

        /// Deleted move constructor.
        Node(       Node&& )                                                                =delete;

        /// Constructor. Custom data is default-initialized.
        /// @tparam TArgs Types of variadic parameters given with parameter \p{args}.
        /// @param  pKey  The key portion of the node.
        /// @param  args  Variadic parameters. Forwarded to the constructor of custom type \p{T}.
        template<typename... TArgs>
        Node( const NodeKey& pKey, TArgs&&... args  )
        : NodeBase( pKey )
        , data    ( std::forward<TArgs>(args)... )                                                {}

        /// Constructor. Custom data is default-initialized.
        /// @tparam TArgs   Types of variadic parameters given with parameter \p{args}.
        /// @param  pParent Parent node to search a child for.
        /// @param  pName   Child name to search
        /// @param  args    Variadic parameters. Forwarded to the constructor of custom type \p{T}.
        template<typename... TArgs>
        Node( NodeBase* pParent, const NameType& pName, TArgs&&... args  )
        : NodeBase( pParent, pName )
        , data    ( std::forward<TArgs>(args)... )                                                {}

    }; // inner class Node

  //################################################################################################
  // StringTreeBase: members
  //################################################################################################
    /// This is a union of either a node with a custom object or without. This tricks us into the
    /// position to embed the memory for a custom type which may optionally be assigned to the root
    /// node, without constructing it.
    /// Construction will only be done with explicit use of method
    /// \alib{containers;StringTree::ConstructRootValue}.
    union RootNodeSpacer
    {
        NodeBase rootBase;   ///< Base version of the root node, which becomes initialized.
        Node     root;       ///< Full version of the root node, without initialization of member T.

        /// Explicitly implement otherwise implicitly deleted constructor
        RootNodeSpacer() : rootBase( nullptr, nullptr)                                            {}

        /// Destructor
        ~RootNodeSpacer()                                                                         {}
    };

    /// The root node.
    RootNodeSpacer      root;

    #if ALIB_DEBUG
    /// Flag available only in debug-compilations to detect access to root node's value
    /// without prior use of method \alib{containers::StringTree;ConstructRootValue}. Also, the
    /// destructor issues a warning, in case the root node's value was not deleted with
    /// \alib{containers::StringTree;DestructRootValue}.
    int dbgRootDataSet                                                                           =0;
    #endif


    /// The separator character to use with path strings.
    /// This is set once with construction.
    CharacterType  separator;

    /// Hash set which contains all children of all nodes.
    /// This is used to find children of nodes by their parent/name combination.
    HashTable< TAllocator,
               typename NodeKey::ValueDescriptor,
               typename NodeKey::Hash,
               typename NodeKey::EqualTo,
               lang::Caching::Enabled,
               TRecycling                 >  nodeTable;

    /// This type definition may be used to define an externally managed shared recycler,
    /// which can be passed to the alternative constructor of this class when template
    /// parameter \p{TRecycling} equals \alib{containers;Recycling;Shared}.
    /// \see
    ///   Chapter \ref alib_contmono_containers_recycling_shared of the Programmer's Manual
    ///   for this \alibmod.
    using SharedRecyclerType=  typename decltype(nodeTable)::SharedRecyclerType;


  //################################################################################################
  // class TCursorBase
  //################################################################################################
    /// Base class for \alib{containers;StringTree::Cursor}
    /// @tparam TConst  If true, internal fields representing the StringTree and the current Node
    ///                become \c const and the method #followPathCreate becomes unavailable.
    template<bool TConst>
    struct TCursorBase
    {
        /// Constant or mutable version of the base tree type, depending on template parameter
        /// \p{TConst}
        using cmTree     = std::conditional_t<!TConst, StringTreeBase, const StringTreeBase>;

        /// Constant or mutable version of type \b NodeBase, depending on template parameter
        /// \p{TConst}
        using cmNodeBase = std::conditional_t<!TConst,       NodeBase, const       NodeBase>;

        /// Constant or mutable version of type \b Node, depending on template parameter \p{TConst}
        using cmNode     = std::conditional_t<!TConst,           Node, const           Node>;

        /// The currently represented node of the #tree.
        cmNode*          node;

        /// The \b %StringTree this object refers to.
        cmTree*          tree;

        /// Constructor initializing both fields #tree and #node.
        /// @param pNode  The node to refer to.
        /// @param pTree  The \b %StringTree we work on.
        TCursorBase( cmNode* pNode, cmTree* pTree)                          noexcept
        : node( pNode )
        , tree( pTree )                                                                           {}

        /// Default constructor. Creates an invalid (uninitialized) object.
        TCursorBase()                                                       noexcept
        : node( nullptr )
        , tree( nullptr )                                                                         {}

        /// Trivial default copy constructor.
        TCursorBase( const TCursorBase&  )                                        noexcept =default;

        /// Trivial default move constructor.
        TCursorBase(       TCursorBase&& )                                        noexcept =default;

        /// Trivial default copy assign operator.
        /// @return A reference to \c this.
        TCursorBase& operator=( const TCursorBase&  )                             noexcept =default;

        /// Trivial default move assign operator.
        /// @return A reference to \c this.
        TCursorBase& operator=(       TCursorBase&& )                             noexcept =default;

        /// Trivial default destructor.
        ~TCursorBase()                                                            noexcept =default;


        /// Finds a child node along the \p{path} given, but does not create new nodes.
        /// Incomplete results may occur if a child along the path was not found.
        /// In this case, parameter \p{path} contains the remaining path, excluding a leading
        /// separator.
        ///
        /// A leading slash (aka \p{TSeparator}) allows absolute path addressing, which means
        /// the root of \p{node} is searched if a leading separator is found.
        ///
        /// Besides normal child names, this method accepts
        /// - multiple separator characters (ignored)
        /// - child name "." (ignored)
        /// - child name ".." for parent node
        ///
        /// @param[in,out] path        Creation path. Provided as reference and consumed as far
        ///                            as the path exits.
        /// @return The node found
        cmNode* followPath( SubstringType& path )                                            const {
            cmNodeBase* actNode= node;

            // check for "root addressing"
            if( path.CharAtStart() == tree->separator ) {
                path.ConsumeChars( 1 );
                while( actNode->parent != nullptr )
                    actNode= actNode->parent;
            }

            // loop over node names in path
            for(;;) {
                // multiple separators are ignored
                while(path.ConsumeChar( tree->separator ) )
                    ;

                if( path.IsEmpty() )
                    return static_cast<cmNode*>(actNode);


                NameType name=path.template Substring<NC>(0,  path.IndexOfOrLength(tree->separator));


                if( name.Length() == 2 && name[0] == '.' &&  name[1] == '.' ) {
                    // we move up only if possible. If not, the ".." is just ignored (consumed)
                    // (This behavior was just more helpful in the use cases so far)
                    if( actNode->parent != nullptr )
                        actNode= actNode->parent;
                }

                else if( name.Length() != 1 || name[0] != '.' ) {
                    cmNodeBase* child= actNode->findChild( tree, name );
                    if( child == nullptr )
                        return static_cast<cmNode*>(actNode);

                    actNode= child;
                }

                path.ConsumeChars( name.Length() );
        }   }

        /// Follows the given path and creates non-existing children along the way.
        ///
        /// Child names <c>"."</c> and <c>".."</c> are allowed and respected same
        /// as in #followPath.
        ///
        /// New child nodes are constructed by forwarding the given \p{args}. Existing children
        /// remain untouched.
        ///
        /// \note This method is only available if the template parameter \p{TConst} of this
        ///       type is \c false.
        /// @tparam TRequires Defaulted template parameter. Must not be specified.
        /// @tparam TArgs      Types of variadic parameters given with parameter \p{args}.
        /// @param  path       The path to move along.
        /// @param  args       Variadic parameters to be forwarded to the constructor of each node
        ///                    that is created.
        /// @return A <c>std::pair</c> containing a resulting \b Node* and the number of nodes
        ///         created.
        template<typename... TArgs, bool TRequires= !TConst >
        requires TRequires
        std::pair<cmNodeBase*, integer>
        followPathCreate( const NameType& path, TArgs&&... args  ) {
            std::pair<cmNodeBase*, integer> result= std::make_pair( node, 0 );
            cmNodeBase*&  actNode= result.first; // a local alias name, let the compiler decide

            SubstringType rest=  path;

            // check for "root addressing"
            if( rest.CharAtStart() == tree->separator ) {
                rest.ConsumeChars( 1 );
                while( actNode->parent != nullptr )
                    actNode= actNode->parent;
            }

            // loop over path string
            for(;;) {
                // consume '/' and check for emptiness
                while(rest.ConsumeChar( tree->separator ) )
                ;
                if( rest.IsEmpty() )
                    return result;

                NameType childName= rest.template Substring<NC>( 0,
                                                    rest.IndexOfOrLength( tree->separator ) );

                // "." or ".."?
         ALIB_WARNINGS_UNINITIALIZED_OFF
                if( childName[0] == '.' )
         ALIB_WARNINGS_RESTORE
                {
                    if( childName.Length() == 1 )
                        continue;
                    if(     childName.Length() == 2 && childName[1] != '.' ) {
                        if ( !actNode->isRoot() )
                            actNode= actNode->parent;
                        continue;
                }   }

                auto childCreation= actNode->findOrCreateChild( tree, childName,
                                                                std::forward<TArgs>(args)... );

                if( childCreation.second )
                  ++result.second;

                actNode= childCreation.first;
                rest.ConsumeChars( childName.Length() + 1);
        }   }
    };  // inner class TCursorBase

    /// The mutable version of type \alib{containers::detail;StringTreeBase::TCursorBase<TConst>}.
    using CursorBase       = TCursorBase<false>;

    /// The constant version of type \alib{containers::detail;StringTreeBase::TCursorBase<TConst>}.
    using ConstCursorBase  = TCursorBase<true>;

  //################################################################################################
  // StringTreeBase interface
  //################################################################################################
    /// Constructor.
    /// @param  allocator     The monotonic allocator to use.
    /// @param  pathSeparator The separation character used with path strings.
    StringTreeBase( TAllocator& allocator, CharacterType  pathSeparator )
    : separator( pathSeparator )
    , nodeTable( allocator )                                                                      {}

    /// Constructor taking a shared recycler.
    /// @param  allocator     The monotonic allocator to use.
    /// @param  pRecycler     The shared recycler.
    /// @param  pathSeparator The separation character used with path strings.
    template<typename TSharedRecycler= SharedRecyclerType>
    requires ( !std::same_as<TSharedRecycler , void> )
    StringTreeBase( TAllocator& allocator, TSharedRecycler& pRecycler, CharacterType  pathSeparator )
    : separator( pathSeparator )
    , nodeTable( allocator, pRecycler )                                                           {}

    /// Constructor taking a shared recycler.
    /// @param  pRecycler     The shared recycler.
    /// @param  pathSeparator The separation character used with path strings.
    /// @tparam TSharedRecycler  Used to select this constructor. Deduced by the compiler.
    template<typename TSharedRecycler= SharedRecyclerType>
    requires (!std::same_as<TSharedRecycler, void>)
    StringTreeBase( TSharedRecycler& pRecycler, CharacterType  pathSeparator )
    : separator( pathSeparator )
    , nodeTable( pRecycler )                                                                      {}

    /// @return Returns the allocator received with construction.
    TAllocator&  GetAllocator()                        noexcept { return nodeTable.GetAllocator(); }

    /// Simple helper method which checks a node name for not being <c>"."</c> or <c>".."</c>
    /// and for not containing a separator character.
    /// In debug-compilations, if it does, a \ref alib_mod_assert "warning is raised".
    ///
    /// @param name  The child name to check.
    /// @return \c true if the name is legal, false otherwise.
    bool checkChildName( const NameType& name )                                              const {
        if(    name.IsEmpty()
            || (       name[0] == '.'
                  && ( name.Length() == 1 || (    name[1] == '.'
                                               && name.Length() == 2 ) )   )
            || name.IndexOf( separator) >=0 )
        {
            ALIB_WARNING( "STRINGTREE", "Illegal child name \"{}\".", name )
            return false;
        }
        return true;
    }

}; // StringTreeBase


} // namespace [alib::containers::detail]

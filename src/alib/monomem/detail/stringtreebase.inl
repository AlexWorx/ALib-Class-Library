/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_monomem of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_MONOMEM_DETAIL_STRINGTREEBASE
#define HPP_ALIB_MONOMEM_DETAIL_STRINGTREEBASE 1

#if !defined(HPP_ALIB_MONOMEM_STRINGTREE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

#if !defined(HPP_ALIB_MONOMEM_HASHTABLE)
#   include "alib/monomem/hashtable.hpp"
#endif

#if !defined(HPP_ALIB_FS_LISTS_BIDILIST)
#   include "alib/lib/fs_lists/bidilist.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

#if ALIB_RESULTS
#   if !defined(HPP_ALIB_RESULTS_REPORT)
#      include "alib/results/report.hpp"
#   endif
#else
#   if !defined(HPP_ALIB_FS_DEBUG_ASSERT)
#      include "alib/lib/fs_debug/assert.hpp"
#   endif
#endif

#if !defined (_GLIBCXX_VECTOR) && !defined(_VECTOR_)
#   include <vector>
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
#   include <algorithm>
#endif

namespace aworx { namespace lib { namespace monomem {

namespace detail {

/**
 * Base struct of \alib{monomem,StringTree} providing internals.
 * \note
 *   The separation of the internals of class \b StringTree to this type in namespace \b detail
 *   has no benefit on compilation speed or other positive "technical" effect, nor is it a matter
 *   of software design.<br>
 *   A user of derived class \alib{monomem,HashTable} finds all interface methods and types
 *   in one place, which is not cluttered by the documentation of the internals found here.
 *   Otherwise, the separation is exclusively supporting source code organization.
 *
 * @tparam T               See
 *                         \ref alib_namespace_monomem_stringtree_referencedoc "template parameters"
 *                         of derived public type.
 * @tparam TNodeMaintainer See
 *                         \ref alib_namespace_monomem_stringtree_referencedoc "template parameters"
 *                         of derived public type.
 * @tparam TRecycling      See
 *                         \ref alib_namespace_monomem_stringtree_referencedoc "template parameters"
 *                         of derived public type.
 */
template<typename  T, typename TNodeMaintainer, typename TRecycling>
struct StringTreeBase
{
    // #############################################################################################
    // Inner types
    // #############################################################################################
    struct Node; // forward declaration

    /** Alias shortcut for a bidirectional list of \b Node elements.  */
    using NodeList     = BidiList<Node>;

    /** Alias shortcut for an iterator of #NodeList.  */
    using NodeIterator = BidiListIterator<Node>;

    /** The string type of node names and paths.  */
    using ValueType    = typename strings::TString<typename TNodeMaintainer::CharacterType>;

    /** The character type of node names and paths strings. This is defined using character type
     *  definition \alib{monomem::StringTreeNamesDynamic,CharacterType} of template type
     *  \p{TNodeMaintainer}. */
    using CharacterType= typename TNodeMaintainer::CharacterType;

    /** The string type of node names and paths. This is defined by
     *  \alib{monomem::StringTreeNamesDynamic,NameStringType} of template type \p{TNodeMaintainer}. */
    using NameType     = typename TNodeMaintainer::NameStringType;

    /** The sub-string type of paths. This is defined using character type definition
     *  \alib{monomem::StringTreeNamesDynamic,CharacterType} of template type \p{TNodeMaintainer}. */
    using SubstringType= typename strings::TSubstring<CharacterType>;

    /**
     * The unique key to any element stored in this container.
     * By being a (second) base type for of type \alib{monomem::detail::StringTreeBase,Node}, any
     * node includes this key.
     */
    struct NodeKey
    {
        /** The parent node. A value of \c nullptr indicates that this is the root node of
         *  the tree, which is always existing. */
        Node*       parent;

        /**
         * A string object containing the pointer to this node's name.
         * Node names constitute path strings and, together with the pointer to their parent, form
         * the key of the hash set found with field \alib{monomem::detail::StringTreeBase,nodeTable}.
         * <br>
         * Node names must not contain the separator character and must not equal to <c>"."</c> or
         * <c>".."</c>.
         *
         * The name of the root node is nulled.
         */
        NameType    name;

        /**
         * Constructor
         * @param pParent  Parent node to search a child for.
         * @param pName    Child name to search
         */
        NodeKey( Node* pParent, const NameType& pName )
        : parent(pParent)
        , name(pName)
        {}

        /** Hash functor for nodes hashed in field #nodeTable. */
        struct Hash
        {
            /**
             * Calculates a hash code for \b NodeKey objects.
             * @param key The key to hash.
             * @return The hash code.
             */
            std::size_t operator()(const NodeKey& key)                                         const
            {
                std::size_t result= 8175383ul;

                for (aworx::integer i = 0; i < key.name.Length(); ++i)
                    result =   31 * result
                             + static_cast<std::size_t>( key.name.template CharAt<false>(i) );

                // add parent
                result+= reinterpret_cast<std::size_t>(key.parent) * 29;

                return static_cast<std::size_t>( result );
            }
        };

        /** Equality functor for nodes in field #nodeTable. */
        struct EqualTo
        {
            /**
             * Invokes \alib{strings,TString::Equals,String::Equals} on \p{lhs}, passing \p{rhs}
             * and returns the result.
             * @param lhs The first string object.
             * @param rhs The second string object.
             * @return The result of the string comparison.
             */
            bool operator()(const NodeKey& lhs,  const NodeKey& rhs )                          const
            {
                return     lhs.parent == rhs.parent
                        && lhs.name.Equals( rhs.name );
            }
        };

        /** Key access method used by hash table #nodeTable. */
        struct Access
        {
            /**
             * Returns the given \p{src} statically casted to type
             * \alib{monomem::detail::StringTreeBase,NodeKey}.
             * @param src The node to hash.
             * @return The key-portion of the stored value.
             */
            NodeKey&       Key(Node& src)                                                      const
            {
                return src;
            }
        };
    };

    /**
     * This is the "final" internal node type, which is created and stored for every node of the
     * tree. It adds field #data of template type \p{T} to store custom container data.
     *
     * Objects of this type can not be received directly and all interface is available
     * via public type \alib{monomem,StringTree::NodePtr} only, which holds a pointer to
     * an object of this class.
     *
     * Note that the templated allocator type \b TAllocator is declared as a friend of this
     * class to be able to access the otherwise protected constructor.
     */
    struct Node : public BidiNode<Node>,
                  public NodeKey
    {
        /** The number of children currently stored in this node. */
        uinteger    qtyChildren;

        /** The hook to the doubly linked list of children. */
        NodeList    children;

        /** The templated custom data object stored with each node. */
        T           data;

        /** Deleted copy constructor. */
        Node( const Node&  )                                                               = delete;

        /** Deleted move constructor. */
        Node(       Node&& )                                                               = delete;

        /**
         * Constructor. Custom data is default-initialized.
         * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
         * @param  pKey   The key portion of the node.
         * @param  args   Variadic parameters to be forwarded to constructor of custom type \p{T}.
         */
        template<typename... TArgs>
        Node( const NodeKey& pKey, TArgs&&... args  )
        : NodeKey    ( pKey )
        , qtyChildren( 0 )
        , data       ( std::forward<TArgs>(args)... )
        {}

        /**
         * Constructor. Custom data is default-initialized.
         * @tparam TArgs   Types of variadic parameters given with parameter \p{args}.
         * @param  pParent Parent node to search a child for.
         * @param  pName   Child name to search
         * @param  args    Variadic parameters to be forwarded to constructor of custom type \p{T}.
         */
        template<typename... TArgs>
        Node( Node* pParent, const NameType& pName, TArgs&&... args  )
        : NodeKey    ( pParent, pName )
        , qtyChildren( 0 )
        , data       ( std::forward<TArgs>(args)... )
        {}



        /**
         * Returns \c true if this is the root node, \c false otherwise.
         * @return \c true if this is the root node, \c false otherwise.
         */
        bool isRoot()                                                                          const
        {
            return NodeKey::parent == nullptr;
        }

        /**
         * Iterates over the parent nodes to the root node and returns the this node's depth.
         * @return The depth of the this node.
         */
        int depth()                                                                            const
        {
            int result= -1;
            const Node* p= this;
            while( p != nullptr )
            {
                ++result;
                p= p->parent;
            }
            return result;
        }

        /**
         * Searches a child with a given name.
         * The name is not checked for <c>.</c>, <c>..</c> or if separation characters.
         *
         * @param tree      The tree this node belongs to.
         * @param childName The name of the child to search.
         * @return The child or \c nullptr if not found.
         */
        Node*  findChild( StringTreeBase* tree, const NameType& childName )
        {
            if( qtyChildren == 0  )
                return nullptr;

            // With a small number of children, the search is faster if we iterate, because
            // a) no hash value has to be calculated and
            // b) the string compare is fast, at least if string have different length or are
            //    different at the beginning.
            // A good value is probably five children. With bigger numbers, it soon becomes better
            // to calculate the hash value. While in the bucket also children of other nodes
            // are found, each entry of the hashtable is first compared against the full hash code.
            if( qtyChildren <= 5 )
            {
                for( auto& childIt : children )
                    if( childIt.name.Equals( childName ) )
                        return &childIt;
                return nullptr;
            }

            // search in hash table
            auto   childIt=   tree->nodeTable.Find( NodeKey( this, childName ) );
            return childIt != tree->nodeTable.end() ?  &childIt.Value()
                                                    :  nullptr;
        }

        /**
         * Searches a child with a given name, if not found, one is created.
         * The name is not checked for <c>.</c>, <c>..</c> or if separation characters.
         *
         * @tparam TArgs     Types of variadic parameters given with parameter \p{args}.
         * @param  tree      The tree this node belongs to.
         * @param  childName The name of the child to search.
         * @param  args      Variadic parameters to be forwarded to constructor of custom type
         *                   \p{T} in the case a child is created.
         * @return A pair containing an iterator referencing either the element found or the new
         *         element added.
         *         The bool component is \c true if the insertion took place and \c false nothing
         *         was changed.
         */
        template<typename... TArgs>
        std::pair<Node*,bool>  findOrCreateChild( StringTreeBase* tree,
                                                  const NameType& childName,  TArgs&&... args )
        {
            auto childCreation= tree->nodeTable.EmplaceIfNotExistent( NodeKey(this, childName),
                                                                      std::forward<TArgs>(args)...);
            Node* child= &childCreation.first.Value();

            if( childCreation.second )
            {
                TNodeMaintainer::InitializeNode( *tree, *child );
                children.pushEnd( child );
                ++qtyChildren;
            }

            return std::make_pair( child, childCreation.second );
        }


        /**
         * Deletes a given child node.
         * \note
         *   If the given node is not a child of this node, the behavior is undefined.
         *   With debug builds, in this case an \alib assertion is raised.
         *
         * @param tree         The tree this node belongs to.
         * @param child        A pointer to a child of this node that is to be deleted.
         * @return The total number of nodes deleted.
         */
        uinteger  deleteChild( StringTreeBase* tree, NodeIterator child )
        {
            ALIB_ASSERT_ERROR(qtyChildren   >  0   , "This node has no children to remove")
            ALIB_ASSERT_ERROR(child->parent == this, "The given node is not a child of this node.")

            --qtyChildren;
            child->remove(); // remove from linked list
            auto count= child->deleteChildren( tree );
            auto handle= tree->nodeTable.Extract( *child );
            ALIB_ASSERT( !handle.IsEmpty() )
            TNodeMaintainer::FreeNode( *tree, handle.Value() );

            return count + 1;
        }

        /**
         * Deletes all child nodes.
         * @param tree           The tree this node belongs to.
         * @return The number of children that were deleted.
         */
        uinteger deleteChildren( StringTreeBase* tree )
        {
            if( children.isEmpty() )
                return 0;

            uinteger     count= qtyChildren;
            for( auto& child : children )
            {
                count+=  child.deleteChildren( tree ); // recursion

                auto handle= tree->nodeTable.Extract( child );    ALIB_ASSERT( !handle.IsEmpty() )
                TNodeMaintainer::FreeNode( *tree, handle.Value() );
            }

            children.reset();
            qtyChildren= 0;

            return count;
        }


        /**
         * Implementation of \alib{monomem,StringTree::NodePtr::AssemblePath}.
         *
         * @param target      The target to append the path to.
         * @param separatorChar   The separator character as defined with the template parameter of
         *                    class \alib{monomem,StringTree}.
         * @return The given \b AString to allow concatenated operations on it.
         */
        AString&    assemblePath( AString& target, character separatorChar )                   const
        {
            auto pos= target.Length();
            const Node* p= this;
            while( p->parent != nullptr )
            {
                if( pos != target.Length() ) // first iteration?
                    target.InsertChars( separatorChar, 1, pos);
                target.InsertAt( p->name, pos );
                p= p->parent;
            }

            return target;
        }

    }; // inner class Node


    // #############################################################################################
    // StringTreeBase: members
    // #############################################################################################

    /** The root node. */
    Node           root;

    /**
     * The separator character to use with path strings.
     * This is set once with construction.
     */
    CharacterType  separator;

    /**
     * The child node hash set.
     * This is used to find children of nodes by name.
     * The set contains all children of all nodes.
     */
    monomem::HashTable<Node, Node,                 // Value type, stored version of value
                       NodeKey,                    // Key type, contained in value type
                       void,                       // This is a hash set, therefor no mapping
                       typename NodeKey::Hash,
                       typename NodeKey::EqualTo,
                       typename NodeKey::Access,
                       Caching::Enabled,
                       TRecycling                 > nodeTable;

    /** The recycler type. See \alib{monomem,StringTree::TSharedRecycler} which exposes this
     *  type publically.                                                                     */
    using TSharedRecycler=  typename decltype(nodeTable)::TSharedRecycler;


    // #############################################################################################
    // class NodePtrBase
    // #############################################################################################
    /**
     * Non-templated base class of \alib{monomem,StringTree::NodePtr}.
     */
    struct NodePtrBase
    {
        /** The \b %StringTree this object refers to. */
        StringTreeBase*     tree;

        /** The currently represented node of the #tree. */
        Node*               node;

        /**
         * Constructor initializing both fields #tree and #node.
         * @param pTree  The \b %StringTree we work on.
         * @param pNode  The node to refer to.
         */
        NodePtrBase( StringTreeBase* pTree, Node* pNode)                    noexcept
        : tree( pTree )
        , node( pNode )
        {}

        /** Default constructor. Creates an invalid (uninitialized) object. */
        NodePtrBase()                                                       noexcept
        : tree( nullptr )
        , node( nullptr )
        {}

        /** Trivial default copy constructor. */
        NodePtrBase( const NodePtrBase&  )                                  noexcept      = default;

        /** Trivial default move constructor. */
        NodePtrBase(       NodePtrBase&& )                                  noexcept      = default;

        /** Trivial default copy assign operator.
          * @return A reference to \c this.        */
        NodePtrBase& operator=( const NodePtrBase&  )                       noexcept      = default;

        /** Trivial default move assign operator.
          * @return A reference to \c this.        */
        NodePtrBase& operator=(       NodePtrBase&& )                       noexcept      = default;

        /** Trivial default destructor.            */
        ~NodePtrBase()                                                      noexcept      = default;


        /**
         * Finds a child node along the \p{path} given, but does not create new nodes.
         * Incomplete results may occur if a child along the path was not found.
         * In this case, parameter \p{path} contains the remaining path, excluding a leading
         * separator.
         *
         * A leading slash (aka \p{TSeparator}) allows absolute path addressing, which means
         * the root of \p{node} is searched if a leading separator is found.
         *
         * Besides normal child names, this method accepts
         * - multiple separator characters (ignored)
         * - child name "." (ignored)
         * - child name ".." for parent node
         *
         * @param[in,out] path        Creation path. Provided as reference and consumed as far
         *                            as the path exits.
         * @return The node found
         */
        Node* followPath( SubstringType& path )
        {
            Node* actNode= node;

            // check for "root addressing"
            if( path.CharAtStart() == tree->separator )
            {
                path.ConsumeChars( 1 );
                while( actNode->parent != nullptr )
                    actNode= actNode->parent;
            }

            // loop over node names in path
            for(;;)
            {
                // multiple separators are ignored
                while(path.ConsumeChar( tree->separator ) )
                    ;

                if( path.IsEmpty() )
                    return actNode;


                NameType name=path.template Substring<false>(0,
                                                             path.IndexOfOrLength(tree->separator));


                if( name.Length() == 2 && name[0] == '.' &&  name[1] == '.' )
                {
                    // we move up only if possible. If not, the ".." is just ignored (consumed)
                    // (This behavior was just more helpful in the use cases so far)
                    if( actNode->parent != nullptr )
                        actNode= actNode->parent;
                }

                else if( name.Length() != 1 || name[0] != '.' )
                {
                    Node* child= actNode->findChild( tree, name );
                    if( child == nullptr )
                        return actNode;

                    actNode= child;
                }

                path.ConsumeChars( name.Length() );
            }
        }

            /**
             * Follows the given path and creates non-existing children along the way.
             *
             * Child names <c>"."</c> and <c>".."</c> are allowed and respected same
             * as in #followPath.
             *
             * New child nodes are constructed by forwarding the given \p{args}. Existing children
             * remain untouched.
             *
             * @tparam TArgs  Types of variadic parameters given with parameter \p{args}.
             * @param  path   The path to move along.
             * @param  args   Variadic parameters to be forwarded to constructor of each node
             *                that is created.
             * @return A <c>std::pair</c> containing a resulting \b Node* and the number of nodes
             *         created.
             */
            template<typename... TArgs>
            std::pair<Node*, integer>  followPathCreate( const NameType& path, TArgs&&... args  )
            {
                std::pair<Node*, integer> result= std::make_pair( node, 0 );
                Node*&  actNode= result.first; // a local alias name, let the compiler decide

                SubstringType rest=  path;

                // check for "root addressing"
                if( rest.CharAtStart() == tree->separator )
                {
                    rest.ConsumeChars( 1 );
                    while( actNode->parent != nullptr )
                        actNode= actNode->parent;
                }

                // loop over path string
                for(;;)
                {
                    // consume '/' and check for emptyness
                    while(rest.ConsumeChar( tree->separator ) )
                    ;
                    if( rest.IsEmpty() )
                        return result;

                    NameType childName= rest.template Substring<false>( 0,
                                                          rest.IndexOfOrLength( tree->separator ) );

                    // "." or ".."?
             ALIB_WARNINGS_UNINITIALIZED_OFF
                    if( childName[0] == '.' )
             ALIB_WARNINGS_RESTORE
                    {
                        if( childName.Length() == 1 )
                            continue;
                        if(     childName.Length() == 2 && childName[1] != '.' )
                        {
                            if ( !actNode->isRoot() )
                                actNode= actNode->parent;
                            continue;
                        }
                    }

                    auto childCreation= actNode->findOrCreateChild( tree, childName,
                                                                    std::forward<TArgs>(args)... );

                    if( childCreation.second )
                      ++result.second;

                    actNode= childCreation.first;
                    rest.ConsumeChars( childName.Length() + 1);
                }
            }
    };  // inner class NodePtrBase


    // #############################################################################################
    // StringTreeBase interface
    // #############################################################################################
    /** ********************************************************************************************
     * Constructor.
     * @tparam TArgs         Types of variadic parameters given with parameter \p{args}.
     * @param  allocator     The monotonic allocator to use.
     * @param  pathSeparator The separation character used with path strings.
     * @param  args          Variadic parameters to be forwarded to constructor of custom type
     *                       \p{T} of this tree's root node.
     **********************************************************************************************/
    template<typename... TArgs>
    StringTreeBase( MonoAllocator* allocator    ,
                    CharacterType  pathSeparator, TArgs&&...       args        )
    : root     ( nullptr, nullptr, std::forward<TArgs>(args)... )
    , separator( pathSeparator )
    , nodeTable( allocator )
    {}

    /** ********************************************************************************************
     * Constructor taking a shared recycler.
     * @tparam TArgs         Types of variadic parameters given with parameter \p{args}.
     * @param  allocator     The monotonic allocator to use.
     * @param  pRecycler     The shared recycler.
     * @param  pathSeparator The separation character used with path strings.
     * @param  args          Variadic parameters to be forwarded to constructor of custom type
     *                       \p{T} of this tree's root node.
     **********************************************************************************************/
    template<typename... TArgs>
    StringTreeBase( MonoAllocator* allocator    , TSharedRecycler& pRecycler,
                    CharacterType  pathSeparator, TArgs&&...       args        )
    : root     ( nullptr, nullptr, std::forward<TArgs>(args)... )
    , separator( pathSeparator )
    , nodeTable( allocator, pRecycler )
    {}

    /**
     * Simple helper method which checks a node name for not being <c>"."</c> or <c>".."</c>
     * and for not containing a separator character.
     * In debug-compilations, if it does, an \alib warning is raised.
     *
     * @param name  The child name to check.
     * @return \c true if the name is legal, false otherwise.
     */
    bool checkChildName( const NameType& name )
    {
        if(    name.IsEmpty()
            || (       name[0] == '.'
                  && ( name.Length() == 1 || (    name[1] == '.'
                                               && name.Length() == 2 ) )   )
            || name.IndexOf( separator) >=0 )
        {
            #if ALIB_RESULTS
                ALIB_WARNING( "Illegal child name: ", name )
            #else
                ALIB_WARNING( "Illegal child name" )
            #endif

            return false;
        }
        return true;
    }

}; // StringTreeBase


}}}}// namespace [aworx::lib::monomem::detail]


#endif // HPP_ALIB_MONOMEM_DETAIL_STRINGTREEBASE

// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CONTAINERS_STRINGTREE)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CONTAINERS_STRINGTREE
//! @cond NO_DOX
#define HPP_ALIB_CONTAINERS_STRINGTREE 1
//! @endcond



// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_UTIL_STRINGMAP)
    #include "alib/strings/util/stringmap.hpp"
#endif

#if !defined (_GLIBCXX_ALGORITHM) && !defined(_ALGORITHM_)
    #include <algorithm>
#endif

#if !defined (_GLIBCXX_STACK) && !defined(_STACK_)
    #include <stack>
#endif


namespace aworx { namespace lib {

/** ************************************************************************************************
 * This \alib namespace holds rather simple utility classes which on the one hand do not
 * 'deserve' to be placed in a separated own namespace and on the other hand are not close enough
 * to the C++ language to be placed in namespace \ref aworx::lib::lang.
 **************************************************************************************************/
namespace util {


/** ************************************************************************************************
 * This class manages an object of internal type
 * \ref aworx::lib::util::StringTree::Node "Node", which consists of
 * - A value of custom (template) type \p{T},
 * - A pointer to a parent \b %Node and
 * - A hash table which recursively stores pointers to \b Node objects, referred to by
 *   a string-type key.
 *
 * This recursive data structure is what comprises a graph with named edges. As this object does
 * not allow insertion of nodes with circular dependencies, the graph is of tree type.
 * The way from the root node to the leafs usually is called "path" and the class incorporates
 * functionality to work with string representations of such paths where names of edges are
 * concatenated and separated by a special separation character.
 * Now, this explains why this class is called \b %StringTree.
 *
 * The search and creation of tree nodes by using aforementioned path strings, is very similar to
 * what is well known from addressing files and folders in file systems.
 * The difference is however, that this class does not differentiate between 'folders' and 'files',
 * hence between 'nodes' and 'leafs'. Every node has the same value type \p{T} and may or may not
 * have child nodes.
 * If such differentiation - or other semantics - is wanted, this may be modeled in custom
 * attributes provided in template type \p{T}.
 *
 * The internal node structure is not exposed publically and the class has no direct interface to
 * manipulate nodes. Instead, the interface is defined by three public inner types.
 * Those are:
 * - \ref aworx::lib::util::StringTree::Cursor       "Cursor",
 * - \ref aworx::lib::util::StringTree::StdIterator  "StdIterator" (C++ version only) and
 * - \ref aworx::lib::util::StringTree::Walker       "Walker".
 *
 * and are explained in the following paragraphs.
 *
 * \anchor cpp_alib_containers_stringtree_cursor
 * <b>Inner class %Cursor: Inserting, Retrieving And Deleting Nodes</b><br>
 * The class has no direct interface to manipulate nodes. The main interface into
 * objects of this type is defined by public, inner type
 * \ref aworx::lib::util::StringTree::Cursor "Cursor". The only way to create an
 * initial cursor is with method #Root, which creates a cursor object referring to the
 * root node of the tree. With this, string names and complete paths can be used to
 * move the cursor along existing nodes of the tree or to create new child nodes or a whole
 * path of children at once.
 * Class cursor is quite lightweight as it contains just two pointers (to the \b %StringTree and the current node).
 * Hence, cursors can be cloned, assigned and passed around very efficiently.
 *
 * Once a cursor is created it can traverse over the tree nodes using methods
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToChild        "Cursor::MoveToChild",
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToParent       "Cursor::MoveToParent",
 * - \ref aworx::lib::util::StringTree::Cursor::MoveTo             "Cursor::MoveTo" and
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToExistingPart "Cursor::MoveToExistingPart".
 *
 * For the creation of new child nodes or a complete path of such, methods
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToChildCreate              "Cursor::MoveToChildCreate" and
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToAndCreateNonExistingPart "Cursor::MoveToAndCreateNonExistingPart"
 *
 * are provided. Then, two methods for erasing nodes exist:
 * - \ref aworx::lib::util::StringTree::Cursor::DeleteChild    "Cursor::DeleteChild" and
 * - \ref aworx::lib::util::StringTree::Cursor::DeleteChildren "Cursor::DeleteChildren"
 *
 * Finally, class cursor can also be used like an iterator. But it explicitly and for good reason
 * does \b not follow the concepts provided with <c>C++ std::iterator</c> types. The iterating
 * feature comes with methods
 * - \ref aworx::lib::util::StringTree::Cursor::MoveToParentUnchecked "Cursor::MoveToParentUnchecked" and
 * - \ref aworx::lib::util::StringTree::Cursor::IsValid               "Cursor::IsValid".
 *
 * The first transitions the cursor to refer to the parent node. Using this method is (almost)
 * the only way how a cursor can get invalidated. Hence a simple loop can be written that visits
 * all nodes from a starting node, up to the root of the tree until \b %Cursor::IsValid evaluates
 * to \c false.
 *
 * <b>Inner class iterator: A std::iterator implementation</b><br>
 * A second inner class is provided with
 * \ref aworx::lib::util::StringTree::StdIterator "StdIterator". It implements the concept
 * [C++ InputIterator](http://en.cppreference.com/w/cpp/concept/InputIterator) and due to the
 * implementation of methods
 * - \ref aworx::lib::util::StringTree::begin "StringTree::begin" and
 * - \ref aworx::lib::util::StringTree::end   "StringTree::end",
 *
 * this type can be used together with <em>C++ 11 ranged-for
 * loops</em> as well as \c std::for_each. And these loops indicate already the typical use of
 * class \b %StdIterator: The implementation of algorithms that need to visit all nodes of a tree or a
 * branch in an depth-first order, with unsorted child order.
 *
 * With methods
 * - \ref aworx::lib::util::StringTree::Cursor::begin "Cursor::begin" and
 * - \ref aworx::lib::util::StringTree::Cursor::end   "Cursor::end",
 *
 * StdIterator objects may be initialized using a different start node than the root, which
 * is chosen with the direct \b %StringTree interface.
 * Existing \b %StdIterator object can also be reused: They can be freshly initialized with methods
 *
 * - \ref aworx::lib::util::StringTree::Cursor::SetIterator   "Cursor::SetIterator" and
 * - \ref aworx::lib::util::StringTree::Walker::SetIterator   "Walker::SetIterator".
 *
 * Please also note the paragraph about recursive iteration below.
 *
 * Class \ref aworx::lib::util::StringTree::StdIterator "StdIterator" is quite lightweight.
 * Nevertheless, if frequently used, it might be advisable
 * to reuse an exiting \b %StdIterator, because internal buffers are allocated then only once. Of-course,
 * re-using an instance is a showstopper for convenient <em>C++ 11 ranged-for loops</em>.
 *
 * <b>Inner class %Walker: Sorted Iterations</b><br>
 * Finally, with
 * \ref aworx::lib::util::StringTree::Walker "Walker", a third \b %StdIterator class is exposed.
 * The rational for having this class is to provide a configurable and controlled way of iterating
 * a tree or a branch. Some features of the class are:
 * - Can be created from (or reset to) the \b %StringTree itself,
 *   \ref aworx::lib::util::StringTree::Cursor   "Cursor"   objects and from
 *   \ref aworx::lib::util::StringTree::StdIterator "StdIterator" objects.
 * - Can work recursively (with depth limitation) or just on the current nodes' direct children.
 * - Has several sort options:
 *   - built-in by path name, ascending/descending, case sensitive or ignoring
 *   - user defined by path name
 *   - user defined by template type
 *
 * The class is rather 'heavy' and recursive iteration needs to allocate memory for a sorted vector
 * of child nodes.
 * Therefore it is recommended to reuse instances of the class with subsequent, similar iterations.
 * In addition this explains why this class does not follow the concept of <c>std::iterator</c>,
 * same as class
 * \ref aworx::lib::util::StringTree::Cursor "Cursor" does not - but then unlike class
 * \ref aworx::lib::util::StringTree::StdIterator "StdIterator" which does!
 *
 * \anchor cpp_alib_containers_stringtree_iterator
 * <b>Recursive Iteration</b><br>
 *
 * Both inner types that provide depth-iteration, namely class
 * \ref aworx::lib::util::StringTree::StdIterator   "StdIterator" and class
 * \ref aworx::lib::util::StringTree::Walker     "Walker", allow recursive iteration. There are
 * two specialities to note on this topic which need explanation:
 *
 * 1. <b>Skipping the Initial %Node:</b><br>
 *    When the recursive iteration is limited, then the very first node (the root node of the
 *    iteration) is skipped. With unlimited recursion, the root node is included (as the first
 *    result). In the case, that the root node should not be included in an unlimited, recursive
 *    iteration, it has to be skipped 'manually'.
 * 2. <b>Default Recursion Depth:</b><br>
 *    There are different default values for the recursion limit, depending on the way a \b %StdIterator
 *    is initialized.
 *    For example, method \b %StringTree::begin() sets up an recursive \b %StdIterator, while
 *    \b %Cursor::begin limits the recursion to \c 1, hence to iterate only the direct child
 *    nodes. This default behavior was identified to be reasonable for many situations.<br>
 *    If different recursiveness is wanted, a parameter needs to be passed to \b %begin, which of
 *    course is then not applicable to nice and short <em>C++ 11 ranged-for loops</em>.
 *
 * <b>Iterators And Changes Of The Underlying Container</b><br>
 * As with container classes in namespace \c std, objects of iterator-like types of this class (i.e.
 * \ref aworx::lib::util::StringTree::Cursor "Cursor",
 * \ref aworx::lib::util::StringTree::StdIterator "StdIterator" and
 * \ref aworx::lib::util::StringTree::Walker "Walker") may become invalid when
 * the underlying \b %StringTree object changes and have to be re-initialized after such change.
 * I.e, there is no guarantee given that changes in the tree are not invalidating iterators that
 * represent 'higher' nodes or or 'sibling' branches. Such invalid state can not be detected. As
 * a result, all iterator like objects have to be reset or disposed on changes of the tree.
 *
 * TOODX: reflect changes. Better to re-read everything here, also the three allocator classes!
 *
 * <b>Key String Allocations:</b><br>
 * The C++ version of this class implements user-defined allocation of the characters sequences
 * stored as keys in the child hash tables of each node. For this, template parameter
 * \p{StringTreeDefaultAllocator} is given which defaults to struct
 * \ref aworx::lib::util::StringTreeDefaultAllocator "StringTreeDefaultAllocator", which
 * just uses new and delete to allocate and free the space.
 * This is done, because chances are good that
 * - either the key values provided in the child creation methods of class \b %Cursor are
 *   permanently valid or
 * - the templated value element \p{T} contains a "copy" of the key.
 *
 * If the first case applies, type
 * \ref aworx::lib::util::StringTreeConstKeyAllocator "StringTreeConstKeyAllocator" may be used,
 * which does not allocate anything and does not copy the key even.<br>
 * If the second case applies, a custom type may be provided which stores the key in \p{T} and
 * returns this copy to be used. Note that for this reason, class \b StringTree asserts that
 * node elements and with them custom objects of type \p{T} are deleted always \b after they have
 * been removed from the tree, so that the key memory can safely freed.
 *
 * @tparam T           The value type of elements stored in the nodes of the tree.
 * @tparam TAllocator  Type with allocation methods as defined in default struct
 *                     \alib{util,StringTreeDefaultAllocator}.
 * @tparam TSeparator  The separation character used with cursor paths.
 **************************************************************************************************/
template<typename T,
         typename TAllocator,
         char     TSeparator   = '/'
        >
class StringTree : public TAllocator
{
    public:
        class Walker;

    // #############################################################################################
    // Inner class Node
    // #############################################################################################
    public:
        /**
         * The internal, protected data structure implementing a node of the tree. This class is
         * not accessible from outside. Instead, inner class
         * \ref aworx::lib::util::StringTree::Cursor "Cursor", which represents node objects,
         * provides an indirect interface to the values of this class.
         */
        class Node
        {
            protected: // the whole class is protected and then opened to friends

            ///Parent class \b %StringTree is a friend.
            friend class StringTree;

            /// Sibling inner class \b %StringTree::Walker is a friend.
            friend class Walker;

            /// The allocator class is a friend as it is allowed to call our protected
            /// constructor/destructor.
            friend TAllocator;

            /// Parent node.
            Node*                       parent;

            /// Child nodes.
            UnorderedStringMap<Node*>   children;

            /// The template type value.
            T                           value;

            /**
             * Private (disabled) constructor.
             * Nodes are to be created using static method #createRoot and methods
             * #createChild and #createChildNoCheck.
             */
            Node()
            : value( nullptr )
            {}

            /** Private (disabled) destructor.
             *  Nodes are to be deleted using method #destruct. */
            ~Node()
            {
                ALIB_ASSERT( children.size() == 0 )
            }

            /**
             * While this class has a private destructor, this methods is to be used for
             * destruction. Deletes all child nodes and then this object.
             * @param allocator  The allocator used to create the name keys.
             * @return A pointer to the newly created child node.
             */
            static
            Node* createRoot( TAllocator& allocator )
            {
                Node* root= allocator.template AllocateNode<TAllocator>();
                root->parent= nullptr;
                return root;
            }

            /**
             * While this class has a private destructor, this methods is to be used for
             * destruction. Deletes all child nodes and then this object.
             * @param allocator  The allocator used to create the name keys.
             * @param key        The key that this node was stored with.
             */
            void destruct( TAllocator& allocator, const String& key )
            {
                deleteChildren( allocator );
                allocator.FreeKey( key, this->value );
                allocator.template FreeNode<TAllocator>(this);
            }

            /**
             * Non-checking version of #createChild. Must be used only if it is assured that
             * no child with that name exists, yet.
             *
             * @param allocator The allocator used to create the name key.
             * @param childName The name of the child to be created and inserted.
             * @return A pointer to the newly created child node.
             */
            Node* createChildNoCheck( TAllocator& allocator, const String& childName )
            {
                Node* child= allocator.template AllocateNode<TAllocator>();
                child->parent= this;

                ALIB_DBG( auto result= )
                children.emplace( allocator.AllocateKey( childName, child->value ), child );
                ALIB_ASSERT_ERROR( result.second, ASTR("Internal error. Replacing child node.") )
                return child;
            }

            /**
             * Creates and returns a child node. If a node already exists, nothing is done and
             * \c nullptr is returned as this is considered an error.
             *
             * @param allocator The allocator used to create the name key.
             * @param  childName The name of the child
             * @return Pointer to the newly created child in case of success. \c nullptr if a child
             *         with the given name already existed.
             */
            Node* createChild( TAllocator& allocator, const String& childName )
            {
                ALIB_ASSERT_ERROR( childName.Length() > 0 , ASTR("Can note create unnamed node"));

                if( getChild( childName ) != nullptr )
                    return nullptr;

                return createChildNoCheck( allocator, childName );
            }

            /**
             * Recursively calculates the depth of a node.
             * @return The depth of the this node.
             */
            int depth()
            {
                int result= -1;
                Node* p= this;
                while( p != nullptr )
                {
                    result++;
                    p= p->parent;
                }
                return result;
            }

            /**
             * Returns \c true if this is the root node, \c false otherwise.
             * @return \c true if this is the root node, \c false otherwise.
             */
            bool isRoot()
            {
                return parent == nullptr;
            }

            /**
             * Returns a child node identified by parameter \p{childName}.
             * If the child is not found or is illegal (empty, <c>"."</c> or <c>".."</c>
             * or contain a separator character), \c nullptr is returned.
             * In debug compilations, an \ref ALIB_WARNING is reported on illegal child names.
             *
             * @param  childName The name of the child.
             * @return The child node found, \c nullptr on failure.
             */
            Node* getChild( const String& childName )
            {
                if(    childName.IsEmpty()
                    || childName.Equals( ASTR(".") )
                    || childName.Equals( ASTR("..") )
                    || childName.IndexOf( TSeparator) >=0 )
                {
                    ALIB_WARNING( ASTR("Illegal child name {!Q}."), childName );
                    return nullptr;
                }
                auto it= children.find( childName );
                return it != children.end()   ? it->second
                                              : nullptr;
            }

            /**
             * Searches and - if no existent - creates a child node.<br>
             * If a child  needs to be created, \p{childName}  is first checked to not equal
             * to <c>"."</c> or <c>".."</c> and that it does not contain the separation character.
             * If it does, still \c true is returned and with \c nullptr for the new child.
             * Furthermore, an \ref ALIB_WARNING is reported.
             *
             * @param allocator The allocator used to create the name key.
             * @param  childName The name of the child
             * @return A pair of the child node found or created and a boolean which is \c true
             *         when the child was created and \c false when it was already existing.
             */
            std::pair<Node*, bool>  getOrCreateChild( TAllocator& allocator, const String& childName )
            {
                Node* child= getChild( childName );
                bool wasCreated= child==nullptr;
                if(  wasCreated )
                {
                    if(    childName.Equals( ASTR(".") )
                        || childName.Equals( ASTR("..") )
                        || childName.IndexOf( TSeparator) >=0 )
                    {
                        ALIB_WARNING( ASTR("Illegal child name {!Q}."), childName );
                    }
                    else
                        child= createChildNoCheck( allocator, childName );
                }
                return std::make_pair( child, wasCreated );
            }

            /**
             * Deletes named child node.
             * Key strings are deleted after the child is removed from the map using template
             * type TAllocator.
             * @param allocator  The allocator used to create the name key.
             * @param  childName The name of the child
             * @return \c true if the child was found and deleted, \c false otherwise.
             */
            bool  deleteChild( TAllocator& allocator, const String& childName )
            {
                auto it= children.find( childName );
                if( it == children.end() )
                    return false;

                // remove child and then delete
                String key=   (*it).first;
                Node*  child= (*it).second;
                children.erase( it );
                child->destruct( allocator, key );
                return true;
            }

            /**
             * Deletes all child nodes. It is assured that all children are removed
             * from the map before the node structs and the key strings are deleted.
             * Key strings are freed using template type TAllocator
             * @param allocator  The allocator used to create the name keys.
             */
            void deleteChildren(TAllocator& allocator)
            {
                Node*   childObjectsLocalStore[1024];
                String  childNamesLocalStore  [1024];
                Node**  childObjects;
                String* childNames;

                size_t qtyChildren= children.size();
                if( qtyChildren > 1024 )
                {
                    childNames=    new String[qtyChildren];
                    childObjects=  new Node* [qtyChildren];
                }
                else
                {
                    childObjects=  childObjectsLocalStore;
                    childNames=    childNamesLocalStore;
                }


                size_t i= 0;
                for (auto it : children )
                {
                    childNames  [i]= it.first;
                    childObjects[i]= it.second;
                    i++;
                }

                children.clear();

                for( i= 0; i < qtyChildren ; ++i )
                {
                    childObjects[i]->destruct(allocator, childNames[i] );
                }

                if( qtyChildren > 1024 )
                {
                    delete[] childObjects;
                    delete[] childNames;
                }
            }
        };


    // #############################################################################################
    // class StdIterator implements std::iterator
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * This inner class implements a \c std::iterator.
         * This \b %StdIterator is very simple compared to what class
         * \ref aworx::lib::util::StringTree::Walker "StringTree::Walker" provides. Hence it is
         * much more lightweight than class \b %Walker.<br>
         * Its primary use is for algorithms that just need to 'visit' all nodes of the
         * \b %StringTree (or a branch of it), regardless of the order.
         * Hence, it is typically used in <em>range-for-loops</em> with <b>std::for_each</b>.
         * This is why methods #begin and #end of class \b %StringTree return a \b %StdIterator of
         * this class type.
         *
         * For more information on iterators see description of outer class
         * \ref aworx::lib::util::StringTree "StringTree".
         ******************************************************************************************/
        class StdIterator  : public std::iterator<std::input_iterator_tag, T>
        {
            friend class StringTree;

            //####  Inner class RecursionData  #########################################################
            protected:

            /// Simple pair of a node pointer and its current child
            struct RecursionDataIt
            {
                /// The current node
                Node*                                               node;

                /// The name of the current node
                String                                              nodeName;

                /** The current child of the node. If this points to the end of the map, then
                 *  the the #node itself is selected.*/
                typename UnorderedStringMap<Node*>::const_iterator  actChild;
            };


            //####  Protected fields (class StdIterator)  #############################################
            protected:

                /// The \b %StringTree we belong to
                StringTree*                    stringTree;

                /** A stack holding the recursive list of child and a \b %StdIterator to their current
                 *  child.
                 *  \note The struct \% RecursionData is an internal, protected struct which is
                 *        currently not documented for technical reasons. You can consult the
                 *        source code for this inner structure
                 *
                 */
                std::stack<RecursionDataIt>   stack;

                /// Requested depth of iteration recursion.
                int                         recursionDepth;

            //####  Protected methods (class iterator)  ############################################
            protected:

                /**
                 * Identifies the node that the \b %StdIterator currently refers to.
                 * @return The actual node.
                 */
                Node* actNode()
                {
                    ALIB_ASSERT_ERROR( !stack.empty(), ASTR("Invalid iterator"))

                    RecursionDataIt& nc= stack.top();
                    return   nc.actChild == nc.node->children.end() ? nc.node
                                                                    : nc.actChild->second;
                }


                /**
                 * Resets this \b %StdIterator to 'point' to the given cursor.<br>
                 * Note the explanation of recursion depth parameter in section
                 * \ref cpp_alib_containers_stringtree_iterator "Recursive Iteration"
                 * of the parent class description.
                 *
                 * @param node            The start node.
                 * @param _recursionDepth Maximum recursion depth.
                 */
                void setStart( typename StringTree::Node* node, int _recursionDepth )
                {
                    if( stack.size() == 0 )
                        stack.emplace();
                    else
                        while(stack.size() > 1 )
                            stack.pop();

                    auto& nc= stack.top();
                    nc.node=       node;
                    nc.nodeName=   &EmptyString;
                    nc.actChild=   node->children.end();

                    // skip root node if recursion depth is limited
                    recursionDepth= _recursionDepth <= 0 ? (std::numeric_limits<int>::max)() : _recursionDepth;
                    if( _recursionDepth > 0 )
                        ++(*this);
                }

                /**
                 * Internal constructor
                 * @param _stringTree        The StringTree we belong to.
                 * @param node            The start node
                 * @param _recursionDepth The maximum iteration recursion depth.
                 */
                StdIterator( StringTree* _stringTree, typename StringTree::Node* node, int _recursionDepth )
                : stringTree( _stringTree )
                {
                    setStart(node, _recursionDepth);
                }

            //####  std::iterator methods (input_iterator)  ########################################
            public:

                /**
                 * Public constructor. Creates a \b %StdIterator which is not valid.
                 * @param _stringTree        The StringTree we belong to.
                 */
                StdIterator(StringTree* _stringTree)
                : stringTree( _stringTree )
                {}

                /**
                 * Comparison operator.
                 * @param other  The object to compare ourselves to.
                 * @return \c true if this and given %StdIterator are equal, \c false otherwise.
                 */
                bool operator==(const StdIterator& other) const
                {
                    bool stackSizesAreEqual= stack.size() == other.stack.size();
                    if ( !stackSizesAreEqual  || stack.size() == 0 || other.stack.size() == 0)
                        return stackSizesAreEqual;

                    return      this->stack.top().node     == other.stack.top().node
                            &&  this->stack.top().actChild == other.stack.top().actChild;
                }

                /**  OK
                 * Comparison operator.
                 * @param other  The object to compare ourselves to.
                 * @return \c false if this and given %StdIterator are equal, \c true otherwise.
                 */
                bool operator!=(const StdIterator &other) const
                {
                    return !(*this == other );
                }


                /**
                 * Skips remaining children of the current node and continues with its next
                 * sibling.
                 */
                void Skip()
                {
                    ALIB_ASSERT_ERROR( !stack.empty(), ASTR("Invalid iterator"))
                    stack.pop();
                    if(!stack.empty())
                        ++(*this);
                }

                /** Increment operator.
                 *  @return A reference to ourselves. */
                StdIterator& operator++()
                {
                    while(stack.size() > 0 )
                    {
                        RecursionDataIt& nc= stack.top();
                        if( nc.actChild == nc.node->children.end() )
                            nc.actChild= nc.node->children.begin();
                        else
                            nc.actChild++;

                        if( nc.actChild != nc.node->children.end() )
                        {
                            if( static_cast<int>( stack.size() ) < recursionDepth )
                            {
                                stack.emplace();
                                auto& ncNew= stack.top();
                                ncNew.node=       nc.actChild->second;
                                ncNew.nodeName=   nc.actChild->first;
                                ncNew.actChild=   nc.actChild->second->children.end();

                            }
                            break;
                        }

                        stack.pop();
                        continue;
                    }

                   return *this;
                }

                /** Retrieves the current StringTree node.
                 * @return The current StringTree node.              */
                T& operator*()
                {
                    return actNode()->value;
                }


            //####  node data access interface (StdIterator)  #########################################
            public:

                /**
                 * Returns the name of the node that this cursor represents.
                 * The name of the node corresponds to the path segment or child name that was
                 * used to create it.
                 *
                 * @return The name of the represented node.
                 */
                String Name()
                {
                    ALIB_ASSERT_ERROR( !stack.empty(), ASTR("Invalid iterator"))

                    RecursionDataIt& nc= stack.top();
                    return   nc.actChild == nc.node->children.end() ? nc.nodeName
                                                                    : nc.actChild->first;
                }

                /**
                 * Returns the  number of direct children.
                 * @return The number of children of the currently represented node.
                 */
                int Size()
                {
                    return static_cast<int>( actNode()->children.size() );
                }

                /**
                 * Returns the depth of the current iteration. This value is available to the
                 * algorithm which means this method executes in constant time.
                 *
                 * To get the absolute depth of the current node, create a cursor object from this
                 * \b %StdIterator and use method
                 * \ref aworx::lib::util::StringTree::Cursor::Depth "Cursor::Depth".
                 *
                 * @return The distance of the current node and the node of the start of the
                 *         iteration.
                 */
                int Depth()
                {
                    return static_cast<int>( stack.size() ) - 1;
                }

        };  // inner class StdIterator

    // #############################################################################################
    // class Cursor
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * This public, inner class provides the main interface into outer class \b StringTree.
         * Only with an object of this class, new nodes can be inserted and removed.
         * As the name indicates, an object of this class represents a current position within
         * a \b %StringTree.
         *
         * The class is very lightweight. In fact, besides a pointer to the \b %StringTree it works
         * on, the only other member  is a pointer to the currently represented node of
         * the \b %StringTree.
         * This allows to copy and move instances of this class very efficiently.
         *
         * For more information on the how this class is used, see
         * \ref cpp_alib_containers_stringtree_cursor "Inserting, Retrieving And Deleting Nodes".
         ******************************************************************************************/
        class Cursor
        {
            friend class StringTree;

            //####  Protected fields (class Cursor)  #############################################
            protected:
                /// The \b %StringTree we belong to
                StringTree*              stringTree;

                /// The node this cursor represents.
                Node*                    node;

            //####  Protected methods (class Cursor)  ############################################
            protected:

                /**
                 * Internal constructor
                 * @param _stringTree The \b %StringTree we work on.
                 * @param _node    The node to refer to.
                 */
                Cursor(StringTree* _stringTree, Node* _node)
                : stringTree( _stringTree )
                , node   ( _node    )
                {}

                /**
                 * Finds a child node along the \p{path} given, but does not create new nodes.
                 * Incomplete results may occur if a child along the path was not found.
                 * In this case, parameter \p{path} contains
                 * the remaining path, excluding a leading separator.
                 *
                 * A leading slash (aka \p{TSeparator}) allows absolute path addressing, which means
                 * the root of \p{node} is searched if a leading separator is found.
                 *
                 * Besides normal child names, this method accepts
                 * - multiple separator characters (ignored)
                 * - child name "." (ignored)
                 * - child name ".." for parent node
                 *
                 * If, while processing the path string, the root node is found an the next
                 * path element is "..", this element is ignored and processing continues.
                 * As a sample, the paths:
                 *
                 *      /a/../b
                 * and
                 *
                 *      /a/../../b
                 * both evaluate to
                 *
                 *      /b
                 * assumed that \e /a and \e /b exist.
                 *
                 * @param         node The start node.
                 * @param[in,out] path Creation path. Provided as reference and consumed as far
                 *                     as the path exits.
                 * @return The node found
                 */
                static
                Node* followPath( Node* node, Substring& path )
                {
                    // check for "root addressing"
                    if( path.CharAtStart() == TSeparator )
                    {
                        path.ConsumeChars( 1 );
                        while( node->parent != nullptr )
                            node= node->parent;
                    }

                    // loop over node names in path
                    for(;;)
                    {
                        // multiple separators are ignored
                        while(path.ConsumeChar( TSeparator ) )
                            ;

                        if( path.IsEmpty() )
                            return node;


                        String name=  path.Substring<false>( 0, path.IndexOfOrLength( TSeparator ) );


                        if( name.Equals( ASTR("..") ) )
                        {
                            // we move up only if possible. If not, the ".." is just ignored (consumed)
                            // (This behavior was just more helpful in the use cases so far)
                            if( node->parent != nullptr )
                                node= node->parent;
                        }

                        else if( !name.Equals( ASTR(".") ) )
                        {
                            auto it= node->children.find( name );
                            if( it == node->children.end() )
                                return node;
                            node= it->second;
                        }

                        path.ConsumeChars( name.Length() );
                    }
                }

                /**
                 * Creates child nodes corresponding to the given \p{path}.
                 * If the first node already exists, nothing is done and \c nullptr is returned as
                 * this is considered an error.
                 * If the given path is empty the given node is returned as this is \b not
                 * considered an error.
                 *
                 * Child names <c>"."</c> and <c>".."</c> are ignored, but considered an error.
                 * In debug builds, an \ref ALIB_WARNING is reported.
                 *
                 * The final leaf node is returned.
                 *
                 * @param allocator The allocator used to create the name key.
                 * @param         node The start node.
                 * @param[in,out] path Creation path. Provided as reference and consumed if not
                 *                     errorneous.
                 * @return The leaf node of all created nodes. \c nullptr in the case that the
                 *         first child given in \p{path} already exists.
                 */
                static
                Node* createPathAndReturnLeaf( TAllocator& allocator, Node* node, Substring& path )
                {
                    bool isFirst= true;
                    for(;;)
                    {
                        // consume and check
                        while(path.ConsumeChar( TSeparator ) )
                            ;
                        if( path.IsEmpty() )
                            return node;

                        // get next name
                        String childName= path.Substring<false>( 0, path.IndexOfOrLength( TSeparator ) );
                        if( childName.Equals( ASTR(".") ) || childName.Equals(ASTR("..")) )
                        {
                            //ALIB_WARNING( "Illegal child name {!Q} in creation path", childName );
                            return node;
                        }

                        Node* child;
                        if( isFirst )
                        {
                            child= node->createChild( allocator, childName );
                            if( child == nullptr )
                                return nullptr;
                        }
                        else
                        {
                            child= node->createChildNoCheck( allocator, childName );
                        }
                        isFirst= false;

                        // done?
                        path.ConsumeChars( childName.Length() + 1);
                        node= child;
                    }
                }


            //####  Constructor, comparison operators, etc     #####################################
            public:

                /// Public constructor. Creates a cursor which is not valid.
                Cursor()    {}

                /**
                 * Constructs a cursor from a \b %StdIterator.
                 * @param _iterator The \b %StdIterator to take the represented node from
                 */
                Cursor( StdIterator& _iterator)
                : stringTree( _iterator.stringTree   )
                , node   ( _iterator.actNode() )
                {
                }

                /** Comparison operator.
                 *  @param other  The object to compare ourselves to.
                 *  @return \c true if this and given cursor are equal, \c false otherwise. */
                bool operator==(const Cursor &other) const
                {
                    return node == other.node;
                }

                /** Comparison operator.
                 *  @param other  The object to compare ourselves to.
                 *  @return \c false if this and given cursor are equal, \c true otherwise. */
                bool operator!=(const Cursor &other) const
                {
                    return node != other.node;
                }

                /**
                 * Return a copy of ourselves.
                 * @return A clone.
                 */
                Cursor      Clone()
                {
                    return Cursor( stringTree, node );
                }

                /**
                 * Return the \b %StringTree we are working on.
                 * @return The \b %StringTree object.
                 */
                StringTree&   GetStringTree()
                {
                    return *stringTree;
                }

            //####  Tree navigation inplace, returning status      #################################
                /**
                 * Moves this cursor to the root of its #stringTree.
                 *
                 * @return *this to allow concatenated calls
                 */
                Cursor&   Root()
                {
                    node= stringTree->root;
                    return (*this);
                }

                /**
                 * Moves this cursor to its parent node.  With template parameter \p{TCheck},
                 * two different versions of the method can be selected.<br>
                 * The default,  checked version (\p{TCheck} = \c true ) will not move a cursor
                 * that is already pointing to the root of the \b %StringTree into an invalid state.
                 * Instead the cursor remains unchanged and \c false is returned.<br>
                 * The unchecked version will turn the cursor into an invalid state if no
                 * parent node exists. This is useful in two situations:
                 * 1. For effective iterating from any node up to
                 *    the \b %StringTree root node. The condition to check in such loop is #IsValid.
                 * 2. In situations when it is was already checked that a cursor does not represent
                 *    the root node. This is demonstrated in the following sample taken from this
                 *    classes' method #SearchPath:
                 *    \snippet "alib/util/stringtree.hpp"     DOX_ALIB_STRING_TREE_MOVETOPARENT
                 *    As can be seen, this sample also uses non-checking versions of the
                 *    insertion methods of class \alib{strings,AStringBase,AString} because
                 *    the code prior to the code snippet ensured the validity of all parameters.
                 *
                 * <p>
                 * \note If this class is nested in another template class, the invocation of this
                 *       method with the template parameter may be ambiguous. Therefore,
                 *       an alias to the non-checking version is provided with
                 *       #MoveToParentUnchecked.
                 *
                 * @tparam TCheck  Defaults to \c true which is the normal invocation mode.
                 *                 If \c \<false\> is added to the method name, no check is
                 *                 performed and this object becomes invalid if it represents the
                 *                 root node
                 * @return \c true if this object was moved to its parent (had a parent), \c false
                 *         otherwise.
                 *         Consequently, in the non-checking version the return value indicates  if
                 *         this cursor is valid after the operation.
                 */
                template <bool TCheck= true>
                bool    MoveToParent()
                {
                    if( TCheck )
                    {
                        if ( node->parent == nullptr )
                            return false;

                        node= node->parent;
                        return true;
                    }

                    // non-checking version
                    return (node= node->parent) == nullptr;
                }

                /**
                 * Non-checking version of #MoveToParent.
                 *
                 * @return \c true if this object is valid after the operation, \c false otherwise.
                 */
                bool    MoveToParentUnchecked()
                {
                    // non-checking version
                    return (node= node->parent) == nullptr;
                }

                /**
                 * Moves this cursor to the child node named \p{childName}. If no child with this
                 * name exists, the cursor remains as is and \c false is returned.
                 *
                 * This method does not check the given \p{childName} to to be valid (i.e not equal
                 * to <c>"."</c> or <c>".."</c> or contain a separator character.
                 * Children with this name do not exist and should not be found. However, in
                 * debug compilations, an \ref ALIB_WARNING is reported.
                 *
                 * @param   childName   The name of the desired child.
                 * @return \c true if the child existed and this object is valid, \c false
                 *         otherwise.
                 */
                bool    MoveToChild( const String& childName )
                {
                    // we do not check for illegal names as they will not be found anyhow
                    Node* child= node->getChild( childName );
                    if( child == nullptr )
                        return false;
                    node= child;
                    return true;
                }

                /**
                 * Moves this cursor to the child node named \p{childName}. If no child with this
                 * name exists, one will be created.
                 *
                 * This method checks the given \p{childName} to not equal to <c>"."</c> or
                 * <c>".."</c> and that it does not contain the separation character.
                 * If it does, \c false is returned and in debug compilations an
                 * \ref ALIB_WARNING is reported.
                 *
                 * @param  childName The name of the desired child.
                 * @return \c true if the child was found or created, \c false otherwise.
                 */
                bool    MoveToChildCreate( const String& childName )
                {
                    auto result= node->getOrCreateChild( *stringTree, childName );
                    if( node == nullptr )
                        return false;
                    node=   result.first;
                    return  result.second;
                }

                /**
                 * Moves this cursor along the given path.<br>
                 * The method supports absolute and relative path addressing: If \p{path} begins
                 * with a separation character, then the cursor is moved to the root of the
                 * \b %StringTree. Furthermore, child name <c>"."</c> is ignored and just skipped while if
                 * <c>".."</c> is found in the path, the cursor is moved to its parent.
                 * Repeated separation characters are ignored.
                 *
                 * If a child along the path does not exist, this cursor remains unchanged and
                 * \c false is returned. This includes the case that child name <c>".."</c> is read
                 * while the cursor references the root node.
                 *
                 * See #MoveToExistingPart and #MoveToAndCreateNonExistingPart for alternatives.
                 *
                 * @param path      The path to move along.
                 * @return A std::pair of a reference to this cursor and a boolean value.
                 *         The boolean value indicates if the path existed and the move to the node
                 *         specified by \p{path} was performed.
                 */
                bool    MoveTo( const String& path )
                {
                    Substring consumable(path);
                    Node* dest= followPath( node, consumable );
                    if( consumable.IsNotEmpty() )
                        return false;
                    node= dest;
                    return true;
                }

                /**
                 * Moves this cursor along the existing portion of the given path. The method
                 * returns the remaining portion of the path that did not exist.
                 *
                 * See #MoveTo and #MoveToAndCreateNonExistingPart for alternatives.
                 *
                 * @param path      The path to move along.
                 * @return The unconsumed portion of the path. Empty if the path existed.
                 */
                Substring   MoveToExistingPart( const String& path )
                {
                    Substring remainingPath(path);
                    node= followPath( node, remainingPath );
                    return remainingPath;
                }


                /**
                 * Moves this cursor along the existing portion of the given path and then
                 * creates any non-existing, remaining portion.
                 *
                 * Child names <c>"."</c> and <c>".."</c> in the creation portion of the given path
                 * are ignored, but considered an error.
                 * In debug builds, an \ref ALIB_WARNING is reported.
                 *
                 * See #MoveTo and #MoveToExistingPart for alternatives.
                 *
                 * @param path      The path to move along.
                 * @return \c false if the complete path existed, \c true if a part of it was
                 *         created.
                 */
                bool    MoveToAndCreateNonExistingPart( const String& path )
                {
                    bool created= false;
                    Substring rest= path;
                    for(;;)
                    {
                        rest= MoveToExistingPart( rest );
                        if( rest.IsEmpty() )
                            break;
                        created= true;
                        node= createPathAndReturnLeaf( *stringTree, node, rest );
                    }
                    return created;
            }


            //####  Node deletion         ##########################################################
                /**
                 * Deletes the children of the node that this cursor refers to.
                 * The cursor itself is not changed.
                 *
                 * @return The number of children that were deleted.
                 */
                int DeleteChildren()                                                          const
                {
                    int result= static_cast<int>( node->children.size() );
                    node->deleteChildren(*stringTree);
                    return result;
                }

                /**
                 * Deletes the child named \p{childName} from the node that this cursor refers to.
                 * The cursor itself is not changed.
                 *
                 * @param   childName   The name of the desired child.
                 * @return \c true if the child existed and was deleted, \c false otherwise.
                 */
                bool DeleteChild( const String& childName)                                     const
                {
                    return node->deleteChild( *stringTree, childName );
                }


                /**
                 * Deletes the node that this cursor refers to from the tree.
                 * If this cursor did not represent the root node, then after the operation, the
                 * cursor refers to the parent node. Otherwise, only the children are deleted and
                 * the cursor remains representing the same (root) node.
                 *
                 * The method involves a call to #SearchName, which - as documented - is not
                 * considered to be efficient. If the name of the current node is known, then
                 * it is advised to use method #DeleteChild on the parent of this node.
                 *
                 * @return \c true if this node was not the root node and could be removed from
                 *         the parent. \c false if this not was the root node.
                 */
                bool SearchNodeNameAndDeleteNode()
                {
                    DeleteChildren();
                    if( IsRoot() )
                        return false;

                    String name= SearchName();
                    node= node->parent;
                    node->deleteChild( *stringTree, name );
                    return true;
                }


            //####  StdIterator creation       ########################################################
                /**
                 * Creates a \b %StdIterator starting with the node represented by this cursor.
                 * Unlike with method
                 * \ref aworx::lib::util::StringTree::begin "StringTree::begin", here the
                 * default value for parameter \p{_recursionDepth} is \1. This means for example,
                 * using a cursor object in a range-for-loop, the iteration is restricted to the
                 * direct children of the node represented by the cursor.
                 *
                 * @param _recursionDepth The maximum iteration recursion depth.
                 *                        Defaults to \1.
                 * @return An \b %StdIterator object.
                 */
                StdIterator begin( int _recursionDepth = 1 )
                {
                    return StdIterator( stringTree, node, _recursionDepth );
                }

                /**
                 * Resets a given \b %StdIterator to point to this cursor and hence allows to re-use
                 * an existing \b %StdIterator instance.
                 * @param it              The \b %StdIterator to change.
                 * @param _recursionDepth The maximum iteration recursion depth.
                 *                        Defaults to \c 1.
                 * @return The \b %StdIterator passed with parameter \p{it}.
                 */
                StdIterator& SetIterator( StdIterator& it, int _recursionDepth = 1 )
                {
                    it.setStart( node, _recursionDepth );
                    return it;
                }

                /**
                 * Returns an invalid end-\b %StdIterator.
                 * @return The non-valid \b %StdIterator object which indicates the end of iteration.
                 */
                StdIterator end()
                {
                    return StdIterator( stringTree );
                }

            //####  node data access interface (Cursor)  #########################################
            public:

                /** Retrieves the value of the currently represented node.
                 * @return The current nodes' value.          */
                T& operator*() const
                {
                    return node->value;
                }

                /**
                 * Returns the number of direct children.
                 * @return The number of children of the represented node.
                 */
                int Size()
                {
                    return static_cast<int>( node->children.size() );
                }

                /**
                 * Returns the name of the node that this cursor represents.
                 * The method searches all children of this nodes' parent node and returns the
                 * key that this node was stored with. If the represented node is the root
                 * node, \ref aworx::NullString "NullString" is returned.
                 *
                 * \note
                 *   In most situations the name of the node should be available in the context
                 *   where an invocation of this method is considered.
                 *   In other words, this method should be rarely needed and used under the
                 *   presumption that it does not perform in constant execution time.
                 *
                 * @return The name of the represented node.
                 */
                const String SearchName()
                {
                    if( node->parent == nullptr )
                        return NullString;
                    for( auto& sibling : node->parent->children )
                        if( sibling.second == node )
                            return sibling.first;
                    ALIB_ERROR(ASTR("Internal Error"))
                    return NullString;
                }

               /**
                 * Builds a path string from the root node to this node.
                 * This is done in reverse order, starting from this node using method
                 * #SearchName. Hence, this method is <b>not considered efficient</b>!
                 * For each depth-level of the represented node, a 'reverse' search in the parent's
                 * map of children is involved.
                 *
                 * It is recommended to search for other ways to keep track of the current
                 * path of a cursor - outside of this class and use this method for e.g. for debug
                 * purposes or really rare cases.
                 *
                 * @param target        The target to append the path to.
                 * @param targetData    Denotes whether \p{target} should be cleared prior to
                 *                      appending the path. Defaults to CurrentData::Clear.
                 */
                void SearchPath( AString&    target,
                                 CurrentData targetData = CurrentData::Clear )
                {
                    if( targetData == CurrentData::Clear )
                        target.Clear();

                    if( !IsValid() )
                        return;
                    target._( EmptyString ); // set non nulled


                    integer insertionPos= target.Length();
#if !defined(DOX_PARSER)
//! [DOX_ALIB_STRING_TREE_MOVETOPARENT]
Cursor iter= *this;
while( !iter.IsRoot() )          // <- not root?
{
    if( target.Length() > insertionPos )
        target.InsertChars<false>( TSeparator, 1, insertionPos );

    target.InsertAt<false>( iter.SearchName(), insertionPos );

    iter.MoveToParent<false>();  // <- non-checking version to avoid a redundant check.
}
//! [DOX_ALIB_STRING_TREE_MOVETOPARENT]
#endif
                }

                /**
                 * Returns \c true if this cursor represents the root node of the \b %StringTree,
                 * \c false otherwise.
                 * @return \c true if this is the root node, \c false otherwise.
                 */
                bool IsRoot()
                {
                    return node->isRoot();
                }

                /**
                 * Determines the depth of the node represented by this cursor. This is done by
                 * counting the iterations needed to reach the root node of the tree.
                 * @return The distance from this node to the root node.
                 */
                int Depth()
                {
                    return node->depth();
                }

                /**
                 * Determines if this is a valid cursor. Cursors usually do not get invalid.
                 * See method #MoveToParent for more information about how cursors may
                 * get invalid and how this is useful.
                 *
                 * @return \c true if this is a valid cursor. If invalid, \c false is returned and
                 *         the cursor must not be used.
                 */
                bool IsValid()
                {
                    return node!=nullptr;
                }

                /**
                 * Returns the opposite of #IsValid.
                 *
                 * @return \c true if this is an invalid cursor that must not be used, \c false
                 *         otherwise.
                 */
                bool IsInvalid()
                {
                    return node==nullptr;
                }

        };  // inner class Cursor

       //##########################################################################################
        //  Walker
        //##########################################################################################
        /** ****************************************************************************************
         * This public, inner class implements an iterator that does <b>not</b> apply to the concept
         * of \c std::iterator (see class
         * \ref aworx::lib::util::StringTree::StdIterator "StdIterator" for a variant that does).
         * The rational for this is the fact that the class provides mechanics to sorting the
         * child nodes, which requires allocation of quite more resources than a usual container
         * iterator. Therefore objects of this type are not supposed to be passed around
         * and copied on the fly. Instead, instances should rather created once and then re-used
         * in subsequent iterations.
         *
         * Sorting of child nodes can be done using a built-in comparison function which works
         * on the child names (path names) and allows to choose ascending and descending order and
         * to ignore letter case, respectively be sensitive about it. Besides this, a custom
         * comparison function for child name based sorting can be established as well as a
         * custom function used to sort by attributes of the custom template type \p{T}.
         * See methods #SortByPathname and #SortByValue for details on sorting.
         *
         * Objects of this type can initialized, respectively reset to distinct start nodes by
         * providing objects of type
         * - \b %StringTree
         * - \b %StringTree::StdIterator
         * - \b %StringTree::Cursor and
         * - \b %StringTree::Walker
         *
         * to overloaded methods #SetStart. Then the iteration is performed with method #Next
         * and its end is detected with method #IsValid. During iteration, method Skip stops
         * the iteration of the current node and continues with its next sibling.
         *
         * Recursiveness of iteration is controlled with method #SetRecursionDepth.
         *
         * Finally, the generation of a string representing the actual path to the current
         * iteration node relative to the start node can be activated. See #PathGeneration
         * for information about this feature.
         *
         * For more information on iterators see description of outer class
         * \ref aworx::lib::util::StringTree "StringTree".
         ******************************************************************************************/
        class Walker
        {
            //######################################################################################
            //  Walker internals
            //######################################################################################
            friend class StringTree;
            protected:
                /**
                 * This is a internal, protected helper struct implementing operator() to sort the
                 * nodes.
                 */
                struct Sorter
                {
                    /// The sort order (used with default path name sorting).
                    SortOrder               sortOrder                         =SortOrder::Ascending;

                    /// The case sensitivity of the sort (used with default path name sorting).
                    Case                    sortSensitivity                           =Case::Ignore;

                    /// A pointer to a user-defined comparison method for path names.
                    bool                    (*pathSorter)(const String&, const String&)    =nullptr;

                    /// A pointer to a user-defined comparison method for value types.
                    bool                    (*valueSorter)(const T&, const T&)             =nullptr;


                    /**
                     * This is the sort function, which will be invoked by \c std::sort.
                     * @param lhs   The left hand side operand.
                     * @param rhs   The right hand side operand.
                     * @return \c true if lhs < rhs, \c false else.
                     */
                    bool operator() ( const std::pair<String,Node*>& lhs,
                                      const std::pair<String,Node*>& rhs )
                    {
                        if( valueSorter != nullptr )
                            return (*valueSorter)( lhs.second->value, rhs.second->value );

                        if( pathSorter != nullptr )
                            return (*pathSorter)( lhs.first, rhs.first );

                        // standard sort by path
                        int compResult= ( sortSensitivity == Case::Sensitive ? lhs.first. template CompareTo<true, Case::Sensitive>( rhs.first )
                                                                             : lhs.first. template CompareTo<true, Case::Ignore   >( rhs.first ) );
                        return sortOrder == SortOrder::Ascending  ? compResult < 0
                                                                  : compResult > 0;
                    }
                };


                /// Protected, internal struct used to store the data of recursive iterations.
                struct RecursionData
                {
                    /// The current node
                    Node*                                                           node;

                    /// The name of the current node.
                    String                                                          nodeName;

                    /** A vector of children copied from the map to this vector for the purpose of
                     *  sorting them. */
                    std::vector<std::pair<String,Node*>>*                           sortedChildren;

                    /** The current child of the current node in case of sorted access
                     *  If this is pointing to the end of the child map, then
                     *  the actual node itself is selected by this walker. */
                    typename std::vector<std::pair<String,Node*>>::iterator         actChild;


                    /**
                     * Constructor
                     */
                    RecursionData()
                    {
                        sortedChildren= nullptr;
                    }
                    /**
                     * Destructor
                     */
                    ~RecursionData()
                    {
                        if( sortedChildren )
                            delete sortedChildren;
                    }

                    /**
                     * Add default move constructor explicitly. On moving objects, nullptr is
                     * written to to field #sortedChildren of original object to prevent deletion.
                     * @param other  The node to copy from
                     */
                    RecursionData( RecursionData&& other )
                    {
                        node=           other.node;
                        nodeName=       other.nodeName;
                        sortedChildren= other.sortedChildren;
                        actChild=       other.actChild;
                        other.sortedChildren= nullptr;
                    }

                    /**
                     * Reset to point to the given node. Used in each recursion step.
                     * Copies all children from the map to a vector and sorts them there.
                     * @param _node     The node to store.
                     * @param _nodeName The name of the node to store.
                     * @param _sorter   The sorter used to sort the children in the node.
                     */
                    void reset( Node* _node, String _nodeName, Sorter& _sorter )
                    {
                        node=     _node;
                        nodeName= _nodeName;
                        if (sortedChildren == nullptr)
                            sortedChildren= new std::vector<std::pair<String,Node*>>();
                        else
                            sortedChildren->clear();
                        sortedChildren->reserve( node->children.size() );
                        for( auto it : node->children )
                            sortedChildren->emplace_back( std::make_pair(&it.first, it.second) );
                        std::sort( sortedChildren->begin(), sortedChildren->end(), _sorter );
                        actChild= sortedChildren->end();
                    }
                };

                /// The \b %StringTree we belong to
                StringTree*              stringTree;

                /** A stack holding the recursive list of child maps and the  idx of their current
                 * child. Implemented as a vector in combination with member #stackSize, to reuse
                 * allocated storage space.  */
                std::vector<RecursionData>  stack;

                /// The current use of the stack.
                size_t                  stackSize                                               = 0;

                /// The path to the actual node (excluding the name of the actual node).
                AString                 actPath;

                /** The sorter struct which provides an overload of <c>bool operator()</c> used
                 *  for sorting the child nodes.*/
                Sorter                  sorter;

                /// The requested depth of iteration recursion.
                int                     recursionDepth           =(std::numeric_limits<int>::max)();

                /**
                 * Internal method to identify the actual node.
                 * @return Pointer to the node object that we currently represent.
                 */
                Node* actNode()
                {
                    ALIB_ASSERT_ERROR( IsValid(), ASTR("Invalid walker"))

                    RecursionData& nc= stack[ stackSize -1 ];
                    return         nc.actChild == nc.sortedChildren->end() ?  nc.node
                                                                           :  nc.actChild->second;
                }

                /**
                 * Resets this walker to represent to the given node.
                 *
                 * @param newNode           The new node to start from
                 */
                void setStart( typename StringTree::Node* newNode )
                {
                    actPath.Clear();

                    stackSize= 1;
                    auto& actRecursionData= stack[0];
                    if( newNode == nullptr )
                        newNode= actRecursionData.node;
                    actRecursionData.reset( newNode, &EmptyString, sorter  );

                    // skip root node if recursion depth is limited
                    if( recursionDepth != (std::numeric_limits<int>::max)() )
                        Next();
                }

            //######################################################################################
            //  Walker public interface
            //######################################################################################
            public:
                /**
                 * Constructor
                 * @param _stringTree The \b %StringTree we walk on.
                 */
                Walker( StringTree* _stringTree )
                : stringTree( _stringTree )
                {
                    stack.emplace_back();
                    setStart( stringTree->root );
                }

                /**
                 * Destructor
                 */
                ~Walker()
                {

                }

                /**
                 * Return the \b %StringTree we are working on.
                 * @return The \b %StringTree object.
                 */
                StringTree&   GetStringTree()
                {
                    return stringTree;
                }

                /**
                 * Resets this walker to work with the given \b %StringTree. Initializes the walker
                 * to its root node.
                 *
                 * @param pm  The \b %StringTree to use.
                 */
                void SetStart( StringTree* pm)
                {
                    stringTree= pm;
                    setStart( stringTree->root );
                }

                /**
                 * Resets this %walker to represent the node that the given cursor object represents.
                 * @param cursor  The cursor to copy the represented node from.
                 */
                void SetStart( Cursor cursor )
                {
                    ALIB_ASSERT_ERROR( stringTree == cursor.stringTree,
                                       ASTR("Differnt referred StringTree objects") );
                    setStart( cursor.node );
                }

                /**
                 * Resets this %walker to represent the node that the given \b %StdIterator object represents.
                 * @param it  The \b %StdIterator to copy the represented node from.
                 */
                void SetStart( StdIterator& it)
                {
                    ALIB_ASSERT_ERROR( stringTree == it.stringTree,
                                       ASTR("Differnt referred StringTree objects") );
                    setStart( it.actNode() );
                }

                /**
                 * Resets this walker to represent the same node that the given other walker
                 * object represents.
                 * @param other  The walker to copy the represented node from.
                 */
                void SetStart( Walker& other )
                {
                    ALIB_ASSERT_ERROR( stringTree == other.stringTree,
                                       ASTR("Differnt referred StringTree objects") );
                    setStart( other.actNode() );
                }

                /**
                 * Invalidates this object. After invoking this method, this walker can not be
                 * used further, until #SetStart is invoked.
                 * After the invocation, method #IsValid will return \c false.
                 */
                void Invalidate()
                {
                    stackSize= 0;
                }

                /**
                 * Determines if this is a valid walker. Walkers may become invalid after
                 * invocations of method #Next at the end of the iteration and become valid with
                 * the invocation of one of the overloaded methods #SetStart
                 * (when invoked with valid objects).
                 *
                 * @return \c true if this is a valid cursor. If invalid, \c false is returned and
                 *         the cursor must not be used.
                 */
                bool IsValid()                                                                 const
                {
                    return stackSize != 0;
                }

                /**
                 * Returns the opposite of #IsValid.
                 *
                 * @return \c true if this is an invalid cursor that must not be used, \c false
                 *         otherwise.
                 */
                bool IsInValid()                                                               const
                {
                    return stackSize == 0;
                }


                /**
                 * Sets the sorting of children by their path name, using the built-in comparison
                 * methods, which in turn use method
                 * \alib{strings,StringBase::Equals,String::Equals}.
                 *
                 * Overloaded methods, which allow different sorting mechanics exist.
                 * This method should be invoked prior to the invocation of one of the #SetStart
                 * methods which start the iteration. Invocation during iteration is allowed,
                 * but only nodes that have not been visited yet will be affected by the new
                 * sorting.
                 *
                 * @param order         The sort order.
                 *                      Defaults to \b %SortOrder::Ascending.
                 * @param sensitivity   The case sensitivity when comparing path names.
                 *                      Defaults to \b %Case::Ignore.
                 */
                void SortByPathname( SortOrder order         = SortOrder::Ascending,
                                     Case      sensitivity   = Case::Ignore            )
                {
                    sorter.sortOrder=       order;
                    sorter.sortSensitivity= sensitivity;
                    sorter.valueSorter=     nullptr;
                    sorter.pathSorter=      nullptr;
                }

                /**
                 * Sets the sorting of children by their path name, using the given callback
                 * function.
                 *
                 * Overloaded methods, which allow different sorting mechanics exist.
                 * This method should be invoked prior to the invocation of one of the #SetStart
                 * methods which start the iteration. Invocation during iteration is allowed,
                 * but only nodes that have not been visited yet will be affected by the new
                 * sorting.
                 *
                 * @param pathSorter    A custom comparison method used for sorting the children
                 *                      of the nodes.
                 */
                void SortByPathname( bool (*pathSorter)(const String&, const String&) )
                {
                    sorter.valueSorter= nullptr;
                    sorter.pathSorter=  pathSorter;
                }

                /**
                 * Sets the sorting of children by their template value, using the given
                 * callback function.
                 *
                 * Overloaded methods, which allow different sorting mechanics exist.
                 * This method should be invoked prior to the invocation of one of the #SetStart
                 * methods which start the iteration. Invocation during iteration is allowed,
                 * but only nodes that have not been visited yet will be affected by the new
                 * sorting.
                 *
                 * @param valueSorter  A custom comparison method used for sorting the children
                 *                     of the nodes.
                 */
                void SortByValue( bool (*valueSorter)(const T&, const T&) )
                {
                    sorter.valueSorter= valueSorter;
                    sorter.pathSorter=  nullptr;
                }

                /**
                 * Sets the requested recursion dept. A value of \c 0 or negative sets unlimited
                 * recursion depth.<br>
                 * Please note the explanation of recursion depth parameter in section
                 * \ref cpp_alib_containers_stringtree_iterator "Recursive Iteration"
                 * of the parent class description.
                 *
                 * @param _recursionDepth Sets the new recursion dept.
                 *                        Use \c 0 for unlimited recursion.
                 */
                void SetRecursionDepth( int _recursionDepth )
                {
                    recursionDepth= _recursionDepth <= 0 ? (std::numeric_limits<int>::max)() : _recursionDepth;
                }


                /**
                 * With this method, the generation of the actual path string is activated. The
                 * given character is used as separation character. The current path can
                 * be received using method #GetPath.<br>
                 * The method must be invoked prior to start walking with method #Next, otherwise
                 * undefined behavior might occur while 'walking'.
                 *
                 * @param generatePath Denotes whether the path should be generated or not.
                 */
                void PathGeneration( Switch generatePath)
                {
                    actPath=  generatePath == Switch::On  ?  EmptyString
                                                          :  NullString;
                }


                /** Retrieves the current StringTree nodes value.
                 * @return The current StringTree node templated, custom value.    */
                const T& operator*() const
                {
                    ALIB_ASSERT_ERROR( IsValid(), ASTR("Invalid iterator"))
                    const RecursionData& nc= stack[ stackSize -1 ];
                    const Node* actNode=  nc.actChild == nc.sortedChildren->end() ?  nc.node
                                                                                  :  nc.actChild->second;

                    return actNode->value;
                }

                /**
                 * Skips remaining children of the current node and continues with its next
                 * sibling.
                 */
                void Skip()
                {
                    ALIB_ASSERT_ERROR( IsValid(), ASTR("Invalid iterator"))
                    RecursionData* nc= &stack[ stackSize -1 ];
                    if( nc->sortedChildren->size() > 0 )
                        nc->actChild= nc->sortedChildren->end() - 1;

                    Next();
                }


                /**
                 * Goes to the next node.
                 *  @return The result of #IsValid after the 'walk'. */
                bool Next()
                {
                    ALIB_ASSERT_ERROR( IsValid(), ASTR("Invalid iterator"))
                    while( stackSize > 0 )
                    {
                        RecursionData* nc= &stack[ stackSize -1 ];

                        if( nc->actChild == nc->sortedChildren->end() )
                            nc->actChild=   nc->sortedChildren->begin();
                        else
                            nc->actChild++;

                        // not end of child ?
                        if( nc->actChild != nc->sortedChildren->end() )
                        {
                            // set path string (if activated)
                            if(static_cast<int>( stackSize ) < recursionDepth )
                            {
                                // set path string for #GetPath
                                if( stackSize > 1 && actPath.IsNotNull() )
                                {
                                    if( actPath.Length() )
                                        actPath <<  TSeparator;
                                    actPath <<  nc->nodeName;
                                }

                                if( stack.size() == stackSize )
                                {
                                    stack.emplace_back();
                                    nc= &stack[ stackSize -1 ]; // might have changed
                                }

                                stack[ stackSize ].reset( nc->actChild->second, nc->actChild->first, sorter );
                                stackSize++;
                            }
                            break;
                        }

                        // climb down
                        stackSize--;
                        if( stackSize > 1 && actPath.IsNotNull() )
                        {
                            actPath.DeleteEnd<false>(  stack[stackSize - 1].nodeName.Length() );
                            if( actPath.CharAtEnd() == TSeparator )
                                actPath.DeleteEnd<false>( 1 );
                        }
                    }

                   return IsValid();
                }


                /**
                 * Retrieves the current path of walking as a string representation.
                 * The path returned is relative to the start node and does not contain a leading
                 * separator character. Also, it does not contain the name of the current node,
                 * which can be received with method #Name.
                 *
                 * Note that this method can be used only if path generation was activated
                 * prior to the current iteration. Activation is performed with method
                 * #PathGeneration.
                 *
                 * @return The path of the current node.
                 */
                String    GetPathToNode()
                {
                    ALIB_ASSERT_ERROR( actPath.IsNotNull() , ASTR("Path generation not activated") );
                    return actPath;
                }

                /**
                 * Writes the results of  #GetPathToNode and #Name, separated by a separator char.
                 *
                 * Note that this method can be used only if path generation was activated
                 * prior to the current iteration. Activation is performed with method
                 * #PathGeneration.
                 *
                 * @param target        The target to append the path to.
                 * @param targetData    Denotes whether \p{target} should be cleared prior to
                 *                      appending the path. Defaults to CurrentData::Clear.
                 * @return The given string to allow concatenated operations
                 */
                AString&    GetPath( AString& target, CurrentData targetData= CurrentData::Clear )
                {
                    ALIB_ASSERT_ERROR( actPath.IsNotNull() , ASTR("Path generation not activated") );

                    if( targetData == CurrentData::Clear )
                        target.Clear();

                    if( actPath.IsNotEmpty() )
                        target._( actPath )._( TSeparator );

                    return target._( Name() );
                }


                /**
                 * Returns the name of the node that this walker currently represents.
                 * The name of the node corresponds to the path segment or child name that was
                 * used to create it.
                 *
                 * \note Unlike method
                 *       \ref aworx::lib::util::StringTree::Cursor::SearchName "Cursor::SearchName"
                 *       which needs some effort to search the name of a node represented by a
                 *       cursor, this method performs in constant time, as the current child name
                 *       is made available during iteration.
                 *
                 * @return The name of the represented node.
                 */
                String Name()
                {
                    ALIB_ASSERT_ERROR( IsValid(), ASTR("Invalid iterator"))

                    RecursionData& nc= stack[ stackSize -1 ];
                    return     nc.actChild == nc.sortedChildren->end() ?  nc.nodeName
                                                                       :  nc.actChild->first;
                }

                /**
                 * Returns the  number of direct children of the currently represented node.
                 * @return The number of children of the represented node.
                 */
                int Size()
                {
                    return static_cast<int>( actNode()->children.size() );
                }

                /**
                 * Returns the depth of the current iteration. This is value is available to the
                 * algorithm which means this method executes in constant time.
                 *
                 * To get the absolute depth of the current node, use a combination of method
                 * #GetCursor and
                 * \ref aworx::lib::util::StringTree::Cursor::Depth "Cursor::Depth".
                 *
                 * @return The distance of the current node and the node of the start of the
                 *         iteration.
                 */
                int Depth()
                {
                    return static_cast<int>( stackSize ) - 1;
                }

                /**
                 * Creates a cursor referencing the same node that this walker references.
                 * @return A cursor object referencing the current node.
                 */
                Cursor GetCursor()
                {
                    return Cursor( stringTree, actNode() );
                }

                /**
                 * Resets a given \b %StdIterator to point to the node this walker currently represents.
                 * Hence allows to re-use an existing \b %StdIterator instance.
                 *
                 * @param it              The \b %StdIterator to change.
                 * @param _recursionDepth The maximum iteration recursion depth.
                 *                        Defaults to \c 0, which specified unlimited recursiveness.
                 * @return The \b %StdIterator passed with parameter \p{it}.
                 */
                StdIterator& SetIterator( StdIterator& it, int _recursionDepth = 0 )
                {
                    it.setStart( actNode(), _recursionDepth );
                    return it;
                }
        };  // inner class "Walker"

    // #############################################################################################
    // StringTree: members
    // #############################################################################################
    protected:

        /// The root node.
        Node*           root;

    // #############################################################################################
    // Constructor/destructor/Clear
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructor
         ******************************************************************************************/
        StringTree()
        {
            root= Node::createRoot( *this );
        }


        /** ****************************************************************************************
         * Destructor
         ******************************************************************************************/
        ~StringTree()
        {
            root->destruct(*this, nullptr);
        }

        /** ****************************************************************************************
         * Clears all nodes and values.
         * Depending on the template type \p{TAllocator} allocator used, it may also declare
         * allocated for future reuse. Therefore is is recommended to use this method instead
         * of "manually" deleting the children of the root node.
         ******************************************************************************************/
        void Clear()
        {
            root->destruct(*this, nullptr);

            TAllocator::AllocatorClear();
            root= Node::createRoot( *this );
        }

        /** ****************************************************************************************
         * Returns an unlimited recursive %StdIterator starting at the root node of this
         * \b %StringTree.
         * @return An recursive %StdIterator starting at the root node.
         ******************************************************************************************/
        StdIterator begin()
        {
            StdIterator it( this );
            it.setStart( root, 0 );
            return it;
        }

        /** ****************************************************************************************
         * Returns a \b %StdIterator pointing to the first character behind this string.
         * @return The end of this string.
         ******************************************************************************************/
        StdIterator end()
        {
            return StdIterator( this );
        }

        /** ****************************************************************************************
         * Creates a cursor instance representing the root node.
         * @return A cursor pointing to the root node of this \b %StringTree.
         ******************************************************************************************/
        Cursor  Root()
        {
            return Cursor( this, root );
        }

        /** ****************************************************************************************
         * A shortcut to method #Root.
         * @return A cursor pointing to the root node of this \b %StringTree.
         ******************************************************************************************/
        Cursor  operator() ()
        {
            return Cursor( this, root );
        }
}; // StringTree



/** ************************************************************************************************
 * This struct is the default implementation of a helper type of class \alib{util,StringTree}.
 * Class \b %StringTree uses this class to allocate memory to store character sequences for child
 * node names when child nodes are created.
 *
 * This default implementation simply allocates and frees the memory needed using C++ keywords
 * \c new and \c delete.
 *
 * See also
 * \ref aworx::lib::util::StringTreeConstKeyAllocator "StringTreeConstKeyAllocator".
 *
 * @tparam T      The value type of elements stored in the nodes of the tree.
 **************************************************************************************************/
template<typename T> struct StringTreeDefaultAllocator
{
    /**
     * Allocates memory and creates a node object.
     * This default implementation allocates the memory simply from the heap using C++ keyword
     * \c new.
     *
     *
     * @tparam TAllocator This template parameter will be set to the corresponding allocator class
     *                    (aka this class itself!) when invoked. The parameter is needed for
     *                    technical reasons: It is not possible to simply specify the name of this
     *                    class here, while it is not completely defined, yet.
     *                    Therefore the template!
     * @return A version of \p{childName} that is safely allocated.
     */
    template<typename TAllocator>
    inline
    typename StringTree<T,TAllocator>::Node*  AllocateNode()
    {
        return new typename StringTree<T,TAllocator>::Node();
    }

    /**
     * Frees memory and invokes destructor of node object.
     * This default implementation simply frees heap memory using C++ keyword \c delete.
     *
     * @param node   The node to delete.
     * @tparam TAllocator See explanation provided with method
     *                    \alib{util::StringTreeDefaultAllocator,AllocateNode}.
     */
    template<typename TAllocator>
    inline
    void    FreeNode( typename StringTree<T,TAllocator>::Node* node )
    {
        delete node;
    }

    /**
     * Invoked when child nodes are created. Needs to return a 'safe' copy of the child name
     * string, ready to be stored as permanent key value.
     *
     * The node value is provided as there might be implementations where the value has already a
     * copy of the child name allocated. In this case, that string can be shared.
     *
     * @param childName The name of child, hence the key string.
     * @param value     The child value that is about to be inserted into the \b StringTree.
     * @return A version of \p{childName} that is safely allocated.
     */
    inline
    String  AllocateKey( const String& childName, T& value )
    {
        (void) value;
        character* buffer= new character[static_cast<size_t>(childName.Length()) ];
        childName.CopyTo( buffer );
        return String( buffer, childName.Length() );
    }


    /**
     * Invoked when child nodes are deleted. Needs to free memory that had been allocated in a
     * corresponding call to #AllocateKey.
     * @param childName The key string used to previously store the node of \p{value}.
     * @param value     The child value that is about to be deleted from \b StringTree.
     */
    inline
    void    FreeKey( const String& childName, T& value )
    {
        (void) value;
        delete[] childName.Buffer();
    }

    /**
     * Invoked with method \alib{util,StringTree::Clear} to tell the allocator that the tree
     * was reset. This default allocator, as well as  \alib{util,StringTreeConstKeyAllocator} do not
     * perform any action with this method. Class \alib{util,StringTreeBlockAllocator} will
     * reset its allocated blocks for reuse.
     */
    inline
    void    AllocatorClear()
    {}
};



/** ************************************************************************************************
 * Implementation of \alib{util,StringTreeDefaultAllocator} which simply does not allocated keys.
 * The original buffer is just returned.
 *
 * This helper can be used in cases when child names and creation path strings used with
 * \alib{util,StringTree} and especially with the node creation methods
 * of class \alib{util,StringTree::Cursor} are 'safely' allocated already.
 * This is for example true in the trivial case of using literal strings for child creation.
 *
 * @tparam T      The value type of elements stored in the nodes of the tree.
 **************************************************************************************************/
template<typename T> struct StringTreeConstKeyAllocator
{
    /**
     * Allocates memory and creates a node object.
     * This implementation allocates the memory simply from the heap using C++ keyword
     * \c new.
     *
     *
     * @tparam TAllocator See explanation provided with method
     *                    \alib{util,StringTreeDefaultAllocator::AllocateNode}.
     * @return A version of \p{childName} that is safely allocated.
     */
    template<typename TAllocator>
    inline
    typename StringTree<T,TAllocator>::Node*  AllocateNode()
    {
        return new typename StringTree<T,TAllocator>::Node();
    }

    /**
     * Frees memory and invokes destructor of node object.
     * This implementation simply frees heap memory using C++ keyword \c delete.
     *
     * @param node   The node to delete.
     * @tparam TAllocator See explanation provided with method
     *                    \alib{util,StringTreeDefaultAllocator::AllocateNode}.
     */
    template<typename TAllocator>
    inline
    void    FreeNode( typename StringTree<T,TAllocator>::Node* node )
    {
        delete node;
    }

    /**
     * Just returns the given string.
     * @param childName The name of child, hence the key string.
     * @return The original string which is safely allocated already.
     */
    inline
    String  AllocateKey( const String& childName, T& )
    {
        return childName;
    }

    /**
     * Empty implementation.
     * See \alib{util,StringTreeDefaultAllocator::FreeKey} for details.
     */
    inline
    void    FreeKey( const String&, T& )
    {}

    /**
     * Empty implementation.
     * See \alib{util,StringTreeDefaultAllocator::AllocatorClear} for details.
     */
    inline
    void    AllocatorClear()
    {}
};

/** ************************************************************************************************
 * Implementation of \alib{util,StringTreeDefaultAllocator} which uses a
 * \alib{util,MemoryBlocks} object to allocate the keys.
 *
 * This helper can be used in cases when a string tree only grows over time and now branches are
 * replaced.
 *
 * @tparam T            The value type of elements stored in the nodes of the tree.
 * @tparam TBlockSize   The size of the memory chunks to allocate for key strings.
 **************************************************************************************************/
template<typename T, int TBlockSize> struct StringTreeBlockAllocator
{
    /// The allocator object.
    MemoryBlocks    memoryBlocks;

    StringTreeBlockAllocator()
    : memoryBlocks( TBlockSize )
    {}

    /**
     * Allocates memory from block allocator and and creates a node object (with placement new).
     * This implementation allocates the memory in field #memoryBlocks.
     *
     *
     * @tparam TAllocator See explanation provided with method
     *                    \alib{util,StringTreeDefaultAllocator::AllocateNode}.
     * @return A version of \p{childName} that is safely allocated.
     */
    template<typename TAllocator>
    inline
    typename StringTree<T,TAllocator>::Node*  AllocateNode()
    {
        return new ( memoryBlocks.Alloc<typename StringTree<T,TAllocator>::Node>() )
                   typename StringTree<T,TAllocator>::Node();
    }

    /**
     * Explicitly invokes destructor of the given node object. The memory remains allocated in the
     * #memoryBlocks.
     *
     * @param node   The node to delete.
     * @tparam TAllocator See explanation provided with method
     *                    \alib{util,StringTreeDefaultAllocator::AllocateNode}.
     */
    template<typename TAllocator>
    inline
    void    FreeNode( typename StringTree<T,TAllocator>::Node* node )
    {
        node->~Node();
    }

    /**
     * Allocates the given key string in field #memoryBlocks.
     * @param childName The name of child, hence the key string.
     * @return The original string which is safely allocated already.
     */
    inline
    String AllocateKey( const String& childName, T& )
    {
        return memoryBlocks.AllocAndCopy( childName );
    }

    /**
     * Empty implementation.
     */
    inline
    void FreeKey( const String&, T& )
    {}

    /**
     * Resets the memory blocks for re-use (aka calls \alib{util,MemoryBlocks::Clear} on field
     * #memoryBlocks).
     *
     * See \alib{util,StringTreeDefaultAllocator::AllocatorClear} for details.
     */
    inline
    void    AllocatorClear()
    {
        memoryBlocks.Clear( CurrentData::Keep );
    }
};

/**
 * Creates a \b %StdIterator on the given \b %StringTree.
 * @param stringTree The object to iterate.
 * @return The \b %StdIterator
 */
template<typename T, typename TAllocator>
typename StringTree<T,TAllocator>::StdIterator begin(StringTree<T,TAllocator>& stringTree )
{
    return stringTree.begin();
}

/**
 * Creates an end-\b %StdIterator.
 * @param stringTree The object to iterate.
 * @return The \b %StdIterator
 */
template<typename T, typename TAllocator>
typename StringTree<T,TAllocator>::StdIterator end(StringTree<T,TAllocator>&   stringTree )
{
    return stringTree.end();
}

/**
 * Creates a \b %StdIterator on the given \b %Cursor.
 * @param cursor The object to iterate.
 * @return The \b %StdIterator
 */
template<typename T, typename TAllocator>
typename StringTree<T,TAllocator>::StdIterator begin(typename StringTree<T,TAllocator>::Cursor& cursor )
{
    return cursor.begin();
}

/**
 * Creates an end-\b %StdIterator.
 * @param cursor The object to iterate.
 * @return The \b %StdIterator
 */
template<typename T, typename TAllocator>
typename StringTree<T,TAllocator>::StdIterator end(typename StringTree<T,TAllocator>::Cursor&   cursor )
{
    return cursor.end();
}



}}// namespace aworx::[lib::util]

/// Type alias in namespace #aworx.
template<typename T, typename TAllocator= aworx::lib::util::StringTreeDefaultAllocator<T>>
using  StringTree=    aworx::lib::util::StringTree<T, TAllocator>;



} // namespace [aworx]


#endif // HPP_ALIB_CONTAINERS_STRINGTREE

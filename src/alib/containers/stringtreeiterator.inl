//==================================================================================================
/// \file
/// This header-file is part of module \alib_containers of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace containers {

/// This class is to be used with instances of class \alib{containers;StringTree} and allows
/// iterating recursively through its nodes.<br>
/// The type does <b>not</b> apply to the concept of \c std::iterator_traits.
/// The rationale for this is the fact that mechanics for sorting the child nodes are
/// provided, which requires the allocation of more resources than usual container iterators do.
/// Therefore, objects of this type are not supposed to be temporary and
/// created "on the fly", e.g., in C++ range-based loops.
/// Instead, instances should rather be created once and then re-used with later iterations.
///
/// The sorting of child nodes is optional and can be changed before each recursion.
/// Whenever a recursion in iteration occurs, the most recent settings of sorting are respected
/// for the children of the node that is processed with that recursion.<br>
/// A built-in comparison function which works on node names (path names) allows choosing
/// ascending and descending order and to ignore or be sensitive about the letter case.
/// Besides this, custom comparison functions that take a combination of arbitrary node
/// attributes, including a node's value of template type \p{T} can be established.
/// See method #SetSorting for details on this topic.
///
/// Method #Initialize starts a new 'use' of this class. Besides the start node, a boolean
/// parameter allows deciding whether the start node should be included in the iteration or
/// not. This is useful in cases where the start-node could optionally be a leaf-node.
/// For example, when processing files with class \alib{files;FTree}, an application might
/// allow accepting a single file or a folder that contains files. In this case, the
/// iteration should include the start node, as otherwise, in case a file was given, that
/// leaf-node would be skipped.
///
/// The maximum depth of recursion may be limited with optional parameter \p{depth}
/// found with each overloaded version of #Initialize.
/// During the iteration, the recursion can be individually selected per node visited.
/// This is done by using either of the methods #Next or #NextSibling to proceed.
/// Furthermore, the method #NextParentSibling allows skipping the rest of the current iteration
/// branch.<br>
/// The end of an iteration is detected with the method #IsValid.
///
/// Finally, the generation of a string representing the actual path to the current
/// iteration node, relative to the iteration's start node, can be activated.
/// See method #SetPathGeneration for more information about this feature.
///
/// \see
///   Further information on how this class is used are given with paragraph
///   \ref alib_ns_containers_stringtree_iterator "2.2 Class StringTreeIterator"
///   of the description of class \b %StringTree.
///
/// @tparam TStringTree  The \alib{containers;StringTree} that this class is working with.
template<typename TStringTree>
class StringTreeIterator {
    #if !DOXYGEN
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            #undef   DCS
            #undef   DCSSHRD
            #define  DCS       ALIB_DCS_WITH(       tree->DbgGetDCS())
            #define  DCSSHRD   ALIB_DCS_SHARED_WITH(tree->DbgGetDCS())
        #endif
    #endif

  public:
    /// Publicly Exposes template parameter \p{TStringTree}.
    using                 StringTreeType= TStringTree;

    /// Evaluates to \c true if the given template parameter \p{TStringTree} is a constant type.
    static constexpr bool IsConst   = std::is_const_v<TStringTree>;

    /// Publicly exposes the cursor type that is used to walk the tree.
    /// Evaluates to the constant or mutable types \alib{containers;StringTree::Cursor} or
    /// or \alib{containers;StringTree::ConstCursor}.
    using                 CursorType  = std::conditional_t< !IsConst,
                                                            typename StringTreeType::Cursor,
                                                            typename StringTreeType::ConstCursor>;

    /// Exposes \alib{containers;StringTree::CharacterType} #StringTreeType.
    using CharacterType     = typename StringTreeType::CharacterType;

  protected:
    /// Constant or mutable version of the base tree type, depending on template parameter
    /// \p{TStringTree}
    using                 cursorHandle= std::conditional_t<!IsConst,
                                                       typename StringTreeType::CursorHandle,
                                                       typename StringTreeType::ConstCursorHandle>;


  //############################################# Sorter ###########################################
  public:

    /// Abstract base type to be used to implement custom sorting.
    /// One simple built-in descendant is provided with struct #NameSorter.
    struct Sorter {
        /// Necessary virtual destructor.
        virtual ~Sorter()                                                                  =default;

        /// Abstract method which needs to be implemented by descendants.
        /// @param lhs  The left-hand side cursor to the node to compare.
        /// @param rhs  The right-hand side cursor to the node to compare.
        /// @return Needs to return \c true if \p{lhs} is 'smaller' than \p{rhs}, and \c false
        ///         otherwise.
        virtual bool  Compare( const StringTreeType::ConstCursor& lhs,
                               const StringTreeType::ConstCursor& rhs)                           =0;
    };

    /// Built-in descendant of struct #Sorter used to perform simple sorting based on
    /// the name of <b>StringTree</b>-nodes.
    struct NameSorter : Sorter {
        /// Unless changed by the caller, this is copied with every recursion step.
        bool                    Descending                                                   =false;

        /// Unless changed by the caller, this is copied with every recursion step.
        bool                    CaseSensitive                                                =false;

        /// Implementation of the abstract method. Compares the names of the nodes using
        /// the string method \alib{strings::TString;CompareTo;String;CompareTo}.
        /// @param lhs  The left-hand side cursor to the node to compare.
        /// @param rhs  The right-hand side cursor to the node to compare.
        /// @return Returns \c true if \p{lhs} is 'smaller' than \p{rhs}, and \c false otherwise.
        bool Compare(const StringTreeType::ConstCursor& lhs,
                     const StringTreeType::ConstCursor& rhs)                              override {

            int compResult=  CaseSensitive
                             ? lhs.Name().template CompareTo<CHK,lang::Case::Sensitive>(rhs.Name())
                             : lhs.Name().template CompareTo<CHK,lang::Case::Ignore   >(rhs.Name());
            return Descending ? compResult > 0
                              : compResult < 0;
        }
    };

  //#################################### Inner type RecursionData ##################################
  protected:
    /// Protected, internal struct used to store the data of recursive iterations.
    struct RecursionData
    {
        /// Combines a node hook and a current vector index used to remember the
        /// current child in unsorted, respectively sorted mode.
        union ActChildIdentifier
        {
            /// The current child of the current node in case of unsorted access.
            /// If this is pointing to the end of the child map, then
            /// the actual node itself is selected by this StringTreeIterator.
            cursorHandle          unsortedHandle;

            /// The current child index in case of sorted access.
            /// A value of <c>size_t(-1)</c> indicates that
            /// the actual node itself is selected.
            size_t                sortedIdx;
        };

        /// The actual child handle, respectively index.
        ActChildIdentifier        actChild;

        /// The child hook of the parent node, used with unsorted iteration.
        /// Note that this is declared \c const, in case \c constexpr field \p{IsConst}
        /// equals \c true.
        cursorHandle              unsortedParent;

        /// A pointer to a dynamically allocated vector of children used with sorting.
        std::vector<cursorHandle> sortedChildren;

        /// The (optional) sorter used with the actual node.
        /// Unless changed by the caller, this is copied with every recursion step.
        Sorter*                   sorter;

        /// The path string length of the actual recursion depth.
        integer                   pathStringLen;

        /// Trivial default constructor.
        RecursionData()                                                           noexcept =default;

        /// Trivial default copy constructor.
        RecursionData( const RecursionData&  )                                             =default;

        /// Trivial default move constructor.
        RecursionData(       RecursionData&& )                                    noexcept =default;

        /// Trivial default copy assign operator.
        /// @return A reference to \c this.
        RecursionData& operator=( const RecursionData&  )                                  =default;

        /// Trivial default move assign operator.
        /// @return A reference to \c this.
        RecursionData& operator=(       RecursionData&& )                         noexcept =default;

        /// Trivial default destructor.
        ~RecursionData()                                                          noexcept =default;
    }; // inner struct RecursionData

  //############################################ members ###########################################
    /// The \b %StringTree that this iterator works on.
    TStringTree*                        tree                                               =nullptr;

    /// The pointer to the actual node.
    cursorHandle                        node;

    /// A stack holding the recursive list of unsorted or sorted children and the
    /// hook to the current child. Implemented as a vector in combination with member #actDepth,
    /// to reuse allocated storage space during iteration and when this iterator is
    /// re-used (freshly initialized).
    std::vector<RecursionData>          stack;

    /// The path to the actual node (excluding the name of the actual node).
    /// If this object is \e nulled, no paths are generated.
    strings::TAString<CharacterType>    actPath;

    /// The current depth of the iteration (and usage but not size of field #stack).
    /// set to \c -1 to if iteration is finished, respectively this iterator was not
    /// initialized.
    int                                 actDepth                                                =-1;

    /// The requested maximum depth of iteration recursion.
    unsigned                            maxDepth        =(std::numeric_limits<unsigned int>::max)();

    /// A pointer to a user-defined comparison object used with the next iteration.
    Sorter*                             nextSorter                                         =nullptr;

//###################################### Constructor/Destructor ####################################
  public:
    /// Default constructor.
    StringTreeIterator()                                                                   =default;

    /// Trivial copy constructor.
    StringTreeIterator( const StringTreeIterator&  )                                       =default;

    /// Trivial default move constructor.
    StringTreeIterator(       StringTreeIterator&& )                                       =default;

    /// Trivial default copy assign operator.
    /// @return A reference to \c this.
    StringTreeIterator& operator    =( const StringTreeIterator&  )                        =default;

    /// Trivial default move assign operator.
    /// @return A reference to \c this.
    StringTreeIterator& operator    =(       StringTreeIterator&& )                        =default;


    /// Destructor.
    ~StringTreeIterator()                                                                  =default;

  //########################################### Interface ##########################################
    /// With this method, the assembly of a string representing the absolute path of the actual
    /// node is activated or deactivated.<br>
    /// If activated, the path to the current node can be received with the method #Path.
    ///
    /// Note that, for technical reasons, the invocation of the method invalidates this iterator.
    /// @param pathGeneration Denotes whether the path should be generated and retrievable or not.
    void        SetPathGeneration( lang::Switch pathGeneration ) {
        Invalidate();
        actPath.Reset( pathGeneration == lang::Switch::On ? EMPTY_STRING
                                                          : NULL_STRING  );
    }

    /// Resets this iterator to the first child of the node that the given cursor
    /// object represents.<br>
    /// If the given node has no children, this iterator is marked invalid when this method returns,
    /// unless param \p{includeStartNode} is set to \c true. In the latter case, at least the
    /// start node is part of the iteration.
    /// @param startNode        The cursor that defines the branch of the tree to be iterated.
    ///                         In debug-builts it is asserted that this instance is valid.
    /// @param includeStartNode Denotes whether the \p{startNode} is included in the iteration or
    ///                         not. If so, the start node will be the first node visited.
    void        Initialize( CursorType startNode, lang::Inclusion includeStartNode ) {
        ALIB_ASSERT_ERROR( startNode.IsValid(), "STRINGTREE", "Invalid start-node given." )
        this->tree= &startNode.template Tree<TStringTree>();
        stack.clear();
        DCSSHRD
        if( actPath.IsNotNull() )
            startNode.AssemblePath(actPath, lang::CurrentData::Clear);

        node= startNode.Export();

        if ( includeStartNode == lang::Inclusion::Include) {
            RecursionData& rd= stack.emplace_back();
            node= startNode.Export();
            rd.actChild.sortedIdx= 0;
            rd.sortedChildren.emplace_back(node);
            actDepth= 0;
            return;
        }


        if( startNode.HasChildren() ) {
            actDepth= -1;
            recursion();
        }
        else
            actDepth= -1;
    }

    /// Invalidates this object. After invoking this method, this iterator cannot be
    /// used further until one of the overloaded methods #Initialize is invoked.
    /// After the invocation, the method #IsValid will return \c false.
    void Invalidate()                                                              { actDepth= -1; }

    /// Determines if this instance is valid. \b StringTreeIterator instances may become invalid
    /// after invocations of one of the methods #Next, #NextSibling or #NextParentSibling
    /// (at the end of the iteration) and become valid with the invocation of one of the
    /// overloaded methods #Initialize.
    ///
    /// @return \c true if this is a valid iterator. If invalid, \c false is returned and
    ///         the iterator must not be evaluated before being initialized.
    bool        IsValid()                                           const { return actDepth != -1; }

    /// The negation of #IsValid.
    ///
    /// @return \c false if this is a valid iterator. If invalid, \c true is returned and
    ///         the iterator must not be evaluated before being initialized.
    bool        IsInvalid()                                             const { return !IsValid(); }


    /// Sets a sorter instance which is used for any next recursion step.
    ///
    /// This method may be invoked at any time, even on invalid iterators and those that are not
    /// initialized, yet. The given \p{sorter} is stored for future use.
    /// Such a use happens whenever a recursive iteration over a list of child nodes is
    /// started. At that moment the current configuration of sorting is applied to
    /// the list of direct children.
    /// @param sorter  A custom comparison method used for sorting the children
    ///                of nodes.
    void        SetSorting(Sorter *sorter)                                   { nextSorter= sorter; }

    /// Iterates to the first child of the current node. If no such child exists,
    /// to the next sibling node. If also no sibling exists, iteration continues
    /// with the next available node of a previous recursion level.
    ///
    /// @return \c true if a next node was found, \c false otherwise.
    ///         If \c false is returned, this iterator is invalid after the call.
    bool        Next()                                                   { DCSSHRD return next(0); }

    /// Omits recursion on the current node's children, even if the current depth
    /// is lower than #MaxDepth.<br>
    ///
    /// If no sibling exists, iteration continues with the next available node of a
    /// previous recursion level.
    ///
    /// @return \c true if a next node was found, \c false otherwise.
    ///         If \c false is returned, this iterator is invalid after the call.
    bool        NextSibling()                                            { DCSSHRD return next(1); }

    /// Skips the remaining siblings of the current recursion level and continues with
    /// the next available sibling of a previous level.
    ///
    /// @return \c true if a next node was found, \c false otherwise.
    ///         If \c false is returned, this iterator is invalid after the call.
    bool        NextParentSibling()                                      { DCSSHRD return next(2); }


    /// Retrieves the current path of walking as a string representation. The path returned is
    /// absolute with a leading separator character.
    ///
    /// Note that this method can be used only if path generation was activated before the current
    /// iteration. Activation is performed with the method SetPathGeneration.
    ///
    /// @return The path of the current node.
    const TStringTree::NameType     Path()                                                   const {
        ALIB_ASSERT_ERROR( actPath.IsNotNull(), "STRINGTREE", "Path generation not activated" )
        return actPath;
    }

    /// Returns the requested maximum depth of iteration, set with #Initialize.
    /// @see For the current iteration depth, use #CurrentDepth.
    /// @return The distance of the current node and the node of the start of the
    ///         iteration.
    unsigned    MaxDepth()                                                const { return maxDepth; }

    /// Changes the maximum depth of iteration. This method may be invoked any time, also after
    /// iteration has started.
    /// @param newMaxDepth The maximum depth to use from now on.
    ///                    Defaults to the maximum <c>unsigned int</c> value.
    void        SetMaxDepth(unsigned int newMaxDepth= (std::numeric_limits<unsigned>::max)())
    { maxDepth= newMaxDepth; }

    /// Returns the depth of the current iteration. This is value is available to the
    /// algorithm, which means this method executes in constant time.
    ///
    /// To get the absolute depth of the current node, the method
    /// \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;ac0ea9d0558dbd3565be5051fec3222fb;TCursor::Depth},
    /// may be used.
    ///
    /// @return The distance of the current node and the node of the start of the
    ///         iteration.
    unsigned    CurrentDepth()                                                               const {
        ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                           "StringTreeIterator not initialized or exceeded (invalid)." )
        return unsigned(actDepth);
    }

    /// Returns the current node, encapsulated in a cursor object.
    ///
    /// \note
    ///   It is \b not allowed to use the method
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a04cf02f30f8a652bb40cf889b8b9e61e;TCursor::Delete}
    ///   on the node returned by this method.
    ///   As a replacement, use the method #DeleteNode provided with this class.<br>
    ///   However, the methods
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;ae406201cd8da023cf9bf50df25626153;TCursor::DeleteChild} and
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;af655aeecbd2712df8db94a340a8385ee;TCursor::DeleteChildren}
    ///   are allowed
    ///   to be invoked and therefore have no replacement in this class.
    ///
    /// @return An instance of the public node interface pointing to the currently
    ///         referenced tree node.
    CursorType  Node()                                                                       const {
        ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                           "StringTreeIterator not initialized or exceeded (invalid)." )
        return tree->ImportCursor(node);
    }

  //######################################### node deletion ########################################

    /// Deletes the node that this iterator currently refers to from the tree.
    /// After the operation, the iterator is moved forward to the next sibling
    /// of the current node, respectively of the first sibling found in the
    /// recursion stack.
    ///
    /// \note
    ///   This method constitutes a legal alternative to method
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;a04cf02f30f8a652bb40cf889b8b9e61e;TCursor::Delete},
    ///   which is forbidden to be invoked on the node returned by method #Node as this would
    ///   invalidate this iterator.<br>
    ///   Methods
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;ae406201cd8da023cf9bf50df25626153;TCursor::DeleteChild} and
    ///   \doxlinkproblem{classalib_1_1containers_1_1StringTree_1_1TCursor.html;af655aeecbd2712df8db94a340a8385ee;TCursor::DeleteChildren}
    ///   are allowed with this iterator type.
    ///   Consequently, no replacement method for those is given with this class.
    ///
    /// @return The total number of nodes deleted.
    uinteger    DeleteNode()                                                                    {DCS
        ALIB_ASSERT_ERROR( IsValid(), "STRINGTREE",
                           "StringTreeIterator not initialized or exceeded (invalid)." )
        auto nodeToDelete= node;
        next( 1 ); // next sibling
        auto ntd=  tree->ImportCursor(nodeToDelete);
        return ntd.Parent().DeleteChild( ntd );
    }

  //################################## StringTreeIterator Internals ################################
  protected:
    /// Sets this iterator to point to the first child of the actual node.
    /// If sorting is enabled, copies all children from the map to a vector and sorts
    /// them there.
    void  recursion() {
        ++actDepth;
        if( stack.size() == size_t(actDepth) )
            stack.emplace_back();

        auto& rd=  stack[size_t(actDepth)];

        auto cursor= tree->ImportCursor(node);
        rd.sorter= nextSorter;

        // no sorting: set link to node's child hook
        if (!rd.sorter) {
            rd.unsortedParent= tree->ImportCursor(node).FirstChild().Export();
            node= (rd.actChild.unsortedHandle= rd.unsortedParent);
        } else {

            // sorting: copy children to a sortable vector
            rd.sortedChildren.clear();
            rd.sortedChildren.reserve( size_t( cursor.CountChildren() ) );
            cursor.GoToFirstChild();
            while( cursor.IsValid() ) {
                rd.sortedChildren.emplace_back( cursor.Export() );
                cursor.GoToNextSibling();
            }

            // sort
            std::sort( rd.sortedChildren.begin(), rd.sortedChildren.end(),
                       [this,&rd]( cursorHandle lhs,
                                   cursorHandle rhs  )
                       {
                            return rd.sorter->Compare( tree->ImportCursor(lhs),
                                                       tree->ImportCursor(rhs) );
                       }
                      );

            // set to first child
            rd.actChild.sortedIdx= 0;
            node= rd.sortedChildren[0];
        }

        // add path information
        if ( actPath.IsNotEmpty() ) {
            rd.pathStringLen= actPath.Length();
            if ( rd.pathStringLen == 1 )    rd.pathStringLen= 0;
            else                            actPath.template _<NC>( tree->Separator() );

            actPath.template _<NC>( tree->ImportCursor(node).Name() );
    }   }


    /// Goes to the next node.
    /// This method is used with interface methods #Next, #NextSibling and
    /// #NextParentSibling, as well as with #DeleteNode}.
    ///
    /// @param skipMode   \c 0: iterates to the first child (if available),
    ///                   \c 1: iterates to the next sibling (if available) and
    ///                   \c 2: to the next available sibling of the parent, respectively the
    ///                         current recursion stack.
    /// @return \c true if this iterator is valid (a next node was found), \c false
    ///         otherwise.
    bool next(int skipMode) {
        ALIB_ASSERT_ERROR( actDepth != -1, "STRINGTREE", "Invalid iterator" )

        auto cursor= tree->ImportCursor(node);

        // recursion to first child of actual node?
        if(    skipMode == 0
            && unsigned(actDepth) < maxDepth
            && cursor.CountChildren() )
        {
            // increase stack capacity
            if( stack.size() == size_t(actDepth) + 1 )
                stack.emplace_back();

            recursion();

            return true;
        }

        for(;;) {
            if( skipMode != 2 ) {
                // next sibling
                RecursionData& rd= stack[ size_t(actDepth) ];
                if( rd.sortedChildren.size() ) {
                    ++rd.actChild.sortedIdx;
                    if( rd.actChild.sortedIdx < rd.sortedChildren.size() ) {
                        node= rd.sortedChildren[rd.actChild.sortedIdx];
                        break;
                    }
                } else {
                    auto next= tree->ImportCursor(rd.actChild.unsortedHandle).NextSibling();
                    node= (rd.actChild.unsortedHandle= next.Export());
                    if ( next.IsValid() )
                        break;
            }   }

            skipMode= 0;

            // climb down
            if (--actDepth == -1 )
                return false;
        }

        // adjust path
        if ( actPath.IsNotEmpty() ) {
            actPath.ShortenTo(stack[ size_t(actDepth) ].pathStringLen + 1)
                   .template _<NC>(tree->ImportCursor(node).Name());
        }

       return true;
    }

    #if !DOXYGEN
        #if ALIB_DEBUG_CRITICAL_SECTIONS
            #undef   DCS
            #undef   DCSSHRD
        #endif
    #endif

};  // class "StringTreeIterator"

} // namespace alib::[containers]

/// Type alias in namespace \b alib.
template<typename TTree>
using      StringTreeIterator = containers::StringTreeIterator<TTree>;

} // namespace [alib]

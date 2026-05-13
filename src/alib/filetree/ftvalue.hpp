//==================================================================================================
/// \file
/// This header-file is part of module \alib_filetree of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace filetree {

class FTree;
class FTFile;
    
/// The entry type which is embedded in each tree node.
/// Extends class #"FileStatus" of \alib_system mainly by #".DirectorySums", some extended
/// entry information and a possible custom value.
class FTValue  : public system::FileStatus {
  protected:
  #if !DOXYGEN
    friend class FTree;     ///< Allow access from within class #"%FTree".
    friend class FTFile;    ///< Allow access from within class #"%FTFile".
  #endif

  public:
    /// Recursively accumulated values for directories.
    struct  DirectorySums {
        uinteger    Size                                         = 0;  ///< The cumulated sizes of all files and directories.
        std::array<uint32_t, size_t(Types::MARKER_TYPES_END)> TypeCounters= {0,0,0,0,0,0,0,0}; ///< Per-type counters.
        uint32_t    QtyErrsAccess                                = 0;  ///< Number of access errors in the folder and subfolders.
        uint32_t    QtyErrsBrokenLink                            = 0;  ///< Number of broken symbolic links in the directory and its subfolders.
        uint32_t    QtyStopsOnMaxDepth                           = 0;  ///< Number of recursion aborts due to reach of maximum recursion depth.
        uint32_t    QtyStopsOnCircularLinks                      = 0;  ///< Number of recursion aborts due to detected circular links reach of maximum recursion depth.


        /// Defaulted default constructor.
        constexpr   DirectorySums()                                               noexcept =default;

        /// Adds the values in the given summary object to this.
        /// @param rhs The values to add.
        /// @return A reference to <c>this</c>
        DirectorySums& operator+= (const DirectorySums& rhs) {
            Size                   += rhs.Size              ;
            for (size_t i = 0; i < size_t(Types::MARKER_TYPES_END); ++i)
                TypeCounters[i]+= rhs.TypeCounters[i];
            QtyErrsAccess          += rhs.QtyErrsAccess     ;
            QtyErrsBrokenLink      += rhs.QtyErrsBrokenLink ;
            QtyStopsOnMaxDepth     += rhs.QtyStopsOnMaxDepth;
            QtyStopsOnCircularLinks+= rhs.QtyErrsBrokenLink ;
           return *this;
        }

        /// Subtracts the values in the given summary object from this.
        /// @param rhs The values to subtract.
        /// @return A reference to <c>this</c>
        DirectorySums& operator-= (const DirectorySums& rhs) {
            Size                   -= rhs.Size              ;
            for (size_t i = 0; i < size_t(Types::MARKER_TYPES_END); ++i)
                TypeCounters[i]-= rhs.TypeCounters[i];
            QtyErrsAccess          -= rhs.QtyErrsAccess     ;
            QtyErrsBrokenLink      -= rhs.QtyErrsBrokenLink ;
            QtyStopsOnMaxDepth     -= rhs.QtyStopsOnMaxDepth;
            QtyStopsOnCircularLinks-= rhs.QtyErrsBrokenLink ;
           return *this;
        }
        /// Returns \c true if the given \p{type} equals either
        /// #"Types::DIRECTORY" or
        /// #"Types::SYMBOLIC_LINK_DIR"
        /// @param type returns \c false if the given type does not represent a directory
        ///            and \c true if \p{type} equals
        ///            #"Types::DIRECTORY" or
        ///            #"Types::SYMBOLIC_LINK_DIR"
        /// @return \c false if the given type does not represent a directory, \c true otherwise.
        constexpr bool IsDirType(Types type)             const noexcept { return    int(type) < 2; }

        /// Adds a file/directory to the counters
        /// @param finfo The entry to add.
        /// @return A reference to <c>this</c>
        constexpr DirectorySums& Add(const FTValue& finfo)                                noexcept {
            ++TypeCounters[size_t(finfo.Type())];
            Size+= finfo.Size();
            return *this;
        }


        /// Returns the cumulated number of entries (of any type).
        /// @return The number of entries counted.
        uint32_t Count()                                                            const noexcept {
            uint32_t result= 0;
            for (size_t i = 0; i < size_t(Types::MARKER_TYPES_END); ++i)
                result+= TypeCounters[i];
            return result;
        }

        /// Returns the number of entries of the given \p{type}.
        /// @param type The type to get the number of entries for.
        /// @return The number of directories or symbolic links to directories.
        uint32_t Count(Types type)                                                  const noexcept {
            ALIB_ASSERT_ERROR( type < Types::MARKER_TYPES_END,
               "FILETREE", "Cant get count for file type \"{}\"", type )
            return    TypeCounters[size_t(type)];
        }

        /// Returns the sum of the number of entries of type
        /// #"Types::DIRECTORY" and
        /// #"Types::SYMBOLIC_LINK_DIR"
        /// @return The number of directories or symbolic links to directories.
        uint32_t CountDirectories()                                                 const noexcept {
            return    TypeCounters[size_t(Types::DIRECTORY)]
                    + TypeCounters[size_t(Types::SYMBOLIC_LINK_DIR)];
        }

        /// Returns the sum of the number of entries which are \b not of type
        /// #"Types::DIRECTORY" and
        /// #"Types::SYMBOLIC_LINK_DIR"
        /// @return The number of regular files, fifo, sockets, etc.
        uint32_t CountNonDirectories()                                              const noexcept {
            uint32_t result= 0;
            for (size_t i = 2; i < size_t(Types::MARKER_TYPES_END); ++i)
                result+= TypeCounters[i];
            return result;
        }

    }; // struct DirectorySums

    /// Base type to create pointers to different extended entry information structs.
    struct ExtendedEntryInfo
    {};

    /// Additional information for entries of directory-type. Allocated in the tree's
    /// #"MonoAllocator" and accessible via #"GetExtendedInfo()" and #".Sums".
    struct  EIDirectory : ExtendedEntryInfo {
        DirectorySums Sums;  ///< The recursive sums evaluated during scan.
    };

    /// Additional information for entries of symlink-type. Allocated in the tree's
    /// #"MonoAllocator" and accessible via #"GetExtendedInfo()",
    /// #"GetLinkTarget" and #"GetRealLinkTarget".
    struct  EISymLinkFile : ExtendedEntryInfo {
        CPathString Target;      ///< The target path. This is a zero-terminated #"%CString".
        CPathString RealTarget;  ///< The resolved real target path.
    };

    /// Additional information for entries of the symbolic link type. Allocated in the tree's
    /// #"MonoAllocator" and accessible via #"GetExtendedInfo",
    /// #"GetLinkTarget", #"GetRealLinkTarget" and #".Sums".
    struct  EISymLinkDir : EISymLinkFile {
        DirectorySums Sums; ///< The recursive sums evaluated during scan.
    };

  protected:
    /// Extended information, depending on the entry type.
    ExtendedEntryInfo*        extendedInfo                                                = nullptr;

    /// Pool-allocated custom data.
    void*                     custom                                                      = nullptr;

    #if ALIB_DEBUG
    /// The custom type attached. Used for asserting misuse in debug-compilations.
    const std::type_info*     dbgCustomType                                               = nullptr;
    #endif


  public:
    /// Retrieves the extended info object of this entry.
    /// @return The extended info object of this entry. If not available \c nullptr is returned.
    [[nodiscard]] constexpr ExtendedEntryInfo*  GetExtendedInfo()     const { return extendedInfo; }

    /// Sets the extended information object. As with all set functions, this method should only
    /// be used from certain code entities, like file scanners. If used, the object passed here
    /// has to be pool-allocated using public instance #"FTree::Pool;*".
    /// The object will be freed with the deletion of the corresponding string tree node
    /// (respectively #"%FTFile" instance).
    /// @param extInfo A pointer to the information object to use.
    constexpr void          SetExtendedInfo(ExtendedEntryInfo* extInfo)   { extendedInfo= extInfo; }

    /// Retrieves the directory sums of this directory or symbolic link to directory.
    /// @return A reference to the sums.
    [[nodiscard]] constexpr DirectorySums&  Sums()                                           const {
        #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
            ALIB_ASSERT_ERROR( IsDirectory(), "FILETREE",
                          "Requesting sums for FTValue that is not a directory.")
            ALIB_ASSERT_ERROR( extendedInfo != nullptr, "FILETREE",
                          "Requesting sums for FTValue that has no sums set. ScanState: ", ScanState() )
        #endif
        if( Type() == FileStatus::Types::DIRECTORY )
            return static_cast<EIDirectory*>(extendedInfo)->Sums;
        return static_cast<EISymLinkDir*>(extendedInfo)->Sums;
    }

    /// Sets the sums of the extended info object of this entry.
    /// @param sums The sums to set.
    constexpr void          SetSums(const DirectorySums& sums)                               const {
        if( Type() == FileStatus::Types::DIRECTORY ) {
            static_cast<EIDirectory*>(extendedInfo)->Sums= sums;
            return;
        }

        ALIB_ASSERT_ERROR( Type() == FileStatus::Types::SYMBOLIC_LINK_DIR, "FILETREE",
                      "Given node is not a directory or symbolic link pointing to a directory.")

        static_cast<EISymLinkDir*>(extendedInfo)->Sums= sums;
    }

    /// Stores the link targets in the extended information object created for symbolic links.
    /// @param tree       The tree that this object belongs to.
    /// @param target     The target as stored in the symlink
    /// @param realTarget The translated, 'real' target path (if not broken).
    void                    SetLinkTarget(FTree& tree, const PathString& target,
                                                       const PathString& realTarget);

    /// Retrieves the non-translated target of a symbolic link. In debug compilations, the method
    /// asserts that #"FileStatus::Type" returns either #"Types::SYMBOLIC_LINK"
    /// or #"Types::SYMBOLIC_LINK_DIR".
    /// @return A reference to a copy of the zero-terminated string stored in the symbolic link file.
    [[nodiscard]] CPathString&  GetLinkTarget()                                     const noexcept {
        ALIB_ASSERT_ERROR(    Type() == FileStatus::Types::SYMBOLIC_LINK
                           || Type() == FileStatus::Types::SYMBOLIC_LINK_DIR,
                          "FILETREE", "Given node is not a symbolic link."  )
        return static_cast<EISymLinkFile*>(extendedInfo)->Target;
    }

    /// Retrieves the resolved target of a symbolic link. In debug compilations, the method
    /// asserts that #"FileStatus::Type" returns either #"Types::SYMBOLIC_LINK" or
    /// #"Types::SYMBOLIC_LINK_DIR".
    /// @return A reference to a zero-terminated string giving the translated real path that a
    ///        symbolic link points to.
    [[nodiscard]] CPathString&  GetRealLinkTarget()                                 const noexcept {
        ALIB_ASSERT_ERROR(    Type() == FileStatus::Types::SYMBOLIC_LINK
                           || Type() == FileStatus::Types::SYMBOLIC_LINK_DIR,
                          "FILETREE", "Given node is not a symbolic link."  )

        return static_cast<EISymLinkFile*>(extendedInfo)->RealTarget;
    }

}; // class FTValue

//==================================================================================================
/// Helper-class to resolve owner and group ids to strings names. The class uses an instance of
/// #"LRUCacheTable" of size 10 for each value to increase the performance of the
/// lookup. Because of this and the fact that the returned string value is located in an
/// internal member buffer, multithreaded invocations of members #".GetOwnerName" and #".GetGroupName"
/// have to be protected against racing conditions. This is up to the user of the type.
//==================================================================================================
class OwnerAndGroupResolver {
  protected:

    #if !defined( _WIN32)
    /// The owner name cache.
    mutable
    LRUCacheMap<PoolAllocator, FileStatus::TOwnerAndGroupID, NString32> ownerCache;

    /// The group name cache.
    mutable
    LRUCacheMap<PoolAllocator, FileStatus::TOwnerAndGroupID, NString32> groupCache;
    #endif

  public:
    #if DOXYGEN || !defined( _WIN32)
    /// Constructor.
    /// @param poolAllocator The allocator passed to the internal instances of type
    ///                      #"LRUCacheTable".
    OwnerAndGroupResolver( PoolAllocator& poolAllocator )
    : ownerCache(poolAllocator, 6,6)
    , groupCache(poolAllocator, 6,6)                                                              {}
    #else
    OwnerAndGroupResolver( PoolAllocator&)                                               {}
    #endif


    #if DOXYGEN
    /// Changes the capacity of the #"%LRUCacheTable" for owner names, by calling
    /// #"LRUCacheTable::Reserve;*".<br>
    /// The default sizes with construction is \b 6 for both values.
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    void SetOwnerCacheCapacity( integer numberOfLists, integer entriesPerList );

    /// Changes the capacity of the #"%LRUCacheTable" for owner names, by calling
    /// #"LRUCacheTable::Reserve;*".<br>
    /// The default sizes with construction is \b 6 for both values.
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    void SetGroupCacheCapacity( integer numberOfLists, integer entriesPerList );
    #elif !defined( _WIN32)
    void SetOwnerCacheCapacity( integer numberOfLists, integer entriesPerList )
    { ownerCache.Reserve( numberOfLists, entriesPerList );  }
    void SetGroupCacheCapacity( integer numberOfLists, integer entriesPerList )
    { groupCache.Reserve( numberOfLists, entriesPerList); }
    #else //_WIN32:
    void SetOwnerCacheCapacity( integer, integer )                                            {}
    void SetGroupCacheCapacity( integer, integer )                                            {}
    #endif

    /// Retrieves the file's owner's name.
    /// @see Method #"FileStatus::GetOwnerName;2".
    /// @param descriptor     The file to examine.
    /// @return The name of the owner of the file.
    ALIB_DLL
    const NString   GetOwnerName( const FileStatus& descriptor )                              const;

    /// Retrieves the file's group name.
    /// @see Method #"FileStatus::GetGroupName;2".
    /// @param descriptor     The file to examine.
    /// @return The name of the group of the file.
    ALIB_DLL
    const NString   GetGroupName( const FileStatus& descriptor )                              const;
}; //class OwnerAndGroupResolver

} // namespace alib[::filetree]


/// Type alias in namespace #"%alib".
using     FTValue                 =   filetree::FTValue;

/// Type alias in namespace #"%alib".
using     OwnerAndGroupResolver   =   filetree::OwnerAndGroupResolver;

}  // namespace [alib]

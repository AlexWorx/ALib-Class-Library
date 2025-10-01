//==================================================================================================
/// \file
/// This header-file is part of module \alib_files of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace files {

class FTree;
class File;
    
/// The entry type which is embedded in each tree node.
class FInfo
{
  protected:
    friend class FTree; ///< Allow access from within class File.
    friend class File;           ///< Allow access from within class File.
    
  public:
    /// Enumeration of the possible file types. This is compatible with the posix list of types,
    /// with the exception that symbolic links are differentiated between those linking to
    /// a directory and those linking to any other file type.
    enum class Types  : uint8_t
    {
        DIRECTORY          = 0, ///< Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< Symbolic link targeting a directory.
                                ///< In case scanning does not resolve links, this is never set.
        REGULAR            = 2, ///< Regular file.
        SYMBOLIC_LINK      = 3, ///< Symbolic link targeting a non-directory file.
                                ///< In case scanning does not resolve links, possibly to a directory.
        BLOCK              = 4, ///< A block special file.
        CHARACTER          = 5, ///< A character special file.
        FIFO               = 6, ///< A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< A socket file.

        UNKNOWN_OR_ERROR   = 8, ///< Filetype (should never or seldom happen).
                                ///< Maybe filesystem changed during scan or similar strange thing.
        MARKER_TYPES_END   = 9, ///< A marker for the last countable type. The rest are unused/errors
     };

    /// This is a resourced enumeration that is equivalent to enum class #Types but uses
    /// a 1-Letter code when serialized. The symbols are taken from GNU/Linux command
    /// <c>'ls -l'</c>, except special type \b SYMBOLIC_LINK_DIR which uses an upper case <c>'L'</c>
    /// in contrast to the lower case <c>'l'</c> used with links to files.
    enum class TypeNames1Letter  : uint8_t
    {
        DIRECTORY          = 0, ///< d: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< L: Symbolic link targeting a directory.
        REGULAR            = 2, ///< -: Regular file.
        SYMBOLIC_LINK      = 3, ///< l: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< b: A block special file.
        CHARACTER          = 5, ///< c: A character special file.
        FIFO               = 6, ///< p: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< s: A socket file.
     };

    /// This is a resourced enumeration that is equivalent to enum class #Types but uses
    /// a 2-Letter code when serialized.
    enum class TypeNames2Letters  : uint8_t
    {
        DIRECTORY          = 0, ///< DR: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< LD: Symbolic link targeting a directory.
        REGULAR            = 2, ///< RF: Regular file.
        SYMBOLIC_LINK      = 3, ///< LF: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< BL: A block special file.
        CHARACTER          = 5, ///< CH: A character special file.
        FIFO               = 6, ///< FF: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< SO: A socket file.
     };

    /// This is a resourced enumeration that is equivalent to enum class #Types but uses
    /// a 3-Letter code when serialized.
    enum class TypeNames3Letters  : uint8_t
    {
        DIRECTORY          = 0, ///< DIR: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< SLD: Symbolic link targeting a directory.
        REGULAR            = 2, ///< REG: Regular file.
        SYMBOLIC_LINK      = 3, ///< SLF: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< BLK: A block special file.
        CHARACTER          = 5, ///< CHR: A character special file.
        FIFO               = 6, ///< FFO: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< SCK: A socket file.
     };


    /// Permission flags. Compatible with posix* definition.
    enum class Permissions : uint32_t
    {
        NONE         =       0, ///< no permission bits are set
        UNKNOWN      =  010000, ///< Unknown permissions (e.g., when not read, or filesystem does not support permissions)
        MASK         =   07777, ///< All valid permission bits. Equivalent to all | set_uid | set_gid | sticky_bit
        ALL          =    0777, ///< All users have read, write, and execute/search permissions Equivalent to owner_all | group_all | others_all

        OWNER_READ   =    0400, ///< Posix \b S_IRUSR: File owner has read permission
        OWNER_WRITE  =    0200, ///< Posix \b S_IWUSR: File owner has write permission
        OWNER_EXEC   =    0100, ///< Posix \b S_IXUSR: File owner has execute/search permission
        OWNER_ALL    =    0700, ///< Posix \b S_IRWXU: File owner has read, write, and execute/search permissions Equivalent to owner_read | owner_write | owner_exec

        GROUP_READ   =     040, ///< Posix \b S_IRGRP: The file's user group has read permission
        GROUP_WRITE  =     020, ///< Posix \b S_IWGRP: The file's user group has write permission
        GROUP_EXEC   =     010, ///< Posix \b S_IXGRP: The file's user group has execute/search permission
        GROUP_ALL    =     070, ///< Posix \b S_IRWXG: The file's user group has read, write, and execute/search permissions Equivalent to group_read | group_write | group_exec

        OTHERS_READ  =      04, ///< Posix \b S_IROTH: Other users have read permission
        OTHERS_WRITE =      02, ///< Posix \b S_IWOTH: Other users have write permission
        OTHERS_EXEC  =      01, ///< Posix \b S_IXOTH: Other users have execute/search permission
        OTHERS_ALL   =      07, ///< Posix \b S_IRWXO: Other users have read, write, and execute/search permissions Equivalent to others_read | others_write | others_exec

        SET_UID      =   04000, ///< Posix \b S_ISUID: Set user ID to file owner user ID on execution
        SET_GID      =   02000, ///< Posix \b S_ISGID: Set group ID to file's user group ID on execution
        STICKY_BIT   =   01000, ///< Posix \b S_ISVTX: Implementation-defined meaning, but POSIX XSI specifies that when set on a directory, only file owners may delete files even if the directory is writeable to others (used with /tmp)
    };

    /// Type definition for owner and group ids
    using TOwnerAndGroupID  = uint32_t;

    /// Constant value for owner and group IDs to denote that the field was not determined.
    static constexpr TOwnerAndGroupID UnknownID= (std::numeric_limits<uint32_t>::max)();

    /// Per-entry information about how a node was scanned.
    enum class Qualities : uint8_t
    {
        NONE               = 0, ///< Node created only from given (real) start path.
        STATS              = 1, ///< Only stats (size, date, owner, etc.) read.

        RESOLVED           = 2, ///< Read symlink target strings.

        MAX_DEPTH_REACHED  = 3, ///< Scanner stopped, because maximum depth was reached.
        NOT_FOLLOWED       = 4, ///< A symbolic link that targets a directory, but scan parameters specify not to follow.
        NOT_CROSSING_FS    = 5, ///< A directory that represented a mounted filesystem was not followed due to field
        NO_AFS             = 6, ///< A directory that represented a mounted filesystem was not followed due to field
                                ///< \alib{files;ScanParameters::CrossFileSystems} being set to \c false.
        RECURSIVE          = 7, ///< Follow symlink target strings.

        NO_ACCESS          = 8, ///< Scanner failure due to limited access rights.
        NO_ACCESS_SL       = 9, ///< Scanner failure due to limited access rights.
        NO_ACCESS_SL_TARGET=10, ///< Scanner failure due to limited access rights.
        NO_ACCESS_DIR      =11, ///< Scanner failure due to limited access rights.
        BROKEN_LINK        =12, ///< A symbolic link targets a non-existent file or directory.
        CIRCULAR_LINK      =13, ///< Set if recursion stopped on a symbolic link which was identified by the
                                ///< operating system as a circular link.
        DUPLICATE          =14, ///< Set if recursion stopped on a symbolic link who's target had been scanned already.
                                ///< This might happen either because the path was already scanned by another search, or
                                ///< because a symbolic link is circular or because of a mixture of both.
                                ///< Thus, this can indicate a circular link, but does not have to.
        NOT_EXISTENT       =15, ///< Set if a given start path does not exist.
        UNKNOWN_ERROR      =16, ///< Unknown scanner failure
    };

    /// This is a resourced enumeration that is equivalent to enum class #Qualities but uses
    /// a 3-Letter code when serialized.
    enum class Qualities3Letters  : uint8_t
    {
        NONE               = 0, ///< NON: Node created only from given (real) start path.
        STATS              = 1, ///< STA: Only stats (size, date, owner, etc.) read.

        RESOLVED           = 2, ///< RES: Read symlink target strings.

        MAX_DEPTH_REACHED  = 3, ///< MDR:Scanner stopped, because maximum depth was reached.
        NOT_FOLLOWED       = 4, ///< NFO:A symbolic link that targets a directory, but scan parameters specify not to follow.
        NOT_CROSSING_FS    = 5, ///< NCF:A directory that represented a mounted filesystem was not followed due to field
        NO_AFS             = 6, ///< NAF:A directory that represented a mounted filesystem was not followed due to field
        ///< \alib{files;ScanParameters::CrossFileSystems} being set to \c false.
        RECURSIVE          = 7, ///< REC:Follow symlink target strings.

        NO_ACCESS          = 8, ///< NAC: Scanner failure due to limited access rights.
        NO_ACCESS_SL       = 9, ///< NSL: Scanner failure due to limited access rights.
        NO_ACCESS_SL_TARGET=10, ///< NAT: Scanner failure due to limited access rights.
        NO_ACCESS_DIR      =11, ///< NAD: Scanner failure due to limited access rights.
        BROKEN_LINK        =12, ///< BRL: A symbolic link targets a non-existent file or directory.
        CIRCULAR_LINK      =13, ///< CIL: Set if recursion stopped on a symbolic link which was identified by the
        ///< operating system as a circular link.
        DUPLICATE          =14, ///< DUP: Set if recursion stopped on a symbolic link who's target had been scanned already.
        ///< This might happen either because the path was already scanned by another search, or
        ///< because a symbolic link is circular or because of a mixture of both.
        ///< Thus, this can indicate a circular link, but does not have to.
        NOT_EXISTENT       =15, ///< NEX: Set if a given start path does not exist.
        UNKNOWN_ERROR      =16, ///< UKE: Unknown scanner failure
    };


    /// Recursively accumulated values for directories.
    struct  DirectorySums
    {
        uinteger    Size                                         = 0;  ///< The cumulated sizes of all files and directories.
        std::array<uint32_t, size_t(Types::MARKER_TYPES_END)> TypeCounters= {0,0,0,0,0,0,0,0}; ///< Per-type counters.
        uint32_t    QtyErrsAccess                                = 0;  ///< Number of access errors in the folder and subfolders.
        uint32_t    QtyErrsBrokenLink                            = 0;  ///< Number of broken symbolic links in the directory and its subfolders.
        uint32_t    QtyStopsOnMaxDepth                           = 0;  ///< Number of recursion aborts due to reach of maximum recursion depth.
        uint32_t    QtyStopsOnCircularLinks                      = 0;  ///< Number of recursion aborts due to detected circular links reach of maximum recursion depth.


        /// Defaulted default constructor.
        constexpr   DirectorySums()                                              noexcept = default;

        /// Adds the values in the given summary object to this.
        /// @param rhs The values to add.
        /// @return A reference to <c>this</c>
        DirectorySums& operator+= (const DirectorySums& rhs)
        {
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
        DirectorySums& operator-= (const DirectorySums& rhs)
        {
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
        /// \alib{files::FInfo;Types::DIRECTORY} or
        /// \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}
        /// @param type returns \c false if the given type does not represent a directory
        ///            and \c true if \p{type} equals
        ///            \alib{files::FInfo;Types::DIRECTORY} or
        ///            \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}
        /// @return \c false if the given type does not represent a directory, \c true otherwise.
        constexpr bool IsDirType(Types type)                                          const noexcept
        {
            return    int(type) < 2;
        }

        /// Adds a file/directory to the counters
        /// @param finfo The entry to add.
        /// @return A reference to <c>this</c>
        constexpr DirectorySums& Add(const FInfo& finfo)                                    noexcept
        {
            ++TypeCounters[size_t(finfo.Type())];
            Size+= finfo.Size();
            return *this;
        }


        /// Returns the cumulated number of entries (of any type).
        /// @return The number of entries counted.
        uint32_t Count()                                                              const noexcept
        {
            uint32_t result= 0;
            for (size_t i = 0; i < size_t(Types::MARKER_TYPES_END); ++i)
                result+= TypeCounters[i];
            return result;
        }

        /// Returns the number of entries of the given \p{type}.
        /// @param type The type to get the number of entries for.
        /// @return The number of directories or symbolic links to directories.
        uint32_t Count(Types type)                                                    const noexcept
        {
            ALIB_ASSERT_ERROR( type < Types::MARKER_TYPES_END,
               "FILES", "Cant get count for file type \"{}\"", type )
            return    TypeCounters[size_t(type)];
        }

        /// Returns the sum of the number of entries of type
        /// \alib{files::FInfo;Types::DIRECTORY} and
        /// \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}
        /// @return The number of directories or symbolic links to directories.
        uint32_t CountDirectories()                                                   const noexcept
        {
            return    TypeCounters[size_t(Types::DIRECTORY)]
                    + TypeCounters[size_t(Types::SYMBOLIC_LINK_DIR)];
        }

        /// Returns the sum of the number of entries which are \b not of type
        /// \alib{files::FInfo;Types::DIRECTORY} and
        /// \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}
        /// @return The number of regular files, fifo, sockets, etc.
        uint32_t CountNonDirectories()                                                const noexcept
        {
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
    /// \alib{MonoAllocator} and accessible via #GetExtendedInfo() and #Sums.
    struct  EIDirectory : public ExtendedEntryInfo
    {
        DirectorySums Sums;  ///< The recursive sums evaluated during scan.
    };

    /// Additional information for entries of symlink-type. Allocated in the tree's
    /// \alib{MonoAllocator} and accessible via #GetExtendedInfo(),
    /// #GetLinkTarget and #GetRealLinkTarget.
    struct  EISymLinkFile : public ExtendedEntryInfo
    {
        system::CPathString   Target;      ///< The target path.
                                                 ///<  This is a zero-terminated \b CString.
        system::CPathString   RealTarget;  ///< The resolved real target path.
    };

    /// Additional information for entries of symbolic link type. Allocated in the tree's
    /// \alib{MonoAllocator} and accessible via #GetExtendedInfo(), #GetLinkTarget,
    /// #GetRealLinkTarget and #Sums.
    struct  EISymLinkDir : public EISymLinkFile
    {
        DirectorySums Sums; ///< The recursive sums evaluated during scan.
    };

  protected:
    DateTime            mDate ;        ///< The last date and time the contents of the file was modified.
                                       ///< This timestamp is should be correct on all platforms/filesystems.
    DateTime            bDate ;        ///< The date this file was created. This timestamp is correctly set only
                                       ///< with certain filesystems under GNU/Linux, e.g., 'EXT4'.
                                       ///< If not available, the smallest value of the other three timestamps
                                       ///< is used.
    DateTime            cDate ;        ///< The date of the last change of the files' meta-information (e.g., owner).
                                       ///< This is not correctly set on all platforms/filesystems.
                                       ///< If not available, it will be set to #mDate.
    DateTime            aDate ;        ///< The date of last read or write access.
                                       ///< This is not correctly set on all platforms/filesystems.
                                       ///< If not available, it will be set to #mDate.
    uinteger            size  ;        ///< The file size. In case of a directory, this is \c 0.
    uint32_t            owner ;        ///< The user id that owns the file.
    uint32_t            group ;        ///< The group id that owns the file.
    uint32_t            qtyHardLinks ; ///< The number of hard links to the file.



    /// A bitfield encoding various information.
    struct Bits {
        Types           Type                    : 4;  ///< The file type.
        bool            IsArtificialFS          : 1;  ///< Denotes whether the file resides in an artificial filesystem.
        bool            TargetIsArtificialFS    : 1;  ///< Denotes whether a link points into an artificial filesystem.
        bool            IsCrossingFS            : 1;  ///< Denotes whether the file is a mount point.
        bool            TargetIsCrossingFS      : 1;  ///<  Denotes whether a link points to a mount point.
        Permissions     Permission              : 13; ///< The unix file-permissions.
        Qualities       Quality                 : 5;  ///< The scan quality.

        /// Default constructor. Sets all members to \b %NONE, \c false or \b %Error.
        Bits()
        : Type(Types::UNKNOWN_OR_ERROR)
        , IsArtificialFS      {false}
        , TargetIsArtificialFS{false}
        , IsCrossingFS        {false}
        , TargetIsCrossingFS  {false}
        , Permission          {Permissions::NONE}
        , Quality             {Qualities::NONE}
        {}
    };

    Bits                      bits; ///< A bitfield encoding various information.

    /// Extended information, depending on the entry type.
    ExtendedEntryInfo*        extendedInfo                                                = nullptr;

    /// Pool-allocated custom data.
    void*                     custom                                                      = nullptr;
    #if ALIB_DEBUG
    /// The custom type attached. Used for asserting misuse in debug-compilations.
    const std::type_info*     dbgCustomType                                               = nullptr;
    #endif


public:

    /// @return Retrieves the permissions of the entry.
    [[nodiscard]] constexpr Permissions  Perms               ()  const noexcept  { return bits.Permission; }
    /// @return Retrieves the type of the entry
    [[nodiscard]] constexpr Types        Type                ()  const noexcept  { return bits.Type; }
    /// @return Checks type for being either directory or symbolic link pointing to one.
    [[nodiscard]] constexpr bool         IsDirectory         ()  const noexcept  { return int(bits.Type) < 2; }
    /// @return Checks type for being a symbolic link (to normal file or to a directory).
    [[nodiscard]] constexpr bool         IsSymbolicLink      ()  const noexcept  { return    Type() == Types::SYMBOLIC_LINK
                                                                                          || Type() == Types::SYMBOLIC_LINK_DIR; }
    /// @return Retrieves the scan quality of the entry.
    [[nodiscard]] constexpr Qualities    Quality             ()  const noexcept { return bits.Quality; }
    /// @return Returns true if the entry resides on an artificial filesystem.
    [[nodiscard]] constexpr bool         IsArtificialFS      ()  const noexcept  { return bits.IsArtificialFS; }
    /// @return Returns true if the entry is a symlink and its target resides on an artificial filesystem.
    [[nodiscard]] constexpr bool         TargetIsArtificialFS()  const noexcept  { return bits.TargetIsArtificialFS; }
    /// @return Returns true if the entry resides on a different filesystem than it's parent.
    [[nodiscard]] constexpr bool         IsCrossingFS        ()  const noexcept  { return bits.IsCrossingFS; }
    /// @return Returns true if the entry is a symlink and resides on a different filesystem than the link.
    [[nodiscard]] constexpr bool         TargetIsCrossingFS  ()  const noexcept  { return bits.TargetIsCrossingFS; }
    /// @return Retrieves the file size.
    [[nodiscard]] constexpr uinteger     Size                ()  const noexcept  { return  size; }
    /// @return Retrieves the \ref mDate "last modification date" of this file/folder.
    [[nodiscard]] constexpr DateTime     MDate               ()  const noexcept  { return mDate; }
    /// @return Retrieves the \ref bDate "birth date" of this file/folder.
    [[nodiscard]] constexpr DateTime     BDate               ()  const noexcept  { return bDate; }
    /// @return Retrieves the \ref cDate "change date" of this file/folder. If unavailable, same as #MDate.
    [[nodiscard]] constexpr DateTime     CDate               ()  const noexcept  { return cDate; }
    /// @return Retrieves the \ref aDate "timestamp of last access" to this file/folder. If unavailable, same as #MDate.
    [[nodiscard]] constexpr DateTime     ADate               ()  const noexcept  { return aDate; }
    /// @return Retrieves the ID of the owner of the file/folder if available.  Otherwise set to #UnknownID.
    [[nodiscard]] constexpr uint32_t     Owner               ()  const noexcept  { return owner; }
    /// @return Retrieves the ID of the group of the file/folder if available.  Otherwise set to #UnknownID.
    [[nodiscard]] constexpr uint32_t     Group               ()  const noexcept  { return group; }
    /// @return Retrieves the number of hard links pointing to this file if available. Otherwise set to #UnknownID.
    [[nodiscard]] constexpr uint32_t     QtyHardLinks        ()  const noexcept  { return qtyHardLinks; }

    /// Retrieves the extended info object of this entry.
    /// @return The extended info object of this entry. If not available \c nullptr is returned.
    [[nodiscard]] constexpr ExtendedEntryInfo*  GetExtendedInfo()  const
    { return extendedInfo; }

    /// Sets the extended information object. As with all set functions, this method should only
    /// be used from certain code entities, like file scanners. If used, the object passed here
    /// has to be pool-allocated using public instance \alib{files;FTree::Pool}.
    /// The object will be freed with the deletion of the corresponding string tree node
    /// (respectively \b File instance).
    /// @param extInfo A pointer to the information object to use.
    constexpr void          SetExtendedInfo(ExtendedEntryInfo* extInfo)
    { extendedInfo= extInfo; }

    /// Retrieves the directory sums of this directory or symbolic link to directory.
    /// @return A reference to the sums.
    [[nodiscard]] constexpr DirectorySums&  Sums()  const
    {
        #if ALIB_DEBUG && !ALIB_DEBUG_ASSERTION_PRINTABLES
            ALIB_ASSERT_ERROR( IsDirectory(), "FILES",
                          "Requesting sums for FInfo that is not a directory.")
            ALIB_ASSERT_ERROR( extendedInfo != nullptr, "FILES",
                          "Requesting sums for FInfo that has no sums set. Quality: ", Quality() )
        #endif
        if( Type() == FInfo::Types::DIRECTORY )
            return static_cast<EIDirectory*>(extendedInfo)->Sums;
        return static_cast<EISymLinkDir*>(extendedInfo)->Sums;
    }

    /// Sets the sums of the extended info object of this entry.
    /// @param sums The sums to set.
    constexpr void          SetSums(const DirectorySums& sums)                                 const
    {
        if( Type() == FInfo::Types::DIRECTORY )
        {
            static_cast<EIDirectory*>(extendedInfo)->Sums= sums;
            return;
        }

        ALIB_ASSERT_ERROR( Type() == FInfo::Types::SYMBOLIC_LINK_DIR, "FILES",
                      "Given node is not a directory or symbolic link pointing to a directory.")

        static_cast<EISymLinkDir*>(extendedInfo)->Sums= sums;
    }

    /// Stores the link targets in the extended information object created for symbolic links.
    /// @param tree       The tree that this object belongs to.
    /// @param target     The target as stored in the symlink
    /// @param realTarget The translated, 'real' target path (if not broken).
    void                    SetLinkTarget(FTree& tree, const system::PathString& target,
                                                       const system::PathString& realTarget);

    /// Retrieves the non-translated target of a symbolic link. In debug compilations, the method
    /// asserts that #Type returns either \alib{files::FInfo;Types::SYMBOLIC_LINK}
    /// or \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}.
    /// @return A reference to a copy of the zero-terminated string stored in the symbolic link file.
    [[nodiscard]] system::CPathString&  GetLinkTarget()  const noexcept
    {
        ALIB_ASSERT_ERROR(    Type() == FInfo::Types::SYMBOLIC_LINK
                           || Type() == FInfo::Types::SYMBOLIC_LINK_DIR,
                          "FILES", "Given node is not a symbolic link."  )

        return static_cast<EISymLinkFile*>(extendedInfo)->Target;
    }

    /// Retrieves the resolved target of a symbolic link. In debug compilations, the method
    /// asserts that #Type returns either \alib{files::FInfo;Types::SYMBOLIC_LINK} or
    /// \alib{files::FInfo;Types::SYMBOLIC_LINK_DIR}.
    /// @return A reference to a zero-terminated string giving the translated real path that a
    ///        symbolic link points to.
    [[nodiscard]] system::CPathString&  GetRealLinkTarget()  const noexcept
    {
        ALIB_ASSERT_ERROR(    Type() == FInfo::Types::SYMBOLIC_LINK
                           || Type() == FInfo::Types::SYMBOLIC_LINK_DIR,
                          "FILES", "Given node is not a symbolic link."  )

        return static_cast<EISymLinkFile*>(extendedInfo)->RealTarget;
    }

    /// Sets the permissions of the entry. \param v The value to set.
                  void         SetPerms      (Permissions  v) noexcept  { bits.Permission= v; }
    /// Sets the type of the entry. \param v The value to set.
                  void         SetType       (Types        v) noexcept  { bits.Type= v; }
    /// Sets the quality of scan of the entry. \param v The value to set.
                  void         SetQuality    (Qualities    v) noexcept  { bits.Quality= v; }
    /// Mark the entry as residing on an artificial filesystem.
                  void         SetArtificialFS()              noexcept  { bits.IsArtificialFS= true; }
    /// Mark the entry as a symlink who's target is residing on an artificial filesystem.
                  void         SetTargetArtificialFS()        noexcept  { bits.TargetIsArtificialFS= true; }
    /// Mark the entry as residing on a different filesystem than its parent.
                  void         SetCrossingFS()                noexcept  { bits.IsCrossingFS= true; }
    /// Mark the entry as a symlink who's target is residing on a different filesystem than the symlink.
                  void         SetTargetCrossingFS()          noexcept  { bits.TargetIsCrossingFS= true; }
    /// Sets the file size. \param v The value to set.
                  void         SetSize       (uinteger    v)  noexcept  { size = v; }
    /// Sets the \ref mDate "last modification date" of this file/folder. \param v The value to set.
                  void         SetMDate      (DateTime    v)  noexcept  { mDate= v; }
    /// Sets the \ref bDate "birth date" of this file/folder. \param v The value to set.
                  void         SetBDate      (DateTime    v)  noexcept  { bDate= v; }
    /// Sets the \ref cDate "change date" of this file/folder.  \param v The value to set.
                  void         SetCDate      (DateTime    v)  noexcept  { cDate= v; }
    /// Sets the \ref aDate "time of last access" of this file/folder.  \param v The value to set.
                  void         SetADate      (DateTime    v)  noexcept  { aDate= v; }
    /// Sets the ID of the owner of the file/folder. \param v The value to set.
                  void         SetOwner      (uint32_t    v)  noexcept  { owner= v; }
    /// Sets the ID of the group of the file/folder. \param v The value to set.
                  void         SetGroup      (uint32_t    v)  noexcept  { group= v; }
    /// Sets the number of hard links that point to this file. \param v The value to set.
                  void         SetQtyHardlinks(uint32_t   v)  noexcept  { qtyHardLinks= v; }
}; // class FInfo


//==================================================================================================
/// Helper-class to resolve owner and group ids to strings names. The class uses an instance of
/// \alib{containers;LRUCacheTable} of size 10 for each value to increase the performance of the
/// lookup. Because of this and the fact that the returned string value is located in an internal
/// member buffer, multithreaded invocations of members #GetOwnerName and #GetGroupName have to be
/// protected against racing conditions. This is up to the user of the type.
//==================================================================================================
class OwnerAndGroupResolver
{
  protected:

    #if !defined( _WIN32)
        /// The owner name cache.
        LRUCacheMap<PoolAllocator, FInfo::TOwnerAndGroupID, NString32> ownerCache;

        /// The group name cache.
        LRUCacheMap<PoolAllocator, FInfo::TOwnerAndGroupID, NString32> groupCache;
    #endif

  public:
    #if DOXYGEN || !defined( _WIN32)
    /// Constructor.
    /// @param poolAllocator The allocator passed to the internal instances of type
    ///                      \alib{containers;LRUCacheTable}.
    OwnerAndGroupResolver( PoolAllocator& poolAllocator )
    : ownerCache(poolAllocator, 6,6)
    , groupCache(poolAllocator, 6,6)                                                              {}
    #else
    OwnerAndGroupResolver( PoolAllocator&)                                               {}
    #endif


    #if DOXYGEN
    /// Changes the capacity of the \b %LRUCacheTable for owner names, by calling
    /// \alib{containers;LRUCacheTable::Reserve}.<br>
    /// The default sizes with construction is \b 6 for both values.
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    void SetOwnerCacheCapacity( integer numberOfLists, integer entriesPerList );

    /// Changes the capacity of the \b %LRUCacheTable for owner names, by calling
    /// \alib{containers;LRUCacheTable::Reserve}.<br>
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
    /// @param fInfo     The file to examine.
    /// @return The name of the owner of the file.
    ALIB_DLL
    const NString&  GetOwnerName( const FInfo& fInfo );

    /// Retrieves the file's group name.
    /// @param fInfo     The file to examine.
    /// @return The name of the group of the file.
    ALIB_DLL
    const NString&  GetGroupName( const FInfo& fInfo );
}; //class OwnerAndGroupResolver

} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     FInfo                 =   files::FInfo;

/// Type alias in namespace \b alib.
using     OwnerAndGroupResolver =   files::OwnerAndGroupResolver;

}  // namespace [alib]

ALIB_ENUMS_MAKE_BITWISE (alib::files::FInfo::Permissions)
ALIB_ENUMS_MAKE_ITERABLE(alib::files::FInfo::Types,
                         alib::files::FInfo::Types::MARKER_TYPES_END)
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::Types            , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::TypeNames1Letter , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::TypeNames2Letters, ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::TypeNames3Letters, ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::Qualities        , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::Qualities3Letters, ERSerializable )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::Types            , alib::FILES, "FT"  )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::TypeNames1Letter , alib::FILES, "FT1" )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::TypeNames2Letters, alib::FILES, "FT2" )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::TypeNames3Letters, alib::FILES, "FT3" )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::Qualities        , alib::FILES, "FQ"  )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::Qualities3Letters, alib::FILES, "FQ3"  )


ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::Permissions         , vt_files_perms  )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::Types               , vt_files_type   )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::TypeNames1Letter    , vt_files_type1  )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::TypeNames2Letters   , vt_files_type2  )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::TypeNames3Letters   , vt_files_type3  )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::Qualities           , vt_files_qual   )
ALIB_BOXING_VTABLE_DECLARE( alib::files::FInfo::Qualities3Letters   , vt_files_qual3  )


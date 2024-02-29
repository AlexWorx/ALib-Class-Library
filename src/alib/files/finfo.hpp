/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_files of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_FILES_FINFO
#define HPP_ALIB_FILES_FINFO 1

#if !defined (HPP_ALIB_FILES_CAMP)
#   include "alib/files/filescamp.hpp"
#endif

#if !defined(HPP_ALIB_CAMP_MESSAGE_REPORT)
#   include "alib/lang/message/report.hpp"
#endif

#if !defined (HPP_ALIB_LANG_SIDILIST)
#   include "alib/lang/sidilist.hpp"
#endif

#if !defined (HPP_ALIB_TIME_DATETIME)
#   include "alib/time/datetime.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_ITERABLE)
#   include "alib/enums/iterable.hpp"
#endif


namespace alib { namespace files {

/** Local path string buffer helper type fixed to size 512. The flexible nature of \alib local
 *  strings allow dynamic allocation if exceeded.*/
using PathString= alib::strings::TLocalString<character, 512>;

/**
 * The entry type which is embedded in each tree node.
 */
class FInfo
{
  public:
      /** Enumeration of the possible file types. This is compatible with the posix list of types,
       *  with the exclamation that symbolic links are differentiated between those linking to
       *  a directory and those linking to any other file type.*/
    enum class Types  : int8_t
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

        MARKER_TYPES_END   = 8, ///< A marker for the last countable type. The rest are unused/errors

        UNKNOWN_OR_ERROR   = 8, ///< Filetype (should never or seldom happen).
                                ///< Maybe filesystem changed during scan or similar strange thing.
     };

    /** Permission flags. Compatible with posix* definition.  */
    enum class Permissions : int32_t
    {
        NONE         =       0, ///< no permission bits are set
        UNKNOWN      =  0xFFFF, ///< Unknown permissions (e.g. when not read or filesystem does not support permissions)
        MASK         =   07777, ///< All valid permission bits. Equivalent to all | set_uid | set_gid | sticky_bit
        ALL          =    0777, ///< All users have read, write, and execute/search permissions Equivalent to owner_all | group_all | others_all

        OWNER_READ   =    0400, /// < S_IRUSR    File owner has read permission
        OWNER_WRITE  =    0200, /// < S_IWUSR    File owner has write permission
        OWNER_EXEC   =    0100, /// < S_IXUSR    File owner has execute/search permission
        OWNER_ALL    =    0700, /// < S_IRWXU    File owner has read, write, and execute/search permissions Equivalent to owner_read | owner_write | owner_exec

        GROUP_READ   =     040, /// < S_IRGRP    The file's user group has read permission
        GROUP_WRITE  =     020, /// < S_IWGRP    The file's user group has write permission
        GROUP_EXEC   =     010, /// < S_IXGRP    The file's user group has execute/search permission
        GROUP_ALL    =     070, /// < S_IRWXG    The file's user group has read, write, and execute/search permissions Equivalent to group_read | group_write | group_exec

        OTHERS_READ  =      04, ///< S_IROTH    Other users have read permission
        OTHERS_WRITE =      02, ///< S_IWOTH    Other users have write permission
        OTHERS_EXEC  =      01, ///< S_IXOTH    Other users have execute/search permission
        OTHERS_ALL   =      07, ///< S_IRWXO    Other users have read, write, and execute/search permissions Equivalent to others_read | others_write | others_exec

        SET_UID      =   04000, ///< S_ISUID    Set user ID to file owner user ID on execution
        SET_GID      =   02000, ///< S_ISGID    Set group ID to file's user group ID on execution
        STICKY_BIT   =   01000, ///< S_ISVTX    Implementation-defined meaning, but POSIX XSI specifies that when set on a directory, only file owners may delete files even if the directory is writeable to others (used with /tmp)
    };

    /** Type definition for owner and group ids */
    using TOwnerAndGroupID  = uint32_t;

    /** Constant value for owner and group IDs to denote that the field was not determined. */
    static constexpr TOwnerAndGroupID UnknownID= (std::numeric_limits<uint32_t>::max)();

    /** Per-entry information about how a node was scanned. */
    enum class Qualities
    {
        NONE               = 0, ///< Node created only from given (real) start path.
        STATS              = 1, ///< Only stats (size, time, owner, etc.) read.

        RESOLVED           = 2, ///< Read symlink target strings.

        MAX_DEPTH_REACHED  = 3, ///< Scanner stopped, because maximum depth was reached.
        NOT_FOLLOWED       = 4, ///< A symbolic link that targets a directory, but scan parameters specify not to follow.
        NOT_CROSSING_FS    = 5, ///< A directory that represented a mounted filesystem was not followed due to field
        NO_AFS             = 6, ///< A directory that represented a mounted filesystem was not followed due to field
                                ///< \alib{files,ScanParameters::CrossFileSystems} being set to \c false.
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



    /** Recursively accumulated values for directories. */
    struct  DirectorySums
    {
        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        uinteger    Size                                         = 0;  ///< The cumulated sizes of all files and directories.
        uint32_t    TypeCounters[int(Types::MARKER_TYPES_END)]= {0,0,0,0,0,0,0,0}; ///< Per-type counters.
        uint32_t    QtyErrsAccess                                = 0;  ///< Number of access errors in the folder and sub-folders.
        uint32_t    QtyErrsBrokenLink                            = 0;  ///< Number of broken symbolic links in the directory and its sub-folders.
        uint32_t    QtyStopsOnMaxDepth                           = 0;  ///< Number of recursion aborts due to reach of maximum recursion depth.
        uint32_t    QtyStopsOnCircularLinks                      = 0;  ///< Number of recursion aborts due to detected circular links reach of maximum recursion depth.


        /** Defaulted default constructor. */
        constexpr   DirectorySums()                                              noexcept = default;

        /** Adds the values in the given summary object to this.
         * @param rhs The values to add.
         *  @return A reference to <c>this</c>                                */
        DirectorySums& operator+= (const DirectorySums& rhs)
        {
            Size                   += rhs.Size              ;
            for (int i = 0; i < int(Types::MARKER_TYPES_END); ++i)
                TypeCounters[i]+= rhs.TypeCounters[i];
            QtyErrsAccess          += rhs.QtyErrsAccess     ;
            QtyErrsBrokenLink      += rhs.QtyErrsBrokenLink ;
            QtyStopsOnMaxDepth     += rhs.QtyStopsOnMaxDepth;
            QtyStopsOnCircularLinks+= rhs.QtyErrsBrokenLink ;
           return *this;
        }

        /** Subtracts the values in the given summary object from this.
         * @param rhs The values to subtract.
         *  @return A reference to <c>this</c>                                */
        DirectorySums& operator-= (const DirectorySums& rhs)
        {
            Size                   -= rhs.Size              ;
            for (int i = 0; i < int(Types::MARKER_TYPES_END); ++i)
                TypeCounters[i]-= rhs.TypeCounters[i];
            QtyErrsAccess          -= rhs.QtyErrsAccess     ;
            QtyErrsBrokenLink      -= rhs.QtyErrsBrokenLink ;
            QtyStopsOnMaxDepth     -= rhs.QtyStopsOnMaxDepth;
            QtyStopsOnCircularLinks-= rhs.QtyErrsBrokenLink ;
           return *this;
        }
        /** Returns \c true if the given \p{type} equals either
         *  \alib{files::FInfo,Types::DIRECTORY} or
         *  \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}
         * @param type returns \c false if the given type does not represent a directory
         *             and \c true if \p{type} equals
         *             \alib{files::FInfo,Types::DIRECTORY} or
         *             \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}
         * @return \c false if the given type does not represent a directory, \c true otherwise. */
        constexpr bool IsDirType(Types type)                                          const noexcept
        {
            return    int(type) < 2;
        }

        /** Adds a file/directory to the counters
         *  @param finfo The entry to add.
         *  @return A reference to <c>this</c>                                */
        constexpr DirectorySums& Add(const FInfo& finfo)                                    noexcept
        {
            ++TypeCounters[int(finfo.Type())];
            Size+= finfo.Size();
            return *this;
        }


        /** Returns the cumulated number of entries (of any type).
         * @return The number of entries counted.    */
        uint32_t Count()                                                              const noexcept
        {
            uint32_t result= 0;
            for (int i = 0; i < int(Types::MARKER_TYPES_END); ++i)
                result+= TypeCounters[i];
            return result;
        }

        /** Returns the number of entries of the given \p{type}.
         *  @param type The type to get the number of entries for.
         * @return The number of directories or symbolic links to directories.    */
        uint32_t Count(Types type)                                                    const noexcept
        {
            ALIB_ASSERT_ERROR( type < Types::MARKER_TYPES_END,
               "CAMP/FILES", "Cant get count for file type {!Q}", type )
            return    TypeCounters[int(type)];
        }

        /** Returns the sum of the number of entries of type
         *  \alib{files::FInfo,Types::DIRECTORY} and
         *  \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}
         * @return The number of directories or symbolic links to directories.    */
        uint32_t CountDirectories()                                                   const noexcept
        {
            return    TypeCounters[int(Types::DIRECTORY)]
                    + TypeCounters[int(Types::SYMBOLIC_LINK_DIR)];
        }

        /** Returns the sum of the number of entries which are \b not of type
         *  \alib{files::FInfo,Types::DIRECTORY} and
         *  \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}
         * @return The number of regular files, fifo, sockets, etc.    */
        uint32_t CountNonDirectories()                                                const noexcept
        {
            uint32_t result= 0;
            for (int i = 2; i < int(Types::MARKER_TYPES_END); ++i)
                result+= TypeCounters[i];
            return result;
        }
        ALIB_WARNINGS_RESTORE
    };

    /** Base type to create pointers to different extended entry information structs. */
    struct ExtendedEntryInfo
    {};

    /** Additional information for entries of directory-type. Allocated in the tree's
     *  \alib{monomem,MonoAllocator} and accessible via #GetExtendedInfo() and #Sums.   */
    struct  EIDirectory : public ExtendedEntryInfo
    {
        DirectorySums Sums;  ///< The recursive sums evaluated during scan.
    };

    /** Additional information for entries of symlink-type. Allocated in the tree's
     *  \alib{monomem,MonoAllocator} and accessible via #GetExtendedInfo(),
     *  #GetLinkTarget and #GetRealLinkTarget.                                       */
    struct  EISymLinkFile : public ExtendedEntryInfo
    {
      protected:
        friend class FInfo;
        static constexpr integer InternalBufSize= 128;          ///< The defined size of the internal buffer.
        character                internalBuf[InternalBufSize];  ///< An internal buffer. If exceeded, a dynamic
                                                                ///< allocation is performed to store the links.

      public:
        CString     Target;      ///< The target path. This is a zero-terminated \b CString.
        CString     RealTarget;  ///< The resolved real target path.
    };

    /** Additional information for entries of symbolic link type. Allocated in the tree's
     *  \alib{monomem,MonoAllocator} and accessible via #GetExtendedInfo(), #GetLinkTarget,
     *  #GetRealLinkTarget and #Sums.                                                    */
    struct  EISymLinkDir : public EISymLinkFile
    {
        DirectorySums Sums; ///< The recursive sums evaluated during scan.
    };

  protected:
    /**
     * This type is used to add a list of custom data object to entries in \alib{files,FTree}.
     * @see Methods #AddCustomData and #GetCustomData.
     */
    struct InfoBox : public lang::SidiNodeBase<InfoBox>
    {
        Box data;       ///< The custom data.

        /** Constructor taking (and auto-boxing) any custom information.
         *  @param pData Custom information that is automatically boxed.        */
        explicit InfoBox( const Box& pData )
        : SidiNodeBase()
        , data(pData)
        {}
    };

    DateTime            mTime ; ///< The last write time. (Always available.)
    DateTime            cTime ; ///< The time of creation. This is not always available,
                                ///< depends on operating- and filesystem.
    DateTime            aTime ; ///< The time of last access.  This is not always available,
                                ///< depends on operating- and filesystem.
    uinteger            size  ; ///< The file size. In case of a directory, this is \c 0.
    uint32_t            owner ; ///< The user id that owns the file.
    uint32_t            group ; ///< The group id that owns the file.

    /** Bits encoding the following information:
     *   0 -  3: Type
     *        4: IsArtificialFS
     *        5: TargetIsArtificialFS
     *        6: IsCrossingFS
     *        7: TargetIsCrossingFS
     *   8 - 23: Perms
     *  24 - 31: Quality                      */
    uint32_t            bits  ;

    ExtendedEntryInfo*             extendedInfo; ///< Extended information, depending on the entry type.
    lang::SidiListHelper<InfoBox>  custom;       ///< A list of boxed custom data objects.


public:
    /** Constructor providing no values. Only sets ScanQuality::NONE */
    FInfo()
    : bits(0)
    , extendedInfo(nullptr)
    {}

    /**
     * This method, together with #GetCustomData, provides a simple mechanism to attach
     * some custom information to an entry in \b FTree.
     *
     * The allocator of the \b FTree object has to be provided as a parameter, as it
     * is otherwise not reachable from within this entry object. While this might also
     * be some other, external allocator, it is not easily thinkable of a use case for this.
     *
     * Note that any object that does not fit as a value into the \alib{boxing,Box} is
     * simply stored as a pointer. No copy is created! Therefore such object has to
     * be allocated persistent. Usually, such objects are created in the same
     * \p{monoAllocator} as given, right before the invocation of this method.
     *
     *
     * @see Method #GetCustomData.<br>
     *      For more information on class \b box, see Programmer's Manual of module
     *      \alib_boxing.
     * @param monoAllocator Usually, the allocator of the \b FTree object that this entry
     *                      belongs to and as such received with
     *                      <b>FTree::GetAllocator()</b>.
     * @param data          The data to attach.
     */
    void AddCustomData(MonoAllocator* monoAllocator, Box data)
    {
        auto* infoNode  = monoAllocator->Emplace<FInfo::InfoBox>( data );
        custom.pushFront(infoNode);
    }

    /**
     * This method, together with #AddCustomData, provides a simple mechanism to attach
     * and retrieve some custom information to an entry in \b FTree
     *
     * Custom data is stored in a single-linked list. This class does not provide an
     * iterator for the data, because the application has to assure for itself
     * how many custom data objects are placed in which order!
     * Usually, only one custom object should be stored.
     *
     * @see Method #AddCustomData.
     * @param idx The sequence number of the stored custom boxed element.
     * @return The boxed value. In case given \p{idx} is greater (or equal) to the number
     *         of stored custom elements, a nulled box is returned.
     */
    Box GetCustomData(int idx)
    {
        auto* list= custom.first();
        for(;;)
        {
            if( list == nullptr )   return Box();
            if( idx == 0 )          return list->data;
            list= list->next();
            --idx;
        }
    }

    /**
     * Alternative version of method #GetCustomData(int idx), which searches the
     * attached list of boxes for a first one that contains custom data of type \p{T}.
     *
     * @see Method #AddCustomData.
     * @tparam T The custom type to search for in the attached list of data.
     * @return The (still) boxed value. In case no box with an object of type \p{T} is
     *         found, a nulled box is returned.
     *
     */
    template<typename T>
    Box GetCustomData()
    {
        auto* list= custom.first();
        while(list!= nullptr)
        {
            if( list->data.IsType<T>() )
                return list->data;
            list= list->next();
        }
    }



    /** @return Retrieves the permissions of the entry. */
    [[nodiscard]] constexpr Permissions  Perms               ()  const noexcept  { return Permissions( (bits >>  8) & 0xFFFF); }
    /** @return Retrieves the type of the entry */
    [[nodiscard]] constexpr Types        Type                ()  const noexcept  { return Types (  bits        &    0xF); }
    /** @return Checks type for being either directory or symbolic link pointing to one.*/
    [[nodiscard]] constexpr bool         IsDirectory         ()  const noexcept  { return       (  bits        &    0xE) == 0; }
    /** @return Checks type for being a symbolic link (to normal file or to a directory). */
    [[nodiscard]] constexpr bool         IsSymbolicLink      ()  const noexcept  { return      Type() == Types::SYMBOLIC_LINK
                                                                        ||  Type() == Types::SYMBOLIC_LINK_DIR; }
    /** @return Retrieves the scan quality of the entry. */
    [[nodiscard]] constexpr Qualities    Quality             ()  const noexcept { return static_cast<Qualities>( (bits >> 24) &    0xF); }
    /** @return Returns true if the entry resides on an artificial filesystem. */
    [[nodiscard]] constexpr bool         IsArtificialFS      ()  const noexcept  { return (bits & (uint32_t(1) << 4)) != 0;   }
    /** @return Returns true if the entry is a symlink and its target resides on an artificial filesystem. */
    [[nodiscard]] constexpr bool         TargetIsArtificialFS()  const noexcept  { return (bits & (uint32_t(1) << 5)) != 0;   }
    /** @return Returns true if the entry resides on a different filesystem than it's parent. */
    [[nodiscard]] constexpr bool         IsCrossingFS        ()  const noexcept  { return (bits & (uint32_t(1) << 6)) != 0;   }
    /** @return Returns true if the entry is a symlink and resides on a different filesystem than the link. */
    [[nodiscard]] constexpr bool         TargetIsCrossingFS  ()  const noexcept  { return (bits & (uint32_t(1) << 7)) != 0;   }
    /** @return Retrieves the file size. */
    [[nodiscard]] constexpr uinteger     Size                ()  const noexcept  { return  size; }
    /** @return Retrieves the last modification time of the file/folder.  */
    [[nodiscard]] constexpr DateTime     MTime               ()  const noexcept  { return mTime; }
    /** @return Retrieves the creation time of the file/folder. If unavailable, same as #MTime.  */
    [[nodiscard]] constexpr DateTime     CTime               ()  const noexcept  { return cTime; }
    /** @return Retrieves the time of last access to the file/folder. If unavailable, same as #MTime.  */
    [[nodiscard]] constexpr DateTime     ATime               ()  const noexcept  { return aTime; }
    /** @return Retrieves the ID of the owner of the file/folder if available.  Otherwise set to #UnknownID. */
    [[nodiscard]] constexpr uint32_t     Owner               ()  const noexcept  { return owner; }
    /** @return Retrieves the ID of the group of the file/folder if available.  Otherwise set to #UnknownID. */
    [[nodiscard]] constexpr uint32_t     Group               ()  const noexcept  { return group; }

    /** Retrieves the extended info object of this entry.
     * @return The extended info object of this entry. If not available \c nullptr is returned. */
    [[nodiscard]] constexpr ExtendedEntryInfo*  GetExtendedInfo()  const
    { return extendedInfo; }

    /** Sets the externally allocated extended information object.
     * @param extInfo A pointer to the information object to use.    */
    constexpr void         SetExtendedInfo(ExtendedEntryInfo* extInfo)
    { extendedInfo= extInfo; }

    /** Retrieves the directory sums of this directory or symbolic link to directory.
     * @return A reference to the sums. */
    [[nodiscard]] constexpr DirectorySums&  Sums()  const
    {
        ALIB_ASSERT_ERROR( IsDirectory(), "CAMP/FILES",
                      "Requesting sums for FInfo that is not a directory.")
        ALIB_ASSERT_ERROR( extendedInfo != nullptr, "CAMP/FILES",
                      "Requesting sums for FInfo that has no sums set. Quality: ", Quality() )
        if( Type() == FInfo::Types::DIRECTORY )
            return static_cast<EIDirectory*>(extendedInfo)->Sums;
        return static_cast<EISymLinkDir*>(extendedInfo)->Sums;
    }

    /** Sets the sums of the extended info object of this entry.
     * @param sums The sums to set. */
    constexpr void                SetSums(const DirectorySums& sums)                           const
    {
        if( Type() == FInfo::Types::DIRECTORY )
        {
            static_cast<EIDirectory*>(extendedInfo)->Sums= sums;
            return;
        }

        ALIB_ASSERT_ERROR( Type() == FInfo::Types::SYMBOLIC_LINK_DIR, "CAMP/FILES",
                      "Given node is not a directory or symbolic link pointing to a directory.")

        static_cast<EISymLinkDir*>(extendedInfo)->Sums= sums;
    }

    /**
     * Stores the link targets in the extended information object created for symbolic links.
     * @param target     The target as stored in the symlink
     * @param realTarget The translated, 'real' target path (if not broken).
     */
    void        SetLinkTarget(const String& target, const String& realTarget);

    /** Retrieves the non-translated target of a symbolic link. In debug compilations, the method
     *  asserts that #Type returns either \alib{files::FInfo,Types::SYMBOLIC_LINK}
     *  or \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}.
     * @return A reference to a copy of the zero-terminated string stored in the symbolic link file. */
    [[nodiscard]] CString&   GetLinkTarget()  const noexcept
    {
        ALIB_ASSERT_ERROR(    Type() == FInfo::Types::SYMBOLIC_LINK
                           || Type() == FInfo::Types::SYMBOLIC_LINK_DIR, "CAMP/FILES",
                      "Given node is not a symbolic link.")

        return static_cast<EISymLinkFile*>(extendedInfo)->Target;
    }

    /** Retrieves the resolved target of a symbolic link. In debug compilations, the method
     *  asserts that #Type returns either \alib{files::FInfo,Types::SYMBOLIC_LINK} or
     *  \alib{files::FInfo,Types::SYMBOLIC_LINK_DIR}.
     * @return A reference to a zero-terminated string giving the translated real path that a
     *         symbolic link points to. */
    [[nodiscard]] CString&   GetRealLinkTarget()  const noexcept
    {
        ALIB_ASSERT_ERROR(    Type() == FInfo::Types::SYMBOLIC_LINK
                           || Type() == FInfo::Types::SYMBOLIC_LINK_DIR, "CAMP/FILES",
                      "Given node is not a symbolic link.")

        return static_cast<EISymLinkFile*>(extendedInfo)->RealTarget;
    }

    /** Sets the permissions of the entry. \param v The value to set. */
                  void         SetPerms      (Permissions  v) noexcept { bits= (bits & 0xFF0000FF) | (uint32_t(v) <<  8); }
    /** Sets the type of the entry. \param v The value to set. */
                  void         SetType       (Types        v) noexcept  { bits= (bits & 0xFFFFFFF0) | (uint32_t(v)      ); }
    /** Sets the quality of scan of the entry. \param v The value to set. */
                  void         SetQuality    (Qualities    v) noexcept  { bits= (bits & 0x00FFFFFF) | (uint32_t(v) << 24); }
    /** Mark the entry as residing on an artificial filesystem. */
                  void         SetArtificialFS()              noexcept  { bits|= (uint32_t(1) << 4); }
    /** Mark the entry as a symlink who's target is residing on an artificial filesystem. */
                  void         SetTargetArtificialFS()        noexcept  { bits|= (uint32_t(1) << 5); }
    /** Mark the entry as residing on a different filesystem than its parent. */
                  void         SetCrossingFS()                noexcept  { bits|= (uint32_t(1) << 6); }
    /** Mark the entry as a symlink who's target is residing on a different filesystem than the symlink. */
                  void         SetTargetCrossingFS()          noexcept  { bits|= (uint32_t(1) << 7); }
    /** Sets the file size. \param v The value to set. */
                  void         SetSize       (uinteger    v)  noexcept  { size = v; }
    /** Sets the last modification time of the file/folder. \param v The value to set. */
                  void         SetMTime      (DateTime    v)  noexcept  { mTime= v; }
    /** Sets the creation time of the file/folder. If unavailable, same as #MTime. \param v The value to set. */
                  void         SetCTime      (DateTime    v)  noexcept  { cTime= v; }
    /** Sets the time of last access to the file/folder. If unavailable, same as #MTime. \param v The value to set. */
                  void         SetATime      (DateTime    v)  noexcept  { aTime= v; }
    /** Sets the ID of the owner of the file/folder if available.  Otherwise set to #UnknownID. \param v The value to set. */
                  void         SetOwner      (uint32_t    v)  noexcept  { owner= v; }
    /** Sets the ID of the group of the file/folder if available.  Otherwise set to #UnknownID. \param v The value to set. */
                  void         SetGroup      (uint32_t    v)  noexcept  { group= v; }


    /**
     * Writes the file type and the permission flags to the given \p{target} string in the
     * same format as GNU/Linux command <em>'ls -l'</em> does.
     * @param target  The target string to write into.
     * @return The given target to allow concatenated calls.
     */
    AString&     WriteTypeAndAccess(AString& target)                                          const;

}; // class FInfo

} // namespace alib[::files]


/// Type alias in namespace \b alib.
using     FInfo                 =   files::FInfo;

}  // namespace [alib]

ALIB_ENUMS_MAKE_BITWISE (alib::files::FInfo::Permissions)
ALIB_ENUMS_MAKE_ITERABLE(alib::files::FInfo::Types,
                         alib::files::FInfo::Types::MARKER_TYPES_END)
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::Types    , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::files::FInfo::Qualities, ERSerializable )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::Types    , alib::FILES, "FT" )
ALIB_RESOURCED_IN_MODULE(alib::files::FInfo::Qualities, alib::FILES, "FQ" )

#endif // HPP_ALIB_FILES_FINFO

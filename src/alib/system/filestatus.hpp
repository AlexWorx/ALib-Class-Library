//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================

ALIB_EXPORT namespace alib { namespace system {

/// Represents metadata of a filesystem entry.
///
/// This type is the lightweight data container to hold file attributes such as:
/// - file type and scan state,
/// - size, timestamps, ownership, permissions and hard-link count,
/// - optional scan flags related to symlink resolution and filesystem boundaries.
///
/// Instances are typically populated with the overloaded method #"Update(Path)".
/// The result of retrieval is reflected in the field #".ScanState" and may indicate partial or failed
/// retrieval (for example, non-existing paths or missing access rights).
///
/// \par Relationship To Module \alib_filetree_nl
/// The module \alibmod_nl \alib_filetree builds on this type by extending this type to the class
/// #"FTValue" with scan- and file-tree-specific payload.
/// With this, users of module \alib_filetree_nl gain additional features when scanning the file
/// system, including symbolic-vs-real path handling, symbolic link resolving, symbolic parent
/// tracking, and formatted file information output.<br>
/// To keep the code efficient, some of the features that are available only with the class
/// #"%FTValue" are already reflected in this class. For example, the enumeration element
/// #"Types::SYMBOLIC_LINK_DIR;3" is never set with methods #".Update(Path)", but only when scanning
/// file trees. While this can be considered an inconsistent API definition, this approach serves
/// runtime efficiency.
///
/// \par Restrictions On Non-POSIX Systems
/// Two versions of the implementation of overloaded methods #".Update(Path)" exist, which depend
/// on the target system and optionally on the configuration macro
/// #"ALIB_SYSTEM_FORCE_STD_FILE_STATUS". The non-POSIX version is restricted due to the fact
/// that the \c std::filesystem API is less feature-richt than what POSIX offers.
///
/// @see Classes #"filetree::FTFile" and #"filetree::FTValue".
class FileStatus {
  public:
    /// Enumeration of the possible file types. This is compatible with the POSIX list of types,
    /// with the exception that symbolic links are differentiated between those linking to
    /// a directory and those linking to any other file type.
    enum class Types  : uint8_t {
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

    /// This is a resourced enumeration that is equivalent to enum class #".Types" but uses
    /// a 1-Letter code when serialized. The symbols are taken from GNU/Linux command
    /// <c>'ls -l'</c>, except special type \b SYMBOLIC_LINK_DIR which uses an upper case <c>'L'</c>
    /// in contrast to the lower case <c>'l'</c> used with links to files.
    enum class TypeNames1Letter  : uint8_t {
        DIRECTORY          = 0, ///< d: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< L: Symbolic link targeting a directory.
        REGULAR            = 2, ///< -: Regular file.
        SYMBOLIC_LINK      = 3, ///< l: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< b: A block special file.
        CHARACTER          = 5, ///< c: A character special file.
        FIFO               = 6, ///< p: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< s: A socket file.
     };

    /// This is a resourced enumeration that is equivalent to enum class #".Types" but uses
    /// a 2-Letter code when serialized.
    enum class TypeNames2Letters  : uint8_t {
        DIRECTORY          = 0, ///< DR: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< LD: Symbolic link targeting a directory.
        REGULAR            = 2, ///< RF: Regular file.
        SYMBOLIC_LINK      = 3, ///< LF: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< BL: A block special file.
        CHARACTER          = 5, ///< CH: A character special file.
        FIFO               = 6, ///< FF: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< SO: A socket file.
     };

    /// This is a resourced enumeration that is equivalent to enum class #".Types" but uses
    /// a 3-Letter code when serialized.
    enum class TypeNames3Letters  : uint8_t {
        DIRECTORY          = 0, ///< DIR: Directory/folder.
        SYMBOLIC_LINK_DIR  = 1, ///< SLD: Symbolic link targeting a directory.
        REGULAR            = 2, ///< REG: Regular file.
        SYMBOLIC_LINK      = 3, ///< SLF: Symbolic link targeting a non-directory file.
        BLOCK              = 4, ///< BLK: A block special file.
        CHARACTER          = 5, ///< CHR: A character special file.
        FIFO               = 6, ///< FFO: A FIFO (also known as pipe) file.
        SOCKET             = 7, ///< SCK: A socket file.
     };


    /// Permission flags. Compatible with POSIX definition.
    enum class Permissions : uint32_t {
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
    enum class ScanStates : uint8_t {
        NONE               = 0, ///< Node created only from the given start path.
        STATS              = 1, ///< Only stats (size, date, owner, etc.) read.

        RESOLVED           = 2, ///< Read symlink target strings.

        MAX_DEPTH_REACHED  = 3, ///< Scanning stopped because maximum depth was reached.
        NOT_FOLLOWED       = 4, ///< A symbolic link that targets a directory, but scan parameters specify not to follow.
        NOT_CROSSING_FS    = 5, ///< A directory that represented a mounted filesystem was not followed due to the field
                                ///< #"ScanParameters::CrossFileSystems;*" being set to \c false.
        NO_AFS             = 6, ///< A directory that represented a mounted filesystem was not followed due to the field
                                ///< #"ScanParameters::IncludeArtificialFS;*" being set to \c false.
        RECURSIVE          = 7, ///< Follow symlink target strings.

        NO_ACCESS          = 8, ///< Scanner failure due to limited access rights.
        NO_ACCESS_SL       = 9, ///< Scanner failure due to limited access rights on a symbolic link.
        NO_ACCESS_SL_TARGET=10, ///< Scanner failure due to limited access rights on a symbolic link's target.
        NO_ACCESS_DIR      =11, ///< Scanner failure due to limited access rights on a directory.
        BROKEN_LINK        =12, ///< A symbolic link targets a non-existent file or directory.
        CIRCULAR_LINK      =13, ///< Set if recursion stopped at a symbolic link which was identified by the
                                ///< operating system as a circular link.
        DUPLICATE          =14, ///< Set if recursion stopped on a symbolic link who's target had been scanned already.
                                ///< This might happen either because the path was already scanned by another search, or
                                ///< because a symbolic link is circular or because of a mixture of both.
                                ///< Thus, this can indicate a circular link, but does not have to.
        NOT_EXISTENT       =15, ///< Set if a given start path does not exist.
        UNKNOWN_ERROR      =16, ///< Unknown scanner failure
    };

    /// This is a resourced enumeration that is equivalent to enum class #"ScanStates" but uses
    /// a 3-Letter code when serialized.
    enum class ScanStates3Letters  : uint8_t {
        NONE               = 0, ///< NON: Node created only from given start path path.
        STATS              = 1, ///< STA: Only stats (size, date, owner, etc.) read.

        RESOLVED           = 2, ///< RES: Read symlink target strings.

        MAX_DEPTH_REACHED  = 3, ///< MDR:Scanner stopped, because maximum depth was reached.
        NOT_FOLLOWED       = 4, ///< NFO:A symbolic link that targets a directory, but scan parameters specify not to follow.
        NOT_CROSSING_FS    = 5, ///< NCF:A directory that represented a mounted filesystem was not followed due to the field
                                ///< #"ScanParameters::CrossFileSystems;*" being set to \c false.
        NO_AFS             = 6, ///< NAF:A directory that represented a mounted filesystem was not followed due to the field
                                ///< #"ScanParameters::IncludeArtificialFS;*" being set to \c false.
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


  protected:
    DateTime  mDate;        ///< The last date and time the contents of the file was modified.
                            ///< This timestamp is should be correct on all platforms/filesystems.
    DateTime  bDate;        ///< The date this file was created. This timestamp is correctly set
                            ///< only with certain filesystems under GNU/Linux, e.g., 'EXT4'.
                            ///< If not available, the smallest value of the other three timestamps
                            ///< is used.
    DateTime  cDate;        ///< The date of the last change of the files' meta-information
                            ///< (e.g., owner). This is not correctly set on all
                            ///< platforms/filesystems. If not available, it will be set to #".mDate".
    DateTime  aDate;        ///< The date of last read or write access.
                            ///< This is not correctly set on all platforms/filesystems.
                            ///< If not available, it will be set to #".mDate".
    uinteger  symParent= 0; ///< A cursor handle to the symbolic parent. See manual chapter
                            ///< #"alib_filetree_tut_scan_realpath" for more information.
    uinteger  size;         ///< The file size. In case of a directory, this is \c 0.
    uint32_t  owner;        ///< The user id that owns the file.
    uint32_t  group;        ///< The group id that owns the file.
    uint32_t  qtyHardLinks; ///< The number of hard links to the file.


    /// A bitfield encoding various information.
    struct Bits {
        Types       Type                : 4;  ///< The file type.
        bool        IsArtificialFS      : 1;  ///< Denotes whether the file resides in an artificial filesystem.
        bool        TargetIsArtificialFS: 1;  ///< Denotes whether a link points into an artificial filesystem.
        bool        IsCrossingFS        : 1;  ///< Denotes whether the file is a mount point.
        bool        TargetIsCrossingFS  : 1;  ///< Denotes whether a link points to a mount point.
        Permissions Permission          : 13; ///< The unix file-permissions.
        ScanStates  ScanState           : 5;  ///< The scan state.

        /// Default constructor. Sets all members to #"Permissions::NONE", \c false, respectively
        /// #"Types::UNKNOWN_OR_ERROR".
        Bits()
        : Type(Types::UNKNOWN_OR_ERROR)
        , IsArtificialFS      {false}
        , TargetIsArtificialFS{false}
        , IsCrossingFS        {false}
        , TargetIsCrossingFS  {false}
        , Permission          {Permissions::NONE}
        , ScanState           {ScanStates::NONE }                                                 {}
    };

    Bits                      bits;       ///< A bitfield encoding various information.

    uint64_t                  device;     ///< The POSIX device ID. Available only if
                                          ///< #"ALIB_SYSTEM_FILE_STATUS_IMPL" equals
                                          ///< #"ALIB_SYSTEM_FILE_POSIX_STATUS".

  public:
    /// Default constructor
    FileStatus() : mDate{lang::Initialization::Nulled}, bDate{lang::Initialization::Nulled},
                   cDate{lang::Initialization::Nulled}, aDate{lang::Initialization::Nulled},
                   size{0}, owner{0}, group{0},
                   qtyHardLinks{0},  device{0}                                                    {}

    /// Constructor taking a path. Calls #".Update(Path);*".
    /// @param path         The file path to get the status for.
    /// @param isCanonical  If given as \c true, then the caller guarantees that \p{path} does not
    ///                     resemble a symbolic link. Defaults to \c false, which causes the
    ///                     method to call #"MakeCanonical;2" on \p{path} first.
    FileStatus(Path& path, bool isCanonical= false)                   { Update(path, isCanonical); }

    /// Updates all members according to the given path.
    /// @param path         The file path to get the status for.
    /// @param isCanonical  If given as \c true, then the caller guarantees that \p{path} does not
    ///                     resemble a symbolic link. Defaults to \c false, which causes the
    ///                     method to call #"MakeCanonical;2" on \p{path} first.
    /// @return The result of retrieving the given filePaths' state.
    ALIB_DLL
    ScanStates      Update(Path& path, bool isCanonical= false);

    /// Updates all members according to the given <c>std::filesystem::path</c>.
    /// @param path         The file path to get the status for.
    /// @param isCanonical  If given as \c true, then the caller guarantees that \p{path} does not
    ///                     resemble a symbolic link. Defaults to \c false, which causes the
    ///                     method to call #"MakeCanonical;2" on \p{path} first.
    /// @return The result of retrieving the given filePaths' state.
    ALIB_DLL
    ScanStates      Update(std::filesystem::path& path, bool isCanonical= false);


    /// @return Retrieves the permissions of the entry.
    [[nodiscard]] constexpr Permissions  Perms               ()  const noexcept { return bits.Permission; }
    /// @return Retrieves the type of the entry
    [[nodiscard]] constexpr Types        Type                ()  const noexcept { return bits.Type; }
    /// @return Checks type for being either directory or symbolic link pointing to one.
    [[nodiscard]] constexpr bool         IsDirectory         ()  const noexcept { return int(bits.Type) < 2; }
    /// @return Checks type for being a symbolic link (to normal file or to a directory).
    [[nodiscard]] constexpr bool         IsSymbolicLink      ()  const noexcept { return    Type() == Types::SYMBOLIC_LINK
                                                                                         || Type() == Types::SYMBOLIC_LINK_DIR; }
    /// @return Retrieves the scan sate of the entry.
    [[nodiscard]] constexpr ScanStates   ScanState           ()  const noexcept { return bits.ScanState    ; }
    /// @return Returns true if the entry resides on an artificial filesystem.
    [[nodiscard]] constexpr bool         IsArtificialFS      ()  const noexcept { return bits.IsArtificialFS; }
    /// @return Returns true if the entry is a symlink and its target resides on an artificial filesystem.
    [[nodiscard]] constexpr bool         TargetIsArtificialFS()  const noexcept { return bits.TargetIsArtificialFS; }
    /// @return Returns true if the entry resides on a different filesystem than it's parent.
    [[nodiscard]] constexpr bool         IsCrossingFS        ()  const noexcept { return bits.IsCrossingFS; }
    /// @return Returns true if the entry is a symlink and resides on a different filesystem than the link.
    [[nodiscard]] constexpr bool         TargetIsCrossingFS  ()  const noexcept { return bits.TargetIsCrossingFS; }
    /// @return Returns the POSIX device code.
    [[nodiscard]] constexpr uint64_t     PosixDevice         ()    const noexcept { return device; }
    /// @return Retrieves the file size.
    [[nodiscard]] constexpr uinteger     Size                ()     const noexcept { return  size; }
    /// @return Retrieves the #"mDate;last modification date" of this file/folder.
    [[nodiscard]] constexpr DateTime     MDate               ()     const noexcept { return mDate; }
    /// @return Retrieves the #"bDate;birth date" of this file/folder.
    [[nodiscard]] constexpr DateTime     BDate               ()     const noexcept { return bDate; }
    /// @return Retrieves the #"cDate;change date" of this file/folder. If unavailable, same as #".MDate".
    [[nodiscard]] constexpr DateTime     CDate               ()     const noexcept { return cDate; }
    /// @return Retrieves the #"aDate;timestamp of last access" to this file/folder. If unavailable, same as #".MDate".
    [[nodiscard]] constexpr DateTime     ADate               ()     const noexcept { return aDate; }
    /// @return Retrieves the ID of the owner of the file/folder if available. Otherwise set to #".UnknownID".
    [[nodiscard]] constexpr uint32_t     Owner               ()     const noexcept { return owner; }
    /// @return Retrieves the ID of the group of the file/folder if available. Otherwise set to #".UnknownID".
    [[nodiscard]] constexpr uint32_t     Group               ()     const noexcept { return group; }
    /// @return Retrieves the number of hard links pointing to this file if available. Otherwise set to #".UnknownID".
    [[nodiscard]] constexpr uint32_t     QtyHardLinks     () const noexcept { return qtyHardLinks; }

    /// @return Retrieves the name of the owner of the file/folder if available.
    ///         Otherwise, and on the WindowsOS platform, <c>"?"</c> is returned.
    ALIB_DLL [[nodiscard]] const NString         GetOwnerName()                               const;

    /// @return Retrieves the name of the group of the file/folder if available.
    ///         Otherwise, and on the WindowsOS platform, <c>"?"</c> is returned.
    ALIB_DLL [[nodiscard]] const NString         GetGroupName()                               const;

    /// Sets the permissions of the entry.
    /// @param v The value to set.
    void SetPerms      (Permissions  v)                             noexcept { bits.Permission= v; }

    /// Sets the type of the entry.
    /// @param v The value to set.
    void SetType       (Types        v)                                   noexcept { bits.Type= v; }

    /// Sets the state of scan of the entry.
    /// @param v The value to set.
    void SetScanState  (ScanStates  v)                               noexcept { bits.ScanState= v; }

    /// Mark the entry as residing on an artificial filesystem.
    void SetArtificialFS()                                   noexcept { bits.IsArtificialFS= true; }

    /// Mark the entry as a symlink who's target is residing on an artificial filesystem.
    void SetTargetArtificialFS()                        noexcept { bits.TargetIsArtificialFS=true; }

    /// Mark the entry as residing on a different filesystem than its parent.
    void SetCrossingFS()                                       noexcept { bits.IsCrossingFS= true; }

    /// Set the POSIX device code.
    /// @param deviceCode  The POSIX device code.
    void SetPosixDevice(uint64_t deviceCode)                        noexcept { device= deviceCode; }

    /// Mark the entry as a symlink who's target is residing in a different filesystem than the
    /// symlink.
    void SetTargetCrossingFS()                            noexcept { bits.TargetIsCrossingFS=true; }

    /// Sets the file size.
    /// @param v The value to set.
    void SetSize        (uinteger    v)                                       noexcept { size = v; }

    /// Sets the #"mDate;last modification date" of this file/folder.
    /// @param v The value to set.
    void SetMDate       (DateTime    v)                                       noexcept { mDate= v; }

    /// Sets the #"bDate;birth date" of this file/folder.
    /// @param v The value to set.
    void SetBDate       (DateTime    v)                                       noexcept { bDate= v; }

    /// Sets the #"cDate;change date" of this file/folder.
    /// @param v The value to set.
    void SetCDate       (DateTime    v)                                       noexcept { cDate= v; }

    /// Sets the #"aDate;time of last access" of this file/folder.
    /// @param v The value to set.
    void SetADate       (DateTime    v)                                       noexcept { aDate= v; }

    /// Sets the ID of the owner of the file/folder.
    /// @param v The value to set.
    void SetOwner       (uint32_t    v)                                       noexcept { owner= v; }

    /// Sets the ID of the group of the file/folder.
    /// @param v The value to set.
    void SetGroup       (uint32_t    v)                                       noexcept { group= v; }

    /// Sets the number of hard links that point to this file.
    /// @param v The value to set.
    void SetQtyHardlinks(uint32_t    v)                                noexcept { qtyHardLinks= v; }
}; // class FTValue


} // namespace alib[::system]


/// Type alias in namespace #"%alib".
using     FileStatus        =   system::FileStatus;

}  // namespace [alib]

ALIB_ENUMS_MAKE_BITWISE (alib::system::FileStatus::Permissions)
ALIB_ENUMS_MAKE_ITERABLE(alib::system::FileStatus::Types,
                         alib::system::FileStatus::Types::MARKER_TYPES_END)
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::Types             , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::TypeNames1Letter  , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::TypeNames2Letters , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::TypeNames3Letters , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::ScanStates        , ERSerializable )
ALIB_ENUMS_ASSIGN_RECORD(alib::system::FileStatus::ScanStates3Letters, ERSerializable )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::Types               , alib::FILETREE, "FT"  )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::TypeNames1Letter    , alib::FILETREE, "FT1" )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::TypeNames2Letters   , alib::FILETREE, "FT2" )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::TypeNames3Letters   , alib::FILETREE, "FT3" )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::ScanStates          , alib::FILETREE, "FQ"  )
//ALIB_RESOURCED_IN_CAMP(alib::system::FileStatus::ScanStates3Letters  , alib::FILETREE, "FQ3"  )


ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::Permissions         , vt_files_perms  )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::Types               , vt_files_type   )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::TypeNames1Letter    , vt_files_type1  )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::TypeNames2Letters   , vt_files_type2  )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::TypeNames3Letters   , vt_files_type3  )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::ScanStates          , vt_files_qual   )
ALIB_BOXING_VTABLE_DECLARE( alib::system::FileStatus::ScanStates3Letters  , vt_files_qual3  )

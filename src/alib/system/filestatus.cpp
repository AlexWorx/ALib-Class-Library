

namespace alib::system {

//==================================================================================================
//=== OwnerAndGroupResolver
//==================================================================================================
const NString  FileStatus::GetOwnerName()                                                    const {
    #if !defined ( _WIN32)
        auto* result= getpwuid(owner);
        return NString(result ? result->pw_name : "?");
    #else
        return "?";
    #endif
}

const NString  FileStatus::GetGroupName()                                                    const {
    #if !defined ( _WIN32)
        auto* result= getgrgid(group);
        return NString(result ? result->gr_name : "?");
    #else
        return "?";
    #endif
}

//==================================================================================================
//=== Path::Exists
//==================================================================================================
bool Path::Exists() {
    #if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS

        ALIB_DBG( errno= 0;)
        struct stat stats;
        return  lstat( Terminate(), &stats ) == 0;
    #else
        ALIB_STRINGS_TO_NARROW(*this,nPath,MAX_PATH)
        fs::path stdPath(nPath.Terminate());
        return fs::exists(stdPath);
    #endif
}

//--------------------------------------------------------------------------------------------------
//--- Path::Exists UNKNOWN platform, using C++17 filesystem (not all functionality given)
//--------------------------------------------------------------------------------------------------

//==================================================================================================
//=== FileStatus::Update Posix version
//==================================================================================================
#if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS

FileStatus::ScanStates FileStatus::Update(std::filesystem::path& path, bool isCanonical) {
    Path alibPath(path.c_str());
    return Update(alibPath, isCanonical);
}

FileStatus::ScanStates FileStatus::Update(Path& path, bool isCanonical) {
    // check
    #if ALIB_DEBUG
    if(isCanonical) {
        Path pc(path);
        pc.MakeCanonical();
        ALIB_ASSERT_ERROR(path.Equals(pc), "SYSTEM",
         "Non-canonical path given, while parameter 'isCanonical' is true: {}", path)
    }
    #endif

    // canonify path
    if(!isCanonical)
        path.MakeCanonical();


    // prerequisites
    // Since Kernel 4.11 Linux/glibc has "statx". We use it if available on the current platform.
#   if defined(__NR_statx)
#      define TMP_STATX_AVAILABLE 1
#      define STATMEMBER(Name) stats.stx_ ## Name
#      define STAT_DEV_MAJOR   stats.stx_dev_major
#      define STAT_DEV_MINOR   stats.stx_dev_minor
#   else
#      define TMP_STATX_AVAILABLE 0
#      define STATMEMBER(Name) stats.st_ ## Name
#      define STAT_DEV_MAJOR   major(stats.st_dev)
#      define STAT_DEV_MINOR   minor(stats.st_dev)
#   endif

#   if ALIB_DEBUG
#      define DBG_CHECKERRNO_WITH_PATH                                                                \
ALIB_ASSERT_WARNING(errno == 0, "SYSTEM", "Errno set ({})\"{}\". Current path: {}",             \
errno, std::errc(errno), path )                                           \
errno= 0;
#   else
#      define DBG_CHECKERRNO_WITH_PATH
#   endif

    SetScanState(FileStatus::ScanStates::STATS);

    // read base stats
    ALIB_DBG( errno= 0;)
    #if TMP_STATX_AVAILABLE
        struct statx  stats;
        int statResult=  statx(  AT_FDCWD,
                                 path.Terminate(),
                                 AT_STATX_DONT_SYNC | AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
                                 STATX_BASIC_STATS | STATX_BTIME,
                                 &stats );

    #else
        struct stat stats;
        int statResult=  lstat  ( path.Terminate(), &stats );
    #endif
    if( statResult ) {
        ALIB_ASSERT_WARNING( errno == ENOENT, "SYSTEM", "Unknown error {} while stating file \"{}\"",
                             std::errc(errno), path )
        SetScanState(errno == ENOENT ?  FileStatus::ScanStates::NOT_EXISTENT
                                     :  FileStatus::ScanStates::UNKNOWN_ERROR);
        ALIB_DBG( errno= 0;)
        return ScanState();
    }
    DBG_CHECKERRNO_WITH_PATH

    // store filesystem type (artificial fs & mount point)
    SetPosixDevice((uint64_t(STAT_DEV_MAJOR) << 32L) + STAT_DEV_MINOR);

    // Use statfs to properly detect artificial filesystems
    #if defined(__linux__)
        struct statfs fsStats;
        if (statfs(path.Terminate(), &fsStats) == 0) {
            // Check for known artificial filesystem types
            switch(fsStats.f_type) {
                case PROC_SUPER_MAGIC:      // /proc
                case SYSFS_MAGIC:           // /sys
                case CGROUP_SUPER_MAGIC:    // cgroups
                case CGROUP2_SUPER_MAGIC:   // cgroups v2
                case DEBUGFS_MAGIC:         // debugfs
                case DEVPTS_SUPER_MAGIC:    // /dev/pts
                case SELINUX_MAGIC:         // selinuxfs
                case SECURITYFS_MAGIC:      // securityfs
                case TRACEFS_MAGIC:         // tracefs
                    SetArtificialFS();
                    break;
                default:
                    break;
        }   }
    #elif defined(__APPLE__)
        struct statfs fsStats;
        if (statfs(path.Terminate(), &fsStats) == 0) {
            // Check macOS artificial filesystems
            if (strcmp(fsStats.f_fstypename, "devfs") == 0 ||
                strcmp(fsStats.f_fstypename, "autofs") == 0) {
                SetArtificialFS();
        }   }
    #else
        // Fallback to device number check for other systems
        if(     STAT_DEV_MAJOR == 0
            &&  STAT_DEV_MINOR != 35 )  // tmpfs
                SetArtificialFS();
    #endif
    // assert that this is not a symbolic link. The stat path with this class is always real.
    ALIB_ASSERT_ERROR((STATMEMBER(mode) & S_IFMT) != S_IFMT, "SYSTEM",
          "Internal error: stat'ed file is symbolic link. Path not canonical \"{}\"", path )

  //=========================================== Copy Stats =========================================
  // 1. type
    {
        auto type= FileStatus::Types::UNKNOWN_OR_ERROR;
        auto posixType= STATMEMBER(mode) & S_IFMT;
        switch(posixType ) {
            case S_IFLNK :  type= FileStatus::Types::SYMBOLIC_LINK;
                ALIB_ERROR( "SYSTEM", "Impossible")  break;
            case S_IFBLK :  type= FileStatus::Types::BLOCK        ; break;
            case S_IFCHR :  type= FileStatus::Types::CHARACTER    ; break;
            case S_IFDIR :  type= FileStatus::Types::DIRECTORY    ; break;
            case S_IFIFO :  type= FileStatus::Types::FIFO         ; break;
            case S_IFREG :  type= FileStatus::Types::REGULAR      ; break;
            case S_IFSOCK:  type= FileStatus::Types::SOCKET       ; break;
            default: ALIB_ERROR("SYSTEM",
                   "Internal error. 'unknown' file type can't happen. File: \"{}\"", path )  break;
        }
        SetType( type );
    }

    // 2. perms
    SetPerms( FileStatus::Permissions(STATMEMBER(mode) & int32_t(FileStatus::Permissions::MASK)) );

    // 3. timestamps
    {
        #if defined(__APPLE__)
        #  define st_mtime_name  STATMEMBER(mtimespec)
        #  define st_ctime_name  STATMEMBER(ctimespec)
        #  define st_atime_name  STATMEMBER(atimespec)
        #else
        #   if TMP_STATX_AVAILABLE
        #       define st_mtime_name  STATMEMBER(mtime)
        #       define st_ctime_name  STATMEMBER(ctime)
        #       define st_atime_name  STATMEMBER(atime)
        #       define st_btime_name  STATMEMBER(btime)
        #   else
        #       define st_mtime_name  STATMEMBER(mtim)
        #       define st_ctime_name  STATMEMBER(ctim)
        #       define st_atime_name  STATMEMBER(atim)
        #   endif
        #endif
        DateTime dt;
        dt.Import(
                std::chrono::system_clock::time_point {
                        std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                std::chrono::seconds    {st_mtime_name.tv_sec }
                              + std::chrono::nanoseconds{st_mtime_name.tv_nsec}          )  } );
        SetMDate(dt);

        dt.Import(
                std::chrono::system_clock::time_point {
                        std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                std::chrono::seconds    {st_ctime_name.tv_sec }
                              + std::chrono::nanoseconds{st_ctime_name.tv_nsec}          )  } );
        SetCDate(dt);

        dt.Import(
                std::chrono::system_clock::time_point {
                        std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                std::chrono::seconds    {st_atime_name.tv_sec }
                              + std::chrono::nanoseconds{st_atime_name.tv_nsec}          )  } );
        SetADate(dt);

        #if TMP_STATX_AVAILABLE
        if( STATMEMBER(mask) & STATX_BTIME )  { // file systems supports "btime"?
            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {st_btime_name.tv_sec }
                                  + std::chrono::nanoseconds{st_btime_name.tv_nsec}   ) } );
            SetBDate(dt);
        } else {
            // use smallest of other times for "btime"
            auto btime= MDate();
            if( btime > CDate() ) btime= CDate();
            if( btime > ADate() ) btime= ADate();
            SetBDate( btime );

        }
        #else
        // use smallest of other times for "btime"
        auto btime= MDate();
        if( btime > CDate() ) btime= CDate();
        if( btime > ADate() ) btime= ADate();
        SetBDate( btime );
        #endif


      #undef st_mtime_name
      #undef st_ctime_name
      #undef st_atime_name
    }

    // 4. size
    SetSize( uinteger(STATMEMBER(size) ) );

    // 5. uid/gid
    SetOwner( STATMEMBER(uid) );
    SetGroup( STATMEMBER(gid) );

    // 6. qty of hard links
    SetQtyHardlinks( STATMEMBER(nlink) );

    return ScanState();

    #undef DBG_CHECKERRNO_WITH_PATH
    #undef TMP_STATX_AVAILABLE
    #undef STATMEMBER
}

//--------------------------------------------------------------------------------------------------
//--- UNKNOWN platform, using C++17 filesystem (not all functionality given)
//--------------------------------------------------------------------------------------------------
#else
#if ALIB_SYSTEM_FORCE_STD_FILE_STATUS
#   pragma message ("ALIB_SYSTEM_FORCE_STD_FILE_STATUS given. Using std::filesystem for " \
                    "FileStatus::Update(). (Limited functionality) " )
#else
#   pragma message ("Non-posix platform. Using std::filesystem for " \
                    "FileStatus::Update(). (Limited functionality) " )
#endif

FileStatus::ScanStates FileStatus::Update(Path& path, bool isCanonical) {
    std::filesystem::path stdPath(path.Terminate());
    return Update(stdPath, isCanonical);
}

FileStatus::ScanStates FileStatus::Update(std::filesystem::path& path, bool isCanonical) {

    // check
    #if ALIB_DEBUG
    if(isCanonical) {
        auto pc= fs::canonical(path);
        ALIB_ASSERT_ERROR(path == pc, "SYSTEM",
         "Non-canonical path given, while parameter 'isCanonical' is true: {}", path.c_str())
    }
    #endif

    // canonify path
    if(!isCanonical)
        path= fs::canonical(path);

    std::error_code errorCode;

    SetScanState( FileStatus::ScanStates::STATS );

    // read base stats (we have to use symlink_status() which does NOT follow the symlink!)
    fs::file_status stats= fs::symlink_status(path);
    ALIB_ALLOW_SPARSE_ENUM_SWITCH
    if(errorCode)
    {
        ALIB_ERROR("SYSTEM",
                   "Unhandled error code invoking 'fs::symlink_status()': {} (\"{}\")\n"
                   "    With file: \"{}\"",
                   errorCode.value(), errorCode.message(), path.c_str() )
        ALIB_DBG( errno= 0;)
        SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
        return ScanState();
    }
    ALIB_POP_ALLOWANCE
    ALIB_DBG(errno= 0;)

    // assert that this is not a symbolic link. The stat path with this class is always real.
    ALIB_ASSERT_ERROR(stats.type() != fs::file_type::symlink, "SYSTEM",
          "Internal error: stat'ed file is symbolic link. Path not canonical \"{}\"", path.c_str() )

  //=========================================== Copy Stats =========================================
  // 1. type
    {
        auto type= FileStatus::Types::UNKNOWN_OR_ERROR;
        switch( stats.type() )
        {
            case fs::file_type::directory:  type= FileStatus::Types::DIRECTORY    ;   break;
            case fs::file_type::regular  :  type= FileStatus::Types::REGULAR      ;   break;
            case fs::file_type::symlink  :  type= FileStatus::Types::SYMBOLIC_LINK;   break;
            case fs::file_type::block    :  type= FileStatus::Types::BLOCK        ;   break;
            case fs::file_type::character:  type= FileStatus::Types::CHARACTER    ;   break;
            case fs::file_type::fifo     :  type= FileStatus::Types::FIFO         ;   break;
            case fs::file_type::socket   :  type= FileStatus::Types::SOCKET       ;   break;

            case fs::file_type::not_found:
                 SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
                 ALIB_WARNING("SYSTEM", "Internal error. 'not found' file type can't happen. File: ", path.c_str() )
                 ALIB_DBG( errno= 0;) return ScanState();
            case fs::file_type::none     :
                 SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
                 ALIB_WARNING("SYSTEM", "Internal error. 'none' file type can't happen. File: ", path.c_str())
                 ALIB_DBG( errno= 0;) return ScanState();
            case fs::file_type::unknown  :
                 SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
                 ALIB_WARNING("SYSTEM", "Internal error. Can't happen. File: ", path.c_str())
                 ALIB_DBG( errno= 0;) return ScanState();
            default:
                 SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
                 ALIB_WARNING("SYSTEM", "Unknown fs::file_status::type '{}' with file {}.", stats.type(), path.c_str())
                 ALIB_DBG( errno= 0;) return ScanState();
        }
        SetType( type );
    }

    // 2. perms
    SetPerms( FileStatus::Permissions(int32_t(stats.permissions())) );

    // 3. timestamps
    // attn: This method always follows symbolic link and uses the target's time
    // This seems to be a confirmed behavior:
    // https://stackoverflow.com/questions/50778660/boost-filesystem-how-to-get-last-write-time-for-symlink-without-resolving
    auto fsTime= std::filesystem::file_time_type(std::filesystem::file_time_type::clock::now());
    if ( ScanState() <= FileStatus::ScanStates::RESOLVED ) // no error
    {
        fsTime= fs::last_write_time( path, errorCode );
        ALIB_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode) switch( std::errc(errorCode.value()) )
        {   // This happens if with symbolic links that point to nowhere.
            case std::errc::no_such_file_or_directory:
                ALIB_ERROR( "SYSTEM", "Internal error. This should never happen, checked above. "
                                     "Undefined system error handling" ) ALIB_DBG( errno= 0;)
                SetScanState(FileStatus::ScanStates::UNKNOWN_ERROR);
                break;

            default:
                ALIB_ERROR( "SYSTEM",
                   "Unhandled error code invoking 'fs::last_write_time()': {} (\"{}\")\n"
                   "    With file \"{}\".", errorCode.value(), errorCode.message(), path.c_str() )
                fsTime= (decltype(fsTime)::min)();                      ALIB_DBG( errno= 0;)
                break;
        }
        ALIB_POP_ALLOWANCE
    }


    #if  defined(__APPLE__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
        SetMDate( DateTime::FromEpochSeconds( to_time_t( fsTime ) ) );
    #else
        SetMDate( DateTime::FromEpochSeconds( std::chrono::system_clock::to_time_t(
                                                    std::chrono::clock_cast<std::chrono::system_clock>(fsTime) ) ) );
    #endif
    SetBDate( MDate() );
    SetCDate( MDate() );
    SetADate( MDate() );

    // 4. size
    errorCode.clear();
    SetSize(uinteger(fs::file_size(path, errorCode)));
    if( Size() == uinteger(-1))
    {
        SetSize(0);
        ALIB_ALLOW_SPARSE_ENUM_SWITCH
        switch( std::errc(errorCode.value()) )
        {
            // target is a directory (no error)
            case std::errc::is_a_directory:
                break;

            case std::errc::no_such_file_or_directory: // this happens if we have a broken symbolic link
                ALIB_ASSERT_ERROR(    Type() == FileStatus::Types::SYMBOLIC_LINK
                                   || Type() == FileStatus::Types::SYMBOLIC_LINK_DIR , "SYSTEM",
                   "Internal error. This should never happen. Undefined system error handling" )
                break;

            // size not supported. Happens with sockets, files in /proc, etc
            case std::errc::operation_not_supported: break;
               default: ALIB_ERROR("SYSTEM",
                   "Unhandled error code invoking 'directory_entry::file_size()':{} (\"{}\")\n"
                    "    With file \"{}\".",
                    errorCode.value(), errorCode.message(), path.c_str() ) ALIB_DBG( errno= 0;)
                break;
        }
        ALIB_POP_ALLOWANCE
    }

    // 5. uid/gid
    SetOwner( FileStatus::UnknownID );
    SetGroup( FileStatus::UnknownID );

    // 6. qty of hard links
    qtyHardLinks= uint32_t( fs::hard_link_count(path, errorCode ) );
    ALIB_ALLOW_SPARSE_ENUM_SWITCH
    if(errorCode)  {
        // fs::hard_link_coun always returns the hardlink-count of the symlink targets.
        // This fails on broken links. Therefore, this is not considered an error
        if( !(     std::errc(errorCode.value()) == std::errc::no_such_file_or_directory
                && Type() == FileStatus::Types::SYMBOLIC_LINK                    ) ) {
            ALIB_MESSAGE("SYSTEM",
                       "Unhandled error code invoking 'fs::hard_link_count()': {} (\"{}\")\n"
                       "    With file: \"{}\"",
                       errorCode.value(), errorCode.message(), path.c_str() )
            ALIB_DBG( errno= 0;)
        }
    }
    ALIB_POP_ALLOWANCE
    SetQtyHardlinks( qtyHardLinks );

    // that's it
    return ScanState();
}
#endif

} // namespace [alib::system]

ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::Permissions         , vt_files_perms  )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::Types               , vt_files_type   )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::TypeNames1Letter    , vt_files_type1  )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::TypeNames2Letters   , vt_files_type2  )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::TypeNames3Letters   , vt_files_type3  )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::ScanStates          , vt_files_qual   )
ALIB_BOXING_VTABLE_DEFINE( alib::system::FileStatus::ScanStates3Letters  , vt_files_qual3  )

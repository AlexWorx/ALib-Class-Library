#if !DOXYGEN
#   include "ALib.Lang.CIFunctions.H"


using namespace alib::system;

namespace alib::filetree {  namespace {

// forward declaration

CanonicalResult makeCanonicalRecursion( Path&               sourcePath,
                                        FTree::Cursor&      node,
                                        Path&               pathToNode,
                                        CanonicalPathList*  resultPaths,
                                        FTree::Cursor       callingNode  );

// scan parameters used with MakeCanonical to evaluate directory entries
ScanParameters paramsPathOnly( nullptr, ScanParameters::SymbolicLinks::RESOLVE_BUT_DONT_FOLLOW, 0, true, true );

} // namespace alib::filetree[::anonymous]

#if ALIB_DEBUG
   String DBG_FILETREE_SCAN_VERBOSE_LOG_FORMAT=
   A_CHAR(" {:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nf l}");
#endif

} // namespace [alib::filetree]


//--------------------------------------------------------------------------------------------------
//--- scanFilePosix
//--------------------------------------------------------------------------------------------------
#if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS

#if ALIB_DEBUG
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
    ALIB_ASSERT_WARNING(errno == 0, "FILETREE", "Errno set ({})\"{}\". Current path: {}",          \
                        errno, std::errc(errno), actPath )                                         \
    errno= 0;
#else
#   define DBG_CHECKERRNO_WITH_PATH
#endif

static_assert(std::is_same_v<alib::PathCharType,char>,
  "Posix systems use type <char> for file paths, while <alib::PathCharType> is not. "
  "Cannot compile POSIX file scanner!" );

// Since Kernel 4.11 Linux/glibc has "statx". We use it if available on the current platform.
#if defined(__NR_statx)
#   define TMP_STATX_AVAILABLE 1
#   define STATMEMBER(Name) stats.stx_ ## Name
#   define STAT_DEV_MAJOR   stats.stx_dev_major
#   define STAT_DEV_MINOR   stats.stx_dev_minor
#else
#   define TMP_STATX_AVAILABLE 0
#   define STATMEMBER(Name) stats.st_ ## Name
#   define STAT_DEV_MAJOR   major(stats.st_dev)
#   define STAT_DEV_MINOR   minor(stats.st_dev)
#endif


namespace alib::filetree {  namespace {
void scanFilePosix( DIR*                        pxDir,
                    FTree::Cursor&              node,
                    const NCString&             nameOrFullPath, // if full path, this has the same buffer as actPath!
                    unsigned                    depth,
                    ScanParameters&             params,
                    uint64_t                    curDev,
                    bool                        curDevIsArtificial,
                    FTValue::DirectorySums&     parentSums ,
                    NAString&                   actPath,
                    CanonicalPathList*          resultPaths )  {

    #if ALIB_DEBUG
        ALIB_ASSERT_ERROR(  actPath.CharAtStart()== DIRECTORY_SEPARATOR
                    &&  (   actPath.Length()==1
                         || actPath.CharAtEnd()  != DIRECTORY_SEPARATOR )
                    &&      actPath.IndexOf(strings::TLocalString<PathCharType,4>(
                                                 DIRECTORY_SEPARATOR).Append(DIRECTORY_SEPARATOR)) < 0 ,
                        "FILETREE","Given path \"{}\" not absolute or ending with '{}'",
                                actPath, DIRECTORY_SEPARATOR )

        NString512  dbgActFile;
        dbgActFile.DbgDisableBufferReplacementWarning();
        if( actPath.Buffer() == nameOrFullPath.Buffer() )
            dbgActFile << nameOrFullPath;
        else {
             dbgActFile << actPath;
             if(dbgActFile.Length()>1)
                 dbgActFile << DIRECTORY_SEPARATOR;
             dbgActFile << nameOrFullPath;
        }
        #if ALOX_DBG_LOG
            LocalAllocator<1> verboseAllocator;
            BoxesMA verboseLogables( verboseAllocator);
            int verboseLoggers;
            Log_IsActive(verboseLoggers, Verbosity::Verbose)
            if( verboseLoggers ) {
                verboseLogables.Add("{!AWidth:>} ");
                if( &params == &paramsPathOnly )
                    verboseLogables.Add("PO"); // 'Path Only'
                else {
                    auto& depthString= *verboseAllocator().New<String128>();
                    depthString << depth << DIRECTORY_SEPARATOR
                                << ( params.MaxDepth < std::numeric_limits<unsigned>::max()
                                     ? String128(params.MaxDepth)
                                     : String(A_CHAR("M")) );
                    verboseLogables.Add(depthString);
                }

                verboseLogables.Add(filetree::DBG_FILETREE_SCAN_VERBOSE_LOG_FORMAT, FTFile(node) );
            }
        #endif // ALOX_DBG_LOG

        ALIB_ASSERT_WARNING(errno == 0, "FILETREE", "Errno set ({})\"{}\" with current file: {}",
                                                errno, std::errc(errno), NString(dbgActFile) )
        errno= 0;
    #endif // ALIB_DEBUG

    FTValue& value       = *node;
    auto     oldScanState= value.ScanState();

  //------------------------------------------- get stats? -----------------------------------------
    if(          value.ScanState() == FTValue::ScanStates::NONE
        || (     value.ScanState() == FTValue::ScanStates::STATS
             &&  params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE ) )
    {
        value.SetScanState(FTValue::ScanStates::STATS);
        strings::TLocalString<PathCharType, PATH_MAX>  symLinkDest;
        strings::TLocalString<PathCharType, PATH_MAX>  symLinkDestReal;

        // read base stats
        ALIB_DBG( errno= 0;)
        #if TMP_STATX_AVAILABLE
            struct statx  stats;
            int statResult=  statx(  pxDir ? dirfd(pxDir) : AT_FDCWD,
                                     nameOrFullPath,
                                     AT_STATX_DONT_SYNC | AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
                                     STATX_BASIC_STATS | STATX_BTIME,
                                     &stats );

        #else
            struct stat stats;
            int statResult=  pxDir ? fstatat(dirfd(pxDir), nameOrFullPath, &stats,
                                     AT_SYMLINK_NOFOLLOW
                                     #if !defined(__APPLE__)
                                         | AT_NO_AUTOMOUNT
                                     #endif
                                     )
                                   : lstat  (              nameOrFullPath, &stats );
        #endif
        if( statResult ) {
            ALIB_ASSERT_WARNING( errno != ENOENT, "FILETREE",
                                   "File does not exist (anymore) while stating \"{}\"",
                                   NString(dbgActFile) )
            ALIB_ASSERT_WARNING( errno == ENOENT, "FILETREE", "Unknown error ({}) \"{}\" while stating file \"{}\"",
                                 errno, std::errc(errno), NString(dbgActFile) )
            value.SetScanState(errno == ENOENT ?  FTValue::ScanStates::NOT_EXISTENT
                                             :  FTValue::ScanStates::UNKNOWN_ERROR);
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;
        }
        DBG_CHECKERRNO_WITH_PATH

        // check filesystem type (artificial fs & mount point)
        uint64_t device= (uint64_t(STAT_DEV_MAJOR) << 32L) + STAT_DEV_MINOR;
        if( curDev == 0 || curDev != device ) {
           value.SetCrossingFS();

        // Use fstatfs to properly detect artificial filesystems
#if defined(__linux__)
            struct statfs fsStats;
            int fsResult;
            if (pxDir) {
                // Need full path for statfs - build it temporarily
                ALIB_STRING_RESETTER(actPath)
                if( actPath.Length() > 1 ) actPath << DIRECTORY_SEPARATOR;
                actPath << nameOrFullPath;
                fsResult = statfs(actPath, &fsStats);
            } else {
                fsResult = statfs(nameOrFullPath, &fsStats);
            }

            if (fsResult == 0) {
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
                        value.SetArtificialFS();
                        break;
                    default:
                        break;
            }   }
#elif defined(__APPLE__)
            struct statfs fsStats;
            int fsResult;
            if (pxDir) {
                // Need full path - build it temporarily
                ALIB_STRING_RESETTER(actPath)
                if( actPath.Length() > 1 ) actPath << DIRECTORY_SEPARATOR;
                actPath << nameOrFullPath;
                fsResult = statfs(actPath.Terminate(), &fsStats);
            } else {
                fsResult = statfs(nameOrFullPath, &fsStats);
            }
            if (fsResult == 0) {
                // Check macOS artificial filesystems
                if (strcmp(fsStats.f_fstypename, "devfs") == 0 ||
                    strcmp(fsStats.f_fstypename, "autofs") == 0) {
                    value.SetTargetArtificialFS();
            }   }
#       else
            // Fallback to device number check for other systems
            if(     STAT_DEV_MAJOR == 0
                &&  STAT_DEV_MINOR != 35 )  // tmpfs
                    value.SetArtificialFS();
#endif
            curDev=             device;
            curDevIsArtificial= value.IsArtificialFS();
        }


      //---------------------------------------- is symlink? ---------------------------------------
        bool origFileIsSymlink= (STATMEMBER(mode) & S_IFMT) == S_IFLNK;
        if(    origFileIsSymlink
            && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE )
        {
            value.SetScanState( FTValue::ScanStates::RESOLVED );

            // 1. Read plain symlink target (only to be attached to the entry)
            symLinkDest.EnsureRemainingCapacity(PATH_MAX - 1);
            ssize_t cntChars= pxDir ? readlinkat( dirfd(pxDir), nameOrFullPath, symLinkDest.VBuffer(), PATH_MAX)
                                    : readlink  (               nameOrFullPath, symLinkDest.VBuffer(), PATH_MAX);

            if (cntChars == -1) switch(errno) {
                case EACCES: value.SetScanState(FTValue::ScanStates::NO_ACCESS_SL);   ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;

                case ENOENT: value.SetScanState(FTValue::ScanStates::NO_ACCESS_SL);
                    ALIB_ASSERT_ERROR(STAT_DEV_MAJOR == 0, "FILETREE",
                      "Posix raised ({}) \"{}\" on reading a symbolic link which is not located on "
                      "an artificial filesystem (like /proc). File:\"{}\"",
                      errno, std::errc(errno), NString(dbgActFile) )             ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;

                default:     value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                    ALIB_ERROR("FILETREE", "Posix raised ({}) \"{}\" on reading symbolic link \"{}\"",
                               errno, std::errc(errno), NString(dbgActFile) )    ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;
            }
            symLinkDest.SetLength(cntChars);

            // 2. Read symlink's real target path (fully and recursively translated)
            ALIB_STRING_RESETTER(actPath)
            if( pxDir )
                actPath << DIRECTORY_SEPARATOR << nameOrFullPath;
            errno= 0;
            symLinkDestReal.EnsureRemainingCapacity(PATH_MAX - 1);
            *symLinkDestReal.VBuffer()= '\0';
            if(! realpath(actPath, symLinkDestReal.VBuffer() ) ) switch (errno)
                {   // The named file does not exist.
                    case ENOENT: if( *symLinkDestReal.VBuffer() != '\0')
                                    symLinkDestReal.DetectLength();
                                 value.SetScanState(FTValue::ScanStates::BROKEN_LINK);    ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    case ELOOP:  value.SetScanState(FTValue::ScanStates::CIRCULAR_LINK);  ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    // this might happen with strange system files
                    case EACCES: value.SetScanState(FTValue::ScanStates::NO_ACCESS_SL_TARGET); ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    default:     ALIB_ERROR("FILETREE", "Posix raised ({}) \"{}\" on resolving symbolic link \"{}\"",
                                            errno, std::errc(errno), NString(dbgActFile) )     ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                }
            symLinkDestReal.DetectLength();

            ALIB_DBG( if( errno == EINVAL) errno= 0;) // this happens, even though realpath() above returned 'OK'
            DBG_CHECKERRNO_WITH_PATH
            ALIB_ASSERT_ERROR( Path::IsAbsolute(symLinkDestReal),  "FILETREE",
                               "Real path is not absolute: ", symLinkDestReal )

            // 3. get resolved status
            DBG_CHECKERRNO_WITH_PATH
            #if TMP_STATX_AVAILABLE
                statResult= statx( AT_FDCWD,
                                   symLinkDestReal,
                                   AT_STATX_DONT_SYNC | AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
                                   STATX_ALL,
                                   &stats );
            #else
                statResult= stat(symLinkDestReal.Terminate(), &stats );
            #endif
            DBG_CHECKERRNO_WITH_PATH
            #if ALIB_CHARACTERS_WIDE
                symLinkDestReal.Reset(symLinkDestReal);
            #endif

            if(statResult == -1 ) {
                ALIB_ALLOW_SPARSE_ENUM_SWITCH
                if(errno) switch( std::errc(errno) )
                {   case std::errc::no_such_file_or_directory:
                        value.SetScanState(FTValue::ScanStates::BROKEN_LINK);
                        ALIB_DBG(errno= 0;)
                        goto APPLY_POST_RECURSION_FILTER;

                    default:
                        ALIB_WARNING("FILETREE",
                           "Unhandled error code invoking 'stat()' on resolved symbolic "
                           "link: {} (\"{}\")\n    Symbolic link target: \"{}\"",
                           errno, std::errc(errno), NString(dbgActFile) )
                        ALIB_DBG(errno= 0;)
                        value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                        goto APPLY_POST_RECURSION_FILTER;
                }
                ALIB_POP_ALLOWANCE
            }

            // check for target artificial fs
            uint64_t linkTargetDevice= (uint64_t(STAT_DEV_MAJOR) << 32L) + STAT_DEV_MINOR;
            if( linkTargetDevice == curDev ) {
                if( curDevIsArtificial ) value.SetTargetArtificialFS();
            }
            else {
#if defined(__linux__)
                struct statfs fsStats;
                if (statfs(symLinkDestReal, &fsStats) == 0) {
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
                            value.SetTargetArtificialFS();
                            break;
                        default:
                            break;
                }   }
#elif defined(__APPLE__)
                struct statfs fsStats;
                if (statfs(symLinkDestReal, &fsStats) == 0) {
                    // Check macOS artificial filesystems
                    if (strcmp(fsStats.f_fstypename, "devfs") == 0 ||
                        strcmp(fsStats.f_fstypename, "autofs") == 0) {
                        value.SetTargetArtificialFS();
                }   }
#else
                // Fallback to device number check for other systems
                if(     STAT_DEV_MAJOR == 0
                    &&  STAT_DEV_MINOR != 35 )  // tmpfs
                        value.SetTargetArtificialFS();
#endif
            }
        } // if is symlink && resolve symlinks

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

      //============================================================================================
      //========================================= Copy Stats =======================================
      //============================================================================================
        // 1. type
        {
            auto type= FileStatus::Types::UNKNOWN_OR_ERROR;
            auto posixType= STATMEMBER(mode) & S_IFMT;
            if( origFileIsSymlink  ) {
                type= posixType == S_IFDIR ? FileStatus::Types::SYMBOLIC_LINK_DIR
                                           : FileStatus::Types::SYMBOLIC_LINK;
            }
            else switch(STATMEMBER(mode) & S_IFMT ) {
                case S_IFLNK :  type= FileStatus::Types::SYMBOLIC_LINK; ALIB_ERROR( "FILETREE", "Impossible")
                    break;
                case S_IFBLK :  type= FileStatus::Types::BLOCK        ; break;
                case S_IFCHR :  type= FileStatus::Types::CHARACTER    ; break;
                case S_IFDIR :  type= FileStatus::Types::DIRECTORY    ; break;
                case S_IFIFO :  type= FileStatus::Types::FIFO         ; break;
                case S_IFREG :  type= FileStatus::Types::REGULAR      ; break;
                case S_IFSOCK:  type= FileStatus::Types::SOCKET       ; break;
                default: ALIB_ERROR("FILETREE",
                                "Internal error. 'unknown' file type can't happen. File: \"{}\"",
                                 NString(dbgActFile) )                  break;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FileStatus::Permissions(STATMEMBER(mode) & int32_t(FileStatus::Permissions::MASK)) );

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
            value.SetMDate(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {st_ctime_name.tv_sec }
                                  + std::chrono::nanoseconds{st_ctime_name.tv_nsec}          )  } );
            value.SetCDate(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {st_atime_name.tv_sec }
                                  + std::chrono::nanoseconds{st_atime_name.tv_nsec}          )  } );
            value.SetADate(dt);

            #if TMP_STATX_AVAILABLE
                if( STATMEMBER(mask) & STATX_BTIME )  { // file systems supports "btime"?
                    dt.Import(
                            std::chrono::system_clock::time_point {
                                    std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                            std::chrono::seconds    {st_btime_name.tv_sec }
                                          + std::chrono::nanoseconds{st_btime_name.tv_nsec}   ) } );
                    value.SetBDate(dt);
                } else {
                    // use smallest of other times for "btime"
                    auto btime= value.MDate();
                    if( btime > value.CDate() ) btime= value.CDate();
                    if( btime > value.ADate() ) btime= value.ADate();
                    value.SetBDate( btime );

                }
            #else
                // use smallest of other times for "btime"
                auto btime= value.MDate();
                if( btime > value.CDate() ) btime= value.CDate();
                if( btime > value.ADate() ) btime= value.ADate();
                value.SetBDate( btime );
            #endif


          #undef st_mtime_name
          #undef st_ctime_name
          #undef st_atime_name
        }

        // 4. size
        value.SetSize( uinteger(STATMEMBER(size) ) );

        // 5. uid/gid
        value.SetOwner( STATMEMBER(uid) );
        value.SetGroup( STATMEMBER(gid) );

        // 6. qty of hard links
        value.SetQtyHardlinks( STATMEMBER(nlink) );

        // 7. Add extended information
        if(    oldScanState < FTValue::ScanStates::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) ) {
                FTFile(node).GetFTree().AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );
        }

    } // if scan stats (state was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.ScanState() == FTValue::ScanStates::BROKEN_LINK) {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_POST_RECURSION_FILTER;
    }

  //================================================================================================
  //================================== recursion with directories? =================================
  //================================================================================================
    if(   !value.IsDirectory()
        || value.ScanState() >= FTValue::ScanStates::RECURSIVE )
        goto APPLY_POST_RECURSION_FILTER;

    // stop recursion due to artificial fs?
    if( value.IsArtificialFS() && !params.IncludeArtificialFS ) {
        Log_Prune( if( verboseLogables.Size() )  verboseLogables.Add(" NO_AFS"); )
        value.SetScanState( FTValue::ScanStates::NO_AFS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems ) {
        Log_Prune( if( verboseLogables.Size() )  verboseLogables.Add(" NOT_CROSSING_FS"); )
        value.SetScanState( FTValue::ScanStates::NOT_CROSSING_FS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth ) {
        Log_Prune( if( verboseLogables.Size() && (&params != &paramsPathOnly) )  verboseLogables.Add(" MAX_DEPTH_REACHED"); )
        value.SetScanState( FTValue::ScanStates::MAX_DEPTH_REACHED );
        ++parentSums.QtyStopsOnMaxDepth;
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to filter
    if(      depth > 0
         &&  params.DirectoryFilterPreRecursion
         && !params.DirectoryFilterPreRecursion->Includes( node, actPath )  )
    {
        Log_Prune( if( verboseLogables.Size() )  verboseLogables.Add(" FILTERED(Pre)"); )
        goto APPLY_POST_RECURSION_FILTER;
    }

    // mark as recursively scanned
    value.SetScanState( FTValue::ScanStates::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FileStatus::Types::SYMBOLIC_LINK_DIR ) {
        // stop if not wanted, and: never recurse with symlinks RESIDING on artificial fs!
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )  {
            value.SetScanState( FTValue::ScanStates::NOT_FOLLOWED );
            goto APPLY_POST_RECURSION_FILTER;
        }

        // stop if target is artificial and such are not wanted
        if( value.TargetIsArtificialFS() &&  !params.IncludeArtificialFS ) {
            value.SetScanState( FTValue::ScanStates::NO_AFS );
            goto APPLY_POST_RECURSION_FILTER;
        }
        
        // recurse into the symlink target
        Path realTargetPath; realTargetPath.DbgDisableBufferReplacementWarning();
        realTargetPath._<NC>(actPath);
        auto targetNode= node.Parent();
        Path sourcePath= value.GetLinkTarget();
        CanonicalResult targetSearchResult= MakeCanonical( sourcePath, targetNode,
                                                           realTargetPath, resultPaths);

        // not found?
        if(targetNode.IsInvalid()) {
            node->SetScanState(FTValue::ScanStates::BROKEN_LINK);
            goto APPLY_POST_RECURSION_FILTER;
        }

        // apply symbolic parent (only if it was not existing before)
        if( targetSearchResult.ScanState != FTValue::ScanStates::DUPLICATE )
            FTFile(targetNode).SetSymbolicParent(node.Export());

        // recurse into target (if it was not existing or if it was not scanned previously)
        if(    targetSearchResult.ScanState != FTValue::ScanStates::DUPLICATE
            || targetNode->ScanState() == FTValue::ScanStates::NONE ) {
        
            CPathString cRealTarget= realTargetPath.Terminate();
            FTValue::DirectorySums childSums;
            scanFilePosix(nullptr, targetNode, cRealTarget,  depth + 1, params,
                          curDev, curDevIsArtificial, parentSums,
                          realTargetPath, resultPaths );

            value.SetSums(childSums);
            parentSums+= childSums;
        }
        goto APPLY_POST_RECURSION_FILTER;
    }

    // DIRECTORY RECURSION
    {ALIB_STRING_RESETTER( actPath )

        // open directory
        if( pxDir == nullptr ) {
            ALIB_ASSERT_ERROR(actPath.Buffer() == nameOrFullPath.Buffer(),"FILETREE","Internal error")
            actPath.SetLength(nameOrFullPath.Length());
        } else {
            if( actPath.Length() > 1 ) actPath << DIRECTORY_SEPARATOR;
            actPath << nameOrFullPath;
        }

        errno= 0;
        int fd;
        if( pxDir)
            fd=   openat( dirfd(pxDir), nameOrFullPath, O_RDONLY | O_DIRECTORY );
        else {
            fd=   open  (               actPath       , O_RDONLY | O_DIRECTORY );
        }

        if (fd != -1)  { // success?
            DBG_CHECKERRNO_WITH_PATH
            FTValue::DirectorySums subSums;
            DIR* childDir = fdopendir(fd);
            for(;;) {
                errno= 0;
                dirent*  pxEntry       = readdir(childDir);
                if( pxEntry == nullptr ) {
                    switch(errno) {
                        // possible errors (according to documentation):
                        // EOVERFLOW One of the values in the structure to be returned cannot  be represented correctly.
                        // EBADF     The dirp argument does not refer to an open directory stream.
                        // ENOENT    The current position of the directory stream is invalid.
                        case 0:      break;
                        case EACCES: value.SetScanState(FTValue::ScanStates::NO_ACCESS_DIR);
                                     break;
                        case EINVAL: value.SetScanState( FTValue::ScanStates::NO_ACCESS_DIR);
                            ALIB_ASSERT_ERROR(major(curDev) == 0, "FILETREE",
                               "Posix raised ({}) \"{}\" on reading a directory which is not "
                               "located on an artificial filesystem (like /proc). File:\"{}\"",
                                              errno, std::errc(errno), NString(dbgActFile) )
                                    break;
                        default:    value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                                    ALIB_ERROR("FILETREE",
                                        "Posix raised ({}) \"{}\" on reading directory \"{}\"",
                                        errno, std::errc(errno), NString(dbgActFile) )
                                    break;
                    }
                    errno= 0;
                    break;
                }

                // skip "." and ".."
                if(             pxEntry->d_name[0] == '.'
                                && (        pxEntry->d_name[1] == '\0'
                                            || (   pxEntry->d_name[1] == '.'
                                                   && pxEntry->d_name[2] == '\0' ) ) )
                    continue;

                //----- recursive call -----
                const NCString childName(const_cast<const char*>(&pxEntry->d_name[0]));
                auto childNode= node;
                childNode.GoToCreateChildIfNotExistent( childName );
                auto childCopy= childNode;
                scanFilePosix( childDir, childNode, childName,
                               depth + 1, params, curDev, curDevIsArtificial, subSums, actPath,
                               resultPaths );
                // In case this entry was reached before through a symbolic link, it is now
                // reached directly. Thus, we clear the symbolic link
                FTFile(childCopy).ClearSymbolicParent();
            } // dir entry loop
            closedir(childDir);
            DBG_CHECKERRNO_WITH_PATH

            // previously scanned in lower state?
            if( oldScanState != FTValue::ScanStates::NONE ) {
                FTree::FixSums( node );
                parentSums+= value.Sums();
            } else {
                value.SetSums(subSums);
                parentSums+= subSums;
            }
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;
        } // success opening directory

        // error with recursion
        ALIB_ASSERT_ERROR(errno != ENOTDIR, "FILETREE",
                          "Internal error opening directory. This must never happen")


        ALIB_ALLOW_SPARSE_ENUM_SWITCH
        switch (std::errc(errno)) {
            case std::errc::permission_denied:
                ++parentSums.QtyErrsAccess;
                value.SetScanState( FTValue::ScanStates::NO_ACCESS_DIR );
                errno= 0;
                break;

            default:
            ALIB_ERROR("FILETREE", "Unknown error {}(\"{}\") while opening directory \"{}\"",
                       errno, std::errc(errno), actPath)
                value.SetScanState( FTValue::ScanStates::UNKNOWN_ERROR );
                break;
    }   }

  //================================================================================================
  //========================= Apply Post Filter and remove empty directories =======================
  //================================================================================================
    APPLY_POST_RECURSION_FILTER:
    // delete node only if this was a new scan.
    // It must not be deleted if this node was created as a path.
    if( oldScanState == FTValue::ScanStates::NONE ) {
        if ( value.IsDirectory() ) {
            if(    depth > 0
                && (    (     params.DirectoryFilterPostRecursion
                          && !params.DirectoryFilterPostRecursion->Includes(node, actPath )  )
                     || (     params.RemoveEmptyDirectories
                          &&  value.Sums().Count() == 0                                      )
                   )                                                                             )
            {
                Log_Prune( if( verboseLogables.Size() ) { verboseLogables.Add(" FILTERED(Post)");
                                                          Log_Verbose( verboseLogables )
                                                          verboseLogables.clear(); } )
                parentSums-= value.Sums();
                value.Sums()= FTValue::DirectorySums();

                // Notify deletion of all children.
                if( node.HasChildren() && node.Tree<FTree>().HasListeners() ) {
                    integer oldLen= actPath.Length();
                    auto it= node.FirstChild();
                    while ( it.IsValid() ) {
                        actPath << DIRECTORY_SEPARATOR << it.Name();
                        node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode, node, actPath );
                        it.GoToNextSibling();
                        actPath.ShortenTo(oldLen);
                }   }
                if( params.RemoveEmptyDirectories ) {
                    ALIB_STRING_RESETTER(actPath)
                    actPath << DIRECTORY_SEPARATOR << node.Name();
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode, node, actPath );
                    node.Delete();
                    return;
                }

                // do not return here. Still count the type below
                node.DeleteChildren();
            }

        } else {
            if (      params.FileFilter
                  && !params.FileFilter->Includes(node, actPath )   )
            {
                if( node.Tree<FTree>().HasListeners() ) {
                    ALIB_STRING_RESETTER(actPath)
                    actPath << DIRECTORY_SEPARATOR << node.Name();
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode, node, actPath );
                }
                Log_Prune( if( verboseLogables.Size() ) { verboseLogables.Add(" FILTERED(Post)");
                                                          Log_Verbose( verboseLogables )  } )
                node.Delete();
                return;
    }   }   }

    Log_Prune( if( verboseLogables.Size() ) Log_Verbose( verboseLogables ) )

    // cnt file type
    parentSums.Add(value);
    if(node.Tree<FTree>().HasListeners()) {
        ALIB_STRING_RESETTER(actPath)
        actPath << DIRECTORY_SEPARATOR << node.Name();
        node.Tree<FTree>().Notify( FTreeListener::Event::CreateNode, node, actPath );
    }

    ALIB_POP_ALLOWANCE
    DBG_CHECKERRNO_WITH_PATH
} // scanFilePosix()

}} // namespace [alib::filetree::anonymous]
#undef DBG_CHECKERRNO_WITH_PATH
#undef TMP_STATX_AVAILABLE
#undef STATMEMBER


//--------------------------------------------------------------------------------------------------
//--- UNKNOWN platform, using C++17 filesystem (not all functionality given)
//--------------------------------------------------------------------------------------------------
#else
#if ALIB_SYSTEM_FORCE_STD_FILE_STATUS
#   pragma message ("ALIB_SYSTEM_FORCE_STD_FILE_STATUS given. Using std::filesystem for " \
                    "filetree::ScanFiles(). (Limited functionality) " )
#else
#   pragma message ("Non-posix platform. Using std::filesystem for " \
                    "filetree::ScanFiles(). (Limited functionality) " )
#endif


// Note: MacOS is currently (as of 231210) missing C++20 library features in the area of std::clock
#if  defined(__APPLE__)  || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
namespace
{
    template <typename TP>
    std::time_t to_time_t(TP tp)
    {
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()
                        + std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }
}
#endif
#if ALIB_DEBUG
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
    ALIB_ASSERT_WARNING(errno == 0, "FILETREE", "Errno set ({})\"{}\". Current path: {}",         \
                        errno, std::errc(errno), path.string() )                            \
    errno= 0;
#else
#   define DBG_CHECKERRNO_WITH_PATH
#endif

namespace alib::filetree {  namespace {

void scanFileStdFS( const fs::path&               path,
                    FTree::Cursor&                node,
                    unsigned                      depth,
                    ScanParameters&               params,
                    FTValue::DirectorySums&         parentSums,
                    CanonicalPathList*            resultPaths     )
{
    #if defined(__MINGW32__)
        Path       pathAsCString(path.c_str());
        pathAsCString.Terminate();
    #else
        CPathString           pathAsCString(path.c_str());
    #endif
    const PathSubstring   parentPath= pathAsCString.Substring(0, pathAsCString.LastIndexOf(DIRECTORY_SEPARATOR));


    Log_Verbose(  "[{}] {}/{}   {}",  &params != &paramsPathOnly ? '>':'P',  depth,
                  params.MaxDepth < (std::numeric_limits<unsigned>::max)()
                            ? String128(params.MaxDepth)
                            : String(A_CHAR("M")),
                  pathAsCString )

    std::error_code errorCode;
    auto&           value     = node.Value();
    auto            oldScanState= value.ScanState();

  //------------------------------------------- get stats? -----------------------------------------
    if(         value.ScanState() == FTValue::ScanStates::NONE
        || (    value.ScanState() == FTValue::ScanStates::STATS
             && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE ) )  {

        value.SetScanState( FTValue::ScanStates::STATS );
        Path          symLinkDest;
        Path          symLinkDestReal;

        // read base stats (we have to use symlink_status() which does NOT follow the symlink!)
        fs::file_status stats= fs::symlink_status(path);
        ALIB_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode)
        {
            ALIB_ERROR("FILETREE",
                       "Unhandled error code invoking 'fs::symlink_status()': {} (\"{}\")\n"
                       "    With file: \"{}\"",
                       errorCode.value(), errorCode.message(), pathAsCString )
            ALIB_DBG( errno= 0;)
            value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
            goto APPLY_POST_RECURSION_FILTER;
        }
        ALIB_POP_ALLOWANCE
        ALIB_DBG(errno= 0;)

      //---------------------------------------- is symlink? ---------------------------------------
        bool origFileIsSymlink= (stats.type() == fs::file_type::symlink);
        if(    origFileIsSymlink
            && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE )
        {
            value.SetScanState( FTValue::ScanStates::RESOLVED );

            // 1. Read plain symlink target (only to be attached to the entry)
            fs::path resolved= fs::read_symlink(path, errorCode);
            if(errorCode)
            {
                ALIB_ALLOW_SPARSE_ENUM_SWITCH
                switch( std::errc(errorCode.value()) )
                {   case std::errc::no_such_file_or_directory: // happens with /proc files
                    case std::errc::permission_denied:
                        value.SetScanState(FTValue::ScanStates::NO_ACCESS_SL);
                        ALIB_DBG(errno= 0;)
                        goto ABORT_SYMLINK;
                    default:
                        ALIB_ERROR("FILETREE",
                            "Unhandled error code invoking 'fs::read_symlink()': {} (\"{}\")\n"
                            "   with file: ", errorCode.value(),
                                                   errorCode.message(), pathAsCString )
                        ALIB_DBG( errno= 0;)
                        value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                        goto APPLY_POST_RECURSION_FILTER;
                }
                ALIB_POP_ALLOWANCE
            }
            DBG_CHECKERRNO_WITH_PATH
            symLinkDest << resolved.c_str();

            // 2. Read symlink's real target path (fully and recursively translated)
            fs::path realPath;
            if( resolved.is_absolute() )
                realPath= fs::canonical(resolved, errorCode);
            else
            {
                symLinkDestReal << pathAsCString;
                symLinkDestReal.ShortenTo( symLinkDestReal.LastIndexOf(DIRECTORY_SEPARATOR) + 1);
                symLinkDestReal << symLinkDest;
                realPath=  fs::canonical(fs::path(
                               std::basic_string_view<PathCharType>(symLinkDestReal.Buffer(),
                                                             size_t(symLinkDestReal.Length()))),
                                         errorCode);
                symLinkDestReal.Reset();
            }
            ALIB_DBG(if(errno==EINVAL && !errorCode) errno= 0;) // this happens!, we do not care, but clean up
            ALIB_DBG(if(errno==ENOENT && !errorCode) errno= 0;)

            ALIB_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( std::errc(errorCode.value()) )
            {   // we ignore this: std::fs would not create the "real path" if the final directory is not accessible.
                case std::errc::permission_denied:             value.SetScanState(FTValue::ScanStates::NO_ACCESS_SL_TARGET); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case std::errc::no_such_file_or_directory:     value.SetScanState(FTValue::ScanStates::BROKEN_LINK);         ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case std::errc::too_many_symbolic_link_levels: value.SetScanState(FTValue::ScanStates::CIRCULAR_LINK);       ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                default:  ALIB_ERROR("FILETREE", "Unhandled error code invoking 'fs::canonical()': {} (\"{}\")\n"
                                     "   with file: ", errorCode.value(), errorCode.message(), pathAsCString )
                    goto ABORT_SYMLINK;
            }
            ALIB_POP_ALLOWANCE
            DBG_CHECKERRNO_WITH_PATH
            symLinkDestReal << realPath.c_str();

            // 3. get resolved status
            ALIB_ALLOW_SPARSE_ENUM_SWITCH
            auto newStatus= fs::status(path, errorCode);
            if(!errorCode)
            {
                // this happens with strange /proc files...
                if(newStatus.type() != fs::file_type::unknown)
                    stats= newStatus;
            }
            else switch( std::errc(errorCode.value()) )
            {   case std::errc::operation_not_permitted:       value.SetScanState( FTValue::ScanStates::NO_ACCESS    ); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case std::errc::no_such_file_or_directory:     value.SetScanState( FTValue::ScanStates::BROKEN_LINK  ); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case std::errc::too_many_symbolic_link_levels: value.SetScanState( FTValue::ScanStates::CIRCULAR_LINK); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                default: ALIB_WARNING("FILETREE",
                          "Unhandled error code invoking 'directory_entry::status()': {} (\"{}\")\n"
                          "  With file: \"{}\"",
                           errorCode.value(), errorCode.message(), pathAsCString )
                         goto ABORT_SYMLINK;
            }
            ALIB_POP_ALLOWANCE

            // check for target artificial fs
            //  -/- Not available with std::filesystem version
        }

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

      //============================================================================================
      //========================================= Copy Stats =======================================
      //============================================================================================
      // 1. type
        {
            auto type= FileStatus::Types::UNKNOWN_OR_ERROR;
            if( origFileIsSymlink  )
            {
                type= is_directory(stats) ? FileStatus::Types::SYMBOLIC_LINK_DIR
                                          : FileStatus::Types::SYMBOLIC_LINK;
            }
            else switch( stats.type() )
            {
                case fs::file_type::directory:  type= FileStatus::Types::DIRECTORY    ;   break;
                case fs::file_type::regular  :  type= FileStatus::Types::REGULAR      ;   break;
                case fs::file_type::symlink  :  type= FileStatus::Types::SYMBOLIC_LINK;   break;  // for now, this is a file.
                case fs::file_type::block    :  type= FileStatus::Types::BLOCK        ;   break;
                case fs::file_type::character:  type= FileStatus::Types::CHARACTER    ;   break;
                case fs::file_type::fifo     :  type= FileStatus::Types::FIFO         ;   break;
                case fs::file_type::socket   :  type= FileStatus::Types::SOCKET       ;   break;

                case fs::file_type::not_found:
                     value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                     ALIB_WARNING("FILETREE", "Internal error. 'not found' file type can't happen. File: ", pathAsCString )
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                case fs::file_type::none     :
                     value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                     ALIB_WARNING("FILETREE", "Internal error. 'none' file type can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                case fs::file_type::unknown  :
                     value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                     ALIB_WARNING("FILETREE", "Internal error. Can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                default:
                     value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                     ALIB_WARNING("FILETREE", "Unknown fs::file_status::type '{}' with file {}.", stats.type(), pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FileStatus::Permissions(int32_t(stats.permissions())) );

        // 3. timestamps
        // attn: This method always follows symbolic link and uses the target's time
        // This seems to be a confirmed behavior:
        // https://stackoverflow.com/questions/50778660/boost-filesystem-how-to-get-last-write-time-for-symlink-without-resolving
        auto fsTime= std::filesystem::file_time_type(std::filesystem::file_time_type::clock::now());
        if ( value.ScanState() <= FTValue::ScanStates::RESOLVED ) // no error
        {
            fsTime= fs::last_write_time( path, errorCode );
            ALIB_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( std::errc(errorCode.value()) )
            {   // This happens if with symbolic links that point to nowhere.
                case std::errc::no_such_file_or_directory:
                    ALIB_ERROR( "FILETREE", "Internal error. This should never happen, checked above. "
                                         "Undefined system error handling" ) ALIB_DBG( errno= 0;)
                    value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
                    break;

                default:
                    ALIB_ERROR( "FILETREE",
                       "Unhandled error code invoking 'fs::last_write_time()': {} (\"{}\")\n"
                       "    With file \"{}\".", errorCode.value(), errorCode.message(), pathAsCString )
                    fsTime= (decltype(fsTime)::min)();                      ALIB_DBG( errno= 0;)
                    break;
            }
            ALIB_POP_ALLOWANCE
        }


        #if  defined(__APPLE__) || defined(_LIBCPP_VERSION) || defined(__ANDROID_NDK__)
            value.SetMDate( DateTime::FromEpochSeconds( to_time_t( fsTime ) ) );
        #else
            value.SetMDate( DateTime::FromEpochSeconds( std::chrono::system_clock::to_time_t(
                                                        std::chrono::clock_cast<std::chrono::system_clock>(fsTime) ) ) );
        #endif
        value.SetBDate( value.MDate() );
        value.SetCDate( value.MDate() );
        value.SetADate( value.MDate() );

        // 4. size
        errorCode.clear();
        value.SetSize(   symLinkDest.Length() > 0                           ?  uinteger(symLinkDest.Length())
                       : value.ScanState() <= FTValue::ScanStates::RESOLVED ?  uinteger(fs::file_size(path, errorCode))
                       : 0 );
        if( value.Size() == uinteger(-1))
        {
            value.SetSize(0);
            ALIB_ALLOW_SPARSE_ENUM_SWITCH
            switch( std::errc(errorCode.value()) )
            {
                // target is a directory (no error)
                case std::errc::is_a_directory:
                    break;

                case std::errc::no_such_file_or_directory: // this happens if we have a broken symbolic link
                    ALIB_ASSERT_ERROR(    value.Type() == FileStatus::Types::SYMBOLIC_LINK
                                       || value.Type() == FileStatus::Types::SYMBOLIC_LINK_DIR , "FILETREE",
                       "Internal error. This should never happen. Undefined system error handling" )
                    break;

                // size not supported. Happens with sockets, files in /proc, etc
                case std::errc::operation_not_supported: break;
                   default: ALIB_ERROR("FILETREE",
                       "Unhandled error code invoking 'directory_entry::file_size()':{} (\"{}\")\n"
                        "    With file \"{}\".",
                        errorCode.value(), errorCode.message(), pathAsCString ) ALIB_DBG( errno= 0;)
                    break;
            }
            ALIB_POP_ALLOWANCE
        }

        // 5. uid/gid
        value.SetOwner( FTValue::UnknownID );
        value.SetGroup( FTValue::UnknownID );

        // 6. qty of hard links
        uint32_t qtyHardLinks= uint32_t( fs::hard_link_count(path, errorCode ) );
        ALIB_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode)
        {
            // fs::hard_link_coun always returns the hardlink-count of the symlink targets.
            // This fails on broken links. Therefore, this is not considered an error
            if( !(     std::errc(errorCode.value()) == std::errc::no_such_file_or_directory
                    && value.Type() == FileStatus::Types::SYMBOLIC_LINK                    ) ) {
                ALIB_MESSAGE("FILETREE",
                           "Unhandled error code invoking 'fs::hard_link_count()': {} (\"{}\")\n"
                           "    With file: \"{}\"",
                           errorCode.value(), errorCode.message(), pathAsCString )
                ALIB_DBG( errno= 0;)
            }
        }
        ALIB_POP_ALLOWANCE
        value.SetQtyHardlinks( qtyHardLinks );

        // 7. Add extended information
        if(    oldScanState < FTValue::ScanStates::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) )
                FTFile(node).GetFTree().AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );
    } // if scan stats (state was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.ScanState() == FTValue::ScanStates::BROKEN_LINK)
    {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_POST_RECURSION_FILTER;
    }

  //---------------------------------- recursion with directories? ---------------------------------
    if(    !value.IsDirectory()
        || value.ScanState() >= FTValue::ScanStates::RECURSIVE )
        goto APPLY_POST_RECURSION_FILTER;


    // stop recursion due to artificial fs?
        // Not supported with std::filesystem!

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems )
    {
        value.SetScanState( FTValue::ScanStates::NOT_CROSSING_FS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth )
    {
        value.SetScanState( FTValue::ScanStates::MAX_DEPTH_REACHED );
        ++parentSums.QtyStopsOnMaxDepth;
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to filter
    if(      depth > 0
         &&  params.DirectoryFilterPreRecursion
         && !params.DirectoryFilterPreRecursion->Includes( node, parentPath )  )
        goto APPLY_POST_RECURSION_FILTER;

    // mark as recursively scanned
    value.SetScanState( FTValue::ScanStates::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FileStatus::Types::SYMBOLIC_LINK_DIR )
    {
        // stop if not wanted, and: never recurse with symlinks RESIDING on artificial fs!
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )  {
            value.SetScanState( FTValue::ScanStates::NOT_FOLLOWED );
            goto APPLY_POST_RECURSION_FILTER;
        }

        // recurse into the symlink target
        Path realTargetPath= parentPath;
        auto targetNode= node.Parent();
        Path sourcePath= value.GetLinkTarget();
        CanonicalResult targetSearchResult= MakeCanonical(sourcePath, targetNode,
                                                          realTargetPath, resultPaths);

        // not found?
        if(targetNode.IsInvalid()) {
            node->SetScanState(FTValue::ScanStates::BROKEN_LINK);
            goto APPLY_POST_RECURSION_FILTER;
        }

        // apply symbolic parent (only if it was not existing before)
        if( targetSearchResult.ScanState != FTValue::ScanStates::DUPLICATE )
            FTFile(targetNode).SetSymbolicParent(node.Export());

        // recurse into target (if it was not existing or if it was not scanned previously)
        if(    targetSearchResult.ScanState != FTValue::ScanStates::DUPLICATE
            || targetNode->ScanState() == FTValue::ScanStates::NONE ) {

            FTValue::DirectorySums childSums;
            scanFileStdFS( fs::path(std::basic_string_view<PathCharType>(realTargetPath.Buffer(),
                                                                  size_t(realTargetPath.Length()))),
                           targetNode, depth + 1, params, childSums, resultPaths );

            value.SetSums(childSums);
            parentSums+= childSums;
        }
        goto APPLY_POST_RECURSION_FILTER;
    }

    // DIRECTORY RECURSION
    {
        fs::directory_iterator dit= fs::directory_iterator(path, errorCode);
        if(!errorCode) // success?
        {
            FTValue::DirectorySums subSums;
            for( const fs::directory_entry& childDir : dit )
            {
                // recursive call
            #if defined(_WIN32)
                Path            mingwBuf( childDir.path().c_str());
                PathSubstring   childName(mingwBuf);
            #else
                NSubstring childName(NCString(childDir.path().c_str()));
            #endif
                childName.ConsumeChars(childName.LastIndexOf(DIRECTORY_SEPARATOR) + 1);
                auto childNode= node;
                childNode.GoToCreateChildIfNotExistent( childName );
                auto childCopy= childNode;
                scanFileStdFS( childDir.path(), childNode, depth + 1, params, subSums,
                               resultPaths );

                // In case this entry was reached before through a symbolic link, it is now
                // reached directly. Thus, we clear the symbolic link
                FTFile(childCopy).ClearSymbolicParent();

            }

            // previously scanned in lower quality?
            if( oldScanState != FTValue::ScanStates::NONE )
            {
                FTree::FixSums( node );
                parentSums+= value.Sums();
            }
            else
            {
                value.SetSums(subSums);
                parentSums+= subSums;
            }
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;
        }
    }

    // error with recursion
    ALIB_ASSERT_ERROR( errorCode.value() != ENOTDIR, "FILETREE",
                       "Internal error opening directory. This must never happen" )

    ALIB_ALLOW_SPARSE_ENUM_SWITCH
    if(errorCode) switch (std::errc(errorCode.value()))
    {
        case std::errc::invalid_argument: // happens with /proc
        case std::errc::permission_denied:
            ++parentSums.QtyErrsAccess;
            value.SetScanState( FTValue::ScanStates::NO_ACCESS_DIR );
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;

        default: value.SetScanState(FTValue::ScanStates::UNKNOWN_ERROR);
            ALIB_ERROR("FILETREE", "Unknown error {}(\"{}\") while opening directory \"{}\"",
                       errorCode.value(), errorCode.message(), pathAsCString)
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;
    }
    ALIB_POP_ALLOWANCE
    ALIB_DBG( errno= 0;)

  //------------------------------------------ Apply Filter ----------------------------------------
    APPLY_POST_RECURSION_FILTER:
    // delete node only if this was a new scan. It must not be deleted if this node was
    // created as a path.
    if( oldScanState == FTValue::ScanStates::NONE ) {
        if ( value.IsDirectory() ) {
            if(    depth > 0
                && (    (     params.DirectoryFilterPostRecursion
                          && !params.DirectoryFilterPostRecursion->Includes(node, parentPath )  )
                     || (     params.RemoveEmptyDirectories
                          &&  value.Sums().Count() == 0                                      )
                   )                                                                             )
            {
                parentSums-= value.Sums();
                value.Sums()= FTValue::DirectorySums();

                // Notify deletion of all children.
                if( node.HasChildren() && node.Tree<FTree>().HasListeners() ) {
                    Path p= pathAsCString;
                    auto it= node.FirstChild();
                    while ( it.IsValid() ) {
                        p.ShortenTo(pathAsCString.Length());
                        p << DIRECTORY_SEPARATOR << it.Name();
                        node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode, node, p );
                        it.GoToNextSibling();
                    }
                }

                if( params.RemoveEmptyDirectories )
                {
                    node.Tree<FTree>().Notify(FTreeListener::Event::DeleteNode, node, pathAsCString);
                    node.Delete();
                    return;
                }


                // do not return here. Still count the type below
                node.DeleteChildren();
            }

        } else { // not directory
            if ( params.FileFilter && !params.FileFilter->Includes(node, parentPath ) )  {
                node.Tree<FTree>().Notify(FTreeListener::Event::DeleteNode, node, pathAsCString)  ;
                node.Delete();
                return;
            }
        }
    }

    // cnt file type and notify listeners
    parentSums.Add(value);
    node.Tree<FTree>().Notify( FTreeListener::Event::CreateNode, node, pathAsCString );

} // scanFileStdFS


}} // namespace [alib::filetree::anonymous]

#undef DBG_CHECKERRNO_WITH_PATH
#endif  // std::fs version

//--------------------------------------------------------------------------------------------------
//--- ALL Platforms
//--------------------------------------------------------------------------------------------------
namespace alib::filetree {

namespace {


struct UnrealPathRootInfo {
    PathRootKind    kind;
    integer         prefixLength;
    PathSubstring   scheme;
};

inline bool isAsciiAlpha(PathCharType c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline bool isSchemeChar(PathCharType c) {
    return isAsciiAlpha(c) || (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.';
}

inline bool IsAsciiNoCaseEqual(PathCharType c, PathCharType cmp) {
    return c == cmp || c == (cmp ^ 0x20);
}

UnrealPathRootInfo detectAndNormalizeUnrealRoot( Path&          sourcePath,
                                                 FTree::Cursor& node,
                                                 Path&          pathToNode ) {
    
    UnrealPathRootInfo info{ PathRootKind::Relative, 0, PathSubstring() };

    if( sourcePath.IsEmpty() )
        return info;

    #if defined(_WIN32)
    // Windows device paths: "\\.\"
    if( sourcePath.Length() >= 4
        && (   (sourcePath.CharAt(0) == '\\' && sourcePath.CharAt(1) == '\\')
            || (sourcePath.CharAt(0) == '/'  && sourcePath.CharAt(1) == '/') )
        &&  sourcePath.CharAt(2) == '.'
        && (sourcePath.CharAt(3) == '\\' || sourcePath.CharAt(3) == '/') ) {
        node.GoToRoot();
        info.kind= PathRootKind::Device;
        sourcePath.DeleteStart(4);
        pathToNode.Reset(DIRECTORY_SEPARATOR);

        if( sourcePath.Length() >= 4
            && IsAsciiNoCaseEqual(sourcePath.CharAt(0), 'U')
            && IsAsciiNoCaseEqual(sourcePath.CharAt(1), 'N')
            && IsAsciiNoCaseEqual(sourcePath.CharAt(2), 'C')
            && (sourcePath.CharAt(3) == '\\' || sourcePath.CharAt(3) == '/') ) {
            sourcePath.DeleteStart(4);
            Path prefix;
            prefix << DIRECTORY_SEPARATOR << DIRECTORY_SEPARATOR;
            sourcePath.InsertAt(prefix, 0);
        } else if( sourcePath.IsEmpty() ) {
            return info;
        } else {
            info.kind= PathRootKind::Device;
            info.prefixLength= 4;
            return info;
    }   }
    #endif

    // URL scheme detection: "<scheme>://"
    integer schemeEnd= sourcePath.IndexOf(':');
    if(    schemeEnd > 0
        && schemeEnd + 2 < sourcePath.Length()
        && sourcePath.CharAt(schemeEnd + 1) == '/'
        && sourcePath.CharAt(schemeEnd + 2) == '/' ) {
        bool schemeValid= isAsciiAlpha(sourcePath.CharAt(0));
        for( integer i= 1; schemeValid && i < schemeEnd; ++i )
            schemeValid= isSchemeChar(sourcePath.CharAt(i));
        if( schemeValid ) {
            info.kind= PathRootKind::URL;
            info.prefixLength= schemeEnd + 3;
            info.scheme= sourcePath.Substring(0, schemeEnd);
            return info;
        }
        info.kind= PathRootKind::Errorneous;
        return info;
    }

    // Windows drive letter: "C:\" or "C:/"
    #if defined(_WIN32)
    if(    sourcePath.Length() >= 2
        && isAsciiAlpha(sourcePath.CharAt(0))
        && sourcePath.CharAt(1) == ':' ) {

        // if no backslash is given, this a relative path
        if(    sourcePath.Length() < 3
            || (sourcePath.CharAt(2) != '\\' && sourcePath.CharAt(2) != '/' ) )
            return info;

        info.kind= PathRootKind::DriveLetter;
        info.prefixLength= 2;
        node.GoToRoot();
        PathSubstring drive= sourcePath.Substring(0, 2);
        node.GoToCreateChildIfNotExistent(drive);
        pathToNode.Reset();
        pathToNode << drive;

        if( sourcePath.Length() > 2
            && (sourcePath.CharAt(2) == '/' || sourcePath.CharAt(2) == '\\') ) {
            ++info.prefixLength;
            sourcePath.DeleteStart(3);
            pathToNode << DIRECTORY_SEPARATOR;
        } else {
            sourcePath.DeleteStart(2);
        }
        return info;
    }
    #endif

    // UNC path (leading double separators).
    if( sourcePath.Length() >= 2
        && (   (sourcePath.CharAt(0) == '/'  && sourcePath.CharAt(1) == '/')
            || (sourcePath.CharAt(0) == '\\' && sourcePath.CharAt(1) == '\\') ) ) {
        info.kind= PathRootKind::UNC;
        info.prefixLength= 2;
        node.GoToRoot();
        sourcePath.DeleteStart(2);
        pathToNode.Reset();
        pathToNode << DIRECTORY_SEPARATOR << DIRECTORY_SEPARATOR;
        return info;
    }

    // if non-windows OS, detect common absolute path starting with /.
    #if !defined(_WIN32)
    int absoluteEnd= Path::IsAbsolute(sourcePath);
    if(absoluteEnd==1) {
        info.kind= PathRootKind::AbsoluteRoot;
        info.prefixLength= 1;
        node.GoToRoot();
        sourcePath.DeleteStart(1);
        pathToNode.Reset(DIRECTORY_SEPARATOR);
    }
    #endif
    

    return info;
}

CanonicalResult  makeCanonicalRecursion( Path&               sourcePath,
                                         FTree::Cursor&      node,
                                         Path&               pathToNode,
                                         CanonicalPathList*  resultPaths,
                                         FTree::Cursor       callingNode   )  {

    auto rootInfo= detectAndNormalizeUnrealRoot(sourcePath, node, pathToNode);
    if( rootInfo.kind == PathRootKind::URL ) {
        node.GoToRoot();
        node.GoToCreateChildIfNotExistent(rootInfo.scheme);
        node->SetType(FileStatus::Types::SOCKET);
        node->SetScanState(FTValue::ScanStates::NOT_EXISTENT);
        pathToNode.Reset();
        pathToNode << rootInfo.scheme << A_CHAR("://");
        sourcePath.Reset();
        return CanonicalResult{rootInfo.kind, FTValue::ScanStates::NOT_EXISTENT};
    }

    if( rootInfo.kind == PathRootKind::Device ) {
        node.GoToRoot();
        Path devName;
        devName << A_CHAR("DEV");
        if( sourcePath.IsNotEmpty() ) {
            devName << A_CHAR(":");
            for( integer i= 0; i < sourcePath.Length(); ++i ) {
                PathCharType c= sourcePath.CharAt(i);
                devName << ( (c == '/' || c == '\\') ? PathCharType(':') : c );
        }   }
        node.GoToCreateChildIfNotExistent(devName);
        node->SetType(FileStatus::Types::SOCKET);
        node->SetScanState(FTValue::ScanStates::NOT_EXISTENT);
        pathToNode.Reset();
        pathToNode << devName;
        sourcePath.Reset();
        return CanonicalResult{rootInfo.kind, FTValue::ScanStates::NOT_EXISTENT};
    }

    // existed already?
    if( sourcePath.IsEmpty() )
        return CanonicalResult{rootInfo.kind, FTValue::ScanStates::DUPLICATE};

    // this is our result value
    bool isNew= false;

    // create folders until we hit a symbolic link
    while(sourcePath.IsNotEmpty()) {
        do {
            PathSubstring name(sourcePath.Substring(0, (std::min)( sourcePath.IndexOfOrLength('/'),
                                                                   sourcePath.IndexOfOrLength('\\')) ));
            if(name.Equals(A_PATH(".")) || name.Equals(A_PATH("/"))) {
                sourcePath.DeleteStart(2);
                continue;
            }
            if(name.Equals(A_PATH(".."))) {
                sourcePath.DeleteStart(3);
                pathToNode.ChangeToParent();
                if(!node.IsRoot())
                    node= node.Parent();
                continue;
            }
            if(name.IsEmpty())
                return CanonicalResult{ rootInfo.kind,   isNew ? FTValue::ScanStates::NONE
                                                               : FTValue::ScanStates::DUPLICATE };

            // create or goto existing
            isNew= node.GoToCreateChildIfNotExistent(name);
            if(pathToNode.Length() >  1)
                    pathToNode << DIRECTORY_SEPARATOR;
            pathToNode << name;
            sourcePath.DeleteStart(name.Length() + 1);
        } while(!isNew && !node->IsSymbolicLink());

        if( isNew ) {
            // scan the node non-recursively, with link-resolving
            FTValue::DirectorySums dummySums;
            #if ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
                pathToNode.Terminate();
                CPathString  fullPathChildName= pathToNode;
                {
                    // add node name to existing path and use same buffer for fullPathChildName!
                    fullPathChildName= pathToNode;
                }
                scanFilePosix( nullptr, node, fullPathChildName,
                               0, paramsPathOnly,
                               0, false, dummySums, pathToNode, resultPaths );
            #else
                scanFileStdFS( fs::path(std::basic_string_view<PathCharType>(pathToNode.Buffer(),
                                                                      size_t(pathToNode.Length()))),
                               node, 0,
                               paramsPathOnly,
                               dummySums, resultPaths );
            #endif

            auto quality= node->ScanState();
            // if not found, put node name back to sourcePath, set node to invalid and return
            // an error.
            if( quality == FTValue::ScanStates::NOT_EXISTENT ) {
                sourcePath.InsertAt(strings::TLocalString<PathCharType, 8>(DIRECTORY_SEPARATOR), 0);
                sourcePath.InsertAt(node.Name(), 0);
                node.Delete();
                node= FTree::Cursor();
                return CanonicalResult{rootInfo.kind, FTValue::ScanStates::NOT_EXISTENT};
            }

            // Correct quality from MAX_DEPTH_REACHED to STATS
            if(isNew) {
                if( quality == FTValue::ScanStates::MAX_DEPTH_REACHED )
                    node->SetScanState(FTValue::ScanStates::NONE);
                else if( quality != FTValue::ScanStates::RESOLVED )
                    return CanonicalResult{rootInfo.kind, quality};
            } else {
                if(    quality >= FTValue::ScanStates::NOT_CROSSING_FS
                    && quality <= FTValue::ScanStates::CIRCULAR_LINK )
                    return CanonicalResult{rootInfo.kind, quality};
        }   }

        // Symbolic link found?
        if(node->IsSymbolicLink()) {
            node->SetScanState(FTValue::ScanStates::RESOLVED);
            // recursive call
            Path unrealPathToSymlink=     node->GetLinkTarget();
            auto targetNode= node.Parent();
            pathToNode.ChangeToParent();
            CanonicalResult result=
            makeCanonicalRecursion(unrealPathToSymlink, targetNode, pathToNode, resultPaths, node );
            if( targetNode.IsInvalid() || result.ScanState == FTValue::ScanStates::NOT_EXISTENT )
                return CanonicalResult{rootInfo.kind, result.ScanState}; // <-- Use THIS level's rootInfo.kind!

            if(    result.ScanState != FTValue::ScanStates::DUPLICATE
                || targetNode->ScanState() == FTValue::ScanStates::NONE ) {
                FTFile(targetNode).SetSymbolicParent(node.Export());
                if(callingNode.IsValid())
                    FTFile(node).SetSymbolicParent(callingNode.Export());
            }

            node= targetNode;
    }   }
    return CanonicalResult{ rootInfo.kind, isNew ? FTValue::ScanStates::NONE
                                                 : FTValue::ScanStates::DUPLICATE };

}  // makeCanonicalRecursion

} // namespace  alib::filetree[::anonymous]

#endif // !DOXYGEN

CanonicalResult  MakeCanonical( Path&               sourcePath,
                                FTree::Cursor&      node,
                                Path&               pathToNode,
                                CanonicalPathList*  resultPaths ) {

    FTree::Cursor recursionNode;
    CanonicalResult result= makeCanonicalRecursion( sourcePath, node, pathToNode, resultPaths,
                                                    recursionNode );

    if(node.IsValid() && result.ScanState==FTValue::ScanStates::NONE && resultPaths)
        resultPaths->Add(node);
    return result;
}

//--------------------------------------------------------------------------------------------------
//--- ScanFiles()
//--------------------------------------------------------------------------------------------------
FTValue::ScanStates  ScanFiles( FTree&             tree,
                                ScanParameters&    parameters,
                                CanonicalPathList* resultPaths,
                                Path*              remainingStart) {

    Log_SetDomain( "ALIB/FILETREE", Scope::Path)
    Log_SetDomain( "SCAN"         , Scope::Filename)


    Log_Info( "Scanning: P=  {}\n"
              "          F={} DPre={} DPost={} XFS={} AFS={} Depth={}",
              parameters.StartPath,
              parameters.FileFilter                  .get() ? 'Y':'N',
              parameters.DirectoryFilterPreRecursion .get() ? 'Y':'N',
              parameters.DirectoryFilterPostRecursion.get() ? 'Y':'N',
              parameters.CrossFileSystems ? 'Y':'N', parameters.IncludeArtificialFS ? 'Y':'N',
              parameters.MaxDepth == ScanParameters::InfiniteRecursion ? String(A_CHAR("Inf"))
                                                                       : String128(parameters.MaxDepth)
            )

    // a relative path was given? Interpret this as relative to the current directory
    Path  sourcePath(parameters.StartPath);
    Log_Prune( bool isAbsolutePath= sourcePath.IsAbsolute(); )
    sourcePath.MakeAbsolute();
    if( !sourcePath.Exists() ) {
        Log_Info( "Scan path (given relative!) not existent. Current folder {!Q} "
                  "has no child-path {!Q}.", sourcePath, parameters.StartPath )
        return FileStatus::ScanStates::NOT_EXISTENT;
    }
    Log_If( !isAbsolutePath, Verbosity::Info, "Changed relative start path to {}", sourcePath)

  //----------------------------------------- start scanning ---------------------------------------
    ALIB_DBG( errno=0;
    Log_Prune(auto firstResultPos= resultPaths->size(); ))
    FTValue::DirectorySums dummySums;

    // make path canonical
    FTree::Cursor targetNode= tree.Root().AsCursor();
    Path realPath; realPath.DbgDisableBufferReplacementWarning();
    CanonicalResult result=  MakeCanonical( sourcePath, targetNode, realPath, resultPaths );
    if(remainingStart) remainingStart->Reset(sourcePath);
    if( targetNode.IsInvalid() ) {
        Log_Info( "Scan path not resolved. Unresolved portion {!Q}", realPath )
        return result.ScanState;
    }
    auto originalTargetNode= targetNode;
    
    // scan
    #if  ALIB_SYSTEM_FILE_STATUS_IMPL == ALIB_SYSTEM_FILE_POSIX_STATUS
        CPathString    fullPathChildName= realPath;
        scanFilePosix(nullptr, targetNode, fullPathChildName, 0 ,parameters, 0, false, dummySums,
                      realPath, resultPaths );
    #else
        scanFileStdFS( fs::path(std::basic_string_view<PathCharType>(realPath.Buffer(),
                                                              size_t(realPath.Length()))),
                       targetNode, 0, parameters, dummySums, resultPaths );
    #endif
    if(resultPaths) {
        Log_Info( "Scan Results: ", resultPaths->size() - firstResultPos )
        Log_Prune( int cntPaths= 0;
                   for( auto& it : *resultPaths ) {
                       Path path;

                       Log_Info( "    Path {}: {}  (Q={} D={}/F={}}",
                                 cntPaths++, path,
                                 it->ScanState(),
                                 it->ScanState() > FTValue::ScanStates::STATS && it->IsDirectory() ? it->Sums().CountDirectories()   : 0,
                                 it->ScanState() > FTValue::ScanStates::STATS && it->IsDirectory() ? it->Sums().CountNonDirectories(): 0    )
                   } )
    }

    return resultPaths && resultPaths->size() > 0 ? resultPaths->back()->ScanState()
                                                  : targetNode->ScanState();
}


void CanonicalPathList::Add(FTFile node) {
    for(auto existingIt= begin(); existingIt != end(); ++existingIt) {

        // check if the new node supersedes this node
        FTFile nodeParents= *existingIt;
        while( nodeParents.IsValid() ) {
            if( nodeParents == node ){
                *existingIt= node;
                return;
            }
            nodeParents= nodeParents.Parent();
        }

        // check if the new node is superseded by this existing
        nodeParents= node.Parent();
        while( nodeParents.IsValid() ) {
            if( nodeParents == *existingIt )
                return;
            nodeParents= nodeParents.Parent();
    }   }
    insert(end(), node);
}
} // namespace [alib::filetree]

#   include "ALib.Lang.CIMethods.H"

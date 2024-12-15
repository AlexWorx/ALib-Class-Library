// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software lbicense, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/files/fscanner.hpp"
#include "alib/monomem/localallocator.hpp"

#include "alib/strings/util/tokenizer.hpp"
#include "alib/alox.hpp"

#include "alib/lang/callerinfo_functions.hpp"

#if !DOXYGEN



using namespace alib::lang::system;

namespace alib::files {  namespace {                    

    // forward declaration of startScan()
    bool startScan( FTree&                      tree,
                    PathString                  realPath,
                    ScanParameters&             params,
                    FInfo::DirectorySums&       parentSums,
                    std::vector<ResultsPaths>&  resultPaths
  IF_ALIB_THREADS(, SharedLock*                 lock          ) );

    // scan parameters used with startScan to evaluate directory entries
    ScanParameters paramsPathOnly( nullptr, ScanParameters::SymbolicLinks::DONT_RESOLVE, 0, true, true );
} // namespace alib::files[::anonymous]

#if ALIB_DEBUG
String DBG_FILES_SCAN_VERBOSE_LOG_FORMAT=
   A_CHAR(" {:ta h{2,r} on{10,r} gn{10,r} s(IEC){10,r} dm qqq FxFa (rd{3r}' D' rf{3r}' F' re{2r}' EA' rb{2r}'BL) 'nf l}");
#endif

} // namespace [alib::files]

//--------------------------------------------------------------------------------------------------
//--- scanFilePosix
//--------------------------------------------------------------------------------------------------
#if  ALIB_FILES_SCANNER_IMPL == ALIB_FILES_SCANNER_POSIX
#   include <unistd.h>
#   if defined(__linux__)
#       include <asm/unistd.h>
#   endif
#   include <dirent.h>
#   if defined(__linux__)
#       include <linux/stat.h>
#       include <sys/stat.h>
#   else
#       include <sys/stat.h>
#   endif
#   if !defined(__APPLE__)
#       include <sys/sysmacros.h>
#   else
#       include <sys/types.h>
#   endif
#   include <pwd.h>
#   include <fcntl.h>
#   include <pwd.h>
#   include <grp.h>

#if ALIB_DEBUG
#   define DBG_CHECKERRNO                                                                          \
        ALIB_ASSERT_WARNING(errno == 0, "FILES", "Errno set ({}){!Q}.",                       \
                            errno, SystemErrors(errno) )                                           \
        errno= 0;
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
        ALIB_ASSERT_WARNING(errno == 0, "FILES", "Errno set ({}){!Q}. Current path: {}",      \
                            errno, SystemErrors(errno), actPath )                                  \
        errno= 0;
#else
#   define DBG_CHECKERRNO
#   define DBG_CHECKERRNO_WITH_PATH
#endif

// Since Kernel 4.11 Linux/glibc has "statx". We use this is available on the current platform.
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

namespace alib::files {  namespace {
void scanFilePosix( DIR*                        pxDir,
                    FTree::Cursor&              node,
                    const CPathString&          nameOrFullPath, // if full path, this has the same buffer as actPath!
                    unsigned int                depth,
                    ScanParameters&             params,
                    uint64_t                    currentDevice,
                    FInfo::DirectorySums&       parentSums ,
                    Path&                       actPath,
                    std::vector<ResultsPaths>&  resultPaths
  IF_ALIB_THREADS(, SharedLock*                 lock)                           )
{
    ALIB_ASSERT_ERROR(          actPath.CharAtStart()== DIRECTORY_SEPARATOR
                        &&  (   actPath.Length()==1
                             || actPath.CharAtEnd()  != DIRECTORY_SEPARATOR )
                        &&      actPath.IndexOf(strings::TLocalString<lang::system::PathCharType,4>(
                                                     DIRECTORY_SEPARATOR).Append(DIRECTORY_SEPARATOR)) < 0 ,
                    "FILES","Given path not absolute or ending with '{}': {}", DIRECTORY_SEPARATOR, actPath )
    ALIB_DBG(  Path       dbgActFile;
               if( actPath.Buffer() == nameOrFullPath.Buffer() )
                   dbgActFile << nameOrFullPath;
               else
               {
                    dbgActFile << actPath;
                    if(dbgActFile.Length()>1)
                        dbgActFile << DIRECTORY_SEPARATOR;
                    dbgActFile << nameOrFullPath;
               }                                                   )
    Log_Prune(
        LocalAllocator<1> verboseAllocator;
        BoxesMA verboseLogables( verboseAllocator);
        int verboseLoggers;
        Log_IsActive(verboseLoggers, Verbosity::Verbose)
        if( verboseLoggers )
        {
            verboseLogables.Add("{!AWidth:>} ");
            if( &params == &paramsPathOnly )
                verboseLogables.Add("PO"); // 'Path Only'
            else
            {
                auto& depthString= *verboseAllocator().New<String128>();
                depthString << depth << DIRECTORY_SEPARATOR
                            << ( params.MaxDepth < std::numeric_limits<unsigned int>::max()
                                 ? String128(params.MaxDepth)
                                 : String(A_CHAR("M")) );
                verboseLogables.Add(depthString); 
            }

            verboseLogables.Add(files::DBG_FILES_SCAN_VERBOSE_LOG_FORMAT,  File(node) );
        }
    )
    ALIB_ASSERT_WARNING(errno == 0, "FILES", "Errno set ({}){!Q} with current file: {}",
                        errno, SystemErrors(errno), dbgActFile )
    ALIB_DBG( errno= 0;)

    auto& value    = *node;
    auto oldQuality= value.Quality();

    ALIB_STRINGS_TO_NARROW(nameOrFullPath, nNameOrFullPath, 512)

    // ------------------------------   get stats?  ------------------------------
    if(          value.Quality() == FInfo::Qualities::NONE
        || (     value.Quality() == FInfo::Qualities::STATS
             &&  params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE ) )
    {
        value.SetQuality(FInfo::Qualities::STATS);
        Path          symLinkDest;
        Path          symLinkDestReal;

        // read base stats
        ALIB_DBG( errno= 0;)
        #if TMP_STATX_AVAILABLE
            struct statx  stats;
            int statResult=  statx(  pxDir ? dirfd(pxDir) : 0,
                                     nNameOrFullPath,
                                     AT_STATX_DONT_SYNC | AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
                                     STATX_BASIC_STATS | STATX_BTIME,
                                     &stats );

        #else
            struct stat stats;
            int statResult=  pxDir ? fstatat(dirfd(pxDir), nNameOrFullPath, &stats,
                                     AT_SYMLINK_NOFOLLOW
                                     #if !defined(__APPLE__)
                                         | AT_NO_AUTOMOUNT
                                     #endif
                                                                )
                                   : lstat  (              nNameOrFullPath, &stats );
        #endif
        if( statResult )
        {
            ALIB_ASSERT_WARNING( errno != ENOENT, "FILES", "File does not exist (anymore) while stating {!Q}",
                                                                  dbgActFile )
            ALIB_ASSERT_WARNING( errno == ENOENT, "FILES", "Unknown error ({}) {!Q} while stating file {!Q}",
                                 errno, SystemErrors(errno), dbgActFile )
            value.SetQuality(errno == ENOENT ?  FInfo::Qualities::NOT_EXISTENT
                                             :  FInfo::Qualities::UNKNOWN_ERROR);
            ALIB_DBG( errno= 0;)
            goto APPLY_POST_RECURSION_FILTER;
        }
        DBG_CHECKERRNO_WITH_PATH

        // check filesystem type (artificial fs & mount point)
        {
            uint64_t device= (uint64_t(STAT_DEV_MAJOR) << 32L) + STAT_DEV_MINOR;
                 if( currentDevice == 0)           currentDevice= device;
            else if( currentDevice != device)   {  currentDevice= device;   value.SetCrossingFS();  }
        }

        if(     STAT_DEV_MAJOR == 0    // artificial?
            &&  STAT_DEV_MINOR != 35 ) // tmpfs included, not considered artificial!
            value.SetArtificialFS();

        //------------ is symlink? ------------
        bool origFileIsSymlink= (STATMEMBER(mode) & S_IFMT) == S_IFLNK;
        if(    origFileIsSymlink
            && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE )
        {
            value.SetQuality( FInfo::Qualities::RESOLVED );

            // 1. Read plain symlink target (only to be attached to the entry)
            ALIB_STRINGS_TO_NARROW(symLinkDest, nSymLinkDest, 512)
            ssize_t cntChars= pxDir ? readlinkat( dirfd(pxDir), nNameOrFullPath, nSymLinkDest.VBuffer(), PATH_MAX)
                                    : readlink  (               nNameOrFullPath, nSymLinkDest.VBuffer(), PATH_MAX);

            if (cntChars == -1) switch(errno)
            {
                case EACCES: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL);      ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;

                case ENOENT: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL);
                    ALIB_ASSERT_ERROR(STAT_DEV_MAJOR == 0, "FILES",
                                      "Posix raised ({}) {!Q} on reading a symbolic link which is not located on an "
                                      "artificial filesystem (like /proc). File:{!Q}",
                                      errno, SystemErrors(errno), dbgActFile )               ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;

                default:     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                    ALIB_ERROR("FILES", "Posix raised ({}) {!Q} on reading symbolic link {!Q}",
                               errno, SystemErrors(errno), dbgActFile )      ALIB_DBG(errno= 0;)
                    goto ABORT_SYMLINK;
            }
            nSymLinkDest.SetLength(cntChars);
            #if ALIB_CHARACTERS_WIDE
                symLinkDest.Reset(nSymLinkDest);
            #endif

            // 2. Read symlink's real target path (fully and recursively translated)
            ALIB_STRING_RESETTER(actPath);
            if( pxDir )
                actPath << DIRECTORY_SEPARATOR << nameOrFullPath;
            errno= 0;
            ALIB_STRINGS_TO_NARROW(actPath        , nActPath        , 512)
            ALIB_STRINGS_TO_NARROW(symLinkDestReal, nSymLinkDestReal, 512)
            *nSymLinkDestReal.VBuffer()= '\0';
            if(! realpath(nActPath.Terminate(), nSymLinkDestReal.VBuffer() ) ) switch (errno)
                {   // The named file does not exist.
                    case ENOENT: if( *nSymLinkDestReal.VBuffer() != '\0')
                                    nSymLinkDestReal.DetectLength();
                                 value.SetQuality(FInfo::Qualities::BROKEN_LINK);    ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    case ELOOP:  value.SetQuality(FInfo::Qualities::CIRCULAR_LINK);  ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    // this might happen with strange system files
                    case EACCES: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL_TARGET); ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    default:     ALIB_ERROR("FILES", "Posix raised ({}) {!Q} on resolving symbolic link {!Q}",
                                            errno, SystemErrors(errno), dbgActFile )     ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                }
            nSymLinkDestReal.DetectLength();

            ALIB_DBG( if( errno == EINVAL) errno= 0;) // this happens, even though realpath() above returned 'OK'
            DBG_CHECKERRNO_WITH_PATH
            ALIB_ASSERT_ERROR( Path::IsAbsolute(symLinkDestReal),  "FILES",
                                                 "Real path is not absolute: ", nSymLinkDestReal )

            // 3. get resolved status
            DBG_CHECKERRNO_WITH_PATH
            #if TMP_STATX_AVAILABLE
                statResult= statx( 0,
                                   nSymLinkDestReal.Terminate(),
                                   AT_STATX_DONT_SYNC | AT_NO_AUTOMOUNT | AT_SYMLINK_NOFOLLOW,
                                   STATX_ALL,
                                   &stats );
            #else
                statResult= stat(nSymLinkDestReal.Terminate(), &stats );
            #endif
            DBG_CHECKERRNO_WITH_PATH
            #if ALIB_CHARACTERS_WIDE
                symLinkDestReal.Reset(nSymLinkDestReal);
            #endif

            if(statResult == -1 )
            {
                ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
                if(errno) switch( SystemErrors(errno) )
                {   case SystemErrors::enoent:  value.SetQuality(FInfo::Qualities::BROKEN_LINK);
                                                ALIB_DBG(errno= 0;)
                                                goto APPLY_POST_RECURSION_FILTER;
                    default: ALIB_WARNING("FILES",
                                 "Unhandled error code invoking 'stat()' on resolved symbolic link: {} ({!Q})\n"
                                 "    Symbolic link target: {!Q}", errno, SystemErrors(errno), dbgActFile )
                                 ALIB_DBG(errno= 0;)
                             value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                             goto APPLY_POST_RECURSION_FILTER;
                }
                ALIB_WARNINGS_RESTORE
            }

            // check for target artificial fs
            if(     STAT_DEV_MAJOR == 0    // artificial?
                &&  STAT_DEV_MINOR != 35 ) // tempfs included!
                value.SetTargetArtificialFS();

        } // if is symlink && resolve symlinks

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

        // =========================================================================================
        // ==================================      Copy Stats     ==================================
        // =========================================================================================
        // 1. type
        {
            auto type= FInfo::Types::UNKNOWN_OR_ERROR;
            auto posixType= STATMEMBER(mode) & S_IFMT;
            if( origFileIsSymlink  )
            {
                type= posixType == S_IFDIR ? FInfo::Types::SYMBOLIC_LINK_DIR
                                           : FInfo::Types::SYMBOLIC_LINK;
            }
            else switch(STATMEMBER(mode) & S_IFMT )
            {
                case S_IFLNK :  type= FInfo::Types::SYMBOLIC_LINK; ALIB_ERROR( "FILES", "Impossible")
                    break;
                case S_IFBLK :  type= FInfo::Types::BLOCK        ; break;
                case S_IFCHR :  type= FInfo::Types::CHARACTER    ; break;
                case S_IFDIR :  type= FInfo::Types::DIRECTORY    ; break;
                case S_IFIFO :  type= FInfo::Types::FIFO         ; break;
                case S_IFREG :  type= FInfo::Types::REGULAR      ; break;
                case S_IFSOCK:  type= FInfo::Types::SOCKET       ; break;
                default:  ALIB_ERROR("FILES", "Internal error. 'unknown' file type can't happen. File: {!Q}",
                                     dbgActFile )    break;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FInfo::Permissions(STATMEMBER(mode) & int32_t(FInfo::Permissions::MASK)) );

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
                                  + std::chrono::nanoseconds{st_mtime_name.tv_nsec}          )    } );
            value.SetMDate(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {st_ctime_name.tv_sec }
                                  + std::chrono::nanoseconds{st_ctime_name.tv_nsec}          )    } );
            value.SetCDate(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {st_atime_name.tv_sec }
                                  + std::chrono::nanoseconds{st_atime_name.tv_nsec}          )    } );
            value.SetADate(dt);

            #if TMP_STATX_AVAILABLE
                if( STATMEMBER(mask) & STATX_BTIME ) // file systems supports "btime"?
                {
                    dt.Import(
                            std::chrono::system_clock::time_point {
                                    std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                            std::chrono::seconds    {st_btime_name.tv_sec }
                                          + std::chrono::nanoseconds{st_btime_name.tv_nsec}          )    } );
                    value.SetBDate(dt);
                }
                else
                {
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

        // 6. qty of symlinks
        value.SetQtyHardlinks( STATMEMBER(nlink) );

        // 7. Add extended information
        if(    oldQuality < FInfo::Qualities::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) ) {
            IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                File(node).GetFTree().AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );
            IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
        }

    } // if scan stats (quality was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.Quality() == FInfo::Qualities::BROKEN_LINK)
    {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_POST_RECURSION_FILTER;
    }

    // =============================================================================================
    // =============================    recursion with directories?     ============================
    // =============================================================================================
    if(   !value.IsDirectory()
        || value.Quality() >= FInfo::Qualities::RECURSIVE )
        goto APPLY_POST_RECURSION_FILTER;

    // stop recursion due to artificial fs?
    if( value.IsArtificialFS() && !params.IncludeArtificialFS )
    {
        Log_Prune( if( verboseLogables.Size() )  verboseLogables.Add(" NO_AFS"); )
        value.SetQuality( FInfo::Qualities::NO_AFS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems )
    {
        Log_Prune( if( verboseLogables.Size() )  verboseLogables.Add(" NOT_CROSSING_FS"); )
        value.SetQuality( FInfo::Qualities::NOT_CROSSING_FS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth )
    {
        Log_Prune( if( verboseLogables.Size() && (&params != &paramsPathOnly) )  verboseLogables.Add(" MAX_DEPTH_REACHED"); )
        value.SetQuality( FInfo::Qualities::MAX_DEPTH_REACHED );
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
    value.SetQuality( FInfo::Qualities::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR )
    {
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )   // never recurse with symlinks RESIDING on artificial fs!
        {
            value.SetQuality( FInfo::Qualities::NOT_FOLLOWED );
            goto APPLY_POST_RECURSION_FILTER;
        }

        if( value.TargetIsArtificialFS() &&  !params.IncludeArtificialFS )
        {
            value.SetQuality( FInfo::Qualities::NO_AFS );
            goto APPLY_POST_RECURSION_FILTER;
        }

        // recurse into symlink target
        FInfo::DirectorySums childSums;
        if( startScan( node.Tree<FTree>(), value.GetRealLinkTarget(), params, childSums,
                       resultPaths   IF_ALIB_THREADS(,lock) )  )
            value.SetQuality(FInfo::Qualities::DUPLICATE);
        value.SetSums( childSums );
        parentSums+= childSums;

        goto APPLY_POST_RECURSION_FILTER;
    }

    // DIRECTORY RECURSION
    {ALIB_STRING_RESETTER( actPath );
        if( pxDir == nullptr )
        {
            ALIB_ASSERT_ERROR( actPath.Buffer() == nameOrFullPath.Buffer(), "FILES", "Internal error" )
            actPath.SetLength(nameOrFullPath.Length());
        }
        else
        {
            if( actPath.Length() > 1 ) actPath << DIRECTORY_SEPARATOR;
            actPath << nameOrFullPath;
        }

        errno= 0;
        int fd;
        if( pxDir)
            fd=   openat( dirfd(pxDir), nNameOrFullPath, O_RDONLY | O_DIRECTORY );
        else
        {
            ALIB_STRINGS_TO_NARROW(actPath, nActPath, 512)
            fd=     open( nActPath , O_RDONLY | O_DIRECTORY );
        }

        if (fd != -1) // success?
        {
            DBG_CHECKERRNO_WITH_PATH
            FInfo::DirectorySums subSums;
            DIR* childDir = fdopendir(fd);
            for(;;)
            {
                errno= 0;
                dirent*  pxEntry       = readdir(childDir);
                if( pxEntry == nullptr )
                {
                    switch(errno)
                    {
                        // possible errors (according to documentation):
                        // EOVERFLOW One of the values in the structure to be returned cannot  be represented correctly.
                        // EBADF     The dirp argument does not refer to an open directory stream.
                        // ENOENT    The current position of the directory stream is invalid.
                        case 0:      break;
                        case EACCES: value.SetQuality(FInfo::Qualities::NO_ACCESS_DIR);
                                     break;
                        case EINVAL: value.SetQuality( FInfo::Qualities::NO_ACCESS_DIR);
                            ALIB_ASSERT_ERROR(major(currentDevice) == 0, "FILES",
                                              "Posix raised ({}) {!Q} on reading a directory which is not located on an "
                                              "artificial filesystem (like /proc). File:{!Q}",
                                              errno, SystemErrors(errno), dbgActFile )
                                    break;
                        default:    value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                                    ALIB_ERROR("FILES", "Posix raised ({}) {!Q} on reading directory {!Q}",
                                       errno, SystemErrors(errno), dbgActFile )
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
                auto childNode= node;
#if ALIB_CHARACTERS_WIDE
                Path childName(const_cast<const char*>(&pxEntry->d_name[0]));
#else
                const CString childName(const_cast<const char*>(&pxEntry->d_name[0]));
#endif
                IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                  childNode.GoToCreateChildIfNotExistent( childName );
                IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
                scanFilePosix( childDir, childNode, childName,
                               depth + 1, params, currentDevice, subSums, actPath,
                               resultPaths   IF_ALIB_THREADS(,lock) );
            } // dir entry loop
            closedir(childDir);
            DBG_CHECKERRNO_WITH_PATH

            // previously scanned in lower quality?
            if( oldQuality != FInfo::Qualities::NONE )
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
        } // success opening director

        // error with recursion
        ALIB_ASSERT_ERROR(errno != ENOTDIR, "FILES",
                          "Internal error opening directory. This must never happen")


        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        switch (SystemErrors(errno))
        {
            case SystemErrors::eacces:
                ++parentSums.QtyErrsAccess;
                value.SetQuality( FInfo::Qualities::NO_ACCESS_DIR );
                errno= 0;
                break;

            default:
            ALIB_ERROR("FILES", "Unknown error {}({!Q}) while opening directory {!Q}",
                       errno, SystemErrors(errno), actPath)
                value.SetQuality( FInfo::Qualities::UNKNOWN_ERROR );
                break;
        }
    }

    // =============================================================================================
    // ====================    Apply Post Filter and remove empty directories    ===================
    // =============================================================================================
    APPLY_POST_RECURSION_FILTER:
    // delete node only if this was a new scan.
    // It must not be deleted if this node was created as a path.
    if( oldQuality == FInfo::Qualities::NONE )
    {
        if ( value.IsDirectory() )
        {
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
                if( params.RemoveEmptyDirectories )
                {
                    File file(node);
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode,
                                               file,
                               IF_ALIB_THREADS(lock,)
                                               actPath );
                    node.Delete();
                    return;
                }

                // Notify deletion of all children.
                auto it= node.FirstChild();
                while ( it.IsValid() )
                {
                    File file(node);
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode,
                                               file,
                               IF_ALIB_THREADS(lock,)
                                               actPath );
                    it.GoToNextSibling();
                }

                // do not return here. Still count the type below
                node.DeleteChildren();
            }

        }
        else
        {
            if (      params.FileFilter
                  && !params.FileFilter->Includes(node, actPath )   )
            {
                Log_Prune( if( verboseLogables.Size() ) { verboseLogables.Add(" FILTERED(Post)");
                                                          Log_Verbose( verboseLogables )  } )
                IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                  node.Delete();
                IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
                return;
            }
        }
    }

    Log_Prune( if( verboseLogables.Size() ) { Log_Verbose( verboseLogables ) } )

    // cnt file type
    parentSums.Add(value);
    File file(node);
    node.Tree<FTree>().Notify( FTreeListener::Event::CreateNode, file, IF_ALIB_THREADS(lock,) actPath );

    ALIB_WARNINGS_RESTORE
    DBG_CHECKERRNO_WITH_PATH
} // scanFilePosix()

}} // namespace [alib::files::anonymous]
#undef DBG_CHECKERRNO_WITH_PATH
#undef TMP_STATX_AVAILABLE
#undef STATMEMBER


//--------------------------------------------------------------------------------------------------
//--- UNKNOWN platform, using C++17 filesystem (not all functionality given)
//--------------------------------------------------------------------------------------------------
#else

#if ALIB_FILES_FORCE_STD_SCANNER
#   pragma message ("ALIB_FILES_FORCE_STD_SCANNER given. Using std::filesystem for scanning. In file: " __FILE__ )
#else
#   pragma message ("Unknown Platform. Using std::filesystem for scanning. In file: " __FILE__ )
#endif

#include "alib/compatibility/std_strings.hpp"
#include <filesystem>
namespace fs = std::filesystem;

// Note: MacOS is currently (as of 231210) missing C++ 20 library features in the area of std::clock
#if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
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
#   define DBG_CHECKERRNO                                                                          \
        ALIB_ASSERT_WARNING(errno == 0, "FILES", "Errno set ({}){!Q}.",                     \
                            errno, SystemErrors(errno) )                                           \
        errno= 0;
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
        ALIB_ASSERT_WARNING(errno == 0, "FILES", "Errno set ({}){!Q}. Current path: {}",    \
                            errno, SystemErrors(errno), path.string() )                            \
        errno= 0;
#else
#   define DBG_CHECKERRNO
#   define DBG_CHECKERRNO_WITH_PATH
#endif

namespace alib::files {  namespace {

void scanFileStdFS( const fs::path&               path,
                    FTree::Cursor&                node,
                    unsigned int                  depth,
                    ScanParameters&               params,
                    FInfo::DirectorySums&         parentSums,
                    std::vector<ResultsPaths>&    resultPaths
 IF_ALIB_THREADS(, SharedLock*                lock)                           )
{
#if defined(__MINGW32__)
    Path       pathAsCString(path.c_str());
    pathAsCString.Terminate();
#else
    CPathString           pathAsCString(path.c_str());
#endif
    const PathSubstring   parentPath= pathAsCString.Substring(0, pathAsCString.LastIndexOf(DIRECTORY_SEPARATOR));

    #if !defined(_WIN32)
    ALIB_ASSERT_ERROR(          pathAsCString.CharAtStart()== DIRECTORY_SEPARATOR
                        &&  (   pathAsCString.Length()==1
                             || pathAsCString.CharAtEnd()  != DIRECTORY_SEPARATOR)
                        &&      pathAsCString.IndexOf(NString8(DIRECTORY_SEPARATOR).Append(DIRECTORY_SEPARATOR))<0,
                    "FILES","Given path not absolute or ending with '{}': {}", DIRECTORY_SEPARATOR, pathAsCString )
    #else
    ALIB_ASSERT_ERROR(      (    (    pathAsCString.CharAt(1)== ':'
                                   && pathAsCString.CharAt(2)== DIRECTORY_SEPARATOR
                                   &&  (   pathAsCString.Length()==3
                                        || pathAsCString.CharAtEnd()  != DIRECTORY_SEPARATOR) )

                              || (    pathAsCString.CharAt(0)== DIRECTORY_SEPARATOR
                                   && pathAsCString.CharAt(1)== DIRECTORY_SEPARATOR
                                   &&  (   pathAsCString.Length()==2
                                        || pathAsCString.CharAtEnd()  != DIRECTORY_SEPARATOR) )
                            )
                        &&  pathAsCString.IndexOf( strings::TLocalString<PathCharType, 8>(
                                                    DIRECTORY_SEPARATOR).Append(DIRECTORY_SEPARATOR),
                                                  2 ) < 0,
                    "FILES","Given path not absolute or ending with '{}': {}", DIRECTORY_SEPARATOR, pathAsCString )
    #endif


    Log_Verbose(  "[{}] {}/{}   {}",  &params != &paramsPathOnly ? '>':'P',  depth,
                  params.MaxDepth < (std::numeric_limits<unsigned int>::max)()
                            ? String128(params.MaxDepth)
                            : String(A_CHAR("M")),
                  pathAsCString )

    std::error_code errorCode;
    auto&           value     = node.Value();
    auto            oldQuality= value.Quality();

    // ------------------------------   get stats?  ------------------------------
    if(         value.Quality() == FInfo::Qualities::NONE
        || (    value.Quality() == FInfo::Qualities::STATS
             && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE ) )
    {
        value.SetQuality( FInfo::Qualities::STATS );
        Path          symLinkDest;
        Path          symLinkDestReal;

        // read base stats (we have to use symlink_status() which does NOT follow the symlink!)
        fs::file_status stats= fs::symlink_status(path);
        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode) 
        {
            ALIB_ERROR("FILES",
                       "Unhandled error code invoking 'fs::symlink_status()': {} ({!Q})\n"
                       "    With file: {!Q}",
                       errorCode.value(), errorCode.message(), pathAsCString )
            ALIB_DBG( errno= 0;)
            value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
            goto APPLY_POST_RECURSION_FILTER;
        }
        ALIB_WARNINGS_RESTORE
        ALIB_DBG(errno= 0;)

        //------------ is symlink? ------------
        bool origFileIsSymlink= (stats.type() == fs::file_type::symlink);
        if(    origFileIsSymlink
            && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE )
        {
            value.SetQuality( FInfo::Qualities::RESOLVED );

            // 1. Read plain symlink target (only to be attached to the entry)
            fs::path resolved= fs::read_symlink(path, errorCode);
            if(errorCode)
            {
                ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
                switch( SystemErrors(errorCode.value()) )
                {   case SystemErrors::enoent: // happens with /proc files
                    case SystemErrors::eacces: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL);
                                               ALIB_DBG(errno= 0;)
                                               goto ABORT_SYMLINK;
                    default:  ALIB_ERROR("FILES", "Unhandled error code invoking 'fs::read_symlink()': {} ({!Q})\n"
                                         "   with file: ", errorCode.value(), errorCode.message(), pathAsCString )
                              ALIB_DBG( errno= 0;)
                              value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                              goto APPLY_POST_RECURSION_FILTER;
                }
                ALIB_WARNINGS_RESTORE
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

            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( SystemErrors(errorCode.value()) )
            {   // we ignore this: std::fs would not create the "real path" if the final directory is not accessible.
                case SystemErrors::eacces: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL_TARGET); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case SystemErrors::enoent: value.SetQuality(FInfo::Qualities::BROKEN_LINK);         ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case SystemErrors::eloop:  value.SetQuality(FInfo::Qualities::CIRCULAR_LINK);       ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                default:  ALIB_ERROR("FILES", "Unhandled error code invoking 'fs::canonical()': {} ({!Q})\n"
                                     "   with file: ", errorCode.value(), errorCode.message(), pathAsCString )
                    goto ABORT_SYMLINK;
            }
            ALIB_WARNINGS_RESTORE
            DBG_CHECKERRNO_WITH_PATH
            symLinkDestReal << realPath.c_str();

            // 3. get resolved status
            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            auto newStatus= fs::status(path, errorCode);
            if(!errorCode)
            {
                // this happens with strange /proc files...
                if(newStatus.type() != fs::file_type::unknown)
                    stats= newStatus;
            }
            else switch( SystemErrors(errorCode.value()) )
            {   case SystemErrors::eperm:   value.SetQuality( FInfo::Qualities::NO_ACCESS    ); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case SystemErrors::enoent:  value.SetQuality( FInfo::Qualities::BROKEN_LINK  ); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                case SystemErrors::eloop:   value.SetQuality( FInfo::Qualities::CIRCULAR_LINK); ALIB_DBG(errno= 0;) goto ABORT_SYMLINK;
                default: ALIB_WARNING("FILES",
                             "Unhandled error code invoking 'directory_entry::status()': {} ({!Q})\n"
                             "    With file: {!Q}", errorCode.value(), errorCode.message(), pathAsCString )
                         goto ABORT_SYMLINK;
            }
            ALIB_WARNINGS_RESTORE

            // check for target artificial fs
            //  -/- Not available with std::filesystem version
        }

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

        // =========================================================================================
        // ==================================      Copy Stats     ==================================
        // =========================================================================================
        // 1. type
        {
            auto type= FInfo::Types::UNKNOWN_OR_ERROR;
            if( origFileIsSymlink  )
            {
                type= is_directory(stats) ? FInfo::Types::SYMBOLIC_LINK_DIR
                                          : FInfo::Types::SYMBOLIC_LINK;
            }
            else switch( stats.type() )
            {
                case fs::file_type::directory:  type= FInfo::Types::DIRECTORY    ;   break;
                case fs::file_type::regular  :  type= FInfo::Types::REGULAR      ;   break;
                case fs::file_type::symlink  :  type= FInfo::Types::SYMBOLIC_LINK;   break;  // for now, this is a file.
                case fs::file_type::block    :  type= FInfo::Types::BLOCK        ;   break;
                case fs::file_type::character:  type= FInfo::Types::CHARACTER    ;   break;
                case fs::file_type::fifo     :  type= FInfo::Types::FIFO         ;   break;
                case fs::file_type::socket   :  type= FInfo::Types::SOCKET       ;   break;

                case fs::file_type::not_found:
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("FILES", "Internal error. 'not found' file type can't happen. File: ", pathAsCString )
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                case fs::file_type::none     :
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("FILES", "Internal error. 'none' file type can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                case fs::file_type::unknown  :
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("FILES", "Internal error. Can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
                default:
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("FILES", "Unknown fs::file_status::type '{}' with file {}.", stats.type(), pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_POST_RECURSION_FILTER;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FInfo::Permissions(int32_t(stats.permissions())) );

        // 3. timestamps
        // attn: This method always follows symbolic link and uses the target's time
        // This seems to be a confirmed behavior:
        // https://stackoverflow.com/questions/50778660/boost-filesystem-how-to-get-last-write-time-for-symlink-without-resolving
        auto fsTime= std::filesystem::file_time_type(std::filesystem::file_time_type::clock::now());
        if ( value.Quality() <= FInfo::Qualities::RESOLVED ) // no error
        {
            fsTime= fs::last_write_time( path, errorCode );
            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( SystemErrors(errorCode.value()) )
            {   // This happens if with symbolic links that point to nowhere.
                case SystemErrors::enoent: ALIB_ERROR( "FILES",
                                              "Internal error. This should never happen, checked above. "
                                              "Undefined system error handling" ) ALIB_DBG( errno= 0;)
                                           value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                                           break;

                default:     ALIB_ERROR( "FILES", "Unhandled error code invoking 'fs::last_write_time()': {} ({!Q})\n"
                                         "    With file {!Q}.", errorCode.value(), errorCode.message(), pathAsCString )
                             fsTime= (decltype(fsTime)::min)();                      ALIB_DBG( errno= 0;)
                             break;
            }
            ALIB_WARNINGS_RESTORE
        }


        #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
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
        value.SetSize(   symLinkDest.Length() > 0                      ?  uinteger(symLinkDest.Length())
                       : value.Quality() <= FInfo::Qualities::RESOLVED ?  uinteger(fs::file_size(path, errorCode))
                       : 0 );
        if( value.Size() == uinteger(-1))
        {
            value.SetSize(0);
            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            switch( SystemErrors(errorCode.value()) )
            {
                // target is a directory (no error)
                case SystemErrors::eisdir:
                    break;

                case SystemErrors::enoent: // this happens if we have a broken symbolic link
                    ALIB_ASSERT_ERROR(    value.Type() == FInfo::Types::SYMBOLIC_LINK
                                       || value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR , "FILES",
                        "Internal error. This should never happen. Undefined system error handling" )
                    break;

                // size not supported. Happens with sockets, files in /proc, etc
                case SystemErrors::eopnotsupp: break;
                   default: ALIB_ERROR("FILES", "Unhandled error code invoking 'directory_entry::file_size()':{} ({!Q})\n"
                                       "    With file {!Q}.", errorCode.value(), errorCode.message(), pathAsCString )
                            ALIB_DBG( errno= 0;)
                    break;
            }
            ALIB_WARNINGS_RESTORE
        }

        // 5. uid/gid
        value.SetOwner( FInfo::UnknownID );
        value.SetGroup( FInfo::UnknownID );

        // 6. qty of symlinks
        uint32_t qtyHardLinks= uint32_t( fs::hard_link_count(path, errorCode ) );
        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode)
        {
            ALIB_MESSAGE("FILES",
                       "Unhandled error code invoking 'fs::hard_link_count()': {} ({!Q})\n"
                       "    With file: {!Q}",
                       errorCode.value(), errorCode.message(), pathAsCString )
            ALIB_DBG( errno= 0;)
        }
        ALIB_WARNINGS_RESTORE
        value.SetQtyHardlinks( qtyHardLinks );

        // 7. Add extended information
        if(    oldQuality < FInfo::Qualities::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) ) {
            IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                File(node).GetFTree().AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );
            IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
        }

    } // if scan stats (quality was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.Quality() == FInfo::Qualities::BROKEN_LINK)
    {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_POST_RECURSION_FILTER;
    }

    // ------------------------------ recursion with directories?  ------------------------------
    if(    !value.IsDirectory()
        || value.Quality() >= FInfo::Qualities::RECURSIVE )
        goto APPLY_POST_RECURSION_FILTER;


    // stop recursion due to artificial fs?
        // Not supported with std::filesystem!

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems )
    {
        value.SetQuality( FInfo::Qualities::NOT_CROSSING_FS );
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth )
    {
        value.SetQuality( FInfo::Qualities::MAX_DEPTH_REACHED );
        ++parentSums.QtyStopsOnMaxDepth;
        goto APPLY_POST_RECURSION_FILTER;
    }

    // stop recursion due to filter
    if(      depth > 0
         &&  params.DirectoryFilterPreRecursion
         && !params.DirectoryFilterPreRecursion->Includes( node, parentPath )  )
        goto APPLY_POST_RECURSION_FILTER;

    // mark as recursively scanned
    value.SetQuality( FInfo::Qualities::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR )
    {
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )   // never recurse with symlinks RESIDING on artificial fs!
        {
            value.SetQuality( FInfo::Qualities::NOT_FOLLOWED );
            goto APPLY_POST_RECURSION_FILTER;
        }
        else
        {
            // recurse into symlink target
            FInfo::DirectorySums childSums;
            if( startScan( File(node).GetFTree(), value.GetRealLinkTarget(), params, childSums,
                           resultPaths IF_ALIB_THREADS(,lock) ) )
                value.SetQuality(FInfo::Qualities::DUPLICATE);
            value.SetSums( childSums );
            parentSums+= childSums;
            goto APPLY_POST_RECURSION_FILTER;
        }
    }

    // DIRECTORY RECURSION
    {
        fs::directory_iterator dit= fs::directory_iterator(path, errorCode);
        if(!errorCode) // success?
        {
            FInfo::DirectorySums subSums;
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
        IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                childNode.GoToCreateChildIfNotExistent( childName );
        IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
                scanFileStdFS( childDir.path(), childNode, depth + 1, params, subSums,
                               resultPaths IF_ALIB_THREADS(,lock) );
            }

            // previously scanned in lower quality?
            if( oldQuality != FInfo::Qualities::NONE )
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
    ALIB_ASSERT_ERROR(errorCode.value() != ENOTDIR, "FILES",
                      "Internal error opening directory. This must never happen")

    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    if(errorCode) switch (SystemErrors(errorCode.value()))
    {
        case SystemErrors::einval: // happens with /proc
        case SystemErrors::eacces: ++parentSums.QtyErrsAccess;
                                   value.SetQuality( FInfo::Qualities::NO_ACCESS_DIR );
                                   ALIB_DBG( errno= 0;)
                                   goto APPLY_POST_RECURSION_FILTER;

        default: value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                 ALIB_ERROR("FILES", "Unknown error {}({!Q}) while opening directory {!Q}",
                             errorCode.value(), SystemErrors(errorCode.value()), pathAsCString)
                 ALIB_DBG( errno= 0;)
                 goto APPLY_POST_RECURSION_FILTER;
    }
    ALIB_WARNINGS_RESTORE
    ALIB_DBG( errno= 0;)

    // ------------------------------------ Apply Filter  ------------------------------------------
    APPLY_POST_RECURSION_FILTER:
    // delete node only if this was a new scan. It must not be deleted if this node was
    // created as a path.
    if( oldQuality == FInfo::Qualities::NONE )
    {
        if ( value.IsDirectory() )
        {
            if(    depth > 0
                && (    (     params.DirectoryFilterPostRecursion
                          && !params.DirectoryFilterPostRecursion->Includes(node, parentPath )  )
                     || (     params.RemoveEmptyDirectories
                          &&  value.Sums().Count() == 0                                      )
                   )                                                                             )
            {
                parentSums-= value.Sums();
                if( params.RemoveEmptyDirectories )
                {
                    File file(node);
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode,
                                               file,
                             IF_ALIB_THREADS(  lock, )
                                               parentPath );
                    node.Delete();
                    return;
                }

                // Notify deletion of all children.
                auto it= node.FirstChild();
                while ( it.IsValid() )
                {
                    File file(node);
                    node.Tree<FTree>().Notify( FTreeListener::Event::DeleteNode,
                                               file,
                             IF_ALIB_THREADS(  lock, )
                                               parentPath );
                    it.GoToNextSibling();
                }

                // do not return here. Still count the type below
                node.DeleteChildren();
            }

        }
        else
        {
            if (      params.FileFilter
                  && !params.FileFilter->Includes(node, parentPath )   )
            {
                IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
                  node.Delete();
                IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
                return;
            }
        }
    }

    // cnt file type
    parentSums.Add(value);
    File file(node);
    node.Tree<FTree>().Notify( FTreeListener::Event::CreateNode, file, IF_ALIB_THREADS(lock,) parentPath );

} // scanFileStdFS


}} // namespace [alib::files::anonymous]

#undef DBG_CHECKERRNO_WITH_PATH
#endif  // std::fs version

//--------------------------------------------------------------------------------------------------
//--- ALL Platforms
//--------------------------------------------------------------------------------------------------
namespace alib::files {

namespace {

// Creates start path nodes and invokes scanFileXXX
bool    startScan( FTree&                      tree,
                   PathString                  realPath,
                   ScanParameters&             params,
                   FInfo::DirectorySums&       parentSums,
                   std::vector<ResultsPaths>&  resultPaths
 IF_ALIB_THREADS(, SharedLock*                 lock)        )
{
    ALIB_ASSERT_ERROR( Path::IsAbsolute(realPath),  "FILES",
                        "Real path is not absolute: ", realPath )

    FTree::Cursor   node= tree.Root().AsCursor();
#if !defined(_WIN32)
    Path path(DIRECTORY_SEPARATOR);

    // travel any existing portion of the path
    IF_ALIB_THREADS( if (lock) lock->AcquireShared(ALIB_CALLER_PRUNED); )
    PathSubstring  pathRemainder= node.GoTo( realPath );
    IF_ALIB_THREADS( if (lock) lock->ReleaseShared(ALIB_CALLER_PRUNED); )
    path << realPath.Substring(1, realPath.Length() - pathRemainder.Length() - 1);
#else
    Path            path;
    PathSubstring       pathRemainder;
    if(realPath.CharAt(1) == ':')
    {
        path << realPath.Substring(0,3);
        IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
          node.GoToCreateChildIfNotExistent(realPath.Substring(0,2));
          pathRemainder= node.GoTo( realPath.Substring(3) );
        IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
        path << realPath.Substring(3, realPath.Length() - pathRemainder.Length() -3 );
    }
    else
    {
        integer serverNameEnd= realPath.IndexOf( DIRECTORY_SEPARATOR, 2);
        if( serverNameEnd < 0)
            serverNameEnd= realPath.Length();
        path << realPath.Substring(0, serverNameEnd);
        IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
          node.GoToCreateChildIfNotExistent(realPath.Substring(2, serverNameEnd - 2));
          pathRemainder= node.GoTo( realPath.Substring(serverNameEnd + 1) );
        IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
        path << realPath.Substring(serverNameEnd, realPath.Length() - pathRemainder.Length() -serverNameEnd );
    }

#endif

    DBG_CHECKERRNO


    // existed already?
    if( pathRemainder.IsEmpty() )
    {
        // For directories, call scan just for the case of having 'higher' scan parameters
        if( node->IsDirectory())
        {

            #if   (    (defined(__GLIBCXX__) && !defined(__MINGW32__))                  \
                     || defined(__APPLE__)                                              \
                     || defined(__ANDROID_NDK__) )        && !ALIB_FILES_FORCE_STD_SCANNER

                path.Terminate();
                CPathString    fullPathChildName(path);
                path.SetLength(path.LastIndexOf(DIRECTORY_SEPARATOR) );
                scanFilePosix( nullptr, node, fullPathChildName, 0, params, 0, parentSums, path,
                               resultPaths IF_ALIB_THREADS(,lock));
            #else
                scanFileStdFS( fs::path(std::basic_string_view<PathCharType>(path.Buffer(),
                                                                      size_t(path.Length()))),
                               node, 0, params, parentSums, resultPaths IF_ALIB_THREADS(,lock) );
            #endif

            //resultPaths.emplace_back(ResultsPaths(realPath, node, true));
        }

        return true;
    }

    // did not exist already
    if( path.Length() > 1 )
        path.DeleteEnd<NC>(1);

    strings::util::TTokenizer<PathCharType>  tknzr( pathRemainder, DIRECTORY_SEPARATOR );
    while(tknzr.HasNext())
    {
        PathString name;
        if( path.Length() != 1 )
        {
            name= tknzr.Next();
IF_ALIB_THREADS( if (lock) lock->Acquire(ALIB_CALLER_PRUNED); )
            node= node.CreateChild(name);
IF_ALIB_THREADS( if (lock) lock->Release(ALIB_CALLER_PRUNED); )
        }

        bool isLastPathElement= !tknzr.HasNext();
        if( isLastPathElement )
            parentSums= FInfo::DirectorySums(); // clear the sums, because only the results of the last element are used.

        IF_ALIB_THREADS( if (lock) lock->AcquireShared(ALIB_CALLER_PRUNED); )
          auto detectNodeDeletion= node.Depth();
        IF_ALIB_THREADS( if (lock) lock->ReleaseShared(ALIB_CALLER_PRUNED); )

        #if   (    (defined(__GLIBCXX__) && !defined(__MINGW32__))                      \
                 || defined(__APPLE__)                                                  \
                 || defined(__ANDROID_NDK__) )        && !ALIB_FILES_FORCE_STD_SCANNER

            if( path.IsEmpty() ) path << DIRECTORY_SEPARATOR;
            CPathString    fullPathChildName;
            {
                // add node name to existing path and use same buffer for fullPathChildName!
                ALIB_STRING_RESETTER( path );
                if( path.Length() > 1 ) path << DIRECTORY_SEPARATOR;
                path << node.Name();
                path.Terminate();
                fullPathChildName= path;
            }

            scanFilePosix( nullptr, node, fullPathChildName,
                           0, isLastPathElement ? params : paramsPathOnly,
                           0, parentSums, path, resultPaths IF_ALIB_THREADS(,lock) );
            if( fullPathChildName.Length() == 1 )  path.Reset();
            else { if(path.Length() > 1)  path << DIRECTORY_SEPARATOR; path << name; }
        #else
            if( path.Length() != 1 )  path << DIRECTORY_SEPARATOR << name;
            scanFileStdFS( fs::path(std::basic_string_view<PathCharType>(path.Buffer(),
                                                                  size_t(path.Length()))),
                           node, 0,
                           isLastPathElement ? params : paramsPathOnly,
                           parentSums, resultPaths IF_ALIB_THREADS(,lock) );
            if( path.Length() == 1 )  path.Reset();
        #endif

        // if the just created node was not deleted during scan, add it to the result list
        if( isLastPathElement)
        {
            IF_ALIB_THREADS( if (lock) lock->AcquireShared(ALIB_CALLER_PRUNED); )
              if (detectNodeDeletion == node.Depth() )
                  resultPaths.insert(resultPaths.begin(), ResultsPaths(realPath, node, false));
            IF_ALIB_THREADS( if (lock) lock->ReleaseShared(ALIB_CALLER_PRUNED); )
        }

        // Correct quality from max depth to stats
        if( !isLastPathElement  && node->Quality() == FInfo::Qualities::MAX_DEPTH_REACHED)
            node->SetQuality(FInfo::Qualities::STATS);
    }

    return false;
}

} // namespace  alib::files[::anonymous]

#endif // !DOXYGEN

// --------------------------------------------------------------------------------------------------
//--- ScanFiles()
//--------------------------------------------------------------------------------------------------
enum FInfo::Qualities  ScanFiles( FTree&                      tree,
                                  ScanParameters&             parameters,
                                  std::vector<ResultsPaths>&  resultPaths
               IF_ALIB_THREADS( , SharedLock*                 lock)         )

{
    Log_SetDomain( "ALIB/FILES", Scope::Path)
    Log_SetDomain( "SCAN"      , Scope::Filename)

    ALIB_DBG(   if( alib::FILES.IsBootstrapped())
                {
                    Log_SetDomain( "ALIB/FILES", Scope::Path)
                    Log_SetDomain( "SCAN"      , Scope::Filename)
                }                                                   )


    //--------------------------------------  get real path  ---------------------------------------
    Path  path(parameters.StartPath);
    Path  realPath;
    realPath.Terminate();

    #if   (     (defined(__GLIBCXX__) && !defined(__MINGW32__))             \
              || defined(__APPLE__)                                         \
              || defined(__ANDROID_NDK__) )        && !ALIB_FILES_FORCE_STD_SCANNER
        ALIB_STRINGS_TO_NARROW(path    , nPath    , 512)
        ALIB_STRINGS_TO_NARROW(realPath, nRealPath, 512)
        if(!realpath(nPath.Terminate(), nRealPath.VBuffer() ) ) switch (errno)
        {
            case EACCES: ALIB_DBG(errno= 0;) return FInfo::Qualities::NO_ACCESS;
            case ENOENT: ALIB_DBG(errno= 0;) return FInfo::Qualities::NOT_EXISTENT;
            case ELOOP:  ALIB_DBG(errno= 0;) return FInfo::Qualities::CIRCULAR_LINK;
            default: ALIB_ERROR("FILES", "Posix raised ({}) {!Q} on resolving start path {!Q}",
                                errno, SystemErrors(errno), path )   ALIB_DBG(errno= 0;)
                     return FInfo::Qualities::UNKNOWN_ERROR;
        }
        nRealPath.DetectLength();
        #if ALIB_CHARACTERS_WIDE
            realPath.Reset(nRealPath);
        #endif
#else
        {
            std::error_code errorCode;
            fs::path fsRealPath= fs::canonical(fs::path(std::basic_string_view<PathCharType>(path.Buffer(),
                                                                                      size_t(path.Length()))),
                                               errorCode);
            ALIB_DBG(if(errno==EINVAL && !errorCode) errno= 0;) // this happens!, we do not care, but clean up
            ALIB_DBG(if(errno==ENOENT && !errorCode) errno= 0;)

            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( SystemErrors(errorCode.value()) )
            {   // we ignore this: std::fs would not create the "real path" if the final directory is not accessible.
                case SystemErrors::eacces: return FInfo::Qualities::NO_ACCESS;
                case SystemErrors::enoent: return FInfo::Qualities::NOT_EXISTENT;
                case SystemErrors::eloop:  return FInfo::Qualities::CIRCULAR_LINK;
                default: ALIB_ERROR("FILES", "std::filesystem raised ({}) {!Q} on resolving start path {!Q}",
                                    errorCode.value(), errorCode.message(), path )   ALIB_DBG(errno= 0;)
                         return FInfo::Qualities::UNKNOWN_ERROR;
            }
            ALIB_WARNINGS_RESTORE
            realPath << fsRealPath.c_str();
        }

    #endif

    Log_Info( "Scanning: P=  {}\n"
              "          RP= {}\n"
              "          F={} DPre={} DPost={} XFS={} AFS={} Depth={}",
              parameters.StartPath, realPath,
              parameters.FileFilter                  .get() ? 'Y':'N',
              parameters.DirectoryFilterPreRecursion .get() ? 'Y':'N',
              parameters.DirectoryFilterPostRecursion.get() ? 'Y':'N',
              parameters.CrossFileSystems ? 'Y':'N', parameters.IncludeArtificialFS ? 'Y':'N',
              parameters.MaxDepth == ScanParameters::InfiniteRecursion ? String(A_CHAR("Inf"))
                                                                       : String128(parameters.MaxDepth)
            )

    //--------------------------------------  start scanning  --------------------------------------
    ALIB_DBG( errno=0; )
    auto firstResultPos= resultPaths.size();
    FInfo::DirectorySums dummySums;

    startScan( tree, realPath, parameters, dummySums, resultPaths IF_ALIB_THREADS( , lock)  );

    Log_Info( "Scan Results: ", resultPaths.size() - firstResultPos )
    Log_Prune( int cntPaths= 0;
               for( auto& it : resultPaths )
               {
                   Log_Info( "    Path {}: {} {}  (Q={} D={}/F={}}",
                             cntPaths++, it.Existed ? ' ' : '+',
                             it.RealPath,
                             it.Node->Quality(),
                             it.Node->Quality() > FInfo::Qualities::STATS && it.Node->IsDirectory() ? it.Node.Value().Sums().CountDirectories()   : 0,
                             it.Node->Quality() > FInfo::Qualities::STATS && it.Node->IsDirectory() ? it.Node.Value().Sums().CountNonDirectories(): 0    )
               } )



    return (*(resultPaths.begin() + int(firstResultPos))).Node->Quality();
}

#undef DBG_CHECKERRNO

} // namespace [alib::files]

#include "alib/lang/callerinfo_methods.hpp"

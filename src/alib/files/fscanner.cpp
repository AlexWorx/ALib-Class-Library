// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#include "alib/files/fscanner.hpp"
#include "alib/strings/util/tokenizer.hpp"

#if !defined(ALIB_DOX)

//#define ALIB_TESTSTDFS

namespace alib::files {  namespace {

    // forward declaration of startScan()
    bool startScan( FTree&                      tree,
                    String                      realPath,
                    ScanParameters&             params,
                    FInfo::DirectorySums&       parentSums,
                    std::vector<ResultsPaths>&  resultPaths);

    // scan parameters used with startScan to evaluate directory entries
    ScanParameters paramsPathOnly( nullptr, ScanParameters::SymbolicLinks::DONT_RESOLVE, 0, true, true );
}}

//--------------------------------------------------------------------------------------------------
//--- scanFilePosix
//--------------------------------------------------------------------------------------------------
#if   (     ( defined(__GLIBCXX__)    && !defined(__MINGW32__) )   \
         || defined(__APPLE__)         \
         || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)
#   include <unistd.h>
#   include <dirent.h>
#   include <sys/stat.h>
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
        ALIB_ASSERT_WARNING(errno == 0, "CAMP/FILES", "Errno set ({}){!Q}.",                       \
                            errno, SystemErrors(errno) )                                           \
        errno= 0;
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
        ALIB_ASSERT_WARNING(errno == 0, "CAMP/FILES", "Errno set ({}){!Q}. Current path: {}",      \
                            errno, SystemErrors(errno), actPath )                                  \
        errno= 0;
#else
#   define DBG_CHECKERRNO
#   define DBG_CHECKERRNO_WITH_PATH
#endif

namespace alib::files {  namespace {
void scanFilePosix( DIR*                        pxDir,
                    FTree::Cursor&              node,
                    const CString&              nameOrFullPath, // if full path, this has the same buffer as actPath!
                    unsigned int                depth,
                    ScanParameters&             params,
                    decltype(stat::st_dev)      currentDevice,
                    FInfo::DirectorySums&       parentSums ,
                    PathString&                 actPath,
                    std::vector<ResultsPaths>&  resultPaths)
{
    ALIB_ASSERT_ERROR(          actPath.CharAtStart()== DirectorySeparator
                        &&  (   actPath.Length()==1
                             || actPath.CharAtEnd()  != DirectorySeparator )
                        &&      actPath.IndexOf(String8(DirectorySeparator).Append(DirectorySeparator)) < 0 ,
                    "CAMP/FILES","Given path not absolute or ending with '{}': {}", DirectorySeparator, actPath )
    ALIB_DBG(  PathString dbgActFile;
               if( actPath.Buffer() == nameOrFullPath.Buffer() )
                   dbgActFile << nameOrFullPath;
               else
               {
                    dbgActFile << actPath;
                    if(dbgActFile.Length()>1)
                        dbgActFile << DirectorySeparator;
                    dbgActFile << nameOrFullPath;
               }                                                   )
    //ALIB_MESSAGE( "CAMP/FILES","[{}] {}/{}   {}",  &params != &paramsPathOnly ? '>':'P',  depth,
    //              params.MaxDepth < std::numeric_limits<unsigned int>::max()
    //                        ? String128(params.MaxDepth)
    //                        : String("M"),
    //              dbgActFile )
    ALIB_ASSERT_WARNING(errno == 0, "CAMP/FILES", "Errno set ({}){!Q} with current file: {}",
                        errno, SystemErrors(errno), dbgActFile )
    ALIB_DBG( errno= 0;)

    auto& value         = node.Value();
    auto oldQuality= value.Quality();

    // ------------------------------   get stats?  ------------------------------
    if(          value.Quality() == FInfo::Qualities::NONE
        || (     value.Quality() == FInfo::Qualities::STATS
             &&  params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE ) )
    {
        value.SetQuality(FInfo::Qualities::STATS);
        PathString    symLinkDest;
        PathString    symLinkDestReal;

        // read base stats
        struct stat   stats;
        int statResult=  pxDir ? fstatat(dirfd(pxDir), nameOrFullPath, &stats,  AT_SYMLINK_NOFOLLOW
                                                                        #if !defined(__APPLE__)
                                                                            | AT_NO_AUTOMOUNT
                                                                        #endif
                                                            )
                               : lstat  (              nameOrFullPath, &stats );

        if( statResult )
        {
            ALIB_ASSERT_WARNING( errno != ENOENT, "CAMP/FILES", "File does not exist (anymore) while stating {!Q}",
                                                                  dbgActFile )
            ALIB_ASSERT_WARNING( errno == ENOENT, "CAMP/FILES", "Unknown error ({}) {!Q} while stating file {!Q}",
                                 errno, SystemErrors(errno), dbgActFile )
            ALIB_DBG( errno= 0;)
            value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
            goto APPLY_FILTER;
        }
        DBG_CHECKERRNO_WITH_PATH

        // check filesystem type (artificial fs & mount point)
        if( currentDevice == 0)
            currentDevice= stats.st_dev;
        else if( currentDevice != stats.st_dev )
        {
            value.SetCrossingFS();
            currentDevice= stats.st_dev;
        }

        if(     major(stats.st_dev) == 0    // artificial?
            &&  minor(stats.st_dev) != 35 ) // tmpfs included, not considered artificial!
            value.SetArtificialFS();


        //------------ is symlink? ------------
        bool origFileIsSymlink= (stats.st_mode & S_IFMT) == S_IFLNK;
        if(    origFileIsSymlink
            && params.LinkTreatment != ScanParameters::SymbolicLinks::DONT_RESOLVE )
        {
            value.SetQuality( FInfo::Qualities::RESOLVED );

            // 1. Read plain symlink target (only to be attached to the entry)
            ssize_t cntChars= pxDir ? readlinkat( dirfd(pxDir), nameOrFullPath, symLinkDest.VBuffer(), PATH_MAX)
                                    : readlink  (               nameOrFullPath, symLinkDest.VBuffer(), PATH_MAX);

            if (cntChars == -1) switch(errno)
                {
                    case EACCES: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL);      ALIB_DBG(errno= 0;)
                        goto ABORT_SYMLINK;

                    case ENOENT: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL);
                        ALIB_ASSERT_ERROR(major(stats.st_dev) == 0, "CAMP/FILES",
                                          "Posix raised ({}) {!Q} on reading a symbolic link which is not located on an "
                                          "artificial filesystem (like /proc). File:{!Q}",
                                          errno, SystemErrors(errno), dbgActFile )               ALIB_DBG(errno= 0;)
                        goto ABORT_SYMLINK;

                    default:     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                        ALIB_ERROR("CAMP/FILES", "Posix raised ({}) {!Q} on reading symbolic link {!Q}",
                                   errno, SystemErrors(errno), dbgActFile )      ALIB_DBG(errno= 0;)
                        goto ABORT_SYMLINK;
                }
            symLinkDest.SetLength(cntChars);


            // 2. Read symlink's real target path (fully and recursively translated)
            ALIB_STRING_RESETTER(actPath);
            if( pxDir )
                actPath << DirectorySeparator << nameOrFullPath;
            errno= 0;
            *symLinkDestReal.VBuffer()= '\0';
            if(! realpath(actPath.Terminate(), symLinkDestReal.VBuffer() ) ) switch (errno)
                {   // The named file does not exist.
                    case ENOENT: if( *symLinkDestReal.VBuffer() != '\0')
                                    symLinkDestReal.DetectLength();
                                 value.SetQuality(FInfo::Qualities::BROKEN_LINK);    ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    case ELOOP:  value.SetQuality(FInfo::Qualities::CIRCULAR_LINK);  ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    // this might happen with strange system files
                    case EACCES: value.SetQuality(FInfo::Qualities::NO_ACCESS_SL_TARGET); ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                    default:     ALIB_ERROR("CAMP/FILES", "Posix raised ({}) {!Q} on resolving symbolic link {!Q}",
                                            errno, SystemErrors(errno), dbgActFile )     ALIB_DBG(errno= 0;)
                                 goto ABORT_SYMLINK;
                }
            symLinkDestReal.DetectLength();

            ALIB_DBG( if( errno == EINVAL) errno= 0;) // this happens, even though realpath() above returned 'OK'
            DBG_CHECKERRNO_WITH_PATH
            ALIB_ASSERT_ERROR( symLinkDestReal.CharAtStart() == DirectorySeparator,
                               "CAMP/FILES", "Real path is not absolute: ", symLinkDestReal )

            // 3. get resolved status
            DBG_CHECKERRNO_WITH_PATH
            statResult= stat(symLinkDestReal.Terminate(), &stats );
            DBG_CHECKERRNO_WITH_PATH

            if(statResult == -1 )
            {
                ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
                if(errno) switch( SystemErrors(errno) )
                {   case SystemErrors::enoent:  value.SetQuality(FInfo::Qualities::BROKEN_LINK);
                                                ALIB_DBG(errno= 0;)
                                                goto APPLY_FILTER;
                    default: ALIB_WARNING("CAMP/FILES",
                                 "Unhandled error code invoking 'stat()' on resolved symbolic link: {} ({!Q})\n"
                                 "    Symbolic link target: {!Q}", errno, SystemErrors(errno), dbgActFile )
                                 ALIB_DBG(errno= 0;)
                             value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                             goto APPLY_FILTER;
                }
                ALIB_WARNINGS_RESTORE
            }

            // check for target artificial fs
            if(     major(stats.st_dev) == 0   // artificial?
                &&  minor(stats.st_dev) != 35 ) // tempfs included!
                value.SetTargetArtificialFS();

        } // if is symlink && resolve symlinks

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

        // 1. type
        {
            auto type= FInfo::Types::UNKNOWN_OR_ERROR;
            auto posixType= stats.st_mode & S_IFMT;
            if( origFileIsSymlink  )
            {
                type= posixType == S_IFDIR ? FInfo::Types::SYMBOLIC_LINK_DIR
                                           : FInfo::Types::SYMBOLIC_LINK;
            }
            else switch(stats.st_mode & S_IFMT )
            {
                case S_IFLNK :  type= FInfo::Types::SYMBOLIC_LINK; ALIB_ERROR( "CAMP/FILES", "Impossible")
                    break;
                case S_IFBLK :  type= FInfo::Types::BLOCK        ; break;
                case S_IFCHR :  type= FInfo::Types::CHARACTER    ; break;
                case S_IFDIR :  type= FInfo::Types::DIRECTORY    ; break;
                case S_IFIFO :  type= FInfo::Types::FIFO         ; break;
                case S_IFREG :  type= FInfo::Types::REGULAR      ; break;
                case S_IFSOCK:  type= FInfo::Types::SOCKET       ; break;
                default:  ALIB_ERROR("CAMP/FILES", "Internal error. 'unknown' file type can't happen. File: {!Q}",
                                     dbgActFile )    break;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FInfo::Permissions(stats.st_mode & int32_t(FInfo::Permissions::MASK)) );

        // 3. timestamps
        {
            #if !defined(__APPLE__)
            #  define st_mtime_name  st_mtim
            #  define st_ctime_name  st_ctim
            #  define st_atime_name  st_atim
            #else
            #  define st_mtime_name  st_mtimespec
            #  define st_ctime_name  st_ctimespec
            #  define st_atime_name  st_atimespec
            #endif
            DateTime dt;
            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {stats.st_mtime_name.tv_sec }
                                  + std::chrono::nanoseconds{stats.st_mtime_name.tv_nsec}          )    } );
            value.SetMTime(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {stats.st_ctime_name.tv_sec }
                                  + std::chrono::nanoseconds{stats.st_ctime_name.tv_nsec}          )    } );
            value.SetCTime(dt);

            dt.Import(
                    std::chrono::system_clock::time_point {
                            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                                    std::chrono::seconds    {stats.st_atime_name.tv_sec }
                                  + std::chrono::nanoseconds{stats.st_atime_name.tv_nsec}          )    } );
            value.SetATime(dt);
          #undef st_mtime_name
          #undef st_ctime_name
          #undef st_atime_name
        }
        // 4. size
        value.SetSize( uinteger(stats.st_size ) );

        // 5. uid/gid
        value.SetOwner( stats.st_uid );
        value.SetGroup( stats.st_gid );

        // 6. Add extended information
        if(    oldQuality < FInfo::Qualities::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) )
            static_cast<FTree&>(node.Tree()).AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );

    } // if scan stats (quality was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.Quality() == FInfo::Qualities::BROKEN_LINK)
    {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_FILTER;
    }

    // ------------------------------ recursion with directories?  ------------------------------
    if(    !value.IsDirectory()
        || value.Quality() >= FInfo::Qualities::RECURSIVE )
        goto APPLY_FILTER;

    // stop recursion due to artificial fs?
    if( value.IsArtificialFS() && !params.IncludeArtificialFS )
    {
        value.SetQuality( FInfo::Qualities::NO_AFS );
        goto APPLY_FILTER;
    }

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems )
    {
        value.SetQuality( FInfo::Qualities::NOT_CROSSING_FS );
        goto APPLY_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth )
    {
        value.SetQuality( FInfo::Qualities::MAX_DEPTH_REACHED );
        ++parentSums.QtyStopsOnMaxDepth;
        goto APPLY_FILTER;
    }

    // stop recursion due to filter
    if(      depth > 0
         &&  params.DirectoryFilterPreRecursion
         && !params.DirectoryFilterPreRecursion->Includes( node, actPath )  )
        goto APPLY_FILTER;

    // mark as recursively scanned
    value.SetQuality( FInfo::Qualities::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR )
    {
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )   // never recurse with symlinks RESIDING on artificial fs!
        {
            value.SetQuality( FInfo::Qualities::NOT_FOLLOWED );
            goto APPLY_FILTER;
        }

        if( value.TargetIsArtificialFS() &&  !params.IncludeArtificialFS )
        {
            value.SetQuality( FInfo::Qualities::NO_AFS );
            goto APPLY_FILTER;
        }

        // recurse into symlink target
        FInfo::DirectorySums childSums;
        if( startScan( static_cast<FTree&>(node.Tree()), value.GetRealLinkTarget(), params, childSums, resultPaths)  )
            value.SetQuality(FInfo::Qualities::DUPLICATE);
        value.SetSums( childSums );
        parentSums+= childSums;

        goto APPLY_FILTER;
    }

    // DIRECTORY RECURSION
    {ALIB_STRING_RESETTER( actPath );
        if( pxDir == nullptr )
        {
            ALIB_ASSERT_ERROR( actPath.Buffer() == nameOrFullPath.Buffer(), "CAMP/FILES", "Internal error" )
            actPath.SetLength(nameOrFullPath.Length());
        }
        else
        {
            if( actPath.Length() > 1 ) actPath << DirectorySeparator;
            actPath << nameOrFullPath;
        }

        errno= 0;
        int fd= pxDir ? openat( dirfd(pxDir), nameOrFullPath, O_RDONLY | O_DIRECTORY )
                      : open  (               actPath       , O_RDONLY | O_DIRECTORY );

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
                            ALIB_ASSERT_ERROR(major(currentDevice) == 0, "CAMP/FILES",
                                              "Posix raised ({}) {!Q} on reading a directory which is not located on an "
                                              "artificial filesystem (like /proc). File:{!Q}",
                                              errno, SystemErrors(errno), dbgActFile )
                                    break;
                        default:    value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                                    ALIB_ERROR("CAMP/FILES", "Posix raised ({}) {!Q} on reading directory {!Q}",
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

                // recursive call
                auto childNode= node;
                const CString childName(const_cast<const char*>(&pxEntry->d_name[0]));
                childNode.GoToCreateChildIfNotExistent( childName );
                scanFilePosix(childDir, childNode, childName,
                              depth + 1, params, currentDevice, subSums, actPath, resultPaths);
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
            goto APPLY_FILTER;
        } // success opening director

        // error with recursion
        ALIB_ASSERT_ERROR(errno != ENOTDIR, "CAMP/FILES",
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
            ALIB_ERROR("CAMP/FILES", "Unknown error {}({!Q}) while opening directory {!Q}",
                       errno, SystemErrors(errno), actPath)
                value.SetQuality( FInfo::Qualities::UNKNOWN_ERROR );
                break;
        }
    }

    // ------------------------------------ Apply Filter  ------------------------------------------
    APPLY_FILTER:
    // delete node only if this was a new scan. It must not be deleted, if this node was
    // created as a path.
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
                parentSums-= value.Sums();
                if( params.RemoveEmptyDirectories )
                {
                    node.Delete();
                    return;
                }
                else
                    // do not return here. Still count the type below
                    node.DeleteChildren();
            }

        }
        else
        {
            if (      params.FileFilter
                  && !params.FileFilter->Includes(node, actPath )   )
            {
                node.Delete();
                return;
            }
        }
    }

    // cnt file type
    parentSums.Add(value);

    ALIB_WARNINGS_RESTORE
    DBG_CHECKERRNO_WITH_PATH
} // scanFilePosix()
}} // namespace [alib::files::anonymous]
#undef DBG_CHECKERRNO_WITH_PATH


//--------------------------------------------------------------------------------------------------
//--- UNKNOWN platform, using C++17 filesystem (not all functionality given)
//--------------------------------------------------------------------------------------------------
#else
#   pragma message ("Unknown Platform. Using std::filesystem for scanning. In file: " __FILE__ )

//#endif
//#if !defined(TOOD_REMOVE_THIS_WITH_THE_ENDIF_ABOVE)

#include "alib/compatibility/std_strings.hpp"
#include <filesystem>
namespace fs = std::filesystem;
using namespace alib;

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
        ALIB_ASSERT_WARNING(errno == 0, "CAMP/FILES", "Errno set ({}){!Q}.",                     \
                            errno, SystemErrors(errno) )                                           \
        errno= 0;
#   define DBG_CHECKERRNO_WITH_PATH                                                                \
        ALIB_ASSERT_WARNING(errno == 0, "CAMP/FILES", "Errno set ({}){!Q}. Current path: {}",    \
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
                    std::vector<ResultsPaths>&    resultPaths  )
{
#if defined(__MINGW32__)
    PathString pathAsCString(path.c_str());
    pathAsCString.Terminate();
#else
    CString     pathAsCString(path.c_str());
#endif
    Substring   parentPath= pathAsCString.Substring(0, pathAsCString.LastIndexOf(DirectorySeparator));

    #if !defined(_WIN32)
    ALIB_ASSERT_ERROR(          pathAsCString.CharAtStart()== DirectorySeparator
                        &&  (   pathAsCString.Length()==1
                             || pathAsCString.CharAtEnd()  != DirectorySeparator)
                        &&      pathAsCString.IndexOf(String8(DirectorySeparator).Append(DirectorySeparator))<0,
                    "CAMP/FILES","Given path not absolute or ending with '{}': {}", DirectorySeparator, pathAsCString )
    #else
    ALIB_ASSERT_ERROR(      (    (    pathAsCString.CharAt(1)== ':'
                                   && pathAsCString.CharAt(2)== DirectorySeparator
                                   &&  (   pathAsCString.Length()==3
                                        || pathAsCString.CharAtEnd()  != DirectorySeparator) )

                              || (    pathAsCString.CharAt(0)== DirectorySeparator
                                   && pathAsCString.CharAt(1)== DirectorySeparator
                                   &&  (   pathAsCString.Length()==2
                                        || pathAsCString.CharAtEnd()  != DirectorySeparator) )
                            )
                        &&  pathAsCString.IndexOf(String8(DirectorySeparator).Append(DirectorySeparator), 2)<0,
                    "CAMP/FILES","Given path not absolute or ending with '{}': {}", DirectorySeparator, pathAsCString )
    #endif


    ALIB_MESSAGE( "CAMP/FILES","[{}] {}/{}   {}",  &params != &paramsPathOnly ? '>':'P',  depth,
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
        PathString    symLinkDest;
        PathString    symLinkDestReal;

        // read base stats (we have to use symlink_status() which does NOT follow the symlink!)
        fs::file_status stats= fs::symlink_status(path);
        ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
        if(errorCode) 
        {
            ALIB_ERROR("CAMP/FILES",
                       "Unhandled error code invoking 'directory_entry::symlink_status()': {} ({!Q})\n"
                       "    With file: {!Q}",
                       errorCode.value(), errorCode.message(), pathAsCString )
            ALIB_DBG( errno= 0;)
            value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
            goto APPLY_FILTER;
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
                    default:  ALIB_ERROR("CAMP/FILES", "Unhandled error code invoking 'fs::read_symlink()': {} ({!Q})\n"
                                         "   with file: ", errorCode.value(), errorCode.message(), pathAsCString )
                              ALIB_DBG( errno= 0;)
                              value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                              goto APPLY_FILTER;
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
                symLinkDestReal.ShortenTo( symLinkDestReal.LastIndexOf(DirectorySeparator) + 1);
                symLinkDestReal << symLinkDest;
                realPath=  fs::canonical(fs::path(
                               std::basic_string_view<character>(symLinkDestReal.Buffer(),
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
                default:  ALIB_ERROR("CAMP/FILES", "Unhandled error code invoking 'fs::canonical()': {} ({!Q})\n"
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
                default: ALIB_WARNING("CAMP/FILES",
                             "Unhandled error code invoking 'directory_entry::status()': {} ({!Q})\n"
                             "    With file: {!Q}", errorCode.value(), errorCode.message(), pathAsCString )
                         goto ABORT_SYMLINK;
            }
            ALIB_WARNINGS_RESTORE

            // check for target artificial fs
                // Not available with std::filesystem version
        }

        ABORT_SYMLINK:
        DBG_CHECKERRNO_WITH_PATH

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
                     ALIB_WARNING("CAMP/FILES", "Internal error. 'not found' file type can't happen. File: ", pathAsCString )
                     ALIB_DBG( errno= 0;) goto APPLY_FILTER;
                case fs::file_type::none     :
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("CAMP/FILES", "Internal error. 'none' file type can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_FILTER;
                case fs::file_type::unknown  :
                     value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                     ALIB_WARNING("CAMP/FILES", "Internal error. Can't happen. File: ", pathAsCString)
                     ALIB_DBG( errno= 0;) goto APPLY_FILTER;
            }
            value.SetType( type );
        }

        // 2. perms
        value.SetPerms( FInfo::Permissions(int32_t(stats.permissions())) );

        // 3. timestamps
        // attn: This method always follows symbolic link and uses the target's time
        // This seems to be a confirmed behaviour:
        // https://stackoverflow.com/questions/50778660/boost-filesystem-how-to-get-last-write-time-for-symlink-without-resolving
        auto fsTime= std::filesystem::file_time_type(std::filesystem::file_time_type::clock::now());
        if ( value.Quality() <= FInfo::Qualities::RESOLVED ) // no error
        {
            fsTime= fs::last_write_time( path, errorCode );
            ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
            if(errorCode) switch( SystemErrors(errorCode.value()) )
            {   // This happens if with symbolic links that point to nowhere.
                case SystemErrors::enoent: ALIB_ERROR( "CAMP/FILES",
                                              "Internal error. This should never happen, checked above. "
                                              "Undefined system error handling" ) ALIB_DBG( errno= 0;)
                                           value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                                           break;

                default:     ALIB_ERROR( "CAMP/FILES", "Unhandled error code invoking 'fs::last_write_time()': {} ({!Q})\n"
                                         "    With file {!Q}.", errorCode.value(), errorCode.message(), pathAsCString )
                             fsTime= (decltype(fsTime)::min)();                      ALIB_DBG( errno= 0;)
                             break;
            }
            ALIB_WARNINGS_RESTORE
        }


        #if ALIB_CPP_STANDARD == 17 || defined(__APPLE__)  || defined(__ANDROID_NDK__)
            value.SetMTime( DateTime::FromEpochSeconds( to_time_t( fsTime ) ) );
        #else
            value.SetMTime( DateTime::FromEpochSeconds( std::chrono::system_clock::to_time_t(
                                                        std::chrono::clock_cast<std::chrono::system_clock>(fsTime) ) ) );
        #endif


        // 4. size
        errorCode.clear();
        value.SetSize(   symLinkDest.Length() > 0                    ?  uinteger(symLinkDest.Length())
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
                                       || value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR , "CAMP/FILES",
                        "Internal error. This should never happen. Undefined system error handling" )
                    break;

                // size not supported. Happens with sockets, files in /proc, etc
                case SystemErrors::eopnotsupp: break;
                   default: ALIB_ERROR("CAMP/FILES", "Unhandled error code invoking 'directory_entry::file_size()':{} ({!Q})\n"
                                       "    With file {!Q}.", errorCode.value(), errorCode.message(), pathAsCString )
                            ALIB_DBG( errno= 0;)
                    break;
            }
            ALIB_WARNINGS_RESTORE
        }

        // 5. uid/gid
        value.SetOwner( FInfo::UnknownID );
        value.SetGroup( FInfo::UnknownID );

        // 6. Add extended information
        if(    oldQuality < FInfo::Qualities::STATS
            && (value.IsDirectory() || symLinkDest.IsNotEmpty()) )
            static_cast<FTree&>(node.Tree()).AllocateExtendedInfo( node, symLinkDest, symLinkDestReal );

    } // if scan stats (quality was just path)

    DBG_CHECKERRNO_WITH_PATH

    // Count broken link.
    if(value.Quality() == FInfo::Qualities::BROKEN_LINK)
    {
        ++parentSums.QtyErrsBrokenLink;
        goto APPLY_FILTER;
    }

    // ------------------------------ recursion with directories?  ------------------------------
    if(    !value.IsDirectory()
        || value.Quality() >= FInfo::Qualities::RECURSIVE )
        goto APPLY_FILTER;


    // stop recursion due to artificial fs?
        // Not supported with std::filesystem!

    // stop recursion due to crossing filesystem?
    if( value.IsCrossingFS() &&   !params.CrossFileSystems )
    {
        value.SetQuality( FInfo::Qualities::NOT_CROSSING_FS );
        goto APPLY_FILTER;
    }

    // stop recursion due to max depth?
    if( depth >= params.MaxDepth )
    {
        value.SetQuality( FInfo::Qualities::MAX_DEPTH_REACHED );
        ++parentSums.QtyStopsOnMaxDepth;
        goto APPLY_FILTER;
    }

    // stop recursion due to filter
    if(      depth > 0
         &&  params.DirectoryFilterPreRecursion
         && !params.DirectoryFilterPreRecursion->Includes( node, parentPath )  )
        goto APPLY_FILTER;

    // mark as recursively scanned
    value.SetQuality( FInfo::Qualities::RECURSIVE );

    // SYMLINK RECURSION
    if (  value.Type() == FInfo::Types::SYMBOLIC_LINK_DIR )
    {
        if(    params.LinkTreatment != ScanParameters::SymbolicLinks::RECURSIVE
            || value.IsArtificialFS() )   // never recurse with symlinks RESIDING on artificial fs!
        {
            value.SetQuality( FInfo::Qualities::NOT_FOLLOWED );
            goto APPLY_FILTER;
        }
        else
        {
            // recurse into symlink target
            FInfo::DirectorySums childSums;
            if( startScan( static_cast<FTree&>(node.Tree()), value.GetRealLinkTarget(), params, childSums, resultPaths) )
                value.SetQuality(FInfo::Qualities::DUPLICATE);
            value.SetSums( childSums );
            parentSums+= childSums;
            goto APPLY_FILTER;
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
            #if defined(__MINGW32__)
                PathString mingwBuf( childDir.path().c_str());
                Substring childName(mingwBuf);
            #else
                Substring childName(CString(childDir.path().c_str()));
            #endif
                childName.ConsumeChars(childName.LastIndexOf(DirectorySeparator) + 1);
                auto childNode= node;
                childNode.GoToCreateChildIfNotExistent( childName );
                scanFileStdFS(childDir.path(), childNode, depth + 1, params, subSums, resultPaths );
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
            goto APPLY_FILTER;
        }
    }

    // error with recursion
    ALIB_ASSERT_ERROR(errorCode.value() != ENOTDIR, "CAMP/FILES",
                      "Internal error opening directory. This must never happen")

    ALIB_WARNINGS_ALLOW_SPARSE_ENUM_SWITCH
    if(errorCode) switch (SystemErrors(errorCode.value()))
    {
        case SystemErrors::einval: // happens with /proc
        case SystemErrors::eacces: ++parentSums.QtyErrsAccess;
                                   value.SetQuality( FInfo::Qualities::NO_ACCESS_DIR );
                                   ALIB_DBG( errno= 0;)
                                   goto APPLY_FILTER;

        default: value.SetQuality(FInfo::Qualities::UNKNOWN_ERROR);
                 ALIB_ERROR("CAMP/FILES", "Unknown error {}({!Q}) while opening directory {!Q}",
                             errorCode.value(), SystemErrors(errorCode.value()), pathAsCString)
                 ALIB_DBG( errno= 0;)
                 goto APPLY_FILTER;
    }
    ALIB_WARNINGS_RESTORE
    ALIB_DBG( errno= 0;)

    // ------------------------------------ Apply Filter  ------------------------------------------
    APPLY_FILTER:
    // delete node only if this was a new scan. It must not be deleted, if this node was
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
                    node.Delete();
                    return;
                }
                else
                    node.DeleteChildren(); // do not return here. Still count the directories
            }

        }
        else
        {
            if (      params.FileFilter
                  && !params.FileFilter->Includes(node, parentPath )   )
            {
                node.Delete();
                return;
            }
        }
    }

    // cnt file type
    parentSums.Add(value);

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
bool startScan( FTree&                      tree,
                String                      realPath,
                ScanParameters&             params,
                FInfo::DirectorySums&       parentSums,
                std::vector<ResultsPaths>&  resultPaths  )
{
#if !defined(_WIN32)
    ALIB_ASSERT_ERROR( realPath.CharAtStart() == DirectorySeparator,
                       "CAMP/FILES", "Real path is not absolute: ", realPath )

    PathString path(DirectorySeparator);
    FTree::Cursor   node         = tree.Root();

    // travel any existing portion of the path
    Substring       pathRemainder= node.GoToTraversedPath( realPath );
    path << realPath.Substring(1, realPath.Length() - pathRemainder.Length() - 1);
#else
    ALIB_ASSERT_ERROR(    (    realPath.CharAt(2) == DirectorySeparator
                            && realPath.CharAt(1) == ':'                )
                      ||  ( realPath.CharAt(0) == DirectorySeparator
                            && realPath.CharAt(1) == DirectorySeparator ),
                       "CAMP/FILES", "Real path is not absolute: ", realPath )

    PathString      path;
    Substring       pathRemainder;
    FTree::Cursor   node = tree.Root();
    if(realPath.CharAt(1) == ':')
    {
        path << realPath.Substring(0,3);
        node.GoToCreateChildIfNotExistent(realPath.Substring(0,2));
        pathRemainder= node.GoToTraversedPath( realPath.Substring(3) );
        path << realPath.Substring(3, realPath.Length() - pathRemainder.Length() -3 );
    }
    else
    {
        integer serverNameEnd= realPath.IndexOf( DirectorySeparator, 2);
        if( serverNameEnd < 0)
            serverNameEnd= realPath.Length();
        path << realPath.Substring(0, serverNameEnd);
        node.GoToCreateChildIfNotExistent(realPath.Substring(2, serverNameEnd - 2));
        pathRemainder= node.GoToTraversedPath( realPath.Substring(serverNameEnd) );
        path << realPath.Substring(serverNameEnd, realPath.Length() - pathRemainder.Length() -serverNameEnd );
    }

#endif

    DBG_CHECKERRNO


    // existed already?
    if( pathRemainder.IsEmpty() )
    {
        // For directories, call scan just for the case of having 'higher' scan parameters
        if( node.Value().IsDirectory())
        {

            #if   (    (defined(__GLIBCXX__) && !defined(__MINGW32__))                  \
                     || defined(__APPLE__)                                              \
                     || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)

                path.Terminate();
                CString    fullPathChildName(path);
                path.SetLength(path.LastIndexOf(DirectorySeparator) );
                scanFilePosix( nullptr, node, fullPathChildName, 0, params, 0, parentSums, path, resultPaths );
            #else
                scanFileStdFS( fs::path(std::basic_string_view<character>(path.Buffer(), size_t(path.Length()))),
                               node, 0, params, parentSums, resultPaths  );
            #endif

            //resultPaths.emplace_back(ResultsPaths(realPath, node, true));
        }

        return true;
    }

    // did not exist already
    if( path.Length() > 1 )
        path.DeleteEnd<false>(1);

    Tokenizer  tknzr( pathRemainder, DirectorySeparator );
    while(tknzr.HasNext())
    {
        String name;
        if( path.Length() != 1 )
        {
            name= tknzr.Next();
            node= node.CreateChild(name);
        }


        bool isLastPathElement= !tknzr.HasNext();
        if( isLastPathElement )
            parentSums= FInfo::DirectorySums(); // clear the sums, because only the results of the last element is used.

        auto detectNodeDeletion= node.Depth();

        #if   (    (defined(__GLIBCXX__) && !defined(__MINGW32__))                      \
                 || defined(__APPLE__)                                                  \
                 || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)

            if( path.IsEmpty() ) path << DirectorySeparator;
            CString    fullPathChildName;
            {
                // add node name to existing path and use same buffer for fullPathChildName!
                ALIB_STRING_RESETTER( path );
                if( path.Length() > 1 ) path << DirectorySeparator;
                path << node.Name();
                path.Terminate();
                fullPathChildName= path;
            }

            scanFilePosix( nullptr, node, fullPathChildName,
                           0, isLastPathElement ? params : paramsPathOnly,
                           0, parentSums, path, resultPaths );
            if( fullPathChildName.Length() == 1 )  path.Reset();
            else { if(path.Length() > 1)  path << DirectorySeparator; path << name; }
        #else
            if( path.Length() != 1 )  path << DirectorySeparator << name;
            scanFileStdFS( fs::path(std::basic_string_view<character>(path.Buffer(), size_t(path.Length()))), node, 0,
                           isLastPathElement ? params : paramsPathOnly,
                           parentSums, resultPaths );
            if( path.Length() == 1 )  path.Reset();
        #endif

        // if the just created node was not deleted during scan, add it to the result list
        if( isLastPathElement && (detectNodeDeletion == node.Depth() ) )
            resultPaths.insert(resultPaths.begin(), ResultsPaths(realPath, node, false));

        // Correct quality from max depth to stats
        if( !isLastPathElement  && node.Value().Quality() == FInfo::Qualities::MAX_DEPTH_REACHED)
            node.Value().SetQuality(FInfo::Qualities::STATS);
    }

    return false;
}

} // namespace  alib::files[::anonymous]

#endif // !defined(ALIB_DOX)

// --------------------------------------------------------------------------------------------------
//--- ScanFiles()
//--------------------------------------------------------------------------------------------------
enum FInfo::Qualities  ScanFiles( FTree&                      tree,
                                  ScanParameters&             parameters,
                                  std::vector<ResultsPaths>&  resultPaths  )

{
    // get real path
    PathString path(parameters.StartPath);
    PathString realPath;
    realPath.Terminate();


    #if   (     (defined(__GLIBCXX__) && !defined(__MINGW32__))             \
              || defined(__APPLE__)                                         \
              || defined(__ANDROID_NDK__) )        && !defined(ALIB_TESTSTDFS)
        if(!realpath(path.Terminate(), realPath.VBuffer() ) ) switch (errno)
        {
            case EACCES: ALIB_DBG(errno= 0;) return FInfo::Qualities::NO_ACCESS;
            case ENOENT: ALIB_DBG(errno= 0;) return FInfo::Qualities::NOT_EXISTENT;
            case ELOOP:  ALIB_DBG(errno= 0;) return FInfo::Qualities::CIRCULAR_LINK;
            default: ALIB_ERROR("CAMP/FILES", "Posix raised ({}) {!Q} on resolving start path {!Q}",
                                errno, SystemErrors(errno), path )   ALIB_DBG(errno= 0;)
                     return FInfo::Qualities::UNKNOWN_ERROR;
        }
        realPath.DetectLength();
    #else
        {
            std::error_code errorCode;
            fs::path fsRealPath= fs::canonical(fs::path(std::basic_string_view<character>(path.Buffer(),
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
                default: ALIB_ERROR("CAMP/FILES", "std::filesystem raised ({}) {!Q} on resolving start path {!Q}",
                                    errorCode.value(), errorCode.message(), path )   ALIB_DBG(errno= 0;)
                         return FInfo::Qualities::UNKNOWN_ERROR;
            }
            ALIB_WARNINGS_RESTORE
            realPath << fsRealPath.c_str();
        }

    #endif

    path.Reset(DirectorySeparator);

    ALIB_DBG( errno=0; )

    auto firstResultPos= resultPaths.size();
    FInfo::DirectorySums dummySums;
    startScan( tree, realPath, parameters, dummySums, resultPaths );
    return (*(resultPaths.begin() + int(firstResultPos))).Node.Value().Quality();
}

#undef DBG_CHECKERRNO


} // namespace [alib::files]

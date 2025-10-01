// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2025 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib_precompile.hpp"
#if !defined(ALIB_C20_MODULES) || ((ALIB_C20_MODULES != 0) && (ALIB_C20_MODULES != 1))
#   error "Symbol ALIB_C20_MODULES has to be given to the compiler as either 0 or 1"
#endif
#if ALIB_C20_MODULES
    module;
#endif
// ======================================   Global Fragment   ======================================
#include "alib/strings/strings.prepro.hpp"
#include "alib/boxing/boxing.prepro.hpp"
#include "alib/system/system.prepro.hpp"
#if !DOXYGEN
#   if defined ( _WIN32 )
#      include <direct.h>
#   elif   defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)    || defined(__ANDROID_NDK__)
#      include <unistd.h>
#      include <dirent.h>
#      include <sys/stat.h>
#      include <pwd.h>
#   else
#      pragma message ("Unknown Platform in file: " __FILE__ )
#   endif
#   include <fstream>
#endif // !DOXYGEN
// ===========================================   Module   ==========================================
#if ALIB_C20_MODULES
    module ALib.System;
#  if ALIB_STRINGS
    import   ALib.Strings;
#  endif
    import   ALib.Strings.StdIOStream;
#  if ALIB_BOXING
    import   ALib.Boxing;
#  endif
#else
#   include "ALib.Strings.H"
#   include "ALib.Strings.StdIOStream.H"
#   include "ALib.Boxing.H"
#   include "ALib.System.H"
#endif
// ======================================   Implementation   =======================================
ALIB_BOXING_VTABLE_DEFINE( alib::system::Path*            , vt_system_path          )
ALIB_BOXING_VTABLE_DEFINE( alib::system::SystemErrors     , vt_system_systemerrors  )

/// This is the reference documentation of module \alib_system, which exposes it's entities
/// in this namespace.
namespace alib::system {

#if !DOXYGEN
//--------------- two versions to load environment variables into a Path instance -------------
namespace {

ALIB_WARNINGS_IGNORE_UNUSED_FUNCTION
template<typename TRequires= PathCharType>
requires std::same_as<TRequires, character>
bool loadEnvVar( const CString& name, AString& target, lang::CurrentData targetData  =lang::CurrentData::Clear ) {
    return EnvironmentVariables::Get( name, target, targetData );
}

template<typename TRequires= PathCharType>
requires (!std::same_as<TRequires, character>)
bool loadEnvVar( const CString& name, Path& target, lang::CurrentData targetData  =lang::CurrentData::Clear ) {
    String256 buf;
    auto result= EnvironmentVariables::Get( name, buf, targetData );
    target.Reset( buf );
    return result;
}
ALIB_WARNINGS_RESTORE

} // anonymous namespace

#endif // !DOXYGEN

// #################################################################################################
// Static variables
// #################################################################################################
PathString  Path::tempDirEvaluatedOnce;
PathString  Path::varTempDirEvaluatedOnce;

// #################################################################################################
// Change
// #################################################################################################
//! @cond NO_DOX
namespace {
void createTempFolderInHomeDir( const PathString& folderName, Path& resultPath,
                                const NString& reasonMsg )
{
    // get home directory and set this as fallback result value
    Path homeTemp( SystemFolders::Home );
    resultPath.Reset( homeTemp  );

    // add given folder name and check if already exists
    homeTemp._( DIRECTORY_SEPARATOR )._( folderName );
    bool exists= homeTemp.IsDirectory();
    if( !exists )
    {
        if( homeTemp.Create() == SystemErrors::OK )
        {
            exists= true;
            NAString fileName( homeTemp ); fileName._( DIRECTORY_SEPARATOR )._( "readme.txt" );

            std::ofstream file ( fileName );
            if ( file.is_open() )
            {
                const ProcessInfo& pi= ProcessInfo::Current();
                file << "This folder was created by \"" << pi.CmdLine
                     << "\"" << std::endl
                     << "to be used for temporary files." << std::endl;
                file.write( reasonMsg.Buffer(), reasonMsg.Length() );
                file << std::endl;
                file.close();
            }
        }
    }

    // if existed or got created
    if( exists )
        resultPath.Reset( homeTemp );
}
} // anonymous namespace
//! @endcond

bool Path::Change( SystemFolders special )
{
    switch( special )
    {
        case SystemFolders::Root:       _( DIRECTORY_SEPARATOR );
                                        return true;

        case SystemFolders::Current:
        {
            Reset();
            nchar charBuf[FILENAME_MAX];

            #if   defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
                if ( ! getcwd( charBuf, sizeof(charBuf ) ) )
                     return false;
            #elif defined ( _WIN32 )
                if ( !_getcwd( charBuf, sizeof(charBuf ) ) )
                     return false;
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
                return false;
            #endif

            this ->_( static_cast<const nchar*>( charBuf ) );
            return true;
        }
        

        case SystemFolders::Home:
        {
            #if defined (__unix__)
                    if ( !loadEnvVar( A_CHAR("HOME"), *this ) )
                    {
                        struct passwd* pwd = getpwuid(getuid());
                        Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                    }
                return true;

            #elif defined(__APPLE__)
                macos::ALIB_APPLE_OC_NSHomeDirectory( *this );
                if ( IsEmpty() )
                {
                    struct passwd* pwd = getpwuid(getuid());
                    Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                }
                return true;


            #elif defined(_WIN32)
                if ( !loadEnvVar( A_CHAR("USERPROFILE"), *this ) || !IsDirectory() )
                {
                    loadEnvVar( A_CHAR("HOMEDRIVE"), *this );
                    loadEnvVar( A_CHAR("HOMEPATH" ), *this, lang::CurrentData::Keep );
                }
                return true;
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
                return false;
            #endif
        }


        case SystemFolders::HomeConfig:
        {
            if( !Change( SystemFolders::Home ) )
                return false;

            // try ".config" and "AppData/Roaming" subdirectories.
            #if defined (__unix__)
                Change( A_PATH(".config") );
                return true;
            #elif defined(__APPLE__)
                Change( A_PATH("Library/Preferences") );
                return true;
        #elif defined(_WIN32)
                Change( Path(A_PATH("AppData")) << DIRECTORY_SEPARATOR << A_PATH("Roaming") );
                return true;
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
                return false;
            #endif
        }

        case SystemFolders::Module:
        {
            Reset( ProcessInfo::Current().ExecFilePath );
            return true;
        }

        case SystemFolders::Temp:
        {
          #if ALIB_MONOMEM
            ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
          #endif
            if ( tempDirEvaluatedOnce.IsNull() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    if ( Path(A_PATH("/tmp") ).IsDirectory() )
                      #if ALIB_MONOMEM
                        tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, A_PATH("/tmp"));
                      #else
                      {
                        lang::HeapAllocator ha;
                        tempDirEvaluatedOnce.Allocate(ha, A_PATH("/tmp"));
                      }
                      #endif


                #elif defined(__APPLE__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    Path temp;
                    macos::ALIB_APPLE_OC_NSTemporaryDirectory( temp );
                    if ( temp.IsNotEmpty() )
                      #if ALIB_MONOMEM
                        tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, temp);
                      #else
                      {
                        lang::HeapAllocator ha;
                        tempDirEvaluatedOnce.Allocate(ha, temp);
                      }
                      #endif
                    else
                    {
                        temp.Reset( A_PATH("/tmp") );
                        if ( temp.IsDirectory() )
                          #if ALIB_MONOMEM
                            tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, temp);
                          #else
                          {
                            lang::HeapAllocator ha;
                            tempDirEvaluatedOnce.Allocate(ha, temp);
                          }
                          #endif
                    }


                #elif defined(_WIN32)
                    NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    Path testDir;
                    if (     ( loadEnvVar( A_CHAR("TMP") , testDir ) && testDir.IsDirectory() )
                         ||  ( loadEnvVar( A_CHAR("TEMP"), testDir ) && testDir.IsDirectory() ) )
                    {
                      #if ALIB_MONOMEM
                        tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, testDir);
                      #else
                      {
                        lang::HeapAllocator ha;
                        tempDirEvaluatedOnce.Allocate(ha, testDir);
                      }
                      #endif
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( tempDirEvaluatedOnce.IsEmpty() )
                {
                    Path homeTemp;
                    createTempFolderInHomeDir( A_PATH(".tmp"), homeTemp, reasonMsg );

                    // If this did not work, use home
                    if( homeTemp.IsNotEmpty() )
                    {
                      #if ALIB_MONOMEM
                        ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                        tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, homeTemp);
                      #else
                        {
                            lang::HeapAllocator ha;
                            tempDirEvaluatedOnce.Allocate(ha, homeTemp);
                        }
                      #endif

                    }
                    else
                    {
                        Change( SystemFolders::Home );
                        {
                      #if ALIB_MONOMEM
                            ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                            tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, *this);
                      #else
                            {
                                lang::HeapAllocator ha;
                                tempDirEvaluatedOnce.Allocate(ha, *this);
                            }
                      #endif
                        }
                    }
                }
            }

            // set path to evaluated dir name
            Reset( tempDirEvaluatedOnce );
            return true;

        } //  case SystemFolders::Temp:

        case SystemFolders::VarTemp:
        {
            #if ALIB_MONOMEM
              ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            #endif
            if ( varTempDirEvaluatedOnce.IsNull() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default folder \"/var/tmp\" could not be found.)";

                    if ( Path( A_PATH("/var/tmp") ).IsDirectory() )
                      #if ALIB_MONOMEM
                        varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, A_PATH("/var/tmp"));
                      #else
                      {
                        lang::HeapAllocator ha;
                        varTempDirEvaluatedOnce.Allocate(ha, A_PATH("/var/tmp"));
                      }
                      #endif

                #elif defined(__APPLE__)
                     const NString reasonMsg=  "(The default folder \"/private/var/tmp\" could not be found.)";
                     Path temp( A_PATH("/private/var/tmp") );
                     if ( temp.IsDirectory() )
                       #if ALIB_MONOMEM
                         varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, temp);
                       #else
                       {
                         lang::HeapAllocator ha;
                         varTempDirEvaluatedOnce.Allocate(ha, temp);
                       }
                       #endif

                #elif defined(_WIN32)
                    const NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    Path testDir;
                    if (     ( loadEnvVar( A_CHAR("TMP") , testDir ) && testDir.IsDirectory() )
                         ||  ( loadEnvVar( A_CHAR("TEMP"), testDir ) && testDir.IsDirectory() ) )
                    {
                       #if ALIB_MONOMEM
                         varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, testDir);
                       #else
                       {
                         lang::HeapAllocator ha;
                         varTempDirEvaluatedOnce.Allocate(ha, testDir);
                       }
                       #endif
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( varTempDirEvaluatedOnce.IsEmpty() )
                {
                    Path varTemp;
                    createTempFolderInHomeDir( A_PATH(".var.tmp"), varTemp, reasonMsg );

                    // If this did not work, use home
                    if( varTemp.IsNotEmpty() )
                    {
                        #if ALIB_MONOMEM
                          ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                          varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, varTemp);
                        #else
                        {
                          lang::HeapAllocator ha;
                          varTempDirEvaluatedOnce.Allocate(ha, varTemp);
                        }
                        #endif

                    }
                    else
                    {
                        Change( SystemFolders::Home );
                        {
                            #if ALIB_MONOMEM
                              ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                              varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, *this);
                            #else
                            {
                              lang::HeapAllocator ha;
                              varTempDirEvaluatedOnce.Allocate(ha, *this);
                            }
                            #endif
                        }
                    }
                }

            }
            // now path to evaluated dir name
            Reset( varTempDirEvaluatedOnce );
            return true;
        }

        default: ALIB_ERROR("SYSTEM", "Illegal switch state.")
                 return false;
    } // switch ( special )
}

void Path::AddModuleName( const PathString& extension )
{
    if( CharAtEnd() != DIRECTORY_SEPARATOR )
        _(DIRECTORY_SEPARATOR);

    _( ProcessInfo::Current().Name  );

    #if defined(_WIN32)
        if( EndsWith( A_PATH(".exe") ) )
            DeleteEnd( 4 );
    #endif

    _( extension );
}

bool Path::Change( const PathString& ppath )
{
    // absolute addressing
    Path path(ppath);
    if( path.CharAtStart() == DIRECTORY_SEPARATOR )
    {
        if( !path.IsDirectory() )
            return false;

        Reset( path );
        return true;
    }


    // relative addressing
    integer origLength= Length();
    _<NC>( DIRECTORY_SEPARATOR )._( path );

    if( IsDirectory() )
        return true;

    ShortenTo( origLength );
    return false;
}


bool Path::IsDirectory()
{
    #if defined (__GLIBC__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(*this, nPath, 512)
        DIR* dir= opendir( nPath );
        if ( dir != nullptr )
        {
            closedir( dir );
            return true;
        }
        return false;

    #elif defined(_WIN32)

        #if !ALIB_PATH_CHARACTERS_WIDE
            DWORD dwAttrib = GetFileAttributesA( Terminate() );
        #else
            DWORD dwAttrib = GetFileAttributesW( Terminate() );
        #endif
        if( dwAttrib == INVALID_FILE_ATTRIBUTES )
            return false;
        if ( dwAttrib & FILE_ATTRIBUTE_DIRECTORY )
            return true;
        return false;

    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

SystemErrors Path::MakeReal() {

    #if    (defined(__GLIBCXX__) && !defined(__MINGW32__))             \
         || defined(__APPLE__)                                         \
         || defined(__ANDROID_NDK__)

        Path realPath;
        if(!realpath(Terminate(), realPath.VBuffer() ) )
            return SystemErrors(errno);

        realPath.DetectLength();
        Reset(realPath);
        return SystemErrors::OK;

    #else

        namespace fs = std::filesystem;

        std::error_code errorCode;
        fs::path fsRealPath= fs::canonical(fs::path(std::basic_string_view<PathCharType>(Buffer(),
                                                                                  size_t(Length()))),
                                           errorCode);
        ALIB_DBG(if(errno==EINVAL && !errorCode) errno= 0;) // this happens!, we do not care, but clean up
        ALIB_DBG(if(errno==ENOENT && !errorCode) errno= 0;)

        if(errorCode)
            return SystemErrors(errorCode.value());

        Reset(fsRealPath.c_str());
        return SystemErrors::OK;
    
    #endif
}
SystemErrors Path::Create( const PathString& ppath )       // static
{
    if( Path::IsAbsolute(ppath) )
        Reset( ppath );
    else
        (*this)._( DIRECTORY_SEPARATOR )._( ppath );

    #if defined (__GLIBC__)  || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(*this,nPath,512)
        int errCode= mkdir( nPath,    S_IRWXU | S_IRGRP | S_IROTH
                                    | S_IXGRP | S_IXOTH             );

        return SystemErrors(errCode);

    #elif defined(_WIN32)
        #if !ALIB_PATH_CHARACTERS_WIDE
            BOOL result= CreateDirectoryA( Terminate(), NULL );
        #else
            BOOL result= CreateDirectoryW( Terminate(), NULL );
        #endif


        if( result )
            return SystemErrors::OK;

        return SystemErrors( GetLastError() );
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

} // namespace [alib::system]

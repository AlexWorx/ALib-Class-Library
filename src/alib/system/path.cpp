ALIB_BOXING_VTABLE_DEFINE( alib::system::Path*            , vt_system_path          )

/// This is the reference documentation of module \alib_system, which exposes it's entities
/// in this namespace.
namespace alib::system {

#if !DOXYGEN
//----------------- two versions to load environment variables into a Path instance ----------------
namespace {

ALIB_ALLOW_UNUSED_FUNCTION
template<typename TRequires= PathCharType>
requires std::same_as<TRequires, character>
bool loadEnvVar( const CString& name, AString& target,
                 lang::CurrentData targetData= lang::CurrentData::Clear ) {
    return EnvironmentVariables::Get( name, target, targetData );
}

template<typename TRequires= PathCharType>
requires (!std::same_as<TRequires, character>)
bool loadEnvVar( const CString& name, Path& target,
                 lang::CurrentData targetData= lang::CurrentData::Clear ) {
    String256 buf;
    auto result= EnvironmentVariables::Get( name, buf, targetData );
    target.Reset( buf );
    return result;
}
ALIB_POP_ALLOWANCE

} // anonymous namespace

#endif // !DOXYGEN

//##################################################################################################
// Static variables
//##################################################################################################
PathString  Path::tempDirEvaluatedOnce;
PathString  Path::varTempDirEvaluatedOnce;

//##################################################################################################
// Change
//##################################################################################################
//! @cond NO_DOX
namespace {
void createTempFolderInHomeDir( const PathString& folderName, Path& resultPath,
                                const NString& reasonMsg ) {
    // get home directory and set this as fallback result value
    Path homeTemp( SystemFolders::Home );
    resultPath.Reset( homeTemp  );

    // add given folder name and check if already exists
    homeTemp._( DIRECTORY_SEPARATOR )._( folderName );
    bool exists= homeTemp.IsDirectory();
    if( !exists ) {
        if( homeTemp.Create() == std::errc(0) ) {
            exists= true;
            NAString fileName( homeTemp ); fileName._( DIRECTORY_SEPARATOR )._( "readme.txt" );

            std::ofstream file ( fileName );
            if ( file.is_open() ) {
                const ProcessInfo& pi= ProcessInfo::Current();
                file << "This folder was created by \"" << pi.CmdLine
                     << "\"" << std::endl
                     << "to be used for temporary files." << std::endl;
                file.write( reasonMsg.Buffer(), reasonMsg.Length() );
                file << std::endl;
                file.close();
    }   }   }

    // if existed or got created
    if( exists )
        resultPath.Reset( homeTemp );
}
} // anonymous namespace
//! @endcond

bool Path::Change( SystemFolders special ) {
    switch( special ) {
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
                    if ( !loadEnvVar( A_CHAR("HOME"), *this ) ) {
                        struct passwd* pwd = getpwuid(getuid());
                        Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                    }
                return true;

            #elif defined(__APPLE__)
                macos::ALIB_APPLE_OC_NSHomeDirectory( *this );
                if ( IsEmpty() ) {
                    struct passwd* pwd = getpwuid(getuid());
                    Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                }
                return true;


            #elif defined(_WIN32)
                if ( !loadEnvVar( A_CHAR("USERPROFILE"), *this ) || !IsDirectory() ) {
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
            if ( tempDirEvaluatedOnce.IsNull() ) {
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
                    else {
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


                if( tempDirEvaluatedOnce.IsEmpty() ) {
                    Path homeTemp;
                    createTempFolderInHomeDir( A_PATH(".tmp"), homeTemp, reasonMsg );

                    // If this did not work, use home
                    if( homeTemp.IsNotEmpty() ) {
                      #if ALIB_MONOMEM
                        ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                        tempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, homeTemp);
                      #else
                        {
                            lang::HeapAllocator ha;
                            tempDirEvaluatedOnce.Allocate(ha, homeTemp);
                        }
                      #endif

                    } else {
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
            }   }   }   }

            // set path to evaluated dir name
            Reset( tempDirEvaluatedOnce );
            return true;

        } //  case SystemFolders::Temp:

        case SystemFolders::VarTemp:
        {
            #if ALIB_MONOMEM
              ALIB_LOCK_RECURSIVE_WITH(monomem::GLOBAL_ALLOCATOR_LOCK)
            #endif
            if ( varTempDirEvaluatedOnce.IsNull() ) {
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


                if( varTempDirEvaluatedOnce.IsEmpty() ) {
                    Path varTemp;
                    createTempFolderInHomeDir( A_PATH(".var.tmp"), varTemp, reasonMsg );

                    // If this did not work, use home
                    if( varTemp.IsNotEmpty() ) {
                        #if ALIB_MONOMEM
                          ALIB_LOCK_RECURSIVE_WITH( monomem::GLOBAL_ALLOCATOR_LOCK )
                          varTempDirEvaluatedOnce.Allocate(monomem::GLOBAL_ALLOCATOR, varTemp);
                        #else
                        {
                          lang::HeapAllocator ha;
                          varTempDirEvaluatedOnce.Allocate(ha, varTemp);
                        }
                        #endif

                    } else {
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
                }   }   }

            }
            // now path to evaluated dir name
            Reset( varTempDirEvaluatedOnce );
            return true;
        }

        default: ALIB_ERROR("SYSTEM", "Illegal switch state.")
                 return false;
    } // switch ( special )
}

void Path::AddModuleName( const PathString& extension ) {
    if( CharAtEnd() != DIRECTORY_SEPARATOR )
        _(DIRECTORY_SEPARATOR);

    _( ProcessInfo::Current().Name  );

    #if defined(_WIN32)
        if( EndsWith( A_PATH(".exe") ) )
            DeleteEnd( 4 );
    #endif

    _( extension );
}

bool Path::Change( const PathString& ppath ) {
    Path path(ppath);

    if( path.IsAbsolute() ) {
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

bool Path::ChangeToParent() {
    integer origLength= Length();

    integer startIdx= length;
    if ( CharAtEnd() == DIRECTORY_SEPARATOR  ) {
        if ( startIdx == 1 )
            return false;
        --startIdx;
    }

    integer lastDirSep= LastIndexOf(DIRECTORY_SEPARATOR, startIdx);
    if ( lastDirSep < 0 )
        return false;
    if ( lastDirSep == 0 )
        lastDirSep= 1;
    ShortenTo(lastDirSep);

    if( IsDirectory() )
        return true;

    length= origLength;
    return false;
}

bool Path::IsDirectory() {
    #if defined (__GLIBC__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(*this, nPath, MAX_PATH)
        DIR* dir= opendir( nPath );
        if ( dir != nullptr ) {
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


std::errc Path::MakeCanonical() {

    #if    (defined(__GLIBCXX__) && !defined(__MINGW32__))             \
         || defined(__APPLE__)                                         \
         || defined(__ANDROID_NDK__)

        Path realPath;
        if(!realpath(Terminate(), realPath.VBuffer() ) )
            return std::errc(errno);

        realPath.DetectLength();
        Reset(realPath);
        return std::errc(0);

    #else
        namespace fs = std::filesystem;

        std::error_code errorCode;
        fs::path fsRealPath= fs::canonical(fs::path(std::basic_string_view<PathCharType>(Buffer(),
                                                                                  size_t(Length()))),
                                           errorCode);
        ALIB_DBG(if(errno==EINVAL && !errorCode) errno= 0;) // this happens!, we do not care, but clean up
        ALIB_DBG(if(errno==ENOENT && !errorCode) errno= 0;)

        if(errorCode)
            return std::errc(errorCode.value());

        Reset(fsRealPath.c_str());
        return std::errc(0);
    
    #endif
}

std::errc Path::Create( const PathString& ppath ) {
    if( Path::IsAbsolute(ppath) )
        Reset( ppath );
    else
        (*this)._( DIRECTORY_SEPARATOR )._( ppath );

    #if defined (__GLIBC__)  || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(*this,nPath,MAX_PATH)
        int errCode= mkdir( nPath,    S_IRWXU | S_IRGRP | S_IROTH
                                    | S_IXGRP | S_IXOTH             );

        return std::errc(errCode);

    #elif defined(_WIN32)
        #if !ALIB_PATH_CHARACTERS_WIDE
            BOOL result= CreateDirectoryA( Terminate(), NULL );
        #else
            BOOL result= CreateDirectoryW( Terminate(), NULL );
        #endif


        if( result )
            return std::errc(0);

        return std::errc( GetLastError() );
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}

void Path::MakeAbsolute() {

    if( IsAbsolute() )
        return;

    Path original(*this);
    #if !defined(_WIN32)
        Change(SystemFolders::Current);
        _(DIRECTORY_SEPARATOR)._(original);
    #else
        if( original.IsEmpty() ) {
            Change( SystemFolders::Current );
            return;
        }

        TLocalString<PathCharType, 8192> fullPath;
        DWORD actualLength= GetFullPathNameW( original.Terminate(), 8192,
                                              fullPath.VBuffer(), nullptr );
        if( actualLength == 0 )
            return;

        fullPath.SetLength( integer(actualLength) );
        Reset( fullPath );
    #endif
}

} // namespace [alib::system]

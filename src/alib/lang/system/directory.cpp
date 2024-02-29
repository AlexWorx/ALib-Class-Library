// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2024 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib_precompile.hpp"

#if !defined(ALIB_DOX)
#if !defined (HPP_ALIB_CAMP_DIRECTORY)
#   include "alib/lang/system/directory.hpp"
#endif
#if !defined (HPP_ALIB_CAMP_PROCESSINFO)
#   include "alib/lang/system/processinfo.hpp"
#endif
#if !defined (HPP_ALIB_CAMP_ENVIRONMENT)
#   include "alib/lang/system/environment.hpp"
#endif

#if defined ( _WIN32 )
    #include <direct.h>
#elif   defined(__GLIBCXX__)  || defined(__APPLE__)    || defined(__ANDROID_NDK__)
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <pwd.h>
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_STRINGS_IOSTREAM)
#   include "alib/compatibility/std_strings_iostream.hpp"
#endif

#include <fstream>
#endif // !defined(ALIB_DOX)


namespace alib {  namespace lang::system {

// #################################################################################################
// Static variables
// #################################################################################################
AString         Directory::evaluatedTempDir;
AString         Directory::evaluatedVarTempDir;

// #################################################################################################
// Change
// #################################################################################################
//! @cond NO_DOX
void createTempFolderInHomeDir( const String& folderName, AString& resultPath, const NString& reasonMsg );
void createTempFolderInHomeDir( const String& folderName, AString& resultPath, const NString& reasonMsg )
{
    // get home directory and set this as fallback result value
    Directory homeTemp( Directory::SpecialFolder::Home );
    resultPath.Reset( homeTemp.Path );

    // add given folder name and check if already exists
    homeTemp.Path._( DirectorySeparator )._( folderName );
    bool exists= Directory::Exists( homeTemp.Path );
    if( !exists )
    {
        if( Directory::Create( homeTemp.Path ) == SystemErrors::OK )
        {
            exists= true;
            NAString fileName( homeTemp.Path ); fileName._( DirectorySeparator )._( "readme.txt" );

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
        resultPath.Reset( homeTemp.Path );
}
//! @endcond

void Directory::Change( SpecialFolder special )
{
    switch( special )
    {
        case SpecialFolder::Root:       Path._( DirectorySeparator );
                                        break;
        case SpecialFolder::Current:    Directory::CurrentDirectory( Path );
                                        break;
        case SpecialFolder::Home:
        {

            #if defined (__unix__)
                if ( !EnvironmentVariables::Get( A_CHAR("HOME"), Path ) )
                {
                    struct passwd* pwd = getpwuid(getuid());
                    Path.Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                }

            #elif defined(__APPLE__)
                macos::ALIB_APPLE_OC_NSHomeDirectory( Path );
                if ( Path.IsEmpty() )
                {
                    struct passwd* pwd = getpwuid(getuid());
                    Path.Reset( pwd ? NString( pwd->pw_dir ) :  "~/" );
                }


            #elif defined(_WIN32)
                if ( !EnvironmentVariables::Get( A_CHAR("USERPROFILE"), Path ) || !Directory::Exists( Path ) )
                {
                    EnvironmentVariables::Get( A_CHAR("HOMEDRIVE"), Path );
                    EnvironmentVariables::Get( A_CHAR("HOMEPATH" ), Path, CurrentData::Keep );
                }
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
            #endif
        }
        break;

        case SpecialFolder::HomeConfig:
        {
            Change( SpecialFolder::Home );

            // try ".config" and "AppData/Roaming" subdirectories.
            #if defined (__unix__)
                Change( A_CHAR(".config") );
            #elif defined(__APPLE__)
                Change( A_CHAR("Library/Preferences") );
            #elif defined(_WIN32)
                Change( String16(A_CHAR("AppData")) << DirectorySeparator << A_CHAR("Roaming") );
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
            #endif
        }
        break;


        case SpecialFolder::Module:
        {
            Path.Reset( ProcessInfo::Current().ExecFilePath );
        }
        break;

        case SpecialFolder::Temp:
        {

            if ( evaluatedTempDir.IsEmpty() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    if ( Directory::Exists( A_CHAR("/tmp") ) )
                        evaluatedTempDir.Reset( A_CHAR("/tmp") );

                #elif defined(__APPLE__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    macos::ALIB_APPLE_OC_NSTemporaryDirectory( evaluatedTempDir );
                    if ( evaluatedTempDir.IsEmpty() )
                    {
                        if ( Directory::Exists( A_CHAR("/tmp") ) )
                            evaluatedTempDir.Reset( A_CHAR("/tmp") );
                    }


                #elif defined(_WIN32)
                    NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    AString testDir;
                    if (     ( EnvironmentVariables::Get( A_CHAR("TMP") , testDir ) && Directory::Exists( testDir ) )
                         ||  ( EnvironmentVariables::Get( A_CHAR("TEMP"), testDir ) && Directory::Exists( testDir ) ) )
                    {
                        evaluatedTempDir= testDir;
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( evaluatedTempDir.IsEmpty() )
                {
                    createTempFolderInHomeDir( A_CHAR(".tmp"), evaluatedTempDir, reasonMsg );

                    // If this did not work, use home
                    if( evaluatedTempDir.IsEmpty() )
                    {
                        Change( SpecialFolder::Home );
                        evaluatedTempDir.Reset( Path );
                    }
                }

            }

            // set path to evaluated dir name
            Path.Reset( evaluatedTempDir );
        }
        break;

        case SpecialFolder::VarTemp:
        {
            if ( evaluatedVarTempDir.IsEmpty() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default folder \"/var/tmp\" could not be found.)";

                    if ( Directory::Exists( A_CHAR("/var/tmp") ) )
                        evaluatedVarTempDir.Reset( A_CHAR("/var/tmp") );
                #elif defined(__APPLE__)
                    const NString reasonMsg=  "(The default folder \"/private/var/tmp\" could not be found.)";

                    if ( Directory::Exists( A_CHAR("/private/var/tmp") ) )
                        evaluatedVarTempDir.Reset( A_CHAR("/private/var/tmp") );

                #elif defined(_WIN32)
                    const NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    AString testDir;
                    if (     ( EnvironmentVariables::Get( A_CHAR("TMP") , testDir ) && Directory::Exists( testDir ) )
                         ||  ( EnvironmentVariables::Get( A_CHAR("TEMP"), testDir ) && Directory::Exists( testDir ) ) )
                    {
                        evaluatedVarTempDir= testDir;
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( evaluatedVarTempDir.IsEmpty() )
                {
                    createTempFolderInHomeDir( A_CHAR(".var.tmp"), evaluatedVarTempDir, reasonMsg );

                    // If this did not work, use home
                    if( evaluatedVarTempDir.IsEmpty() )
                    {
                        Change( SpecialFolder::Home );
                        evaluatedVarTempDir.Reset( Path );
                    }
                }

            }
            // now path to evaluated dir name
            Path.Reset( evaluatedVarTempDir );
        }
        break;
    }
}

bool Directory::Change( const CString& path )
{
    // absolute addressing
    if( path.CharAtStart() == DirectorySeparator )
    {
        if( !Directory::Exists( path ) )
            return false;

        Path.Reset( path );
        return true;
    }


    // relative addressing
    integer origLength= Path.Length();
    Path._<false>( DirectorySeparator )
        ._( path );

    if( Directory::Exists( Path ) )
        return true;

    Path.ShortenTo( origLength );
    return false;
}

bool Directory::Change( const StringNZT& path )
{
    // absolute addressing
    if( path.CharAtStart() == DirectorySeparator )
    {
        integer origLength= Path.Length();
        Path._<false>( path ).Terminate();

        ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
        if( !Directory::Exists( CString(Path.Buffer() + origLength, path.Length() ) ) )
        ALIB_WARNINGS_RESTORE
        {
            Path.ShortenTo( origLength );
            return false;
        }

        Path.Reset( path );
        return true;
    }


    // relative addressing
    integer origLength= Path.Length();
    Path._<false>( DirectorySeparator )
        ._( path );

    if( Directory::Exists( Path ) )
        return true;

    Path.ShortenTo( origLength );
    return false;
}


// #################################################################################################
// Static methods
// #################################################################################################

bool Directory::CurrentDirectory( AString& target )         // static
{
    target.Reset();
    nchar charBuf[FILENAME_MAX];

    #if   defined(__GLIBCXX__) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
        if ( ! getcwd( charBuf, sizeof(charBuf ) ) )
    #elif defined ( _WIN32 )
        if ( !_getcwd( charBuf, sizeof(charBuf ) ) )
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
    {
        return false;
    }

    target._( static_cast<const nchar*>( charBuf ) );
    return true;
}

bool Directory::Exists( const CString& path )               // static
{
    #if defined (__GLIBC__) || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(path,nPath,1024)
        DIR* dir= opendir( nPath );
        if ( dir != nullptr )
        {
            closedir( dir );
            return true;
        }
        return false;

    #elif defined(_WIN32)

        #if !ALIB_CHARACTERS_WIDE
            DWORD dwAttrib = GetFileAttributesA( path );
        #else
            DWORD dwAttrib = GetFileAttributesW( path );
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

SystemErrors Directory::Create( const CString& path )       // static
{
    #if defined (__GLIBC__)  || defined(__APPLE__) || defined(__ANDROID_NDK__)
        ALIB_STRINGS_TO_NARROW(path,nPath,1024)
        int errCode= mkdir( nPath,    S_IRWXU | S_IRGRP | S_IROTH
                                    | S_IXGRP | S_IXOTH             );

        return SystemErrors(errCode);

    #elif defined(_WIN32)
        #if !ALIB_CHARACTERS_WIDE
            BOOL result= CreateDirectoryA( path, NULL );
        #else
            BOOL result= CreateDirectoryW( path, NULL );
        #endif


        if( result )
            return SystemErrors::OK;

        return SystemErrors( GetLastError() );
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}



}} // namespace [alib::lang::system]

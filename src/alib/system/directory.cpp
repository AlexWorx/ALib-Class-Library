// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#include "alib/alib.hpp"

#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
#   include "alib/system/directory.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_PROCESSINFO)
#   include "alib/system/process.hpp"
#endif
#if !defined (HPP_ALIB_SYSTEM_ENVIRONMENT)
#   include "alib/system/environment.hpp"
#endif

#if   defined(__GLIBCXX__)  || defined(__APPLE__)
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include <pwd.h>

#elif defined ( _WIN32 )
    #include <direct.h>
#else
    #pragma message ("Unknown Platform in file: " __FILE__ )
#endif

#if !defined (HPP_ALIB_COMPATIBILITY_STD_IOSTREAM)
    #include "alib/compatibility/std_iostream.hpp"
#endif

#include <fstream>


namespace aworx { namespace lib { namespace system {

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
    resultPath=   homeTemp.Path;

    // add given folder name and check if already exists
    homeTemp.Path._( DirectorySeparator )._( folderName );
    bool exists= Directory::Exists( homeTemp.Path );
    if( !exists )
    {
        if( Directory::Create( homeTemp.Path ) == SystemErrors::OK )
        {
            exists= true;
            NAString fileName( homeTemp.Path ); fileName._( DirectorySeparator )._( "readme.txt" );

            std::ofstream file ( fileName.ToCString() );
            if ( file.is_open() )
            {
                const ProcessInfo& pi= ProcessInfo::Current();
                file << "This folder was created by \"" << pi.CmdLine.ToCString()
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
        resultPath=   homeTemp.Path;
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
                if ( !system::GetEnvironmentVariable( ASTR("HOME"), Path ) )
                {
                    struct passwd* pwd = getpwuid(getuid());
                    if (pwd)
                        Path= pwd->pw_dir;
                    else
                        Path._( "~/" );
                }

            #elif defined(__APPLE__)
                macos::ALIB_APPLE_OC_NSHomeDirectory( Path );
                if ( Path.IsEmpty() )
                {
                    struct passwd* pwd = getpwuid(getuid());
                    if (pwd)
                        Path= pwd->pw_dir;
                    else
                        Path._( "~/" );
                }


            #elif defined(_WIN32)
                if ( !system::GetEnvironmentVariable( ASTR("USERPROFILE"), Path ) || !Directory::Exists( Path ) )
                {
                    system::GetEnvironmentVariable( ASTR("HOMEDRIVE"), Path );
                    system::GetEnvironmentVariable( ASTR("HOMEPATH" ), Path, CurrentData::Keep );
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
                Change( ASTR(".config") );
            #elif defined(__APPLE__)
                Change( ASTR("Library/Preferences") );
            #elif defined(_WIN32)
                Change( String16(ASTR("AppData")) << DirectorySeparator << ASTR("Roaming") );
            #else
                #pragma message ("Unknown Platform in file: " __FILE__ )
            #endif
        }
        break;


        case SpecialFolder::Module:
        {
            Path._()._( ProcessInfo::Current().ExecFilePath );
        }
        break;

        case SpecialFolder::Temp:
        {

            if ( evaluatedTempDir.IsEmpty() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    if ( Directory::Exists( ASTR("/tmp") ) )
                        evaluatedTempDir= ASTR("/tmp");

                #elif defined(__APPLE__)
                    NString reasonMsg=  "(The default temporary folder \"/tmp\" could not be found.)";
                    macos::ALIB_APPLE_OC_NSTemporaryDirectory( evaluatedTempDir );
                    if ( evaluatedTempDir.IsEmpty() )
                    {
                        if ( Directory::Exists( ASTR("/tmp") ) )
                            evaluatedTempDir= ASTR("/tmp");
                    }


                #elif defined(_WIN32)
                    NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    AString testDir;
                    if (     ( system::GetEnvironmentVariable( ASTR("TMP") , testDir ) && Directory::Exists( testDir ) )
                         ||  ( system::GetEnvironmentVariable( ASTR("TEMP"), testDir ) && Directory::Exists( testDir ) ) )
                    {
                        evaluatedTempDir= testDir;
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( evaluatedTempDir.IsEmpty() )
                {
                    createTempFolderInHomeDir( ASTR(".tmp"), evaluatedTempDir, reasonMsg );

                    // If this did not work, use home
                    if( evaluatedTempDir.IsEmpty() )
                    {
                        Change( SpecialFolder::Home );
                        evaluatedTempDir= Path;
                    }
                }

            }

            // set path to evaluated dir name
            Path= evaluatedTempDir;
        }
        break;

        case SpecialFolder::VarTemp:
        {
            if ( evaluatedVarTempDir.IsEmpty() )
            {
                #if defined (__unix__)
                    NString reasonMsg=  "(The default folder \"/var/tmp\" could not be found.)";

                    if ( Directory::Exists( ASTR("/var/tmp") ) )
                        evaluatedVarTempDir= ASTR("/var/tmp");
                #elif defined(__APPLE__)
                    const NString reasonMsg=  "(The default folder \"/private/var/tmp\" could not be found.)";

                    if ( Directory::Exists( ASTR("/private/var/tmp") ) )
                        evaluatedVarTempDir= ASTR("/private/var/tmp");

                #elif defined(_WIN32)
                    const NString reasonMsg=  "(Environment variables TMP and TEMP either not set or not containing valid paths.)";
                    AString testDir;
                    if (     ( system::GetEnvironmentVariable( ASTR("TMP") , testDir ) && Directory::Exists( testDir ) )
                         ||  ( system::GetEnvironmentVariable( ASTR("TEMP"), testDir ) && Directory::Exists( testDir ) ) )
                    {
                        evaluatedVarTempDir= testDir;
                    }
                #else
                    #pragma message ("Unknown Platform in file: " __FILE__ )
                #endif


                if( evaluatedVarTempDir.IsEmpty() )
                {
                    createTempFolderInHomeDir( ASTR(".var.tmp"), evaluatedVarTempDir, reasonMsg );

                    // If this did not work, use home
                    if( evaluatedVarTempDir.IsEmpty() )
                    {
                        Change( SpecialFolder::Home );
                        evaluatedVarTempDir= Path;
                    }
                }

            }
            // now path to evaluated dir name
            Path= evaluatedVarTempDir;
        }
        break;
    }
}

bool Directory::Change( const String& path )
{
    integer origLength= Path.Length();
    Path._<false>( DirectorySeparator )
        ._( path );

    if( Directory::Exists( Path ) )
        return true;

    Path.SetLength<false>( origLength );
    return false;
}

// #################################################################################################
// Static methods
// #################################################################################################

bool Directory::CurrentDirectory( AString& target )
{
    target.Clear();
    char charBuf[FILENAME_MAX];

    #if   defined(__GLIBCXX__) || defined(__APPLE__)
        if ( !getcwd( charBuf, sizeof(charBuf ) ) )
    #elif defined ( _WIN32 )
        if ( !_getcwd( charBuf, sizeof(charBuf ) ) )
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
    {
        return false;
    }

    charBuf[ sizeof(charBuf) - 1 ] = '\0'; // not really required, but who knows

    target._( static_cast<const char*>( charBuf ) );
    return true;
}

bool Directory::Exists( const TString& path )
{
    #if defined (__GLIBC__) || defined(__APPLE__)
        ALIB_STD_TO_NARROW_TSTRING(path,nPath)
        DIR* dir= opendir( nPath.ToCString() );
        if ( dir != nullptr )
        {
            closedir( dir );
            return true;
        }
        return false;

    #elif defined(_WIN32)

        #if ALIB_NARROW_STRINGS
            DWORD dwAttrib = GetFileAttributesA( path.ToCString() );
        #else
            DWORD dwAttrib = GetFileAttributesW( path.ToCString() );
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

SystemErrors Directory::Create( const TString& path )
{
    #if defined (__GLIBC__)  || defined(__APPLE__)
        ALIB_STD_TO_NARROW_TSTRING(path,nPath)
        int errCode= mkdir( nPath.ToCString(), S_IRWXU | S_IRGRP | S_IROTH
                                                       | S_IXGRP | S_IXOTH  );

        return SystemErrors(errCode);

    #elif defined(_WIN32)
        #if ALIB_NARROW_STRINGS
            BOOL result= CreateDirectoryA( path.ToCString(), NULL );
        #else
            BOOL result= CreateDirectoryW( path.ToCString(), NULL );
        #endif


        if( result )
            return SystemErrors::OK;

        return SystemErrors( GetLastError() );
    #else
        #pragma message ("Unknown Platform in file: " __FILE__ )
    #endif
}



}}}// namespace [aworx::lib::system]


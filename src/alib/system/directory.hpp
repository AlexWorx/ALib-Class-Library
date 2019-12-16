/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_system of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_SYSTEM_DIRECTORY
#define HPP_ALIB_SYSTEM_DIRECTORY 1

#if !defined(HPP_ALIB_SYSTEM_SYSTEM)
#   include "alib/system/system.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_STRINGNZT)
#   include "alib/strings/stringnzt.hpp"
#endif


namespace aworx { namespace lib { namespace system {

// #############################################################################################
// MacOS System call wrappers (implemented in Objective-C)
// #############################################################################################
#if defined(__APPLE__)
namespace macos
{
    void ALIB_APPLE_OC_NSTemporaryDirectory( AString& target);
    void ALIB_APPLE_OC_NSHomeDirectory     ( AString& target);
}
#endif


/** ************************************************************************************************
 *  Represents a directory (folder) in a file system.
 **************************************************************************************************/
class Directory
{
    // #############################################################################################
    // Types
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Denotes upper and lower case character conversions
         ******************************************************************************************/
        enum class SpecialFolder
        {
            /** The root directory . */
            Root,

            /** The current directory of the process. */
            Current,

            /** The user's home directory */
            Home,

            /**
             * The user's configuration directory. This is evaluated as follows:
             * - Using environment variable \e HOME (under WindowsOS a combination of \e HOMEDRIVE
             *   and \e HOMEPATH), the home directory is determined.
             * - If within this a directory \e ".config" exists, it is used, otherwise
             * - if within this the directories \e "AppData/Roaming" exist, this is used.
             */
            HomeConfig,

            /** The directory of the executable of the process. */
            Module,

            /**
             * A directory to be used for creation of temporary files.
             *
             * - On GNU/Linux OS this defaults to <c>/tmp</c>.
             * - On Windows OS, environment variables TMP and TEMP are evaluated.<br>
             *
             * If the directory does not exist, then (on all OS), a new directory named \c ".tmp"
             * is created in the user's home directory and returned (if not existent already).
             * If this fails, the home directory itself is returned.
             *
             * \note With the potential creation of the directory \c ".tmp" in the user's home
             *       directory, a small \c readme.txt file is created containing  the name of
             *       the running application and the reason of the creation.
             *
             * To overrule this behavior, public static variable #evaluatedTempDir
             * may be set arbitrarily prior to using this enum value.
             */
            Temp,

            /**
             * A directory to be used for creation of temporary files that survives reboots of the
             * host machine.
             *
             * - On GNU/Linux OS this defaults to <c>/var/tmp</c>.
             * - On Windows OS, environment variables TMP and TEMP are evaluated (same as
             *   with \b %SpecialFolder::Temp).<br>
             *
             * If the directory does not exist, then (on all OS), a new directory named \c ".var.tmp"
             * is created in the user's home directory and returned (if not existent already).
             * If this fails, the home directory itself is returned.
             *
             * \note With the potential creation of the directory \c ".var.tmp" in the user's home
             *       directory, a small \c readme.txt file is created, containing the name of
             *       the running application and the reason of the creation.
             *
             * To overrule this behavior, public static variable #evaluatedVarTempDir
             * may be set arbitrarily prior to using this enum value.
             */
            VarTemp,
        };

    // #############################################################################################
    // Fields
    // #############################################################################################
    public:
        /**
         * Singleton containing the path for the use of enum value
         * \ref SpecialFolder "SpecialFolder::Temp".
         * This is evaluated once with the first use of \b %SpecialFolder::Temp.
         * To change the default behavior of evaluation, this variable may be filled with a proper
         * path prior to using enum \b %SpecialFolder::Temp.
         */
        static      AString         evaluatedTempDir;

        /**
         * Singleton containing the path for the use of enum value
         * \ref SpecialFolder "SpecialFolder::VarTemp".
         * This is evaluated once with the first use of \b %SpecialFolder::VarTemp.
         * To change the default behavior of evaluation, this variable may be filled with a proper
         * path prior to using enum \b %SpecialFolder::VarTemp.
         */
        static      AString         evaluatedVarTempDir;

        /** The path of the directory represented by this instance */
        AString                     Path;

    // #############################################################################################
    // Constructors
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an object representing one of the known special directories.
         * @param special  The special directory to initialize this instance to.
         ******************************************************************************************/
                                Directory( SpecialFolder special )
        {
            Change( special );
        }

        /** ****************************************************************************************
         * Constructs an object representing the given path.
         * @param path  The initial path. If this is a relative path, it relates to the processes
         *              current directory.
         ******************************************************************************************/
                                Directory( const String& path )
        {
            Path._( '/' );
            Change( path );
        }


    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Changes the directory. If the given path is relative (e.g. a name of a sub directory
         * or ".."), such path is added to the current #Path. Otherwise, field #Path is replaced.
         * If the resulting destination directory is not valid, \c false is returned and the
         * field #Path stays intact.
         * @param    path  The relative or absolute path to change to.
         * @returns \c true if the change was successful, \c false otherwise.
         ******************************************************************************************/
        ALIB_API   bool         Change( const CString& path );

        /** ****************************************************************************************
         * Overloaded version of #Change(const CString&) accepting a non-zero-terminated string.
         * @param    path  The relative or absolute path to change to.
         * @returns \c true if the change was successful, \c false otherwise.
         ******************************************************************************************/
        ALIB_API   bool         Change( const StringNZT& path );

        /** ****************************************************************************************
         * Changes the directory to one of the known special directories.
         * @param special  The special directory to change this instance to.
         ******************************************************************************************/
        ALIB_API   void         Change( SpecialFolder special );


    // #############################################################################################
    // static interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         *  Returns the actual directory in the file system associated with the current process.
         *  @param target The target AString to store the directory. Will be cleared in any case
         *  @return \c true on success, \c false on failure.
         ******************************************************************************************/
        ALIB_API
        static  bool           CurrentDirectory( AString& target );

        /** ****************************************************************************************
         *  Tests if the given (absolute or relative) path represents a directory in the file system.
         *  @param path     Zero-terminated string defining the path to test.
         *  @return \c true if a directory was found.
         ******************************************************************************************/
        ALIB_API
        static  bool           Exists( const CString&   path );

        /** ****************************************************************************************
         * Overloaded version of #Change(const CString&) accepting a non-zero-terminated string.
         * @param    path  The relative or absolute path to change to.
         * @returns \c true if the change was successful, \c false otherwise.
         ******************************************************************************************/
        static  bool           Exists( const StringNZT& path )
        {
            return Exists( String256(path) );
        }

        /** ****************************************************************************************
         *  Creates the directory of the given (absolute or relative) path.
         *  @param path     Zero-terminated string defining the path to test.
         *  @return A value of enum type \alib{system,SystemErrors}.
         ******************************************************************************************/
        ALIB_API
        static SystemErrors    Create( const CString& path );
}; //class Directory

}} // namespace lib::system

/// Type alias in namespace #aworx.
using     Directory=       lib::system::Directory;

/**
 * The standard path separator character. Defaults to '\\' on Windows OS, '/' else.
 * Note: Available only with including "alib/system/directory.hpp"
 */
constexpr nchar    DirectorySeparator
#if !defined(ALIB_DOX)
               #if defined( _WIN32 )
                                       = '\\';
               #else
                                       = '/';
               #endif
#else
    ;
#endif
}  // namespace [aworx]

ALIB_ENUMS_ASSIGN_RECORD( aworx::Directory::SpecialFolder, aworx::lib::enums::ERSerializable )
#endif // HPP_ALIB_SYSTEM_DIRECTORY

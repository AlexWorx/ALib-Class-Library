//==================================================================================================
/// \file
/// This header-file is part of module \alib_system of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace  system {

/// The character type that this \alibmod_nl uses for String representations.
/// This is taken from the C++ standard library.
using PathCharType  = std::filesystem::path::value_type;

#if !defined(ALIB_PATH_CHARACTERS_WIDE)
#   if defined(_WIN32)
       static_assert(std::same_as<PathCharType, characters::wchar>,
           "Compiler symbol ALIB_PATH_CHARACTERS_WIDE wrongly detected: 1 but std::filesystem::path::value_type does not equal characters::wchar.");
#   else
       static_assert(std::same_as<PathCharType, characters::nchar>,
           "Compiler symbol ALIB_PATH_CHARACTERS_WIDE wrongly detected: 0 but std::filesystem::path::value_type does not equal characters::nchar.");
#   endif
#else
#   if ALIB_PATH_CHARACTERS_WIDE
       static_assert(std::same_as<PathCharType, characters::wchar>,
       "Given compiler-symbol ALIB_PATH_CHARACTERS_WIDE mismatch: Given 1 but std::filesystem::path::value_type does not equal characters::wchar.");
#   else
       static_assert(std::same_as<PathCharType, characters::nchar>,
       "Given compiler-symbol ALIB_PATH_CHARACTERS_WIDE mismatch: Given 0 but std::filesystem::path::value_type does not equal characters::nchar.");
#   endif
#endif

/// The string-type used with this \alibmod_nl.
using PathString     = strings::TString<PathCharType>;

/// The string-type used with this \alibmod_nl.
using CPathString    = strings::TCString<PathCharType>;

/// The string-type used with this \alibmod_nl.
using PathSubstring  = strings::TSubstring<PathCharType>;

#if ALIB_MONOMEM
/// A mono-allocated string representing a path.
using PathStringMA   = strings::TAString<PathCharType, PoolAllocator >;

/// A pool-allocated string representing a path.
using PathStringPA   = strings::TAString<PathCharType, PoolAllocator >;
#endif

#if DOXYGEN
/// A nulled path string.
inline constexpr PathString NULL_PATH;

/// An empty path string.
inline constexpr PathString EMPTY_PATH;
#else
inline constexpr PathString NULL_PATH(nullptr);
inline constexpr PathString EMPTY_PATH(A_PATH(""));
#endif


#if !defined( _WIN32 ) || DOXYGEN
     /// The standard path separator character. Defaults to '\\' on Windows OS, '/' else.
    inline constexpr PathCharType    DIRECTORY_SEPARATOR = '/';
#else
    inline constexpr PathCharType    DIRECTORY_SEPARATOR = '\\';
#endif

//##################################################################################################
// MacOS System call wrappers (implemented in Objective-C)
//##################################################################################################
#if defined(__APPLE__)
class Path;
namespace macos
{
    void ALIB_APPLE_OC_NSTemporaryDirectory( Path& target );
    void ALIB_APPLE_OC_NSHomeDirectory     ( Path& target );
}
#endif

//==================================================================================================
/// Enumerates special system folders like "home", "temp", "config", etc.
/// This enumeration is primarily used with class \alib{system;Path} and this documentation
/// refers to this type.
//==================================================================================================
enum class SystemFolders
{
    /// The root directory .
    Root,

    /// The current directory of the process.
    Current,

    /// The user's home directory
    Home,

    /// The user's configuration directory. This is evaluated as follows:
    /// - Using environment variable \e HOME (under WindowsOS a combination of \e HOMEDRIVE
    ///   and \e HOMEPATH), the home directory is determined.
    /// - If within this directory \e ".config" exists, it is used, otherwise
    /// - if within this direcotory the directories \e "AppData/Roaming" exist, this is used.
    HomeConfig,

    /// The directory of the executable of the process.
    Module,

    /// A directory to be used for creation of temporary files.
    ///
    /// - On GNU/Linux OS this defaults to <c>/tmp</c>.
    /// - On Windows OS, environment variables "TMP" and "TEMP" are evaluated.<br>
    ///
    /// If the directory does not exist, then (on all OS), a new directory named \c ".tmp"
    /// is created in the user's home directory and returned (if not existent already).
    /// If this fails, the home directory itself is returned.
    ///
    /// \note With the potential creation of the directory \c ".tmp" in the user's home
    ///       directory, a small \c readme.txt file is created containing  the name of
    ///       the running application and the reason of the creation.
    ///
    /// To overrule this behavior, public static variable
    /// \alib{system;Path::tempDirEvaluatedOnce} may be set arbitrarily before using this
    /// enum value.
    Temp,

    /// A directory to be used for creation of temporary files that survives reboots of the
    /// host machine.
    ///
    /// - On GNU/Linux OS this defaults to <c>/var/tmp</c>.
    /// - On Windows OS, environment variables "TMP" and "TEMP" are evaluated (same as
    ///   with \b %SystemFolders::Temp).<br>
    ///
    /// If the directory does not exist, then (on all OS), a new directory named \c ".var.tmp"
    /// is created in the user's home directory and returned (if not existent already).
    /// If this fails, the home directory itself is returned.
    ///
    /// \note With the potential creation of the directory \c ".var.tmp" in the user's home
    ///       directory, a small \c readme.txt file is created, containing the name of
    ///       the running application and the reason of the creation.
    ///
    /// To overrule this behavior, public static variable
    /// \alib{system;Path::varTempDirEvaluatedOnce} may be set arbitrarily before using this
    /// enum value.
    VarTemp,
};


//==================================================================================================
/// Represents a directory or file in a file system.
///
/// Inherits a local string buffer fixed to size 256. The flexible nature of \alib
/// \alib{strings;TLocalString;local strings} allows dynamic allocation if this should be exceeded
/// (which usually is not expected with paths).
///
/// The character type of templated parent class \b AString is defined by the type definition
/// \alib{system::PathCharType}, which in turn aliases <c>std::filesystem::path::value_type</c>.
//==================================================================================================
class Path : public  strings::TLocalString<PathCharType, 256>
{
  //################################################################################################
  // Fields
  //################################################################################################
  public:
        /// Singleton containing the path for the use of enum value
        /// \alib{system;SystemFolders;SystemFolders::Temp}.
        /// This is evaluated once with the first use of \b %SystemFolders::Temp.
        /// To change the default behavior of evaluation, this variable may be filled with a proper
        /// path before using enum \b %SystemFolders::Temp with this class.
        ///
        /// The one-time evaluated value is stored in the \alib{monomem;GLOBAL_ALLOCATOR} and
        /// access to this variable is consequently protected with \alib{monomem;GLOBAL_ALLOCATOR_LOCK}.
        static      PathString          tempDirEvaluatedOnce;

        /// Singleton containing the path for the use of enum value
        /// \alib{system;SystemFolders;SystemFolders::VarTemp}".
        /// This is evaluated once with the first use of \b %SystemFolders::VarTemp.
        /// To change the default behavior of evaluation, this variable may be filled with a proper
        /// path before using enum \b %SystemFolders::VarTemp with this class.
        ///
        /// The one-time evaluated value is stored in the \alib{monomem;GLOBAL_ALLOCATOR} and
        /// access to this variable is consequently protected with \alib{monomem;GLOBAL_ALLOCATOR_LOCK}.
        static      PathString          varTempDirEvaluatedOnce;

  //################################################################################################
  // Constructors
  //################################################################################################
  public:
    using TLocalString<PathCharType, 256>::TLocalString;

    /// Constructs an object representing no directory.
    /// Field #Path remains empty.
    Path()                                                                                        {}

    /// Constructs an object representing one of the known special directories.
    /// @param special  The special directory to initialize this instance to.
    Path( SystemFolders special )                                             { Change( special ); }

    /// Constructs an object representing one of the known special directories.
    /// @param special  The special directory to initialize this instance to.
    /// @param fileName The filename in folder \p{special}.
    Path( SystemFolders special, const PathString& fileName )  { Change( special ); Add(fileName); }

    /// Constructs an object representing the given path.
    /// @param path  The initial path.
    Path( const PathString& path )
    : strings::TLocalString<PathCharType, 256>(path)                                              {}


  //################################################################################################
  // Interface
  //################################################################################################
  public:
    /// Creates the directory of the given \p{path}. If a relative path is given, then
    /// it is appended to the currently stored path, otherwise the current path is replaced.
    /// @param path     Zero-terminated string defining the path to test.
    /// @return A value of enum type \alib{system;SystemErrors}.
    ALIB_DLL
    SystemErrors            Create( const PathString& path );

    /// Creates the directory represented by this object by invoking #Create(const PathString&)
    /// passing an empty String.
    /// @return A value of the enum type \alib{system;SystemErrors}.
    ALIB_DLL
    SystemErrors            Create()                                  { return Create(EMPTY_PATH); }

    /// Changes the directory. If the given path is relative (e.g., a name of a subdirectory
    /// or ".."), such path is added to the current #Path. Otherwise, this objects' path
    /// string is replaced.
    /// If the resulting destination directory is not valid, \c false is returned and this
    /// object is not changed.
    /// @param    path  The relative or absolute path to change to.
    /// @returns \c true if the change was successful, \c false otherwise.
    ALIB_DLL   bool         Change( const PathString& path );

    /// Changes the directory to one of the known special directories.
    /// @param special  The special directory to change this instance to.
    /// @returns \c true if the change was successful, \c false otherwise.
    ALIB_DLL   bool         Change( SystemFolders special );

    /// Changes the directory to one of the known special directories and adds the given
    /// \p{fileName} to this path.
    /// @param special  The special directory to change this instance to.
    /// @param fileName The filename within \p{special}.
    inline    void          Change( SystemFolders special, const PathString& fileName )
    {
        Change( special );
        Add(fileName);
    }

    /// In the case of directories, this path is set to the parent directory.
    /// In the case of files, the file name is removed from the path.
    ///
    /// If the resulting destination directory is not valid, \c false is returned and this
    /// object is not changed.
    /// @returns \c true if the change was successful, \c false otherwise.
    ///          I.e., in case this object represented the root folder \c false is returned.
    ALIB_DLL   bool         ChangeToParent();


    /// Adds a directory or filename to the internal string. If the current string does not
    /// end with a \alib{system;DIRECTORY_SEPARATOR}, one will be inserted first.
    ///
    /// @param dirOrFilename The path component to add.
    /// @return \c *this to allow concatenated operations.
    inline    Path&         Add( const PathString& dirOrFilename ) {
        if( CharAtEnd() != DIRECTORY_SEPARATOR )
            _(DIRECTORY_SEPARATOR);
        _(dirOrFilename);
        return *this;
    }

    /// Adds this processes' module name to the currently stored folder path.
    ///
    /// @param extension  An additional extension string that is added to the evaluated file
    ///                   name, for example, <c>".jpg"</c>.
    ALIB_DLL   void         AddModuleName( const PathString& extension );

    /// Tests if this path represents an existing directory in the file system.
    /// @return \c true if a directory was found, \c false if not found or the path represents
    ///         a file.
    ALIB_DLL
    bool                    IsDirectory();

    /// Invokes static overload of this method, passing this object.
    /// @returns The result of a call to #IsAbsolute(const PathString&).
    int                     IsAbsolute()                         const { return IsAbsolute(*this); }

    /// Gets the name portion of the path.
    /// @returns A string containing the characters after the last directory separator.
    PathString              Name()                                                           const {
        auto idx= LastIndexOf(DIRECTORY_SEPARATOR);
        if( idx < 0 )
            return *this;
        return Substring(idx + 1);
    }

    /// In the case of directories, the path string to the parent directory is returned.
    /// In the case of files, the directory that the file resides in is returned.
    /// @returns A string containing the characters from the start to the last directory
    ///          separator. If the currently stored string ends with a separator, this is
    ///          returned first.
    PathString              Parent()                                                         const {
        integer startIdx= length;
        if ( CharAtEnd() == DIRECTORY_SEPARATOR  ) {
            if ( startIdx == 1 )
                return *this;
            --startIdx;
        }

        integer lastDirSep= LastIndexOf(DIRECTORY_SEPARATOR, startIdx);
        if (lastDirSep==0)
            ++lastDirSep;
        return Substring(0, lastDirSep );
    }

    /// Resolves the path by removing all symbolic links and relative addressing.
    /// @return \alib{system;SystemErrors;SystemErrors::OK} on success, otherwise
    ///         an error code.
    ALIB_DLL
    SystemErrors            MakeReal();

    /// Determines if the given string contains a path/filename with absolute addressing or
    /// not.
    /// \attention This method must be invoked on "real path strings" only. Assembled paths
    ///            which contain redundant slashes return a false result.
    /// @param    path  The relative or absolute path to check.
    /// @returns \c 0 if \p{path} is relative. If absolute, the position of the first character
    ///          that does not belong to the root symbol. For example, with windows OS, if
    ///          <c>"C:\XYZ"</c> was given, \c 3 is returned.
    ///          With other OSes "//" is searched.
    static  int             IsAbsolute( const PathString& path ) {
        #if defined(_WIN32)
           if( path.Length() >= 3 ) {
                if(    path.CharAt<NC>(1) == ':'
                    && path.CharAt<NC>(2)== DIRECTORY_SEPARATOR    )
                    return 3;

                if (   path.CharAt<NC>(0) == DIRECTORY_SEPARATOR
                    && path.CharAt<NC>(1) == DIRECTORY_SEPARATOR)
                    return int(path.IndexOf<NC>( DIRECTORY_SEPARATOR, 2 ) + 1);
           }
           return 0;
        #else
            if( path.CharAtStart() == '/' )
                return 1;
            auto slashSlashPos= path.IndexOf(A_PATH("//"));
            if( slashSlashPos > 0 )
                return int(slashSlashPos + 2);
            return 0;
        #endif
    }

}; //class Path

// Note(25/01/17):
//   Clang strangely did not find the following templated operators when they resided in an
//   exported namespace.
//   The workaround was to not export the namespace but export each operator instead.
//   We think this is wrong behavior and not aligned with the language specification.
#if !DOXYGEN
}} namespace alib::system {

ALIB_EXPORT
inline bool operator== (const Path& lhs, const Path& rhs)
{ return  lhs. template Equals   <CHK, lang::Case::Sensitive>(rhs); }

ALIB_EXPORT
inline auto operator<=> (const Path& lhs, const Path& rhs)
{ return  lhs. template CompareTo<CHK, lang::Case::Sensitive>(rhs); }


} ALIB_EXPORT namespace alib { namespace system {
#endif // DOXYGEN

} // namespace alib[::system]

/// Type alias in namespace \b alib.
using     Path=             system::Path;

/// Type alias in namespace \b alib.
using     SystemFolders=    system::SystemFolders;


//##################################################################################################
// Specializations of ZTArrayTraits for  class Path
//##################################################################################################
#if !DOXYGEN
namespace characters {
template<>
struct  ZTArrayTraits<system::Path, system::PathCharType>
{
    static constexpr Policy                      Access                          = Policy::Implicit;
    static constexpr Policy                      Construction                    = Policy::NONE;
    static constexpr const system::PathCharType* Buffer(const system::Path& src) { return src.Terminate(); }
    static constexpr integer                     Length(const system::Path& src) { return src.Length(); }
};
}// namespace alib[::characters]
#endif //!DOXYGEN

}  // namespace [alib]

ALIB_ENUMS_ASSIGN_RECORD(   alib::system::SystemFolders, alib::enumrecords::ERSerializable )
ALIB_BOXING_VTABLE_DECLARE( alib::system::Path*        , vt_system_path              )

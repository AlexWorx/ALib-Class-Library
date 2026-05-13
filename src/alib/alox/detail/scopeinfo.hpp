//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace lox { namespace detail {

//==================================================================================================
/// Encapsulates information of the caller that can be collected. This is platform-specific, in
/// this case, C++. What we receive from the C++ preprocessor is the source file name
/// of the calling code, the line number within the source file name, and the name of the method
/// the call is placed in.
/// Furthermore, we collect the <c>std::type_info*</c> in case the call was not placed in a
/// static or namespace function.
///
/// The method has a list of interface functions that gets such source information and some derived
/// variants of it. In addition, thread and timer information is stored and managed.
///
/// As far as possible, 'lazy' techniques come to practice with this class. This means, only
/// the things that are queried in-between to invocations of method #"Set" are calculated.
///
/// ## Friends ##
/// class #"Lox"
//==================================================================================================
class ScopeInfo {
  public:
#if !ALIB_SINGLE_THREADED
    /// A map we use to translate thread IDs to thread names
    using ThreadDictionary=   HashMap<MonoAllocator, threads::ThreadID, String32>;
#endif

  //################################################################################################
  // Protected fields
  //################################################################################################
  protected:
  #if !DOXYGEN
    friend struct LoxImpl;
    friend void  LI::MapThreadName(LoxImpl*, const String&, threads::ThreadID);
    friend void  LI::GetState( LoxImpl*, NAString&, StateInfo);
  #endif

    /// Defines how source paths names are to be trimmed. \alox allows a set of global rules and
    /// a set that is 'local', hence specific to a #"%Lox".
    struct SourcePathTrimRule {
        NAString            Path;            ///< The path string
        NAString            TrimReplacement; ///< Optional replacement string for trimmed paths.
        int                 TrimOffset;      ///< Additional offset of the trim position
        lang::Inclusion     IncludeString;   ///< Denotes if #"Path" itself should be included when trimmed
        lang::Case          Sensitivity;     ///< The sensitivity of the comparison when trimming
        variables::Priority Priority;        ///< The priority of the rule. Depends on origin: source code, config...)
        bool                IsPrefix;        ///< \c true if path was not starting with <c>'*'</c>, when provided.
    };

    /// List of trim definitions for portions of source paths to be ignored.
    ALIB_DLL static
    std::vector<SourcePathTrimRule>  GlobalSPTRs;

    /// List of trim definitions for portions of source paths to be ignored.
    std::vector<SourcePathTrimRule>  LocalSPTRs;

    /// Flag to determine if global rules have been read from config already.
    ALIB_DLL static
    bool                             GlobalSPTRsReadFromConfig;

    /// If true, next time a source path cannot be trimmed successfully with custom
    /// trim information provided with
    /// #"alib::lox::Lox::SetSourcePathTrimRule;Lox::SetSourcePathTrimRule"
    /// some trim information is automatically created by comparing such source file's path
    /// with the path of the executable of the current process.
    bool                                    AutoDetectTrimableSourcePath                      =true;

  #if !ALIB_SINGLE_THREADED
    /// The C++ native ID.
    std::thread::id                         threadNativeIDx;

    /// The thread passed with #"Set".
    Thread*                                 thread                                         =nullptr;

    /// The name of the thread that executed the log.
    String                                  threadName;

    /// Dictionary to translate thread IDs into something maybe nicer/shorter.
    /// The dictionary may be filled by the user of the library using #"Lox.MapThreadName".
    ThreadDictionary                        threadDictionary;
  #endif

    /// Information of a single source file.
    /// Stored in the field #"ScopeInfo::parsedFileNameCache;2".
    struct ParsedFileName {
        /// Path and name of source file (given by the C++ preprocessor).
        NCString        origFile;

        /// Full path of source file (evaluated).
        NString         fullPath                                                           =nullptr;

        /// Trimmed path of source file (evaluated).
        NString         trimmedPath                                                        =nullptr;

        /// Prefix for the trimmed path taken from trim rule. Has to be added on writing
        /// the trimmed path *
        NString         trimmedPathPrefix                                                  =nullptr;

        /// File name (evaluated).
        NString         name                                                               =nullptr;

        /// File name without extension (evaluated).
        NString         nameWOExt                                                          =nullptr;

        /// Index of the last path separator in #".origFile".
        integer         origFilePathLength                                                      =-2;

        /// Constructor.
        /// @param filename Stored in #".origFile".
        ParsedFileName( const NCString  filename) : origFile(filename)                            {}
    };

    /// Serves as template parameter \p{TValueDescriptor} of the field
    /// #"ScopeInfo::parsedFileNameCache;2".
    struct ValueDescriptorPFN : containers::TSubsetKeyDescriptor<ParsedFileName, NCString> {
        /// Provides access to the key-portion of the cached set.
        /// @param src  The cached element.
        /// @return The key portion of the element.
        NCString            Key   (ParsedFileName& src)               const { return src.origFile; }
    };

    /// Least recently used cache of parsed file name.
    LRUCacheTable< MonoAllocator, ValueDescriptorPFN, std::hash<NString>>   parsedFileNameCache;

    /// Holds values for the current scope. Because recursive logging might occur (e.g., when
    /// parameters rely on method invocations which incorporate log statements), objects of this
    /// class are stored in stack #"ScopeInfo::callStack".
    struct FrameRecord {
        /// Time of the call that created this record.
        Ticks                   timeStamp;

        /// The entry from the #"ScopeInfo::parsedFileNameCache;2".
        ParsedFileName*         Parsed;

        /// Line number within the source file (given by the C++ preprocessor).
        int                     origLine;

        /// Function/method name (given by the C++ preprocessor).
        NCString                origMethod;

        /// Type information. Nullptr if call from static or global function.
        const std::type_info*   typeInfo;
    };

    /// A stack of scopes (allows recursive calls/nested logging).
    StdVectorMA<FrameRecord>    callStack;

    /// The current depth of recursive invocations.
    int                         callStackSize                                                   =-1;

  //################################################################################################
  // Types exposed
  //################################################################################################
  public:
    /// The name of the Lox we are attached to.
    NString                     loxName;


  //################################################################################################
  // Constructor
  //################################################################################################
  public:
    /// Constructs a scope info.
    /// @param name       The name of the Lox that this object belongs to.
    ///                   Will be converted to upper case.
    /// @param allocator  The monotonic allocator of "our" #"%Lox", used for long-term allocations
    ALIB_DLL
    ScopeInfo( const NString& name, MonoAllocator& allocator);

 //#################################################################################################
 // public interface
 //#################################################################################################
  public:
    /// Changes the capacity of the #"%LRUCacheTable" for parsed file names by calling
    /// #"LRUCacheTable::Reserve;*".
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    void SetFileNameCacheCapacity( integer numberOfLists, integer entriesPerList )
    { parsedFileNameCache.Reserve( numberOfLists, entriesPerList ); }

    /// Stores C++ specific caller parameters and some other values like the time stamp.
    /// Also, flags thread information is not received, yet.
    /// Counts the recursion counter up.
    /// @param ci   The caller information.
    ALIB_DLL
    void Set( const lang::CallerInfo& ci  );

    /// Releases latest scope information.
    void PopNestedScope()            { --callStackSize; ALIB_ASSERT( callStackSize >= -1 , "ALOX") }


    /// Implements for #"Lox::SetSourcePathTrimRule;2" and #"Lox::ClearSourcePathTrimRules;2".
    /// @param path            The path to search for. If not starting with <c> '*'</c>,
    ///                        a prefix is searched.
    /// @param includeString   Determines if \p{path} should be included in the trimmed path or not.
    /// @param trimOffset      Adjusts the portion of \p{path} that is trimmed. 999999 to clear!
    /// @param sensitivity     Determines if the comparison of \p{path} with a source file's path
    ///                        is performed case-sensitive or not.
    /// @param trimReplacement Replacement string for trimmed portion of the path.
    /// @param reach           Denotes whether the rule is applied locally (to this #"%Lox" only)
    ///                        or applies to all instances of class #"%Lox".
    ///                        Defaults to #"%Reach::Global".
    /// @param priority        The priority of the setting.
    ALIB_DLL
    void      SetSourcePathTrimRule( const NCString&    path,
                                     lang::Inclusion    includeString,
                                     int                trimOffset,
                                     lang::Case         sensitivity,
                                     const NString&     trimReplacement,
                                     lang::Reach        reach,
                                     Priority           priority                );

    /// Receives the name of the #"%Lox" we are belonging to (this is a 1:1 relationship).
    /// @return The name of the #"%Lox".
    const NString   GetLoxName()                                                 { return loxName; }

    /// Receives the original filename and path of the source file
    /// (usually provided by the preprocessor).
    /// @return The full path and filename of the source file.
    const NCString& GetOrigFile()      { return callStack[size_t(callStackSize)].Parsed->origFile; }

    /// Receives the path of the source file (not trimmed, see #".GetTrimmedPath").
    /// @return The path of the source file.
    const NString   GetFullPath() {
        auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
        if( srcFile.fullPath.IsNull() ) {
            integer idx= getPathLength();
            if( idx >= 0 )
                srcFile.fullPath= NString( srcFile.origFile.Buffer(), idx );
            else
                srcFile.fullPath= "";
        }

        return srcFile.fullPath;
    }

    /// Writes the path of the source file, trimmed according to trim-information provided
    /// with #".SetSourcePathTrimRule" or detected according to #".AutoDetectTrimableSourcePath".
    /// @param target The target string to append the trimmed path to.
    void            GetTrimmedPath( AString& target ) {
        auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
        if( srcFile.trimmedPath.IsNull() )
            trimPath();

        target._( srcFile.trimmedPathPrefix )
              ._( srcFile.trimmedPath );
    }

    /// Receives the source file name excluding the path (usually provided by the preprocessor).
    /// @return The source file name excluding the path
    const NString   GetFileName() {
        auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
        if( srcFile.name.IsNull() ) {
            integer idx= getPathLength();
            if( idx >= 0 )
                srcFile.name= NString( srcFile.origFile.Buffer() + idx + 1,
                                       srcFile.origFile.Length() - idx - 1 );
            else
                srcFile.name= "";
        }
        return srcFile.name;
    }

    /// Receives the source file name excluding the path and without an extension
    /// (usually provided by the preprocessor).
    /// @return The source file name excluding the path and extension.
    const NString   GetFileNameWithoutExtension() {
        auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
        if( srcFile.nameWOExt.IsNull() ) {
            srcFile.nameWOExt= GetFileName();
            integer lastDot=   srcFile.nameWOExt.LastIndexOf( '.' );
            if ( lastDot > 0 )
                srcFile.nameWOExt= NString( srcFile.nameWOExt.Buffer(), lastDot );
        }
        return srcFile.nameWOExt;
    }


    /// Receives the method name (usually provided by the preprocessor).
    /// @return The method name.
    const NCString   GetMethod()             { return callStack[size_t(callStackSize)].origMethod; }

    /// Receives the source file line number (usually provided by the preprocessor).
    /// @return The source file line number.
    int             GetLineNumber()            { return callStack[size_t(callStackSize)].origLine; }

    /// The timestamp of the last invocation of #".Set".
    /// @return The latest timestamp.
    Ticks           GetTimeStamp()            { return callStack[size_t(callStackSize)].timeStamp; }

    /// Receives the type information of the caller.
    /// If called from global function or static method, this will be \c nullptr.
    /// @returns The type information, if available.
    const std::type_info*   GetTypeInfo()      { return callStack[size_t(callStackSize)].typeInfo; }

#if !ALIB_SINGLE_THREADED
    /// Receives the thread ID of the caller.
    /// @returns The thread ID.
    threads::ThreadID  GetThreadID() {
        if( thread == nullptr )
            thread= Thread::Get(threadNativeIDx);
        return thread->GetID();
    }

    /// Receives the thread ID of the caller.
    /// @returns The thread ID.
    std::thread::id     GetThreadNativeID()                              { return threadNativeIDx; }

    /// Receives information about the thread that the current call was invoked with.
    /// @param id  Output parameter receiving the \alib thread ID. If nullptr, it will be
    ///            ignored.
    /// @returns The name of the current thread. The id is stored within the provided
    ///          pointer.
    const alib::String& GetThreadNameAndID( threads::ThreadID* id ) {
        if ( threadName.IsNull() ) {
            // get current thread, if not passed with set()
            if( thread == nullptr )
                thread =  Thread::Get(threadNativeIDx);

            if (id != nullptr)
                *id=    thread->GetID();

            // do we have a dictionary entry?
            auto it= threadDictionary.Find( thread->GetID() );
            if (it != threadDictionary.end() )
                threadName= it->second;
            else
                threadName= thread->GetName();
        }

        return    threadName;
    }
#endif

  //################################################################################################
  // Internals
  //################################################################################################
  protected:

    /// Tries to trim the source file's path. Sets variable SourceFile::trimmedPath to either
    /// the successfully trimmed path or to the non-trimmed one. This way, it is executed
    /// only once, at it is 'lazily' invoked by #".GetTrimmedPath"
    ALIB_DLL
    void            trimPath();

    /// Gets the position of path/filename separator. This is evaluated only once after an
    /// invocation of #".Set".
    /// @return The index of the path separator in SourceFile::origFile.
    integer         getPathLength() {
        auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
        if( srcFile.origFilePathLength == -2 ) {
            srcFile.origFilePathLength= srcFile.origFile.LastIndexOf( DIRECTORY_SEPARATOR );

            // it may happen that the path separator of scope information is Unix-style while the
            // platform is WindowsOS (e.g. with MinGW-cross compilation).
            #if defined( _WIN32 )
                if( srcFile.origFilePathLength == -1 )
                    srcFile.origFilePathLength= srcFile.origFile.LastIndexOf( '/' );
            #endif
        }
        return srcFile.origFilePathLength;
    }
}; // class ScopeInfo

}}} // namespace [alib::lox::detail]

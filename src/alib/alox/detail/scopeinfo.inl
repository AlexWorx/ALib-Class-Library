//==================================================================================================
/// \file
/// This header file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_LOX_DETAIL_SCOPEINFO
#define HPP_ALIB_LOX_DETAIL_SCOPEINFO 1
#pragma once
#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// includes
// #################################################################################################
#include "alib/time/ticks.hpp"
#include "alib/lang/system/path.hpp"
#include "alib/containers/lrucachetable.hpp"
#if ALIB_THREADS
#   include "alib/threads/thread.hpp"
#endif
#include "alib/containers/hashtable.hpp"
#include "alib/monomem/aliases/stdvector.hpp"

namespace alib {

namespace config { class Variable; }
namespace lox    { namespace detail {


struct LoxImpl;

//==================================================================================================
/// Encapsulates information of the caller that can be collected. This is platform-specific, in
/// this case, C++. What we receive from the C++ preprocessor is the source file name
/// of the calling code, the line number within the source file name and the name of the method
/// the call is placed in.
/// Furthermore, we collect the <c>std::type_info*</c> in case the call was not placed in a
/// static or namespace function.
///
/// The method has a list of interface functions that gets such source information and some derived
/// variants of it. In addition, thread and timer information is stored and managed.
///
/// As far as possible, 'lazy' techniques come to practice with this class. This means, only
/// the things that are queried in-between to invocations of method #Set are calculated.
///
/// ## Friends ##
/// class \alib{lox;Lox}
//==================================================================================================
class ScopeInfo
{
    public:
#if ALIB_THREADS
        /// A map we use to translate thread IDs to thread names
        using ThreadDictionary=   HashMap<MonoAllocator, threads::ThreadID, String32>;
#endif

  // ###############################################################################################
  // Protected fields
  // ###############################################################################################
  protected:
    #if !DOXYGEN
        friend struct LoxImpl;
        friend void  LI::MapThreadName(LoxImpl*, const String&, threads::ThreadID);
        friend void  LI::GetState( LoxImpl*, NAString&, StateInfo);
    #endif

        /// Defines how source paths names are to be trimmed. \alox allows a set of global rules and
        /// a set that is 'local', hence specific to a \b Lox.
        struct SourcePathTrimRule
        {
            NAString            Path;            ///< The path string
            NAString            TrimReplacement; ///< Optional replacement string for trimmed paths.
            int                 TrimOffset;      ///< Additional offset of the trim position
            lang::Inclusion     IncludeString;   ///< Denotes if #Path itself should be included when trimmed
            lang::Case          Sensitivity;     ///< The sensitivity of the comparison when trimming
            config::Priority    Priority;        ///< The priority of the rule. Depends on origin: source code, config...)
            bool                IsPrefix;        ///< \c true if path was not starting with <c>'*'</c>, when provided.
        };

        /// List of trim definitions for portions of source paths to be ignored.
        ALIB_API
        static std::vector<SourcePathTrimRule>  GlobalSPTRs;

        /// List of trim definitions for portions of source paths to be ignored.
               std::vector<SourcePathTrimRule>  LocalSPTRs;

        /// Flag to determine if global rules have been read from config already.
        ALIB_API
        static bool                             GlobalSPTRsReadFromConfig;

        ///  If true, next time a source path cannot be trimmed successfully with custom
        ///  trim information provided with
        ///  \ref alib::lox::Lox::SetSourcePathTrimRule "Lox::SetSourcePathTrimRule"
        ///  some trim information is automatically created by comparing such source file's path
        ///  with the path of the executable of the current process.
        bool                                    AutoDetectTrimableSourcePath                 = true;

    #if ALIB_THREADS
        /// The C++ native ID.
        std::thread::id                         threadNativeIDx;

        /// The thread passed with #Set.
        Thread*                                 thread                                    = nullptr;

        /// The name of the thread that executed the log.
        String                                  threadName;

        /// Dictionary to translate thread IDs into something maybe nicer/shorter.
        /// The dictionary may be filled by the user of the library using \alox{Lox.MapThreadName}.
        ThreadDictionary                        threadDictionary;
    #endif

        /// Information of a single source file. Stored in field #parsedFileNameCache.
        struct ParsedFileName
        {
            /// Path and name of source file (given by the C++ preprocessor).
            NCString        origFile;

            /// Full path of source file  (evaluated).
            NString         fullPath                                                      = nullptr;

            /// Trimmed path of source file  (evaluated).
            NString         trimmedPath                                                   = nullptr;

            /// Prefix for the trimmed path taken from trim rule. Has to be added on writing
            /// the trimmed path *
            NString         trimmedPathPrefix                                             = nullptr;

            /// File name (evaluated).
            NString         name                                                          = nullptr;

            /// File name without extension (evaluated).
            NString         nameWOExt                                                     = nullptr;

            /// Index of last path separator in #origFile.
            integer         origFilePathLength                                                 = -2;

            /// Constructor.
            /// @param filename Stored in #origFile.
            ParsedFileName( const NCString  filename) : origFile(filename)                        {}
        };

        /// Serves as template parameter \p{TValueDescriptor} of field #parsedFileNameCache.
        struct ValueDescriptorPFN : containers::TSubsetKeyDescriptor<ParsedFileName, NCString>
        {
            /// Provides access to the key-portion of the cached set.
            /// @param src  The cached element.
            /// @return The key portion of the element.
            NCString            Key   (ParsedFileName& src)       const { return src.origFile; }
        };

        /// Least recently used cache of parsed file name.
        LRUCacheTable< MonoAllocator, ValueDescriptorPFN, std::hash<NString>>   parsedFileNameCache;

        /// Holds values for the current scope. Because recursive logging might occur (e.g., when
        /// parameters rely on method invocations which incorporate log statements), objects of this
        /// class are stored in stack #callStack.
        struct FrameRecord
        {
            /// Time of the call that created this record.
            Ticks                   timeStamp;

            /// The entry from the #parsedFileNameCache
            ParsedFileName*         Parsed;

            /// Line number within the source file (given by the C++ preprocessor)
            int                     origLine;

            /// Function/method name (given by the C++ preprocessor)
            NCString                origMethod;

            /// Type information. Nullptr if call from static or global function.
            const std::type_info*   typeInfo;
        };

        /// A stack of scopes (allows recursive calls/nested logging).
        StdVectorMono<FrameRecord>  callStack;

        /// The current depth of recursive invocations.
        int                         callStackSize                                              = -1;

    // #############################################################################################
    // Types exposed
    // #############################################################################################
    public:
        /// The name of the Lox we are attached to.
        NString                     loxName;


    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        //==========================================================================================
        /// Constructs a scope info.
        /// @param name       The name of the Lox that this object belongs to.
        ///                   Will be converted to upper case.
        /// @param allocator  The monotonic allocator of "our" \b %Lox, used for long-term allocations
        //==========================================================================================
        ALIB_API
        ScopeInfo( const NString& name, MonoAllocator& allocator);

     // #############################################################################################
     // public interface
     // #############################################################################################
    public:
        /// Changes the capacity of the \b %LRUCacheTable for parsed file names by calling
        /// \alib{containers;LRUCacheTable::Reserve}.
        /// @param numberOfLists  The number of LRU-lists to use.
        /// @param entriesPerList The maximum length of each cache list.
        void SetFileNameCacheCapacity( integer numberOfLists, integer entriesPerList )
        { parsedFileNameCache.Reserve( numberOfLists, entriesPerList ); }

        //==========================================================================================
        /// Stores C++ specific caller parameters and some other values like the time stamp.
        /// Also, flags thread information as not received, yet.
        /// Counts the recursion counter up.
        /// @param ci   The caller information.
        //==========================================================================================
        ALIB_API
        void Set( const lang::CallerInfo& ci  );

        /// Releases latest scope information.
        void PopNestedScope()
        {
            --callStackSize;
            ALIB_ASSERT( callStackSize >= -1 )
        }


        //==========================================================================================
        /// Does the job for
        /// \ref alib::lox::Lox::SetSourcePathTrimRule    "Lox::SetSourcePathTrimRule" and
        /// \ref alib::lox::Lox::ClearSourcePathTrimRules "Lox::ClearSourcePathTrimRules".
        ///
        /// @param path            The path to search for. If not starting with <c> '*'</c>,
        ///                        a prefix is searched.
        /// @param includeString   Determines if \p{path} should be included in the trimmed path or not.
        /// @param trimOffset      Adjusts the portion of \p{path} that is trimmed. 999999 to clear!
        /// @param sensitivity     Determines if the comparison of \p{path} with a source file's path
        ///                        is performed case-sensitive or not.
        /// @param trimReplacement Replacement string for trimmed portion of the path.
        /// @param reach           Denotes whether the rule is applied locally (to this \b %Lox only)
        ///                        or applies to all instances of class \b %Lox.
        ///                        Defaults to \b %Reach::Global.
        /// @param priority        The priority of the setting.
        //==========================================================================================
        ALIB_API
        void      SetSourcePathTrimRule( const NCString&    path,
                                         lang::Inclusion    includeString,
                                         int                trimOffset,
                                         lang::Case         sensitivity,
                                         const NString&     trimReplacement,
                                         lang::Reach        reach,
                                         Priority           priority                );

        //==========================================================================================
        /// Receives the name of the \b Lox we are belonging to (this is a 1:1 relationship).
        /// @return The name of the \b Lox.
        //==========================================================================================
        const NString   GetLoxName()
        {
            return loxName;
        }

        //==========================================================================================
        /// Receives the original filename and path of the source file
        /// (usually provided by the preprocessor).
        /// @return The full path and filename of the source file.
        //==========================================================================================
        const NCString& GetOrigFile()
        {
            return callStack[size_t(callStackSize)].Parsed->origFile;
        }

        //==========================================================================================
        /// Receives the path of the source file (not trimmed, see #GetTrimmedPath).
        /// @return The path of the source file.
        //==========================================================================================
        const NString   GetFullPath()
        {
            auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
            if( srcFile.fullPath.IsNull() )
            {
                integer idx= getPathLength();
                if( idx >= 0 )
                    srcFile.fullPath= NString( srcFile.origFile.Buffer(), idx );
                else
                    srcFile.fullPath= "";
            }

            return srcFile.fullPath;
        }

        //==========================================================================================
        /// Writes the path of the source file, trimmed according to trim-information provided
        /// with #SetSourcePathTrimRule or detected according to #AutoDetectTrimableSourcePath.
        /// @param target The target string to append the trimmed path to.
        //==========================================================================================
        void            GetTrimmedPath( AString& target )
        {
            auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
            if( srcFile.trimmedPath.IsNull() )
                trimPath();

            target._( srcFile.trimmedPathPrefix )
                  ._( srcFile.trimmedPath );
        }

        //==========================================================================================
        /// Receives the source file name excluding the path (usually provided by the preprocessor).
        /// @return The source file name excluding the path
        //==========================================================================================
        const NString   GetFileName()
        {
            auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
            if( srcFile.name.IsNull() )
            {
                integer idx= getPathLength();
                if( idx >= 0 )
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    srcFile.name= NString( srcFile.origFile.Buffer() + idx + 1,
                                           srcFile.origFile.Length() - idx - 1 );
                    ALIB_WARNINGS_RESTORE
                }
                else
                    srcFile.name= "";
            }
            return srcFile.name;
        }

        //==========================================================================================
        /// Receives the source file name excluding the path and without an extension
        /// (usually provided by the preprocessor).
        /// @return The source file name excluding the path and extension.
        //==========================================================================================
        const NString   GetFileNameWithoutExtension()
        {
            auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
            if( srcFile.nameWOExt.IsNull() )
            {
                srcFile.nameWOExt= GetFileName();
                integer lastDot=   srcFile.nameWOExt.LastIndexOf( '.' );
                if ( lastDot > 0 )
                    srcFile.nameWOExt= NString( srcFile.nameWOExt.Buffer(), lastDot );
            }
            return srcFile.nameWOExt;
        }


        //==========================================================================================
        /// Receives the method name (usually provided by the preprocessor).
        /// @return The method name.
        //==========================================================================================
        const NCString   GetMethod()
        {
             return callStack[size_t(callStackSize)].origMethod;
        }

        //==========================================================================================
        /// Receives the source file line number (usually provided by the preprocessor).
        /// @return The source file line number.
        //==========================================================================================
        int             GetLineNumber()
        {
            return callStack[size_t(callStackSize)].origLine;
        }

        //==========================================================================================
        /// The timestamp of the last invocation of #Set.
        /// @return The latest timestamp.
        //==========================================================================================
        Ticks           GetTimeStamp()
        {
            return callStack[size_t(callStackSize)].timeStamp;
        }

        //==========================================================================================
        /// Receives the type information of the caller.
        /// If called from global function or static method, this will be \c nullptr.
        /// @returns The type information, if available.
        //==========================================================================================
        const std::type_info*   GetTypeInfo()
        {
            return callStack[size_t(callStackSize)].typeInfo;
        }

#if ALIB_THREADS
        //==========================================================================================
        /// Receives the thread ID of the caller.
        /// @returns The thread ID.
        //==========================================================================================
        threads::ThreadID  GetThreadID()
        {
            if( thread == nullptr )
                thread= Thread::Get(threadNativeIDx);
            return thread->GetID();
        }

        //==========================================================================================
        /// Receives the thread ID of the caller.
        /// @returns The thread ID.
        //==========================================================================================
        std::thread::id     GetThreadNativeID()                           { return threadNativeIDx; }

        //==========================================================================================
        /// Receives information about the thread that the current call was invoked with.
        /// @param id  Output parameter receiving the \alib thread ID. If nullptr, it will be
        ///            ignored.
        /// @returns The name of the current thread. The id is stored within the provided
        ///          pointer.
        //==========================================================================================
        const alib::String& GetThreadNameAndID( threads::ThreadID* id )
        {
            if ( threadName.IsNull() )
            {
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

    // #############################################################################################
    // Internals
    // #############################################################################################
        protected:

        //==========================================================================================
        /// Tries to trim the source file's path. Sets variable SourceFile::trimmedPath to either
        /// the successfully trimmed path or to the non-trimmed one. This way, it is executed
        /// only once, at it is 'lazily' invoked by #GetTrimmedPath()
        //==========================================================================================
        ALIB_API
        void            trimPath();

        //==========================================================================================
        /// Gets position of path/filename separator. This is evaluated only once after an
        /// invocation of #Set.
        /// @return The index of the path separator in SourceFile::origFile.
        //==========================================================================================
        integer         getPathLength()
        {
            auto& srcFile= *callStack[size_t(callStackSize)].Parsed;
            if( srcFile.origFilePathLength == -2 )
                srcFile.origFilePathLength= srcFile.origFile.LastIndexOf( lang::system::DIRECTORY_SEPARATOR );
            return srcFile.origFilePathLength;
        }
}; // class ScopeInfo

}}} // namespace [alib::lox::detail]

#endif // HPP_ALIB_LOX_DETAIL_SCOPEINFO


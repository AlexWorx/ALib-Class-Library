/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_alox of the \aliblong.
 *
 * \emoji :copyright: 2013-2023 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALOX_DETAIL_SCOPEINFO
#define HPP_ALOX_DETAIL_SCOPEINFO 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_TIME_TICKS)
    #include "alib/time/ticks.hpp"
#endif

#if !defined (HPP_ALIB_SYSTEM_DIRECTORY)
    #include "alib/system/directory.hpp"
#endif

#if ALIB_THREADS && !defined (HPP_ALIB_THREADS_THREAD)
    #include "alib/threads/thread.hpp"
#endif

#if !defined (HPP_ALIB_MONOMEM_HASHMAP)
#   include "alib/monomem/hashmap.hpp"
#endif
#if !defined (HPP_ALIB_MONOMEM_STDCONTAINERMA)
#   include "alib/monomem/stdcontainerma.hpp"
#endif

namespace aworx { namespace lib {

namespace config { class Variable; }
namespace lox    { namespace detail {


struct LoxImpl;

/** ************************************************************************************************
 * Encapsulates information of the caller that can be collected. This is platform specific, in
 * this case, C++. What we receive from the C++ preprocessor, is the source file name
 * of the calling code, the line number within the source file name and the name of the method
 * the call is placed in. E.g., wee do not get the class name of the object or even its instance.
 *
 * The method has a list of interface functions that gets such source information and some derived
 * variants of it. In addition, thread and timer information is stored and managed.
 *
 * As far as possible, 'lazy' techniques come to practice with this class. This means, only
 * the things that are queried in-between to invocations of method #Set are calculated.
 *
 * ## Friends ##
 * class \alib{lox,Lox}
 **************************************************************************************************/
class ScopeInfo
{
    #if !defined(ALIB_DOX)
        friend void  LI::Reset(LoxImpl*, bool);
        friend void  LI::MapThreadName(LoxImpl*, const String&, threads::ThreadID);
        friend void  LI::GetState( LoxImpl*, NAString&, StateInfo);
    #endif

    // #############################################################################################
    // Types exposed
    // #############################################################################################
    public:
#if ALIB_THREADS
        /** A std::map we use to translate thread IDs to thread names */
        using ThreadDictionary=   HashMap<threads::ThreadID, String32>;
#endif

        /**
         * The number of source file path and corresponding, evaluated derived values.
         * This might be modified, prior to creating any object of class \b Lox.<br>
         * Defaults to 5.
         */
        ALIB_API
        static  int             DefaultCacheSize;

        /** The name of the Lox we are attached to. */
        NString                 loxName;

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** Defines portions of source paths to be ignored. */
        struct SourcePathTrimRule
        {
            NAString     Path;            ///< The path string
            bool         IsPrefix;        ///< true if path was not starting with '\*' when provided.
            Inclusion    IncludeString;   ///< Denotes if #Path itself should be included when trimmed
            Case         Sensitivity;     ///< The sensitivity of the comparison when trimming
            int          TrimOffset;      ///< Additional offset of the trim position
            NAString     TrimReplacement; ///< Optional replacement string for trimmed paths.
            Priorities   Priority;        ///< The priority of the rule. Depends on origin: source code, config...)
        };

        /** List of trim definitions for portions of source paths to be ignored. */
        ALIB_API
        static std::vector<SourcePathTrimRule>   GlobalSPTRs;

        /** List of trim definitions for portions of source paths to be ignored. */
               std::vector<SourcePathTrimRule>   LocalSPTRs;

        /** Flag to determine if global rules have been read from config already. */
        ALIB_API
        static bool                              GlobalSPTRsReadFromConfig;

        /**
         *  If true, next time a source path can not be trimmed successfully with custom
         *  trim information provided with
         *  \ref aworx::lib::lox::Lox::SetSourcePathTrimRule "Lox::SetSourcePathTrimRule"
         *  some trim information is automatically created by comparing such source file's path
         *  with the path of the executable of the current process.
         */
        bool                                     AutoDetectTrimableSourcePath                = true;


#if ALIB_THREADS
        /** The thread passed with #Set. */
        Thread*                                 thread                                    = nullptr;

        /** The name of the thread that executed the log. */
        String                                  threadName;

        /**
         * Dictionary to translate thread IDs into something maybe nicer/shorter.
         * The dictionary may be filled by the user of the library using \alox{Lox.MapThreadName}.
         */
        ThreadDictionary                        threadDictionary;
#endif

        /** Information of a single source file. Stored in field #cache. */
        struct SourceFile
        {
            /** 'Timestamp' for LRU overwriting (not a time, but using field #cacheRun). */
            uint64_t        timeStamp                                                           = 0;

            /** Path and name of source file (given by the C++ preprocessor). */
            NCString        origFile                                                      = nullptr;

            /** Full path of source file  (evaluated). */
            NString         fullPath                                                      = nullptr;

            /** Trimmed path of source file  (evaluated). */
            NString         trimmedPath                                                   = nullptr;

            /** Prefix for the trimmed path taken from trim rule. Has to be added on writing
             *  the trimmed path **/
            NString         trimmedPathPrefix                                             = nullptr;

            /** File name (evaluated).  */
            NString         name                                                          = nullptr;

            /** File name without extension (evaluated). */
            NString         nameWOExt                                                     = nullptr;

            /** Index of last path separator in #origFile. */
            integer         origFilePathLength                                                 = -2;

            /** Clears calculated values. Keeps #origFile intact. */
            void Clear()
            {
                origFilePathLength= -2;
                fullPath=           nullptr;
                trimmedPath=        nullptr;
                name=               nullptr;
                nameWOExt=          nullptr;
            }
        };


        /** The 'timestamp' used to identify the LRU entry. Incremented, whenever a different source
            file is evaluated.  */
        uint64_t                                cacheRun                                        = 0;

        /**
          * The number of source file path and corresponding, evaluated derived values.
          * Determined in the constructor by reading static field #DefaultCacheSize.
          */
        int                                     cacheSize;

        /** An array of source files. Its size is dependent on static field #DefaultCacheSize. */
        SourceFile*                             cache;

        /**
         * Holds values for the current scope. Because recursive logging might occur (e.g. when
         * parameters rely on method invocations which incorporate log statements), objects of this
         * class are stored in stack #scopes.
         */
        struct Scope
        {
            /** Time of the call represented by this instance. */
            Ticks                               timeStamp;

            /** The entry of the source file cache */
            SourceFile*                         sourceFile;

            /** Line number within the source file (given by the C++ preprocessor) */
            int                                 origLine;

            /** Function/method name (given by the C++ preprocessor) */
            NCString                            origMethod;
        };

        /** A stack of scopes (allows recursive calls/nested logging). */
        std::vector<Scope,StdContMA<Scope>>     scopes;

        /** The current depth of recursive invocations. */
        int                                     actScopeDepth                                  = -1;

        /** The last source file used. This is tried first with next invocation. If it does not
         *  match, the cache is searched for another matching one.*/
        SourceFile*                             lastSourceFile;


    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs a scope info.
         * @param name       The name of the Lox that this object belongs to.
         *                   Will be converted to upper case.
         * @param allocator  The monotonic allocator of "our" \b %Lox, used for long-term allocations
         * @param tempVar    A temporary variable for internal use.
         ******************************************************************************************/
        ALIB_API
        ScopeInfo( const NString& name, MonoAllocator* allocator, config::Variable& tempVar);

     // #############################################################################################
     // public interface
     // #############################################################################################
    public:
    #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * Stores C++ specific caller parameters and some other values like the time stamp.
         * Also, flags thread information as not received, yet.
         * Counts the recursion counter up.
         * @param source      Name, including path, of the source code file of the actual log
         *                    invocation (__FILE__).
         * @param lineNumber  Line number within the source code file of the actual log
         *                    invocation (__LINE__).
         * @param method      Name of method or function of the actual log invocation (mostly
         *                    __func__/ __FUNCTION__).
         * @param thread      The thread. If \c nullptr, it will be determined if needed.
         ******************************************************************************************/
        ALIB_API
        void Set( const NCString&  source, int lineNumber, const NCString& method,
                  threads::Thread* thread                                            );
    #else
        void Set( const NCString& source, int lineNumber, const NCString& method
                  ALIB_IF_THREADS( , threads::Thread* thread )                       );
    #endif

        /** ****************************************************************************************
         * Releases latest scope information.
         ******************************************************************************************/
        void Release()
        {
            lastSourceFile= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            --actScopeDepth;
            ALIB_ASSERT( actScopeDepth >= -1 )
        }


        /** ****************************************************************************************
         * Does the job for
         * \ref aworx::lib::lox::Lox::SetSourcePathTrimRule    "Lox::SetSourcePathTrimRule" and
         * \ref aworx::lib::lox::Lox::ClearSourcePathTrimRules "Lox::ClearSourcePathTrimRules".
         *
         * @param path            The path to search for. If not starting with <c> '*'</c>,
         *                        a prefix is searched.
         * @param includeString   Determines if \p{path} should be included in the trimmed path or not.
         * @param trimOffset      Adjusts the portion of \p{path} that is trimmed. 999999 to clear!
         * @param sensitivity     Determines if the comparison of \p{path} with a source file's path
         *                        is performed case sensitive or not.
         * @param trimReplacement Replacement string for trimmed portion of the path.
         * @param reach           Denotes whether the rule is applied locally (to this \b %Lox only)
         *                        or applies to all instances of class \b %Lox.
         *                        Defaults to \b %Reach::Global.
         * @param priority        The priority of the setting.
        ******************************************************************************************/
        ALIB_API
        void      SetSourcePathTrimRule( const NCString&    path,
                                         Inclusion          includeString,
                                         int                trimOffset,
                                         Case               sensitivity,
                                         const NString&     trimReplacement,
                                         Reach              reach,
                                         Priorities         priority                );

        /** ****************************************************************************************
         * Receives the name of the \b Lox we are belonging to (this is a 1:1 relationship).
         * @return The name of the \b Lox.
         ******************************************************************************************/
        const NString   GetLoxName()
        {
            return loxName;
        }

        /** ****************************************************************************************
         * Receives the original filename and path of the source file
         * (usually provided by the preprocessor).
         * @return The full path and filename of the source file.
         ******************************************************************************************/
        const NCString& GetOrigFile()
        {
            return scopes[static_cast<size_t>(actScopeDepth)].sourceFile->origFile;
        }

        /** ****************************************************************************************
         * Receives the path of the source file (not trimmed, see #GetTrimmedPath).
         * @return The path of the source file.
         ******************************************************************************************/
        const NString   GetFullPath()
        {
            SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            if( actual->fullPath.IsNull() )
            {
                integer idx= getPathLength();
                if( idx >= 0 )
                    actual->fullPath= NString( actual->origFile.Buffer(), idx );
                else
                    actual->fullPath= "";
            }

            return actual->fullPath;
        }

        /** ****************************************************************************************
         * Writes the path of the source file, trimmed according to trim-information provided
         * with #SetSourcePathTrimRule or detected according to #AutoDetectTrimableSourcePath.
         * @param target The target string to append the trimmed path to.
         ******************************************************************************************/
        void            GetTrimmedPath( AString& target )
        {
            SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            if( actual->trimmedPath.IsNull() )
                trimPath();

            target._( actual->trimmedPathPrefix )
                  ._( actual->trimmedPath );
        }

        /** ****************************************************************************************
         * Receives the source file name excluding the path (usually provided by the preprocessor).
         * @return The source file name excluding the path
         ******************************************************************************************/
        const NString   GetFileName()
        {
            SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            if( actual->name.IsNull() )
            {
                integer idx= getPathLength();
                if( idx >= 0 )
                {
                    ALIB_WARNINGS_ALLOW_UNSAFE_BUFFER_USAGE
                    actual->name= NString( actual->origFile.Buffer() + idx + 1,
                                           actual->origFile.Length() - idx - 1 );
                    ALIB_WARNINGS_RESTORE
                }
                else
                    actual->name= "";
            }
            return actual->name;
        }

        /** ****************************************************************************************
         * Receives the source file name excluding the path and without an extension
         * (usually provided by the preprocessor).
         * @return The source file name excluding the path and extension.
         ******************************************************************************************/
        const NString   GetFileNameWithoutExtension()
        {
            SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            if( actual->nameWOExt.IsNull() )
            {
                actual->nameWOExt= GetFileName();
                integer lastDot=   actual->nameWOExt.LastIndexOf( '.' );
                if ( lastDot > 0 )
                    actual->nameWOExt= NString( actual->nameWOExt.Buffer(), lastDot );
            }
            return actual->nameWOExt;
        }


        /** ****************************************************************************************
         * Receives the method name (usually provided by the preprocessor).
         * @return The method name.
         ******************************************************************************************/
        const NCString   GetMethod()
        {
            return scopes[static_cast<size_t>(actScopeDepth)].origMethod;
        }

        /** ****************************************************************************************
         * Receives the source file line number (usually provided by the preprocessor).
         * @return The source file line number.
         ******************************************************************************************/
        int             GetLineNumber()
        {
            return scopes[static_cast<size_t>(actScopeDepth)].origLine;
        }

        /** ****************************************************************************************
         * The timestamp of the last invocation of #Set.
         * @return The latest timestamp.
         ******************************************************************************************/
        Ticks           GetTimeStamp()
        {
            return scopes[static_cast<size_t>(actScopeDepth)].timeStamp;
        }

#if ALIB_THREADS
        /** ************************************************************************************
         * Receives the thread ID of the caller.
         * @returns The thread ID.
         **************************************************************************************/
        threads::ThreadID  GetThreadID()
        {
            if( thread == nullptr )
                thread= Thread::GetCurrent();
            return thread->GetId();
        }

        /** ************************************************************************************
         * Receives information about the thread that the current call was invoked with.
         * @param id  Output parameter receiving the \alib thread ID. If nullptr, it will be
         *            ignored.
         * @returns The name of the current thread. The id is stored within the provided
         *          pointer.
         **************************************************************************************/
        const aworx::String& GetThreadNameAndID( threads::ThreadID* id )
        {
            if ( threadName.IsNull() )
            {
                // get current thread, if not passed with set()
                if( thread == nullptr )
                    thread =  Thread::GetCurrent();

                if (id != nullptr)
                    *id=    thread->GetId();

                // do we have a dictionary entry?
                auto it= threadDictionary.Find( thread->GetId() );
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

        /** ************************************************************************************
         * Tries to trim the source file's path. Sets variable SourceFile::trimmedPath to either
         * the successfully trimmed path or to the non-trimmed one. This way, it is executed
         * only once, at it is 'lazily' invoked by #GetTrimmedPath()
         **************************************************************************************/
        ALIB_API
        void            trimPath();

        /** ************************************************************************************
         * Gets position of path/filename separator. This is evaluated only once after an
         * invocation of #Set.
         * @return The index of the path separator in SourceFile::origFile.
         **************************************************************************************/
        integer        getPathLength()
        {
            SourceFile* actual= scopes[static_cast<size_t>(actScopeDepth)].sourceFile;
            if( actual->origFilePathLength == -1 )
                return -1;
            return ( actual->origFilePathLength= actual->origFile.LastIndexOf( aworx::DirectorySeparator ) );
        }
}; // class ScopeInfo

}}}}// namespace [aworx::lib::lox::detail]

#endif // HPP_ALOX_DETAIL_SCOPEINFO

// #################################################################################################
//  aworx::lib::lox - ALox Logging Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALOX_LOX
#define HPP_ALOX_LOX 1

#if !defined(HPP_ALIB_LOX_PROPPERINCLUDE)
#   error "ALib sources with ending '.inl' must not be included from outside."
#endif

// #################################################################################################
// includes
// #################################################################################################
#if !defined (HPP_ALIB_THREADS_SMARTLOCK)
    #include "alib/threads/smartlock.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_UTIL_TOKENIZER)
    #include "alib/strings/util/tokenizer.hpp"
#endif
#if !defined (HPP_ALIB_CONFIG_CONFIGURATION)
    #include "alib/config/configuration.hpp"
#endif
#if !defined (HPP_ALOX_CORE_SCOPEINFO)
    #include "alib/alox/detail/scopeinfo.inl"
#endif
#if !defined (HPP_ALOX_CORE_LOGGER)
    #include "alib/alox/detail/logger.inl"
#endif
#if !defined (HPP_ALOX_CORE_DOMAIN)
    #include "alib/alox/detail/domain.inl"
#endif
#if !defined (HPP_ALOX_CORE_SCOPE)
    #include "alib/alox/detail/scopestore.inl"
#endif

#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
    #include <map>
#endif


namespace aworx { namespace lib { namespace lox { namespace detail {
    class Logger;
    namespace textlogger
    {
        class TextLogger;
    }
}

/** ************************************************************************************************
 * Used to store prefixes set. Those provided as
 * \ref aworx::lib::boxing::Box "boxes" of character arrays are copied into an internal AString
 * and will be deleted with the object. This ensures, that simple strings might get assembled
 * on the stack and still be used as a prefix logable.
 **************************************************************************************************/
class PrefixLogable : public Box
{
    protected:
        /** If set, it will be deleted. */
        AString* copy   = nullptr;

    public:
    /**
     * Constructor taking the originally provided box. If this is an array of characters, the
     * contents is copied into a heap allocated (new) AString and our vtable is replaced accordingly.
     * @param src The prefix object provided by the user
     */
    PrefixLogable( const Box& src )
    : Box( src )
    {
        // uses "placement new" to overwrite the box part of ourselves
             if ( IsArrayOf<nchar>() )  new (this) Box( copy= new AString( Unbox<NString>() ));
        else if ( IsArrayOf<wchar>() )  new (this) Box( copy= new AString( Unbox<WString>() ));
        else if ( IsArrayOf<xchar>() )  new (this) Box( copy= new AString( Unbox<XString>() ));
    }

    ~PrefixLogable()
    {
        if(copy)
            delete copy;
    }
    /**
     * Move assignment operator.
     * Default implementation of compiler is used.
     * (Needs to be explicitly given due to custom destructor.)
     * @param move The object to be moved.
     * @return This object
     */
    PrefixLogable& operator= (PrefixLogable&& move ) = default;
};

/** ************************************************************************************************
 * This class acts as a container for \e Loggers and provides a convenient interface to logging.
 * Fore information how to use this class, checkout the \alox tutorials and the ALox manual.
 *
 * If thread safeness is not needed for the logging system (see user manual),
 * the mutex can be switched of using inherited method \alib{threads,ThreadLock::SetSafeness}.
 * The goal here would be to increase logging performance. This is really a very
 * seldom case when performance is absolutely key, and it is better to be kept in safe mode.
 *
 * <b>'Logable Objects' in <em>%ALox for C++</em></b><br>
 * While other \alox implementations, like ALox for C# or ALox for Java use the 'run-time type
 * information' feature of their underlying programming language to identify any object type,
 * in the C++ version
 * \ref aworx::lib::boxing "%ALib Boxing" is used to be able to log arbitrary object types.
 **************************************************************************************************/
class Lox
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:
        /** A mutex to control parallel access. */
        threads::ThreadLock                         Lock;

        /**
         * A counter for the quantity of calls. The count includes logs that were suppressed by
         * disabled <em>Log Domain</em> and those suppressed by the optional log condition parameter.
         */
        int                                         CntLogCalls                                  =0;

        /**
         * Denotes flags used with methods #GetState and #State to select different parts
         * of the state receive.
         */
        enum class StateInfo
        {
           NONE                     = 0,       ///< No state
           Basic                    = 1 <<  0, ///< Name and number of log calls
           Version                  = 1 <<  1, ///< Library Version and thread safeness
           Loggers                  = 1 <<  2, ///< Loggers

           Domains                  = 1 <<  3, ///< Log domains currently registered
           InternalDomains          = 1 <<  4, ///< Internal domains
           ScopeDomains             = 1 <<  5, ///< Scope domains
           DSR                      = 1 <<  6, ///< Domain substitution rules
           PrefixLogables           = 1 <<  7, ///< Prefix logables
           Once                     = 1 <<  8, ///< Log once counters
           LogData                  = 1 <<  9, ///< Log data objects
           ThreadMappings           = 1 << 10, ///< Named threads

           SPTR                     = 1 << 20, ///< Source path trim rules
           CompilationFlags         = 1 << 21, ///< \alib/\alox compilation flags

           All                      = ~0L,
        };


    // #############################################################################################
    // Private/protected fields  and constants
    // #############################################################################################
    protected:
        /** A list of a lists of logables used for (recursive) logging. */
        std::vector<Boxes*>                         logableContainers;

        /** A list of a lists of logables used for (recursive) internal logging. */
        std::vector<Boxes*>                         internalLogables;

        /** The recursion counter for internal logging. */
        size_t                                      internalLogRecursionCounter             = 0;

        /** Information about the source code, method, thread, etc. invoking a log  call */
        detail::ScopeInfo                             scopeInfo;

        /**
         * The root domain \"/\". All registered domains become a sub domain of this root.
         * If a <em>Sub-Log Domain's Verbosity</em> is not explicitly set, such sub domain inherits
         * the verbosity of its parent.
         */
        detail::Domain                                domains;

        /** The root domain for internal <em>Log Domains</em>. */
        detail::Domain                                internalDomains;


        /** Scope Domains */
        detail::ScopeStore<NAString*>                 scopeDomains;

        /** Log once counters */
        detail::ScopeStore<std::map<NAString, int>*>  scopeLogOnce;

        /** Prefix logables store */
        detail::ScopeStore<Box*>                      scopePrefixes;

        /** Log data store */
        detail::ScopeStore<std::map<NAString, Box>*>  scopeLogData;

        /** Used for tabular output of logger lists */
        integer                                     maxLoggerNameLength                       =0;

        /** Used for tabular output of logger lists */
        integer                                     maxDomainPathLength;

        /** A key value used in stores if no key is given (global object). */
        const aworx::NString                        noKeyHashKey                    = "$";

        /** Domain stubstitution rules. */
        struct DomainSubstitutionRule
        {
            /** Rule types. */
            public: enum class Type
            {
                Exact,       ///< Exact match.
                StartsWith,  ///< Starts with match.
                EndsWith,    ///< Ends with match.
                Substring    ///< Any sub-string.
            };

            Type            type;          ///< Denotes the type of the rule, depending of what
                                           ///<was set in originally as search path
            NString32       Search;        ///< The path to search.
            NString32       Replacement;   ///< The replacement.

            /**  Constructor.
             * @param s The path to search.
             * @param r The replacement.
            */
            DomainSubstitutionRule( const NString& s, const NString& r )
            {
                Search     .DbgDisableBufferReplacementWarning();
                Replacement.DbgDisableBufferReplacementWarning();

                // get type and adjust given search parameter
                integer startPos=   0;
                integer length=     s.Length();
                if ( s.CharAtStart() == '*' )
                {
                    startPos++;
                    length--;
                    if ( s.CharAtEnd() == '*' )
                    {
                        type= Type::Substring;
                        length--;
                    }
                    else
                        type= Type::EndsWith;
                }
                else
                {
                    if ( s.CharAtEnd() == '*' )
                    {
                        type= Type::StartsWith;
                        length--;
                    }
                    else
                        type= Type::Exact;
                }
                Search._( s, startPos, length );

                // minimum rule check
                if (   (     (    type == Type::Exact
                               || type == Type::StartsWith )
                         && Search.CharAtStart() != '/'
                        )
                    || (    type == Type::EndsWith
                        &&  Search.CharAtEnd() == '/'
                       )
                   )
                    Search.Reset(); // illegal rule


                Replacement= r;
            }
        };

        /** The list of domain substitution rules. */
        std::vector<DomainSubstitutionRule>        domainSubstitutions;

        /** Flag if a warning on circular rule detection was logged. */
        bool                                       oneTimeWarningCircularDS                  =false;

        /** Flag used with configuration variable LOXNAME_DUMP_STATE_ON_EXIT. */
        bool                                       loggerAddedSinceLastDebugState            =false;


    // #############################################################################################
    // Constructors/destructor
    // #############################################################################################
    public:

        /** ****************************************************************************************
         * Constructs a new, empty Lox with the given \p{name}.
         * The name is immutable and all \b %Lox objects registered with \alox must be unique.
         * Lower case letters in the name are converted to upper case.
         * The name \c "Log" is reserved for the internal default singleton used for debug-logging.
         * In addition, name \c "GLOBAL" is not allowed.
         *
         * If parameter \p{register} is \c true (the default), static method
         * \ref aworx::lib::lox::ALox::Register "ALox::Register" is invoked and the object will be
         * retrievable with static method
         * \ref aworx::lib::lox::ALox::Get "ALox::Get". In some situations, such 'registration'
         * may not be wanted.
         *
         * @param name       The name of the Lox. Will be copied and converted to upper case.
         * @param doRegister If \c true, this object is registered with static class
         *                   \ref aworx::lib::lox::ALox "ALox".
         *                   Optional and defaults to \c true.
         ******************************************************************************************/
        ALIB_API
        Lox(const NString& name, bool doRegister =true );

        /** ****************************************************************************************
         * Destructs a lox
         ******************************************************************************************/
        ALIB_API       ~Lox();

    // #############################################################################################
    // Interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Returns the name of this \b %Lox. The name user defined, provided in the constructor,
         * converted to upper case and otherwise immutable.
         *
         * @returns The name of this %Lox.
         ******************************************************************************************/
        const NString   GetName()
        {
            return scopeInfo.loxName;
        }

        /** ****************************************************************************************
         * Status of registration with \alox. To keep a \b %Lox "private" using parameter
         * \p{doRegister} of the constructor, allows to suppress registration.
         * Registered instances of this class can be statically received (by their name) using
         * \ref aworx::lib::lox::ALox::Get "ALox::Get".
         *
         * @returns \c true if this instance was registered with \alox, \c false if not.
         ******************************************************************************************/
        bool            IsRegistered()
        {
            return ALOX.Get( GetName() ) == this;
        }



        /** ***************************************************************************************
         * Acquire s this \b %Lox and sets the scope information data for the next log.
         *
         * @param file  The name of the source code file that the call is placed in.
         *              Usually the predefined preprocessor macro __FILE__ is passed here.
         * @param line  The line number within the source code file that the call is placed in.
         *              Usually the predefined preprocessor macro __LINE__ is passed here.
         * @param func  The name of the function that the call is placed in.
         *              Usually the predefined preprocessor macro __func__ (or __FUNCTION__) is passed here.
         **************************************************************************************/
        void            Acquire( const NCString& file, int line, const NCString& func )
        {
            ALIB_REL_DBG(    Lock.Acquire();
                           , Lock.Acquire(file,line,func);    )

            scopeInfo.Set( file, line, func, Lock.Owner() );
        }

        /** ****************************************************************************************
         * Releases ownership of this object. If #Acquire was called multiple times, the same
         * number of calls to this method have to be performed to release ownership.
         ******************************************************************************************/
        void            Release()
        {
            scopeInfo.Release();
            Lock.Release();
        }

        /** ****************************************************************************************
         * Resets this object. Concretely the following steps are performed:
         * - All loggers are removed
         * - All domain settings are cleared
         * - Scope Domains are cleared
         * - Log data is cleared
         * - Log once counters are cleared
         * - The thread dictionary is cleared.
         * - All Trim domains which set are cleared
         *
         * \attention
         *   This method was introduced to support resetting the debug \b %Lox objects in the unit
         *   tests. In real applications, and for release logging it recommended to delete a Lox
         *   and create a new one instead of resetting one.
         *   Side effects might appear using this method!
         ******************************************************************************************/
        ALIB_API
        void        Reset();

        /** ****************************************************************************************
         * Adds \p{path} to an internal list of sub-strings that are used to trim the path of
         * a source file name. Trimmed paths are used for \e Scope mechanisms and can be
         * logged (e.g. with meta information of class \b TextLogger.
         *
         * By default such setting affects all instances of class \b Lox, not only
         * this instance. This can be altered using parameter \p{global}.
         * one other The given trim information can either
         *
         * If given \p{path} starts with character <c> '*'</c>, the rest of the string is searched
         * within source paths. Otherwise, it is checked if a source path starts with the given
         * path.
         *
         * Parameter \p{trimReplacement} optionally provides a replacement string for the trimmed
         * path. This can be used for example to provide the right absolute path for an IDE
         * to find source files of a library.
         *
         * Parameter \p{includeString} determines if the searched sub-string should be included in the
         * resulting source path or not. In addition, parameter \p{trimOffset}, which can be negative
         * or positive, is added to the position of trimming. This can be used to increase the
         * length of the search path, and then cut only a portion of what was searched for.
         *
         * Finally, parameter \p{sensitivity} determines whether the match is performed case
         * sensitive or not. It defaults to non-sensitive, for convenience and for the fact that
         * for example Microsoft C++ compiler's preprocessor passes lower case path-strings!
         *
         * \note
         *   If the platform (compiler) specific path separator is <c>'/'</c>, then characters
         *   <c>'\'</c> found in parameters \p{path} and \p{trimReplacement} are replaced by <c>'\'</c>
         *   and vice versa. This allows to specify paths and sub-strings thereof in a platform
         *   independent way.
         *
         * \attention
         *   Setting global rules (when parameter \p{global} equals \c Inclusion::Include) is not
         *   protected by a \c mutex against concurrent access. Therefore, global rules have
         *   to be either at bootstrap of a process, before threads are created, or such creation
         *   has to 'manually' be protected by locking all existing instances of this class!
         *
         * @param path            The path to search for. If not starting with <c> '*'</c>,
         *                        a prefix is searched.
         * @param includeString   Determines if \p{path} should be included in the trimmed path or
         *                        not.
         *                        Optional and defaults to \b %Inclusion::Exclude.
         * @param trimOffset      Adjusts the portion of \p{path} that is trimmed.
         *                        Optional and defaults to \c 0.
         * @param sensitivity     Determines if the comparison of \p{path} with a source file's path
         *                        is performed case sensitive or not.
         *                        Optional and defaults to \b Case::Ignore.
         * @param trimReplacement Replacement string for trimmed portion of the path.
         *                        Optional and defaults to \b %NullString().
         * @param reach           Denotes whether the rule is applied locally (to this \b %Lox only)
         *                        or applies to all instances of class \b %Lox.
         *                        Defaults to \b %Reach::Global.
         * @param priority        The priority of the setting. Defaults to
         *                        \ref aworx::lib::config::Priorities "Priorities::DefaultValues".
         ******************************************************************************************/
        void      SetSourcePathTrimRule( const NCString& path,
                                         Inclusion       includeString   = Inclusion::Exclude,
                                         int             trimOffset      = 0,
                                         Case            sensitivity     = Case::Ignore,
                                         const NString&  trimReplacement = NullNString(),
                                         Reach           reach           = Reach::Global,
                                         Priorities      priority        = Priorities::DefaultValues )

        {
            scopeInfo.SetSourcePathTrimRule( path, includeString, trimOffset, sensitivity,
                                             trimReplacement, reach, priority );
        }


        /** ****************************************************************************************
         * Removes all local trimming rules set with #SetSourcePathTrimRule.
         * If parameter \p{global} is set to \b Inclusion::Include, the global rules are cleared
         * in addition.
         *
         * Setting parameter \p{allowAutoRule} to \c false, allows to suppress the creation of an
         * automatic rule based on the executables path.
         *
         * \see [ALox User Manual](https://alexworx.github.io/ALox-Logging-Library/manual.html)
         *      for more information.
         *
         * @param reach         Denotes whether only local rules are cleared or also global ones.
         *                      Defaults to \b %Reach::Global.
         * @param allowAutoRule Determines if an auto rule should be tried to be detected next
         *                      no appropriate rule is found.
         ******************************************************************************************/
        void      ClearSourcePathTrimRules( Reach       reach           = Reach::Global,
                                            bool        allowAutoRule   = true                  )
        {
            scopeInfo.SetSourcePathTrimRule( nullptr, allowAutoRule ? Inclusion::Include
                                                                    : Inclusion::Exclude,
                                             999999, // code for clearing
                                             Case::Ignore, NullNString(), reach, Priorities::NONE  );
        }

        /** ****************************************************************************************
         * This static method creates a console logger. To decide which logger type to choose,
         * configuration variable [ALOX_CONSOLE_TYPE](http://alexworx.github.io/ALox-Logging-Library/group__GrpALoxConfigVars.html) is checked.
         * If this variable is not set, then the decision is made as follows:
         * - On GNU/Linux OS, a
         *   \ref aworx::lib::lox::loggers::AnsiConsoleLogger "AnsiConsoleLogger" is chosen.
         * - On Windows OS, if a console window is attached, type
         *   \ref aworx::lib::lox::loggers::WindowsConsoleLogger "WindowsConsoleLogger" is chosen. If
         *   no console is attached to the process, instead a
         *   \ref aworx::lib::lox::loggers::ConsoleLogger "ConsoleLogger" is returned.
         *
         *
         * @param name The name of the \e Logger. Defaults to nullptr, which implies standard
         *             logger names defined in the \e Logger sub-classes.
         *
         * @return An instance of the chosen console type logger.
         ******************************************************************************************/
        ALIB_API
        static detail::textlogger::TextLogger*  CreateConsoleLogger( const NString& name= nullptr );

        /** ****************************************************************************************
         * Retrieves an instance of a Logger by its name. This might be useful when access to a
         * \e %Logger is needed to change its configuration.
         *
         * @param loggerName    The name of the \e Logger to search for (case insensitive).
         * @return  The logger, nullptr if not found.
         ******************************************************************************************/
        ALIB_API
        detail::Logger*   GetLogger( const NString& loggerName );

        /** ****************************************************************************************
         * Removes a logger from this container.
         * \note
         *  To (temporarily) disable a logger without removing it, a call to
         *  \ref SetVerbosity(detail::Logger*,Verbosity,const NString&,Priorities) "SetVerbosity(logger, Verbosity::Off)"
         *   can be used.
         *
         * @param logger   The logger to be removed.
         * @returns \c true, if the \e Logger was found and removed, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool            RemoveLogger( detail::Logger* logger );

        /** ****************************************************************************************
         * Removes logger named \p{loggerName} from this container.
         * \note
         *  To (temporarily) disable a logger without removing it, a call to
         *  \ref SetVerbosity(detail::Logger*,Verbosity,const NString&,Priorities) "SetVerbosity(logger, Verbosity::Off)"
         *   can be used.
         *
         * @param loggerName  The name of the \e Logger(s) to be removed (case insensitive).
         * @returns The logger that was removed, \c nullptr if not found.
         ******************************************************************************************/
        ALIB_API
        detail::Logger*   RemoveLogger( const NString& loggerName );

        /** ****************************************************************************************
         * Sets the \e %Verbosity of the <em>Log Domain</em> which is evaluated from parameter
         * \p{domain} and applicable <em>Scope Domains</em>. The \p{verbosity} given is set
         * recursively for all sub-domains.
         *
         * With the first invocation of this method for a distinct \p{logger}, this \e %Logger
         * is registered with this \e %Lox. In this case, prior to setting the given \e Verbosity
         * for the evaluated sub-domain, the \e Verbosity for all domains is set to
         * \b %Verbosity::Off.
         *
         * To deregister a \e Logger with a \b Lox, use method #RemoveLogger.
         * To 'disable' a \e Logger, invoke this method with parameters \p{verbosity} equaling to
         * \b %Verbosity::Off and \p{domain} to \c "/".
         *
         * Optional parameter \p{priority} defaults to
         * \ref aworx::lib::config::Priorities "Priorities::DefaultValues",
         * which is a lower priority than those of the standard plug-ins of external configuration
         * data. Therefore, external configuration by default 'overwrite' settings made from
         * 'within the source code', which simply means by invoking this method.<br>
         * The parameter can be provided for two main reasons:
         * - To 'lock' a verbosity setting against external manipulation.
         * - to 'break' the standard mechanism that an invocation of this method sets all
         *   sub-domains recursively. If a sub-domain was set with a higher priority
         *   (e.g. <c>%Config::PriorityOf(Priorities::DefaultValues) + 1</c>, then this sub-domain will not be affected
         *   by future invocations of this method with standard-priority given.
         *
         * For more information on how to use external configuration variables with priority and
         * on protecting verbosity settings, consult the
         * [ALox User Manual](https://alexworx.github.io/ALox-Logging-Library/manual.html).
         *
         * \attention
         *   The same as with most interface methods of this class, the given \p{domain} parameter is
         *   combined with <em>%Scope Domains</em> set for the caller's \e %Scope. In standard use
         *   cases of %ALox, the \e %Verbosity of a \e Domain is set using absolute domain path
         *   addressing. Therefore, it is recommended to have any domain path passed to this method
         *   starting with <c> '/'</c>, which suppresses the concatenation of <em>%Scope Domains</em>.<br>
         *   This is why this parameter with this method defaults to <c> '/'</c>, while with other
         *   methods of this class, it defaults to an empty string.
         * <p>
         * \attention
         *   Even when using an absolute domain path, <em>%Scope Domains</em> of
         *   \e %Scope::ThreadInner, will still apply. This means that from within a thread that
         *   has such <em>%Scope Domain</em> set, this method is (almost) not usable!
         *   This all aligns with the concept (advice), that \e Loggers and their \e %Verbosity
         *   are generally set outside of such scopes, hence in configuration sections of a
         *   process.<p>
         * \attention
         *   Consequently, this method may be (mis-) used to modify the 'actual' (default) scope
         *   when explicitly giving an empty string with parameter \p{domain}. This is useful, to
         *   temporarily adjust a scope. But remember: \alox was designed to avoid temporary
         *   code lines...
         *
         * @param logger     The logger to be to be affected (case insensitive).
         * @param verbosity  The 'level of verboseness' to be set.
         * @param domain     The parent (start) domain to be set. The use of absolute paths
         *                   starting with <c> '/'</c> are recommended.
         *                   Defaults to root domain \"/\".
         * @param priority   The priority of the setting. Defaults to
         *                   \ref aworx::lib::config::Priorities "Priorities::DefaultValues".
         ******************************************************************************************/
        ALIB_API
        void            SetVerbosity( detail::Logger*    logger,
                                      Verbosity        verbosity,
                                      const NString&   domain        = "/",
                                      Priorities       priority      = Priorities::DefaultValues  );

        /** ****************************************************************************************
         * Same as \ref #SetVerbosity(detail::Logger*,Verbosity,const NString&,Priorities) "SetVerbosity"
         * but addressing the \e %Logger to manipulate by its name.<br>
         * This method may only be used after a \e %Logger was once 'registered' with this \b %Lox
         * using \ref #SetVerbosity(detail::Logger*,Verbosity,const NString&,Priorities) "SetVerbosity".
         *
         * @param loggerName The logger to be to be affected, identified by its name (case
         *                   insensitive).
         * @param verbosity  The 'level of verboseness' to be set.
         * @param domain     The parent (start) domain to be set. The use of absolute paths
         *                   starting with <c> '/'</c> are recommended.
         *                   Defaults to root domain \"/\".
         * @param priority   The priority of the setting. Defaults to
         *                   \ref aworx::lib::config::Priorities "Priorities::DefaultValues".
         ******************************************************************************************/
        ALIB_API
        void            SetVerbosity( const NString&   loggerName,
                                      Verbosity        verbosity,
                                      const NString&   domain        = "/",
                                      Priorities       priority      = Priorities::DefaultValues  );

        /** ****************************************************************************************
         * The given \p{scopeDomain} becomes the default domain path for given \p{scope}.
         * This means, that any subsequent log invocations (from within this same scope) can omit
         * the domain parameter, or if they provide one, this Scope Domain path is prepended.
         * If subsequent log calls specify a domain name with a leading '/' character,
         * then the Scope Domain of the scope is ignored.<br>
         * Furthermore, if the given scope is an inner scope, outer scopes are prepended to the
         * given \p{scopeDomain} when the resulting domain of a log invocation is evaluated.
         * Again, this behavior can be overruled by prepending a leading '/' character to
         * \p{scopeDomain}.
         *
         * To remove a previously set Scope Domain a \e nulled or empty string has to be passed with
         * parameter \p{scopeDomain}.
         * For \e %Scope::ThreadOuter and \e %Scope::ThreadInner, passing an empty or \e nulled
         * string removes the most recently added domain path. For removing an explicitly named
         * domain path of \e %Scope::ThreadOuter and \e %Scope::ThreadInner use method
         * #RemoveThreadDomain.
         *
         * \note  The C++ version of \alox implements scope mechanisms using scope information
         *        generated by the preprocessor. By default, debug logging supports such caller
         *        information, release logging does not. This can be changed. For more information
         *        on how to change such defaults, see \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI
         *        in section \ref GrpALibCodeSelectorSymbols.
         *
         * @param scopeDomain The domain path to register.
         * @param scope       The scope that should the given \p{domain} be registered for.
         *                    Available Scope definitions are platform/language dependent.
         ******************************************************************************************/
        void        SetDomain( const NString& scopeDomain, Scope scope  )
        {
            setDomainImpl( scopeDomain, scope, false, nullptr );
        }

        /** ****************************************************************************************
         * This overloaded version of
         * \ref SetDomain(const NString&,Scope) "SetDomain" is applicable only for
         * \e %Scope::ThreadOuter and \e %Scope::ThreadInner and allows to specify the thread that
         * the setting should be associated with.
         *
         * If \p{scopeDomain} is nullptr or empty, the most recently added domain path is removed.
         * For removing an explicitly named domain associated with  a thread use method
         * #RemoveThreadDomain.
         *
         * @param scopeDomain The domain path to register.
         * @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
         *                    values, an internal error is logged.
         * @param thread      The thread to set/unset a thread-related Scope Domains for.
         ******************************************************************************************/
        void        SetDomain( const NString& scopeDomain, Scope scope, Thread* thread )
        {
            if ( !isThreadRelatedScope( scope ) )
                return;
            setDomainImpl( scopeDomain, scope, false, thread );
        }

        /** ****************************************************************************************
         * Adds a <em>Domain Substitution Rule</em>.
         * <em>Domain Substitution</em> is performed as a last step when evaluating the domain path of a <em>Log Statement</em>,
         * taking <em>Scope Domains</em> and the optional parameter \p{domain} of the statement into
         * account.<br>
         *
         * <b>Wildcards</b><br>
         * Parameter \p{domainPath} supports \b 'wildcard' character <c> '*'</c> at its beginning
         * and at its end (or both). This allows to have four types of rules:
         * - Exact match
         * - Prefix match (\c * at the end of \p{domainPath})
         * - Suffix match (\c * at the start of \p{domainPath})
         * - Sub-string match (\c * at both, start and the end of \p{domainPath})
         *
         * Only minimal checks are performed, e.g. if an exact match is requested, but \p{domainPath}
         * does not start with character <c> '/'</c>. In this and some other cases, the rule is not
         * stored and an internal warning is logged. Further checks, for example for illegal
         * domain path characters are not performed (those will be eliminated when the resulting
         * domain path is to be created internally).
         *
         * <b>Circular Dependencies</b><br>
         * If the given rules have circular dependencies, only a limited number (ten) replacements
         * are performed. If this number of replacements for one <em>Log Statement</em> is exceeded, an internal
         * warning message is logged. This is done only \e once over the life-time of a \e Logger.
         *
         * <b>Application of Rules</b><br>
         * Rules are applied in the order of their definition. After all rules have been applied
         * this is repeated as long as at least one rule matched (up to ten times).
         *
         * <b>Deletion of Rules</b>
         * To delete a rule, invoke the method with same parameter \p{domainPath} and a \e 'nulled'
         * or empty string for parameter \p{replacement}.
         * To delete all rules, invoke the method with parameter \p{domainPath} \e 'nulled'
         * or empty.
         *
         * <b>Final remarks</b>
         * Domain substitution is useful to permanently change ('redirect') domain paths of
         * 3rd party code (e.g. libraries using \alox) or log statements that must not be changed
         * for other reasons. It is advised to not 'overuse' this feature, as side effects
         * are inherent to the concept of <em>Domain Substitution</em>. For example, an unwanted
         * side effect might be that <em>Prefix Logables</em> are not applicable to the substituted
         * domain, while other <em>Prefix Logables</em> are bound to the resulting domain.
         *
         * For \b %Lox objects that should be protected of external manipulation, it is advisable,
         * to remove all <em>Domain Substitution Rules</em> right after the \b %Lox was created by
         * invoking this method with a \e nulled value for parameter \p{domainPath}.
         * The reason is, that otherwise, through configuration files or command line parameters,
         * domains of the \b %Lox can be substituted and then the resulting domains \e Verbosities
         * be \e overwritten using further configuration variables.
         * Any prioritized \e 'internal' setting of \e Verbosities this way could be circumvented!
         *
         * For more information consult the
         * [ALox User Manual](https://alexworx.github.io/ALox-Logging-Library/alox_man_domain_substitution.html).
         *
         * @param domainPath  The path to search. Has to start with either  <c> '/'</c> or <c> '*'</c>.
         * @param replacement The replacement path.
         ******************************************************************************************/
        ALIB_API
        void     SetDomainSubstitutionRule( const NString& domainPath, const NString& replacement );

        /** ****************************************************************************************
         * This method is used to remove an <em>explicitly given</em> domain path from the list
         * of domain paths set for \e %Scope::ThreadOuter or \e %Scope::ThreadInner.
         *
         * To remove the most recently added domain path from such thread-related \e %Scope,
         * use one of the overloaded methods #SetDomain and provide an empty or \e nulled
         * value for parameter \p{scopeDomain} (the same as how domain paths of other \e %Scopes
         * are removed).
         *
         * \note
         *   The long name of the method already indicates that this method is a little special.
         *   Only seldom, more than one <em>%Scope Domain</em> is needed to be added. And if this
         *   is needed, then such <em>%Scope Domains</em> usually get removed in reverse order of
         *   their definition, with is performed using the standard interface that allows 'removing'
         *   any other <em>%Scope Domain</em>. (Passing an empty or \e nulled domain
         *   path to method #SetDomain.)
         *
         * @param scopeDomain The domain path to register.
         * @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
         *                    values, an internal error is logged.
         * @param thread      The thread to set/unset a thread-related Scope Domains for.
         *                    Defaults to the current thread.
         ******************************************************************************************/
        ALIB_API
        void        RemoveThreadDomain( const NString& scopeDomain, Scope scope, Thread* thread= nullptr );

        /** ****************************************************************************************
         * The given \p{prefix} becomes a <em>Prefix Logable</em> provided to loggers with each log
         * statement executed within the given \p{scope}.
         * The list of objects received by a logger is sorted from outer scope to inner scope.
         * The logable of the <em>%Log Statement</em> itself, is the last in the list, except one or
         * more <em>Prefix Logables</em> of \e %Scope::ThreadInner are set. Those are (similar to how
         * this \e %Scope is used with <em>%Scope Domains</em>) appended to the end of the
         * list.
         *
         * To remove a previously set <em>Prefix Logable</em>, \c nullptr has to be passed with
         * parameter \p{prefix}.
         * For \e %Scope::ThreadOuter and \e %Scope::ThreadInner, passing \c nullptr (respectively
         * with the overloaded method accepting string messages, a \e nulled string)
         * removes the most recently added <em>Prefix Logable</em>.
         *
         *
         * \note
         *   \e Logables of boxed character array types are duplicated internally by \alox when
         *   setting as <em>Prefix Logables</em>.
         *   This means, in contrast to other types, for string-type <em>Prefix Logables</em>
         *   the life-cycle of the object passed in parameter \p{prefix} is allowed to end
         *   right after the invocation of this method. This is a convenience feature of \alox.
         *   However, this also means, that changes of the strings that occur after the string
         *   objects got set as a <em>Prefix Logable</em>, are \b not reflected.<br>
         *   To implement a "variable" <em>Prefix Logable</em> of string-type, an object of type
         *   \b %AString might be passed wrapped in class \c std::reference_wrapper<AString>.<br>
         *   For more information consult the
         *   [ALox User Manual](https://alexworx.github.io/ALox-Logging-Library/alox_man_prefix_logables.html#alox_man_prefix_logables_cppspecifics).
         *   as well as chapter \ref alib_boxing_customizing_identity of the Programmer's Manual
         *   of module \alibmod_boxing.
         *<p>
         * \note
         *   Unlike other methods of this class which accept an arbitrary amount of logables, this
         *   method and its overloaded variants accept only one logable (the prefix).
         *   To supply several objects to be prefix logables at once, a container of type
         *   \ref aworx::lib::boxing::Boxes "Boxes" may be passed with parameter \p{logables}, like
         *   in the following sample:
         *   \snippet "ut_alox_log_scopes.cpp"      DOX_ALOX_LOX_SETPREFIX
         *   The provided container as well as the prefixes themselves have to be kept in memory
         *   until they are unset.
         *
         *<p>
         * \note
         *   The C++ version of \alox implements scope mechanisms using scope information
         *   generated by the preprocessor. By default, debug logging supports such caller
         *   information, release logging does not. This can be changed. For more information
         *   on how to change such defaults, see \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI
         *   in section \ref GrpALibCodeSelectorSymbols.
         *
         *<p>
         * \note
         *   The word 'prefix' in this method's name and in the name of \alox feature
         *   <em>Prefix Logables</em> is chosen for the fact that with text loggers (which is the
         *   most widely applied use case for \alox) such objects are prefixes to the log
         *   message. Of-course, with using \e %Scope::ThreadInner, this turns into a suffix!<br>
         *   When using \alox to process arbitrary objects instead of text messages, the concept of
         *   <em>Prefix Logables</em> is still very useful. Just the name does not fit so well anymore.
         *   Think of 'SetContext' and <em>Context Objects</em> instead.
         *
         * @param prefix      The <em>Prefix Logable</em> to set.
         * @param scope       The scope that should the given \p{domain} be registered for.
         *                    Available Scope definitions are platform/language dependent.
         ******************************************************************************************/
        void        SetPrefix( const Box& prefix, Scope scope )
        {
            setPrefixImpl( prefix, scope, nullptr );
        }

        /** ****************************************************************************************
         * This overloaded version of
         * \ref SetPrefix(const Box&,Scope) "SetPrefix" is applicable only for
         * \b %Scope::ThreadOuter and \b %Scope::ThreadInner and allows to specify the thread that
         * the setting should be associated with.
         *
         * If \p{scopeDomain} is nullptr or empty, the most recently added <em>Prefix Logable</em>
         * is removed.
         *
         * @param prefix      The <em>Prefix Logable</em> to set.
         * @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
         *                    values, an internal error is logged.
         * @param thread      The thread to set/unset a thread-related Scope Domains for.
         ******************************************************************************************/
        void        SetPrefix( const Box& prefix, Scope scope, Thread* thread )
        {
            setPrefixImpl( prefix, scope, thread );
        }

        /** ****************************************************************************************
         * The given \p{prefix} becomes a <em>Prefix Logable</em> associated to the given
         * <em>Log Domain</em>.
         * <em>Prefix Logables</em> associated with the <em>Log Domain</em> are added to the
         * list of \e Logables right
         * before the main \e Logable of the <em>Log Statement</em> itself.
         * Multiple <em>Prefix Logables</em> can be added per <em>Log Domain</em>.
         *
         * To remove the most recently added <em>Prefix Logable</em> associated with a <em>Log Domain</em>,
         * \c nullptr has to be passed with parameter \p{prefix}.
         *
         * \note
         *   String-type \e Logables are duplicated internally by \alox when setting as
         *   <em>Prefix Logables</em>.
         *   This means, different to <em>Prefix Logables</em> of type \b %AString or custom types,
         *   the life-cycle of the object passed in parameter \p{prefix} is allowed to end
         *   right after the invocation of this method.
         *
         * \attention
         *   The same as with most interface methods of this class, the given \p{domain} parameter is
         *   combined with <em>%Scope Domains</em> set for the caller's \e %Scope.
         *   To suppress this, an absolute domain path can be used. (Still any <em>Scope Domain</em> of
         *   \e %Scope::Thread::Inner will be applied).
         *   The default value of parameter \p{domain} is \c "" which addresses the domain evaluated
         *   for the current scope.
         *
         * @param prefix      The <em>Prefix Logable</em> to set.
         * @param domain      The domain path. Defaults to \c nullptr, resulting in
         *                    evaluated <em>Scope Domain</em> path.
         * @param otherPLs    If set to \c Inclusion::Exclude, scope-related <em>Prefix Logables</em>
         *                    are ignored and only domain-related <em>Prefix Logables</em> are passed to
         *                    the \e Loggers.
         *                    Defaults to \c Inclusion::Include.
         ******************************************************************************************/
        ALIB_API
        void        SetPrefix( const Box& prefix, const NString& domain= nullptr,
                               Inclusion otherPLs=  Inclusion::Include );

        /** ****************************************************************************************
         * This method is used reset (or to explicitly set) the start time of one or all logger(s).
         * The only impact is the output of time differences in the log lines. Hence, this method
         * is useful to reset them and see some absolute time values when doing basic performance
         * tests using the \e Logger.
         *
         * \note This affects loggers that are registered for at least one standard domain.
         *       In other words, loggers that are exclusively attached to the internal domain,
         *       will not be affected.
         *
         * @param startTime  Optional parameter with the new start time. Defaults
         *                   to current time if omitted.
         * @param loggerName The name of the \e Logger(s) whose start time is to be set
         *                   (case insensitive).
         *                   Defaults to nullptr, which indicates that all loggers are to
         *                   be affected.
         ******************************************************************************************/
        ALIB_API
        void SetStartTime( Ticks           startTime    = lib::time::Ticks    (),
                           const NString&  loggerName   = nullptr                 );

        #if defined (__GLIBCXX__) || defined(__APPLE__)
            /** ************************************************************************************
             * Converts the given \p{startTime} and invokes #SetStartTime(Ticks,const NString&).
             * \note  GLib specific.
             *
             * @param startTime  The new start time in system specific time unit.
             * @param loggerName The name of the \e Logger whose start time is to be set (case
             *                   insensitive).
             *                   Defaults to empty string, which indicates that all loggers are to
             *                   be affected.
             **************************************************************************************/
            ALIB_API
            void SetStartTime( time_t startTime, const NString& loggerName= nullptr );


        #endif // no elif here, otherwise doxygen would ignore it!

        #if defined( _MSC_VER )
            /** ************************************************************************************
             * Converts the given \p{startTime} and invokes#SetStartTime(Ticks,const NString&).
             * \note  Microsoft Windows specific.
             *
             * @param startTime  The new start time in system specific time unit.
             * @param loggerName The name of the \e Logger whose start time is to be set (case
             *                   insensitive).
             *                   Defaults to empty string, which indicates that all loggers are to
             *                   be affected.
             **************************************************************************************/
            ALIB_API
            void    SetStartTime( const FILETIME& startTime, const NString& loggerName= nullptr );

        #endif

        /** ****************************************************************************************
         * This method sets a human readable name to the given thread ID (or current thread) which
         * is optionally included in each log line.
         *
         * @param threadName    The name of the thread as it should be displayed in the logs
         * @param id            (Optional) Parameter providing the thread ID. If omitted, the
         *                      current thread's ID is used.
         ******************************************************************************************/
        ALIB_API
        void        MapThreadName( const String& threadName, ThreadID id= 0 );

        /** ****************************************************************************************
         * Stores data encapsulated in an object of class
         * \ref aworx::lib::boxing::Box "Box" which can be retrieved back by invoking
         * #Retrieve. Using the optional \p{key} and \p{scope} offer various possibilities to
         * reference this data later.
         *
         * To remove data from the store, pass \c nullptr with parameter \p{data}.
         *
         * \attention
         *  When data objects are 'overwritten', previous objects will be deleted internally.
         *  Hence, only pointers to heap-allocated objects (created with \c new) may be passed!<br>
         *  For more information, consult the
         *  [ALox User Manual](https://alexworx.github.io/ALox-Logging-Library/manual.html).
         *
         * \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
         *       It is not advised to use <em>Log Data</em> to implement application logic.
         *
         * @param data      The data object to store.
         *                  If \c nullptr, currently stored data will be removed.
         *                  In C++, has to be heap allocated and will be deleted
         *                  by this \b %Lox when overwritten or this lox is deleted.
         * @param key       The optional key to the data.
         *                  If omitted (or empty or nullptr), the data is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  data is unique to the \e Lox.
         * @param scope     The \e %Scope that the data is bound to.
         ******************************************************************************************/
        inline
        void        Store( const Box& data,   const NString& key,
                           Scope      scope= Scope::Global )
        {
            storeImpl( data, key, scope );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * Store(const Box&,const String&,Scope,int) "Store" which omits parameter \p{key}.
         * @param data      The data object to store.
         *                  In C++, has to be heap allocated and will be deleted
         *                  by this \b %Lox when overwritten or this lox is deleted.
         * @param scope     The \e %Scope that the data is bound to.
         ******************************************************************************************/
        inline
        void        Store( const Box& data, Scope scope= Scope::Global )
        {
            storeImpl( data, nullptr, scope );
        }

        /** ****************************************************************************************
         * Retrieves \alox <em>Log Data</em>, an object type
         * \ref aworx::lib::boxing::Box "Box" which had been stored in a prior call to
         * #Store. Using the optional \p{key} and \p{scope} offer various possibilities to reference
         * such objects.<br>
         *
         * \note If no data is found, an \e nulled object is returned. This can be tested using method
         *       \ref aworx::lib::boxing::Box::IsNull "Box::IsNull".
         *
         * <p>
         * \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
         *       It is not advised to use <em>Log Data</em> to implement application logic.
         *
         * @param key       The optional key to the data.
         *                  If omitted (or empty or nullptr), the data is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  data is unique to the \e Lox.
         * @param scope     The \e %Scope that the data is bound to.
         * @return The data, a \e nulled box if no value was found.
         ******************************************************************************************/
        ALIB_API
        Box       Retrieve  ( const NString& key, Scope scope= Scope::Global )
        {
            return retrieveImpl( key, scope );
        }

        /** ****************************************************************************************
         * * Overloaded version of #Retrieve which omits parameter \p{key}.
         *
         * \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
         *       It is not advised to use <em>Log Data</em> to implement application logic.
         *
         * @param scope     The \e %Scope that the data is bound to.
         * @return The data, a \e nulled box if no value was found.
         ******************************************************************************************/
        ALIB_API
        Box  Retrieve  ( Scope scope= Scope::Global )
        {
            return retrieveImpl( nullptr, scope );
        }

        /** ****************************************************************************************
         * This method logs the current configuration of this Lox and its encapsulated objects.
         * It uses method #GetState to assemble the logable string.
         *
         * \note
         *   As an alternative to (temporarily) adding an invocation of <b>%Lox.State</b> to
         *   your code, \alox provides configuration variable
         *   [ALOX_LOXNAME_DUMP_STATE_ON_EXIT](group__GrpALoxConfigVars.html).
         *   This allows to enable an automatic invocation of this method using external
         *   configuration data like command line parameters, environment variables or
         *   INI files.
         *
         * @param domain        Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
         *                      set for the \e %Scope of invocation.
         * @param verbosity     The verbosity.
         * @param headLine      If given, a separated headline will be logged at first place.
         * @param flags         Flag bits that define which state information is logged.
         ******************************************************************************************/
        ALIB_API
        void        State   ( const NString&    domain,
                              Verbosity         verbosity,
                              const String&     headLine,
                              StateInfo         flags        = StateInfo::All  );

        /** ****************************************************************************************
         * This method collects state information about this lox in a formatted multi-line AString.
         * Parameter \p{flags} is a bitwise enum type (operators on elements available).
         *
         * \note
         *   As an alternative to (temporarily) adding an invocation of <b>%Lox.State</b> to
         *   your code, \alox provides configuration variable
         *   [ALOX_LOXNAME_DUMP_STATE_ON_EXIT](group__GrpALoxConfigVars.html).
         *   This allows to enable an automatic invocation of this method using external
         *   configuration data like command line parameters, environment variables or
         *   INI files.
         * @param buf        The target string.
         * @param flags      Bits that define which state information is collected.
         ******************************************************************************************/
        ALIB_API
        void        GetState( NAString& buf, StateInfo flags= StateInfo::All   );


    // #############################################################################################
    // Main logging methods
    // #############################################################################################
        /** ****************************************************************************************
         * Returns a reference to a list of boxes to be used for logging. The list is recycled
         * from a previous log operation and cleared. The method may be used to retrieve
         * a container for logables that then are collected until finally logged.<br>
         * Note that the \b %Lox instance has to be acquired prior to invoking this method and
         * the container returned must be used only while the object is still acquired.
         *
         * With each recursive acquirement of this object, a different container is returned.
         * This is implemented to allow recursive log calls.
         *
         * @return An empty list of boxes.
         ******************************************************************************************/
        ALIB_API
        Boxes&  GetLogableContainer();



        /** ****************************************************************************************
         * Logs the current list of \e Logables that previously have been received using
         * #GetLogableContainer with the given \p{verbosity}.
         *
         * This method is usually \b not used directly. Instead, methods
         * #Info, #Verbose, #Warning and #Error provide simpler interfaces which take variadic
         * arguments that are collected in a list of boxed objects and then passed to
         * this methods.<br>
         * Note that the other interface methods accept an "external" list of boxes as a parameter.
         * as well. This means also with these methods it is allowed to collect the logables in
         * an user-specific list first and later pass them to these methods.
         *
         * Hence, the use of this method is recommended only if the verbosity of a log statement
         * is is evaluated only at run-time.
         *
         * @param domain        The domain.
         * @param verbosity     The verbosity.
         ******************************************************************************************/
        ALIB_API
        void Entry( const NString&  domain, Verbosity verbosity );



        /** ****************************************************************************************
         * Logs a list of \e Logables with the given \e %Verbosity.
         *
         * If more than one \e Logable is given and the first one is of string type and comprises a
         * valid domain name, then this first argument is interpreted as a the domain name!
         * Valid domain names are strings that consists only of characters of the following set:
         * - upper case letters,
         * - numbers,
         * - hyphen (\c '-') and
         * - underscore (\c '_').
         *
         * If a first \e Logable could be misinterpreted as being a domain name, an empty string
         * (the "neutral" domain) has to be added as a first argument. Alternatively, a character
         * which is illegal in respect to domain names could be added to the first argument,
         * for example a simple space at the end of an output string.
         *
         * \note
         *   This method allows a consistent interface of overloaded methods \b %Info, \b Error,
         *   etc, without introducing a separate version which excepts a then mandatory domain
         *   parameter.
         *   The little drawback of the auto detection is the possibility of ambiguous invocations.
         *
         * @param verbosity The verbosity.
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        void EntryDetectDomain( Verbosity verbosity,  BoxedObjects&&...  logables )
        {
            GetLogableContainer().Add( std::forward<BoxedObjects>(logables)... );
            entryDetectDomainImpl( verbosity );
        }

        /** ****************************************************************************************
         * Logs given logables using \ref Verbosity::Verbose.
         *
         * The first object provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void Verbose( BoxedObjects&&... logables )
        {
            EntryDetectDomain( Verbosity::Verbose, std::forward<BoxedObjects>(logables)... );
        }

        /** ****************************************************************************************
         * Logs given logables using \ref Verbosity::Info.
         *
         * The first object provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void Info( BoxedObjects&&... logables )
        {
            EntryDetectDomain( Verbosity::Info,  std::forward<BoxedObjects>(logables)... );
        }

        /** ****************************************************************************************
         * Logs given logables using \ref Verbosity::Warning.
         *
         * The first object provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void Warning( BoxedObjects&&... logables )
        {
            EntryDetectDomain( Verbosity::Warning,  std::forward<BoxedObjects>(logables)... );
        }

        /** ****************************************************************************************
         * Logs given logables using \ref Verbosity::Error.
         *
         * The first object provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void Error( BoxedObjects&&... logables )
        {
            EntryDetectDomain( Verbosity::Error,  std::forward<BoxedObjects>(logables)... );
        }

        /** ****************************************************************************************
         * Logs given logables only if the parameter \p{condition} is not \c true.
         * If executed, \ref Verbosity::Error is used.
         *
         * The first object provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * @param condition If \c false, the <em>Log Statement</em> is executed.
         * @param logables  The list of \e Logables, optionally including a domain name at the start.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void Assert( bool condition, BoxedObjects&&... logables )
        {
            if (!condition )
            {
                EntryDetectDomain( Verbosity::Error,  std::forward<BoxedObjects>(logables)... );
            }
            else
                CntLogCalls++;
        }

        /** ****************************************************************************************
         * Logs a list of \e Logables only if the parameter \p{condition} is \c true.
         *
         * \see Method #Assert.
         *
         * @param condition If \c false, the <em>Log Statement</em> is executed.
         * @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
         *                  set for the \e %Scope of invocation.
         * @param verbosity The verbosity.
         * @param logables  The list of \e Logables.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void If( bool condition, const NString& domain, Verbosity verbosity, BoxedObjects&&... logables )
        {
            if ( condition )
            {
                GetLogableContainer().Add( std::forward<BoxedObjects>(logables)... );
                Entry( domain, verbosity );
            }
            else
                CntLogCalls++;
        }

        /** ****************************************************************************************
         * Logs a list of \e Logables only if the parameter \p{condition} is \c true.<br>
         *
         * This overloaded version omits parameter \p{domain}.
         * The first \e logable provided may be a domain name. All values are passed to
         * #EntryDetectDomain. See documentation of this method for information on how to avoid
         * ambiguities in respect to domain names.
         *
         * If one of the arguments (or a single argument given) is of type
         * \ref aworx::lib::boxing::Boxes "Boxes", then the contents of this list is inserted into
         * the list of logables. This allows to collect logables prior to invoking the method.
         *
         * \see Method #Assert.
         *
         * @param condition If \c false, the <em>Log Statement</em> is executed.
         * @param verbosity The verbosity.
         * @param logables  The list of \e Logables.
         ******************************************************************************************/
        template <typename... BoxedObjects>
        inline
        void If( bool condition, Verbosity verbosity, BoxedObjects&&... logables )
        {
            if ( condition )
            {
                EntryDetectDomain( verbosity, std::forward<BoxedObjects>(logables)... );
            }
            else
                CntLogCalls++;
        }


        /** ****************************************************************************************
         * Logs given \e logables once, up to \p{quantity} times, or every n-th time.
         * In its simplest overloaded version, the counter is bound to the source code line, hence,
         * only the first execution of this exact <em>Log Statement</em> is executed.
         *
         * With parameter \p{group}, a set of <em>Log Statements</em> that share the same group key,
         * can be grouped and of such set, only the one which is first executed actually logs.<br>
         * Alternatively, when \p{key} is omitted (or nullptr or empty), but a
         * \ref aworx::lib::lox::Scope "Scope" is given with parameter \p{scope}, then the
         * counter is associated with the scope.<br>
         * Finally, parameters \p{key} and \p{scope} can also be used in combination. The key is
         * then unique in respect to the \ref aworx::lib::lox::Scope "Scope" provided.
         *
         * Using, none, one or both of the parameters \p{group} and \p{scope}, among others, the
         * following use cases can be achieved.
         * - %Log a specific statement up to n-times.
         * - %Log only the first n of a group of statements.
         * - %Log only the first n statements within a method.
         * - %Log only the first n statements belonging to the same group and method .
         * - %Log only the first n statements within any method of
         *   - a source file
         *   - a directory of source files
         *   - a parent directory of source files and all sources recursively
         * - %Log only the first n statements which belong to the same group and are placed within
         *   any method of
         *   - a source file
         *   - a directory of source files
         *   - a parent directory of source files and all sources recursively
         * - %Log a <em>Log Statement</em> n-times per new thread.
         * - %Log only the first n statements of a group of statements executed by a specific thread.
         *
         * When parameter \p{quantity} is a negative value, the log statement is executed every n-th time
         * instead n-times. E.g, if \p{quantity} is \c -5, the first statement is executed and afterwards
         * every fifth invocation.
         *
         * \note
         *   Unlike other methods of this class which accept an arbitrary amount of logables, this
         *   method and its overloaded variants accept only one object. To supply several objects
         *   at once, a container of type \ref aworx::lib::boxing::Boxes "Boxes" may be passed with
         *   parameter \p{logables}, like in the following sample:
         *   \snippet "ut_alox_lox.cpp"      DOX_ALOX_LOX_ONCE
         *
         *
         * @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
         *                  set for the \e %Scope of invocation.
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param group     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em> itself.
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         *                  If negative, the first and every "-quantity-th" statement is executed.
         ******************************************************************************************/
        inline
        void Once( const NString&   domain  , Verbosity verbosity,
                   const Box&       logables,
                   const String&    group,
                   Scope            scope= Scope::Global,
                   int quantity= 1)
        {
            once( domain, verbosity, logables, group, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param group     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em> itself.
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                        Verbosity verbosity, const Box& logables,
                   const String& group,
                   Scope scope,
                   int quantity= 1)
        {
            once( nullptr, verbosity, logables, group, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param group     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em> itself.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                        Verbosity verbosity, const Box& logables,
                   const String& group,
                   int quantity= 1)
        {
            once( nullptr, verbosity, logables, group, Scope::Global, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                        Verbosity verbosity, const Box& logables,
                   int quantity= 1)
        {
            once( nullptr, verbosity, logables, nullptr, Scope::Global, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param group     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em> itself.
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                                          const Box& logables,
                   const String& group,
                   Scope scope,
                   int quantity= 1)
        {
            once( nullptr, Verbosity::Info, logables, group, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
         *                  set for the \e %Scope of invocation.
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once( const NString& domain, Verbosity verbosity, const Box& logables,
                   Scope scope= Scope::Global ,
                   int quantity= 1)
        {
            once( domain, verbosity, logables, nullptr, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                        Verbosity verbosity, const Box& logables,
                   Scope scope,
                   int quantity= 1)
        {
            once( nullptr, verbosity, logables, nullptr, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                                          const Box& logables,
                   Scope scope,
                   int quantity= 1)
        {
            once( nullptr, Verbosity::Info, logables, nullptr, scope, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                                          const Box& logables,
                   int quantity= 1)
        {
            once( nullptr, Verbosity::Info, logables, nullptr, Scope::Global, quantity );
        }

        /** ****************************************************************************************
         * Overloaded version of
         * \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
         *
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param group     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the \e %Scope
         *                  provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em> itself.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        inline
        void Once(                                          const Box& logables,
                   const String& group, int quantity= 1 )
        {
            once( nullptr, Verbosity::Info, logables, group, Scope::Global, quantity );
        }


    // #############################################################################################
    // Debug methods
    // #############################################################################################
    #if ALIB_DEBUG
        /** ****************************************************************************************
         * Returns the memory allocator of type \alib{memory,MemoryBlocks} used with the different
         * language-related scopes stores.
         *
         * Parameter \p{which} denotes the language store as follows:
         * 1. Scope domains
         * 2. Log-once information
         * 3. Prefix logables
         * 4. Stored log data.
         *
         * This method is available only in debug compilations.
         *
         * @param which  Denotes the language store to address.
         * @return The memory allocator. May be used to receive performance metrics.
         ******************************************************************************************/
        MemoryBlocks& DbgGetStoreAllocator( int which )
        {
            if( which == 1 )  return scopeDomains .languageStore->memoryBlocks;
            if( which == 2 )  return scopeLogOnce .languageStore->memoryBlocks;
            if( which == 3 )  return scopePrefixes.languageStore->memoryBlocks;
            return scopeLogData .languageStore->memoryBlocks;
        }
    #endif


    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

        /** ****************************************************************************************
         * Assembles the resulting domain from the given \p{domainPath} and the Scope Domain paths
         * (see #SetDomain) according to the scope identified by \p{scopeInfo}.
         * The resulting full domain string is assembled from inner to outer scope.
         * If \p{domainPath}, respectively as soon as any of the Scope Domain's paths
         * start with the character defined with
         * \ref aworx::lib::lox::detail::Domain::Separator "Domain::Separator",
         * the evaluation is stopped (the path is interpreted as absolute).
         *
         * @param domainPath The domain path. If starting with the character defined with
         *                   \ref aworx::lib::lox::detail::Domain::Separator "Domain::Separator",
         *                   no scope domains are applied.
         * @return The resulting \ref aworx::lib::lox::detail::Domain "Domain".
         ******************************************************************************************/
        ALIB_API
        detail::Domain*   evaluateResultDomain( const NString& domainPath );

        /** ****************************************************************************************
         * Invokes \b Find on the given domain and logs internal message when the domain was
         * not known before.
         *
         * @param domainSystem  The domain system. Either the standard or the internal one.
         * @param domainPath    The domain path.
         * @return The resulting \ref aworx::lib::lox::detail::Domain "Domain".
         ******************************************************************************************/
        ALIB_API
        detail::Domain*   findDomain( detail::Domain& domainSystem, NString domainPath );

        /** ****************************************************************************************
         * This method is looping over the \e Loggers, checking their verbosity against the given
         * one, and, if they match, invoke the log method of the \e Logger.
         * With the first logger identified to be active, the <em>Prefix Objects</em> get
         * collected from the scope store.
         * @param dom          The domain to log on
         * @param verbosity    The verbosity.
         * @param logables     The objects to log.
         * @param prefixes     Denotes if prefixes should be included or not.
         ******************************************************************************************/
        ALIB_API
        void      log( detail::Domain*  dom,       Verbosity verbosity,
                       Boxes&         logables,  Inclusion prefixes );


        /** ****************************************************************************************
         * Logs an internal error message using the internal domain tree as defined in
         * \ref aworx::lib::lox::ALox::InternalDomains "ALox::InternalDomains".
         *
         * @param verbosity The verbosity.
         * @param subDomain The sub-domain of the internal domain to log into.
         * @param msg       The message.
         ******************************************************************************************/
        ALIB_API
        void            logInternal( Verbosity verbosity, const NString& subDomain, Boxes& msg );

        /** ****************************************************************************************
         * Overloaded version accepting a string to log.
         *
         * @param verbosity The verbosity.
         * @param subDomain The sub-domain of the internal domain to log into.
         * @param msg       The message.
         ******************************************************************************************/
        ALIB_API
        void            logInternal( Verbosity verbosity, const NString& subDomain, const NString& msg );

        /** ****************************************************************************************
         * Returns a reference to a list of boxes to be used by internal logging.
         * Each invocation has to be followed by an invocation to #logInternal
         * which releases the logables.
         *
         * @return A list of boxes.
         ******************************************************************************************/
        Boxes&  acquireInternalLogables()
        {
            if( internalLogables.size() == internalLogRecursionCounter )
                internalLogables.emplace_back( new Boxes() );

            return *internalLogables[internalLogRecursionCounter++];
        }

        /** ****************************************************************************************
         * Implementation of the interface method fetching all possible parameters.
         *
         * @param scopeDomain The domain path to register.
         * @param scope       The scope that the given \p{domain} should be registered for.
         *                    Available Scope definitions are platform/language dependent.
         * @param removeNTRSD Used to remove a named thread-related Scope Domain (and is true only
         *                    when invoked by interface method #RemoveThreadDomain.
         * @param thread      The thread to set/unset a thread-related Scope Domain for.
         ******************************************************************************************/
        ALIB_API
        void      setDomainImpl( const NString& scopeDomain, Scope scope,
                                 bool removeNTRSD, Thread* thread  );

        /** ****************************************************************************************
         * Implementation of the interface method fetching all possible parameters.
         *
         * @param prefix      The <em>Prefix Logable</em> to set.
         * @param scope       The scope that the given \p{logable} should be registered for.
         *                    Available Scope definitions are platform/language dependent.
         * @param thread      The thread to set/unset a thread-related <em>Prefix Logable</em> for.
         ******************************************************************************************/
        ALIB_API
        void      setPrefixImpl( const Box& prefix, Scope scope, Thread* thread );

        /** ****************************************************************************************
         * The implementation of #EntryDetectDomain.
         *
         * @param verbosity     The verbosity.
         ******************************************************************************************/
        ALIB_API
        void entryDetectDomainImpl( Verbosity verbosity);

        /** ****************************************************************************************
         * Internal method serving public interface #Once.
         *
         * @param domain    Optional <em>Log Domain</em> which is combined with
         *                  <em>%Scope Domains</em> set for the \e %Scope of invocation.
         * @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
         * @param logables  The objects to log (Multiple objects may be provided within
         *                  container class Boxes.)
         * @param pGroup     The optional name of the statement group . If used, all statements that
         *                  share the same group name are working on the same counter (according
         *                  to the \p{scope}.)
         *                  If omitted (or empty or nullptr), the counter is is bound to the
         *                  \e %Scope provided. If omitted and \p{scope} is Scope::Global, then the
         *                  counter is associated exclusively with the single <em>Log Statement</em>
         *                  itself.
         * @param scope     The \e %Scope that the group or counter is bound to.
         * @param quantity  The number of logs to be performed. As the name of the method indicates,
         *                  this defaults to \c 1.
         ******************************************************************************************/
        ALIB_API
        void once( const NString& domain, Verbosity verbosity, const Box& logables,
                   const String& pGroup,  Scope     scope, int quantity );

        /** ****************************************************************************************
         * Internal method serving public interface #Store.
         *
         * @param data      The data object to store.
         *                  In C++, has to be heap allocated and will be deleted
         *                  by this \b %Lox when overwritten or this lox is deleted.
         * @param pKey       The key to the data.
         * @param scope     The \e %Scope that the data is bound to.
         ******************************************************************************************/
        ALIB_API
        void      storeImpl( const Box& data, const NString& pKey, Scope scope );

        /** ****************************************************************************************
         * Internal method serving public interface #Retrieve.
         *
         * @param pKey       The key to the data.
         * @param scope     The \e %Scope that the data is bound to.
         * @return The data, a \e nulled box if no value was found.
         ******************************************************************************************/
        ALIB_API
        Box  retrieveImpl  ( const NString& pKey, Scope scope );

        /** ****************************************************************************************
         * Checks if given scope is thread-related.
         *
         * @param scope     The scope that is to be checked.
         * @return \c true if \p{scope} is thread-related, \c false else.
         ******************************************************************************************/
        ALIB_API
        bool      isThreadRelatedScope( Scope scope );

        /** ****************************************************************************************
         * Checks if given scope needs information that is not available. In addition, the
         * in/out parameter \p{scope} is changed to \b Scope::Path, in case a level was added.
         * That level is returned.
         *
         *
         * @param[in,out] scope  A reference to the scope that is to be checked (and eventually
         *                       modified.
         * @param internalDomain The internal sub-domain to log any error/warning into.
         * @return A posititve value providing the path level deducted from \p{scope} if all is fine,
         *        \c -1 else.
         ******************************************************************************************/
        ALIB_API
        int       checkScopeInformation( Scope& scope, const NString& internalDomain );

        /** ****************************************************************************************
         * Used on destruction and with #Reset.
         ******************************************************************************************/
        ALIB_API
        void      clear();

        /** ****************************************************************************************
         * Reads the verbosity for the given logger and domain from the \alib configuration system.
         * This internal method is used in two occasions:
         * - when a new logger is added: recursively for all existing domains (\p{configStr} is
         *   given)
         * - when a new domain is created on the fly(\p{configStr} is not given)
         *
         * @param logger      The logger to set the verbosity for.
         * @param dom         The domain to set the verbosity for.
         * @param variable    The (already read) variable to set verbosities from.
         ******************************************************************************************/
        ALIB_API
        void      getVerbosityFromConfig( detail::Logger*    logger, detail::Domain*  dom,
                                          Variable& variable );

        /** ****************************************************************************************
         * Reads a prefix string from the \alib configuration system.
         * This internal method is used when a new domain is created,
         *
         * @param dom         The domain to set the verbosity for.
         ******************************************************************************************/
        ALIB_API
        void      getDomainPrefixFromConfig( detail::Domain*  dom );


        /** ****************************************************************************************
         * Reads the verbosity for the given logger and domain from the \alib configuration system.
         * This internal method is used when a new logger is added.
         * Walks recursively for all existing domains.
         *
         * @param logger      The logger to set the verbosity for.
         * @param dom         The domain to set the verbosity for.
         * @param variable    The (already read) variable to set verbosities from.
         ******************************************************************************************/
        ALIB_API
        void      getAllVerbosities( detail::Logger*  logger,   detail::Domain*  dom,
                                     Variable& variable  );

        /** ****************************************************************************************
         * Implements functionality for configuration variable \c LOXNAME_DUMP_STATE_ON_EXIT.
         * Is called when a logger is removed.
         ******************************************************************************************/
        ALIB_API
        void     dumpStateOnLoggerRemoval();

        /** ****************************************************************************************
         * Implements functionality for configuration variable \c LOXNAME_LOGGERNAME_VERBOSITY.
         * Is called when a logger is removed.
         * @param logger      The logger to write the verbosity for.
         ******************************************************************************************/
        ALIB_API
        void     writeVerbositiesOnLoggerRemoval( Logger* logger );

}; // class Lox

}} // namespace aworx[::lib::lox]

/// Type alias in namespace #aworx.
using     Lox=           aworx::lib::lox::Lox;

}  // namespace [aworx]

ALIB_ENUM_IS_BITWISE( aworx::lib::lox::Lox::StateInfo )
ALIB_ENUM_PARSABLE(   aworx::lib::lox::Lox::StateInfo )
ALIB_RESOURCED_IN_MODULE(  aworx::lib::lox::Lox::StateInfo, aworx::lib::ALOX,  "StateInfo"  )

#endif // HPP_ALOX_LOX

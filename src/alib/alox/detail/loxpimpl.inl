//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib::lox {
namespace detail {
class Domain;
class Logger;
}
namespace textlogger {  class TextLogger; }
} // namespace [alib::lox]


ALIB_EXPORT namespace alib {  namespace lox { namespace detail {

struct LoxImpl;

//==================================================================================================
/// Struct that collects static functions that provide the implementation of methods of
/// class \alib{lox;Lox}. For most; the first parameter is of type \p{lox;detail::LoxImpl}.
//==================================================================================================
struct LI
{
    /// Implementation of the constructor \alib{lox;Lox::Lox;the constructor} of class \b %Lox.
    /// @param name The name of the Lox. Will be copied and converted to upper case.
    /// @return The implementation for the calling \b Lox.
    ALIB_DLL static
    LoxImpl*        Construct( const NString& name );

    /// Destructs a lox.
    /// @param lox The \b %Lox that is to be destructed.
    ALIB_DLL static
    void            Destruct( LoxImpl* lox );

    /// Resets this object. Concretely, the following steps are performed:
    /// - If the debug lox singleton exists, it is deleted.
    /// - It is asserted that no other lox object is registered.
    /// - The \alox path of the configuration is deleted.
    ///
    /// \attention
    ///   This method was introduced to support resetting \alox in the unit tests.
    ///   In real applications, this method should NOT be used.
    ///   Side effects might appear using this method and it is not tested otherwise than
    ///   used in tests!
    ALIB_DLL void   Reset();

    /// Implementation of the method \alib{lox;Lox::GetName}.
    ///
    /// @param impl  The implementation struct of the \b Lox.
    /// @returns The name of this %Lox.
    ALIB_DLL static
    const NString&  GetName(LoxImpl* impl);

    /// Implementation of the method \alib{lox;Lox::GetLogCounter}.
    ///
    /// @param impl  The implementation struct of the \b Lox.
    /// @returns The name of this %Lox.
    ALIB_DLL static
    integer&        GetLogCounter(LoxImpl* impl);

    /// Implementation of the method \alib{lox;Lox::Acquire}.
    ///
    /// @param impl  The implementation struct of the \b Lox.
    /// @param ci    The source location that the call is placed at.
    ///              Usually macro \ref ALIB_CALLER is passed here.
    ALIB_DLL static
    void            Acquire(LoxImpl* impl, const lang::CallerInfo& ci );

    /// Implementation of the method \alib{lox;Lox::Release}.
    /// @param impl  The implementation struct of the \b Lox.
    ALIB_DLL static
    void            Release(LoxImpl* impl);

    /// Implementation of the method \alib{lox;Lox::Reset}.
    /// @param impl         The implementation struct of the \b Lox.
    ALIB_DLL static
    void            Reset(LoxImpl* impl);

    /// Implementation of the method \alib{lox;Lox::SetFileNameCacheCapacity}.
    /// @param impl           The implementation struct of the \b Lox.
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    ALIB_DLL static
    void SetFileNameCacheCapacity(LoxImpl* impl, integer numberOfLists, integer entriesPerList );

    /// Implementation of the method \alib{lox;Lox::SetSourcePathTrimRule}.
    /// @param impl            The implementation struct of the \b Lox.
    /// @param path            The path to search for. If not starting with <c> '*'</c>,
    ///                        a prefix is searched.
    /// @param includeString   Determines if \p{path} should be included in the trimmed path or
    ///                        not.
    ///                        Optional and defaults to \b %Inclusion::Exclude.
    /// @param trimOffset      Adjusts the portion of \p{path} that is trimmed.
    ///                        Optional and defaults to \c 0.
    /// @param sensitivity     Determines if the comparison of \p{path} with a source file's path
    ///                        is performed case-sensitive or not.
    ///                        Optional and defaults to \b Case::Ignore.
    /// @param trimReplacement Replacement string for trimmed portion of the path.
    ///                        Optional and defaults to \b %NULL_STRING.
    /// @param reach           Denotes whether the rule is applied locally (to this \b %Lox only)
    ///                        or applies to all instances of class \b %Lox.
    ///                        Defaults to \b %Reach::Global.
    /// @param priority        The priority of the setting. Defaults to
    ///                        \alib{variables;Priority;DefaultValues}.
    ALIB_DLL static
    void            SetSourcePathTrimRule( LoxImpl* impl,
                                           const NCString& path,
                                           lang::Inclusion includeString  ,
                                           int             trimOffset     ,
                                           lang::Case      sensitivity    ,
                                           const NString&  trimReplacement,
                                           lang::Reach     reach          ,
                                           Priority        priority         );


    /// Implementation of the method \alib{lox;Lox::GetLogger}.
    /// @param impl          The implementation struct of the \b Lox.
    /// @param loggerName    The name of the \e Logger to search for (case-insensitive).
    /// @return The logger, nullptr if not found.
    ALIB_DLL static
    detail::Logger* GetLogger( LoxImpl* impl, const NString& loggerName );

    /// Implementation of the method \alib{lox;Lox::RemoveLogger}.
    /// @param impl      The implementation struct of the \b Lox.
    /// @param logger    The logger to be removed.
    /// @returns \c true, if the \e Logger was found and removed, \c false otherwise.
    ALIB_DLL static
    bool            RemoveLogger( LoxImpl* impl, detail::Logger* logger );

    /// Implementation of the method \alib{lox;Lox::RemoveLogger}.
    /// @param impl          The implementation struct of the \b Lox.
    /// @param loggerName    The name of the \e Logger(s) to be removed (case-insensitive).
    /// @returns The logger that was removed, \c nullptr if not found.
    ALIB_DLL static
    detail::Logger* RemoveLogger( LoxImpl* impl, const NString& loggerName );

    /// Implementation of the method \alib{lox;Lox::SetVerbosity}.
    /// @param impl       The implementation struct of the \b Lox.
    /// @param logger     The logger to be to be affected (case-insensitive).
    /// @param verbosity  The 'level of verboseness' to be set.
    /// @param domain     The parent (start) domain to be set. The use of absolute paths
    ///                   starting with <c> '/'</c> are recommended.
    ///                   Defaults to root domain \"/\".
    /// @param priority   The priority of the setting. Defaults to
    ///                   \alib{variables;Priority;DefaultValues}.
    ALIB_DLL static
    void            SetVerbosity( LoxImpl* impl,
                                  detail::Logger*  logger,
                                  Verbosity        verbosity,
                                  const NString&   domain,
                                  Priority         priority    );

    /// Implementation of the method \alib{lox;Lox::SetVerbosity}.
    /// @param impl       The implementation struct of the \b Lox.
    /// @param loggerName The logger to be to be affected, identified by its name (case
    ///                   insensitive).
    /// @param verbosity  The 'level of verboseness' to be set.
    /// @param domain     The parent (start) domain to be set. The use of absolute paths
    ///                   starting with <c> '/'</c> are recommended.
    ///                   Defaults to root domain \"/\".
    /// @param priority   The priority of the setting. Defaults to
    ///                   \alib{variables;Priority;DefaultValues}.
    ALIB_DLL static
    void            SetVerbosity( LoxImpl* impl,
                                  const NString&   loggerName,
                                  Verbosity        verbosity,
                                  const NString&   domain,
                                  Priority       priority      );


    /// Implementation of the method \alib{lox;Lox::SetDomain}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param scopeDomain The domain path to register.
    /// @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
    ///                    values, an internal error is logged.
    /// @param thread      The thread to set/unset a thread-related Scope Domains for.
    ALIB_DLL static
    void            SetDomain( LoxImpl* impl, const NString& scopeDomain, Scope scope,
                               threads::Thread* thread );

    /// Implementation of the method \alib{lox;Lox::SetDomainSubstitutionRule}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param domainPath  The path to search. Has to start with either  <c> '/'</c> or <c> '*'</c>.
    /// @param replacement The replacement path.
    ALIB_DLL static
    void            SetDomainSubstitutionRule( LoxImpl*       impl,
                                               const NString& domainPath,
                                               const NString& replacement );

    /// Implementation of the method \alib{lox;Lox::RemoveThreadDomain}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param scopeDomain The domain path to register.
    /// @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
    ///                    values, an internal error is logged.
    /// @param thread      The thread to set/unset a thread-related Scope Domains for.
    ALIB_DLL static
    void            RemoveThreadDomain( LoxImpl* impl, const NString& scopeDomain, Scope scope,
                                        threads::Thread* thread );



    /// Implementation of the method \alib{lox;Lox::SetPrefix}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param prefix      The <em>Prefix Logable</em> to set.
    /// @param domain      The domain path. Defaults to \c nullptr, resulting in
    ///                    evaluated <em>Scope Domain</em> path.
    /// @param otherPLs    If set to \c Inclusion::Exclude, scope-related <em>Prefix Logables</em>
    ///                    are ignored and only domain-related <em>Prefix Logables</em> are passed to
    ///                    the \e Loggers.
    ALIB_DLL static
    void            SetPrefix( LoxImpl* impl, const Box& prefix, const NString& domain,
                               lang::Inclusion otherPLs );

    /// Implementation of the method \alib{lox;Lox::SetStartTime}.
    /// @param impl       The implementation struct of the \b Lox.
    /// @param startTime  Optional parameter with the new start time. Defaults
    ///                   to current time if omitted.
    /// @param loggerName The name of the \e Logger(s) whose start time is to be set
    ///                   (case-insensitive).
    ///                   Defaults to nullptr, which indicates that all loggers are to
    ///                   be affected.
    ALIB_DLL static
    void            SetStartTime( LoxImpl*        impl,
                                  Ticks           startTime,
                                  const NString&  loggerName  );

    #if defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)  || defined(__ANDROID_NDK__)
    /// Implementation of the method \alib{lox;Lox::SetStartTime}.
    /// @param impl       The implementation struct of the \b Lox.
    /// @param startTime  The new start time in system-specific time unit.
    /// @param loggerName The name of the \e Logger whose start time is to be set (case
    ///                   insensitive).
    ///                   Defaults to empty string, which indicates that all loggers are to
    ///                   be affected.
    ALIB_DLL static
    void        SetStartTime( LoxImpl* impl, time_t startTime, const NString& loggerName );


    #endif // no elif here, otherwise doxygen would ignore it!

    #if defined( _MSC_VER )
        //==========================================================================================
        /// Implementation of the method \alib{lox;Lox::SetStartTime}.
        /// @param impl       The implementation struct of the \b Lox.
        /// @param startTime  The new start time in system-specific time unit.
        /// @param loggerName The name of the \e Logger whose start time is to be set (case
        ///                   insensitive).
        ///                   Defaults to empty string, which indicates that all loggers are to
        ///                   be affected.
        //==========================================================================================
        ALIB_DLL static
        void        SetStartTime( LoxImpl*          impl,
                                  const FILETIME&   startTime,
                                  const NString&    loggerName );
    #endif

    /// Implementation of the method \alib{lox;Lox::MapThreadName}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param threadName  The name of the thread as it should be displayed in the logs.
    /// @param id          The thread ID.
    ALIB_DLL static
    void            MapThreadName( LoxImpl* impl, const String& threadName, threads::ThreadID id );

    /// Implementation of the method \alib{lox;Lox::State}.
    /// @param impl          The implementation struct of the \b Lox.
    /// @param domain        Optional <em>Log Domain</em> which is combined with
    ///                      <em>%Scope Domains</em> set for the \e %Scope of invocation.
    /// @param verbosity     The verbosity.
    /// @param headLine      If given, a separated headline will be logged at first place.
    /// @param flags         Flag bits that define which state information is logged.
    ALIB_DLL static
    void            State( LoxImpl* impl,
                           const NString&    domain,
                           Verbosity         verbosity,
                           const String&     headLine,
                           StateInfo         flags        );

    /// Implementation of the method \alib{lox;Lox::GetState}.
    /// @param impl       The implementation struct of the \b Lox.
    /// @param buf        The target string.
    /// @param flags      Bits that define which state information is collected.
    ALIB_DLL static
    void            GetState( LoxImpl* impl, NAString& buf, StateInfo flags );

    /// Implementation of the method \alib{lox;Lox::GetLogableContainer}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @return An empty list of boxes.
    ALIB_DLL static
    BoxesMA&        GetLogableContainer( LoxImpl* impl );

    /// Implementation of the method \alib{lox;Lox::Entry}.
    /// @param impl        The implementation struct of the \b Lox.
    /// @param domain      The domain.
    /// @param verbosity   The verbosity.
    ALIB_DLL static
    void            Entry( LoxImpl* impl, const NString&  domain, Verbosity verbosity );

    /// Implementation of the method \alib{lox;Lox::IsActive}.
    ///
    /// @param impl         The implementation struct of the \b Lox.
    /// @param verbosity    The verbosity to query for activity.
    /// @param domain       The log domain. All rules for resolving the effective log domain
    ///                     apply as with normal log statements.
    /// @param resultDomain An optional pointer to an \b AString. If given, the resulting
    ///                     domain is appended to the string.
    ///                     (The domain that results from the optional \p{domain} and all \alox
    ///                     rules that apply at the place of invocation).
    /// @return The number of active loggers.
    ALIB_DLL static
    int IsActive( LoxImpl* impl, Verbosity verbosity, const NString&  domain,
                  NAString* resultDomain );

//##################################################################################################
// Debug methods
//##################################################################################################
#if ALIB_DEBUG_MEMORY
    //==============================================================================================
    /// Implementation of the method \alib{lox;Lox::DbgGetMonoAllocator}.
    /// @param impl   The implementation struct of the \b Lox.
    /// @return The monotonic allocator of this \b Lox.
    //==============================================================================================
    ALIB_DLL static
    MonoAllocator& DbgGetMonoAllocator( LoxImpl* impl );
#endif


//##################################################################################################
// internals
//##################################################################################################

    /// Assembles the resulting domain from the given \p{domainPath} and the Scope Domain paths
    /// (see #SetDomain) according to the scope identified by \p{scopeInfo}.
    /// The resulting full domain string is assembled from inner to outer scope.
    /// If \p{domainPath}, respectively as soon as any of the Scope Domain's paths
    /// start with the character defined with
    /// \ref alib::lox::detail::Domain::Separator "Domain::Separator",
    /// the evaluation is stopped (the path is interpreted as absolute).
    ///
    /// @param impl       The implementation struct of the \b Lox.
    /// @param domainPath The domain path. If starting with the character defined with
    ///                   \ref alib::lox::detail::Domain::Separator "Domain::Separator",
    ///                   no scope domains are applied.
    /// @return The resulting \ref alib::lox::detail::Domain "Domain".
    ALIB_DLL static
    detail::Domain* evaluateResultDomain( LoxImpl* impl, const NString& domainPath );

    /// Invokes \b Find on the given domain and logs internal message when the domain was
    /// not known before.
    ///
    /// @param impl          The implementation struct of the \b Lox.
    /// @param domainSystem  The domain system. Either the standard or the internal one.
    /// @param domainPath    The domain path.
    /// @return The resulting \ref alib::lox::detail::Domain "Domain".
    ALIB_DLL static
    detail::Domain* findDomain( LoxImpl*            impl,
                                detail::Domain&     domainSystem,
                                NString domainPath );

    /// This method is looping over the \e Loggers, checking their verbosity against the given
    /// one, and, if they match, invoke the log method of the \e Logger.
    /// With the first logger identified to be active, the <em>Prefix Objects</em> get
    /// collected from the scope store.
    /// @param impl         The implementation struct of the \b Lox.
    /// @param dom          The domain to log on
    /// @param verbosity    The verbosity.
    /// @param logables     The objects to log.
    /// @param prefixes     Denotes if prefixes should be included or not.
    ALIB_DLL static
    void            log( LoxImpl*         impl,
                         detail::Domain*  dom,
                         Verbosity        verbosity,
                         BoxesMA&         logables,
                         lang::Inclusion  prefixes    );


    /// Logs an internal error message using the internal domain tree as defined in
    /// \ref alib::lox::Lox::InternalDomains "Lox::InternalDomains".
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param verbosity The verbosity.
    /// @param subDomain The subdomain of the internal domain to log into.
    /// @param msg       The message.
    ALIB_DLL static
    void            logInternal( LoxImpl* impl, Verbosity verbosity, const NString& subDomain,
                                 BoxesMA& msg );

    /// Overloaded version accepting a string to log.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param verbosity The verbosity.
    /// @param subDomain The subdomain of the internal domain to log into.
    /// @param msg       The message.
    ALIB_DLL static
    void            logInternal( LoxImpl* impl, Verbosity verbosity, const NString& subDomain,
                                 const NString& msg );

    /// Returns a reference to a list of boxes to be used by internal logging.
    /// Each invocation has to be followed by an invocation to #logInternal
    /// which releases the logables.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @return A list of boxes.
    ALIB_DLL static
    BoxesMA&          acquireInternalLogables(LoxImpl* impl);

    /// Implementation of the interface method fetching all possible parameters.
    ///
    /// @param impl        The implementation struct of the \b Lox.
    /// @param scopeDomain The domain path to register.
    /// @param scope       The scope that the given \p{domain} should be registered for.
    ///                    Available Scope definitions are platform/language dependent.
    /// @param removeNTRSD Used to remove a named thread-related Scope Domain (and is true only
    ///                    when invoked by interface method #RemoveThreadDomain.
    /// @param thread      The thread to set/unset a thread-related Scope Domain for.
    ALIB_DLL static
    void            setDomain( LoxImpl* impl, const NString& scopeDomain, Scope scope,
                               bool removeNTRSD, threads::Thread* thread  );

    /// Implementation of the interface method fetching all possible parameters.
    ///
    /// @param impl        The implementation struct of the \b Lox.
    /// @param prefix      The <em>Prefix Logable</em> to set.
    /// @param scope       The scope that the given \p{logable} should be registered for.
    ///                    Available Scope definitions are platform/language dependent.
    /// @param thread      The thread to set/unset a thread-related <em>Prefix Logable</em> for.
    ALIB_DLL static
    void  setPrefix( LoxImpl* impl, const Box& prefix, Scope scope, threads::Thread* thread );

    /// Increases the internal log counter of \p{impl}.
    /// @param impl      The implementation struct of the \b Lox.
    ALIB_DLL static
    void            IncreaseLogCounter( LoxImpl* impl);

    /// Implementation of the method \alib{lox;Lox::EntryDetectDomain}.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param verbosity The verbosity.
    ALIB_DLL static
    void            entryDetectDomainImpl( LoxImpl* impl, Verbosity verbosity);

    /// Internal method serving public interface \alib{lox;Lox::Once}.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param domain    Optional <em>Log Domain</em> which is combined with
    ///                  <em>%Scope Domains</em> set for the \e %Scope of invocation.
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param pGroup    The optional name of the statement group. If used, all statements that
    ///                  share the same group name are working on the same counter (according
    ///                  to the \p{scope}.)
    ///                  If omitted (or empty or nullptr), the counter is bound to the
    ///                  \e %Scope provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  counter is associated exclusively with the single <em>Log Statement</em>
    ///                  itself.
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    ALIB_DLL static
    void            once( LoxImpl*   impl    , const NString& domain, Verbosity verbosity,
                          const Box& logables, const String&  pGroup, Scope     scope,
                          int        quantity );

    /// Internal method serving public interface \alib{lox;Lox::Store}.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param data      The data object to store.
    ///                  In C++, has to be heap allocated and will be deleted
    ///                  by this \b %Lox when overwritten or this lox is deleted.
    /// @param pKey      The key to the data.
    /// @param scope     The \e %Scope that the data is bound to.
    ALIB_DLL static
    void            store( LoxImpl* impl, const Box& data, const NString& pKey, Scope scope );

    /// Internal method serving public interface \alib{lox;Lox::Retrieve}.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param pKey       The key to the data.
    /// @param scope     The \e %Scope that the data is bound to.
    /// @return The data, a \e nulled box if no value was found.
    ALIB_DLL static
    Box             retrieve( LoxImpl* impl, const NString& pKey, Scope scope );

    /// Checks if given scope is thread-related.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param scope     The scope that is to be checked.
    /// @return \c true if \p{scope} is thread-related, \c false else.
    ALIB_DLL static
    bool            isThreadRelatedScope( LoxImpl* impl, Scope scope );

    /// Checks if given scope needs information that is not available. In addition, the
    /// in/out parameter \p{scope} is changed to \b Scope::Path, in case a level was added.
    ///
    /// @param impl           The implementation struct of the \b Lox.
    /// @param[in,out] scope  A reference to the scope that is to be checked (and eventually
    ///                       modified.
    /// @param internalDomain The internal subdomain to log any error/warning into.
    /// @return A positive value providing the path level deduced from \p{scope} if all is fine,
    ///        \c -1 else.
    ALIB_DLL static
    int             checkScopeInformation( LoxImpl*       impl          , Scope& scope,
                                           const NString& internalDomain                 );

    /// Used on construction and with #Reset.
    /// @param impl      The implementation struct of the \b Lox.
    ALIB_DLL static
    void            init(LoxImpl* impl);

    /// Reads the verbosity for the given logger and domain from the \alib variable system.
    /// This internal method is used in two occasions:
    /// - when a new logger is added: recursively for all existing domains (\p{configStr} is
    ///   given)
    /// - when a new domain is created on the fly(\p{configStr} is not given)
    ///
    /// The variable is already read and found in an internal instance.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param variable  The loaded variable with verbosity information.
    /// @param logger    The logger to set the verbosity for.
    /// @param dom       The domain to set the verbosity for.
    ALIB_DLL static
    void            getVerbosityFromConfig( LoxImpl*             impl,
                                            variables::Variable&    variable,
                                            detail::Logger*      logger,
                                            detail::Domain&      dom                  );

    /// Reads a prefix string from the \alib variable system.
    /// This internal method is used when a new domain is created,
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param dom       The domain to set the verbosity for.
    ALIB_DLL static
    void            getDomainPrefixFromConfig( LoxImpl* impl, detail::Domain&  dom );


    /// Reads the verbosity for the given logger and domain from the \alib variable system.
    /// This internal method is used when a new logger is added.
    /// Walks recursively for all existing domains.
    ///
    /// @param impl      The implementation struct of the \b Lox.
    /// @param variable  The loaded variable with verbosity information.
    /// @param logger    The logger to set the verbosity for.
    /// @param dom       The domain to set the verbosity for.
    ALIB_DLL static
    void            getAllVerbosities( LoxImpl* impl, variables::Variable& variable,
                                       detail::Logger* logger, detail::Domain& dom );

    /// Implements functionality for configuration variable \c LOXNAME_DUMP_STATE_ON_EXIT.
    /// Is called when a logger is removed.
    /// @param impl      The implementation struct of the \b Lox.
    ALIB_DLL static
    void            dumpStateOnLoggerRemoval(LoxImpl* impl);

    /// Implements functionality for configuration variable \c LOXNAME_LOGGERNAME_VERBOSITY.
    /// Is called when a logger is removed.
    /// @param impl    The implementation struct of the \b Lox.
    /// @param logger  The logger to write the verbosity for.
    ALIB_DLL static
    void            writeVerbositiesOnLoggerRemoval( LoxImpl* impl, Logger* logger );

#if !ALIB_SINGLE_THREADED
    /// Returns the internal lock.
    /// @param impl  The implementation struct of the \b Lox.
    /// @returns The internal \b RecursiveLock of this \b %Lox.
    ALIB_DLL static
    threads::RecursiveLock&     getLock(LoxImpl* impl);
#endif

}; // struct LI

}}} // namespace [alib::lox::detail]

ALIB_BOXING_VTABLE_DECLARE( std::pair<alib::lox::Verbosity
                            ALIB_COMMA alib::variables::Priority> , vt_lox_pair_verby_prio )

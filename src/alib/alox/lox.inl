//==================================================================================================
/// \file
/// This header-file is part of module \alib_alox of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace lox {

//==================================================================================================
/// This class acts as a container for \e Loggers and provides a convenient interface to logging.
//==================================================================================================
class Lox
{
    #if !DOXYGEN
        friend class   ALox;
        friend struct  detail::LI;
    #endif

  protected:
    /// The implementation.
    detail::LoxImpl*                                    impl;

  public:
    /// This is the path for logging to the internal domain. By manipulating this
    ///  <em>%Log %Domain's %Verbosity</em>, the verbosity of \alox itself can be controlled.
    /// For example, with \e Verbosity.INFO, the 'on the fly' creation of <em>Log Domains</em>
    /// are logged, which can be helpful to determine the <em>Lo                                                                 bg Domains</em> that are
    /// created by libraries and larger projects.
    ///
    /// The following Subdomains are used by \alox:
    ///
    ///   Subdomain  | Description
    ///   - - - - - -| - - - - - - - - - - - - - - - - - - -
    ///   LGR        | Used when \e %Loggers are registered, retrieved or removed from a \b %Lox and when the \e Verbosity of a <em>Log Domain</em> for a \e %Logger is changed.<br>In addition used with method \b %Lox::SetStartTime.
    ///   DMN        | Used when <em>Log Domains</em> are registered (on first use), when <em>Scope Domains</em> are set or removed and when <em>Domain Substitution Rules</em> are set.
    ///   PFX        | Used when <em>Prefix Logables</em> are set or removed.
    ///   THR        | Used with method \b %Lox::MapThreadName.
    ///   LGD        | Used with storing and retrieving <em>Log Data</em> objects.
    ///
    /// \note For internal logging an separated <em>domain tree</em> is used. This means, that
    ///       setting the root domain of a \b %Lox to a certain \e Verbosity does \e not affect
    ///       the internal domains. In other words, the \e Verbosity of the internal domain
    ///       (or one of its Subdomains) has to be set explicitly.
    ///
    static constexpr  NString  InternalDomains                                            {  "$/" };

  //################################################################################################
  // Constructors/destructor
  //################################################################################################
    /// Constructs a new, empty Lox with the given \p{name}.
    /// The name is immutable and all \b %Lox objects registered with \alox must be unique.
    /// Lower case letters in the name are converted to upper case.
    /// The name \c "Log" is reserved for the internal default singleton used for debug-logging.
    /// In addition, name \c "GLOBAL" is not allowed.
    ///
    /// If parameter \p{register} is \c true (the default), static method
    /// \alib{lox,ALoxCamp::Register} is invoked and the object will be
    /// retrievable with static method
    /// \alib{lox,ALoxCamp::Get}. In some situations, such 'registration'
    /// may not be wanted.
    ///
    /// @param name       The name of the Lox. Will be copied and converted to upper case.
    /// @param doRegister If \c true, this object is registered with the static class
    ///                   \alib{lox,ALoxCamp}.
    ///                   Optional and defaults to \c true.
    ALIB_DLL
    Lox(const NString& name, bool doRegister =true );

    /// Destructs a lox
    ALIB_DLL
    ~Lox();


    /// Returns a \b %Lox with the given name.
    /// A \b %Lox is only found if it was created and registered with \alox using #Register.
    /// If not found, and parameter \p{create} is \c true (the default), a new \b Lox is created,
    /// registered and returned.
    ///
    /// @param name      The name of the \b %Lox to search and optionally to create.
    ///                  Comparison is case-insensitive.
    /// @param create    Denotes whether a \b %Lox that was not found is created.
    ///                  Optional and defaults to \b %CreateIfNotExists::No.
    /// @return The \b Lox found, \c nullptr in case of failure.
    ALIB_DLL
    static Lox*     Get( const NString&          name,
                         lang::CreateIfNotExists create= lang::CreateIfNotExists::No );

    /// Registers or un-registers a \b %Lox object statically with \alox.
    /// Once registered, any code entity of the same process is enabled to retrieve
    /// the \b %Lox using #Get.<br>
    /// No two objects with the same name must be registered. If this is done, the latter
    /// will not be registered and not be found by #Get. In debug-compilations, an \alib
    /// assertion is raised if a name is registered twice.<br>
    /// Note that name comparison is performed case <b>in</b>-sensitive.
    ///
    /// If debug-logging is enabled (depends on optional compiler-symbols) and used, the
    /// singleton of type \c %Lox provided for debug-logging is registered. This uses the name
    /// \c "Log".
    ///
    /// Registration is \e not mandatory but done by default by the constructor of class \b %Lox.
    /// Therefore, to keep a \b Lox private, an optional parameter is available.
    ///
    /// @param lox       The \b %Lox to register.
    /// @param operation If \b %ContainerOp::Remove, the given \p{Lox} is deregistered.
    ///                  Defaults to \b %ContainerOp::Insert.
    ALIB_DLL
    static void     Register( Lox* lox, lang::ContainerOp operation );


  //################################################################################################
  // Interface
  //################################################################################################
    /// Returns the name of this \b %Lox. The name is user-defined, and provided in the
    /// constructor,
    /// converted to upper case and otherwise immutable.
    ///
    /// @returns The name of this %Lox.
    const NString&  GetName()                                { return detail::LI::GetName( impl ); }

    /// Returns the number of logs that have been performed with this \b Lox.
    /// The counter is not used internally other than for providing a
    /// unique log number: While each logger attached has a own number, if more than one
    /// logger is attached, their numbers may differ due to different log domain settings.
    /// \note
    ///    The result is given as a mutual reference to the internal counter, which is allowed
    ///    to be manipulated. This is, for example, used in unit tests.
    ///
    /// @returns \c true if this instance was registered with \alox, \c false if not.
    integer&        GetLogCounter()                    { return detail::LI::GetLogCounter( impl ); }

    /// Acquires this \b %Lox and sets the scope information data for the next log.
    /// @param ci  The source location that the call is placed at.
    void            Acquire( const lang::CallerInfo& ci  )        { detail::LI::Acquire(impl, ci); }

    /// Releases ownership of this object. If #Acquire was called multiple times, the same
    /// number of calls to this method have to be performed to release ownership.
    void            Release()                                         { detail::LI::Release(impl); }

    /// Resets this object.
    ///
    /// \attention
    ///   This method was introduced to support resetting the debug \b %Lox objects in the unit
    ///   tests. In real applications, and for release logging it recommended to delete a Lox
    ///   and create a new one instead of resetting one.
    ///   Side effects might appear when using this method!
    void        Reset()                                                 { detail::LI::Reset(impl); }

    /// Changes the capacity of the \b %LRUCacheTable for parsed file names by calling
    /// \alib{containers;LRUCacheTable::Reserve}.
    /// @param numberOfLists  The number of LRU-lists to use.
    /// @param entriesPerList The maximum length of each cache list.
    void SetFileNameCacheCapacity(integer numberOfLists, integer entriesPerList )
    { detail::LI::SetFileNameCacheCapacity(impl, numberOfLists, entriesPerList ); }

    /// Adds \p{path} to an internal list of substrings that are used to trim the path of
    /// a source file name. Trimmed paths are used for \e Scope mechanisms and can be
    /// logged (e.g., with meta-information of class \b TextLogger.
    ///
    /// By default such setting affects all instances of class \b Lox, not only
    /// this instance. This can be altered using parameter \p{global}.
    /// one other The given trim information can either
    ///
    /// If given \p{path} starts with character <c> '*'</c>, the rest of the string is searched
    /// within source paths. Otherwise, it is checked if a source path starts with the given
    /// path.
    ///
    /// Parameter \p{trimReplacement} optionally provides a replacement string for the trimmed
    /// path. This can be used, for example, to provide the right absolute path for an IDE
    /// to find source files of a library.
    ///
    /// Parameter \p{includeString} determines if the searched substring should be included in the
    /// resulting source path or not. In addition, parameter \p{trimOffset}, which can be negative
    /// or positive, is added to the position of trimming. This can be used to increase the
    /// length of the search path, and then cut only a portion of what was searched for.
    ///
    /// Finally, parameter \p{sensitivity} determines whether the match is performed case
    /// sensitive or not. It defaults to non-sensitive, for convenience and for the fact that,
    /// for example, Microsoft C++ compiler's preprocessor passes lower case path-strings!
    ///
    /// \note
    ///   If the platform (compiler) specific path separator is <c>'/'</c>, then characters
    ///   <c>'\'</c> found in parameters \p{path} and \p{trimReplacement} are replaced by <c>'\'</c>
    ///   and vice versa. This allows specifying paths and substrings thereof in a platform
    ///   independent way.
    ///
    /// \attention
    ///   Setting global rules (when parameter \p{global} equals \c Inclusion::Include) is not
    ///   protected by a \c mutex against concurrent access. Therefore, global rules have
    ///   to be either at bootstrap of a process, before threads are created, or such creation
    ///   has to 'manually' be protected by locking all existing instances of this class!
    ///
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
    ///                        \alib{variables;Priority;Standard}.
    void      SetSourcePathTrimRule( const NCString& path,
                                     lang::Inclusion includeString   = lang::Inclusion::Exclude,
                                     int             trimOffset      = 0,
                                     lang::Case      sensitivity     = lang::Case::Ignore,
                                     const NString&  trimReplacement = NULL_NSTRING,
                                     lang::Reach     reach           = lang::Reach::Global,
                                     Priority        priority        = Priority::Standard ) {
        detail::LI::SetSourcePathTrimRule( impl, path, includeString, trimOffset, sensitivity,
                                         trimReplacement, reach, priority );
    }


    /// Removes all local trimming rules set with #SetSourcePathTrimRule.
    /// If parameter \p{global} is set to \b Inclusion::Include, the global rules are cleared
    /// in addition.
    ///
    /// Setting parameter \p{allowAutoRule} to \c false, allows suppressing the creation of an
    /// automatic rule based on the executables path.
    ///
    /// \see Chapter \ref alib_mod_alox_trim_source_path for more information.
    ///
    /// @param reach         Denotes whether only local rules are cleared or also global ones.
    ///                      Defaults to \b %Reach::Global.
    /// @param allowAutoRule Determines if an auto rule should be tried to be detected next
    ///                      no appropriate rule is found.
    void      ClearSourcePathTrimRules( lang::Reach reach           = lang::Reach::Global,
                                        bool        allowAutoRule   = true                  ) {
        detail::LI::SetSourcePathTrimRule( impl, nullptr, allowAutoRule ? lang::Inclusion::Include
                                                                        : lang::Inclusion::Exclude,
                                         999999, // code for clearing
                                         lang::Case::Ignore, NULL_NSTRING, reach, Priority::NONE  );
    }

    /// This static method creates a console logger. To decide which logger type to choose,
    /// configuration variable \ref alxcvALOX_CONSOLE_TYPE is checked. If this variable is not set,
    /// then the decision is made as follows:
    /// - On GNU/Linux OS, a
    ///   \ref alib::lox::loggers::AnsiConsoleLogger "AnsiConsoleLogger" is chosen.
    /// - On Windows OS, if a console window is attached, type
    ///   \ref alib::lox::loggers::WindowsConsoleLogger "WindowsConsoleLogger" is chosen. If
    ///   no console is attached to the process, instead a
    ///   \ref alib::lox::loggers::ConsoleLogger "ConsoleLogger" is returned.
    ///
    ///
    /// @param name The name of the \e Logger. Defaults to nullptr, which uses standard
    ///             names defined by derived \e Logger types.
    ///
    /// @return An instance of the chosen console type logger.
    ALIB_DLL static
    textlogger::TextLogger*  CreateConsoleLogger( const NString& name= nullptr );

    /// Retrieves an instance of a Logger by its name. This might be useful when access to a
    /// \e %Logger is needed to change its configuration.
    ///
    /// @param loggerName    The name of the \e Logger to search for (case-insensitive).
    /// @return The logger, nullptr if not found.
    detail::Logger*   GetLogger( const NString& loggerName )
    { return detail::LI::GetLogger(impl, loggerName); }

    /// Removes a logger from this container.
    /// \note
    ///   To (temporarily) disable a logger without removing it, a call to
    ///   \ref SetVerbosity(detail::Logger*,Verbosity,const NString&,Priority) "SetVerbosity(logger, Verbosity::Off)"
    ///   can be used.
    ///
    /// @param logger   The logger to be removed.
    /// @returns \c true, if the \e Logger was found and removed, \c false otherwise.
    bool            RemoveLogger( detail::Logger* logger )
    { return detail::LI::RemoveLogger(impl, logger); }

    /// Removes logger named \p{loggerName} from this container.
    /// \note
    ///   To (temporarily) disable a logger without removing it, a call to
    ///   \ref SetVerbosity(detail::Logger*,Verbosity,const NString&,Priority) "SetVerbosity(logger, Verbosity::Off)"
    ///   can be used.
    ///
    /// @param loggerName  The name of the \e Logger(s) to be removed (case-insensitive).
    /// @returns The logger that was removed, \c nullptr if not found.
    detail::Logger*   RemoveLogger( const NString& loggerName )
    { return detail::LI::RemoveLogger(impl, loggerName); }

    /// Sets the \e %Verbosity of the <em>Log Domain</em> which is evaluated from parameter
    /// \p{domain} and applicable <em>Scope Domains</em>. The \p{verbosity} given is set
    /// recursively for all Subdomains.
    ///
    /// With the first invocation of this method for a distinct \p{logger}, this \e %Logger
    /// is registered with this \e %Lox. In this case, before setting the given \e Verbosity
    /// for the evaluated subdomain, the \e Verbosity for all domains is set to
    /// \b %Verbosity::Off.
    ///
    /// To deregister a \e Logger with a \b Lox, use method #RemoveLogger.
    /// To 'disable' a \e Logger, invoke this method with parameters \p{verbosity} equaling to
    /// \b %Verbosity::Off and \p{domain} to \c "/".
    ///
    /// Optional parameter \p{priority} defaults to
    /// \alib{variables;Priority;Standard},
    /// which is a lower priority than those of the standard plug-ins of external configuration
    /// data. Therefore, external configuration by default 'overwrite' settings made from
    /// 'within the source code', which simply means by invoking this method.<br>
    /// The parameter can be provided for two main reasons:
    /// - To 'lock' a verbosity setting against external manipulation.
    /// - to 'break' the standard mechanism that an invocation of this method sets all
    ///   Subdomains recursively. If a subdomain was set with a higher priority
    ///   (e.g., <c>%Config::PriorityOf(Priority::Standard) + 1</c>, then this subdomain will
    ///   not be affected by future invocations of this method with standard-priority given.
    ///
    /// For more information on how to use external configuration variables with priority and
    /// on protecting verbosity settings, consult the \ref alib_mod_alox.
    ///
    /// \attention
    ///   The same as with most interface methods of this class, the given \p{domain} parameter is
    ///   combined with <em>%Scope Domains</em> set for the caller's \e %Scope. In standard use
    ///   cases of %ALox, the \e %Verbosity of a \e Domain is set using absolute domain path
    ///   addressing. Therefore, it is recommended to have any domain path passed to this method
    ///   starting with <c> '/'</c>, which suppresses the concatenation of <em>%Scope Domains</em>.<br>
    ///   This is why this parameter with this method defaults to <c> '/'</c>, while with other
    ///   methods of this class, it defaults to an empty string.
    /// <p>
    /// \attention
    ///   Even when using an absolute domain path, <em>%Scope Domains</em> of
    ///   \e %Scope::ThreadInner, will still apply. This means that from within a thread that
    ///   has such <em>%Scope Domain</em> set, this method is (almost) not usable!
    ///   This all aligns with the concept (advice), that \e Loggers and their \e %Verbosity
    ///   are generally set outside of such scopes, hence in configuration sections of a
    ///   process.<p>
    /// \attention
    ///   Consequently, this method may be (mis-) used to modify the 'actual' (default) scope
    ///   when explicitly giving an empty string with parameter \p{domain}. This is useful, to
    ///   temporarily adjust a scope. But remember: \alox was designed to avoid temporary
    ///   code lines...
    ///
    /// @param logger     The logger to be to be affected (case-insensitive).
    /// @param verbosity  The 'level of verboseness' to be set.
    /// @param domain     The parent (start) domain to be set. The use of absolute paths
    ///                   starting with <c> '/'</c> are recommended.
    ///                   Defaults to root domain \"/\".
    /// @param priority   The priority of the setting. Defaults to
    ///                   \alib{variables;Priority;Standard}.
    void            SetVerbosity( detail::Logger*  logger,
                                  Verbosity        verbosity,
                                  const NString&   domain        = "/",
                                  Priority         priority        = Priority::Standard  )
    { detail::LI::SetVerbosity(impl, logger, verbosity, domain, priority); }

    /// Same as
    /// \ref #SetVerbosity(detail::Logger*,Verbosity,const NString&,Priority) "SetVerbosity"
    /// but addressing the \e %Logger to manipulate by its name.<br>
    /// This method may only be used after a \e %Logger was once 'registered' with this \b %Lox
    /// using
    /// \ref #SetVerbosity(detail::Logger*,Verbosity,const NString&,Priority) "SetVerbosity".
    ///
    /// @param loggerName The logger to be to be affected, identified by its name (case
    ///                   insensitive).
    /// @param verbosity  The 'level of verboseness' to be set.
    /// @param domain     The parent (start) domain to be set. The use of absolute paths
    ///                   starting with <c> '/'</c> are recommended.
    ///                   Defaults to root domain \"/\".
    /// @param priority   The priority of the setting. Defaults to
    ///                   \alib{variables;Priority;Standard}.
    void            SetVerbosity( const NString&   loggerName,
                                  Verbosity        verbosity,
                                  const NString&   domain        = "/",
                                  Priority         priority      = Priority::Standard  )
    { detail::LI::SetVerbosity(impl, loggerName, verbosity, domain, priority); }

    /// The given \p{scopeDomain} becomes the default domain path for given \p{scope}.
    /// This means, that any subsequent log invocations (from within this same scope) can omit
    /// the domain parameter, or if they provide one, this Scope Domain path is prepended.
    /// If subsequent log calls specify a domain name with a leading '/' character,
    /// then the Scope Domain of the scope is ignored.<br>
    /// Furthermore, if the given scope is an inner scope, outer scopes are prepended to the
    /// given \p{scopeDomain} when the resulting domain of a log invocation is evaluated.
    /// Again, this behavior can be overruled by prepending a leading '/' character to
    /// \p{scopeDomain}.
    ///
    /// To remove a previously set Scope Domain a \e nulled or empty string has to be passed with
    /// parameter \p{scopeDomain}.
    /// For \e %Scope::ThreadOuter and \e %Scope::ThreadInner, passing an empty or \e nulled
    /// string removes the most recently added domain path. For removing an explicitly named
    /// domain path of \e %Scope::ThreadOuter and \e %Scope::ThreadInner use method
    /// #RemoveThreadDomain.
    ///
    /// \note
    ///   The C++ version of \alox implements scope mechanisms using scope information generated
    ///   by the preprocessor. By default, debug logging supports such caller information,
    ///   release logging does not. This can be changed.<br>
    ///   For more information on how to change such defaults, see documentation of preprocessor
    ///   symbols \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI.
    ///
    /// @param scopeDomain The domain path to register.
    /// @param scope       The scope that should the given \p{domain} be registered for.
    ///                    Available Scope definitions are platform/language dependent.
    void        SetDomain( const NString& scopeDomain, Scope scope  )
    { detail::LI::setDomain(impl, scopeDomain, scope, false, nullptr ); }

    /// This overloaded version of
    /// \ref SetDomain(const NString&,Scope) "SetDomain" is applicable only for
    /// \e %Scope::ThreadOuter and \e %Scope::ThreadInner and allows specifying the thread that
    /// the setting should be associated with.
    ///
    /// If \p{scopeDomain} is nullptr or empty, the most recently added domain path is removed.
    /// For removing an explicitly named domain associated with a thread use method
    /// #RemoveThreadDomain.
    ///
    /// @param scopeDomain The domain path to register.
    /// @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
    ///                    values, an internal error is logged.
    /// @param thread      The thread to set/unset a thread-related Scope Domains for.
    void        SetDomain( const NString& scopeDomain, Scope scope, threads::Thread* thread )
    { detail::LI::setDomain( impl, scopeDomain, scope, false, thread ); }

    /// Adds a <em>Domain Substitution Rule</em>.
    /// <em>Domain Substitution</em> is performed as a last step when evaluating the domain path
    /// of a <em>Log Statement</em>, taking <em>Scope Domains</em> and the optional parameter
    /// \p{domain} of the statement into account.<br>
    ///
    /// <b>Wildcards</b><br>
    /// Parameter \p{domainPath} supports \b 'wildcard' character <c> '*'</c> at its beginning
    /// and at its end (or both). This allows having four types of rules:
    /// - Exact match
    /// - Prefix match (\c * at the end of \p{domainPath})
    /// - Suffix match (\c * at the start of \p{domainPath})
    /// - Substring match (\c * at both, start and the end of \p{domainPath})
    ///
    /// Only minimal checks are performed, e.g., if an exact match is requested, but \p{domainPath}
    /// does not start with character <c> '/'</c>. In this and some other cases, the rule is not
    /// stored and an internal warning is logged. Further checks, for example, for illegal
    /// domain path characters, are not performed.
    /// (Illegal domain path characters will be eliminated when the resulting domain path is to
    /// be created internally).
    ///
    /// <b>Circular Dependencies</b><br>
    /// If the given rules have circular dependencies, only a limited number (ten) replacements
    /// are performed. If this number of replacements for one <em>Log Statement</em> is exceeded,
    /// an internal warning message is logged. This is done only \e once over the life-time of
    /// a \e Logger.
    ///
    /// <b>Application of Rules</b><br>
    /// Rules are applied in the order of their definition. After all rules have been applied,
    /// this is repeated as long as at least one rule matches (up to ten times).
    ///
    /// <b>Deletion of Rules</b>
    /// To delete a rule, invoke the method with the same parameter \p{domainPath} but with
    /// a \e 'nulled' or empty string for parameter \p{replacement}.
    /// To delete all rules, invoke the method with parameter \p{domainPath} \e 'nulled'
    /// or empty.
    ///
    /// <b>Final remarks</b>
    /// Domain substitution is useful to permanently change ('redirect') domain paths of
    /// 3rd party code (e.g., libraries using \alox) or log statements that must not be changed
    /// for other reasons. It is advised to not 'overuse' this feature, as side effects
    /// are inherent to the concept of <em>Domain Substitution</em>. For example, an unwanted
    /// side effect might be that <em>Prefix Logables</em> are not applicable to the substituted
    /// domain, while other <em>Prefix Logables</em> are bound to the resulting domain.
    ///
    /// For \b %Lox objects that should be protected of external manipulation, it is advisable
    /// to remove all <em>Domain Substitution Rules</em> right after the \b %Lox was created by
    /// invoking this method with a \e nulled value for parameter \p{domainPath}.
    /// The reason is that otherwise, through configuration files or command line parameters,
    /// domains of the \b %Lox can be substituted and then the resulting domains \e Verbosities
    /// be \e overwritten using further configuration variables.
    /// Any prioritized \e 'internal' setting of \e Verbosities this way could be circumvented!
    ///
    /// For more information, consult the chapter \ref alib_mod_alox_domain_substitution of the
    /// Programmer's Manual.
    ///
    /// @param domainPath  The path to search. Has to start with either <c>'/'</c> or <c>'*'</c>.
    /// @param replacement The replacement path.
    void     SetDomainSubstitutionRule( const NString& domainPath, const NString& replacement )
    { detail::LI::SetDomainSubstitutionRule(impl, domainPath, replacement); }

    /// This method is used to remove an <em>explicitly given</em> domain path from the list
    /// of domain paths set for \e %Scope::ThreadOuter or \e %Scope::ThreadInner.
    ///
    /// To remove the most recently added domain path from such thread-related \e %Scope,
    /// use one of the overloaded methods #SetDomain and provide an empty or \e nulled
    /// value for parameter \p{scopeDomain} (the same as how domain paths of other \e %Scopes
    /// are removed).
    ///
    /// \note
    ///   The long name of the method already indicates that this method is a little special.
    ///   Only seldom, more than one <em>%Scope Domain</em> is needed to be added. And if this
    ///   is needed, then such <em>%Scope Domains</em> usually get removed in reverse order of
    ///   their definition, with is performed using the standard interface that allows 'removing'
    ///   any other <em>%Scope Domain</em>. (Passing an empty or \e nulled domain
    ///   path to method #SetDomain.)
    ///
    /// @param scopeDomain The domain path to register.
    /// @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
    ///                    values, an internal error is logged.
    /// @param thread      The thread to set/unset a thread-related Scope Domains for.
    ///                    Defaults to the current thread.
    void        RemoveThreadDomain( const NString&   scopeDomain, Scope scope,
                                    threads::Thread* thread= nullptr )
    { detail::LI::RemoveThreadDomain(impl, scopeDomain, scope, thread); }

    /// The given \p{prefix} becomes a <em>Prefix Logable</em> provided to loggers with each log
    /// statement executed within the given \p{scope}.
    /// The list of objects received by a logger is sorted from outer scope to inner scope.
    /// The logable of the <em>%Log Statement</em> itself, is the last in the list, except one
    /// or more <em>Prefix Logables</em> of \e %Scope::ThreadInner are set. Those are (similar
    /// to how this \e %Scope is used with <em>%Scope Domains</em>) appended to the end of the
    /// list.
    ///
    /// To remove a previously set <em>Prefix Logable</em>, \c nullptr has to be passed with
    /// parameter \p{prefix}.
    /// For \e %Scope::ThreadOuter and \e %Scope::ThreadInner, passing \c nullptr (respectively
    /// with the overloaded method accepting string messages, a \e nulled string)
    /// removes the most recently added <em>Prefix Logable</em>.
    ///
    ///
    /// \note
    ///   \e Logables of boxed character array types are duplicated internally by \alox when
    ///   setting as <em>Prefix Logables</em>.
    ///   This means, in contrast to other types, for string-type <em>Prefix Logables</em>
    ///   the life-cycle of the object passed in parameter \p{prefix} is allowed to end
    ///   right after the invocation of this method. This is a convenience feature of \alox.
    ///   However, this also means, that changes of the strings that occur after the string
    ///   objects got set as a <em>Prefix Logable</em>, are \b not reflected.<br>
    ///   To implement a "variable" <em>Prefix Logable</em> of string-type, an object of type
    ///   \b %AString might be passed wrapped in class \c std::reference_wrapper<AString>.<br>
    ///   For more information, consult manual chapter
    ///   \ref alib_mod_alox_prefix_logables_lifecycle
    ///   as well as chapter \ref alib_boxing_customizing_identity of the Programmer's Manual
    ///   of module \alib_boxing.
    ///<p>
    /// \note
    ///   Unlike other methods of this class which accept an arbitrary amount of logables, this
    ///   method and its overloaded variants accept only one logable (the prefix).
    ///   To supply several objects to be prefix logables at once, a container of type
    ///   \alib{boxing;TBoxes} may be passed with parameter \p{logables}, like
    ///   in the following sample:
    ///   \snippet "ut_alox_log_scopes.cpp"      DOX_ALOX_LOX_SETPREFIX
    ///   The provided container as well as the prefixes themselves have to be kept in memory
    ///   until they are unset.
    ///
    ///<p>
    /// \note
    ///   The C++ version of \alox implements scope mechanisms using scope information
    ///   generated by the preprocessor. By default, debug logging supports such caller
    ///   information, release logging does not. Both defaults can be changed with preprocessor
    ///   symbols \ref ALOX_DBG_LOG_CI and \ref ALOX_REL_LOG_CI.
    ///
    ///<p>
    /// \note
    ///   The word 'prefix' in this method's name and in the name of \alox feature
    ///   <em>Prefix Logables</em> is chosen for the fact that with text loggers (which is the
    ///   most widely applied use case for \alox) such objects are prefixes to the log
    ///   message. Of course, with using \e %Scope::ThreadInner, this turns into a suffix!<br>
    ///   When using \alox to process arbitrary objects instead of text messages, the concept of
    ///   <em>Prefix Logables</em> is still very useful. Just the name does not fit so well
    ///   anymore. Think of 'SetContext' and <em>Context Objects</em> instead.
    ///
    /// @param prefix      The <em>Prefix Logable</em> to set.
    /// @param scope       The scope that should the given \p{domain} be registered for.
    ///                    Available Scope definitions are platform/language dependent.
    void        SetPrefix( const Box& prefix, Scope scope )
    { detail::LI::setPrefix( impl, prefix, scope, nullptr ); }

    /// This overloaded version of
    /// \ref SetPrefix(const Box&,Scope) "SetPrefix" is applicable only for
    /// \b %Scope::ThreadOuter and \b %Scope::ThreadInner and allows specifying the thread that
    /// the setting should be associated with.
    ///
    /// If \p{scopeDomain} is nullptr or empty, the most recently added <em>Prefix Logable</em>
    /// is removed.
    ///
    /// @param prefix      The <em>Prefix Logable</em> to set.
    /// @param scope       Either \e %Scope::ThreadOuter or \e %Scope::ThreadInner. With other
    ///                    values, an internal error is logged.
    /// @param thread      The thread to set/unset a thread-related Scope Domains for.
    void        SetPrefix( const Box& prefix, Scope scope, threads::Thread* thread )
    { detail::LI::setPrefix( impl, prefix, scope, thread ); }

    /// The given \p{prefix} becomes a <em>Prefix Logable</em> associated to the given
    /// <em>Log Domain</em>.
    /// <em>Prefix Logables</em> associated with the <em>Log Domain</em> are added to the
    /// list of \e Logables right
    /// before the main \e Logable of the <em>Log Statement</em> itself.
    /// Multiple <em>Prefix Logables</em> can be added per <em>Log Domain</em>.
    ///
    /// To remove the most recently added <em>Prefix Logable</em> associated with a
    /// <em>Log Domain</em>, \c nullptr has to be passed with parameter \p{prefix}.
    ///
    /// \note
    ///   String-type \e Logables are duplicated internally by \alox when setting as
    ///   <em>Prefix Logables</em>.
    ///   This means, different to <em>Prefix Logables</em> of type \b %AString or custom types,
    ///   the life-cycle of the object passed in parameter \p{prefix} is allowed to end
    ///   right after the invocation of this method. For more information, consult manual
    ///   chapter \ref alib_mod_alox_prefix_logables_lifecycle as well as chapter
    ///   \ref alib_boxing_customizing_identity of the Programmer's Manual of module
    ///   \alib_boxing.
    ///
    /// \attention
    ///   The same as with most interface methods of this class, the given \p{domain} parameter
    ///   is combined with <em>%Scope Domains</em> set for the caller's \e %Scope.
    ///   To suppress this, an absolute domain path can be used. (Still any
    ///   <em>Scope Domain</em> of \e %Scope::Thread::Inner will be applied).
    ///   The default value of parameter \p{domain} is \c "" which addresses the domain
    ///   evaluated for the current scope.
    ///
    /// @param prefix      The <em>Prefix Logable</em> to set.
    /// @param domain      The domain path. Defaults to \c nullptr, resulting in
    ///                    evaluated <em>Scope Domain</em> path.
    /// @param otherPLs    If set to \c Inclusion::Exclude, scope-related
    ///                    <em>Prefix Logables</em> are ignored and only domain-related
    ///                    <em>Prefix Logables</em> are passed to the \e Loggers.<br>
    ///                    Defaults to \c Inclusion::Include.
    void        SetPrefix( const Box& prefix, const NString& domain= nullptr,
                           lang::Inclusion otherPLs=  lang::Inclusion::Include )
    { detail::LI::SetPrefix(impl, prefix, domain, otherPLs); }

    /// This method is used reset (or to explicitly set) the start time of one or all logger(s).
    /// The only impact is the output of time differences in the log lines. Hence, this method
    /// is useful to reset them and see some absolute time values when doing basic performance
    /// tests using the \e Logger.
    ///
    /// \note This affects loggers that are registered for at least one standard domain.
    ///       In other words, loggers that are exclusively attached to the internal domain,
    ///       will not be affected.
    ///
    /// @param startTime  Optional parameter with the new start time. Defaults
    ///                   to current time if omitted.
    /// @param loggerName The name of the \e Logger(s) whose start time is to be set
    ///                   (case-insensitive).
    ///                   Defaults to nullptr, which indicates that all loggers are to
    ///                   be affected.
    void SetStartTime( Ticks           startTime    = time::Ticks    (),
                       const NString&  loggerName   = nullptr                 )
    { detail::LI::SetStartTime(impl, startTime, loggerName); }

        #if defined (__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(__APPLE__)
    /// Converts the given \p{startTime} and invokes #SetStartTime(Ticks,const NString&).
    /// \note GLib specific.
    ///
    /// @param startTime  The new start time in system-specific time unit.
    /// @param loggerName The name of the \e Logger whose start time is to be set (case
    ///                   insensitive).
    ///                   Defaults to empty string, which indicates that all loggers are to
    ///                   be affected.
    void SetStartTime( time_t startTime, const NString& loggerName= nullptr )
    { detail::LI::SetStartTime(impl, startTime, loggerName); }


        #endif // no elif here, otherwise doxygen would ignore it!

        #if defined( _MSC_VER )
            //======================================================================================
            /// Converts the given \p{startTime} and invokes#SetStartTime(Ticks,const NString&).
            /// \note  Microsoft Windows specific.
            ///
            /// @param startTime  The new start time in system-specific time unit.
            /// @param loggerName The name of the \e Logger whose start time is to be set (case
            ///                   insensitive).
            ///                   Defaults to empty string, which indicates that all loggers are to
            ///                   be affected.
            //======================================================================================
            void    SetStartTime( const FILETIME& startTime, const NString& loggerName= nullptr )
            {
                detail::LI::SetStartTime(impl, startTime, loggerName);
            }

        #endif

    /// This method sets a human-readable name to the given thread ID (or current thread) which
    /// is optionally included in each log line.
    ///
    /// @param threadName    The name of the thread as it should be displayed in the logs.
    /// @param id            (Optional) Parameter providing the thread ID. If omitted, the
    ///                      current thread's ID is used.<br>
    ///                      If given, the associated object of \alib{threads;Thread} must not
    ///                      be deleted until this method returns. This is a race condition
    ///                      that a using code has do ensure.
    void        MapThreadName( const String& threadName, threads::ThreadID id= 0 )
    { detail::LI::MapThreadName(impl, threadName, id); }

    /// Stores data encapsulated in an object of class
    /// \ref alib::boxing::Box "Box" which can be retrieved back by invoking
    /// #Retrieve. Using the optional \p{key} and \p{scope} offer various possibilities to
    /// reference this data later.
    ///
    /// To remove data from the store, pass \c nullptr with parameter \p{data}.
    ///
    /// \attention
    ///   When data objects are 'overwritten', previous objects will be deleted internally.
    ///   Hence, only pointers to heap-allocated objects (created with \c new) may be
    ///   passed!<br>
    ///   For more information, consult chapter \ref alib_mod_alox_log_data of the
    ///   Programmer's Manual.
    ///
    /// \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
    ///       It is not advised to use <em>Log Data</em> to implement application logic.
    ///
    /// @param data      The data object to store.
    ///                  If \c nullptr, currently stored data will be removed.
    ///                  In C++, has to be heap allocated and will be deleted
    ///                  by this \b %Lox when overwritten or this lox is deleted.
    /// @param key       The optional key to the data.
    ///                  If omitted (or empty or nullptr), the data is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  data is unique to the \e Lox.
    /// @param scope     The \e %Scope that the data is bound to.
    void        Store( const Box& data, const NString& key, Scope scope= Scope::Global )
    { detail::LI::store( impl, data, key, scope ); }

    /// Overloaded version of
    /// Store(const Box&,const String&,Scope,int) "Store" which omits parameter \p{key}.
    /// @param data      The data object to store.
    ///                  In C++, has to be heap allocated and will be deleted
    ///                  by this \b %Lox when overwritten or this lox is deleted.
    /// @param scope     The \e %Scope that the data is bound to.
    void        Store( const Box& data, Scope scope= Scope::Global )
    { detail::LI::store( impl, data, nullptr, scope ); }

    /// Retrieves \alox <em>Log Data</em>, an object type
    /// \ref alib::boxing::Box "Box" which had been stored in a prior call to
    /// #Store. Using the optional \p{key} and \p{scope} offer various possibilities to reference
    /// such objects.<br>
    ///
    /// \note If no data is found, an \e nulled object is returned. This can be tested using method
    ///       \ref alib::boxing::Box::IsNull "Box::IsNull".
    ///
    /// <p>
    /// \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
    ///       It is not advised to use <em>Log Data</em> to implement application logic.
    ///
    /// @param key       The optional key to the data.
    ///                  If omitted (or empty or nullptr), the data is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  data is unique to the \e Lox.
    /// @param scope     The \e %Scope that the data is bound to.
    /// @return The data, a \e nulled box if no value was found.
    ALIB_DLL
    Box       Retrieve  ( const NString& key, Scope scope= Scope::Global )
    { return detail::LI::retrieve( impl, key, scope ); }

    /// * Overloaded version of #Retrieve which omits parameter \p{key}.
    ///
    /// \note <em>Log Data</em> is a feature provided by \alox to support debug-logging.
    ///       It is not advised to use <em>Log Data</em> to implement application logic.
    ///
    /// @param scope     The \e %Scope that the data is bound to.
    /// @return The data, a \e nulled box if no value was found.
    ALIB_DLL
    Box  Retrieve  ( Scope scope= Scope::Global )
    { return detail::LI::retrieve( impl, nullptr, scope ); }

    /// This method logs the current configuration of this Lox and its encapsulated objects.
    /// It uses method #GetState to assemble the logable string.
    ///
    /// \note
    ///   As an alternative to (temporarily) adding an invocation of <b>%Lox.State</b> to
    ///   your code, \alox provides configuration variable\ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT.
    ///   This allows enabling an automatic invocation of this method using external
    ///   configuration data like command line parameters, environment variables or
    ///   INI files.
    ///
    /// @param domain        Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
    ///                      set for the \e %Scope of invocation.
    /// @param verbosity     The verbosity.
    /// @param headLine      If given, a separated headline will be logged at first place.
    /// @param flags         Flag bits that define which state information is logged.
    void        State   ( const NString&    domain,
                          Verbosity         verbosity,
                          const String&     headLine,
                          StateInfo         flags        = StateInfo::All  )
    { detail::LI::State(impl, domain, verbosity, headLine, flags); }

    /// This method collects state information about this lox in a formatted multi-line AString.
    /// Parameter \p{flags} is a bitwise enum type (operators on elements available).
    ///
    /// \note
    ///   As an alternative to (temporarily) adding an invocation of <b>%Lox.State</b> to
    ///   your code, \alox provides configuration variable \ref alxcvALOX_LOXNAME_DUMP_STATE_ON_EXIT.
    ///   This allows enabling an automatic invocation of this method using external
    ///   configuration data like command line parameters, environment variables or
    ///   INI files.
    /// @param buf        The target string.
    /// @param flags      Bits that define which state information is collected.
    void        GetState( NAString& buf, StateInfo flags= StateInfo::All   )
    { detail::LI::GetState(impl, buf, flags); }


  //################################################################################################
  // Main logging methods
  //################################################################################################
    /// Returns a reference to a list of boxes to be used for logging. The list is recycled
    /// from a previous log operation and cleared. The method may be used to retrieve
    /// a container for logables that then are collected until finally logged.<br>
    /// Note that the \b %Lox instance has to be acquired before invoking this method and
    /// the container returned must be used only while the object is still acquired.
    ///
    /// With each recursive acquirement of this object, a different container is returned.
    /// This is implemented to allow recursive log calls.
    ///
    /// @return An empty list of boxes.
    BoxesMA&  GetLogableContainer()                { return detail::LI::GetLogableContainer(impl); }

    /// Logs the current list of \e Logables that previously have been received using
    /// #GetLogableContainer with the given \p{verbosity}.
    ///
    /// This method is usually \b not used directly. Instead, methods
    /// #Info, #Verbose, #Warning and #Error provide simpler interfaces which take variadic
    /// arguments that are collected in a list of boxed objects and then passed to
    /// this methods.<br>
    /// Note that the other interface methods accept an "external" list of boxes as a parameter.
    /// as well. This means also with these methods it is allowed to collect the logables in
    /// an user-specific list first and later pass them to these methods.
    ///
    /// Hence, the use of this method is recommended only if the verbosity of a log statement
    /// is evaluated only at run-time.
    ///
    /// @param domain        The domain.
    /// @param verbosity     The verbosity.
    void Entry( const NString&  domain, Verbosity verbosity )
    { detail::LI::Entry(impl, domain, verbosity); }

    /// Logs a list of \e Logables with the given \e %Verbosity.
    ///
    /// If more than one \e Logable is given and the first one is of string-type and comprises a
    /// valid domain path, then this first argument is interpreted as the domain name!
    /// Valid domain path are strings that consists only of characters of the following set:
    /// - upper case letters,
    /// - numbers,
    /// - hyphen (\c '-'),
    /// - underscore (\c '_') and
    /// - forward slash (\c '/').
    ///
    /// If a first \e Logable could be misinterpreted as being a domain name, an empty string
    /// (the "neutral" domain) has to be added as a first argument. Alternatively, a character
    /// which is illegal in respect to domain names could be added to the first argument,
    /// for example, a simple space at the end of an output string.
    ///
    /// \note
    ///   This method allows a consistent interface of overloaded methods \b %Info, \b Error,
    ///   etc, without introducing a separate version which excepts a - then mandatory - domain
    ///   parameter.
    ///   The little drawback of the auto detection is the possibility of ambiguous invocations.
    ///
    /// @param verbosity The verbosity.
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void EntryDetectDomain( Verbosity verbosity,  BoxedObjects&&...  logables )
    {
        detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
        detail::LI::entryDetectDomainImpl( impl, verbosity );
    }

    /// Logs given logables using \alib{lox;Verbosity;Verbosity::Verbose}.
    ///
    /// The first object provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type
    /// \alib{boxing,TBoxes}, then the contents of this list is inserted into
    /// the list of logables. This allows collecting logables before invoking the method.
    ///
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void Verbose( BoxedObjects&&... logables )
    {
        detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
        detail::LI::entryDetectDomainImpl( impl, Verbosity::Verbose );
    }

    /// Logs given logables using \alib{lox;Verbosity;Verbosity::Info}.
    ///
    /// The first object provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type
    /// \alib{boxing,TBoxes}, then the contents of this list is inserted into
    /// the list of logables. This allows collecting logables before invoking the method.
    ///
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void Info( BoxedObjects&&... logables )
    {
        detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
        detail::LI::entryDetectDomainImpl( impl, Verbosity::Info );
    }

    /// Logs given logables using \alib{lox;Verbosity;Verbosity::Warning}.
    ///
    /// The first object provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type
    /// \alib{boxing,TBoxes}, then the contents of this list is inserted into
    /// the list of logables. This allows collecting logables before invoking the method.
    ///
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void Warning( BoxedObjects&&... logables )
    {
        detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
        detail::LI::entryDetectDomainImpl( impl, Verbosity::Warning );
    }

    /// Logs given logables using \alib{lox;Verbosity;Verbosity::Error}.
    ///
    /// The first object provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type
    /// \alib{boxing,TBoxes}, then the contents of this list is inserted into
    /// the list of logables. This allows collecting logables before invoking the method.
    ///
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void Error( BoxedObjects&&... logables )
    {
        detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
        detail::LI::entryDetectDomainImpl( impl, Verbosity::Error );
    }

    /// Logs given logables only if the parameter \p{condition} is not \c true.
    /// If executed, \alib{lox;Verbosity;Verbosity::Error} is used.
    ///
    /// The first object provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type \alib{boxing,TBoxes},
    /// then the contents of this list are inserted into the list of logables. This allows
    /// collecting logables before invoking the method.
    ///
    /// @param condition If \c false, the <em>Log Statement</em> is executed.
    /// @param logables  The list of \e Logables, optionally including a domain name at the start.
    template <typename... BoxedObjects>
    void Assert( bool condition, BoxedObjects&&... logables ) {
        if (!condition ) {
            detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
            detail::LI::entryDetectDomainImpl( impl, Verbosity::Error );
        }
        else
            detail::LI::IncreaseLogCounter( impl );
    }

    /// Logs a list of \e Logables only if the parameter \p{condition} is \c true.
    ///
    /// \see Method #Assert.
    ///
    /// @param condition If \c false, the <em>Log Statement</em> is executed.
    /// @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
    ///                  set for the \e %Scope of invocation.
    /// @param verbosity The verbosity.
    /// @param logables  The list of \e Logables.
    template <typename... BoxedObjects>
    void If( bool condition, const NString& domain, Verbosity verbosity,
             BoxedObjects&&... logables ) {
        if ( condition ) {
            detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
            detail::LI::Entry( impl, domain, verbosity );
        }
        else
            detail::LI::IncreaseLogCounter( impl );
    }

    /// Logs a list of \e Logables only if the parameter \p{condition} is \c true.<br>
    ///
    /// This overloaded version omits parameter \p{domain}.
    /// The first \e logable provided may be a domain name. All values are passed to
    /// #EntryDetectDomain. See documentation of this method for information on how to avoid
    /// ambiguities in respect to domain names.
    ///
    /// If one of the arguments (or a single argument given) is of type
    /// \alib{boxing,TBoxes}, then the contents of this list is inserted into
    /// the list of logables. This allows collecting logables before invoking the method.
    ///
    /// \see Method #Assert.
    ///
    /// @param condition If \c false, the <em>Log Statement</em> is executed.
    /// @param verbosity The verbosity.
    /// @param logables  The list of \e Logables.
    template <typename... BoxedObjects>
    void If( bool condition, Verbosity verbosity, BoxedObjects&&... logables ) {
        if ( condition ) {
            detail::LI::GetLogableContainer(impl).Add( std::forward<BoxedObjects>(logables)... );
            detail::LI::entryDetectDomainImpl( impl, verbosity );
        }
        else
            detail::LI::IncreaseLogCounter( impl );
    }


    /// Logs given \e logables once, up to \p{quantity} times, or every n-th time.
    /// In its simplest overloaded version, the counter is bound to the source code line, hence,
    /// only the first execution of this exact <em>Log Statement</em> is executed.
    ///
    /// With parameter \p{group}, a set of <em>Log Statements</em> that share the same group key,
    /// can be grouped and of such set, only the one which is first executed actually logs.<br>
    /// Alternatively, when \p{key} is omitted (or nullptr or empty), but a
    /// \ref alib::lox::Scope "Scope" is given with parameter \p{scope}, then the
    /// counter is associated with the scope.<br>
    /// Finally, parameters \p{key} and \p{scope} can also be used in combination. The key is
    /// then unique in respect to the \ref alib::lox::Scope "Scope" provided.
    ///
    /// Using, none, one or both of the parameters \p{group} and \p{scope}, among others, the
    /// following use cases can be achieved.
    /// - %Log a specific statement up to n-times.
    /// - %Log only the first n of a group of statements.
    /// - %Log only the first n statements within a method.
    /// - %Log only the first n statements belonging to the same group and method .
    /// - %Log only the first n statements within any method of
    ///   - a source file
    ///   - a directory of source files
    ///   - a parent directory of source files and all sources recursively
    /// - %Log only the first n statements which belong to the same group and are placed within
    ///   any method of
    ///   - a source file
    ///   - a directory of source files
    ///   - a parent directory of source files and all sources recursively
    /// - %Log a <em>Log Statement</em> n-times per new thread.
    /// - %Log only the first n statements of a group of statements executed by a specific thread.
    ///
    /// When parameter \p{quantity} is a negative value, the log statement is executed every n-th time
    /// instead n-times. E.g, if \p{quantity} is \c -5, the first statement is executed and afterwards
    /// every fifth invocation.
    ///
    /// \note
    ///   Unlike other methods of this class which accept an arbitrary amount of logables, this
    ///   method and its overloaded variants accept only one boxed object.
    ///   To still be able to  supply several objects at once, an array of boxes or a container
    ///   of type \alib{boxing,TBoxes} may be passed with parameter
    ///   \p{logables}, like in the following sample:
    ///   \snippet "ut_alox_lox.cpp"      DOX_ALOX_LOX_ONCE
    ///   This is why the parameter name \p{logables} still uses the plural with its name!
    ///
    ///
    /// @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
    ///                  set for the \e %Scope of invocation.
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param group     The optional name of the statement group . If used, all statements that
    ///                  share the same group name are working on the same counter (according
    ///                  to the \p{scope}.)
    ///                  If omitted (or empty or nullptr), the counter is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  counter is associated exclusively with the single <em>Log Statement</em> itself.
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    ///                  If negative, the first and every "-quantity-th" statement is executed.
    void Once( const NString&   domain  , Verbosity verbosity,
               const Box&       logables,
               const String&    group,
               Scope            scope= Scope::Global,
               int quantity= 1)
    {
        detail::LI::once( impl, domain, verbosity, logables, group, scope, quantity );
    }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param group     The optional name of the statement group . If used, all statements that
    ///                  share the same group name are working on the same counter (according
    ///                  to the \p{scope}.)
    ///                  If omitted (or empty or nullptr), the counter is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  counter is associated exclusively with the single <em>Log Statement</em> itself.
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                        Verbosity verbosity, const Box& logables,
               const String& group,
               Scope scope,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, verbosity, logables, group, scope, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param group     The optional name of the statement group . If used, all statements that
    ///                  share the same group name are working on the same counter (according
    ///                  to the \p{scope}.)
    ///                  If omitted (or empty or nullptr), the counter is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  counter is associated exclusively with the single <em>Log Statement</em> itself.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                        Verbosity verbosity, const Box& logables,
               const String& group,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, verbosity, logables, group, Scope::Global, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                        Verbosity verbosity, const Box& logables,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, verbosity, logables, nullptr, Scope::Global, quantity); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param group     The optional name of the statement group . If used, all statements that
    ///                  share the same group name are working on the same counter (according
    ///                  to the \p{scope}.)
    ///                  If omitted (or empty or nullptr), the counter is bound to the \e %Scope
    ///                  provided. If omitted and \p{scope} is Scope::Global, then the
    ///                  counter is associated exclusively with the single <em>Log Statement</em> itself.
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                                          const Box& logables,
               const String& group,
               Scope scope,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, Verbosity::Info, logables, group, scope, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param domain    Optional <em>Log Domain</em> which is combined with <em>%Scope Domains</em>
    ///                  set for the \e %Scope of invocation.
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once( const NString& domain, Verbosity verbosity, const Box& logables,
               Scope scope= Scope::Global ,
               int quantity= 1)
    { detail::LI::once( impl, domain, verbosity, logables, nullptr, scope, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param verbosity The verbosity of the <em>Log Statement</em> (if performed).
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                        Verbosity verbosity, const Box& logables,
               Scope scope,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, verbosity, logables, nullptr, scope, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param scope     The \e %Scope that the group or counter is bound to.
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                                          const Box& logables,
               Scope scope,
               int quantity= 1)
    { detail::LI::once( impl, nullptr, Verbosity::Info, logables, nullptr, scope, quantity ); }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param logables  The objects to log (Multiple objects may be provided within
    ///                  container class Boxes.)
    /// @param quantity  The number of logs to be performed. As the name of the method indicates,
    ///                  this defaults to \c 1.
    void Once(                                          const Box& logables,
               int quantity= 1) {
        detail::LI::once( impl, nullptr, Verbosity::Info, logables, nullptr, Scope::Global,
                        quantity );
    }

    /// Overloaded version of
    /// \ref Once(const NString&,Verbosity,const Box&,const String&,Scope,int) "Once".
    ///
    /// @param logables The objects to log (Multiple objects may be provided within
    ///                 container class Boxes.)
    /// @param group    The optional name of the statement group . If used, all statements that
    ///                 share the same group name are working on the same counter (according
    ///                 to the \p{scope}.)
    ///                 If omitted (or empty or nullptr), the counter is bound to the \e %Scope
    ///                 provided. If omitted and \p{scope} is Scope::Global, then the
    ///                 counter is associated exclusively with the single <em>Log Statement</em>
    ///                 itself.
    /// @param quantity The number of logs to be performed. As the name of the method indicates,
    ///                 this defaults to \c 1.
    void Once(                                          const Box& logables,
               const String& group, int quantity= 1 ) {
        detail::LI::once( impl, nullptr, Verbosity::Info, logables, group, Scope::Global,
                          quantity );
    }

    /// Returns the number of loggers attached, which are active in respect to the given
    /// combination of verbosity and log domain.
    ///
    /// This method may be used to avoid the execution of more complex logging code when such
    /// logging would not result in log activity due to the current log level settings.
    ///
    /// @param      verbosity    The verbosity to query for activity.
    /// @param      domain       The log domain. All rules for resolving the effective log
    ///                          domain apply as with normal log statements.<br>
    ///                          Defaults to \b NULL_NSTRING.
    /// @param[out] resultDomain If given, the resulting domain is copied to this string.
    ///                          The resulting domain is that domain which is evaluated by
    ///                          \alox from the optional \p{domain} and all rules that apply
    ///                          at the place of invocation.
    /// @return The number of active loggers.
    int IsActive( Verbosity verbosity, const NString&  domain      = NULL_NSTRING,
                                            NAString*  resultDomain= nullptr   )
    { return detail::LI::IsActive( impl, verbosity, domain, resultDomain ); }

  //################################################################################################
  // Debug methods
  //################################################################################################
    #if ALIB_DEBUG_MEMORY
        //==========================================================================================
        /// Returns the internal \b MonoAllocator used for storing permanent data.
        ///
        /// \par Availability
        ///   This method is available only with debug-builds with \ref ALIB_DEBUG_MEMORY set.
        ///
        /// @return The monotonic allocator of this \b Lox.
        //==========================================================================================
        MonoAllocator& DbgGetMonoAllocator()
        {
            return  detail::LI::DbgGetMonoAllocator(impl);
        }
    #endif

}; // class Lox

namespace detail {
/// Internal lox management.
/// @return The current number of loxes
integer dbgCountLoxes();

/// Internal lox management.
void    shutdownLoxes();
}

} // namespace alib[::lox]

/// Type alias in namespace \b alib.
using     Lox=           lox::Lox;

}  // namespace [alib]

ALIB_ENUMS_MAKE_BITWISE( alib::lox::StateInfo )

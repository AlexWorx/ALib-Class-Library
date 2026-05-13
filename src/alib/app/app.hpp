//==================================================================================================
/// \file
/// This header-file is part of module \alib_app of the \aliblong.
///
/// Copyright 2013-2026 A-Worx GmbH, Germany.
/// Published under #"mainpage_license".
//==================================================================================================
ALIB_EXPORT namespace alib { namespace app {

//==================================================================================================
/// This is a central class of library module \alib_app_nl.
///
/// For information about general use and features of this class consult the
/// #"alib::app;ALib App User Manual".
//==================================================================================================
class App {
  protected:
    friend class AppCamp;
    
    /// Built-in commands used with class #"App".
    enum class Commands {
        Help                 = 1, ///< Show help text.
        Version              = 2, ///< Show version information.
        Info                 = 3, ///< Show info and configuration details.
    };

    /// Built-in options used with class #"App".
    enum class Options {
        Help                  = 1, ///< Alternative to command 'help' as many people are used to
                                   ///< pass the help command as an option.
        Version               = 2, ///< Alternative to command 'version' as many people are used to
                                   ///< pass the version command as an option.
        DryRun                = 3, ///< Sets the flag #"App::dryrun".
        Config                = 4, ///< Change the configured config file path(s).
        Verbose               = 5, ///< Reserved option for increasing verbosity.
    };

    /// Built-in parameters of commands and options used with class #"App".
    enum class Parameters {
        Topic                 = 1, ///< Used with command \e help to optionally denote a help topic.
    };

    /// Built-in exit-codes of the \alib application.
    /// The codes are mostly generated in the method #".exceptionToExitCode", where
    /// #"CLIExceptions;exceptions thrown by the CLI implementation" are translated to these
    /// codes.<br>
    /// In the case that an application desires to use different exit codes, the latter method has
    /// to be overridden.
    ///
    /// This enumeration gets the according #"alib_enums_records;ALib Enum Records" assigned
    /// as well as the #"alib_app_cli_candp_defines;resources describing the exit error".
    enum class ExitCodes {
        OK                      =   0, ///< Success.
        ErrNoCmdGiven           =   1, ///< No command given.
        ErrUnknownCommand       =   2, ///< Unknown command.
        ErrParsingOption        =   3, ///< Error when parsing an option.
        ErrParsingCommand       =   4, ///< Error when parsing a command.
        ErrMissingCmdParam      =   5, ///< Missing parameter of a command.
        ErrMissingOptParam      =   6, ///< Missing parameter of an option.
        ErrBadParamValue        =   7, ///< Bad parameter value.
        ErrConfigFileNotFound   =  10, ///< The default configuration file(s), or that (those)
                                       ///< specified by option '--config', could not be found
                                       ///< or read.
        ErrConfigFileNotWritable=  11, ///< The configuration file(s) could not be written.
        ErrUnknown              = 127, ///< An unknown exception occurred. This should never happen.
    };


  public:
  //================================================================================================
  // App Inner Types
  //================================================================================================

  //=========================================== Exceptions =========================================
    /// Generic Exceptions handled by class #"App".
    /// This enumeration contains only one single entry.
    enum class Exceptions {
        /// This exception is used to support the exception/error/exit code paradigm introduced
        /// by class #"App".
        /// @see A detailed description on the usage of this exception is given in chapter
        ///      #"alib_app_controlledearlyexit" of the Programmer's Manual of the
        ///      module \alib_app.
        ControlledEarlyExit,
    };


  //=========================================== ExitCodes ==========================================


  protected:
  //============================================= Flags ============================================
    /// Configuration flags used with class #"App".
    enum class Flags {
        /// No flags set.
        NONE                                          = 0   ,

        /// If set, method #"App::onBsSetupALox;*" will create a release #"Lox"
        /// instance available via field #"App::GetRLox;*", as well as an attached logger
        /// available via field #"App::GetRLogger;*".
        CreateReleaseLox                              =(1<<0),

        /// If set, method #"App::onBsSetupALox;*" will create and attach the release
        /// logger to the debug #"%Lox" #"Lox".
        UseReleaseLoggerForDebugLogging               =(1<<1),

        /// If set, method #"App::onSdExportConfig;*" adds 'ExportAll' and 'writeback' to the
        /// verbosity variable of the #"App::releaseLogger;*" in the case that this variable
        /// had not been read from the INI file with #"App::onBsImportConfig;*".
        ALoxVerbosityExportAllAndWriteBackRelLogger   =(1<<2),

        /// If set, method #"App::onSdExportConfig;*" adds 'ExportAll' and 'writeback' to
        /// the verbosity variable of the debug-logger in the case that this variable had not been
        /// read from the INI file with #"App::onBsImportConfig;*".
        ALoxVerbosityExportAllAndWriteBackDbgLogger   =(1<<3),
    };


  //============================================= States ===========================================
    /// The predefined states of the simple linear state-machine implemented by outer class #"App".
    /// For each state, a corresponding virtual function exists.
    /// The pairs of states and the corresponding virtual functions are collected in the field
    /// #"StateMachine::Program;2".<br>
    /// Custom applications (derived from class #"%App") define their own enum type in case they
    /// want to insert additional pairs of state/methods into the program.
    /// This is possible because the collection of states in the field
    /// #"StateMachine::Program;2" is implemented as type #"boxing::Enum", which may
    /// hold enumeration elements of an arbitrary type.
    ///
    /// Note that the numbering of the enum elements is \b not of importance. The numbers
    /// found here were chosen to be quickly identifiable while debugging.
    /// The simple scheme is:
    /// - 1x: Bootstrap phase resources. (See also #"BootstrapPhases;2").
    /// - 2x: Bootstrap phase configuration/variables.
    /// - 3x: Bootstrap phase finalization.
    /// - 1xx: running phase.
    /// - 9xx: shutdown phase. (See also #"ShutdownPhases;2").
    ///
    /// Also, the numbers are \b not defining the order of execution, which is solely defined by
    /// the order in the vector #"StateMachine::Program;2".<br>
    ///
    /// The custom enumeration type used for potential custom states may but does not need to
    /// align. It even may have the same integral values as given here. This flexibility is
    /// gained by the little magic of class #"boxing::Enum" introduced by module \alib_boxing.
    enum class States {
        /// Initial state before starting the App.
        NOT_STARTED         = 0, ///< The starting state not associated with a method.

          //---------------------------------- initialize resources --------------------------------
        SetCamps             =1100 , ///< Invokes the virtual method #"onBsSetCamps".
        PrepareResources     =1200 , ///< Invokes the virtual method #"onBsPrepareResources".
        SetNameVersionAndInfo=1300 , ///< Invokes the virtual method #"onBsSetNameVersionAndInfo".
            // 3
          //------------------------ Initialize camps on configuration level -----------------------
        PrepareConfig        =2100, ///< Invokes the virtual method #"onBsPrepareConfig".
        PreloadVariables     =2200, ///< Invokes the virtual method #"onBsPreloadVariables".
        CLIDefine            =2300, ///< Invokes the virtual method #"onBsCLIDefine".
        CLIReadOptions       =2400, ///< Invokes the virtual method #"onBsCLIReadOptions".
        ImportConfig         =2500, ///< Invokes the virtual method #"onBsImportConfig".
        ConfigureCLI         =2600, ///< Invokes the virtual method #"onBsConfigureCLI".
            // 9

          //--------------------------- After alib::BootstrapPhases::Final -------------------------
        FinalizeBootstrap    =3100, ///< Invokes the virtual method #"onBsFinalizeBootstrap".
        ReadDryRunOption     =3200, ///< Invokes the virtual method #"onBsReadDryRunOption".
        SetupALox            =3300, ///< Invokes the virtual method #"onBsSetupALox".
            // 12

          //------------------------------------ The custom code -----------------------------------
        RunStart             =5000, ///< Invokes the virtual method #"onRunStart".
        Run                  =5100, ///< Invokes the virtual method #"onRun".
        RunEnd               =5200, ///< Invokes the virtual method #"onRunEnd".
            // 15

          //---------------------------------------- Shutdown --------------------------------------
        AnnounceShutdown     =9100, ///< Invokes the virtual method #"onSdAnnounceShutdown".
        CleanALox            =9200, ///< Invokes the virtual method #"onSdCleanALox".
        ExportConfig         =9300, ///< Invokes the virtual method #"onSdExportConfig".
        Output               =9400, ///< Invokes the virtual method #"onSdOutput".
        FinalizeShutdown     =9500, ///< Invokes the virtual method #"onSdFinalizeShutdown".
            // 20
    };

  //========================================== StateMachine ========================================
    /// This struct is used a single time with the member #"App::machine" and comprises all
    /// fields associated with the state of the application and the execution of the 'program'.
    struct StateMachine {

      //=================================== StateMachine::Command ==================================
        /// A struct denoting the next state and the corresponding method to execute.
        struct Command {
            /// A union holding either a pointer to a virtual method of class #"%App" or one to
            /// a non-virtual method of a different type.
            union MethodPointer{
                /// A pointer to a virtual method of class #"%App".
                void (App::*BuiltIn)();

                /// A pointer to a method of a custom derived type.
                void      (*Custom )(App&);

                /// Constructor accepting a pointer to a virtual method of class #"%App".
                /// @param method   The method to call.
                MethodPointer( void (App::*method)()     ) : BuiltIn(method)                      {}

                /// Constructor accepting a pointer to a non-virtual method of a derived type.
                /// @param method   The non-virtual method of a derived type.
                MethodPointer( void (     *method)(App&) ) : Custom (method)                      {}
            };

            /// The identifier of the command which is likewise the state of the machine.
            /// The underlying integral enumeration value determines the order of execution.
            /// During execution, this identifier will be stored in the member
            /// #"StateMachine::State" of the outer class #"%StateMachine".
            /// In case it is a built-in identifier of type #"App::States", it will
            /// be stored in addition in the field #"App::StateMachine::BuiltInState".
            Enum            State;

            /// A pointer union to either a built-in virtual method or a method of the custom
            /// derived type.
            MethodPointer   Method;
        };

      //================================= StateMachine::CommandList ================================
        /// A list of pairs of states and corresponding methods to process.
        /// This struct is used only once, namely with the field #"StateMachine::Program;2".
        /// The constructor of class #"%App" fills this list with all commands defined in the
        /// enumeration #"App::States;*".
        /// While the insertion method #".Add" is provided for convenience, more complex
        /// manipulation of the list is allowed using the interface inherited from
        /// <c>std::vector</c>.<br>
        /// The list will be sorted at the start of method #"App::Main;2".
        /// Consequently all commands have to be added prior to its invocation.
        struct CommandList : std::vector<Command> {

            /// Adds a custom execution command to this list.
            /// Creates a custom command, i.e. a command that is not built-in and as such is not
            /// executing one of the pre-defined virtual methods of class #"App".<br>
            /// Instead, it is executing a member of the given type \p{TDerived}, which is usually
            /// the descendant singleton-type, that implements the custom application.
            /// \note For technical reasons, the invocation of this static function is a little
            ///       counter-intuitive. The two template parameters have to be explicitly provided,
            ///       where the second is the address of the custom method! (This is C++20 magic).
            ///       The only normal parameter then is the custom enumeration element.
            /// @see A code sample is provided in chapter #"alib_app_app_statemachine" of the
            ///      Programmer's Manual of this module.
            /// @tparam TDerived        The enclosing type of the method. This usually is the
            ///                         custom implementation type of class #"%App".
            /// @tparam TMethodAddress  The address of the member which is to be called with this
            ///                         command.
            /// @param  state           The enum element (of a custom type) to identify the command
            ///                         and define the state of the machine during execution.
            template<typename TDerived, auto TMethodAddress >
            void Add(Enum state) {

                struct Helper {
                    static void Wrapper(App& base) {
                        auto& self = static_cast<TDerived&>(base);
                        (self.*TMethodAddress)();
                    }
                };

                push_back({ state, &Helper::Wrapper });
            }

        };

      //==================================== StateMachine Fields ===================================
      protected:
        /// The exit code. Only the first set is stored and returned later.
        Enum    exitCode                                                             =ExitCodes::OK;

        #if ALIB_DEBUG
        /// This vector collects exit codes which are not used, as they got set
        /// after a first code was given. This field is available in debug-compilations only.
        std::vector<Enum> dbgFurtherExitCodes;
        #endif
        
      //=================================== StateMachine Interface =================================
      public:
        /// The list of commands to execute.
        CommandList  Program;

        /// The current state in respect to the built-in program commands. Note that this is
        /// not of type #"boxing::Enum", but uses the original C++ enum type.
        /// Consequently, this is only updated when a built-in command is processed.
        /// With that, methods may check for a more general state without getting "confused"
        /// by custom insertions which often are not relevant for the decision-making.
        States  BuiltInState                                                   =States::NOT_STARTED;

        /// The current state. Either a built-in state enumeration or a custom one.
        /// @see Field #".BuiltInState", which is more appropriate to use in most cases.
        Enum    State                                                          =States::NOT_STARTED;

        /// Emergency stopper. As soon as this gets set, the execution of the application
        /// will stop. The only thing that is still performed is the standard
        /// #"alib::Shutdown;ALib shutdown procedure", but not the shutdown procedure defined
        /// by this app (or its derived type). I.e., no configuration files are written.
        bool    EmergencyStop                                                                =false;

        /// Sets this applications' cli-exit code (the result of the method \c main()).
        /// If the given \p{pExitCode}'s integral value is \c 0, nothing is done.
        ///
        /// Only the value of the first call is used. Later calls will not overwrite the first
        /// given value. In debug-builds, the method #".DbgDumpFurtherExitCodes" may be used
        /// to display later given codes, what the default implementation of the method
        /// #"App::onSdFinalizeShutdown;*" does.
        /// @param pExitCode The exit code to set.
        ///
        void SetExitCode(Enum pExitCode) {
            if ( pExitCode.Integral() == 0)
                return;

            if ( exitCode.IsNull() || exitCode.Integral() == 0 )
                exitCode= pExitCode;
            #if ALIB_DEBUG
                else
                    dbgFurtherExitCodes.push_back(pExitCode);
            #endif
        }

        /// Returns this applications' cli-exit code (the result of the method \c main()).
        /// @return The first value given with #".SetExitCode".
        Enum GetExitCode()                                                      { return exitCode; }

        /// Writes exit codes not respected by the method #".SetExitCode" (because an earlier code
        /// was already set) to the \p{target} string. Then clears the debug-field
        /// #".dbgFurtherExitCodes".
        /// This method is called by the default implementations of the methods
        /// #"App::exceptionDisplay;2" and #"onSdOutput"<br>.
        /// In release-builds, this method is pruned.
        /// @param target The target string.
        void DbgDumpFurtherExitCodes(AString& target) {
            #if ALIB_DEBUG
                for ( auto& ec : dbgFurtherExitCodes )
                    target << "Unused (later) exit-code: " << ec << NEW_LINE;
                dbgFurtherExitCodes.clear();
            #else
                (void) target;
            #endif
        }
    };

  //====================================== ConfigFileDescriptor ====================================
    /// A record collecting information about configuration files. A vector of this type is
    /// created by the method #"App::getConfigFilePaths;*".
    struct ConfigFileDescriptor {
        /// The resolved full path to the file.
        Path      Pathname;

        /// A comma-separated list of variables (and especially variable tree paths) to be
        /// explicitly exported to this configuration file.
        String    Exports;

        /// The files' main comment to set, in case it does not exist, yet.
        String    Comment;

        /// If set, the file name (and maybe path) came from option '--CONFIG'.
        bool      FromCfgOption                                                              =false;

        /// If set, the file did not exist or was empty at the start of the application.
        bool      WasEmpty                                                                   =false;
    };


  //================================================================================================
  // App Fields
  //================================================================================================

    /// The state-machine singleton.
    StateMachine machine;

    /// A stop-watch usable for measuring performance of different steps of an application.
    /// Time is taken in the constructor.
    StopWatch                       stopWatch;

    /// The name of the application.
    /// If not set from the descendant, it is tried to be loaded from resource <c>"AppName"</c>
    /// by the default implementation of #".onBsSetNameVersionAndInfo".
    String                          appName                                                =nullptr;

    /// A version string of the application.
    /// If not set from the descendant, it is tried to be loaded from resource <c>"AppVersion"</c>
    /// by the default implementation of #".onBsSetNameVersionAndInfo".
    String                          appVersion                                             =nullptr;

    /// Some short information and probably copyright message of the application.
    /// If not set from the descendant, it is tried to be loaded from resource <c>"AppInfo"</c>
    /// by the default implementation of #".onBsSetNameVersionAndInfo".
    String                          appInfo                                                =nullptr;

    /// Used to assemble and collect the output for standard output character stream \c std::cout
    /// during the run of this application.<br>
    /// The collected output is scheduled to be written by the method #".onSdOutput". This method,
    /// may also be "manually" invoked at any time to flush the current buffers to the output
    /// streams.
    Paragraphs*                     cOut                                                   =nullptr;

    /// Same as #".cOut", but used for stream \c std::err.
    Paragraphs*                     cErr                                                   =nullptr;


  //============================================== ALox ============================================
    /// This will be the name of the release #"%Lox".
    /// The name defaults to <c>"RLOX"</c> and may be changed prior to the call to the method
    /// #".onBsSetupALox".<br>
    NString                         releaseLoxName                                          ="RLOX";

    /// This will be the name of the release logger attached to the release #"%Lox".
    /// The name defaults to <c>"RLOGGER"</c> and may be changed prior to the call to the method
    /// #".onBsSetupALox".<br>
    NString                         releaseLoggerName                                    ="RLOGGER";

    /// The release #"%Lox" used by the application.
    /// @see Method onBsSetupALox.
    lox::Lox*                       releaseLox                                             =nullptr;

    /// The release logger used by the application.
    /// @see Method onBsSetupALox.
    lox::textlogger::TextLogger*    releaseLogger                                          =nullptr;

    /// Various boolean flags used to configure the application.
    Flags                           flags                                              =Flags::NONE;

  //============================================== CLI =============================================
    /// The command-line parser.
    CommandLine    cli;

    /// \brief Flag to stop the CLI processing loop.
    ///
    /// When set to \c true, no further commands from the command line are processed.
    /// Typical usage is the built-in or custom \c help command: once help text has been
    /// generated and printed, further command processing is not desired.
    bool                            cliStop                                                  =false;

    /// Set with built-in option <em>"--dryrun"</em>, which is read with
    /// #"States::ReadDryRunOption;2". Defaults to false.
    bool                            dryrun                                                   =false;



  //================================================================================================
  // Constructors/destructor
  //================================================================================================
    /// Constructor.
    /// Initializes this app. Mainly by filling the vector #".StateMachine::Program;2" with all
    /// states found in the enumeration #".States" along with the corresponding virtual methods.
    ALIB_DLL
    App();


    /// Virtual destructor.
    ALIB_DLL
    virtual ~App();

  //================================================================================================
  // Public Interface
  //================================================================================================
  public:
    /// Stores cli-parameters and executes this app.
    /// @param argc  The number of command-line arguments.
    /// @param argv  List of command-line arguments, given as single byte character strings.
    /// @param argvw The CLI arguments on platforms that use wide command-line strings.
    ///              Defaults to \c nullptr. If given, parameter \p{argv} has to be nulled.
    /// @return The exit code to be returned by C/C++ \c main().
    ALIB_DLL
    virtual int Main( int argc, const char** argv, const wchar_t** argvw= nullptr );

    /// Has to return the #"%Lox" instance used with release-logging.
    /// @return This implementation returns field #".releaseLox".
    virtual lox::Lox*                       GetRLox()                         { return releaseLox; }

    /// Has to return the #"%detail::Logger" instance used with release-logging.
    /// @return This implementation returns field #".releaseLogger".
    virtual lox::textlogger::TextLogger*    GetRLogger()                   { return releaseLogger; }

    /// Returns the name of this application.
    /// This default implementation returns the field #".appName".
    /// This is not necessarily equal to the process module name which can be received with
    /// <c>ProcessInfo::Current().ExecFileName</c>.
    /// @return This application's name.
    String                                  GetName()                            { return appName; }

    /// Returns the version of this application.
    /// This default implementation returns the field #".appVersion".
    /// @return A string containing version information of this application.
    String                                  GetVersion()                      { return appVersion; }

    /// Returns the version of this application.
    /// This default implementation returns the field #".appVersion".
    /// @return A usually multi-line string containing information about this application.
    String                                  GetInfo()                            { return appInfo; }

  //================================================================================================
  // Helper Hooks usable by the Program Methods
  //================================================================================================
    /// Translates exceptions to exit-codes of the application.
    /// Implementations should call this method first and, if necessary, perform their own
    /// conversions then. If they do so, they should check whether this default version returned
    /// #"ExitCodes::ErrUnknown", perform their own translation in that case, and probably
    /// debug-assert on failure.
    /// @param exception The exception that was thrown.
    /// @return The exit-code resulting from the given \p{exception}. Unknown exceptions are
    ///         translated to <c>BuiltInExitCodes::ErrUnknown</c>.
    virtual Enum   exceptionToExitCode( Exception& exception );

    /// Writes exception information to \c std::cerr<br>.
    /// @param exception The exception that was thrown.
    /// @param target    The target string to write into. An option is to pass <c>cErr->Buffer</c>
    ///                  here. Of course a custom buffer might be passed, i.e. if the text is to be
    ///                  logged with \alox.
    virtual void    exceptionDisplay  ( Exception& exception, AString& target );

    /// Writes configuration file information into the given \p{target}.
    /// @param target The target to write into. The common option is to pass #"cOut" here.
    virtual void    printConfigFileInfo( Paragraphs& target );

    /// This default implementation first calls #"getConfigFilePathsFromCLIParam" and,
    /// if this is unsuccessful, the method #".getConfigFilePathsFromResources". Then, method
    /// #".getConfigFilePathsMakeAbsolutePaths" is called.<br>
    /// CLI arguments may affect which configuration files are considered. Descendants can
    /// override #"getConfigFilePathsFromCLIParam" to implement their own CLI-based selection
    /// strategy before resource-defined defaults are completed.
    /// @param files A pointer to an object of type <c>StdVectorMA<ConfigFileDescriptor></c> that
    ///              is to be filled by this method.
    ALIB_DLL
    virtual void    getConfigFilePaths(StdVectorMA<ConfigFileDescriptor>& files);

    /// If the given vector \p{files} is empty, this method tries resources named
    /// <c>"CFGF_NAME_1"</c>, <c>"CFGF_NAME_2"</c>, <c>"CFGF_NAME_3"</c> and so forth until
    /// a first name is not found. For all names found, an entry in vector \p{files} is
    /// created.
    ///
    /// Then, for each entry in the vector (regardless if they were preexisting or created with
    /// this method), comment information is loaded from resources. The resource name is composed
    /// from <c>"CFGF_CMT_"</c> and the number of the entry. If found, the resource is assigned
    /// to the field #"App::ConfigFileDescriptor::Comment;*".
    ///
    /// Then for each entry resources named <c>"CFGF_EXP_"</c> and the amount of the entry
    /// are searched. If found, the field #"App::ConfigFileDescriptor::Exports;*" is set.
    /// If more resources (with higher numbers) are existing, such additional exports are
    /// appended to the list of the last config file entry.
    /// @param files The vector of file paths that is to be filled or modified by this method.
    ALIB_DLL
    virtual void      getConfigFilePathsFromResources  (StdVectorMA<ConfigFileDescriptor>& files);

    /// Checks all filenames in the given vector and converts them to an absolute path.
    /// @param files The vector of file paths that is to be modified by this method.
    ALIB_DLL
    virtual void      getConfigFilePathsMakeAbsolutePaths(StdVectorMA<ConfigFileDescriptor>& files);

    /// Adds configuration file paths influenced by a dedicated CLI parameter.
    /// The default implementation interprets option <em>"--config"</em> and updates or appends
    /// corresponding #"ConfigFileDescriptor" entries in \p files.
    /// Descendants may override this to parse different CLI options or search paths.
    /// @param files Output vector to append file descriptors to.
    virtual void getConfigFilePathsFromCLIParam(StdVectorMA<ConfigFileDescriptor>& files);

    /// Used when errors are detected. May be overwritten, for example, to suppress help output
    /// and/or otherwise change the standard behavior.
    /// This default implementation
    /// - calls #"onSdOutput"
    /// - calls #"CLIUtil::GetHelp(CommandLine&, const String&)"
    /// - calls #"SetExitCode"
    /// - throws #"ControlledEarlyExit".
    /// @param exitCode     The exit code to set in field #"App::machine;*". Also used to
    ///                     write an error message by retrieving
    ///                     #"ExitCodeDecl::FormatString;*".
    /// @param helpTopic    An optional parameter that defines a topic for the help text written
    ///                     to #"App::cOut;*".
    /// @param formatParam1 An optional parameter to be used with the format string.
    /// @param formatParam2 A second optional parameter to be used with the format string.
    ALIB_DLL
    virtual void exitWithHelpOutput( Enum exitCode, const String& helpTopic= NULL_STRING,
                                     Box formatParam1 = EMPTY_STRING,
                                     Box formatParam2 = EMPTY_STRING );

    /// Processes a single parsed CLI command.
    /// The default implementation handles the built-in #"App::Commands".
    /// @param cmd The command to execute.
    /// \return \c true if the command was handled successfully; \c false to indicate that
    ///         the command was not recognized or could not be executed. Implementations
    ///         may set #"cliStop" to terminate further processing.
    ALIB_DLL
    virtual bool processCLICmd(Command* cmd );


  //================================================================================================
  // Virtual Methods called by the State-Machine Program  -  Bootstrap
  //================================================================================================
    /// Called on transition to state #"App::States::SetCamps".<br>
    /// This default implementation just calls the #"alib::BootstrapAddDefaultCamps;2".
    ALIB_DLL
    virtual void        onBsSetCamps();

    /// Called on transition to state #"App::States::PrepareResources".<br>
    /// This default implementation
    /// - calls the #"alib::Bootstrap(BootstrapPhases);2" passing
    ///   #"BootstrapPhases::PrepareResources;2" as its parameter.
    /// - creates instances for fields #".cOut" and #".cErr" inside the global allocator.
    /// - if configuration macro #"ALIB_DEBUG_RESOURCES" is set, then the field
    ///   #"LocalResourcePool::DbgResourceLoadObserver;*" is set to <c>std::cout</c>.
    ALIB_DLL
    virtual void        onBsPrepareResources();

    /// Called on transition to state #"App::States::SetNameVersionAndInfo".<br>
    /// This default implementation tries resources <c>"AppName"</c>, <c>"AppVersion"</c>, and
    /// <c>"AppInfo"</c> to set fields #appName, #appVersion, and #appInfo.
    /// The latter is supported with three placeholders: the name, the version, and the current
    /// year and is created using the method #"Paragraphs::AddMarked(const BoxedObjects& ...);*".
    ///
    /// A string that is set before this method is called, is not replaced.
    ALIB_DLL
    virtual void        onBsSetNameVersionAndInfo();


    /// Called on transition to state #"App::States::PrepareConfig".<br>
    /// This default implementation just calls the function #"alib::Bootstrap(BootstrapPhases);2"
    /// passing #"BootstrapPhases::PrepareConfig;2" as the parameter.
    ALIB_DLL
    virtual void        onBsPrepareConfig();

    /// Called on transition to state #"App::States::PreloadVariables".<br>
    /// Preloading variables allows later fetching and exporting those variables into a
    /// configuration file (if freshly created), even if they have not been read/used in the
    /// first run of an application.<br>
    /// This default implementation calls #"Configuration::PreloadVariables;*"
    /// on the variables found with the enumeration #"lox::Variables".
    ALIB_DLL
    virtual void        onBsPreloadVariables();

    /// Define CLI commands, options, parameters, and exit-codes.
    /// This default implementation initializes field #cli with #"APP" and registers
    /// the built-in declarations defined by the enum types #"App::Commands", #"App::Options",
    /// #"App::Parameters" and #"App::ExitCodes".
    ALIB_DLL
    virtual void        onBsCLIDefine();

    /// Implements #"States::CLIReadOptions": calls #"CommandLine::ReadOptions;*" on
    /// member #".cli". This is done quite early in the boot sequence to allow option '--config'
    /// to change the configuration files used.
    ALIB_DLL
    virtual void        onBsCLIReadOptions();

    /// Called on transition to state #"App::States::ImportConfig".<br>
    /// This default implementation creates the vector of file descriptors using
    /// #getConfigFilePaths and then uses type #"IniFileFeeder" to import the
    /// variables from the INI-files.
    ALIB_DLL
    virtual void        onBsImportConfig();

    /// Finalizes CLI after configuration import.
    /// The default implementation forwards ignored option arguments to the
    /// #"CLIVariablesPlugin" and installs the default shortcut categories \c ALIB and
    /// \c ALOX.
    ALIB_DLL
    virtual void        onBsConfigureCLI();


    /// Called on transition to state #"App::States::FinalizeBootstrap".<br>
    /// This default implementation just calls the function #"alib::Bootstrap(BootstrapPhases);2"
    /// passing #"BootstrapPhases::Final;2" as the parameter.
    ALIB_DLL
    virtual void        onBsFinalizeBootstrap();

    /// Implements #"States::ReadDryRunOption". Reads the option <em>"--dryrun[=yes|no]"</em> from
    /// the CLI and sets the flag #".dryrun" accordingly. The optional argument is parsed
    /// as a #"lang::Bool;2" and hence may be specified with various words, defined with the
    /// according resources.
    ALIB_DLL
    virtual void        onBsReadDryRunOption();

    /// Called on transition to state #"App::States::SetupALox".<br>
    /// If this method returns an error, bootstrapping is aborted.
    /// This default implementation interprets the flags
    /// #"App::Flags::CreateReleaseLox" and
    /// #"App::Flags::UseReleaseLoggerForDebugLogging", and furthermore
    /// uses the fields #releaseLoxName and #releaseLoggerName
    /// into account and potentially creates #releaseLox and #releaseLogger.
    ALIB_DLL
    virtual void        onBsSetupALox();

  //================================================================================================
  // Virtual Methods called by the State-Machine Program  -  Run
  //================================================================================================
    /// This method is preceding the main execution method #onRun. When this is called, all
    /// bootstrapping is done. Final preparations might be performed here to unclutter #onRun.<br>
    ///
    /// This default implementation
    /// - Tries resource <c>"AppStart"</c>. If found its content is printed to the console
    ///   using method #"Paragraphs::AddMarked(const BoxedObjects)" together with the results of the
    ///   methods #".GetName" and #".GetVersion". Hence the resourced application info has to have
    ///   two placeholders for these values.
    /// - Tests flag #".dryrun". If set, tries resource <c>DryRunStart</c> and prints it to
    ///   the console. Again the result of the method #".GetName" is passed as a placeholder
    ///   parameter.
    /// - Tests if the built-in #"app::Option" <c>"Help"</c> was given. If so, calls
    ///   #"CLIUtil::GetHelp(Option)" and stops processing.
    /// - Tests if the built-in #"app::Option" <c>"Version"</c> was given. If so, prints the
    ///   result of #".GetVersion" and stops processing.
    ALIB_DLL
    virtual void onRunStart();

    /// This is the main execution method of the application.
    /// This default implementation processes CLI commands until #"cliStop" is set or all
    /// commands are consumed and dispatches each recognized command to #"processCLICmd".
    ALIB_DLL
    virtual void onRun();

    /// This method is following the main execution method #onRun.
    /// Overwriting this method is useful to collect code which has to be executed at the end
    /// of all (or most) execution paths of the custom implementation of #onRun.
    ALIB_DLL
    virtual void onRunEnd();

  //================================================================================================
  // Virtual Methods called by the State-Machine Program  -  Shutdown
  //================================================================================================
    /// Called on transition to state #"App::States::AnnounceShutdown".<br>
    /// This default implementation just calls the function #"alib::Shutdown(ShutdownPhases);2"
    /// passing #"ShutdownPhases::Announce;2" as the parameter.
    ALIB_DLL
    virtual void        onSdAnnounceShutdown();

    /// Called on transition to state #"App::States::CleanALox".<br>
    /// This default implementation checks the flags
    /// #"App::Flags::ALoxVerbosityExportAllAndWriteBackRelLogger", and
    /// #"App::Flags::ALoxVerbosityExportAllAndWriteBackDbgLogger" and calls
    /// #"Lox::SetVerbosityExport(detail::Logger*)" if set.<br>
    /// Then, if set, the release logger and #"%Lox" are deleted.
    ALIB_DLL
    virtual void        onSdCleanALox();

    /// Called on transition to state #"App::States::ExportConfig".<br>
    /// This default implementation creates the vector of file descriptors using
    /// #getConfigFilePaths and then uses type #"IniFileFeeder" to export
    /// variables that are either not existing yet, or which have a writeback flag set,
    /// into the INI-files.<br>
    /// This is done for all variables (and variable trees) determined by the field
    /// #"App::ConfigFileDescriptor::Exports;*".
    ALIB_DLL
    virtual void        onSdExportConfig();

    /// Flushes what is collected in the fields #cOut and #cErr to their respective streams and
    /// clears the buffers.
    /// With that, this method - while named with the prefix <em>onSd</em> and scheduled at
    /// the end of the #"StateMachine::Program;2" - may be directly called by
    /// descendants whenever they wish to flush the current collected output stream, for example,
    /// after important milestones, before long-running operations, or prior to program termination
    /// when early partial output is desired.
    ALIB_DLL
    virtual void        onSdOutput();

    /// Called on transition to state #"App::States::FinalizeShutdown".<br>
    /// This default implementation calls the function #"alib::Shutdown;2" passing
    /// #"ShutdownPhases::Destruct;2" as the parameter.<br>
    /// Prior to that, some memory cleaning and debug exercises are done.
    ALIB_DLL
    virtual void        onSdFinalizeShutdown();


}; // class App

/// The application singleton-instance. This defaults to \c nullptr and is set by the
/// constructor of class #"App".
ALIB_DLL extern App*    APP_SINGLETON;

/// Convenience function that returns the singleton instance of an \alib application.
/// @tparam TApp The app type to cast to.
/// @return The dereferenced pointer to #"APP_SINGLETON".
template<typename TApp= App>
inline TApp&    Get()                           { return *dynamic_cast<TApp*>(app::APP_SINGLETON); }

} // namespace alib[::app]



} // namespace [alib]

ALIB_ENUMS_MAKE_BITWISE( alib::app::App::Flags )

// assigning ALib enum records
// Specifying our custom module to hold resources of our enum records
ALIB_CAMP_ENUM( alib::app::App::Commands     , alib::app::ERCommandDecl   , alib::APP , "Commands"   )
ALIB_CAMP_ENUM( alib::app::App::Options      , alib::app::EROptionDecl    , alib::APP , "Options"    )
ALIB_CAMP_ENUM( alib::app::App::Parameters   , alib::app::ERParameterDecl , alib::APP , "Parameters" )
ALIB_CAMP_ENUM( alib::app::App::ExitCodes    , alib::app::ERExitCodeDecl  , alib::APP , "ExitCodes"  )

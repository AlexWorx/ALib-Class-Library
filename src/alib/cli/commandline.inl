//==================================================================================================
/// \file
/// This header-file is part of module \alib_cli of the \aliblong.
///
/// \emoji :copyright: 2013-2025 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
ALIB_EXPORT namespace alib {  namespace cli {

class CLIUtil;

//==================================================================================================
/// This class provides a foundation for software executables that processes command line
/// parameters.
///
/// \see
///   "Utility" methods which could have been implemented as an interface of this
///   class have instead been located as static methods in friend class \alib{cli;CLIUtil} which
///   receive a pointer to an instance of this type.
///
/// ## Friends ##
/// class \alib{cli;CLIUtil}
///
///\I{##############################################################################################}
/// @throws #alib::cli::Exceptions \I{CLANGDUMMY}
//==================================================================================================
class CommandLine
{
    // #############################################################################################
    // Type definitions
    // #############################################################################################
    #if !DOXYGEN
        // This friend provides utility methods for using this class.
        friend class  CommandDecl;
        friend struct Command;
        friend struct Option;
        friend struct Parameter;

        friend class  CLIUtil;
    #endif

    // #############################################################################################
    // Type definitions
    // #############################################################################################
    public:

    // #############################################################################################
    // protected fields
    // #############################################################################################
    protected:
        /// Monotonic allocator used for all resourced static definitions as well as the data
        /// used during parsing.
        MonoAllocator       allocator;

        /// The element recycler shared between lists of strings.
        List<MonoAllocator, String    , Recycling::Shared>::SharedRecyclerType   stringListRecycler;

        /// The element recycler shared between fields \alib{cli;Command::ParametersMandatory} and
        /// \alib{cli;Command::ParametersOptional}.
        List<MonoAllocator, Parameter*, Recycling::Shared>::SharedRecyclerType    paramListRecycler;

    // #############################################################################################
    // Fields
    // #############################################################################################
    public:
        /// Application information text.
        /// Used as a sort of "header" output by class \alib{cli;CLIUtil} .
        String                                  AppInfo;

        // ################################## Arguments ############################################
        /// A vector of args. If the type of CLI argument strings provided with the constructor does
        /// not match the \ref ALIB_CHARACTERS_WIDE "default ALib string width", the strings get
        /// converted.<br>
        /// Values that are 'consumed' by options that get defined, are \b not removed from this
        /// list. Instead, they are removed from index vector #ArgsLeft.
        StdVectorMono<String>                           ArgStrings;

        /// A vector of args. If constructor variant accepting \b wchar strings is used,
        /// those unicode strings get converted to 1-byte strings using the current locale.<br>
        /// Values that are 'consumed' by options that get defined, are removed.
        StdVectorMono<integer>                          ArgsLeft;

        // ############################ Declarations (from custom enums) ###########################
        /// Commands defined.
        List<MonoAllocator, CommandDecl*>               CommandDecls;

        /// Possible Options.
        List<MonoAllocator, OptionDecl*>                OptionDecls;

        /// Possible Parameters.
        List<MonoAllocator, ParameterDecl*>             ParameterDecls;

        /// Possible Errors.
        HashMap<MonoAllocator, Enum, ExitCodeDecl*>     ExitCodeDecls;

    // ################################ Parsed CLI objects  ########################################

        /// The options parsed in the order of their appearance.
        List<MonoAllocator, Option*>                    Options;

        /// List of arguments that start with a hyphen and are ignored by this class due to the
        /// fact that they were not recognized.
        ///
        /// \see Method #ReadOptions for details on this.
        List<MonoAllocator, String, Recycling::Shared>  OptionArgsIgnored;

        /// A list of commands actually parsed. Filled with method #ReadNextCommands.
        List<MonoAllocator, Command*>                   CommandsParsed;

        /// The next command in #CommandsParsed to be processed. Used with method #NextCommand.
        List<MonoAllocator, Command*>::iterator         NextCommandIt;

        /// The maximum length of token names:
        ///  - 0: Commands
        ///  - 1: Options
        ///
        ///  Note: Used for formatted help/dump output. (0= command, 1= option, 2= param)
        integer                                 MaxNameLength[3]                      = { 0, 0, 0 };

        /// The resource pool used to fetch resources from.
        /// Several resources are loaded from this in addition
        /// to what is loaded as enum meta-information of the cli declaration objects.
        ///
        /// It is recommended to have the main application implement a custom module, as
        /// \ref alib_mod_bs_custommods "described here".
        resources::ResourcePool*          Resources                                 = nullptr;

        /// The resource category to fetch CLI resources within field #Resources.
        NCString                                ResourceCategory;

        /// Specifies if a "dry run" should be performed.
        /// For more information, see \alib{cli;CLIUtil::GetDryOpt}.
        DryRunModes                             DryRun                            =DryRunModes::Off;

     // #############################################################################################
     // Constructor/destructor
     // #############################################################################################
     public:
        /// Constructor.
        CommandLine()
        : allocator        ( ALIB_DBG("CommandLine",) 2 )
        , stringListRecycler(allocator)
        , paramListRecycler(allocator)
        , AppInfo          (A_CHAR("<AppInfo not set>") )
        , ArgStrings       (allocator)
        , ArgsLeft         (allocator)
        , CommandDecls     (allocator)
        , OptionDecls      (allocator)
        , ParameterDecls   (allocator)
        , ExitCodeDecls    (allocator, 3.0, 10.0)
        , Options          (allocator)
        , OptionArgsIgnored(stringListRecycler)
        , CommandsParsed   (allocator)
        , NextCommandIt    (CommandsParsed.end())
        {}

        /// Virtual empty destructor.
        virtual ~CommandLine()
        {}

    // #############################################################################################
    // Definition interface
    // #############################################################################################
    public:
        //==========================================================================================
        /// Returns the allocator used for all command parsing, resourced enum record creation
        /// and so on. This allocator might be used for allocations that align with (or are shorter
        /// as) the lifecylce of the instance of this class.
        ///
        /// @return  The internal allocator
        //==========================================================================================
        MonoAllocator&    GetAllocator()                                           noexcept
        { return allocator; }

        //==========================================================================================
        /// Helper function that uses fields #Resources and #ResourceCategory to fetch a
        /// resourced string.<br>
        ///
        /// With debug-builds, this method asserts that a resource was found. If this is not
        /// wanted, use #TryResource.
        ///
        /// @param name  The resource name.
        /// @return The resource string, respectively a \e nulled string on failure.
        //==========================================================================================
        const String&   GetResource( const NString& name )
        { return Resources->Get( ResourceCategory, name   ALIB_DBG(, true)  ); }

        //==========================================================================================
        /// Helper function that uses fields #Resources and #ResourceCategory to fetch a
        /// resourced string.<br>
        ///
        /// \note
        ///   Usually, it is recommended to use #GetResource, which asserts with debug-builds
        ///   if a resource was not found.
        ///
        /// @param name  The resource name.
        /// @return The resource string, respectively a \e nulled string on failure.
        //==========================================================================================
        const String&   TryResource( const NString& name )
        { return Resources->Get( ResourceCategory, name  ALIB_DBG(, false)  );  }


        //==========================================================================================
        /// Simple helper method that invokes #Init(resources::ResourcePool*, NCString )
        /// providing the resource pool and categery of the given \p{resModule}.
        ///
        /// @param resModule The module used to load resource strings.
        //==========================================================================================
        void      Init( camp::Camp* resModule )
        { Init( resModule->GetResourcePool().Get(), resModule->ResourceCategory );  }

        //==========================================================================================
        /// Initializes this class. This function has to be invoked after construction and
        /// after \alib \ref alib_mod_bs "was bootstrapped".
        ///
        /// This method accesses global \alib variables \alib{ARG_C}, \alib{ARG_VN}, and
        /// \alib{ARG_VW}, and thus those have to be set by the user's <c>main()</c>-function
        /// properly.
        ///
        /// A resource pool has to be provided along with a corresponding resource category
        /// to use. While it is not necessary to do, it is recommended to create a custom
        /// \alib module, which holds such resource pool. For this case, overloaded
        /// helper method #Init(camp::Camp*) is provided which calls this method by forwarding
        /// the pool and category name from that module.
        ///
        /// @param resourcePool The resource pool used to load resource strings.
        /// @param resCategory  The resource category used to load resource strings.
        //==========================================================================================
        ALIB_DLL
        virtual
        void      Init( resources::ResourcePool* resourcePool, NCString resCategory );

        //==========================================================================================
        /// Defines parameters given with enumeration \p{TEnum}.
        /// \ref alib_enums_records "ALib Enum Records" of type \alib{cli;ERParameterDecl} need to
        /// be associated to the given enumeration type \p{TEnum}.
        ///
        /// @tparam TEnum  The enum type.
        //==========================================================================================
        template<typename TEnum>
        requires ( EnumRecords<TEnum>::template AreOfType<ERParameterDecl>() )
        void DefineParameters()
        {
            for( auto recordIt=  EnumRecords<TEnum>::begin()
                 ;    recordIt!= EnumRecords<TEnum>::end  ()
                 ; ++ recordIt                                )
            {
                ParameterDecls.emplace_back( allocator().New<ParameterDecl>(recordIt.Enum() ) );

                if ( MaxNameLength[2] < recordIt->EnumElementName.Length() )
                     MaxNameLength[2]=  recordIt->EnumElementName.Length();
            }
        }

        //==========================================================================================
        /// Defines commands given with enumeration \p{TEnum}.
        /// \ref alib_enums_records "ALib Enum Records" of type \alib{cli;ERCommandDecl} need to
        /// be associated to the given enumeration type \p{TEnum}.
        ///
        /// @tparam TEnum  The enum type.
        //==========================================================================================
        template<typename TEnum>
        requires( EnumRecords<TEnum>::template AreOfType<ERCommandDecl>() )
        void DefineCommands()
        {
            for( auto recordIt=  EnumRecords<TEnum>::begin()
                 ;    recordIt!= EnumRecords<TEnum>::end  ()
                 ; ++ recordIt                                )
            {
                CommandDecls.emplace_back(  allocator().New<CommandDecl>(recordIt.Enum(), *this ) );

                auto& name= CommandDecls.back()->Identifier();
                if ( MaxNameLength[0] < name.Length() )
                     MaxNameLength[0]=  name.Length();
            }
        }


        //==========================================================================================
        /// Defines options given with enumeration \p{TEnum}.
        /// \ref alib_enums_records "ALib Enum Records" of type \alib{cli;EROptionDecl} need to
        /// be associated to the given enumeration type \p{TEnum}.
        ///
        /// @tparam TEnum  The enum type.
        //==========================================================================================
        template<typename TEnum>
        requires ( EnumRecords<TEnum>::template AreOfType<EROptionDecl>() )
        void DefineOptions()
        {
            for( auto recordIt=  EnumRecords<TEnum>::begin()
                 ;    recordIt!= EnumRecords<TEnum>::end  ()
                 ; ++ recordIt                                )
            {
                OptionDecls.emplace_back( allocator().New<OptionDecl>(recordIt.Enum() ) );

                if ( MaxNameLength[1] < recordIt->EnumElementName.Length() )
                     MaxNameLength[1]=  recordIt->EnumElementName.Length();
            }
        }

        //==========================================================================================
        /// Defines errors given with enumeration \p{TEnum}.
        /// \ref alib_enums_records "ALib Enum Records" of type \alib{cli;ERExitCodeDecl} need to
        /// be associated to the given enumeration type \p{TEnum}.
        ///
        /// @tparam TEnum  The enum type.
        //==========================================================================================
        template<typename TEnum>
        requires ( EnumRecords<TEnum>::template AreOfType<ERExitCodeDecl>() )
        void DefineExitCodes()
        {
            for( auto recordIt=  EnumRecords<TEnum>::begin()
                 ;    recordIt!= EnumRecords<TEnum>::end  ()
                 ; ++ recordIt                                )
                ExitCodeDecls.EmplaceUnique( recordIt.Enum(),
                                             allocator().New<ExitCodeDecl>(recordIt.Enum()) );
        }

    // #############################################################################################
    // Parsing interface
    // #############################################################################################
    public:
        //==========================================================================================
        /// Finalizes Initialization and has to be called after all invocations of:
        /// - DefineCommands,
        /// - DefineOptions,
        /// - DefineParameters and
        /// - DefineExitCodes,
        ///
        /// have been performed. All options recognized get collected in list #Options
        /// The arguments of the options get removed from #ArgsLeft.
        ///
        /// In case of options that have own parameter arguments, such arguments may not be fully
        /// removed. This depends on whether it is possible with the simple flags and values
        /// provided in \alib{cli;OptionDecl} to enable class \alib{cli;Option} to fully detect
        /// them. Therefore, after this method is invoked, for options with more complex syntax,
        /// custom code may be needed to pull the "remainders" of option arguments from #ArgsLeft.
        /// For this, field \alib{cli;Option::Position} is quite useful, as well as method
        /// #RemoveArg.
        ///
        /// It has to be ensured that before the next step, which is the invocation of
        /// #ReadNextCommands, all option-related CLI arguments are cleaned away!
        ///
        /// For this reason, this method removes all arguments that start with a
        /// hyphen \c '-' from the #ArgsLeft, even if they got \e not recognized. Those CLI arguments
        /// get collected in #OptionArgsIgnored.
        /// Finding unrecognized options is not considered an error, because other libraries used
        /// with the software might read CLI options autonomously.
        ///
        /// \note
        ///  A good sample for this is class \alib{variables;CLIVariablesPlugin} used with \alib
        ///  configuration variables. After this method has been invoked, vector #OptionArgsIgnored
        ///  may/should be passed to the plug-in \b %CLIVariablesPlugin of (all)
        ///  \alib{variables;Configuration} object(s) used with the application. For this, an
        ///  invocation, similar to this may be used with all \alibmods that use an own configuration
        ///  object:
        ///
        ///          XYZModule.GetConfig()->GetPluginTypeSafe<alib::variables::CLIVariablesPlugin>()->SetArgs( &OptionArgsIgnored );
        ///
        /// In the case that other libraries have more complex option syntax, e.g., options
        /// consisting of multiple arguments or such that do not even start with a hyphen character,
        /// then, this method should be called <b>only after a custom code removes such 3rd party
        /// options in a reliable way!</b>
        ///
        /// If all this was not done, the "remainder" of custom options might confuse parsing
        /// of commands and its parameters and most probably would lead to an
        /// "unknown" command error when the remainders of non-removed 3rd party option arguments
        /// are consumed later.
        ///
        /// As a consequence of this approach, a subsequent call to this method has no effect.
        //==========================================================================================
        ALIB_DLL
        virtual
        void            ReadOptions();

        //==========================================================================================
        /// Searches and returns the last occurrence of the specified option.
        ///
        /// This method is to be used with options that overwrite previous values in case
        /// that it was given multiple times as a CLI argument. Instead, options that may occur
        /// multiple times without overriding a previous occurrence, are to be processed
        /// "manually" by examining field #Options.
        ///
        /// @param element The option's declaration enum element.
        /// @return A pointer to the parsed option, respectively \c nullptr if not given.
        //==========================================================================================
        ALIB_DLL
        Option*         GetOption( Enum element );

        //==========================================================================================
        /// Parses as many commands as possible and stores them in #CommandsParsed. Prior
        /// to invoking this methods, all CLI declarations have to be made. Furthermore, usually
        /// method #ReadOptions has to be invoked before this method.
        ///
        /// The name of the method indicates that one or more, but maybe not all commands are read.
        /// The reason for this behavior lies in the fact that commands may be defined that
        /// do their own, specifically coded parsing. As a matter of the fact that the flags and
        /// options of structs \alib{cli;CommandDecl} and \alib{cli;ParameterDecl} are kept rather
        /// simple to match the most usual cases, the parsing of arguments of a command often
        /// has to be left to be done by custom code. Mostly just when processing (executing) a
        /// command. In contrast to the need of parsing (and processing) all CLI options,
        /// given before processing commands, this is not a problem. The usual inner part of a
        /// command processing loop then looks like this:
        /// - Check if #CommandsParsed is empty
        /// - Invoke this method (\b %ReadNextCommands )
        /// - If still no command is found, break loop
        /// - Remove and process first command in #CommandsParsed.
        ///
        /// A similar parsing approach is supported with method #NextCommand. The only difference
        /// is that the parsed commands stay in #CommandsParsed and instead field #NextCommandIt
        /// indicates the position of the next command to read. This way, commands may refer
        /// to previous ones, if this is needed.
        ///
        /// As a final note it should be mentioned, that implementing a "dry run" option on the
        /// level of command parsing, for the reasons explained above, might need some specific
        /// custom code to be able to parse all commands (without processing them). If such
        /// functionality is wanted, it is best to separate custom command parsing from
        /// command execution (the opposite was recommended above). Only the last command in the list
        /// has to be 'manually' processed, as the previous ones obviously got parsed well.
        /// With this approach, all commands can be parsed without executing them. Static utility
        /// method \alib{cli;CLIUtil::DumpParseResults} is a predefined way to then write information
        /// about all options and commands parsed.<br>
        /// A lower level "dry run", that receives information about the concrete actions that the
        /// processing of commands would perform, is of course a different, application specific
        /// task.
        //==========================================================================================
        ALIB_DLL
        virtual
        void            ReadNextCommands();

        //==========================================================================================
        /// Returns the next item in vector #NextCommand. If needed, #ReadNextCommands is
        /// invoked.
        /// @return The next command parsed from CLI argument list. \c nullptr, if no more commands
        ///         are found.
        //==========================================================================================
        ALIB_DLL
        virtual
        Command*        NextCommand();

        //==========================================================================================
        /// Retrieves the number of arguments.
        ///
        /// @return The number of arguments given.
        //==========================================================================================
        virtual
        integer         ArgCount()
        {
            return integer(ArgStrings.size());
        }

        //==========================================================================================
        /// Retrieves the argument at the given position.<br>
        /// In debug-builds, this method assert the index is in the available range.
        /// @param idx The requested argument's index.
        /// @return The element \p{idx} of vector #ArgStrings.
        //==========================================================================================
        virtual
        String          GetArg(integer idx)
        {
            ALIB_ASSERT_ERROR( idx >= 0 && size_t(idx) < ArgStrings.size(),
                               "CLI", "Argument index out of bounds" )
            return ArgStrings[size_t(idx)];
        }

        //==========================================================================================
        /// Retrieves the next argument from the list without removing it.
        ///
        /// \see Method #PopArg, #RemoveArg and #ReadNextCommands.
        ///
        /// @return The first argument of (respectively remaining in) the list.
        ///         If no argument is available, a \e nulled string is returned.
        //==========================================================================================
        virtual
        String          PeekArg()
        {
            return ArgsLeft.size() > 0 ? ArgStrings[size_t(ArgsLeft[0])]
                                       : String();
        }

        //==========================================================================================
        /// Retrieves the next argument and removes it from list #ArgsLeft.
        ///
        /// \see Method #PeekArg, #RemoveArg and #ReadNextCommands.
        ///
        /// @return The first argument of vector #ArgsLeft.
        ///         If no argument is available, a \e nulled string is returned.
        //==========================================================================================
        ALIB_DLL
        virtual
        String          PopArg();

        //==========================================================================================
        /// Removes the argument at position \p{argNo}.
        /// If the argument is not in #ArgsLeft, an \alib assertion is raised.
        ///
        /// \see Method #PeekArg, #PopArg and #ReadNextCommands.
        ///
        /// @param argNo The argument number to remove.
        //==========================================================================================
        ALIB_DLL
        void            RemoveArg( integer argNo );
}; // class CommandLine



// #############################################################################################
// Definitions of methods of related objects that can be only mede after CommandLine is defined
// #############################################################################################
inline
Parameter::Parameter( CommandLine* cmdLine )
: Parsed( cmdLine )
, Args  ( cmdLine->stringListRecycler )
{}

Option::Option( CommandLine* cmdLine )
: Parsed( cmdLine )
, Args  ( cmdLine->stringListRecycler )
{}

template<typename TEnum>
CommandDecl::CommandDecl( TEnum element, CommandLine& cmdLine )
: declElement (element)
, resourceInfo(element)
, CmdLine     (cmdLine )
, Parameters  (cmdLine.allocator)
{
    // make a copy of the resourced record
    record= enumrecords::GetRecord(element);

    addParamDecls();
}

Command::Command( CommandLine* cmdLine )
: Parsed        ( cmdLine )
, ParametersMandatory( cmdLine->paramListRecycler )
, ParametersOptional ( cmdLine->paramListRecycler )
{}


} // namespace alib[::basecamp]

/// Type alias in namespace \b alib.
using     CommandLine=           cli::CommandLine;


}  // namespace [alib]


// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CLI_CLIAPP
#define HPP_ALIB_CLI_CLIAPP 1

#if !defined (HPP_ALIB_RESOURCES_RESOURCESTRING)
    #include "alib/resources/resourcestring.hpp"
#endif

#if !defined (HPP_ALIB_CLI_ARGUMENTS)
    #include "alib/cli/arguments.hpp"
#endif

#if !defined(HPP_ALIB_COMPATIBILITY_STD_BOXING_FUNCTIONAL)
    #include "alib/compatibility/std_boxing_functional.hpp"
#endif

namespace aworx { namespace lib { namespace cli {

class CLIUtil;


/** ************************************************************************************************
 * This class provides a foundation for software executables that processes command line
 * parameters.
 *
 * For an introduction to its use see documentation of \ref aworx::lib::cli "ALib Module CLI".
 *
 * \see
 *   "Utility" methods which could have been implemented as an interface of this
 *   class have instead been located in \alib{cli,CLIUtil}.
 *
 * ## Friends ##
 * class \alib{cli,CLIUtil}
 *
\~Comment ####################################################################################### \~
 * @throws aworx::lib::cli::Exceptions
 **************************************************************************************************/
class CLIApp
{
    #if !ALIB_DOCUMENTATION_PARSER
        // This friend provides utility methods for using this class.
        friend class CLIUtil;
    #endif

    // #############################################################################################
    // Fields
    // #############################################################################################
    public:

        // ################################## Arguments ############################################
        /**
         * The original number of command line arguments (provided in constructor)
         */
        int                                 ArgcOriginal;

        /**
         * The original command line arguments (provided in constructor). Might be nullptr, if
         * \c wchar_t variant of constructor was used.
        */
        const char**                       ArgNOriginal;

        /**
         * The original command line arguments (provided in constructor) Might be nullptr, if
         * \c char variant of constructor was used.
         */
        const wchar_t**                     ArgWOriginal;

        /**
         * A vector of args. If the type of CLI argument strings provided with the constructor does
         * not match the \ref ALIB_CHARACTERS_ARE_NARROW "default ALib string width", the strings get
         * converted.<br>
         * Values that are 'consumed' by options that get defined, are \b not removed from this
         * list. Instead, they are removed from index vector #ArgsLeft.
         */
        std::vector<String>                 ArgStrings;

        /**
         * A vector of args. If constructor variant accepting \b wchar strings is used,
         * those unicode strings get converted to 1-byte strings using the current locale.<br>
         * Values that are 'consumed' by options that get defined, are removed.
         */
        std::vector<size_t>                 ArgsLeft;

        // ############################ Declarations (from custom enums) ###########################
        /** Commands defined. */
        std::vector<CommandDecl>            CommandDecls;

        /** Possible Options. */
        std::vector<OptionDecl>             OptionDecls;

        /** Possible Parameters. */
        std::vector<ParameterDecl>          ParameterDecls;

        /** Possible Errors. */
        std::map<Enum, ExitCodeDecl>        ExitCodeDecls;

    // ################################ Parsed CLI objects  ########################################

        /** A map of lists of options that were found in the parameter list. */
        std::map<Enum, std::vector<Option>> OptionsFound;

        /** A map of options that were found in the parameter list. */
        std::map<Enum, std::vector<Option>> OptionsOverwritten;

        /**
         * List of arguments that start with a hyphen and are ignored by this class due to the
         * fact that they were not recognized.
         *
         * \see Method #ReadOptions for details on this.
         */
        std::vector<String>                 OptionArgsIgnored;

        /** A lists of commands actually parsed. Filled with method #ReadNextCommands. */
        std::vector<Command>                CommandsParsed;

        /** The next command in #CommandsParsed to be processed. Used with method #NextCommand. */
        size_t                              NextCommandNo                                       = 0;

        /**
         * The maximum length of token names:
         *  - 0: Commands
         *  - 1: Options
         *
         *  Note: Used for formatted help/dump output. (0= command, 1= option, 2= param) */
        integer                             MaxNameLength[3]                           = { 0,0, 0 };

        /**
         * The module of the main application. Several resources are loaded from this in addition
         * to what is loaded as enum meta information of the cli declaration objects.
         */
        Module*                             ResModule                                    = nullptr;

        /**
         * Specifies if a "dry run" should be performed.
         * For more information, see \alib{cli,CLIUtil::GetDryOpt}.
         */
        DryRunModes                         DryRun                                =DryRunModes::Off;

    protected:
        /**
         * List of string buffers used in the case that a different character version was passed
         * with the constructor than the \alib{characters,character,default character type}.
         */
        std::vector<AString>                convertedArgStrings;

     // #############################################################################################
     // destructor
     // #############################################################################################
     public:
        /**
         * Virtual empty destructor.
         */
        virtual ~CLIApp()
        {}

    // #############################################################################################
    // Definition interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Initializes this class. This function has to be invoked after construction and
         * after the \alib module system is initialized.
         *
         * @param resModule The module used to load resource strings.
         ******************************************************************************************/
        ALIB_API
        virtual
        void      Init( Module* resModule );

        /** ****************************************************************************************
         * Defines parameters given with enumeration \p{TEnum}.
         * \alib{resources,T_EnumMetaDataDecl,Enum meta data} needs to be available for the
         * given type using macro \ref ALIB_CLI_PARAMETERS.
         *
         * @tparam TEnum  The enum type.
         ******************************************************************************************/
        template<typename TEnum,
                 typename  TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type >
        void  DefineParameters()
        {
            auto& emd= EnumMetaData<TEnum>::GetSingleton();
            emd.CheckLoad();
            for( auto& entry : emd.Table )
            {
                ParameterDecls.emplace_back( TEnum(std::get<0>(entry)) );

                String name= ParameterDecls.back().Name();
                if ( MaxNameLength[2] < name.Length() )
                     MaxNameLength[2]=  name.Length();
            }
        }


        /** ****************************************************************************************
         * Defines commands given with enumeration \p{TEnum}.
         * \alib{resources,T_EnumMetaDataDecl,Enum meta data} needs to be available for the
         * given type using macro \ref ALIB_CLI_COMMANDS.
         *
         * @tparam TEnum  The enum type.
         ******************************************************************************************/
        template<typename TEnum,
                 typename  TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type >
        void  DefineCommands()
        {
            auto& emd= EnumMetaData<TEnum>::GetSingleton();
            emd.CheckLoad();
            for( auto& entry : emd.Table )
            {
                CommandDecls.emplace_back( TEnum(std::get<0>(entry)),  *this  );

                String name= CommandDecls.back().Identifier();
                if ( MaxNameLength[0] < name.Length() )
                     MaxNameLength[0]=  name.Length();
            }
        }


        /** ****************************************************************************************
         * Defines options given with enumeration \p{TEnum}.
         * \alib{resources,T_EnumMetaDataDecl,Enum meta data} needs to be available for the given
         * type using macro \ref ALIB_CLI_OPTIONS.
         *
         * @tparam TEnum  The enum type.
         ******************************************************************************************/
        template<typename TEnum,
                 typename  TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type >
        void  DefineOptions()
        {
            auto& emd= EnumMetaData<TEnum>::GetSingleton();
            emd.CheckLoad();
            for( auto& entry : emd.Table )
            {
                OptionDecls.emplace_back( TEnum(std::get<0>(entry))  );

                String name= OptionDecls.back().Identifier();
                if ( MaxNameLength[1] < name.Length() )
                     MaxNameLength[1]=  name.Length();
            }
        }




        /** ****************************************************************************************
         * Defines errors given with enumeration \p{TEnum}.
         * \alib{resources,T_EnumMetaDataDecl,Enum meta data} needs to be available for the given
         * type using macro \ref ALIB_CLI_EXIT_CODES.
         *
         * @tparam TEnum  The enum type.
         ******************************************************************************************/
        template<typename TEnum,
                 typename  TEnableIf= typename  std::enable_if<std::is_enum<TEnum>::value>::type >
        void  DefineExitCodes()
        {
            auto& emd= EnumMetaData<TEnum>::GetSingleton();
            emd.CheckLoad();
            for( auto& entry : emd.Table )
            {
                TEnum elem= TEnum( std::get<0>( entry ) );
                ExitCodeDecls.emplace( elem, elem );
            }
        }


    // #############################################################################################
    // Parsing interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Finalizes Initialization and has to be called after all invocations of:
         * - DefineCommands,
         * - DefineOptions,
         * - DefineParameters and
         * - DefineExitCodes,
         *
         * have been performed. All options recognized get collected in lists #OptionsFound and
         * #OptionsOverwritten. The arguments of the options get removed from #ArgsLeft.
         *
         * In case of options that have own parameter arguments, such arguments may not be fully
         * removed. This depends on whether it is possible with the simple flags and values
         * provided in \alib{cli,OptionDecl} to enable class \alib{cli,Option} to fully detect
         * them. Therefore, after this method is invoked, for options with more complex syntax,
         * custom code may be needed to pull the "remainders" of option arguments from #ArgsLeft.
         * For this, field \alib{cli,Option::Position} is quite useful, as well as method
         * #RemoveArg.
         *
         * It has to be assured that before the next step, which is the invocation of
         * #ReadNextCommands, all option-related CLI arguments are cleaned away!
         *
         * For this reason, this method removes all arguments that start with a
         * hyphen \c '-' from the #ArgsLeft, even if they got \e not recognized. Those CLI arguments
         * get collected in #OptionArgsIgnored.
         * Finding unrecognized options is not considered an error, because other libraries used
         * with the software might read CLI options autonomously.
         *
         * \note
         *  A good sample for this is class \alib{config,CLIArgs} used with \alib
         *  configuration variables. After this method has been invoked, vector #OptionArgsIgnored
         *  may/should be passed to the plug-in \b %CLIArgs of (all)
         *  \alib{config,Configuration} object(s) used with the application. For this, an
         *  invocation, similar to this may be used with all \alibmods that use an own configuration
         *  object:
         *
         *          XYZModule.Config->GetPluginTypeSafe<aworx::lib::config::CLIArgs>()->SetArgs( &OptionArgsIgnored );
         *
         * In the case that other libraries have more complex option syntax, e.g. options
         * consisting of multiple arguments or such that do not even start with a hyphen character,
         * then, this method should be called <b>only after a custom code removes such 3rd party
         * options in a reliable way!</b>
         *
         * If all this was not done, the "remainder" of custom options might confuse parsing
         * of commands and its parameters and most probably would lead to an
         * "unknown" command error when the remainders of non-removed 3rd party option arguments
         * are consumed later.
         *
         * As a consequence of this approach, a subsequent call to this method has no effect.
         ******************************************************************************************/
        ALIB_API
        virtual
        void            ReadOptions();

        /** ****************************************************************************************
         * Parses as many commands as possible and stores them in #CommandsParsed. Prior
         * to invoking this methods, all CLI declarations have to be made. Furthermore, usually
         * method #ReadOptions has to be invoked prior to this method.
         *
         * The name of the method indicates that one or more, but maybe not all commands are read.
         * The reason for this behaviour lies in the fact that commands may be defined that
         * do their own, specifically coded parsing. As a matter of the fact that the flags and
         * options of structs \alib{cli,CommandDecl} and \alib{cli,ParameterDecl} are kept rather
         * simple to match the most usual cases, the parsing of arguments of a command often
         * has to be left to be done by custom code. Mostly just when processing (executing) a
         * command. In contrast to the need of parsing (and processing) all CLI options,
         * given prior to processing commands, this is not a problem. The usual inner part of a
         * command processing loop then looks like this:
         * - Check if #CommandsParsed is empty
         * - Invoke this method (\b %ReadNextCommands )
         * - If still no command is found, break loop
         * - Remove and process first command in #CommandsParsed.
         *
         * A similar parsing approach is supported with method #NextCommand. The only difference
         * is that the parsed commands stay in #CommandsParsed and instead field #NextCommandNo
         * indicates the position of the next command to read. This way, commands may refer
         * to previous ones, if this is needed.
         *
         * As a final note it should be mentioned, that implementing a "dry run" option on the
         * level of command parsing, for the reasons explained above, might need some specific
         * custom code to be able to parse all commands (without processing them). If such
         * functionality is wanted, it is best to separate custom command parsing from
         * command execution (the opposite was recommended above). Only the last command in the list
         * has to be 'manually' processed, as the previous ones obviously got parsed well.
         * With this approach, all commands can be parsed without executing them. Static utility
         * method \alib{cli,CLIUtil::DumpParseResults} is a predefined way to then write information
         * about all options and commands parsed.<br>
         * A lower level "dry run", that receives information about the concrete actions that the
         * processing of commands would perform, is of-course a different, application specific
         * task.
         ******************************************************************************************/
        ALIB_API
        virtual
        void            ReadNextCommands();

        /** ****************************************************************************************
         * Returns the #NextCommandNo in vector #NextCommand. If needed, #ReadNextCommands is
         * invoked.
         * @return The next command parsed from CLI argument list. \c nullptr, if no more commands
         *         are found.
         ******************************************************************************************/
        ALIB_API
        virtual
        Command*        NextCommand();

        /** ****************************************************************************************
         * Retrieves the next argument from the list without removing it.
         *
         * \see Method #PopArg, #RemoveArg and #ReadNextCommands.
         *
         * @return The first argument of (respectively remaining in) the list.
         *         If no argument is available, a \e nulled string is returned.
         ******************************************************************************************/
        inline
        virtual
        String          PeekArg()
        {
            return ArgsLeft.size() > 0 ? ArgStrings[ArgsLeft[0]]
                                       : String();
        }


        /** ****************************************************************************************
         * Retrieves the next argument and removes it from list #ArgsLeft.
         *
         * \see Method #PeekArg, #RemoveArg and #ReadNextCommands.
         *
         * @return The first argument of vector #ArgsLeft.
         *         If no argument is available, a \e nulled string is returned.
         ******************************************************************************************/
        ALIB_API
        virtual
        String          PopArg();

        /** ****************************************************************************************
         * Removes the argument at position \p{argNo}.
         * If the argument is not in #ArgsLeft, an \alib assertion is raised.
         *
         * \see Method #PeekArg, #PopArg and #ReadNextCommands.
         *
         * @param argNo The argument number to remove.
         ******************************************************************************************/
        ALIB_API
        virtual
        void            RemoveArg( size_t argNo );
};

}} // namespace lib::system

/// Type alias in namespace #aworx.
using     CLIApp=           aworx::lib::cli::CLIApp;


}  // namespace [aworx]



#endif // HPP_ALIB_CLI_CLIAPP

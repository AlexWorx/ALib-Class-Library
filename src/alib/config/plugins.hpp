// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CONFIG_PLUGINS
#define HPP_ALIB_CONFIG_PLUGINS 1

#if !defined (HPP_ALIB_CONFIG_VARIABLE)
    #include "alib/config/variable.hpp"
#endif

#if !defined (HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

namespace aworx { namespace lib { namespace config {


// #################################################################################################
// class XTernalizer
// #################################################################################################

/** ************************************************************************************************
 * This virtual class is used by classes derived from
 * \ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin"
 * to convert external string to internal values and vice versa, and for parsing lists of values
 * from an external string.
 *
 * Class \b %ConfigurationPlugin owns a default object with field
 * \alib{config::ConfigurationPlugin,defaultStringConverter} which by default points to field
 * \alib{config::ConfigurationPlugin,StringConverter}.
 *
 * \note
 *   Replacing the converters is deemed to be an advanced usage of \alib. Consult the source code
 *   for detailed information about how to implement an application specific converter.
 *
 * This default implementation proceeds as follows:
 * - "Externalizing" a value:
 *   - Value is surrounded by quotes if it starts or ends with spaces or if it includes
 *     the delimiter token.
 *   - A few characters are escaped using \c '\\'. Those are
 *     \c \\n, \c \\r, \c \\t , \c \\a, \c \\b, \c \\v, \c \\f, \c \\e and also
 *     the double quotation marks \c \\" and the backslash itself (\c \\\\).
 *
 * - "Internalizing" a value:
 *   - If (non-escaped) quote \c " characters are found, those are removed and whitespaces
 *     within such quotes are kept.
 *   - Escaped characters are converted to their original value.
 *
 * - Loading variables from external strings:
 *   - If provided variable has a valid delimiter set, this character is used to tokenize
 *     the external string.
 *   - Values are trimmed, unless quoted. Quotes characters themselves are removed.
 *   - Delimiters found within a pair of quotes are ignored.
 *   - Each value found is internalized separately
 **************************************************************************************************/
class XTernalizer
{
    public:

    /** ****************************************************************************************
     * Virtual destructor
     *****************************************************************************************/
    virtual ~XTernalizer()  {}

    /** ********************************************************************************************
     * If field \alib{config,Variable::Delim} is <c>'\0'</c>, just invokes #InternalizeValue.
     * Otherwise, parses values using the delimiter. Quotes are removed and parts within quotes
     * are kept as is. Also, delimiters in quotes are ignored.
     *
     * @param  variable The destination variable.
     * @param  src      The source string
     **********************************************************************************************/
    ALIB_API
    virtual
    void    LoadFromString( Variable& variable, const String& src );

    /** ********************************************************************************************
     * Trims \p{src}, removes surrounding quotes and , un-escapes characters.
     *
     * @param  src      The source string
     * @param  dest     The destination string
     **********************************************************************************************/
    ALIB_API
    virtual
    void InternalizeValue( Substring& src, AString& dest );

    /** ********************************************************************************************
     * Converts the given \p{src} string to an external representation.
     *
     * @param  src      The source string
     * @param  dest     The destination string
     * @param  delim    If this character is found in the string, the value is quoted
     **********************************************************************************************/
    ALIB_API
    virtual
    void ExternalizeValue( Substring& src, AString& dest, character delim );
};

/** ************************************************************************************************
 * Abstract class that represents a plug in for class
 * \ref aworx::lib::config::Configuration "Configuration"
 * to provide configuration data from specific configuration data source.
 *
 * See documentation of namespace #aworx::lib::config for more information on \alib
 * external configuration variables.
 *
 * The plug-in also may have the ability to write data. The default implementation for writing
 * configuration data returns constant false, indicating that this plug-in never writes
 * anything (e.g. command line parameter plug-in, environment variable plug-in).
 *
 * Category and Variable names are character case insensitive for the plug-ins predefined
 * with \alib.
 * It is recommended to ignore character case in custom specializations of this class
 * as well.
 **************************************************************************************************/
class ConfigurationPlugin
{
    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:
        /** The default external string converter. */
        XTernalizer             defaultStringConverter;


    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /** The external string converter.
         *  By default this points to field #defaultStringConverter.                 */
        XTernalizer*            StringConverter;

    // #############################################################################################
    // protected constructor, public destructor
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Constructor which is protected, as this is an abstract class.
         ******************************************************************************************/
        ConfigurationPlugin()
        {
            StringConverter= &defaultStringConverter;
        }

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
    public:
        virtual ~ConfigurationPlugin() {}

    // #############################################################################################
    // abstract/virtual interface
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Abstract method to return a plug-in name. The name may be used in human readable
         * output, e.g. log-files or exception messages to tell a user for example, which plug-in
         * loaded a variable containing a syntax error.
         * @return The name of the plug-in.
         ******************************************************************************************/
        virtual String  Name()                                           const                   =0;


        /** ****************************************************************************************
         * Abstract method that has to be overwritten by descendants.
         * Searches and by default retrieves the value of a configuration variable.
         * If \p{searchOnly} is \c true, then the variable value is not read.
         *
         * @param variable      The variable to retrieve.
         * @param searchOnly    If \c true, the variable must not be set. Defaults to \c false.
         * @return \c true if variable was found within this configuration source, \c false if not.
         ******************************************************************************************/
        ALIB_API
        virtual bool  Load( Variable& variable, bool searchOnly= false )  const                  =0;

        /** ****************************************************************************************
         * Writes a variable to the configuration.
         * This default implementation just returns \c false. If this method is not overwritten
         * in descendants, those descendants are not designed to write data.
         *
         * @param variable  The variable to store.
         *
         * @return \c true if the variable was written, \c false if not which typically indicates
         *         that this plug-in is not able to write values.
         ******************************************************************************************/
        virtual bool  Store( Variable& variable )
        {
            (void) variable;
            return false;
        }


        /** ****************************************************************************************
         * Convenience method that parses the values from the given string using field
         * #StringConverter and then invokes \ref Store(Variable&) "Store".
         *
         * @param variable          The variable to store.
         * @param externalizedValue The value to parse into the variable before storing
         *
         * @return \c true if the variable was written, \c false if not which typically indicates
         *         that this plug-in is not able to write values.
         ******************************************************************************************/
        virtual bool  Store( Variable& variable, const String& externalizedValue )
        {
            if( externalizedValue.IsNotNull() )
                StringConverter->LoadFromString( variable, externalizedValue );
            return Store( variable );
        }

    // #############################################################################################
    // Iteration.
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Iterator interface class returned by #GetIterator.
         *
         * \note Method \alib{config,Configuration::GetIterator} returns an iterator that allows
         *       to iterate across all variables of a section found in all plug-ins
         ******************************************************************************************/
        class Iterator
        {
            public:

            /**
             * Virtual destructor.
             */
            virtual                ~Iterator()
            {}

            /**
             * Searches and loads the next variable from the iterated section. On success, the
             * variable data is stored in \p{variable}.
             *
             * @param variable  The variable to load from this plug-in on success.
             * @return \c true, if a next variable was found. \c false otherwise.
             */
            virtual bool            Next( Variable& variable )                                  = 0;
        };

        /** ****************************************************************************************
         * Creates an iterator object to return all variables within a section.
         * The iterator object returned, needs to be deleted by the caller.
         *
         * If a plug-in can not perform iteration, it will return \c nullptr. This is for example
         * true for plug-in class \alib{config,Environment}.
         *
         * \note Method \alib{config,Configuration::GetIterator} returns an iterator that allows
         *       to iterate across all variables of a section found in all plug-ins
         *
         * @param sectionName  The name of the section to iterate.
         *
         * @returns The iterator requested or \c nullptr if the plug-in does not support section
         *          iteration.
         ******************************************************************************************/
        virtual Iterator*   GetIterator( const String& sectionName )
        {
            (void) sectionName;
            return nullptr;
        }
};


class CLIArgs;

/**
 * Internal details of namespace #aworx::lib::config.
 */
namespace detail
{
    /**
     * Internal friend function of \alib{config,CLIArgs} used for iteration.
     * @param cliArgs       The command line args plug-in.
     * @param nextArgNo     The argument to continue the search with.
     * @param sectionName   The name of the section to search for.
     * @param variable      The variable to load
     * @return \c true on success, otherwise \c false.
     */
    bool nextCLIArg( CLIArgs& cliArgs, size_t& nextArgNo, const String& sectionName,
                     Variable& variable );
}

/** ************************************************************************************************
 * Specialization of abstract interface class #ConfigurationPlugin, which takes all command line
 * parameters in the constructor and reads variable values from those parameters on request.
 * Its priority value usually is \alib{config,Priorities::CLI}, which is higher
 * than all other default plug-ins provided.
 *
 * Variable categories are used as a prefix together with an underscore \c '_'.
 * This means, if variable <em>LOCALE</em> in category <em>ALIB</em> is accessed, the command line
 * parameter <em>--ALIB_LOCALE=xyz</em> is read.
 *
 * Category and Variable names are insensitive in respect to character case.
 *
 * Command line variables may be passed with either one hyphen ('-') or two ('--').
 * Both are accepted.
 *
 * An application can specify one or more "default categories" by adding their string names to
 * public field #DefaultCategories. Variables of these categories are recognized by the plug-in
 * also when given without the prefix of category name and underscore \c '_'.
 *
 * Furthermore, an application may set public field #AllowedMinimumShortCut to a value greater than
 * \c 0. In this case, the plug-in recognizes variables in CLI arguments already when at least
 * this amount of characters is provided. In other words, when reading an argument as many
 * characters of the variable name as possible are 'consumed' and if a minimum number is given with
 * #AllowedMinimumShortCut, such minimum is sufficient. If the remaining part of the argument
 * string is either empty or continues with an equal sign \c '=', then the variable is recognized
 * (with an empty value or the value after the equal sign).<br>
 * Fields #AllowedMinimumShortCut and #DefaultCategories may also be used in combination.
 *
 * ## Friends ##
 * class \alib{config,detail::nextCLIArg} (Used for implementation of iterator.)
 **************************************************************************************************/
class CLIArgs : public ConfigurationPlugin
{
    #if !ALIB_DOCUMENTATION_PARSER
        // Friend detail function used for implementation of iterator.
        friend bool detail::nextCLIArg( CLIArgs&, size_t&, const String&, Variable&);
    #endif

    protected:
        size_t        argCount  = 0;        ///< The number of command line arguments
        const void**  argVector = nullptr;  ///< The list of command line arguments
        bool          wArgs     = false;    ///< Determines if argv is of type '<em>wchar **</em>'
                                            ///< or '<em>nchar **</em>'

        /**
         * This vector may be set with method #SetArgs(std::vector<String>*). If set, it is used
         * as the source of options instead of using fields #argCount and #argVector.
         */
        std::vector<String>*                args                                          = nullptr;

    public:

        /**
         * An application can specify one or more "default categories" by adding the category names
         * here. Variables of these categories are recognized by the plug-in
         * also when given without the prefix of <c>category_</c>.
         */
        std::vector<String>                 DefaultCategories;

        /**
         * If this field is set to a value greater than \c 0, this plug-in recognizes variables in
         * CLI arguments already when at least this amount of characters is provided.
         * If the remaining part of the argument string is either empty or continues with an equal
         * sign \c '=', then the variable is recognized.
         */
        int                                 AllowedMinimumShortCut                              = 0;

        /** ****************************************************************************************
         * Constructor. After creation, method #SetArgs should be called to equip this instance
         * with the command line arguments.
         ******************************************************************************************/
                 CLIArgs() : ConfigurationPlugin() {}

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
        virtual ~CLIArgs() {}

        /** ****************************************************************************************
         * Sets the command line argument list. Needs to be called once after construction.
         * Should not be invoked directly. Rather use method
         * \alib{config,Configuration::SetCommandLineArgs}.
         *
         *\note
         *   In standard application scenarios, this method is invoked with corresponding
         *   \aworx{lib,Module::Init,module initialization} during
         *   \ref alib_manual_bootstrapping "library bootstrap".
         *
         * <p>
         *\note On the Windows platform, the Microsoft compiler provides the global variables
         *      <em>__argc</em> and <em>__argv</em> (respectively <em>__wargv</em> for wide
         *      character binaries. These variables a can be used if this method is invoked
         *      outside of the <em>main()</em> method.
         *
         * @param argc          The number of arguments in argv
         * @param argv          The list of command line arguments.
         * @param areWideChar   If \c true, parameter \p{argv} is of type <c>wchar_t **</c>',
         *                      otherwise of type <c>char **</c>.
         *                      Defaults to \c false.
         ******************************************************************************************/
        void SetArgs( int argc, const void** argv= nullptr, bool areWideChar= false )
        {
            this->argCount= static_cast<size_t>( argc );
            this->argVector=    argv;
            this->wArgs=        areWideChar;
        }

        /** ****************************************************************************************
         * Alternative method to set the command line argument list. This version can be used
         * by applications that have a dedicated (more sophisticated) CLI interface that
         * do more complex processing of CLI arguments. Such processing might detect which
         * CLI arguments are remaining as potential configuration variables in the overall list
         * of options. In this case, only those arguments should be processed by this class.
         *
         * If this method is invoked (and parameter \p{unrecognizedOptions} is not \c nullptr), then
         * the arguments that previously had been set with overloaded method
         * #SetArgs(int,const void**,bool) are ignored.
         *
         * @param unrecognizedOptions  A vector of argument strings that were not recognized by a
         *                             CLI argument processor and thus left to this class to
         *                             be consume as potential configuration variables.
         ******************************************************************************************/
        void                    SetArgs( std::vector<String>* unrecognizedOptions )
        {
            this->args=     unrecognizedOptions;
        }

         /** ****************************************************************************************
          * Return the plug-in name, in this case, we read resource variable "CfgPlgCLI".
          * @return The name of the plug-in.
          ******************************************************************************************/
         virtual String         Name()   const
         {
            return CONFIG.GetResource( "CfgPlgCLI" );
         }

        /** ****************************************************************************************
         * Searches the variable in the command line parameters.
         *
         * @param variable     The variable to retrieve.
         * @param searchOnly   If \c true, the variable is not set. Defaults to \c false.
         * @return \c true if variable was found, \c false if not.
         ******************************************************************************************/
        ALIB_API virtual bool   Load( Variable& variable, bool searchOnly= false ) const;

        /** ****************************************************************************************
         * Creates an iterator object to return all variables within a section.
         *
         * The iterator object returned, needs to be deleted by the caller.
         *
         * \note Method \alib{config,Configuration::GetIterator} returns an iterator that allows
         *       to iterate across all variables of a section found in all plug-ins
         *
         * @param sectionName  The name of the section to iterate.
         *
         * @returns The iterator requested.
         ******************************************************************************************/
        ALIB_API
        virtual Iterator*       GetIterator( const String& sectionName );
};

/** ************************************************************************************************
 * Specialization of abstract interface class #ConfigurationPlugin, retrieves configuration
 * data from the system environment.
 *
 * Its priority value usually is \alib{config,Priorities::Environment}, which is higher
 * than \alib{config,Priorities::Standard} but lower than \alib{config,Priorities::CLI}.
 *
 * Variable categories are used as a prefix together with an underscore '_'.
 * This means, if variable <em>LOCALE</em> in category <em>ALIB</em> is accessed, the environment
 * variable <em>ALIB_LOCALE</em> is read.
 *
 * Category and Variable names are insensitive in respect to character case.
**************************************************************************************************/
class Environment : public ConfigurationPlugin
{
    protected:

    public:

        /** ****************************************************************************************
         * Constructor.
         ******************************************************************************************/
        Environment();

        /** ****************************************************************************************
         * Virtual Destructor.
         ******************************************************************************************/
        virtual ~Environment() {}

         /** ****************************************************************************************
          * Return the plug-in name, in this case, we read resource variable CfgPlgEnv.
          * @return The name of the plug-in.
          ******************************************************************************************/
         virtual String  Name()   const
         {
            return CONFIG.GetResource( "CfgPlgEnv" );
         }

        /** ****************************************************************************************
         * Searches the variable in the environment.
         *
         * @param variable  The variable to retrieve.
         * @param searchOnly   If \c true, the variable is not set. Defaults to \c false.
         * @return \c true if variable was found, \c false if not.
         ******************************************************************************************/
        ALIB_API virtual bool  Load( Variable& variable, bool searchOnly= false  ) const;
};


}}}// namespace [aworx::lib::config]

#endif // HPP_ALIB_CONFIG_PLUGINS

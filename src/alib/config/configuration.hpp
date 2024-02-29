/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_CONFIGURATION
#define HPP_ALIB_CONFIG_CONFIGURATION 1

#if !defined (HPP_ALIB_ENUMS_BITWISE)
    #include "alib/enums/bitwise.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined(HPP_ALIB_STRINGS_SUBSTRING)
    #include "alib/strings/substring.hpp"
#endif

#if !defined(HPP_ALIB_STRINGS_NUMBERFORMAT)
    #include "alib/strings/numberformat.hpp"
#endif

#if !defined(HPP_ALIB_CONFIG_VARIABLE)
    #include "alib/config/variable.hpp"
#endif
#if !defined(HPP_ALIB_CONFIG_PLUGINS)
    #include "alib/config/plugins.hpp"
#endif
#if !defined(HPP_ALIB_LANG_PLUGINS)
    #include "alib/lang/plugins.hpp"
#endif
#if !defined(HPP_ALIB_CONFIG_INMEMORY_PLUGIN)
    #include "alib/config/inmemoryplugin.hpp"
#endif



namespace alib {  namespace config {

// #################################################################################################
// class Configuration
// #################################################################################################

/** ************************************************************************************************
 * This class is a container for objects of type
 * \ref alib::config::ConfigurationPlugin "ConfigurationPlugin",
 * and provides a single user interface to query configuration data
 * from those.
 *
 * By default, all category and variable names are case insensitive. This is at least true for the
 * default plug-ins delivered with \alib.
 *
 * Variables by default can contain other variables, which are recursively substituted
 * by this method. For more information about the (adjustable) syntax, see
 * \ref alib_config_substitution "Variable Substitution".
 *
 * See documentation of namespace #alib::config for more information on \alib
 * external configuration variables.
 **************************************************************************************************/
class Configuration : public lang::PluginContainer<ConfigurationPlugin, Priorities>
{
    // #############################################################################################
    // internal fields
    // #############################################################################################
    protected:

    // #############################################################################################
    // public fields
    // #############################################################################################
    public:
        /**
         * Values considered to indicate "true". Defaults to
         * { "1", "true", "t", "yes", "y", "on", "ok" }.
         * See methods #IsTrue.
         *
         * Application specific values (e.g. localization languages) might be added by publicly
         * accessing this field and adding new values (or removing existing).
         */
        std::vector<String>                 TrueValues;

        /** Number format definition used to read and write int and float values. */
        alib::NumberFormat                 NumberFormat;

        /**
         * The start string to identify substitution variables.
         * Defaults to single character \c '$'. If a string is set, i.e. \c "${", then field
         * #SubstitutionVariableEnd may be set accordingly, i.e. \c "}"
         */
        String                              SubstitutionVariableStart                 = A_CHAR("$");

        /**
         * The end of a substitution variables.
         * If this field is set, then field #SubstitutionVariableDelimiters is ignored. If this
         * field is nullptr (the default) or empty, it is ignored and characters in field
         * #SubstitutionVariableDelimiters are used to identify the end of the variable.
         */
        String                              SubstitutionVariableEnd                       = nullptr;

        /**
         * The delimiters used to identify the end of a substitutable variable.
         * If field #SubstitutionVariableEnd is not empty, this field is ignored. Otherwise, all
         * characters defined in this string are used to identify the end of a substitution
         * variable.
         */
        CString        SubstitutionVariableDelimiters= A_CHAR(" $@,.;:\"\'+-*/\\§%&()[]{}<>=?'`~#");

        /** A temporary variable to be reused (allocate once pattern). */
        Variable                            tempVar;

    // #############################################################################################
    // Constructor/destructor
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs a Configuration. If \p{addDefaultPlugins} is \c true, registers the initial
         * plug-ins as follows
         *
         *  Priority         | Plug-in Type
         * ------------------|----------------------------------------------------------
         *  \alib{config,Priorities::ProtectedValues}   | \alib{config,InMemoryPlugin}
         *  \alib{config,Priorities::Environment}       | \alib{config,Environment}
         *  \alib{config,Priorities::CLI}               | \alib{config,CLIArgs}
         *  \alib{config,Priorities::DefaultValues}     | \alib{config,InMemoryPlugin}
         *
         * @param addPlugins If \b Yes the default plugins are added.
         ******************************************************************************************/
        ALIB_API        Configuration( lang::CreateDefaults addPlugins );

        /** ****************************************************************************************
         * Sets the command line arguments for default plug-in \alib{config,CLIArgs}.
         * This method should be called for instances of this class after construction.
         *
         * \note
         *   For the standard configuration objects found in class \alib{lang,Camp},
         *   this method is automatically invoked by function \alib{Bootstrap}.
         *
         * \note On the Windows platform, the Microsoft compiler provides the global variables
         *       <em>__argc</em> and <em>__argv</em> (respectively <em>__wargv</em> for wide
         *       character binaries. These variables a can be used if this method is invoked
         *       outside of the <em>main()</em> method.
         *
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         *                Defaults to 0.
         * @param argv    Parameter usually taken from <em>standard C</em> \c main() method
         *                (pointer to a zero-terminated list of zero-terminated strings comprising
         *                the command line arguments).
         *                Defaults to nullptr.
         ******************************************************************************************/
        void        SetCommandLineArgs( int argc  =0, const nchar** argv  =nullptr )
        {
            if ( argc > 1 )
            {
                CLIArgs* cliParameters= GetPluginTypeSafe<CLIArgs>();
                ALIB_ASSERT_ERROR( cliParameters, "CONFIG", "No CLIArgs plugin registered" )
                cliParameters->SetArgs( argc, reinterpret_cast<const void**>( argv ), false );
            }
        }

        /** ****************************************************************************************
         * Variant of method #SetCommandLineArgs, accepting command line arguments of
         * type \c wchar.
         *
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         * @param argv    The command line arguments as a zero-terminated list of pointers to
         *                zero-terminated \c wchar_c strings.
         ******************************************************************************************/
        void        SetCommandLineArgs( int argc, const wchar_t **argv )
        {
            if ( argc > 1 )
            {
                CLIArgs* cliParameters= GetPluginTypeSafe<CLIArgs>();
                ALIB_ASSERT_ERROR( cliParameters, "CONFIG", "No CLIArgs plugin registered" )
                cliParameters->SetArgs( argc, reinterpret_cast<const void**>( argv ),  true );
            }
        }

    // #############################################################################################
    // interface
    // #############################################################################################
    public:

        #if defined(ALIB_DOX)
        /** ****************************************************************************************
         * This method loads all variables of the given enum type in case no placeholders
         * are contained in their name and a default value is given with the declaraition.
         *
         * With that they become stored in this configuration's default plug-in and a later
         * invocation of #FetchFromDefault may be used to move those to a plug-in that persitently
         * stores them, for example an INI-file.
         *
         * The goal of this approach is to have all (most) variables presented in an external
         * configuration file that is automatically created on a software's first run - regardless
         * if that first run used the variables or not.
         *
         * @tparam TEnum Enumeration type equipped with \ref alib_enums_records "ALib Enum Records"
         *               of type \alib{config,VariableDecl}.
         ******************************************************************************************/
        template<typename TEnum>
        inline
        void            PreloadVariables();
        #else
            template<typename TEnum>
            ATMP_VOID_IF( EnumRecords<TEnum>::template AreOfType<VariableDecl>() )
            PreloadVariables()
            {
                for( auto recordIt=  EnumRecords<TEnum>::begin()
                     ;    recordIt!= EnumRecords<TEnum>::end  ()
                     ; ++ recordIt                                 )
                {
                    VariableDecl decl( recordIt.Enum() );
                    if(    decl.DefaultValue.IsNotEmpty()
                        && decl.EnumElementName.IndexOf('%') < 0)   // no replacements
                    {
                        tempVar.Declare( decl, nullptr );
                        tempVar.SetConfig  (this);
                        tempVar.SetPriority( loadImpl( tempVar, false ) );

                        if (  tempVar.Priority()    == Priorities::NONE   )
                            Store( tempVar, tempVar.DefaultValue() );
                    }
                }
            }
    #endif

        /** ****************************************************************************************
         * This method fetches all values from a plug-in of priority
         * \alib{config,Priorities::DefaultValues}, which are not present in the given plug-in
         * \p{dest} and stores them in that.
         * This is useful to collect all generated default values and store them in a user's
         * configuration file. This way, the user can identify configurable options easily.
         *
         * \note
         *   Applications might want to copy only a portion of the default values (of a section)
         *   to not blow up a user's configuration. To achieve this, a custom method to fetch
         *   selected values has to be implemented. In this respect, this method is a very simple
         *   default and its source might be used as a jump start.
         *
         * @param dest      The destination plug-in.
         * @param section   Optional string denoting a section to fetch.
         *                  Defaults to \b NullString().
         * @return The number of variables fetched.
         ******************************************************************************************/
        ALIB_API
        int             FetchFromDefault( ConfigurationPlugin&  dest,
                                          const String&         section= NullString() );


        /** ****************************************************************************************
         * Utility method that checks if a given value represents boolean \b true.
         * Uses field #TrueValues. Comparisons are made case insensitive.
         *
         * @param value  The value to check
         *
         * @return   Returns \c true, if the value represents 'true', \c false if not.
         ******************************************************************************************/
        ALIB_API
        bool            IsTrue( const String&  value );

        /** ****************************************************************************************
         * Receives and optionally creates configuration variable.
         *
         * If the variable was not found and \alib{config,Variable::DefaultValue}
         * in \p{variable} is set, the method adds the value to a plug-in of priority
         * \alib{config,Priorities::DefaultValues},
         * (respectively to the plug-in found at or below \b DefaultValues).
         * For the conversion of the value, field
         * \alib{config,ConfigurationPlugin::StringConverter} of field a plug-in of priority
         * \b DefaultValues, is used.
         *
         * If no plug-in at or below priority \b DefaultValues is found, then the declared default
         * value is added to the variable using the default implementation of
         * \alib{config,XTernalizer::LoadFromString}.
         *
         * @param variable       The variable to receive.
         *
         * @returns The priority of the configuration plug-in that provided the result.
         *          \c 0 if not found.
         ******************************************************************************************/
        ALIB_API
        Priorities      Load( Variable& variable );

        /** ****************************************************************************************
         * Writes the variable into one of the plug-ins registered with this configuration object.
         * The plug-ins are looped in descending order of priority, until a first plug-in
         * confirms the variable to be written.
         *
         * The first (most highly prioritized) plug-in to start the loop with, is defined by
         * field \alib{config,Variable::Priority} of the given \p{variable}.
         *
         * If this is unset (\alib{config,Priorities::NONE}), which is the default value of the field
         * for freshly declared variable objects, the starting priority value  of the loop is
         * \b detected.
         *
         * Detection is made by searching the variable in the plug-ins prior to storing it.
         * The search order is likewise by priority, starting with the highest.
         * If the variable was not found, \alib{config,Priorities::DefaultValues}
         * is chosen. Usually the writing loop then will also already end there,
         * because standard configuration sets have a write-enabled in-memory plug-in at
         * that priority.
         *
         * If the variable was found in the plug-in of priority
         * \alib{config,Priorities::ProtectedValues}, the method stops without storing anything.
         *
         *
         * This approach of storing variables, supports various use cases very nicely.
         * Some of the frequent ones shall be named here:
         * - By setting the field to \alib{config,Priorities::DefaultValues}, the value
         *   becomes just a default and does not overwrite externally specified values.
         * - Leaving the field to its uninitialized state \alib{config,Priorities::NONE},
         *   allows to store the variable in the plug-in that it was originally read from,
         *   or - if not stored already - in default values.
         * - Setting the field to \alib{config,Priorities::ProtectedValues} allows to protect the
         *   variable value in respect to external manipulation.
         * - Setting the field to \alib{config,Priorities::ProtectedValues}<c> - 1</c>,
         *   allows to store the variable just in the plug-in with highest possible priority.
         * - Finally, an application specific reason might motivate a caller of this method to
         *   preset a distinct priority value prior to invoking this method. For example, a variable
         *   might be related to a second one. If the priority of the second one is
         *   known, that priority might be set prior to invoking this message, to have both
         *   variables potentially stored in the same plug-in.
         *
         * The method returns the priority of the configuration plug-in that the value was written
         * to. This value is as well stored in field \alib{config,Variable::Priority} of the given
         * object.
         *
         * A result of \alib{config,Priorities::NONE} indicates that the variable was not written.
         * This might only happen if
         * - the default plug-in of this configuration is not configured or does not support
         *   writing variables,
         * - field \p{Priority} of the variable was set below \alib{config,Priorities::DefaultValues}
         *   (but greater than \alib{config,Priorities::NONE}),
         * - the \b detected priority was \alib{config,Priorities::ProtectedValues}.
         *   (In this case, obviously the application does not want to allow changes and writing the
         *   variable into a different plug-in has no effect.
         *   This way, such variables also do not appear in a user's configuration
         *   in the case that on program termination, new default values are copied there.)
         *
         * Optional parameter \p{externalizedValue} allows to provide a string that is parsed
         * by the storing plug-in to reset the variable's values prior to writing.
         *
         * \see Default variables are often to be stored with the termination of a process.
         *      For this, see method #FetchFromDefault.
         *
         * @param variable              The variable object.
         * @param externalizedValue     Optional externalized value string. If given, the variable
         *                              is set prior to writing.
         *
         * @returns The priority of the configuration plug-in that the value was written to.
         ******************************************************************************************/
        ALIB_API
        Priorities      Store( Variable& variable, const String& externalizedValue= nullptr );

        /** ****************************************************************************************
         * Convenience method that stores the \p{variable} with priority
         * \alib{config,Priorities,Priorities::DefaultValues}.
         *
         * The variable value is determined as follows:
         * - If optional parameter \p{externalizedValue} is provided and not \e nulled, the values
         *   are loaded from that string.
         * - Otherwise, if the variable has no values set but \alib{config,Variable::DefaultValue}
         *   is not \e nulled then values are loaded from this field.
         * - If all is unset (the variable values, parameter \p{externalizedValue} and field
         *   \alib{config,Variable::DefaultValue}), then the unset variable is stored, which results
         *   in removing an existing default value from the configuration.
         *
         *
         * @param variable              The variable object.
         * @param externalizedValue     Optional externalized value string. If given, the variable
         *                              is set prior to writing.
         * @returns The result of \alib{config,Configuration::Store}.
         ******************************************************************************************/
        ALIB_API
        Priorities      StoreDefault( Variable& variable,  const String& externalizedValue= nullptr );

        /** ****************************************************************************************
         * Convenience method that stores the \p{variable} with priority
         * \alib{config,Priorities,Priorities::ProtectedValues}.
         *
         * The variable value is determined as follows:
         * - If optional parameter \p{externalizedValue} is provided and not \e nulled, the values
         *   are loaded from that string.
         * - Otherwise, if the variable has no values set but \alib{config,Variable::DefaultValue}
         *   is not \e nulled then values are loaded from this field.
         * - If all is unset (the variable values, parameter \p{externalizedValue} and field
         *   \alib{config,Variable::DefaultValue}), then the unset variable is stored, which
         *   results in removing an existing protection value from the configuration.
         *
         * @param variable              The variable object.
         * @param externalizedValue     Optional externalized value string. If given, the variable
         *                              is set prior to writing.
         * @returns The result of \alib{config,Configuration::Store}.
         ******************************************************************************************/
        ALIB_API
        Priorities      Protect( Variable& variable, const String& externalizedValue= nullptr );


        /** ****************************************************************************************
         * Convenience method to set values in \p{variable} according to the provided string.
         * For the conversion of the "externalized" string, method
         * \ref alib::config::XTernalizer::LoadFromString "XTernalizer::LoadFromString"
         * of field \alib{config,ConfigurationPlugin::StringConverter}
         * of a plug-in of priority \alib{config,Priorities::DefaultValues}, is used.
         *
         * @param variable              The variable object.
         * @param externalizedValue     The new value to write.
         *
         * @returns The result of \alib{config,Variable::Size} after parsing.
         ******************************************************************************************/
        ALIB_API
        integer         LoadFromString( Variable& variable, const String& externalizedValue );



    // #############################################################################################
    // Iteration
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Iterator interface class, returned by method #GetIterator.
         ******************************************************************************************/
        class Iterator
        {
            public:
                /** The actual variable loaded with successful call to #Next. */
                Variable                Actual;

                /** Virtual destructor.  */
                virtual                ~Iterator()
                {}

                /**
                 * Searches and loads the next variable from the iterated section. On success, the
                 * variable data is stored in #Actual.
                 * @return \c true, if a next variable was found. \c false otherwise.
                 */
                virtual bool            Next()                                                  = 0;

                /**
                 * Resets this iterator to work on a different section of the same configuration.
                 * @param sectionName  The name of the section to iterate.
                 */
                virtual void            ResetToSection( const String& sectionName )             = 0;
        };


        /** ****************************************************************************************
         * Creates an iterator object to return all variables within a section.
         *
         * The iterator object returned, needs to be deleted by the caller.
         *
         * @param sectionName  The name of the section to iterate.
         *
         * @returns The result of \alib{config,Variable::Size} after parsing.
         ******************************************************************************************/
        ALIB_API
        Iterator*       GetIterator( const String& sectionName );

    // #############################################################################################
    // internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Implementation of method #Load.
         *
         * @param variable    The variable to get.
         * @param substitute  If \c false, automatic variable substitutions are suppressed.
         *
         * @return Zero if the variable was not found. Otherwise it returns the priority of the
         *         (first) plug-in that contained the variable.
         ******************************************************************************************/
        ALIB_API
        Priorities     loadImpl( Variable& variable, bool substitute );

}; // class Configuration

} // namespace alib[::config]

/// Type alias in namespace \b alib.
using     Configuration=       config::Configuration;

}  // namespace [alib]



#endif // HPP_ALIB_CONFIG_CONFIGURATION

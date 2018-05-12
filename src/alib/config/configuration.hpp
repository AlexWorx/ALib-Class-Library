// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
    #error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CONFIG_CONFIGURATION)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CONFIG_CONFIGURATION
//! @cond NO_DOX
#define HPP_ALIB_CONFIG_CONFIGURATION 1
//! @endcond

// #################################################################################################
// includes
// #################################################################################################

#if !defined (HPP_ALIB_LANG_ENUM_BITWISE)
    #include "alib/lang/enumbitwise.hpp"
#endif

#if !defined (HPP_ALIB_THREADS_THREADLOCK)
    #include "alib/threads/threadlock.hpp"
#endif

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
#if !defined(HPP_ALIB_UTIL_PLUGINCONTAINER)
    #include "alib/util/plugincontainer.hpp"
#endif
#if !defined(HPP_ALIB_CONFIG_INMEMORY_PLUGIN)
    #include "alib/config/inmemoryplugin.hpp"
#endif



namespace aworx { namespace lib { namespace config {

// #################################################################################################
// class Configuration
// #################################################################################################

/** ************************************************************************************************
 * This class is a container for objects of type
 * \ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin",
 * and provides a single user interface to query configuration data
 * from those.
 *
 * The class inherits from
 * \ref aworx::lib::threads::ThreadLock "ThreadLock" and interface methods are implemented
 * <em>synchronized</em>.
 *
 * By default, all category and variable names are case insensitive. This is at least true for the
 * default plug-ins delivered with \alib.
 *
 * Variables by default can contain other variables, which are recursively substituted
 * by this method. For more information about the (adjustable) syntax, see
 * \ref cpp_alib_namespace_config_substitution "Variable Substitution".
 *
 * See documentation of namespace #aworx::lib::config for more information on \alib
 * external configuration variables.
 **************************************************************************************************/
class Configuration : public util::PluginContainer<ConfigurationPlugin, Priorities>
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

        /// Number format definition used to read and write int and float values.
        aworx::NumberFormat                 NumberFormat;

        /**
         * The start string to identify substitution variables.
         * Defaults to single character \c '$'. If a string is set, i.e. \c "${", then field
         * #SubstitutionVariableEnd may be set accordingly, i.e. \c "}"
         */
        String                              SubstitutionVariableStart                   = ASTR("$");

        /**
         * The end of a substitution variables.
         * If this field is set, then field #SubstitutionVariableDelimiters is ignored. If this field
         * is nullptr (the default) or empty, it is ignored and characters in field
         * #SubstitutionVariableDelimiters are used to identify the end of the variable.
         */
        String                              SubstitutionVariableEnd                       = nullptr;

        /**
         * The delimiters used to identify the end of a substitutable variable.
         * If field #SubstitutionVariableEnd is not empty, this field is ignored. Otherwise, all
         * characters defined in this string are used to identify the end of a substitution
         * variable.
         */
        String                              SubstitutionVariableDelimiters= ASTR(" $@,.;:\"\'+-*/\\§%&()[]{}<>=?'`~#");

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
         * @param addDefaultPlugins If \c true, the default plugins are added.
         *                          Defaults to \c true.
         ******************************************************************************************/
        ALIB_API            Configuration( bool addDefaultPlugins = true );

        /** ****************************************************************************************
         * Destructs a configuration plug-in. All ConfigurationPlugin that were assigned
         * (using #InsertPlugin) will be deleted.
         ******************************************************************************************/
        ALIB_API           ~Configuration();

        /** ****************************************************************************************
         * Sets the command line arguments for default plug-in \alib{config,CLIArgs}.
         * This method should be called for instances of this class after construction.
         *
         * \note
         *   In standard application scenarios, this method is invoked when the
         *   \alib{lang,Library} is initialized using an interface method that accepts
         *   CLI arguments.
         *
         * <p>
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
         *                (pointer to a list of command line arguments).
         *                Defaults to nullptr.
         ******************************************************************************************/
        inline void         SetCommandLineArgs( int argc  =0,  char** argv  =nullptr )
        {
            if ( argc > 1 )
            {
                ALIB_LOCK
                CLIArgs* cliParameters= GetPluginTypeSafe<CLIArgs>();
                ALIB_ASSERT_ERROR( cliParameters, ASTR("No CLIArgs  installed") );
                cliParameters->SetArgs( argc, reinterpret_cast<void**>( argv ), false );
            }
        }

        /** ****************************************************************************************
         * Variant of method #SetCommandLineArgs, accepting command line arguments of
         * type \c wchar_t.
         *
         * @param argc    Parameter usually taken from <em>standard C</em> \c main() method
         *                (the number of arguments in \p{argv}).
         * @param argv    The command line parameters as \c wchar_t.
         ******************************************************************************************/
        inline void         SetCommandLineArgs( int  argc,    wchar_t **argv )
        {
            if ( argc > 1 )
            {
                ALIB_LOCK
                CLIArgs* cliParameters= GetPluginTypeSafe<CLIArgs>();
                ALIB_ASSERT_ERROR( cliParameters, ASTR("No CLIArgs  installed") );
                cliParameters->SetArgs( argc, reinterpret_cast<void**>( argv ),  true );
            }
        }

    // #############################################################################################
    // interface
    // #############################################################################################
    public:


        /** ****************************************************************************************
         * This method "pre-loads" all variables of the given enum type.
         * This way, their default value is stored in the default configuration plug-in and
         * a later invocation of #FetchFromDefault fetches all variables, regardless if a
         * "run" of the software used the variables or not.
         *
         * @tparam TEnum         Enum type equipped with meta data according to the scheme of
         *                       configuration variables.
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this constructor
         *                       only for associated custom C++ enum types.
         ******************************************************************************************/
        template<typename TEnum,
                 typename TEnableIf= typename std::enable_if<
                            std::is_same< VariableDecl::TTuple, typename lang::T_EnumMetaDataDecl<TEnum>::TTuple >::value
                                                             >::type>
        inline
        void     PreloadVariables()
        {
            Variable  var;
            auto metaData= lib::lang::EnumMetaData<TEnum>::GetSingleton();
            metaData->CheckLoad();
            for( auto& entry : metaData->Table )
                Load( var.Declare( TEnum( std::get<0>(entry) ) ));
        }

        /** ****************************************************************************************
         * This method fetches all values from a plug-in of priority #DefaultValues, which are not
         * present in the given plug-in \p{dest} and stores them in that.
         * This is useful to collect all generated default values and store them in a users'
         * configuration file. This way, the user can identify configurable options easily.
         *
         * \note
         *   Applications might want to copy only a portion of the default values (of a section)
         *   to not blow up a users' configuration. To achieve this, a custom method to fetch
         *   selected values has to be implemented. In this respect, this method is a very simple
         *   default and its source might be used as a jump start.
         *
         * @param dest      The destination plug-in.
         * @param section   Optional string denoting a section to fetch.
         *                  Defaults to \c NullString.
         * @return The number of variables fetched.
         ******************************************************************************************/
        ALIB_API
        int     FetchFromDefault( ConfigurationPlugin& dest, const String& section= NullString );


        /** ****************************************************************************************
         * Utility method that checks if a given value represents boolean \b true.
         * Uses field #TrueValues. Comparisons are made case insensitive.
         *
         * @param value  The value to check
         *
         * @return   Returns \c true, if the value represents 'true', \c false if not.
         ******************************************************************************************/
        ALIB_API
        bool    IsTrue( const String&  value );

        /** ****************************************************************************************
         * Receives and optionally creates configuration variable.
         *
         * If the variable was not found and \alib{config,Variable::DefaultValue}
         * in \p{variable} is set, the method adds the value value to a plug-in of priority
         * #DefaultValues, (respectively to the plug-in found at or below #DefaultValues).
         * For the conversion of the value, field
         * \alib{config,ConfigurationPlugin::StringConverter} of field a plug-in of priority
         * #DefaultValues, is used. (Which has to be present.)
         *
         * @param variable       The variable to receive.
         *
         * @returns The priority of the configuration plug-in that provided the result.
         *          \c 0 if not found.
         ******************************************************************************************/
        ALIB_API
        Priorities     Load( Variable& variable );

        /** ****************************************************************************************
         * Writes the variable to the configuration.
         * In general, this is done by asking  each of our installed plug-ins - in the order of
         * their priority - to do so.
         * As soon as the first plug-in returns \c true, the variable is considered written and no
         * other plug-in is asked.
         * This way, the variable is stored by the plug-in with the highest priority.
         *
         * The maximum priority to start the loop with depends on field
         * \ref aworx::lib::config::Variable::Priority "Variable::Priority"
         * of the given \p{variable}. The rules are as follows:
         * - If the value is \alib{config,Priorities::NONE}, which is the default value of new
         *   variables or ones that were freshly declared, then prior to storing the value, the
         *   variable is (tried to be) loaded first (without actually overwriting the values).
         *   After that, one of the next two rules apply.
         * - If the value is still \alib{config,Priorities::NONE}, which indicates that a previous
         *   load operation failed, the loop starts with \alib{config,Priorities,Priorities::DefaultValues}
         *   (and usually ends there, as standard configuration sets
         *   do not have plug-ins with lower priorities installed). In other words, newly created
         *   variables are stored in the plug-in of priority #DefaultValues (usually an
         *   \b InMemoryPlugin). This way, they are
         *   not written to external configuration files, unless the application explicitly moves
         *   such new default values to dedicated other plug-ins (e.g. on termination).
         * - If the value is greater than \c 0, the value is used as the start of the loop.
         *   This way, an already defined variable will be stored in the same plug-in as it was
         *   found or one with a lower priority, if that plug-in does not provide writing
         *   capabilities.
         *
         * Consequently, as field \alib{config,Variable::Priority} is public, the behavior
         * can be manipulated, by setting the field explicitly prior to invoking this method.
         * Some frequent use cases shall be named here:
         * - Setting the field to \alib{config,Priorities::ProtectedValues} allows to protect the
         *   variable value in respect to external manipulation.
         * - By setting the field to #DefaultValues, the value becomes just a default
         *   and does not overwrite externally specified values.
         * - Setting the field to a distinct priority value that names a user-specific configuration
         *   (vs. a system-wide configuration) to store into.
         * - Setting the field to #ProtectedValues <c>- 1</c>, allows to store the
         *   variable just in the plug-in with highest possible priority, for example
         *   a user specific configuration is preferred to a system wide configuration)
         * - A variable might be related to a second one. If the priority of the second one is
         *   known, that priority might be set to possibly have both variables stored in the same
         *   plug-in.
         *
         * The method returns the priority of the configuration plug-in that the value was written
         * to as well as storing this value in field \alib{config,Variable::Priority}.
         * If the result is \c 0, the variable was not written. This might only happen if
         * - either field default plug-in of priority #DefaultValues was modified
         *   (removed or exchanged with a different  plug-in that does not write the value)
         * - or if field \p{Priority} of the variable was set below #DefaultValues and greater
         *   than \c 0.
         * - The detected (!) priority was #ProtectedValues.
         *   In this case, obviously the application does not want to allow changes and writing the
         *   variable into a different plug-in has no effect.
         *   This way, such variables also do not appear in a users' configuration
         *   in the case that on program termination, new default values are copied there.
         *
         * Optional parameter \p{externalizedValue} allows to provide a string that is parsed
         * by the storing plug-in to reset the variables' values prior to writing.
         *
         * @param variable              The variable object.
         * @param externalizedValue     Optional externalized value string. If given, the variable
         *                              is set prior to writing.
         *
         * @returns The priority of the configuration plug-in that the value was written to.
         ******************************************************************************************/
        ALIB_API
        Priorities   Store( Variable& variable, const String& externalizedValue= nullptr );

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
        Priorities     StoreDefault( Variable& variable,  const String& externalizedValue= nullptr );

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
        Priorities     Protect( Variable& variable, const String& externalizedValue= nullptr );


        /** ****************************************************************************************
         * Convenience method to set values in \p{variable} according to the provided string.
         * For the conversion of the "externalized" string, method
         * \ref aworx::lib::config::XTernalizer::LoadFromString "XTernalizer::LoadFromString"
         * of field \alib{config,ConfigurationPlugin::StringConverter}
         * of a plug-in of priority #DefaultValues, is used.
         *
         * @param variable              The variable object.
         * @param externalizedValue     The new value to write.
         *
         * @returns The result of \alib{config,Variable::Size} after parsing.
         ******************************************************************************************/
        ALIB_API
        int              LoadFromString( Variable& variable, const String& externalizedValue );



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
            /// The actual variable loaded with successful call to #Next.
            Variable                Actual;

            /**
             * Virtual destructor.
             */
            virtual                ~Iterator()
            {}

            /**
             * Searches and loads the next variable from the iterated section. On success, the
             * variable data is stored in #Actual.
             * @return \c true, if a next variable was found. \c false otherwise.
             */
            virtual bool            Next()                                                      = 0;
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
         * Implementation of get method. No locking is performed (has to be done before
         * invoking this method)
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

}} // namespace aworx[::lib::config]


/// Type alias in namespace #aworx.
using     Configuration=       aworx::lib::config::Configuration;

/// Type alias in namespace #aworx.
using     Variable= aworx::lib::config::Variable;

}  // namespace [aworx]



#endif // HPP_ALIB_CONFIG_CONFIGURATION

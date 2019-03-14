// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CONFIG_VARIABLE
#define HPP_ALIB_CONFIG_VARIABLE 1

#if !defined(HPP_ALIB_CONFIG_CONFIG)
#   include "alib/config/config.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

namespace aworx { namespace lib { namespace config {

// Forward declarations
class Configuration;


/** ************************************************************************************************
 * This class is used to load and store external configuration data with objects of class
 * \ref aworx::lib::config::Configuration       "Configuration" and its plug-ins
 * \ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin".
 *
 * \note
 *   For general information about external configuration variables, see namespace documentation
 *   \ref aworx::lib::config "aworx::lib::config".
 *
 * <b>Construction/Redeclaration:</b><br>
 * While constructors accepting attributes of a variable exist, it is recommended to
 * declare all external configuration variables in a central place, using statically or dynamically
 * allocated objects of type
 * \ref aworx::lib::config::VariableDecl "VariableDecl" and pass such record
 * to the constructor of a variable.
 *
 * The class is designed to be 'reused' to avoid repeated allocation/de-allocation of memory.
 * After invoking one of the overloaded methods #Declare, which share the same signatures as
 * the overloaded constructors, a variable is freshly initialized. Internally, the memory
 * allocated for values remains allocated.
 *
 * <b>Values:</b><br>
 * A variable can contain zero, one or several values. If zero, then the variable was either not
 * loaded, yet or the load operation failed (no configuration plug-in defined the variable).
 * Method #Size reports the currently available values and methods #GetString(int), #GetInteger(int)
 * and #GetFloat(int) return a value. Internally all values are stored as strings. If
 * field #Config is set, its field
 * \ref aworx::lib::config::Configuration::NumberFormat "Configuration::NumberFormat"
 * is used for floating point conversion.
 *
 * When storing a variable that contains more than one value, field #Delim has to be set.
 * Conversely, when reading a variable that contains multiple values, the delimiter has to be set
 * prior to the load operation.
 * \note
 *   This is not true when loading/storing a variable directly in a plug-in of type
 *   \ref aworx::lib::config::InMemoryPlugin "InMemoryPlugin"   or might also not be true
 *   with custom configuration plug-in types which
 *   for example might store the values in a database.<br>
 *   However, with the default plug-ins
 *   \ref aworx::lib::config::CLIArgs "CLIArgs",
 *   \ref aworx::lib::config::Environment "Environment" and
 *   \ref aworx::lib::config::IniFile "IniFile"
 *   the delimiter is needed! Therefore, it is best practice to always define a proper delimiter if
 *   a variable is multi-valued.
 *
 * <b>Loading and Storing:</b><br>
 * There are two ways of loading and storing a variable:
 * - Using the interface of class \ref aworx::lib::config::Configuration "Configuration"
 *   which allows to load and store variables from different sources (plug-ins) in a prioritized
 *   way.
 * - Using the interface of class \ref aworx::lib::config::ConfigurationPlugin "ConfigurationPlugin"
 *   which may be used if the decision about the source or drain of a load/store operation is
 *   explicitly made by a code unit - instead of by the configuration.
 *
 * Storing empty variables (method #Size returns \c 0) deletes a variable from the those
 * configuration plug-ins that are write enabled.
 **************************************************************************************************/
class Variable
{
    // #############################################################################################
    // Public fields
    // #############################################################################################
    public:

        /** The \b %Configuration that was recently used to request or store the value. */
        Configuration*  Config;

        /** The configuration variable category. */
        String64        Category;

        /** The configuration variable name. */
        String64        Name;

        /**
         *  The delimiter used for parsing and storing values by simple textual plug-ins which
         *  use the default version of
         *  \ref aworx::lib::config::XTernalizer "XTernalizer"
         *  for in- and externalizing variables.
         */
        character       Delim;

        /** Hints for formatting textual configuration files. (Used by class
            \ref aworx::lib::config::IniFile "IniFile" and potentially by custom plug-ins.*/
        FormatHints     FmtHints;

        /** If set, attributes written in multi-lines are vertically aligned by this character or
         *  string. Use cases are "=", ":" or "->".<br> Used by
         *  \ref aworx::lib::config::IniFile "IniFile" and potentially by custom plug-ins. */
        String16        FormatAttrAlignment;

        /** The configuration variable comments with placeholders replaced. */
        AString         Comments;



        /** The configuration variable category and name concatenated with an underscore
         *  character \c '_'. This is useful e.g. for naming variables in log messages. */
        String64        Fullname;

        /**
         * The default value provided as an externalized string.
         *
         * The only occasion that this value is used is with method
         * \ref aworx::lib::config::Configuration::Load   "Configuration::Load".
         * If no plug-in has this variable defined and this field is not \e nulled, then the value
         * is written into plug-in of priority \alib{config,Priorities::DefaultValues},
         * respectively - if this default plug-in was replaced by the user - into a plug-in found at
         * or below this priority.
         *
         * In this case, the value is parsed using method
         * \ref aworx::lib::config::XTernalizer::LoadFromString  "XTernalizer::LoadFromString"
         * of field
         * \ref aworx::lib::config::ConfigurationPlugin::StringConverter  "ConfigurationPlugin::StringConverter"
         * of the plug-in writing the value.
         *
         * \note
         *   The field is ignored when using the plug-in interface
         *   \ref aworx::lib::config::ConfigurationPlugin::Load "ConfigurationPlugin::Load"
         *   directly. To store this value 'manually' directly to a plug-in, invoke
         *   \ref aworx::lib::config::ConfigurationPlugin::Store "ConfigurationPlugin::Store"
         *   with providing value explicitly.
         */
        AString         DefaultValue;

        /**
         * A value related to the priority of a configuration plug-in.
         * The following values apply:
         * - \alib{config,Priorities::NONE} after creation or declaration (reuse).
         * - The priority of the plug-in that loaded the value (after calling
         *   \ref aworx::lib::config::Configuration::Load   "Configuration::Load").
         * - The priority of the plug-in that stored the value (after calling
         *   \ref aworx::lib::config::Configuration::Store "Configuration::Store").
         * - \c 0, if the last load or store operation failed.
         * In addition prior to storing a variable, the value might be manually set. See
         * documentation of
         * \ref aworx::lib::config::Configuration::Store "Configuration::Store") for details.
         */
        Priorities   Priority;

    // #############################################################################################
    // Protected fields
    // #############################################################################################
    protected:
        /** The values. */
        std::vector<AString> values;

        /** The number of values currently stored. This may be less than items found in #values
            as those are reused objects. */
        int qtyValues=   0;


    // #############################################################################################
    // Constructors and Define
    // #############################################################################################
    public:
        /** ****************************************************************************************
         * Constructs an undefined Variable. Prior to using this, #Declare has to be invoked.
         ******************************************************************************************/
         Variable()    { clear(); }

        /** ****************************************************************************************
         * Constructs a variable from a declaration.
         * Strings named \c "%1", \c "%2" ... \c "%N" found in the fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement strings found
         * in variadic argument list \p{replacements}.
         *
         * @param declaration    The declaration data of the variable.
         * @param replacements   List of arguments. Must be of types that are accepted by constructor
         *                       of class \alib{strings,TString,String}.
         * @tparam StringTypes   The variadic argument types.
         ******************************************************************************************/
        template<typename... StringTypes>
        Variable( const VariableDecl& declaration,  const StringTypes&... replacements )
        {
            Declare( declaration, replacements... );
        }

        /** ****************************************************************************************
         * Constructs a variable using the declaration of another variable. The values are not
         * copied.
         *
         * @param variable  A variable to copy the declaration (which is comprised with fields
         *                  #Category, #Name, #Fullname, #Delim, #Comments and #DefaultValue) from.
         ******************************************************************************************/
        Variable( const Variable& variable )
        {
            Declare( variable );
        }

        /** ****************************************************************************************
         * Constructs a variable from the given values
         *
         * @param category  The category of the variable.
         * @param name      The name of the variable
         * @param delim     A proposal for a delimiter that might be used by some plug-ins
         *                  to delimit different values from each other (e.g. INI files).
         * @param comments  Comment lines that might be added in the configuration storage
         *                  (plug-in implementation dependent).
         ******************************************************************************************/
        Variable( const String& category,  const String& name,  character delim= '\0',
                  const String& comments  = nullptr      )
        {
            Declare( category, name, delim, comments );
        }

#if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * Constructs a variable from a \ref ALIB_CONFIG_VARIABLES "resourced variable declaration".
         *
         * \see Description of class \alib{config,VariableDecl} and
         *      macro \ref ALIB_CONFIG_VARIABLES.
         *
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       constructor only for associated custom C++ enum types.
         ******************************************************************************************/
        template<typename TEnum, typename TEnableIf=void>
        inline
        Variable( TEnum declaration );
#else
        template<typename TEnum, typename TEnableIf=
        ATMP_VOID_IF(ATMP_EQ(VariableDecl::TTuple, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
        inline
        Variable( TEnum declaration )
        {
            Declare( declaration );
        }
#endif

#if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * Constructs a variable from a \ref ALIB_CONFIG_VARIABLES "resourced variable declaration".
         *
         * Strings named \c "%1", \c "%2" ... \c "%N" found in the fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement strings found
         * in variadic argument list \p{replacements}.
         *
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @param replacements   List of arguments. Must be of types that are accepted by constructor
         *                       of class \alib{strings,TString,String}.
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       constructor only for associated custom C++ enum types.
         ******************************************************************************************/
        template<typename TEnum, typename... StringTypes, typename TEnableIf= void>
        Variable( TEnum declaration,  const StringTypes&... replacements );
#else
        template<typename TEnum, typename... StringTypes, typename TEnableIf=
        ATMP_VOID_IF(ATMP_EQ(VariableDecl::TTuple, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
        Variable( TEnum declaration,  const StringTypes&... replacements )
        {
            Declare( declaration, replacements... );
        }
#endif

        /** ****************************************************************************************
         * Re-initializes a variable from a declaration. Strings named \c "%1", \c "%2" ... \c "%N"
         * found in the fields  #Category, #Name, #Comments and #DefaultValue are replaced with
         * given replacement string arguments in vector \p{replacements}.
         *
         * @param declaration    The declaration data of the variable.
         * @param replacements   List of replacement strings.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Declare( const  VariableDecl&   declaration, Boxes&   replacements );

        /** ****************************************************************************************
         * Re-initializes a variable from a declaration.
         *
         * Strings named \c "%1", \c "%2" ... \c "%N" found in fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement string arguments in
         * variadic argument list \p{replacements}.
         *
         * @param declaration    The declaration data of the variable.
         * @param replacements   Replacement values. Must be of types that are
         *                       \alib{strings,T_Append,appendable} to \b %AString objects.
         * @tparam TArgs         The variadic argument types.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        template<typename... TArgs>
        Variable&   Declare( const VariableDecl& declaration, const TArgs&... replacements )
        {
            Boxes argsAsVector= {replacements...};

            return Declare( declaration, argsAsVector );
        }

#if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * \ref clear "Clears" the variable resets its declaration.
         * Internally uses an instance of class \alib{config,VariableDecl} constructed with
         * enum element \p{declaration}.
         *
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       method only for associated custom C++ enum types.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        template<typename TEnum, typename TEnableIf= void>
        Variable&   Declare( TEnum declaration );
#else
        template<typename TEnum,typename TEnableIf=
        ATMP_VOID_IF(ATMP_EQ(VariableDecl::TTuple, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
        Variable&   Declare( TEnum declaration )
        {
            VariableDecl decl( declaration );
            return Declare( decl );
        }
#endif


#if ALIB_DOCUMENTATION_PARSER
        /** ****************************************************************************************
         * \ref clear "Clears" the variable resets its declaration.
         * Internally uses an instance of class \alib{config,VariableDecl} constructed with
         * enum element \p{declaration}.
         *
         * Strings named \c "%1", \c "%2" ... \c "%N" found in fields #Category, #Name,
         * #Comments and #DefaultValue are replaced with given replacement values in
         * variadic argument list \p{replacements}.
         *
         * @param declaration    Element of an enum class that is representing configuration
         *                       variables.
         * @param replacements   Replacement values. Must be of types that are
         *                       \alib{strings,T_Append,appendable} to \b %AString objects.
         * @tparam TEnum         The type of parameter \p{declaration}
         * @tparam TEnableIf     Not to be specified. Used by the compiler to select this
         *                       method only for associated custom C++ enum types.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        template<typename TEnum, typename... TArgs, typename TEnableIf= void>
        Variable&   Declare( TEnum declaration, const TArgs&... replacements );
#else
        template<typename TEnum, typename... TArgs, typename TEnableIf=
        ATMP_VOID_IF(ATMP_EQ(VariableDecl::TTuple, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
        Variable&   Declare( TEnum declaration, const TArgs&... replacements )
        {
            VariableDecl decl( declaration );
            Boxes argsAsVector= {replacements...};
            return Declare( decl, argsAsVector );
        }
#endif

        /** ****************************************************************************************
         * Constructs a variable using the declaration of another variable. The values are not
         * copied.
         *
         * @param variable  A variable to copy the declaration (which is comprised with fields
         *                  #Category, #Name, #Fullname, #Delim, #Comments and #DefaultValue) from.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        Variable& Declare ( const Variable& variable )
        {
            clear();

            Category._(variable.Category);
            Name    ._(variable.Name);
            Fullname._(variable.Fullname);
            Comments._(variable.Comments);
            Delim=     variable.Delim;
            return *this;
        }

        /** ****************************************************************************************
         * Re-initializes the variable using resources.
         *
         * @param category  The category of the variable.
         * @param name      The name of the variable
         * @param delim     A proposal for a delimiter that might be used by some plug-ins
         *                  to delimit different values from each other (e.g. INI files).
         *                  Defaults to \c '\0'.
         * @param comments  Comment lines that might be added in the configuration storage
         *                  (plug-in implementation dependent).
         *                  Defaults to \c nullptr.
         *
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        ALIB_API
        Variable&   Declare( const String& category,  const String& name,
                             character         delim    = '\0',
                             const String& comments = nullptr                  );


    // #############################################################################################
    // Interface
    // #############################################################################################
        /** ****************************************************************************************
         * Returns the number of values set in this object.
         *
         * @return The number of values set.
         ******************************************************************************************/
        inline
        int         Size() const
        {
            return qtyValues;
        }

        /** ****************************************************************************************
         * Clears the value, respectively the list of values set.
         * After invoking this method, #Size will return \p{startIdx} and #GetString will
         * return \c nullptr for any index greater or equal to \p{startIdx}.
         *
         * @param  startIdx  The index of the first value to be cleared. Defaults to \c 0.
         * @return \c *this to allow concatenated operations.
         ******************************************************************************************/
        inline
        Variable&   ClearValues(int startIdx= 0)
        {
            qtyValues= startIdx;
            return *this;
        }

        /** ****************************************************************************************
         * Adds a value to the end of the list of values.
         * @return A reference to the string which can be manipulated to set the value.
         ******************************************************************************************/
        ALIB_API
        AString&    Add();

        /** ****************************************************************************************
         * Adds the given value to the end of the list of values.
         * Template type \p{TAppendable} needs to be a type which is
         * \ref alib_strings_assembly_ttostring "appendable" to objects of type \b %AString.
         *
         * If a different format is desired (minimum digits, etc.), then
         * #Add is to be used and conversion done proprietary on the returned string objects.
         *
         * @param  value  The value to set.
         * @return A reference to the string representing the integer value.
         ******************************************************************************************/
        template<typename TAppendable>
        inline
        AString&  Add( const TAppendable& value )
        {
            return Add()._( value );
        }

        /** ****************************************************************************************
         * Replaces the value at \p{idx} with the values of the given other variable.
         *
         * @param  idx          The index of the value to replace.
         * @param  replVariable The variable providing the replacement values.
         ******************************************************************************************/
        ALIB_API
        void ReplaceValue( int idx, Variable& replVariable );

        /** ****************************************************************************************
         * Returns the value with the given index. Valid values for parameter \p{idx} are
         * between \b 0 and #Size <b> - 1</b>.<br>
         * In debug-compilations an error is raised if the given \p{idx} is out of range.
         *
         * \note
         *  It is explicitly allowed to change the contents of the \b %AString object returned,
         *  e.g. for a subsequent store operation.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx}.
         ******************************************************************************************/
        inline
        AString&    GetString( int idx= 0 )
        {
            ALIB_ASSERT_ERROR( idx >= 0  &&  idx < qtyValues, "Index out of range: ", idx );
            return values[static_cast<size_t>(idx)];
        }

        /** ****************************************************************************************
         * Returns the value at \p{idx} interpreted as an integer.
         * If the index is invalid, \c 0 is returned.
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as an integer value.
         ******************************************************************************************/
        ALIB_API
        integer GetInteger(int idx= 0);

        /** ****************************************************************************************
         * Returns the value at \p{idx} interpreted as a double value.
         * If the index is invalid, \c 0.0 is returned.
         * Parsing is done using field \c NumberFormat of field #Config, respectively, if this is
         * not set, the static singleton \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as a double value.
         ******************************************************************************************/
        ALIB_API
        double  GetFloat(int idx= 0);

        /** ****************************************************************************************
         * Returns \c true if the first value represents a boolean 'true'.
         * Evaluation is done using field #Config, respectively if this is not set, the static
         * singleton \alib{strings,TNumberFormat::Global,NumberFormat::Global}.
         *
         * @param  idx  The index of the value to be retrieved.  Defaults to \c 0.
         * @return The value at \p{idx} interpreted as a boolean value.
         ******************************************************************************************/
        ALIB_API
        bool    IsTrue(int idx= 0);

        /** ****************************************************************************************
         * Searches in the values of this variable for the pattern <c>attrName = result</c> and
         * sets parameter \p{result} to the string following this pattern.
         *
         * @param      attrName     The name of the attribute searched.
         * @param[out] result       A sub-string with the result.
         * @param      attrDelim    The delimiter to search for. Defaults to '='.
         * @return \c true if the attribute was found, \c false otherwise.
         ******************************************************************************************/
        ALIB_API
        bool    GetAttribute( const String& attrName, Substring& result, character attrDelim= A_CHAR('=') );


    // #############################################################################################
    // protected methods
    // #############################################################################################
    protected:
        /** Clears all values. */
        ALIB_API
        void    clear();

};  // class Variable

}} // namespace aworx[::lib::config]



// /// Type alias in namespace #aworx.
using     Variable=           aworx::lib::config::Variable;

}  // namespace [aworx]

#endif // HPP_ALIB_CONFIG_VARIABLE

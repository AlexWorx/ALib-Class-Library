// #################################################################################################
//  ALib C++ Library
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_CONFIG_CONFIG
#define HPP_ALIB_CONFIG_CONFIG 1

#if !defined (HPP_ALIB_RESULTS_EXCEPTION)
    #include "alib/results/exception.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

#if !defined (HPP_ALIB_ENUMS_ENUM_ARITHMETICAL)
    #include "alib/enums/enumarithmetical.hpp"
#endif

#ifndef HPP_ALIB_ENUMS_ENUM_VALUE
#   include "alib/enums/enumvalue.hpp"
#endif


namespace aworx { namespace lib { namespace config {


/** ************************************************************************************************
 * Exception codes of namespace #aworx::lib::config.
 **************************************************************************************************/
enum class Exceptions
{
    /** File not found when reading. */
    ErrorOpeningFile= 1,

    /** An error occurred writing the file . */
    ErrorWritingFile= 2,
};

/** ************************************************************************************************
 * \alib{enums,T_EnumIsArithmetical,Arithmetical enumeration} used to prioritize
 * of type \alib{config,ConfigurationPlugin} objects attached to instances of
 * class \alib{config,Configuration}.
 **************************************************************************************************/
enum class Priorities
{
    /** Used to denote that no variable or plug-in was found. */
    NONE                    = 0,

    /**
     * Constant providing a priority which is even lower than default. This priority is not used by
     * any standard plug-ins and currently not even anywhere in detail \alib. The rationale to
     * provide it, is for third party libraries that may use this value in cases where values are
     * estimated or detected instead of defaulted.<br>
     * A user of the module may then overwrite such auto-detection by setting a default value in
     * the configuration.<br>
     * The underlying integer value is \c 500.
     */
    AutoDetected            = 500,

    /**
     * Used to store default values. By default, a plug-in of type \alib{config,InMemoryPlugin} is
     * installed here.. The underlying integer value is \c 500. \c 10000.
     */
    DefaultValues           = 10000,


    /**
     * Used to attach the main application configuration plug-in.   For example an plug-in of type
     * \alib{config,IniFile}. The underlying integer value is \c 500. \c 20000.
     */
    Standard                = 20000,


    /**
     * Used with plug-in \alib{config,Environment}.  The underlying integer value is \c 30000.
     */
    Environment             = 30000,


    /**
     * Used with plug-in \alib{config,CLIArgs}. The underlying integer value is \c 40000.
     */
    CLI                     = 40000,

    /**
     * Used to store protected values. By default, a plug-in of type \alib{config,InMemoryPlugin} is
     * installed here. No other plug-in can overwrite configuration variables which are
     * defined in a plug-in with this priority.<br>
     * The underlying integer value is  <c>std::numeric_limits<int>::max()</c>.
     */
    ProtectedValues         = (std::numeric_limits<int>::max)(),
};

/** ************************************************************************************************
 * Denotes hints for formatting variables when storing in external configuration files
 **************************************************************************************************/
enum class FormatHints
{
    /* No hints **/
    None                    = 0      ,

    /* Write each argument in a new line **/
    MultLine                = 1 <<  0,

    /* Suppress spaces around the delimiter (only used in single line mode) **/
    NoDelimSpaces           = 1 <<  1,

    /* This and upward bits are reserved for custom plug-ins **/
    Custom                  = 1 <<  16,
};

/** ************************************************************************************************
 * The module class.
 *
 * This is a strict singleton class. The only instance is found with namespace variable
 * \ref aworx::lib::CONFIG.
 **************************************************************************************************/
class Config : public Module
{
    public:
        /** ****************************************************************************************
         * Constructor.<br>
         * While this is public, it must not be invoked as this is a strict singleton type.
         * (See notes in \ref alib_manual_bootstrapping_class_modsingletons).
         ******************************************************************************************/
        Config();

    protected:
        /** ****************************************************************************************
         * Initializes this module.
         *
         * @param level  The initialization level to reach.
         * @param argc   The number of command line arguments. Defaults to \c 0.
         * @param argv   List of command line arguments if given as single byte character strings.
         * @param wargv  List of command line arguments if given as multi-byte character strings.
         ******************************************************************************************/
        virtual void        init( InitLevels level,
                                  int argc, const char** argv, const wchar_t** wargv )     override;

        /** ****************************************************************************************
         * Frees resources of this module.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class Config

struct VariableDecl;

} // namespace aworx::lib[::config]

/** The singleton of module-class \alib{config,Config}. */
extern ALIB_API config::Config CONFIG;

}} // namespace [aworx::lib]

#if !ALIB_DOCUMENTATION_PARSER
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::config::VariableDecl,
                              String, String, String, character, String, int, String )
#endif

namespace aworx { namespace lib { namespace config {

/** ************************************************************************************************
 * A data record used to declare an \alib{config,Variable,ALib Configuration Variable}.
 * While variables can also be declared by setting their attributes individually, it is recommended
 * to declare all external configuration variables in a central place, using statically or
 * dynamically allocated objects of this type.<br>
 * Objects of class \b %Variable can be constructed and existing objects can be reused by invoking
 * \ref aworx::lib::config::Variable::Declare "Variable::Declare". Both, construction and
 * re-declaration of variables, use objects of this type.
 *
 * All fields (except #Delim) support placeholders \c "%1", \c "%2" ... \c "%N", which are
 * replaced with the constructor of class \b %Variable and method
 * \ref aworx::lib::config::Variable::Declare "Variable::Declare".
 * This allows to define a series of variables whose category, name, description and value is
 * dependent on run-time information.
 *
 * Besides the use for passing parameters into objects of type Variable, this struct does not
 * provide further logic.
 *
 * In this C++ implementation of \alib, this class can be created from elements of enum types.
 * For this, the concept of \alib{resources,T_EnumMetaDataDecl,enum meta data} is used.
 * The meta data of applicable enumeration types has to follow a certain tuple type, specified
 * with #TTuple.
 * Macro \ref ALIB_CONFIG_VARIABLES implements this association and is to be used to make a C++ enum
 * type compatible with the constructor of this struct. The macro in addition enables the enum
 * meta data to be loaded from the \alib{resources,Resources} of a \alib{Module}.
 **************************************************************************************************/
struct VariableDecl
{
    /**
     * The type of the tuple of \alib{resources,T_EnumMetaDataDecl,enum meta data} used with
     * enumerations that denote configuration variables.
     *
     * The elements of the tuple have the following meaning:
     *
     * Index | Description
     * ------|-------------
     *   0   | The originating enum value of the element of the custom C++ enumeration type.
     *   1   | Value for field \alib{config,Variable::Category}.
     *   2   | Value for field \alib{config,Variable::Name}.
     *   3   | Value for field \alib{config,Variable::DefaultValue}.
     *   4   | Value for field \alib{config,Variable::Delim}.
     *   5   | Value for field \alib{config,Variable::FormatAttrAlignment}.
     *   6   | Value for field \alib{config,Variable::FmtHints}.
     *   7   | Value for field \alib{config,Variable::Comments}.
     *
     * \see Macro \ref ALIB_CONFIG_VARIABLES.
     */
     using TTuple= resources::T_EnumMetaDataSpecification<VariableDecl>::TTuple;


    /** The variable declaration data. */
    TTuple      Tuple;


    /** ********************************************************************************************
     * Constructor usually used with static variable declarations (declarations that are not
     * using enumeration types associated with \ref ALIB_CONFIG_VARIABLES.
     *
     * @param category            Value for field \alib{config,Variable::Category}.
     * @param name                Value for field \alib{config,Variable::Name}.
     * @param defaultValue        Value for field \alib{config,Variable::DefaultValue}.
     * @param delim               Value for field \alib{config,Variable::Delim}.
     * @param formatAttrAlignment Value for field \alib{config,Variable::FormatAttrAlignment}.
     * @param formatHints         Value for field \alib{config,Variable::FmtHints}.
     * @param comments            Value for field \alib{config,Variable::Comments}.
     **********************************************************************************************/
    inline
    VariableDecl( String        category,
                  String        name,
                  String        defaultValue,
                  character     delim,
                  String        formatAttrAlignment,
                  FormatHints   formatHints,
                  String        comments                )
    : Tuple( std::make_tuple( -1, category, name, defaultValue,
                              delim, formatAttrAlignment, EnumValue(formatHints), comments ) )
    {}


#if ALIB_DOCUMENTATION_PARSER
    /** ********************************************************************************************
     * Constructor that accepts an element of a C++ enum type equipped with meta information
     * that contains the declaration data.
     *
     * First, method \alib{resources,EnumMetaData::CheckLoad} is invoked on the meta data singleton
     * of the enum type \p{TEnum}. Then, meta data table entry corresponding to parameter
     * \p{declaration} is assigned to field #Tuple.
     *
     * After that, the following steps are performed:
     * - If tuple element \c 3 ("default value") is an empty string, the string is \e nulled.
     *   Otherwise the string is used as a resource name and the resulting default value is loaded
     *   from the resources.
     * - If tuple element \c 7 ("comment") is an empty string, the string is \e nulled. Otherwise
     *   the string is used as a resource name and the resulting comment string is loaded from
     *   the resources
     *
     * For both load operations, the same module and resource category as for the enum meta
     * data itself is used.
     *
     * \see Description of this class and macro \ref ALIB_CONFIG_VARIABLES.
     *
     * @param declaration    Element of an enum class that is representing configuration
     *                       variables.
     * @tparam TEnum         The type of parameter \p{declaration}
     * @tparam TEnableIf     Not to be specified. Used by the compiler to select this constructor
     *                       only for associated custom C++ enum types.
     **********************************************************************************************/
    template<typename TEnum, typename TEnableIf= void>
    VariableDecl( TEnum declaration );
#else
    template<typename TEnum,typename TEnableIf=
    ATMP_VOID_IF(ATMP_EQ(VariableDecl::TTuple, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
    VariableDecl( TEnum declaration )
    {
        // copy our data tuple from the enum meta data table entry
        auto& enumMetaData= EnumMetaData<TEnum>::GetSingleton();
        enumMetaData.CheckLoad();
        Tuple= *enumMetaData.Get( declaration );

        // if element 3 (default value) or 7 (comment) is empty, set to nullptr, else use as
        // resource name and load
        for(int defvalOrComment= 0; defvalOrComment<2 ; ++defvalOrComment )
        {
            String& value= defvalOrComment == 0 ? std::get<3>(Tuple)
                                                : std::get<7>(Tuple);
            if( value.IsEmpty() )
                value= NullString();
            else
            {
                ALIB_STRINGS_TO_NARROW(value, resName, 64)
                value= aworx::lib::resources::T_Resourced<TEnum>::Resource()->Get(
                       aworx::lib::resources::T_Resourced<TEnum>::Category(),
                       resName
                       ALIB_DBG(, true) );
            }
        }
    }
#endif


    /**
     * The value for field \alib{config,Variable::Category}.
     * @returns The category string of the variable.
     */
    String      Category()                                const     {  return std::get<1>( Tuple ); }

    /**
     * The value for field \alib{config,Variable::Name}.
     * @returns The name string of the variable.
     */
    String      Name()                                    const     {  return std::get<2>( Tuple ); }

    /**
     * The value for field \alib{config,Variable::DefaultValue}.
     * @returns The default value of the variable.
     */
    String      DefaultValue()                            const     {  return std::get<3>( Tuple ); }

    /**
     * The value for field \alib{config,Variable::Delim}.
     * @returns The delimiter character of the variable.
     */
    character   Delim()                                   const     {  return std::get<4>( Tuple ); }

    /**
     * The value for field \alib{config,Variable::FormatAttrAlignment}.
     * @returns The alignment identifier string of the variable.
     */
    String      FormatAttrAlignment()                     const     {  return std::get<5>( Tuple ); }

    /**
     * The value for field \alib{config,Variable::FmtHints}.
     * @returns The format hint bits of the variable.
     */
    FormatHints FmtHints()                                const     {  return FormatHints(std::get<6>( Tuple )); }

    /**
     * The value for field \alib{config,Variable::Comments}.
     * @returns The comment string of the variable.
     */
    String      Comments()                                const     {  return std::get<7>( Tuple ); }


}; // struct VariableDecl


}} // namespace aworx::[lib::config]

/// Type alias in namespace #aworx.
using     VariableDecl=     aworx::lib::config::VariableDecl;

/// Type alias in namespace #aworx.
using     Priorities=       aworx::lib::config::Priorities;

} // namespace [aworx]

// #################################################################################################
// Macros
// #################################################################################################
#define  ALIB_CONFIG_VARIABLES( TEnum, TModule, ResourceName )                                     \
ALIB_RESOURCED_IN_MODULE( TEnum, TModule, ResourceName)                                            \
ALIB_ENUM_SPECIFICATION(  aworx::lib::config::VariableDecl,  TEnum)                                \


ALIB_BOXING_VTABLE_DECLARE( aworx::lib::config::Priorities, vt_config_priorities )
ALIB_BOXING_VTABLE_DECLARE( aworx::lib::config::Exceptions, vt_config_exceptions )
ALIB_EXCEPTIONS(            aworx::lib::config::Exceptions, aworx::lib::CONFIG, "Exceptions" )
ALIB_ENUM_IS_ARITHMETICAL(  aworx::lib::config::Priorities  )
ALIB_RESOURCED_IN_MODULE(   aworx::lib::config::Priorities, aworx::lib::CONFIG, "Priorities" )

ALIB_ENUM_PARSABLE(         aworx::lib::config::Priorities  )
ALIB_ENUM_IS_BITWISE(       aworx::lib::config::FormatHints )



#endif // HPP_ALIB_CONFIG_CONFIG

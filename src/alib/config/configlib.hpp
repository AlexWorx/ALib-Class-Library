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
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_CONFIG_LIB)
    #error "Header already included"
#endif

// then, set include guard
#ifndef HPP_ALIB_CONFIG_LIB
//! @cond NO_DOX
#define HPP_ALIB_CONFIG_LIB 1
//! @endcond

#if !defined (HPP_ALIB_LANG_EXCEPTION)
    #include "alib/lang/exception.hpp"
#endif

#if !defined (HPP_ALIB_LANG_ENUM_ARITHMETICAL)
    #include "alib/lang/enumarithmetical.hpp"
#endif


namespace aworx { namespace lib { namespace config {


/** ************************************************************************************************
 * Exception codes of namespace #aworx::lib::config.
 **************************************************************************************************/
enum class Exceptions
{
    /// File not found when reading.
    ErrorOpeningFile= 1,

    /// An error occurred writing the file .
    ErrorWritingFile= 2,
};

/** ************************************************************************************************
 * \alib{lang,T_EnumIsArithmetical,Arithmetical enumeration} used to prioritize
 * of type \alib{config,ConfigurationPlugin} objects attached to instances of
 * class \alib{config,Configuration}.
 **************************************************************************************************/
enum class Priorities
{
    /// Used to denote that no variable or plug-in was found.
    NONE                    = 0,

    /**
     * Constant providing a priority which is even lower than default. This priority is not used by
     * any standard plug-ins and currently not even anywhere in core \alib. The rational to
     * provide it is for third party libraries that may use this value in cases where values are
     * estimated or detected instead of defaulted.<br>
     * A user of the library may then overwrite such auto-detection by setting a default value in
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
 * The library class for namespace \ref aworx::lib::config.
 **************************************************************************************************/
class Config  : public lang::Library
{
    // #############################################################################################
    // internals
    // #############################################################################################
    protected:

    // #############################################################################################
    // Constructor
    // #############################################################################################
    public:
        inline  Config()
        : lang::Library( ALIB_VERSION, ALIB_REVISION, ASTR("ALIB_CONFIG") )
        {}


    // #############################################################################################
    // Interface methods
    // #############################################################################################
    public:


    // #############################################################################################
    // Internal methods
    // #############################################################################################
    protected:
        /** ****************************************************************************************
         * Initializes the \alib \ref aworx#lib#config namespace.
         *
         * In phase \alib{lang,Library::Phases::final} performs:
         * - Adds default resources for enum types
         *   - \alib{config,Exceptions} and
         *   - \alib{config,Priorities}.
         * - Adds boxing interface \alib{strings::boxing,IApply_TApplicable} for type
         *   \alib{config,Exceptions}.
         *
         * @param phase  The initialization phase to perform.
         ******************************************************************************************/
        virtual void        init( Phases phase )                                           override;

        /** ****************************************************************************************
         * Frees resources of the \alib \c string namespace.
         ******************************************************************************************/
        virtual void        terminationCleanUp()                                           override;

}; // class Config

//! @cond NO_DOX

struct VariableDecl;
}}} // namespace [aworx::lib::config]
ALIB_ENUM_SPECIFICATION_DECL( aworx::lib::config::VariableDecl,
                              String, String, String, character, String, int, String )
namespace aworx { namespace lib { namespace config {

//! @endcond


/** ************************************************************************************************
 * A data record used to declare an
 * \ref aworx::lib::config::Variable "ALib Configuration Variable".
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
 * dependent on runtime information.
 *
 * Besides the use for passing parameters into objects of type Variable, this struct does not
 * provide further logic.
 *
 * In this C++ implementation of \alib, this class can be created from elements of enum types.
 * For this, the concept of \alib{lang,T_EnumMetaDataDecl,enum meta data} is used. The meta data
 * of applicable enumeration types has to follow a certain tuple type, specified with #TTuple.
 * Macro \ref ALIB_CONFIG_VARIABLES implements this association and is to be used to make a C++ enum
 * type compatible with the constructor of this struct. The macro in addition enables the enum
 * meta data to be loaded from the \alib{lang,Resources} of a \alib{lang,Library}.
 **************************************************************************************************/
struct VariableDecl
{
    /**
     * The type of the tuple of \alib{lang,T_EnumMetaDataDecl,enum meta data} used with
     * enumerations that denote configuration variables.
     *
     * The elements of the tuple have the following meaning:
     *
     * <center>Index</center> | <center>Description</center>
     * - - - - - - - - - - - -| - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     *  0 | The originating enum value of the element of the custom C++ enumeration type.
     *  1 | Value for field \alib{config,Variable::Category}.
     *  2 | Value for field \alib{config,Variable::Name}.
     *  3 | Value for field \alib{config,Variable::DefaultValue}.
     *  4 | Value for field \alib{config,Variable::Delim}.
     *  5 | Value for field \alib{config,Variable::FormatAttrAlignment}.
     *  6 | Value for field \alib{config,Variable::FmtHints}.
     *  7 | Value for field \alib{config,Variable::Comments}.
     *
     * \see Macro \ref ALIB_CONFIG_VARIABLES.
     */
     using TTuple= lang::T_EnumMetaDataSpecification<VariableDecl>::TTuple;


    /// The variable declaration data.
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
                  character         delim,
                  String        formatAttrAlignment,
                  FormatHints   formatHints,
                  String        comments                )
    : Tuple( std::make_tuple( -1, category, name, defaultValue,
                              delim, formatAttrAlignment, EnumValue(formatHints), comments ) )
    {}


    /** ********************************************************************************************
     * Constructor that accepts an element of a C++ enum type equipped with meta information
     * that contains the declaration data.
     *
     * First, method \alib{lang,EnumMetaData::CheckLoad} is invoked on the meta data singleton
     * of the enum type \p{TEnum}. Then, meta data table entry corresponding to parameter
     * \p{declaration} is assigned to field #Tuple.
     *
     * After that, the following steps are performed:
     * - If tuple element \c 3 ("default value") is an empty string, the string is nulled. Otherwise
     *   the string is used as a resource name and the resulting default value is loaded from
     *   the resources.
     * - If tuple element \c 7 ("comment") is an empty string, the string is nulled. Otherwise
     *   the string is used as a resource name and the resulting comment string is loaded from
     *   the resources
     *
     * For both load operations, the the same library and resource category as for the enum meta
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
    template<typename TEnum,
             typename TEnableIf= typename std::enable_if<
                        std::is_same< VariableDecl::TTuple, typename lang::T_EnumMetaDataDecl<TEnum>::TTuple >::value
                                                         >::type>
    VariableDecl( TEnum declaration )
    {
        // copy our data tuple from the enum meta data table entry
        auto enumMetaData= EnumMetaData<TEnum>::GetSingleton();
        enumMetaData->CheckLoad();
        Tuple= *enumMetaData->Get( declaration );

        // if element 3 (default value) or 7 (comment) is empty, set to nullptr, else use as
        // resource name and load
        for(int i= 0; i<2 ; ++i )
        {
            String& value= i == 0 ? std::get<3>(Tuple) : std::get<7>(Tuple);
            if( value.IsEmpty() )
                value= NullString;
            else
                value= lang::T_Resourced<TEnum>::Lib().Get( value  );
        }
    }


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


} // namespace aworx::lib[::config]

/** ************************************************************************************************
 * The static library singleton of namespace #aworx::lib::config.
 **************************************************************************************************/
ALIB_API
extern config::Config CONFIG;

} // namespace aworx[::lib]

/// Type alias in namespace #aworx.
using     VariableDecl=     aworx::lib::config::VariableDecl;

/// Type alias in namespace #aworx.
using     Config=           aworx::lib::config::Config;

/// Type alias in namespace #aworx.
using     Priorities=       aworx::lib::config::Priorities;

} // namespace [aworx]

// #################################################################################################
// Macros (for technical reasons, doxed in file alib.cpp)
// #################################################################################################
#define  ALIB_CONFIG_VARIABLES( TEnum, ResourceLibrary, ResourceName )                             \
ALIB_ENUM_SPECIFICATION(  aworx::lib::config::VariableDecl,                                        \
                                TEnum, ResourceLibrary, ResourceName )                             \



ALIB_LANG_EXCEPTIONS(  aworx::lib::config::Exceptions, lib::CONFIG, ASTR("Exceptions") )

ALIB_LANG_ENUM_IS_ARITHMETICAL( aworx::lib::config::Priorities )
ALIB_LANG_ENUM_PARSABLE(        aworx::lib::config::Priorities )

ALIB_LANG_ENUM_IS_BITWISE( aworx::lib::config::FormatHints )



#endif // HPP_ALIB_CONFIG_LIB

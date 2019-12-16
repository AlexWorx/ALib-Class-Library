/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2019 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_CONFIG_VARIABLEDECL
#define HPP_ALIB_CONFIG_VARIABLEDECL 1

#if !defined (HPP_ALIB_CONFIG_PRIORITIES)
    #include "alib/config/priorities.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_BITWISE)
#   include "alib/enums/bitwise.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

namespace aworx { namespace lib { namespace config {

// Forward declarations
struct VariableDecl;

/** ************************************************************************************************
 * Denotes hints for formatting variables when storing in external configuration files
 **************************************************************************************************/
enum class FormatHints
{
    /* No hints **/
    None                    = 0      ,

    /* Write each argument in a new line **/
    MultiLine               = 1 <<  0,

    /* Suppress spaces around the delimiter (only used in single line mode) **/
    NoDelimSpaces           = 1 <<  1,

    /* This and upward bits are reserved for custom plug-ins **/
    Custom                  = 1 <<  16,
};


/** ************************************************************************************************
 * A data record used to declare an \alib{config,Variable,ALib Configuration Variable}.
 *
 * All fields (except #Delim) support placeholders \c "%1", \c "%2" ... \c "%N", which are
 * replaced with the constructor of class \b %Variable and method
 * \ref aworx::lib::config::Variable::Declare "Variable::Declare".
 * This allows to define a series of variables whose category, name, description and value is
 * dependent on run-time information.
 *
 * Objects of class \b %Variable can be constructed and existing objects can be reused by invoking
 * \ref aworx::lib::config::Variable::Declare "Variable::Declare". Both, construction and
 * re-declaration of variables, use objects of this type.
 *
 * While variables can also be declared by setting their attributes "manually", it is recommended
 * to declare all external configuration variables in a central place, this type.<br>
 *
 * In this C++ implementation of \alib, this class can be created from elements of enum types.
 * For this, the concept of \ref alib_enums_records "ALib Enum Records" is used.
 * The record associated with an enum element used for construction must be of this type itself!
 * Consequently, equipping enum type \b MyType is performed like this:
 *
 *      ALIB_ENUMS_ASSIGN_RECORD( MyEnum, aworx::lib::config::VariableDecl )
 *
 * Besides defining the enum record type, a custom enum has to have a specialization of
 * type \alib{resources,T_Resourced}. The reason for this is that enum records of this type
 * use field #DefaultValue and #Comments <em>indirectly</em>, namely just specifying a resource
 * name. This way, both values are loaded from separated resource strings, what has the following
 * advantages:
 * - The values may contain the separation character used.
 * - The values can be manipulated within the (externalized) resources more easily.
 *
 * With this - and the corresponding resource data! - in place, elements of custom enum type can be
 * used to declare configuration variables by passing them to one of following constructors and
 * methods:
 * - \alib{config,VariableDecl::VariableDecl(TEnum)}
 * - \alib{config,Variable::Variable(TEnum)}
 * - \alib{config,Variable::Variable(TEnum),Variable::Variable(TEnum\,const StringTypes&...)}
 * - \alib{config,Variable::Declare(TEnum)}
 * - \alib{config,Variable::Declare(TEnum),Variable::Declare(TEnum\,const StringTypes&...)}
 *
 * The resource data has to provide eight values in the following order:
 * 1. The custom integral enum value (this is mandatory with every resourced enum record).
 * 2. Field #Category.
 * 3. Base class's field \alib{enums,ERSerializable::EnumElementName}.
 *    \note Field \alib{enums,ERSerializable::MinimumRecognitionLength} is not read from the string,
 *          but set to fixed value \c 0.
 * 4. Field #DefaultValue.
 * 5. Field #Delim.
 * 6. Field #FormatAttrAlignment.
 * 7. Field #FmtHints.
 * 8. Field #Comments.
 **************************************************************************************************/
struct VariableDecl : public enums::ERSerializable
{
    /** Defaulted constructor leaving the declaration undefined.          */
    VariableDecl()                                                              noexcept  = default;

    /** The value for field \alib{config,Variable::Category}.             */
    String      Category;

    /** The value for field \alib{config,Variable::DefaultValue}.
     *  \note
     *    If TMP struct \alib{resources,T_Resourced} is specialized for an enumeration,
     *    this field is interpreted as a resource name to load the description from.     */
    String      DefaultValue;

    /** The value for field \alib{config,Variable::Delim}.                */
    character   Delim;

    /** The value for field \alib{config,Variable::FormatAttrAlignment}.  */
    String      FormatAttrAlignment;

    /** The value for field \alib{config,Variable::FmtHints}.             */
    FormatHints FmtHints;

    /** The value for field \alib{config,Variable::Comments}.
     *  \note
     *    If TMP struct \alib{resources,T_Resourced} is specialized for an enumeration,
     *    this field is interpreted as a resource name to load the description from.     */
    String      Comments;

    /** ********************************************************************************************
     * Constructor usually used with static variable declarations (declarations that are not
     * using enumeration types associated with \ref alib_enums_records "ALib Enum Records" of this
     * type).
     *
     * If used however to to define an enum record during bootstrap of a software (by user code
     * that omits the preferred option of parsing resourced strings to create such records), then
     * each parameter of type \b String passed, has to be of "static nature".
     * This means, that string buffers and their contents are deemed to survive the life-cycle of
     * an application. Usually, C++ string literals are passed in such situation.
     *
     * @param category            Value for field \alib{config,Variable::Category}.
     * @param name                Value for field \alib{config,Variable::Name}.
     * @param defaultValue        Value for field \alib{config,Variable::DefaultValue}.
     * @param delim               Value for field \alib{config,Variable::Delim}.
     * @param formatAttrAlignment Value for field \alib{config,Variable::FormatAttrAlignment}.
     * @param formatHints         Value for field \alib{config,Variable::FmtHints}.
     * @param comments            Value for field \alib{config,Variable::Comments}.
     **********************************************************************************************/
    VariableDecl( const String&  category,
                  const String&  name,
                  const String&  defaultValue,
                  character      delim,
                  const String&  formatAttrAlignment,
                  FormatHints    formatHints,
                  const String&  comments            )
    : ERSerializable            (name               )
    , Category           (category           )
    , DefaultValue       (defaultValue       )
    , Delim              (delim              )
    , FormatAttrAlignment(formatAttrAlignment)
    , FmtHints           (formatHints        )
    , Comments           (comments           )
    {}

    /**
     * Implementation of \alib{enums,EnumRecordPrototype::Parse}.
     * \note Field \alib{enums,ERSerializable::MinimumRecognitionLength} is not read from the string,
     *       but set to fixed value \c 0.
     *
     */
    ALIB_API
    void Parse();

#if defined(ALIB_DOX)
    /** ********************************************************************************************
     * Constructor that accepts an element of a C++ enum type equipped with
     * \ref alib_enums_records "ALib Enum Records" of this type (\alib{config,VariableDecl}).
     * that contains the declaration data. A copy of the enum record is created.
     *
     * In the case that a specialization of type \alib{resources,T_Resourced} exists for the given
     * enumeration type, fields #DefaultValue and #Comments are interpreted as a resource name
     * and are loaded from the resource pool specified.
     *
     * @tparam TEnum         The type of parameter \p{declaration}
     * @tparam TEnableIf     Not to be specified. Used by the compiler to select this constructor
     *                       only for associated custom C++ enum types.
     * @param declaration    Element of an enum class that is representing configuration
     *                       variables.
     **********************************************************************************************/
    template<typename TEnum, typename TEnableIf= void>
    VariableDecl( TEnum declaration );
#else
    template<typename TEnum,typename TEnableIf=
    ATMP_VOID_IF( EnumRecords<TEnum>::template AreOfType<VariableDecl>() )   >
    VariableDecl( TEnum declaration )
    {
        // copy our data tuple from the enum record
        *this=  enums::GetRecord(declaration);

        // if default value is empty, set to nullptr, else interpret as resource name and load
        if( DefaultValue.IsEmpty() )
            DefaultValue= NullString();

        else
            if ALIB_CONSTEXPR_IF( T_Resourced<TEnum>::value )
                DefaultValue= ResourcedType<TEnum>::Get( DefaultValue   ALIB_DBG(, true) );

        // if default value is empty, set to nullptr, else interpret as resource name and load
        if( Comments.IsEmpty() )
            Comments= NullString();
        else
            if ALIB_CONSTEXPR_IF( T_Resourced<TEnum>::value )
                Comments=     ResourcedType<TEnum>::Get( Comments       ALIB_DBG(, true) );
    }
#endif
}; // struct VariableDecl

}} // namespace aworx::[lib::config]

/// Type alias in namespace #aworx.
using     VariableDecl=     lib::config::VariableDecl;

} // namespace [aworx]


ALIB_ENUMS_MAKE_BITWISE( aworx::lib::config::FormatHints )


#endif // HPP_ALIB_CONFIG_VARIABLEDECL

/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_config of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
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

#if !defined (HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif

ALIB_ASSERT_MODULE(CONFIGURATION)

namespace alib {  namespace config {

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
 * \ref alib::config::Variable::Declare "Variable::Declare".
 * This allows to define a series of variables whose category, name, description and value is
 * dependent on run-time information.
 *
 * Objects of class \b %Variable can be constructed and existing objects can be reused by invoking
 * \ref alib::config::Variable::Declare "Variable::Declare". Both, construction and
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
 *      ALIB_ENUMS_ASSIGN_RECORD( MyEnum, alib::config::VariableDecl )
 *
 * Besides defining the enum record type, a custom enum has to have a specialization of
 * type \alib{lang::resources,T_Resourced}. The reason for this is that enum records of this type
 * do load fields #DefaultValue and #Comments <em>indirectly</em> from resources by
 * adding postfixes <b>_D</b>, respectively <b>_C</b> to the variable's resource name along with
 * the variable's underlying enumeration element's integral value.
 * This way, both values are loaded from separated resource strings, what has the following
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
 * The resource data has to provide six values in the following order:
 * 1. The custom integral enum value (this is mandatory with every resourced enum record).
 * 2. Field #Category.
 * 3. Base class's field \alib{enums,ERSerializable::EnumElementName}.
 *    \note Field \alib{enums,ERSerializable::MinimumRecognitionLength} is not read from the string,
 *          but set to fixed value \c 0.
 * 4. Field #Delim.
 * 5. Field #FormatAttrAlignment.
 * 6. Field #FmtHints.
 *
 * As already noted above, fields #DefaultValue and #Comments can be defined in two
 * separate resource strings named like the variable's resource itself with concatenated postfixes
 * <b>_D</b>, respectively <b>_C</b> and the variable's underlying enumeration element's integral
 * value. Both resources are optional and not mandatory to be existent.
 *
 * A sample of variable resources is given with the
 * \ref alib_ns_strings_propertyformatter_map_sample "documentation of class PropertyFormatter".
 **************************************************************************************************/
struct VariableDecl : public enums::ERSerializable
{
    /** Defaulted constructor leaving the declaration undefined.          */
    VariableDecl()                                                              noexcept  = default;

    /** The value for field \alib{config,Variable::Category}.             */
    String      Category;

    /** The value for field \alib{config,Variable::DefaultValue}.
     *  \note
     *    If TMP struct \alib{lang::resources,T_Resourced} is specialized for an enumeration,
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
     *    If TMP struct \alib{lang::resources,T_Resourced} is specialized for an enumeration,
     *    this field is interpreted as a resource name to load the description from.     */
    String      Comments;

    /** ********************************************************************************************
     * Constructor usually used with static variable declarations (declarations that are not
     * using enumeration types associated with \ref alib_enums_records "ALib Enum Records" of this
     * type).
     *
     * If used however to define an enum record during bootstrap of a software (by user code
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
     * In the case that a specialization of type \alib{lang::resources,T_Resourced} exists for the given
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

        // try to load default value and comment from resources
        if constexpr( T_Resourced<TEnum>::value )
        {
            DefaultValue = NullString();
            NString128 resName;
            resName << T_Resourced<TEnum>::Name() << "_D";
            integer codePos = resName.Length() - 1;
            resName << UnderlyingIntegral( declaration );
            DefaultValue = ResourcedType<TEnum>::Get( resName ALIB_DBG( , false ) );

            resName[codePos] = 'C';
            Comments = ResourcedType<TEnum>::Get( resName ALIB_DBG( , false ) );
        }

        // check for nulled strings
        if( DefaultValue.IsNull() ) DefaultValue= NullString();
        if( Comments    .IsNull() )     Comments= NullString();
    }
#endif
}; // struct VariableDecl

} // namespace alib::[config]

/// Type alias in namespace \b alib.
using     VariableDecl=     config::VariableDecl;

} // namespace [alib]


ALIB_ENUMS_MAKE_BITWISE( alib::config::FormatHints )


#endif // HPP_ALIB_CONFIG_VARIABLEDECL

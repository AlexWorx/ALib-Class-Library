//==================================================================================================
/// \file
/// This header file is part of module \alib_config of the \aliblong.
///
/// \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
/// Published under \ref mainpage_license "Boost Software License".
//==================================================================================================
#ifndef HPP_ALIB_CONFIG_DECLARATION
#define HPP_ALIB_CONFIG_DECLARATION 1
#pragma once
#include "alib/enums/records.hpp"
#include "alib/enums/recordparser.hpp"
#include "alib/lang/resources/resources.hpp"

ALIB_ASSERT_MODULE(CONFIGURATION)

namespace alib {  namespace config {

class Configuration;
class Variable;

namespace detail { class Entry; }

//==================================================================================================
/// \par Introduction
/// This data record is used as an alternative (and often preferred) method to declare and optionally
/// already define an \alib{config;Variable;ALib Configuration Variable}.
/// The record contains the following information:
/// - The variables' name,
/// - the variables' type name,
/// - an optional defaultValue, and
/// - optional comments.
///
/// The latter may be used by external configuration file to place human-readable comments next to
/// each configuration entry.
///
/// With a \b Declaration instance in place, constructors
/// \alib{config::Variable;Variable(Configuration& cfg, const Declaration* )}, and
/// \alib{config::Variable;Variable(lang::Camp& camp, const Declaration* )}, as well as methods
/// \alib{config::Variable;Declare(const Declaration*)}, and
/// \alib{config::Variable;Try(const Declaration*)} can be used.
///
/// Nevertheless, as seen below, a much nicer option exists.
///
/// \par Placeholders
///
/// All fields support placeholders \c "%1", \c "%2" ... \c "%N", which are replaced with optional
/// constructors of class \b %Variable, some of its methods \alib{config::Variable;Declare}
/// and with this classes' static method #Get.-
///
/// This allows defining a series of variables whose name, description, default value and
/// comments are dependent on run-time information. For example, the text logging types of
/// camp \alib_alox, have an own name. Now this name fills the placeholders within the variable
/// declarations that each logger creates. This way, one resourced declaration is copied
/// to be used by one or more loggers.
///
/// \par String Data Allocation
/// The nature of this type's contained string data is "static" and "read only". Consequently
/// this classes' members are simple \alib strings, without a own string buffer. The class relies
/// on the fact that the lifecycle of the string data is permanent, just like C++ string literals
/// that you might pass to the constructor, or like externalized resource strings, which is described
/// in the next sections, are.<br>
/// A little challenge arises when placeholders are used: Here, new string data has to be
/// allocated. For this, the convenience method \alib{config;Configuration::StoreDeclaration} is
/// available. The method not only copies the data into the internal \alib{MonoAllocator},
/// but it also checks if a corresponding replaced record was previously already stored. In this
/// case no next copy is generated, but the old one is re-used.
/// In the sample of \alox loggers above this means: The repeated creation and deletion of a
/// logger with the same name generates only one declaration.
///
/// If a programmer provides self-generated variable names in combination with declarations,
/// it has to be ensured that these variable names survive the lifecycle of the declaration.
/// \note This is not true for the variable names of declared variables. They are duly copied,
///       allocated, disposed, and recycled by class \b Configuration.
///
///
/// \par Enum Records
/// This type inherits class \alib{enums::ERSerializable} and is enabled to be read from
/// (externalized) resource data associated with C++ enumerations.
/// The record associated with an enum element used for construction must be of this type itself.
/// Consequently, equipping enum type \b MyEnum is performed like this:
///
///      ALIB_ENUMS_ASSIGN_RECORD( MyEnum, alib::config::Declaration )
///
/// (For more information, see \ref alib_enums_records "ALib Enum Records".)
///
/// \par Resourced Enums
/// Besides defining the enum record type, a custom enum has to have a specialization of
/// type \alib{lang::resources;T_Resourced}.
/// This is an exception to similar types found across \alib.
/// Usually, a C++ enum type can be equipped with  enum records and then optionally be resourced.
/// The reason for this exception is that enum records of this type do load fields #DefaultValue and
/// #Comments <em>indirectly</em> from resources by adding postfixes <b>_D</b>, respectively <b>_C</b>
/// to the variable's resource name, along with the variable's underlying enumeration element's
/// integral value.
/// This way, both values are loaded from separated resource strings, what has the following
/// advantages:
/// - The values may contain the separation character used.
/// - The values can be manipulated within the (externalized) resources more easily.
///
/// \note What was said here does not mean that \b Declaration instances have to be resourced.
///       The point is that only if declarations are to be associated with C++ enums, which
///       enumerate the available variables, then this second step of resourcing the enums has
///       to be made!
///
/// With these prerequisites in place, variables can be declared very conveniently by passing just
/// a C++ enum element of custom enum type to one of the following methods:
/// - Static method \alib{config;Declaration::Get(TEnum)}
/// - \alib{config;Variable::Variable(Configuration&, TEnum)}
/// - \alib{config;Variable::Variable(Configuration&, TEnum, const Box)}
/// - \alib{config;Variable::Variable(lang::Camp&, TEnum)}
/// - \alib{config;Variable::Variable(lang::Camp&, TEnum, const Box)}
/// - \alib{config;Variable::Declare(TEnum)}
/// - \alib{config;Variable::Declare(TEnum, const Box&)}
///
/// Furthermore, a whole set of variables can be declared by passing just the enum type itself
/// to method
/// - \alib{config;Configuration::PreloadVariables}.
///
/// This is usually done with bootstrap which makes all variables seamlessly available at least
/// with a resourced default value, or overwritten by configuration files, CLI arguments or
/// command lines. Usually the using code does not care and just accesses a variables
/// value through the enumeration.
///
/// The resource data has to provide only three values in the following order:
/// 1. The enum element value (this is mandatory with every resourced enum record).
/// 2. Base class's field \alib{enums;ERSerializable::EnumElementName}.
///    \note Field \alib{enums;ERSerializable::MinimumRecognitionLength} is not read from the string,
///          but set to fixed value \c 0.
/// 3. The typename.
///
/// As already noted above, fields #DefaultValue and #Comments can be defined in two separate
/// resource strings named like the variable's resource itself with concatenated postfixes
/// <b>_D</b>, respectively <b>_C</b> and the variable's underlying enumeration element's
/// integral value. Both resources are optional and not mandatory to be existent.
///
/// A sample of variable resources is given with the
/// \ref alib_ns_strings_propertyformatter_map_sample "documentation of class PropertyFormatter".
///
/// This makes it very convenient to have all attributes of a variable, including their name
/// managed independently from the program code and allows localizing variable names to other
/// languages. Of course, this applies to such variables only which become externalized.
/// Therefore, this library likewise allows declaring and use variables without providing
/// an instance of this type.
///
/// \par Reference Documentation
//==================================================================================================
class Declaration : public enums::ERSerializable
{
  friend class alib::config::Configuration;
  friend class alib::config::Variable;

  protected:
    /// The type of the variable. Corresponds to what is defined with macros
    /// \ref ALIB_CONFIG_VARIABLE_DEFINE_TYPE and ALIB_CONFIG_VARIABLE_REGISTER_TYPE.<br>
    /// Built-in types are listed with chapter \ref alib_config_types_builtin of the Programmer's
    /// Manual.
    String      typeName;

    /// The default value provided as an C++ string that may have to be parsed when imported into
    /// a variable definition. If set, the variable becomes automatically defined with
    /// \alib{config;Priority;Priority::DefaultValues} when declared passing this instance.
    /// @see Method #DefaultValue.
    String      defaultValue;

    /// The configuration variable's comments.
    /// \note
    ///   If TMP struct \alib{lang::resources;T_Resourced} is specialized for an enumeration,
    ///   this field is interpreted as a resource name to load the description/comments from.
    String      comments;

  public:
    /// Default constructor.
    Declaration()                                                                        = default;

    //==============================================================================================
    /// Constructor usually used with static variable declarations (declarations that are not
    /// using enumeration types associated with \ref alib_enums_records "ALib Enum Records" of this
    /// type).
    ///
    /// If used however to define an enum record during bootstrap of a software (by user code
    /// that omits the preferred option of parsing resourced strings to create such records), then
    /// each parameter of type \b String passed, has to be of "static nature".
    /// This means, that string buffers and their contents are deemed to survive the life-cycle of
    /// an application. Usually, C++ string literals are passed in such situation.
    ///
    /// @param pName         Value for field \alib{enums;ERSerializable::EnumElementName}.
    /// @param pTypeName     Value for field #typeName.
    /// @param pDefaultValue Value for field #defaultValue.
    /// @param pComments     Value for field #comments.
    //==============================================================================================
    Declaration(  const String&  pName,           const String&  pTypeName,
                  const String&  pDefaultValue,   const String&  pComments   )
    : ERSerializable(pName               )
    , typeName      (pTypeName           )
    , defaultValue  (pDefaultValue       )
    , comments      (pComments           )
    {}

    /// Implementation of \alib{enums;EnumRecordPrototype::Parse}.
    /// \note Field \alib{enums;ERSerializable::MinimumRecognitionLength} is not read from the
    ///       string, but set to fixed value \c 0.
    ALIB_API
    void Parse();

#if DOXYGEN
    //==============================================================================================
    /// Static method that accepts an element of a C++ enum type equipped with
    /// \ref alib_enums_records "ALib Enum Records" of this type, that contains the declaration data.
    ///
    /// In the case that a specialization of type \alib{lang::resources;T_Resourced} exists for the
    /// enumeration type, #DefaultValue and #Comments are interpreted as a resource name
    /// given and are loaded with this method separatedly from the resource pool.
    ///
    /// @tparam TEnum         The custom enum type that is associated with variable declarations.
    /// @tparam TEnableIf     Not to be specified. Used by the compiler to select this method
    ///                       only for associated custom C++ enum types.
    /// @param element        The desired variable (aka element of type \p{TEnum}.
    /// @return The resourced declaration associated with the given enumeration \p{element} of type
    ///         \p{TEnum}.
    //==============================================================================================
    template<typename TEnum, typename TEnableIf= void>
    static
    const Declaration* Get( TEnum element );
#else
    template<typename TEnum,typename TEnableIf=
    ATMP_VOID_IF( EnumRecords<TEnum>::template AreOfType<Declaration>() )   >
    static
    const Declaration* Get(TEnum element)
    {
        // get enum record singleton
        const Declaration* result=  &enums::GetRecord(element);

        // if resourced, check for default value and comments
        if constexpr( T_Resourced<TEnum>::value )
        {
            NString128 resName;
            resName << T_Resourced<TEnum>::Name() << "_D";
            integer codePos = resName.Length() - 1;

            // note: we need to do cast the const away. This is OK. We just did not have the
            //       possibility to get the resourced values right away with the record.
            resName << UnderlyingIntegral( element );
            const_cast<Declaration*>(result)->defaultValue = ResourcedType<TEnum>::Get( resName ALIB_DBG( , false ) );

            resName[codePos] = 'C';
            const_cast<Declaration*>(result)->comments = ResourcedType<TEnum>::Get( resName ALIB_DBG( , false ) );
        }

        return result;
    }
#endif
    /// Returns this configuration variable's type.
    /// @return The name of this variable.
    const String&           TypeName()                            const        { return  typeName; }

    /// Returns this configuration variable's name.
    /// @return The name of this variable.
    const String&           Name()                                const { return  EnumElementName; }

    /// The default value provided as a C++ string (not escaped).
    /// If provided, the variable becomes automatically defined with
    /// \alib{config;Priority;Priority::DefaultValues}.
    ///
    /// @return The contents of field #defaultValue.
    const String&           DefaultValue()                        const    { return  defaultValue; }

    /// Returns this configuration variable's comments.
    /// @return The comments of this variable.
    const String&           Comments()                            const        { return  comments; }

}; // struct Declaration

} // namespace alib::[config]

/// Type alias in namespace \b alib.
using     Declaration=     config::Declaration;

} // namespace [alib]


#endif // HPP_ALIB_CONFIG_DECLARATION


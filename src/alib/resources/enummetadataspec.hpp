// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESOURCES_ENUM_META_DATA_SPECIFICATION
#define HPP_ALIB_RESOURCES_ENUM_META_DATA_SPECIFICATION 1

#if !defined(HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif


#if ALIB_FILESET_MODULES && !defined(HPP_ALIB_LIB_MODULE)
#   include "alib/lib/module.hpp"
#endif


namespace aworx { namespace lib { namespace resources {

/**
 * \note
 *   It is important to have a basic understanding of the concept of
 *   \alib{resources,T_EnumMetaDataDecl,ALib enum meta data} to understand the use case of this type.
 *
 \~Comment ####################################################################################### \~
 * ## Introduction ##
 *
 * This TMP struct is used to define a certain fixed type of
 * \alib{resources,T_EnumMetaDataDecl,enum meta data} and to enable a set of macros that easily
 * assign this type to a custom enum type and to implement a corresponding resource loader.
 * It could be said, that this struct provides meta information for enum meta data.
 *
 * The use case for this concept is given, when a module wants to expose information about the
 * meta data it expects from external enum types it interfaces with.
 *
 \~Comment ####################################################################################### \~
 * ## Step By Step Sample##
 *
 * The concept provided with this TMP struct and according preprocessor macros is best understood
 * by looking at a sample.
 *
 * \alib itself uses this concept with class \alib{results,Exception} (among others).
 * This class is not designed to be used as a base class for other
 * exceptions (like for example <c>std::exception</c> is). Instead, it accepts elements of
 * arbitrary enum types in its constructor. From the enum meta data associated with the
 * enum element, a (first) \alib{results,Message,exception message entry} is created.
 *
 * Then, while the stack is unwinded, catch blocks are allowed to add further entries
 * and then rethrow the object. This way, each entry is created from an element of a custom enum
 * type and those types may even be different in each entry!
 *
 * The only restriction that the "usable enum types" need to fulfill is that a specific type
 * of meta data needs to be associated to them. Such meta data includes for example a format
 * string which is used when the exception is \alox{LogTools,Exception,logged}.
 *
 * It is easily understood that \alib exceptions meet exactly the use case of the concept
 * implemented with this TMP struct and the according macros.
 *
 * There are two procedures that need to be followed to use this concept. One for the module
 * implementation, another for a user of the module that wants to enable custom enum types to
 * become usable with the module.
 *
 * <br>
 * ### What A %Module Needs To Do ###
 *
 * 1. Provide types (usually empty structs named in capital case letters) that denote the
 *    specifications of the enum meta data for each enum type accepted.
 *
 *    In our sample, only one type of meta data is needed. Therefore, the exception class itself
 *    is used. We simply do a forward declaration at the start of the exception's header file:
 *    \snippet "exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType
 *
 * 2. Use macro \ref ALIB_ENUM_SPECIFICATION_DECL in a header file to specialize this TMP struct
 *    (\b %T_EnumMetaDataSpecification).
 *
 *    Class \b %Exception expects two strings, one holding the enum element name and a second
 *    holding the resource name of the format string used with method \alib{results,Exception::Format}.
 *
 *    \snippet "exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl
 *
 * 3. Use macro \ref ALIB_ENUM_SPECIFICATION_IMPL in a compilation unit to implement the
 *    tuple loader once. Note that the header file for the generic, templated resource loader
 *    object has to be included in the compilation unit.
 *
 *    In code file \e exception.hpp we find:
 *    \snippet "exception.cpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_impl
 *
 * 4. Optionally provide a shortcut macro to \ref ALIB_ENUM_SPECIFICATION that predefines the
 *    custom type of step 1. It is good practice to combine such custom macro with
 *    either \ref ALIB_RESOURCES_DEFINE or alternatively \ref ALIB_RESOURCED_IN_MODULE, which
 *    defines where to load the custom enum type's resources from.
 *
 *    This is done again in the header file of class Exception:
 *    \snippet "exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro
 * <p>
 *
 * With these four simple steps, class Exception is ready to accept custom enum types and use
 * their associated enum meta data. Methods and functions of a module that accept enum
 * elements of arbitrary type should check whether the type provided uses the right meta data.
 * This can be done using <c>std::enable_if</c>. To stay with our sample, let us look at
 * method \alib{results,Exception::Add} which is called by all other methods that accept
 * enum types. The class implements two versions, one for enum types that are equipped with
 * meta information and one for those that are not.
 *
 * The selection is performed as follows:
 *
 *    \snippet "exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib
 *
 * where \alib{results,Exception::TEnumMetaData} is the tuple defining the enum meta data record.
 *
 * ### What The User Of A %Module Needs To Do:###
 * With this setup of a module type, now users of the type can declare their enum types to
 * be compatible with the module.
 *
 * The only thing that the using code needs do for preparation is to use macro
 * \ref ALIB_ENUM_SPECIFICATION (or a shortcut macro, if provided by the module as shown in step
 * 4 above). This macro expect the specification type and the custom enum type.
 *
 * Prior to using the macro, the resource information of the custom enum type has to be defined
 * (either \ref ALIB_RESOURCES_DEFINE or \ref ALIB_RESOURCED_IN_MODULE).
 *
 * \anchor cpp_alib_lang_T_EnumMetaDataSpecification_exception_sample
 * To stay with the example of class \alib{results,Exception} above, let us look how the
 * \alib formatter classes throw exceptions.
 *
 * First, the scoped enum class for the exceptions is defined:
 *
 *    \snippet "stringformat/stringformat.hpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum
 *
 * Then, this enum is associated with exceptions. Of-course, we are using the shortcut macro
 * provided by \e exception.hpp, instead of \ref ALIB_ENUM_SPECIFICATION, which is integrated
 * with the shortcut:
 *
 *    \snippet "stringformat/stringformat.hpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2
 *
 * In method \alib{Module::init} of singleton class \alib{stringformat,Stringformat}, the resources for the
 * exceptions are defined. Remember: besides the underlying integer value of the enum, class
 * \b %Exception expects two strings: One with the name of the enum element, one with the
 * resource name of the format string. In addition, we provide a prefix for the output of
 * exception element names:
 *
 *    \snippet "stringformat/stringformat.cpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3
 *    \snippet "stringformat/stringformat.cpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3b
 *
 * Now, \alib exceptions can be thrown by the formatter classes using their own enum types:
 *
 *    \snippet "stringformat/formatterpythonstyle.cpp"  DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum4
 *
 * The constructor of the \b Exception object thrown, will identify that given exception type,
 * namely enumeration element <b>Exceptions::MissingClosingBracket</b>, is equipped with meta data.
 * Due to this, it will insert an additional exception message argument, prior to the three
 * arguments given in the code: The resourced format string used to format the three arguments
 * to a human readable string.
 *
 \~Comment ####################################################################################### \~
 * # Reference Documentation #
 *
 * @tparam TEMD Type provided by the module that exposes a definition of a specific
 *              enum meta data tuple type.
 */
template<typename TEMD>
struct T_EnumMetaDataSpecification
{
    /** Enum meta data tuple type. */
    using TTuple= std::tuple<void>;

    /**
     * %Enum meta data loader.
     * This method needs to be implemented for specializations of this struct.
     * The implementation should be done using macro \ref ALIB_ENUM_SPECIFICATION_IMPL.
     *
     * \see Similar motivation and implementation as with
     * \alib{resources::detail,loadStandardEnumTable}.
     *
     * @param table     The table to the enum tuple given as <c>void*</c>
     * @param resources Reference to the resources object used.
     * @param category  The resource category.
     * @param name      The resource name.
     */
    static
    void   LoadTable( void*            table,
                      Resources&       resources,
                      const NString&   category,
                      const NString&   name       );
};


}}} // namespace [aworx::lib::resources]

// #################################################################################################
// Macros
// #################################################################################################
#define ALIB_ENUM_SPECIFICATION_DECL(TSpecific, ...)                                               \
namespace aworx { namespace lib { namespace resources {                                            \
template<>  struct T_EnumMetaDataSpecification<TSpecific>                                          \
{   using TTuple= std::tuple<int,__VA_ARGS__>;                                                     \
    ALIB_API static void LoadTable( void*,Resources&,const NString&, const NString&); };}}}

#define ALIB_ENUM_SPECIFICATION_IMPL(TSpecific)                                                    \
namespace aworx { namespace lib { namespace resources {                                            \
void T_EnumMetaDataSpecification<TSpecific>::LoadTable(void* t, Resources& r,                      \
                                                       const NString& cat,const NString& name )    \
{ ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTuple>*>(t), r, cat, name); }}}}

#define ALIB_ENUM_SPECIFICATION( TSpecific, TEnum )                                                \
namespace aworx { namespace lib { namespace resources {                                            \
template<> struct T_EnumMetaDataDecl<TEnum>                                                        \
{ using  TTuple=  T_EnumMetaDataSpecification<TSpecific>::TTuple;  };}}}                           \
ALIB_ENUM_PRE_AND_POSTFIX( TEnum, A_CHAR("Prefix"), A_CHAR("Postfix") )                            \
ALIB_ENUM_READ_WRITE( TEnum, 1, 0 )                                                                \
namespace aworx { namespace lib { namespace resources {                                            \
template<>  inline void EnumMetaData<TEnum>::CheckLoad()                                           \
{ if( Table.size() == 0 )                                                                          \
  { T_EnumMetaDataSpecification<TSpecific>::LoadTable( &Table,                                     \
     *T_Resourced<TEnum>::Resource(), T_Resourced<TEnum>::Category(), T_Resourced<TEnum>::Name() );\
    CheckMap(true); }} }}}


#endif // HPP_ALIB_RESOURCES_ENUM_META_DATA_SPECIFICATION

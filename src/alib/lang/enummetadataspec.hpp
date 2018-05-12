// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen
// check for alib.hpp already there but not us
#if !defined (HPP_ALIB)
#error "include \"alib/alib.hpp\" before including this header"
#endif
#if defined(HPP_COM_ALIB_TEST_INCLUDES) && defined(HPP_ALIB_LANG_ENUM_META_DATA_SPECIFICATION)
#error "Header already included"
#endif

#ifndef HPP_ALIB_LANG_ENUM_META_DATA_SPECIFICATION
//! @cond NO_DOX
#define HPP_ALIB_LANG_ENUM_META_DATA_SPECIFICATION 1
//! @endcond


namespace aworx { namespace lib { namespace lang {

/**
 * \note
 *   It is important to have a basic understanding of the concept of
 *   \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } to understand the use case of this type.
 *
 \~Comment ####################################################################################### \~
 * ## Introduction ##
 *
 * This TMP struct is used to define a certain fixed type of
 * \alib{lang,T_EnumMetaDataDecl,enum meta data} and to enable a set of macros that easily
 * assign this type to a custom enum type and to implement a corresponding resource loader.
 * It could be said, that this struct provides meta information for enum meta data.
 *
 * The use case for this concept is given, when a library wants to expose information about the
 * meta data of enum types it expects with to interface with.
 *
 \~Comment ####################################################################################### \~
 * ## Step By Step Sample##
 *
 * The concept provided with this TMP struct and according preprocessor macros is best understood
 * by looking at a sample.
 *
 * \alib itself uses this concept with class \alib{lang,Exception} (among others).
 * This class is not not designed to be used as a base class for other
 * exceptions (like for example <c>std::exception</c> is). Instead, it accepts elements of
 * arbitrary enum types in its constructor. From the enum meta data associated with the
 * enum element, a first \alib{lang,Exception::Entry,exception entry} is created.
 *
 * Then, while the stack is unwinded, catch blocks are allowed to add further entries
 * and then rethrow the object. This way, each entry is created from an element of a custom enum
 * type and those types may even be different in each entry!
 *
 * The only restriction that the "usable enum types" need to fullfill is that a specific type
 * of meta data needs to be associated to them. Such meta data includes for example a format
 * string which is used when the exception is \alox{LogTools,Exception,logged}.
 *
 * It is easily understood that \alib exceptions meet exactly the use case of the concept
 * implemented with this TMP struct and the according macros.
 *
 * There are two procedures that need to be followed to use this concept. One for the library
 * implementation, another for a user of the library that wants to enable custom enum types to
 * become usable with the library.
 *
 * <br>
 * ### What A %Library Needs To Do ###
 *
 * 1. Provide types (usually empty structs named in capital case letters) that denotes the
 *    specifications of the enum meta data.
 *
 *    In our sample, only one type of meta data is needed. Therefore, the expression class itself
 *    is used. We simply do a forward declaration at the start of the exception's header file:
 *    \snippet "lang/exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_TheType
 *
 * 2. Use macro \ref ALIB_ENUM_SPECIFICATION_DECL in a header file to speciialize this TMP struct
 *    (\b %T_EnumMetaDataSpecification).
 *
 *    Class \b %Exception expects two strings, one holding the enum element name and a second
 *    holding a the resource name of the format string used with method \alib{lang,Exception::Describe}.
 *
 *    \snippet "lang/exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_decl
 *
 * 3. Use macro \ref ALIB_ENUM_SPECIFICATION_IMPL in a compilation unit to implement the
 *    tuple loader once. Note that the heder file for the generic, templated resource loader
 *    object has to be included in the compilation unit.
 *
 *    In code file \e exception.hpp we find:
 *    \snippet "lang/exception.cpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_impl
 *
 * 4. Optionally provide a shortcut macro to \ref ALIB_ENUM_SPECIFICATION that predefineds the
 *    custom type of step 1.
 *
 *    This is done again in the header file of class Exception:
 *    \snippet "lang/exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_macro
 * <p>
 *
 * With these four simple steps, class Exception is ready to accept custom enum types and use
 * their associated enum meta data. Methods and functions of a library that accept enum
 * elements of arbitrary type should check whether the type provided uses the right meta data.
 * This can be done using <c>std::enable_if</c>. To stay with our sample, let us look at
 * method \alib{lang,Exception::AddFromBoxes} which is called by all other methods that accept
 * enum types. Hence, it is sufficient to "block" this function for the compiler, if a
 * wrong enum type is passed:
 *
 *    \snippet "lang/exception.hpp"     DOX_ALIB_ENUM_META_DATA_SPECIFCATION_usinginlib
 *
 * <br>
 * ### What The User Of A %Library Needs To Do:###
 * With this setup of of a library type, now users of the type can declare their enum types to
 * be compatible with the library.
 *
 * The only thing that the using code needs do for preparation is to use macro
 * \ref ALIB_ENUM_SPECIFICATION (or a shortcut macro, if provided by the library) in a header
 * file providing:
 * - the specification type (a shortcut macro would omit exactly this)
 * - the custom enum type and
 * - information about the resource \alib{lang,Library} and the resource name for the custom type.
 *
 * And, of-course, the resources have to be defined.
 *
 * \anchor cpp_alib_lang_T_EnumMetaDataSpecification_exception_sample
 * To stay with the example of class \alib{lang,Exception} above, let us look how the
 * \alib formatter classes throw exceptions.
 *
 * First, the scoped enum class for the exceptions is defined:
 *
 *    \snippet "strings/format/formatter.hpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum
 *
 * Then, this enum is associated with exceptions. Of-course, we are using the shortcut macro
 * provided by \e exception.hpp, instead of \ref ALIB_ENUM_SPECIFICATION "ALIB_ENUM_SPECIFICATION":
 *
 *    \snippet "strings/format/formatter.hpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum2
 *
 * In method \alib{lang,Library::init} of <b>%ALib Module Strings</b>, the resources for the
 * exceptions are defined. Remember: besides the underlying integer value of the enum, class
 * \b %Exception expects two strings: One with the name of the enum element, one with the
 * resource name of the format string. In addition, we provide a pefix for the output of
 * exception element names:
 *
 *    \snippet "strings/stringlib.cpp"    DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum3
 *
 * Now, \alib exceptions can be thrown by the formatter classes using their own enum types:
 *
 *    \snippet "strings/format/formatterpythonstyle.cpp"  DOX_ALIB_ENUM_META_DATA_SPECIFCATION_using_enum4
 *
 \~Comment ####################################################################################### \~
 * ## Restrictions ##
 * In respect to using the "core" macros and TMP struct specializations that
 * \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } provides, there is one restriction: The
 * specific meta data table of cutom types is to be defined using the resource system provided
 * with \alib{lang,Library}.
 *
 * This restriction not a technical one, but a design decision: If also different provision methods
 * for the data were allowed, the set of macros exposed would become much more complicated.
 * With the openness of \alib resources, this should not be a problem. It is notable that
 * its features include options to:
 * - attach 3rd party resource sources and to
 * - forbid external changes of the resources if needed.
 *
 * For the same reason of simplification, macro \ref ALIB_ENUM_SPECIFICATION includes
 * macro \ref ALIB_LANG_ENUM_PRE_AND_POSTFIX providing fixed names for the resource strings,
 * namely  <c>"Prefix"</c> and <c>"Postfix"</c>.
 *
 \~Comment ####################################################################################### \~
 * # Reference Documentation #
 *
 * @tparam TEMD Type provided by the library that exposes a definition of a specifc
 *              enum meta data tuple type.
 */
template<typename TEMD>
struct T_EnumMetaDataSpecification
{
    /// Enum meta data tuple type.
    using TTuple= std::tuple<void>;

    /**
     * %Enum meta data loader.
     * This method needs to be implemented for specializations of this struct.
     * The implementation should be done using macro \ref ALIB_ENUM_SPECIFICATION_IMPL.
     *
     * \see Similar motivation and implementation as with \alib{lang::detail,loadStandardEnumTable}.
     *
     *
     * @param table     The table to the enum tuple given as <c>void*</c>
     * @param library   Reference to a library object used.
     * @param name      The resource name.
     */
    static
    void   LoadTable( void*                        table,
                      aworx::Library&              library,
                      const aworx::String&         name          );
};


}}} // namespace [aworx::lib::lang]

// #################################################################################################
// Macros
// #################################################################################################
#define ALIB_ENUM_SPECIFICATION_DECL(TSpecific, ...)                                               \
namespace aworx { namespace lib { namespace lang {                                                 \
template<>  struct T_EnumMetaDataSpecification<TSpecific>                                          \
{   using TTuple= std::tuple<int,__VA_ARGS__>;                                                     \
    ALIB_API static void LoadTable( void*,aworx::Library&,const aworx::String&); };}}}             \

#define ALIB_ENUM_SPECIFICATION_IMPL(TSpecific)                                                    \
namespace aworx { namespace lib { namespace lang {                                                 \
void T_EnumMetaDataSpecification<TSpecific>::LoadTable(void* t, Library& l,const String& n )       \
{ ResourcedTupleLoader::LoadTable( *reinterpret_cast< std::vector<TTuple>*>(t), l, n );  }}}}      \

#define ALIB_ENUM_SPECIFICATION( TSpecific, TEnum, ResourceLibrary, ResourceName )                 \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_EnumMetaDataDecl<TEnum>  :  public std::true_type                              \
{ using  TTuple=  aworx::lib::lang::T_EnumMetaDataSpecification<TSpecific>::TTuple;  };}}}         \
ALIB_LANG_RESOURCED( TEnum, ResourceLibrary, ResourceName)                                         \
ALIB_LANG_ENUM_PRE_AND_POSTFIX( TEnum, ASTR("Prefix"), ASTR("Postfix") )                           \
ALIB_LANG_ENUM_READ_WRITE( TEnum, 1, 0 )                                                           \
namespace aworx {  namespace lib { namespace lang {                                                \
template<>  inline void EnumMetaData<TEnum>::CheckLoad()                                           \
{ if( Table.size() == 0 )                                                                          \
  { aworx::lib::lang::T_EnumMetaDataSpecification<TSpecific>::LoadTable( &Table,                   \
                                                                  ResourceLibrary, ResourceName ); \
    CheckMap(true);  }}}}}                                                                         \


#endif // HPP_ALIB_LANG_ENUM_META_DATA_SPECIFICATION

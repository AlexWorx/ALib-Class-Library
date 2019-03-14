// #################################################################################################
//  ALib C++ Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2019 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
#ifndef HPP_ALIB_RESOURCES_ENUM_META_DATA
#define HPP_ALIB_RESOURCES_ENUM_META_DATA 1

#if !defined(HPP_ALIB_SINGLETONS_SINGLETON)
#   include "alib/singletons/singleton.hpp"
#endif

ALIB_ASSERT_MODULE(RESOURCES)

#if ALIB_MODULE_ENUMS
#   if !defined(HPP_ALIB_ENUMS_ENUM_BITWISE)
#      include "alib/enums/enumbitwise.hpp"
#   endif
#   ifndef HPP_ALIB_ENUMS_ENUM_VALUE
#      include "alib/enums/enumvalue.hpp"
#   endif
#endif

#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#   include "alib/strings/localstring.hpp"
#endif

#if !defined (HPP_ALIB_RESOURCES_RESOURCES)
#   include "alib/resources/resources.hpp"
#endif

#if ALIB_MODULE_RESULTS && !defined (HPP_ALIB_RESULTS_REPORT)
#   include "alib/results/report.hpp"
#endif


#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
#   include <map>
#endif



namespace aworx { namespace lib { namespace resources {

/**
 * This namespace implements internals of namespace #aworx::lib::resources.
 */
namespace detail {
/**
 * Helper function used with \alib{resources,T_EnumMetaDataDecl,ALib enum meta data} when
 * such is defined using either macro \ref ALIB_ENUM_NAMED or \ref ALIB_ENUM_PARSABLE,
 * respectively if the meta data types match the ones given with these macros.
 *
 * The function is invoked by the default implementation of \alib{resources,EnumMetaData::CheckLoad}
 * if \alib{resources,T_Resourced} is specialized for the enumeration type.
 *
 * The trick here here is to avoid template inlining and using a <c>void*</c>
 * pointer for the type of parameter \p{table}. In the implementation of this
 * function, the pointer is reinterpreted to the correct type.
 *
 * \note
 *   While this might not be considered a correct C++ code style, this approach enables
 *   \alib{resources,T_EnumMetaDataDecl,ALib Enum Meta Information} to be
 *   included at a much earlier stage of compilation as it avoids a circular dependency with
 *   \alib features for parsing string values.
 *
 * ## Availability ##
 * This class is only available if \ref alib_manual_modules_filesets "file-set" "Modules" is
 * included in the \alibdist.
 *
 * @param tupleType The either \c 2 if named tuple, or \c 3 if parsable.
 * @param table     The table to the enum tuple given as <c>void*</c>
 * @param resources Pointer to the resource object used.
 * @param category  The resource category.
 * @param name      The resource name.
 */
ALIB_API extern
void   loadStandardEnumTable( int              tupleType,
                              void*            table,
                              Resources*       resources,
                              const NString&   category,
                              const NString&   name          );
} // namespace aworx::lib[::detail]

/**
 * ## Introduction ##
 * In software, enumerations often need more static/constant information along with each
 * enumeration element. This TMP struct is used to associate meta data with the elements
 * of non-scoped or C++ 11 scoped enum types.<br>
 * The meta data is stored in a \alib{singletons,Singleton,singleton} of type
 * \alib{resources,EnumMetaData} within a \c std::vector of user defined \c std::tuple objects.
 * Besides the vector, a reference to the data sets is available through a hash map that uses
 * the underlying integer value of the elements of the enumeration as its hash key.
 *
 * While the code of this part of \alib uses template meta programming (TMP), when using the library
 * this is not noticed as the otherwise complicated parts are supported by preprocessor macros
 * which are very simple to use. The next section introduces a simple sample to implement
 * an "element name translation table". While this is a most simple form of meta data,
 * it is so frequently needed, that programming languages like C# or Java provide such table as a
 * built-in language feature!
 *
 * \~Comment #################################################################################### \~
 *
 * ## %Enum Element Name Output ##
 * One obvious use case of enum meta data is the frequent requirement of writing enum elements
 * in human readable form (e.g. log or configuration files).
 * To achieve this, a character string needs to be stored
 * for every element of the enumeration.<br>
 * For this use case, macro \ref ALIB_ENUM_NAMED does almost all that is needed.
 * Consider the following sample code:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_NORMAL
 * Macro \ref ALIB_ENUM_NAMED is used to specialize this TMP struct for scoped enum class
 * \c Fruits.
 * The macro has to be placed <b>in the header file in the default namespace</b>.
 * Then, in a non-header code unit, the meta data has to be initialized.
 * Using macro \b %ALIB_ENUM_NAMED, the tuples that represent the meta data are fixed to type:
 *
 *          std::tuple<int, aworx::String>
 * The first element of the meta data tuple always stores the enumeration element value
 * using the integer type which the enum class uses.
 * Hence, the definition of the data \alib{resources,EnumMetaData::Table,Table} uses corresponding
 * initializers:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_NORMAL_NAME_TABLE_DEFINITION
 *
 * That is all we need to add to our software to start using the meta data! The following code
 * gives a simple sample:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_NORMAL_SAMPLE
 *
 * This code produces the following output:
 *  \snippet "DOX_ALIB_ENUMS_NORMAL_OUTPUT.txt"     OUTPUT
 *
 * While custom code might also use the meta data for its own purpose, this sample uses
 * \alib built-in mechanism that become available to all enum types that specialize
 * this TMP struct and in addition type-traits struct \alib{resources,T_EnumMetaDataDeclReadWrite}.
 * The latter is also done with macro \b ALIB_ENUM_NAMED.
 *
 * The output of the name is implemented by making such enum types
 * \ref alib_strings_assembly_ttostring "appendable" to objects of type
 * \alib{strings,TAString,AString}.
 *
 * Documentation and details to this topic is found with
 * - \alib{strings,T_Append}:
 *   For general information about converting user types to \b %AStrings.
 * - \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>}:
 *   For information and code sample of appending non-bitwise enum types to \b %AStrings.
 * - \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}:
 *   For information and code sample of appending bitwise enum types to \b %AStrings.
 *
 * \note
 *   If TMP struct \alib{enums,T_EnumIsBitwise} is specialized for the enum type, a different
 *   specialization of functor \alib{strings,T_Append} is chosen. This version has a few special demands
 *   on the meta data given. Those are well documented and sampled
 *   with \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}.
 * <p>
 *
 * \note
 *    Only for completeness, it should be mentioned that in the sample above, in addition operator
 *    \alib{strings::compatibility::std,operator<<(std::ostream&\,const TAppendable&)}
 *    comes into play, which allows any \alib{strings,T_Append,appendable type} to be used with
 *    \c std::cout respectively \c std::ostream.
 *
 *
 * \~Comment #################################################################################### \~
 *
 * \anchor anchor_T_EnumMetaDataDecl_read_enum
 * ## Parsing %Enum Elements ##
 * The opposite of writing the name of an enum element in human readable form is parsing string
 * data (back) to enumeration values.
 * For this, \alib provides built-in methods
 * - \alib{strings,TSubstring::ConsumeEnum,Substring::ConsumeEnum},
 * - \alib{strings,TSubstring::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} and
 * - \alib{strings,TSubstring::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}.
 *
 * Besides what was stored as meta data in the previous section, a third tuple element
 * of type \c int is needed: This is used to optionally limit the necessary number of characters to
 * match an element name. If the table value is \c 0 or negative, all characters need to match.
 * Otherwise, abbreviations may be used, which is often helpful when writing configuration files
 * manually.
 *
 * To define the specialization of this TMP struct for enum types that support reading values,
 * instead of \ref ALIB_ENUM_NAMED, macro \ref ALIB_ENUM_PARSABLE can be used.
 * \note
 *   Like macro \ref ALIB_ENUM_NAMED,this macro also specializes
 *   \alib{resources,T_EnumMetaDataDeclReadWrite} in one step.
 *
 * As a sample we take the actual source code of built-in \alib enum \alib{Case}:
 *
 *  \snippet "alib/lib/commonenums.hpp"            DOX_ALIB_ENUMS_PARSABLE_1
 *
 * Because \alib reads all enum meta data from resource files, an additional macro,
 * \ref ALIB_RESOURCES_DEFINE is used:
 *
 *  \snippet "alib/lib/commonenumsresourced.hpp"   DOX_ALIB_ENUMS_PARSABLE_2
 *
 * This macro receives the \alib{Module} singleton to use for loading the resources along
 * with the resource identifier.
 *
 * The corresponding default resource data definition  looks like this:
 *
 * \snippet "alib/lib/alibmodules.cpp"     DOX_ALIB_ENUMS_PARSABLE_3
 *
 * Here, the minimum length to identify the elements when parsing is \c 1 for both
 * elements. The reason is that they have different starting characters, so one character is enough
 * to identify each.
 *
 * A quick sample for parsing enum class \c %Case:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_PARSABLE_SAMPLE
 *
 * produces the following output:
 *  \snippet "DOX_ALIB_ENUMS_PARSABLE_OUTPUT.txt"     OUTPUT
 *
 * \~Comment #################################################################################### \~
 * ### Sort Order of Table Entries ###
 * In more complicated cases, it might be needed change the sort order of the table: longer
 * names which include shorter ones have to be earlier in the table. The default resource
 * for meta data of \alib enum type \alib{ContainerOp} looks like this:
 *
 * \snippet "alib/lib/alibmodules.cpp"     DOX_ALIB_ENUMS_PARSABLE_4
 *
 * If in this definition, "Get" was placed before "GetCreate", the latter could never be identified,
 * because even if "Get" had a minimum length of \c 3, it would always be positively detected
 * on a string like "GetCr".
 *
 * \~Comment #################################################################################### \~
 * \anchor anchor_T_EnumMetaDataDecl_multiple_entries
 * ### Enable Multiple Keywords For Parsing ###
 * To allow multiple optional keywords for parsing enum elements, such keywords simply are
 * added as new entries to the name translation table.
 * In other words, it is allowed to have more than one table entry representing the same enum
 * element. This is again true for both, normal enum types and those that are defined
 * \alib{enums,T_EnumIsBitwise,bitwise}.
 *
 * This feature is extensively used with built-in enum \alib{Bool} as seen in the \alib source
 * code:
 *
 * \snippet "alib/lib/alibmodules.cpp"     DOX_ALIB_ENUMS_PARSABLE_MULTIPLE
 *
 * \note
 *   Only the first entry for a dedicated enum element that is found in the table is stored in
 *   hash map \alib{resources,EnumMetaData::Map}. Furthermore, with output methods
 *   \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>} and
 *   \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}, it is always the first entry found in a table
 *   that is used for writing the human readable name.
 *
 *
 * \~Comment #################################################################################### \~
 * ## Custom Meta data ##
 * The last documentation sections introduced simple but commonly used and therefore predefined
 * meta data types.
 * Now, a sample that uses of a fully customized meta data tuple is discussed.
 * The code above used macro \ref ALIB_ENUM_NAMED, which evaluates to
 *
 *      ALIB_ENUM_META_DATA(  TEnum, aworx::lib::strings::String )
 *      ALIB_ENUM_READ_WRITE( TEnum, 1, 0 )
 * respectively \ref ALIB_ENUM_PARSABLE, which evaluates to
 *
 *      ALIB_ENUM_META_DATA(  TEnum, aworx::lib::strings::String, int )
 *      ALIB_ENUM_READ_WRITE( TEnum, 1, 2 )
 *
 * This shows, that both just use a more general macro: \ref ALIB_ENUM_META_DATA.<br>
 * This takes the enumeration type as the first element, followed by a variadic argument list,
 * that is used to define the tuple elements.
 * To be precise: the tuple elements besides the first element (which as said before, has the same
 * integer type which is underlying \p{TEnum} and stores the enum element value).
 *
 * The second macro, \ref ALIB_ENUM_READ_WRITE, has three parameters
 * - The enumeration type
 * - The tuple index that denotes a textual representation of the element name
 *   (often just the same string as the C++ element identifier).
 * - The tuple index that denoting the minimum characters to read when parsing this element
 *   with methods
 *   \alib{strings,TSubstring::ConsumeEnum,ConsumeEnum},
 *   \alib{strings,TSubstring::ConsumeEnumBitwise,ConsumeEnumBitwise} or
 *   \alib{strings,TSubstring::ConsumeEnumOrBool,ConsumeEnumOrBool}
 *   (or a custom parsing method).
 *
 * Let's use these macros to set a custom type of the meta data:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM
 * Again, please note that the macros have to be placed in the header file in the global scope.
 *
 * Then, in a non-header code unit, the meta data has to be initialized:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_NAME_TABLE_DEFINITION
 *
 * While in the previous sample this is all we needed to start, there is one thing left here.
 * Usually, after changes of the data vector \alib{resources,EnumMetaData::Table}, the hash map
 * \alib{resources,EnumMetaData::Map} has to be updated.
 * This is done with method \alib{resources,EnumMetaData::CheckLoad}.
 * In the former sample this was not needed, because the values were
 * \ref alib_strings_assembly_ttostring "appended" to an \b %AString and the built-in method invokes
 * this function for convenience with every run. In the sample code below, we have to do this
 * manually.
 *
 * If the data is set with an initializer as shown here, it is good practice to invoke this function
 * for each of your enums right away after the initialization.
 *
 * So let's call it:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_SAMPLE_CHECKMAP
 *
 *  Now anywhere in the software, code like this can be performed:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_CUSTOM_SAMPLE
 *
 * \note
 *    For iteration, we need the \c std::vector of Pets. Check out
 *    \alib \alib{enums,T_EnumIsIteratable,enum iterating feature} for information on how to avoid
 *    this error prone and inefficient approach.
 *
 * This sample produces the following output:
 *  \snippet "DOX_ALIB_ENUMS_CUSTOM_OUTPUT.txt"     OUTPUT
 *
 *
 *
 * \~Comment #################################################################################### \~
 * ## Using Class %Resources For Loading %Enum Meta Data ##
 * As it was already shown in some samples above, \alib{resources,Resources,ALib Resource Data} might
 * be used to "externalize" enum meta data.
 *
 * This is supported by specializing TMP struct \alib{resources,T_Resourced} in addition
 * to specializing TMP struct \alib{resources,T_EnumMetaDataDecl}. Such specialization is easily
 * done with macros:
 * - \ref ALIB_RESOURCES_DEFINE and
 * - \ref ALIB_RESOURCED_IN_MODULE
 *
 * The latter macro, which defines the location of the resources embedded in a module object is the
 * usual and preferred way. \alib itself exclusively accesses resources through module singletons,
 * like \alib{expressions,Expressions} or \alib{lox,ALox}.
 *
 * For standard meta data types ("named" and "parsable") this is all that is needed to
 * start loading the meta data from the resources. However, with custom tuple types,
 * method \alib{resources,EnumMetaData::CheckLoad} has to specialized to perform the load.
 * With macro  \ref ALIB_ENUM_META_DATA_RESOURCE_LOADER such specialization is implemented
 * in a way that it should satisfy the most common needs: Static helper method
 * \alib{resources,ResourcedTupleLoader::LoadTable} is used to load the data.
 * As documented, this method (optionally) allows to have all data stored in one resource string
 * or to separate the data of each enum element in an own numbered resource string.
 *
 * Using the macros above, the following things have to be kept in mind:
 * - If using macro \ref ALIB_RESOURCED_IN_MODULE, the resources object usually is shared among
 *   \alibmods and probably even with 3rd-party library.
 * - Externalization allows translation of strings to different languages, as well as to
 *   change other data that is contained in the strings (and is parsed from them at run-time).
 *   This way, you as a user of \alib might translate and change many built-in features, like
 *   for example names of configuration variables.
 * - Meta data is not loaded automatically from the resources, instead this is done only
 *   with an explicit invocation of
 *   \alib{resources,EnumMetaData::CheckLoad,EnumMetaData<TEnum>::GetSingleton()->CheckLoad}.<br>
 *   Therefore, prior to accessing the data, this method has to be invoked "manually".<br>
 *   All write and parse methods built-in to \alib, invoke this method upfront.
 *
 *
 * \~Comment #################################################################################### \~
 * # Reference Documentation #
 *
 * @tparam TEnum    The enumeration type this struct applies to.
 */

template<typename TEnum, typename TCheckEnum= ATMP_VOID_IF( std::is_enum<TEnum>::value )>
struct T_EnumMetaDataDecl
{
    /**
     * The \c std::tuple type used for entries in the static meta data
     * \alib{resources,EnumMetaData::Table,Table}.
     */
    using  TTuple=   void;
};



/** ************************************************************************************************
 * This template struct implements what we call
 * \alib{resources,T_EnumMetaDataDecl,ALib enum meta data}.
 * While the TMP struct that needs to be specialized to enable enum meta data is
 * \alib{resources,T_EnumMetaDataDecl}, this template class is the interface into it: As soon as
 * \b %T_EnumMetaDataDecl is specialized for type \p{TEnum},
 * (preferably with one of the macros found in
 * \ref GrpALibMacros "Macros Supporting ALib Enum Types"), this template class is ready to be used
 * with the type.
 *
 * The class leverages \alibmod_singletons to provide the static meta data table, which is a
 * \c std::vector of \alib{resources,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple}
 * objects.
 * Besides this, a \c std::map that hashes the enumeration element values to the table entries is
 * provided.
 *
 * \note
 *   This class is not implemented to be a \alib{singletons,Singleton,strict singleton}. Therefore,
 *   an application might create different meta data tables for the same \p{TEnum} type.
 *   However, each of these tables will have the same data types attached, as those are defined
 *   with specialization of \alib{resources,T_EnumMetaDataDecl}, unless different compilation units
 *   use different specializations. In this case, the singletons must not be accessed using
 *   \alib{singletons,Singleton::GetSingleton,GetSingleton} as those are true singletons across
 *   processes.
 *
 * Finally, the class implements various interface methods to easily access the meta data of
 * enumeration elements.
 *
 * \note
 *   For technical reasons, this concept is not applicable to enum types that are defined as
 *   \c private or \c protected inner types of structs or classes.
 *
 * \see For a tutorial and source code sample see documentation of type-traits struct
 *      \alib{resources,T_EnumMetaDataDecl}.
 *
 * @tparam TEnum       The enumeration type.
 * @tparam TEnableIf   To be ignored. Used to select this struct only for enum types which have
 *                     a specialization of \alib{resources,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf=
ATMP_VOID_IF( !ATMP_EQ(void, typename T_EnumMetaDataDecl<TEnum>::TTuple ) ) >
struct EnumMetaData :  public Singleton<EnumMetaData<TEnum>>
{

    /** The type of the meta data table. */
    using  TTable=    std::vector<typename T_EnumMetaDataDecl<TEnum>::TTuple>;

    /** The meta info data table. */
    TTable                   Table;

    /** The type of the hash #Map. */
    using   TMap=    std::map<typename std::underlying_type<TEnum>::type,
                              typename T_EnumMetaDataDecl<TEnum>::TTuple&>;

    /**
     * A container maps the underlying value of elements of enum class \p{TEnum} to meta data
     * records of type \alib{resources,T_EnumMetaDataDecl::TTuple}.
     * \note After changes in \alib{resources,EnumMetaData::Table,Table}, the map has to be rebuilt
     * using \alib{resources,EnumMetaData::CheckLoad,CheckLoad}.
     */
    TMap                     Map;


    /**
     * Clears field #Map and re-inserts all elements of #Table unless both containers are
     * equally sized and \p{force} evaluates to \c false.
     *
     * If a table contains more than one entry for the same enum element, only the
     * first is inserted into the map.
     * (See \ref anchor_T_EnumMetaDataDecl_multiple_entries "Enable Multiple Keywords For Parsing"
     * for more information on duplicate table rows.)
     *
     * This method has to be invoked after changes of #Table.
     *
     * @param force If \c true, the map is rebuilt, even if it has the same size as the table.
     *              Defaults to \c false.
     */
    void    CheckMap( bool force= false )
    {
        if( force || (Map.size() != Table.size() ) )
        {
            Map.clear();
            for( auto& e : Table )
                Map.emplace( std::get<0>(e), e) ;
        }
    }

    /**
     * This default implementation just invokes #CheckMap unless:
     * - \alib{resources,T_Resourced} is specialized for \p{TEnum} and
     * - the meta data tuple scheme is either the same as defined with macro \ref ALIB_ENUM_NAMED
     *   or macro \ref ALIB_ENUM_PARSABLE.
     *
     * If this applies, the data is loaded from the resources.
     *
     * Specialized versions might load other data types from resources or otherwise fill
     * the #Table.
     *
     * If the data is provided "in code", this method does not need a specialization.
     * Instead #CheckMap has to be invoked after the insertions if it is not done by the methods
     * that use the data.
     */
    void    CheckLoad()
    {
        if( Table.size() == 0 )
        {
            if(      (   ATMP_EQ( typename T_EnumMetaDataDecl<TEnum>::TTuple,
                                  std::tuple<typename std::underlying_type<TEnum>::type ALIB_COMMA String>)
                      || ATMP_EQ( typename T_EnumMetaDataDecl<TEnum>::TTuple,
                                  std::tuple<typename std::underlying_type<TEnum>::type ALIB_COMMA String ALIB_COMMA int>) )
                  && T_Resourced<TEnum>::Resource() != nullptr
              )
                detail::loadStandardEnumTable( std::tuple_size<typename T_EnumMetaDataDecl<TEnum>::TTuple>::value,
                                               &Table,
                                               T_Resourced<TEnum>::Resource(),
                                               T_Resourced<TEnum>::Category(),
                                               T_Resourced<TEnum>::Name()           );
        }

        CheckMap();
    }

    /**
     * Searches and returns the meta data tuple for enum element \p{element}.
     * The elements of the returned tuple can be accessed using <c>std::get<idx>()</c>.
     * To receive the "original" enum element value, use static method
     * \ref Enum(const typename T_EnumMetaDataDecl<TEnum>::TTuple&)  "Enum( TTuple& )".
     *
     * In case \alib{resources,T_EnumMetaDataDeclReadWrite} is specialized for type \p{TEnum},
     * the returned tuple may be used with static method
     * \alib{resources,EnumReadWriteInfo::Name(const typename T_EnumMetaDataDecl<TEnum>::TTuple&),EnumReadWriteInfo::Name( TTuple& )}
     *
     * If no entry was found, \c nullptr is returned.
     *
     * In debug compilations, an \alib assertion is raised, in case the map is empty.
     * @param  element   The enum element to search for.
     * @return The tuple that is associated with \p{element}.
     */
    typename T_EnumMetaDataDecl<TEnum>::TTuple*  Get ( TEnum element )
    {
        #if ALIB_MODULE_RESULTS
            ALIB_ASSERT_ERROR( Map.size() > 0,
                               "No meta data for enum {}({}) available or "
                               "T_EnumInfo<TEnum>::CheckMap() not invoked.",
                               typeid(TEnum),
                               static_cast<typename std::underlying_type<TEnum>::type>( element )  )
        #else
            ALIB_ASSERT_ERROR( Map.size() > 0,
                               "No meta data for enum available or "
                               "T_EnumInfo<TEnum>::CheckMap() not invoked. Value=",
                               static_cast<typename std::underlying_type<TEnum>::type>( element )  )
        #endif
        auto it= Map.find( EnumValue(element) );
        return it != Map.end() ? &it->second : nullptr;
    }


    /**
     * Returns the enum element of \p{tuple}.
     *
     * @param  tuple   An entry of the #Table.
     * @return The enum element that is represented by \p{tuple}.
     */
    static inline
    TEnum   Enum( const typename T_EnumMetaDataDecl<TEnum>::TTuple& tuple )
    {
        return TEnum( std::get<0>(tuple) );
    }



    /**
     * Searches the meta data tuple for enum element \p{element} and returns
     * its element at index \p{TupleIdx}.<br>
     * The return type of the template function is inferred from
     * \alib{resources,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple} and \p{TupleIdx}.
     *
     * @param  element   The enum element to search for.
     * @tparam TupleIdx  The index of the tuple element to return.
     * @return The element at tuple index \p{TupleIdx} of given \p{tuple}.
     */
    template<size_t TupleIdx>
    typename std::tuple_element<TupleIdx, typename T_EnumMetaDataDecl<TEnum>::TTuple>::type
    Value   ( TEnum element )
    {
        return std::get<TupleIdx>( *Get(element) );
    }

    /**
     * Returns the enum element, which is stored in given \p{row} of the #Table.
     *
     * @param   row         The row in the name table (aka the index of the tuple in the table
     *                      vector.
     * @return The enum element stored in Table[\p{row}].
     */
    TEnum  EnumAt ( size_t row )
    {
        return TEnum( std::get<0>( Table[row] ) );
    }

    /**
     * Returns the value in column \p{TupleIdx} and row \p{row} of the #Table.<br>
     * The return type of the template function is inferred from
     * \alib{resources,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple} and
     * \p{TupleIdx}.
     *
     * @tparam  TupleIdx    The column in the name table (aka the index of the tuple element).
     * @param   row         The row in the name table (aka the index of the tuple in the table
     *                      vector.
     * @return The value in #Table at given column and row.
     */
    template<size_t TupleIdx>
    typename std::tuple_element<TupleIdx, typename T_EnumMetaDataDecl<TEnum>::TTuple>::type
                   ValueAt( size_t row )
    {
        return std::get<TupleIdx>( Table[row] );
    }


}; // struct EnumMetaData

/** ************************************************************************************************
 * This TMP struct used to enable static template type \alib{resources,EnumReadWriteInfo}.
 *
 * \see For more information see:
 *   - \alib{resources,T_EnumMetaDataDecl,ALib Enum Meta data}
 *   - Macro \ref ALIB_ENUM_READ_WRITE which specializes this struct for a given
 *     enumeration type.
 *   - Template class \alib{resources,EnumReadWriteInfo}, which becomes available when this
 *     struct is specialized.
 *   - Specializations
 *     \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>} and
 *     \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}.
 *   - Parsing methods of class \b %Substring,
 *     \alib{strings,TSubstring::ConsumeEnum,ConsumeEnum},
 *     \alib{strings,TSubstring::ConsumeEnumBitwise,ConsumeEnumBitwise} and
 *     \alib{strings,TSubstring::ConsumeEnumOrBool,ConsumeEnumOrBool}.
 *
 * @tparam TEnum        The enumeration type.
 * @tparam TEnableIf    To be ignored. Used to select this struct only for enum types which have
 *                      a specialization of \alib{resources,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf =
            ATMP_VOID_IF( !ATMP_EQ(void, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >

struct T_EnumMetaDataDeclReadWrite
{
#if ALIB_DOCUMENTATION_PARSER
    /** The index  of the name string in the enum meta data tuple. */
    static constexpr  size_t        NameIndex               = 0;

    /**
     * The tuple index in the enum meta data denoting minimum characters to read when
     * parsing this element with methods
     * \alib{strings,TSubstring::ConsumeEnum,Substring::ConsumeEnum},
     * \alib{strings,TSubstring::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} or
     * \alib{strings,TSubstring::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}
     * (or a custom parsing method).
     */
    static constexpr  size_t        MinParseLengthIndex      =  0;
#endif
};


/** ************************************************************************************************
 * This TMP struct is used in combination with template class
 * \alib{resources,EnumReadWriteInfo} and its methods
 * \alib{resources,EnumReadWriteInfo::Prefix,Prefix} and
 * \alib{resources,EnumReadWriteInfo::Prefix,Postfix}. These methods are invoked for example by
 * \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>} and
 * \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>} (or custom code) to retrieve and write
 * a prefix and postfix string around a writable enum element.
 *
 * \attention
 * In the case that
 * \alib{resources,T_Resourced} is specialized for type \p{TEnum}, methods
 * \alib{resources,EnumReadWriteInfo::Prefix} and
 * \alib{resources,EnumReadWriteInfo::Postfix} use the fields #Prefix and #Postfix of this
 * struct as a postfix that is appended to the <b>resource name</b> for the strings and loads
 * the values from the resources.
 *
 * Otherwise, the fields are directly returned.
 *
 * Macro \ref ALIB_ENUM_PRE_AND_POSTFIX is used to specializes this struct.
 *
 * @tparam TEnum        The enumeration type.
 * @tparam TEnableIf    To be ignored. Used to select this struct only for enum types which have
 *                      a specialization of \alib{resources,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf=
    ATMP_VOID_IF( !ATMP_EQ(void, typename T_EnumMetaDataDecl<TEnum>::TTuple)) >
struct T_EnumMetaDataPreAndPostFix
{
    /** The prefix string or resource name.
     *  @return The prefix string or resource name. */
    static constexpr String        Prefix()    { return NullString(); }

    /** The postfix string or resource name.
     *  @return The postfix string or resource name. */
    static constexpr String        Postfix()   { return NullString(); }
};


/** ************************************************************************************************
 * When enums are equipped with \alib{resources,T_EnumMetaDataDecl,enum meta data} and in addition
 * type-traits struct \alib{resources,T_EnumMetaDataDeclReadWrite} is specialized, the enum
 * elements become \alib{strings,T_Append,appendable} to objects of class \b %AString automatically
 * through templated struct specializations
 * \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>} and
 * \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}.<br>
 * These methods write out the name of the enum element found within the static meta data table.
 *
 * If in addition TMP struct \alib{resources,T_EnumMetaDataPreAndPostFix} is specialized for a type,
 * the output is prefixed and postfixed with what methods #Prefix and #Postfix return.
 *
 * @see Macro \ref ALIB_ENUM_PRE_AND_POSTFIX
 *
 * @tparam TEnum       The enum type to announce a name for.
 * @tparam TEnableIf   To be ignored. Used to select this struct only for enum types which have
 *                     a specialization of \alib{resources,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum, typename TEnableIf= void>
struct EnumReadWriteInfo
{
    /**
     * The prefix string written before the element name. Used with methods
     * \alib{strings::APPENDABLES,T_Append<TEnum\,character>,T_Append<TEnum\,TChar>} and
     * \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}
     * (or a custom write method).
     *
     * If \alib{resources,T_EnumMetaDataPreAndPostFix} is not specialized for \p{TEnum}, an empty
     * string is returned.<br>
     *
     * If \ref alib_manual_modules_filesets "file-set" "Modules" is included in the \alibdist and
     * \alib{resources,T_Resourced} is specialized for \p{TEnum}, then the value of
     * \alib{resources,T_EnumMetaDataPreAndPostFix::Prefix} is appended to
     * \alib{resources,T_Resourced::Name} and used as the resource name to load.
     *
     * Otherwise, \alib{resources,T_EnumMetaDataPreAndPostFix::Prefix} is returned "directly".
     *
     * @return The prefix string.
     */
    static inline String   Prefix()
    {
        if( T_EnumMetaDataPreAndPostFix<TEnum>::Prefix() == NullString() )
            return EmptyString();

        if( T_Resourced<TEnum>::Resource() != nullptr )
        {
            String prefix= T_EnumMetaDataPreAndPostFix<TEnum>::Prefix();
            if( prefix.IsEmpty() )
                return prefix;
            return T_Resourced<TEnum>::Resource()->Get( T_Resourced<TEnum>::Category(),
                                                        NString64(T_Resourced<TEnum>::Name() ) <<  prefix
                                                        ALIB_DBG(, true) );
        }

        return T_EnumMetaDataPreAndPostFix<TEnum>::Prefix()
        ;
    }

    /**
     * Same as #Prefix but for the postfix string.
     *
     * @return The postfix string.
     */
    static inline String   Postfix()
    {
        if( T_EnumMetaDataPreAndPostFix<TEnum>::Postfix() == NullString() )
            return EmptyString();

        if( T_Resourced<TEnum>::Resource() != nullptr )
        {
            String postfix= T_EnumMetaDataPreAndPostFix<TEnum>::Postfix();
            if( postfix.IsEmpty() )
                return postfix;
            return T_Resourced<TEnum>::Resource()->Get( T_Resourced<TEnum>::Category(),
                                                        NString64(T_Resourced<TEnum>::Name() ) <<  postfix
                                                        ALIB_DBG(, true)      );
        }

        return T_EnumMetaDataPreAndPostFix<TEnum>::Postfix();
    }

    /**
     * Returns the name string of given \p{tuple}.
     *
     * @param  tuple   An entry of the meta data table of type \p{TEnum}.
     * @return The string representation of the enum element as defined by \p{tuple}.
     */
    static inline
    String  Name    ( const typename T_EnumMetaDataDecl<TEnum>::TTuple& tuple )
    {
        return std::get<T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex>( tuple );
    }

    /**
     * Searches the meta data tuple for enum element \p{element} and returns
     * the name element of it.
     *
     * @param  enumMetaData     The meta data singleton
     * @param  element          The enum element to search for.
     * @return The string representation of the enum element stored in Table[\p{row}].
     */
    static inline
    String  Name    ( EnumMetaData<TEnum>& enumMetaData, TEnum element )
    {
        return std::get<T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex>( *enumMetaData.Get( element ) );
    }

    /**
     * Searches the meta data tuple for enum element \p{element} and returns
     * the name element of it.
     *
     * @param  element          The enum element to search for.
     * @return The string representation of the enum element.
     */
    static inline
    String  Name    ( TEnum element )
    {
        return Name( EnumMetaData<TEnum>::GetSingleton(), element );
    }

    /**
     * Returns the string which represents the name of the enum element, which is stored in
     * given \p{row} of the meta info table of the given static meta data singleton.
     *
     * @param   enumMetaData    The meta data singleton
     * @param   row             The row in the name table (aka the index of the tuple in the table
     *                          vector.
     * @return The string representation of the enum element stored \p{row} of the meta data
     *         table.
     */
    static inline
    String  Name    ( EnumMetaData<TEnum>& enumMetaData, size_t row )
    {
        return std::get<T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex>( enumMetaData.Table[row] );
    }


}; // struct  EnumReadWriteInfo

}} // namespace aworx[::lib::resources]

/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumMetaDataDecl=           lib::resources::T_EnumMetaDataDecl<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumMetaDataDeclReadWrite=  lib::resources::T_EnumMetaDataDeclReadWrite<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     EnumMetaData=                 lib::resources::EnumMetaData<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     EnumReadWriteInfo=            lib::resources::EnumReadWriteInfo<TEnum>;

} // namespace [aworx]


// #################################################################################################
// Helper Macros
// #################################################################################################

#define  ALIB_ENUM_READ_WRITE( TEnum, NameIdx, MinParseLengthIdx )                                 \
namespace aworx { namespace lib { namespace resources {                                            \
template<> struct T_EnumMetaDataDeclReadWrite<TEnum>                                               \
{                                                                                                  \
    static constexpr  size_t        NameIndex              = NameIdx;                              \
    static constexpr  size_t        MinParseLengthIndex    = MinParseLengthIdx;                    \
};}}}

#define  ALIB_ENUM_PRE_AND_POSTFIX( TEnum, PrefixString, PostfixString )                           \
namespace aworx { namespace lib { namespace resources {                                            \
template<> struct T_EnumMetaDataPreAndPostFix<TEnum>                                               \
{                                                                                                  \
    static constexpr  String       Prefix()               { return PrefixString;   }               \
    static constexpr  String       Postfix()              { return PostfixString;  }               \
};}}}


#define  ALIB_ENUM_META_DATA( TEnum, ... )                                                         \
namespace aworx { namespace lib { namespace resources {                                            \
template<> struct T_EnumMetaDataDecl<TEnum>  :  public std::true_type                              \
{                                                                                                  \
    using  TTuple=  std::tuple< typename std::underlying_type<TEnum>::type , __VA_ARGS__ >;        \
};}}}


#define  ALIB_ENUM_META_DATA_RESOURCE_LOADER( TEnum )                                              \
namespace aworx { namespace lib { namespace resources {                                            \
template<> void EnumMetaData<TEnum>::CheckLoad()                                                   \
{   if( Table.size() == 0 )                                                                        \
    {   ResourcedTupleLoader::LoadTable( Table,                                                    \
                                        *T_Resourced<TEnum>::Resource(),                           \
                                         T_Resourced<TEnum>::Category(),                           \
                                         T_Resourced<TEnum>::Name()        );                      \
        CheckMap(true);  }                                                                         \
};}}}

#define  ALIB_ENUM_NAMED( TEnum )                                                                  \
ALIB_ENUM_META_DATA(  TEnum, aworx::String )                                                       \
ALIB_ENUM_READ_WRITE( TEnum, 1, 0 )                                                                \

#define  ALIB_ENUM_PARSABLE( TEnum )                                                               \
ALIB_ENUM_META_DATA(  TEnum, aworx::String, int )                                                  \
ALIB_ENUM_READ_WRITE( TEnum, 1, 2 )                                                                \




// #################################################################################################
// T_Append<Enum>
// #################################################################################################
#if ALIB_MODULE_STRINGS

namespace aworx { namespace lib { namespace strings {

// We are faking specializations of T_Append for doxygen into namespace aworx::lib::strings::APPENDABLES
#if ALIB_DOCUMENTATION_PARSER
    namespace APPENDABLES {
#endif


#if ALIB_DOCUMENTATION_PARSER
/**
 * Templated specialization of functor \alib{strings,T_Append} for enum types which meet the
 * following conditions:
 * - A specialization of TMP struct \alib{resources,T_EnumMetaDataDeclReadWrite} exists whose member
 *   \alib{resources,T_EnumMetaDataDeclReadWrite::NameIndex,NameIndex} does not evalute to \c 0.
 * - \b No specialization of TMP struct \alib{enums,T_EnumIsBitwise} exists that inherits from
 *   \c std::true_type.
 *
 * (For \alib{enums,T_EnumIsBitwise,bitwise enum types} a separated
 * \alib{strings::APPENDABLES,T_Append<TEnumBitwise,character>,specialization of T_Append} is
 * provided.)<br>
 * The conditions are evaluated by the compiler using \c std::enable_if on optional template
 * parameter \p{TEnableIf} of \alib{strings,T_Append}.
 *
 * Member \b operator() writes the name of the given enumeration value to \p{target}.
 * To achieve this, the static vector of tuples provided with the specialization of TMP struct
 * \alib{resources,T_EnumMetaDataDecl} is used as an "enum element to enum name" translation table.
 *
 * As with all specializations of functor \b %T_Append, the use of it is done implicitly.
 *
 * \see
 *   - TMP struct \alib{resources,T_EnumMetaDataPreAndPostFix}.
 *   - Some sample code is given with documentation of TMP struct \alib{resources,T_EnumMetaDataDecl}.
 *
 * # Reference Documentation #
 * @tparam TEnum The object that \b AString::Append was invoked on.
 * @tparam TChar The character type of the target \b %AString.
 */
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, character>
#else
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, TChar,
    #if ALIB_MODULE_ENUMS
         ATMP_VOID_IF(    T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex !=  0
                      && !aworx::lib::enums::T_EnumIsBitwise <TEnum>::value )  >
    #else
         ATMP_VOID_IF(   T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex !=  0 ) >
    #endif
#endif
{
    /** ********************************************************************************************
     * Searches the value of \p{src} in the name table provided with field
     * \alib{resources,EnumMetaData::Table,Table} of a specialized version of that TMP struct
     * and writes it to \p{target}.
     *
     * Prior to the element name, if specialized for \p{TEnum}, a type prefix received with
     * \alib{resources,EnumReadWriteInfo::Prefix} is written, as well as the corresponding
     * postfix \alib{resources,EnumReadWriteInfo::Postfix}.
     *
     * See class documentation for more information and a sample.
     *
     * @param target The \b ASstring that \b Append was invoked on.
     * @param src    The enum value.
     **********************************************************************************************/
    inline void operator()( TAString<TChar>& target, TEnum src )
    {
        auto& enumMetaData= EnumMetaData<TEnum>::GetSingleton();
        enumMetaData.CheckLoad();
        target << EnumReadWriteInfo<TEnum>::Prefix();
        auto  entry= enumMetaData.Get( src );
        if ( entry != nullptr )
            target << EnumReadWriteInfo<TEnum>::Name( *entry );
        else
            target << EnumValue( src );

        target << EnumReadWriteInfo<TEnum>::Postfix();
    }
};


#if ALIB_MODULE_ENUMS
#if ALIB_DOCUMENTATION_PARSER
/**
 * Templated specialization of functor \alib{strings,T_Append} for enum types which meet the
 * following conditions:
 * - A specialization of TMP struct \alib{resources,T_EnumMetaDataDeclReadWrite} exists whose member
 *   \alib{resources,T_EnumMetaDataDeclReadWrite::NameIndex,NameIndex} does not evalute to \c 0.
 * - A specialization of TMP struct \alib{enums,T_EnumIsBitwise} exists that inherits from
 *   \c std::true_type.
 *
 * (For enum types that only fulfill the first requirement, a separated
 *  \alib{strings::APPENDABLES,T_Append<TEnum,character>,specialization of T_Append} is
 * provided.)<br>
 * The conditions are evaluated by the compiler using \c std::enable_if on optional template
 * parameter \p{TEnableIf} of \alib{strings,T_Append}.
 *
 * Member \b operator() writes all value names corresponding to the bits set in \p{src},
 * separated by a comma <c>','</c>.
 * To achieve this, the static vector of tuples provided with the specialization of TMP struct
 * \alib{resources,T_EnumMetaDataDecl} is used as a "enum element to enum name" translation table.
 *
 * If the enum values of the type may be become \c 0 and one entry (probably the first)
 * of the name table contains a line with \c 0 as enum element value,
 * the string of this entry is written for \c 0 values.
 *
 * Furthermore, with bitwise type enums, the name table may include enumeration values that
 * represent combinations of more than one enum bit. Such combination entries are supported
 * but have to be positioned in the table \b prior to the single bit entries!
 * If combinational entries get matched, the corresponding single entries will not be written.
 *
 * As a sample, lets consider a window manager software which has a scoped enum type representing
 * window states.
 * In the header file, where the enum type is defined, outside of
 * any namespace, macro \ref ALIB_ENUM_NAMED is used to specialize TMP
 * struct \alib{resources,T_EnumMetaDataDecl}. Because a window might have more than one state, macro
 * \ref ALIB_ENUM_IS_BITWISE is used to specialize TMP struct \alib{enums,T_EnumIsBitwise}:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_DECLARATION
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_SPECIALIZE
 *
 * A window that is both, vertically and horizontally maximized, is considered to be just
 * \e maximized. Therefore, we define the entries in \alib{resources,EnumMetaData::Table} that
 * is used as a "name translation table" as follows:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_DEFINITION
 *
 * That is all that is needed! With this setup, the following code:
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_SAMPLE
 *
 * produces this output:
 *  \snippet "DOX_ALIB_ENUMS_BITWISE_OUTPUT.txt"     OUTPUT
 *
 * @see TMP struct \alib{resources,T_EnumMetaDataPreAndPostFix}.
 *
 * # Reference Documentation #
 * @tparam TEnum The \b AString that \b Append was invoked on.
 * @tparam TChar The character type of the target \b %AString.
 */
template<typename TEnumBitwise, typename TChar>
struct  T_Append<TEnumBitwise, TChar>
#else
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, TChar, ATMP_VOID_IF(   T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex !=  0
                                            && aworx::lib::enums::T_EnumIsBitwise <TEnum>::value  )>
#endif
{
    /** ********************************************************************************************
     * Writes all enum value names corresponding to the bits set in \p{src} and the ones found
     * in entries of specialized TMP struct member \alib{resources,EnumMetaData::Table},
     * separated by a comma <c>','</c> to \p{target}.
     *
     * Prior to the element name list, if specialized for \p{TEnum}, a type prefix received with
     * \alib{resources,EnumReadWriteInfo::Prefix} is written, as well as the corresponding
     * postfix \alib{resources,EnumReadWriteInfo::Postfix}.
     *
     * See class documentation for more information and a sample.
     *
     * @param target The \b AString that \b Append was invoked on.
     * @param src    The enum value.
     **********************************************************************************************/
    inline void operator()( TAString<TChar>& target, TEnum src )
    {
        EnumMetaData<TEnum>::GetSingleton().CheckLoad();

        target << EnumReadWriteInfo<TEnum>::Prefix();

        // check what has been covered and omit double entries
        TEnum covered= TEnum(0);

        // loop over entry 2 to end, check bit
        integer len= target.Length();

        auto& enumMD= EnumMetaData<TEnum>::GetSingleton();
        for( size_t i= 0; i < enumMD.Table.size(); i++ )
        {
            TEnum tableValue= enumMD.EnumAt( i );

            // no bits are set and this entry does not contain bits, then stop here
            if( tableValue == TEnum(0) )
            {
                if( src ==  TEnum(0) )
                {
                    target << EnumReadWriteInfo<TEnum>::Name( enumMD, i );
                    target << EnumReadWriteInfo<TEnum>::Postfix();
                    return;
                }
            }
            else if( (src & tableValue) == tableValue )
            {
                // check if was covered by a previous entry
                if( EnumContains( covered, tableValue ) )
                    continue;
                covered|= tableValue;

                target << EnumReadWriteInfo<TEnum>::Name( enumMD, i ) << ',';
            }
        }
        len= target.Length() - len;

        // remove the last comma
        if( len != 0 )
            target.DeleteEnd( 1 );

        target << EnumReadWriteInfo<TEnum>::Postfix();
    }
};

#endif // ALIB_MODULE_ENUMS


#if ALIB_DOCUMENTATION_PARSER
} // namespace aworx::lib::strings[::appendables]
#endif
}}} // namespace [aworx::lib::strings]
#endif // ALIB_MODULE_STRINGS


#endif // HPP_ALIB_RESOURCES_ENUM_META_DATA

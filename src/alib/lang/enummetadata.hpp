// #################################################################################################
//  ALib - A-Worx Utility Library
//
//  Essential ALib types needed by every module
//
//  Copyright 2013-2018 A-Worx GmbH, Germany
//  Published under 'Boost Software License' (a free software license, see LICENSE.txt)
// #################################################################################################
/** @file */ // Hello Doxygen

// to preserve the right order, we are not includable directly from outside.
#if !defined(ALIB_PROPER_INCLUSION) || defined(HPP_ALIB_LANG_ENUM_META_DATA)
    #error "include alib/alib.hpp instead of this header"
#endif

#ifndef HPP_ALIB_LANG_ENUM_META_DATA
#define HPP_ALIB_LANG_ENUM_META_DATA 1

#if !defined (_GLIBCXX_MAP) && !defined(_MAP_)
    #include <map>
#endif



namespace aworx { namespace lib { namespace lang {


/**
 * Internals of namespace #aworx::lib::lang
 */
namespace detail {
/**
 * Helper function used with \alib{lang,T_EnumMetaDataDecl,ALib enum meta data } when
 * such is defined using either macro \ref ALIB_LANG_ENUM_NAMED or \ref ALIB_LANG_ENUM_PARSABLE,
 * respectively if the meta data types match the ones given with these macros.
 *
 * The function is invoked by the default implementation of \alib{lang,EnumMetaData::CheckLoad}
 * if \alib{lang,T_Resourced} is specialized for the enumeration type.
 *
 * The trick here here is to avoid template inlining and using a <c>void*</c>
 * pointer for the type of parameter \p{table}. In the implementation of this
 * function, the pointer is reinterpreted to the correct type.
 *
 * \note
 *   While this might not be considered a correct C++ code style, this approach enables
 *   \alib{lang,T_EnumMetaDataDecl,ALib Enum Meta Information} to be
 *   included at a much earlier stage of compilation as it avoids a circular dependency with
 *   \alib features for parsing string values.
 *
 *
 * @param tupleType The either \c 2 if named tuple, or \c 3 if parsable.
 * @param table     The table to the enum tuple given as <c>void*</c>
 * @param library   Reference to a library object used.
 * @param name      The resource name.
 */
ALIB_API extern
void   loadStandardEnumTable( int                          tupleType,
                              void*                        table,
                              aworx::Library&              library,
                              const aworx::String&         name          );

} // namespace aworx::lib::lang[::detail]

/**
 * ## Introduction ##
 * In software, enumerations often need more static/constant information along with each
 * enumeration element. This TMP struct is used to associate meta data with the elements
 * of non-scoped or C++ 11 scoped enum types.<br>
 * The meta data is stored in a a \alib{lang,Singleton,singleton} of type
 * \alib{lang,EnumMetaData} within a \c std::vector of user defined \c std::tuple objects.
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
 * For this use case, macro \ref ALIB_LANG_ENUM_NAMED does almost all that is needed.
 * Consider the following sample code:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_NORMAL
 * Macro \ref ALIB_LANG_ENUM_NAMED is used to specialize this TMP struct for scoped enum class
 * \c Fruits.
 * The macro has to be placed <b>in the header file in the default namespace</b>.
 * Then, in a non-header code unit, the meta data has to be initialized.
 * Using macro \b %ALIB_LANG_ENUM_NAMED, the tuples that represent the meta data are fixed to type:
 *
 *          std::tuple<int, aworx::String>
 * The first element of the meta data tuple always stores the enumeration element value
 * using the integer type which the enum class uses.
 * Hence, the definition of the data \alib{lang,EnumMetaData::Table,Table} uses corresponding
 * initializers:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_NORMAL_NAME_TABLE_DEFINITION
 *
 * That is all we need to add to our software to start using the meta data! The following code
 * gives a simple sample:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_NORMAL_SAMPLE
 *
 * This code produces the following output:
 *  \snippet "DOX_ALIB_LANG_ENUMS_NORMAL_OUTPUT.txt"     OUTPUT
 *
 * While custom code might also use the meta data for its own purpose, this sample uses
 * \alib built-in mechanism that become available to all enum types that specialize
 * this TMP struct and in addition TMP struct \alib{lang,T_EnumMetaDataDeclReadWrite}.
 * The latter is also done with macro \b ALIB_LANG_ENUM_NAMED.
 *
 * The output of the name is implemented by making such enum types "applicable" to objects of type
 * \alib{strings,AStringBase,AString}.
 *
 * Documentation and details to this topic is found with
 * - \alib{strings,T_Apply}:
 *   For general information about converting user types to \b %AStrings.
 * - \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>}:
 *   For information and code sample of applying non-bitwise enum types to \b %AStrings.
 * - \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}:
 *   For information and code sample of applying bitwise enum types to \b %AStrings.
 *
 * \note
 *   If TMP struct \alib{lang,T_EnumIsBitwise} is specialized for the enum type, a different
 *   specialization of \alib{strings,T_Apply} is chosen. This version has a few special demands
 *   on the meta data given. Those are well documented and sampled
 *   with \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}.
 * <p>
 *
 * \note
 *    Only for completeness, it should be mentioned that in the sample above, in addition operator
 *    \alib{strings::thirdparty::std,operator<<(std::ostream&\,const TApplicable&)}
 *    comes into play, which allows any \alib{strings,T_Apply,applicable type} to be used with
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
 * - \alib{strings,SubstringBase::ConsumeEnum,Substring::ConsumeEnum},
 * - \alib{strings,SubstringBase::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} and
 * - \alib{strings,SubstringBase::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}.
 *
 * Besides what was stored as meta data in the previous section, a third tuple element
 * of type \c int is needed: This is used to optionally limit the necessary number of characters to
 * match an element name. If the table value is \c 0 or negative, all characters need to match.
 * Otherwise, abbreviations may be used, which is often helpful when writing configuration files
 * manually.
 *
 * To define the specialization of this TMP struct for enum types that support reading values,
 * instead of \ref ALIB_LANG_ENUM_NAMED, macro \ref ALIB_LANG_ENUM_PARSABLE can be used.
 * \note
 *   Like macro \ref ALIB_LANG_ENUM_NAMED,this macro also specializes
 *   \alib{lang,T_EnumMetaDataDeclReadWrite} in one step.
 *
 * As a sample we take the actual source code of built-in \alib enum \alib{lang,Case}:
 *
 *  \snippet "alib/lang/enums.hpp"            DOX_ALIB_LANG_ENUMS_PARSABLE_1
 *
 * Because \alib reads all enum meta data from resource files, an additional macro,
 * \ref ALIB_LANG_RESOURCED is used:
 *
 *  \snippet "alib/lang/enummetadata.hpp"     DOX_ALIB_LANG_ENUMS_PARSABLE_2
 *
 * This macro receives the \alib{lang,Library} singleton to use for loading the resources along
 * with the resource identifier.
 *
 * The corresponding default resource data definition  looks like this:
 *
 * \snippet "alib/lang/langlib.cpp"     DOX_ALIB_LANG_ENUMS_PARSABLE_3
 *
 * Here, the minimum length to identify the elements when parsing is \c 1 for both
 * elements. The reason is that they have different starting characters, so one character is enough
 * to identify each.
 *
 * A quick sample for parsing enum class \c %Case:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_PARSABLE_SAMPLE
 *
 * produces the following output:
 *  \snippet "DOX_ALIB_LANG_ENUMS_PARSABLE_OUTPUT.txt"     OUTPUT
 *
 * \~Comment #################################################################################### \~
 * ### Sort Order of Table Entries ###
 * In more complicated cases, it might be needed change the sort order of the table: longer
 * names which include shorter ones have to be earlier in the table. The default resource
 * for meta data of \alib enum type \alib{lang,ContainerOp} looks like this:
 *
 * \snippet "alib/lang/langlib.cpp"     DOX_ALIB_LANG_ENUMS_PARSABLE_4
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
 * \alib{lang,T_EnumIsBitwise,bitwise}.
 *
 * This feature is extensively used with built-in enum \alib{lang,Bool} as seen in the \alib source
 * code:
 *
 * \snippet "alib/lang/langlib.cpp"     DOX_ALIB_LANG_ENUMS_PARSABLE_MULTIPLE
 *
 * \note
 *   Only the first entry for a dedicated enum element that is found in the table is stored in
 *   hash map \alib{lang,EnumMetaData::Map}. Furthermore, with output methods
 *   \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 *   \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}, it is always the first entry found in a table
 *   that is used for writing the human readable name.
 *
 *
 * \~Comment #################################################################################### \~
 * ## Custom Meta data ##
 * The last documentation sections introduced simple but commonly used and therefore predefined
 * meta data types.
 * Now, a sample that uses of a fully customized meta data tuple is discussed.
 * The code above used macro \ref ALIB_LANG_ENUM_NAMED, which evaluates to
 *
 *      ALIB_LANG_ENUM_META_DATA(  TEnum, aworx::lib::strings::String )
 *      ALIB_LANG_ENUM_READ_WRITE( TEnum, 1, 0 )
  * respectively \ref ALIB_LANG_ENUM_PARSABLE, which evaluates to
 *
 *      ALIB_LANG_ENUM_META_DATA(  TEnum, aworx::lib::strings::String, int )
 *      ALIB_LANG_ENUM_READ_WRITE( TEnum, 1, 2 )
 *
 * This shows, that both just use a more general macro: \ref ALIB_LANG_ENUM_META_DATA.<br>
 * This takes the enumeration type as the first element, followed by a variadic argument list,
 * that is used to define the tuple elements.
 * To be precise: the tuple elements besides the first element (which as said before, has the same
 * integer type which is underlying \p{TEnum} and stores the enum element value).
 *
 * The second macro, \ref ALIB_LANG_ENUM_READ_WRITE, has three parameters
 * - The enumeration type
 * - The tuple index that denotes a textual representation of the element name
 *   (often just the same string as the C++ element identifier).
 * - The tuple index that denoting the minimum characters to read when parsing this element
 *   with methods
 *   \alib{strings,SubstringBase::ConsumeEnum,ConsumeEnum},
 *   \alib{strings,SubstringBase::ConsumeEnumBitwise,ConsumeEnumBitwise} or
 *   \alib{strings,SubstringBase::ConsumeEnumOrBool,ConsumeEnumOrBool}
 *   (or a custom parsing method).
 *
 * Let's use these macros to set a custom type of the meta data:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_CUSTOM
 * Again, please note that the macros have to be placed in the header file in the global scope.
 *
 * Then, in a non-header code unit, the meta data has to be initialized:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_CUSTOM_NAME_TABLE_DEFINITION
 *
 * While in the previous sample this is all we needed to start, there is one thing left here.
 * Usually, after changes of the data vector \alib{lang,EnumMetaData::Table}, the hash map
 * \alib{lang,EnumMetaData::Map} has to be updated.
 * This is done with method \alib{lang,EnumMetaData::CheckLoad}.
 * In the former sample this was not needed, because
 * the values were "applied" to an \b %AString and the built-in method invokes this function
 * for convenience with every run. In the sample code below, we have to do this manually.
 *
 * If the data is set with an initializer as shown here, it is good practice to invoke this function
 * for each of your enums right away after the initialization.
 *
 * So let's call it:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE_CHECKMAP
 *
 *  Now anywhere in the software, code like this can be performed:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_CUSTOM_SAMPLE
 *
 * \note
 *    For iteration, we need the \c std::vector of Pets. Check out
 *    \alib \alib{lang,T_EnumIsIteratable,enum iterating feature} for information on how to avoid
 *    this error prone and inefficient approach.
 *
 * This sample produces the following output:
 *  \snippet "DOX_ALIB_LANG_ENUMS_CUSTOM_OUTPUT.txt"     OUTPUT
 *
 *
 *
 * \~Comment #################################################################################### \~
 * ## Using Class %Resources For Loading %Enum Meta Data ##
 * As it was already shown in some samples above, \alib{lang,Resources,ALib Resource Data} might
 * be used to "externalize" enum meta data.
 *
 * This is supported by specializing TMP struct \alib{lang,T_Resourced} in addition
 * to specializing TMP struct \alib{lang,T_EnumMetaDataDecl}. Such specialization is easily
 * done with macros:
 * - \ref ALIB_LANG_RESOURCED and
 * - \ref ALIB_LANG_RESOURCED
 *
 * For standard meta data types ("named" and "parsable") this is all that is needed to
 * start loading the meta data from the resources. However, with custom tuple types,
 * method \alib{lang,EnumMetaData::CheckLoad} has to specialized to perform the load.
 * With macro  \ref ALIB_LANG_ENUM_META_DATA_RESOURCE_LOADER such specialization is implemented
 * in a way that it should satisfy the most common needs: Static helper method
 * \alib{lang,ResourcedTupleLoader::LoadTable} is used to load the data.
 * As documented, this method (optionally) allows to have all data stored in one resource string
 * or to separate the data of each enum element in an own numbered resource string.
 *
 * Using the macros above, the following things have to be kept in mind:
 * - The macros refer to class \alib{lang,Library} which internally holds and instances of
 *   \alib{lang,Resources} and probably shares this with other libraries, like \alox.
 * - Externalization allows translation of strings to different languages, as well as to
 *   change identifiers that a library uses by default. This way, a user of \alib might
 *   translate and change all of the built-in identifier names, configuration variables,
 *   exception messages and so forth.
 * - The meta data is not loaded automatically from the resources, instead this is done only
 *   with an explicit invocation of
 *   \alib{lang,EnumMetaData::CheckLoad,EnumMetaData<TEnum>::GetSingleton()->CheckLoad}.<br>
 *   Therefore, prior to accessing the data, this method has to be invoked "manually".
 * - All write and parse methods built-in to \alib, invoke this method upfront.
 *
 *
 * \~Comment #################################################################################### \~
 * # Reference Documentation #
 *
 * @tparam TEnum    The enumeration type this struct applies to.
 */

template<typename TEnum,
         typename TCheckEnum= typename std::enable_if<std::is_enum<TEnum>::value>::type>
struct T_EnumMetaDataDecl          :  public std::false_type
{
    /**
     * The \c std::tuple type used for entries in the static meta data
     * \alib{lang,EnumMetaData::Table,Table}.
     */
    using  TTuple=   void;
};



/** ************************************************************************************************
 * This template struct implements what we call \alib{lang,T_EnumMetaDataDecl,ALib enum meta data }.
 * While the TMP struct that needs to be specialized to enable enum meta data is
 * \alib{lang,T_EnumMetaDataDecl}, this template class is the interface into it: As soon as
 * \b %T_EnumMetaDataDecl is specialized for type \p{TEnum},
 * (preferably with one of the macros found in \ref GrpALibMacros "Macros Supporting ALib Enum Types"),
 * this template class is ready to be used with the type.
 *
 * The class leverages \alib{lang,Singleton,ALib Templated Singletons} to provide the static
 * meta data table, a \c std::vector of
 * \alib{lang,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple}.
 * Besides this, a \c std::map that hashes the enumeration
 * element values to the table entries is provided.<br>
 *
 * \note
 *   This class is not implemented to be a \alib{lang,Singleton,strict singleton}. Therefore,
 *   an application might create different meta data tables for the same \p{TEnum} type.
 *   However, each of these tables will have the same data types attached, as those are defined
 *   with specialization \alib{lang,T_EnumMetaDataDecl}, unless different compilation units
 *   use different specializations. In this case, the singletons must not be accessed using
 *   \alib{lang,Singleton::GetSingleton,GetSingleton} as those are true singletons across
 *   processes.
 *
 * Finally, the class implements various interface methods to easily access the meta data of
 * enumeration elements.
 *
 * \see For a tutorial and source code sample see documentation of TMP struct
 *      \alib{lang,T_EnumMetaDataDecl}.
 *
 * @tparam TEnum        The enumeration type.
 * @tparam TEnableIf   To be ignored. Used to select this struct only for enum types which have
 *                      a specialization of \alib{lang,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum,
         typename TEnableIf = typename std::enable_if<T_EnumMetaDataDecl<TEnum>::value>::type>

struct EnumMetaData :  public lang::Singleton<EnumMetaData<TEnum>>
{

    /// The type of the meta data table.
    using  TTable=    std::vector<typename T_EnumMetaDataDecl<TEnum>::TTuple>;

    /// The meta info data table.
    TTable                   Table;

    /// The type of the hash #Map.
    using   TMap=    std::map<typename std::underlying_type<TEnum>::type,
                              typename T_EnumMetaDataDecl<TEnum>::TTuple&>;

    /**
     * A container maps the underlying value of elements of enum class \p{TEnum} to meta data
     * records of type \alib{lang,T_EnumMetaDataDecl::TTuple}.
     * \note After changes in \alib{lang,EnumMetaData::Table,Table}, the map has to be rebuilt
     * using \alib{lang,EnumMetaData::CheckLoad,CheckLoad}.
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
     * - \alib{lang,T_Resourced} is specialized for \p{TEnum} and
     * - the meta data tuple type is either the same as defined with macro \ref ALIB_LANG_ENUM_NAMED
     *   or macro \ref ALIB_LANG_ENUM_PARSABLE.
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
        ALIB_WARNINGS_ALLOW_TEMPLATE_META_PROGRAMMING
        if( Table.size() == 0 )
        {
            if(      T_Resourced<TEnum>::value
                  && (   std::is_same<typename T_EnumMetaDataDecl<TEnum>::TTuple,
                                  std::tuple<typename std::underlying_type<TEnum>::type,String>>::value
                      || std::is_same<typename T_EnumMetaDataDecl<TEnum>::TTuple,
                              std::tuple<typename std::underlying_type<TEnum>::type,String,int>>::value )
              )
                detail::loadStandardEnumTable( std::tuple_size<typename T_EnumMetaDataDecl<TEnum>::TTuple>::value,
                                               &Table,
                                               T_Resourced<TEnum>::Lib(),
                                               T_Resourced<TEnum>::Name()         );
        }

        CheckMap();
        ALIB_WARNINGS_RESTORE
    }

    /**
     * Searches and returns the meta data tuple for enum element \p{element}.
     * The elements of the returned tuple can be accessed using <c>std::get<idx>()</c>.
     * To receive the "original" enum element value, use static method
     * \ref Enum(const typename T_EnumMetaDataDecl<TEnum>::TTuple&)  "Enum( TTuple& )".
     *
     * In case \alib{lang,T_EnumMetaDataDeclReadWrite} is specialized for type \p{TEnum},
     * the returned tuple may be used with static method
     * \alib{lang,EnumReadWriteInfo::Name(const typename T_EnumMetaDataDecl<TEnum>::TTuple&),EnumReadWriteInfo::Name( TTuple& )}
     *
     * If no entry was found, \c nullptr is returned.
     *
     * In debug compilations, an \alib assertion is raised, in case the map is empty.
     * @param  element   The enum element to search for.
     * @return The tuple that is associated with \p{element}.
     */
    typename T_EnumMetaDataDecl<TEnum>::TTuple*  Get ( TEnum element )
    {
        #if ALIB_MODULE_CORE //Report
            ALIB_ASSERT_ERROR( Map.size() > 0,
                               ASTR("No meta data for enum {}({}) available or "
                                    "T_EnumInfo<TEnum>::CheckMap() not invoked." ),
                               typeid(TEnum),
                               static_cast<typename std::underlying_type<TEnum>::type>( element )  )
        #else
            ALIB_ASSERT_ERROR( Map.size() > 0,
                               ASTR("No meta data for enum available or "
                                    "T_EnumInfo<TEnum>::CheckMap() not invoked. Value=" ),
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
     * \alib{lang,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple} and \p{TupleIdx}.
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
     * \alib{lang,T_EnumMetaDataDecl::TTuple,T_EnumMetaDataDecl<TEnum>::TTuple} and \p{TupleIdx}.
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
 * This TMP struct used to enable static template type \alib{lang,EnumReadWriteInfo}.
 *
 * \see For more information see:
 *   - \alib{lang,T_EnumMetaDataDecl,ALib Enum Meta data}
 *   - Macro \ref ALIB_LANG_ENUM_READ_WRITE which specializes this struct for a given
 *     enumeration type.
 *   - Template class \alib{lang,EnumReadWriteInfo}, which becomes available when this
 *     struct is specialized.
 *   - Specializations
 *     \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 *     \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}.
 *   - Parsing methods of class \b %Substring,
 *     \alib{strings,SubstringBase::ConsumeEnum,ConsumeEnum},
 *     \alib{strings,SubstringBase::ConsumeEnumBitwise,ConsumeEnumBitwise} and
 *     \alib{strings,SubstringBase::ConsumeEnumOrBool,ConsumeEnumOrBool}.
 *
 * @tparam TEnum        The enumeration type.
 * @tparam TEnableIf    To be ignored. Used to select this struct only for enum types which have
 *                      a specialization of \alib{lang,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum,
         typename TEnableIf = typename std::enable_if<T_EnumMetaDataDecl<TEnum>::value>::type>
struct T_EnumMetaDataDeclReadWrite   :  public std::false_type
{
    /// The index  of the name string in the enum meta data tuple.
    static constexpr  size_t        NameIndex               = 0;

    /**
     * The tuple index in the enum meta data  denoting minimum characters to read when
     * parsing this element with methods
     * \alib{strings,SubstringBase::ConsumeEnum,Substring::ConsumeEnum},
     * \alib{strings,SubstringBase::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} or
     * \alib{strings,SubstringBase::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}
     * (or a custom parsing method).
     */
    static constexpr  size_t        MinParseLengthIndex      =  0;
};


/** ************************************************************************************************
 * This TMP struct is used in combination with template class
 * \alib{lang,EnumReadWriteInfo} and its methods
 * \alib{lang,EnumReadWriteInfo::Prefix,Prefix} and
 * \alib{lang,EnumReadWriteInfo::Prefix,Postfix}. These methods are invoked for example by
 * \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 * \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>} (or custom code) to retrieve and write
 * a prefix and postfix string around a writable enum element.
 *
 * \attention
 * In the case that
 * \alib{lang,T_Resourced} is specialized for type \p{TEnum}, methods
 * \alib{lang,EnumReadWriteInfo::Prefix} and
 * \alib{lang,EnumReadWriteInfo::Postfix} use the fields #Prefix and #Postfix of this
 * struct as a postfix that is appended to the <b>resource name</b> for the strings and loads
 * the values from the resources.
 *
 * Otherwise, the fields are directly returned.
 *
 * Macro \ref ALIB_LANG_ENUM_PRE_AND_POSTFIX is used to specializes this struct.
 *
 * @tparam TEnum        The enumeration type.
 * @tparam TEnableIf    To be ignored. Used to select this struct only for enum types which have
 *                      a specialization of \alib{lang,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum,
         typename TEnableIf = typename std::enable_if<T_EnumMetaDataDecl<TEnum>::value>::type>
struct T_EnumMetaDataPreAndPostFix  :  public std::false_type
{
    /** The prefix string or resource name.
     *  @return The prefix string or resource name. */
    static constexpr  String        Prefix()    { return NullString; }

    /** The postfix string or resource name.
     *  @return The postfix string or resource name. */
    static constexpr  String        Postfix()   { return NullString; }
};


/** ************************************************************************************************
 * When enums are equipped with \alib{lang,T_EnumMetaDataDecl,enum meta data} and in addition
 * TMP struct \alib{lang,T_EnumMetaDataDeclReadWrite} is specialized, the enum elements become
 * \alib{strings,T_Apply,applicable} to objects of class \b %AString automatically through
 * templated struct specializations
 * \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 * \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}.<br>
 * These methods write out the name of the enum element found within the static meta data table.
 *
 * If in addition TMP struct \alib{lang,T_EnumMetaDataPreAndPostFix} is specialized for a type,
 * the output is prefixed and postfixed with what methods #Prefix and #Postfix return.
 *
 * @see Macro \ref ALIB_LANG_ENUM_PRE_AND_POSTFIX
 *
 * @tparam TEnum       The enum type to announce a name for.
 * @tparam TEnableIf   To be ignored. Used to select this struct only for enum types which have
 *                     a specialization of \alib{lang,T_EnumMetaDataDecl} in place.
 **************************************************************************************************/
template<typename TEnum,
         typename TEnableIf = typename std::enable_if<T_EnumMetaDataDeclReadWrite<TEnum>::value>::type>
struct EnumReadWriteInfo
{
    /**
     * The prefix string written before the element name. Used with methods
     * \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}
     * (or a custom write method).
     *
     * If \alib{lang,T_EnumMetaDataPreAndPostFix} is not specialized for \p{TEnum}, an empty
     * string is returned.<br>
     * If \alib{lang,T_Resourced} is specialized for \p{TEnum}, then the value of
     * \alib{lang,T_EnumMetaDataPreAndPostFix::Prefix} is appended to \alib{lang,T_Resourced::Name}
     * and used as the resource name to load.
     *
     * Otherwise, \alib{lang,T_EnumMetaDataPreAndPostFix::Prefix} is returned "directly".
     *
     * @return The prefix string.
     */
    static inline String   Prefix()
    {
        if( !T_EnumMetaDataPreAndPostFix<TEnum>::value )
            return EmptyString;

        if( T_Resourced<TEnum>::value )
        {
            String prefix= T_EnumMetaDataPreAndPostFix<TEnum>::Prefix();
            if( prefix.IsEmpty() )
                return prefix;
            return T_Resourced<TEnum>::Lib().Get( String64(T_Resourced<TEnum>::Name() ) <<  prefix );
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
        if( !T_EnumMetaDataPreAndPostFix<TEnum>::value )
            return EmptyString;

        if( T_Resourced<TEnum>::value )
        {
            String postfix= T_EnumMetaDataPreAndPostFix<TEnum>::Postfix();
            if( postfix.IsEmpty() )
                return postfix;
            return T_Resourced<TEnum>::Lib().Get( String64(T_Resourced<TEnum>::Name() ) <<  postfix );
        }

        return T_EnumMetaDataPreAndPostFix<TEnum>::Postfix();
    }

    /**
     * Returns the name string of given \p{tuple}.
     *
     * @param  tuple   An entry of the the meta data table of type \p{TEnum}.
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
        return Name( *EnumMetaData<TEnum>::GetSingleton(), element );
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


}} // namespace aworx::[lib::lang]
/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumMetaDataDecl=           lib::lang::T_EnumMetaDataDecl<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     T_EnumMetaDataDeclReadWrite=  lib::lang::T_EnumMetaDataDeclReadWrite<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     EnumMetaData=                 lib::lang::EnumMetaData<TEnum>;

/// Type alias in namespace #aworx.
template<typename TEnum>
using     EnumReadWriteInfo=            lib::lang::EnumReadWriteInfo<TEnum>;
} // namespace [aworx]


// #################################################################################################
// Helper Macros
// #################################################################################################

/**
 * @addtogroup GrpALibMacros
 * @{
 * @name  Macros Supporting ALib Enum Types
 * The macros in this section allow to specialize TMP structs
 * \alib{lang,T_EnumMetaDataDecl},
 * \alib{lang,T_EnumIsBitwise} and
 * \alib{lang,T_EnumIsIteratable} for scoped or non-scoped enum types.
 *
 * In respect to \alib enum meta data, besides the general macro, two predefined, oftenly used
 * meta data types (tuple types) are provided: one which is just adding element names for output
 * of enum element names and a the second that also allows parsing the names.<br>
 * In addition, for both predefined meta data types a version which standardizes the externalization
 * of the data with class \alib{lang,Resources} is provided.
 *
 * \attention
 *  All macros in this section have to be placed in global scope (outside of namespaces and types).
 *
 * @{
 *
 * \def  ALIB_LANG_ENUM_META_DATA
 *   Macro used to specialize TMP struct \alib{lang,T_EnumMetaDataDecl,T_EnumMetaDataDecl<TEnum>}.
 *
 *   The type of meta data (per enumeration element)  will be set in a specialization of
 *   \alib{lang,T_EnumMetaDataDecl::TTuple} to a \c std::tuple. The first tuple element will
 *   be of the underlying integer type of \p{TEnum}. The list of types given as variadic
 *   macro arguments are appended to the tuple, starting with tuple index \c 1.
 *
 *   With the specializations in place, template class \alib{lang,EnumMetaData} becomes
 *   available for type \p{TEnum}.
 *
 *   @param TEnum             The enumeration type to define meta data for.
 *   @param ...               The types of the tuple excluding the first element of type \c int
 *                            which always is present.
 *
 *
 *
 * \def ALIB_LANG_ENUM_META_DATA_RESOURCE_LOADER
 *   This macro may be used with enum types that:
 *   - are equipped with \alib{lang,T_EnumMetaDataDecl,enum meta data}
 *     (see also macro \ref ALIB_LANG_ENUM_META_DATA)
 *   - such meta data is not of a simple type as defined with
 *     macros \ref ALIB_LANG_ENUM_NAMED or ALIB_LANG_ENUM_PARSABLE.
 *   - are resource-defined (see macro \ref ALIB_LANG_RESOURCED)
 *
 *   For enum types that meet the conditions above, this macro implements method
 *   \alib{lang,EnumMetaData::CheckLoad}. The implementation uses static method
 *   \alib{lang,ResourcedTupleLoader::LoadTable} to load meta data from \alib{lang,Resources}
 *   into \alib{lang,EnumMetaData::Table}.
 *
 *   \attention
 *     Prior to using this macro, header file <c>"alib/lang/resourcedtupleloader.hpp"</c> has
 *     to be included.<br>
 *     (One of the reasons why the load methods are implemented using macros, is to avoid including
 *     this header by default with \alib meta data feature.)
 *
 *   \note
 *     Predefined, simple tuple types as defined with macros
 *     \ref ALIB_LANG_ENUM_NAMED and \ref ALIB_LANG_ENUM_PARSABLE do not need a dedicated
 *     implementation of \alib{lang,EnumMetaData::CheckLoad}, because the default implementation
 *     is capable of loading these types from resources.
 *
 *
 *   @param TEnum     The enumeration type to define meta data for.
 *
 *
 * \def ALIB_LANG_ENUM_NAMED
 *   Shortcut to macros
 *   - \ref ALIB_LANG_ENUM_META_DATA providing just one meta info type, \c aworx::String
 *   - \ref ALIB_LANG_ENUM_READ_WRITE providing value \c 1 for parameter \p{NameIdx} and
 *     \c 0 for parameter \p{MinParseLengthIdx}.
 *
 *   This macro is useful to define a simple "name translation table" for enum types, which will
 *   be used by TMP structs
 *   \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 *   \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>} and this way allow to "apply" enum elements
 *   to instances of class \alib{strings,AStringBase,AString}.
 *
 *   @param TEnum             The enumeration type to define meta data for.
 *
 *
 * \def ALIB_LANG_ENUM_PARSABLE
 *   Shortcut to macros
 *   - \ref ALIB_LANG_ENUM_META_DATA providing two meta info type, \c aworx::String and \c int
 *   - \ref ALIB_LANG_ENUM_READ_WRITE providing value \c 1 for parameter \p{NameIdx} and
 *     \c 2 for parameter \p{MinParseLengthIdx}.
 *
 *   Same as \ref ALIB_LANG_ENUM_NAMED, this macro is useful to define a "name translation table"
 *   for enum types, which will be used by TMP structs
 *   \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 *   \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>} and this way allow to "apply" enum elements
 *   to instances of class \alib{strings,AStringBase,AString}.
 *
 *   Furthermore, with \alib{lang,T_EnumMetaDataDeclReadWrite::MinParseLengthIndex} being set to
 *   the denote tuple element \c 2  (the second custom tuple element of type \c int), methods
 *   \alib{strings,SubstringBase::ConsumeEnum,Substring::ConsumeEnum},
 *   \alib{strings,SubstringBase::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} and
 *   \alib{strings,SubstringBase::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}
 *   become available.
 *
 *   @param TEnum             The enumeration type to define meta data for.
 *
 *
 * \~Comment #####################    EnumReadWriteInfo      ############################# \~
 *
 *
 * \def  ALIB_LANG_ENUM_READ_WRITE
 *   Macro used to specialize TMP struct
 *   \alib{lang,T_EnumMetaDataDeclReadWrite,T_EnumMetaDataDeclReadWrite<TEnum>}.
 *
 *   The specialization is effective only if TMP struct
 *   \alib{lang,T_EnumMetaDataDecl,T_EnumMetaDataDecl<TEnum>} is specialized as well
 *   (see macro \ref ALIB_LANG_ENUM_META_DATA).
 *
 *   Parameter \p{NameIdx} denotes the tuple element that holds the name of each element
 *   usable for output or parsing. Consequently, this parameter must not equal \c 0
 *   which is the fixed tuple element index representing the enum element value.
 *   The value is used with methods
 *   \alib{strings::applyto,T_Apply<TEnum\,character>,T_Apply<TEnum\,TChar>} and
 *   \alib{strings::applyto,T_Apply<TEnumBitwise\,character>,T_Apply<TEnumBitwise\,TChar>}, which write enum element names into
 *   objects of type \alib{strings,AStringBase,AString}.
 *
 *   Parameter \p{MinReadLengthIdx} \e may denote the tuple element that holds an \c int value
 *   giving the minimum number of characters to read, when parsing this enum element with
 *   \alib{strings,SubstringBase::ConsumeEnum,Substring::ConsumeEnum},
 *   \alib{strings,SubstringBase::ConsumeEnumBitwise,Substring::ConsumeEnumBitwise} or
 *   \alib{strings,SubstringBase::ConsumeEnumOrBool,Substring::ConsumeEnumOrBool}.
 *   If set to \c 0, parsing is disabled.
 *
 *   With the specializations in place, template class \alib{lang,EnumReadWriteInfo} becomes
 *   available for type \p{TEnum}.
 *
 *
 *   @param TEnum             The enumeration type to specialize \p{%T_EnumMetaDataDeclReadWrite}
 *                            for.
 *   @param NameIdx           The "column" of the tuple that represents the element name.
 *                            element given in column \p{enumIdx}.
 *   @param MinParseLengthIdx The tuple element index that holds an \c int giving the minimum number
 *                            of characters to parse.
 *
 *
 *
 * \def  ALIB_LANG_ENUM_PRE_AND_POSTFIX
 *   Macro used to specialize TMP struct
 *   \alib{lang,T_EnumMetaDataPreAndPostFix,T_EnumMetaDataPreAndPostFix<TEnum>}.
 *
 *   \see For more information relate to
 *     - Documentation of TMP struct \alib{lang,T_EnumMetaDataPreAndPostFix}.
 *     - Methods
 *       \alib{lang,EnumReadWriteInfo::Prefix,Prefix} and
 *       \alib{lang,EnumReadWriteInfo::Prefix,Postfix} of template class
 *       \alib{lang,EnumReadWriteInfo}.
 *
 *   @param TEnum             The enumeration type to specialize
 *                            \alib{lang,T_EnumMetaDataPreAndPostFix} for.
 *   @param PrefixString      A constant string that denotes the type prefix, respectively a
 *                            postfix to the resource name of it.
 *   @param PostfixString     A constant string that denotes the type postfix, respectively a
 *                            postfix to the resource name of it.
 *
 * \~Comment ######## T_EnumIsArithmetical (has to be doxed here inside this sub-group) ######### \~
 *
 * \def  ALIB_LANG_ENUM_IS_ARITHMETICAL
 *   Specializes TMP struct \alib{lang,T_EnumIsArithmetical} to inherit \c std::true_type.
 *
 *   Usually, this macro is placed in a header file, probably close to the enum type definition.
 *   However, it has to be placed <b>in global (no) namespace</b>.
 *
 *   \see Documentation of TMP struct \alib{lang,T_EnumIsArithmetical} for more information.
 *
 *   @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared a bitwise
 *                   type.
 *
 * \~Comment ########### T_EnumIsBitwise (has to be doxed here inside this sub-group) ########### \~
 *
 * \def  ALIB_LANG_ENUM_IS_BITWISE
 *   Specializes TMP struct \alib{lang,T_EnumIsBitwise} to inherit \c std::true_type.
 *
 *   Usually, this macro is placed in a header file, probably close to the enum type definition.
 *   However, it has to be placed <b>in global (no) namespace</b>.
 *
 *   \see Documentation of TMP struct \alib{lang,T_EnumIsBitwise} for more information.
 *
 *   @param TEnum    Type of a scoped or non-scoped enumeration that is to be declared a bitwise
 *                   type.
 *
 *
 *
 * \~Comment ########### T_EnumIsIteratable (has to be doxed here inside this sub-group) ####### \~
 *
 * \def  ALIB_LANG_ENUM_IS_ITERATABLE
 *   Shortcut to \ref ALIB_LANG_ENUM_IS_ITERATABLE_BEGIN_END, providing <c>TEnum(0)</c> as macro
 *   parameter \p{StartElement}.
 *
 *   Usually, this macro is placed in a header file, probably close to the enum type definition.
 *   However, it has to be placed <b>in global (no) namespace</b>.
 *
 *   @param TEnum         Type of a scoped or non-scoped enumeration that is to be declared an
 *                        iteratable enum type.
 *   @param StopElement   The enum element after the last "valid" element in the enumeration.<br>
 *                        Will be used as expression for constexpr \alib{lang,T_EnumIsIteratable::End}.
 *
 * \def  ALIB_LANG_ENUM_IS_ITERATABLE_BEGIN_END
 *   Specializes TMP struct \alib{lang,T_EnumIsIteratable} to inherit
 *   \c std::true_type and implements methods:
 *   - \b %Begin to return \p{StartElement}
 *   - \b %End to return \p{StopElement}
 *
 *   Usually, this macro is placed in a header file, probably close to the enum type definition.
 *   However, it has to be placed <b>in global (no) namespace</b>.
 *
 *   \see Macro \ref ALIB_LANG_ENUM_IS_ITERATABLE
 *
 *   @param TEnum        Type of a scoped or non-scoped enumeration that is to be declared an
 *                       iteratable enum type.
 *   @param StartElement The first element of the enumeration.<br>
 *                       Will be used as expression for constexpr \alib{lang,T_EnumIsIteratable::End}.
 *   @param StopElement  The enum element after the last "valid" element in the enumeration.<br>
 *                      Will be used as expression for constexpr \alib{lang,T_EnumIsIteratable::End}.
 *
 *
 * \~Comment ######################## T_EnumMetaDataSpecification  ############################## \~
 *
 * \def ALIB_ENUM_SPECIFICATION_DECL
 *   Used to specialize \alib{lang,T_EnumMetaDataSpecification} for a certain type. The type
 *   may be a simple, empty struct, or any other type that is furtheron used to identify
 *   that specific enum meta data type.
 *
 *   The specialization includes the declaration of a method that loads the tuple data from the
 *   resources. This method is to be implemented with \ref ALIB_ENUM_SPECIFICATION_IMPL.
 *
 *   \see
 *     See documentation of TMP struct \alib{lang,T_EnumMetaDataSpecification} for more
 *     information and a step by step sample.
 *
 *   @param TSpecific    Type denoting the specification of an enum meta data tuple type.
 *   @param ...          The elemments of the tuple, excluding the initial integer.
 *
 *
 * \def ALIB_ENUM_SPECIFICATION_IMPL
 *   Implements the tuple resource loader method \alib{lang,T_EnumMetaDataSpecification::LoadTable}
 *   specialized with \ref ALIB_ENUM_SPECIFICATION_DECL. The latter macro also specializes
 *   \alib{lang,EnumMetaData::CheckLoad} which is using this method.
 *
 *   \see
 *     See documentation of TMP struct \alib{lang,T_EnumMetaDataSpecification} for more
 *     information and a step by step sample.
 *
 *   @param TSpecific    Type denoting the specification of an enum meta data tuple type.
 *
 *
 * \def ALIB_ENUM_SPECIFICATION
 *   Used to associate a custom enum type with a type of enum meta data that is in turn associated
 *   to given type \p{TSpecific}. Hence, this macro is used by code to enable its custom enum types
 *   to be usable with a library that accepts enum elements with a certain meta data scheme.
 *
 *   Along with the specialization of \alib{lang,T_EnumMetaDataSpecification}, the macro also
 *   comprises macros
 *   - \ref ALIB_LANG_RESOURCED,
 *   - \ref ALIB_LANG_ENUM_PRE_AND_POSTFIX (using fixed strings <c>"Prefix"</c> and <c>"Postfix"</c>
 *     as resource names, and
 *   - \ref ALIB_LANG_ENUM_READ_WRITE.
 *
 *   In addition, \alib{lang,EnumMetaData::CheckLoad} is specialized for type \p{TEnum} to use
 *   method \alib{lang,T_EnumMetaDataSpecification::LoadTable} which has to be implemented
 *   using macro \ref ALIB_ENUM_SPECIFICATION_IMPL.
 *
 *   Often, libraries provide an explicit shortcut to this macro, that omits parameter
 *   \p{TSpecific} and gives a more readable name.
 *   For examples, see \ref ALIB_LANG_EXCEPTIONS or \ref ALIB_CONFIG_VARIABLES.
 *
 *   \see
 *     See documentation of TMP struct \alib{lang,T_EnumMetaDataSpecification} for more
 *     information and a step by step sample.
 *
 *   @param TSpecific       Type denoting the specification of an enum meta data tuple type.
 *   @param TEnum           The enumeration type to specialize \p{%T_EnumMetaDataSpecification}
 *                          for.
 *   @param ResourceLibrary A reference to the \alib{lang,Library} the enum meta data is loaded
 *                          from.
 *   @param ResourceName    The resource name of the meta information of the \p{TEnum}.
 * @}
 * @}
 */

#define  ALIB_LANG_ENUM_READ_WRITE( TEnum, NameIdx, MinParseLengthIdx )                            \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_EnumMetaDataDeclReadWrite<TEnum>  :  public std::true_type                     \
{                                                                                                  \
    static constexpr  size_t        NameIndex              = NameIdx;                              \
    static constexpr  size_t        MinParseLengthIndex    = MinParseLengthIdx;                    \
};}}}                                                                                              \

#define  ALIB_LANG_ENUM_PRE_AND_POSTFIX( TEnum, PrefixString, PostfixString )                      \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_EnumMetaDataPreAndPostFix<TEnum>  :  public std::true_type                     \
{                                                                                                  \
    static constexpr  String        Prefix()               { return PrefixString;   }              \
    static constexpr  String        Postfix()              { return PostfixString;  }              \
};}}}                                                                                              \


#define  ALIB_LANG_ENUM_META_DATA( TEnum, ... )                                                    \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> struct T_EnumMetaDataDecl<TEnum>  :  public std::true_type                              \
{                                                                                                  \
    using  TTuple=  std::tuple< typename std::underlying_type<TEnum>::type , __VA_ARGS__ >;        \
};}}}                                                                                              \


#define  ALIB_LANG_ENUM_META_DATA_RESOURCE_LOADER( TEnum )                                         \
namespace aworx { namespace lib { namespace lang {                                                 \
template<> void EnumMetaData<TEnum>::CheckLoad()                                                   \
{   if( Table.size() == 0 )                                                                        \
    {   ResourcedTupleLoader::LoadTable( Table,                                                    \
                                         T_Resourced<TEnum>::Lib(),                                \
                                         T_Resourced<TEnum>::Name()        );                      \
        CheckMap(true);  }                                                                         \
}}}}                                                                                              \



#define  ALIB_LANG_ENUM_NAMED( TEnum )                                                             \
ALIB_LANG_ENUM_META_DATA(  TEnum, aworx::String )                                                  \
ALIB_LANG_ENUM_READ_WRITE( TEnum, 1, 0 )                                                           \

#define  ALIB_LANG_ENUM_PARSABLE( TEnum )                                                          \
ALIB_LANG_ENUM_META_DATA(  TEnum, aworx::String, int )                                             \
ALIB_LANG_ENUM_READ_WRITE( TEnum, 1, 2 )                                                           \




// #################################################################################################
// T_APPLY<Enum>
// #################################################################################################
#if ALIB_MODULE_STRINGS



namespace aworx { namespace lib { namespace strings {

// We are faking specializations of T_Apply for doxygen into namespace aworx::lib::strings::applyto
#if defined(DOX_PARSER)
    namespace applyto {
#endif


#if defined(DOX_PARSER)
/**
 * Templated specialization of TMP struct \alib{strings,T_Apply} for enum types which meet the
 * following conditions:
 * - A specialization of TMP struct \alib{lang,T_EnumMetaDataDeclReadWrite} exists whose member
 *   \alib{lang,T_EnumMetaDataDeclReadWrite::NameIndex,NameIndex} does not evalute to \c 0.
 * - \b No specialization of TMP struct \alib{lang,T_EnumIsBitwise} exists that inherits from
 *   \c std::true_type.
 *
 * (For \alib{lang,T_EnumIsBitwise,bitwise enum types} a separated
 * \alib{strings::applyto,T_Apply<TEnumBitwise,character>,specialization of T_Apply} is
 * provided.)<br>
 * The conditions are evaluated by the compiler using \c std::enable_if on optional template
 * parameter \p{TEnableIf} of \alib{strings,T_Apply}.
 *
 * Member function #Apply writes the name of the given enumeration value to \p{target}.
 * To achieve this, the static vector of tuples provided with the specialization of TMP struct
 * \alib{lang,T_EnumMetaDataDecl} is used as an "enum element to enum name" translation table.
 *
 * As with all specializations of TMP struct \b %T_Apply, the use of it is done implicit.
 *
 * \see
 *   - TMP struct \alib{lang,T_EnumMetaDataPreAndPostFix}.
 *   - Some sample code is given with documentation of TMP struct \alib{lang,T_EnumMetaDataDecl}.
 *
 * # Reference Documentation #
 * @tparam TEnum The object that \b Apply was invoked on.
 */
template<typename TEnum>
struct  T_Apply<TEnum, character>
#else
template<typename TEnum>
struct  T_Apply<TEnum, character, typename std::enable_if<(     T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex
                                                 !=  0 )
                                                 && !aworx::lib::lang::T_EnumIsBitwise <TEnum>::value    >::type>
        : public std::true_type
#endif

{
    /** ********************************************************************************************
     * Searches the the value of \p{src} in the name table provided with field
     * \alib{lang,EnumMetaData::Table,Table} of a specialized version of that TMP struct
     * and writes it to \p{target}.
     *
     * Prior to the element name, if specialized for \p{TEnum}, a type prefix received with
     * \alib{lang,EnumReadWriteInfo::Prefix} is written, as well as the corresponding
     * postfix \alib{lang,EnumReadWriteInfo::Postfix}.
     *
     * See class documentation for more information and a sample.
     *
     * @param target The object that \b Apply was invoked on.
     * @param src    The enum value.
     * @return \c 1 if characters were appended to the target, \c 0 otherwise.
     **********************************************************************************************/
    static inline integer Apply( AString& target, TEnum src )
    {
        auto& enumMetaData= *EnumMetaData<TEnum>::GetSingleton();
        enumMetaData.CheckLoad();
        target << EnumReadWriteInfo<TEnum>::Prefix();
        auto  entry= enumMetaData.Get( src );
        if ( entry != nullptr )
            target << EnumReadWriteInfo<TEnum>::Name( *entry );
        else
            target << EnumValue( src );

        target << EnumReadWriteInfo<TEnum>::Postfix();
        return 1;
    }
};


#if defined(DOX_PARSER)
/**
 * Templated specialization of template struct
 * \alib{strings,T_Apply} for enum types which meet the following conditions:
 * - A specialization of TMP struct \alib{lang,T_EnumMetaDataDeclReadWrite} exists whose member
 *   \alib{lang,T_EnumMetaDataDeclReadWrite::NameIndex,NameIndex} does not evalute to \c 0.
 * - A specialization of TMP struct \alib{lang,T_EnumIsBitwise} exists that inherits from
 *   \c std::true_type.
 *
 * (For enum types that only fulfill the first requirement, a separated
 *  \alib{strings::applyto,T_Apply<TEnum,character>,specialization of T_Apply} is
 * provided.)<br>
 * The conditions are evaluated by the compiler using \c std::enable_if on optional template
 * parameter \p{TEnableIf} of \alib{strings,T_Apply}.
 *
 * Member function #Apply writes all value names corresponding to the bits set in \p{src},
 * separated by a comma <c>','</c>.
 * To achieve this, the static vector of tuples provided with the specialization of TMP struct
 * \alib{lang,T_EnumMetaDataDecl} is used as a "enum element to enum name" translation table.
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
 * any namespace, macro \ref ALIB_LANG_ENUM_NAMED is used to specialize TMP
 * struct \alib{lang,T_EnumMetaDataDecl}. Because a window might have more than one state, macro
 * \ref ALIB_LANG_ENUM_IS_BITWISE is used to specialize TMP struct \alib{lang,T_EnumIsBitwise}:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_DECLARATION
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_SPECIALIZE
 *
 * A window that is both, vertically and horizontally maximized, is considered to be just
 * \e maximized. Therefore, we define the entries in \alib{lang,EnumMetaData::Table} that
 * is used as a "name translation table" as follows:
 *
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_DEFINITION
 *
 * That is all that is needed! With this setup, the following code:
 *  \snippet "DOX_ALIB_LANG.cpp"     DOX_ALIB_LANG_ENUMS_BITWISE_SAMPLE
 *
 * produces this output:
 *  \snippet "DOX_ALIB_LANG_ENUMS_BITWISE_OUTPUT.txt"     OUTPUT
 *
 * @see TMP struct \alib{lang,T_EnumMetaDataPreAndPostFix}.
 *
 * # Reference Documentation #
 * @tparam TEnum The object that \b Apply was invoked on.
 */
template<typename TEnumBitwise>
struct  T_Apply<TEnumBitwise, character>
#else
template<typename TEnum>
struct  T_Apply<TEnum, character, typename std::enable_if<(     T_EnumMetaDataDeclReadWrite<TEnum>::NameIndex
                                                 !=  0 )
                                                 &&  aworx::lib::lang::T_EnumIsBitwise <TEnum>::value    >::type>
        : public std::true_type
#endif
{
    /** ********************************************************************************************
     * Writes all enum value names corresponding to the bits set in \p{src} and the ones found
     * in entries of specialized TMP struct member \alib{lang,EnumMetaData::Table},
     * separated by a comma <c>','</c> to \p{target}.
     *
     * Prior to the element name list, if specialized for \p{TEnum}, a type prefix received with
     * \alib{lang,EnumReadWriteInfo::Prefix} is written, as well as the corresponding
     * postfix \alib{lang,EnumReadWriteInfo::Postfix}.
     *
     * See class documentation for more information and a sample.
     *
     * @param target The object that \b Apply was invoked on.
     * @param src    The enum value.
     * @return \c 1 if characters were appended to the target, \c 0 otherwise.
     **********************************************************************************************/
    static inline integer Apply( AString& target, TEnum src )
    {
        EnumMetaData<TEnum>::GetSingleton()->CheckLoad();

        target << lang::EnumReadWriteInfo<TEnum>::Prefix();

        // check what has been covered and omit double entries
        TEnum covered= TEnum(0);

        // loop over entry 2 to end, check bit
        integer len= target.Length();

        auto& enumMD= *EnumMetaData<TEnum>::GetSingleton();
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
                    return 1;
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

        target << lang::EnumReadWriteInfo<TEnum>::Postfix();
        return 1;
    }
};

#if defined(DOX_PARSER)
} // namespace aworx::lib::strings[::applyto]
#endif

}}} // namespace [aworx::lib::strings]


#endif // ALIB_MODULE_STRINGS



// #################################################################################################
// T_EnumIsBitwise specializations for built-in ALib enums
// Note: These can not be implemented in enum.hpp because it is included prior to alib strings
//       while this header is included after string headers.
//       Nevertheless, the corresponding static members T_EnumIsBitwise::NameTables are defined in
//       file enum.cpp.
// #################################################################################################

//! [DOX_ALIB_LANG_ENUMS_PARSABLE_2]
ALIB_LANG_ENUM_PARSABLE( aworx::lib::lang::Case )
ALIB_LANG_RESOURCED ( aworx::lib::lang::Case, LANG, ASTR( "Case" ) )
//! [DOX_ALIB_LANG_ENUMS_PARSABLE_2]

ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Bool          )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Switch        )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Alignment     )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::SortOrder     )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Inclusion     )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Reach         )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::CurrentData   )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::SourceData    )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Safeness      )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Responsibility)
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Timezone      )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::LockMode      )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Whitespaces   )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Create        )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Propagation   )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::Phase         )
ALIB_LANG_ENUM_PARSABLE ( aworx::lib::lang::ContainerOp   )

ALIB_LANG_RESOURCED( aworx::lib::lang::Bool          , LANG, ASTR( "Bool"          ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Switch        , LANG, ASTR( "Switch"        ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Alignment     , LANG, ASTR( "Alignment"     ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::SortOrder     , LANG, ASTR( "SortOrder"     ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Inclusion     , LANG, ASTR( "Inclusion"     ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Reach         , LANG, ASTR( "Reach"         ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::CurrentData   , LANG, ASTR( "CurrentData"   ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::SourceData    , LANG, ASTR( "SourceData"    ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Safeness      , LANG, ASTR( "Safeness"      ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Responsibility, LANG, ASTR( "Responsibility") )
ALIB_LANG_RESOURCED( aworx::lib::lang::Timezone      , LANG, ASTR( "Timezone"      ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::LockMode      , LANG, ASTR( "LockMode"      ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Whitespaces   , LANG, ASTR( "Whitespaces"   ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Create        , LANG, ASTR( "Create"        ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Propagation   , LANG, ASTR( "Propagation"   ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::Phase         , LANG, ASTR( "Phase"         ) )
ALIB_LANG_RESOURCED( aworx::lib::lang::ContainerOp   , LANG, ASTR( "ContainerOp"   ) )



ALIB_LANG_ENUM_IS_BITWISE( aworx::lib::lang::Phase )

#endif // HPP_ALIB_LANG_ENUM_META_DATA

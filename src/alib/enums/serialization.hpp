/** ************************************************************************************************
 * \file
 * This header file is part of module \alib_enums of the \aliblong.
 *
 * \emoji :copyright: 2013-2024 A-Worx GmbH, Germany.
 * Published under \ref mainpage_license "Boost Software License".
 **************************************************************************************************/
#ifndef HPP_ALIB_ENUMS_SERIALIZATION
#define HPP_ALIB_ENUMS_SERIALIZATION 1

#if !defined(HPP_ALIB_ENUMS_RECORDS)
#   include "alib/enums/records.hpp"
#endif

ALIB_ASSERT_MODULE(STRINGS)

#if ALIB_CAMP && !defined(HPP_ALIB_LANG_RESOURCES_RESOURCES)
#   include "alib/lang/resources/resources.hpp"
#endif

#if !defined (HPP_ALIB_ENUMS_BITWISE)
#    include "alib/enums/bitwise.hpp"
#endif
#if !defined (HPP_ALIB_STRINGS_LOCALSTRING)
#    include "alib/strings/localstring.hpp"
#endif

#if ALIB_DEBUG && !defined (HPP_ALIB_STRINGS_FORMAT)
#    include "alib/strings/format.hpp"
#endif

namespace alib {  namespace enums {

// #################################################################################################
// Parsing (Consume)
// #################################################################################################


#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Consumes an element value of a C++ enumeration that is equipped with
 * \ref alib_enums_records "ALib Enum Records" of type \alib{enums,ERSerializable} (or of a derived
 * type) from a given \alib{strings,TSubstring,Substring}.
 *
 * In debug builds, the method asserts that at least one record is defined for \p{TEnum}.
 *
 * For more information consult chapter
 * \ref alib_enums_records_details_serialization "4.3.1 Serialization/Deserialization" of the
 * Programmer's Manual of module \alib_enums.
 *
 * \note
 *   This namespace function is applicable to \alib{enums,T_EnumIsBitwise,bitwise enums} as well.
 *   However, only one element name is parsed.
 *   To parse multiple elements (ored to one resulting enum value), use sibling function
 *   \alib{enums,ParseBitwise}.
 *
 * @tparam TEnum              The enumeration type equipped with <b>ALib Enum Recorss</b>
 *                            of type \alib{enums,ERSerializable}.
 * @tparam TChar              The character type of the \p{input} substring.
 *                            Deduced by the compiler.
 * @tparam TSensitivity       The sensitivity of the comparison.
 *                            Defaults to \b Case::Sensitive.
 * @tparam TTrimBeforeConsume Determines if the sub string should be (left-) trimmed before the
 *                            consume operation. If so, in case of parsing failure, trimming is
 *                            \b not restored.<br>
 *                            Defaults to \b Whitespaces::Trim.
 * @param[in,out] input       The substring to parse. Passed as reference. On success, the
 *                            substring will be shortened by the characters consumed. On failure
 *                            only trimmed characters are consumed.
 * @param[out]    result      The result enum element given as reference.
 * @return \c true if an enum element was successfuly recognized, \c false otherwise.
 *
 * ### Module Dependencies ###
 *   This method is only available if \alib_strings is included in the \alibdist.
 **************************************************************************************************/
template<typename    TEnum,
         typename    TChar,
         Case        TSensitivity        = Case::Ignore,
         Whitespaces TTrimBeforeConsume  = Whitespaces::Trim>
inline
bool    Parse(  strings::TSubstring<TChar>& input, TEnum&  result );
#else
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4127 )
#endif

template<typename    TEnum,
         typename    TChar,
         lang::Case        TSensitivity        = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume  = lang::Whitespaces::Trim >
ATMP_T_IF(bool, EnumRecords<TEnum>::template AreOfType<ERSerializable>() )
Parse( strings::TSubstring<TChar>& input,  TEnum&  result )
{
    ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                       NString128() << "No Enum Records for type <"
                                    << lang::DbgTypeDemangler( typeid(TEnum)).Get() << "> found." )
    if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
        input.TrimStart();

    for( auto recordIt=   EnumRecords<TEnum>().begin() ;
              recordIt != EnumRecords<TEnum>().end()   ; ++recordIt  )
        if ( input.template ConsumePartOf<TSensitivity>( recordIt->EnumElementName,
                                                         recordIt->MinimumRecognitionLength ) > 0 )
        {
            result= recordIt.Enum();
            return true;
        }
    return false;
}
#endif

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Repeatedly invokes sibling function \alib{enums,Parse} until \p{delim} is not found.
 * The enum element values are or'ed in \p{result}.
 *
 * In debug builds, the method asserts that at least one record is defined for \p{TEnum}.
 *
 * \note
 *   This method is applicable only to \alib{enums,T_EnumIsBitwise,bitwise enums} that likewise
 *   are equipped with \ref alib_enums_records "ALib Enum Records" of (derived) type
 *   \alib{enums,ERSerializable}.
 *
 *
 * @tparam TEnum              The enumeration type equipped with <b>ALib Enum Records</b>
 *                            of (derived) type \alib{enums,ERSerializable} and furthermore with a
 *                            specialization of \alib{enums,T_EnumIsBitwise}.
 * @tparam TChar              The character type of the \p{input} substring.
 *                            Deduced by the compiler.
 * @tparam TSensitivity       The sensitivity of the comparison.
 *                            Defaults to \b Case::Ignore.
 * @tparam TTrimBeforeConsume Determines if the substring should be (left-) trimmed before and
 *                            after each consume operation. If so, in case of parsing failure,
 *                            trimming is \b not restored.<br>
 *                            Defaults to \b Whitespaces::Trim.
 * @tparam delimiter          The delimiter character of the enum elements.<br>
 *                            Defaults to <c>','</c>.
 * @tparam keepLastDelim      If \c true , the delimiter will be kept in this sub-string, if
 *                            after the delimiter no further enum element was found.
 *                            If \c false, the delimiter will be kept.<br>
 *                            Defaults to \c true.
 * @param[in,out] input       The substring to parse. Passed as reference. On success, the
 *                            substring will be shortened by the characters consumed. On failure
 *                            only trimmed characters are consumed.
 * @param[out] result         The result enum element given as reference.
 * @return \c true if an enum element was successfuly recognized, \c false otherwise.
 *
 * ### Module Dependencies ###
 *   This method is only available if \alib_strings is included in the \alibdist.
 **************************************************************************************************/
template<typename    TEnum,
         typename    TChar,
         Case        TSensitivity       = Case::Ignore,
         Whitespaces TTrimBeforeConsume = Whitespaces::Trim,
         TChar       delimiter          = ',',
         bool        keepLastDelim      = true                  >
inline
bool    ParseBitwise( strings::TSubstring<TChar>& input, TEnum&  result );
#else
template<typename          TEnum,
         typename          TChar,
         lang::Case        TSensitivity       = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume = lang::Whitespaces::Trim,
         TChar             delimiter          = ',',
         bool              keepLastDelim      = true                  >
ATMP_T_IF(bool,      EnumRecords<TEnum>::template AreOfType<ERSerializable>()
                && T_EnumIsBitwise<TEnum>::value)
ParseBitwise( strings::TSubstring<TChar>& input, TEnum&  result )
{
    bool mResult= false;
    result= TEnum(0);
    strings::TSubstring<TChar> restoreBeforeDelim;
    if constexpr ( keepLastDelim )
        restoreBeforeDelim= input;
    for(;;)
    {
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            input.TrimStart();
        TEnum actEnum;
        if ( !Parse<TEnum, TChar, TSensitivity, TTrimBeforeConsume>( input, actEnum ) )
        {
            if constexpr ( keepLastDelim )
                input= restoreBeforeDelim;
            return mResult;
        }
        result|=  actEnum;
        mResult=  true;
        if constexpr ( TTrimBeforeConsume == lang::Whitespaces::Trim )
            input.TrimStart();
        if constexpr ( keepLastDelim )
            restoreBeforeDelim=  input;

        if( !input.template ConsumeChar<TSensitivity, TTrimBeforeConsume>( delimiter ) )
            return mResult;

    }
}
#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#endif

#if defined(ALIB_DOX)
/** ************************************************************************************************
 * Convenience method that first uses \alib{enums,Parse} to try and read an element of a C++
 * enum. If this is not successful, an enum of type \alib{lang,Bool} is tried to be read.
 * If this is successful, depending on the value read, the \p{TEnum} values given
 * as parameters \p{falseValue} and \p{trueValue} are assigned.
 * Otherwise false is returned.
 *
 * In debug builds, the method asserts that at least one record is defined for \p{TEnum}.
 *
 * \see
 *   For more information consult chapter
 *   \ref alib_enums_records_details_serialization of the
 *   Programmer's Manual of module \alib_basecamp.
 *
 * @tparam TEnum              The enumeration type equipped with <b>ALib Enum Records</b>
 *                            of (derived) type \alib{enums,ERSerializable}.
 * @tparam TChar              The character type of the \p{input} substring.
 *                            Deduced by the compiler.
 * @tparam TSensitivity       The sensitivity of the comparison.
 *                            Defaults to \b Case::Ignore.
 * @tparam TTrimBeforeConsume Determines if the substring should be (left-) trimmed before and
 *                            after each consume operation. If so, in case of parsing failure,
 *                            trimming is \b not restored.<br>
 *                            Defaults to \b Whitespaces::Trim.
 * @param  trueValue          The \p{TEnum} value to use in case of \c Bool::True was read.
 * @param  falseValue         The \p{TEnum} value to use in case of \c Bool::False was read.
 * @param[in,out] input       The substring to parse. Passed as reference. On success, the
 *                            substring will be shortened by the characters consumed. On failure
 *                            only trimmed characters are consumed.
 * @param[out] result         The result enum element given as reference.
 * @return \c true if an element of \p{TEnum} or \alib{lang,Bool} could be read,
 *         \c false otherwise.
 **************************************************************************************************/
template<typename    TEnum,
         typename    TChar,
         Case        TSensitivity       = Case::Ignore,
         Whitespaces TTrimBeforeConsume = Whitespaces::Trim >
inline
bool    ParseEnumOrTypeBool( strings::TSubstring<TChar>&   input,
                             TEnum&                        result,
                             TEnum                         falseValue,
                             TEnum                         trueValue         );
#else
template<typename          TEnum,
         typename          TChar,
         lang::Case        TSensitivity       = lang::Case::Ignore,
         lang::Whitespaces TTrimBeforeConsume = lang::Whitespaces::Trim >
ATMP_T_IF(bool, EnumRecords<TEnum>::template AreOfType<ERSerializable>() )
ParseEnumOrTypeBool( strings::TSubstring<TChar>&   input,
                     TEnum&                        result,
                     TEnum                         falseValue,
                     TEnum                         trueValue         )
{
    // first try to read a TEnum
    if( Parse<TEnum, TChar, TSensitivity, TTrimBeforeConsume>( input, result ) )
        return true;

    // if failed, read boolean
    lang::Bool boolEnum;
    if( enums::Parse<lang::Bool, TChar, TSensitivity, lang::Whitespaces::Keep>( input, boolEnum ) )
    {
        result= (boolEnum == lang::Bool::True) ? trueValue : falseValue;
        return true;
    }

    // failed
    return false;
}
#endif

}} // namespace [alib::enums]



// #################################################################################################
// Writing (T_Append<Enum>)
// #################################################################################################

namespace alib {  namespace strings {

// Faking specializations of T_Append for doxygen into namespace alib::strings::APPENDABLES
#if defined(ALIB_DOX)
    namespace APPENDABLES {
#endif


#if defined(ALIB_DOX)
/**
 * Templated specialization of functor \alib{strings,T_Append} makes elements of C++ enumeration
 * type \p{TEnum} appendable to \alib{strings,TAString,AString} instances, if:
 * - Type \p{TEnum} is equipped with \ref alib_enums_records "ALib Enum Records" of type
 *   \alib{enums,ERSerializable} (or a derived type ), and if:
 * - \b No specialization of TMP struct \alib{enums,T_EnumIsBitwise} exists for \p{TEnum}.
 *   (For those, a different implementation of this functor is given.)
 *
 * \note
 *    The conditions are evaluated by the compiler using \c std::enable_if on optional template
 *    parameter \p{TEnableIf} of unspecialized \alib{strings,T_Append}.
 *
 * Member \b operator() writes the name of the given enumeration value to \p{target}.
 * As with all specializations of functor \b %T_Append, the use of it is done implicitly
 * with various interface method of class \alib{strings,TAString,AString}.
 *
 * If furthermore TMP struct \alib{lang::resources,T_Resourced} is specialized for type \p{TEnum}
 * and an externalizd resouce string exists according to the specification described with
 * methods \alib{lang::resources,ResourcedType::TypeNamePrefix} and
 * \alib{lang::resources,ResourcedType::TypeNamePostfix} then these resourced strings are written
 * prior and after the enumeration element's name.
 *
 * \see
 *   - Sibling specialization
 *     \alib{strings::APPENDABLES,T_Append<TEnumBitwise\,TChar>,T_Append<TEnumBitwise\,TChar>}
 *   - \ref alib_enums_records "ALib Enum Records".
 *   - TMP struct \alib{lang::resources,T_Resourced}.
 *   - Corresponding convenience type  \alib{lang::resources,ResourcedType}.
 *   - Some sample code is given with documentation \ref alib_enums_records "ALib Enum Records".
 *
 * @tparam TEnum The enumeration type of the element that is to be appended to an \b %AString.
 * @tparam TChar The character type of the target \b %AString.
 */
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, TChar>
#else
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, TChar,
                 ATMP_VOID_IF(    ATMP_ISOF( typename T_EnumRecords<TEnum>::Type, enums::ERSerializable )
                              && !T_EnumIsBitwise<TEnum>::value )  >
#endif
{
    /** ********************************************************************************************
     * Writes the name of the given enumeration \p{element} it to \p{target}.
     *
     * If available for \p{TEnum}, the resourced name
     * \alib{lang::resources,ResourcedType::TypeNamePrefix,prefix} and
     * \alib{lang::resources,ResourcedType::TypeNamePostfix,postfix} are written before and after the
     * element's name.
     *
     * If no record exists for \p{element}, its underlying integral value is written.
     * In debug builds, the method asserts that at least one record is defined for \p{TEnum}.
     *
     *
     * @param target    The \b AString that \p{element} is to be appended to.
     * @param element   The enumeration element to append to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, TEnum element )
    {
        ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                           NString128() << "No Enum Records for type <"
                                        << lang::DbgTypeDemangler( typeid(TEnum)).Get() << "> found." )

        ALIB_IF_CAMP( target << ResourcedType<TEnum>::TypeNamePrefix(); )
        auto* record= enums::TryRecord( element );
        if( record != nullptr )
            target << record->EnumElementName;
        else
            target << UnderlyingIntegral( element );
        ALIB_IF_CAMP( target << ResourcedType<TEnum>::TypeNamePostfix(); )
    }
};


#if defined(ALIB_DOX)
/**
 * Templated specialization of functor \alib{strings,T_Append} makes elements of C++ enumeration
 * type \p{TEnum} appendable to \alib{strings,TAString,AString} instances, if
 * - Type \p{TEnum} is equipped with \ref alib_enums_records "ALib Enum Records" of type
 *   \alib{enums,ERSerializable} (or a derived type derived from \b ERSerializable), and
 * - \b A specialization of TMP struct \alib{enums,T_EnumIsBitwise} exists for \p{TEnum}.
 *   (For non-bitwise types, a different implementation of this functor is given.)
 *
 * \note
 *    The conditions are evaluated by the compiler using \c std::enable_if on optional template
 *    parameter \p{TEnableIf} of unspecialized \alib{strings,T_Append}.
 *
 * Member \b operator() writes all value names corresponding to the bits set in \p{src},
 * separated by delimiter character <c>','</c>.
 * \note
 *    For technical reasons, the delimiter is not adjustable. In cases a different delimter
 *    is to be used, it needs to be replaced in the string after the value is written.
 *
 * If the underlying integral value of a given enum element may be become \c 0, a corresponding
 * enum record associated to such non-bit value will be used if existent.
 *
 * Furthermore, with bitwise type enums, the defined enum records may contain entries that
 * represent combinations of more than one integral bit. Such combination entries are supported
 * but have to be registered with class \alib{enums,EnumRecords} \b prior to the
 * standard single bit entries!
 * If combined bit values are matched, the corresponding element names that represent the
 * single bit values will not be written.
 *
 * As a sample, lets consider a window manager software which has a scoped enum type representing
 * window states. Because a window might have more than one state, macro
 * \ref ALIB_ENUMS_MAKE_BITWISE is used to specialize TMP struct \alib{enums,T_EnumIsBitwise}.
 * Next, macro \ref ALIB_ENUMS_ASSIGN_RECORD used to associate the type with enum record type
 * \alib{enums,ERSerializable}:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_DECLARATION
 *
 * A window that is both, vertically and horizontally maximized, is considered to be "just"
 * \e maximized. Therefore, during bootstrap of the software, enum records that fetch that the
 * combination of states are defined \b prior to the single-state records:
 *
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_DEFINITION
 *
 * That is all that is needed! With this setup, the following code:
 *  \snippet "DOX_ALIB_ENUMS.cpp"     DOX_ALIB_ENUMS_BITWISE_SAMPLE
 *
 * produces this output:
 *  \snippet "DOX_ALIB_ENUMS_BITWISE_OUTPUT.txt"     OUTPUT
 *
 * If furthermore TMP struct \alib{lang::resources,T_Resourced} is specialized for type \p{TEnum}
 * and an externalizd resouce string exists according to the specification described with
 * methods \alib{lang::resources,ResourcedType::TypeNamePrefix} and
 * \alib{lang::resources,ResourcedType::TypeNamePostfix} then these resourced strings are written
 * prior and after the enumeration element name(s).
 *
 * \see
 *   - Sibling specialization
 *     \alib{strings::APPENDABLES,T_Append<TEnum\,TChar>,T_Append<TEnum\,TChar>}
 *   - TMP struct \alib{lang::resources,T_Resourced}.
 *   - Corresponding convenience type  \alib{lang::resources,ResourcedType}.
 *   - Some sample code is given with documentation \ref alib_enums_records "ALib Enum Records".
 *
 * # Reference Documentation #
 * @tparam TEnum The \b AString that \b Append was invoked on.
 * @tparam TChar The character type of the target \b %AString.
 */
template<typename TEnumBitwise, typename TChar>
struct  T_Append<TEnumBitwise, TChar>
#else
template<typename TEnum, typename TChar>
struct  T_Append<TEnum, TChar,
                 ATMP_VOID_IF(   ATMP_ISOF( typename T_EnumRecords<TEnum>::Type, enums::ERSerializable )
                              && T_EnumIsBitwise<TEnum>::value  )>
#endif
{
    /** ********************************************************************************************
     * Writes a comma-separated list of element names of the bitwise defined enumeration \p{TEnum}
     * to \p{target}.
     *
     * In debug builds, the method asserts that at least one record is defined for \p{TEnum}.
     * It is furthermore asserted that all bits contained in \p{elements} have been "covered"
     * by corresponding names.
     *
     * The enum records defined may aggregate several bits. Aggregations have to be defined
     * prior to records that represent the corresponding single bits (or another subset of those).
     *
     * \see
     *   This struct's documentation for more information and a sample.
     *
     * @param target    The \b AString that \p{elements} is to be appended to.
     * @param elements  The enumeration element to append to \p{target}.
     **********************************************************************************************/
    void operator()( TAString<TChar>& target, TEnum elements )
    {
        ALIB_ASSERT_ERROR( EnumRecords<TEnum>().begin() != EnumRecords<TEnum>().end(), "ENUMS",
                           NString128() << "No Enum Records for type <"
                                        << lang::DbgTypeDemangler( typeid(TEnum)).Get() << "> found." )

        ALIB_IF_CAMP( target << ResourcedType<TEnum>::TypeNamePrefix(); )

        // check what has been covered and omit double entries
        TEnum covered= TEnum(0);

        // loop over entry 2 to end, check bit
        integer len= target.Length();

        for( auto recordIt=   EnumRecords<TEnum>().begin() ;
                  recordIt != EnumRecords<TEnum>().end()   ; ++recordIt  )
        {
            // no bits are set and this entry does not contain bits, then stop here
            if( recordIt.Integral() == 0 )
            {
                if( elements == TEnum(0) )
                {
                    target << recordIt->EnumElementName;
                    ALIB_IF_CAMP( target << ResourcedType<TEnum>::TypeNamePostfix(); )
                    return;
                }
            }
            else if(     HasBits( elements, recordIt.Enum() )
                     && !HasBits( covered , recordIt.Enum() )  )
            {
                covered|= recordIt.Enum();
                target << recordIt->EnumElementName << ',';
            }
        }
        len= target.Length() - len;

        // remove the last comma
        if( len != 0 )
            target.DeleteEnd( 1 );

        #if ALIB_STRINGS
            ALIB_ASSERT_ERROR( covered == elements, "ENUMS",
               NString128() << "Not all bits have been covered while writing bitset '"
                            << NFormat::Bin( elements ) << "' of enumeration type <"
                            << lang::DbgTypeDemangler( typeid(TEnum)).Get() << ">. Remaining bits are '"
                            << NFormat::Bin( covered & elements )  << "'."                        )
        #else
            ALIB_ASSERT_ERROR( covered == elements, "ENUMS",
                               "Not all bits have been covered while writing a bitset of type <",
                               DbgTypeDemangler( typeid(TEnum)).Get(),   ">."                     )
        #endif

        ALIB_IF_CAMP( target << ResourcedType<TEnum>::TypeNamePostfix(); )
    }
};


#if defined(ALIB_DOX)
} // namespace alib::strings[::appendables]
#endif
}} // namespace [alib::strings]


#endif // HPP_ALIB_ENUMS_SERIALIZATION
